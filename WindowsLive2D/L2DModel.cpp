#include "L2DModel.h"
#include <Model/CubismMoc.hpp>
#include <Model/CubismModel.hpp>
#include <CubismModelSettingJson.hpp>
#include <Rendering/D3D11/CubismRenderer_D3D11.hpp>
#include <Effect/CubismPose.hpp>
#include <Math/CubismModelMatrix.hpp>
#include <Motion/CubismMotionManager.hpp>

void Live2D::Model::updateMatrix()
{
	auto model = model_.Get<Csm::CubismModel*>();
	Csm::CubismModelMatrix modelMatrix(model->GetCanvasWidth(), model->GetCanvasHeight());

	Csm::csmMap<Csm::csmString, Csm::csmFloat32> layout;
	asset_->GetSetting().Get<Csm::CubismModelSettingJson*>()->GetLayoutMap(layout);
	modelMatrix.SetupFromLayout(layout);

	modelMatrix.SetPosition(translate_.x, translate_.y);
	modelMatrix.Scale(scale_.x, scale_.y);

	Csm::CubismMatrix44 projectionMatrix;
	projectionMatrix.Scale(1, ratio_);
	projectionMatrix.MultiplyByMatrix(&modelMatrix);
	renderer_.Get<Csm::Rendering::CubismRenderer_D3D11*>()->SetMvpMatrix(&projectionMatrix);
}

Live2D::Model::Model(Snowing::Graphics::Context* ctx,const ModelAsset* asset,float ratio):
	ratio_{ ratio },
	ctx_{ ctx },
	asset_{ asset },
	model_{
		asset_->GetMoc().Get<Csm::CubismMoc*>()->CreateModel(),
		[moc = asset_->GetMoc().Get<Csm::CubismMoc*>()](void* model) {
			moc->DeleteModel(
				static_cast<Csm::CubismModel*>(model));
		}
	},
	renderer_{
		std::invoke([this,ratio,asset,model = model_.Get<Csm::CubismModel*>()] {
			Csm::Rendering::CubismRenderer* renderer = Csm::Rendering::CubismRenderer::Create();
			renderer->Initialize(model);
			auto p = dynamic_cast<Csm::Rendering::CubismRenderer_D3D11*>(renderer);

			for (Csm::csmUint32 i = 0; i < asset->GetTextures().size(); ++i)
				if (asset->GetTextures()[i].has_value())
					p->BindTexture(i,asset->GetTextures()[i]->GetImpl().ShaderResource().Cast<IUnknown*, ID3D11ShaderResourceView*>());

			return p;
		}),
		[](void* p) {
			Csm::Rendering::CubismRenderer::Delete(static_cast<Csm::Rendering::CubismRenderer_D3D11*>(p));
		}
	},
	motionManager_{
		new (motionManagerBox_.data()) Csm::CubismMotionManager,
		[](void * p){
			static_cast<Csm::CubismMotionManager*>(p)->
				 Csm::CubismMotionManager::~CubismMotionManager();
		}
	},
	expressionManager_{
		new (expressionManagerBox_.data()) Csm::CubismMotionManager,
		[](void* p) {
			static_cast<Csm::CubismMotionManager*>(p)->
				 Csm::CubismMotionManager::~CubismMotionManager();
		}
	}
{
	static_assert(sizeof(Csm::CubismMotionManager) <= CsmMotionManagerSize);

	updateMatrix();

	if (asset_->GetPose().has_value())
	{
		const auto& poseBlob = asset_->GetPose().value();
		pose_ = Snowing::Platforms::Handler
		{
			Csm::CubismPose::Create(poseBlob.Get<Csm::csmByte*>(),static_cast<Csm::csmSizeInt>(poseBlob.Size())),
			[](void* ptr) { Csm::CubismPose::Delete(static_cast<Csm::CubismPose*>(ptr)); }
		};
	}

	motionManager_.Get<Csm::CubismMotionManager*>()->SetEventCallback([] (auto,auto,auto){}, nullptr);
}

bool Live2D::Model::Update()
{
	const float dt = Snowing::Engine::Get().DeltaTime();

	motionManager_.Get<Csm::CubismMotionManager*>()->UpdateMotion(
		model_.Get<Csm::CubismModel*>(),
		dt);

	expressionManager_.Get<Csm::CubismMotionManager*>()->UpdateMotion(
		model_.Get<Csm::CubismModel*>(),
		dt);

	if(pose_.IsSome())
		pose_.Get<Csm::CubismPose*>()->UpdateParameters(
			model_.Get<Csm::CubismModel*>(),
			dt);

	model_.Get<Csm::CubismModel*>()->Update();

	Snowing::Engine::Get().Draw([this] {
		const auto renderer = renderer_.Get<Csm::Rendering::CubismRenderer_D3D11*>();
		const auto device = Snowing::PlatformImpls::WindowsImpl::D3D::Device::Get().GetHandler().Cast<IUnknown*, ID3D11Device*>();
		const auto context = ctx_->GetImpl().GetHandler().Cast<IUnknown*, ID3D11DeviceContext*>();

		const auto rt = ctx_->GetRenderTarget();
		const auto desc = rt->GetTexture().Size();
		context->ClearState();
		ctx_->SetRenderTarget(rt);

		// »æÖÆLive2DÄÚÈÝ
		renderer->StartFrame(device, context, desc.x, desc.y);
		renderer->DrawModel();
		renderer->EndFrame(device);
	});

	return true;
}

bool Live2D::Model::MotionFinished() const
{
	return motionManager_.Get<Csm::CubismMotionManager*>()->IsFinished();
}

bool Live2D::Model::ExpressionFinished() const
{
	return expressionManager_.Get<Csm::CubismMotionManager*>()->IsFinished();
}

void Live2D::Model::StopAllMotion()
{
	motionManager_.Get<Csm::CubismMotionManager*>()->StopAllMotions();
}

void Live2D::Model::StopAllExpression()
{
	expressionManager_.Get<Csm::CubismMotionManager*>()->StopAllMotions();
}

void Live2D::Model::SetTranslate(Snowing::Math::Vec2f translate)
{
	translate_ = translate;
	updateMatrix();
}

void Live2D::Model::SetScale(Snowing::Math::Vec2f scale)
{
	scale_ = scale;
	updateMatrix();
}

const Snowing::Platforms::Handler& Live2D::Model::GetModel() const
{
	return model_;
}

const Live2D::ModelAsset* Live2D::Model::GetAsset() const
{
	return asset_;
}

const Snowing::Platforms::Handler& Live2D::Model::GetMotionManager() const
{
	return motionManager_;
}

const Snowing::Platforms::Handler& Live2D::Model::GetExpressionManager() const
{
	return expressionManager_;
}

Snowing::Math::Vec2f Live2D::Model::GetCanvasSize() const
{
	auto h = model_.Get<Csm::CubismModel*>()->GetCanvasHeight();
	auto w = model_.Get<Csm::CubismModel*>()->GetCanvasWidth();
	return { w,h };
}
