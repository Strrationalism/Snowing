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

	Font font = LoadFont(LoadAsset(L"Font-zh-CN.fnt"));
	Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
	Graphics::EffectTech tech1 = eff.LoadTechnique("FontTestBasic", Sprite::DataLayout);

	std::map<wchar_t, Math::Vec2f> fix{};
	Scene::DebugDisplay frameTime(
		&eff, &tech1, &font, &fix, L"Time", Scene::DebugDisplay::FrameTimeGetter);
	Scene::DebugDisplay fps(
		&eff, &tech1, &font, &fix, L"FPS", Scene::DebugDisplay::FPSGetter);

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

		if (time > 1s && time < 2s && b1)
		{
			b1 = false;
			Window::Get().SetWindowed(false);
		}

		else if (time > 2s && time < 3s && b2)
		{
			b2 = false;
			Window::Get().SetWindowed(true);
		}

		else if (time > 3s && time < 4s && b3)
		{
			b3 = false;
			Window::Get().Resize({ 1024,768 });
		}

		else if (time > 4s && time < 5s && b4)
		{
			b4 = false;
			Window::Get().SetWindowed(false);
		}

		if (time > 5s)
			Engine::Get().Exit();
	});
}