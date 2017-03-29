#pragma once
#include <gl/glew.h>
namespace Tank {
	class C_TankBase {
		static const size_t nTurret=2, nGun=2, nChassis=2;
		static GLuint *mChassis, *mHull, *mTurret, *mGun;//VAOs
		const GLfloat leftAng=180, rightAng=-180, upAng=12, downAng=-5;
		glm::vec3 m_position, m_angle,m_targetpos;//玩家坐标，车身转角，准星瞄准位置
	public:
		C_TankBase() {

		}
		glm::vec3 GetPosition()const {
			return m_position;
		}
		glm::vec3 GetTargetPos() {
			return m_targetpos;
		}
		virtual void load()=0;

		virtual void draw(GLfloat x=0, GLfloat y=0, GLfloat z=0,
						  GLfloat angleX=0, GLfloat angleY=0, GLfloat angleZ=0,
						  GLfloat turretrot=0, GLfloat gunrot=0) {
		}
		virtual ~C_TankBase() {

		}
	};
	//GLuint* C_TankBase::mChassis=nullptr;
	//GLuint* C_TankBase::mHull=nullptr;
	//GLuint* C_TankBase::mTurret=nullptr;
	//GLuint* C_TankBase::mGun=nullptr;

}

#include "C_KV2.h"