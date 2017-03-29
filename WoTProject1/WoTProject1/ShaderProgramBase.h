#pragma once
#include <GL/glew.h>
#include <string>

#include "Glm_Common.h"

namespace Shader
{
	class ShaderProgramBase {
	public:
		ShaderProgramBase(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
		~ShaderProgramBase();

		void bind();
		void unbind();

		GLuint getID() const;

	protected:
		virtual void getUniformLocations() = 0;


		void loadFloat(GLuint location, float value);
		void loadVector2(GLuint location, const Vector2& vector);
		void loadMatrix4(GLuint location, const Matrix4& matrix);

	private:
		GLuint m_programID;
	};
}