#include <fstream>

#include "GFX_shader.h"

std::string GFX_Shader::loadShader(std::string fileName)
{
	std::ifstream file;

	std::string output;

	file.open(fileName);

	if (file.is_open())
	{
		while (file.good())
		{
			std::string line;

			std::getline(file, line);

			output.append(line + '\n');
		}

		file.close();
	}

	return output;
}

GLuint GFX_Shader::createShader(std::string input, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);

	const GLchar *strings[1];
	GLint lengths[1];

	strings[0] = input.c_str();
	lengths[0] = input.length();

	glShaderSource(shader, 1, strings, lengths);
	glCompileShader(shader);

	return shader;
}

void GFX_Shader::bind()
{
	glUseProgram(program_);
}

GFX_Shader::GFX_Shader(std::string vertexShaderFileName, std::string fragmentShaderFileName, unsigned int xRes, unsigned int yRes) : left_(0), right_(xRes), top_(0), bottom_(yRes)
{

	program_ = glCreateProgram();


	vertexShader_ = createShader(loadShader(vertexShaderFileName), GL_VERTEX_SHADER);
	fragmentShader_ = createShader(loadShader(fragmentShaderFileName), GL_FRAGMENT_SHADER);


	glAttachShader(program_, vertexShader_);
	glAttachShader(program_, fragmentShader_);
	
	std::string vertex = loadShader(vertexShaderFileName);
	std::string fragment = loadShader(fragmentShaderFileName);


	glAttachShader(program_, vertexShader_);
	glAttachShader(program_, fragmentShader_);


	glBindAttribLocation(program_, 0, "position");
	glBindAttribLocation(program_, 1, "texCoord");
	glBindAttribLocation(program_, 2, "spriteColor");
	glLinkProgram(program_);
	glValidateProgram(program_);


	orthoID_ = glGetUniformLocation(program_, "ORTHO");



	


}

void GFX_Shader::update()
{
	float tx = -(right_ + left_) / (right_ - left_);
	float ty = -(top_ + bottom_) / (top_ - bottom_);
	float tz = -(-1.0f + 1.0f) / (-1.0f - 1.0f);
	
	GLfloat data[] = {
		2.0f / (right_ - left_), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top_ - bottom_), 0.0f, 0.0f,
		0.0f, 0.0f, -2.0f / (-1.0f - 1.0f), 0.0f,
		tx, ty, tz, 1.0f
	};

	/*
	GLfloat data[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	*/
	glUniformMatrix4fv(orthoID_, 1, GL_FALSE, data);
}


GFX_Shader::~GFX_Shader()
{
	glDetachShader(program_, vertexShader_);
	glDeleteShader(vertexShader_);

	glDetachShader(program_, fragmentShader_);
	glDeleteShader(fragmentShader_);

	glDeleteProgram(program_);
}