#pragma once
#include <cassert>

namespace Snowing
{
	template <typename TImpl>
	class LibraryInterface final
	{
	private:
		TImpl impl_;

	public:
		LibraryInterface(const char *libName) :
			impl_{ libName }
		{}


		template <typename TResult,typename... TArgs>
		auto Get(const char *funcName) const
		{
			using FuncPtr = TResult(*)(TArgs...);
			auto p = static_cast<FuncPtr>(impl_.Get(funcName));
			assert(p);
			return p;
		}
	};
}
