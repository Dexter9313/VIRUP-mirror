/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@epfl.ch>

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

#include "vr/Hand.hpp"

Hand::Hand(Side side)
    : side(side)
    , shaderProgram("default")
    , _isValid(false)
    , _isFlat(false)
    , _isClosed(false)
    , _palmNormal()
    , _direction()
{
	shaderProgram.setUniform("alpha", 1.f);
	std::vector<unsigned int> ebo = {
	    2,  3,  3,  4,                                        // thumb
	    5,  6,  6,  7,  7,  8,                                // index
	    9,  10, 10, 11, 11, 12,                               // middle
	    13, 14, 14, 15, 15, 16,                               // ring
	    17, 18, 18, 19, 19, 20,                               // pinkie
	    2,  5,  5,  9,  9,  13, 13, 17, 17, 21, 21, 22, 22, 2 // palm
	};

	std::vector<float> vertices(22 * 3); // 21 random positions
	mesh.setVertexShaderMapping(shaderProgram, {{"position", 3}});
	mesh.setVertices(vertices, ebo);

	// transforms leap coordinates to GL coordinates
	// account for the controller position on the headset
	// fully empirical
	model.setColumn(0, {-1.f / 1000.f, 0, 0, 0});
	model.setColumn(1, {0, 0, -1.f / 1000.f, 0});
	model.setColumn(2, {0, -1.f / 1000.f, 0, 0});
	model.setColumn(3, {0, -0.05f, -0.05f, 1.f});
}

#ifdef LEAP_MOTION
void Hand::update(Leap::Hand const& hand)
{
	_isValid = hand.isValid();
	if(!_isValid)
	{
		return;
	}

	mesh.setVertices(getHandVBO(hand));

	_isFlat   = hand.grabStrength() == 0;
	_isClosed = hand.grabStrength() == 1;

	_palmNormal = leapMotionToGLM(hand.palmNormal());
	_palmNormal = QVector3D(model * QVector4D(_palmNormal, 0.f));
	_palmNormal.normalize();

	_palmPosition = leapMotionToGLM(hand.palmPosition());
	_palmPosition = QVector3D(model * QVector4D(_palmPosition, 1.f));

	_direction = leapMotionToGLM(hand.direction());
	_direction = QVector3D(model * QVector4D(_direction, 0.f));
	_direction.normalize();
}
#endif

void Hand::render() const
{
	GLHandler::setUpRender(shaderProgram, model,
	                       GLHandler::GeometricSpace::HMD);
	if(isFlat() && side == Side::LEFT)
	{
		shaderProgram.setUniform("color", QColor::fromRgbF(1.0f, 1.0f, 0.0f));
	}
	else if(isFlat())
	{
		shaderProgram.setUniform("color", QColor::fromRgbF(0.0f, 1.0f, 1.0f));
	}
	else if(isClosed() && side == Side::LEFT)
	{
		shaderProgram.setUniform("color", QColor::fromRgbF(1.0f, 1.0f, 1.0f));
	}
	else if(isClosed())
	{
		shaderProgram.setUniform("color", QColor::fromRgbF(1.0f, 0.0f, 1.0f));
	}
	else if(side == Side::LEFT)
	{
		shaderProgram.setUniform("color", QColor::fromRgbF(1.0f, 0.0f, 0.0f));
	}
	else
	{
		shaderProgram.setUniform("color", QColor::fromRgbF(0.0f, 1.0f, 0.0f));
	}

	mesh.render(PrimitiveType::LINES);
}

#ifdef LEAP_MOTION
std::vector<float> Hand::leapMotionToGL(Leap::Vector const& v)
{
	return {v.x, v.y, v.z};
}

QVector3D Hand::leapMotionToGLM(Leap::Vector const& v)
{
	return {v.x, v.y, v.z};
}

std::vector<float> Hand::getHandVBO(Leap::Hand const& hand)
{
	std::vector<float> result = {};
	append(result, leapMotionToGL(hand.palmPosition()));
	for(Leap::Finger finger : hand.fingers())
	{
		append(result,
		       leapMotionToGL(
		           finger.bone(Leap::Bone::TYPE_METACARPAL).nextJoint()));
		append(result, leapMotionToGL(
		                   finger.bone(Leap::Bone::TYPE_PROXIMAL).nextJoint()));
		append(result,
		       leapMotionToGL(
		           finger.bone(Leap::Bone::TYPE_INTERMEDIATE).nextJoint()));
		append(result, leapMotionToGL(
		                   finger.bone(Leap::Bone::TYPE_DISTAL).nextJoint()));
	}
	Leap::Vector wrist(hand.arm().wristPosition());
	// from wrist center to below pinkie
	Leap::Vector outward
	    = hand.palmNormal().cross(hand.direction()).normalized();
	if(hand.isLeft())
	{
		outward *= -1;
	}
	append(result,
	       leapMotionToGL(wrist + 0.75f * hand.palmWidth() * outward / 2.f));
	append(result,
	       leapMotionToGL(wrist - 0.75f * hand.palmWidth() * outward / 2.f));
	return result;
}
#endif
