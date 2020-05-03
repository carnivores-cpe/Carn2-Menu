/***************************************************
* AtmosFear 2.1
* Hunt2.cpp
*
* Engine Related Processes
*
*/

#define _MAIN_
#include "Hunt.h"

#include "resource.h" // For IDI_ICON1

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>


std::streambuf* g_COutBuf = nullptr;
std::ofstream g_LogFile;
std::chrono::steady_clock::duration Timer::m_TimeStart;


void CreateLog()
{
#if !defined(_DEBUG)
	g_LogFile.open("menu.log", std::ios::trunc);

	if (g_LogFile.is_open()) {
		g_COutBuf = std::cout.rdbuf(g_LogFile.rdbuf());
	}
#endif //_DEBUG

	std::cout << "Carnivores Menu\nVersion: 1.1 " << __DATE__ << "\n" << std::setfill('=') << std::setw(40) << "=" << std::endl;
}


void CloseLogs()
{
#ifdef _DEBUG
	std::cout.rdbuf(g_COutBuf);
	g_LogFile.close();
#endif //_DEBUG
}


BOOL CALLBACK WindowProcedure(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONDOWN:
	{
		MenuMouseLEvent();
	}
	break;
	case WM_RBUTTONDOWN:
	{
		//MenuMouseREvent();
	}
	break;

	case WM_KEYDOWN:
	{
		if (wParam == VK_F9) { PostQuitMessage(1); }
		MenuKeyDownEvent(wParam);
	}
	break;
	case WM_CHAR:
	{
		MenuKeyCharEvent(wParam);
	}
	break;
	default:
		return (DefWindowProc(hwnd, message, wParam, lParam));
	}
	return FALSE;
}


bool CreateMainWindow()
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WindowProcedure;
	wc.hInstance = (HINSTANCE)hInst;
	wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "CarnivoresMenu2";

	if (!RegisterClassEx(&wc)) {
		std::stringstream ss;
		ss << "Failed to register the window class, error code" << std::hex << GetLastError() << std::endl;
		throw std::runtime_error(ss.str());
		return false;
	}

	hwndMain = CreateWindowEx(0,
		wc.lpszClassName,
		"Carnivores 2",
		WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,// |  WS_POPUP,
		0, 0, 800, 600,
		NULL, NULL, (HINSTANCE)hInst, NULL
	);

	if (!hwndMain) {
		std::stringstream ss;
		ss << "Failed to create the window, error code: " << std::hex << GetLastError() << std::endl;
		throw std::runtime_error(ss.str());
		return false;
	}

	hdcMain = GetDC(hwndMain);

	std::cout << "Main Window Creation: Ok!" << std::endl;

	// Resize the window so the client (drawing) area is 800x600, and reposition it to the center of the primary screen
	int WX = (GetSystemMetrics(SM_CXSCREEN) / 2) - 400;
	int WY = (GetSystemMetrics(SM_CYSCREEN) / 2) - 300;
	RECT rc = { WX, WY, WX + 800, WY + 600 };
	AdjustWindowRect(&rc, GetWindowLong(hwndMain, GWL_STYLE), false);
	SetWindowPos(hwndMain, HWND_TOP, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
	UpdateWindow(hwndMain);

	return true;
}


#ifdef _DEBUG
int main(int argc, char* argv[]) {
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {
#endif
	MSG msg = MSG();
	Timer::Init();

	try {
		// Create Log Files
		CreateLog();

		InitGameMenu();
		CreateMainWindow();
		InitNetwork();
		InitInterface();

		LoadResourcesScript();

		// -- Message Loop
		std::cout << "Entering Messages Loop." << std::endl;

		while (true)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)  break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				if (GetActiveWindow() == hwndMain) {
					ProcessMenu();
					// We need to sleep this_thread every frame, because the program emits a strange subtle 'noise' otherwise. It also
					// renders way too many frames
					//std::this_thread::sleep_for(std::chrono::milliseconds(5));
				}
				else {
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}
			}
		}

		std::cout << "Exited Message Loop." << std::endl;
	}
	catch (std::runtime_error& e) {
		std::cout << "! FATAL EXCEPTION: Runtime Error !" << std::endl;
		std::cout << "A runtime_error exception has occurred, the reason is:\n" << e.what() << std::endl;
	}
	catch (std::exception& e) {
		std::cout << "! FATAL EXCEPTION: C++ Standard Library !" << std::endl;
		std::cout << "A C++ Standard Library exception has occurred, the reason is:\n" << e.what() << std::endl;
	}

	ReleaseResources();
	ShutdownInterface();
	ShutdownNetwork();
	DestroyWindow(hwndMain);
	UnregisterClass("CarnivoresMenu2", hInst);
	CloseLogs();

	return msg.wParam;
}
