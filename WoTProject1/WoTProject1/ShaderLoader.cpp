
#include "stdafx.h"
#include "ShaderLoader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace Shader
{
	GLuint compileShader(const GLchar* source, GLenum type) {
		auto id = glCreateShader(type);

		glShaderSource(id, 1, &source, nullptr);
		glCompileShader(id);

		GLint isSuccess;
		GLchar infoLog[512];

		glGetShaderiv(id, GL_COMPILE_STATUS, &isSuccess);

		if(!isSuccess) {
			glGetShaderInfoLog(id, 512, nullptr, infoLog);
			throw std::runtime_error("Error compiling shader: " + std::string(infoLog));
		}

		return id;
	}

	std::string getSource(const std::string& sourceFile) {
		std::ifstream inFile("Shaders/" + sourceFile + ".glsl");
		std::string source;
		std::stringstream stringStream;

		if(!inFile.is_open()) {
			std::cerr << ("ERROR LOADING SHADERS" "Could not open file: " + sourceFile) << std::endl;
			std::cerr << "Shaders/" + sourceFile + ".glsl" << std::endl;
			throw std::runtime_error("Could not open file: " + sourceFile);
		}

		stringStream << inFile.rdbuf();
		source = stringStream.str();

		return source;
	}

	GLuint createProgram(GLuint vertexShaderID, GLuint fragmentShaderID) {
		GLuint id = glCreateProgram();

		glAttachShader(id, vertexShaderID);
		glAttachShader(id, fragmentShaderID);

		glLinkProgram(id);

		return id;
	}

	GLuint loadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {
		std::string vertexSource   = getSource(vertexShaderFile);
		std::string fragmentSource = getSource(fragmentShaderFile);

		GLuint vertexShaderID     = compileShader(vertexSource.c_str(), GL_VERTEX_SHADER);
		GLuint fragmentShaderID   = compileShader(fragmentSource.c_str(), GL_FRAGMENT_SHADER);

		GLuint programID = createProgram(vertexShaderID, fragmentShaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);

		return programID;
	}
}










