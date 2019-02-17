#pragma once
#include <cstdint>
#include <SingleInstance.h>
#include <LibraryImpl.h>

#ifdef X64
#pragma comment(lib,"DbgHelp.lib")
#endif

namespace Snowing::PlatformImpls::WindowsSteam
{
	class Steam final : public SingleInstance<Steam>
	{
	private:
		const std::uint32_t appid_;
		Snowing::Library lib_;
	public:
		Steam();
		~Steam();

		Steam(const Steam&) = delete;
		Steam(Steam&&) = delete;
		Steam& operator=(const Steam&) = delete;
		Steam& operator=(Steam&&) = delete;
	};
}
