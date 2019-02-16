#pragma once
#include "Controller.h"
#include "Platforms.h"
#include "TweenSetter.h"
#include "CSVParser.h"
#include "Group.h"
#include "TweenSetter.h"

namespace Snowing::Scene
{
	template <typename>
	class VibratePlayer;

	struct VibrateEffect
	{
	private:
		template <typename>
		friend class VibratePlayer;

		enum class OpType { Tween, Set, Loop, Stop };
		enum class VibAxe { Left, Right, All };
		std::vector<std::tuple<float, OpType, VibAxe, float, float, TweenFX::FX>> actions;
	public:
		inline VibrateEffect(const Blob& effectCSV)
		{
			CSVParser<char> csv{ &effectCSV };
			constexpr const char* parseError = "Can not parse vibrate effect.";
			do
			{
				OpType op;
				VibAxe axe = VibAxe::Left;
				TweenFX::FX fx = nullptr;
				float duration = 0;
				float force = 0;

				const float beginTime = csv.Pop<float>();

				const auto opStr = csv.Pop();
				if (opStr == "Tween") op = OpType::Tween;
				else if (opStr == "Set") op = OpType::Set;
				else if (opStr == "Loop") op = OpType::Loop;
				else if (opStr == "Stop") op = OpType::Stop;
				else throw std::invalid_argument{ parseError };

				if (op == OpType::Tween || op == OpType::Set || op == OpType::Stop)
				{
					const auto axeStr = csv.Pop();
					if (axeStr == "L") axe = VibAxe::Left;
					else if (axeStr == "R") axe = VibAxe::Right;
					else if (axeStr == "All") axe = VibAxe::All;
					else throw std::invalid_argument{ parseError };

					if (op == OpType::Tween || op == OpType::Set)
						force = csv.Pop<float>();

					if (op == OpType::Tween)
					{
						duration = csv.Pop<float>();
						const auto fxStr = csv.Pop();
						fx = TweenFX::GetFX(fxStr);
					}
				}
				actions.emplace_back(beginTime, op, axe, force, duration, fx);
			} while (csv.NextLine());
		}
	};

	template <typename TVibrator>
	class VibratePlayer final : public Object
	{
	private:
		Group<BaseTask> g_;
		TVibrator &vib_;
		const VibrateEffect &effect_;
		size_t position_ = 0;
		bool live_ = true;

		float lLimit_, rLimit_;

		friend struct LeftSetter;
		friend struct RightSetter;

		struct LeftSetter
		{
			VibratePlayer* cur;
			void operator ()(float l)
			{
				(cur->vib_).Vibration(Input::VibrationLeftRight{ l * cur->lLimit_, *(cur->rightTween_) });
			}
		};

		struct RightSetter
		{
			VibratePlayer* cur;
			void operator ()(float r)
			{
				(cur->vib_).Vibration(Input::VibrationLeftRight{ *(cur -> leftTween_),r * cur->rLimit_ });
			}
		};

		TweenSetter<float, LeftSetter> leftTween_;
		TweenSetter<float, RightSetter> rightTween_;

	

		void nextAction()
		{
			if (position_ >= effect_.actions.size())
			{
				live_ = false;
				return;
			}

			auto [beginTime, op, axe, force, duration, fx] = effect_.actions.at(position_++);

			switch (op)
			{
				case VibrateEffect::OpType::Tween: {
					auto task = [this, axe, force, duration, fx] {
						if (axe == VibrateEffect::VibAxe::Left)
							leftTween_.Start(force, duration, fx);
						else if (axe == VibrateEffect::VibAxe::Right)
							rightTween_.Start(force, duration, fx);
						else
						{
							leftTween_.Start(force, duration, fx);
							rightTween_.Start(force, duration, fx);
						}
						nextAction();
					};
					g_.Emplace<Task<decltype(task)>>(beginTime,std::move(task));
					break;
				}

				case VibrateEffect::OpType::Set: {
					auto task = [this, axe, force] {
						if (axe == VibrateEffect::VibAxe::Left)
							leftTween_ = force;
						else if (axe == VibrateEffect::VibAxe::Right)
							rightTween_ = force;
						else
						{
							leftTween_ = force;
							rightTween_ = force;
						}
						nextAction();
					};
					g_.Emplace<Task<decltype(task)>>(beginTime,std::move(task));
					break;
				}

				case VibrateEffect::OpType::Stop: {
					auto task = [this, axe] {
						if (axe == VibrateEffect::VibAxe::Left)
							leftTween_ = 0;
						else if (axe == VibrateEffect::VibAxe::Right)
							rightTween_ = 0;
						else 
						{
							leftTween_ = 0;
							rightTween_ = 0;
							vib_.Vibration(Input::VibrationStop{});
						}
						nextAction();
					};
					g_.Emplace<Task<decltype(task)>>(beginTime,std::move(task));
					break;
				}
				
				case VibrateEffect::OpType::Loop: {
					auto task = [this] {
						position_ = 0;
						nextAction();
					};
					g_.Emplace<Task<decltype(task)>>(beginTime,std::move(task));
					break;
				}
				default:
					assert(false);
					break;
			};
		}

	public:
		VibratePlayer(TVibrator *vib, const VibrateEffect *effect,float lLimit = 1.0f,float rLimit = 1.0f):
			vib_{ *vib },
			effect_{ *effect },
			leftTween_{ 0.0f,LeftSetter{this} },
			rightTween_{ 0.0f,RightSetter{this} },
			lLimit_{ lLimit },
			rLimit_{ rLimit }
		{
			nextAction();
		}

		void SetLeftLimit(float l)
		{
			lLimit_ = l;
		}

		void SetRightLimit(float r)
		{
			rLimit_ = r;
		}

		void Stop()
		{
			live_ = false;
			g_.Clear();
			leftTween_ = 0;
			rightTween_ = 0;
			vib_.Vibration(Input::VibrationStop{});
		}

		~VibratePlayer()
		{
			Stop();
		}

		bool Update() override
		{
			const bool gu = g_.Update();
			const bool lu = leftTween_.Update();
			const bool ru = rightTween_.Update();
			return 
				live_ || 
				gu || 
				lu || 
				ru;
		}
	};
}
