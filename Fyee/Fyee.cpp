#include "pch.h"
#include "Fyee.h"

Fyee::BGMPlayer::PlayingTrack::PlayingTrack(const TrackInfo& t, uint32_t position,float fadingVolume):
	metronome_{&player_,180,0,0},
	fadeOutVolume_{fadingVolume},
	blob_{t.soundBlob}
{
	player_.SetVolume(fadingVolume);
	player_.Play(blob_.get(), position);
	const auto metadata = player_.GetMetadata();
	metronome_.Reset(&player_, metadata.Bpm, metadata.BeatsPerBar, metadata.BeatOffset);
}


bool Fyee::BGMPlayer::PlayingTrack::Update()
{
	fadeOutVolume_.Update();
	player_.SetVolume(std::clamp(fadeOutVolume_.Value(),0.0f,1.0f));
	metronome_.Update();
	return player_.GetPlaying() || fadeOutVolume_.Value() < 0.0f;
}

void Fyee::BGMPlayer::PlayingTrack::FadeOutAndStop(float time)
{
	fadeOutVolume_.Start(-0.001f, time);
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
	auto playingTrack = getPlayingTrack();

	if (playQueue_.empty()) playingTrack = nullptr;

	bool update = false;
	if (playingTrack == nullptr)
		update = true;
	else if (playingTrack->metronome_.GetTime() >= playQueue_.front().length)
		update = true;

	if(update)
	{
		if (!playQueue_.empty())
		{
			if (!playQueue_.front().loop && playingTrack)
				playQueue_.pop_front();
		}

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
	if(playQueue_.empty())
	{
		breakSchedule_ = NoSchedule{};
		return;
	}

	// BreakOnNextLoop
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

	// BreakWhenJumpTime
	else if (auto args = std::get_if<BreakWhenJumpTime>(&breakSchedule_))
	{
		const auto cur = getPlayingTrack();
		bool isTime = false;
		switch (args->whenJump)
		{
		case BreakTime::Now:
			isTime = true;
			break;
		case BreakTime::NextBeat:
			if (cur)
			{
				if (cur->metronome_.IsBeat())
					isTime = true;
			}
			else
				isTime = true;
			break;
		case BreakTime::NextBar:
			if (cur)
			{
				if (cur->metronome_.IsBeat() && cur->metronome_.GetTime().Beat == 0)
					isTime = true;
			}
			else
				isTime = true;
			break;	
		};

		if(isTime)
		{
			if (cur) cur->FadeOutAndStop(args->fadeOutTime);
			mainlyTrack_ = nullptr;
			breakSchedule_ = NoSchedule{};
			playQueue_.pop_front();
			updateCurrentPlayingTrack();
		}
	}
}


void Fyee::BGMPlayer::ScheduleBreakLoop(BreakLoopSchedule schedule)
{
	breakSchedule_ = schedule;
}

Fyee::BeatTime Fyee::BGMPlayer::GetTime()
{
	const auto cur = getPlayingTrack();
	if (cur)
		return cur->metronome_.GetTime();
	else
		return {};
}

bool Fyee::BGMPlayer::IsBeatFrame()
{
	const auto cur = getPlayingTrack();
	if (cur)
		return cur->metronome_.IsBeat();
	else
		return false;
}
