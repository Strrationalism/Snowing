#include "stdafx.h"
#include "Steam.h"
#include <fstream>
#include <Snowing.h>
#include <PlatformImpls.h>

using namespace Snowing::PlatformImpls::WindowsSteam;

static void WriteMiniDumpX64(uint32_t uStructuredExceptionCode, void* pvExceptionInfo, uint32_t uBuildID)
{
}

static void SetMiniDumpCommentX64(const char* pchMsg)
{
	Snowing::Abort("Unhandled Exception:", pchMsg);
}

static void (*WriteMiniDump)(uint32_t uStructuredExceptionCode, void* pvExceptionInfo, uint32_t uBuildID) = &WriteMiniDumpX64;
static void (*SetMiniDumpComment)(const char *pchMsg) = &SetMiniDumpCommentX64;

static LONG WINAPI UnhandledException(EXCEPTION_POINTERS *pExp)
{
	SetMiniDumpComment("BUILD TIME:" __DATE__ "\tUnhandledExceptionFilter");
	WriteMiniDump(0, pExp, 0);
	return EXCEPTION_EXECUTE_HANDLER;
}


static void MiniDumpFunction(unsigned int nExceptionCode, EXCEPTION_POINTERS *pException)
{
	SetMiniDumpComment("BUILD TIME:" __DATE__ "\tSEH");
	WriteMiniDump(nExceptionCode, pException, 0);
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
#ifdef WIN32
		WriteMiniDump = lib_.Get<void, uint32_t, void*, uint32_t>("SteamAPI_WriteMiniDump");
		SetMiniDumpComment = lib_.Get<void, const char *>("SteamAPI_SetMiniDumpComment");
#endif

	if (!IsDebuggerPresent())
	{
		_set_se_translator(MiniDumpFunction);
		SetUnhandledExceptionFilter(UnhandledException);
	}

	if (lib_.Get<bool, std::uint32_t>("SteamAPI_RestartAppIfNecessary")(appid_))
		std::exit(0);
	if (!lib_.Get<bool>("SteamAPI_Init")())
		std::abort();
}

Snowing::PlatformImpls::WindowsSteam::Steam::~Steam()
{
	lib_.Get<void>("SteamAPI_Shutdown")();

	WriteMiniDump = &WriteMiniDumpX64;
	SetMiniDumpComment = &SetMiniDumpCommentX64;
}

