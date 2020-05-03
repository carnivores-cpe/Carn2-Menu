
// -- Define UNICODE for Wide Characters WCHAR (extra symbols, Japanese Text, etc)
//#define UNICODE

// -- Define no secure warnings for sanity when compiling
#ifndef _CRT_SECURE_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Targa.h"

#include <Windows.h>
//std
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <chrono>



#ifdef _MAIN_
#	define EXTERNAL
#else
#	define EXTERNAL extern
#endif


#define HIRGB(r,g,b) ((uint16_t)( (b) | (g<<5) | (r<<10) ))
#define TRURGB(r,g,b) ((uint32_t)( (b) | (g<<8) | (r<<16) | (0xFF<<24) ) )
#define HIRGBA(r,g,b,a) ((uint16_t)( (b) | (g<<5) | (r<<10) ) | (a<<15) )
#define TRURGBA(r,g,b,a) ((uint32_t)( (b) | (g<<8) | (r<<16) | (a<<24) ) )

// ======================================================================= //
// Global Enumerators
// ======================================================================= //
enum GameStateEnum
{
	STATE_SPLASH = 0,
	STATE_MENU,
	STATE_LOADING,
	STATE_GAME
};

enum AIIndexEnum
{
	AI_HUNTER = 0,

	AI_BRACH = 9,

	AI_PARA = 10,
	AI_ANKY = 11,
	AI_STEGO = 12,
	AI_ALLO = 13,
	AI_CHASMO = 14,
	AI_VELO = 15,
	AI_SPINO = 16,
	AI_CERA = 17,
	AI_TREX = 18
};

enum ResolutionsEnum
{
	RES_320x240 = 0,
	RES_400x300 = 1,
	RES_512x384 = 2,
	RES_640x480 = 3,
	RES_800x600 = 4,
	RES_1024x768 = 5,
	RES_1280x1024 = 6,
	RES_1600x1200 = 7
};
/*
if (!HARD3D && OptRes > 5) { OptRes = 0; }
if (OptRes==0) { WinW = 320; WinH=240; }
if (OptRes==1) { WinW = 400; WinH=300; }
if (OptRes==2) { WinW = 512; WinH=384; }
if (OptRes==3) { WinW = 640; WinH=480; }
if (OptRes==4) { WinW = 800; WinH=600; }
if (OptRes==5) { WinW =1024; WinH=768; }
if (OptRes==6) { WinW =1280; WinH=1024; }
if (OptRes==7) { WinW =1600; WinH=1200; }
*/

enum AudioSystemEnum {
	AUDIO_SOFTWARE = 0,
	AUDIO_DIRECTSOUND3D = 1,
	AUDIO_AUREAL3D = 2,
	AUDIO_EAX = 3,
	// Custom audio begins here
	AUDIO_OPENAL = 4,
	AUDIO_XAUDIO = 5
};

// ======================================================================= //
// Global Types & Classes
// ======================================================================= //

#pragma pack(push, 1)
struct Color16 {
	int r : 5;
	int g : 5;
	int b : 5;
	int a : 1;

	Color16(Color16 c, int a) :
		r(c.r),
		g(c.g),
		b(c.b),
		a(a)
	{}

	Color16(uint16_t c, int a) :
		r(c & 5),
		g((c >> 5) & 5),
		b((c >> 10) & 5),
		a(a)
	{}
};
#pragma pack(pop)
typedef Color16 RGBA16;


class TargaImage
{
public:
	TARGAINFOHEADER m_Header;
	uint8_t* m_Data;

	TargaImage() : m_Header(), m_Data(nullptr) {}
	~TargaImage() {
		if (m_Data) delete[] m_Data;
	}
};


class Timer
{
private:
	static std::chrono::steady_clock::duration m_TimeStart;

public:

	static void Init()
	{
		Timer::m_TimeStart = std::chrono::steady_clock::now().time_since_epoch();
	}

	/* Replacement for winmm.lib : timeGetTime() */
	static int64_t GetTime()
	{
		auto t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch() - Timer::m_TimeStart);
		return t.count();
	}

};


/*typedef struct _tagVertex {
	float x, y, z;
	int16_t hide, bone;
} TVertex;


typedef struct _tagFace {
	int32_t		v1, v2, v3;
	int32_t		tx1, tx2, tx3;
	int32_t		ty1, ty2, ty3;
	uint16_t	Flags, DMask;
	int32_t		Distant, Next, group;
	uint32_t	reserv[3];
} TFace;


class Model
{
public:
	int32_t		VCount;
	int32_t		FCount;
	uint32_t	TexLength;

	void* lpTexture;

	TVertex* lpVertices;
	TFace* lpFace;
};*/

