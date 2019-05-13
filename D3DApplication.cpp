#include "D3DApplication.h" 
int posX;
int posY;

D3DApplication::D3DApplication()
{
}

D3DApplication::~D3DApplication()
{
}

D3DApplication * D3DApplication::Instance()
{
	static D3DApplication* m_instance;
	if (!m_instance)
	{
		m_instance = new D3DApplication();
	}
	return m_instance;
}

void D3DApplication::Initialization()
{
	int width, height;
	InitWindow(width, height);

	m_graphics = new D3DGraphics();
	m_graphics->Initialization(width, height, m_hWnd);

	m_RubikCube = new RubikCube( m_graphics->ProjMatrix );
	m_RubikCube->InitResource( m_graphics , m_hWnd );

	m_Input = new D3DInput();
	m_Input->Init(m_hInstance, m_hWnd);
}

void D3DApplication::Shutdown()
{
}

void D3DApplication::Run()
{
	MSG msg;
	bool done, result;
	ZeroMemory(&msg, sizeof(MSG));
	done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			done = true;
			Shutdown();
		}
		else {
			m_graphics->Render();
			m_RubikCube->InputLogicUpdate(m_Input);
			m_RubikCube->Render(m_graphics->GetDeviceContext());
			m_Input->Frame();
			m_graphics->Present();

		}

	}
}

LRESULT D3DApplication::MessageHandler(HWND hwnd , UINT umessage , WPARAM wparam , LPARAM lparam )
{
	return DefWindowProc(hwnd, umessage, wparam, lparam);
}

void D3DApplication::InitWindow(int & width, int & height)
{
	WNDCLASSEX wc;
	m_hInstance = GetModuleHandle(NULL);
	m_ApplicationName = L"Test";

	//ÉèÖÃWNDCLASSEX
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.cbClsExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = m_ApplicationName;
	wc.lpszMenuName = NULL;

	RegisterClassEx(&wc);
	int fs_width = GetSystemMetrics(SM_CXSCREEN);
	int fs_height = GetSystemMetrics(SM_CYSCREEN);

	width = 800; height = 600;
	posX = (fs_width - width) / 2;
	posY = (fs_height - height) / 2;
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName, m_ApplicationName,
		WS_OVERLAPPEDWINDOW,
		posX, posY, width, height, NULL, NULL, m_hInstance, NULL);
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
	ShowCursor(true);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return D3DApplication::Instance()->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
