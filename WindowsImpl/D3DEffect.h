#pragma once
#include "Handler.h"
#include <Effect.h>
#include "D3DDevice.h"

namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	class D3DEffectReflectionObject final
	{
	private:
		Handler ref_;

	public:
		D3DEffectReflectionObject(Handler&& ref);

		void SetAsFloat(float);
		void SetAsVec2(Math::Vec2f);
		void SetAsVec4(Math::Vec4f);
		void SetAsConstantBuffer(const Graphics::Buffer* b);
		void SetAsTexture2D(const Graphics::Texture2D* tex);
		void SetAsTexture2D(nullptr_t);
	};
}

namespace Snowing::Graphics
{
	using EffectReflectionObject = EffectReflectionObjectInterface
		<Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectReflectionObject>;
}


namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	class D3DEffect;

	class D3DEffectPass final
	{
	private:
		Handler pass_;
		Handler ia_;

	public:
		D3DEffectPass(Handler&& hnd,Handler&& ia);

		void Apply(const Graphics::Context& ctx) const;
	};
}

namespace Snowing::Graphics
{
	using EffectPass = EffectPassInterface<Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectPass>;
}

namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	class D3DEffectTech final
	{
	private:
		using EffectPass = Snowing::Graphics::EffectPass;

		std::vector<Graphics::EffectPass> passes_;

		Handler createInputLayout(const Graphics::EffectDataElement* eles, int elesize,const void* btc, size_t btcsize) const;
	public:
		D3DEffectTech(Handler& hnd, const Graphics::EffectDataElement* eles, int elesize);

		EffectPass& GetPass(size_t passID);
		size_t PassCount() const;
	};
}

namespace Snowing::Graphics
{
	using EffectTech = EffectTechInterface<
		Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectTech,
		Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffect>;
}


namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	class D3DEffectGroup final
	{
	private:
		using EffectTech = Snowing::Graphics::EffectTech;
		const bool isRoot_;
		Handler group_;
	public:
		D3DEffectGroup(Handler&& handle, bool isRoot);

		EffectTech LoadTechnique(
			const char* techName,
			const Graphics::EffectDataElement* eles,
			int elesize,
			Graphics::EffectInterface<D3DEffect> * effect) const;
	};
}

namespace Snowing::Graphics
{
	using EffectGroup = EffectGroupInterface<
		Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectGroup,
		Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffect>;
}

namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	class D3DEffect final
	{
	private:
		using Group = Snowing::Graphics::EffectGroup;
		Handler effect_;
	public:
		D3DEffect(const Blob& fx);

		Group RootGroup(Graphics::EffectInterface<D3DEffect> *) const;
		Group GetGroupByName(const char* groupName, Graphics::EffectInterface<D3DEffect> *) const;

		void SetConstantBuffer(const char* name, const Graphics::Buffer& b);
		void SetConstantBuffer(int id, const Graphics::Buffer& b);

		void SetTexture(const char* name, const Graphics::Texture2D& tex);

		Graphics::EffectReflectionObject Reflection(const char* name) const;
	};
}

namespace Snowing::Graphics
{
	using Effect = EffectInterface<Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffect>;
}