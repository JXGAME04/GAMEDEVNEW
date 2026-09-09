# [ANDROID 08/09] Tai nguon SDL3 (cung phien ban voi goi VC dung san o ThirdParty\SDL3) vao ThirdParty\SDL3-src\SDL3-<ver>
# (khong dua vao git: ~100 MB). CMake Android (android/CMakeLists.txt) add_subdirectory thu muc nay de dung libSDL3.so.
# Nguon: https://github.com/libsdl-org/SDL/releases (giay phep zlib). Chay: powershell -File android\tai_sdl3_src.ps1
$root = Split-Path -Parent $PSScriptRoot
$verH = Get-Content "$root\ThirdParty\SDL3\include\SDL3\SDL_version.h"
$maj = ($verH | Select-String 'define SDL_MAJOR_VERSION\s+(\d+)').Matches[0].Groups[1].Value
$min = ($verH | Select-String 'define SDL_MINOR_VERSION\s+(\d+)').Matches[0].Groups[1].Value
$mic = ($verH | Select-String 'define SDL_MICRO_VERSION\s+(\d+)').Matches[0].Groups[1].Value
$ver = "$maj.$min.$mic"
$dst = "$root\ThirdParty\SDL3-src"
if (Test-Path "$dst\SDL3-$ver\CMakeLists.txt") { "da co SDL3-$ver"; exit 0 }
New-Item -ItemType Directory -Force $dst | Out-Null
$url = "https://github.com/libsdl-org/SDL/releases/download/release-$ver/SDL3-$ver.tar.gz"
"tai $url"
Invoke-WebRequest -Uri $url -OutFile "$dst\SDL3-$ver.tar.gz"
tar -xzf "$dst\SDL3-$ver.tar.gz" -C $dst
"xong: $dst\SDL3-$ver (" + (Get-Item "$dst\SDL3-$ver.tar.gz").Length + " B)"
