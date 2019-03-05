#pragma once
#include <functional>
#include "Tween.h"

namespace Snowing::Scene
{
	template <typename TVariable,typename TSetter>
	class TweenSetter : public Object
	{
	private:
		Tween<TVariable> tween_;
		TSetter setter_;

		bool mustUpdate_ = true;

	public:
		TweenSetter(const TVariable& init, const TSetter& setter) :
			tween_{ init },
			setter_{ setter }
		{
		}

		void Start(const TVariable& target, float time, TweenFX::FX fx = TweenFX::Once)
		{
			tween_.Start(target, time, fx);
			Update();
		}

		auto& operator = (const TVariable& v)
		{
			tween_ = v;
			return *this;
		}

		[[nodiscard]]
		TVariable& operator * ()
		{
			return *tween_;
		}

		[[nodiscard]]
		TVariable* operator -> ()
		{
			return tween_.operator->();
		}

		void Stop()
		{
			tween_.Stop();
		}

		bool Update() override
		{
			if (tween_.Update() || mustUpdate_)
			{
				mustUpdate_ = false;
				setter_(*tween_);
				return true;
			}
			else
				return false;
		}
	};

	template <typename TVariable>
	class VirtualTweenSetter final : 
		public TweenSetter<TVariable, std::function<void(const TVariable&)>>,
		public MemPool<VirtualTweenSetter<TVariable>>
	{
	private:
		using Base = TweenSetter<TVariable, std::function<void(const TVariable&)>>;

	public:
		using Base::operator=;
		using Base::Base;
	};
}
