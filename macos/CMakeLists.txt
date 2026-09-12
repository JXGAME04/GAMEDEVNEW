# [MACOS 11/09] Client JX1 tren macOS (SDL3 + SDL_GPU/Metal + miniaudio + BSD socket).
# Cung nguon voi ban Android va voi GameSDL.exe: danh sach tep/dinh nghia dung LAI android/lists/*.cmake
# (sinh boi android/gen_lists.py tu cac vcxproj cau hinh ReleaseSDL) - KHONG chep tay danh sach tep.
#
# Khac ban Android o bon cho, tat ca deu co ly do:
#   1. Moi TU deu la thu vien TINH roi gop vao MOT chuong trinh: iOS khong nap thu vien dong tu ngoai goi.
#      => LoadLibraryA("Rainbow.dll" / "Represent3.dll" / "Lua54Dll.dll") phai doi sang bang tra tinh (viec cua buoc C).
#   2. Khong chay va_sdl3_donhip.py / va_sdl3_d1.py: hai ban va do CHI sua src/gpu/vulkan/SDL_gpu_vulkan.c,
#      ma ban iOS tat han Vulkan (SDL_VULKAN=OFF) va di duong Metal. Cay nguon SDL3 dung chung van an toan.
#   3. Khong kiem phien ban Java (khong co SDLActivity).
#   4. Dinh nghia JX_MOBILE + JX_IOS thay cho JX_ANDROID. Day la CO Y: vong bien dich dau tien se bao loi
#      o dung nhung cho dang rao JX_ANDROID ma le ra phai la JX_MOBILE -> do chinh la ban khao sat cua buoc B.
#
# Cach dung:
#   cmake -S ios -B build/ios-sim -G Xcode -DCMAKE_SYSTEM_NAME=iOS \
#         -DCMAKE_OSX_SYSROOT=iphonesimulator -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=15.0
#   cmake --build build/ios-sim --config Release
cmake_minimum_required(VERSION 3.22)
project(JX1Mac C CXX OBJC OBJCXX)

set(JX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(JX_IOS_DIR ${JX_ROOT}/ios)   # dung chung tep cua ban iOS (bang do, do so)
set(JX_SRC ${JX_ROOT}/Sources)
set(JX_ANDROID_DIR ${JX_ROOT}/android)

# ---- SDL3 tu nguon, dung chung cay voi ban Android ----
set(JX_SDL3_VER 3.2.30)
set(JX_SDL3_SRC ${JX_ROOT}/ThirdParty/SDL3-src/SDL3-${JX_SDL3_VER})
if(NOT EXISTS ${JX_SDL3_SRC}/CMakeLists.txt)
  message(FATAL_ERROR "Chua co nguon SDL3 ${JX_SDL3_VER} o ${JX_SDL3_SRC} - tai: curl -fL -o ThirdParty/SDL3-src/SDL3-${JX_SDL3_VER}.tar.gz https://github.com/libsdl-org/SDL/releases/download/release-${JX_SDL3_VER}/SDL3-${JX_SDL3_VER}.tar.gz && tar -xzf ... -C ThirdParty/SDL3-src")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT TARGET SDL3::SDL3)
  set(SDL_SHARED OFF CACHE BOOL "" FORCE)
  set(SDL_STATIC ON  CACHE BOOL "" FORCE)
  set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
  set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(SDL_VULKAN OFF CACHE BOOL "" FORCE)   # macOS di Metal; tat Vulkan de khoi keo MoltenVK
  add_subdirectory(${JX_SDL3_SRC} SDL3 EXCLUDE_FROM_ALL)
endif()

# ---- co chung cho moi TU cua game (giu bang voi android/CMakeLists.txt) ----
# -fsigned-char : char CO DAU nhu MSVC (byte TCVN3 > 0x7F, moi phep 'if (c < 0)')
# -U__linux     : tat cac nhanh port may chu Linux cu
# -D_WIN64      : di dung nhanh x64 da kiem chung (hop ngu -> C, con tro 8 byte); KHONG phai la Windows
# KHONG co JX_MOBILE: macOS chay giao dien ban PC (chuot + ban phim)
set(JX_COMMON_DEFS JX_PLATFORM_SDL JX_POSIX JX_APPLE JX_MACOS JX_NO_DIRECTX JX_D3D9MINI _WIN64 NDEBUG _MBCS USE_STANDALONE_SPR
    _LIBCPP_ENABLE_CXX17_REMOVED_FEATURES _LIBCPP_ENABLE_CXX20_REMOVED_FEATURES
    _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)   # libc++ cua Xcode 26 doi macro rieng cho std::auto_ptr (MultiServer/Common/Utils.cpp)
