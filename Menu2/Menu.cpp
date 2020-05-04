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
#include <thread>


class MenuSet {
public:
	int			x0, y0;
	int			Count;
	std::string	Item[32];
	int32_t		Selected;
	int32_t		Hilite;
	RECT		Rect;

	MenuSet() :
		x0(0), y0(0),
		Count(0),
		Selected(-1),
		Hilite(-1),
		Rect({ 0,0,0,0 })
	{}
};


enum MenuSetEnum {
	OPT_GAME = 0,
	OPT_KEYBINDINGS = 1,
	OPT_VIDEO = 2,
	OPT_MAX
};


enum DrawTextAlignEnum {
	// Uses the same values as wingdi.h TA_LEFT and so on.
	// TODO: Write a conversion function for Linux if we port it to Linux
	DTA_LEFT = 0,
	DTA_RIGHT = 2,
	DTA_CENTER = 6,
	DTA_BOTTOM = 8
};


MenuSet MenuOptions[3];

void* lpVideoBuf;
HDC hdcCMain;
HBITMAP bmpMain;
HBITMAP hbmpOld;
HFONT hfntOld;
POINT g_CursorPos;


// String table
const char st_Boolean[2][4] = { "Off", "On" };
const char st_HMLText[4][8] = { "Low", "Medium", "High", "Ultra" };


void WaitForMouseRelease()
{
	while (GetAsyncKeyState(VK_RBUTTON) & 0x80000000);
	while (GetAsyncKeyState(VK_MBUTTON) & 0x80000000);
	while (GetAsyncKeyState(VK_LBUTTON) & 0x80000000);
}


void ChangeMenuState(int32_t ms)
{
	g_PrevMenuState = g_MenuState;
	g_MenuState = ms;
	LoadGameMenu(g_MenuState);
}

int GetTextW(HDC hdc, const std::string& s)
{
	SIZE sz;
	GetTextExtentPoint(hdc, s.c_str(), (int)s.length(), &sz);
	return sz.cx;
}

int GetTextH(HDC hdc, const std::string& s)
{
	SIZE sz;
	GetTextExtentPoint(hdc, s.c_str(), (int)s.length(), &sz);
	return sz.cy;
}

int MapVKKey(int k)
{
	if (k == VK_LBUTTON) return 124;
	if (k == VK_RBUTTON) return 125;
	if (k == VK_MBUTTON) return 126;
	return MapVirtualKey(k, 0);
}


void AddMenuItem(MenuSet& ms, const std::string& txt)
{
	ms.Item[ms.Count] = txt;
	//ms.y0 = ms.y0 + (ms.Count * 25);
	ms.Count++;
}


