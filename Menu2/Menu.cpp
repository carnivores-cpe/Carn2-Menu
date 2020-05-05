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

	void AddItem(const std::string& txt);
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
MenuSet MenuRegistry;

void* lpVideoBuf;
HDC hdcCMain;
HBITMAP bmpMain;
HBITMAP hbmpOld;
HFONT hfntOld;

POINT g_CursorPos;
int g_WaitKey = -1;
BOOL g_KeyboardUsed = false;


// String table
const char st_BoolText[2][4] = { "Off", "On" };
const char st_UnitText[2][10] = { "Metric", "Imperial" };
const char st_HMLText[4][8] = { "Low", "Medium", "High", "Ultra" };
const char st_TextureText[3][5] = { "Low", "High", "Auto" };
const char st_AlphaKeyText[2][14] = { "Color Key", "Alpha Channel" };
const char st_RenText[4][11] = { "Software", "3Dfx Glide", "Direct3D 7", "OpenGL" };
const char st_AudText[5][16] = { "Software", "Direct Sound 3D", "Aureal 3D", "EAX", "OpenAL" };


void WaitForMouseRelease()
{
	while (GetAsyncKeyState(VK_RBUTTON) & 0x80000000);
	while (GetAsyncKeyState(VK_MBUTTON) & 0x80000000);
	while (GetAsyncKeyState(VK_LBUTTON) & 0x80000000);
}


