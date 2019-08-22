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
			playQueue_.pop_front();

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

void Fyee::BGMPlayer::ClearQueueTail()
{
	while (playQueue_.size() > 1)
		playQueue_.pop_back();
}



bool Fyee::BGMPlayer::Update()
{
	updateScheduledBreakLoop();
	const bool hasObject = playground_.Update();
	updateCurrentPlayingTrack();
	return !(!hasObject && playQueue_.empty());
}

void Fyee::BGMPlayer::updateScheduledBreakLoop()
{
	if (auto _ = std::get_if<BreakOnNextLoop>(&breakSchedule_))
	{
		const auto playingTrack = getPlayingTrack();

		bool update = false;
		if (playingTrack == nullptr)
			update = true;
		else if (playingTrack->metronome_.GetTime() >= playQueue_.front().length)
			update = true;

		if (update)
		{
			playQueue_.front().loop = false;

			breakSchedule_ = NoSchedule{};
		}
	}
}


void Fyee::BGMPlayer::ScheduleBreakLoop(BreakLoopSchedule schedule)
{
	breakSchedule_ = schedule;
}
