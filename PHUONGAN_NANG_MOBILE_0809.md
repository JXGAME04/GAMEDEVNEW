# PHƯƠNG ÁN NÂNG LÊN MOBILE — công nghệ mới, không theo cocos2d, lấy TRỌN BỘ tính năng (08/09/2026)

Chủ hỏi 08/09: *"xem qua dự án để xem nếu tôi muốn nâng lên mobile thì nên nâng theo hướng nào để lấy được trọn bộ tính năng — chỉ phân tích không fix code"*, rồi chốt thêm: *"tôi muốn nâng cấp công nghệ mới, không đi theo cocos2d"*.

Tài liệu này CHỈ phân tích. Không sửa mã, không build. Mọi con số đều đo bằng lệnh trên hai cây `D:\GAMEDEVNEW` (client PC hiện tại) và
`E:\Source Jx1m Cocos V3 full\File\Client2912\Client` (client mobile JX1M cũ); chỗ nào là ước lượng thì ghi rõ "ước".

---

## 0. Kết luận ngắn

1. **Hướng đúng = "MỘT MÃ NGUỒN"**: mang chính client PC của `D:\GAMEDEVNEW` (Core phía client + hệ cửa sổ KWnd của S3Client + Engine +
   Lua 5.4 + dữ liệu pak) lên mobile, và **chỉ viết mới bốn lớp nền**: bộ vẽ, nền tảng (cửa sổ/chạm/bàn phím/tệp/luồng), mạng, âm thanh
   (ước 10–15 nghìn dòng mới). Không viết lại logic, không viết lại UI. Đây là cách **duy nhất** để "trọn bộ" và **giữ trọn bộ mãi**:
   client mobile dùng chung `Headers/KProtocol.h` với máy chủ y hệt client PC hôm nay, nên không thể lệch giao thức.
2. **Công nghệ mới đề xuất**: **SDL3** làm lớp nền (Android/iOS/Windows: cửa sổ, chạm, bàn phím, âm thanh, tệp, luồng) + **SDL_GPU**
   (Vulkan trên Android, Metal trên iOS, D3D12/Vulkan trên Windows) cho bộ vẽ mới `Represent4`. Phương án dự phòng: **bgfx** nếu cần chạy cả
   máy Android rất cũ chỉ có OpenGL ES. `Represent4` cài đúng giao diện `iRepresentShell` (40 hàm ảo) đang có, nên **thay được cả
   Represent3 trên PC** — một bộ vẽ cho mọi nền, và giải luôn bài RAM texture D3D9 + trần 2 GB đã ghi ở `PHUONGAN_KIENTRUC_HIEUUNG_RAM_0809.md`.
3. **JX1M (cocos) dừng đầu tư tính năng**, giữ chạy cho máy chủ USVOLAM; chỉ lấy làm **tham chiếu** (bố cục chạm, auto 8 tab, bộ tải CDN,
   mã hoá pak, bài học iOS). Lý do bằng số ở mục 1.2: JX1M khớp 100 % giao thức USVOLAM nhưng với GAMEDEVNEW chỉ khớp **46/143 c2s** và
   **48/191 s2c**; Core hai bên trùng **4–41 %** dòng; **55 tệp Core + ~60 cửa sổ UI** của PC không có ở mobile. Đuổi theo bằng tay là vô tận.
4. **Lộ trình 6 pha** (mục 4), pha nào cũng có mốc đo được và có lợi riêng cho PC nên không có pha "chi phí chìm". Mốc đầu: **Game.exe 64-bit**
   (ước 2–3 tuần) — vừa là bước 1 của mobile vừa gỡ trần 2 GB. Tổng **ước 5–7 tháng** tới bản Android đủ tính năng, **+1 tháng iOS**.
5. **Chủ cần quyết 5 điều** ở mục 7 trước khi bắt tay (chọn SDL_GPU hay bgfx; có thay Represent3 trên PC không; bắt đầu pha 0 ở worktree riêng;
   dữ liệu 9,6 GB; pháp lý App Store).

---

## 1. Hiện trạng đo được

### 1.1 Client PC `D:\GAMEDEVNEW` — thứ phải mang đi

