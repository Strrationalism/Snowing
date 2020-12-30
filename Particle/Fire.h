#pragma once
#include <Snowing.h>

class Fire final : public Snowing::Scene::Object {
private:
	// 创建粒子类型
	struct ParticleSprite final
	{
		Snowing::Graphics::Sprite Sp;	// 基本的精灵数据
		float SpeedY;			// Y方向速度
		float Life = 0.0f;		// 生命值
		float OriginX;			// 原始X坐标
	};

	// 最大允许8192个精灵作为粒子
	constexpr static size_t MaxParticle = 8192;

	// 要使用的SpriteRenderer类型
	using SpriteRenderer = Snowing::Graphics::SpriteRenderer <
		ParticleSprite,
		Snowing::Graphics::SpriteSheet,
		MaxParticle>;

	Snowing::Graphics::Buffer gpuSpriteSheet_, vertexBuffer_;

	Snowing::Graphics::EffectTech tech_;

	SpriteRenderer spriteRenderer_;

	std::vector<ParticleSprite> particles_;

	float time_ = 0;

public:
	Fire(Snowing::Graphics::Effect* effect, Snowing::Math::Coordinate2DCenter coord);

	bool Update() override;

	size_t GetParticlesCount() const;
};