set(JX_COMMON_OPTS -fsigned-char -fms-extensions -U__linux -w -Wno-register -Wno-address-of-temporary -fno-strict-aliasing -Wno-error)
set(JX_COMMON_INCS
    ${JX_ROOT}/ios/shim          # header CHI cho iOS (malloc.h ...) - Android khong thay
    ${JX_SRC}/Engine/Src/Platform/winshim     # windows.h, winsock2.h, mmsystem.h ... -> KPosixCompat.h
    ${JX_SRC}/Engine/Src/Platform
    ${JX_SRC}/Engine/Src/Platform/posixssl    # openssl/*.h -> KPosixCrypto.cpp
    ${JX_ROOT}/ThirdParty/miniaudio
    ${JX_ROOT}/ThirdParty/stb)

function(jx_setup tgt)
  target_compile_definitions(${tgt} PRIVATE ${JX_COMMON_DEFS})
  target_compile_options(${tgt} PRIVATE ${JX_COMMON_OPTS})
  target_include_directories(${tgt} PRIVATE ${JX_COMMON_INCS})
  target_link_libraries(${tgt} PRIVATE SDL3::SDL3-static)
  if(NOT "${tgt}" STREQUAL "JxPosix")
    target_link_libraries(${tgt} PRIVATE JxPosix)
  endif()
endfunction()

# ---- danh sach nguon: DUNG CHUNG voi ban Android ----
include(${JX_ANDROID_DIR}/lists/lua54.cmake)
include(${JX_ANDROID_DIR}/lists/filtertext.cmake)
include(${JX_ANDROID_DIR}/lists/commonlib.cmake)
include(${JX_ANDROID_DIR}/lists/rainbow.cmake)
include(${JX_ANDROID_DIR}/lists/engine.cmake)
include(${JX_ANDROID_DIR}/lists/represent3.cmake)
include(${JX_ANDROID_DIR}/lists/core.cmake)
include(${JX_ANDROID_DIR}/lists/s3client.cmake)

# ---- JxPosix: lop tuong thich Win32 (thu muc du lieu, cua so chinh, moc phim) ----
add_library(JxPosix STATIC ${JX_SRC}/Engine/Src/Platform/KPosixWin32.cpp
    ${JX_SRC}/Engine/Src/Platform/KPosixCrypto.cpp)
jx_setup(JxPosix)

# ---- Lua 5.4 (+ lua4compat) ----
add_library(Lua54 STATIC ${JX_LUA54_SOURCES}
    ${JX_SRC}/Library/Lua54/jx_lua_fopen.c)   # moi fopen trong Lua -> JxPathPosix
target_include_directories(Lua54 PUBLIC ${JX_LUA54_INCS})
target_compile_definitions(Lua54 PRIVATE ${JX_LUA54_DEFS} LUA_USE_MACOSX JX_APPLE JX_MACOS fopen=jx_lua_fopen)   # LUA_USE_IOS: loslib.c tu tat system(); JX_IOS: rao trong jx_lua_fopen.c
target_compile_options(Lua54 PRIVATE -fsigned-char -fms-extensions -w)
target_link_libraries(Lua54 PRIVATE JxPosix)

# ---- FilterText ----
add_library(FilterText STATIC ${JX_FILTERTEXT_SOURCES})
jx_setup(FilterText)
target_compile_definitions(FilterText PRIVATE ${JX_FILTERTEXT_DEFS})

# ---- JpgLib: tren POSIX chi con ban stb_image ----
add_library(JpgLib STATIC ${JX_SRC}/JpgLib/Src/KJpegLib_stb.cpp)
jx_setup(JpgLib)
target_include_directories(JpgLib PRIVATE ${JX_SRC}/JpgLib/Src)

# ---- Common + Rainbow (mang) ----
add_library(Common STATIC ${JX_COMMONLIB_SOURCES})
jx_setup(Common)
target_compile_definitions(Common PRIVATE ${JX_COMMONLIB_DEFS})
target_include_directories(Common PRIVATE ${JX_COMMONLIB_INCS})

add_library(Rainbow STATIC ${JX_RAINBOW_SOURCES})
jx_setup(Rainbow)
target_compile_definitions(Rainbow PRIVATE ${JX_RAINBOW_DEFS})
target_include_directories(Rainbow PRIVATE ${JX_RAINBOW_INCS})
target_link_libraries(Rainbow PRIVATE Common)

