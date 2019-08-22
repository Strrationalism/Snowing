#pragma once
#include <queue>
#include <Snowing.h>

namespace Fyee
{
	using Metronome = Snowing::Scene::Metronome<>;
	using BeatTime = Snowing::Scene::Metronome<>::Time;

	struct TrackInfo final
	{
		const Snowing::Blob* soundBlob = nullptr;
		BeatTime length;
		bool loop;
		std::function<void()> onPlayStarted;
	};

	class BGMPlayer final : public Snowing::Scene::Object
	{
	private:
		class PlayingTrack final : public Snowing::Scene::Object
		{
		public:
			Snowing::Audio::SoundPlayer player_;
			Metronome metronome_;

		public:
			PlayingTrack(const TrackInfo&,uint32_t position = 0);
			bool Update() override;
		};

	private:
		std::deque<TrackInfo> playQueue_;	// 头部总是“正在播放的track”

		Snowing::Scene::Group<PlayingTrack> playground_;
		const PlayingTrack* mainlyTrack_ = nullptr;

		PlayingTrack* getPlayingTrack();
		void updateCurrentPlayingTrack();

	public:
		// schedule break loop
		enum class BreakTime
		{
			Now,
			NextBar,
			NextBeat
		};

		struct NoSchedule{};
		struct BreakOnNextLoop{};
		struct BreakWhenJumpTime
		{
			BreakTime whenJump;
			float fadeOutTime;
		};
		using BreakLoopSchedule = std::variant<NoSchedule, BreakOnNextLoop, BreakWhenJumpTime>;

	private:
		BreakLoopSchedule breakSchedule_;
		void updateScheduledBreakLoop();

	public:
		template <typename TrackInfo>
		void AddToPlayQueue(TrackInfo&& track)
		{
			playQueue_.emplace_back(std::forward<TrackInfo>(track));
			updateCurrentPlayingTrack();
		}

		bool Update() override;

		// 清除所有待播放的Track，只保留正在播放的Track
		void ClearQueueTail();

		void ScheduleBreakLoop(BreakLoopSchedule schedule);
	};
}
