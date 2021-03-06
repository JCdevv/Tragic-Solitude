#include <iostream>
#include "SlimMem.h"
#include <Windows.h>
#include "Wallhack.h"
#include "Globals.h"
#include <TlHelp32.h>


int screenX = GetSystemMetrics(SM_CXSCREEN);
int screenY = GetSystemMetrics(SM_CYSCREEN);
HBRUSH TeamMaxHP = CreateSolidBrush(RGB(0, 255, 0));
HBRUSH EnemyMaxHP = CreateSolidBrush(RGB(255, 0, 0));

//Struct containing various needed offsets.
struct offsets {
	DWORD dwLocalPlayer = 0xD3ABEC;
	DWORD dwViewMatrix = 0x4D40BA4;
    DWORD health = 0x100;
    DWORD vecOrigin = 0x138;
	DWORD entityList = 0x4D4F25C;
	DWORD team = 0xF4;
} offsets;

HDC hdc = GetDC(FindWindowA(NULL, "Counter-Strike: Global Offensive"));

struct view_matrix_t {
	float* operator[ ](int index) {
		return matrix[index];
	}

	float matrix[4][4];
};

struct Vector3 {
	float x, y, z;
};

Vector3 WorldToScreen(const Vector3 pos, view_matrix_t matrix) {
    float _x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
    float _y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];

    float w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];

    float inv_w = 1.f / w;
    _x *= inv_w;
    _y *= inv_w;

    float x = screenX * .5f;
    float y = screenY * .5f;

    x += 0.5f * _x * screenX + 0.5f;
    y -= 0.5f * _y * screenY + 0.5f;

    return { x,y,w };
}

void DrawTeam(int x, int y, int w, int h,int hp) {
    
    RECT rect = { x,y,x + w,y + h };
    FillRect(hdc, &rect, TeamMaxHP);
}

void DrawEnemy(int x, int y, int w, int h,int hp) {

    RECT rect = { x,y,x + w,y + h };
    FillRect(hdc, &rect, EnemyMaxHP);
}

void DrawBorder(int x, int y, int w, int h, int thickness,int hp,bool currentTeam)
{
    if (currentTeam) {
        DrawTeam(x, y, w, thickness,hp);
        DrawTeam(x, y, thickness, h,hp);
        DrawTeam((x + w), y, thickness, h,hp);
        DrawTeam(x, y + h, w + thickness, thickness,hp);
    }
    else {
        DrawEnemy(x, y, w, thickness,hp);
        DrawEnemy(x, y, thickness, h,hp);
        DrawEnemy((x + w), y, thickness, h,hp);
        DrawEnemy(x, y + h, w + thickness, thickness,hp);
    }
    
}

void walls::load() {
    while (true) {
        view_matrix_t matrix = mem.Read<view_matrix_t>(base + offsets.dwViewMatrix);
        auto entityList = mem.Read<DWORD>(base + offsets.entityList);
        auto localPlayer = mem.Read<DWORD>(base + offsets.dwLocalPlayer);
        auto team = mem.Read<int>(localPlayer + offsets.team);

        for (int i = 1; i < 64; i++) {
            auto entity = mem.Read<DWORD>(base + offsets.entityList + i * 0x10);
            auto entityHealth = mem.Read<int>(entity + offsets.health);

            auto entityTeam = mem.Read<int>(entity + offsets.team);
            Vector3 vec = mem.Read<Vector3>(entity + offsets.vecOrigin);
            Vector3 vecHead;
            vecHead.x = vec.x;
            vecHead.y = vec.y;
            vecHead.z = vec.z + 75.f;

            Vector3 screen = WorldToScreen(vec, matrix);
            Vector3 head = WorldToScreen(vecHead, matrix);

            float height = head.y - screen.y;
            float width = height / 2.4f;

            //If player is visible on screen
            if (screen.z >= 0.01f && team != entityTeam && entityHealth > 0 && entityHealth <= 100) {
                DrawBorder(screen.x - (width / 2), screen.y, width, height, 1, entityHealth, false);
            }
            else if (screen.z >= 0.01f && team == entityTeam && entityHealth > 0 && entityHealth <= 100) {
                DrawBorder(screen.x - (width / 2), screen.y, width, height, 1, entityHealth, true);
            }
        }
    }
}