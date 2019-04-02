#pragma once
#include <vector>
#include "Blob.h"

namespace Snowing
{
	class[[nodiscard]] BlobWriter final
	{
	private:
		std::vector<uint8_t> blob_;

	public:

		template <typename TStruct>
		void Write(const TStruct& data)
		{
			static_assert(std::is_standard_layout<TStruct>::value);
			blob_.reserve(blob_.size() + sizeof(TStruct));
			const std::uint8_t *ptr = reinterpret_cast<const uint8_t*>(&data);
			for (size_t i = 0; i < sizeof(TStruct); ++i)
				blob_.push_back(ptr[i]);
		}

		template <typename TChar>
		void WriteString(std::basic_string_view<TChar> ch)
		{
			blob_.reserve(blob_.size() + sizeof(std::uint64_t) + ch.size() * sizeof(TChar));
			Write(static_cast<std::uint64_t>(ch.size()));
			for (auto p : ch)
				Write(static_cast<TChar>(p));
		}

		Blob Borrow()
		{
			return { &blob_,false };
		}

		Blob Copy()
		{
			return { &blob_,true };
		}

		
	};
}
