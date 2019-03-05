#pragma once
#include <utility>
#include <optional>
#include <cmath>
#include <string_view>
#include "SMath.h"
#include "Object.h"
#include "MemPool.h"

namespace Snowing::Scene
{
	namespace TweenFX
	{
		using FX = float(*)(float);

		constexpr auto Once = [](float x) { return x; };
		constexpr auto Twice = [](float x) { return x * x; };
		constexpr auto Cubic = [](float x) { return x * x * x; };
		constexpr auto Quad = [](float x) { return x * x * x * x; };
		constexpr auto Sin = [](float x) { return std::sinf(x * Math::Constants::Pi<float> / 2); };
		constexpr auto Sqrt = [](float x) { return std::sqrtf(x); };

		constexpr inline FX GetFX(std::string_view sv)
		{
			if (sv == "Once") return Once;
			else if (sv == "Twice") return Twice;
			else if (sv == "Cubic") return Cubic;
			else if (sv == "Quad") return Quad;
			else if (sv == "Sin") return Sin;
			else if (sv == "Sqrt") return Sqrt;
			else throw std::invalid_argument{ "Unknown FX name." };
		}
	}
	

	template <typename TVariable>
	class[[nodiscard]] Tween final : public Object, public MemPool<Tween<TVariable>>
	{
	private:
		TVariable variable_;

		struct TweenData
		{
			TVariable begin;
			TVariable target;
			float allTime;
			float nowTime;
			TweenFX::FX fx;
		};

		std::optional<TweenData> tween_;
		bool mustUpdateTrue_ = false;
	public:
		Tween(const TVariable& init = { 0 }) :
			variable_{ init }
		{}

		[[nodiscard]]
		TVariable& operator * ()
		{
			return variable_;
		}

		[[nodiscard]]
		TVariable* operator -> ()
		{
			return &variable_;
		}

		void Start(const TVariable& target, float time, TweenFX::FX fx = TweenFX::Once)
		{
			assert(fx);

			if (time <= 0)
			{
				variable_ = target;
				mustUpdateTrue_ = true;
			}
			else
				tween_ = TweenData
				{
					variable_,
					target,
					time,
					0,
					fx
				};
		}

		void Stop()
		{
			tween_.reset();
		}

		auto& operator = (const TVariable& t)
		{
			Stop();
			variable_ = t;
			mustUpdateTrue_ = true;
			return *this;
		}

		bool Update() override
		{
			if (tween_.has_value())
			{
				tween_->nowTime += Engine::Get().DeltaTime();
				if (tween_->nowTime >= tween_->allTime)
				{
					variable_ = tween_->target;
					Stop();
				}
				else
				{
					float t = tween_->nowTime / tween_->allTime;
					if (tween_->fx) t = tween_->fx(t);
					variable_ = Math::Interpo(t, tween_->begin, tween_->target);
				}
				return true;
			}
			else
			{
				if (mustUpdateTrue_)
				{
					mustUpdateTrue_ = false;
					return true;
				}
				else
					return false;
			}
		}
	};
}
