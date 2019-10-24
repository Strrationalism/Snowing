#pragma once
#include "Handler.h"
#include "SingleInstance.h"

#ifdef USE_XAUDIO2
#pragma comment(lib,"xaudio2_8.lib")
#endif

namespace Snowing::PlatformImpls::WindowsImpl::XAudio2
{
	void XAVoiceDeleter(void*);

	class XADevice final : SingleInstance<XADevice>
	{
	private:
		static inline XADevice *current = nullptr;
		Handler xaEngine_;
		Handler masterTrack_;
	public:
		XADevice() noexcept;
		~XADevice();

		bool Avaliable() const;

		const Handler& GetHandler() const;
		const Handler& GetMasterTrack() const;
		static XADevice& Get();

		XADevice& operator = (XADevice&&) = delete;
	};
}

