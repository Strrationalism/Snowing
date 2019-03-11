#include "pch.h"
#include <Snowing.h>
#include <Steam.h>
#include <fstream>


TEST(Init, InitSteamAPI) {
	Snowing::PlatformImpls::WindowsSteam::Steam steam;
}