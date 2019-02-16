#pragma once
#include "Handler.h"
#include <Library.h>
namespace Snowing::PlatformImpls::WindowsImpl
{
	class LibraryImpl final
	{
	private:
		Handler lib_;

	public:
		LibraryImpl(const char* libName);
		void *Get(const char* funcName) const;
	};
}

namespace Snowing
{
	using Library = LibraryInterface<Snowing::PlatformImpls::WindowsImpl::LibraryImpl>;
}