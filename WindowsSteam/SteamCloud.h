#pragma once
#include <Blob.h>
#include <MacroException.h>

namespace Snowing::PlatformImpls::WindowsSteam::Cloud
{
	
	EXCEPTION(CanNotReadFile);
	EXCEPTION(CanNotWriteFile);

	void Write(const char* fileName, const Snowing::Blob& blob);
	Snowing::Blob Read(const char* fileName);
	void Delete(const char* fileName);
	bool Exists(const char* fileName);
	size_t GetFileSize(const char* fileName);

	size_t GetFileCount();
	std::pair<const char*, size_t> GetFileNameAndSize(size_t index);
}

#undef EXCEPTION
