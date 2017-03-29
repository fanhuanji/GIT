#pragma once
#include "Glm_Common.h"

#include <GL/glew.h>


// Default camera values
const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  3.0f;
const GLfloat SENSITIVTY =  0.25f;
const GLfloat ZOOM       =  45.0f;

const GLfloat MovementSpeed=0.1;
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
class PlayerCamera {
public:
	PlayerCamera();
	void update();
	GLfloat Zoom;
	//Vector3 position;
	//Vector3 rotation;
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	GLfloat MouseSensitivity;
	// Eular Angles
	GLfloat Yaw;
	GLfloat Pitch;

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}
	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) {
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw   += xoffset;
		this->Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if(constrainPitch) {
			if(this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if(this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		this->updateCameraVectors();
	}
	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset) {
		if(this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if(this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if(this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}
	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)//����������
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
		GLfloat velocity = MovementSpeed * deltaTime;
		if(direction == FORWARD)
			this->Position += this->Front * velocity;
		if(direction == BACKWARD)
			this->Position -= this->Front * velocity;
		if(direction == LEFT)
			this->Position -= this->Right * velocity;
		if(direction == RIGHT)
			this->Position += this->Right * velocity;
	}
private:
	//const Entity* m_P_entity = this;
private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors() {
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
	}
};