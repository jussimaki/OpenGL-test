#include "GFX_sprite.h"
#include "GFX_mesh.h"

#include "file.h"


BMPData* GFX_Sprite::readBMPData(char *fileName)
{
	BMPData *structure = new BMPData;

	//std::ifstream file(filename, std::ios_base::binary);

	BinaryFile file;
	openBinaryFile(fileName, &file, LITTLE_ENDIAN);

	unsigned short type;
	unsigned int useless_dword;
	unsigned short useless_word;
	unsigned short bits;
	unsigned int size;

	type = readUWord(&file);


	if (type != 0x4D42) { delete structure; closeBinaryFile(&file); return 0; }

	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);

	structure->orig_width = readUDword(&file);
	structure->orig_height = readUDword(&file);


	// the texture width and height must be in a power of 2
	unsigned int correctWidth = 1;
	while (correctWidth < structure->orig_width) correctWidth *= 2;

	unsigned int correctHeight = 1;
	while (correctHeight < structure->orig_height) correctHeight *= 2;

	unsigned int oldWidth = structure->orig_width;
	unsigned int oldHeight = structure->orig_height;

	structure->width = correctWidth;
	structure->height = correctHeight;

	structure->data = new unsigned char[structure->width*structure->height * 4];

	useless_word = readUWord(&file);

	bits = readUWord(&file);

	if (bits != 24) { delete[]structure->data; delete structure; closeBinaryFile(&file); return 0; }

	useless_dword = readUDword(&file);

	size = readUDword(&file);

	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);
	useless_dword = readUDword(&file);

	// the amount of useless bytes to make the bitmap's horizontal byte amount to be divisible by 4
	unsigned char useless_bytes = (4 - ((oldWidth * 3) % 4)) % 4;

	unsigned char *dataChunk = new unsigned char[(3 * oldWidth + useless_bytes)*oldHeight];

	readToBuffer((char*)dataChunk, sizeof(unsigned char)*((3 * oldWidth + useless_bytes)*oldHeight), &file);



	unsigned char *dataPointer = dataChunk;

	int spot = 4 * correctWidth*(correctHeight - oldHeight);



	for (unsigned int row = 0; row < oldHeight; row++)
	{
		for (unsigned int column = 0; column < oldWidth; column++)
		{

			structure->data[spot + 2] = dataPointer[0];
			structure->data[spot + 1] = dataPointer[1];
			structure->data[spot + 0] = dataPointer[2];

			dataPointer += 3;
			if (structure->data[spot] == 255 && structure->data[spot + 2] == 255 &&
				structure->data[spot + 1] == 0) structure->data[spot + 3] = 0;
			else structure->data[spot + 3] = 255;


			spot += 4;
		}

		spot += 4 * (correctWidth - oldWidth);
		dataPointer += useless_bytes;
	}



	delete[]dataChunk;

	closeBinaryFile(&file);

	return structure;

}

void GFX_Sprite::reverseBMPData(BMPData *data)
{
	unsigned char *row_data;

	row_data = new unsigned char[4 * data->width];

	for (unsigned int row = 0; row < data->height/2; row++)
	{

		// copy the bottom row to row_data
		memcpy(
			row_data,
			&(data->data[data->width * 4 * (data->height - 1 - row)]), 4 * sizeof(unsigned char)*data->width);

		// copy the top row to bottom row
		memcpy(
			&(data->data[data->width * 4 * (data->height - 1 - row)]),
			&(data->data[data->width * 4 * (row)]), 4 * sizeof(unsigned char)*data->width);

		// copy row_data to top row
		memcpy(
			&(data->data[data->width * 4 * (row)]),
			row_data, 4 * sizeof(unsigned char)*data->width);
	}

	delete[]row_data;
}