# ---- Engine ----
add_library(Engine STATIC ${JX_ENGINE_SOURCES}
    ${JX_SRC}/Engine/Src/Platform/KDDrawStub.cpp)
jx_setup(Engine)
target_compile_definitions(Engine PRIVATE ${JX_ENGINE_DEFS})
target_include_directories(Engine PRIVATE ${JX_ENGINE_INCS})
target_link_libraries(Engine PRIVATE JpgLib Lua54)

# ---- Represent3 (D3D9 tren SDL_GPU) ----
add_library(Represent3 STATIC ${JX_REPRESENT3_SOURCES}
    ${JX_SRC}/Represent/Represent3/RepresentUtilityStub.cpp    # thay iRepresent/RepresentUtility.cpp (GDI+)
    ${JX_SRC}/Represent/Represent3/D3D9on11Stub.cpp)           # thay D3D9on11*.cpp
jx_setup(Represent3)
target_compile_definitions(Represent3 PRIVATE ${JX_REPRESENT3_DEFS})
target_include_directories(Represent3 PRIVATE ${JX_REPRESENT3_INCS} ${JX_SRC}/Represent/Represent3)
target_link_libraries(Represent3 PRIVATE Engine)

# ---- CoreClient ----
add_library(CoreClient STATIC ${JX_CORE_SOURCES})
jx_setup(CoreClient)
target_compile_definitions(CoreClient PRIVATE ${JX_CORE_DEFS})
target_include_directories(CoreClient PRIVATE ${JX_CORE_INCS}
    ${JX_ROOT}/ThirdParty/nlohmann)   # nlohmann/json.hpp 3.11.3 (chi header) - ban Windows lay tu vcpkg_installed, may Mac khong co cay do
target_link_libraries(CoreClient PRIVATE Engine Lua54)

# ---- S3Client: buoc B dung dang THU VIEN TINH de khao sat loi bien dich ----
# Diem vao macOS o JxMacMain.cpp. Khong lay JxAndroidMain.cpp (JNI) va JxPerfHudAndroid.cpp (doc /proc).
add_library(S3Client STATIC ${JX_S3CLIENT_SOURCES}
    ${JX_SRC}/S3Client/Platform/JxAndroidStubs.cpp    # stub JxReplay / PerfHud / TrayMode, rao JX_POSIX
    # [JXMOBILE 11/09] cac tep ma ban Android khai rieng; nay rao JX_MOBILE nen iOS dung chung.
    # KHONG lay JxPerfHudAndroid.cpp: no doc /proc, chi Android moi co.
    ${JX_SRC}/S3Client/Platform/JxWAutoNoiBo.cpp      # ben gui WAuto trong tien trinh
    ${JX_SRC}/S3Client/Ui/UiCase/UiWAuto.cpp          # khung Auto trong game
    ${JX_SRC}/S3Client/Ui/UiCase/UiWAutoTrang.cpp     # trang noi dung cua khung Auto
    ${JX_SRC}/S3Client/Ui/UiCase/UiVatPham.cpp       # bang thong tin vat pham
    ${JX_IOS_DIR}/JxIosStubs.cpp                    # dem khung + ghi jx_nhip.log
    ${JX_IOS_DIR}/JxIosDoNhip.mm                    # do CPU/RAM/nhiet/pin bang API Apple
    ${JX_IOS_DIR}/JxIosHud.cpp)                     # bang do hien trong game
jx_setup(S3Client)
target_compile_definitions(S3Client PRIVATE ${JX_S3CLIENT_DEFS})
target_include_directories(S3Client PRIVATE ${JX_S3CLIENT_INCS})
target_link_libraries(S3Client PRIVATE CoreClient Engine FilterText Lua54 Represent3 Rainbow)

# ---- ung dung macOS ----
add_executable(jx1mac MACOSX_BUNDLE ${CMAKE_CURRENT_SOURCE_DIR}/JxMacMain.cpp)
jx_setup(jx1mac)
set_target_properties(jx1mac PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    OUTPUT_NAME jx1mac)
target_include_directories(jx1mac PRIVATE ${JX_SRC}/Engine/Src ${JX_SRC}/Engine/Src/Platform)
target_link_libraries(jx1mac PRIVATE S3Client CoreClient Represent3 Rainbow Common Engine FilterText Lua54 JpgLib)
target_link_options(jx1mac PRIVATE -Wl,-all_load)
