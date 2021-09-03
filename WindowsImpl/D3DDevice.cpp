#include "stdafx.h"
#include "D3DDevice.h"
#include <dxgi.h>
#include <d3d11.h>
#include <Utils.h>
#include <VersionHelpers.h>
#include "COMHelper.h"
#include "WindowImpl.h"

using namespace Snowing::PlatformImpls::WindowsImpl;
using namespace Snowing::PlatformImpls::WindowsImpl::D3D;

Handler Snowing::PlatformImpls::WindowsImpl::D3D::Device::createSwapChainAndDevice_ReturnMainContext(const Handler & hWnd, bool windowed)
{
	const auto hwnd = hWnd.Get<HWND>();

	const auto size = WindowImpl::Get().GetSize().Cast<UINT>();

	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferCount = 2;
	sd.BufferDesc.Width = (size.x / 4) * 4;
	sd.BufferDesc.Height = (size.y / 4) * 4;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_SEQUENTIAL;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = windowed;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (IsWindows8OrGreater())
		sd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	const D3D_FEATURE_LEVEL levels[] = {
		//D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;

#ifdef _DEBUG
	constexpr UINT flag = D3D11_CREATE_DEVICE_DEBUG;
#else
	constexpr UINT flag = 0;
#endif


	constexpr D3D_DRIVER_TYPE types[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};


	//MessageBoxA(nullptr, "Ready to create swap chain and device.", __FUNCTION__, 0);

	bool succ = false;
	HRESULT hr;
	for (auto type : types)
	{
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			type,
			nullptr,
			flag,
			levels,
			sizeof(levels) / sizeof(levels[0]),
			D3D11_SDK_VERSION,
			&sd,
			&swapChain,
			&device,
			nullptr,
			&context);

		if (SUCCEEDED(hr))
		{
			succ = true;
			break;
		}
		else 
		{
			constexpr auto MsgBoxType =
				MB_OK | MB_ICONSTOP | MB_DEFBUTTON1 | MB_APPLMODAL | MB_TOPMOST;
			char s[128] = { '\0' };
			sprintf_s(s, "D3D11CreateDeviceAndSwapChain failed: \nHRESULT=%x, type=%d",
				hr, type);
			MessageBoxA(nullptr, s, "D3D11CreateDeviceAndSwapChain failed", MsgBoxType);

#ifdef _DEBUG
			const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_0;
			hr = D3D11CreateDeviceAndSwapChain(
				nullptr,
				type,
				nullptr,
				0,
				&featureLevel,
				1,
				D3D11_SDK_VERSION,
				&sd,
				&swapChain,
				&device,
				nullptr,
				&context);

			if (SUCCEEDED(hr))
			{
				succ = true;
				break;
			}
			else
			{
				sprintf_s(s, "D3D11CreateDeviceAndSwapChain failed(debug): \nHRESULT=%x, type=%d", 
					hr, type);
				MessageBoxA(nullptr, s, "D3D11CreateDeviceAndSwapChain failed", MsgBoxType);
			}
#endif
		}
	}

	//MessageBoxA(nullptr, "Swapchain and device create succeed.", __FUNCTION__, 0);

	if (!succ)
		throw std::runtime_error{ "D3D Device create failed:" + std::to_string(hr) };

	const_cast<Handler&>(swapChain_) =
	{ static_cast<IUnknown*>(swapChain),COMHelper::COMIUnknownDeleter };
	const_cast<Handler&>(device_) =
	{ static_cast<IUnknown*>(device),COMHelper::COMIUnknownDeleter };

	ShowWindow(hwnd, SW_SHOW);

	return { static_cast<IUnknown*>(context),COMHelper::COMIUnknownDeleter };
}

D3DTexture2D Snowing::PlatformImpls::WindowsImpl::D3D::Device::getBackBuffer()
{
	ID3D11Texture2D *pBackBuffer = NULL;
	COMHelper::AssertHResult(
		"Can not get back buffer.",
		static_cast<IDXGISwapChain*>(swapChain_.Get<IUnknown*>())->
			GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));

	return D3DTexture2D{ { static_cast<IUnknown*>(pBackBuffer),COMHelper::COMIUnknownDeleter } };
}

Snowing::PlatformImpls::WindowsImpl::D3D::Device::Device(const Handler & hWnd, bool windowed):
	mainContext_{ createSwapChainAndDevice_ReturnMainContext(hWnd,windowed) },
	mainRenderTarget_{ D3DRenderTarget(getBackBuffer(),device_) }
{
	//MessageBoxA(nullptr, "Device creation succeed.", __FUNCTION__, 0);
}

Snowing::PlatformImpls::WindowsImpl::D3D::Device::~Device()
{
	if(swapChain_.IsSome())
		swapChain_.Cast<IUnknown*, IDXGISwapChain*>()->SetFullscreenState(false,nullptr);
}

Snowing::Graphics::Context & Snowing::PlatformImpls::WindowsImpl::D3D::Device::MainContext()
{
	return mainContext_;
}

Snowing::Graphics::RenderTarget & Snowing::PlatformImpls::WindowsImpl::D3D::Device::MainRenderTarget()
{
	return mainRenderTarget_;
}

void Snowing::PlatformImpls::WindowsImpl::D3D::Device::SetFullscreen(bool b)
{
	auto sc = static_cast<IDXGISwapChain*>(swapChain_.Get<IUnknown*>());
	sc->SetFullscreenState(b,nullptr);

	const HWND h = b ? GetDesktopWindow() : WindowImpl::Get().GetHWND().Get<HWND>();
	RECT r;
	if (!GetClientRect(h, &r))
		throw std::runtime_error("Get client window failed.");
	Resize({ static_cast<int>(r.right - r.left),static_cast<int>(r.bottom - r.top) });
}

bool Snowing::PlatformImpls::WindowsImpl::D3D::Device::GetFullscreen() const
{
	BOOL b = false;
	auto sc = static_cast<IDXGISwapChain*>(swapChain_.Get<IUnknown*>());
	sc->GetFullscreenState(&b, nullptr);
	return b;
}

void Snowing::PlatformImpls::WindowsImpl::D3D::Device::Resize(Math::Vec2<int> size)
{
	if (!swapChain_.IsSome()) return;
	std::destroy_at(&mainRenderTarget_);
	swapChain_.Cast<IUnknown*, IDXGISwapChain*>()->ResizeBuffers(
		0,
		static_cast<UINT>(size.x) / 4 * 4,
		static_cast<UINT>(size.y) / 4 * 4,
		DXGI_FORMAT_UNKNOWN,
		0);
	new (&mainRenderTarget_) Graphics::RenderTarget{ D3DRenderTarget(getBackBuffer(),device_) };
}


void Snowing::PlatformImpls::WindowsImpl::D3D::Device::Update()
{
	COMHelper::AssertHResult("Present Failed",
		static_cast<IDXGISwapChain*>(swapChain_.Get<IUnknown*>())->Present(1, 0));
}

const Handler & Snowing::PlatformImpls::WindowsImpl::D3D::Device::GetHandler() const
{
	return device_;
}

Snowing::Math::Vec2<int> Snowing::Graphics::GetScreenSize()
{
	return {
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN)
	};
}
