#include "pch.h"
#include <Windows.h>
using namespace std::chrono;


TEST(D3DTest, BasicllyRendering) {

	constexpr Snowing::Graphics::EffectDataElement ias[] =
	{
		Snowing::Graphics::EffectDataElement::Position_Float32_2D,
		Snowing::Graphics::EffectDataElement::Color_Float32_4D
	};

	struct MyVertex {
		Math::Vec2f pos;
		Math::Vec4f col;
	};

	const MyVertex vtx[] = {
		{Math::Vec2f{0.0f,-0.9f},Math::Vec4f{0.0f,1.0f,1.0f,1.0f}},
		{Math::Vec2f{-0.9f,0.9f},Math::Vec4f{1.0f,1.0f,0.0f,1.0f}},
		{Math::Vec2f{0.9f,0.9f},Math::Vec4f{1.0f,0.0f,1.0f,1.0f}}
	};

	MyVertex blank[3];

	WindowImpl window{ L"TestDevice,Effect,CBuf,Tex" ,WinSize };
	D3D::Device device{ window.GetHWND(),true };
	Engine engine;

	Snowing::Graphics::Effect effect{ Snowing::LoadAsset("HelloEffect") };
	auto root = (*effect);
	auto tech = root.LoadTechnique("Hello",ias);
	auto& p = tech[0];

	Snowing::Graphics::SpriteSheet sheet = { 0 };
	auto tex = Snowing::Graphics::LoadSpriteAndSpriteSheet(Snowing::LoadAsset("Background.ctx"), sheet);

	Snowing::Graphics::Buffer buf{
		sizeof(vtx),
		Graphics::DefaultVertexBufferMode,
		(void*)vtx,
		Snowing::Graphics::BufferUsage::Dynamic,
		Snowing::Graphics::BufferCPUAccessFlag::CPUWriteOnly };

	float a = 0;
	Snowing::Graphics::Buffer cbuf{
		sizeof(float) * 4,
		Graphics::DefaultConstantBufferMode,
		(void*)&a,
		Snowing::Graphics::BufferUsage::Dynamic,
		Snowing::Graphics::BufferCPUAccessFlag::CPUWriteOnly };

	effect.Reflection("tex").SetAsTexture2D(&tex);
	auto colMulRef = effect.Reflection("colMul");

	Graphics::BufferBindMode mode;
	mode.BindType = Graphics::BufferBindMode::BindTypeEnum::None;
	Snowing::Graphics::Buffer outbuf{
		sizeof(vtx),
		mode,
		blank,
		Snowing::Graphics::BufferUsage::Staging,
		Snowing::Graphics::BufferCPUAccessFlag::CPUReadWrite };

	engine.Draw([&]
	{
		buf.CopyTo(Device::Get().MainContext(), outbuf);
		outbuf.Access(Device::Get().MainContext(), Graphics::AccessType::ReadWrite, [](void* data)
		{
			auto p = (MyVertex*)data;
			for (int i = 0; i < 3; ++i)
				std::cout << "Vertex " << i << ":" << p[i].pos.x << "," << p[i].pos.y << std::endl;
		});
	});

	
	std::chrono::high_resolution_clock clk;
	const auto beg = clk.now();

	engine.Draw([&buf,&vtx] {
		buf.Access(Device::Get().MainContext(), Snowing::Graphics::AccessType::WriteDiscard, [&vtx](void* data) {
			memcpy_s(data, sizeof(vtx), vtx, sizeof(vtx));
		});
	});

	bool b1 = true;
	bool b2 = true;
	bool b3 = true, b4 = true;

	Graphics::RenderTarget rt2{ {800,600} };

	Font font = LoadFont(LoadAsset("Font-chs.fnt"));
	Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
	Graphics::EffectTech tech1 = eff.LoadTechnique("FontTestBasic", Sprite::DataLayout);

	Scene::Debug::DebugDisplay frameTime(
		&tech1, &font, L"Time", Scene::Debug::DebugDisplay::FrameTimeGetter);
	Scene::Debug::DebugDisplay fps(
		&tech1, &font, L"FPS", Scene::Debug::DebugDisplay::FPSGetter);

	engine.Run([&] {
		engine.Draw(
			[&] {
			auto& g = Device::Get();
			auto& rt = g.MainRenderTarget();
			g.MainContext().ClearRenderTarget(rt, { 0.0f,0.0f,1.0f,1.0f });
			g.MainContext().ClearRenderTarget(rt2, { 1.0f,1.0f,1.0f,1.0f });
			p(g.MainContext());

			cbuf.Access(g.MainContext(), Snowing::Graphics::AccessType::WriteDiscard, [&](void* out) {
				auto p = (float*)out;
				*p = (clk.now() - beg).count() / 1000000000.0f;
			});

			colMulRef.SetAsFloat((clk.now() - beg).count() / 1000000000.0f);


			std::array<RenderTarget*, 1> rts = { &g.MainRenderTarget() };
			g.MainContext().SetRenderTarget(rts);

			std::array<RenderTarget*, 1> rts2 = { &rt2 };

			g.MainContext().SetVertexBuffer(Snowing::Graphics::Primitive::TriangleList, &buf, sizeof(MyVertex));
			g.MainContext().Draw(3);

			g.MainContext().SetRenderTarget(rts2);
			g.MainContext().Draw(3);

			frameTime.Update();
			fps.Update();
		});

		const auto time = clk.now() - beg;

		if (time > 500ms && time < 1s && b1)
		{
			b1 = false;
			Window::Get().SetWindowed(false);
		}

		else if (time > 1s && time < 1500ms && b2)
		{
			b2 = false;
			Window::Get().SetWindowed(true);
		}

		else if (time > 1500ms && time < 2s && b3)
		{
			b3 = false;
			Window::Get().Resize({ 1024,768 });
		}

		else if (time > 2s && time < 2500ms && b4)
		{
			b4 = false;
			Window::Get().SetWindowed(false);
		}

		if (time > 2500ms)
			Engine::Get().Exit();
	});
}

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
	auto tech = effect.LoadTechnique("StreamOutDemoTech", ias);


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

		if (clk.now() - beg > 200ms)
			engine.Exit();
	});
}
