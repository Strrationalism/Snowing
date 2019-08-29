#pragma once
#include <queue>
#include <Snowing.h>
#include <memory>

namespace Fyee
{
	using Metronome = Snowing::Scene::Metronome<>;
	using BeatTime = Snowing::Scene::Metronome<>::Time;

	struct TrackInfo final
	{
		std::shared_ptr<Snowing::Blob> soundBlob;
		BeatTime length;
		bool loop;
		std::function<void()> onPlayStarted;
	};

	class BGMPlayer : public Snowing::Scene::Object
	{
	private:
		class PlayingTrack final : public Snowing::Scene::Object,public Snowing::MemPool<PlayingTrack>
		{
		public:
			std::shared_ptr<Snowing::Blob> blob_;
			Snowing::Audio::SoundPlayer player_;
			Metronome metronome_;

			Snowing::Scene::Tween<float> fadeOutVolume_ = 1.0f;

			

		public:
			PlayingTrack(const TrackInfo&,uint32_t position = 0,float fadeingVolume = 1);
			bool Update() override;

			void FadeOutAndStop(float time);
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
			float fadeOutTime = 0.1f;
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

		const Snowing::Blob* GetPlaying();

		template <typename TrackInfo>
		void EditionFading(TrackInfo&& anotherEdition,float fadeOutTime = 0.5f)
		{
			assert(!playQueue_.empty());

			playQueue_.front() = std::forward<TrackInfo>(anotherEdition);

			playQueue_.front().onPlayStarted();
			playQueue_.front().onPlayStarted = [] {};

			const auto cur = getPlayingTrack();
			uint32_t position = 0;
			if (cur) 
			{
				cur->FadeOutAndStop(fadeOutTime);
				position = cur->player_.GetPosition();
			}
			auto p = playground_.Emplace<>(playQueue_.front(), position, 0.0f);
			p->fadeOutVolume_.Start(1, fadeOutTime);
			mainlyTrack_ = p;
		}

		BeatTime GetTime();
		bool IsBeatFrame();
	};
}
