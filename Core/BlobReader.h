#pragma once
#include "Blob.h"

namespace Snowing
{
	class[[nodiscard]] BlobReader final
	{
	private:
		const Blob * const blob_;
		size_t position_ = 0;

	public:
		BlobReader(const Blob *blob):
			blob_{ blob }
		{}

		template <typename TPOD>
		TPOD Read()
		{
			auto p = blob_->Get<TPOD*>(position_);
			position_ += sizeof(*p);

			static_assert(std::is_pod<TPOD>::value);

			return *p;
		}

		template <typename TChar>
		std::basic_string_view<TChar> ReadString()
		{
			auto str = blob_->GetString<TChar>(position_);
			position_ += sizeof(std::uint64_t);
			position_ += str.size() * sizeof(TChar);
			return str;
		}

		Blob BorrowSlice(size_t size)
		{
			auto blob = blob_->BorrowSlice(size, position_);
			position_ += size;
			return blob;
		}
	};
}
