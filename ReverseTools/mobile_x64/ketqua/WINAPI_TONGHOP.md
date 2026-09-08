# Khao sat tinh phu thuoc Windows/MSVC trong ma client (dem lan xuat hien, khong ke chu thich)

## Theo phan he

| phan he | tep | tep co byte >127 (TCVN3/GBK) | cua so / thong diep Win32 | tep / thu muc Win32 | luong / dong bo | thoi gian | mang Winsock | DirectX cu (DDraw/DInput/DSound/D3D9/D3DX) | hop thoai / IME / clipboard | registry / ini / he thong | __asm / MSVC-ism | CRT chi co o MSVC |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Engine | 291 | 168 | 90 | 47 | 51 | 26 | 25 | 38 | 19 | 15 | 64 | 38 |
| Core | 300 | 183 | 20 | 24 | 92 | 268 | 8 | 0 | 1 | 36 | 305 | 149 |
| S3Client | 408 | 301 | 246 | 44 | 60 | 69 | 0 | 0 | 42 | 66 | 49 | 265 |
| iRepresent | 24 | 23 | 0 | 0 | 0 | 0 | 0 | 15 | 0 | 0 | 10 | 0 |
| Represent3 | 23 | 11 | 23 | 5 | 16 | 24 | 0 | 264 | 0 | 5 | 233 | 3 |
| JpgLib | 10 | 9 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 35 | 0 |
| FilterText | 6 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 2 | 0 |

## Header Windows duoc include (so tep)

| header | tep |
|---|---|
| crtdbg.h | 85 |
| windows.h | 38 |
| direct.h | 4 |
| winsock2.h | 4 |
| shlwapi.h | 4 |
| winsock.h | 3 |
| process.h | 3 |
| io.h | 3 |
| tlhelp32.h | 3 |
| d3d9.h | 3 |
| commdlg.h | 2 |
| dbghelp.h | 2 |
| psapi.h | 2 |
| tchar.h | 2 |
| d3dx9.h | 2 |
| imm.h | 1 |
| conio.h | 1 |
| mmsystem.h | 1 |
| dsound.h | 1 |
| dinput.h | 1 |
| ddraw.h | 1 |
| winbase.h | 1 |
| objbase.h | 1 |
| wincodec.h | 1 |

## 60 tep phu thuoc nhieu nhat (tong lan)