| Khối | Cỡ | Phụ thuộc Windows (đếm tệp có dùng) | Mang đi |
|---|---|---|---|
| `Sources/Core/Src` | 300 tệp, 234.249 dòng; `_SERVER` 1.801 chỗ | `windows.h` 6 (KMySQLDB, ScriptFuns, KPlayerAuto, cLittleMap, JXServer), `GetPrivateProfile` 8, `CreateThread/PeekMessage/SetTimer` 5, `__asm` 0 | **nguyên** — mọi tính năng và toàn bộ giao thức nằm đây |
| `Sources/Engine/Src` | 262 tệp, 57.647 dòng | **202 tệp / 27.033 dòng thuần C++** (pak, ini, tab, lua, list, cache…); 60 tệp / 27.156 dòng dính Win32: DDraw/Canvas/DrawSprite* (vẽ CPU cho Represent2 — chỉ `CoreShell.cpp` còn include), DSound/KMusic/Mp3/Mpg (1.536 dòng), DInput/Kime (645), KFile/KFilePath/XPackFile (`CreateFile` → `fopen`), KNet* (Winsock, client không dùng), `__asm` 8 tệp (KDraw* có nhánh C sẵn; KStrBase/KCanvas không) | phần thuần nguyên; phần Win32 thay hoặc bỏ |
| `Sources/Represent` | 58 tệp, 25.294 dòng | `iRepresentShell.h` **40 hàm ảo** = hợp đồng bộ vẽ; `Represent3` D3D9 **8.164 dòng** (KRepresentShell3 4.143, TextureRes 1.390, TextureResMgr 655); `iRepresent/Font/KFontRes.cpp` font bitmap RLE riêng (**không GDI**), `Image/ImageOperation.cpp` 3 khối `__asm`, `TextureRes.cpp` 1 khối | Represent3 → thay bằng Represent4; Font/Image mang, viết lại asm |
| `Sources/S3Client` | 566 tệp, 170.041 dòng | `Ui/Elem` hệ KWnd 72 tệp, chỉ xử lý **13 loại thông điệp** (LBUTTONDOWN 11 chỗ, MOUSEMOVE 10, RBUTTONDOWN 7, MOUSEWHEEL 5, LBUTTONUP 5, DBLCLK 4, HOVER 3, KEYDOWN 2, SYSKEYDOWN/RBUTTONUP/MBUTTONDOWN/IME_CHAR/CHAR 1), **một điểm vào duy nhất** `Wnd_ProcessInput` (`Ui/UiShell.cpp:425`, `Ui/Elem/Wnds.cpp:284`); `Ui/UiCase` **134 cửa sổ / 318 tệp**; `S3Client.cpp:209` WinMain; `NetConnect/NetConnectAgent.cpp` 451 dòng nạp `Rainbow.dll` qua COM `IClient` (`:22`, `:66`); `AntiHack/` 8 luồng dò cửa sổ Windows (bỏ); HWND 15 tệp, MessageBox 28, GetKeyState 10, IME 1 | UI **nguyên** (vẽ qua Represent, bố cục từ 230 ini); WinMain/NetConnect/AntiHack/JxReplay/UpdateDLL/KLVideo thay hoặc bỏ |
| Mạng client | `MultiServer/Rainbow` 579 dòng, giao diện COM `Headers/IClient.h` (Startup/Cleanup/ConnectTo/…) | Windows | viết lại bằng BSD socket (ước 600–1.000 dòng; JX1M `network/` 3.618 dòng đã làm đúng khung gói + mã hoá XOR, dùng làm mẫu) |
| Script client | Lua 5.4.7 (lua4compat), **450 tệp** `bin\client\script` đã chuyển 05/09 (`LUA54_DA_CHUYEN.txt`) | không | nguyên (C thuần) |
| Dữ liệu client | `bin\client\data` **7.879 MB / 39 pak** (updatejx14 1.814 MB, updatejx15 1.521 MB, spr 880 MB, updatejx16 818 MB…), `Spr` rời 1.355 MB, `Maps` 358 MB, `settings` 81 MB ⇒ **≈ 9,6 GB** | không (cùng bộ đọc pak/spr của Engine) | nguyên định dạng; cần dedup/cắt gói (mục 5) |
| 64-bit | `Core`, `Engine` có cấu hình x64 (máy chủ chạy x64); **`S3Client`, `Represent3` chỉ Win32**; `Engine/Src/KWin32.h:52` `#define DWORD unsigned long` (đúng bẫy LP64 mà JX1M dính ở R152); `Headers/KProtocol.h` 173 trường `DWORD` + 3 `long`; ép con trỏ vào số (đếm thô bằng regex, chỉ để so cỡ): Core ~590, S3Client/Ui ~500, S3Client.cpp 25, Represent 17 | | typedef cố định cỡ + `static_assert` cỡ mọi struct gói (JX1M đã có bộ) + sửa theo lỗi biên dịch |
| RAM/CPU client | RAM riêng 500–800 MB (texture cache 512–1.024 MB, `.bss` 103 MB: `Npc[800]` 42 MB + lưới 58 MB), Game.exe không LAA (trần 2 GB); 145 người ≈ 2.800 draw call/khung; logic 18 Hz | | mục 5 |

