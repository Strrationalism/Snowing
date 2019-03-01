#include <Snowing.h>
#include <XInputController.h>
#include <iostream>

using namespace Snowing;
using namespace Input;

PlatformImpls::WindowsImpl::WindowImpl win{ L"TestWindowsXInput",{320,240} };	//创建窗口
PlatformImpls::WindowsImpl::D3D::Device grap{ win.GetHWND() };
Engine engine;															//创建引擎对象
auto ctrls = PlatformImpls::WindowsXInput::GetConnectedControllers();	//获取已经连接的手柄列表

int main()
{
	std::cout << "注意：一次只能连接一个手柄！" << std::endl;
	std::cout << "按下A键打印摇杆数据。" << std::endl;
	std::cout << "按下L键开始左震动。" << std::endl;
	std::cout << "按下R键开始右震动。" << std::endl;
	std::cout << "按下X键停止震动。" << std::endl;

	if (ctrls.empty())
	{
		std::cout << "没有手柄！" << std::endl;
		return 0;
	}

	auto ctrl = std::move(ctrls.front());	//获取第一个手柄
	ctrls.clear();

	using KW = KeyWatcher<decltype(ctrl), Snowing::Input::ClassicGamepadKeys>;
	using KAW = KeyWatcher<decltype(ctrl), Snowing::Input::ClassicGamepadArrow>;

	KAW Up{ &ctrl,ClassicGamepadArrow::Up };
	KAW Down{ &ctrl,ClassicGamepadArrow::Down };
	KAW Left{ &ctrl,ClassicGamepadArrow::Left };
	KAW Right{ &ctrl,ClassicGamepadArrow::Right };
	KW A{ &ctrl,ClassicGamepadKeys::A };
	KW B{ &ctrl,ClassicGamepadKeys::B };
	KW X{ &ctrl, ClassicGamepadKeys::X };
	KW Y{ &ctrl,ClassicGamepadKeys::Y };
	KW L{ &ctrl,ClassicGamepadKeys::L };
	KW R{ &ctrl,ClassicGamepadKeys::R };
	KW LT{ &ctrl,ClassicGamepadKeys::LThumb };
	KW RT{ &ctrl,ClassicGamepadKeys::RThumb };
	KW Select{ &ctrl, ClassicGamepadKeys::SelectOrBack };
	KW Start{ &ctrl,ClassicGamepadKeys::Start };

	using T2K = ThumbToKey<decltype(ctrl), Snowing::Input::ClassicGamepadThumbs>;
	T2K LThumb{ &ctrl,ClassicGamepadThumbs::LThumb };
	T2K RThumb{ &ctrl,ClassicGamepadThumbs::RThumb };

	using TKW = KeyWatcher<T2K, Snowing::Input::ClassicGamepadArrow>;
	TKW LLeft{ &LThumb,ClassicGamepadArrow::Left };
	TKW LRight{ &LThumb,ClassicGamepadArrow::Right };
	TKW LUp{ &LThumb,ClassicGamepadArrow::Up };
	TKW LDown{ &LThumb,ClassicGamepadArrow::Down };
	TKW RLeft{ &RThumb,ClassicGamepadArrow::Left };
	TKW RRight{ &RThumb,ClassicGamepadArrow::Right };
	TKW RUp{ &RThumb,ClassicGamepadArrow::Up };
	TKW RDown{ &RThumb,ClassicGamepadArrow::Down };

	using TW = TriggerWatcher<decltype(ctrl), Snowing::Input::ClassicGamepadTriggers>;
	TW TL{ &ctrl,ClassicGamepadTriggers::LTrigger };
	TW TR{ &ctrl,ClassicGamepadTriggers::RTrigger };

	engine.Run([&]
	{
		if (!ctrl.Update())
		{
			std::cout << "已断开连接！" << std::endl;
			Engine::Get().Exit();
		}

		TL.Update(); if (TL.IsNew()) std::cout << "Trigger L:" << TL.Value() << std::endl;
		TR.Update(); if (TR.IsNew()) std::cout << "Trigger R:" << TR.Value() << std::endl;

#define W(Name) \
Name.Update(); \
if(Name.JustPress()) \
	std::cout << #Name " Just Pressed!" << std::endl; \
else if (Name.JustRelease()) \
	std::cout << #Name " Just Released!" << std::endl;

		W(Up);
		W(Down);
		W(Left);
		W(Right);
		W(B);
		W(Y);
		W(LT);
		W(RT);
		W(Select);
		W(Start);

		W(LLeft);
		W(LRight);
		W(LUp);
		W(LDown);
		W(RLeft);
		W(RUp);
		W(RDown);
		W(RRight);

		A.Update();
		if (A.JustPress())
		{
			auto tl = ctrl.Thumb(ClassicGamepadThumbs::LThumb);
			auto tr = ctrl.Thumb(ClassicGamepadThumbs::RThumb);
			std::cout << "L:" << tl.x << ',' << tl.y << " R:" << tr.x << ',' << tr.y << std::endl;
		}

		L.Update();
		R.Update();
		X.Update();
		if (L.JustPress())
			ctrl.Vibration(VibrationLeftRight{ 1,0 });
		if (R.JustPress())
			ctrl.Vibration(VibrationLeftRight{ 0,1 });
		if (X.JustPress())
			ctrl.Vibration(VibrationLeftRight{ });
	});

	return 0;
}
