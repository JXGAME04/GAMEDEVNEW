# [ANDROID 08/09] Tai nguon SDL3 vao ThirdParty\SDL3-src\SDL3-<ver>. [DONHIP 12/09 b] Android dung 3.2.30 (bca30aa + sua quay lai tu nen),
# KHONG con lay phien ban tu goi VC Windows ThirdParty\SDL3 (van 3.2.14). Phai khop JX_SDL3_VER trong android/CMakeLists.txt.
# (khong dua vao git: ~100 MB). CMake Android (android/CMakeLists.txt) add_subdirectory thu muc nay de dung libSDL3.so.
# Nguon: https://github.com/libsdl-org/SDL/releases (giay phep zlib). Chay: powershell -File android\tai_sdl3_src.ps1
param([string]$Ver = "3.2.30")   # [DONHIP 12/09 b] phien ban SDL cho Android (= JX_SDL3_VER trong android/CMakeLists.txt)
$root = Split-Path -Parent $PSScriptRoot
$ver = $Ver
$dst = "$root\ThirdParty\SDL3-src"
if (Test-Path "$dst\SDL3-$ver\CMakeLists.txt") { "da co SDL3-$ver"; exit 0 }
New-Item -ItemType Directory -Force $dst | Out-Null
$url = "https://github.com/libsdl-org/SDL/releases/download/release-$ver/SDL3-$ver.tar.gz"
"tai $url"
Invoke-WebRequest -Uri $url -OutFile "$dst\SDL3-$ver.tar.gz"
tar -xzf "$dst\SDL3-$ver.tar.gz" -C $dst
"xong: $dst\SDL3-$ver (" + (Get-Item "$dst\SDL3-$ver.tar.gz").Length + " B)"
