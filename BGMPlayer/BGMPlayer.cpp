#define USE_XAUDIO2			//当需要在Windows下使用XAudio2作为音频后端时，在PlatformImpls.h头文件之前定义此宏
#include <Snowing.h>
#include <PlatformImpls.h>

using namespace Snowing;

int main()
{
	// 创建引擎实例
	auto engine = PlatformImpls::WindowsImpl::MakeEngine(
		L"BGMPlayer", 
		{ 400,300 },
		true);

	// 创建XAudio2后端实例
	PlatformImpls::WindowsImpl::XAudio2::XADevice xaDevice;
	
	// 加载BGM数据
	auto music = LoadAsset("Dir/Sound/ExBoss.snd");
	// 关于此BGM数据：
	// 这首曲子为《东方蝶梦志》的StageEx Boss BGM。
	// 这首曲子仅用于示例，不得二次分发或用于其他用途。
	
	// 创建声音播放器
	Audio::SoundPlayer player;

	// 播放BGM
	player.Play(&music);

	// 打印说明
	Log<wchar_t>(L"按方向键上下键即可调整音量。");
	Log<wchar_t>(L"按方向键左右键即可调整声相。");
	Log<wchar_t>(L"按下空格键暂停/恢复播放。");
	Log<wchar_t>(L"按下Z/X键可以调整播放速度。");

	// 按键检测器
	Input::KeyWatcher up{ &Input::Input::Get(),Input::KeyboardKey::Up };
	Input::KeyWatcher down{ &Input::Input::Get(),Input::KeyboardKey::Down };
	Input::KeyWatcher left{ &Input::Input::Get(),Input::KeyboardKey::Left };
	Input::KeyWatcher right{ &Input::Input::Get(),Input::KeyboardKey::Right };
	Input::KeyWatcher space{ &Input::Input::Get(),Input::KeyboardKey::Space };
	Input::KeyWatcher z{ &Input::Input::Get(),'Z' };
	Input::KeyWatcher x{ &Input::Input::Get(),'X' };

	// 音量
	float volume = 1.0f;

	// 声相
	float pan = 0.0f;

	// 播放速度
	float spd = 1.0f;

	// 暂停时使用的播放进度
	uint32_t position = 0;

	// 启动主循环
	Engine::Get().Run([&] {

		// 更新每个按键检测器
		up.Update();
		down.Update();
		left.Update();
		right.Update();
		space.Update();
		z.Update();
		x.Update();

		// 响应上下按键，设置音量
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

		// 响应左右键，设置声相
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

		// 响应ZX键，设置播放速度
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

		// 响应空格键，暂停/继续播放
		if (space.JustPress())
		{
			if (player.GetPlaying())			// 如果正在播放
				position = player.Stop();		// 暂停并保存当前播放位置
			else								// 否则当前为正在暂停的状态
				player.Play(&music, position);	// 从断点继续播放
		}
	});

	return 0;
}