// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>

#include "mem.h"
#include "proc.h"

DWORD WINAPI HackThread(HMODULE hModule) {
    // Create Console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    // calling it with NULL also gives you the address of the .exe module
    moduleBase = (uintptr_t)GetModuleHandle(NULL);

    bool bHealth = false, bAmmo = false, bRecoil = false, bGrenade = false;
    std::cout << "Assault Cube Cheats successfully loaded\n";

    while (true) {
        if (GetAsyncKeyState(VK_F5) & 1) {
            break;
        }

        if (GetAsyncKeyState(VK_F1) & 1) { 
            bHealth = !bHealth;
            const char* status = bHealth ? "ON" : "OFF";
            std::cout << "Infinite/Frozen HP: " << status << std::endl;
        }

        if (GetAsyncKeyState(VK_F2) & 1) {
            bAmmo = !bAmmo;
            const char* status = bAmmo ? "ON" : "OFF";
            std::cout << "Unlimited Ammo: " << status << std::endl;
        }

        // no recoil NOP
        if (GetAsyncKeyState(VK_F3) & 1) {
            bRecoil = !bRecoil;
            const char* status = bRecoil ? "ON" : "OFF";
            std::cout << "No Recoil: " << status << std::endl;
            if (bRecoil) {
                mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
            } else {
                // 50 8D 4C 24 1C 51 8B CE FF D2 the original stack setup and call
                mem::Patch((BYTE*)(moduleBase + 0x63786),
                    (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
            }
        }

        // infinite grenades
        if (GetAsyncKeyState(VK_F4) & 1) {
            bGrenade = !bGrenade;
            const char* status = bGrenade ? "ON" : "OFF";
            std::cout << "Infinite Grenades: " << status << std::endl;
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
            } else {
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
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CloseHandle(
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr)
        );
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
