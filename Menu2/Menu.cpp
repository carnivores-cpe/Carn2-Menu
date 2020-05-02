/***************************************************
* AtmosFear 2.1
* Menu.cpp
*
* Main Menu Code
*
*/

#include "Hunt.h"
#include <cmath>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>


using namespace std;


class TMenuSet {
public:
	int			x0, y0;
	int			Count;
	std::string	Item[32];
};


TMenuSet MenuOptions[3];
void* lpVideoBuf;
HDC		hdcCMain;
HBITMAP	bmpMain;
HBITMAP hbmpOld;
HFONT   hfntOld;


void ChangeMenuState(int32_t ms)
{
	g_PrevMenuState = g_MenuState;
	g_MenuState = ms;
}

int GetTextW(HDC hdc, const std::string& s)
{
	SIZE sz;
	GetTextExtentPoint(hdc, s.c_str(), s.length(), &sz);
	return sz.cx;
}

int MapVKKey(int k)
{
	if (k == VK_LBUTTON) return 124;
	if (k == VK_RBUTTON) return 125;
	if (k == VK_MBUTTON) return 126;
	return MapVirtualKey(k, 0);
}


void AddMenuItem(TMenuSet& ms, const std::string& txt)
{
	ms.Item[ms.Count] = txt;
	//ms.y0 = ms.y0 + (ms.Count * 25);
	ms.Count++;
}


void InitInterface()
{
	fnt_Big = CreateFont(
		23, 10, 0, 0,
		600, 0, 0, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);

	fnt_Small = CreateFont(
		14, 5, 0, 0,
		100, 0, 0, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);

	fnt_Midd = CreateFont(
		16, 7, 0, 0,
		550, 0, 0, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);

	if (!fnt_Small)
		std::cout << "Interface : Failed to create Small Font" << std::endl;
	if (!fnt_Midd)
		std::cout << "Interface : Failed to create Medium Font" << std::endl;
	if (!fnt_Big)
		std::cout << "Interface : Failed to create Large Font" << std::endl;

	hdcCMain = CreateCompatibleDC(hdcMain);

	if (hdcCMain == NULL)
	{
		throw std::runtime_error("Interface: Failed to create CompatibleDC!");
		return;
	}

	BITMAPINFOHEADER bmih;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = 800;
	bmih.biHeight = -600;
	bmih.biPlanes = 1;
	bmih.biBitCount = 16;
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = 0;
	bmih.biXPelsPerMeter = 400;
	bmih.biYPelsPerMeter = 400;
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;

	BITMAPINFO binfo;
	binfo.bmiHeader = bmih;
	bmpMain = CreateDIBSection(hdcMain, &binfo, DIB_RGB_COLORS, &lpVideoBuf, NULL, 0);

	if (!bmpMain)
	{
		throw std::runtime_error("Interface: Failed to create DIB Section!");
		return;
	}

	g_PrevMenuState = -1;
	g_MenuState = 0;
	g_TypingBuffer = "";
	InitGameMenu(0);
	LoadGameMenu(0);
}


void ShutdownInterface()
{
	if (bmpMain)
		DeleteObject((HBITMAP)bmpMain);
	if (hdcCMain)
		DeleteDC(hdcCMain);

	if (fnt_Small)
		DeleteObject(fnt_Small);
	if (fnt_Midd)
		DeleteObject(fnt_Midd);
	if (fnt_Big)
		DeleteObject(fnt_Big);

	std::cout << "Interface: Shutdown Ok!" << std::endl;
}


void DrawRectangle(int x, int y, int w, int h, Color16 c)
{
	Color16* back_buffer = static_cast<Color16*>(lpVideoBuf);
	for (int i = y; i < y + h; i++)
		for (int j = x; j < x + w; j++)
		{
			back_buffer[(j)+(i * 800)] = Color16(c, 1);
			//*((uint16_t*)lpVideoBuf + (j)+(i) * 800) = c | 0x8000;
		}
}


void DrawPicture(int x, int y, int w, int h, uint16_t* lpImage)
{
	for (int i = 0; i < h; i++)
		memcpy((WORD*)lpVideoBuf + x + (y + i) * 800, lpImage + (h - i - 1) * w, w * 2);
}


