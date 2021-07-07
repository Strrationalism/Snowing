#pragma once
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")

#include "Handler.h"
#include "D3DContext.h"
#include "D3DRenderTarget.h"
#include <GraphicContext.h>
#include <Graphics.h>
#include "SingleInstance.h"

namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	class Device final : public SingleInstance<Device>
	{
	private:
		Handler swapChain_, device_;
		Graphics::Context mainContext_;
		Graphics::RenderTarget mainRenderTarget_;

		Handler createSwapChainAndDevice_ReturnMainContext(const Handler& hWnd, bool windowed);
		D3DTexture2D getBackBuffer();

		Graphics::GraphicsInterface<PlatformImpls::WindowsImpl::D3D::Device> keep_;
	public:
		Device(const Handler& hWnd, bool windowed = true);
		~Device();

		Device(Device&&) = default;

		Graphics::Context& MainContext();
		Graphics::RenderTarget& MainRenderTarget();

		void SetFullscreen(bool b);
		bool GetFullscreen() const;
		void Resize(Math::Vec2<int> size);

		void Update();
		const Handler& GetHandler() const;
	};

}

namespace Snowing::Graphics
{
	using Device = GraphicsInterface<PlatformImpls::WindowsImpl::D3D::Device>;
	Snowing::Math::Vec2<int> GetScreenSize();
}
