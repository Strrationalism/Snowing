#pragma once
#include "Handler.h"
#include "Texture2D.h"
#include "GraphicBuffer.h"
#include <Blob.h>
#include <optional>

namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	enum class TextureFormat : std::uint8_t
	{
		R8G8B8A8_UNORM = 0,
		R8_UNORM = 1,
		DDS = 2
	};

	size_t FormatPixleSize(TextureFormat f);

	class D3DTexture2D final
	{
	private:
		using CPUAccessFlag = Graphics::BufferCPUAccessFlag;
		using BufferUsage = Graphics::BufferUsage;
		mutable Handler shaderRes_;
		Handler tex_;
		

	public:
		D3DTexture2D(
			TextureFormat,
			Math::Vec2<std::uint16_t> size,
			Graphics::BufferBindMode mode,
			void* pixels = nullptr,
			BufferUsage usage = BufferUsage::Immutable,
			CPUAccessFlag cpuAccessFlag = CPUAccessFlag::NoAccess);

		D3DTexture2D(Handler&& hnd);

		D3DTexture2D& operator = (D3DTexture2D&&) = delete;
		D3DTexture2D(D3DTexture2D&&) = default;
		Math::Vec2<size_t> Size() const;

		const Handler& GetHandler() const;
		const Handler& ShaderResource() const;
	};

	constexpr size_t MaxSpriteRects = 512;
}

namespace Snowing::Graphics
{
	constexpr size_t MaxSpriteRects = Snowing::PlatformImpls::WindowsImpl::D3D::MaxSpriteRects;
	struct SpriteSheet
	{
		Math::Vec4f Sheet[MaxSpriteRects];
	};

	using Texture2D = Texture2DInterface<Snowing::PlatformImpls::WindowsImpl::D3D::D3DTexture2D>;
	Texture2D LoadSpriteAndSpriteSheet(const Snowing::Blob& b,SpriteSheet& sheet);
	Texture2D LoadTexture(const Snowing::Blob& b);
}
