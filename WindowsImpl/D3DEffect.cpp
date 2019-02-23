#include "stdafx.h"
#include "D3DEffect.h"
#include "D3DBuffer.h"
#include "COMHelper.h"
#include <d3dx11effect.h>

using namespace Snowing::PlatformImpls::WindowsImpl::D3D;

Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffect::D3DEffect(const Blob& effBlob)
{
	auto device = Device::Get().GetHandler().Cast<IUnknown*,ID3D11Device*>();

	ID3DX11Effect* effect;
	COMHelper::AssertHResult(
		"Can not load effect.",
		D3DX11CreateEffectFromMemory(effBlob.Get<void*>(), effBlob.Size(), 0, device, &effect));
	effect_ = { static_cast<IUnknown*>(effect),COMHelper::COMIUnknownDeleter };
}

D3DEffect::Group Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffect::RootGroup(Graphics::EffectInterface<D3DEffect> * effectInterface) const
{
	const auto h = static_cast<ID3DX11Effect*>(effect_.Get<IUnknown*>());
	return Group{ {Handler{h,Handler::DoNothingDeleter},true},effectInterface };
}

D3DEffect::Group Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffect::GetGroupByName(const char * groupName, Graphics::EffectInterface<D3DEffect> * effectInterface) const
{
	const auto h = static_cast<ID3DX11Effect*>(effect_.Get<IUnknown*>());
	const auto g = static_cast<IUnknown*>(h->GetGroupByName(groupName));
	if (g == nullptr)
		throw std::invalid_argument{ "Can not find fx group." };
	return Group{ {Handler{g,COMHelper::COMIUnknownDeleter},false},effectInterface };
}

void Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffect::SetConstantBuffer(const char* name,const Graphics::Buffer & b)
{
	auto buf = static_cast<ID3D11Buffer*>(b.GetImpl().GetHandler().Get<IUnknown*>());
	auto fx = static_cast<ID3DX11Effect*>(effect_.Get<IUnknown*>());
	auto p = fx->GetConstantBufferByName(name);
	COMHelper::AssertHResult("Can not set constant buffer for effect.",
		p->SetConstantBuffer(buf));
	p->Release();
}

void Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffect::SetConstantBuffer(int id, const Graphics::Buffer & b)
{
	auto buf = static_cast<ID3D11Buffer*>(b.GetImpl().GetHandler().Get<IUnknown*>());
	auto fx = static_cast<ID3DX11Effect*>(effect_.Get<IUnknown*>());
	auto p = fx->GetConstantBufferByIndex(id);
	COMHelper::AssertHResult("Can not set constant buffer for effect.",
		p->SetConstantBuffer(buf));
	p->Release();
}

void Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffect::SetTexture(const char* name, const Graphics::Texture2D & tex)
{
	auto t = tex.GetImpl().ShaderResource().Cast<IUnknown*, ID3D11ShaderResourceView*>();
	auto fx = static_cast<ID3DX11Effect*>(effect_.Get<IUnknown*>());
	auto p = fx->GetVariableByName(name);
	auto sr = p->AsShaderResource();
	COMHelper::AssertHResult("Can not set texture buffer for effect.",
		sr->SetResource(t));
	sr->Release();
	p->Release();
}

Snowing::Graphics::EffectReflectionObject Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffect::Reflection(const char * name) const
{
	auto p = effect_.Cast<IUnknown*, ID3DX11Effect*>()->GetVariableByName(name);
	if (!p->IsValid())
	{
		p->Release();
		p = effect_.Cast<IUnknown*, ID3DX11Effect*>()->GetConstantBufferByName(name);
	}
	if (!p->IsValid())
	{
		p->Release();
		throw std::exception{ "Invalid reflection!" };
	}

	return D3DEffectReflectionObject{ Handler { static_cast<IUnknown*>(p),COMHelper::COMIUnknownDeleter } };
}


Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectGroup::D3DEffectGroup(Handler && handle, bool isRoot):
	isRoot_{ isRoot },
	group_{ std::move(handle) }
{
}

Snowing::Graphics::EffectTech Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectGroup::LoadTechnique(const char * techName, const Graphics::EffectDataElement * eles, int elesize, Graphics::EffectInterface<D3DEffect> * effect) const
{
	IUnknown* t = nullptr;;
	if (isRoot_)
		t = group_.Get<ID3DX11Effect*>()->GetTechniqueByName(techName);
	else
		t = static_cast<ID3DX11EffectGroup*>(group_.Get<IUnknown*>())->GetTechniqueByName(techName);
	if (t == nullptr)
		throw std::invalid_argument{ "Can not find fx tech." };
	return { D3D::D3DEffectTech{Handler{t,COMHelper::COMIUnknownDeleter},eles,elesize},effect };
}

