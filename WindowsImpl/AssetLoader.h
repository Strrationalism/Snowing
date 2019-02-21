#pragma once
#include "Blob.h"

namespace Snowing::PlatformImpls::WindowsImpl
{
	[[nodiscard]]
	Blob ReadFile(const char* fileName,size_t alignment = 0);

	[[nodiscard]]
	Blob ReadFile(const wchar_t* fileName,size_t alignment = 0);
}
