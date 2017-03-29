#pragma once
#include "Glm_Common.h"

class PlayerCamera;
namespace Maths
{
	Matrix4 createViewMatrix(const PlayerCamera& camera);
	//Matrix4 createModelMatrix(const Entity& entity);
	Matrix4 createProjMatrix();

}