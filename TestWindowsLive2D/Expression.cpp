#include "pch.h"
#include <L2DExpression.h>

TEST(Expression, HaruExpressions)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Expression.HaruExpressions", { 800,600 }, true);

	Graphics::Device::MainContext().SetRenderTarget(
		&Graphics::Device::MainRenderTarget());


	Live2D::Device device;

	Live2D::ModelAsset haru{ "Live2D/Haru/","Haru.model3.json" };

	Scene::Group<> s;

	auto model = s.Emplace<Live2D::Model>(
		&Graphics::Device::MainContext(),
		&haru,
		800.0f / 600.0f);

	model->SetTranslate({ 0,-2.7F });
	model->SetScale({ 4.0F,4.0F });

	

	for (size_t i = 0; i < model->GetAsset()->GetExpressionNames().size(); ++i)
	{
		auto expressionName = model->GetAsset()->GetExpressionNames()[i];
		auto expression = s.Emplace<Live2D::Expression>(
			model,
			model->GetAsset()->GetExpressionID(expressionName));
			
		s.Emplace<Scene::VirtualTask>(1.0f + i , [expression, expressionName,&s] {
			expression->Apply();
			Snowing::Log("Playing Expression:", expressionName,"Object Count:",s.Count());
		});
	}

	s.Emplace<Scene::VirtualTask>(model->GetAsset()->GetExpressionNames().size() + 2.0f, [] {Snowing::Engine::Get().Exit(); });

	

	Engine::Get().RunObject(s);
}