void DrawMenuItem(TMenuItem* mptr)
{
	BYTE id = 0;
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(hwndMain, &p);

	if (p.x >= 0 && p.y >= 0 && p.x <= 800 && p.y <= 600)
		id = mptr->Image_Map[(p.x / 2) + (p.y / 2) * 400];
	else
		id = 0;

	for (int y = 0; y < 600; y++)
	{
		memcpy((WORD*)lpVideoBuf + (y * 800), &mptr->Image[(600 - y - 1) * 800], 800 * 2);
	}

	if (id == 0) return;

	for (int y = 0; y < 600; y++)
		for (int x = 0; x < 800; x++)
		{
			uint8_t id2 = mptr->Image_Map[(x / 2) + (y / 2) * 400];
			if (id == id2)
			{
				*((WORD*)lpVideoBuf + (x + y * 800)) = mptr->Image_On[x + (600 - y - 1) * 800];
			}
		}
}


void DrawText(int x, int y, string Text, uint32_t Color)
{
	SetBkMode(hdcCMain, TRANSPARENT);
	SetTextColor(hdcCMain, Color);
	TextOut(hdcCMain, x, y, Text.c_str(), Text.size());
}


void InterfaceSetFont(HFONT font)
{
	if (font == NULL) {
		SelectObject(hdcCMain, hfntOld);
	}
	else {
		hfntOld = (HFONT)SelectObject(hdcCMain, font);
	}
}


void InterfaceClear(WORD Color)
{
	memset(lpVideoBuf, 0, (800 * 2) * 600);
	hbmpOld = (HBITMAP)SelectObject(hdcCMain, bmpMain);
	hfntOld = (HFONT)SelectObject(hdcCMain, fnt_Small);
}


void InterfaceBlt()
{
	BitBlt(hdcMain, 0, 0, 800, 600, hdcCMain, 0, 0, SRCCOPY);

	SelectObject(hdcCMain, hfntOld);
	SelectObject(hdcCMain, hbmpOld);
}


void InitGameMenu(UINT menu)
{
	MenuOptions[0].x0 = 70;
	MenuOptions[0].y0 = 100;
	MenuOptions[0].Count = 0;
	AddMenuItem(MenuOptions[0], "Agressivity");
	AddMenuItem(MenuOptions[0], "Density");
	AddMenuItem(MenuOptions[0], "Sensitivity");
	AddMenuItem(MenuOptions[0], "Measurement");

	//MenuOptions[1].x0 = 610;
	MenuOptions[1].x0 = 460;
	MenuOptions[1].y0 = 90;
	MenuOptions[1].Count = 0;
	AddMenuItem(MenuOptions[1], "Forward");
	AddMenuItem(MenuOptions[1], "Backward");
	AddMenuItem(MenuOptions[1], "Turn Up");
	AddMenuItem(MenuOptions[1], "Turn Down");
	AddMenuItem(MenuOptions[1], "Turn Left");
	AddMenuItem(MenuOptions[1], "Turn Right");
	AddMenuItem(MenuOptions[1], "Fire");
	AddMenuItem(MenuOptions[1], "Get weapon");
	AddMenuItem(MenuOptions[1], "Step Left");
	AddMenuItem(MenuOptions[1], "Step Right");
	AddMenuItem(MenuOptions[1], "Strafe");
	AddMenuItem(MenuOptions[1], "Jump");
	AddMenuItem(MenuOptions[1], "Run");
	AddMenuItem(MenuOptions[1], "Crouch");
	AddMenuItem(MenuOptions[1], "Call");
	AddMenuItem(MenuOptions[1], "Binoculars");
	AddMenuItem(MenuOptions[1], "Supply");

	AddMenuItem(MenuOptions[1], "Reverse mouse");
	//AddMenuItem(MenuOptions[1], "Mouse sensitivity");

	MenuOptions[2].x0 = 70;
	MenuOptions[2].y0 = 370;
	MenuOptions[2].Count = 0;
	AddMenuItem(MenuOptions[2], "Resolution");
	AddMenuItem(MenuOptions[2], "Fog");
	AddMenuItem(MenuOptions[2], "Textures");
	AddMenuItem(MenuOptions[2], "Shadows");
	AddMenuItem(MenuOptions[2], "ColorKey");

	// -- Set default controls
	g_Options.KeyMap.fkForward = 'W';
	g_Options.KeyMap.fkBackward = 'S';
	g_Options.KeyMap.fkSLeft = 'A';
	g_Options.KeyMap.fkSRight = 'D';
	g_Options.KeyMap.fkFire = VK_LBUTTON;
	g_Options.KeyMap.fkShow = VK_RBUTTON;
	g_Options.KeyMap.fkJump = VK_SPACE;
	g_Options.KeyMap.fkCall = VK_MENU;
	g_Options.KeyMap.fkBinoc = 'B';
	g_Options.KeyMap.fkCrouch = 'C';
	g_Options.KeyMap.fkRun = VK_LSHIFT;
	g_Options.KeyMap.fkUp = VK_UP;
	g_Options.KeyMap.fkDown = VK_DOWN;
	g_Options.KeyMap.fkLeft = VK_LEFT;
	g_Options.KeyMap.fkRight = VK_RIGHT;

	/*
	g_Options.KeyMap.fkMenu		= VK_ESCAPE;
	g_Options.KeyMap.fkMap		= VK_TAB;
	g_Options.KeyMap.fkNightVision= 'N';
	g_Options.KeyMap.fkReload		= 'R';
	g_Options.KeyMap.fkZoomIn		= VK_ADD;
	g_Options.KeyMap.fkZoomOut	= VK_SUBTRACT;
	g_Options.KeyMap.fkConsole	= 126; // Tilde '~'
	*/
}


