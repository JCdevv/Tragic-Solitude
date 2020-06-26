#include <iostream>
#include "SlimMem.h"
#include <Windows.h>
#include "Glow.h"
#include "Globals.h"

//Struct containing various needed offsets.
struct offset {
	DWORD dwLocalPlayer = 0xD3ABEC;
	DWORD GlowIndex = 0xA438;
	DWORD dwGlowObjectManager = 0x5297080;
	DWORD entityList = 0x4D4F25C;
	DWORD team = 0xF4;
} offset;



void esp::glow() {
	std::cout << base;
	std::cout << "hELLO";

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
}

