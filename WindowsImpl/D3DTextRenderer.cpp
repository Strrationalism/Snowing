#include "stdafx.h"
#include "Impls.h"

using namespace Snowing;
using namespace Snowing::Graphics;

struct FontHead
{
	uint16_t charCount;
	uint16_t faceCount;
};

struct FontCharInfo
{
	wchar_t charater;
	uint16_t faceID;
	uint16_t spriteID;
};


struct FontFaceInfo
{
	uint32_t faceID;
	uint32_t offset;
	uint32_t size;
};

static std::tuple<
	std::vector<SpriteSheet>,
	std::vector<Texture2D>,
	std::map<wchar_t, std::pair<uint16_t, uint16_t>>>
	loadFont(
		const Blob& blob)
{
	const auto pHead = blob.Get<FontHead*>();
	const auto pFaceInfos = blob.Get<FontFaceInfo*>(sizeof(FontHead));
	const auto pChInfos = blob.Get<FontCharInfo*>(sizeof(FontHead) + sizeof(FontFaceInfo) * pHead->faceCount);


	std::vector<SpriteSheet> sheets;
	sheets.resize(pHead->faceCount);

	std::vector<Texture2D> tex;
	for (uint16_t i = 0; i < pHead->faceCount; ++i)
	{
		assert(i == pFaceInfos[i].faceID);
		tex.emplace_back(
			LoadSpriteAndSpriteSheet(
				blob.BorrowSlice(
					pFaceInfos[i].size,
					pFaceInfos[i].offset),
				sheets[i]));
	}

	std::map<wchar_t, std::pair<uint16_t, uint16_t>> chars;
	for (uint16_t i = 0; i < pHead->charCount; ++i)
		chars[pChInfos[i].charater] = std::make_pair(pChInfos[i].faceID, pChInfos[i].spriteID);

	return std::make_tuple(std::move(sheets), std::move(tex), std::move(chars));
}

Font Snowing::Graphics::LoadFont(const Blob & b)
{
	auto[sheet, tex, ch] = loadFont(b);
	std::vector<Buffer> buffers;
	for (auto& p : sheet)
		buffers.emplace_back(Snowing::Graphics::SpriteRenderer<>::MakeGPUSpriteSheet(p));
	return
	{
		std::move(tex),
		std::move(buffers),
		std::move(sheet),
		std::move(ch)
	};
}