void LoadGameMenu(unsigned int menu)
{
	std::string	mf_off = "",
		mf_on = "",
		mf_map = "";

	if (menu == MENU_REGISTER)
	{
		mf_off = "HUNTDAT\\MENU\\MENUR.TGA";
		mf_on = "HUNTDAT\\MENU\\MENUR_ON.TGA";
		mf_map = "HUNTDAT\\MENU\\MR_MAP.RAW";
	}
	else if (menu == MENU_MAIN)
	{
		mf_off = "HUNTDAT\\MENU\\MENUM.TGA";
		mf_on = "HUNTDAT\\MENU\\MENUM_ON.TGA";
		mf_map = "HUNTDAT\\MENU\\MAIN_MAP.RAW";
	}
	else if (menu == MENU_HUNT)
	{
		mf_off = "HUNTDAT\\MENU\\MENU2.TGA";
		mf_on = "HUNTDAT\\MENU\\MENU2_ON.TGA";
		mf_map = "HUNTDAT\\MENU\\M2_MAP.RAW";
	}
	else if (menu == MENU_OPTIONS)
	{
		mf_off = "HUNTDAT\\MENU\\OPT_OFF.TGA";
		mf_on = "HUNTDAT\\MENU\\OPT_ON.TGA";
		mf_map = "HUNTDAT\\MENU\\OPT_MAP.RAW";
	}
	else if (menu == MENU_CREDITS)
	{
		mf_off = "HUNTDAT\\MENU\\CREDITS.TGA";
		mf_on = "";
		mf_map = "";
	}
	else if (menu == MENU_STATISTICS)
	{
		mf_off = "HUNTDAT\\MENU\\MENUS.TGA";
		mf_on = "";
		mf_map = "";
	}
	else if (menu == MENU_QUIT)
	{
		mf_off = "HUNTDAT\\MENU\\MENUQ.TGA";
		mf_on = "HUNTDAT\\MENU\\MENUQ_ON.TGA";
		mf_map = "HUNTDAT\\MENU\\MQ_MAP.RAW";
	}

	if (mf_off.empty())
	{
		// ERROR!!!!
		std::cout << "ERROR: Invalid menu state was supplied to LoadGameMenu(#)!" << std::endl;
	}

	FILE* file = fopen(mf_off.c_str(), "rb");
	fseek(file, 18, SEEK_SET);
	fread(g_MenuItem.Image, 800 * 600 * 2, 1, file);
	fclose(file);

	if (!mf_on.empty())
	{
		file = fopen(mf_on.c_str(), "rb");
		fseek(file, 18, SEEK_SET);
		fread(g_MenuItem.Image_On, 800 * 600 * 2, 1, file);
		fclose(file);
	}
	else
	{
		memcpy(g_MenuItem.Image_On, g_MenuItem.Image, 800 * 600 * 2);
	}

	if (!mf_map.empty())
	{
		file = fopen(mf_map.c_str(), "rb");
		fread(g_MenuItem.Image_Map, 400 * 300, 1, file);
		fclose(file);
	}
	else
	{
		memset(g_MenuItem.Image_Map, 0, 400 * 300);
	}
}


