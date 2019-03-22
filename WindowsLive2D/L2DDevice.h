#pragma once
#include <SingleInstance.h>
#include <Snowing.h>

namespace Live2D
{
	using AssetLoader = Snowing::Blob(*) (Snowing::AssetName);
	class Device final : 
		public Snowing::SingleInstance<Device>
	{
	public:
		Device();
		~Device();
	};
}
