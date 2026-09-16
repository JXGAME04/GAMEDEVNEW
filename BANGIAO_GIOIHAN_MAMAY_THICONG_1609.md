# BÀN GIAO — THI CÔNG GIỚI HẠN MÃ MÁY (7 bước, phiên client PC 16/09/2026)

**Nhánh `claude/machine-code-limit-analysis-ac47d1` (worktree cùng tên), 7 commit `7a42ca6f` → `5cee3b55`, CHƯA gộp `main`, CHƯA push.**
**Bản dựng đã đặt theo quy ước `.moi` trong `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin`. CHƯA khởi động lại máy chủ.**
Phân tích gốc: `PHANTICH_GIOIHAN_MAMAY_PC_1609.md`.

---

## 1. Đã làm gì — từng bước, mỗi bước một commit, dựng và kiểm riêng

| Bước | Commit | Nội dung | Kiểm |
|---|---|---|---|
| **P1** client | `7a42ca6f` | UUID mẫu so **cả hai thứ tự byte** (thô + hiển thị), 4 mẫu hay gặp, 16 byte đều nhau, ≥ 8 byte 0 ⇒ loại. Log `uuid_smbios=<dạng wmic> [ok / LOAI:lý do]` | Harness `scratchpad/test_kmachineid.cpp` (include thẳng `.cpp`): **13/13 PASS**, mã máy dev **không đổi** `A27EADC9…`; log in `34B87C86-CAC0-0416-…` khớp `wmic csproduct get uuid`. Game.exe dựng 0 lỗi |
| **P5** relay | `3058fd50` | Đếm qua **mọi** kết nối GameServer (`CNetServer::ForEachConnect` trên `m_mapId2Connect`), int + kẹp 255, rTRACE khi kẹp | S3Relay dịch 0 lỗi C |
| **P3** GS+relay | `393bfa2b` | GS **hỏi sau khi ghi sổ**: hàng đợi `gs_aHoiTre` 512 chỗ, trễ 3 nhịp sau `c2s_entergame`, `m_dwParam = 0x80000000 \| khe` (layout gói không đổi); S3Relay đánh số `PARAMINFO.seq`, chỉ đếm mục cùng mã máy **ghi trước** người hỏi (tìm mục người hỏi theo serRegIndex/gsNumber > IP > bất kỳ), vọng lại `m_dwParam`; GS đối chiếu **khe** + mã máy trước khi đá; log khi link Tong chết | S3Relay exe sinh ra 0 lỗi; GameServer x64 `Build succeeded` |
| **P6** GS | `62b0ac09` | `BoQuaHangC` mặc định **0**; `MAYID_MAX_DONG` 64 → 256 | GameServer `Build succeeded` |
| **P2** client | `b2cf99d9` | Hạng B = UUID > **MAC** > ổ đĩa; MAC ưu tiên card **có gateway**, loại thêm `ndis/usb/tether`; ổ đĩa = **ổ hệ thống** (`IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS`), bỏ tháo rời/USB/SD/iSCSI/ảo; đệm IOCTL chừa NUL. Log `mac=… [n-card-that,co/khong-gateway] odia=[…]` | Harness PASS, mã dev không đổi; Game.exe 0 lỗi |
| **P7** client | `53b9dcef` | Bị `s2c_exitgame` ⇒ về màn hình đầu **và hiện hộp thoại** `UIMessageBox` (g_UiInformation sống qua UiEndGame) | Chuỗi TCVN3 +35 byte cao đúng số ký tự có dấu; Game.exe 0 lỗi. **Chưa thấy trên màn hình** (cần test sống) |
| **P8** client | `5cee3b55` | `KMachineId.cpp` rào `#ifdef JX_MOBILE` (dùng shim `GetCurrentHwProfile` của mobile); PC không đổi | Game.exe 0 lỗi. **Chưa dựng mobile** |

**P4 bỏ** (không cần): GameServer **không bao giờ tự nối lại** S3Relay (`CGameClient` không có reconnect, `KSOServer.cpp:299-306`
không làm gì khi kết nối) ⇒ không có sự kiện "nối lại" để gửi lại EnterGame. Hệ quả vận hành: **khởi động lại S3Relay thì
phải khởi động lại GameServer** — đã ghi vào `ChayS3Relay.bat`. Thay vào đó P3 ghi log `[MAYID] KHONG hoi duoc so phien`
(1/200 lần) khi link Tong chết, thay vì im lặng.

