#include "L2DDevice.h"
#include <CubismFramework.hpp>
#include <Rendering/D3D11/CubismRenderer_D3D11.hpp>
#include <Snowing.h>
#include <string_view>

using namespace Live2D;
using namespace Csm;

// Live2D����Ҫ���ڴ������
class L2DAllocator final: public Csm::ICubismAllocator
{
public:
	void* Allocate(const Csm::csmSizeType size)
	{
		return malloc(size);
	}

	void Deallocate(void* memory)
	{
		return free(memory);
	}

	void* AllocateAligned(const Csm::csmSizeType size, const Csm::csmUint32 alignment)
	{
		size_t offset, shift, alignedAddress;
		void* allocation;
		void** preamble;

		offset = alignment - 1 + sizeof(void*);
		allocation = Allocate(size + static_cast<csmUint32>(offset));
		alignedAddress = reinterpret_cast<size_t>(allocation) + sizeof(void*);
		shift = alignedAddress % alignment;

		if (shift)
			alignedAddress += (alignment - shift);

		preamble = reinterpret_cast<void**>(alignedAddress);
		preamble[-1] = allocation;

		return reinterpret_cast<void*>(alignedAddress);
	}

	void DeallocateAligned(void* alignedMemory)
	{
		void** preamble = static_cast<void**>(alignedMemory);
		Deallocate(preamble[-1]);
	}
};

static L2DAllocator allocator;

Device::Device()
{
	Csm::CubismFramework::Option opt;
	opt.LogFunction = [](const char* log)
	{
		std::string_view sv = log;
		if (sv.back() == '\n')
			sv = sv.substr(0, sv.size() - 1);
		Snowing::Log(sv);
	};
	opt.LoggingLevel = opt.LogLevel_Off;
#ifdef _DEBUG
	opt.LoggingLevel = opt.LogLevel_Info;
#endif // _DEBUG

	Csm::CubismFramework::StartUp(&allocator, &opt);
	Csm::CubismFramework::Initialize();

	Csm::Rendering::CubismRenderer_D3D11::InitializeConstantSettings(2,
		Snowing::PlatformImpls::WindowsImpl::D3D::Device::Get().GetHandler().Cast<IUnknown*, ID3D11Device*>());
}

Device::~Device()
{
	Csm::CubismFramework::Dispose();
}

Snowing::Blob Live2D::DefaultAssetLoader(Snowing::AssetName homeDir, Snowing::AssetName file)
{
	char path[128];
	strcpy_s(path, homeDir);
	strcat_s(path, file);
	return Snowing::LoadAsset(path);
}
