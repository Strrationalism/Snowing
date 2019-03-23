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
		Csm::CubismBreath::Create(),
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
		params.Angle.x.Offset, params.Angle.x.Peak, params.Angle.x.Cycle, params.Angle.x.Weight));
	breathParameters.PushBack(Csm::CubismBreath::BreathParameterData(_idParamAngleY,
		params.Angle.y.Offset, params.Angle.y.Peak, params.Angle.y.Cycle, params.Angle.y.Weight));
	breathParameters.PushBack(Csm::CubismBreath::BreathParameterData(_idParamAngleZ,
		params.Angle.z.Offset, params.Angle.z.Peak, params.Angle.z.Cycle, params.Angle.z.Weight));
	breathParameters.PushBack(Csm::CubismBreath::BreathParameterData(_idParamBodyAngleX,
		params.BodyAngleX.Offset, params.BodyAngleX.Peak, params.BodyAngleX.Cycle, params.BodyAngleX.Weight));
	breathParameters.PushBack(Csm::CubismBreath::BreathParameterData(_idParamBreath,
		params.Breath.Offset, params.Breath.Peak, params.Breath.Cycle, params.Breath.Weight));
	p->SetParameters(breathParameters);
}

bool Live2D::Breath::Update()
{
	breath_.Get<Csm::CubismBreath*>()->UpdateParameters(
		model_.Get<Csm::CubismModel*>(),
		Snowing::Engine::Get().DeltaTime());
	return true;
}
