# [IOS 11/09] Client JX1 tren iOS (SDL3 + SDL_GPU/Metal + miniaudio + BSD socket).
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
#
# [IOS-PHATHANH 15/09] Ban phat hanh (Archive de tai len App Store Connect): chay ios/dung_ban_phat_hanh.sh.
#   KHONG chay "xcodebuild archive" trong build/ios-dev: luc Archive Xcode doi san pham sang DerivedData va de lai
#   LIEN KET TREO o Release-iphoneos, sau do ban thu khong dung lai duoc ("unable to create directory").
#   Tu 15/09 ban Release mang them -g de sinh dSYM, nen tep chay trong cay thu to hon truoc (khoang 6,3 MB);
#   ban nam trong Archive da duoc strip nen nho hon (khoang 4,9 MB). KHONG ghim so byte chinh xac vao day:
#   moi lan dung lai no lech vai tram byte, ghi so cu the la chac chan sai.
cmake_minimum_required(VERSION 3.22)
project(JX1Ios C CXX OBJC OBJCXX)

# [IOS-PHATHANH 15/09] Release phai sinh dSYM, khong thi khong doc duoc bao cao sap tu TestFlight/App Store.
# CMake Release chi co -O3 -DNDEBUG (khong -g) => Xcode generator ghi GCC_GENERATE_DEBUGGING_SYMBOLS=NO cho MOI target
# (ca SDL3-static va cac thu vien tinh). -g khong doi ma may; khi Archive Xcode tu strip ban cai.
# Dat TRUOC add_subdirectory(SDL3) de SDL cung nhan.
foreach(jx_lang C CXX OBJC OBJCXX)
  string(APPEND CMAKE_${jx_lang}_FLAGS_RELEASE " -g")
endforeach()

# [IOS-PHATHANH 15/09] So ban dung (CFBundleVersion). App Store Connect TU CHOI ban tai len trung so voi ban truoc,
# nen moi lan tai len phai tang: cmake -S ios -B <cay> -DJX_IOS_SO_BAN_DUNG=<so>.
# ios/Info.plist ghi $(CURRENT_PROJECT_VERSION); Xcode thay bang gia tri dat o duoi (jx1ios).
set(JX_IOS_SO_BAN_DUNG "1" CACHE STRING "CFBundleVersion cua ban iOS - moi lan tai len App Store Connect phai tang")

