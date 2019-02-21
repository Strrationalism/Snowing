#pragma once
#include <cassert>
#include <cstdint>
#include <utility>
#include <string_view>
#include <vector>

namespace Snowing
{
	class[[nodiscard]] Blob final
	{
	private:
		std::uint8_t* bytes_;
		size_t size_;

		bool owner_;
		bool aligned_;

		inline Blob(std::uint8_t* bytes, size_t size, bool owner) noexcept :
			bytes_{ bytes },
			size_{ size },
			owner_{ owner },
			aligned_{ false }
		{}

	public:
		inline Blob():
			bytes_{ nullptr },
			size_{ 0 },
			owner_{ false },
			aligned_{ false }
		{}

		explicit inline Blob(size_t size) :
			bytes_{ new std::uint8_t[size] },
			size_{ size },
			owner_{ true },
			aligned_{ false }
		{
			assert(size);
		}

		inline Blob(size_t size, size_t alignment):
			size_{ size },
			owner_{ true },
			aligned_{ true }
		{
			size_t offset, shift, alignedAddress;
			void* allocation;
			void** preamble;

			offset = alignment - 1 + sizeof(void*);

			allocation = static_cast<void*>(new uint8_t[size + static_cast<uint32_t>(offset)]);

			alignedAddress = reinterpret_cast<size_t>(allocation) + sizeof(void*);

			shift = alignedAddress % alignment;

			if (shift)
			{
				alignedAddress += (alignment - shift);
			}

			preamble = reinterpret_cast<void**>(alignedAddress);
			preamble[-1] = allocation;

			bytes_ = reinterpret_cast<uint8_t*>(alignedAddress);
		}

		inline Blob(std::vector<uint8_t>* blob,bool copyAndOwner = true):
			bytes_{ blob->data() },
			size_{ blob->size() },
			owner_{ copyAndOwner },
			aligned_{ false }
		{
			if (owner_)
			{
				bytes_ = new uint8_t[size_];
				memcpy_s(bytes_, size_, blob->data(), blob->size());
			}
		}

		inline ~Blob() noexcept
		{
			if (owner_)
			{
				if (!aligned_)
					delete[] bytes_;
				else
				{
					void** preamble = reinterpret_cast<void**>(bytes_);
					delete [] reinterpret_cast<uint8_t*>(preamble[-1]);
				}
			}
				
		}

		[[nodiscard]]
		Blob BorrowSlice(size_t size, size_t offset = 0) const
		{
			assert(offset + size <= size_);
			return { bytes_ + offset,size,false };
		}

		Blob(const Blob&) = delete;

		inline Blob(Blob&& b) noexcept :
			bytes_{ b.bytes_ },
			size_{ b.size_ },
			owner_{ b.owner_ },
			aligned_{ b.aligned_ }
		{
			b.size_ = 0;
			b.bytes_ = nullptr;
			b.owner_ = false;
			b.aligned_ = false;
		}

		Blob& operator= (const Blob&) = delete;

		inline Blob& operator= (Blob&& b) noexcept
		{
			assert(this != &b);

			if (owner_)
				delete[] bytes_;
			bytes_ = b.bytes_;
			size_ = b.size_;
			owner_ = b.owner_;
			aligned_ = b.aligned_;

			b.bytes_ = nullptr;
			b.size_ = 0;
			b.owner_ = false;
			b.aligned_ = false;

			return *this;
		}

		template <typename TPointer>
		[[nodiscard]]
		TPointer Get(std::size_t offset = 0) const noexcept
		{
			static_assert(std::is_pointer<TPointer>::value);
			TPointer ret = reinterpret_cast<TPointer>(bytes_ + offset);

			assert(nullptr != bytes_);
			assert(offset + sizeof(*ret) <= size_);

			return ret;
		}

		template <>
		void* Get<void*>(std::size_t offset) const noexcept
		{
			return reinterpret_cast<void*>(Get<std::uint8_t*>(offset));
		}

		template <typename TChar>
		[[nodiscard]]
		std::basic_string_view<TChar> GetString(std::size_t offset) const noexcept
		{
			size_t size = static_cast<size_t>(*Get<uint64_t*>(offset));
			return { Get<TChar*>(offset + sizeof(uint64_t)),size };
		}

		[[nodiscard]]
		size_t Size() const noexcept
		{
			return size_;
		}
	};
}
