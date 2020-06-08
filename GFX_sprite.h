#ifndef GFX_SPRITE_H
#define GFX_SPRITE_H

#include <GL/glew.h>
#include <string>

#include "GFX_mesh.h"



struct BMPData
{
	unsigned int width, height;
	unsigned int orig_width, orig_height;
	unsigned char *data;
};

struct Rect
{
	int x, y, width, height;
};

struct TextCoord
{
	TextCoord(GLfloat x, GLfloat y) : x_(x), y_(y){}
	TextCoord() {};
	GLfloat x_, y_;

};

struct Color
{
	GLfloat rgba_[4];
};

struct Physics
{
	float x, y;
	float xv, yv;

	unsigned char r, g, b;

	int rDelta, gDelta, bDelta;
};
class GFX_Sprite
{
public:
	bool loadSprite(std::string filename);
	
	~GFX_Sprite() { delete[] dynamicVertices_; delete[] dynamicTextCoords_; }

	void draw(Rect *destination, Rect *Source, Rect *clip);
	void draw2(int x, int y, unsigned char alpha);
	int getWidth() { return width_; }
	int getHeight() { return height_; }

private:

	int rows = 100, columns = 100;
	Vertex vertices_[6];
	TextCoord textCoords_[6];


	Vertex *dynamicVertices_;
	TextCoord *dynamicTextCoords_;
	Color *dynamicColors_;
	Physics *dynamicPhysics_;

	BMPData* readBMPData(char *fileName);
	void reverseBMPData(BMPData *data);

	int width_, height_;
	float textureWidth_, textureHeight_;
	GLuint texture_;

	GLuint vertexArrayObject_;
	GLuint vertexArrayBuffers_[3];
};


#endif
