#pragma once
#include "D3DEffect.h"
#include "D3DContext.h"
#include "InputImpl.h"
#include "WindowImpl.h"
#include "Platforms.h"
#include "BKDRHash.h"
#include "Blob.h"
#include "D3DDevice.h"
#include "D3DBuffer.h"
#include "AssetLoader.h"
#include "XADevice.h"
#include "XASoundPlayer.h"
#include "D3DImpls.h"
#include <string>
#include "Engine.h"
#include "LibraryImpl.h"


namespace Snowing
{
	using Engine = EngineInterface<
		PlatformImpls::WindowsImpl::WindowImpl,
		PlatformImpls::WindowsImpl::D3D::Device,
		PlatformImpls::WindowsImpl::InputImpl>;

#ifndef PUBLISH_MODE
	template <typename TChar>
	[[nodiscard]]
	inline Blob LoadAsset(const TChar* name)
	{
		return Snowing::PlatformImpls::WindowsImpl::ReadFile(name);
	}
#endif
}

namespace Snowing::PlatformImpls
{
	constexpr auto Platform = Platforms::EngineImplName::Windows;

	void Log(const wchar_t *log);

	void Log(const char *log);

	void Abort(const char *log);

	void Abort(const wchar_t *log);

	namespace WindowsImpl
	{
		[[nodiscard]]
		inline auto MakeEngine(const wchar_t* title, Math::Vec2<int> size, bool windowed)
		{
			WindowImpl win{ title, size };
			D3D::Device d3d{ WindowImpl::Get().GetHWND(),windowed };
			Engine eng;
			return std::tuple{ std::move(win), std::move(d3d), std::move(eng) };
		};
	}
}

