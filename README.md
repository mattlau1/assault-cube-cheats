# Assault Cube Cheats
## What is this?
This project repo contains external and internal cheats for Assault Cube based off code from some beginner tutorials.

## external_cheat
- This folder contains the source code and a compiled `.exe` file
- This cheat just sets your HP to 10000
- After opening Assault Cube, open `main.exe` to run the cheat

## external_cheatv2
- This folder also contains the source code and a compiled `.exe` file
- This one is a better version of `external_cheat` since it includes memory manipulation
- After opening Assault Cube, open `main.exe` to run the cheat

## internal_cheat
- This folder contains a Visual Studio 2022 project which features an internal cheat for Assault Cube.
- This cheat contains infinite HP, ammo, grenades and a no recoil cheat (activate with F1-F4 keys)
- The compiled .dll file is in `comp6841-project/internal_cheat/Release/`
- The source code is in `comp6841-project/internal_cheat/src/`
- To use this, inject the `.dll` file into the Assault Cube client with a DLL injector. I recommend this one: https://github.com/DarthTon/Xenos

## ac_internal_gui
- It also contains the same features as `internal_cheat`, but has a GUI
- The compiled .dll file is in `comp6841-project/ac_internal_gui/Release/ac_internal_gui.dll`
- The source code is in `comp6841-project/ac_internal_gui/src/src/`
- The GUI code was based off https://github.com/adafcaefc/Universal-OpenGL-2-Kiero-Hook, which utilises [Kiero](https://github.com/Rebzzel/kiero) and [ImGUI](https://github.com/ocornut/imgui)

## Side Notes
- `external_cheat` and `external_cheatv2` were compiled with the Windows API from https://www.mingw-w64.org/, following [this guide](https://code.visualstudio.com/docs/cpp/config-mingw) 
- `internal_cheat` and `ac_internal_gui` were compiled with the Windows API from Visual Studio 2022
- Both APIs were slightly different, so both external cheats need to be slightly modified (change `const char *`s to `const wchar_t *`s) to compile with Visual Studio 2022
