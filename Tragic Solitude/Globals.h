#pragma once

#include <Windows.h>
#include "SlimMem.h"

extern SlimUtils::SlimMem mem;
extern DWORD pid;
extern uintptr_t base;
extern const SlimUtils::SlimModule* mod;