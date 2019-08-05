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
#include "Impls.h"
#include <string>
#include "Engine.h"
#include "LibraryImpl.h"
#include <filesystem>

namespace Snowing
{

#ifndef PUBLISH_MODE

	using AssetName = const char*;

	[[nodiscard]]
	inline Blob LoadAsset(AssetName name)
	{
		return Snowing::PlatformImpls::WindowsImpl::ReadFile(name);
	}

	inline void CallAssetBuilder()
	{
		constexpr char path[] = "../AssetBuilderRelease/AssetBuilder.exe";
		constexpr char cmd[] = 
			"start /wait /b "
			"/d ..\\..\\Assets "
			"../AssetBuilderRelease/AssetBuilder.exe "
			"../build/data";
		if (std::filesystem::exists(path))
			system(cmd);
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
			Engine eng{ Engine::ConstructToken{} };
			return std::tuple{ std::move(win), std::move(d3d), std::move(eng) };
		};
	}
}

