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

		template <typename TPOD>
		void Write(const TPOD& data)
		{
			static_assert(std::is_pod<TPOD>::value);
			blob_.reserve(blob_.size() + sizeof(TPOD));
			const std::uint8_t *ptr = reinterpret_cast<const uint8_t*>(&data);
			for (size_t i = 0; i < sizeof(TPOD); ++i)
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
