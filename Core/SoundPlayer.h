#pragma once
#include "Blob.h"
#include <cstdint>
#include <cassert>
#include <optional>

namespace Snowing::Audio
{
	struct alignas(4) Metadata
	{
		float Bpm = 120;
		uint32_t BeatsPerBar = 4;
		int32_t BeatOffset = 0;
	};

	template <typename TImpl>
	class[[nodiscard]] SoundPlayerInterface final
	{
	private:
		TImpl impl_;

	public:
		SoundPlayerInterface()
		{}

		//传入要播放的Blob和开始播放的位置
		void Play(const Blob *blob,uint32_t begin = 0)
		{
			impl_.Play(blob,begin);
		}

		[[nodiscard]]
		uint32_t GetPosition() const
		{
			return impl_.GetPosition();
		}

		//立即停止并返回当时正在播放的位置
		uint32_t Stop()
		{
			const auto ret = impl_.GetPosition();
			impl_.Stop();
			return ret;
		}

		//播放时返回被播放的Blob，停止时返回nullptr
		[[nodiscard]]
		const Blob * GetPlaying() const
		{
			return impl_.GetPlaying();
		}

		//范围：0~1
		void SetVolume(float v)
		{
			assert(v >= 0 && v <= 1);
			impl_.SetVolume(v);
		}

		//范围：-1~1（-1为极左，1为极右）
		void SetPan(float pan)
		{
			assert(pan >= -1 && pan <= 1);
			impl_.SetPan(pan);
		}

		//范围：0~正无穷
		void SetSpeed(float spd)
		{
			assert(spd > 0);
			impl_.SetSpeed(spd);
		}

		// 获取音量
		[[nodiscard]]
		float GetVolume() const
		{
			return impl_.GetVolume();
		}

		// 获取实时音量
		[[nodiscard]]
		float GetRealtimeVolume() const
		{
			return impl_.GetRealtimeVolume();
		}

		void ResetStates()
		{
			SetSpeed(1);
			SetPan(0);
			SetVolume(1);
		}

		// 获取元数据
		Metadata GetMetadata() const
		{
			return impl_.GetMetadata();
		}
	};
}
