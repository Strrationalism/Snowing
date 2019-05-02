#pragma once
#include <optional>
#include "Snowing.h"

namespace Snowing::Scene
{
	template <typename TSoundPlayer,size_t Hz = 44100>
	class Metronome final : public Snowing::Scene::Object
	{
	public:
		struct Time
		{
			size_t Bar, Beat;
			long double Remainder;
		};

	private:
		float bpm_ = 120;
		size_t beatPerBar_ = 4;

		bool isBeat_ = false;
		Time prevTime_ = { 0,0 };

		TSoundPlayer* soundPlayer_ = nullptr;

	public:

		bool IsBeat() const
		{
			return isBeat_;
		}

		void Reset()
		{
			isBeat_ = false;
			prevTime_.Bar = 0;
			prevTime_.Beat = 0;
			prevTime_.Remainder = 0;
		}

		void Reset(TSoundPlayer* player,float bpm, size_t beatPerBar)
		{
			soundPlayer_ = player;
			bpm_ = bpm;
			beatPerBar_ = beatPerBar;
			Reset();
		}

		Time GetTime() const
		{
			const long double beatsld = 
				(bpm_ * static_cast<long double(soundPlayer_->GetPosition())) / 
				static_cast<long double>(Hz) / 
				60.0l;
			const size_t beats = static_cast<size_t>(beatsld);
			return {
				beats / beatPerBar_,
				beats % beatPerBar_,
				beatsld - beats
			};
		}

		bool Update() override
		{
			isBeat_ = false;
			const auto t = GetTime();
			isBeat_ = t.Beat != prevTime_.Beat;
			prevTime_ = t;

			return true;
		}

		Metronome(TSoundPlayer* player,float bpm, size_t beatPerBar)
		{
			Reset(player,bpm, beatPerBar);
		}
	};
}
