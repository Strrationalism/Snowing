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

		template <typename TFuncPtrType>
		auto GetCast(const char* funcName) const
		{
			auto p = static_cast<TFuncPtrType>(impl_.Get(funcName));
			assert(p);
			return p;
		}

		template <typename TResult,typename... TArgs>
		auto Get(const char *funcName) const
		{
			using FuncPtr = TResult(*)(TArgs...);
			return GetCast<FuncPtr>(funcName);
		}
	};
}
