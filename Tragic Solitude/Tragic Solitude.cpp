// Tragic Solitude.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SlimMem.h"
#include <Windows.h>
#include "Glow.h"
#include "Wallhack.h"
//#include "Globals.h"

SlimUtils::SlimMem mem;
DWORD pid;
uintptr_t base;
const SlimUtils::SlimModule *mod;


//Will be used for improving glow write logic.
/*struct glow {
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
};*/

//Struct containing various needed offsets.
/*
struct offset {
	DWORD dwLocalPlayer = 0xD2FB94;
	DWORD GlowIndex = 0xA428;
	DWORD dwGlowObjectManager = 0x528B8A0;
	DWORD entityList = 0x4D43AC4;
	DWORD team = 0xF4;
} offset;*/

bool getProcess() {
	//Get process ID of csgo.
	while (!SlimUtils::SlimMem::GetPID(L"csgo.exe", &pid))
		Sleep(500);
	
	//Return if process opened with full perms successfully.
	return mem.Open(pid, SlimUtils::ProcessAccess::Full);
}

bool getModule() {
	//Ensure a handle to the process has been made.
	if (!mem.HasProcessHandle()) {
		return false;
	}
	if (!mem.ParseModules()) {
		return false;
	}

	//Get the panorama module.
	mod = mem.GetModule(L"client.dll");
	
	
	//If the module is null, return
	if (mod == nullptr) {
		return false;
	}

	//Set base address to the base pointer of the module.
	base = mod->ptrBase;
	return true;
}

/*void glow() {
	//Get local player.
	auto localPlayer = mem.Read<DWORD>(base + offset.dwLocalPlayer);

	//If localPlayer is null, keep trying to access it until not null.
	if (localPlayer == NULL) {
		while (localPlayer == NULL) {
			localPlayer = mem.Read<DWORD>(base + offset.dwLocalPlayer);
		}
	}

	while (true) {
		auto glowObjectManager = mem.Read<DWORD>(base + offset.dwGlowObjectManager);
		auto team = mem.Read<int>(localPlayer + offset.team);

		for (int i = 0; i < 64; i++) {
			//Getting reading various values
			auto entity = mem.Read<DWORD>(base + offset.entityList + i * 0x10);
			auto entityTeam = mem.Read<int>(entity + offset.team);
			auto glow = mem.Read<int>(entity + offset.GlowIndex);

			//Writing glow, differing colours depending on enemies team.
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
}*/

int main(){

	//Attach to process, load panorama module and wait for user input
	if (getProcess()) {
		std::cout << "Attached To Process Successfully." << std::endl;
		if (getModule()) {
			std::cout << "Loaded Panorama Module Successfully. \nPress A Key To Choose An Option..." << std::endl;
			std::cout << "L - Exit" << std::endl;
			std::cout << "G - Enable Glow" << std::endl;
			std::cout << "X - Enable Walls" << std::endl;

			esp p;
			walls w;

			while (true) {
				if (GetKeyState('G') && 0x8000) {
					p.glow();
				}
				else if (GetKeyState('L') && 0x8000) {
					break;
				}
				else if (GetKeyState('X') && 0x8000) {
					w.load();
				}
			}
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



