#pragma once
#include <optional>
#include <Controller.h>
#include <InputParam.h>

namespace Yukimi
{
	enum class AVGKeys
	{
		Ok,
		Cancel,
		Skip,
		HideTextWindow,
		Click,
		Space
	};

	class AVGInputDevice
	{
	public:
		virtual ~AVGInputDevice() = default;

		virtual bool KeyPressed(AVGKeys) = 0;
		virtual bool KeyPressed(Snowing::Input::ClassicGamepadArrow) = 0;
		virtual void Vibration(Snowing::Input::VibrationLeftRight) = 0;
		virtual void Vibration(Snowing::Input::VibrationStop) = 0;
	};

	class AVGInput final : Snowing::NoCopyMove,public Snowing::SingleInstance<AVGInput>
	{
	private:
		class AVGInputKeyboardMouse final : public AVGInputDevice
		{
		public:
			bool KeyPressed(AVGKeys) override;
			bool KeyPressed(Snowing::Input::ClassicGamepadArrow) override;
			void Vibration(Snowing::Input::VibrationLeftRight) override;
			void Vibration(Snowing::Input::VibrationStop) override;
		};

		std::vector<std::unique_ptr<AVGInputDevice>> devices_;

	public:
		AVGInput();

		bool KeyPressed(AVGKeys) ;
		bool KeyPressed(Snowing::Input::ClassicGamepadArrow);
		void Vibration(Snowing::Input::VibrationLeftRight);
		void Vibration(Snowing::Input::VibrationStop);
		std::optional<Snowing::Math::Vec2f> Position(Snowing::Input::MousePosition);

		template <typename TDevice,typename ... TArgs>
		void Emplace(TArgs&& ...args)
		{
			devices_.emplace_back(std::make_unique<TDevice>(std::forward(args)...));
		}
	};
}