bool GFX_Sprite::loadSprite(std::string filename)
{
	BMPData *loaded_BMP;
		
	loaded_BMP = readBMPData((char*)filename.c_str());

	if (!loaded_BMP) return false;

	reverseBMPData(loaded_BMP);
		

	textureWidth_ = loaded_BMP->width;
	textureHeight_ = loaded_BMP->height;

	width_ = loaded_BMP->orig_width;
	height_ = loaded_BMP->orig_height;


	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &texture_);
		
	glBindTexture(GL_TEXTURE_2D, texture_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth_, textureHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, loaded_BMP->data);

	delete[]loaded_BMP->data;
	delete loaded_BMP;




	glGenVertexArrays(1, &vertexArrayObject_);
	glGenBuffers(3, vertexArrayBuffers_);
	glBindVertexArray(0);
	
	
	dynamicVertices_ = new Vertex[6 * rows * columns];
	dynamicTextCoords_ = new TextCoord[6 * rows * columns];
	dynamicColors_ = new Color[6 * rows * columns];

	dynamicPhysics_ = new Physics[rows*columns];

	for (int index = 0; index < rows*columns; index++)
	{
		dynamicPhysics_[index].x = rand() % 1000;
		dynamicPhysics_[index].y = rand() % 1000;
		dynamicPhysics_[index].xv = (rand() % 6) - 3;
		dynamicPhysics_[index].yv = (rand() % 6) - 3;
		dynamicPhysics_[index].r = rand() % 256;
		dynamicPhysics_[index].g = rand() % 256;
		dynamicPhysics_[index].b = rand() % 256;

		dynamicPhysics_[index].rDelta = rand()%7 -3;
		dynamicPhysics_[index].gDelta = rand() % 7 - 3;
		dynamicPhysics_[index].bDelta = rand() % 7 - 3;



	}

	dynamicPhysics_[0].r = 255;
	dynamicPhysics_[0].g = 255;
	dynamicPhysics_[0].b = 255;

	dynamicPhysics_[0].rDelta = 0;
	dynamicPhysics_[0].gDelta = 0;
	dynamicPhysics_[0].bDelta = 0;

	
	

	return true;

}
void GFX_Sprite::draw2(int x, int y, unsigned char alpha)
{
	int side = 60;


	for (int index = 0; index < rows*columns; index++)
	{
		dynamicPhysics_[index].x += dynamicPhysics_[index].xv;
		dynamicPhysics_[index].y += dynamicPhysics_[index].yv;


		if (dynamicPhysics_[index].x < 0 && dynamicPhysics_[index].xv < 0) dynamicPhysics_[index].xv *= -1;
		if (dynamicPhysics_[index].y < 0 && dynamicPhysics_[index].yv < 0) dynamicPhysics_[index].yv *= -1;

		if (dynamicPhysics_[index].x > 1700 && dynamicPhysics_[index].xv > 0) dynamicPhysics_[index].xv *= -1;
		if (dynamicPhysics_[index].y > 1200 && dynamicPhysics_[index].yv > 0) dynamicPhysics_[index].yv *= -1;
		/*
		for (int index2 = 0; index2 < rows*columns; index2++)
		{
			if (index == index2) continue;

			if (
				dynamicPhysics_[index].x + side > dynamicPhysics_[index2].x &&
				dynamicPhysics_[index].x < dynamicPhysics_[index2].x + side &&
				dynamicPhysics_[index].y + side > dynamicPhysics_[index2].y &&
				dynamicPhysics_[index].y < dynamicPhysics_[index2].y + side
				)
			{
				dynamicPhysics_[index].xv *= -1;
				dynamicPhysics_[index].yv *= -1;

				dynamicPhysics_[index2].xv *= -1;
				dynamicPhysics_[index2].yv *= -1;
			}
		}
		*/
	}


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glBindVertexArray(vertexArrayObject_);

	int index = 0;

	for (int yloop = 0; yloop < rows; yloop++)
	{
		for (int xloop = 0; xloop < columns; xloop++)
		{
		
			float xPos = dynamicPhysics_[index].x;
			float yPos = dynamicPhysics_[index].y;

			dynamicVertices_[index*6+0] = Vertex(xPos, yPos, 0);
			dynamicVertices_[index * 6 + 1] = Vertex(xPos + side, yPos, 0);
			dynamicVertices_[index * 6 + 2] = Vertex(xPos, yPos + side, 0);
			dynamicVertices_[index * 6 + 3] = Vertex(xPos + side, yPos, 0);
			dynamicVertices_[index * 6 + 4] = Vertex(xPos + side, yPos + side, 0);
			dynamicVertices_[index * 6 + 5] = Vertex(xPos, yPos + side, 0);

			

			float fx = 0.0f, fy = 0.0f;
			float fWidth = width_, fHeight = height_;
			float divX = textureWidth_, divY = textureHeight_;

			dynamicTextCoords_[index * 6 + 0] = TextCoord(fx / divX, fy / divY);
			dynamicTextCoords_[index * 6 + 1] = TextCoord((fx + fWidth) / divX, fy / divY);
			dynamicTextCoords_[index * 6 + 2] = TextCoord(fx / divX, (fy + fHeight) / divY);
			dynamicTextCoords_[index * 6 + 3] = TextCoord((fx + fWidth) / divX, fy / divY);
			dynamicTextCoords_[index * 6 + 4] = TextCoord((fx + fWidth) / divX, (fy + fHeight) / divY);
			dynamicTextCoords_[index * 6 + 5] = TextCoord(fx / divX, (fy + fHeight) / divY);

			dynamicPhysics_[index].r += dynamicPhysics_[index].rDelta;
			dynamicPhysics_[index].g += dynamicPhysics_[index].gDelta;
			dynamicPhysics_[index].b += dynamicPhysics_[index].bDelta;

			unsigned char r= dynamicPhysics_[index].r;
			unsigned char g = dynamicPhysics_[index].g;
			unsigned char b = dynamicPhysics_[index].b;
			alpha = 255;
			for (int vertex = 0; vertex < 6; vertex++)
			{
				dynamicColors_[index * 6 + vertex].rgba_[0] = ((float)r) / (float)255;
				dynamicColors_[index * 6 + vertex].rgba_[1] = ((float)g) / (float)255;
				dynamicColors_[index * 6 + vertex].rgba_[2] = ((float)b) / (float)255;
				dynamicColors_[index * 6 + vertex].rgba_[3] = ((float)alpha) / (float)255;
			}

			
			index ++;
		}
	}



	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers_[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * rows*columns*sizeof(Vertex), dynamicVertices_, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers_[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * rows*columns*sizeof(TextCoord), dynamicTextCoords_, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);


	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers_[2]);
	glBufferData(GL_ARRAY_BUFFER, 6 * rows*columns * sizeof(Color), dynamicColors_, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);



	glBindBuffer(GL_ARRAY_BUFFER, 0);








	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, 6*300*300);


	glBindVertexArray(0);

}


