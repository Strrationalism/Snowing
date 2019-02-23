#include "pch.h"
using namespace std::chrono;
TEST(D3DTest, StreamOut)
{
	constexpr Snowing::Graphics::EffectDataElement ias[] =
	{
		Snowing::Graphics::EffectDataElement::Position_Float32_2D
	};

	float buf[1000];
	std::fill(buf, buf + 1000, 0.5f);

	WindowImpl window{ L"TestStreamOut" ,WinSize };
	D3D::Device device{ window.GetHWND(),true,D3D::Device::FeatureLevel::Level_10_0 };
	Engine engine;

	Graphics::Effect effect{ LoadAsset("StreamOutEffect") };
	auto tech = effect.LoadTechnique("StreamOutDemoTech",ias);
	

	Graphics::BufferBindMode streamOutBufferMode;
	streamOutBufferMode.BindType = BufferBindMode::BindTypeEnum::VertexBuffer;
	streamOutBufferMode.StreamOutput = true;

	Buffer outBuffer{
		sizeof(buf),
		streamOutBufferMode,
		buf,
		BufferUsage::Default,
		BufferCPUAccessFlag::NoAccess
	};

	Buffer outBuffer2{
		sizeof(buf),
		streamOutBufferMode,
		buf,
		BufferUsage::Default,
		BufferCPUAccessFlag::NoAccess
	};

	Buffer *b1 = &outBuffer, *b2 = &outBuffer2;
	std::chrono::high_resolution_clock clk;
	auto beg = clk.now();
	engine.Run([&]() {
		engine.Draw([&]
		{
			auto& d = Device::Get();
			std::swap(b1, b2);
			std::array<Graphics::RenderTarget*, 1> rts = { &d.MainRenderTarget() };
			d.MainContext().SetRenderTarget(rts);
			tech[0](d.MainContext());

			d.MainContext().SetVertexBuffer(Primitive::PointList, b1, 0, 0);

			std::array<Buffer*, 1> buf = { b2 };
			d.MainContext().SetStreamOutBuffer(buf);
			d.MainContext().Draw(500);

			d.MainContext().CloseStreamOut();
		});

		if (clk.now() - beg > 1s)
			engine.Exit();
	});
}