| tep | phan he | tong | chi tiet |
|---|---|---|---|
| Represent/Represent3/D3D9on11i.h | Represent3 | 262 | __asm…=195; DirectX…=59; cua…=5; luong…=3 |
| Represent/Represent3/KRepresentShell3.cpp | Represent3 | 154 | DirectX…=135; cua…=7; __asm…=6; thoi…=2; registry…=2; tep…=1; CRT…=1 |
| S3Client/Ui/UiCase/UiMeridian.cpp | S3Client | 120 | CRT…=113; thoi…=5; __asm…=2 |
| Core/Src/KPlayerBot.cpp | Core | 109 | __asm…=78; thoi…=17; CRT…=11; registry…=3 |
| Core/Src/Scene/KScenePlaceC.cpp | Core | 79 | luong…=65; tep…=5; thoi…=4; registry…=2; CRT…=2; __asm…=1 |
| S3Client/S3Client.cpp | S3Client | 76 | thoi…=36; registry…=13; tep…=12; cua…=7; luong…=5; __asm…=2; hop…=1 |
| S3Client/CrashLog.cpp | S3Client | 64 | CRT…=19; __asm…=18; registry…=15; tep…=7; thoi…=3; luong…=2 |
| Core/Src/CoreShell.cpp | Core | 59 | thoi…=48; CRT…=10; __asm…=1 |
| Core/Src/KTongJX2.cpp | Core | 56 | __asm…=52; CRT…=4 |
| Core/Src/KPlayer.cpp | Core | 50 | thoi…=36; cua…=8; __asm…=6 |
| Core/Src/KSubWorld.cpp | Core | 49 | thoi…=42; luong…=4; tep…=2; __asm…=1 |
| Core/Src/KChienLenh.cpp | Core | 45 | CRT…=34; __asm…=11 |
| Represent/Represent3/D3D9on11Dev.cpp | Represent3 | 45 | DirectX…=27; thoi…=7; cua…=5; luong…=3; tep…=2; __asm…=1 |
| Engine/Src/KWin32App.cpp | Engine | 43 | cua…=35; __asm…=3; thoi…=2; CRT…=2; registry…=1 |
| S3Client/AntiHack/CheckSumCRC/CheckSumCRC.cpp | S3Client | 43 | luong…=22; hop…=20; tep…=1 |
| Core/Src/KNpc.cpp | Core | 33 | thoi…=14; __asm…=14; registry…=4; CRT…=1 |
| Represent/Represent3/TextureResMgr.cpp | Represent3 | 30 | __asm…=15; thoi…=11; tep…=2; registry…=1; CRT…=1 |
| Core/Src/KMySQLDB.cpp | Core | 29 | luong…=17; __asm…=7; tep…=2; registry…=2; thoi…=1 |
| Core/Src/KAuctionServer.cpp | Core | 28 | __asm…=19; CRT…=9 |
| S3Client/Ui/PerfHud.cpp | S3Client | 27 | CRT…=12; registry…=8; __asm…=5; thoi…=2 |
| Engine/Src/Kime.cpp | Engine | 26 | hop…=16; cua…=10 |
| Core/Src/KProtocolProcess.cpp | Core | 26 | thoi…=17; __asm…=8; registry…=1 |
| Core/Src/KSimCity.cpp | Core | 26 | __asm…=24; CRT…=2 |
| S3Client/Ui/UiCase/UiPlayerBar.cpp | S3Client | 26 | CRT…=23; hop…=2; cua…=1 |
| S3Client/JxReplay.cpp | S3Client | 24 | registry…=6; __asm…=6; tep…=4; CRT…=4; thoi…=2; hop…=2 |
| Engine/Src/XPackFile.cpp | Engine | 21 | luong…=13; tep…=8 |
| Core/Src/KPlayerAuto.cpp | Core | 20 | thoi…=18; CRT…=2 |
| S3Client/AntiHack/Splash/Splash.cpp | S3Client | 20 | cua…=16; __asm…=3; registry…=1 |
| Core/Src/ScriptFuns.cpp | Core | 19 | thoi…=14; CRT…=4; registry…=1 |
| S3Client/Ui/Elem/UiToaDo.cpp | S3Client | 18 | cua…=9; CRT…=6; thoi…=2; registry…=1 |
| S3Client/Ui/Elem/Wnds.cpp | S3Client | 18 | cua…=18 |
| Core/Src/KCauHinhWeb.cpp | Core | 17 | CRT…=12; __asm…=5 |
| Core/Src/KPerfTick.cpp | Core | 17 | registry…=8; thoi…=4; CRT…=3; __asm…=2 |
| Core/Src/KRegion.cpp | Core | 17 | registry…=6; thoi…=4; __asm…=4; CRT…=3 |
| Core/Src/KSortScript.cpp | Core | 17 | CRT…=11; registry…=4; thoi…=2 |
| Represent/Represent3/D3D9on11.cpp | Represent3 | 17 | luong…=8; DirectX…=5; __asm…=3; CRT…=1 |
| S3Client/Ui/UiBase.cpp | S3Client | 16 | cua…=6; CRT…=6; tep…=4 |
| S3Client/Ui/UiCase/UiPlayVideo.cpp | S3Client | 15 | registry…=13; cua…=2 |
| Represent/Represent3/KRepresentShell3.h | Represent3 | 15 | DirectX…=15 |
| Engine/Src/KNetClient.cpp | Engine | 13 | mang…=10; thoi…=2; cua…=1 |
| Engine/Src/KLubCmpl_Blocker.cpp | Engine | 12 | CRT…=12 |
| Engine/Src/KScanDir.cpp | Engine | 11 | tep…=8; CRT…=3 |
| Engine/Src/ZSPRPackFile.cpp | Engine | 11 | luong…=9; tep…=2 |
| Core/Src/CoreServerShell.cpp | Core | 11 | thoi…=8; __asm…=2; CRT…=1 |
| S3Client/Ui/UiShell.cpp | S3Client | 11 | thoi…=5; tep…=4; cua…=2 |
| S3Client/Ui/UiCase/UiGame.cpp | S3Client | 11 | cua…=11 |
| S3Client/Ui/UiCase/UiSuperShop.cpp | S3Client | 11 | CRT…=6; cua…=3; hop…=2 |
| JpgLib/Src/KJpegIdct.c | JpgLib | 11 | __asm…=11 |
| Engine/Src/KImageStore.cpp | Engine | 10 | thoi…=9; registry…=1 |
| Core/Src/KJx2League.cpp | Core | 10 | thoi…=7; tep…=2; __asm…=1 |
| S3Client/AntiHack/DetectWindowsText/DetectWindowsText.cpp | S3Client | 10 | luong…=6; hop…=2; cua…=1; registry…=1 |
| S3Client/Ui/UiCase/UiMiniMap.cpp | S3Client | 10 | cua…=8; CRT…=2 |
| S3Client/Ui/UiCase/UiTaskGuide.cpp | S3Client | 10 | CRT…=9; thoi…=1 |
| JpgLib/Src/KJpegColor.c | JpgLib | 10 | __asm…=10 |
| Engine/Src/KDrawSpriteAlpha.cpp | Engine | 9 | __asm…=9 |
| Engine/Src/KFileCopy.cpp | Engine | 9 | tep…=9 |
| Engine/Src/KNetServer.cpp | Engine | 9 | mang…=7; cua…=2 |
| Engine/Src/KWin32Wnd.cpp | Engine | 9 | cua…=9 |
| Core/Src/KCore.cpp | Core | 9 | thoi…=3; registry…=3; CRT…=2; tep…=1 |
| S3Client/Ui/Elem/WndEdit.cpp | S3Client | 9 | cua…=7; hop…=2 |