void GFX_Sprite::draw(Rect *destination, Rect *Source, Rect *clip)
{
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_);
	/*
	vertices_[0] = Vertex(destination->x, destination->y, 0);
	vertices_[1] = Vertex(destination->x + destination->width, destination->y, 0);
	vertices_[2] = Vertex(destination->x, destination->y + destination->height, 0);
	vertices_[3] = Vertex(destination->x + destination->width, destination->y, 0);
	vertices_[4] = Vertex(destination->x + destination->width, destination->y + destination->height, 0);
	vertices_[5] = Vertex(destination->x, destination->y + destination->height, 0);
	

	
	textCoords_[0] = TextCoord(0, 0);
	textCoords_[1] = TextCoord(1, 0);
	textCoords_[2] = TextCoord(0, 1);
	textCoords_[3] = TextCoord(1, 0);
	textCoords_[4] = TextCoord(1, 1);
	textCoords_[5] = TextCoord(0, 1);
	*/

	vertices_[0] = Vertex(destination->x, destination->y, 0);
	vertices_[1] = Vertex(destination->x + destination->width, destination->y, 0);
	vertices_[2] = Vertex(destination->x, destination->y + destination->height, 0);
	vertices_[3] = Vertex(destination->x + destination->width, destination->y, 0);
	vertices_[4] = Vertex(destination->x + destination->width, destination->y + destination->height, 0);
	vertices_[5] = Vertex(destination->x, destination->y + destination->height, 0);


	float fx = 0.0f, fy = 0.0f;
	float fWidth = width_, fHeight = height_;
	float divX = textureWidth_, divY = textureHeight_;

	textCoords_[0] = TextCoord(fx / divX, fy / divY);
	textCoords_[1] = TextCoord((fx + fWidth) / divX, fy / divY);
	textCoords_[2] = TextCoord(fx / divX, (fy + fHeight) / divY);
	textCoords_[3] = TextCoord((fx + fWidth) / divX, fy / divY);
	textCoords_[4] = TextCoord((fx + fWidth) / divX, (fy + fHeight) / divY);
	textCoords_[5] = TextCoord(fx / divX, (fy + fHeight) / divY);



	glBindVertexArray(vertexArrayObject_);



	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers_[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), vertices_, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers_[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(TextCoord), textCoords_, GL_DYNAMIC_DRAW);


	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	

	

		

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, 6);


	glBindVertexArray(0);


}