### 1.2 Client mobile JX1M (cocos2d-x 3.17.2) — vì sao không phải nền để đi tiếp

- **Engine**: nhánh riêng của cocos 3.17.2 (10 tệp vá engine không có trong git), OpenGL ES 2, **Lua 4.0.1**; vẽ tay từng sprite **1 draw call**
  (`KgameWorld.cpp` 17.046 + `KgameWorldVN.cpp` 12.924 dòng); chính `D:\USVOLAM\JX1M_PHANTICH_NANG_ENGINE.md` kết luận nâng engine = 10–20
  người-tháng, không có đường lùi. Chủ đã loại hướng cocos.
- **Mã**: `gamecore` 155 tệp / 117.872 dòng là **nhánh Core cũ**: trùng tên 144/266 tệp với `Core/Src` nhưng nội dung trùng chỉ **4–41 %**
  (đo bằng tập dòng ASCII đã chuẩn hoá: KNpc 20–25 %, KProtocolProcess 17–19 %, KPlayer 18–33 %, CoreShell 4–14 %, KSkills 39–41 %). UI 152 tệp cocos
  (`gameui` 110.729 + `vn` 81.343 dòng).
- **Giao thức** (so `KProtocolDef.h` ba bên):

| Cặp | c2s trùng tên & giá trị | c2s trùng tên khác giá trị | s2c trùng | s2c khác giá trị |
|---|---|---|---|---|
| mobile ↔ USVOLAM (`D:\USVOLAM\Headers`) | **117/117** | 0 | **162/162** | 0 |
| mobile ↔ GAMEDEVNEW (`D:\GAMEDEVNEW\Headers`) | **46/143** | 70 (+27 chỉ GAMEDEVNEW) | **48/191** | 103 (+40 chỉ GAMEDEVNEW) |
| USVOLAM ↔ GAMEDEVNEW | 45/117 | 46 | 48/162 | 71 |

  ⇒ JX1M được căn (07–08/2026, R51→R208) theo **dòng dõi USVOLAM**; `D:\GAMEDEVNEW` (repo `JXGAME04/GAMEDEVNEW`, 1.447 commit từ 09/2024)
  là **dòng dõi khác** (Core trùng USVOLAM 17–66 %). Mobile cách dự án hiện tại **hai bậc**, và GAMEDEVNEW còn đổi giao thức hàng tuần
  (tháng 9: gói 221/222/223 đồng bộ delta, hello v2, băm ngoại hình FNV-1a, cỡ struct xếp hạng…).
