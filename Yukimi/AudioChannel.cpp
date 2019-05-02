#include "pch.h"
#include "AudioChannel.h"

using namespace Yukimi;

AudioChannel::AudioChannel(AudioLoader loader,std::string&& s, float fadeIn, uint32_t begin, float pan):
	onSoundLoaded_{
		[this] { return soundLoading_.valid(); },
		[this,fadeIn,begin,pan] {
			sound_ = soundLoading_.get();
			fadeVolume_ = 0;
			player_.SetPan(pan);
			player_.Play(&sound_, begin);

			fadeVolume_.Start(1, fadeIn);

			auto metadata = player_.GetMetadata();
			metronome_.emplace(&player_,metadata.Bpm,metadata.BeatsPerBar,metadata.BeatOffset);
		}
	},
	soundLoading_{
		std::async(std::launch::async,[s = std::move(s),loader]{return loader(s.c_str()); })
	}
{
}

void AudioChannel::Stop(float fadeOut)
{
	fadeVolume_.Start(-0.0001f,fadeOut);
}

void AudioChannel::FadeVolume(float volume, float fade)
{
	mainVolume_.Start(volume, fade);
}

bool AudioChannel::Update()
{
	onSoundLoaded_.Update();
	fadeVolume_.Update();
	mainVolume_.Update();
	player_.SetVolume(*mainVolume_ * std::clamp(*fadeVolume_,0.0f,1.0f));

	if (metronome_.has_value())
		metronome_->Update();
	
	return player_.GetPlaying() && *fadeVolume_ >= 0;
}

void AudioChannel::Pause(float fadeOut)
{
	if (!pausePoint_.has_value())
	{
		pausePoint_ = player_.GetPosition();
		fadeVolume_.Start(0, fadeOut);
	}
}

void AudioChannel::Resume(float fadeIn)
{
	if (pausePoint_.has_value())
	{
		player_.Play(&sound_, pausePoint_.value());
		pausePoint_.reset();
		fadeVolume_.Start(1, fadeIn);
	}
}

uint32_t AudioChannel::GetPosition() const
{
	return player_.GetPosition();
}

float AudioChannel::GetRealtimeVolume() const
{
	return player_.GetRealtimeVolume();
}

Yukimi::AudioChannel::AudioChannel(Snowing::Blob&& soundBlob, float fadeIn, uint32_t begin, float pan):
	sound_{ std::move(soundBlob) },
	onSoundLoaded_{
		[this] { return soundLoading_.valid(); },
		[this,fadeIn,begin,pan] {
			sound_ = soundLoading_.get();
			fadeVolume_ = 0;
			player_.SetPan(pan);
			player_.Play(&sound_, begin);

			fadeVolume_.Start(1, fadeIn);
		}
	}
{
	fadeVolume_ = 0;
	player_.SetPan(pan);
	player_.Play(&sound_, begin);

	fadeVolume_.Start(1, fadeIn);

	auto metadata = player_.GetMetadata();
	metronome_.emplace(&player_, metadata.Bpm, metadata.BeatsPerBar, metadata.BeatOffset);
}

Snowing::Blob Yukimi::AudioChannel::MoveOutBlob()
{
	auto blob = std::move(sound_);
	sound_ = blob.BorrowSlice(blob.Size());
	return blob;
}

std::optional<const Snowing::Scene::Metronome<>*> Yukimi::AudioChannel::GetMetronome() const
{
	if (metronome_.has_value())
		return std::make_optional(&(metronome_.value()));
	else
		return std::nullopt;
}
