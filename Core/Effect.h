#pragma once
#include <cassert>
#include "Platforms.h"
#include "Blob.h"
#include "Vec2.h"
#include "Vec4.h"
#include "Texture2D.h"

namespace Snowing::Graphics
{
	template <typename TImpl>
	class[[nodiscard]] EffectReflectionObjectInterface final
	{
	private:
		TImpl impl_;
	public:
		using ImplType = TImpl;

		EffectReflectionObjectInterface(TImpl&& impl) :
			impl_{ std::move(impl) }
		{}

		EffectReflectionObjectInterface(EffectReflectionObjectInterface&& r) = default;

		void SetAsFloat(float f)
		{
			impl_.SetAsFloat(f);
		}

		void SetAsVec2(Math::Vec2f f)
		{
			impl_.SetAsVec2(f);
		}

		void SetAsVec4(Math::Vec4f f)
		{
			impl_.SetAsVec4(f);
		}
		
		template <typename TTex>
		void SetAsTexture2D(const TTex* t)
		{
			assert(t);
			impl_.SetAsTexture2D(t);
		}

		void SetAsTexture2D(nullptr_t)
		{
			impl_.SetAsTexture2D(nullptr);
		}

		template <typename TBuf>
		void SetAsConstantBuffer(const TBuf* b)
		{
			assert(b);
			impl_.SetAsConstantBuffer(b);
		}
	};

	template <typename TImpl, typename TParameter>
	class [[nodiscard]] EffectReflectionInterface final
	{
		EffectReflectionInterface() = delete;
	};

	template <typename TImpl>
	class [[nodiscard]] EffectReflectionInterface<TImpl,float> final
	{
	private:
		using ReflectionObject = EffectReflectionObjectInterface<TImpl>;
		ReflectionObject ref_;
	public:
		EffectReflectionInterface(ReflectionObject&& ref) :ref_{ std::move(ref) } {}
		void Set(float v) { ref_.SetAsFloat(v); }
	};

	template <typename TImpl>
	class [[nodiscard]] EffectReflectionInterface<TImpl, Math::Vec2f> final
	{
	private:
		using ReflectionObject = EffectReflectionObjectInterface<TImpl>;
		ReflectionObject ref_;
	public:
		EffectReflectionInterface(ReflectionObject&& ref) :ref_{ std::move(ref) } {}
		void Set(Math::Vec2f v) { ref_.SetAsVec2(v); }
	};

	template <typename TImpl>
	class [[nodiscard]] EffectReflectionInterface<TImpl, Math::Vec4f> final
	{
	private:
		using ReflectionObject = EffectReflectionObjectInterface<TImpl>;
		ReflectionObject ref_;
	public:
		EffectReflectionInterface(ReflectionObject&& ref) :ref_{ std::move(ref) } {}
		void Set(Math::Vec4f v) { ref_.SetAsVec4(v); }
	};

	template <typename TImpl, typename TParameter>
	class [[nodiscard]] EffectReflectionInterface<TImpl, Texture2DInterface<TParameter>*> final
	{
	private:
		using ReflectionObject = EffectReflectionObjectInterface<TImpl>;
		using Tex2D = Texture2DInterface<TParameter>;
		ReflectionObject ref_;
	public:
		EffectReflectionInterface(ReflectionObject&& ref) :ref_{ std::move(ref) } {}
		void Set(const Tex2D* v) { ref_.SetAsTexture2D(v); }
		void Set(nullptr_t) { ref_.SetAsTexture2D(nullptr); }
	};

	template <typename TImpl, typename TConstantBuffer>
	class [[nodiscard]] EffectReflectionInterface<TImpl, TConstantBuffer*> final
	{
	private:
		using ReflectionObject = EffectReflectionObjectInterface<TImpl>;
		ReflectionObject ref_;
	public:
		EffectReflectionInterface(ReflectionObject&& ref) :ref_{ std::move(ref) } {}
		void Set(const TConstantBuffer* v) { ref_.SetAsConstantBuffer(v); }
		void Set(nullptr_t) { ref_.SetAsConstantBuffer(nullptr); }
	};

	template <typename TImpl>
	class[[nodiscard]] EffectPassInterface final
	{
	private:
		TImpl impl_;

	public:
		EffectPassInterface(TImpl&& impl) :
			impl_{ std::move(impl) }
		{}

