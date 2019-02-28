#include <Snowing.h>
#include <iostream>
#include "PlatformImpls.h"
#include "KeyKeepTranslator.h"
#include <cassert>

using namespace Snowing::Input;
using namespace Snowing::PlatformImpls::WindowsImpl;
using namespace std;

#define PrintCommandKeyMessage(X) \
case CommandMessage::CommandID::X:\
	cout<<#X;\
break

int __cdecl main()
{
	Snowing::PlatformImpls::WindowsImpl::WindowImpl wndImpl{ L"TestWindowsInput", {320,240} };
	Snowing::PlatformImpls::WindowsImpl::D3D::Device grap{ wndImpl.GetHWND() };
	Snowing::Engine engine;

	assert(&engine == &Snowing::Engine::Get());

	Snowing::Input::TriggerWatcher Wheel{ &Input::Get() ,MouseWheel{} };

#define KEYWATCHER(KeyValue,KeyName) \
	Snowing::Input::KeyWatcher<Input,decltype(KeyValue)> KeyName{ &Input::Get(),KeyValue }

	KEYWATCHER(Snowing::Input::KeyboardKey::F1, F1);
	KEYWATCHER(Snowing::Input::KeyboardKey::F2, F2);
	KEYWATCHER(Snowing::Input::KeyboardKey::F3, F3);
	KEYWATCHER('Z', Z);
	KEYWATCHER('X', X);
	KEYWATCHER('C', C);
	KEYWATCHER(Snowing::Input::MouseKey::Left, MouseLeft);
	KEYWATCHER(Snowing::Input::MouseKey::Right, MouseRight);
	KEYWATCHER(Snowing::Input::MouseKey::Middle, MouseMiddle);
	KEYWATCHER(Snowing::Input::MouseKey::SideKeyA, SideKeyA);
	KEYWATCHER(Snowing::Input::MouseKey::SideKeyB, SideKeyB);
#undef KEYWATCHER

	Snowing::Input::KeyKeepTranslator<decltype(X)> XKeeper{ &X,1.0f,0.2f };

	Snowing::Input::KeyWatcher WinFocused{ &Snowing::Graphics::Window::Get() ,Snowing::Input::WindowFocused{} };

	std::cout << "= Press F1 to print current states =" << std::endl;

	engine.Run([&]{
		XKeeper.Update();
		if (XKeeper.Event())
		{
			std::cout << "X Event!" << std::endl;
		}

		F1.Update();
		if (F1.JustPress())
		{
			cout << endl;
			cout << "=== States ===" << endl;
			cout << "WindowFocused:" << Snowing::Graphics::Window::Get().KeyPressed(Snowing::Input::WindowFocused{}) << endl;
			
			cout << "MousePosition:";
			if (Input::Get().Position(Snowing::Input::MousePosition{}).has_value())
				cout
				<< Input::Get().Position(Snowing::Input::MousePosition{})->x << ','
				<< Input::Get().Position(Snowing::Input::MousePosition{})->y << endl;
			else
				cout << "Leave!" << endl;

			cout << endl;
			cout << "==============" << endl;
			cout << endl;
		}

		Wheel.Update();
		if (Wheel.IsNew())
			cout << "Mouse Wheel:" << Wheel.Value() << endl;
		

#define WATCHKEY(Key) {\
		Key.Update();\
		if(Key.JustPress()) \
			cout << "Key:" << #Key << " JustPressed!"<<std::endl; \
		else if (Key.JustRelease()) \
			cout << "Key:" << #Key << " JustReleased!"<<std::endl;}

		WATCHKEY(F2);
		WATCHKEY(F3);
		WATCHKEY(Z);
		WATCHKEY(X);
		WATCHKEY(C);
		WATCHKEY(MouseLeft);
		WATCHKEY(MouseRight);
		WATCHKEY(MouseMiddle);
		WATCHKEY(SideKeyA);
		WATCHKEY(SideKeyB);
#undef WATCHKEY
		WinFocused.Update();
		if (WinFocused.JustPress())
			cout << "Window Focused!" << endl;
		else if (WinFocused.JustRelease())
			cout << "Window Unfocused!" << endl;
	});
	return 0;
}
