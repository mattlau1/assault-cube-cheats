#pragma once
#ifndef __PROC_H__
#define __PROC_H__


#include <vector>
#include <windows.h>
#include <tlhelp32.h>

// returns process ID of corresponding process, if running.
// returns 0 if not found.
DWORD GetProcId(const wchar_t* procName);

// gets module base address of process
// returns 0 if not found
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

// gets dma address with offsets
uintptr_t FindDMAAddyEx(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
#endif // __PROC_H__