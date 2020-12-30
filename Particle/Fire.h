#pragma once
#include <Snowing.h>

class Fire final : public Snowing::Scene::Object {
private:
	// ������������
	struct ParticleSprite final
	{
		Snowing::Graphics::Sprite Sp;	// �����ľ�������
		float SpeedY;			// Y�����ٶ�
		float Life = 0.0f;		// ����ֵ
		float OriginX;			// ԭʼX����
	};

	// �������8192��������Ϊ����
	constexpr static size_t MaxParticle = 8192;

	// Ҫʹ�õ�SpriteRenderer����
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
