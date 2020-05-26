// Tragic Solitude.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SlimMem.h"

SlimUtils::SlimMem mem;
DWORD pid;
uintptr_t base;
const SlimUtils::SlimModule *mod;

struct Glow {
	float baseBuffer; // buffer as red begins 4 btyes along
	float red; // red,green, blue alpha are 4 bytes
	float green;
	float blue;
	float alpha;
	float buffer[4]; // 4x4 = 16 byte buffer. 16 byte BYTE buffer?
	bool occludedTrue;
	bool occuldedFalse;
	BYTE Buffer[6]; // Buffers over fullBloom and bloomStencil
	int glowStyle;
};

struct offset {
	DWORD dwLocalPlayer = 0xD2FB94;
	DWORD GlowIndex = 0xA428;
	DWORD dwGlowObjectManager = 0x528B8A0;
	DWORD entityList = 0x4D43AC4;
	DWORD team = 0xF4;
} offset;

struct vars {
	DWORD localPlayer;
} val;

bool open() {
	std::cout << "Waiting for process..." << std::endl;
	while (!SlimUtils::SlimMem::GetPID(L"csgo.exe", &pid))
		Sleep(500);
	std::cout << "Opening process..." << std::endl;

	return mem.Open(pid, SlimUtils::ProcessAccess::Full);

}

bool loadModules() {
	if (!mem.HasProcessHandle()) {
		return false;
	}
	if (!mem.ParseModules()) {
		return false;
	}

	mod = mem.GetModule(L"client_panorama.dll");
	
	if (mod == nullptr) {
		std::cout << "Mod is null";
		return false;
	}

	std::cout << "\"Module Loaded\":" <<
		" Base=0x" << std::hex << mod->ptrBase <<
		" Size=0x" << std::hex << mod->dwSize << std::endl;

	base = mod->ptrBase;
	return true;
}

void doEverything() {
	auto localPlayer = mem.Read<DWORD>(base + offset.dwLocalPlayer);

	if (localPlayer == NULL) {
		while (localPlayer == NULL) {
			localPlayer = mem.Read<DWORD>(base + offset.dwLocalPlayer);
		}
	}

	while (true) {
		auto glowObjectManager = mem.Read<DWORD>(base + offset.dwGlowObjectManager);
		auto team = mem.Read<int>(localPlayer + offset.team);

		for (int i = 0; i < 64; i++) {
			auto entity = mem.Read<DWORD>(base + offset.entityList + i * 0x10);
			auto entityTeam = mem.Read<int>(entity + offset.team);
			auto glow = mem.Read<int>(entity + offset.GlowIndex);

			if (entityTeam == team) {
				mem.Write<float>(glowObjectManager + ((glow * 0x38) + 0x4), 0);
				mem.Write<float>(glowObjectManager + ((glow * 0x38) + 0x8), 0);
				mem.Write<float>(glowObjectManager + ((glow * 0x38) + 0xC), 2);
				mem.Write<float>(glowObjectManager + ((glow * 0x38) + 0x10), 1.7);
			}
			else {
				mem.Write<float>(glowObjectManager + ((glow * 0x38) + 0x4), 3);
				mem.Write<float>(glowObjectManager + ((glow * 0x38) + 0x8), 0);
				mem.Write<float>(glowObjectManager + ((glow * 0x38) + 0xC), 2);
				mem.Write<float>(glowObjectManager + ((glow * 0x38) + 0x10), 1.7);
			}

			mem.Write<bool>(glowObjectManager + ((glow * 0x38) + 0x24), true);
			mem.Write<bool>(glowObjectManager + ((glow * 0x38) + 0x25), false);
		}		
	}
}



int main(){

	if (open()) {
		std::cout << "Attached To Process Successfully." << std::endl;
		if (loadModules()) {
			std::cout << "Loaded Panorama Module Successfully." << std::endl;

			doEverything();

		}
		else {
			std::cout << "Module Failed To Load." << std::endl;
		}
	}
	else {
		std::cout << "Failed To Find Process. Make Sure CS:GO Is Open." << std::endl;
	}
	
	return 1;
	
}



