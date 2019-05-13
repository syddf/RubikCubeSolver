#ifndef D3D_INPUT_H
#define D3D_INPUT_H

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")

class D3DInput
{
public:
	D3DInput();
	~D3DInput();

public:
	bool																Init(HINSTANCE instance, HWND wnd);
	void																Shutdown();
	bool																Frame();
	void																GetMouseLocation(int & x, int & y);
	bool																GetKeyState();
	bool																IsKeyDown(int key);
	bool																IsMouseLeftButtonDown();

private:
	bool																ReadKeyBoard();
	bool																ReadMouse();

private:
	IDirectInput8 *												m_InputDirect;
	IDirectInputDevice8 *									m_KeyBoard;
	IDirectInputDevice8 *									m_Mouse;

private:
	unsigned char												m_Keys[256];
	DIMOUSESTATE											m_MouseState;
	int																m_ScreenWidth;
	int																m_ScreenHeight;
	int																m_MouseX;
	int																m_MouseY;
};


#endif