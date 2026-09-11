# [ANDROID 08/09] Client JX1 tren Android (SDL3 + SDL_GPU/Vulkan + miniaudio + BSD socket).
# Cung nguon voi GameSDL.exe (cau hinh ReleaseSDL cua cac vcxproj); danh sach tep/dinh nghia sinh tu vcxproj boi gen_lists.py.
# Cau truc .so giong cau truc DLL tren Windows: libLua54.so, libEngine.so, libCoreClient.so, libRepresent3.so, libRainbow.so, libmain.so
# (Rainbow/Represent3 duoc nap luc chay qua LoadLibrary -> dlopen, nhu tren Windows).
# Cach dung: Android Studio / gradle goi qua android/app (externalNativeBuild), hoac tay:
#   cmake -S android -B build/arm64 -G Ninja -DCMAKE_TOOLCHAIN_FILE=<NDK>/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24
cmake_minimum_required(VERSION 3.22)
project(JX1Mobile C CXX)

set(JX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(JX_SRC ${JX_ROOT}/Sources)
# [DONHIP 12/09 b] SDL Android = 3.2.30 (ban release-3.2.x cuoi, 01/01/2026): co bca30aa (khong dung lai swapchain vi VK_SUBOPTIMAL_KHR
# khi man xoay - Fold 7 45 -> 120 fps, BANGIAO_DONHIP_MOBILE_1209.md muc 8) + hai sua Android khi quay lai tu nen. CHI Android: goi VC
# ThirdParty/SDL3 3.2.14 cua Windows giu nguyen. Khong CACHE de cay cu khong giu duong 3.2.14. Tai bang android/tai_sdl3_src.ps1.
set(JX_SDL3_VER 3.2.30)
set(JX_SDL3_SRC ${JX_ROOT}/ThirdParty/SDL3-src/SDL3-${JX_SDL3_VER})
if(NOT EXISTS ${JX_SDL3_SRC}/CMakeLists.txt)
  message(FATAL_ERROR "Chua co nguon SDL3 ${JX_SDL3_VER} o ${JX_SDL3_SRC} - chay: powershell -File android/tai_sdl3_src.ps1")
endif()
# [DONHIP 12/09 c] Java cua SDL trong gradle-project PHAI cung phien ban voi nguon C: SDLActivity.onCreate so nativeGetVersion() voi
# SDL_MAJOR/MINOR/MICRO_VERSION trong SDLActivity.java, lech thi mBrokenLibraries = hop thoai loi + dong app (Fold 7 11/09 00:23:
# Java 3.2.14 + libSDL3.so 3.2.30). Nang SDL = chep lai android-project/app/src/main/java/org/libsdl/app cua nguon moi.
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/gradle-project/app/src/main/java/org/libsdl/app/SDLActivity.java JX_SDLACT_JAVA)
string(REGEX MATCH "SDL_MAJOR_VERSION = ([0-9]+)" JX_VM "${JX_SDLACT_JAVA}")
set(JX_JAVA_MAJ ${CMAKE_MATCH_1})
string(REGEX MATCH "SDL_MINOR_VERSION = ([0-9]+)" JX_VM "${JX_SDLACT_JAVA}")
set(JX_JAVA_MIN ${CMAKE_MATCH_1})
string(REGEX MATCH "SDL_MICRO_VERSION = ([0-9]+)" JX_VM "${JX_SDLACT_JAVA}")
set(JX_JAVA_MIC ${CMAKE_MATCH_1})
if(NOT "${JX_JAVA_MAJ}.${JX_JAVA_MIN}.${JX_JAVA_MIC}" STREQUAL "${JX_SDL3_VER}")
  message(FATAL_ERROR "Java SDL trong gradle-project la ${JX_JAVA_MAJ}.${JX_JAVA_MIN}.${JX_JAVA_MIC}, nguon C la ${JX_SDL3_VER}: chep lai org/libsdl/app tu ${JX_SDL3_SRC}/android-project/app/src/main/java/org/libsdl/app")
