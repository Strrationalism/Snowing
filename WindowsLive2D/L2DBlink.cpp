#include "L2DBlink.h"
#include <Effect/CubismEyeBlink.hpp>
#include <CubismModelSettingJson.hpp>

Live2D::Blink::Blink(Model* model, float blinkInterval):
	model_{
		model->GetModel().Get<Csm::CubismModel*>(),
		Snowing::Platforms::Handler::DoNothingDeleter
	},
	blink_{
		std::invoke([model] {
			auto p = Csm::CubismEyeBlink::Create();

			auto setting = model->GetAsset()->GetSetting().Get<Csm::CubismModelSettingJson*>();
			auto eyeBlinkIdCount = setting->GetEyeBlinkParameterCount();
			Csm::csmVector<Csm::CubismIdHandle> eyeParams;
			for (int i = 0; i < eyeBlinkIdCount; ++i)
			{
				eyeParams.PushBack(setting->GetEyeBlinkParameterId(i));
			}
			p->SetParameterIds(eyeParams);

			return p;
		}),
		[](void* p) { Csm::CubismEyeBlink::Delete(static_cast<Csm::CubismEyeBlink*>(p)); }
	}
{
	SetBlinkInterval(blinkInterval);
}

void Live2D::Blink::SetBlinkInterval(float i)
{
	blink_.Get<Csm::CubismEyeBlink*>()->SetBlinkingInterval(i);
}

bool Live2D::Blink::Update()
{
	blink_.Get<Csm::CubismEyeBlink*>()->UpdateParameters(
		model_.Get<Csm::CubismModel*>(),
		Snowing::Engine::Get().DeltaTime());
	return true;
}
