#pragma once
#include <SingleInstance.h>

namespace Snowing::Live2D
{
	class Device final : 
		public SingleInstance<Device>
	{
	public:
		Device();
		~Device();
	};
}
