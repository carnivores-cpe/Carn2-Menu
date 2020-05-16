#include <windows.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define req_versionH 0x0001
#define req_versionL 0x0002

HINSTANCE hAudioDLL = nullptr;
HANDLE hAudioLog = nullptr;
int iAudioDriver = 0;

typedef void (WINAPI * LPFUNC1)(void);
typedef void (WINAPI * LPFUNC2)(HWND, HANDLE);
typedef void (WINAPI * LPFUNC3)(float, float, float, float, float);
typedef void (WINAPI * LPFUNC4)(int, short int*, int);
typedef void (WINAPI * LPFUNC5)(int, short int*, float, float, float);
typedef void (WINAPI * LPFUNC6)(int, short int*, float, float, float, int);

typedef int  (WINAPI * LPFUNC7)(void);
typedef void (WINAPI * LPFUNC8)(int, float);

LPFUNC1 audio_restore = nullptr;
LPFUNC1 audiostop = nullptr;
LPFUNC1 audio_shutdown = nullptr;

LPFUNC2 initaudiosystem = nullptr;
LPFUNC3 audiosetcamerapos = nullptr;
LPFUNC4 setambient = nullptr;
LPFUNC5 setambient3d = nullptr;
LPFUNC6 addvoice3dv = nullptr;
LPFUNC7 audio_getversion = nullptr;
LPFUNC8 audio_setenvironment = nullptr;


void PrintLog(const std::string &l)
{
	DWORD w;

	if (l.at(l.size() - 1) == 0x0A) {
		uint8_t b = 0x0D;
		WriteFile(hAudioLog, l.c_str(), l.size() - 1, &w, NULL);
		WriteFile(hAudioLog, &b, 1, &w, NULL);
		b = 0x0A;
		WriteFile(hAudioLog, &b, 1, &w, NULL);
	}
	else
		WriteFile(hAudioLog, l.c_str(), l.size(), &w, NULL);

}


void Audio_Shutdown()
{
	if (audio_shutdown) audio_shutdown();
	if (hAudioDLL) FreeLibrary(hAudioDLL);
	if (hAudioLog) CloseHandle(hAudioLog);
	hAudioDLL = nullptr;

	audio_restore = nullptr;
	audiostop = nullptr;
	audio_shutdown = nullptr;
	initaudiosystem = nullptr;
	audiosetcamerapos = nullptr;
	setambient = nullptr;
	setambient3d = nullptr;
	addvoice3dv = nullptr;
	audio_getversion = nullptr;
	audio_setenvironment = nullptr;
}


