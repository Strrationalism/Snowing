#pragma once
#include "AudioChannel.h"

namespace Yukimi
{
	class[[nodiscard]] SEPlayer final :
		private virtual Snowing::Scene::Group<AudioChannel>,
		public virtual Snowing::Scene::Object,
		public Snowing::SingleInstance<SEPlayer>
	{
	public:
		void Play(Snowing::AssetName se,float pan = 0,float volume = 1);
		void FadeOutAll(float fade);

		bool Update() override;
	};
}
