#include "pch.h"
#include "BGMPlayer.h"

using namespace Snowing;
using namespace Yukimi;


void BGMPlayer::stopAllChannels(float fadeTime)
{
	Iter([fadeTime](AudioChannel& channel) {
		channel.Stop(fadeTime);
	});
}

void BGMPlayer::Play(AssetName bgm, float fadeTime, uint32_t begin)
{
	stopAllChannels(fadeTime);
	currentChannel_ = Emplace<AudioChannel>(bgm, fadeTime, begin);
}

void BGMPlayer::FadeTo(AssetName a, float fadeTime)
{
	uint32_t continuation = 0;
	Iter([this, fadeTime,&continuation](AudioChannel & channel) {
		if (currentChannel_ == &channel)
			continuation = channel.GetPosition();
	});

	Play(a, fadeTime, continuation);
}

void BGMPlayer::Pause(float fadeTime)
{
	Iter([this, fadeTime](AudioChannel & channel) {
		if (currentChannel_ == &channel)
			channel.Pause(fadeTime);
	});
}

void BGMPlayer::Resume(float fadeTime)
{
	Iter([this, fadeTime](AudioChannel & channel) {
		if (currentChannel_ == &channel)
			channel.Resume(fadeTime);
	});
}

bool BGMPlayer::Update()
{
	Group<AudioChannel>::Update();
	return true;
}

void BGMPlayer::Stop(float fadeTime)
{
	currentChannel_ = nullptr;
	Iter([fadeTime](AudioChannel & channel) {
			channel.Stop(fadeTime);
	});
}
