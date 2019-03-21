#include "pch.h"
#include <L2DDevice.h>

TEST(Init, InitQuit) 
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Init.InitQuit", { 800,600 }, true);
	Live2D::Device device;
}
