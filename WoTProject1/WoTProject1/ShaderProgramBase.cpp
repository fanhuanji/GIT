
#include "stdafx.h"
#include "ShaderProgramBase.h"

#include "ShaderLoader.h"

namespace Shader
{
	ShaderProgramBase::ShaderProgramBase(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
		: m_programID(loadShader(vertexShaderFile, fragmentShaderFile)) {
	}

	ShaderProgramBase::~ShaderProgramBase() {
		glDeleteProgram(m_programID);
	}

	void ShaderProgramBase::bind() {
		glUseProgram(m_programID);
	}

	void ShaderProgramBase::unbind() {
		glUseProgram(0);
	}

	void ShaderProgramBase::loadFloat(GLuint location, float value) {
		glUniform1f(location, value);
	}

	void ShaderProgramBase::loadVector2(GLuint location, const Vector2& vector) {
		glUniform2f(location, vector.x, vector.y);
	}

	void ShaderProgramBase::loadMatrix4(GLuint location, const Matrix4& matrix) {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}


	GLuint ShaderProgramBase::getID() const {
		return m_programID;
	}

}
