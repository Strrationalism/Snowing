#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#include "DInputController.h"
#define DIRECTINPUT_HEADER_VERSION 0x0800
#define DIRECTINPUT_VERSION       DIRECTINPUT_HEADER_VERSION
#include <dinput.h>
#include <COMHelper.h>
#include <Windows.h>
#include <WindowImpl.h>
using namespace Snowing::PlatformImpls::WindowsImpl;
using namespace Snowing::PlatformImpls::WindowsDInput;

static BOOL IsXInputDevice(const GUID* pGuidProductFromDirectInput);

static inline auto UnwrapDInput(const Handler& hnd)
{
	auto p = hnd.Get<IUnknown*>();
	return static_cast<LPDIRECTINPUT8>(p);
}


static inline auto UnwrapDevice(const Handler& hnd)
{
	auto p = hnd.Get<IUnknown*>();
	return static_cast<LPDIRECTINPUTDEVICE8>(p);
}


Snowing::PlatformImpls::WindowsDInput::DInputContext::DInputContext()
{
	LPDIRECTINPUT8 dinput8;
	COMHelper::AssertHResult("Can not create DInput8 context instance",
		DirectInput8Create(
			GetModuleHandle(nullptr),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&dinput8,
			nullptr
		)
	);

	context_ = { static_cast<IUnknown*>(dinput8),COMHelper::COMIUnknownDeleter };
}


using Pack = std::tuple<std::vector<DInputCntroller>*, bool, LPDIRECTINPUT8>;
BOOL FAR PASCAL DIEnumDevicesCallback(
	LPCDIDEVICEINSTANCE lpddi,
	LPVOID pvRef)
{
	Pack* pack = static_cast<Pack*>(pvRef);
	auto[pVec, exceptXInput,dinput] = *pack;

	if (exceptXInput)
		if (IsXInputDevice(&lpddi->guidInstance))
			return DIENUM_CONTINUE;

	LPDIRECTINPUTDEVICE8 device;
	if (FAILED(dinput->CreateDevice(lpddi->guidInstance, &device, nullptr)))
		return DIENUM_CONTINUE;

	Handler deviceHandler{ static_cast<IUnknown*>(device),COMHelper::COMIUnknownDeleter };
	if(FAILED(device->SetDataFormat(&c_dfDIJoystick)))
		return DIENUM_CONTINUE;

	const auto hwnd = Snowing::PlatformImpls::WindowsImpl::WindowImpl::Get().GetHWND().Get<HWND>();
	
	if (FAILED(device->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE)))
		return DIENUM_CONTINUE;

	if (FAILED(device->Acquire()))
		return DIENUM_CONTINUE;

	pVec->emplace_back(DInputCntroller{ std::move(deviceHandler) });
	return DIENUM_CONTINUE;
}

std::vector<DInputCntroller> Snowing::PlatformImpls::WindowsDInput::DInputContext::GetControllers(bool exceptXInputDevives) const
{
	std::vector<DInputCntroller> ret;
	auto dinput = UnwrapDInput(context_);
	Pack pack { &ret,exceptXInputDevives,dinput };

	COMHelper::AssertHResult("Can not enum devices from DInput8.",
		dinput->EnumDevices(DI8DEVCLASS_GAMECTRL, &DIEnumDevicesCallback, &pack, DIEDFL_ATTACHEDONLY));
	return ret;
}

Snowing::PlatformImpls::WindowsDInput::DInputControllerImpl::DInputControllerImpl(WindowsImpl::Handler && hnd) :
	device_{ std::move(hnd) },
	states_{ DIJOYSTATE{} }
{
}

Snowing::PlatformImpls::WindowsDInput::DInputControllerImpl::~DInputControllerImpl()
{
	if (device_.IsSome())
		UnwrapDevice(device_)->Unacquire();
}

bool Snowing::PlatformImpls::WindowsDInput::DInputControllerImpl::Update()
{
	const auto device = UnwrapDevice(device_);
	if (FAILED(device->Poll()))
		return false;

	static_assert(StateSize == sizeof(DIJOYSTATE));
	auto& states = states_.GetWrite<DIJOYSTATE>();
	device->GetDeviceState((DWORD)StateSize, reinterpret_cast<void*>(&states));

	return true;
}


Snowing::Math::Vec2f Snowing::PlatformImpls::WindowsDInput::DInputControllerImpl::Thumb(Input::ClassicGamepadThumbs a) const
{
	const auto& state = states_.Get<DIJOYSTATE>();
	Snowing::Math::Vec2<LONG> v;

	switch (a)
	{
	case Input::ClassicGamepadThumbs::LThumb:
		v = { state.lX,state.lY };
		break;
	case Input::ClassicGamepadThumbs::RThumb:
		v = { state.lRx,state.lRy };
		break;
	default:
		throw std::invalid_argument{ __FUNCDNAME__ "Thumb ID not supported." };
	}
	
	v -= Snowing::Math::Vec2<LONG>{32767L, 32766L};

	return
	{
		std::clamp(v.x / 32767.0f,-1.0f,1.0f),
		std::clamp(v.y / 32766.0f,-1.0f,1.0f)
	};
}

bool Snowing::PlatformImpls::WindowsDInput::DInputControllerImpl::KeyPressed(int k) const
{
	if (k >= KeyCounts)
		throw std::invalid_argument(__FUNCDNAME__ "Key ID is too big.");

	return (states_.Get<DIJOYSTATE>().rgbButtons[k] & 0x80) > 0;
}