/*
Initialise all the required settings and interface elements
*/
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

	if (!bmpMain) {
		throw std::runtime_error("Interface: Failed to create DIB Section!");
		return;
	}

	g_PrevMenuState = -1;
	g_MenuState = 0;
	g_TypingBuffer = "";
	LoadGameMenu(0);

	g_ProfileIndex = 0;

	int m = OPT_GAME;
	MenuOptions[m].x0 = 70;
	MenuOptions[m].y0 = 85;
	MenuOptions[m].Count = 0;
	AddMenuItem(MenuOptions[m], "Agressivity");
	AddMenuItem(MenuOptions[m], "Density");
	AddMenuItem(MenuOptions[m], "Sensitivity");
	AddMenuItem(MenuOptions[m], "Measurement");
	AddMenuItem(MenuOptions[m], "Render");
	AddMenuItem(MenuOptions[m], "Audio");
	MenuOptions[m].Rect = { 190 - 140, 85, 190 + 140, 85 + (MenuOptions[0].Count * 25) };

	m = OPT_KEYBINDINGS;
	MenuOptions[m].x0 = 610;
	MenuOptions[m].y0 = 85;
	MenuOptions[m].Count = 0;
	AddMenuItem(MenuOptions[m], "Forward");
	AddMenuItem(MenuOptions[m], "Backward");
	AddMenuItem(MenuOptions[m], "Turn Up");
	AddMenuItem(MenuOptions[m], "Turn Down");
	AddMenuItem(MenuOptions[m], "Turn Left");
	AddMenuItem(MenuOptions[m], "Turn Right");
	AddMenuItem(MenuOptions[m], "Fire");
	AddMenuItem(MenuOptions[m], "Draw Weapon");
	AddMenuItem(MenuOptions[m], "Step Left");
	AddMenuItem(MenuOptions[m], "Step Right");
	AddMenuItem(MenuOptions[m], "Strafe");
	AddMenuItem(MenuOptions[m], "Jump");
	AddMenuItem(MenuOptions[m], "Run");
	AddMenuItem(MenuOptions[m], "Crouch");
	AddMenuItem(MenuOptions[m], "Lure Call");
	AddMenuItem(MenuOptions[m], "Binoculars");
	AddMenuItem(MenuOptions[m], "Call Resupply");
	AddMenuItem(MenuOptions[m], "Invert Mouse");
	//AddMenuItem(MenuOptions[m], "Mouse sensitivity");
	MenuOptions[m].Rect = { 610 - 140, 85, 610 + 140, 85 + (MenuOptions[1].Count * 25) };

	m = OPT_VIDEO;
	MenuOptions[m].x0 = 70;
	MenuOptions[m].y0 = 360;
	MenuOptions[m].Count = 0;
	AddMenuItem(MenuOptions[m], "Resolution");
	AddMenuItem(MenuOptions[m], "Fog");
	AddMenuItem(MenuOptions[m], "Textures");
	AddMenuItem(MenuOptions[m], "Shadows");
	AddMenuItem(MenuOptions[m], "ColorKey");
	MenuOptions[m].Rect = { 190 - 140, 360, 190 + 140, 360 + (MenuOptions[2].Count * 25) };

	UINT prev_align = SetTextAlign(hdcCMain, TA_BASELINE);
	SetTextAlign(hdcCMain, prev_align);
	std::cout << "TextAlign default=" << prev_align << std::endl;
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


void DrawPicture(int x, int y, Picture& pic)
{
	if (pic.m_Data == nullptr || pic.m_Width == 0 || pic.m_Height == 0)
		return;

	/*for (int y = 0; y < 600; y++) {
		memcpy((uint16_t*)lpVideoBuf + (y * 800), &menu.m_Image[(600 - y - 1) * 800], 800 * 2);
	}*/

	for (int i = 0; i < pic.m_Height; i++) {
		memcpy((uint16_t*)lpVideoBuf + x + (y + i) * 800, pic.m_Data + (pic.m_Height - i - 1) * pic.m_Width, pic.m_Width * 2);
	}
}


void DrawPicture(int x, int y, int w, int h, uint16_t* lpImage)
{
	for (int i = 0; i < h; i++)
		memcpy((uint16_t*)lpVideoBuf + x + (y + i) * 800, lpImage + (h - i - 1) * w, w * 2);
}


