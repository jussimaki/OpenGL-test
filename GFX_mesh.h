#ifndef GFX_MESH_H
#define GFX_MESH_H

#include <GL/glew.h>


struct Vertex
{
	GLfloat x_, y_, z_;
	
	Vertex(GLfloat x, GLfloat y, GLfloat z) : x_(x), y_(y), z_(z){}
	Vertex() {};
};

class GFX_Mesh
{
public:
	GFX_Mesh(Vertex *vertices, unsigned int numVertices);
	~GFX_Mesh();

	void draw();

private:

	unsigned int numVertices_;
	GLuint vertexArrayObject_;
	GLuint vertexArrayBuffers_[1];

};

#endif
