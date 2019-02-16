#pragma once
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include <Controller.h>
#include <Handler.h>
#include <DataBox.h>
#include <vector>
#include <SingleInstance.h>

namespace Snowing::PlatformImpls::WindowsDInput
{
	constexpr int KeyCounts = 32;
	constexpr int ArrowCounts = 4;
	constexpr int TriggerCounts = 6;

	class DInputControllerImpl final
	{
	private:
		constexpr static size_t StateSize = 80;
		WindowsImpl::Handler device_;
		WindowsImpl::DataBox<StateSize> states_;
	public:
		DInputControllerImpl(WindowsImpl::Handler&& hnd);
		~DInputControllerImpl();

		DInputControllerImpl(DInputControllerImpl&&) = default;
		DInputControllerImpl() = delete;

		bool Update();

		Math::Vec2f Thumb(Input::ClassicGamepadThumbs a) const;
		bool KeyPressed(int k) const;	//注意：最大为KeyCounts
		bool KeyPressed(Input::ClassicGamepadArrow arrow,int arrowID = 0) const;	//注意：ID最大为ArrowCounts

		float Trigger(int t) const;
		
	};

	using DInputCntroller = Snowing::Input::ControllerInterface<DInputControllerImpl>;

	class DInputContext final : public SingleInstance<DInputContext>
	{
	private:
		WindowsImpl::Handler context_;

	public:
		DInputContext();

		[[nodiscard]]
		std::vector<DInputCntroller> GetControllers(bool exceptXInputDevives = true) const;

		DInputContext(const DInputContext&) = delete;
		DInputContext(DInputContext&&) = delete;
		DInputContext& operator= (const DInputContext&) = delete;
		DInputContext& operator= (DInputContext&&) = delete;
	};
}