Mọi tệp TCVN3 (`KSOServer.cpp/.h`, `HostServer.cpp`, `TongConnect.cpp`, `GameSpaceChangedNotify.cpp`) được vá bằng kịch bản
Python latin-1 neo theo dòng, **kiểm số byte cao trước/sau** (không đổi, hoặc đổi đúng bằng khối chú thích/chuỗi chủ đích).

---

## 2. Bản dựng và vị trí

| Tệp | md5 (8 đầu) | Cỡ | Đã đặt tại | Áp bằng |
|---|---|---|---|---|
| `GameServer.exe` (Release x64) | `2a729f12` | 1 996 288 | `bin\server\GameServer.exe.moi` | `ChayGameServer.bat` (sẵn có) |
| `Game.exe` (Release Win32) | `1284f509` | 1 536 512 | `bin\client\Game.exe.moi` | `ChoiGame.bat` (sẵn có) |
| `S3Relay.exe` (Release Win32) | `cc1d4e5a` | 4 484 096 | `bin\multiserver\S3Relay.exe.moi` | **`ChayS3Relay.bat` (mới viết)** |

Chuỗi mốc đã kiểm có trong từng tệp: Game.exe `uuid_smbios=`, `LOAI:mau-so-%d`, `co-gateway`, `drive%d(he-thong)`;
GameServer.exe `[MAYID] hang doi hoi tre DAY`, `phien vao truoc, khe `, `KHONG hoi duoc so phien`; S3Relay.exe
`[MAYID] khong thay muc cua nguoi hoi`, `count limit: [%d] seq=%u`, `KEP ve 255`.

🔴 **Trong `bin\server` còn `CoreServer.dll.moi` (14/09 16:16) của phiên KHÁC** — `ChayGameServer.bat` sẽ áp nó cùng lúc.
Không phải của tôi; nếu không muốn ăn bản đó thì đổi tên nó đi trước khi chạy bat.

**Lùi:** mỗi bat giữ bản cũ thành `.truoc` (`GameServer.exe.truoc`, `Game.exe.truoc`, `S3Relay.exe.truoc`).

Dựng từ worktree cần chép thủ công các thư mục bị `.gitignore`: `Lib\release64`, `Lib\x64\Engine Server Release`,
`Lib\debug\libdb181sd.lib` (đã chép từ `D:\GAMEDEVNEW`). Build báo "FAILED" chỉ vì bước hậu dựng `md` (thư mục đã có),
exe vẫn sinh ra — đã đối chiếu `error C` = 0 và mốc thời gian tệp.

S3Relay.exe mới 4,48 MB, bản đang chạy 6,30 MB: cùng Release x86, cùng linker 14.44, cùng bộ DLL nhập, bản cũ có đủ chuỗi
mốc 15/09 ⇒ khác **chế độ liên kết** (bản cũ có `.idata/.tls`, không `.reloc`), không phải khác mã.

---

## 3. Cấu hình liên quan (`bin\server\GameServer_cfg.ini`, mục `[LimitLogin]`)

Hiện: `MaxLogin=2` (bản đang chạy vẫn nhớ ngưỡng 6 vì cfg sửa sau khi mở), **không có `ChiQuanSat`** ⇒ mặc định 1 = chỉ quan sát,
`BoQuaHangC` nay mặc định **0**. Ý nghĩa `num_login` mới: **số phiên cùng mã máy đã vào TRƯỚC** (không tính mình); điều kiện
đá `num_login >= MaxLogin` ⇒ `MaxLogin=2` cho đúng 2 phiên/máy.

---

## 4. Trạng thái máy chủ lúc bàn giao (11:15)

Máy chủ chạy từ 15/09 00:40 (bản cũ). Cổng client 6670 có 2 kết nối: `127.0.0.1` (Game.exe trên máy này, mở 9:24) và
`10.0.0.140` (cũng là máy này — giả lập). Sáng nay có điện thoại `10.0.0.127/.187` vào ra. **Tôi không tự khởi động lại**
vì chủ đang nối; chủ chọn lúc.

---

## 5. Kịch bản test sống (chạy khi chủ đồng ý khởi động lại)

Thứ tự bắt buộc: **S3Relay trước, GameServer sau** (GS không tự nối lại).

1. Tắt `S3Relay.exe` → chạy `bin\multiserver\ChayS3Relay.bat` (áp `.moi`, mở S3Relay). Kiểm log relay
   `bin\multiserver\relay_log\<ngày>\<giờ>.log` có dòng `host server startup`.
