#define GLEW_STATIC

#include <GL/glew.h>


//#include <GL/GL.h>
//#include <GL/glext.h>
#include <windows.h>

#include "GFX_shader.h"
#include "GFX_mesh.h"
#include "GFX_sprite.h"

/*
int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	//MessageBox(NULL, TEXT("foo"), TEXT("MESSAGE"), MB_OK);

	return 0;
}
*/


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	unsigned int xRes = 1700;
	unsigned int yRes = 1000;
	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = L"OpenGLWindows";
	wc.style = CS_OWNDC;
	if (!RegisterClass(&wc))
		return 1;

	HWND hWnd = CreateWindowW(wc.lpszClassName, L"OpenGLWindows", WS_VISIBLE | WS_OVERLAPPEDWINDOW, 0, 0, xRes, yRes, 0, 0, hInstance, 0);


	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
		PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
		32,                        //Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                        //Number of bits for the depthbuffer
		8,                        //Number of bits for the stencilbuffer
		0,                        //Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	HDC ourWindowHandleToDeviceContext = GetDC(hWnd);


	int  letWindowsChooseThisPixelFormat;
	letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd);
	SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd);

	HGLRC ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
	wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);




	GLenum err = glewInit();

	if (err != GLEW_OK)
		MessageBoxA(0, "ERROR", "ERROR", 0);
	else
		MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);



	GLint m_viewport[4];

	glGetIntegerv(GL_VIEWPORT, m_viewport);

	int realXRes = m_viewport[2];
	int realYRes = m_viewport[3];


	GFX_Shader textureShader("vertex.txt", "fragmentTexture.txt", realXRes, realYRes);
	GFX_Shader shader("vertex.txt", "fragment.txt", realXRes, realYRes);
	GFX_Shader shaderBlue("vertex.txt", "fragmentBlue.txt", realXRes, realYRes);

	
	
	Vertex vertices[30];
	for (int index = 0; index < 10; index++)
	{
		vertices[index * 3 + 0] = Vertex(0 + index * 10, 0 + index * 10, 0);
		vertices[index * 3 + 1] = Vertex(9 + index * 10 + 1, 0 + index * 10, 0);
		vertices[index * 3 + 2] = Vertex(0 + index * 10, 9 + index * 10 + 1, 0);
	}

	Vertex blueVertices[30];
	for (int index = 0; index < 10; index++)
	{
		
		blueVertices[index * 3 + 0] = Vertex(9 + index * 10 + 1, 0 + index * 10, 0);
		blueVertices[index * 3 + 1] = Vertex(9 + index * 10 + 1, 9 + index * 10 + 1, 0);
		blueVertices[index * 3 + 2] = Vertex(0 + index * 10, 9 + index * 10 + 1, 0);
		
	}
	

	GFX_Mesh mesh(vertices, sizeof(vertices)/sizeof(Vertex));
	GFX_Mesh blueMesh(blueVertices, sizeof(blueVertices) / sizeof(Vertex));
	/*
	while (GetMessage(&msg, NULL, 0, 0) > 0)
		DispatchMessage(&msg);
	*/

	GFX_Sprite sprite;
	if (!sprite.loadSprite("jee.bmp")) return 0;
	

	float size = 0;
	unsigned char alpha = 0;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		glClearColor((float)alpha/ (float)255,0, (float)alpha/ (float)255, 255);
		glClear(GL_COLOR_BUFFER_BIT);

		/*
		shader.bind();
		shader.update();
		mesh.draw();

		shaderBlue.bind();
		shaderBlue.update();
		blueMesh.draw();

		*/


		//shaderBlue.bind();
		//shaderBlue.update();
		textureShader.bind();
		textureShader.update();

		
		//Rect dest = { 100, 100, size, size };


		size += 1;

		if (size > 100) size = 0;
		/*
		for (int x = 0; x < 50; x++)
		{
			for (int y = 0; y < 50; y++)
			{
				Rect dest = { size + x*sprite.getWidth()*10 , size + y *sprite.getHeight()*10, sprite.getWidth()*10,sprite.getHeight()*10 };
				sprite.draw(&dest, 0, 0);
			}
		}

		*/

		alpha++;
		sprite.draw2(size, size, alpha);

		

		if (alpha == 255) alpha = 0;
		SwapBuffers(GetDC(hWnd));
	}

	wglDeleteContext(ourOpenGLRenderingContext);
	

	return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_CREATE:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
			
	default:
	
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}