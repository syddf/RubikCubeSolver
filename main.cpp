#include "D3DApplication.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	D3DApplication * app = D3DApplication::Instance();
	app->Initialization();
	app->Run();
	return 0;
}