void DrawTextShadow(int x, int y, const char* text, uint32_t color)
{
	DrawText(x + 1, y + 1, text, RGB(0, 0, 0));
	DrawText(x + 1, y, text, RGB(0, 0, 0));
	DrawText(x, y + 1, text, RGB(0, 0, 0));
	DrawText(x, y, text, color);
}


int LaunchProcess(std::string exe_name)
{
	PROCESS_INFORMATION processInformation = { 0 };
	STARTUPINFO startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);
	uint32_t exitCode = 0;
	BOOL result = FALSE;

	ShowWindow(hwndMain, SW_HIDE);

	// Create the process
	result = CreateProcess(exe_name.c_str(), nullptr,
		NULL, NULL, FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL, NULL, &startupInfo, &processInformation);

	// Successfully created the process.  Wait for it to finish.
	WaitForSingleObject(processInformation.hProcess, INFINITE);

	ShowWindow(hwndMain, SW_SHOW);

	// Get the exit code.
	result = GetExitCodeProcess(processInformation.hProcess, (DWORD*)&exitCode);

	// Close the handles.
	CloseHandle(processInformation.hProcess);
	CloseHandle(processInformation.hThread);

	// Reset to the main menu like the original game does
	ChangeMenuState(MENU_MAIN);
	LoadGameMenu(g_MenuState);

	return exitCode;
}


