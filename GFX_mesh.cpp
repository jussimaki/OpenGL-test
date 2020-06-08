#include "GFX_mesh.h"

GFX_Mesh::GFX_Mesh(Vertex *vertices, unsigned int numVertices) : numVertices_(numVertices)
{
	
	glGenVertexArrays(1, &vertexArrayObject_);
	glBindVertexArray(vertexArrayObject_);

	glGenBuffers(1, vertexArrayBuffers_);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers_[0]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}

GFX_Mesh::~GFX_Mesh()
{
	glDeleteVertexArrays(1, &vertexArrayObject_);
}

void GFX_Mesh::draw()
{
	glBindVertexArray(vertexArrayObject_);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, numVertices_);
	glBindVertexArray(0);
}