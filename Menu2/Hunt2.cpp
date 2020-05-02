/***************************************************
* AtmosFear 2.1
* Hunt2.cpp
*
* Engine Related Processes
*
*/

#define _MAIN_
#include "Hunt.h"

#include "resource.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>


std::streambuf* g_COutBuf = nullptr;
std::ofstream g_LogFile;


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
#endif
	g_LogFile.close();
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
		if (g_MenuState == MENU_REGISTER)
		{
			if (wParam == 8) {
				if (g_TypingBuffer.empty()) g_TypingBuffer.pop_back();
			}
			else {
				if (g_TypingBuffer.size() < 24) {
					if (wParam >= 32 || wParam <= 128) {
						g_TypingBuffer.push_back(static_cast<char>(wParam));
					}
				}
			}
		}
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
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,// |  WS_POPUP,
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

	// I assume this jargon is meant to fix the client area? gosh...
	RECT rcClient, rcWindow;
	int ptDiffx, ptDiffy;
	GetClientRect(hwndMain, &rcClient);
	GetWindowRect(hwndMain, &rcWindow);
	ptDiffx = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiffy = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	MoveWindow(hwndMain, rcWindow.left, rcWindow.top, 800 + ptDiffx, 600 + ptDiffy, TRUE);

	return true;
}


#ifndef _DEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
	MSG msg;

	try {
		// Create Log Files
		CreateLog();

		InitGameMenu(0);
		CreateMainWindow();
		InitNetwork();
		InitInterface();

		// (test)Send a HTTP GET message
		//NetworkGet( "/", "www.whatsmyip.org", "" );

		//NetworkGet( "/carnivores/servers.php?ip=124.181.149.112&name=Carnivores_Server", "www.epiczen.net", "null" );

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
				}
				else {
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
