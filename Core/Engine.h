#pragma once
#include <chrono>
#include "Graphics.h"
#include "Window.h"
#include "Input.h"
#include "SingleInstance.h"

namespace Snowing
{
	template <
		typename WindowImpl,
		typename GraphicsImpl,
		typename InputImpl>
	class[[nodiscard]] EngineInterface final : public SingleInstance<EngineInterface<WindowImpl,GraphicsImpl,InputImpl>>
	{
	private:
		using PEngine = EngineInterface<WindowImpl, GraphicsImpl, InputImpl>*;

		bool keep_ = false;
		float deltaTime_ = 0;

	public:
		EngineInterface() noexcept = default;
		EngineInterface(const EngineInterface&) = delete;
		EngineInterface(EngineInterface&&) noexcept = default;
		auto operator = (const EngineInterface&) = delete;
		auto operator = (EngineInterface&&) = delete;

		template <typename TFunc>
		inline auto Draw(TFunc& func) { return func(); }


		template <typename TFunc>
		inline void Run(TFunc& updateFunc)
		{
			keep_ = true;

			const std::chrono::high_resolution_clock clock;
			auto lastFrame = clock.now();

			while (keep_)
			{
				if (!WindowImpl::Get().Update())
					keep_ = false;

				InputImpl::Get().Update();

				deltaTime_ = std::chrono::duration<float, std::ratio<1>>{ clock.now() - lastFrame }.count();
				lastFrame = clock.now();
				updateFunc();


				GraphicsImpl::Get().Update();
			}

			deltaTime_ = 0;
		}

		template <typename TObject>
		inline void RunObject(TObject& obj)
		{
			Run([&obj] {
				if (!obj.Update())
					Get().Exit();
			});
		}

		inline float DeltaTime() const
		{
			return std::clamp(deltaTime_,0.0f,0.1f);
		}

		inline void Exit() noexcept
		{
			keep_ = false;
		}
	};
}
