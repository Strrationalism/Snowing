#include "pch.h"
#include "Fyee.h"

Fyee::BGMPlayer::PlayingTrack::PlayingTrack(const TrackInfo& t, uint32_t position,float fadingVolume, float volume):
	metronome_{&player_,180,0,0},
	fadeOutVolume_{fadingVolume},
	blob_{t.soundBlob},
	volume_{std::clamp(volume,0.0f,1.0f)}
{
	player_.SetVolume(fadingVolume * volume_);
	player_.Play(blob_.get(), position);
	const auto metadata = player_.GetMetadata();
	metronome_.Reset(&player_, metadata.Bpm, metadata.BeatsPerBar, metadata.BeatOffset);
}

void Fyee::BGMPlayer::PlayingTrack::SetVolume(float v)
{
	volume_ = std::clamp(v, 0.0f, 1.0f);
}


bool Fyee::BGMPlayer::PlayingTrack::Update()
{
	fadeOutVolume_.Update();
	player_.SetVolume(volume_ * std::clamp(fadeOutVolume_.Value(),0.0f,1.0f));
	metronome_.Update();
	return player_.GetPlaying() || fadeOutVolume_.Value() < 0.0f;
}

void Fyee::BGMPlayer::PlayingTrack::FadeOutAndStop(float time)
{
	isFadingOut_ = true;
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
	else
	{
		const auto metadata = playingTrack->player_.GetMetadata();
		const auto offset = 
			metadata.BeatOffset ? metadata.BeatsPerBar - metadata.BeatOffset : 0;
		auto t = playingTrack->metronome_.GetTime();
		auto t2 = t;
		t.Beat += offset;
		if(t.Beat >= metadata.BeatsPerBar)
		{
			t.Beat -= metadata.BeatsPerBar;
			t.Bar++;
		}
		if (t >= playQueue_.front().length)
			update = true;
	}

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

			PlayingTrack* track;
				mainlyTrack_ = track =
				playground_.Emplace(playQueue_.front(),0, playQueue_.front().fadeInTime > 0.01f ? 0.0f : 1.0f,volume_);

			track->fadeOutVolume_.Start(1.0, playQueue_.front().fadeInTime);

			playQueue_.front().fadeInTime = 0;
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

void Fyee::BGMPlayer::Reset(float fadeout)
{
	playQueue_.clear();
	playground_.Iter([fadeout](auto& x) {x.FadeOutAndStop(fadeout); });
	mainlyTrack_ = nullptr;
	breakSchedule_.clear();
	volume_ = 1;
}

void Fyee::BGMPlayer::SetVolume(float v)
{
	volume_ = v;
}

const std::deque<Fyee::BGMPlayer::BreakLoopSchedule>& Fyee::BGMPlayer::BreakScheduleQueue()
{
	return breakSchedule_;
}



bool Fyee::BGMPlayer::Update()
{
	playground_.Iter([this](auto& c) {c.SetVolume(volume_); });
	const bool hasObject = playground_.Update();
	UpdateScheduledBreakLoop();
	updateCurrentPlayingTrack();
	return !(!hasObject && playQueue_.empty());
}

void Fyee::BGMPlayer::UpdateScheduledBreakLoop()
{
	if(playQueue_.empty())
	{
		while (!breakSchedule_.empty())
			breakSchedule_.pop_front();
		return;
	}

	if (!breakSchedule_.empty())
	{
		// BreakOnNextLoop
		if (auto _ = std::get_if<BreakOnNextLoop>(&breakSchedule_.front()))
		{
			if(!playQueue_.empty())
				playQueue_.front().loop = false;

			const auto playingTrack = getPlayingTrack();

			bool update = false;
			if (playingTrack == nullptr)
				update = true;
			else if (playingTrack->metronome_.GetTime() >= playQueue_.front().length)
				update = true;

			if (update)
				breakSchedule_.pop_front();
		}

		// BreakWhenJumpTime
		else if (auto args = std::get_if<BreakWhenJumpTime>(&breakSchedule_.front()))
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
					if (cur->metronome_.IsBeat())
					{
						const auto offset =
							std::invoke([this] {
							if (playQueue_.size() >= 2)
								return Snowing::Audio::GetSoundMetadata(
									*(playQueue_.begin() + 1)->soundBlob).BeatOffset;
							else return 0;
							});
						if (cur->metronome_.GetTime().Beat == offset && cur->player_.GetMetadata().BeatsPerBar)
						{
							isTime = true;
						}
					}
				}
				else
					isTime = true;
				break;
			};

			if (isTime)
			{
				if (cur) cur->FadeOutAndStop(args->fadeOutTime);
				mainlyTrack_ = nullptr;
				breakSchedule_.pop_front();
				playQueue_.pop_front();
				updateCurrentPlayingTrack();
			}
		}
	}
}


void Fyee::BGMPlayer::ScheduleBreakLoop(BreakLoopSchedule schedule)
{
	breakSchedule_.push_back(schedule);
}

const Snowing::Blob* Fyee::BGMPlayer::GetPlaying() 
{
	const auto playingTrack = getPlayingTrack();
	if (playingTrack)
		return playingTrack->blob_.get();
	else
		return nullptr;
}

size_t Fyee::BGMPlayer::GetQueuedTrackCount() const
{
	return playQueue_.size();
}

const Snowing::Blob* Fyee::BGMPlayer::GetQueuedTrackBlob(size_t index) const
{
	return playQueue_.at(index).soundBlob.get();
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

void Fyee::BGMPlayer::ClearCoda(float time)
{
	playground_.Iter([time](auto& p) {
		if (p.IsFadingOut())
			p.FadeOutAndStop(time);
	});
}
