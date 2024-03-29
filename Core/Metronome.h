#pragma once
#include <optional>
#include "Snowing.h"

namespace Snowing::Scene
{
	template <typename TSoundPlayer = Snowing::Audio::SoundPlayer,size_t Hz = 44100>
	class Metronome final : public Snowing::Scene::Object
	{
	public:
		struct Time
		{
			size_t Bar = 0, Beat = 0;
			long double Remainder = 0;

			bool operator >= (const Time& x) const
			{
				if (Bar == x.Bar)
				{
					if (Beat == x.Beat)
						return Remainder >= x.Remainder;
					else
						return Beat >= x.Beat;
				}
				else
					return Bar >= x.Bar;
			}
		};

	private:
		float bpm_ = 120;
		size_t beatPerBar_ = 4;

		bool isBeat_ = false;
		Time prevTime_ = { 0,0 };

		TSoundPlayer* soundPlayer_ = nullptr;
		int offsetBeat_ = 0;

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

		void Reset(TSoundPlayer* player,float bpm, size_t beatPerBar, int offsetBeat)
		{
			soundPlayer_ = player;
			bpm_ = bpm;
			beatPerBar_ = beatPerBar;
			offsetBeat_ = offsetBeat;
			Reset();
		}

		Time GetTime() const
		{
			const long double beatsld = 
				((bpm_ * static_cast<long double>(soundPlayer_->GetPosition())) /
				static_cast<long double>(Hz) / 
				60.0l) + static_cast<long double>(offsetBeat_);
			const size_t beats = static_cast<size_t>(beatsld);
			return {
				beats / beatPerBar_,
				beats % beatPerBar_,
				beatsld - beats
			};
		}

		bool Update() override
		{
			const auto t = GetTime();
			
			isBeat_ = false;
			if(t.Bar != prevTime_.Bar || t.Beat != prevTime_.Beat)
				isBeat_ = true;
			
			prevTime_ = t;

			return true;
		}

		Metronome(TSoundPlayer* player,float bpm, size_t beatPerBar,int offsetBeat = 0)
		{
			Reset(player,bpm, beatPerBar, offsetBeat);
		}
	};
}
