/***************************************************
* AtmosFear 2.1
* Characters.cpp
*
* Resources, Files and Memory Management
*
*/

#include "Hunt.h"
#include "Targa.h"

#include <iostream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>


void ReadWeapons(FILE*);
void ReadCharacters(FILE*);


void ReadWeapons(FILE *stream)
{
	char line[256], *value;

    while (fgets( line, 255, stream))
	{
		if (strstr(line, "}")) break;
		if (strstr(line, "{"))
		{
			WeapInfo Blank;
			g_WeapInfo.push_back( Blank );

			while (fgets( line, 255, stream))
			{
				size_t CurW = g_WeapInfo.size()-1;

				if (strstr(line, "}")) { break; }
				value = strstr(line, "=");
				if (!value) throw std::runtime_error("Script loading error");
				value++;

				if (strstr(line, "power"))  g_WeapInfo[CurW].m_Power = (float)atof(value);
				if (strstr(line, "prec"))   g_WeapInfo[CurW].m_Prec  = (float)atof(value);
				if (strstr(line, "loud"))   g_WeapInfo[CurW].m_Loud  = (float)atof(value);
				if (strstr(line, "rate"))   g_WeapInfo[CurW].m_Rate  = (float)atof(value);
				if (strstr(line, "shots"))  g_WeapInfo[CurW].m_Shots =        atoi(value);
				if (strstr(line, "reload")) g_WeapInfo[CurW].m_Reload=        atoi(value);
				if (strstr(line, "trace"))  g_WeapInfo[CurW].m_TraceC=        atoi(value)-1;
				if (strstr(line, "optic"))  g_WeapInfo[CurW].m_Optic = (float)atof(value);
				if (strstr(line, "fall"))   g_WeapInfo[CurW].m_Fall  =        atoi(value);
				if (strstr(line, "price"))	g_WeapInfo[CurW].m_Price =        atoi(value);

				if (strstr(line, "name")) {
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					g_WeapInfo[CurW].m_Name = &value[1]; }

				if (strstr(line, "file")) {
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					g_WeapInfo[CurW].m_FilePath = &value[1]; }

				if (strstr(line, "pic")) {
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					g_WeapInfo[CurW].m_BulletFilePath = &value[1];}
			}
		}

	}

}


