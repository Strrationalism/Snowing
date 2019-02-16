#pragma once
#include <Blob.h>
#include "Handler.h"
#include "BKDRHash.h"
#include <SoundPlayer.h>

namespace Snowing::PlatformImpls::WindowsImpl::XAudio2
{
	class XASoundPlayer final
	{
	private:
		const Blob *playingBlob_ = nullptr;
		Handler xaVoice_;

		std::uint32_t beginSample_ = 0;
	public:
		XASoundPlayer();
		~XASoundPlayer();

		void Play(const Blob * blob,uint32_t begin = 0);
		void Stop();
		uint32_t GetPosition() const;
		const Blob * GetPlaying() const;

		void SetVolume(float v);
		void SetPan(float pan);
		void SetSpeed(float pitch);
	};
}

namespace Snowing::Audio
{
	using SoundPlayer = SoundPlayerInterface<Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer>;
}
