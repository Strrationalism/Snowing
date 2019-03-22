#include "pch.h"
#include <L2DDevice.h>
#include <L2DModel.h>
TEST(Init, InitQuit) 
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Init.InitQuit", { 800,600 }, true);
	Live2D::Device device;
}

TEST(Init, LoadModel)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Init.LoadModel", { 800,600 }, true);

	Live2D::Device device;

	Live2D::Model haru{ "Live2D/Haru/","Haru.model3.json" };
	Live2D::Model hiyori{ "Live2D/Hiyori/","Hiyori.model3.json" };
	Live2D::Model mark{ "Live2D/Mark/","Mark.model3.json" };

}