/*
Draw the menu background and overlay the 'on' layer
*/
void DrawMenuBg(MenuItem& menu)
{
	POINT& p = g_CursorPos;
	uint8_t cursor_id = 0;

	cursor_id = menu.GetID((p.x / 2), (p.y / 2));

	// OLD: Render background image
	/*for (int y = 0; y < 600; y++) {
		memcpy((uint16_t*)lpVideoBuf + (y * 800), &menu.m_Image[(600 - y - 1) * 800], 800 * 2);
	}*/

	// Render the base background from the on/off states
	for (int yy = 0; yy < 300; yy++) {
		for (int xx = 0; xx < 400; xx++)
		{
			int x = xx * 2;
			int y = yy * 2;
			int on = false;
			uint8_t id2 = menu.GetID((xx), (yy));

			if (cursor_id == id2) on |= true;

			if (g_MenuState == MENU_HUNT && (id2 >= 1 && id2 <= 6)) {
				on |= (int)g_MenuItem.GetIsElementSet(id2);
			}
			else if (g_MenuState == MENU_OPTIONS && (id2 >= 1 && id2 <= 3)) {
				on |= (int)g_MenuItem.GetIsElementSet(id2);
			}

			if (id2 == 0) on = false;

			if (on) {
				*((uint16_t*)lpVideoBuf + ((x + 0) + (y + 0) * 800)) = menu.m_Image_On[(x + 0) + (600 - (y + 0) - 1) * 800];
				*((uint16_t*)lpVideoBuf + ((x + 1) + (y + 0) * 800)) = menu.m_Image_On[(x + 1) + (600 - (y + 0) - 1) * 800];
				*((uint16_t*)lpVideoBuf + ((x + 0) + (y + 1) * 800)) = menu.m_Image_On[(x + 0) + (600 - (y + 1) - 1) * 800];
				*((uint16_t*)lpVideoBuf + ((x + 1) + (y + 1) * 800)) = menu.m_Image_On[(x + 1) + (600 - (y + 1) - 1) * 800];
			}
			else {
				*((uint16_t*)lpVideoBuf + ((x + 0) + (y + 0) * 800)) = menu.m_Image[(x + 0) + (600 - (y + 0) - 1) * 800];
				*((uint16_t*)lpVideoBuf + ((x + 1) + (y + 0) * 800)) = menu.m_Image[(x + 1) + (600 - (y + 0) - 1) * 800];
				*((uint16_t*)lpVideoBuf + ((x + 0) + (y + 1) * 800)) = menu.m_Image[(x + 0) + (600 - (y + 1) - 1) * 800];
				*((uint16_t*)lpVideoBuf + ((x + 1) + (y + 1) * 800)) = menu.m_Image[(x + 1) + (600 - (y + 1) - 1) * 800];
			}
		}
	}
}


void DrawTextColor(int x, int y, const std::string& text, uint32_t color, int align = DTA_LEFT)
{
	if (align != DTA_LEFT)
		SetTextAlign(hdcCMain, TA_BASELINE);

	SetBkMode(hdcCMain, TRANSPARENT);
	SetTextColor(hdcCMain, color);
	TextOut(hdcCMain, x, y, text.c_str(), (int)text.size());

	if (align != DTA_LEFT)
		SetTextAlign(hdcCMain, 0);
}


