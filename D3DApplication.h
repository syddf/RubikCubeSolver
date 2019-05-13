#ifndef _D3D_APPLICATION_H_
#define _D3D_APPLICATION_H_

#include <Windows.h>
#include "D3DGraphics.h"
#include "RubikCube.h"
#include "D3DInput.h"

class D3DApplication
{
public:
	D3DApplication();
	~D3DApplication();
	static D3DApplication * Instance();
public:
	void	Initialization();
	void Shutdown();
	void Run();
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	void InitWindow( int & width , int & height ) ;

private:
	LPCWSTR m_ApplicationName;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	D3DGraphics * m_graphics;
	RubikCube * m_RubikCube;
	D3DInput * m_Input;
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

#endif