Snowing::PlatformImpls::WindowsImpl::Handler Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectTech::createInputLayout(const Graphics::EffectDataElement * eles, int elesize,const void* btc,size_t btcSize) const
{
	constexpr int elespace[] =
	{
		2*sizeof(float),
		4*sizeof(float),
		2*sizeof(float),
		sizeof(uint32_t),
		2 * sizeof(float),
		2 * sizeof(float),
		sizeof(float),
	};

	const D3D11_INPUT_ELEMENT_DESC map[] =
	{ 
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SPRITEIMAGEINDEX", 0, DXGI_FORMAT_R32_UINT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"CENTER", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"ROTATING", 0, DXGI_FORMAT_R32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> elems{ (size_t)elesize };
	int offset = 0;
	for (int i = 0; i < elesize; ++i)
	{
		assert((int)eles[i] < ARRAYSIZE(elespace));
		assert((int)eles[i] < ARRAYSIZE(map));

		auto cur = map[(int)eles[i]];
		cur.AlignedByteOffset = offset;
		offset += elespace[(int)eles[i]];
		elems[i] = cur;
	}

	return std::invoke([&elems,btc,btcSize] {
		ID3D11InputLayout* ia;
		COMHelper::AssertHResult("Create input layout failed.",
			Device::Get().GetHandler().Cast<IUnknown*,ID3D11Device*>()->CreateInputLayout(
				elems.data(),
				static_cast<UINT>(elems.size()),
				btc,
				static_cast<UINT>(btcSize),
				&ia));
		return Handler{ static_cast<IUnknown*>(ia),COMHelper::COMIUnknownDeleter };
	});
}

Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectTech::D3DEffectTech(Handler & hnd,const Graphics::EffectDataElement* eles, int elesize)
{
	D3DX11_TECHNIQUE_DESC desc;
	auto tech = hnd.Cast<IUnknown*, ID3DX11EffectTechnique*>();
	tech->GetDesc(&desc);
	for (uint32_t i = 0; i < desc.Passes; ++i)
	{
		const auto p = tech->GetPassByIndex(i);
		if (p == nullptr)
			throw std::invalid_argument{ "Can not get pass by index." };

		D3DX11_PASS_DESC passDesc;
		COMHelper::AssertHResult(
			"Can not get pass desc.",
			p->GetDesc(&passDesc));

		passes_.emplace_back(D3DEffectPass{
			Handler{ static_cast<IUnknown*>(p),COMHelper::COMIUnknownDeleter },
			createInputLayout(eles,elesize,passDesc.pIAInputSignature,passDesc.IAInputSignatureSize) });
	}
}


Snowing::Graphics::EffectPass & Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectTech::GetPass(size_t passID)
{
	assert(static_cast<size_t>(passID) < passes_.size());
	return passes_[passID];
}

size_t Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectTech::PassCount() const
{
	return passes_.size();
}

Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectPass::D3DEffectPass(Handler && hnd,Handler&& ia):
	pass_{ std::move(hnd) },
	ia_{ std::move(ia) }
{
}

void Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectPass::Apply(const Graphics::Context& ctx) const
{
	static_cast<ID3D11DeviceContext*>(ctx.GetImpl().GetHandler().Get<IUnknown*>())->IASetInputLayout(
		static_cast<ID3D11InputLayout*>(ia_.Get<IUnknown*>()));
	COMHelper::AssertHResult("Can not apply fx.",
		static_cast<ID3DX11EffectPass*>(pass_.Get<IUnknown*>())->Apply(0,
			static_cast<ID3D11DeviceContext*>(
				ctx.GetImpl().GetHandler().Get<IUnknown*>())));
}

Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectReflectionObject::D3DEffectReflectionObject(Handler && ref):
	ref_{ std::move(ref) }
{
}

void Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectReflectionObject::SetAsFloat(float f)
{
	COMHelper::AssertHResult("Can not set effect float.",
		ref_.Cast<IUnknown*, ID3DX11EffectVariable*>()->SetRawValue(&f, 0, sizeof(float)));
}

void Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectReflectionObject::SetAsVec2(Math::Vec2f f)
{
	COMHelper::AssertHResult("Can not set effect vec2.",
		ref_.Cast<IUnknown*, ID3DX11EffectVariable*>()->SetRawValue(&f, 0, sizeof(Math::Vec2f)));
}

void Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectReflectionObject::SetAsVec4(Math::Vec4f f)
{
	COMHelper::AssertHResult("Can not set effect vec4.",
		ref_.Cast<IUnknown*, ID3DX11EffectVariable*>()->SetRawValue(&f, 0, sizeof(Math::Vec4f)));
}

void Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectReflectionObject::SetAsConstantBuffer(const Graphics::Buffer * b)
{
	auto con = ref_.Cast<IUnknown*, ID3DX11EffectVariable*>()->AsConstantBuffer();
	COMHelper::AssertHResult("Can not set effect constant buffer.",
		con->SetConstantBuffer(b->GetImpl().GetHandler().Cast<IUnknown*, ID3D11Buffer*>()));
	con->Release();
}

void Snowing::PlatformImpls::WindowsImpl::D3D::D3DEffectReflectionObject::SetAsTexture2D(const Graphics::Texture2D * tex)
{
	auto t = ref_.Cast<IUnknown*, ID3DX11EffectVariable*>()->AsShaderResource();
	COMHelper::AssertHResult("Can not set effect texture2d.",
		t->SetResource(tex->GetImpl().ShaderResource().Cast<IUnknown*, ID3D11ShaderResourceView*>()));
	t->Release();
}
