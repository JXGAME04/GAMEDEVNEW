# BÀN GIAO — VẬN TIÊU: BỘ TEST TRÊN LỆNH BÀI ADMIN + HƯỚNG DẪN TRONG CHỈ NAM NHIỆM VỤ (06/09/2026)

Nhánh `chinam-vantieu-0609` (worktree `D:\GAMEDEVNEW_wt_vtcn`, tách từ `origin/main` f771a93f) → đã gộp vào `origin/main`.
Đọc cùng: `BANGIAO_VANTIEU_TEST_0609.md` (đợt port), `DACTA_ENGINE_XETIEU_0609.md` (engine xe tiêu).

---

## 1. MỘT PHÚT NẮM VIỆC

Chủ yêu cầu hai việc; cả hai đã làm xong và đặt vào cây chạy thật:

1. **Lệnh bài admin**: menu chính rút từ 19 mục xuống **8 mục** (các bộ test gom thành 2 nhóm "Bộ test hoạt động" và "Bộ test hệ thống nhân vật"); thêm **bộ test vận tiêu** `VT_TestRoot` (8 mục con: xe tiêu engine, nhánh cá nhân, nhánh bang, cờ test bỏ qua điều kiện, dịch chuyển tới 45 NPC, xem trạng thái, xem nhật ký, sinh lại NPC).
2. **Chỉ nam nhiệm vụ (F11)**: nhóm mới **"Long Môn Tiêu Cục"** với 2 mục **"Vận tiêu cá nhân"** và **"Vận tiêu bang hội"**: bài hướng dẫn đầy đủ 4 bước + điều kiện + phần thưởng + trạng thái sống (tuyến đang nhận, xe xuất phát chưa, số lần trong ngày, điểm bám xe). **Nhấp vào dòng nhiệm vụ = tự đi tới nơi nhận**: chưa nhận → Ông chủ Tiêu cục ở Long Môn trấn (khác bản đồ thì máy chủ thuê xe, giá `VT_TIEN_XE_CHINAM` mặc định 1000 lượng), đã nhận → Tiêu Sư điểm đầu (miễn phí, máy chủ đưa đi), bang → Tiếp Dẫn áp Tiêu Bang (ngoài thành thì tự dùng phù về thành). Nút **Bỏ nhiệm vụ** và **Theo dõi** của bảng cũng hoạt động với 2 mục này.

Trong lúc làm, phát hiện **nhánh cá nhân của đợt port không thể chạy** trên JX1 (mục 3) → đã nối NPC cá nhân sang **mã gốc Linux không trạng thái** có sẵn trong `config\129\extend.lua`. Đây là thay đổi lớn nhất, lùi được bằng một hằng số (mục 3.2).

---

## 1b. CẬP NHẬT 06/09 TỐI (v2) — CHỦ CHỐT HAI LUẬT MỚI

1. **Chỉ nam KHÔNG được nhảy map.** Bấm dòng nhiệm vụ nay chỉ đi bằng đường người chơi vẫn đi: đang ở **thành/thôn** (map có bến Xa Phu trong `g_MoveStation`) → chạy tới Xa Phu, tự chọn "Những thành thị đã đi qua" → tên bến của map đích (cột DESC `settings\Station.txt`, ví dụ "Trấn Long Môn", "Lâm An Phủ"; tốn tiền xe như thường) → sang map rồi đi bộ tới NPC; đang ở **map luyện công** → dùng phù về thành trước, về tới thành mới ra Xa Phu. Máy chủ không còn `vt_goto_canhan` (đã xoá khỏi `vt_chinam.lua` và danh sách trắng C++); nhánh bang chọn thành = thành có `szOwner` (CITYINFO) trùng tên bang mình.
2. **Map sự kiện chặn mọi tự-di-chuyển của Chỉ nam** (Dã Tẩu → Xa Phu, Sát Thủ → boss, Vận tiêu): `TG_ChanMapSuKien` dùng bảng `KMapSuKien.h` (`WA_MapSuKien`), báo "[Chỉ nam] Đang ở map sự kiện (<tên>) - Chỉ nam không tự di chuyển ở đây."

