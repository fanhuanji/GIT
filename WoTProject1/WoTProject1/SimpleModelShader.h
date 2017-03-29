#pragma once
#include "ShaderProgramBase.h"

namespace Shader
{
	class SimpleModelShader: public ShaderProgramBase {
	public:
		SimpleModelShader();
		~SimpleModelShader() = default;

		void setViewMatrix(const Matrix4& matrix);
		void setModelMatrix(const Matrix4& matrix);
		void setProjMatrix(const Matrix4& matrix);

	protected:
		SimpleModelShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

		virtual void getUniformLocations() override;

		GLuint m_locationViewMatrix     = 0;
		GLuint m_locationModelMatrix    = 0;
		GLuint m_locationProjMatrix     = 0;
	};
}