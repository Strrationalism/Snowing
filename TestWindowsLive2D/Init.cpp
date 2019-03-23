#include "pch.h"

TEST(Init, InitQuit) 
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Init.InitQuit", { 800,600 }, true);
	Live2D::Device device;
}

TEST(Init, LoadModelAsset)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Init.LoadModel", { 800,600 }, true);

	Live2D::Device device;

	Live2D::ModelAsset haru{ "Live2D/Haru/","Haru.model3.json" };
	Live2D::ModelAsset hiyori{ "Live2D/Hiyori/","Hiyori.model3.json" };
	Live2D::ModelAsset mark{ "Live2D/Mark/","Mark.model3.json" };

}