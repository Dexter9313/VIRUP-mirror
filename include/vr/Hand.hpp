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

#ifndef HAND_H
#define HAND_H

#ifdef LEAP_MOTION
#include <Leap.h>
#endif
#include "../BasicCamera.hpp"
#include "../utils.hpp"

/** @ingroup pycall
 *
 * Callable in Python as the "leftHand" or "rightHand" objects when relevant (VR
 * is enabled and hands are available).
 */
class Hand : public QObject
{
	Q_OBJECT
	Q_PROPERTY(Side side READ getSide)
	Q_PROPERTY(bool isvalid READ isValid)
	Q_PROPERTY(bool isflat READ isFlat)
	Q_PROPERTY(bool isclosed READ isClosed)
	Q_PROPERTY(QVector3D palmnormal READ palmNormal)
	Q_PROPERTY(QVector3D palmposition READ palmPosition)
	Q_PROPERTY(QVector3D direction READ direction)

  public:
	explicit Hand(Side side);
	Side getSide() const { return side; };
	bool isValid() const { return _isValid; };
	bool isFlat() const { return _isFlat; };
	bool isClosed() const { return _isClosed; };
	QVector3D palmNormal() const { return _palmNormal; };
	QVector3D palmPosition() const { return _palmPosition; };
	QVector3D direction() const { return _direction; };
	void invalidate() { _isValid = false; };
#ifdef LEAP_MOTION
	void update(Leap::Hand const& hand);
#endif
	void render() const;

	const Side side;

  private:
	GLShaderProgram shaderProgram;
	GLMesh mesh;
	QMatrix4x4 model;

	bool _isValid;
	bool _isFlat;
	bool _isClosed;
	QVector3D _palmNormal;
	QVector3D _palmPosition;
	QVector3D _direction;

#ifdef LEAP_MOTION
	static std::vector<float> leapMotionToGL(Leap::Vector const& v);
	static QVector3D leapMotionToGLM(Leap::Vector const& v);
	static std::vector<float> getHandVBO(Leap::Hand const& hand);
#endif
};

#endif // HAND_H