set(JX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
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
  set(SDL_VULKAN OFF CACHE BOOL "" FORCE)   # iOS di Metal; tat Vulkan de khoi keo MoltenVK
  # [IOS-PHATHANH 15/09] Tat 3 phan he SDL ma game KHONG dung, vi chung keo ma dung toi quyen rieng tu vao tep chay:
  #   SDL_CAMERA (AVCaptureDevice) va SDL_HIDAPI (CBCentralManager, tay cam Bluetooth) => App Store Connect doi cau giai
  #   thich quyen camera/Bluetooth khi tai ban len (ITMS-90683). SDL_AUDIO: am thanh cua game di miniaudio (KSoundMa.cpp).
  #   Game chi goi SDL_Init(VIDEO|EVENTS) (trong KSdlApp::Init; khong ghi so dong o day vi ban va khac lam lech).
  #   Tat them SDL_SENSOR: no bo src/sensor/coremotion
  #   (CMMotionManager) khoi ban dung - kiem bang nm -u tren tep chay: 0 ky hieu CMMotion.
  #   NHUNG -framework CoreMotion VAN con trong OTHER_LDFLAGS, vi SDL dat SDL_FRAMEWORK_COREMOTION trong khoi
  #   if(SDL_JOYSTICK) (SDL3 CMakeLists.txt:2244), khong phai khoi SENSOR. Muon bo han thi phai tat JOYSTICK
  #   (khong duoc, xem duoi) hoac va SDL. Lien ket suong, khong dung toi quyen rieng tu, nen de nguyen.
  #   KHONG tat SDL_JOYSTICK: SDL_uikitevents.m dung GCKeyboard/GCMouse cho ban phim + chuot, ma SDL chi lien ket
  #   GameController.framework trong khoi if(SDL_JOYSTICK) => tat la hong buoc link.
  #   KHONG tat SDL_HAPTIC: tren iOS no chi glob src/haptic/dummy (vo hai), nhung CMake cua SDL bao FATAL_ERROR neu
  #   HAPTIC bat ma JOYSTICK tat - giu ca hai cho khoi vuong.
  #   KHONG tat SDL_POWER: ios/JxIosDoNhip.mm:99 goi SDL_GetPowerInfo.
  set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
  set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)
  set(SDL_AUDIO  OFF CACHE BOOL "" FORCE)
  set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
  # [MOBILE 13/09] D1 cho Metal: drawable = khung logic theo hint JX_SWAPCHAIN_W/H (ios/va_sdl3_d1_metal.py, idempotent) - nhu android/va_sdl3_d1.py ben Vulkan
  find_package(Python3 COMPONENTS Interpreter)
  if(Python3_Interpreter_FOUND)
    execute_process(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/va_sdl3_d1_metal.py ${JX_SDL3_SRC}
                    RESULT_VARIABLE JX_VA_D1M_KQ OUTPUT_VARIABLE JX_VA_D1M_RA ERROR_VARIABLE JX_VA_D1M_LOI OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT JX_VA_D1M_KQ EQUAL 0)
      message(FATAL_ERROR "va_sdl3_d1_metal.py loi (${JX_VA_D1M_KQ}): ${JX_VA_D1M_RA} ${JX_VA_D1M_LOI}")
    endif()
    message(STATUS "va_sdl3_d1_metal.py: ${JX_VA_D1M_RA}")
  else()
    message(WARNING "Khong co python3 -> bo qua va_sdl3_d1_metal.py (D1 Metal tat: GPU to full res nhu truoc)")
  endif()
  add_subdirectory(${JX_SDL3_SRC} SDL3 EXCLUDE_FROM_ALL)
endif()

# ---- co chung cho moi TU cua game (giu bang voi android/CMakeLists.txt) ----
# -fsigned-char : char CO DAU nhu MSVC (byte TCVN3 > 0x7F, moi phep 'if (c < 0)')
# -U__linux     : tat cac nhanh port may chu Linux cu
# -D_WIN64      : di dung nhanh x64 da kiem chung (hop ngu -> C, con tro 8 byte); KHONG phai la Windows
set(JX_COMMON_DEFS JX_PLATFORM_SDL JX_POSIX JX_MOBILE JX_APPLE JX_IOS JX_NO_DIRECTX JX_D3D9MINI _WIN64 NDEBUG _MBCS USE_STANDALONE_SPR
    _LIBCPP_ENABLE_CXX17_REMOVED_FEATURES _LIBCPP_ENABLE_CXX20_REMOVED_FEATURES
    _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)   # libc++ cua Xcode 26 doi macro rieng cho std::auto_ptr (MultiServer/Common/Utils.cpp)
set(JX_COMMON_OPTS -fsigned-char -fms-extensions -U__linux -w -Wno-register -Wno-address-of-temporary -fno-strict-aliasing -Wno-error)
set(JX_COMMON_INCS
    ${CMAKE_CURRENT_SOURCE_DIR}/shim          # header CHI cho iOS (malloc.h ...) - Android khong thay
    ${JX_SRC}/Engine/Src/Platform/winshim     # windows.h, winsock2.h, mmsystem.h ... -> KPosixCompat.h
    ${JX_SRC}/Engine/Src/Platform
    ${JX_SRC}/Engine/Src/Platform/posixssl    # openssl/*.h -> KPosixCrypto.cpp
    ${JX_ROOT}/ThirdParty/miniaudio
    ${JX_ROOT}/ThirdParty/stb)

