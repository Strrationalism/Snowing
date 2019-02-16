#pragma once
#include <cassert>
#include <string_view>
#include <charconv>
#include "Convert.h"
#include "Blob.h"

namespace Snowing
{
	/* CSV 规则 */
	/* 1. 各个cell使用逗号分隔。
	/* 2. 编码使用Unicode或者ANSI。
	/* 3. cell内不准出现逗号、双引号、单引号和换行符
	/* 4. 每行使用/r/n作为结尾。*/
	/* 5. 使用#作为单行注释符号。 */

	template <typename CharType>
	class[[nodiscard]] CSVParser final
	{
	private:
		using StringView = std::basic_string_view<CharType>;

		StringView fullText_;
		StringView curLine_;
		size_t curLineEndPosition_ = 0;
		size_t cellPosition_ = 0;


		bool setCurLine(size_t startPos)
		{
			cellPosition_ = 0;
			curLineEndPosition_ = fullText_.find('\n', startPos);
			if (curLineEndPosition_ == StringView::npos)
			{
				if(startPos >= fullText_.length())
					return false;
				else
				{
					curLineEndPosition_ = fullText_.length();
					curLine_ = fullText_.substr(startPos, fullText_.length() - startPos);

					const auto comment = curLine_.find('#');
					if (comment != StringView::npos)
						curLine_ = curLine_.substr(0, comment);
					return true;
				}
			}
			else
			{
				curLine_ = fullText_.substr(startPos, curLineEndPosition_ - startPos);

				if (curLine_.empty())
					return setCurLine(startPos + 1);
				else if (curLine_.front() == '#')
					return setCurLine(curLineEndPosition_ + 1);
				else if (curLine_.back() == '\r')
				{
					curLine_ = curLine_.substr(0, curLine_.length() - 1);
					if (curLine_.empty())
						return setCurLine(startPos + 1);
				}

				const auto comment = curLine_.find('#');
				if (comment != StringView::npos)
					curLine_ = curLine_.substr(0, comment);
				return true;
			}
		}

		

	public:
		CSVParser(const Snowing::Blob *blob) :
			fullText_{ blob->Get<CharType*>(),blob->Size() / sizeof(CharType) }
		{
			assert(blob);

			if (fullText_.front() == 65279u)
				fullText_ = fullText_.substr(1, fullText_.length() - 1);

			if (!setCurLine(0))
				throw std::invalid_argument{ "This is not a csv file!" };
		}

		StringView FullText() const
		{
			return fullText_;
		}

		StringView CurrentLine() const
		{
			return curLine_;
		}

		bool NextLine()
		{
			return setCurLine(curLineEndPosition_);
		}

		void Reset()
		{
			setCurLine(0);
		}

		StringView Pop()
		{
			if (cellPosition_ >= curLine_.length())
				throw std::out_of_range{ "Can not pop cell on line end." };
			auto comma = curLine_.find(',', cellPosition_);
			if (comma == StringView::npos)
				comma = curLine_.length();
			auto cell = curLine_.substr(cellPosition_, comma - cellPosition_);
			cellPosition_ = comma + 1;
			return cell;
		}

		template <typename T>
		T Pop()
		{
			return To<T>(Pop());
		}
	};
}
