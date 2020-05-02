/***************************************************
* AtmosFear 2.1
* Characters.cpp
*
* Resources, Files and Memory Management
*
*/

#include "Hunt.h"

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>


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
			TWeapInfo Blank;
			g_WeapInfo.push_back( Blank );

			while (fgets( line, 255, stream))
			{
				int CurW = g_WeapInfo.size()-1;

				if (strstr(line, "}")) { break; }
				value = strstr(line, "=");
				if (!value) throw std::runtime_error("Script loading error");
				value++;

				if (strstr(line, "power"))  g_WeapInfo[CurW].Power = (float)atof(value);
				if (strstr(line, "prec"))   g_WeapInfo[CurW].Prec  = (float)atof(value);
				if (strstr(line, "loud"))   g_WeapInfo[CurW].Loud  = (float)atof(value);
				if (strstr(line, "rate"))   g_WeapInfo[CurW].Rate  = (float)atof(value);
				if (strstr(line, "shots"))  g_WeapInfo[CurW].Shots =        atoi(value);
				if (strstr(line, "reload")) g_WeapInfo[CurW].Reload=        atoi(value);
				if (strstr(line, "trace"))  g_WeapInfo[CurW].TraceC=        atoi(value)-1;
				if (strstr(line, "optic"))  g_WeapInfo[CurW].Optic = (float)atof(value);
				if (strstr(line, "fall"))   g_WeapInfo[CurW].Fall  =        atoi(value);
				if (strstr(line, "price"))	g_WeapInfo[CurW].Price =        atoi(value);

				if (strstr(line, "name")) {
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					strcpy(g_WeapInfo[CurW].Name, &value[1]); }

				if (strstr(line, "file")) {
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					strcpy(g_WeapInfo[CurW].FName, &value[1]);}

				if (strstr(line, "pic")) {
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					strcpy(g_WeapInfo[CurW].BFName, &value[1]);}
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
			TDinoInfo Blank;
			g_DinoInfo.push_back( Blank );

			while (fgets( line, 255, stream))
			{
				int CurC = g_DinoInfo.size()-1;

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

				if (strstr(line, "mass"     )) g_DinoInfo[CurC].Mass      = (float)atof(value);
				if (strstr(line, "length"   )) g_DinoInfo[CurC].Length    = (float)atof(value);
				if (strstr(line, "radius"   )) g_DinoInfo[CurC].Radius    = (float)atof(value);
				if (strstr(line, "health"   )) g_DinoInfo[CurC].Health0   = atoi(value);
				if (strstr(line, "basescore")) g_DinoInfo[CurC].BaseScore = atoi(value);
				if (strstr(line, "ai"       )) g_DinoInfo[CurC].AI        = atoi(value);
				if (strstr(line, "smell"    )) g_DinoInfo[CurC].SmellK    = (float)atof(value);
				if (strstr(line, "hear"     )) g_DinoInfo[CurC].HearK     = (float)atof(value);
				if (strstr(line, "look"     )) g_DinoInfo[CurC].LookK     = (float)atof(value);
				// -> Safety Check
				if (strstr(line, "smellk"   )) g_DinoInfo[CurC].SmellK    = (float)atof(value);
				if (strstr(line, "heark"    )) g_DinoInfo[CurC].HearK     = (float)atof(value);
				if (strstr(line, "lookk"    )) g_DinoInfo[CurC].LookK     = (float)atof(value);
				// <- End
				if (strstr(line, "shipdelta")) g_DinoInfo[CurC].ShDelta   = (float)atof(value);
				if (strstr(line, "scale0"   )) g_DinoInfo[CurC].Scale0    = atoi(value);
				if (strstr(line, "scaleA"   )) g_DinoInfo[CurC].ScaleA    = atoi(value);
				if (strstr(line, "danger"   )) g_DinoInfo[CurC].DangerCall= true;

				if (strstr(line, "name"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					strcpy(g_DinoInfo[CurC].Name, &value[1]);
				}

				if (strstr(line, "file"))
				{
					/*int inc = atoi(line);
					char error[256];
					sprintf(error, "The file value is integer %d", inc);
					MessageBox(hwndMain, error, "Integer", MB_OK);*/
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					strcpy(g_DinoInfo[CurC].FName, &value[1]);
				}

				if (strstr(line, "pic"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					strcpy(g_DinoInfo[CurC].PName, &value[1]);
				}
			}
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
			TAreaInfo Blank;
			g_AreaInfo.push_back( Blank );

			while (fgets( line, 255, stream))
			{
				int CurC = g_AreaInfo.size()-1;

				if (strstr(line, "}"))
				{
					break;
				}

				value = strstr(line, "=");
				if (!value)
					throw std::runtime_error("Script loading error");
				value++;

				if (strstr(line, "cost"   ))	g_AreaInfo[CurC].Cost = atoi(value);
				if (strstr(line, "rank"))		g_AreaInfo[CurC].Rank = atoi(value);

				if (strstr(line, "name"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					strcpy(g_AreaInfo[CurC].Name, &value[1]);
				}

				if (strstr(line, "map"))
				{
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					strcpy(g_AreaInfo[CurC].MapFile, &value[1]);
				}

				if (strstr(line, "rsc"))
				{
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					strcpy(g_AreaInfo[CurC].RscFile, &value[1]);
				}

				if (strstr(line, "thumbnail"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value)-2] = 0;
					///TODO: Load TPicture
					//strcpy(g_AreaInfo[CurC].Thumbnail, &value[1]);
				}
			}
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

void LoadTrophy(TProfile& profile, int pr)
{
	//PlayerProfile.RegNumber
	memset(&profile, 0, sizeof(TProfile));
	profile.RegNumber = pr;

	std::stringstream fname;
	fname << "trophy" << std::setfill('0') << std::setw(2) << profile.RegNumber << ".sav";

	std::ifstream fs(fname.str(), std::ios::binary);

	if (!fs.is_open()) {
		std::cout << "===> Error loading trophy!" << std::endl;
		return;
	}

	fs.read((char*)&profile, sizeof(TProfile));

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

	//SetupRes();

	//TrophyRoom.RegNumber = rn;
	std::cout << "Trophy Loaded." << std::endl;
}


void SaveTrophy(TProfile& profile)
{
	std::stringstream fname;
	fname << "trophy" << std::setfill('0') << std::setw(2) << profile.RegNumber << ".sav";

	std::ofstream fs(fname.str(), std::ios::binary | std::ios::trunc);

	if (!fs.is_open()) {
		std::cout << "===> Error saving trophy!" << std::endl;
		return;
	}

	fs.write((char*)&profile, sizeof(TProfile));

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

	//SetupRes();

	std::cout << "Trophy Saved." << std::endl;
}

