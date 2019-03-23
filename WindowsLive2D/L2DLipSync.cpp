#include "L2DLipSync.h"
#include <CubismModelSettingJson.hpp>
#include <Model/CubismModel.hpp>

Live2D::LipSync::LipSync(Model* model):
	model_{ model }
{
}

void Live2D::LipSync::SetVolume(float v)
{
	const auto setting = 
		model_->GetAsset()->GetSetting().Get<Csm::CubismModelSettingJson*>();

	const auto paramCount = setting->GetLipSyncParameterCount();
	for (Csm::csmInt32 id = 0; id < paramCount; ++id)
		model_->GetModel().Get<Csm::CubismModel*>()->SetParameterValue(
			setting->GetLipSyncParameterId(id), v, 0.8f);
}

