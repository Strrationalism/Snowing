#include "L2DBreath.h"
#include <Effect/CubismBreath.hpp>
#include <Id/CubismIdManager.hpp>
#include <CubismDefaultParameterId.hpp>

Live2D::Breath::Breath(Model* model, const Params& params) :
	model_{
		model->GetModel().Get<Csm::CubismModel*>(),
		Snowing::Platforms::Handler::DoNothingDeleter
	},
	breath_{
		std::invoke([&params] {
			auto p = Csm::CubismBreath::Create();
			return p;
		}),
		[](void* p){
			Csm::CubismBreath::Delete(static_cast<Csm::CubismBreath*>(p));
		}
	}
{
	SetParams(params);
}

void Live2D::Breath::SetParams(const Params& params)
{
	auto p = breath_.Get<Csm::CubismBreath*>();
	const auto _idParamAngleX = Csm::CubismFramework::GetIdManager()->GetId(Csm::DefaultParameterId::ParamAngleX);
	const auto _idParamAngleY = Csm::CubismFramework::GetIdManager()->GetId(Csm::DefaultParameterId::ParamAngleY);
	const auto _idParamAngleZ = Csm::CubismFramework::GetIdManager()->GetId(Csm::DefaultParameterId::ParamAngleZ);
	const auto _idParamBodyAngleX = Csm::CubismFramework::GetIdManager()->GetId(Csm::DefaultParameterId::ParamBodyAngleX);
	const auto _idParamBreath = Csm::CubismFramework::GetIdManager()->GetId(Csm::DefaultParameterId::ParamBreath);

	Csm::csmVector<Csm::CubismBreath::BreathParameterData> breathParameters;

	breathParameters.PushBack(Csm::CubismBreath::BreathParameterData(_idParamAngleX,
		params.Angle.x.x, params.Angle.x.y, params.Angle.x.z, params.Angle.x.w));
	breathParameters.PushBack(Csm::CubismBreath::BreathParameterData(_idParamAngleY,
		params.Angle.y.x, params.Angle.y.y, params.Angle.y.z, params.Angle.y.w));
	breathParameters.PushBack(Csm::CubismBreath::BreathParameterData(_idParamAngleZ,
		params.Angle.z.x, params.Angle.z.y, params.Angle.z.z, params.Angle.z.w));
	breathParameters.PushBack(Csm::CubismBreath::BreathParameterData(_idParamBodyAngleX,
		params.BodyAngleX.x, params.BodyAngleX.y, params.BodyAngleX.z, params.BodyAngleX.w));
	breathParameters.PushBack(Csm::CubismBreath::BreathParameterData(_idParamBreath,
		params.Breath.x, params.Breath.y, params.Breath.z, params.Breath.w));
	p->SetParameters(breathParameters);
}

bool Live2D::Breath::Update()
{
	breath_.Get<Csm::CubismBreath*>()->UpdateParameters(
		model_.Get<Csm::CubismModel*>(),
		Snowing::Engine::Get().DeltaTime());
	return true;
}
