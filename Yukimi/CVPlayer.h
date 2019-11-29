#pragma once
#include "AudioChannel.h"

namespace Yukimi
{
	class[[nodiscard]] CVPlayer final:
		private virtual Snowing::Scene::Group<AudioChannel>,
		public virtual Snowing::Scene::Object
	{
	private:
		const AudioChannel::AudioLoader loader_;
		AudioChannel* activeChannel_;
		float volume_ = 1;

	public:
		CVPlayer(AudioChannel::AudioLoader loader);

		void Play(Snowing::AssetName ass);
		void FadeOutAll();
		bool Update() override;
		void VolumeDown();
		void VolumeUp();
		float GetRealtimeVolume() const;

		void SetVolumePan(float v,float pan);
	};
}