- **Tính năng thiếu so với PC** (tệp `Core/Src` chỉ có ở PC, 55 tệp): đấu giá (`KAuctionClient/Server`), chiến lệnh, thư (`KMailClient/Server`),
  nhiệm vụ (`KMission`), JX2 công thành/liên đấu/danh hiệu/kho bang/Phi Phong (`KJx2*`, `KJx2WarInfra`), bang hội JX2 100 % (`KTongJx2`),
  bạn đồng hành/pet (`KPlayerPartner/Pet`, `KPartnerSkill`), SimCity, ghép/xúc xắc/bầu cua (`KItemCompound/Dice`, `baucua`), cấu hình web,
  gift code, `KOption`, chat vật phẩm, kỹ năng theo Linux (`KFightSkill/KStateFightSkill/KMissleSkill/SkillCommon`), thời tiết, tìm đường
  (`KJxPathFinder`), auto trong game (`KPlayerAuto`, `KAutoAI`), `KPerfTick`… Cửa sổ UI PC không có bản mobile (ước ~60/134): UiAuction,
  UiChienLenh, UiMail v2, UiWorldMap, UiTongKimInfo/Score, UiPartner* ×5, UiPet, UiMantleInlay/Wash, UiCityWar, UiBattleReport, UiStrengthRank,
  UiSkillTree, UiSmelt, UiGamble, UiTaskGuide/Note/Trace, UiTongJX2, UiDiceItem, UiBreakItem, UiTrembleItem, UiKtc, SpringGame… Cộng 450 script
  client Lua 5.4 (mobile Lua 4) và dữ liệu: **pak mobile là định dạng fork, bảng lệch cột** (`level_add` 13 vs 17, `meleeweapon` 53 vs 45…).
- **Đáng giữ làm tham chiếu (không giữ mã)**: bố cục chạm 152 cửa sổ + `KUiCustomPos` (811 dòng); `KuiAutoPlay.cpp` 13.736 dòng (auto 8 tab);
  `KDownLoadFile.cpp` 637 + `KUpdateLayerVN.cpp` 487 (tải CDN có resume, `paklist.ini`, bẫy `_r=0`); `KPakCrypto.h` (R190); tự cập nhật APK (R180);
  chat thoại; pipeline Android 2 ABI + iOS (Mac M5, `cmake -GXcode`) với 7 bẫy iOS đã ghi; bộ `static_assert` cỡ gói; luật `-fsigned-char`.

---

## 2. Các hướng và vì sao chọn

| Hướng | Lấy trọn bộ? | Công (ước) | Rủi ro chính | Kết luận |
|---|---|---|---|---|
| **A. Tiếp tục JX1M cocos, căn lại theo GAMEDEVNEW** | **Không bao giờ** — mục tiêu di động; 2 Core, 2 UI, 2 Lua, 2 định dạng pak | căn lại giao thức 3–6 tuần + đuổi tính năng 4–8 tháng + 30–50 % công của MỌI tính năng mới, mãi mãi | engine fork không nâng được; đã trượt 2 bậc | chủ đã loại |
| **B. Một mã nguồn + nền mới (SDL3 + SDL_GPU / bgfx)** | **Có, theo cấu trúc** (cùng Core, cùng Headers, cùng UI, cùng script, cùng pak) | 10–15 nghìn dòng mới + dọn 64-bit; 5–7 tháng | phần client chưa từng biên dịch ngoài MSVC/Win32; UX chạm; RAM máy yếu | **ĐỀ XUẤT** |
| **C1. Viết lại trong Unity / Godot / Unreal** | Không — viết lại ~400 nghìn dòng logic+UI, tách khỏi Headers máy chủ | 1–2 năm, rồi lệch mãi như A | như A nhưng đắt hơn | không |
| **C2. Nhúng Core C++ làm plugin (GDExtension / native plugin), engine chỉ làm vỏ** | Có | ≈ B + chi phí engine | engine không mang lại gì cho game sprite 2D palette (UI KWnd vẫn tự vẽ), thêm cỡ app, marshalling, license | không hơn B |
| **D. Axmol (hậu duệ cocos)** | Có, nếu nhúng Core như C2 | ≈ C2 | cùng dòng cocos | chủ đã loại |
| **E. Cloud streaming (chạy client PC trên máy chủ, stream về điện thoại)** | Tức thì, 0 port | thuê GPU/người chơi | độ trễ, chi phí theo giờ chơi, không phải app | chỉ làm cầu nối tạm |

Vì sao B là "công nghệ mới" thật sự mà vẫn giữ trọn bộ: mọi thứ mới nằm **sau các giao diện đã có sẵn** trong mã (`iRepresentShell`,
`IClient`, `KMusic/KWavSound`, `Wnd_ProcessInput`), nên logic và 134 cửa sổ UI không biết mình đang chạy trên Vulkan/Metal hay D3D9.
Dự án đã làm đúng kiểu này một lần: công tắc `Render.ini [Render] Represent3=0/1` (Represent2 ↔ Represent3, 07/2026).

