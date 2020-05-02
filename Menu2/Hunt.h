
// -- Define UNICODE for Wide Characters WCHAR (extra symbols, Japanese Text, etc)
//#define UNICODE

// -- Define no secure warnings for sanity when compiling
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>
//std
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>



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
		g((c>>5 ) & 5),
		b((c>>10) & 5),
		a(a)
	{}
};
#pragma pack(pop)
typedef Color16 RGBA16;

typedef struct _tagVertex {
	float x,y,z;
	short hide,bone;
} TVertex;


typedef struct _tagFace {
	int		v1,v2,v3;
	int		tx1,tx2,tx3;
	int		ty1,ty2,ty3;
	WORD	Flags,DMask;
	int		Distant, Next, group;
	char	reserv[12];
} TFace;


class TModel
{
public:
	int		VCount;
	int		FCount;
	int		TexLength;

	void*	lpTexture;

	TVertex*	lpVertices;
	TFace*		lpFace;
};

class TWave
{
public:
	short int*	lpData;
	UINT		Length;

	TWave()
	{
		lpData = NULL;
		Length = 0;
	}
	~TWave()
	{
		if (lpData) delete [] lpData;
	}
};


class TMenuItem
{
public:
	uint16_t Image[800*600];
	uint16_t Image_On[800*600];
	uint8_t Image_Map[400*300];
};


class TPicture
{
public:
	int w,h;
	WORD *lpImage;

	TPicture()
	{
		w = 0;
		h = 0;
		lpImage = NULL;
	}
	~TPicture()
	{
		if (lpImage) delete [] lpImage;
	}
};


class TDinoInfo
{
public:
	char	Name[48],	//<- Creature Name
			FName[48],	//<- File Name
			PName[48];	//<- ???

	int		Health0,	//<- Base health
			AI;			//<- AI code

	BOOL DangerCall;	//<- Dangerous call

	float	Mass,		//<- Weight of creature
			Length,		//<- Length of creature
			Radius,		//<- ???
			SmellK,		//<- Scent Sensitivity
			HearK,		//<- Audio Sensitivity
			LookK,		//<- Visual Sensitivity
			ShDelta;	//<- Height of ship hook from ground
	int		Scale0,		//<- Base Scale
			ScaleA,		//<- Scale Variation
			BaseScore;	//<- Base score of creature

	TPicture CallIcon;	//<- Icon associated with this species
};


typedef struct _TWeapInfo
{
	char	Name[48],	//<- Weapon Name
			FName[48],	//<- Weapon File Name
			BFName[48];	//<- ???

	float	Power,		//<- Damage Per Shot of Weapon [0-9999]
			Prec,		//<- Precision of Weapon [0-1]
			Loud,		//<- Volume of Weapon [0-1]
			Rate,		//<- Rate Of Fire, Shots Per Second
			Recoil,		//<- Force of recoil [0-9999]
			Optic;		//<- Zoom of weapon when iron-sighted [1-10]

	int		Shots,		//<- Number of Bullets Per Clip/Chamber
			Fall,		//<- Gravity on projectile during ray tracing
			TraceC,		//<- ???
			Reload,		//<- Shots before reload
			Price;
} TWeapInfo;

class TAreaInfo
{
public:
	char	Name[48],	//<- Creature Name
			MapFile[255],	//<- MAP filename
			RscFile[255];	//<- RSC filename

	int		Cost;			//<-Credits cost
	int		Rank;			//<-Rank required/recommended to play map

	TPicture Thumbnail;	//<- Icon associated with this area
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

class TOptions
{
public:
	// === Game === //
	bool	ScentMode,
			CamoMode,
			RadarMode,
			TranqMode;
	int32_t	Aggression,
			Density,
			Sensitivity,
			OptSys;

	// === Graphics === //
	int32_t	Resolution,
			Resolution_W, Resolution_H, /// [NEW] Resolution in pixels
			Textures,
			ViewRange,
			LevelOfDetailBias,			/// [NEW] Range where the Level of Detail decreases
			Brightness,
			AlphaColorKey,
			RenderAPI;					/// The Render API/Version to use
	bool	Fog,						/// Render volumetric fog
			Shadows,					/// Render real-time shadows
			Particles;					/// [NEW] Wether to render/process particles

	// === Audio === //
	int32_t	SoundAPI;

	// === Controls === //
	TKeyMap KeyMap;
	bool	MouseInvert;
	int32_t	MouseSensitivity;
};

class TTrophyItem
{
public:
	int32_t ctype, weapon, phase, height, weight, score, date, time;
	float scale, range;
	int32_t r1, r2, r3, r4;
};


class TStats
{
public:
	int32_t	smade,
			success;
	float	path,
			time;
};

class TProfile
{
public:

	char		Name[128];
	int32_t		RegNumber,
				Score,
				Rank;
	TStats		Last,
				Total;
	TTrophyItem Body[24];

};

class TProfileShort
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
EXTERNAL HFONT					fnt_Small,
								fnt_Midd,
								fnt_Big;

EXTERNAL TMenuItem				g_MenuItem;
EXTERNAL std::int32_t			g_PrevMenuState;
EXTERNAL std::int32_t			g_MenuState;
EXTERNAL std::string			g_TypingBuffer;

EXTERNAL unsigned int			g_GameState;

EXTERNAL std::vector<TDinoInfo>	g_DinoInfo;
EXTERNAL std::vector<TWeapInfo>	g_WeapInfo;
EXTERNAL std::vector<TAreaInfo>	g_AreaInfo;
EXTERNAL TProfile				g_UserProfile;
EXTERNAL TProfileShort			g_Profiles[10];
EXTERNAL TOptions				g_Options;


// ======================================================================= //
// Global Functions
// ======================================================================= //

/*** Menu ***/
void InitInterface();
void ShutdownInterface();
void InterfaceClear(uint16_t);
void InterfaceBlt();
void InterfaceSetFont(HFONT);
void DrawRectangle(int,int,int,int, Color16);
void DrawMenuItem(TMenuItem* mptr);
void DrawPicture(int,int, int, int, uint16_t*);
//void DrawText(int, int, std::string, uint32_t);
void InitGameMenu(UINT);
void LoadGameMenu(unsigned int);
void MenuKeyDownEvent(uint16_t);
void MenuMouseLEvent();
void ProcessMenu();

/*** Resources ***/
void LoadResourcesScript();
void ReleaseResources();
void LoadTrophy(TProfile &profile, int pr);
void SaveTrophy(TProfile &profile);

/*** Networking ***/
void	InitNetwork();
void	ShutdownNetwork();
void	NetworkGet( LPSTR url, LPSTR host, LPSTR data );
void	NetworkPost( LPSTR url, LPSTR host, LPSTR data );


#ifdef _MAIN_
EXTERNAL char KeysName[256][24] = {
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
"Mouse1",
"Mouse2",
"Mouse3",
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
   EXTERNAL char KeysName[128][24];
#endif
