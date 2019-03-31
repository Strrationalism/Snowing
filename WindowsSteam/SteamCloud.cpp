#include "stdafx.h"
#include "SteamCloud.h"
#include "Steam.h"

using namespace Snowing::PlatformImpls::WindowsSteam;

#define API "SteamAPI_ISteamRemoteStorage_"

void Snowing::PlatformImpls::WindowsSteam::Cloud::Write(const char* fileName, const Snowing::Blob& blob)
{
	const static auto steamWrite = Steam::Get().GetDLL().Get<bool,const char*, const void*, int32_t>(API"FileWrite");
	if (!steamWrite(fileName, blob.Get<void*>(), blob.Size()))
		throw CanNotWriteFile{};
}

Snowing::Blob Snowing::PlatformImpls::WindowsSteam::Cloud::Read(const char* fileName)
{
	const static auto steamRead = Steam::Get().GetDLL().Get<int32_t, const char*, void*, int32_t>(API"FileRead");
	auto size = GetFileSize(fileName);
	Snowing::Blob blob{ size };

	auto readSize = steamRead(fileName, blob.Get<void*>(), size);
	if (size != readSize)
		throw CanNotReadFile{};

	return blob;
}

void Snowing::PlatformImpls::WindowsSteam::Cloud::Delete(const char* fileName)
{
	const static auto steamDel = Steam::Get().GetDLL().Get<bool, const char*>(API"FileDelete");
	steamDel(fileName);
}

bool Snowing::PlatformImpls::WindowsSteam::Cloud::Exists(const char* fileName)
{
	const static auto steamExists = Steam::Get().GetDLL().Get<bool, const char*>(API"FileExists");
	return steamExists(fileName);
}

size_t Snowing::PlatformImpls::WindowsSteam::Cloud::GetFileSize(const char* fileName)
{
	const static auto steamGetSize = Steam::Get().GetDLL().Get<int32_t, const char*>(API"GetFileSize");
	return steamGetSize(fileName);
}

size_t Snowing::PlatformImpls::WindowsSteam::Cloud::GetFileCount()
{
	const static auto steamGetCount = Steam::Get().GetDLL().Get<int32_t>(API"GetFileCount");
	return steamGetCount();
}

std::pair<const char*, size_t> Snowing::PlatformImpls::WindowsSteam::Cloud::GetFileNameAndSize(size_t index)
{
	const static auto steamGet = Steam::Get().GetDLL().Get<const char*,int,int32_t*>(API"GetFileNameAndSize");
	int32_t size = 0;
	const char* name = steamGet(index, &size);
	return { name,static_cast<size_t>(size) };
}