---

## 3. Kiến trúc đề xuất (hướng B)

```
GIỮ NGUYÊN, dùng chung với máy chủ:  CoreClient (Core/Src) + UI KWnd (S3Client/Ui) + Engine thuần + Lua 5.4 + pak/spr/ini
      │ iRepresentShell (40 hàm)   │ IClient (COM, Rainbow.dll)   │ KMusic / KWavSound / KMp3Music   │ WinMain + Wnd_ProcessInput
      ▼                            ▼                              ▼                                 ▼
 Represent4                     Net4                            Sound4                            Platform4
 SDL_GPU (Vulkan/Metal/D3D12)   BSD socket, khung [WORD size]   SDL audio / miniaudio / FMOD      SDL3: cửa sổ, vòng lặp, chạm→chuột,
 hoặc bgfx (thêm GLES)          + XOR key như Rainbow                                              bàn phím/IME, timer, tệp, luồng, đường dẫn
 batching + atlas + A8L8 shader
```

Điểm chạm cụ thể (để phiên thi công không phải tìm lại):

| Việc | Chỗ trong mã |
|---|---|
| Vòng lặp chính, tạo cửa sổ Windows | `S3Client/S3Client.cpp:209` `WinMain`; `Engine/Src/KWin32App.cpp`, `KWin32Wnd.cpp` |
| Input vào UI | `S3Client/Ui/UiShell.cpp:425` → `Wnd_ProcessInput` (`Ui/Elem/Wnds.cpp:284`), `Wnd_TransmitInputToGameSpace` (`:489`); 13 loại `WM_` ở bảng 1.1 |
| Bộ vẽ | `Represent/iRepresent/iRepresentShell.h` (40 hàm ảo); mẫu cài đặt `Represent3/KRepresentShell3.cpp`; `TextureRes.cpp:609` `CreateTexture16Bit`; font `iRepresent/Font/KFontRes.cpp`; asm `iRepresent/Image/ImageOperation.cpp` |
| Mạng | `S3Client/NetConnect/NetConnectAgent.cpp:22,66` (nạp `Rainbow.dll`); `Headers/IClient.h`; `MultiServer/Rainbow/*.cpp` 579 dòng; mẫu BSD: JX1M `Classes/network/SocketClient.cpp` 594 dòng |
| Âm thanh | `Engine/Src/KMusic.cpp`, `KDSound.cpp`, `KWavFile.cpp`, `KMp3Music.cpp`, `KMpgMusic.cpp` (1.536 dòng); Core/S3Client chỉ include `KMusic.h`, `KWavSound.h`, `KSoundCache.h` |
| Tệp/pak/ini | `Engine/Src/KFile.cpp`, `KFilePath.cpp`, `XPackFile.cpp`, `ZSPRPackFile.cpp` (`CreateFile/FindFirstFile` → `fopen/opendir`); `GetPrivateProfile` 8 tệp Core → `KIniFile` |
| 64-bit | `Engine/Src/KWin32.h:43,52,63` (`LONG`, `DWORD`); `Headers/KProtocol.h` (`#pragma pack(1)`, 173 `DWORD`, 3 `long`); cấu hình x64 thiếu ở `S3Client.vcxproj`, `Represent3.vcxproj` |
| Bỏ trên mobile | `S3Client/AntiHack` (8 luồng dò cửa sổ), `JxReplay`, `UpdateDLL`, `KLVideo`/`KMp4Video`, `KFileDialog`, `Kime` (thay bằng bàn phím hệ thống) |
| Auto | WAuto = Win32 (`J:\CayChay\...\_WAuto`, 5.766 dòng) nói chuyện với CoreClient qua bộ nhớ chung `ipc_shared.h` (11 lệnh `PRT_*`, 7 phản hồi `PRG_*`); logic auto nằm trong `CoreShell.cpp` (`:16628` `IPCMainSync`); cửa sổ trong game sẵn có `UiCase/UiAuto`, `UiAutoPlay`, `UiAutoParty` |
| Bố cục UI theo màn | `Ui/Elem/WndWindow.cpp:73-74` `FitToScreen` ghim 1024×768; trình sửa trong game `[UITOADO]` (Ctrl+U, `Ui/Elem/UiToaDo.cpp`, ini `UserData\UiToaDo.ini`, mục `Main1024`) |

