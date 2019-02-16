#pragma once
#include "Menu.h"

namespace Snowing::Scene
{
	template <typename TMenuItem>
	class[[nodiscard]] MenuKeyController final : public Object
	{
	private:
		Menu<TMenuItem> &menu_;

		void moveMenuSelect(int move)
		{
			if (!menu_.GetSelectedIndex().has_value())
				menu_.Select(0);
			else
			{
				auto selected = menu_.GetSelectedIndex().value();
				selected += move;
				selected += menu_.Count();
				selected %= menu_.Count();
				menu_.Select(selected);
			}
		}

	public:
		MenuKeyController(Menu<TMenuItem> *menu) :
			menu_{ *menu }
		{}

		void Next()
		{
			moveMenuSelect(1);
		}

		void Prev()
		{
			moveMenuSelect(-1);
		}
	};
}