function(jx_setup tgt)
  # [IOS-PHATHANH 15/09] Thu vien tinh phai GIU thong tin go loi khi dung ban phat hanh.
  # Luc Archive, Xcode chuyen sang che do "install" va strip ca thu vien tinh (thay ro trong archive.log truoc khi
  # sua: co dong "Strip" cho tung tep .a); thu vien tinh bi strip
  # thi trinh lien ket khong con ban do go loi -> dSYM mat phan ma nam trong thu vien do, bao cao sap tu TestFlight
  # chi con ten ham, khong co so dong. Do ngay 15/09: truoc khi sua, dSYM trong Archive chi co 2 tep nguon
  # (dung hai tep nguon cua rieng target jx1ios).
  # Rieng jx1ios VAN strip: dsymutil chay TRUOC strip (thay ro trong nhat ky Archive) nen dSYM van day du.
  # SKIP_INSTALL=YES de Archive chi chua app, khong lan thu vien (khai ro, khong dua vao mac dinh cua Xcode).
  # LUU Y: ham nay KHONG chay cho Lua54, SDL3-static, SDL_uclibc - ba target do dat rieng o duoi.
  if(NOT "${tgt}" STREQUAL "jx1ios")
    set_target_properties(${tgt} PROPERTIES
        XCODE_ATTRIBUTE_STRIP_INSTALLED_PRODUCT NO
        XCODE_ATTRIBUTE_SKIP_INSTALL YES)
  endif()
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
target_compile_definitions(Lua54 PRIVATE ${JX_LUA54_DEFS} LUA_USE_IOS JX_APPLE JX_IOS fopen=jx_lua_fopen)   # LUA_USE_IOS: loslib.c tu tat system(); JX_IOS: rao trong jx_lua_fopen.c
target_compile_options(Lua54 PRIVATE -fsigned-char -fms-extensions -w)
target_link_libraries(Lua54 PRIVATE JxPosix)

# [IOS-PHATHANH 15/09] Ba target nay KHONG di qua jx_setup (Lua54 khai tay, hai target SDL den tu add_subdirectory),
# nen phai dat rieng, neu khong Archive van strip chung: khi con thieu buoc nay, archive.log co dong "Strip" cho
# libSDL3.a, libLua54.a, libSDL_uclibc.a, va dSYM mat gan het nguon cua SDL3 lan Lua54 -> khung SDL/Lua trong bao
# cao sap chi con ten ham, khong co so dong. Sau khi sua, do duoc: dSYM co 106 tep nguon SDL3 (.c/.m) tren tong so
# 186 tep SDL3 duoc bien dich - so con lai khong duoc lien ket vao nen khong co trong dSYM; va 34 tep Lua54 (.c),
# dung bang so tep Lua54 duoc bien dich.
# (Luu y khi doc dwarfdump --show-sources: no liet ke CA header, nen con so "331 dong SDL3" khong phai so tep nguon.)
foreach(jx_tgt Lua54 SDL3-static SDL_uclibc)
  if(TARGET ${jx_tgt})
    set_target_properties(${jx_tgt} PROPERTIES
        XCODE_ATTRIBUTE_STRIP_INSTALLED_PRODUCT NO
        XCODE_ATTRIBUTE_SKIP_INSTALL YES)
  endif()