void DrawTextShadow(int x, int y, const std::string& text, uint32_t color, int align = DTA_LEFT)
{
	if (align != DTA_LEFT)
		SetTextAlign(hdcCMain, TA_BASELINE);

	DrawTextColor(x + 1, y + 1, text, RGB(0, 0, 0));
	DrawTextColor(x + 1, y, text, RGB(0, 0, 0));
	DrawTextColor(x, y + 1, text, RGB(0, 0, 0));
	DrawTextColor(x, y, text, color);

	if (align != DTA_LEFT)
		SetTextAlign(hdcCMain, 0);
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


void MenuEventStart(int32_t menu_state)
{
	g_MenuItem.ResetElementSet();

	switch (menu_state) {
	case MENU_REGISTER: {
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
	} break;
	case MENU_OPTIONS: {
		for (int m = 0; m < OPT_MAX; m++) {
			MenuOptions[m].Hilite = -1;
			MenuOptions[m].Selected = -1;
		}
	} break;
	}
}


void LoadGameMenu(int32_t menu)
{
	std::string	mf_off = "";
	std::string	mf_on = "";
	std::string	mf_map = "";

	switch (menu) {
	case MENU_REGISTER:
	{
		mf_off = "HUNTDAT/MENU/MENUR.TGA";
		mf_on = "HUNTDAT/MENU/MENUR_ON.TGA";
		mf_map = "HUNTDAT/MENU/MR_MAP.RAW";
	} break;
	case MENU_MAIN:
	{
		mf_off = "HUNTDAT/MENU/MENUM.TGA";
		mf_on = "HUNTDAT/MENU/MENUM_ON.TGA";
		mf_map = "HUNTDAT/MENU/MAIN_MAP.RAW";
	} break;
	case MENU_HUNT:
	{
		mf_off = "HUNTDAT/MENU/MENU2.TGA";
		mf_on = "HUNTDAT/MENU/MENU2_ON.TGA";
		mf_map = "HUNTDAT/MENU/M2_MAP.RAW";
	} break;
	case MENU_OPTIONS:
	{
		mf_off = "HUNTDAT/MENU/OPT_OFF.TGA";
		mf_on = "HUNTDAT//MENU/OPT_ON.TGA";
		mf_map = "HUNTDAT/MENU/OPT_MAP.RAW";
	} break;
	case MENU_CREDITS:
	{
		mf_off = "HUNTDAT/MENU/CREDITS.TGA";
		mf_on = "";
		mf_map = "";
	} break;
	case MENU_STATISTICS:
	{
		mf_off = "HUNTDAT/MENU/MENUS.TGA";
		mf_on = "";
		mf_map = "";
	} break;
	case MENU_QUIT:
	{
		mf_off = "HUNTDAT/MENU/MENUQ.TGA";
		mf_on = "HUNTDAT/MENU/MENUQ_ON.TGA";
		mf_map = "HUNTDAT/MENU/MQ_MAP.RAW";
	} break;
	default:
	{
		throw std::invalid_argument("Invalid argument: unsigned int menu.\nExpected a member of MenuStateEnum!");
	} break;
	}

	TargaImage tga;

	if (ReadTGAFile(mf_off, tga)) {
		memcpy(g_MenuItem.m_Image, tga.m_Data, (800 * 2) * 600);
	}
	else {
		throw std::runtime_error("Failed to open the file...");
		return;
	}

	if (!mf_on.empty()) {
		if (ReadTGAFile(mf_on, tga)) {
			memcpy(g_MenuItem.m_Image_On, tga.m_Data, (800 * 2) * 600);
		}
		else {
			throw std::runtime_error("Failed to open the file...");
			return;
		}
	}

	if (!mf_map.empty()) {
		std::ifstream fs(mf_map, std::ios::binary);
		if (fs.is_open())
			fs.read((char*)g_MenuItem.m_Image_Map, 400 * 300);
	}
	else {
		memset(g_MenuItem.m_Image_Map, 0, 400 * 300);
	}
}


int LaunchProcess(const std::string& exe_name, std::string cmd_line)
{
	PROCESS_INFORMATION processInformation = { 0 };
	STARTUPINFO startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);
	uint32_t exitCode = 0;

	ShowWindow(hwndMain, SW_HIDE);

	// Create the process
	BOOL result = CreateProcess(exe_name.c_str(), const_cast<char*>(cmd_line.c_str()),
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


/*
Draw the profile details that appear at the top of the main menu,
such as Profile Name, Score, Rank.
*/
void DrawProfileMenu()
{
	std::stringstream ss;

	int c = RGB(239, 228, 176);

	InterfaceSetFont(fnt_Big);
	DrawTextShadow(90, 9, g_UserProfile.Name, c);

	ss << g_UserProfile.Score;
	DrawTextShadow(540, 9, ss.str().c_str(), c);

	/*switch (UserProfile.Rank)
	{
	case 0: DrawTextShadow(344, 9, "Novice", c); break;
	case 1: DrawTextShadow(344, 9, "Advanced", c); break;
	case 2: DrawTextShadow(344, 9, "Expert", c); break;
	}*/

	InterfaceSetFont(NULL);
}


/*
Draw this profile's last hunt statistics and total hunt statistics, also calls DrawProfileMenu()
to draw the profile name and score/rank as an overlay.
*/
void DrawStatisticsMenu()
{
	std::stringstream ss;
	int c = RGB(239, 228, 176);

	InterfaceSetFont(fnt_Small);
	//int  ttm = (int)g_UserProfile.Total.time;
	int  ltm = (int)g_UserProfile.Last.time;

	DrawTextShadow(718 - GetTextW(hdcCMain, "Path travelled  "), 78, "Path travelled  ", c);

	if (g_Options.OptSys)
		ss << std::setprecision(4) << (g_UserProfile.Last.path / 0.3f) << " ft.";
	else
		ss << std::setprecision(4) << (g_UserProfile.Last.path) << " m.";

	DrawTextShadow(718, 78, ss.str(), c);
	ss.str(""); ss.clear();

	DrawTextShadow(718 - GetTextW(hdcCMain, "Time hunted  "), 98, "Time hunted  ", c);
	ss << std::dec << (ltm / 3600) << ":"; // Hours
	ss << std::setfill('0') << std::setw(2) << ((ltm % 3600) / 60) << ":"; // Minutes
	ss << std::setfill('0') << std::setw(2) << (ltm % 60); // Seconds
	DrawTextShadow(718, 98, ss.str(), c);
	ss.str(""); ss.clear();

	DrawTextShadow(718 - GetTextW(hdcCMain, "Shots made  "), 118, "Shots made  ", c);
	ss << g_UserProfile.Last.smade;
	DrawTextShadow(718, 118, ss.str(), c);
	ss.str(""); ss.clear();

	int accuracy = 0;
	if (g_UserProfile.Last.success > 0)
		accuracy = ((g_UserProfile.Last.smade / g_UserProfile.Last.success) * 100);

	DrawTextShadow(718 - GetTextW(hdcCMain, "Accuracy  "), 138, "Accuracy  ", c);
	ss << accuracy << "%";
	DrawTextShadow(718, 138, ss.str(), c);
	ss.str(""); ss.clear();

	// TODO: implement total stats

	InterfaceSetFont(NULL);

	DrawProfileMenu();
}


void DrawHuntMenu()
{
	std::stringstream ss;

	ss << g_UserProfile.Score;

	InterfaceSetFont(fnt_Big);
	DrawTextShadow(335, 38, ss.str(), RGB(239, 228, 176));
	ss.str(""); ss.clear();

	ss << g_UserProfile.Score; // Subtract the costs of selections
	DrawTextShadow(406, 38, ss.str(), RGB(239, 228, 176));
	ss.str(""); ss.clear();

	InterfaceSetFont(fnt_Midd);
	DrawTextShadow(424, 96, "<Summary>", RGB(239, 228, 176));


	InterfaceSetFont(fnt_Small);

	for (unsigned i = 0; i < g_AreaInfo.size(); i++) {
		DrawTextShadow(14, 382 + (16 * i), g_AreaInfo[i].m_Name, 0xB0B070);
	}

	for (unsigned i = 0; i < g_DinoInfo.size(); i++) {
		DrawTextShadow(214, 382 + (16 * i), g_DinoInfo[i].m_Name, 0xB0B070);
	}

	for (unsigned i = 0; i < g_WeapInfo.size(); i++) {
		DrawTextShadow(414, 382 + (16 * i), g_WeapInfo[i].m_Name, 0xB0B070);
	}
}


/*
Draw the save file 'registry' menu
*/
void DrawRegistryMenu(POINT& p)
{
	std::stringstream ss;

	uint32_t color = RGB(239, 228, 176);
	InterfaceSetFont(fnt_Small);

	if ((timeGetTime() % 800) > 300)
		ss << g_TypingBuffer << "_";
	else
		ss << g_TypingBuffer;

	DrawTextShadow(315, 326, ss.str(), color);

	g_HiliteProfileIndex = g_ProfileIndex;

	// 320, 370
	for (int i = 0; i < 6; i++)
	{
		color = RGB(255, 170, 10);

		if (i == g_HiliteProfileIndex)
			color = 0xB0B070;

		if (p.x >= 308 && p.y >= (368 + (16 * i)) && p.x <= 408 && p.y <= (368 + (16 * i) + 16)) {
			g_HiliteProfileIndex = i; //temporary, move to another function for user input
			color = RGB(255, 42, 42);
		}

		std::string tname = g_Profiles[i].m_Name;

		if (g_Profiles[i].m_Name.empty()) {
			tname = "...";
		}

		DrawTextShadow(320, 370 + (20 * i), tname, color);
	}

	InterfaceSetFont(NULL);
}


/*
Keyboard and Mouse handling for menus
NOTE: Could move these to individual functions if a state machine is confusing
*/
void MenuEventInput(int32_t menu)
{
	POINT& p = g_CursorPos;
	uint8_t id = g_MenuItem.GetID(g_CursorPos.x / 2, g_CursorPos.y / 2);

#ifdef _DEBUG
	if (g_KeyboardState[VK_RETURN] & 128) {
		std::cout << "MenuLeftClick: " << ((int)id) << std::endl;
	}
#endif

	if (menu == MENU_CREDITS)
	{
		if (g_KeyboardState[VK_RETURN] & 128) {
			g_KeyboardState[VK_LBUTTON] |= 128;
		}

		if (g_KeyboardState[VK_SPACE] & 128) {
			g_KeyboardState[VK_LBUTTON] |= 128;
		}

		if (g_KeyboardState[VK_LBUTTON] & 128) {
			WaitForMouseRelease();
			ChangeMenuState(MENU_MAIN);
		}
	}
	else if (menu == MENU_REGISTER)
	{
		if (g_KeyboardState[VK_RETURN] & 128) {
			g_KeyboardState[VK_LBUTTON] |= 128;
			id = 1;
		}

		if (g_KeyboardState[VK_DELETE] & 128) {
			g_KeyboardState[VK_LBUTTON] |= 128;
			id = 2;
		}

		if (g_KeyboardState[VK_LBUTTON] & 128) {
			if (id == 1) {
				if (g_Profiles[g_ProfileIndex].m_Name.empty()) {
					g_UserProfile.New(g_TypingBuffer);
					g_Options.Default();
					SaveTrophy(g_UserProfile);
				}
				else {
					LoadTrophy(g_UserProfile, g_ProfileIndex);
				}

				WaitForMouseRelease();
				ChangeMenuState(MENU_MAIN);
			}
			else if (id == 2) {
				// Delete the selected 'save'
				//DeleteFile();
				WaitForMouseRelease();
			}
			else {
				WaitForMouseRelease();
				g_ProfileIndex = g_HiliteProfileIndex;
			}
		}
	}
	else if (menu == MENU_OPTIONS) {
		if (g_KeyboardState[VK_ESCAPE] & 128) {
			ChangeMenuState(MENU_QUIT);
		}
		else if (id == 4) {
			if (g_KeyboardState[VK_LBUTTON] & 128) {
				WaitForMouseRelease();
				ChangeMenuState(MENU_MAIN);
			}
		}
		else
		{
			for (int m = 0; m < OPT_MAX; m++)
			{
				MenuSet& mo = MenuOptions[m];
				if (p.x > mo.Rect.left && p.y > mo.Rect.top && p.x < mo.Rect.right && p.y < mo.Rect.bottom)
				{
					int yd = p.y - mo.y0;
					g_MenuItem.SetIsElementSet(m + 1, true);

					if (yd > 0)
					{
						mo.Hilite = yd / 25;
					}
					else mo.Hilite = -1;

					if (g_KeyboardState[VK_LBUTTON] & 128)
					{
						WaitForMouseRelease();

						if (m == OPT_GAME) {
							//click
							mo.Selected = mo.Hilite;
						}
						if (m == OPT_KEYBINDINGS) {
							//click
							mo.Selected = mo.Hilite;
						}
						if (m == OPT_VIDEO) {
							//click
							mo.Selected = mo.Hilite;
						}
					}
				}
				else
				{
					g_MenuItem.SetIsElementSet(m + 1, false);
					mo.Hilite = -1;
				}
			}
		}
	}
	else if (menu == MENU_HUNT) {
		if (g_KeyboardState[VK_ESCAPE] & 128) {
			ChangeMenuState(MENU_QUIT);
		}

		if (g_KeyboardState[VK_LBUTTON] & 128) {
			if (id >= 1 && id <= 6) {
				WaitForMouseRelease();
				g_MenuItem.ToggleIsElementSet(id);
			}
			else if (id == 7) // Back
			{
				WaitForMouseRelease();
				ChangeMenuState(MENU_MAIN);
			}
			else if (id == 8) // Hunt/Next
			{
				// Launch the game
				WaitForMouseRelease();
				//LaunchProcess("", "");
			}
		}
	}
	else if (menu == MENU_MAIN)
	{
		if (g_KeyboardState[VK_ESCAPE] & 128) {
			ChangeMenuState(MENU_QUIT);
		}
		else if (g_KeyboardState[VK_LBUTTON] & 128) {
			WaitForMouseRelease();
			if (id == 1) { WaitForMouseRelease(); ChangeMenuState(MENU_HUNT); }
			else if (id == 2) { WaitForMouseRelease(); ChangeMenuState(MENU_OPTIONS); }
			else if (id == 3) { WaitForMouseRelease(); /*LaunchProcess("", "");*/ }
			else if (id == 4) { WaitForMouseRelease(); ChangeMenuState(MENU_CREDITS); }
			else if (id == 5) { WaitForMouseRelease(); ChangeMenuState(MENU_QUIT); }
			else if (id == 6) { WaitForMouseRelease(); ChangeMenuState(MENU_STATISTICS); }
		}
	}
	else if (menu == MENU_STATISTICS)
	{
		if (g_KeyboardState[VK_ESCAPE] & 128) {
			ChangeMenuState(MENU_QUIT);
		}

		if (g_KeyboardState[VK_LBUTTON] & 128) {
			WaitForMouseRelease();
			ChangeMenuState(MENU_MAIN);
		}
	}
	else if (menu == MENU_QUIT)
	{
		if (g_KeyboardState[VK_LBUTTON] & 128) {
			WaitForMouseRelease();
			if (id == 1)      PostQuitMessage(0);
			else if (id == 2) ChangeMenuState(MENU_MAIN);
		}
	}
}


void DrawOptionsMenu()
{
	const int off_c = RGB(239, 228, 176);
	const int on_c = RGB(30, 239, 30);
	int c = off_c;

	InterfaceSetFont(fnt_Big);

	// Game options
	for (int i = 0; i < MenuOptions[OPT_GAME].Count; i++) {
		int x0 = MenuOptions[OPT_GAME].x0;
		int y0 = MenuOptions[OPT_GAME].y0 + (25 * i);

		if (MenuOptions[OPT_GAME].Hilite == i) c = on_c;
		else c = off_c;
		if (MenuOptions[OPT_GAME].Selected == i) c = RGB(100, 100, 239);

		DrawTextShadow(x0, y0, MenuOptions[OPT_GAME].Item[i], c);
	}

	// Control key bindings
	for (int i = 0; i < MenuOptions[OPT_KEYBINDINGS].Count; i++) {
		int x0 = MenuOptions[OPT_KEYBINDINGS].x0;
		int y0 = MenuOptions[OPT_KEYBINDINGS].y0 + (25 * i);

		std::stringstream ss;

		ss << KeyNames[MapVKKey(*((int32_t*)&g_Options.KeyMap + i))];

		if (MenuOptions[OPT_KEYBINDINGS].Hilite == i) c = on_c;
		else c = off_c;
		if (MenuOptions[OPT_KEYBINDINGS].Selected == i) c = RGB(100, 100, 239);

		//DrawTextShadow(x0 - (GetTextW(hdcCMain, MenuOptions[m].Item[i]) + 5), y0, MenuOptions[m].Item[i], c);
		DrawTextShadow(x0 - 160, y0, MenuOptions[OPT_KEYBINDINGS].Item[i], c);
		DrawTextShadow(x0 + 5, y0, ss.str(), c);
	}

	// Video/Graphics options
	for (int i = 0; i < MenuOptions[OPT_VIDEO].Count; i++) {
		int x0 = MenuOptions[OPT_VIDEO].x0;
		int y0 = MenuOptions[OPT_VIDEO].y0 + (25 * i);

		if (MenuOptions[OPT_VIDEO].Hilite == i) c = on_c;
		else c = off_c;
		if (MenuOptions[OPT_VIDEO].Selected == i) c = RGB(100, 100, 239);

		DrawTextShadow(x0, y0, MenuOptions[OPT_VIDEO].Item[i], c);
	}

	InterfaceSetFont(NULL);
}


// TODO: Move these to Hunt.h
#define FPS_TARGET 60L
#define FRAME_TIME_DELTA (1000L / FPS_TARGET) // The time in milliseconds a frame takes to process to achieve FPS_TARGET

int64_t g_PrevFrameTime = 0;
#ifdef _DEBUG
int32_t g_Frames = 0;
int32_t g_FramesPerSecond = 0;
int64_t g_PrevFrameCountTime = 0;
#endif

/*
Perform per-frame/tick update of the menus
*/
void ProcessMenu()
{
	GetCursorPos(&g_CursorPos);
	ScreenToClient(hwndMain, &g_CursorPos);

	// Restrict the virtual cursor to the client area
	if (g_CursorPos.x < 0) g_CursorPos.x = 0;
	if (g_CursorPos.y < 0) g_CursorPos.y = 0;
	if (g_CursorPos.x >= 800) g_CursorPos.x = 800 - 1;
	if (g_CursorPos.y >= 600) g_CursorPos.y = 600 - 1;

	// Get the keyboard state
	if (GetActiveWindow() == hwndMain) {
		GetKeyboardState(g_KeyboardState);
	}
	else {
		memset(g_KeyboardState, 0, 256);
	}

	// Trigger the Start() event of the menu if applicable
	if (g_PrevMenuState != g_MenuState) {
		MenuEventStart(g_MenuState);
		g_PrevMenuState = g_MenuState;
	}

	// Handle input from mouse/keyboard
	MenuEventInput(g_MenuState);

	// Clear the video buffer and then draw the menu background
	InterfaceClear(HIRGB(0, 0, 10));
	DrawMenuBg(g_MenuItem);

	// Draw menus
	switch (g_MenuState)
	{
	case MENU_REGISTER: DrawRegistryMenu(g_CursorPos); break;
	case MENU_MAIN: DrawProfileMenu(); break;
	case MENU_STATISTICS: DrawStatisticsMenu(); break;
	case MENU_QUIT: DrawProfileMenu(); break;
	case MENU_OPTIONS: DrawOptionsMenu(); break;
	case MENU_CREDITS: /* do rendering */ break;
	case MENU_HUNT: DrawHuntMenu(); break;
	}

#ifdef _DEBUG
	// Perform some framerate metric stuff, only for [Debug] builds though
	g_Frames++;

	int64_t t = Timer::GetTime();
	int64_t t_diff = t - g_PrevFrameTime;

	std::stringstream ss;
	ss << "FPS: " << g_FramesPerSecond;
	DrawTextShadow(2, 2, ss.str(), RGB(255, 60, 60));
	ss.str(""); ss.clear();

	ss << "FT:  " << t_diff << "ms";
	DrawTextShadow(2, 2 + 14, ss.str(), RGB(255, 60, 60));
	ss.str(""); ss.clear();

	ss << "XY:  " << g_CursorPos.x << "x" << g_CursorPos.y;
	DrawTextShadow(2, 2 + 28, ss.str(), RGB(255, 60, 60));

	if (t_diff < FRAME_TIME_DELTA) {
		std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_TIME_DELTA - t_diff));
	}

	if ((t - g_PrevFrameCountTime) >= 1000) {
		g_FramesPerSecond = g_Frames;
		g_Frames = 0;
		g_PrevFrameCountTime = t;
	}

	g_PrevFrameTime = Timer::GetTime();
#else // [Release] build frame limiter

	int64_t t = Timer::GetTime();
	int64_t t_diff = t - g_PrevFrameTime;

	if (t_diff < 16) {
		std::this_thread::sleep_for(std::chrono::milliseconds(16 - t_diff));
	}

	g_PrevFrameTime = Timer::GetTime();
#endif

	// Draw the GDI buffer to the window
	InterfaceBlt();

	//g_PrevMenuState = g_MenuState;
}


void MenuKeyCharEvent(uint16_t wParam)
{
	if (g_MenuState == MENU_REGISTER) {
		if (wParam == 8) {
			if (!g_TypingBuffer.empty())
				g_TypingBuffer.pop_back();
		}
		else {
			if (g_TypingBuffer.size() < 19) {
				if (wParam >= 32 || wParam <= 128) {
					g_TypingBuffer.push_back(static_cast<char>(wParam));
				}
			}
		}
	}
}

