#pragma once
#include <cassert>
#include <cstdint>
#include <tuple>
#include <functional>


namespace Snowing::Input
{
	// ���ڽ���������ɶ�ζ̰���ʹ��Event()�����̰������
	class[[nodiscard]] KeyKeepTranslatorBase
	{
	public:
		virtual ~KeyKeepTranslatorBase() = default;
		virtual bool KeyPressed() const = 0;
		virtual bool Event() const = 0;
		virtual void Update() = 0;
	};

	template <typename Input,typename ... KeyArgs>
	class KeyKeepTranslator final : public KeyKeepTranslatorBase
	{
	private:
		Input& input_;
		std::tuple<KeyArgs...> args_;

		float keepTime_ = 0;
		bool event_ = false;

		const float delaySec_, durationSec_;

	public:
		KeyKeepTranslator(Input* input,float delaySec, float durationSec,KeyArgs ... keyArgs) noexcept :
			input_{ *input },
			args_{ keyArgs... },
			durationSec_{ durationSec },
			delaySec_{ delaySec }
		{
			assert(input);
		}

		[[nodiscard]]
		bool KeyPressed() const override
		{
			const auto k = [this](KeyArgs... a)
			{
				return input_.KeyPressed(a...);
			};

			return std::apply(k, args_);
		}

		[[nodiscard]]
		bool Event() const override
		{
			return event_;
		}

		void Update() override
		{
			if (KeyPressed())
			{
				if (keepTime_ <= 0.001)
					event_ = true;
				else
					event_ = false;

				keepTime_ += Engine::Get().DeltaTime();

				if (keepTime_ > delaySec_)
				{
					if (keepTime_ + delaySec_ > durationSec_)
					{
						keepTime_ -= durationSec_;
						event_ = true;
					}
					else
						event_ = false;
				}
			}
			else
			{
				keepTime_ = 0;
				event_ = false;
			}
		}
	};
}
