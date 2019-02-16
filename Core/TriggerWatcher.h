#pragma once
#include <cassert>
#include <optional>

namespace Snowing::Input
{
	class[[nodiscard]] TriggerWatcherBase
	{
	public:
		virtual ~TriggerWatcherBase() = default;

		virtual void Update() = 0;
		virtual bool IsNew() const noexcept = 0;
		virtual float Value() const noexcept = 0;
	};

	template <typename Input,typename TriggerIDType>
	class[[nodiscard]] TriggerWatcher final : public TriggerWatcherBase
	{
	private:
		const TriggerIDType id_;
		const Input& input_;
		float prevState_ = 0.0f, curState_ = 0.0f;

	public:
		TriggerWatcher(const Input* input,TriggerIDType t) noexcept :
			input_{ *input },
			id_{ t }
		{
			assert(input);
			prevState_ = curState_ = input_.Trigger(id_);
		}

		void Update() override
		{
			prevState_ = curState_;
			curState_ = input_.Trigger(id_);
		}

		[[nodiscard]]
		bool IsNew() const noexcept override
		{
			return curState_ != prevState_;
		}

		[[nodiscard]]
		float Value() const noexcept override
		{
			return curState_;
		}
	};
}
