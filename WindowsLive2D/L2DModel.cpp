#include "L2DModel.h"
#include <Model/CubismMoc.hpp>
#include <Model/CubismModel.hpp>
#include <CubismModelSettingJson.hpp>
#include <Rendering/D3D11/CubismRenderer_D3D11.hpp>
#include <Effect/CubismPose.hpp>

// Magic Matrix
#include <Math/CubismModelMatrix.hpp>

Live2D::Model::Model(Snowing::Graphics::Context* ctx,const ModelAsset* asset,float ratio):
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
		std::invoke([ratio,asset,model = model_.Get<Csm::CubismModel*>()] {
			Csm::Rendering::CubismRenderer* renderer = Csm::Rendering::CubismRenderer::Create();
			renderer->Initialize(model);
			auto p = dynamic_cast<Csm::Rendering::CubismRenderer_D3D11*>(renderer);

			for (Csm::csmUint32 i = 0; i < asset->GetTextures().size(); ++i)
				if (asset->GetTextures()[i].has_value())
					p->BindTexture(i,asset->GetTextures()[i]->GetImpl().ShaderResource().Cast<IUnknown*, ID3D11ShaderResourceView*>());

			Csm::CubismModelMatrix modelMatrix(model->GetCanvasWidth(), model->GetCanvasHeight());

			Csm::csmMap<Csm::csmString, Csm::csmFloat32> layout;
			asset->GetSetting().Get<Csm::CubismModelSettingJson*>()->GetLayoutMap(layout);
			modelMatrix.SetupFromLayout(layout);

			Csm::CubismMatrix44 projectionMatrix;
			projectionMatrix.Scale(1, ratio);
			projectionMatrix.MultiplyByMatrix(&modelMatrix);
			dynamic_cast<Csm::Rendering::CubismRenderer_D3D11*>(renderer)->SetMvpMatrix(&projectionMatrix);

			return p;
		}),
		[](void* p) {
			Csm::Rendering::CubismRenderer::Delete(static_cast<Csm::Rendering::CubismRenderer_D3D11*>(p));
		}
	}
{
}

bool Live2D::Model::Update()
{
	if(asset_->GetPose().has_value())
		asset_->GetPose().value().Get<Csm::CubismPose*>()->UpdateParameters(
			model_.Get<Csm::CubismModel*>(),
			Snowing::Engine::Get().DeltaTime());

	model_.Get<Csm::CubismModel*>()->Update();

	Snowing::Engine::Get().Draw([this] {
		const auto renderer = renderer_.Get<Csm::Rendering::CubismRenderer_D3D11*>();
		const auto device = Snowing::PlatformImpls::WindowsImpl::D3D::Device::Get().GetHandler().Cast<IUnknown*, ID3D11Device*>();
		const auto context = ctx_->GetImpl().GetHandler().Cast<IUnknown*, ID3D11DeviceContext*>();
		
		// 获取当前RenderTarget的大小
		ID3D11Resource* rtTex = nullptr;
		ID3D11RenderTargetView* renderTarget = nullptr;
		context->OMGetRenderTargets(1,&renderTarget,nullptr);
		assert(renderTarget);
		renderTarget->GetResource(&rtTex);
		assert(rtTex);

		D3D11_TEXTURE2D_DESC desc;

		static_cast<ID3D11Texture2D*>(rtTex)->GetDesc(&desc);
		rtTex->Release();
		renderTarget->Release();

		// 绘制Live2D内容
		renderer->StartFrame(device, context, desc.Width, desc.Height);
		renderer->DrawModel();
		renderer->EndFrame(device);
	});

	return true;
}
