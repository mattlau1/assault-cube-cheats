#include <windows.h>

#include <iostream>
#include <vector>

#include "mem.h"
#include "proc.h"

int main(int argv, char *argc[]) {
    uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0, grenadeAddr = 0;
    bool bHealth = false, bAmmo = false, bRecoil = false, bGrenade = false;
    const int newValue = 99999;
    const char *procName = "ac_client.exe";

    DWORD procId = GetProcId(procName);
    if (!procId || procId == 0) {
        std::cout << "Process Id could not be retrieved" << std::endl;
        return 1;
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, (WINBOOL) false, procId);
    moduleBase = GetModuleBaseAddress(procId, procName);

    // resolve address to player pointer
    localPlayerPtr = moduleBase + 0x10f4f4;

    // get health address pointer
    healthAddr = FindDMAAddy(hProc, localPlayerPtr, {0xF8});
    DWORD dwExit = 0;

    // check if keys are pressed
    while (GetExitCodeProcess(hProc, &dwExit) && dwExit == STILL_ACTIVE) {
        // infinite HP
        if (GetAsyncKeyState(VK_F1) & 1) {
            bHealth = !bHealth;
            const char *status = bHealth ? "ON" : "OFF";
            std::cout << "Infinite/Frozen HP: " << status << std::endl;
        }

        // unlimited ammo
        if (GetAsyncKeyState(VK_F2) & 1) {
            bAmmo = !bAmmo;
            const char *status = bAmmo ? "ON" : "OFF";
            std::cout << "Unlimited Ammo: " << status << std::endl;

            if (bAmmo) {
                // FF06 = inc [esi]
                mem::PatchEx((BYTE *)(moduleBase + 0x637e9), (BYTE *)"\xFF\x06", 2, hProc);
            } else {
                // FF0E = dec [esi]
                mem::PatchEx((BYTE *)(moduleBase + 0x637e9), (BYTE *)"\xFF\x0E", 2, hProc);
            }
        }

        // no recoil NO OP
        if (GetAsyncKeyState(VK_F3) & 1) {
            bRecoil = !bRecoil;
            const char *status = bRecoil ? "ON" : "OFF";
            std::cout << "No Recoil: " << status << std::endl;

            if (bRecoil) {
                // nop recoil assembly code
                mem::NopEx((BYTE *)(moduleBase + 0x63786), 10, hProc);
            } else {
                // add recoil back in with original code
                mem::PatchEx((BYTE *)(moduleBase + 0x63786),
                             (BYTE *)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10, hProc);
            }
        }

        // infinite grenades
        if (GetAsyncKeyState(VK_F4) & 1) {
            bGrenade = !bGrenade;
            const char *status = bGrenade ? "ON" : "OFF";
            std::cout << "Infinite Grenades: " << status << std::endl;

            if (bGrenade) {
                // inc [eax] instead of dec when called
                mem::PatchEx((BYTE *)(moduleBase + 0x63378), (BYTE *)"\xFF\x00", 2, hProc);
            } else {
                mem::PatchEx((BYTE *)(moduleBase + 0x63378), (BYTE *)"\xFF\x08", 2, hProc);
            }
        }

        if (GetAsyncKeyState(VK_F5) & 1) {
            // close program
            return 0;
        }

        // Continuous write to HP address
        if (bHealth) {
            // set hp to newValue
            mem::PatchEx((BYTE *)healthAddr, (BYTE *)&newValue, sizeof(newValue), hProc);
        } else {
            // reset hp to 100
            const int normalHP = 100;
            mem::PatchEx((BYTE *)healthAddr, (BYTE *)&normalHP, sizeof(newValue), hProc);
        }

        // sleep 10ms to lower cpu usage
        Sleep(10);
    }
}