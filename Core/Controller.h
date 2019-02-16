#pragma once
#include "Vec2.h"
#include "Platforms.h"

// 注意：所有的模拟输入均未处理死区，应当手动处理。

namespace Snowing::Input
{
	enum class ClassicGamepadArrow
	{
		Up, Down, Left, Right
	};

	enum class ClassicGamepadKeys
	{
		A,B,X,Y,L,R,
		Start,SelectOrBack,
		LThumb,RThumb
	};

	enum class ClassicGamepadThumbs
	{
		LThumb,RThumb
	};

	enum class ClassicGamepadTriggers
	{
		LTrigger,RTrigger
	};

	struct VibrationLeftRight
	{
		float l, r;
	};

	struct VibrationStop
	{};

	template <typename TImpl>
	class[[nodiscard]] ControllerInterface final
	{
	private:
		TImpl impl_;
	public:

		ControllerInterface(TImpl&& impl) :
			impl_{ std::move(impl) }
		{
		}

		bool Update()
		{
			return impl_.Update();
		}

		template <typename ...KeyType>
		[[nodiscard]]
		bool KeyPressed(KeyType&&... k) const
		{
			return impl_.KeyPressed(std::forward<KeyType>(k)...);
		}

		template <typename ThumbType>
		[[nodiscard]]
		Math::Vec2f Thumb(ThumbType&& a) const
		{
			return impl_.Thumb(std::forward<ThumbType>(a));
		}

		template <typename TriggerType>
		[[nodiscard]]
		float Trigger(TriggerType&& t) const
		{
			return impl_.Trigger(std::forward<TriggerType>(t));
		}

		template <typename VibrationParam>
		void Vibration(VibrationParam&& rumble) const
		{
			impl_.Vibration(std::forward<VibrationParam>(rumble));
		}

	};
}
