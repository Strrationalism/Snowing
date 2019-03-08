#include "pch.h"
#include "CVPlayer.h"

using namespace Snowing;
using namespace Yukimi;

bool CVPlayer::Update()
{
	Scene::Group<AudioChannel>::Update();
	return true;
}

void CVPlayer::FadeOutAll()
{
	Iter([](AudioChannel& c) {
		c.Stop(speedTime);
	});
}

void CVPlayer::Play(Snowing::AssetName ass)
{
	constexpr static float pan = 0.0f;
	auto p = Emplace<>(ass, 0.0F, 0u, pan);
	p->FadeVolume(1.0f);
}

void CVPlayer::VolumeUp()
{
	SetVolumeHelper(1.0f);
}

void CVPlayer::VolumeDown()
{
	SetVolumeHelper(0.5f);
}

inline void CVPlayer::SetVolumeHelper(float volume)
{
	Iter([volume](AudioChannel& c) {
		c.FadeVolume(volume);
	});
}
