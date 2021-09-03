#pragma once
#include <cassert>
#include <vector>
#include "InputParam.h"
#include "Platforms.h"
#include "SingleInstance.h"

namespace Snowing::Input
{
	template <typename TImpl>
	class[[nodiscard]] InputInterface final : public SingleInstance<InputInterface<TImpl>>
	{
	public:
		template <typename KeyType>
		[[nodiscard]]
		static bool KeyPressed(KeyType k)
		{
			return TImpl::Get().KeyPressed(k);
		}

		template <typename PositionType>
		[[nodiscard]]
		static std::optional<Math::Vec2f> Position(PositionType t)
		{
			return TImpl::Get().Position(t);
		}

		[[nodiscard]]
		static size_t GetMaxTouchInputCount()
		{
			return TImpl::Get().GetMaxTouchInputCount();
		}

		template <typename TriggerType>
		[[nodiscard]]
		static float Trigger(TriggerType t)
		{
			return TImpl::Get().Trigger(t);
		}
	};
}
