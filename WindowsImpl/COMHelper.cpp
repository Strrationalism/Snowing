#include "stdafx.h"
#include "COMHelper.h"

void Snowing::PlatformImpls::WindowsImpl::COMHelper::COMIUnknownDeleter(void * p)
{
	static_cast<IUnknown*>(p)->Release();
}

void Snowing::PlatformImpls::WindowsImpl::COMHelper::AssertHResult(const char* msg,std::uint32_t r)
{
	static_assert(sizeof(HRESULT) == sizeof(r));
	const auto res = (HRESULT)r;

	if (FAILED(r))
	{
		throw COMException(("COMException " + std::to_string(r) + ":" + msg).data());
	}
}
