#ifndef GFX_SHADER_H
#define GFX_SHADER_H


#include <GL/glew.h>
#include <string>


class GFX_Shader
{
public:
	std::string loadShader(std::string fileName);
	GLuint createShader(std::string input, GLenum shaderType);
	void bind();
	void update();

	GFX_Shader(std::string vertexShaderFileName, std::string fragmentShaderFileName, unsigned int xRes, unsigned int yRes);
	~GFX_Shader();

private:
	float left_, right_, top_, bottom_;
	GLuint program_, vertexShader_, fragmentShader_;

	GLint orthoID_;
	GFX_Shader() {}

};




#endif