## Tep co __asm (phai viet lai C)

- `Engine/Src/KColors.cpp`: 7 khoi __asm, 7 dong #if kiem tra kien truc
- `Engine/Src/KDrawBase.cpp`: 6 khoi __asm, 10 dong #if kiem tra kien truc
- `Engine/Src/KDrawBitmap.cpp`: 1 khoi __asm, 1 dong #if kiem tra kien truc
- `Engine/Src/KDrawBitmap16.cpp`: 4 khoi __asm, 4 dong #if kiem tra kien truc
- `Engine/Src/KDrawDotFont.cpp`: 1 khoi __asm, 0 dong #if kiem tra kien truc
- `Engine/Src/KDrawFade.cpp`: 1 khoi __asm, 1 dong #if kiem tra kien truc
- `Engine/Src/KDrawFont.cpp`: 4 khoi __asm, 4 dong #if kiem tra kien truc
- `Engine/Src/KDrawSprite.cpp`: 4 khoi __asm, 4 dong #if kiem tra kien truc
- `Engine/Src/KDrawSpriteAlpha.cpp`: 8 khoi __asm, 14 dong #if kiem tra kien truc
- `Engine/Src/KImageRes.cpp`: 1 khoi __asm, 0 dong #if kiem tra kien truc
- `Engine/Src/KPalette.cpp`: 4 khoi __asm, 3 dong #if kiem tra kien truc
- `Engine/Src/KSprite.cpp`: 1 khoi __asm, 1 dong #if kiem tra kien truc
- `Represent/iRepresent/Font/KFontRes.cpp`: 1 khoi __asm, 1 dong #if kiem tra kien truc
- `Represent/iRepresent/Image/ImageOperation.cpp`: 3 khoi __asm, 0 dong #if kiem tra kien truc
- `Represent/Represent3/TextureRes.cpp`: 1 khoi __asm, 1 dong #if kiem tra kien truc
- `JpgLib/Src/KJpegColor.c`: 5 khoi __asm, 0 dong #if kiem tra kien truc
- `JpgLib/Src/KJpegDecode.c`: 2 khoi __asm, 0 dong #if kiem tra kien truc
- `JpgLib/Src/KJpegIdct.c`: 1 khoi __asm, 0 dong #if kiem tra kien truc
- `JpgLib/Src/KJpegLib.h`: 6 khoi __asm, 0 dong #if kiem tra kien truc
