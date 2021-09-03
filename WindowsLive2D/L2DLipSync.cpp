#include "L2DLipSync.h"
#include <CubismModelSettingJson.hpp>
#include <Model/CubismModel.hpp>

Live2D::LipSync::LipSync(Model* model):
	model_{ model }
{
	model_->lipSync_ = this;
}

Live2D::LipSync::~LipSync()
{
	model_->lipSync_ = nullptr;
}

void Live2D::LipSync::SetVolume(float v)
{
	volume_ = v;
}
