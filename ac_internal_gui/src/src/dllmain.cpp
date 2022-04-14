#include <Windows.h>
#include "../include/imgui_hook.h"
#include "../imgui/imgui.h"
#include "mem.h"
#include "proc.h"
bool bHealth = false, bAmmo = false, bRecoil = false, bGrenade = false;

DWORD WINAPI HackThread(HMODULE hModule) {
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    // calling it with NULL also gives you the address of the .exe module
    moduleBase = (uintptr_t)GetModuleHandle(NULL);

    while (true) { 
        // infinite HP
        if (GetAsyncKeyState(VK_F1) & 1) {
            bHealth = !bHealth;
        }
            
        // infinite ammo
        if (GetAsyncKeyState(VK_F2) & 1) {
            bAmmo = !bAmmo;
        }

        // infinite grenades
        if (GetAsyncKeyState(VK_F3) & 1) {
            bGrenade = !bGrenade;
        }

        // no recoil NOP
        if (GetAsyncKeyState(VK_F4) & 1) {
            bRecoil = !bRecoil;
            if (bRecoil) {
                mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
            }
            else {
                // 50 8D 4C 24 1C 51 8B CE FF D2 the original stack setup and call
                mem::Patch((BYTE*)(moduleBase + 0x63786),
                    (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
            }
        }

        // exit
        if (GetAsyncKeyState(VK_F5) & 1) {
            break;
        }


        // need to use uintptr_t for pointer arithmetic later
        uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10F4F4);

        // continuous writes / freeze
        if (localPlayerPtr) {
            if (bHealth) {
                //*localPlayerPtr = dereference the pointer, to get the localPlayerAddr
                // add 0xF8 offset for health addr
                // cast to an int pointer, this pointer now points to the health address
                // dereference it and assign the value 1337 to the health variable it points to
                *(int*)(*localPlayerPtr + 0xF8) = 1000;
            }
            else {
                // if toggled off, reset health to 100
                *(int*)(*localPlayerPtr + 0xF8) = 100;
            }

            if (bAmmo) {
                // cast address to an int pointer, dereference it and change the value
                *(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 }) = 1000;
            }

            if (bGrenade) {
                // freeze grenades at 1000
                *(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x368, 0x14, 0x0 }) = 1000;
            } else {
                // reset grenades to 0
                *(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x368, 0x14, 0x0 }) = 0;
            }


        }
        Sleep(5);
    }

    fclose(f);
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

void RenderMain() 
{   
    int flags = (
        ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize 
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings 
        | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoFocusOnAppearing
    );
    ImVec2 windowSize = ImVec2(225, 135);
    ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x - (windowSize.x + 5) , ImGui::GetIO().DisplaySize.y - (windowSize.y + 25));
    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowBgAlpha(0.3f);
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
	ImGui::Begin("Matthew's Assault Cube Trainer", NULL, flags);
	ImGui::Checkbox("Infinite HP [F1]", &bHealth);
	ImGui::Checkbox("Infinite Ammo [F2]", &bAmmo);
	ImGui::Checkbox("Infinite Grenades [F3]", &bGrenade);
	ImGui::Checkbox("No Recoil [F4]", &bRecoil);
	ImGui::End();
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		ImGuiHook::Load(RenderMain);
        CloseHandle(
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hMod, 0, nullptr)
        );
		break;
	case DLL_PROCESS_DETACH:
		ImGuiHook::Unload();
		break;
	}
	return TRUE;
}