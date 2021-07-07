//
// pch.h
// Header for standard system include files.
//

#pragma once

#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#define USE_XAUDIO2
#define GTEST_HAS_TR1_TUPLE 0
#define GTEST_HAS_STD_TUPLE_ 1
#include <gtest/gtest.h>

#include <Snowing.h>
#include <PlatformImpls.h>
#include <D3DDevice.h>
#include <Windows.h>
#include <thread>
#include <fstream>

using namespace Snowing;
using namespace Snowing::Graphics;
using namespace Snowing::PlatformImpls::WindowsImpl;

constexpr Math::Vec2<size_t> WinSize{ 800,600 };

template <typename T,typename U>
void Assert(const T& a, const U& b)
{
	if (a != b)
		std::abort();
}