class Wave
{
public:
	int16_t* lpData;
	uint32_t Length;

	Wave() :
		lpData(nullptr),
		Length(0U)
	{}

	~Wave()
	{
		if (lpData)
			delete[] lpData;
	}
};


class MenuItem
{
public:
	uint16_t Image[800 * 600];
	uint16_t Image_On[800 * 600];
	uint8_t Image_Map[400 * 300];
};


class TPicture
{
public:
	int32_t w, h;
	uint16_t* lpImage;

	TPicture() :
		w(0),
		h(0),
		lpImage(nullptr)
	{}

	~TPicture()
	{
		if (lpImage)
			delete[] lpImage;
	}
};


class DinoInfo
{
public:
	std::string	Name; // Creature name
	std::string	FilePath; // Character file path
	std::string	PicturePath; // Picture file path

	int32_t Health0; // Base health
	int32_t	AI; // AI index

	bool DangerCall; // Dangerous lure call

	float Mass; // Weight of creature
	float Length; // Length of creature
	float Radius; // Area the creature occupies (for pathing and hit detection)
	float SmellK; // Scent Sensitivity
	float HearK; // Audio Sensitivity
	float LookK; // Visual Sensitivity
	float ShDelta; // Height of ship hook from ground
	int32_t Scale0; // Base Scale
	int32_t ScaleA; // Scale Variation
	int32_t BaseScore; // Base score of creature

	TPicture CallIcon;	//<- Icon associated with this species
};


class WeapInfo
{
public:
	std::string Name; //<- Weapon Name
	std::string FilePath; //<- Weapon Character File Name
	std::string BulletFilePath; //<- Bullet Icon File Name

	float Power; // Damage Per Shot of Weapon [0.0~]
	float Prec; // Precision of Weapon [0.0 - 1.1]
	float Loud; // Volume of Weapon [0.0 - 1.1]
	float Rate; // Rate Of Fire, Shots Per Second
	float Recoil; // Force of recoil [0-9999]
	float Optic; // Zoom of weapon when iron-sighted [1-10]

	int32_t Shots; // Number of Bullets Per Clip/Mag/Tube
	int32_t Fall; // Gravity on projectile during ray tracing (used for X-Bow)
	int32_t TraceC; // Amount of projectiles per shot
	int32_t Reload; // Shots before reload
	int32_t	Price;
};

class AreaInfo
{
public:
	std::string	Name; // Area Name
	std::string	MapPath; // MAP filename
	std::string	RscPath;	// RSC filename

	int Cost; // Credits cost
	int Rank; // Rank required/recommended to play map

	std::vector<int> DinosAvail; // A list of animal AI indexes that are available on this map.

	TPicture Thumbnail; // Preview icon/image associated with this area
};

class TKeyMap
{
public:
	int32_t	fkForward,
		fkBackward,
		fkUp,
		fkDown,
		fkLeft,
		fkRight,
		fkFire,
		fkShow,
		fkSLeft,
		fkSRight,
		fkStrafe,
		fkJump,
		fkRun,
		fkCrouch,
		fkCall,
		fkCCall,
		fkBinoc,
		fkSupply;
};

class Options
{
public:
	/* Game */

	bool ScentMode;
	bool CamoMode;
	bool RadarMode;
	bool TranqMode;
	int32_t	Aggression;
	int32_t Density;
	int32_t Sensitivity;
	int32_t OptSys;

	/* Graphics */

	int32_t	Resolution; // ResolutionsEnum
	int32_t Resolution_W, Resolution_H; // [NEW] Resolution in pixels
	int32_t Textures;
	int32_t ViewRange;
	int32_t LevelOfDetailBias; // [NEW] Range where the Level of Detail decreases
	int32_t Brightness;
	int32_t AlphaColorKey;
	int32_t RenderAPI; // The Render API/Version to use
	bool Fog; // Render volumetric fog
	bool Shadows; // Render real-time shadows
	bool Particles; // [NEW] Wether to render/process particles

	int32_t	SoundAPI; // Audio system

	TKeyMap KeyMap; // Controls mapping
	bool	MouseInvert;
	int32_t	MouseSensitivity;

	void Default();
};

class TrophyItem
{
public:
	int32_t ctype, weapon, phase, height, weight, score, date, time;
	float scale, range;
	int32_t r1, r2, r3, r4;
};


