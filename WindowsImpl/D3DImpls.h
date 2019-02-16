#pragma once
#include <FontRenderer.h>
#include <PostEffect.h>
#include <Blob.h>
#include "D3DTexture2D.h"
#include "D3DBuffer.h"
#include "D3DEffect.h"
#include <TextMenuItem.h>

namespace Snowing::Graphics
{
	template <typename TSprite = Sprite, typename TSpriteSheet = SpriteSheet, size_t BufferSize = 8>
	using SpriteRenderer = SpriteRendererInterface<
		PlatformImpls::WindowsImpl::D3D::Context,
		PlatformImpls::WindowsImpl::D3D::D3DEffect,
		PlatformImpls::WindowsImpl::D3D::D3DEffectTech,
		PlatformImpls::WindowsImpl::D3D::D3DEffectPass,
		PlatformImpls::WindowsImpl::D3D::D3DEffectReflectionObject,
		PlatformImpls::WindowsImpl::D3D::D3DTexture2D,
		PlatformImpls::WindowsImpl::D3D::Buffer,
		TSprite,
		TSpriteSheet,
		BufferSize>;

	using Font = FontInterface<SpriteSheet, Texture2D,Buffer>;
	Font LoadFont(const Blob& b);

	template <
		typename TSprite = Sprite,
		typename TSpriteSheet = SpriteSheet,
		size_t SpriteBufferSize = 8>
	using FontRenderer = FontRendererInterface<
		PlatformImpls::WindowsImpl::D3D::Context,
		PlatformImpls::WindowsImpl::D3D::D3DEffect,
		PlatformImpls::WindowsImpl::D3D::D3DEffectTech,
		PlatformImpls::WindowsImpl::D3D::D3DEffectPass,
		PlatformImpls::WindowsImpl::D3D::D3DEffectReflectionObject,
		PlatformImpls::WindowsImpl::D3D::D3DTexture2D,
		PlatformImpls::WindowsImpl::D3D::Buffer,
		Font,
		TSprite,
		TSpriteSheet,
		SpriteBufferSize>;

	using PostEffect = PostEffectInterface <
		PlatformImpls::WindowsImpl::D3D::Buffer,
		PlatformImpls::WindowsImpl::D3D::D3DEffectTech,
		PlatformImpls::WindowsImpl::D3D::Context> ;
}

namespace Snowing::Scene
{
	using TextMenuItem = TextMenuItemInterface<
		Graphics::FontRenderer<>>;
}