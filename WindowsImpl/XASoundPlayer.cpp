#include "stdafx.h"
#include "XASoundPlayer.h"
#include "XADevice.h"
#include <xaudio2.h>
#include "COMHelper.h"

struct SoundHead
{
	uint32_t headSize;
	uint32_t loopSize;
};


constexpr static WAVEFORMATEX format
{
	WAVE_FORMAT_PCM,
	2,
	44100,
	2*2*44100,		//nBlockAlign * nAvgBytesPerSec
	2*2,	//bytes*channels
	16,
	0
};

Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer::XASoundPlayer()
{
	if (XADevice::Get().Avaliable())
	{
		IXAudio2SourceVoice *sv;
		COMHelper::AssertHResult("Can not create source voice.",
			XADevice::Get().GetHandler().Cast<IUnknown*, IXAudio2*>()->CreateSourceVoice(
				&sv, &format));
		xaVoice_ = { static_cast<IXAudio2Voice*>(sv),XAVoiceDeleter };
	}
}

Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer::~XASoundPlayer()
{
	if (XADevice::Get().Avaliable())
		xaVoice_.Cast<IXAudio2Voice*, IXAudio2SourceVoice*>()->Stop();
}

void Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer::Play(const Blob * blob,uint32_t begin)
{
	if (XADevice::Get().Avaliable())
	{
		const auto sv = xaVoice_.Cast<IXAudio2Voice*, IXAudio2SourceVoice*>();
		const auto head = blob->Get<SoundHead*>(0);

		XAUDIO2_BUFFER buf;
		buf.AudioBytes = head->headSize + head->loopSize;
		buf.Flags = XAUDIO2_END_OF_STREAM;
		buf.pAudioData = blob->Get<BYTE*>(sizeof(SoundHead));
		buf.LoopBegin = head->loopSize ? head->headSize / 4 : 0;
		buf.LoopCount = head->loopSize ? XAUDIO2_LOOP_INFINITE : 0;
		buf.LoopLength = head->loopSize / 4;
		buf.pContext = nullptr;
		buf.PlayBegin = begin;
		buf.PlayLength = 0;

		beginSample_ = begin;

		COMHelper::AssertHResult("Can not submit XAudio voice source buffer.",
			sv->SubmitSourceBuffer(&buf));

		sv->Start();

		playingBlob_ = blob;
	}
}

void Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer::Stop()
{
	if (XADevice::Get().Avaliable())
	{
		xaVoice_.Cast<IXAudio2Voice*, IXAudio2SourceVoice*>()->Stop();
		playingBlob_ = nullptr;
	}
}

uint32_t Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer::GetPosition() const
{
	if (XADevice::Get().Avaliable())
	{
		//TODO:´ý²âÊÔµÄÊµÏÖ
		if (!playingBlob_) return 0;
		const auto dataHead = playingBlob_->Get<SoundHead*>(0);
		const auto headSamples = dataHead->headSize / 4;
		const auto loopSamples = dataHead->loopSize / 4;

		const auto p = xaVoice_.Cast<IXAudio2Voice*, IXAudio2SourceVoice*>();
		XAUDIO2_VOICE_STATE state;
		p->GetState(&state);

		auto pos = static_cast<uint32_t>(state.SamplesPlayed) + beginSample_;

		while (pos >= headSamples + loopSamples)
			pos -= loopSamples;
		
		return pos;
	}
	else
		return 0;
}

const Snowing::Blob* Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer::GetPlaying() const
{
	if (XADevice::Get().Avaliable())
	{
		const auto p = xaVoice_.Cast<IXAudio2Voice*, IXAudio2SourceVoice*>();
		XAUDIO2_VOICE_STATE state;
		p->GetState(&state);
		return state.BuffersQueued > 0 ? playingBlob_ : nullptr;
	}
	else
		return nullptr;
}

void Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer::SetVolume(float v)
{
	if (XADevice::Get().Avaliable())
	{
		const auto sv = xaVoice_.Cast<IXAudio2Voice*, IXAudio2SourceVoice*>();
		const auto res =
			sv->SetVolume(v);

#ifdef _DEBUG
		COMHelper::AssertHResult("Can not set volume.", res);
#endif // _DEBUG
	}
}

void Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer::SetPan(float pan)
{
	if (XADevice::Get().Avaliable())
	{
		const float channels[] = { 0.5f - pan / 2,0.5f + pan / 2 };

		const auto res =
			xaVoice_.Cast<IXAudio2Voice*, IXAudio2SourceVoice*>()->
			SetChannelVolumes(2, channels);

#ifdef _DEBUG
		COMHelper::AssertHResult("Can not set pan.", res);
#endif
	}
}

void Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer::SetSpeed(float pitch)
{
	if (XADevice::Get().Avaliable())
	{
		const auto sv = xaVoice_.Cast<IXAudio2Voice*, IXAudio2SourceVoice*>();
		const auto res =
			sv->SetFrequencyRatio(pitch);

#ifdef _DEBUG
		COMHelper::AssertHResult("Can not set res.", res);
#endif // _DEBUG
	}
}

float Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer::GetVolume() const
{
	if (XADevice::Get().Avaliable())
	{
		const auto sv = xaVoice_.Cast<IXAudio2Voice*, IXAudio2SourceVoice*>();

		float v;
		sv->GetVolume(&v);

		return v;
	}
	else
		return 0.0f;
}

float Snowing::PlatformImpls::WindowsImpl::XAudio2::XASoundPlayer::GetRealtimeVolume() const
{
	const auto blob = GetPlaying();

	if (blob)
	{
		const size_t sampleCount = blob->Size() / (format.wBitsPerSample / 8) / format.nChannels;
		const size_t currentSampleID = GetPosition();
		constexpr size_t sampleNeed = 4096;

		const auto beginSample = std::clamp(currentSampleID - sampleNeed / 2, 0u, sampleCount);
		const auto endSample = std::clamp(currentSampleID + sampleNeed / 2, 0u, sampleCount);

		float volume = 0;
		for (size_t currentSampleID = beginSample; currentSampleID < endSample; ++currentSampleID)
		{
			const size_t bytePosition = currentSampleID * (format.wBitsPerSample / 8) * format.nChannels;
			const auto pSample = blob->Get<uint16_t*>(bytePosition);

			for (size_t channel = 0; channel < format.nChannels; ++channel)
			{
				const float vol = static_cast<float>(pSample[channel]) / 65535.0f;
				volume += vol;
			}
		}


		return volume / (endSample - beginSample) / format.nChannels;
	}
	else
		return 0;
}