Đồng thời phiên SAPXEP đã **sắp xếp lại cây script** (restart 18:04): `script\activitysys` → `script\tinhnang\activitysys`, `script\item` → `script\vatpham` (lệnh bài + bộ test vận tiêu nằm ở `script\vatpham\`), `script\event\lmbiaoche` → `script\tinhnang\sukien\lmbiaoche`; đường cũ chạy qua bí danh `script\_duongdan_cu.txt` (dòng `--@ cũ=mới`, engine R2). Mọi đường dẫn ở các mục dưới hiểu theo bí danh; **ghi tệp mới phải ghi vào đường dẫn mới** rồi chạy `tools\sapxep\kiem_duongdan_cu.py` (đã chạy sau khi dời `vt_chinam.lua` v2: 1238 bí danh, 0 tệp ở đường cũ, 0 đích thiếu).

**Máy chủ 18:04 đang chạy `CoreServer.dll` 6ba06754 (phiên SAPXEP build từ origin/main sau khi tôi đẩy) = đã có engine xe tiêu + bí danh + lệnh `vt_quit_*`; client 18:05 đang chạy `Game.exe` 3c4daa2c + `CoreClient.dll` 8afed3eb (v1 của tôi).** Bản v2 (không nhảy map + chặn map sự kiện) chỉ đổi **client** → chủ chỉ cần `ChoiGame.bat` lần nữa; `CoreServer.dll.moi` v2 (bỏ `vt_goto_canhan`, siêu tập origin/main c4761ea5) đặt sẵn cho lần restart sau, không bắt buộc.

## 1c. CHỦ CHỐT (06/09 tối): 'làm như bản Linux' + 'cấp hạ xuống 90'

- **Như bản Linux**: giữ nhánh cá nhân theo mã gốc Linux không trạng thái (`extend.lua`), phần thưởng `award.lua` số Linux, luật giờ/lượt/Tiêu Kỳ/Tán Lạc Tiêu Vật nguyên gốc. Giá cửa hàng Hộ Tiêu Lệnh (5/5/10) và tiền thuê Xa Phu là chỗ Linux không còn số nên vẫn là số tạm qua `HD_CFG`.
- **Cấp 90 cho MỌI ngưỡng cấp của vận tiêu** (Linux là 120 nhận cá nhân / 150 nhận xe bang, nhặt Tiêu Kỳ, nhặt Tán Lạc, dùng đạo cụ): đã sửa 12 chỗ bằng `ReverseTools\vantieu\va_vtcn_cap90_0609.py` (byte-safe, có `.truoc_cap90_0609`): `npc_canhan.lua` (nhận), `npc_consigner.lua` (nhận xe bang), `npc_lmbiaoqi.lua` (Tiêu Kỳ), `npc_lmbiaowu.lua` (Tán Lạc), `item_addproperty.lua` (đạo cụ), `dialog.lua`/`lmbj_config.lua`/`lang.lua` (lớp cũ, cho đồng bộ), bộ test admin và bài hướng dẫn trên client (`Game.exe.moi` mới, mục 2.1).

## 2. CÁCH CHẠY (chủ làm)

| Bước | Lệnh | Ghi chú |
|---|---|---|
| Máy chủ | (không bắt buộc) tắt GameServer → `bin\server\ChayGameServer.bat` | Nhận `CoreServer.dll.moi` v2 61bc3e0f; chỉ khác bản đang chạy ở chỗ bỏ lệnh chết `vt_goto_canhan`. Máy chủ 18:04 đã có mọi thứ cần cho test. |
| Client | **BẮT BUỘC** thoát game → `bin\client\ChoiGame.bat` | Nhận `Game.exe.moi` acc36aba + `CoreClient.dll.moi` 96dc5115 (v2: không nhảy map, chặn map sự kiện). `Ui\uitasklist.ini` đã có 3 mục. |
| Sau khi lên | Lệnh bài admin → "Bộ test hoạt động" → "Vận tiêu Long Môn Tiêu Cục" | mục 5 |

### 2.1 Nhị phân v2 (build từ `origin/main` c4761ea5 = đã có SAPXEP R2 bí danh + LUA54b + BOTNOI + vận tiêu + relay, cộng nhánh này; worktree sạch)

| Tệp | Đặt ở | SHA-256 (8 đầu) | Có đủ (kiểm bằng `ReverseTools\vantieu\deploy_vtcn2_0609.py`) |
|---|---|---|---|
| `CoreServer.dll.moi` | `bin\server` | **61bc3e0f** (18.455.552 B, 18:12) | CreateBiaoChe/BC_SetEnable, AUC_MsgTong, CL_Cong, UpdateBattleInfo, st3_goboss, Lua54Dll, [RELAYHT], LUA_CALL, `_duongdan_cu` (R2), **vt_quit_canhan/vt_quit_bang**, KHÔNG còn vt_goto_canhan. Không bắt buộc swap ngay: bản 6ba06754 đang chạy chỉ thừa lệnh chết. |
| `Game.exe.moi` | `bin\client` | **04ae18d2** (bản cấp 90, thay acc36aba) | NewTask/F11, tg_quit/st3_quit, vt_quit_*, bài hướng dẫn vận tiêu v2 (gợi ý Xa Phu / phù về thành / map sự kiện); `re_pe_crt` UCRT-RELEASE đúng |
| `CoreClient.dll.moi` | `bin\client` | **96dc5115** (2.577.408 B, 18:12) | TG_VanTieu v2 (6 pha, không nhảy map), TG_ChanMapSuKien cho Dã Tẩu/Sát Thủ/Vận tiêu, tên bến Xa Phu |

Cặp thư viện: `Lib\lua54\x64\Lua54Dll.dll` 5db18c30 = `bin\server\Lua54Dll.dll` đang chạy; `Lib\lua54\Win32\Lua54Dll.dll` 462453cf = `bin\client\Lua54Dll.dll` đang chạy (không cần đổi thư viện).

Bản v1 (15:34: CoreServer a9db7810, Game 3c4daa2c, CoreClient 8afed3eb) đã được chủ swap lúc 18:04/18:05; CoreServer v1 sau đó bị phiên SAPXEP thay bằng 6ba06754 (siêu tập, có bí danh) trước khi restart.

Kiểm nhanh sau restart: `Logs\ScriptError.log` không có dòng mới về `lmbiaoche`, `longmenbiaoju`, `config\129`, `lenhbaiadmin`; vào game bấm **F11** thấy nhóm "Long Môn Tiêu Cục".

---

## 3. PHÁT HIỆN QUAN TRỌNG (đọc trước khi test)

### 3.1 Khe `.moi` máy chủ bị nhiều phiên thay nhau trong chiều nay
`grep -a -c CreateBiaoChe` trên `CoreServer.dll` đang chạy (10:23) và bản `.moi` 14:23 của phiên relay đều = **0** (bản 8af0c392 của đợt port đã bị đè). Bản 15:21 của phiên LUA54b có engine nhưng chưa có lệnh `vt_*`. Vì vậy tôi gộp `origin/main` mới nhất (75b1091d) vào nhánh rồi build lại cả ba nhị phân để bản đặt vào khe là **siêu tập** của mọi phiên (kiểm dấu hiệu: `CreateBiaoChe`, `[RELAYHT]`, `LUA_CALL`, `AUC_MsgTong`, `CL_Cong`, `UpdateBattleInfo`, `st3_goboss`, `vt_goto_canhan` — kịch bản `deploy.py` trong scratchpad, ghi ở mục 8).

### 3.2 Nhánh cá nhân của đợt port không chạy được trên JX1 → nối sang mã gốc Linux không trạng thái
- JX1: **mỗi tệp `.lua` là một `lua_State` riêng** (`Engine\Src\KLuaScript.cpp:23 lua_open`). Lớp `LongMenBiaoJu.*` giữ sổ đăng ký xe trong state của `tasknpc.lua`, còn `OnTimer`/`OnDeath` của xe chạy trong state của `biaoche.lua` → không thấy nhau: xe tạo bằng `AddNpcEx` đứng yên, không hết hạn, chết không rơi Tiêu Kỳ.
- `tasknpc.lua` gọi `LongMenBiaoJu:GetDlgClass()` nhưng không Include `dialog.lua` → thoại NPC lỗi ngay khi bấm.
- `config\129\extend.lua` (mã Linux 2013) đã có **trọn nhánh cá nhân không trạng thái**: `ReceiveTaskLow/Random/High`, `ReceiveBiaoChe` (xe = `CreateBiaoChe` engine), `ReceiveAward`, `ViewTaskInfor`, `Transport2Dest`, `AbandonTask`, thưởng `award.lua` (số Linux), vật phẩm mã JX1 4771–4778, Tiêu Kỳ/Tán Lạc Tiêu Vật bằng `npc_lmbiaoqi.lua`/`npc_lmbiaowu.lua` (NpcParam). Chỉ thiếu `main()` cho NPC.
- **Đã làm**: `script\activitysys\config\129\npc_canhan.lua` (mới, 2 thoại NPC + cửa hàng đổi Hộ Tiêu Lệnh) và `script\startgame\lmbj_addnpc.lua` trỏ `LMBJ_SCRIPT_CANHAN` vào tệp này. **Lùi**: đổi hằng đó về `\script\event\longmenbiaoju\tasknpc.lua`.
- Hệ quả: biến nhiệm vụ cá nhân nay là **4178–4187** (chung khối với nhánh bang, cùng bảng tuyến `tbBJPathLevel` 1–26), phần thưởng là **số Linux gốc** (`award.lua`: 1 sao 1 lần bốc 1.288 vạn exp hoặc vật phẩm … 9 sao 30 lần, 8.888 vạn) thay cho số tự đặt trong `lmbj_config.lua`. `lmbj_config.lua`/`taskclass.lua`/`dialog.lua` vẫn nằm trong cây nhưng không NPC nào dùng.

### 3.3 Sáu lỗi có sẵn khác đã vá (đều cùng gốc "state riêng" hoặc mã vật phẩm)
| # | Tệp | Lỗi | Vá |
|---|---|---|---|
| 1 | `lmbj_config.lua` | mã vật phẩm Linux 4200–4207 (ở JX1 là "Thương Long Văn Tinh - Bình") | → 4771–4778; Lễ Bao 4534 (dòng rác) → 4809 Hộ Tiêu Lễ Hộp |
| 2 | `longmenbiaoju\item.lua` | so `"6,1,4204"`… nên Khoái Mã Gia Tiên/Kiên Bất Khả Tỏa/Tiêu Xa Di Vị bấm không chạy (cả bang lẫn cá nhân) | → 4775/4776/4778 + Include `extend.lua` (getSkillLevel đọc `tbBJPathLevel`) |
| 3 | `extend.lua` | dùng `self.tbAward`/`tbAllCountCell` (award.lua) mà không Include → giao xe bang / nhận thưởng cá nhân lỗi nil | Include `award.lua` ngay sau `variables.lua` (trước mọi định nghĩa vì `head.lua` tạo `pActivity` mới) |
| 4 | `npc_lmbiaowu.lua`, `npc_lmbiaobox.lua` | dùng `pActivity.tbPickCountCell`/`AddTaskDailyA`/`CheckJoinTongDays` mà không Include extend | Include `extend.lua` |
| 5 | `event.lua` `g2s_GetBiaoChePos` | `RemoteExecute` JX1 chạy **tại chỗ** (KJx2SharedStore.cpp) không đặt PlayerIndex; `GetBiaoChePos` JX1 là bản GameServer (không tham số, trả x32/y32/subworldIdx) trong khi callback chờ (mapId, ô, ô, cờ) → truyền tống tới xe sai chỗ | tra theo TÊN qua `SearchPlayer` + `CallPlayerFunction`, đổi về dạng relay |
| 6 | `ReverseTools\vantieu\sinh_lmbj_config.py` | bộ sinh vẫn phát 4200… | đổi số để tái sinh không mất vá |

### 3.4 Cờ test (bit trong biến nhiệm vụ 4169 của nhân vật admin)
Vì state riêng nên không dùng biến toàn cục; `VT_TestBoQua(nBit)` ở `script\lib\lib_lmbiaoche.lua`, chỉ tác dụng với người bật:
bit 1 bỏ qua giờ/thứ (cá nhân 10–23h, bang T7/CN 12–23h), bit 2 bỏ qua "vào bang 7 ngày", bit 3 coi bang mình là bang chiếm thành đang đứng. Móc tại `npc_consigner.lua` (9 chỗ), `extend.lua` (`CheckJoinTongDays`, `GetOccupyTongId`, 2 khung giờ), `taskclass.lua` (3 khung giờ, nhánh cũ).

---

## 4. LỆNH BÀI ADMIN — CẤU TRÚC MỚI

```
Lệnh bài admin (8 mục)
├─ Quản lý máy chủ (system)
├─ Chức năng chính: nhận trang bị, điểm, vật phẩm, thử nghiệm (manager)
├─ BOT người chơi (PB_Menu)
├─ Chiến Lệnh admin (CL_MenuAdmin)
├─ Gửi thư thử: có vật phẩm (mailtest2)
├─ Bộ test hoạt động (ADM_TestHoatDong)
│   ├─ Vận tiêu Long Môn Tiêu Cục  ← MỚI (VT_TestRoot)
│   ├─ Bang hội - công thành (TX_Root) · Dã Tẩu (DT_AdminMenu) · Liên đấu (LD_AdminMenu)
│   ├─ Bang Chiến - Bách Nhân - Tỷ Võ - Thành Bảo (HD_AdminMenu)
│   ├─ Hoạt động Linux: Sát Thủ - PLĐ - Vượt ải (HD3_AdminMenu) · Viêm Đế (TTHD_Root)
├─ Bộ test hệ thống nhân vật (ADM_TestNhanVat)
│   └─ Lò rèn · Kinh Mạch · Đồng hành · Phi Phong · Dung luyện
└─ Kết thúc
```
Mọi hàm gốc giữ nguyên tên; chỉ đổi chỗ bấm. `ADM_Main` = quay lại menu chính (có `admincheck`).

**`VT_TestRoot`** (`script\item\test_vantieu_admin.lua`, Include từ `lenhbaiadmin.lua` → sửa không cần restart):
1. Xe tiêu (engine): XT_Tao / XT_ViTri / XT_Song / XT_Xoa / tắt-bật hệ (gọi `thu_xetieu.lua` đúng state của nó).
2. Nhánh cá nhân: dịch chuyển tới Ông chủ; nhận 5 Hoán Tiêu Chỉ + 2 ủy nhiệm trạng + 20 Hộ Tiêu Lệnh; nhận 3 đạo cụ; nhận Tiêu Kỳ; reset bộ đếm ngày; huỷ nhiệm vụ (không phạt, xoá xe); xem trạng thái.
3. Nhánh bang: đặt **biến bang 1149** = 1 hoặc 3 cho bang mình (công thành chưa ghi biến này); reset tuần (1150/1151/1152/1154); huỷ nhiệm vụ bang; nhận 8 Rương Tiêu Vật Bang; nhận 2 gói thưởng bám xe; xem trạng thái bang (1149–1156, chức vị, ngày vào bang, thành đang chiếm).
4. Cờ test (mục 3.4) — đảo từng cờ, tắt hết.
5. Dịch chuyển: Ông chủ, 14 Tiêu Sư, 7 Tiếp Dẫn, 23 Nhận Hàng (tên + bản đồ, rơi cạnh NPC 2 ô).
6. Xem trạng thái đầy đủ (cá nhân + bang + engine `IsBiaoCheAlive`/`GetBiaoChePos`).
7. 12 dòng cuối `Logs\KSG_YunBiaoLog_<ngày>.txt`.
8. Sinh lại 45 NPC (`lmbj_addnpc()`, cảnh báo nhân bản).

---

## 5. TRÌNH TỰ TEST ĐỀ NGHỊ

**A. Xe tiêu engine** (như BANGIAO_VANTIEU_TEST mục 2, nay bấm menu thay vì gõ lệnh): 1→Tạo, đi xa, chuyển map bằng trap, phù về thành, chết, xoá.

**B. Cá nhân** (nhân vật admin cấp ≥120, có môn phái):
1. Cờ test → bật "bỏ qua giờ-thứ" nếu ngoài 10–23h.
2. Nhánh cá nhân → nhận vật phẩm test → dịch chuyển tới Ông chủ → thoại: Nhận nhiệm vụ áp Tiêu → 1 sao (đưa ngay tới Tiêu Sư điểm đầu) → Bắt đầu áp Tiêu → xe xuất hiện, bám theo.
3. **F11** → mục "Vận tiêu cá nhân": dòng trạng thái phải hiện đúng sao/tuyến/"đã xuất phát, còn ~30 phút"; nút Theo dõi hiện bản rút gọn ở khung mép phải.
4. Dẫn xe (Lâm An nam → Lâm An bắc, cùng bản đồ) tới Tiêu Sư điểm cuối → Nhận phần thưởng → thưởng theo `award.lua` + 2 Hộ Tiêu Lệnh; cửa hàng đổi đạo cụ.
5. Thử ngẫu nhiên: Nhận nhiệm vụ ngẫu nhiên → bảng tuyến 2–9 sao → làm mới miễn phí 5 lần → lần 6 đòi Hoán Tiêu Chỉ. Thử cao cấp với ủy nhiệm trạng.
6. Thử phá xe bằng nhân vật khác/bot: rơi "Tiêu Xa bị phá hoại" (Tiêu Kỳ) + "Tán Lạc Tiêu Vật"; mang Tiêu Kỳ về Tiêu Sư nhận thưởng an ủi.
7. Thử F11 dẫn đường từ bản đồ khác: bấm dòng nhiệm vụ → thuê xe → tới Long Môn trấn → tự chạy tới Ông chủ, mở thoại.

**C. Bang** (bang chủ, cấp ≥150, đứng trong một trong 7 thành):
1. Cờ test: bật cả 3 (giờ-thứ, 7 ngày, chiếm thành) hoặc đúng T7/CN 12–23h với bang chiếm thành thật.
2. Nhánh bang → "Đặt 1149 = 1" → dịch chuyển tới Tiếp Dẫn thành đó → Mở áp Tiêu Bang → Nhận áp Tiêu Bang → xe 10 sao + loa.
3. F11 mục "Vận tiêu bang hội": trạng thái tuyến + điểm bám xe; thành viên khác đứng gần xe (trạng thái chiến đấu) mỗi phút +400 vạn exp.
4. Dẫn xe tới Nhận Hàng điểm cuối → Giao Tiêu Xa Bang → 8 Rương; thành viên bám xe → Nhận phần thưởng hộ tiêu cá nhân; bang chủ nộp rương tại Tiếp Dẫn (`HandInBiaoCheBox`) rồi nhận thưởng nhóm.
5. Thử "Xem thông tin Tiêu Xa" và "Truyền tống đến Tiêu Xa" (vá mục 3.3-5) → phải tới đúng ô cạnh xe.

---

## 6. CHỈ NAM NHIỆM VỤ — BẢN ĐỒ MÃ

| Việc | Nơi |
|---|---|
| Bài hướng dẫn + tên 44 điểm + số thưởng | `Sources\S3Client\Ui\UiCase\UiTaskGuideVanTieu.h` — **SINH TỰ ĐỘNG** bởi `ReverseTools\vantieu\gen_vantieu_chinam.py` (đọc `extend.lua`, `award.lua`, `lmbj_addnpc.lua` của cây chạy thật). Sửa lời văn: sửa trong script sinh rồi chạy lại + build Game.exe. |
| Toạ độ NPC/tuyến (Core) | `Sources\Core\Src\KVanTieuPos.h` (sinh), thông báo dẫn đường `KVanTieuMsg.h` (sinh) |
| Mục 11/12, dựng nội dung, theo dõi, nút Bỏ | `UiTaskGuide.cpp`: `BuildVanTieuCaNhanText/BangText`, `VT_BuildBrief`, `UpdateButtons`, `TryGoXaFu` |
| Dẫn đường | `CoreShell.cpp` `TG_VanTieuStart/Tick` (3 pha: đợi máy chủ đổi map / phù về thành / đi bộ tới NPC theo template 2145/2157/2230/2231), enum `GOI_TASKGUIDE_GOTO_VANTIEU` cuối `CoreShell.h` |
| Máy chủ | `KProtocolProcess.cpp` UI_CMD case 6 thêm `vt_goto_canhan` / `vt_quit_canhan` / `vt_quit_bang` → `script\event\lmbiaoche\vt_chinam.lua` |
| Danh mục | `Ui\uitasklist.ini` [6] nhóm, [7] TaskId 11, [8] TaskId 12 (bản git `TaskGuideRes\Ui\uitasklist.ini`) |

Client đọc task 4179 (tuyến×10 + đảo chiều), 4180 (mốc xuất phát, giây máy chủ), 4178/4183 (đếm ngày), 3542–3545 (điểm bám xe) — tất cả đã đồng bộ qua `UI_TASKVALUE` (id ≥256) từ đợt F11.

---

## 7. CÒN MỞ / CẦN CHỦ QUYẾT

1. ~~Phần thưởng nhánh cá nhân~~ **ĐÃ CHỐT: như bản Linux** (`award.lua`).
2. **Giá cửa hàng Tiêu cục** (Hộ Tiêu Lệnh → đạo cụ) tự đặt 5/5/10, đọc qua `HD_CFG` khoá `VT_GIA_KMGT / VT_GIA_KBKT / VT_GIA_TXDV`; **tiền xe dẫn đường** `VT_TIEN_XE_CHINAM` 1000 lượng. Chưa khai trong từ điển web `cfgw_vietngu`.
3. `Hộ Tiêu Lễ Hộp` (4809) và 3 món mới (Thề Non Hẹn Biển, Địa Linh Đơn, Lệnh bài Cổ Tháp) trong `magicscript.txt` có `Script=0` → nhận được nhưng bấm không chạy.
4. ~~Ngưỡng cấp 120/150~~ **ĐÃ CHỐT: mọi ngưỡng = 90** (mục 1c).
5. **Biến bang 1149** vẫn chưa được công thành chiến ghi (như BANGIAO cũ) — test bằng menu "Đặt 1149".
6. Relay `bin\multiserver\script` không có `event\longmenbiaoju` — không cần vì JX1 chạy `RemoteExecute` tại chỗ; nếu sau này chia nhiều GameServer thì phải làm phần relay.
7. `lmbj_config.lua` (đã sửa mã) + `taskclass.lua` + `dialog.lua` là mã chết khi dùng `npc_canhan.lua`; gỡ hẳn hay không tuỳ chủ.

---

## 8. TỆP ĐÃ ĐỔI (mọi tệp cây chạy thật có bản `.truoc_vtcn_0609`)

Máy chủ (`bin\server`): `script\item\lenhbaiadmin.lua`, `script\item\test_vantieu_admin.lua` (mới), `script\event\lmbiaoche\vt_chinam.lua` (mới), `script\activitysys\config\129\npc_canhan.lua` (mới), `…\129\{variables,npc_consigner,extend,npc_lmbiaowu,npc_lmbiaobox}.lua`, `script\event\longmenbiaoju\{taskclass,lmbj_config,item,event}.lua`, `script\startgame\lmbj_addnpc.lua`, `script\lib\lib_lmbiaoche.lua`. Gương git: `serverscript_jx2\vantieu\moi\…` (+ `serverscript_jx2\chienlenh\script\item\lenhbaiadmin.lua`), kịch bản vá `ReverseTools\vantieu\va_vtcn_0609.py`, `va_vtcn_cpp_0609.py`.

Client (`bin\client`): `Ui\uitasklist.ini`, `Game.exe.moi`, `CoreClient.dll.moi`.

Kiểm: 15 tệp Lua qua `kiem_54.py` (Lua 5.4 thật) 0 lỗi; mọi tệp TCVN3 giữ nguyên số byte cao (ASCII-only patch) trừ `lenhbaiadmin.lua` (240→299, do menu mới).