		template <typename TGraphicsContext>
		void operator() (TGraphicsContext& ctx) const
		{
			impl_.Apply(ctx);
		}
	};

	enum class EffectDataElement : int
	{
		Position_Float32_2D = 0,
		Color_Float32_4D = 1,
		TexCoord_Float32_2D = 2,
		SpriteImageIndex_UInt32 = 3,
		Size_Float32_2D = 4,
		Center_Float32_2D = 5,
		Rotating_Float32 = 6
	};

	template <typename>
	class EffectInterface;

	template <typename TImpl,typename TEffectImpl> 
	class[[nodiscard]] EffectTechInterface final
	{
	private:
		TImpl impl_;
		EffectInterface<TEffectImpl> *effect_;
	public:
		EffectTechInterface(TImpl&& impl, EffectInterface<TEffectImpl> *effect) :
			impl_{ std::move(impl) },
			effect_{ effect }
		{}

		[[nodiscard]]
		auto& operator [] (size_t passID)
		{
			assert(passID < impl_.PassCount());
			auto& r = impl_.GetPass(passID);
			Platforms::AssertInterface<EffectPassInterface>(r);
			return r;
		}

		[[nodiscard]]
		size_t PassCount() const
		{
			return impl_.PassCount();
		}

		[[nodiscard]]
		EffectInterface<TEffectImpl> *  GetEffect()
		{
			return effect_;
		}
	};

	template <typename TImpl,typename TEffectImpl>
	class[[nodiscard]] EffectGroupInterface final
	{
	private:
		TImpl impl_;
		EffectInterface<TEffectImpl> *effect_;

	public:
		EffectGroupInterface(TImpl&& impl, EffectInterface<TEffectImpl> *effect) :
			impl_{ std::move(impl) },
			effect_{ effect }
		{}

		template <size_t N>
		[[nodiscard]]
		auto LoadTechnique(const char* techName, const EffectDataElement(&dataLayout)[N]) const
		{
			assert(techName);
			auto tech = impl_.LoadTechnique(techName, dataLayout,N, effect_);
			Platforms::AssertInterface<EffectTechInterface>(tech);
			return tech;
		}
	};

	template <typename TImpl>
	class[[nodiscard]] EffectInterface final
	{
	private:
		TImpl impl_;

	public:
		EffectInterface(EffectInterface&&) = delete;

		template <typename TDevice>
		explicit EffectInterface(const Snowing::Blob& fx) :
			impl_{ fx }
		{}

		EffectInterface(TImpl&& impl) :
			impl_{ std::move(impl) }
		{}

		template <typename TBuffer>
		void SetConstantBuffer(const char* name, const TBuffer& b)
		{
			assert(name);
			impl_.SetConstantBuffer(name, b);
		}

		template <typename TBuffer>
		void SetConstantBuffer(int id, const TBuffer& b)
		{
			impl_.SetConstantBuffer(id, b);
		}

		template <typename TTex>
		void SetTexture(const char* name, const TTex *tex)
		{
			assert(name);
			assert(tex);
			impl_.SetTexture(name, *tex);
		}

		[[nodiscard]]
		auto Reflection(const char* objectName)
		{
			assert(objectName);
			auto p = impl_.Reflection(objectName);
			Platforms::AssertInterface<EffectReflectionObjectInterface>(p);
			return p;
		}

		template <typename TReflectionObjectType>
		[[nodiscard]]
		auto Reflection(const char* objectName)
		{
			auto p = Reflection(objectName);
			return EffectReflectionInterface<decltype(p)::ImplType, TReflectionObjectType>(
				std::move(p));
		}

		[[nodiscard]]
		auto operator * ()
		{
			auto p = impl_.RootGroup(this);
			Platforms::AssertInterface<EffectGroupInterface>(p);
			return p;
		}

		template <size_t N>
		[[nodiscard]]
		auto LoadTechnique(const char* techName, const EffectDataElement(&dataLayout)[N])
		{
			assert(techName);
			auto p = impl_.RootGroup(this).LoadTechnique(techName, dataLayout);
			Platforms::AssertInterface<EffectTechInterface>(p);
			return p;
		}

		[[nodiscard]]
		auto operator [] (const char* groupName) const
		{
			assert(groupName);
			auto p = impl_.GetGroupByName(groupName);
			Platforms::AssertInterface<EffectGroupInterface>(p);
			return p;
		}
	};
}