endforeach()

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
# Chua them diem vao iOS (JxIosMain) va chua them cac tep chi-Android
# (JxAndroidMain.cpp, JxPerfHudAndroid.cpp, JxWAutoNoiBo.cpp, UiWAuto*.cpp, UiVatPham.cpp) - viec cua buoc C.
add_library(S3Client STATIC ${JX_S3CLIENT_SOURCES}
    ${JX_SRC}/S3Client/Platform/JxAndroidStubs.cpp    # stub JxReplay / PerfHud / TrayMode, rao JX_POSIX
    # [JXMOBILE 11/09] cac tep ma ban Android khai rieng; nay rao JX_MOBILE nen iOS dung chung.
    # KHONG lay JxPerfHudAndroid.cpp: no doc /proc, chi Android moi co.
    ${JX_SRC}/S3Client/Platform/JxWAutoNoiBo.cpp      # ben gui WAuto trong tien trinh
    ${JX_SRC}/S3Client/Ui/UiCase/UiWAuto.cpp          # khung Auto trong game
    ${JX_SRC}/S3Client/Ui/UiCase/UiWAutoTrang.cpp     # trang noi dung cua khung Auto
    ${JX_SRC}/S3Client/Ui/UiCase/UiWAutoDsach.cpp     # [WAUTO 13/09] bang phu: sua 6 danh sach + chon muc (Android da khai tu 28a100a0; thieu o day thi link hong)
    ${JX_SRC}/S3Client/Ui/UiCase/UiZoomThanh.cpp     # [ZOOMTHANH 14/09] thanh keo zoom o mep man choi
    ${JX_SRC}/S3Client/Ui/UiCase/UiVatPham.cpp       # bang thong tin vat pham khi cham o do
    ${CMAKE_CURRENT_SOURCE_DIR}/JxIosStubs.cpp      # [IOS-DO] dem khung + ghi jx_nhip.log moi 10 s
    ${CMAKE_CURRENT_SOURCE_DIR}/JxTaiDuLieu.mm                 # [IOS-TAI 11/09] buoc F: bo tai du lieu trong app (manifest chung voi Android)
    ${CMAKE_CURRENT_SOURCE_DIR}/JxIosMiniAudio.mm               # [IOS-AM 11/09] cai dat miniaudio (tach vi AVFoundation da BOOL)
    ${CMAKE_CURRENT_SOURCE_DIR}/JxIosAnGame.mm                  # [IOS-AN 11/09] nut an game: day ung dung xuong nen
    ${CMAKE_CURRENT_SOURCE_DIR}/JxIosKhoaMat.mm                 # [BAOMAT 12/09 KHOMAT] Keychain: tai khoan / mat khau
    ${CMAKE_CURRENT_SOURCE_DIR}/JxIosNhatKy.mm                  # [IOS-LOG 13/09] gui nhat ky ve may chu tren PC, chung thu muc voi Android
    ${CMAKE_CURRENT_SOURCE_DIR}/JxIosDoNhip.mm      # do CPU/RAM/nhiet/pin bang API Apple
    ${CMAKE_CURRENT_SOURCE_DIR}/JxIosHud.cpp)       # [IOS-HUD] bang do hien trong game
jx_setup(S3Client)
target_compile_definitions(S3Client PRIVATE ${JX_S3CLIENT_DEFS})
target_include_directories(S3Client PRIVATE ${JX_S3CLIENT_INCS})
target_link_libraries(S3Client PRIVATE CoreClient Engine FilterText Lua54 Represent3 Rainbow)
# [IOS-PHATHANH 15/09] SDL_AUDIO=OFF thi SDL khong con keo AVFoundation/AudioToolbox, ma miniaudio (JxIosMiniAudio.mm) can ca hai.
# S3Client la thu vien TINH nen PRIVATE cung tu truyen sang ban dung cuoi; de PUBLIC cho ro y, khoi phai tra lai sau.
# Da kiem: OTHER_LDFLAGS cua ca jx1ios lan JxIosProbeLink deu co hai framework nay o Debug va Release.
target_link_libraries(S3Client PUBLIC "-framework AVFoundation" "-framework AudioToolbox")

# ---- phep thu link (buoc B): ep nap moi tep .o de lo ky hieu trung ten va ky hieu thieu ----
# Khong phai ung dung that; chi de moi lan dung deu tu kiem duoc phan link tinh.
add_executable(JxIosProbeLink ${CMAKE_CURRENT_SOURCE_DIR}/JxIosProbeLink.cpp)
jx_setup(JxIosProbeLink)
target_link_libraries(JxIosProbeLink PRIVATE S3Client CoreClient Represent3 Rainbow Common Engine FilterText Lua54 JpgLib)
target_link_options(JxIosProbeLink PRIVATE -Wl,-all_load)
target_link_libraries(JxIosProbeLink PRIVATE "-framework Security")	# [BAOMAT 12/09] Keychain + kiem chu ky
set_target_properties(JxIosProbeLink PROPERTIES MACOSX_BUNDLE OFF)

