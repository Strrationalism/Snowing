#pragma once
#include <cassert>
#include "Vec3.h"
#include <optional>

namespace Snowing::Graphics
{
	enum class BufferUsage
	{
		Default,
		Immutable,
		Staging,
		Dynamic
	};

	enum class BufferCPUAccessFlag
	{
		NoAccess,
		CPUReadOnly,
		CPUWriteOnly,
		CPUReadWrite
	};

	struct BufferBindMode final
	{
		enum class BindTypeEnum
		{
			VertexBuffer,
			ConstantBuffer,
			ShaderResources,
			None
		}BindType = BindTypeEnum::None;
		bool StreamOutput = false;
		bool RenderTarget = false;
	};

	constexpr BufferBindMode DefaultVertexBufferMode =
	{
		BufferBindMode::BindTypeEnum::VertexBuffer,
		false
	};

	constexpr BufferBindMode DefaultConstantBufferMode =
	{
		BufferBindMode::BindTypeEnum::ConstantBuffer,
		false
	};

	enum class AccessType
	{
		Read,
		Write,
		ReadWrite,
		WriteDiscard
	};

	template <typename T>
	struct Box final
	{
		T left, right, top, bottom, front, back;
	};

	template <typename TImpl>
	class[[nodiscard]] BufferInterface final
	{
	private:
		TImpl impl_;
		const size_t size_;
	public:
		BufferInterface(
			size_t size,
			BufferBindMode mode,
			const void* initData = nullptr,
			BufferUsage usage = BufferUsage::Default,
			BufferCPUAccessFlag cpuAccessFlag = BufferCPUAccessFlag::NoAccess) :
			impl_{ size,mode,initData,usage,cpuAccessFlag } ,
			size_{ size }
		{
			assert(size);
		};

		[[nodiscard]]
		size_t Size() const
		{
			return size_;
		}

		template <typename TContext,typename TFunc>
		void Access(TContext& ctx, Snowing::Graphics::AccessType accessType,TFunc& func)
		{
			impl_.Access(ctx,accessType,func);
		}

		template <typename TContext>
		void UpdateData(
			TContext& context,
			const void* data, 
			const std::optional<Box<size_t>>& slice = std::nullopt,
			size_t dataRowPitch = 0,
			size_t dataDepthPitch = 0)
		{
			assert(data);
			impl_.UpdateData(context, data, slice, dataRowPitch, dataDepthPitch);
		}

		template <typename TContext,typename TBuffer>
		void CopyTo(
			TContext& context,
			TBuffer& dst) const
		{
			impl_.CopyTo(context, dst);
		}

		template <typename TContext, typename TBuffer>
		void CopyTo(
			TContext& context,
			TBuffer& dst,
			Math::Vec3<size_t> dstSlice,
			const std::optional<Box<size_t>>& srcSlice) const
		{
			impl_.CopyTo(context, dst,dstSlice,srcSlice);
		}

		[[nodiscard]]
		const auto& GetImpl() const
		{
			return impl_;
		}
	};
}
