/*
    Copyright (C) 2020 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef CSVOBJECTS
#define CSVOBJECTS

#include <QColor>
#include <QFile>
#include <QString>
#include <algorithm>
#include <cmath>

#include "UniverseElement.hpp"
#include "gl/GLHandler.hpp"
#include "graphics/renderers/LabelRenderer.hpp"
#include "physics/Color.hpp"
#include "physics/blackbody.hpp"

class CSVObjects : public UniverseElement
{
  public:
	enum class Designation
	{
		UNKNOWN,
		PROPER,
		BAYER,
		FLAMSTEED
	};

	static QString desigToStr(Designation desig);

	struct StarName
	{
		Designation designation;
		QString name;
	};

	struct Object
	{
		double x;
		double y;
		double z;
		double absmag;
		QColor color;
		std::vector<StarName> names;
	};

	CSVObjects(QString const& csvFile, bool galaxies = false);
	CSVObjects(QString const& csvFile, QString const& constellationsFile);
	virtual BBox getBoundingBox() const override;
	virtual void render(Camera const& camera,
	                    ToneMappingModel const* tmm) override;
	virtual ~CSVObjects();

	float colormix = 0.0f;

	float constellationsLabels = 0.f;
	float constellationsAlpha  = 0.f;

  private:
	static float clamp(float x, float lo, float hi)
	{
		return (x < lo) ? lo : ((x > hi) ? hi : x);
	}
	static QColor colorFromColorIndex(float ci);
	static Object parseLine(QString const& line,
	                        std::map<QString, int> const& columnsNumbers);

	std::vector<Object> objects;

	std::map<QString, unsigned int> indexByName;

	GLShaderProgram shader;
	GLMesh mesh;
	static GLHandler::Texture& starTex();
	static GLHandler::Texture& galTex();
	static bool& texLoaded();

	bool galaxies = false;

	// CONSTELLATIONS
	bool containsConstellations = false;
	GLShaderProgram conShader;
	GLMesh conMesh;

	std::vector<std::pair<Vector3, LabelRenderer*>> conLabels;
};

#endif // CSVOBJECTS