bool Snowing::PlatformImpls::WindowsDInput::DInputControllerImpl::KeyPressed(Input::ClassicGamepadArrow arrow, int arrowID) const
{
	if (arrowID >= ArrowCounts)
		throw std::invalid_argument(__FUNCDNAME__ "Arrow Device ID is too big.");

	auto v = states_.Get<DIJOYSTATE>().rgdwPOV[arrowID];
	
	if ((LOWORD(v) == 0xFFFF))
		return false;
	else
	{
		switch (arrow)
		{
		case Snowing::Input::ClassicGamepadArrow::Up:
			return v >= 31500 || v <= 4500;
		case Snowing::Input::ClassicGamepadArrow::Down:
			return v >= 13500 && v <= 22500;
		case Snowing::Input::ClassicGamepadArrow::Left:
			return v >= 22500 && v <= 31500;
		case Snowing::Input::ClassicGamepadArrow::Right:
			return v >= 4500 && v <= 13500;
		default:
			throw std::invalid_argument(__FUNCDNAME__ "Arrow is not supported.");
		}
	}
}

float Snowing::PlatformImpls::WindowsDInput::DInputControllerImpl::Trigger(int t) const
{
	LONG v;
	const auto& state = states_.Get<DIJOYSTATE>();
	switch (t)
	{
	case 0:
		v = -std::clamp(state.lZ - 32767L, -32767L, 0L);
		break;
	case 1:
		v = std::clamp(state.lZ - 32767L, 0L, 32767L);
		break;
	case 2:
	case 3:
		v = -std::clamp(state.rglSlider[t-2] - 32767L, -32767L, 0L);
		break;
	case 4:
	case 5:
		v = std::clamp(state.rglSlider[t - 4] - 32767L, 0L, 32767L);
		break;
	default:
		throw std::invalid_argument(__FUNCDNAME__ "Trigger ID is too big!");
	}
	return std::clamp(v / 32600.f,0.0f,1.0f);
}



/*********** Magic code from M$DN **********/
// https://docs.microsoft.com/zh-cn/windows/desktop/xinput/xinput-and-directinput

#define SAFE_RELEASE(X) if((X) != nullptr) { (X)->Release(); (X) = nullptr; } 

#include <wbemidl.h>
#include <oleauto.h>

//-----------------------------------------------------------------------------
// Enum each PNP device using WMI and check each device ID to see if it contains 
// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
// Unfortunately this information can not be found by just using DirectInput 
//-----------------------------------------------------------------------------
BOOL IsXInputDevice(const GUID* pGuidProductFromDirectInput)
{
	IWbemLocator*           pIWbemLocator = NULL;
	IEnumWbemClassObject*   pEnumDevices = NULL;
	IWbemClassObject*       pDevices[20] = { 0 };
	IWbemServices*          pIWbemServices = NULL;
	BSTR                    bstrNamespace = NULL;
	BSTR                    bstrDeviceID = NULL;
	BSTR                    bstrClassName = NULL;
	DWORD                   uReturned = 0;
	bool                    bIsXinputDevice = false;
	UINT                    iDevice = 0;
	VARIANT                 var;
	HRESULT                 hr;

	// CoInit if needed
	hr = CoInitialize(NULL);
	bool bCleanupCOM = SUCCEEDED(hr);

	// Create WMI
	hr = CoCreateInstance(__uuidof(WbemLocator),
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWbemLocator),
		(LPVOID*)&pIWbemLocator);
	if (FAILED(hr) || pIWbemLocator == NULL)
		goto LCleanup;

	bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2"); if (bstrNamespace == NULL) goto LCleanup;
	bstrClassName = SysAllocString(L"Win32_PNPEntity");   if (bstrClassName == NULL) goto LCleanup;
	bstrDeviceID = SysAllocString(L"DeviceID");          if (bstrDeviceID == NULL)  goto LCleanup;

	// Connect to WMI 
	hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L,
		0L, NULL, NULL, &pIWbemServices);
	if (FAILED(hr) || pIWbemServices == NULL)
		goto LCleanup;

	// Switch security level to IMPERSONATE. 
	CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
		RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
	if (FAILED(hr) || pEnumDevices == NULL)
		goto LCleanup;

	// Loop over all devices
	for (;; )
	{
		// Get 20 at a time
		hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);
		if (FAILED(hr))
			goto LCleanup;
		if (uReturned == 0)
			break;

		for (iDevice = 0; iDevice < uReturned; iDevice++)
		{
			// For each device, get its device ID
			hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
			if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL)
			{
				// Check if the device ID contains "IG_".  If it does, then it's an XInput device
					// This information can not be found from DirectInput 
				if (wcsstr(var.bstrVal, L"IG_"))
				{
					// If it does, then get the VID/PID from var.bstrVal
					DWORD dwPid = 0, dwVid = 0;
					WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
					if (strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1)
						dwVid = 0;
					WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
					if (strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1)
						dwPid = 0;

					// Compare the VID/PID to the DInput device
					DWORD dwVidPid = MAKELONG(dwVid, dwPid);
					if (dwVidPid == pGuidProductFromDirectInput->Data1)
					{
						bIsXinputDevice = true;
						goto LCleanup;
					}
				}
			}
			SAFE_RELEASE(pDevices[iDevice]);
		}
	}

LCleanup:
	if (bstrNamespace)
		SysFreeString(bstrNamespace);
	if (bstrDeviceID)
		SysFreeString(bstrDeviceID);
	if (bstrClassName)
		SysFreeString(bstrClassName);
	for (iDevice = 0; iDevice < 20; iDevice++)
		SAFE_RELEASE(pDevices[iDevice]);
	SAFE_RELEASE(pEnumDevices);
	SAFE_RELEASE(pIWbemLocator);
	SAFE_RELEASE(pIWbemServices);

	if (bCleanupCOM)
		CoUninitialize();

	return bIsXinputDevice;
}

