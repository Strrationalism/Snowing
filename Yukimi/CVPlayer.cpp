#include "pch.h"
#include "CVPlayer.h"

using namespace Snowing;
using namespace Yukimi;

constexpr float FadeTime = 0.25f;
constexpr float LoVolume = 0.2f;
constexpr float HiVolume = 1.0f;

CVPlayer::CVPlayer(AudioChannel::AudioLoader loader) :
	loader_{ loader }
{}

void CVPlayer::Play(Snowing::AssetName ass)
{
	FadeOutAll();
	activeChannel_ = Emplace<>(loader_, ass, 0.0f, 0u, 0.0f);
	activeChannel_->FadeVolume(volume_);
}

void CVPlayer::FadeOutAll()
{
	Iter([this](AudioChannel & c) {
		c.Stop(FadeTime);
	});
}

bool CVPlayer::Update()
{
	Snowing::Scene::Group<AudioChannel>::Update();
	bool found = false;
	if (!Exist(activeChannel_))
		activeChannel_ = nullptr;

	return true;
}

void CVPlayer::VolumeDown()
{
	if (Exist(activeChannel_))
		activeChannel_->FadeVolume(LoVolume, FadeTime);

	volume_ = LoVolume;
}

void CVPlayer::VolumeUp()
{
	if (Exist(activeChannel_))
		activeChannel_->FadeVolume(LoVolume, HiVolume);

	volume_ = HiVolume;
}

float CVPlayer::GetRealtimeVolume() const
{
	if (ExistIgnoreNewObjects(activeChannel_))
		return activeChannel_->GetRealtimeVolume();
	else
		return 0.0f;
}
