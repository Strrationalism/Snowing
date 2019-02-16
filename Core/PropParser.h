#pragma once
#include "Blob.h"
#include "CSVParser.h"
#include "Convert.h"
namespace Snowing
{
	template <typename CharType>
	class[[nodiscard]] PropParser final
	{
	private:
		const Blob &blob_;
		using StringView = std::basic_string_view<CharType>;
	
	public:
		PropParser(const Blob *b):
			blob_{ *b }
		{}

		StringView Get(StringView requestKey) const
		{
			CSVParser<CharType> csv(&blob_);

			do
			{
				const auto line = csv.CurrentLine();
				const auto equalTokenPosition = line.find('=');
				if (equalTokenPosition == StringView::npos)
					continue;
				else
				{
					auto key = line.substr(0, equalTokenPosition);
					{
						const auto firstNotSpace = key.find_first_not_of(' ');
						if(firstNotSpace != StringView::npos)
							key.remove_prefix(firstNotSpace);
						const auto lastNotSpace = key.find_last_not_of(' ');
						if (lastNotSpace != StringView::npos)
							key = key.substr(0,lastNotSpace + 1);

						if (key == requestKey)
						{
							auto value = line.substr(equalTokenPosition + 1, line.length() - equalTokenPosition - 1);
							const auto firstQuote = value.find('\"');
							if (firstQuote != StringView::npos)
							{
								value.remove_prefix(firstQuote + 1);
								const auto lastQuote = value.rfind('\"');
								if (lastQuote != StringView::npos)
									value = value.substr(0, lastQuote);
							}
							return value;
						}
					}
				}
			} while (csv.NextLine());
			throw std::out_of_range{ "Can not find key." };
		}

		template <typename T>
		T Get(StringView requestKey)
		{
			return To<T>(Get(requestKey));
		}
	};
}