endif()
message(STATUS "SDL Java ${JX_JAVA_MAJ}.${JX_JAVA_MIN}.${JX_JAVA_MIC} = nguon C ${JX_SDL3_VER}")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# ---- SDL3 tu nguon (libSDL3.so vao APK) ----
if(NOT TARGET SDL3::SDL3)
  set(SDL_SHARED ON CACHE BOOL "" FORCE)
  set(SDL_STATIC OFF CACHE BOOL "" FORCE)
  set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
  # [DONHIP 12/09 b] va bo dem swapchain (hint JX_DEM_SUBOPTIMAL / JX_DEM_DUNG_LAI_SWAPCHAIN / JX_DUNG_LAI_SUBOPTIMAL) vao nguon SDL
  # truoc khi dich: idempotent, chay o MOI cay -> moi APK cung mot libSDL3.so (10/09: APK -m dung o cay chua va, pha 1 vo hieu).
  find_package(Python3 COMPONENTS Interpreter)
  if(NOT Python3_Interpreter_FOUND)
    message(FATAL_ERROR "Can python de chay android/va_sdl3_donhip.py (va nguon SDL3)")
  endif()
  execute_process(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/va_sdl3_donhip.py ${JX_SDL3_SRC}
                  RESULT_VARIABLE JX_VA_SDL_KQ OUTPUT_VARIABLE JX_VA_SDL_RA ERROR_VARIABLE JX_VA_SDL_LOI OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT JX_VA_SDL_KQ EQUAL 0)
    message(FATAL_ERROR "va_sdl3_donhip.py loi (${JX_VA_SDL_KQ}): ${JX_VA_SDL_RA} ${JX_VA_SDL_LOI}")
  endif()
  message(STATUS "va_sdl3_donhip.py: ${JX_VA_SDL_RA}")
  add_subdirectory(${JX_SDL3_SRC} SDL3 EXCLUDE_FROM_ALL)
endif()

# ---- co chung cho moi TU cua game ----
# -fsigned-char : char CO DAU nhu MSVC (byte TCVN3 > 0x7F, moi phep 'if (c < 0)')
# -U__linux     : tat cac nhanh port may chu Linux cu (#ifndef __linux) -> client di nhanh "Windows" tren lop tuong thich
# -D_WIN64      : di dung nhanh x64 da kiem chung (hop ngu -> C, con tro 8 byte); KHONG phai la Windows
# -fms-extensions: __int64, __forceinline, __super... ; -w: tat canh bao (nguon TCVN3 sinh 'illegal character encoding')
set(JX_COMMON_DEFS JX_PLATFORM_SDL JX_POSIX JX_ANDROID JX_NO_DIRECTX JX_D3D9MINI _WIN64 NDEBUG _MBCS USE_STANDALONE_SPR
    _LIBCPP_ENABLE_CXX17_REMOVED_FEATURES _LIBCPP_ENABLE_CXX20_REMOVED_FEATURES)
set(JX_COMMON_OPTS -fsigned-char -fms-extensions -U__linux -w -Wno-register -Wno-address-of-temporary -fno-strict-aliasing -Wno-error)
set(JX_COMMON_INCS
    ${JX_SRC}/Engine/Src/Platform/winshim     # windows.h, winsock2.h, mmsystem.h ... -> KPosixCompat.h
    ${JX_SRC}/Engine/Src/Platform
    ${JX_SRC}/Engine/Src/Platform/posixssl   # openssl/aes.h sha.h rand.h hmac.h -> KPosixCrypto.cpp (thay OpenSSL vcpkg)
    ${JX_ROOT}/ThirdParty/miniaudio
    ${JX_ROOT}/ThirdParty/stb)

