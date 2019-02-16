#pragma once
#include "Blob.h"

namespace Snowing::PlatformImpls::WindowsImpl
{
	[[nodiscard]]
	Blob ReadFile(const char* fileName);

	[[nodiscard]]
	Blob ReadFile(const wchar_t* fileName);
}
