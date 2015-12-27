//// Copyright 2015 Carl Hewett
////
//// This file is part of SDL3D.
////
//// SDL3D is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// SDL3D is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <Entity.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For lookAt() and others

class Camera : public Entity
{
private:
	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix; // Gives perspective

	glm::vec4 mDirection; // Point or vector
	glm::vec3 mUpVector; // glm::vec3(0, -1, 0) will the camera be upside down, in contrast to glm::vec3(0, 1, 0);
						// This up vector represents the direction of the up side of the camera.

	float mFieldOfViewX; // The horizontal field of view, in degrees
	float mAspectRatio;
	float mNearClippingPlane;
	float mFarClippingPlane;

	void init();

public:
	Camera();
	Camera(float fieldOfView, float aspectRatio);
	~Camera();

	void setDirection(glm::vec4 direction);
	void setUpVector(glm::vec3 upVector);

	void setFieldOfView(float mFieldOfViewX);
	void setAspectRatio(float aspectRatio);

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
};

#endif /* CAMERA_HPP_ */