#include "stdafx.h"
#include "XADevice.h"
#include "COMHelper.h"
#include <xaudio2.h>

using namespace Snowing::PlatformImpls::WindowsImpl;

Snowing::PlatformImpls::WindowsImpl::XAudio2::XADevice::XADevice() noexcept
{
	IXAudio2* xa;
	COMHelper::AssertHResult("Can not create xaudio engine.",
		XAudio2Create(&xa));
	xaEngine_ = { static_cast<IUnknown*>(xa),COMHelper::COMIUnknownDeleter };

	try 
	{
		IXAudio2MasteringVoice* mas;
		COMHelper::AssertHResult("Can not create mastering voice.",
			xa->CreateMasteringVoice(&mas));
		masterTrack_ = { static_cast<IXAudio2Voice*>(mas),XAVoiceDeleter };
	}
	catch (COMHelper::COMException)
	{
		
	}

	current = this;
}

Snowing::PlatformImpls::WindowsImpl::XAudio2::XADevice::~XADevice()
{
	current = nullptr;
}

bool Snowing::PlatformImpls::WindowsImpl::XAudio2::XADevice::Avaliable() const
{
	return masterTrack_.IsSome();
}

const Handler & Snowing::PlatformImpls::WindowsImpl::XAudio2::XADevice::GetHandler() const
{
	return xaEngine_;
}

const Handler & Snowing::PlatformImpls::WindowsImpl::XAudio2::XADevice::GetMasterTrack() const
{
	return masterTrack_;
}

Snowing::PlatformImpls::WindowsImpl::XAudio2::XADevice & Snowing::PlatformImpls::WindowsImpl::XAudio2::XADevice::Get()
{
	assert(current);
	return *current;
}

void Snowing::PlatformImpls::WindowsImpl::XAudio2::XAVoiceDeleter(void * v)
{
	static_cast<IXAudio2Voice*>(v)->DestroyVoice();
}

