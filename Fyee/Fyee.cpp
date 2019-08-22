#include "pch.h"
#include "Fyee.h"

Fyee::BGMPlayer::PlayingTrack::PlayingTrack(const TrackInfo& t, uint32_t position):
	metronome_{&player_,180,0,0}
{
	player_.Play(t.soundBlob, position);
	const auto metadata = player_.GetMetadata();
	metronome_.Reset(&player_, metadata.Bpm, metadata.BeatsPerBar, metadata.BeatOffset);
}


bool Fyee::BGMPlayer::PlayingTrack::Update()
{
	metronome_.Update();
	return player_.GetPlaying();
}

Fyee::BGMPlayer::PlayingTrack* Fyee::BGMPlayer::getPlayingTrack()
{
	return playground_.FindFirst([this](const PlayingTrack& obj)
	{
		return &obj == mainlyTrack_;
	});
}


void Fyee::BGMPlayer::updateCurrentPlayingTrack()
{
	const auto playingTrack = getPlayingTrack();

	bool update = false;
	if (playingTrack == nullptr)
		update = true;
	else if (playingTrack->metronome_.GetTime() >= playQueue_.front().length)
		update = true;

	if(update)
	{
		if (!playQueue_.front().loop && playingTrack)
			playQueue_.pop();

		if(playQueue_.empty())
		{
			mainlyTrack_ = nullptr;
		}
		else
		{
			mainlyTrack_ =
				playground_.Emplace(playQueue_.front());

			if(playQueue_.front().onPlayStarted)
				playQueue_.front().onPlayStarted();
			playQueue_.front().onPlayStarted = [] {};
		}
	}
}

bool Fyee::BGMPlayer::Update()
{
	const bool hasObject = playground_.Update();
	updateCurrentPlayingTrack();
	return !(!hasObject && playQueue_.empty());
}