#include "pch.h"
#include "SEPlayer.h"

using namespace Snowing;
using namespace Yukimi;

SEPlayer::SEPlayer(AudioChannel::AudioLoader l) :
	loader_{ l }
{}

void SEPlayer::Play(Snowing::AssetName se, float pan, float volume)
{
	auto p = Emplace<>(loader_,se, 0.0F, 0u, pan);
	p->FadeVolume(volume);
}

void SEPlayer::FadeOutAll(float fade)
{
	Iter([fade](AudioChannel& c) {
		c.Stop(fade);
	});
}

bool SEPlayer::Update()
{
	Scene::Group<AudioChannel>::Update();
	return true;
}
