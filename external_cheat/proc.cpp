#include "proc.h"

DWORD GetProcId(const char *procName) {
    DWORD procId = 0;

    // get a snapshot of all processes currently running
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    // check if invalid snapshot
    if (hSnap == INVALID_HANDLE_VALUE) return procId;

    PROCESSENTRY32 procEntry;
    procEntry.dwSize = sizeof(procEntry);

    if (Process32First(hSnap, &procEntry)) {
        // loop through processes in snapshot
        do {
            // wchar_t case insensitive string compare to find matching process name
            if (!_stricmp(procEntry.szExeFile, procName)) {
                // set return value to that process' process ID
                procId = procEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnap, &procEntry));
    }

    CloseHandle(hSnap);
    return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const char *modName) {
    uintptr_t modBaseAddress = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);

    if (hSnap == INVALID_HANDLE_VALUE) return modBaseAddress;

    MODULEENTRY32 modEntry;
    modEntry.dwSize = sizeof(modEntry);
    if (Module32First(hSnap, &modEntry)) {
        do {
            if (!_stricmp(modEntry.szModule, modName)) {
                modBaseAddress = (uintptr_t)modEntry.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnap, &modEntry));
    }
    CloseHandle(hSnap);
    return modBaseAddress;
}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); ++i) {
        ReadProcessMemory(hProc, (BYTE *)addr, &addr, sizeof(addr), 0);
        addr += offsets[i];
    }
    return addr;
}