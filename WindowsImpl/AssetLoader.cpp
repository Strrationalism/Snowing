#include "stdafx.h"
#include "AssetLoader.h"

using namespace Snowing;
using namespace std;
Blob Snowing::PlatformImpls::WindowsImpl::ReadFile(const char * fileName)
{
	std::ifstream in{ fileName,std::ios::binary };

	if (in.fail())
	{
		char filePath[128];
		sprintf_s(filePath, "../data/%s", fileName);
		in.open(filePath, std::ios::binary);
		if (in.fail())
			throw std::runtime_error{ "Can not find file:"s + fileName };
	}

	in.seekg(0, std::ios::end);
	const auto size = static_cast<size_t>(in.tellg());

	Blob b{ size };

	in.seekg(0, std::ios::beg);
	in.read(b.Get<char*>(), size);
	in.close();

	return b;
}