# ---- ung dung that (buoc C): goi .app chay duoc tren may ao / may that ----
# -all_load: Represent3 va Rainbow tren Windows/Android duoc nap luc chay qua LoadLibrary/dlopen nen
# khong co tham chieu tinh nao toi chung; neu khong ep nap thi trinh lien ket se bo het.
add_executable(jx1ios MACOSX_BUNDLE ${CMAKE_CURRENT_SOURCE_DIR}/JxIosMain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JxIosDuongDan.mm)   # .mm KHONG duoc include KWin32.h - xem ghi chu trong tep
jx_setup(jx1ios)
target_include_directories(jx1ios PRIVATE ${JX_SRC}/Engine/Src ${JX_SRC}/Engine/Src/Platform)
target_link_libraries(jx1ios PRIVATE S3Client CoreClient Represent3 Rainbow Common Engine FilterText Lua54 JpgLib)
target_link_options(jx1ios PRIVATE -Wl,-all_load)
# [BAOMAT 12/09 KY] Security.framework: SecKeyVerifySignature de kiem chu ky ban ke tep tai ve
target_link_libraries(jx1ios PRIVATE "-framework Security")
set_target_properties(jx1ios PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER vn.jx1.mobile
    OUTPUT_NAME jx1ios)
# [IOS-ICON 14/09] Icon ung dung (chu: "toi co icon trong download ban lay lam icon game"): MOT anh 1024x1024 trong
# ios/Assets.xcassets/AppIcon.appiconset. actool cua Xcode tu sinh cac co va ghi CFBundleIcons vao Info.plist cua goi.
set(JX_IOS_ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/Assets.xcassets)
target_sources(jx1ios PRIVATE ${JX_IOS_ASSETS})
set_source_files_properties(${JX_IOS_ASSETS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set_target_properties(jx1ios PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)
# [IOS-PHATHANH 15/09] Ban ke quyen rieng tu: tu 01/05/2024 App Store Connect tu choi ban tai len thieu tep nay (ITMS-91053).
# Tep chay goi stat/fstat (NSPrivacyAccessedAPICategoryFileTimestamp, ly do C617.1: tep trong thu muc cua app) va
# mach_absolute_time qua SDL/miniaudio (NSPrivacyAccessedAPICategorySystemBootTime, ly do 35F9.1: do thoi gian troi).
# Doi engine/SDL thi kiem lai bang nm -u tren tep chay (vd dung statvfs thi phai khai them DiskSpace).
set(JX_IOS_PRIVACY ${CMAKE_CURRENT_SOURCE_DIR}/PrivacyInfo.xcprivacy)
target_sources(jx1ios PRIVATE ${JX_IOS_PRIVACY})
set_source_files_properties(${JX_IOS_PRIVACY} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
# [IOS-MANTAI 15/09] Anh nen cho man TAI DU LIEU. Phai nam TRONG GOI: man nay chay khi may VUA CAI,
# luc do chua co mot byte du lieu game nao nen khong lay anh tu cay du lieu duoc (VNKU cung nhet san
# LaunchScreenBackground.png trong goi cua ho). Chua co tep thi bo qua, ma tu ve nen chuyen sac toi
# (JxTaiDuLieu.mm, ham datNen) - khong can sua ma khi them anh sau.
foreach(jx_nen nen_tai.jpg nen_tai.png)
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${jx_nen})
    target_sources(jx1ios PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${jx_nen})
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/${jx_nen}
                                PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    message(STATUS "[IOS-MANTAI] dong goi anh nen man tai: ${jx_nen}")
  endif()
endforeach()
# [IOS-PHATHANH 15/09] Archive phai chua app: INSTALL_PATH trong => Xcode tu dat SKIP_INSTALL=YES cho jx1ios => xcodebuild
# archive ra goi khong co app, khong tai len duoc. Cac thu vien tinh van tu bo qua vi INSTALL_PATH cua chung trong.
set_target_properties(jx1ios PROPERTIES
    XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"
    XCODE_ATTRIBUTE_SKIP_INSTALL NO
    XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION "${JX_IOS_SO_BAN_DUNG}")
