#pragma once
#include "Menu.h"
#include "Box2D.h"
#include "Coordinate2D.h"

namespace Snowing::Scene
{
	template <typename TMenuItem,typename TPositionDevice,typename... TPositionArgs>
	class[[nodiscard]] MenuPositionController final : public Object
	{
	private:
		Menu<TMenuItem> &menu_;
		const TPositionDevice &device_;
		std::tuple<TPositionArgs...> deviceArgs_;
		const Math::Coordinate2DCenter menuCoord_, deviceCoord_;

		decltype(std::declval<TPositionDevice>().Position(std::declval<TPositionArgs>()...))
			lastDevicePosition_;

		void select(const std::optional<size_t>& index)
		{
			auto p = menu_.GetSelectedIndex();
			if (p != index)
			{
				if (index.has_value())
					menu_.Select(index.value());
				else
					menu_.Unselect();
			}
		}

	public:

		template <typename... TPositionArgs,typename TCoordA,typename TCoordB>
		MenuPositionController(
			Menu<TMenuItem> *menu,
			const TCoordA& menuCoord,
			TPositionDevice *device,
			const TCoordB& deviceCoord,
			TPositionArgs&&... args) :
			menu_{ *menu },
			device_{ *device },
			deviceArgs_ {std::forward<TPositionArgs>(args)... },
			menuCoord_{ Math::ConvertCoordnate2DToCenter(menuCoord) },
			deviceCoord_{ Math::ConvertCoordnate2DToCenter(deviceCoord) }
		{}

		bool Update() override
		{
			const auto p = 
				std::apply(
					[this](TPositionArgs... args) {return device_.Position(args...); }, 
					deviceArgs_);

			if (lastDevicePosition_ != p)
			{
				lastDevicePosition_ = p;

				if (!p.has_value())
					select(std::nullopt);
				else
				{
					const auto position =
						Math::ConvertPosition2DCoordinate(
							p.value(),
							deviceCoord_, menuCoord_);

					bool selected = false;
					for (size_t i = 0; i < menu_.Count(); ++i)
					{
						const auto& menuItem = menu_[i];
						if (IsPositionInBox(position, menuItem->GetBox()))
						{
							select(i);
							selected = true;
							break;
						}
					}

					if (!selected)
						select(std::nullopt);
				}
			}

			return true;
		}
	};
}
