#pragma once
#include "AudioChannel.h"
#include <Snowing.h>
#include <future>

namespace Yukimi
{
	class[[nodiscard]] BGMPlayer final : 
		public Snowing::SingleInstance<BGMPlayer>,
		private virtual Snowing::Scene::Group<AudioChannel>,
		public virtual Snowing::Scene::Object
	{
	private:
		// 不应该直接通过指针访问对象，仅用于从Group里判断某个对象是否为当前的播放对象
		AudioChannel* currentChannel_ = nullptr;

		const AudioChannel::AudioLoader loader_;

		void stopAllChannels(float fadeTime);
	public:

		BGMPlayer(AudioChannel::AudioLoader loader);

		// 播放BGM
		void Play(std::string&& name, float fadeTime = 0, uint32_t begin = 0);

		// 停止BGM
		void Stop(float fadeTime = 0);

		// 从当前播放位置，淡出淡入切换到另外一个BGM
		void FadeTo(Snowing::AssetName, float fadeTime = 0);

		// 暂停
		void Pause(float fadeTime = 0);

		// 从暂停中恢复
		void Resume(float fadeTime = 0);

		bool Update() override;
	};
}