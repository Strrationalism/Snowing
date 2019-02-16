#pragma once
#include <variant>
#include <optional>
#include "Vec2.h"
#include "Coordinate2D.h"

namespace Snowing::Input
{
	/* 输入类型 */
	/* KeyPressed - 按键型输入，返回bool，表示是否按键已经被按下。 */
	/* Position - 指点设备输入，返回std::optional<Math::Vec2f>，表示当前指点设备位置，可能为空，左上角为(0,0),右下角为(1,1)。 */
	/* Trigger - 触发器输入，返回float，范围在0~1之间。 */
	/* Axis - 轴输入，返回Math::Vec2f，表示当前坐标，中心为(0,0)，左上角为(-1,-1)，右下角为(1,1)。*/

	//鼠标按键消息，作为KeyPressed参数
	enum class MouseKey : int
	{
		Left,
		Middle,
		Right,
		SideKeyA,
		SideKeyB
	};

	//鼠标坐标，作为Input的Position参数
	struct MousePosition final
	{
		constexpr static Math::Coordinate2DRect CoordinateSystem
		{
			{0,0},
			{1,1}
		};
	};

	//鼠标滚轮，作为Input的Trigger参数
	struct MouseWheel final
	{};

	//窗口被Focus消息，作为Window的KeyPressed参数
	struct WindowFocused
	{};

	//键盘消息，作为Input的KeyPressed参数
	enum class KeyboardKey : int
	{
		Escape,
		Enter,
		Space,

		Up,
		Down,
		Left,
		Right,

		LShift,
		RShift,
		LCtrl,
		RCtrl,

		F1,
		F2,
		F3
	};
}