function(jx_setup tgt)
  target_compile_definitions(${tgt} PRIVATE ${JX_COMMON_DEFS})
  target_compile_options(${tgt} PRIVATE ${JX_COMMON_OPTS})
  target_include_directories(${tgt} PRIVATE ${JX_COMMON_INCS})
  target_link_libraries(${tgt} PRIVATE SDL3::SDL3 log android dl m)
  if(NOT "${tgt}" STREQUAL "JxPosix")
    target_link_libraries(${tgt} PRIVATE JxPosix)
  endif()
  # '#pragma comment(lib, "ws2_32")' cua ma Windows -> clang ghi ".deplibs"; lld ELF phai bo qua
  target_link_options(${tgt} PRIVATE -Wl,--no-dependent-libraries -Wl,--error-limit=0)
endfunction()

# ---- libJxPosix.so: lop tuong thich Win32 (KPosixWin32.cpp) - MOT ban duy nhat (thu muc du lieu, cua so chinh, moc phim) cho moi .so ----
add_library(JxPosix SHARED ${JX_SRC}/Engine/Src/Platform/KPosixWin32.cpp
    ${JX_SRC}/Engine/Src/Platform/KPosixCrypto.cpp)
jx_setup(JxPosix)

include(lists/lua54.cmake)
include(lists/filtertext.cmake)
include(lists/commonlib.cmake)
include(lists/rainbow.cmake)
include(lists/engine.cmake)
include(lists/represent3.cmake)
include(lists/core.cmake)
include(lists/s3client.cmake)

# ---- Lua 5.4 (+ lua4compat) : libLua54.so, dung chung cho Engine/Core/main nhu Lua54Dll.dll ----
add_library(Lua54 SHARED ${JX_LUA54_SOURCES}
    ${JX_SRC}/Library/Lua54/jx_lua_fopen.c)   # moi fopen trong Lua -> JxPathPosix (script goi bang duong dan Windows)
set_target_properties(Lua54 PROPERTIES OUTPUT_NAME Lua54Dll)   # libLua54Dll.so: GetModuleHandleA("Lua54Dll.dll") (KSortScript/KPerfTick) -> dlopen("libLua54Dll.so")
target_include_directories(Lua54 PUBLIC ${JX_LUA54_INCS})
target_compile_definitions(Lua54 PRIVATE ${JX_LUA54_DEFS} LUA_USE_LINUX fopen=jx_lua_fopen)
target_compile_options(Lua54 PRIVATE -fsigned-char -fms-extensions -w)
target_link_libraries(Lua54 PRIVATE JxPosix dl m)

# ---- FilterText (tinh, vao main nhu FilterText_StaticLib.lib) ----
add_library(FilterText STATIC ${JX_FILTERTEXT_SOURCES})
jx_setup(FilterText)
target_compile_definitions(FilterText PRIVATE ${JX_FILTERTEXT_DEFS})

# ---- JpgLib: tren POSIX chi con ban stb_image (4 ham API cua KJpegLib.h) ----
add_library(JpgLib STATIC ${JX_SRC}/JpgLib/Src/KJpegLib_stb.cpp)
jx_setup(JpgLib)
target_include_directories(JpgLib PRIVATE ${JX_SRC}/JpgLib/Src)

# ---- Common (tinh) + Rainbow (.so, nap luc chay: LoadLibrary("Rainbow.dll") -> dlopen("libRainbow.so")) ----
add_library(Common STATIC ${JX_COMMONLIB_SOURCES})
jx_setup(Common)
target_compile_definitions(Common PRIVATE ${JX_COMMONLIB_DEFS})
target_include_directories(Common PRIVATE ${JX_COMMONLIB_INCS})

add_library(Rainbow SHARED ${JX_RAINBOW_SOURCES})
jx_setup(Rainbow)
target_compile_definitions(Rainbow PRIVATE ${JX_RAINBOW_DEFS})
target_include_directories(Rainbow PRIVATE ${JX_RAINBOW_INCS})
target_link_libraries(Rainbow PRIVATE Common)