void ProcessMenu()
{
	uint8_t id = 0;
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(hwndMain, &p);

	if (p.x >= 0 && p.y >= 0 && p.x < 800 && p.y < 600)
		id = g_MenuItem.Image_Map[(p.x / 2) + (p.y / 2) * 400];
	else
		id = 0;

	InterfaceClear(HIRGB(0, 0, 0));

	//DrawRectangle(0,0, 64,64, 31);
	DrawMenuItem(&g_MenuItem);

#ifdef _DEBUG
	InterfaceSetFont(fnt_Big);
	char cursor_pos[64];
	sprintf(cursor_pos, "X: %ld, Y: %ld", p.x, p.y);
	DrawTextShadow(0, 0, cursor_pos, RGB(255, 0, 0));
	InterfaceSetFont(NULL);
#endif

	switch (g_MenuState)
	{
	case MENU_REGISTER:
	{
		if (g_PrevMenuState != g_MenuState) {
			char tname[128];
			for (int i = 0; i < 6; i++) {
				g_Profiles[i].m_Name = "";
				g_Profiles[i].m_RegNumber = i;
				g_Profiles[i].m_Rank = 0;
				g_Profiles[i].m_Score = 0;

				std::stringstream sn;
				sn << "trophy" << std::setfill('0') << std::setw(2) << i << ".sav";

				std::ifstream fs(sn.str());
				if (!fs.is_open()) { continue; }

				fs.read(tname, 128);
				fs.read((char*)&g_Profiles[i].m_RegNumber, 4);
				fs.read((char*)&g_Profiles[i].m_Score, 4);
				fs.read((char*)&g_Profiles[i].m_Rank, 4);

				g_Profiles[i].m_Name = tname;
			}
		}

		std::stringstream ss;

		uint32_t color = RGB(239, 228, 176);
		InterfaceSetFont(fnt_Small);

		if ((timeGetTime() % 800) > 300)
			ss << g_TypingBuffer << "_";
		else
			ss << g_TypingBuffer;

		DrawTextShadow(330, 326, ss.str().c_str(), color);

		// 320, 370
		for (int i = 0; i < 6; i++)
		{
			if (p.x >= 308 && p.y >= (368 + (16 * i)) && p.x <= 408 && p.y <= (368 + (16 * i) + 16))
				color = RGB(255, 42, 42);
			else
				color = 0xB0B070;

			std::string tname = "";

			if (!g_Profiles[i].m_Name.empty()) {
				//ss << "trophy" << std::setfill('0') << std::setw(2) << i << ".sav";
				tname = g_Profiles[i].m_Name;
			}
			else {
				color = RGB(255, 170, 10);
				tname = "...";
			}

			DrawTextShadow(320, 370 + (20 * i), tname.c_str(), color);
		}

		InterfaceSetFont(NULL);
	}
	break;
	case MENU_MAIN:
	{
		char t[32];
		int c = RGB(239, 228, 176);

		InterfaceSetFont(fnt_Big);
		DrawTextShadow(90, 9, g_UserProfile.Name, c);

		wsprintf(t, "%d", g_UserProfile.Score);
		DrawTextShadow(540, 9, t, c);

		/*switch (UserProfile.Rank)
		{
		case 0: DrawTextShadow(344, 9, "Novice", c); break;
		case 1: DrawTextShadow(344, 9, "Advanced", c); break;
		case 2: DrawTextShadow(344, 9, "Expert", c); break;
		}*/

		InterfaceSetFont(NULL);
	}
	break;
	case MENU_STATISTICS:
	{
		char t[32];
		int c = RGB(239, 228, 176);

		InterfaceSetFont(fnt_Big);
		DrawTextShadow(90, 9, g_UserProfile.Name, c);

		wsprintf(t, "%d", g_UserProfile.Score);
		DrawTextShadow(540, 9, t, c);
		InterfaceSetFont(NULL);

		/*switch (UserProfile.Rank)
		{
		case 0: DrawTextShadow(344, 9, "Novice", c); break;
		case 1: DrawTextShadow(344, 9, "Advanced", c); break;
		case 2: DrawTextShadow(344, 9, "Expert", c); break;
		}*/

		InterfaceSetFont(fnt_Small);
		//int  ttm = (int)g_UserProfile.Total.time;
		int  ltm = (int)g_UserProfile.Last.time;

		DrawTextShadow(718 - GetTextW(hdcCMain, "Path travelled  "), 78, "Path travelled  ", c);

		if (g_Options.OptSys)  sprintf(t, "%1.0f ft.", g_UserProfile.Last.path / 0.3f);
		else         sprintf(t, "%1.0f m.", g_UserProfile.Last.path);

		DrawTextShadow(718, 78, t, c);

		DrawTextShadow(718 - GetTextW(hdcCMain, "Time hunted  "), 98, "Time hunted  ", c);
		sprintf(t, "%d:%02d:%02d", (ltm / 3600), ((ltm % 3600) / 60), (ltm % 60));
		DrawTextShadow(718, 98, t, c);

		DrawTextShadow(718 - GetTextW(hdcCMain, "Shots made  "), 118, "Shots made  ", c);
		sprintf(t, "%d", g_UserProfile.Last.smade);
		DrawTextShadow(718, 118, t, c);

		int accuracy = 0;
		if (g_UserProfile.Last.success > 0)
			accuracy = ((g_UserProfile.Last.smade / g_UserProfile.Last.success) * 100);

		DrawTextShadow(718 - GetTextW(hdcCMain, "Accuracy  "), 138, "Accuracy  ", c);
		sprintf(t, "%d%%", accuracy);
		DrawTextShadow(718, 138, t, c);
		InterfaceSetFont(NULL);
	}
	break;
	case MENU_OPTIONS:
	{
		int c = RGB(239, 228, 176);

		//===== CONTROLS =====//
		//char keymap_input_str[80];

		for (int m = 0; m < 3; m++)
			for (int i = 0; i < MenuOptions[m].Count; i++)
			{
				int x0 = MenuOptions[m].x0;
				int y0 = MenuOptions[m].y0 + (18 * i);
				InterfaceSetFont(fnt_Big);
				//DrawTextShadow(x0 - GetTextW(hdcCMain, MenuOptions[m].Item[i]), y0, MenuOptions[m].Item[i].c_str(), c);
				DrawTextShadow(x0, y0, MenuOptions[m].Item[i].c_str(), c);
				//sprintf(keymap_input_str, "Forward: %s", KeysName[MapVKKey( *((int32_t*)&Options.KeyMap + i) )]);
				//DrawTextShadow(442, 86 + (i*25), keymap_input_str, c);
				InterfaceSetFont(NULL);
			}
	}
	break;
	case MENU_CREDITS:
	{
		// -- do rendering
	}
	break;
	case MENU_HUNT:
	{
		std::stringstream ss;

		ss << g_UserProfile.Score;

		InterfaceSetFont(fnt_Big);
		DrawTextShadow(335, 38, ss.str().c_str(), RGB(239, 228, 176));
		DrawTextShadow(406, 38, "1000", RGB(239, 228, 176));
		InterfaceSetFont(NULL);

		InterfaceSetFont(fnt_Small);
		DrawTextShadow(424, 96, "<Summary>", RGB(239, 228, 176));
		InterfaceSetFont(NULL);

		for (size_t i = 0; i < g_AreaInfo.size(); i++)
		{
			DrawTextShadow(14, 382 + (16 * i), g_AreaInfo[i].Name, 0xB0B070);
		}

		for (size_t i = 0; i < g_DinoInfo.size(); i++)
		{
			DrawTextShadow(214, 382 + (16 * i), g_DinoInfo[i].Name, 0xB0B070);
		}

		for (size_t i = 0; i < g_WeapInfo.size(); i++)
		{
			DrawTextShadow(414, 382 + (16 * i), g_WeapInfo[i].Name, 0xB0B070);
		}
	}
	break;
	}

	InterfaceBlt();

	g_PrevMenuState = g_MenuState;
}