void InitAudioSystem(HWND hw, HANDLE hlog, int  driver)
{
	Audio_Shutdown();

	iAudioDriver = driver;

	switch (driver)
	{
	case 0:
		hAudioDLL = LoadLibrary("a_soft.dll");
		if (!hAudioDLL) throw std::runtime_error("Can't load A_SOFT.DLL");
		break;
	case 1:
		hAudioDLL = LoadLibrary("a_ds3d.dll");
		if (!hAudioDLL) throw std::runtime_error("Can't load A_DS3D.DLL");
		break;
	case 2:
		hAudioDLL = LoadLibrary("a_a3d.dll");
		if (!hAudioDLL) throw std::runtime_error("Can't load A_A3D.DLL");
		break;
	case 3:
		hAudioDLL = LoadLibrary("a_eax.dll");
		if (!hAudioDLL) throw std::runtime_error("Can't load A_EAX.DLL");
		break;
	case 4:
		hAudioDLL = LoadLibrary("a_oal.dll");
		if (!hAudioDLL) throw std::runtime_error("Can't load A_OAL.DLL");
		break;
	}


	initaudiosystem   = (LPFUNC2) GetProcAddress(hAudioDLL, "InitAudioSystem");
	if (!initaudiosystem) throw std::runtime_error("Can't find procedure address.");

	audio_restore     = (LPFUNC1) GetProcAddress(hAudioDLL, "Audio_Restore");
	if (!audio_restore) throw std::runtime_error("Can't find procedure address.");

	audiostop         = (LPFUNC1) GetProcAddress(hAudioDLL, "AudioStop");
	if (!audiostop)   throw std::runtime_error("Can't find procedure address.");

	audio_shutdown    = (LPFUNC1) GetProcAddress(hAudioDLL, "Audio_Shutdown");
	if (!audio_shutdown) throw std::runtime_error("Can't find procedure address.");

	audiosetcamerapos = (LPFUNC3) GetProcAddress(hAudioDLL, "AudioSetCameraPos");
	if (!audiosetcamerapos) throw std::runtime_error("Can't find procedure address.");

	setambient        = (LPFUNC4) GetProcAddress(hAudioDLL, "SetAmbient");
	if (!setambient) throw std::runtime_error("Can't find procedure address.");

	setambient3d      = (LPFUNC5) GetProcAddress(hAudioDLL, "SetAmbient3d");
	if (!setambient3d) throw std::runtime_error("Can't find procedure address.");

	addvoice3dv       = (LPFUNC6) GetProcAddress(hAudioDLL, "AddVoice3dv");
	if (!addvoice3dv) throw std::runtime_error("Can't find procedure address.");

	audio_getversion  = (LPFUNC7) GetProcAddress(hAudioDLL, "Audio_GetVersion");
	if (!audio_getversion) throw std::runtime_error("Can't find procedure address.");

	audio_setenvironment = (LPFUNC8) GetProcAddress(hAudioDLL, "Audio_SetEnvironment");
	if (!audio_setenvironment) throw std::runtime_error("Can't find procedure address.");

	int v1 = audio_getversion()>>16;
	int v2 = audio_getversion() & 0xFFFF;
	if ( (v1!=req_versionH) || (v2<req_versionL) )
		throw std::runtime_error("Incorrect audio driver version.");

	if (hlog == NULL) {
		hAudioLog = CreateFile("audio.log",
			GENERIC_WRITE,
			FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		hlog = hAudioLog;

		if (driver == 0)
			PrintLog("CarnivoresII Software audio driver.\n");
		if (driver == 1)
			PrintLog("CarnivoresII DirectSound 3D audio driver.\n");
		if (driver == 2)
			PrintLog("CarnivoresII Aureal 3D audio driver.\n");
		if (driver == 3)
			PrintLog("CarnivoresII EAX audio driver.\n");
		if (driver == 4)
			PrintLog("CarnivoresII OpenAL audio driver.\n");

		std::stringstream ss;
		ss << " Version " << (audio_getversion() >> 16) << "." << (audio_getversion() & 0xFFFF) << ". Sep.24 1999.\n";
		PrintLog(ss.str());
	}

	initaudiosystem(hw, hlog);
}


void AudioStop()
{
    if(audiostop)
		audiostop();
}

void Audio_Restore()
{
	if (audio_restore)
  	  audio_restore();
}



void AudioSetCameraPos(float cx, float cy, float cz, float ca, float cb)
{
	if (audiosetcamerapos)
		audiosetcamerapos(cx, cy, cz, ca, cb);
}


void Audio_SetEnvironment(int e, float f)
{
	if (audio_setenvironment)
	   audio_setenvironment(e, f);
}

void SetAmbient(int length, short int* lpdata, int av)
{
	if (setambient)
		setambient(length, lpdata, av);
}


void SetAmbient3d(int length, short int* lpdata, float cx, float cy, float cz)
{
	if (setambient3d)
		setambient3d(length, lpdata, cx, cy, cz);
}


void AddVoice3dv(int length, short int* lpdata, float cx, float cy, float cz, int vol)
{
	if (addvoice3dv)
		addvoice3dv(length, lpdata, cx, cy, cz, vol);
}




void AddVoice3d(int length, short int* lpdata, float cx, float cy, float cz)
{
   AddVoice3dv(length, lpdata, cx, cy, cz, 256);
}


void AddVoicev(int length, short int* lpdata, int v)
{
   AddVoice3dv(length, lpdata, 0,0,0, v);
}


void AddVoice(int length, short int* lpdata)
{
   AddVoice3dv(length, lpdata, 0,0,0, 256);
}