Quyết định thiết kế quan trọng:

- **Giữ khoá hình học 1 world-unit = 1 pixel** (`REPRESENT3_UPGRADE.md` §1: đổi tỉ lệ thế giới = lệch máy chủ). Trên điện thoại: vẽ vào
  **khung ảo** 1024×576 … 1138×640 rồi phóng cả khung lên màn (viewport scale trong Represent4, không phải zoom thế giới); toạ độ chạm chia ngược
  lại trước khi đưa vào `Wnd_ProcessInput`. JX1M cũng làm thế (design 1040×603 + `EXACT_FIT`), nhưng ở Represent4 có thể lọc/scale sạch hơn.
- **Chạm → chuột** ngay tại Platform4: tap = `WM_LBUTTONDOWN/UP`, giữ = `WM_RBUTTONDOWN`, hai ngón = `WM_MOUSEWHEEL`, kéo = `WM_MOUSEMOVE`
  có nút; joystick ảo phát lệnh đi như auto (`MoveTo`/`GotoWhereDirect`, không đi qua `Mouse_Action` để tránh bẫy `[TUKICH]`).
  Nhờ KWnd chỉ dùng 13 loại thông điệp, không phải sửa cửa sổ nào.
- **Chữ**: font bitmap TCVN3 sẵn trong `font.pak` (2,2 MB) — vẽ được ngay; nhập từ bàn phím điện thoại (UTF-8) → chuyển TCVN3 tại Platform4
  (mẫu: JX1M `Classes/vn/Translate.cpp`).
- **Texture**: theo `PHUONGAN_KIENTRUC_HIEUUNG_RAM_0809.md` §RAM — palette 8-bit giữ nguyên, texture **A8L8/R8 + bảng màu** + pixel shader
  (÷4 RAM so với RGBA8888), atlas theo tệp spr, batching theo atlas. Đây là việc **làm một lần cho cả PC lẫn mobile**.
- **Auto trên mobile**: không có tiến trình ngoài ⇒ cửa sổ auto trong game gửi thẳng các lệnh `PRT_*` vào đường IPC hiện có (bỏ bộ nhớ chung,
  gọi hàm). Bố cục 8 tab của `KuiAutoPlay` JX1M là mẫu.
- **Đóng gói**: CMake dựng Engine/Core/S3Client/Represent4 thành thư viện tĩnh + app SDL; Android: Gradle + NDK arm64-v8a (+armeabi-v7a nếu
  cần), iOS: `cmake -GXcode`; bộ tải HTTP có resume + `paklist.ini` viết lại bằng libcurl (S3Client đã có vcpkg curl) theo mẫu JX1M; mã hoá pak
  tuỳ chọn (R190).

---

## 4. Lộ trình theo pha — mỗi pha có mốc đo được, dừng được, và có lợi riêng cho PC

