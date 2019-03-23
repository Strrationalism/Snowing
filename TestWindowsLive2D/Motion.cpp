#include "pch.h"

void RenderMotion(const char* homeDir,const char* entryFile)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Motion.RenderMotion", { 800,600 }, true);

	Graphics::Device::MainContext().SetRenderTarget(
		&Graphics::Device::MainRenderTarget());


	Live2D::Device device;

	Live2D::ModelAsset ass{ homeDir,entryFile };

	Scene::Group<> s;

	s.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),
		&Graphics::Device::MainRenderTarget(),
		Math::Vec4f{ 0,0,0,0 });

	auto model = s.Emplace<Live2D::Model>(
		&Graphics::Device::MainContext(),
		&ass,
		800.0f / 600.0f);

	float waitTime = 0;
	for (size_t groupID = 0; groupID < ass.GetMotionGroups().size(); ++groupID)
	{
		for (size_t motionID = 0; motionID < ass.GetMotionGroup(groupID).second.size(); ++motionID)
		{
			auto motion =s.Emplace<Live2D::Motion>(model, groupID, motionID);

			s.Emplace<Scene::VirtualTask>(waitTime, [groupID, motionID,motion,&ass] {
				motion->Play();
				Log("Play Motion:", ass.GetMotionGroups()[groupID].first, motionID);
			});

			waitTime += 4;
		}
	}

	s.Emplace<Scene::PointerTask>(waitTime, [] { Snowing::Engine::Get().Exit(); });

	Engine::Get().RunObject(s);
}

TEST(Motion, Hiyori)
{
	RenderMotion("Live2D/Hiyori/", "Hiyori.model3.json");
}

TEST(Motion, Haru)
{
	RenderMotion("Live2D/Haru/", "Haru.model3.json");
}

TEST(Motion, Mark)
{
	RenderMotion("Live2D/Mark/", "Mark.model3.json");
}
