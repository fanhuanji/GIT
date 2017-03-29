#include "stdafx.h"

#include "SimpleModelShader.h"
#include "MatrixMath.h"
namespace Shader
{
	SimpleModelShader::SimpleModelShader()
		: ShaderProgramBase("model_loading.vs", "model_loading.frag") {
		getUniformLocations();

		bind();
		loadMatrix4(m_locationProjMatrix, Maths::createProjMatrix());
		unbind();

	}

	SimpleModelShader::SimpleModelShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
		: ShaderProgramBase(vertexShaderFile, fragmentShaderFile) {

	}


	void SimpleModelShader::setViewMatrix(const Matrix4& matrix) {
		loadMatrix4(m_locationViewMatrix, matrix);
	}

	void SimpleModelShader::setModelMatrix(const Matrix4& matrix) {
		loadMatrix4(m_locationModelMatrix, matrix);
	}

	void SimpleModelShader::setProjMatrix(const Matrix4& matrix) {
		loadMatrix4(m_locationProjMatrix, matrix);
	}


	void SimpleModelShader::getUniformLocations() {
		m_locationViewMatrix    = glGetUniformLocation(getID(), "viewMatrix");
		m_locationModelMatrix   = glGetUniformLocation(getID(), "modelMatrix");
		m_locationProjMatrix    = glGetUniformLocation(getID(), "projMatrix");

	}

}
