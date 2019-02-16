#pragma once
#include <exception>
#include <cstdint>

namespace Snowing::PlatformImpls::WindowsImpl::COMHelper
{

#include "MacroException.h"
	EXCEPTION(COMException)
#undef EXCEPTION

	void COMIUnknownDeleter(void*);
	void AssertHResult(const char*,std::uint32_t);
}