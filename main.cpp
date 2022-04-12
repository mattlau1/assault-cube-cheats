#include <windows.h>

#include <iostream>
#include <vector>

#include "proc.h"

int main(int argv, char *argc[]) {
    const char *procName = "ac_client.exe";

    // get proc id
    DWORD procId = GetProcId(procName);
    if (procId == 0) {
        std::cout << "ProcId could not be retrieved" << std::endl;
        return 1;
    }

    // module base addr
    uintptr_t moduleBaseAddr = GetModuleBaseAddress(procId, procName);
    if (moduleBaseAddr == 0) {
        std::cout << "Base address could not be retrieved" << std::endl;
        return 1;
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, (WINBOOL)true, procId);
    if (!hProc) {
        std::cout << "Process could not be opened" << std::endl;
        return 1;
    }

    // get base address of pointer chain (entity object pointer)
    // ac_client.exe+10F4F4
    uintptr_t dynamicPtrBaseAddr = moduleBaseAddr + 0x10f4f4;

    std::cout << "DynamicPtrBaseAddr: 0x" << std::hex << dynamicPtrBaseAddr << std::endl;

    // get offsets, found using cheat engine
    std::vector<unsigned int> healthOffsets = {0xF8};

    // print health address by using offsets
    uintptr_t healthAddr = FindDMAAddy(hProc, dynamicPtrBaseAddr, healthOffsets);
    std::cout << "healthAddr = " << "0x" << std::hex << healthAddr << std::endl;

    // print curr health
    int healthValue = 0;
    ReadProcessMemory(hProc, (BYTE *)healthAddr, &healthValue, sizeof(healthValue), nullptr);
    std::cout << "Current health: " << std::dec << healthValue << std::endl;

    // write new value to memory
    int newHealthVal = 10000;
    WriteProcessMemory(hProc, (BYTE *)healthAddr, &newHealthVal, sizeof(newHealthVal), nullptr);

    // read it again and print
    ReadProcessMemory(hProc, (BYTE *)healthAddr, &healthValue, sizeof(healthValue), nullptr);
    std::cout << "New Health: " << std::dec << healthValue << std::endl;

    return 0;
}