void ReadCharacters(FILE *stream)
{
	char line[256], *value;
    while (fgets( line, 255, stream))
	{
		if (strstr(line, "}")) break;
		if (strstr(line, "{"))
		{
			DinoInfo di;

			while (fgets( line, 255, stream))
			{
				if (strstr(line, "}"))
				{
                    //AI_to_CIndex[DinoInfo[TotalC].AI] = TotalC;
					//TotalC++;
					break;
				}

				value = strstr(line, "=");
				if (!value)
					throw std::runtime_error("Script loading error");
				value++;

				if (strstr(line, "mass"     )) di.m_Mass      = (float)atof(value);
				if (strstr(line, "length"   )) di.m_Length    = (float)atof(value);
				if (strstr(line, "radius"   )) di.m_Radius    = (float)atof(value);
				if (strstr(line, "health"   )) di.m_Health0   = atoi(value);
				if (strstr(line, "basescore")) di.m_BaseScore = atoi(value);
				if (strstr(line, "ai"       )) di.m_AI        = atoi(value);
				if (strstr(line, "smell"    )) di.m_SmellK    = (float)atof(value);
				if (strstr(line, "hear"     )) di.m_HearK     = (float)atof(value);
				if (strstr(line, "look"     )) di.m_LookK     = (float)atof(value);
				// -> Safety Check
				if (strstr(line, "smellk"   )) di.m_SmellK    = (float)atof(value);
				if (strstr(line, "heark"    )) di.m_HearK     = (float)atof(value);
				if (strstr(line, "lookk"    )) di.m_LookK     = (float)atof(value);
				// <- End
				if (strstr(line, "shipdelta")) di.m_ShDelta   = (float)atof(value);
				if (strstr(line, "scale0"   )) di.m_Scale0    = atoi(value);
				if (strstr(line, "scaleA"   )) di.m_ScaleA    = atoi(value);
				if (strstr(line, "danger"   )) di.m_DangerCall= true;

				if (strstr(line, "name"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					di.m_Name = &value[1];
				}

				if (strstr(line, "file"))
				{
					/*int inc = atoi(line);
					char error[256];
					sprintf(error, "The file value is integer %d", inc);
					MessageBox(hwndMain, error, "Integer", MB_OK);*/
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					di.m_FilePath = &value[1];
				}

				if (strstr(line, "pic"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					di.m_PicturePath = &value[1];
				}
			}

			g_DinoInfo.push_back(di);
		}

	}
}


void ReadAreas(FILE *stream)
{
	char line[256], *value;
    while (fgets( line, 255, stream))
	{
		if (strstr(line, "}")) break;
		if (strstr(line, "{"))
		{
			AreaInfo area;

			while (fgets( line, 255, stream))
			{
				if (strstr(line, "}"))
				{
					break;
				}

				value = strstr(line, "=");
				if (!value)
					throw std::runtime_error("Script loading error");
				value++;

				if (strstr(line, "cost"   ))	area.m_Cost = atoi(value);
				if (strstr(line, "rank"))		area.m_Rank = atoi(value);

				if (strstr(line, "name"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					area.m_Name = &value[1];
				}

				if (strstr(line, "pname"))
				{
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					area.m_ProjectName = &value[1];
				}

				if (strstr(line, "thumbnail"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					///TODO: Load TPicture
					//strcpy(area.Thumbnail, &value[1]);
				}
			}

			g_AreaInfo.push_back(area);
		}

	}
}


void LoadResourcesScript()
{
    FILE *file;
	char line[256];

	file = fopen("huntdat/_RES.TXT", "r");
	if (!file) {
		throw std::runtime_error("Can't open resources file _res.txt");
		return;
	}

	while (fgets( line, 255, file))
	{
		if (line[0] == '.') break;			//endoffile EOF
		if (strstr(line, "//") ) continue;	//comment
		if (strstr(line, "#") ) continue;	//comment
		if (strstr(line, "--") ) continue;	//comment
		if (strstr(line, "~") ) continue;	//comment
		if (strstr(line, "version") ) continue; //(todo) read version
		if (strstr(line, "weapons") ) ReadWeapons(file);
		if (strstr(line, "characters") ) ReadCharacters(file);
		if (strstr(line, "dinosaurs") ) ReadCharacters(file);
		if (strstr(line, "creatures") ) ReadCharacters(file);
		if (strstr(line, "areas") ) ReadAreas(file);
	}

	fclose (file);
}


void ReleaseResources()
{
	g_WeapInfo.clear();
	g_DinoInfo.clear();
	g_AreaInfo.clear();
}

void TrophyLoad(Profile& profile, int pr)
{
	//PlayerProfile.RegNumber
	memset(&profile, 0, sizeof(Profile));
	profile.RegNumber = pr;

	std::stringstream fname;
	fname << "trophy" << std::setfill('0') << std::setw(2) << profile.RegNumber << ".sav";

	std::ifstream fs(fname.str(), std::ios::binary);

	if (!fs.is_open()) {
		std::cout << "===> Error loading trophy!" << std::endl;
		return;
	}

	fs.read((char*)&profile, sizeof(Profile));

	fs.read((char*)&g_Options.Aggression, 4);
	fs.read((char*)&g_Options.Density, 4);
	fs.read((char*)&g_Options.Sensitivity, 4);
	fs.read((char*)&g_Options.Resolution, 4);
	fs.read((char*)&g_Options.Fog, 4);
	fs.read((char*)&g_Options.Textures, 4);
	fs.read((char*)&g_Options.ViewRange, 4);
	fs.read((char*)&g_Options.Shadows, 4);
	fs.read((char*)&g_Options.MouseSensitivity, 4);
	fs.read((char*)&g_Options.Brightness, 4);
	fs.read((char*)&g_Options.KeyMap, sizeof(TKeyMap));
	fs.read((char*)&g_Options.MouseInvert, 4);
	fs.read((char*)&g_Options.ScentMode, 4);
	fs.read((char*)&g_Options.CamoMode, 4);
	fs.read((char*)&g_Options.RadarMode, 4);
	fs.read((char*)&g_Options.TranqMode, 4);
	fs.read((char*)&g_Options.AlphaColorKey, 4);
	fs.read((char*)&g_Options.OptSys, 4);
	fs.read((char*)&g_Options.SoundAPI, 4);
	fs.read((char*)&g_Options.RenderAPI, 4);

	//fs.read((char*)profile.Body, sizeof(TrophyItem) * 24);

	//SetupRes();

	//TrophyRoom.RegNumber = rn;
	std::cout << "Trophy Loaded." << std::endl;
}


void TrophySave(Profile& profile)
{
	std::stringstream fname;
	fname << "trophy" << std::setfill('0') << std::setw(2) << profile.RegNumber << ".sav";

	std::ofstream fs(fname.str(), std::ios::binary | std::ios::trunc);

	if (!fs.is_open()) {
		std::cout << "===> Error saving trophy!" << std::endl;
		return;
	}

	fs.write((char*)&profile, sizeof(Profile));

	fs.write((char*)&g_Options.Aggression, 4);
	fs.write((char*)&g_Options.Density, 4);
	fs.write((char*)&g_Options.Sensitivity, 4);
	fs.write((char*)&g_Options.Resolution, 4);
	fs.write((char*)&g_Options.Fog, 4);
	fs.write((char*)&g_Options.Textures, 4);
	fs.write((char*)&g_Options.ViewRange, 4);
	fs.write((char*)&g_Options.Shadows, 4);
	fs.write((char*)&g_Options.MouseSensitivity, 4);
	fs.write((char*)&g_Options.Brightness, 4);
	fs.write((char*)&g_Options.KeyMap, sizeof(TKeyMap));
	fs.write((char*)&g_Options.MouseInvert, 4);
	fs.write((char*)&g_Options.ScentMode, 4);
	fs.write((char*)&g_Options.CamoMode, 4);
	fs.write((char*)&g_Options.RadarMode, 4);
	fs.write((char*)&g_Options.TranqMode, 4);
	fs.write((char*)&g_Options.AlphaColorKey, 4);
	fs.write((char*)&g_Options.OptSys, 4);
	fs.write((char*)&g_Options.SoundAPI, 4);
	fs.write((char*)&g_Options.RenderAPI, 4);

	fs.write((char*)profile.Body, sizeof(TrophyItem) * 24);

	//SetupRes();

	std::cout << "Trophy Saved." << std::endl;
}


bool ReadTGAFile(const std::string& path, TargaImage& tga)
{
	std::ifstream fs(path, std::ios::binary);

	if (!fs.is_open()) {
		std::cout << "Failed to open TGA file: " << path << std::endl;
		return false;
	}

	fs.read((char*)&tga.m_Header, sizeof(TARGAINFOHEADER));

	if (tga.m_Header.tgaColorMapType) {
		std::cout << "Has a color palette: " << path << std::endl;
		return false;
	}

	if (tga.m_Header.tgaImageType != TGA_IMAGETYPE_RGB) {
		std::cout << "Not an RGB image: " << path << std::endl;
		return false;
	}

	if (tga.m_Header.tgaIdentSize) {
		fs.seekg(tga.m_Header.tgaIdentSize, std::ios::cur); // Skip Ident header
	}

	if (tga.m_Data)
		delete[] tga.m_Data;

	int size = (tga.m_Header.tgaWidth * (tga.m_Header.tgaBits / 8)) * tga.m_Header.tgaHeight;
	tga.m_Data = new uint8_t[size];
	fs.read((char*)tga.m_Data, size);

	return true;
}


void TrophyDelete(uint32_t trophy_index)
{
	std::stringstream fname;
	fname << "trophy" << std::setfill('0') << std::setw(2) << trophy_index << ".sav";
	std::filesystem::path fp = std::filesystem::current_path();
	fp /= std::filesystem::path(fname.str());
	std::cout << "Deleting trophy: \'" << fp << "\'" << std::endl;
	std::filesystem::remove(fp);
}


void Profile::New(const std::string& name)
{
	memset(Name, 0, 128);
	memcpy(Name, name.data(), name.length());
	RegNumber = g_ProfileIndex;
	Score = 100; // TODO get this from _RES
	Rank = 0;
	memset(&Last, 0, sizeof(ProfileStats));
	memset(&Total, 0, sizeof(ProfileStats));
	memset(Body, 0, sizeof(TrophyItem) * 24);
}


void Options::Default()
{
	this->Aggression = 0;
	this->Density = 0;
	this->Sensitivity = 0;
	this->Resolution = 5;
	this->Fog = true;
	this->Textures = 1;
	this->ViewRange = 10;
	this->Shadows = true;
	this->MouseSensitivity = 128;
	this->Brightness = 0;
	// -- Set default controls
	this->KeyMap.fkForward = 'W';
	this->KeyMap.fkBackward = 'S';
	this->KeyMap.fkSLeft = 'A';
	this->KeyMap.fkSRight = 'D';
	this->KeyMap.fkFire = VK_LBUTTON;
	this->KeyMap.fkShow = VK_RBUTTON;
	this->KeyMap.fkJump = VK_SPACE;
	this->KeyMap.fkCall = VK_MENU;
	this->KeyMap.fkBinoc = 'B';
	this->KeyMap.fkCrouch = 'C';
	this->KeyMap.fkRun = VK_LSHIFT;
	this->KeyMap.fkUp = VK_UP;
	this->KeyMap.fkDown = VK_DOWN;
	this->KeyMap.fkLeft = VK_LEFT;
	this->KeyMap.fkRight = VK_RIGHT;
	/*
	this->KeyMap.fkMenu		= VK_ESCAPE;
	this->KeyMap.fkMap		= VK_TAB;
	this->KeyMap.fkNightVision= 'N';
	this->KeyMap.fkReload		= 'R';
	this->KeyMap.fkZoomIn		= VK_ADD;
	this->KeyMap.fkZoomOut	= VK_SUBTRACT;
	this->KeyMap.fkConsole	= 126; // Tilde '~'
	*/
	this->MouseInvert = false;
	this->ScentMode = 0;
	this->CamoMode = 0;
	this->RadarMode = 0;
	this->TranqMode = 0;
	this->AlphaColorKey = 0;
	this->OptSys = 0;
	this->SoundAPI = 0;
	this->RenderAPI = 0;
}


Wave::Wave() :
	m_Data(nullptr),
	m_Length(0U),
	m_Frequency(22050U)
{
}


Wave::Wave(const Wave& w)
{
	if (w.m_Length && w.m_Data) {
		m_Data = new int16_t[w.m_Length];
		memcpy(m_Data, w.m_Data, w.m_Length);
	}

	m_Length = w.m_Length;
	m_Frequency = w.m_Frequency;
}


Wave::~Wave()
{
	if (m_Data)
		delete[] m_Data;
}


Picture::Picture() :
	m_Width(0),
	m_Height(0),
	m_Data(nullptr)
{}


Picture::Picture(const Picture& p)
{
	m_Data = nullptr;

	if (p.m_Width && p.m_Height && p.m_Data) {
		m_Data = new uint16_t[p.m_Width * p.m_Height];
		memcpy(m_Data, p.m_Data, (p.m_Width * 2) * p.m_Height);
	}

	m_Width = p.m_Width;
	m_Height = p.m_Height;
}


Picture::~Picture()
{
	if (m_Data)
		delete[] m_Data;
}