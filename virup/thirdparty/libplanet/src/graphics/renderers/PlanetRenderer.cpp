/*
    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "../../../include/graphics/renderers/PlanetRenderer.hpp"

PlanetRenderer::PlanetRenderer(Planet const* drawnBody)
    : CelestialBodyRenderer(
          drawnBody, computePointColor(*drawnBody, drawnBody->getHostStar()))
    , boundingSphere(drawnBody->getCelestialBodyParameters().radius)
    , hostStar(drawnBody->getHostStar())
    , unloadedShader(
          GLHandler::newShader("planet/planet", "planet/uniformplanet"))
    , unloadedMesh(Primitives::newUnitSphere(unloadedShader, 50, 50))
{
	// UNLOADED
	GLHandler::setShaderParam(
	    unloadedShader, "color",
	    Utils::toQt(drawnBody->getCelestialBodyParameters().color));
	GLHandler::setShaderParam(
	    unloadedShader, "oblateness",
	    Utils::toQt(drawnBody->getCelestialBodyParameters().oblateness));

	if(hostStar != nullptr)
	{
		lightcolor = Utils::toQt(hostStar->getCelestialBodyParameters().color);
	}
	else
	{
		lightcolor = QColor(0, 0, 0);
	}
}

void PlanetRenderer::updateMesh(UniversalTime uT,
                                OrbitalSystemCamera const& camera)
{
	CelestialBodyRenderer::updateMesh(uT, camera);
	auto drawnPlanet(dynamic_cast<Planet const*>(drawnBody));

	// custom models have km units, not radius units
	if(customModel)
	{
		model.scale(1000.0 / drawnBody->getCelestialBodyParameters().radius);
	}

	if(drawnPlanet->getPlanetParameters().outerRing == 0.f)
	{
		culled = camera.shouldBeCulled(position, boundingSphere * scale);
	}
	else
	{
		// take rings into account !
		double cullingRadius(drawnPlanet->getPlanetParameters().outerRing
		                     * scale);
		culled = camera.shouldBeCulled(position, cullingRadius);
	}

	if(customModel
	   && (apparentAngle < 0.005 || (planet != nullptr && !planet->isValid())))
	{
		model.scale(drawnBody->getCelestialBodyParameters().radius / 1000.0);
	}

	if(apparentAngle < 0.005)
	{
		unloadPlanet();
		if(apparentAngle < 0.0026)
		{
			return;
		}
	}
	else
	{
		loadPlanet();
	}
	if(culled)
	{
		return;
	}

	if(hostStar != nullptr)
	{
		Vector3 centralBodyCenter(camera.getRelativePositionTo(hostStar, uT));
		lightpos    = Utils::toQt(centralBodyCenter - camRelPos);
		lightradius = hostStar->getCelestialBodyParameters().radius;

		if(customModel)
		{
			lightpos /= drawnBody->getCelestialBodyParameters().radius / 1000.f;
			lightradius
			    /= drawnBody->getCelestialBodyParameters().radius / 1000.f;
		}
		else
		{
			lightpos /= drawnBody->getCelestialBodyParameters().radius;
			lightradius /= drawnBody->getCelestialBodyParameters().radius;
		}
	}

	unsigned int i(0);

	if(drawnBody->getParent() != nullptr
	   && drawnBody->getParent()->getOrbitableType() == Orbitable::Type::PLANET)
	{
		Vector3 parentRelPos(-1.0 * drawnBody->getRelativePositionAtUT(uT));
		float parentRad(dynamic_cast<Planet const*>(drawnBody->getParent())
		                    ->getCelestialBodyParameters()
		                    .radius);

		// in shader, drawnbody radius == 1.0
		if(customModel)
		{
			parentRelPos
			    /= drawnBody->getCelestialBodyParameters().radius / 1000.f;
			parentRad
			    /= drawnBody->getCelestialBodyParameters().radius / 1000.f;
		}
		else
		{
			parentRelPos /= drawnBody->getCelestialBodyParameters().radius;
			parentRad /= drawnBody->getCelestialBodyParameters().radius;
		}

		neighborsPosRadius[i] = QVector4D(Utils::toQt(parentRelPos), parentRad);
		neighborsOblateness[i] = QVector3D(
		    Utils::toQt(dynamic_cast<Planet const*>(drawnBody->getParent())
		                    ->getCelestialBodyParameters()
		                    .oblateness));
		++i;
	}
	std::vector<Orbitable*> const& children(drawnBody->getChildren());
	for(auto child : children)
	{
		if(i >= 5)
		{
			break;
		}
		if(child->getOrbitableType() != Orbitable::Type::PLANET)
		{
			continue;
		}

		auto childPlanet(dynamic_cast<Planet*>(child));
		Vector3 childPlanetRelPos(
		    CelestialBody::getRelativePositionAtUt(drawnBody, childPlanet, uT));
		float childPlanetRad(childPlanet->getCelestialBodyParameters().radius);

		// in shader, drawnbody radius == 1.0
		if(customModel)
		{
			childPlanetRelPos
			    /= drawnBody->getCelestialBodyParameters().radius / 1000.f;
			childPlanetRad
			    /= drawnBody->getCelestialBodyParameters().radius / 1000.f;
		}
		else
		{
			childPlanetRelPos /= drawnBody->getCelestialBodyParameters().radius;
			childPlanetRad /= drawnBody->getCelestialBodyParameters().radius;
		}

		neighborsPosRadius[i]
		    = QVector4D(Utils::toQt(childPlanetRelPos), childPlanetRad);
		neighborsOblateness[i] = QVector3D(
		    Utils::toQt(childPlanet->getCelestialBodyParameters().oblateness));
		++i;
	}
	for(; i < 5; ++i)
	{
		neighborsPosRadius[i]  = QVector4D(0.f, 0.f, 0.f, 0.f);
		neighborsOblateness[i] = QVector3D(1.f, 1.f, 1.f);
	}

	// custom models have (1, 0, 0) at planetographic origin
	// non custom have (-1, 0, 0) at planetographic origin
	if(!customModel)
	{
		QMatrix4x4 sideralRotationShift;
		sideralRotationShift.rotate(180.f, QVector3D(0.f, 0.f, 1.f));
		properRotation = properRotation * sideralRotationShift;
	}

	if(planet != nullptr)
	{
		planet->updateTextureLoading();
		float modelRadius(planet->updateModelLoading());
		customModel = (modelRadius > 0.f);
		if(customModel)
		{
			boundingSphere = modelRadius * 1000.f;
		}
	}
}

void PlanetRenderer::render(BasicCamera const& camera)
{
	if(culled)
	{
		return;
	}

	if(apparentAngle < 0.0026)
	{
		CelestialBodyRenderer::render(camera);
	}

	else if(apparentAngle < 0.005 || planet == nullptr || !planet->isValid())
	{
		GLHandler::setShaderParam(unloadedShader, "lightpos", lightpos);
		GLHandler::setShaderParam(unloadedShader, "lightradius", lightradius);
		GLHandler::setShaderParam(unloadedShader, "lightcolor", lightcolor);
		GLHandler::setShaderParam(unloadedShader, "neighborsPosRadius", 5,
		                          &(neighborsPosRadius[0]));
		GLHandler::setShaderParam(unloadedShader, "neighborsOblateness", 5,
		                          &(neighborsOblateness[0]));
		GLHandler::setShaderParam(unloadedShader, "properRotation",
		                          properRotation);
		GLHandler::setUpRender(unloadedShader, model);
		GLHandler::render(unloadedMesh);
	}
	else
	{
		planet->render(model, lightpos, lightradius, lightcolor,
		               neighborsPosRadius, neighborsOblateness, properRotation,
		               customModel);
	}
	handleDepth();
}

PlanetRenderer::~PlanetRenderer()
{
	GLHandler::deleteMesh(unloadedMesh);
	GLHandler::deleteShader(unloadedShader);
	unloadPlanet(true);
}

void PlanetRenderer::loadPlanet()
{
	if(planet != nullptr)
	{
		return;
	}

	auto drawnPlanet(dynamic_cast<Planet const*>(drawnBody));

	QString name(drawnPlanet->getName().c_str());

	planet = new DetailedPlanetRenderer(
	    1.f, Utils::toQt(drawnPlanet->getCelestialBodyParameters().oblateness));

	QString diffuse(""), normal("");
	QString str(QSettings().value("simulation/planetsystemdir").toString()
	            + "/images/" + name + "/diffuse.jpg");
	if(QFileInfo(str).exists())
	{
		diffuse = str;
	}
	str = QSettings().value("simulation/planetsystemdir").toString()
	      + "/images/" + name + "/diffuse.png";
	if(QFileInfo(str).exists())
	{
		diffuse = str;
	}
	if(diffuse != "")
	{
		str = QSettings().value("simulation/planetsystemdir").toString()
		      + "/images/" + name + "/normal.jpg";
		if(QFileInfo(str).exists())
		{
			normal = str;
		}
		str = QSettings().value("simulation/planetsystemdir").toString()
		      + "/images/" + name + "/normal.png";
		if(QFileInfo(str).exists())
		{
			normal = str;
		}
	}

	if(normal != "")
	{
		planet->initFromTex(diffuse, normal,
		                    drawnPlanet->getPlanetParameters().atmosphere);
	}
	else if(diffuse != "")
	{
		planet->initFromTex(diffuse,
		                    drawnPlanet->getPlanetParameters().atmosphere);
	}
	else if(drawnPlanet->getPlanetParameters().type == Planet::Type::GAZGIANT)
	{
		planet->initGazGiant(
		    drawnBody->getPseudoRandomSeed(),
		    Utils::toQt(drawnPlanet->getCelestialBodyParameters().color), 0.75f,
		    0.75f);
	}
	else
	{
		planet->initTerrestrial(
		    drawnBody->getPseudoRandomSeed(),
		    Utils::toQt(drawnPlanet->getCelestialBodyParameters().color), 90.f,
		    drawnPlanet->getPlanetParameters().atmosphere);
	}

	if(QFileInfo(QSettings().value("simulation/planetsystemdir").toString()
	             + "/models/" + name + ".ply")
	       .exists())
	{
		planet->updateModel(
		    QSettings().value("simulation/planetsystemdir").toString()
		    + "/models/" + name + ".ply");
	}

	// RINGS

	float outerRing(drawnPlanet->getPlanetParameters().outerRing);
	if(outerRing != 0.f)
	{
		float innerRing(drawnPlanet->getPlanetParameters().innerRing);
		float radius(drawnPlanet->getCelestialBodyParameters().radius);
		QString rings;
		str = QSettings().value("simulation/planetsystemdir").toString()
		      + "/images/" + name + "/rings.jpg";
		if(QFileInfo(str).exists())
		{
			rings = str;
		}
		str = QSettings().value("simulation/planetsystemdir").toString()
		      + "/images/" + name + "/rings.png";
		if(QFileInfo(str).exists())
		{
			rings = str;
		}

		planet->initRings(drawnBody->getPseudoRandomSeed(), innerRing / radius,
		                  outerRing / radius, rings);
	}
}

void PlanetRenderer::unloadPlanet(bool waitIfPlanetNotLoaded)
{
	if(planet == nullptr)
	{
		return;
	}

	if(planet->isLoading() && !waitIfPlanetNotLoaded)
	{
		planet->updateTextureLoading();
		planet->updateModelLoading();
		return;
	}

	delete planet;
	planet = nullptr;
}

QColor PlanetRenderer::computePointColor(Planet const& drawnPlanet,
                                         Star const* star)
{
	QColor lightcolor;
	if(star == nullptr)
	{
		lightcolor = QColor::fromRgbF(1.0, 1.0, 1.0, 1.0);
	}
	else
	{
		lightcolor = Utils::toQt(star->getCelestialBodyParameters().color);
	}
	Color color(drawnPlanet.getCelestialBodyParameters().color);
	color.r *= lightcolor.redF() / 4;
	color.g *= lightcolor.greenF() / 4;
	color.b *= lightcolor.blueF() / 4;
	return Utils::toQt(color);
}
