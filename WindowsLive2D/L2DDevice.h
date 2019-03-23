#pragma once
#include <SingleInstance.h>
#include <Snowing.h>

namespace Live2D
{
	using AssetLoader = Snowing::Blob(*) (Snowing::AssetName homeDir,Snowing::AssetName file);
	Snowing::Blob DefaultAssetLoader(Snowing::AssetName homeDir, Snowing::AssetName file);

	class Device final : 
		public Snowing::SingleInstance<Device>
	{
	public:
		Device();
		~Device();
	};
}