| Pha | Việc | Mốc nghiệm thu | Ước công |
|---|---|---|---|
| **0. Game.exe 64-bit** | thêm cấu hình x64 cho `S3Client`, `Represent3`, Engine client; `DWORD/LONG` cố định cỡ (`uint32_t`); `static_assert` cỡ **mọi** struct `KProtocol.h` (mượn bộ của JX1M); sửa ép con trỏ theo lỗi biên dịch; cờ `-fsigned-char`/`/J`-tương-đương kiểm tra băm pak | `Game64.exe` vào Tống Kim, `CheckProtocolSize` khớp 100 %, RAM vượt 2 GB không sập | 2–3 tuần — **lợi ngay cho PC**: hết trần LAA, `Rep3CacheMB=1500` an toàn |
| **1. Biên dịch bằng clang** | clang-cl trên Windows rồi clang NDK arm64 (chỉ biên dịch, chưa link/chạy): dọn MSVC-ism, 5 tệp `__asm` không có nhánh C → C, `windows.h`, `MessageBox` → log, `GetPrivateProfile` → `KIniFile`, `#pragma pack` nhãn | CoreClient + Engine + S3Client/Ui biên dịch sạch thành `.a` arm64 | 2–3 tuần — ra **danh sách port chính xác** thay cho đếm regex ở 1.1 |
| **2. Platform4 + Net4 + Sound4 trên Windows** | thay WinMain/DInput/IME/timer/tệp/luồng bằng SDL3; `IClient` bằng BSD socket; âm thanh; hình vẫn qua Represent3 | `Game64-SDL.exe` chơi được trên Windows, không đổi hành vi | 3–4 tuần |
| **3. Represent4** | 40 hàm `iRepresentShell` trên SDL_GPU (hoặc bgfx); batching + atlas; A8L8 + shader; text bitmap; khung ảo + scale; công tắc `[Render] Represent=4`; so ảnh với Represent3 bằng `bin\client\JxCap` | PC chạy `Represent=4` cùng FPS, RAM texture giảm ≥ 50 %, ảnh trùng Represent3 | 4–6 tuần |
| **4. Android chạy** | CMake/NDK + app SDL; bộ tải CDN; chạm → chuột; đường dẫn ghi/đọc; vào game trên LDPlayer rồi điện thoại | vào Tống Kim trên điện thoại, mở đủ 134 cửa sổ | 3–4 tuần |
| **5. UX chạm + hiệu năng + dữ liệu** | joystick/skill bar/bàn phím chat/cửa sổ auto; bố cục `Main<res>` soạn bằng UITOADO; ẩn cửa sổ chỉ PC; ngân sách RAM (texture 256–512 MB, `MAX_NPC` theo máy), nhịp 54/36 tiết kiệm pin; dedup pak 9,6 GB | bản Android chơi thật; đo RAM/FPS/pin/MB tải trên 2 máy (mạnh + 4 GB) | 4–6 tuần |
| **6. iOS** | Metal qua SDL_GPU; `cmake -GXcode`; TestFlight; áp 7 bẫy iOS + pháp lý theo `D:\USVOLAM\JX1M_BANGIAO_IOS.md` | iPhone thật | 2–4 tuần |

Tổng **ước 5–7 tháng** Android, **+1 tháng** iOS (một người + AI, theo tốc độ thực của chính dự án: JX1M căn giao thức 2 tuần, iOS 17 vòng
trong 2 ngày, 5 UI/ngày; Represent3 8 nghìn dòng đã viết trong dự án này).

Song song, **nên làm trong Core trước pha 5**: thiết kế lại hiệu ứng theo sự kiện máy chủ (`PHUONGAN_KIENTRUC_HIEUUNG_RAM_0809.md` §1) — client
bớt mô phỏng lại chiêu/đạn, CPU điện thoại yếu hơn PC hưởng lợi nhiều nhất, và PC hết mất hiệu ứng.

---

## 5. Rủi ro và cách gỡ

| Rủi ro | Cách gỡ |
|---|---|
| Phần client chưa từng biên dịch ngoài MSVC/Win32 (S3Client, Represent, nửa client của Core) | pha 0–1 là **đo**, không có gì để mất; kết quả là danh sách chính xác; chỉ sau đó mới ước lại công |
| `char` có dấu: `KPakList.cpp` băm tên pak bằng `signed char`, ARM mặc định `unsigned` | `-fsigned-char` toàn dự án + test băm 1 tệp (bẫy ③ JX1M iOS) |
| Mã nguồn TCVN3/GBK | giữ luật hiện có: chỉ sửa bằng công cụ byte (`safe_edit`/python latin-1), cấm Edit/Write thường |
| RAM: PC 500–800 MB; điện thoại 4 GB kẹt | A8L8 (÷4 texture), cache 256 MB, giữ `Npc[800]` 42 MB + lưới 58 MB; hiệu ứng theo sự kiện máy chủ; đo trên máy 4 GB thật ở pha 5 |
| Hiệu năng vẽ: 2.800 draw call/khung ở 145 người | Represent4 **batch theo atlas ngay từ đầu** (JX1M thất bại vì 1 draw/sprite); mục tiêu ≤ 200 draw call/khung |
| Dữ liệu 9,6 GB (updatejx14–16 = 4,15 GB) | dedup lớp `updatejx` (pak sau đè pak trước, first-hit-wins) trước; tải theo gói (map/phái) sau; đo lại trước khi hứa cỡ tải |
| iOS bỏ OpenGL ES; App Store và bản quyền Kingsoft/VNG | SDL_GPU Metal là đúng "công nghệ mới"; pháp lý ngoài kỹ thuật, đã ghi ở JX1M — Android phát hành bằng APK tự cập nhật không vướng |
| Bỏ dở giữa chừng | pha 0 (64-bit), pha 3 (bộ vẽ mới, ít RAM) có giá trị riêng cho PC; pha 2 cho bản PC không phụ thuộc D3D9/Rainbow |
| Hai phiên/hai cây song song (bẫy đã ghi nhiều lần) | nhánh mobile ở **worktree riêng** của `D:\GAMEDEVNEW`, không đụng cây live `E:\SourceTuanLe\...\TESTLOFFF_ONLINE`; khe `.moi` Game.exe đang có bản UITOADO chờ swap — không chen |
| Máy Android quá cũ không có Vulkan | chọn bgfx (có GLES2/3) thay SDL_GPU, hoặc đặt tối thiểu Android 8 + Vulkan (đa số máy từ 2017) — chủ quyết |

