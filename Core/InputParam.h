#pragma once
#include <variant>
#include <optional>
#include "Vec2.h"
#include "Coordinate2D.h"

namespace Snowing::Input
{
	/* �������� */
	/* KeyPressed - ���������룬����bool����ʾ�Ƿ񰴼��Ѿ������¡� */
	/* Position - ָ���豸���룬����std::optional<Math::Vec2f>����ʾ��ǰָ���豸λ�ã�����Ϊ�գ����Ͻ�Ϊ(0,0),���½�Ϊ(1,1)�� */
	/* Trigger - ���������룬����float����Χ��0~1֮�䡣 */
	/* Axis - �����룬����Math::Vec2f����ʾ��ǰ���꣬����Ϊ(0,0)�����Ͻ�Ϊ(-1,-1)�����½�Ϊ(1,1)��*/

	//��갴����Ϣ����ΪKeyPressed����
	enum class MouseKey : int
	{
		Left,
		Middle,
		Right,
		SideKeyA,
		SideKeyB
	};

	//������꣬��ΪInput��Position����
	struct MousePosition final
	{
		constexpr static Math::Coordinate2DRect CoordinateSystem
		{
			{0,0},
			{1,1}
		};
	};

	//�����֣���ΪInput��Trigger����
	struct MouseWheel final
	{};

	//���ڱ�Focus��Ϣ����ΪWindow��KeyPressed����
	struct WindowFocused final
	{};

	//����������
	struct TouchScreen final
	{
		constexpr static Math::Coordinate2DRect CoordinateSystem = 
			MousePosition::CoordinateSystem;
		struct AnyPoint final {};
		size_t TouchID;
	};

	//������Ϣ����ΪInput��KeyPressed����
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