class ProfileStats
{
public:
	int32_t smade;
	int32_t success;
	float path;
	float time;
};

class Profile
{
public:

	char Name[128]; // Leaving this as a char array for convenience
	int32_t RegNumber;
	int32_t Score;
	int32_t Rank;
	ProfileStats Last;
	ProfileStats Total;
	TrophyItem Body[24];

	void New(const std::string& name);

};

class ProfileShort
{
public:

	std::string	m_Name;
	int32_t	m_RegNumber;
	int32_t	m_Score;
	int32_t	m_Rank;
};

// ======================================================================= //
// Global Enums
// ======================================================================= //
enum MenuStateEnum {
	//MENU_SPLASH, // Rework the menu to support this
	MENU_REGISTER,
	MENU_MAIN,
	MENU_STATISTICS,
	MENU_HUNT,
	MENU_TROPHY,
	MENU_OPTIONS,
	MENU_CREDITS,
	MENU_QUIT,
};

// ======================================================================= //
// Global Variables
// ======================================================================= //
EXTERNAL HINSTANCE				hInst;
EXTERNAL HWND					hwndMain;
EXTERNAL HDC					hdcMain;
EXTERNAL HFONT					fnt_Small;
EXTERNAL HFONT					fnt_Midd;
EXTERNAL HFONT					fnt_Big;

EXTERNAL MenuItem				g_MenuItem;
EXTERNAL std::int32_t			g_PrevMenuState;
EXTERNAL std::int32_t			g_MenuState;
EXTERNAL std::string			g_TypingBuffer;

EXTERNAL uint32_t				g_GameState;

EXTERNAL std::vector<DinoInfo>	g_DinoInfo;
EXTERNAL std::vector<WeapInfo>	g_WeapInfo;
EXTERNAL std::vector<AreaInfo>	g_AreaInfo;
EXTERNAL uint32_t				g_ProfileIndex;
EXTERNAL uint32_t				g_HiliteProfileIndex;
EXTERNAL Profile				g_UserProfile;
EXTERNAL ProfileShort			g_Profiles[10];
EXTERNAL Options				g_Options;


// ======================================================================= //
// Global Functions
// ======================================================================= //

/*** Menu ***/
void InitInterface();
void ShutdownInterface();
void InterfaceClear(uint16_t);
void InterfaceBlt();
void InterfaceSetFont(HFONT);
void DrawRectangle(int, int, int, int, Color16);
void DrawMenuItem(MenuItem& menu);
void DrawPicture(int, int, int, int, uint16_t*);
void DrawTextShadow(int x, int y, const std::string& text, uint32_t color);
void InitGameMenu();
void LoadGameMenu(unsigned int);
void MenuKeyDownEvent(uint16_t);
void MenuKeyCharEvent(uint16_t);
void MenuMouseLEvent();
void ProcessMenu();

/*** Resources ***/
void LoadResourcesScript();
void ReleaseResources();
void LoadTrophy(Profile& profile, int pr);
void SaveTrophy(Profile& profile);
bool ReadTGAFile(const std::string& path, TargaImage& tga);

/*** Networking ***/
void InitNetwork();
void ShutdownNetwork();
void NetworkGet(LPSTR url, LPSTR host, LPSTR data);
void NetworkPost(LPSTR url, LPSTR host, LPSTR data);


#ifdef _MAIN_
EXTERNAL char KeyNames[256][24] = {
"...",
"Esc",
"1",
"2",
"3",
"4",
"5",
"6",
"7",
"8",
"9",
"0",
"-",
"=",
"BackSpace",
"Tab",
"Q",
"W",
"E",
"R",
"T",
"Y",
"U",
"I",
"O",
"P",
"[",
"]",
"Enter",
"Ctrl",
"A",
"S",
"D",
"F",
"G",
"H",
"J",
"K",
"L",
";",
"'",
"~",
"Shift",
"\\",
"Z",
"X",
"C",
"V",
"B",
"N",
"M",
",",
".",
"/",
"Shift",
"*",
"Alt",
"Space",
"CLock",
"F1",
"F2",
"F3",
"F4",
"F5",
"F6",
"F7",
"F8",
"F9",
"F10",
"NLock",
"SLock",
"Home",
"Up",
"PgUp",
"-",
"Left",
"Middle",
"Right",
"+",
"End",
"Down",
"PgDn",
"Ins",
"Del",
"",
"",
"",
"F11",
"F12",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"MouseL",
"MouseR",
"MouseM",
"<?>",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
};
#else
EXTERNAL char KeyNames[128][24];
#endif