void AcceptNewKey()
{
	uint8_t keystate[256];

	if (GetKeyboardState(keystate)) {

		if (keystate[VK_ESCAPE] & 128)
		{
			*((uint32_t*)(&g_Options.KeyMap) + g_WaitKey) = 0;
			g_WaitKey = -2;
			return;
		}
		else
		{
			for (int k = 0; k < 255; k++)
			{
				if (keystate[k] & 128)
				{
					for (int t = 0; t < 16; t++)
						if (*((uint32_t*)(&g_Options.KeyMap) + t) == k)
							*((uint32_t*)(&g_Options.KeyMap) + t) = 0;

					*((int*)(&g_Options.KeyMap) + g_WaitKey) = k;
					WaitForMouseRelease();
					g_WaitKey = -1;
					return;
				}
			}
		}
	}

	//g_WaitKey = -1;
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


void MenuSet::AddItem(const std::string& txt)
{
	this->Item[this->Count] = txt;
	//ms.y0 = ms.y0 + (ms.Count * 25);
	this->Count++;
}


/*
Initialise all the required settings and interface elements
*/
void InitInterface()
{
	std::cout << "Initialising Menu Interface..." << std::endl;
	fnt_Big = CreateFont(
		23, 10, 0, 0,
		600, 0, 0, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);

	g_FontOptions = CreateFont(
		21, 9, 0, 0,
		500, 0, 0, 0,
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

	MenuRegistry.x0 = 360;
	MenuRegistry.y0 = 363;
	//MenuEventStart(MENU_REGISTER) contains the AddMenuItem() calls

	MenuRegistry.Rect = { 360, 363, 499, 491 }; // Enough room for 9.14 items with fnt_Small

	int m = OPT_GAME;
	MenuOptions[m].x0 = 40; // 170 HX, 380 W
	MenuOptions[m].y0 = 75;
	MenuOptions[m].Count = 0;
	MenuOptions[m].AddItem("Agressivity");
	MenuOptions[m].AddItem("Density");
	MenuOptions[m].AddItem("Sensitivity");
	MenuOptions[m].AddItem("View range");
	MenuOptions[m].AddItem("Measurement");
	MenuOptions[m].AddItem("Sound API");
	MenuOptions[m].Rect = { 40, 75, 380, 75 + (MenuOptions[0].Count * 24) };

	m = OPT_KEYBINDINGS;
	MenuOptions[m].x0 = 422;
	MenuOptions[m].y0 = 75;
	MenuOptions[m].Count = 0;
	MenuOptions[m].AddItem("Forward");
	MenuOptions[m].AddItem("Backward");
	MenuOptions[m].AddItem("Turn Up");
	MenuOptions[m].AddItem("Turn Down");
	MenuOptions[m].AddItem("Turn Left");
	MenuOptions[m].AddItem("Turn Right");
	MenuOptions[m].AddItem("Fire");
	MenuOptions[m].AddItem("Draw Weapon");
	MenuOptions[m].AddItem("Step Left");
	MenuOptions[m].AddItem("Step Right");
	MenuOptions[m].AddItem("Strafe");
	MenuOptions[m].AddItem("Jump");
	MenuOptions[m].AddItem("Run");
	MenuOptions[m].AddItem("Crouch");
	MenuOptions[m].AddItem("Lure Call");
	MenuOptions[m].AddItem("Change Call");
	MenuOptions[m].AddItem("Binoculars");
	MenuOptions[m].AddItem("Call Resupply");
	MenuOptions[m].AddItem("Invert Mouse");
	MenuOptions[m].AddItem("Mouse sensitivity");
	MenuOptions[m].Rect = { 422, 75, 760, 75 + (MenuOptions[1].Count * 24) };

	m = OPT_VIDEO;
	MenuOptions[m].x0 = 70;
	MenuOptions[m].y0 = 350;
	MenuOptions[m].Count = 0;
	MenuOptions[m].AddItem("Graphics API");
	MenuOptions[m].AddItem("Resolution");
	MenuOptions[m].AddItem("Shadows");
	MenuOptions[m].AddItem("Fog");
	MenuOptions[m].AddItem("Textures");
	MenuOptions[m].AddItem("Alpha Source");
	MenuOptions[m].AddItem("Brightness");
	MenuOptions[m].Rect = { 40, 350, 380, 350 + (MenuOptions[2].Count * 24) };

	std::cout << "Menu Interface initialised!" << std::endl;
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
	if (g_FontOptions)
		DeleteObject(g_FontOptions);

	std::cout << "Interface: Shutdown Ok!" << std::endl;
}


void _Line(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}


void DrawProgressBar(int x, int y, float l)
{
	int W = 120; y += 13;

	HPEN wp = CreatePen(PS_SOLID, 0, 0x009F9F9F);
	HBRUSH wb = CreateSolidBrush(0x003FAF3F);

	HPEN oldpen = (HPEN)SelectObject(hdcCMain, GetStockObject(BLACK_PEN));
	HBRUSH oldbrs = (HBRUSH)SelectObject(hdcCMain, GetStockObject(BLACK_BRUSH));


	x += 1; y += 1;
	_Line(hdcCMain, x, y - 9, x + W + 1, y - 9);
	_Line(hdcCMain, x, y, x + W + 1, y);
	_Line(hdcCMain, x, y - 8, x, y);
	_Line(hdcCMain, x + W, y - 8, x + W, y);
	_Line(hdcCMain, x + W / 2, y - 8, x + W / 2, y);
	_Line(hdcCMain, x + W / 4, y - 8, x + W / 4, y);
	_Line(hdcCMain, x + W * 3 / 4, y - 8, x + W * 3 / 4, y);

	x -= 1; y -= 1;
	SelectObject(hdcCMain, wp);
	_Line(hdcCMain, x, y - 9, x + W + 1, y - 9);
	_Line(hdcCMain, x, y, x + W + 1, y);
	_Line(hdcCMain, x, y - 8, x, y);
	_Line(hdcCMain, x + W, y - 8, x + W, y);
	_Line(hdcCMain, x + W / 2, y - 8, x + W / 2, y);
	_Line(hdcCMain, x + W / 4, y - 8, x + W / 4, y);
	_Line(hdcCMain, x + W * 3 / 4, y - 8, x + W * 3 / 4, y);

	W -= 2;
	PatBlt(hdcCMain, x + 2, y - 5, (int)(W * l / 2.f), 4, PATCOPY);

	SelectObject(hdcCMain, wb);
	PatBlt(hdcCMain, x + 1, y - 6, (int)(W * l / 2.f), 4, PATCOPY);


	SelectObject(hdcCMain, oldpen);
	SelectObject(hdcCMain, oldbrs);
	DeleteObject(wp);
	DeleteObject(wb);
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


void DrawSliderBar(int x, int y, int w, float v, int slider_rgb = RGB(239, 228, 176))
{
	if (v < 0.0f)
		v = 0.0f;
	if (v > 1.0f)
		v = 1.0f;

	int xs = x + ((w-2) * v);

	//HPEN wp = CreatePen(PS_SOLID, 0, 0x009F9F9F);

	HBRUSH wb = CreateSolidBrush(slider_rgb);// RGB(239, 228, 176));

	HPEN oldpen = (HPEN)SelectObject(hdcCMain, GetStockObject(BLACK_PEN));
	HBRUSH oldbrs = (HBRUSH)SelectObject(hdcCMain, GetStockObject(BLACK_BRUSH));

	// Draw track
	_Line(hdcCMain, x, y - 4, x + w + 1, y - 4);
	_Line(hdcCMain, x, y - 3, x + w + 1, y - 3);
	_Line(hdcCMain, x, y - 2, x + w + 1, y - 2);
	_Line(hdcCMain, x, y - 1, x + w + 1, y - 1);
	_Line(hdcCMain, x, y + 0, x + w + 1, y + 0);
	_Line(hdcCMain, x, y + 1, x + w + 1, y + 1);
	_Line(hdcCMain, x, y + 2, x + w + 1, y + 2);
	_Line(hdcCMain, x, y + 3, x + w + 1, y + 3);

	SelectObject(hdcCMain, wb);
	PatBlt(hdcCMain, xs, y-4, 6, 8, PATCOPY);

	SelectObject(hdcCMain, oldpen);
	SelectObject(hdcCMain, oldbrs);
	//DeleteObject(wp);
	DeleteObject(wb);
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
	if (align == DTA_RIGHT)
	{
		x -= GetTextW(hdcCMain, text);
	}

	SetBkMode(hdcCMain, TRANSPARENT);
	SetTextColor(hdcCMain, color);
	TextOut(hdcCMain, x, y, text.c_str(), (int)text.size());
}


void DrawTextShadow(int x, int y, const std::string& text, uint32_t color, int align = DTA_LEFT)
{
	if (align == DTA_RIGHT)
	{
		x -= GetTextW(hdcCMain, text);
	}

	DrawTextColor(x + 1, y + 1, text, RGB(0, 0, 0));
	//DrawTextColor(x + 1, y, text, RGB(0, 0, 0));
	//DrawTextColor(x, y + 1, text, RGB(0, 0, 0));
	DrawTextColor(x, y, text, color);
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


/*
Menu screen initialization event
NOTE: This only gets called on the first tick that the menu exists, it can be used to
initialize variables and reset various things.
*/
void MenuEventStart(int32_t menu_state)
{
	g_MenuItem.ResetElementSet();

	switch (menu_state) {
	case MENU_REGISTER: {
		MenuRegistry.Count = 0;
		MenuRegistry.AddItem("");
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
		g_WaitKey = -1;

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

	InterfaceSetFont(fnt_Midd);
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
		color = 0xB0B070; // Base colour

		if (i == g_HiliteProfileIndex)
			color = RGB(255, 170, 10);

		if (p.x >= 308 && p.y >= (368 + (16 * i)) && p.x <= 408 && p.y <= (368 + (16 * i) + 16)) {
			g_HiliteProfileIndex = i; //temporary, move to another function for user input
#ifdef _DEBUG
			color = RGB(42, 255, 42);
#endif
		}

		std::string tname = g_Profiles[i].m_Name;

		if (g_Profiles[i].m_Name.empty()) {
			tname = "...";
		}

		DrawTextShadow(320, 370 + (16 * i), tname, color);
	}

	InterfaceSetFont(NULL);
}


/*
Keyboard and Mouse handling for menus
NOTE: Could move these to individual functions if a state machine is confusing
*/
void MenuEventInput(int32_t menu)
{
	if (!g_KeyboardUsed) return;

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
					TrophySave(g_UserProfile);
				}
				else {
					TrophyLoad(g_UserProfile, g_ProfileIndex);
				}

				WaitForMouseRelease();
				ChangeMenuState(MENU_MAIN);
			}
			else if (id == 2) {
				// Delete the selected 'save'
				WaitForMouseRelease();
				//TrophyDelete(g_ProfileIndex);
				MenuEventStart(menu); // -- Retrigger the start event
			}
			else {
				WaitForMouseRelease();
				g_ProfileIndex = g_HiliteProfileIndex;
			}
		}
	}
	/**************************************
	* Options Menu
	*/
	else if (menu == MENU_OPTIONS) {
		if (g_WaitKey >= 0)
		{
			AcceptNewKey();
		}
		else if (id == 4)
		{
			if (g_KeyboardState[VK_LBUTTON] & 128)
			{
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

					// We use a little division to find out what item is hovered over
					// the original games used for loops to do this... crazy
					if (yd > 0)
					{
						mo.Hilite = yd / 22; // 22 is the height of each item, we should store this in a variable
					}
					else mo.Hilite = -1;

					if (g_KeyboardState[VK_LBUTTON] & 128)
					{
						WaitForMouseRelease();

						if (m == OPT_GAME) {
							//click
							mo.Selected = mo.Hilite;
							if (mo.Hilite == 19)
							{
								g_Options.Aggression = 128;
							}
							else if (mo.Hilite == 1)
							{
								g_Options.Density = 128;
							}
							else if (mo.Hilite == 2)
							{
								g_Options.Sensitivity = 128;
							}
							else if (mo.Hilite == 3)
							{
								g_Options.ViewRange = 128;
							}
							else if (mo.Hilite == 4) // Metric or Imperial(US)
							{
								g_Options.OptSys = !g_Options.OptSys;
							}
							else if (mo.Hilite == 5)
							{
								g_Options.SoundAPI++;
								if (g_Options.SoundAPI == 4)
									g_Options.SoundAPI = 0;
							}
						}
						if (m == OPT_KEYBINDINGS) {
							//click
							mo.Selected = mo.Hilite;

							if (mo.Hilite < 18)
							{
								g_WaitKey = mo.Hilite;
							}
							else if (mo.Hilite == 18) // Mouse Y-Axis Inverted
							{
								g_Options.MouseInvert = !g_Options.MouseInvert;
							}
							else if (mo.Hilite == 19) // Mouse Sensitivty Slider
							{
								g_Options.MouseSensitivity = 128;
							}
						}
						if (m == OPT_VIDEO) {
							//click
							mo.Selected = mo.Hilite;
							if (mo.Hilite == 0)
							{
								g_Options.RenderAPI++;
								if (g_Options.RenderAPI == 3)
									g_Options.RenderAPI = 0;
							}
							if (mo.Hilite == 1) // Resolution
							{
								g_Options.Resolution++;
								if (g_Options.Resolution == RES_MAX)
									g_Options.Resolution = 0;
							}
							else if (mo.Hilite == 2) // Shadows
							{
								g_Options.Shadows = !g_Options.Shadows;
							}
							else if (mo.Hilite == 3) // Fog
							{
								g_Options.Fog = !g_Options.Fog;
							}
							else if (mo.Hilite == 4) // Textures
							{
								g_Options.Textures++;
								if (g_Options.Textures == 3)
									g_Options.Textures = 0;
							}
							else if (mo.Hilite == 5) // Colorkey
							{
								g_Options.AlphaColorKey = !g_Options.AlphaColorKey;
							}
							else if (mo.Hilite == 6) // Brightness
							{
								g_Options.Brightness = 128; // TODO: Sliders
							}
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
	const int label_c = 0x007696b5; // From Carnivores: Ice Age (JPEG image)
	const int value_c = 0x00abb4a7; // From Carnivores: Ice Age (JPEG image)
	const int off_c = RGB(239, 228, 176);
	const int on_c = RGB(30, 239, 30);
	int c = off_c;

	InterfaceSetFont(g_FontOptions);

	// Game options
	for (int i = 0; i < MenuOptions[OPT_GAME].Count; i++) {
		int x0 = MenuOptions[OPT_GAME].Rect.left + 20;// .x0;
		int x1 = MenuOptions[OPT_GAME].Rect.right - 20;// .x0;
		int y0 = MenuOptions[OPT_GAME].y0 + (22 * i);
		int tbw = ((MenuOptions[OPT_GAME].Rect.right - MenuOptions[OPT_GAME].Rect.left) / 2) - 20;

		if (MenuOptions[OPT_GAME].Hilite == i) c = on_c;
		else c = label_c;
		//if (MenuOptions[OPT_GAME].Selected == i) c = RGB(100, 100, 239);

		DrawTextShadow(x0, y0, MenuOptions[OPT_GAME].Item[i], c);

		if (i == 0) DrawSliderBar(x1 - tbw, y0 + 12, tbw, (float)g_Options.Aggression / 255.0f, label_c);
		if (i == 1) DrawSliderBar(x1 - tbw, y0 + 12, tbw, (float)g_Options.Density / 255.0f, label_c);
		if (i == 2) DrawSliderBar(x1 - tbw, y0 + 12, tbw, (float)g_Options.Sensitivity / 255.0f, label_c);
		if (i == 3) DrawSliderBar(x1 - tbw, y0 + 12, tbw, (float)g_Options.ViewRange / 255.0f, label_c);
		if (i == 4) DrawTextShadow(x1, y0, st_UnitText[g_Options.OptSys], value_c, DTA_RIGHT);
		if (i == 5) DrawTextShadow(x1, y0, st_AudText[g_Options.SoundAPI], value_c, DTA_RIGHT);
	}

	// Control key bindings
	for (int i = 0; i < MenuOptions[OPT_KEYBINDINGS].Count; i++) {
		int x0 = MenuOptions[OPT_KEYBINDINGS].Rect.left + 20;// .x0;
		int x1 = MenuOptions[OPT_KEYBINDINGS].Rect.right - 20;// .x0;
		int y0 = MenuOptions[OPT_KEYBINDINGS].y0 + (22 * i);
		int tbw = ((MenuOptions[OPT_KEYBINDINGS].Rect.right - MenuOptions[OPT_KEYBINDINGS].Rect.left) / 2) - 20;

		std::stringstream ss;

		ss << KeyNames[MapVKKey(*((int32_t*)&g_Options.KeyMap + i))];

		if (MenuOptions[OPT_KEYBINDINGS].Hilite == i) c = on_c;
		else c = label_c;
		//if (MenuOptions[OPT_KEYBINDINGS].Selected == i) c = RGB(100, 100, 239);

		DrawTextShadow(x0, y0, MenuOptions[OPT_KEYBINDINGS].Item[i], c);

		if (i < 18)
		{
			if (g_WaitKey == i) DrawTextShadow(x1, y0, "<?>", value_c, DTA_RIGHT);
			else                DrawTextShadow(x1, y0, ss.str(), value_c, DTA_RIGHT);
		}
		else if (i == 18) DrawTextShadow(x1, y0, st_BoolText[(int)g_Options.MouseInvert], value_c, DTA_RIGHT);
		else if (i == 19) DrawSliderBar(x1 - tbw, y0 + 12, tbw, (float)g_Options.MouseSensitivity / 255.0f, label_c);
	}

	// Video/Graphics options
	for (int i = 0; i < MenuOptions[OPT_VIDEO].Count; i++) {
		int x0 = MenuOptions[OPT_VIDEO].Rect.left + 20;// x0;
		int x1 = MenuOptions[OPT_VIDEO].Rect.right - 20;// x0;
		int y0 = MenuOptions[OPT_VIDEO].y0 + (22 * i);
		int tbw = ((MenuOptions[OPT_VIDEO].Rect.right - MenuOptions[OPT_VIDEO].Rect.left) / 2) - 20;

		if (MenuOptions[OPT_VIDEO].Hilite == i) c = on_c;
		else c = label_c;
		//if (MenuOptions[OPT_VIDEO].Selected == i) c = RGB(100, 100, 239);

		DrawTextShadow(x0, y0, MenuOptions[OPT_VIDEO].Item[i], c);

		if (i == 0) DrawTextShadow(x1, y0, st_RenText[g_Options.RenderAPI], value_c, DTA_RIGHT);
		else if (i == 1) {
			switch (g_Options.Resolution) {
			case 0: DrawTextShadow(x1, y0, "320 x 240", value_c, DTA_RIGHT); break;
			case 1: DrawTextShadow(x1, y0, "400 x 300", value_c, DTA_RIGHT); break;
			case 2: DrawTextShadow(x1, y0, "512 x 384", value_c, DTA_RIGHT); break;
			case 3: DrawTextShadow(x1, y0, "640 x 480", value_c, DTA_RIGHT); break;
			case 4: DrawTextShadow(x1, y0, "800 x 600", value_c, DTA_RIGHT); break;
			case 5: DrawTextShadow(x1, y0, "1024 x 768", value_c, DTA_RIGHT); break;
			case 6: DrawTextShadow(x1, y0, "1280 x 1024", value_c, DTA_RIGHT); break;
			case 7: DrawTextShadow(x1, y0, "1600 x 1200", value_c, DTA_RIGHT); break;
			}
		}
		else if (i == 2) DrawTextShadow(x1, y0, st_BoolText[g_Options.Shadows], value_c, DTA_RIGHT);
		else if (i == 3) DrawTextShadow(x1, y0, st_BoolText[g_Options.Fog], value_c, DTA_RIGHT);
		else if (i == 4) DrawTextShadow(x1, y0, st_TextureText[g_Options.Textures], value_c, DTA_RIGHT);
		else if (i == 5) DrawTextShadow(x1, y0, st_AlphaKeyText[g_Options.AlphaColorKey], value_c, DTA_RIGHT);
		else if (i == 6) DrawSliderBar(x1 - tbw, y0 + 12, tbw, (float)g_Options.Brightness / 255.0f, label_c);
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
		g_KeyboardUsed = GetKeyboardState(g_KeyboardState);
	}
	else {
		memset(g_KeyboardState, 0, 256);
		g_KeyboardUsed = false;
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

