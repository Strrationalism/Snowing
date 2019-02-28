#include <Snowing.h>
#include <DInputController.h>
#include <iostream>
using namespace Snowing;

int __cdecl main()
{
	PlatformImpls::WindowsImpl::WindowImpl wnd{ L"TestWindowsDInput",{320,240} };
	PlatformImpls::WindowsImpl::D3D::Device grap{ wnd.GetHWND() };
	Engine engine;
	PlatformImpls::WindowsDInput::DInputContext dinput;

	auto ctrls = dinput.GetControllers(false);


	if (ctrls.empty())
	{
		std::cout << "Ã»ÓÐÊÖ±ú£¡" << std::endl;
		return 0;
	}

	auto ctrl = std::move(ctrls.front());
	ctrls.clear();

	using KAW = Snowing::Input::KeyWatcher<decltype(ctrl), Snowing::Input::ClassicGamepadArrow>;
	KAW Left{ &ctrl,Input::ClassicGamepadArrow::Left };
	KAW Right{ &ctrl,Input::ClassicGamepadArrow::Right };
	KAW Up{ &ctrl,Input::ClassicGamepadArrow::Up };
	KAW Down{ &ctrl,Input::ClassicGamepadArrow::Down };

	using T2K = Snowing::Input::ThumbToKey<decltype(ctrl), Snowing::Input::ClassicGamepadThumbs>;
	T2K LThumb{ &ctrl,Input::ClassicGamepadThumbs::LThumb };
	T2K RThumb{ &ctrl,Input::ClassicGamepadThumbs::RThumb };

	using KTW = Snowing::Input::KeyWatcher<T2K, Snowing::Input::ClassicGamepadArrow>;
	KTW LLeft{ &LThumb,Input::ClassicGamepadArrow::Left };
	KTW LRight{ &LThumb,Input::ClassicGamepadArrow::Right };
	KTW LUp{ &LThumb,Input::ClassicGamepadArrow::Up };
	KTW LDown{ &LThumb,Input::ClassicGamepadArrow::Down };
	KTW RLeft{ &RThumb,Input::ClassicGamepadArrow::Left };
	KTW RRight{ &RThumb,Input::ClassicGamepadArrow::Right };
	KTW RUp{ &RThumb,Input::ClassicGamepadArrow::Up };
	KTW RDown{ &RThumb,Input::ClassicGamepadArrow::Down };

	std::vector<Snowing::Input::KeyWatcher<decltype(ctrl), int>> keys;
	for (int i = 0; i < Snowing::PlatformImpls::WindowsDInput::KeyCounts;++i)
		keys.emplace_back(&ctrl, i);

	std::vector<Snowing::Input::TriggerWatcher<decltype(ctrl), int>> trs;
	for (int i = 0; i < Snowing::PlatformImpls::WindowsDInput::TriggerCounts; ++i)
		trs.emplace_back(&ctrl, i);
	
#define W(Name) \
Name.Update(); \
if(Name.JustPress()) \
	std::cout << #Name " Just Pressed!" << std::endl; \
else if (Name.JustRelease()) \
	std::cout << #Name " Just Released!" << std::endl;

	engine.Run([&]
	{
		ctrl.Update();
		
		W(Left);
		W(Right);
		W(Up);
		W(Down);
		W(LLeft);
		W(LRight);
		W(LUp);
		W(LDown);
		W(RLeft);
		W(RRight);
		W(RUp);
		W(RDown);

		for (size_t i = 0; i < keys.size(); ++i)
		{
			keys[i].Update();
			if (keys[i].JustPress())
				std::cout << "Key " << i << " JustPressed!"<<std::endl;
			else if (keys[i].JustRelease())
				std::cout << "Key " << i << " Just Released!"<<std::endl;
		}

		for (size_t i = 0; i < trs.size(); ++i)
		{
			trs[i].Update();
			if (trs[i].IsNew())
				std::cout << "Trigger " << i << " Value:" << trs[i].Value() << std::endl;
		}
	});
	
	return 0;
}
