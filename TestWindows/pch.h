//
// pch.h
// Header for standard system include files.
//

#pragma once

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

const static inline auto ignore = _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
/* 注意：有4个内存泄露区块，这是正常的。*/

#define USE_XAUDIO2
#define GTEST_HAS_TR1_TUPLE 0
#define GTEST_HAS_STD_TUPLE_ 1
#include "gtest/gtest.h"

#include <Snowing.h>
#include <PlatformImpls.h>
#include <D3DDevice.h>
#include <Windows.h>
#include <XInputController.h>
#include <thread>
#include <fstream>

using namespace Snowing;
using namespace Snowing::Graphics;
using namespace Snowing::PlatformImpls::WindowsImpl;

constexpr Math::Vec2<int> WinSize{ 800,600 };

template <typename T,typename U>
void Assert(const T& a, const U& b)
{
	if (a != b)
		std::abort();
}