# ---- Engine.so ----
add_library(Engine SHARED ${JX_ENGINE_SOURCES}
    ${JX_SRC}/Engine/Src/Platform/KDDrawStub.cpp)
jx_setup(Engine)
target_compile_definitions(Engine PRIVATE ${JX_ENGINE_DEFS})
target_include_directories(Engine PRIVATE ${JX_ENGINE_INCS})
target_link_libraries(Engine PRIVATE JpgLib Lua54)

# ---- Represent3.so (D3D9 tren SDL_GPU: Rep3Api=100, d3d9mini.h) ----
add_library(Represent3 SHARED ${JX_REPRESENT3_SOURCES}
    ${JX_SRC}/Represent/Represent3/RepresentUtilityStub.cpp    # thay iRepresent/RepresentUtility.cpp (GDI+)
    ${JX_SRC}/Represent/Represent3/D3D9on11Stub.cpp)           # thay D3D9on11*.cpp (bo dem thong ke + API bang mau -> SDL_GPU)
jx_setup(Represent3)
target_compile_definitions(Represent3 PRIVATE ${JX_REPRESENT3_DEFS})
target_include_directories(Represent3 PRIVATE ${JX_REPRESENT3_INCS} ${JX_SRC}/Represent/Represent3)   # winshim/d3d9.h -> d3d9mini.h
target_link_libraries(Represent3 PRIVATE Engine)

# ---- CoreClient.so ----
add_library(CoreClient SHARED ${JX_CORE_SOURCES})
jx_setup(CoreClient)
target_compile_definitions(CoreClient PRIVATE ${JX_CORE_DEFS})
target_include_directories(CoreClient PRIVATE ${JX_CORE_INCS}
    ${JX_SRC}/Core/vcpkg_installed/x64-windows-static/x64-windows-static/include)   # nlohmann/json.hpp (header-only; openssl/* da bi posixssl che truoc)
target_link_libraries(CoreClient PRIVATE Engine Lua54)

# ---- main (S3Client -> libmain.so, SDLActivity nap "SDL3" + "main", SDL_main trong Platform/JxAndroidMain.cpp) ----
add_library(main SHARED ${JX_S3CLIENT_SOURCES}
    ${JX_SRC}/S3Client/Platform/JxAndroidMain.cpp
    ${JX_SRC}/S3Client/Platform/JxAndroidStubs.cpp    # JxReplay / TrayMode (chi Windows)
    ${JX_SRC}/S3Client/Platform/JxPerfHudAndroid.cpp   # [ANDROID 11/09 HUD] bang do FPS / CPU / RAM / pin / GPU
    ${JX_SRC}/S3Client/Platform/JxWAutoNoiBo.cpp       # [ANDROID 11/09 WAUTO B0] ben gui WAuto trong tien trinh (LOTRINH_WAUTO_MOBILE_1109.md)
    ${JX_SRC}/S3Client/Ui/UiCase/UiWAuto.cpp           # [ANDROID 11/09 WAUTO B1] khung Auto trong game (icon + BAT/TAT + dai tab)
    ${JX_SRC}/S3Client/Ui/UiCase/UiWAutoTrang.cpp      # [ANDROID 11/09 WAUTO B2] trang noi dung chay theo bang (UiWAutoBang.h sinh bang may)
    ${JX_SRC}/S3Client/Ui/UiCase/UiVatPham.cpp)        # [VATPHAM 12/09] bang thong tin vat pham + nut (cham vao o vat pham)
jx_setup(main)
target_compile_definitions(main PRIVATE ${JX_S3CLIENT_DEFS})
target_include_directories(main PRIVATE ${JX_S3CLIENT_INCS})
target_link_libraries(main PRIVATE CoreClient Engine FilterText Lua54)
# Rainbow / Represent3 khong link truc tiep (nap qua dlopen) nhung phai duoc build cung de vao APK
add_dependencies(main Rainbow Represent3)