2. Tắt `GameServer.exe` (để nó tự lưu) → chạy `bin\server\ChayGameServer.bat`. Dòng đầu `GameServer.log` phải có
   `[MAYID] MaxLogin=2 ChiQuanSat=1 BoQuaHangC=0 TreNhipDong=2` (**BoQuaHangC=0** = bản mới).
3. Thoát Game.exe → chạy `bin\client\ChoiGame.bat` → đăng nhập. Kiểm:
   - `bin\client\jx_machineid.log` dòng cuối: `id=A27EADC94CFCBAD3CB54E1FA92D025ACE hang=A nguon_dung=uuid-bo-mach … uuid_smbios=34B87C86-CAC0-0416-A358-047C1642FA19 [ok] mac=… odia=[drive0(he-thong)]`
   - `GameServer.log`: `[MAYID] dang nhap: mamay [A27EAD…] hang A IP 127.0.0.1 khe N` rồi **sau khi vào game** (không còn ngay lúc đăng nhập):
     `[MAYID-DEM] mamay [A27EAD…] dang co 0 phien vao truoc, khe N (nguong 2)`
   - relay log: `Host:player login: … (…, 0000000N)` rồi `strHWID: [A27EAD…] count limit: [1] seq=K`.
   - **Không** được có `[MAYID] khong thay muc cua nguoi hoi` (nếu có: EnterGame tới sau câu hỏi — báo tôi, tăng `MAYID_NHIP_HOI_TRE`).
4. Mở client thứ 2 cùng máy (tài khoản khác): `[MAYID-DEM] … dang co 1 phien vao truoc` — chưa đá.
5. Mở client thứ 3: `dang co 2 phien vao truoc` ⇒ `[MAYID] Acc [..] MaMay [..] dang co 2 phien vao truoc, nguong 2 -> CHI QUAN SAT`.
6. **Thử đá thật:** thêm `ChiQuanSat=0` vào `[LimitLogin]`, khởi động lại GS (bước 2), lặp bước 3–5: client thứ 3 phải
   `-> DA RA`, client hiện hộp thoại *"Kết nối đã bị máy chủ đóng: tài khoản bị đưa ra khỏi trò chơi (…)"* rồi về màn hình đầu;
   relay log của tài khoản đó có `player logout`. Hai client đầu **không** bị ảnh hưởng.
7. **Thử cửa sổ đua:** thoát hết, mở 3 client gần như cùng lúc và đăng nhập nhanh: chỉ **2 người đầu ở lại** (`0`, `1` phiên vào
   trước), người thứ 3 bị đá. Trước bản này cả 3 đều vào.
8. Thoát ra vào lại nhiều lần: `[MAYID-DEM]` luôn về `0 phien vao truoc` khi chỉ còn một client (không mồ côi).
9. Điện thoại: mã vẫn dạng `{…}`, `hang {`, đếm/đá bình thường (mobile chưa dùng KMachineId).

Xong test: đặt lại `ChiQuanSat=1` nếu chưa muốn cưỡng chế, hoặc giữ 0 với `MaxLogin` đã chọn theo số liệu.

---

## 6. Còn lại / lưu ý

- **Gộp nhánh vào `main` và push** sau khi test sống OK (phiên khác đang giao bản từ `main` — bản `.moi` này chưa nằm trong `main`).
- Máy hạng B trước đây dùng **sê-ri ổ đĩa** (không có UUID) sẽ **đổi mã một lần** khi lên bản này (nay ưu tiên MAC); máy đã dùng MAC giữ nguyên mã.
- P8 chưa dựng trên mobile: khi gộp sang `mobile-0809` phải thêm `Sources/S3Client/Login/KMachineId.cpp` vào danh sách nguồn S3Client trong `android/CMakeLists.txt`, dựng thử.
- Không đụng `CoreServer.dll` (giao diện `CoreServerShell` giữ nguyên) — vì thế thông báo đá là chung, không kèm con số.
- Log quan sát cũ `[MAYID-DEM] … dang co N phien (nguong M)` đổi thành `… dang co N phien vao truoc, khe K (nguong M)`; công cụ đọc log (nếu có) cần theo.
- Đã bỏ hẳn câu hỏi ở `logiclogin`: mã máy vẫn được ghi log `[MAYID] dang nhap:` ngay lúc đăng nhập như cũ.
