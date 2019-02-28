#define USE_XAUDIO2			//����Ҫ��Windows��ʹ��XAudio2��Ϊ��Ƶ���ʱ����PlatformImpls.hͷ�ļ�֮ǰ����˺�
#include <Snowing.h>

using namespace Snowing;

int main()
{
	// ��������ʵ��
	auto engine = PlatformImpls::WindowsImpl::MakeEngine(
		L"BGMPlayer", 
		{ 400,300 },
		true);

	// ����XAudio2���ʵ��
	PlatformImpls::WindowsImpl::XAudio2::XADevice xaDevice;
	
	// ����BGM����
	auto music = LoadAsset("Dir/Sound/ExBoss.snd");
	// ���ڴ�BGM���ݣ�
	// ��������Ϊ����������־����StageEx Boss BGM��
	// �������ӽ�����ʾ�������ö��ηַ�������������;��
	
	// ��������������
	Audio::SoundPlayer player;

	// ����BGM
	player.Play(&music);

	// ��ӡ˵��
	Log<wchar_t>(L"����������¼����ɵ���������");
	Log<wchar_t>(L"����������Ҽ����ɵ������ࡣ");
	Log<wchar_t>(L"���¿ո����ͣ/�ָ����š�");
	Log<wchar_t>(L"����Z/X�����Ե��������ٶȡ�");

	// ���������
	Input::KeyWatcher up{ &Input::Input::Get(),Input::KeyboardKey::Up };
	Input::KeyWatcher down{ &Input::Input::Get(),Input::KeyboardKey::Down };
	Input::KeyWatcher left{ &Input::Input::Get(),Input::KeyboardKey::Left };
	Input::KeyWatcher right{ &Input::Input::Get(),Input::KeyboardKey::Right };
	Input::KeyWatcher space{ &Input::Input::Get(),Input::KeyboardKey::Space };
	Input::KeyWatcher z{ &Input::Input::Get(),'Z' };
	Input::KeyWatcher x{ &Input::Input::Get(),'X' };

	// ����
	float volume = 1.0f;

	// ����
	float pan = 0.0f;

	// �����ٶ�
	float spd = 1.0f;

	// ��ͣʱʹ�õĲ��Ž���
	uint32_t position = 0;

	// ������ѭ��
	Engine::Get().Run([&] {

		// ����ÿ�����������
		up.Update();
		down.Update();
		left.Update();
		right.Update();
		space.Update();
		z.Update();
		x.Update();

		// ��Ӧ���°�������������
		if (up.JustPress())
		{
			volume = std::clamp(volume + 0.1f, 0.0f, 1.0f);
			player.SetVolume(volume);
		}

		if (down.JustPress())
		{
			volume = std::clamp(volume - 0.1f, 0.0f, 1.0f);
			player.SetVolume(volume);
		}

		// ��Ӧ���Ҽ�����������
		if (left.JustRelease())
		{
			pan = std::clamp(pan - 0.1f, -1.0f, 1.0f);
			player.SetPan(pan);
		}

		if (right.JustRelease())
		{
			pan = std::clamp(pan + 0.1f, -1.0f, 1.0f);
			player.SetPan(pan);
		}

		// ��ӦZX�������ò����ٶ�
		if (z.JustRelease())
		{
			spd = std::clamp(spd - 0.1f, 0.001f, 10.0f);
			player.SetSpeed(spd);
		}

		if (x.JustRelease())
		{
			spd = std::clamp(spd + 0.1f, 0.001f, 10.0f);
			player.SetSpeed(spd);
		}

		// ��Ӧ�ո������ͣ/��������
		if (space.JustPress())
		{
			if (player.GetPlaying())			// ������ڲ���
				position = player.Stop();		// ��ͣ�����浱ǰ����λ��
			else								// ����ǰΪ������ͣ��״̬
				player.Play(&music, position);	// �Ӷϵ��������
		}
	});

	return 0;
}