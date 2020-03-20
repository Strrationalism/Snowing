#pragma once
#include <deque>
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

		float fadeInTime = 0.0f;
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
			float volume_;
			

		public:
			PlayingTrack(const TrackInfo&,uint32_t position = 0,float fadeingVolume = 1,float volume = 1);
			void SetVolume(float v);
			bool Update() override;

			void FadeOutAndStop(float time);
		};

	private:
		std::deque<TrackInfo> playQueue_;	// ͷ�����ǡ����ڲ��ŵ�track��

		Snowing::Scene::Group<PlayingTrack> playground_;
		const PlayingTrack* mainlyTrack_ = nullptr;

		PlayingTrack* getPlayingTrack();
		void updateCurrentPlayingTrack();

	public:
		// schedule break loop
		enum class BreakTime : uint32_t
		{
			Now,
			NextBar,
			NextBeat
		};

		struct BreakOnNextLoop{};
		struct BreakWhenJumpTime
		{
			BreakTime whenJump;
			float fadeOutTime = 0.1f;
		};
		using BreakLoopSchedule = std::variant<BreakOnNextLoop, BreakWhenJumpTime>;

	private:
		std::deque<BreakLoopSchedule> breakSchedule_;
		float volume_ = 1;

	public:
		template <typename TrackInfo>
		void AddToPlayQueue(TrackInfo&& track,float fadeTime = 0.0f)
		{
			playQueue_.emplace_back(std::forward<TrackInfo>(track));
			updateCurrentPlayingTrack();
		}
		void UpdateScheduledBreakLoop();
		bool Update() override;

		// ������д����ŵ�Track��ֻ�������ڲ��ŵ�Track
		void ClearQueueTail();

		void Reset();

		void SetVolume(float v);

		const std::deque<BreakLoopSchedule>& BreakScheduleQueue();

		void ScheduleBreakLoop(BreakLoopSchedule schedule);

		const Snowing::Blob* GetPlaying();

		size_t GetQueuedTrackCount() const;
		const Snowing::Blob* GetQueuedTrackBlob(size_t index) const;

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
