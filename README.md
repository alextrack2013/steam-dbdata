# Winmm Proxy
Used with [Frosty Mod Manager](https://github.com/maniman303/FrostyToolsuite) fork for Linux, alongside [CryptHook](https://github.com/maniman303/CryptHook).

### Dragon Age Inquisition controller support on Linux
This dll contains ported workaround to fix controller support in DAI, from https://github.com/cammoore1/DAI-proton-ge-custom. It should work with any Proton version. Workaround is applied via method hooking.

### Build
Requires VCPKG and MinHook installed: `.\vcpkg install minhook:x64-windows-static-md`.