#pragma once
#include <future>
#include <Snowing.h>

namespace Yukimi
{
	class[[nodiscard]] AudioChannel final :
	        public Snowing::MemPool<AudioChannel>,
		public Snowing::Scene::Object
	{
	private:
		std::future<Snowing::Blob> soundLoading_;
		Snowing::Blob sound_;
		Snowing::Audio::SoundPlayer player_;
		Snowing::Scene::Tween<float> fadeVolume_, mainVolume_ = 1;
		Snowing::Scene::VirtualConditionTask onSoundLoaded_;

		std::optional<uint32_t> pausePoint_;

		std::optional<Snowing::Scene::Metronome<>> metronome_;

	public:

		using AudioLoader = Snowing::Blob(*)(Snowing::AssetName);

		// 异步加载并播放一个声音
		AudioChannel(AudioLoader loader,std::string&& name,float fadeIn = 0,uint32_t begin = 0,float pan = 0);

		// 从已有Blob创建声音
		AudioChannel(Snowing::Blob&& soundBlob, float fadeIn = 0.0f, uint32_t begin = 0, float pan = 0);

		// 将自己的Blob转换为借用，并转移出Blob的所有权
		Snowing::Blob MoveOutBlob();

		// 停止播放声音
		void Stop(float fadeOut = 0);

		// 获取播放位置
		uint32_t GetPosition() const;

		// 暂停
		void Pause(float fadeOut = 0);

		// 从暂停中恢复
		void Resume(float fadeIn = 0);

		// 淡入淡出调整音量
		void FadeVolume(float volume,float fade = 0);

		// 当声音播放结束或被完全停止后，对象死亡
		bool Update() override;

		// 获取实时音量
		float GetRealtimeVolume() const;

		// 获取节拍器
		std::optional<const Snowing::Scene::Metronome<>*> GetMetronome() const;
	};
}
