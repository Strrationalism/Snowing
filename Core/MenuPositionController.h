#pragma once
#include "Menu.h"
#include "Box2D.h"
#include "Coordinate2D.h"

namespace Snowing::Scene::UI
{
	template <typename TMenuItem,typename TPositionDevice,typename... TPositionArgs>
	class[[nodiscard]] MenuPositionController final : public Object
	{
	private:
		Menu<TMenuItem> &menu_;
		const TPositionDevice &device_;
		std::tuple<TPositionArgs...> deviceArgs_;
		Math::Coordinate2DCenter menuCoord_, deviceCoord_;

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

		std::optional<Math::Vec2f> prevPosition_;

		bool prevPosOnItem_ = false;

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

		template <typename TCoord>
		void SetMenuCoordinateSystem(const TCoord& coord)
		{
			menuCoord_ = Math::ConvertCoordnate2DToCenter(coord);
		}

		void RefreshSelect()
		{
			menu_.Unselect();
			prevPosition_.reset();
		}

		bool Update() override
		{
			const auto p = 
				std::apply(
					[this](TPositionArgs... args) {return device_.Position(args...); }, 
					deviceArgs_);

			const auto prevSelect = menu_.GetSelectedIndex();

			bool isOnItem = false;

			if(p.has_value())
			{
				const auto position =
					Math::ConvertPosition2DCoordinate(
						p.value(),
						deviceCoord_, menuCoord_);

				std::optional<size_t> selected;
				for (size_t i = 0; i < menu_.Count(); ++i)
				{
					const auto& menuItem = menu_[i];
					if (IsPositionInBox(position, menuItem->GetBox()))
					{
						selected = i;
						isOnItem = true;
						break;
					}
				}

				if (selected != prevSelect && p != prevPosition_ && selected.has_value())
					select(selected);

				if (prevPosOnItem_ && !isOnItem)
					select(std::nullopt);
			}

			prevPosition_ = p;
			prevPosOnItem_ = isOnItem;
			return true;
		}
	};
}
