Start-BitsTransfer -Source https://github.com/msys2/msys2-installer/releases/download/2022-09-04/msys2-x86_64-20220904.exe -Destination .\msys2-x86_64-latest.exe
.\msys2-x86_64-latest.exe in --confirm-command --accept-messages --root C:/msys64
Remove-Item .\msys2-x86_64-latest.exe
