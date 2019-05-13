#include "D3DInput.h"

#define HR(p) if(FAILED((p))) return false
#define IR(p) if(!(p)) return false
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

D3DInput::D3DInput()
{

}

D3DInput::~D3DInput()
{

}

bool D3DInput::Init(HINSTANCE instance, HWND wnd)
{
	memset(m_Keys, 0, sizeof(m_Keys));

	HRESULT result;

	result = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_InputDirect, NULL);
	HR(result);

	result = m_InputDirect->CreateDevice(GUID_SysKeyboard, &m_KeyBoard, NULL);
	HR(result);

	result = m_KeyBoard->SetDataFormat(&c_dfDIKeyboard);
	HR(result);

	result = m_KeyBoard->SetCooperativeLevel(wnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	HR(result);

	result = m_KeyBoard->Acquire();
	HR(result);
	
	result = m_InputDirect->CreateDevice(GUID_SysMouse ,&m_Mouse, NULL);
	HR(result);

	result = m_Mouse->SetDataFormat(&c_dfDIMouse);
	HR(result);

	result = m_Mouse->SetCooperativeLevel(wnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	HR(result);

	result = m_Mouse->Acquire();
	HR(result);
	return true;
}

bool D3DInput::Frame()
{
	bool result;
	IR(GetKeyState());
	IR(ReadMouse());
	return true;
}

void D3DInput::GetMouseLocation(int & x, int & y)
{
	x = m_MouseX;
	y = m_MouseY;
}

bool D3DInput::IsKeyDown(int key)
{
	if (m_Keys[key] & 0x80)
	{
		return true;
	}
	return false;
}

bool D3DInput::IsMouseLeftButtonDown()
{
	return (m_MouseState.rgbButtons[0] & 0x80 ) != 0 ;
}

bool D3DInput::ReadKeyBoard()
{
	HRESULT result;
	result = m_KeyBoard->GetDeviceState(sizeof(m_Keys), (LPVOID)&m_Keys);

	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_KeyBoard->Acquire();
		}
		else {
			throw "Update KeyBoardState Error!";
		}
	}
}

bool D3DInput::ReadMouse()
{
	HRESULT result;
	result = m_Mouse->GetDeviceState(sizeof(m_MouseState), (LPVOID)&m_MouseState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_Mouse->Acquire();
		}
		else {
			throw "Update Mouse Position Error!";
		}
	}
	m_MouseX += m_MouseState.lX;
	m_MouseY += m_MouseState.lY;
	if (m_MouseX < 0)
	{
		m_MouseX = 0;
	}
	if (m_MouseY < 0)
	{
		m_MouseY = 0;
	}
	if (m_MouseX > SCREEN_WIDTH)
	{
		m_MouseX = SCREEN_WIDTH;
	}
	if (m_MouseY > SCREEN_HEIGHT)
	{
		m_MouseY = SCREEN_HEIGHT;
	}
}

bool D3DInput::GetKeyState()
{
	HRESULT result;
	result = m_KeyBoard->GetDeviceState(sizeof(m_Keys), (LPVOID)&m_Keys);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_KeyBoard->Acquire();
		}
		else {
			return false;
		}
	}
	return true;
}

void D3DInput::Shutdown()
{
	if (m_KeyBoard)
	{
		m_KeyBoard->Unacquire();
	}
	m_KeyBoard->Release();
	m_InputDirect->Release();

}