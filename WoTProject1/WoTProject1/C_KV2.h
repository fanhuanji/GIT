#pragma once
#include <gl/glew.h>
namespace Tank {
	class C_KV2:public Tank::C_TankBase {
		static const size_t nTurret=2, nGun=2, nChassis=2;
		static GLuint mChassis[nChassis], mHull[1], mTurret[nTurret], mGun[nGun];//VAOs
		C_KV2() {

		}

	};


GLuint C_KV2::mChassis[nChassis]={0};
GLuint C_KV2::mHull[1]={0};
GLuint C_KV2::mTurret[nTurret]={0};
GLuint C_KV2::mGun[nGun]={0};
}
