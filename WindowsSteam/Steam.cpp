#include "stdafx.h"
#include "Steam.h"
#include <fstream>

using namespace Snowing::PlatformImpls::WindowsSteam;

static void (*SteamAPI_WriteMiniDump)(uint32_t uStructuredExceptionCode, void* pvExceptionInfo, uint32_t uBuildID) = nullptr;
static void (*SteamAPI_SetMiniDumpComment)(const char *pchMsg) = nullptr;

void MiniDumpFunction(unsigned int nExceptionCode, EXCEPTION_POINTERS *pException)
{
	SteamAPI_SetMiniDumpComment("BUILD TIME:" __DATE__);
	SteamAPI_WriteMiniDump(nExceptionCode, pException, 0);
}

#ifdef X64
constexpr auto DLLFile = "steam_api64.dll";
constexpr std::uint64_t DLLHash = 0xc6b8392872b5d17d;
#else
constexpr auto DLLFile = "steam_api.dll";
constexpr std::uint64_t DLLHash = 0x406aa22538821819;
#endif // WIN32

Snowing::PlatformImpls::WindowsSteam::Steam::Steam():
	lib_{ 
		std::invoke([] {
			const auto steamDll = Snowing::PlatformImpls::WindowsImpl::ReadFile(DLLFile);
			const auto dllHash = Snowing::BKDRHash{ steamDll.Get<char*>(),steamDll.Size() };

			if (dllHash != DLLHash) std::abort();
			return DLLFile;
		}) },
	appid_{
		std::invoke([] {
			std::ifstream in("appid.txt");
			std::uint32_t appID;
			in >> appID;
			return appID;
	})}
{
	SteamAPI_WriteMiniDump = lib_.Get<void, uint32_t, void*, uint32_t>("SteamAPI_WriteMiniDump");
	SteamAPI_SetMiniDumpComment = lib_.Get<void,const char *>("SteamAPI_SetMiniDumpComment");

	if (!IsDebuggerPresent())
		_set_se_translator(MiniDumpFunction);

	if (lib_.Get<bool, std::uint32_t>("SteamAPI_RestartAppIfNecessary")(appid_))
		std::exit(0);
	if (!lib_.Get<bool>("SteamAPI_Init")())
		std::abort();
}

Snowing::PlatformImpls::WindowsSteam::Steam::~Steam()
{
	lib_.Get<void>("SteamAPI_Shutdown")();

	SteamAPI_WriteMiniDump = nullptr;
	SteamAPI_SetMiniDumpComment = nullptr;
}