---

## 6. Độ chắc của số liệu

- Đếm phụ thuộc Win32 bằng grep theo tệp; đếm ép con trỏ bằng regex thô — đúng để **so cỡ**, không đúng từng chỗ. Con số port chính xác chỉ có
  sau pha 1.
- Trùng dòng Core PC ↔ mobile đo bằng tập dòng ASCII đã bỏ khoảng trắng (vì mã hoá hai bên khác nhau nên `diff` thường vô nghĩa).
- Ước công lấy từ tốc độ thực của chính dự án (mục 4), chưa tính thời gian chủ test.
- Chưa đo: hiệu năng SDL_GPU/bgfx trên máy Android yếu; cỡ pak sau dedup; RAM thực của client 64-bit.
- Không biên dịch, không chạy gì trong phiên này. Lệnh nặng nhất đã chạy: quét kích thước thư mục `bin\client` (I/O ~2 phút).

---

## 7. Chủ cần quyết trước khi bắt tay

1. Chốt hướng **B (một mã nguồn)** — có / không.
2. Bộ vẽ: **SDL_GPU** (Vulkan/Metal/D3D12; bỏ máy Android không Vulkan) hay **bgfx** (thêm GLES cho máy cũ, thư viện to hơn, C++ nhiều hơn).
3. Cho `Represent4` **thay Represent3 trên PC** (một bộ vẽ cho mọi nền, RAM thấp hơn) hay chỉ dùng cho mobile.
4. Bắt đầu **pha 0 (Game.exe 64-bit)** ngay ở worktree riêng — có / không. (Không đụng khe `.moi` đang chờ.)
5. Dữ liệu mobile: chấp nhận ~9,6 GB hay cắt gói (map/phái tải sau) — cần trước pha 5.

---

## 8. Ghi chú thêm

- Thư mục đang mở `J:\CayChay\Src_Auto_Ngoai\WAuto\WAuto` có `_DO_NOT_USE_WRONG_TREE.txt` (cây bẫy, không build WAuto ở đây) và một bản sao
  **dở dang** của `Classes/`, `cocos2d/`, `proj.android/` JX1M ghi ngày 06–07/08 (chỉ `KuiAutoPlay`, `KgameWorldVN`, `AutoPathFinder`…, 2 tệp
  gamecore) — không phải dự án chạy được, không dùng cho phương án này.
- Tài liệu liên quan: `PHUONGAN_KIENTRUC_HIEUUNG_RAM_0809.md` (hiệu ứng theo sự kiện, A8L8), `BANGIAO_HIEUUNG_KHAOSAT_0709.md` (RAM/LAA, bể đạn),
  `D:\USVOLAM\REPRESENT3_UPGRADE.md` §1 (khoá hình học 1:1), `D:\USVOLAM\JX1M_PHANTICH_NANG_ENGINE.md` (vì sao không nâng cocos),
  `D:\USVOLAM\JX1M_BANGIAO_IOS.md` (7 bẫy iOS + pháp lý), `D:\USVOLAM\JX1M_PROTOCOL_SYNC.md` (cơ chế `CheckProtocolSize`, desync stream).
