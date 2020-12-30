#include "Fire.h"
#include <functional>

using namespace Snowing;

Fire::Fire(Snowing::Graphics::Effect* effect, Snowing::Math::Coordinate2DCenter coord):
	gpuSpriteSheet_{
		std::invoke([] {
			// 创建精灵列表
			Snowing::Graphics::SpriteSheet sheet;

			// 写入0号精灵为{ 0,0,1,1 }，方便几何着色器生成点
			sheet.Sheet[0] = { 0,0,1,1 };

			return SpriteRenderer::MakeGPUSpriteSheet(sheet);
		})
	},
	tech_{
		effect->LoadTechnique("Particle", Graphics::Sprite::DataLayout)
	},
	vertexBuffer_{ SpriteRenderer::MakeGPUVertexBuffer() },
	spriteRenderer_{
		&Graphics::Device::MainContext(),
		&tech_,
		coord,
		nullptr,
		&gpuSpriteSheet_,
		&vertexBuffer_
	}
{
	// 创建顶点缓存
	auto vertexSheet = SpriteRenderer::MakeGPUVertexBuffer();

	particles_.reserve(MaxParticle);
}

bool Fire::Update()
{
	// 绘制
	Engine::Get().Draw([this] {

		// 获取图形设备
		auto& g = Snowing::Graphics::Device::Get();

		// 清除画布
		g.MainContext().ClearRenderTarget(g.MainRenderTarget());

		// 设置屏幕为主画布
		g.MainContext().SetRenderTarget(&g.MainRenderTarget());

		// 绘制精灵列表
		spriteRenderer_.Draw(particles_.data(), particles_.size());
	});

	// 距离上一帧已经经过的时间
	const auto dt = Engine::Get().DeltaTime();
	time_ += dt;

	// 创建粒子
	auto dt2 = dt;
	while (dt2 >= 0.00008f)
	{
		dt2 -= 0.00008f;

		particles_.emplace_back();
		auto& sp = particles_.back();
		sp.Sp.Position =
		{
			0,
			rand() / float(RAND_MAX) * 200.0f + 200.0f
		};

		sp.Sp.Size = { 72.0f,72.0f };
		sp.SpeedY = rand() / float(RAND_MAX) * 700.0f + 250.0f;
		sp.OriginX = rand() / float(RAND_MAX) * 800.0f - 400.0f;

		sp.Sp.Color = { 1.0f,0.5f,0.5f,1.0f };

	}

	// 更新粒子
	for (auto& p : particles_)
	{
		p.Sp.Position.y -= dt * p.SpeedY;
		p.Life += dt;
		p.Sp.Position.x = p.OriginX * (1 - p.Life / 2.0f) * (1 - p.Life / 2.0f);
		p.SpeedY -= 1.5f * dt * 40;

		if (p.Life <= 0.1f)
			p.Sp.Color.w = 10 * p.Life;
		else
			p.Sp.Color.w -= dt * 2;

		p.Sp.Size.x -= dt * 8;
		p.Sp.Size.y -= dt * 8;
	}


	// 杀死无用粒子
	const auto nend = std::remove_if(particles_.begin(), particles_.end(), [](auto& p) {
		return p.Sp.Color.w <= 0;
	});
	particles_.erase(nend, particles_.end());

	return true;
}

size_t Fire::GetParticlesCount() const
{
	return particles_.size();
}
