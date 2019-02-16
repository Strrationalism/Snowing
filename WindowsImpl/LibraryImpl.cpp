#include "stdafx.h"
#include "LibraryImpl.h"

Snowing::PlatformImpls::WindowsImpl::LibraryImpl::LibraryImpl(const char * libName):
	lib_{ LoadLibraryA(libName),[](void *p) { FreeLibrary(static_cast<HMODULE>(p)); } }
{
	static_assert(sizeof(HMODULE) == sizeof(void*));
}

void * Snowing::PlatformImpls::WindowsImpl::LibraryImpl::Get(const char * funcName) const
{
	return GetProcAddress(
		lib_.Get<HMODULE>(),
		funcName);
}
