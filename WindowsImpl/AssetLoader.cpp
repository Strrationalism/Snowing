#include "stdafx.h"
#include "AssetLoader.h"

using namespace Snowing;

Blob Snowing::PlatformImpls::WindowsImpl::ReadFile(const char * fileName)
{
	std::ifstream in{ fileName,std::ios::binary };

	if (in.fail())
	{
		in.open(std::string{ "../data/" } +fileName, std::ios::binary);
		if (in.fail())
			throw std::runtime_error{ "Can not find file!" };
	}

	in.seekg(0, std::ios::end);
	const auto size = static_cast<size_t>(in.tellg());

	Blob b{ size };

	in.seekg(0, std::ios::beg);
	in.read(b.Get<char*>(), size);
	in.close();

	return b;
}
