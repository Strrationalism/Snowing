#pragma once
#include "AudioChannel.h"

namespace Yukimi
{
	class[[nodiscard]] CVPlayer final:
		private virtual Snowing::Scene::Group<AudioChannel>,
		public virtual Snowing::Scene::Object
	{
		constexpr static float speedTime = 0.25f;

		const AudioChannel::AudioLoader loader_;
	public:
		CVPlayer(AudioChannel::AudioLoader);

		void Play(Snowing::AssetName ass);
		void FadeOutAll();
		bool Update() override;
		void VolumeDown();
		void VolumeUp();
	private:
		inline void SetVolumeHelper(float volume);
	};
}