void MenuKeyDownEvent(uint16_t keycode)
{
	if (keycode == VK_ESCAPE)
	{
		ChangeMenuState(MENU_QUIT);
		LoadGameMenu(g_MenuState);
		return;
	}

	if (g_MenuState == MENU_CREDITS)
	{
		ChangeMenuState(MENU_MAIN);
		LoadGameMenu(g_MenuState);
		return;
	}
	else if (g_MenuState == MENU_REGISTER)
	{
		if (keycode == VK_RETURN)
		{
			// -- Create new save profile
			//SaveTrophy();
		}
		else if (keycode == VK_BACK)
		{
			if (g_TypingBuffer.size() > 0)
				g_TypingBuffer.pop_back();
		}
	}
}


void MenuMouseLEvent()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(hwndMain, &p);

	if (p.x < 0) return;
	if (p.y < 0) return;
	if (p.x > 799) return;
	if (p.y > 599) return;

	uint8_t id = g_MenuItem.Image_Map[(p.x / 2) + (p.y / 2) * 400];

	switch (g_MenuState)
	{
	case MENU_REGISTER: {
		if (id == 1) {
			g_PrevMenuState = g_MenuState;
			ChangeMenuState(MENU_MAIN);
			LoadTrophy(g_UserProfile, 0);
		}
		if (id == 2) {/* Delete the selected 'save' */ }
	}
					  break;
	case MENU_MAIN: {
		if (id == 1) ChangeMenuState(MENU_HUNT);
		if (id == 2) ChangeMenuState(MENU_OPTIONS);
		if (id == 3) { /*  */ }
		if (id == 4) ChangeMenuState(MENU_CREDITS);
		if (id == 5) ChangeMenuState(MENU_QUIT);
		if (id == 6) ChangeMenuState(MENU_STATISTICS);
	}
				  break;
	case MENU_STATISTICS: {
		ChangeMenuState(MENU_MAIN);
	}
						break;
	case MENU_QUIT: {
		std::cout << "MenuLClick: " << id << std::endl;
		if (id == 1) PostQuitMessage(0);
		else ChangeMenuState(MENU_MAIN);
	}
	case MENU_OPTIONS: {
		if (id == 4) ChangeMenuState(MENU_MAIN);
	}
					 break;
	case MENU_CREDITS: {
		ChangeMenuState(MENU_MAIN);
	}
					 break;
	case MENU_HUNT: {
		if (id == 7) ChangeMenuState(MENU_MAIN);
		if (id == 8) { /* Launch the game */ }
	}
				  break;
	default:
		if (id != 0) {
			std::stringstream ss;
			ss << "id: " << id;
			MessageBox(hwndMain, ss.str().c_str(), "Click ID", MB_OK);
		}
		break;
	}

	LoadGameMenu(g_MenuState);
}
