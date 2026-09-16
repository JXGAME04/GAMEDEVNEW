# PHÂN TÍCH LẠI — GIỚI HẠN ĐĂNG NHẬP THEO MÃ MÁY (phiên client PC)

**16/09/2026 · chỉ phân tích, KHÔNG sửa mã · đọc trên `main` = `dfaffdcf` (nhánh `claude/machine-code-limit-analysis-ac47d1`)**
**Đối chiếu với `BANGIAO_GIOIHAN_DANGNHAP_MAMAY_1509.md`.**

---

## 0. Kết luận ngắn

Bản vá 14–15/09 sửa đúng các lỗi tràn bộ đệm, sập từ xa và mồ côi sổ sách; đọc lại mã thì các vá đó
đứng vững. Nhưng còn những chỗ có thể hỏng, trong đó **hai lỗi đủ nặng để tái phát đúng triệu chứng
"người vào được người không" ngay khi bật cưỡng chế**:

1. **Client PC — hằng số nhận diện UUID mẫu viết sai thứ tự byte.** Bo mạch OEM để UUID mặc định vẫn
   được coi là UUID thật, hạng **A**. Cả dàn máy cùng lô bo mạch = MỘT mã, và vì là hạng A nên không có
   đường nào miễn.
2. **Máy chủ — hỏi số phiên ở `logiclogin` nhưng ghi sổ ở `entergame`, trên hai socket khác nhau.**
   Mở N client cùng lúc thì mọi câu hỏi đều nhận số cũ, tất cả đều vào, ngưỡng vô nghĩa.

Kèm theo: đếm sót khi nhiều GameServer chung IP; mất đếm im lặng sau khi S3Relay khởi động lại; hạng C
vẫn được miễn theo cấu hình mặc định; client bị đá không thấy lý do; gộp sang nhánh mobile sẽ vỡ build.

---

## 1. Luồng thật (đã đọc mã, không lấy từ tài liệu)

```
Client PC                          GameServer (Heaven)                            S3Relay
JX_GetMachineId() ──┐
KLogin::Request ────┼─ c2s_login(sHWID) ───────► Bishop: BỎ QUA sHWID (LimitAccountFile là mã chết)
ConnectToGameSvr ───┘─ c2s_logiclogin(sHWID) ──► ProcessLoginProtocol (KSOServer.cpp:4064)
                                                  lọc ký tự → AttachPlayer → m_nPlayerHWID
                                                  HỎI: GET_LOGIN_LIMIT(HWID) ──[socket Tong 5005]──► TongConnect.cpp:498
                                                  ◄── RETURN_LOGIN_LIMIT(num_login) ──────────────── CountLoginByHWID
                                                  num_login >= MaxLogin → s2c_exitgame + đóng cứng sau 2 nhịp
                        … SendGameDataToClient + client đồng bộ: 1–5 giây …
                        enumPlayerSyncEnd → c2s_entergame(HWID) ──[socket Transfer 5003]──► HostConnect.cpp:74 GHI SỔ
                        thoát / hết ping / uỷ thác → c2s_leavegame(TÊN TÀI KHOẢN) ─[5003]──► HostConnect.cpp:160 XOÁ
```

Sự thật đã xác minh (khác hoặc bổ sung so với bàn giao):

- S3Relay chạy **đơn luồng xử lý gói**: `_WORKMODE_SINGLETHREAD` (`S3Relay.vcxproj:75`), `CSockThread::Main`
  gọi `g_HostServer.Route()` → `g_TongServer.Route()` → `g_ChatServer.Route()` tuần tự (`SockThread.cpp:60-66`).
  Vì vậy các khoá `DUMMY_AUTOLOCK*` = 0 (`Global.h:25-29`) **hôm nay không gây tranh chấp**.
- Bishop **từ chối** đăng nhập trùng tài khoản (`E_ACCOUNT_EXIST` → `LOGIN_R_ACCOUNT_EXIST`,
  `Bishop/GamePlayer.cpp:944-953`), không đá phiên cũ.
- Ba đường GS → S3Relay là **ba socket riêng**: Transfer 5003, Chat 5004, Tong 5005 (`KSOServer.cpp:529-534`).
- Mã máy mobile (nhánh `mobile-0809`) vẫn là shim `GetCurrentHwProfile` băm hostname + thư mục dữ liệu,
  dạng `{%08lX-4E44-4A58-B1B1-%012lX}` (`Sources/Engine/Src/Platform/KPosixWin32.cpp:1019-1022`), bắt đầu bằng `{`.

---

## 2. Lỗi phía CLIENT PC (`Sources/S3Client/Login/KMachineId.cpp`)

### 2.1 🔴 Hằng số UUID mẫu sai thứ tự byte → bo mạch dùng UUID mặc định vẫn ra CÙNG mã, hạng A

- **Vị trí:** `KMachineId.cpp:190-204`. Mã so 16 byte thô với
  `03 00 02 00 | 04 00 | 05 00 | 00 06 | 00 07 00 08 00 09`.
- **Vì sao sai:** chuỗi Windows hiển thị `03000200-0400-0500-0006-000700080009` là **sau khi đảo little-endian
  ba trường đầu**. Byte thô nằm trong bảng SMBIOS là `00 02 00 03 | 00 04 | 00 05 | 00 06 | 00 07 00 08 00 09`
  (dãy tăng dần — đúng thứ firmware để mặc định). Hằng số trong mã chép theo thứ tự **hiển thị**, nên `memcmp`
  **không bao giờ khớp**.
- **Bằng chứng đo trên chính máy này (chỉ đọc WMI):**
  raw `86 7C B8 34 | C0 CA | 16 04 | A3 58 …` ↔ WMI hiển thị `34B87C86-CAC0-0416-A358-…`.
- **Hậu quả:** UUID mẫu đi qua như UUID thật → `nManh = 3` → hạng **A**. Phòng máy dùng bo mạch cùng lô để
  UUID mặc định (rất phổ biến ở bo OEM giá rẻ) ⇒ cả phòng chung một mã; `BoQuaHangC` không che vì là hạng A.
  Đây chính là triệu chứng ban đầu, chỉ đổi từ "GUID registry trùng" sang "UUID mẫu trùng".
- **Còn thiếu** các mẫu hay gặp khác (`00020003-0004-0005-0006-000700080009`,
  `12345678-1234-5678-90AB-CDDEEFAABBCC`, …); chỉ có toàn-0 và toàn-FF được loại.
- **Kiểm nhanh không cần sửa mã:** trên máy nghi ngờ chạy `wmic csproduct get uuid`; nếu ra `03000200-…`
  mà `jx_machineid.log` vẫn ghi `hang=A nguon_dung=uuid-bo-mach` thì đã xác nhận.

### 2.2 🟠 Hạng B ưu tiên sê-ri ổ đĩa trước MAC → phòng máy không ổ cứng (diskless) chung mã

- `_LaySeriODia` mở `\\.\PhysicalDrive0..3`, lấy sê-ri đầu tiên "có nghĩa" (`KMachineId.cpp:236-293`).
  Phòng net diskless (iSCSI/CCBoot/iCafe): PhysicalDrive0 là đĩa ảo từ máy chủ boot, sê-ri thường giống nhau
  hoặc rỗng — trong khi **MAC** mới là thứ máy chủ boot dùng để phân biệt từng ghế.
- Không loại thiết bị tháo rời: `STORAGE_DEVICE_DESCRIPTOR` có `RemovableMedia` / `BusType` nhưng không được
  xem; USB/đầu đọc thẻ đứng số 0 (tuỳ BIOS) làm mã đổi theo việc cắm rút.
- Chỉ chạm tới khi UUID không có; nhưng đây là đường lui của bo mạch không UUID và Windows XP, vẫn đáng sửa.

### 2.3 🟠 Nguồn MAC vẫn phụ thuộc TẬP card mạng — trái với khẳng định "chỉ đổi khi nguồn biến mất"

- `_LayMac` lấy MAC **nhỏ nhất** trong mọi card vật lý đang bật và có IPv4 (`KMachineId.cpp:337-386`).
  Cắm USB-NIC, bật Wi-Fi, chia sẻ mạng từ điện thoại ("Remote NDIS based Internet Sharing Device" **không**
  nằm trong danh sách card ảo) ⇒ có thể xuất hiện MAC nhỏ hơn ⇒ mã đổi dù không nguồn nào "biến mất".
  `GetAdaptersInfo` chỉ liệt kê card đang bật, nên tắt/bật card cũng đổi tập.
- `_LaCardAo` thiếu `ndis`, `usb`, `tether`; mục `ppp` khớp mọi mô tả có chứa chuỗi này.

### 2.4 🟡 Các điểm nhỏ

- `strncpy(szOut, byBuf + SerialNumberOffset, nOut - 1)` với `nOut = 256`: nếu offset lớn và không có NUL
  thì đọc quá đuôi `byBuf[1024]` (`:278`). Thực tế offset < 100, rủi ro thấp.
- `jx_machineid.log` ghi **nối** mỗi lần mở game, lộ nguồn đang dùng cho người muốn lách; thư mục chỉ đọc thì
  im lặng (`:425-434`).
- Tính lần đầu **trên luồng giao diện** ngay trong `KLogin::Request`; `GetAdaptersInfo` có thể treo 1–3 s trên
  máy có VPN/card lỗi ⇒ khựng một lần khi bấm đăng nhập.
- Tính một lần cho cả tiến trình: nguồn hỏng tạm thời lúc đăng nhập đầu ⇒ giữ hạng thấp tới khi tắt game;
  cùng một máy có thể xuất hiện với hai mã ở hai lần chạy (nhiễu số liệu quan sát, không lách được).
- Mở `\\.\PhysicalDriveN` là hành vi AV / phần mềm quản lý phòng máy hay chặn ⇒ âm thầm rơi xuống MAC.

### 2.5 ℹ️ Client bị đá KHÔNG thấy lý do

- `s2c_exitgame` → `GDCNI_S2C_EXIT_GAME` → về màn hình khởi đầu, **không hộp thoại**
  (`Ui/GameSpaceChangedNotify.cpp:564-571`, `Core/Src/KProtocolProcess.cpp:905-908`). Gói này cũng dùng cho
  lệnh đá của GM (`ScriptFuns.cpp:10037`) nên client không phân biệt được lý do. "Đóng trễ 2 nhịp" chỉ bảo đảm
  gói tới nơi, không bảo đảm người chơi hiểu.

### 2.6 ℹ️ Bẫy gộp sang nhánh mobile

- `Login.cpp` / `NetConnectAgent.cpp` trên `main` gọi `JX_GetMachineId()` không rào; `KMachineId.cpp` chỉ
  Windows (`windows.h`, `winioctl.h`, `iphlpapi`, `__try`). Gộp `main` → `mobile-0809` sẽ **vỡ build Android/iOS**
  nếu không có bản POSIX. Các commit MAYID hiện chỉ nằm trên `main`.

---

## 3. Lỗi phía MÁY CHỦ

### 3.1 🔴 Cửa sổ đua: hỏi lúc `logiclogin`, ghi sổ lúc `entergame`, trên hai socket khác nhau

- Hỏi: `KSOServer.cpp:4176-4190` (socket Tong). Ghi: `KSOServer.cpp:3076-3101` gửi `tagEnterGame2` qua socket
  Transfer → `HostConnect.cpp:74-158`. Giữa hai mốc là `SendGameDataToClient` + client đồng bộ (1–5 giây).
- Mở N client cùng lúc (công cụ multi-open + tự đăng nhập, mỗi client một tài khoản nên Bishop không chặn):
  mọi câu hỏi đều nhận `num_login` cũ ⇒ **tất cả vào**, bất kể `MaxLogin`. Không có bước đếm lại định kỳ;
  quyết định chỉ xảy ra đúng một lần khi trả lời về.
- Vì hai socket khác nhau, ngay cả "hỏi sau khi ghi" cũng **không chắc** S3Relay xử lý EnterGame trước câu hỏi
  (Route Host trước Tong trong một vòng, nhưng gói nào tới trước tuỳ mạng).

### 3.2 🟠 Mất giới hạn im lặng khi S3Relay khởi động lại hoặc link Tong rớt

- `if (m_pTongClient)` (`:4190`): link Tong hỏng ⇒ không hỏi ⇒ không giới hạn, không log.
- S3Relay khởi động lại ⇒ `CHostConnect` là đối tượng mới với ba bảng rỗng; GameServer **không gửi lại**
  EnterGame cho người đang online (`TongClientEventNotify` không làm gì khi kết nối, `KSOServer.cpp:299-306`)
  ⇒ mọi phiên đang chơi không được đếm cho tới khi họ đăng nhập lại. Nếu link không tự nối lại thì mất luôn
  bang hội tới khi khởi động lại GS — cả hai đều là mất giới hạn im lặng.
- Hệ quả phụ: số `[MAYID-DEM]` sau đó **thấp giả tạo** ⇒ chọn ngưỡng sai.

### 3.3 🟠 Nhiều GameServer chung một IP ⇒ chỉ đếm được một (nặng ngang 3.1 nếu đúng cấu hình đó)

- `m_mapIp2Connect[ip] = pConn` ghi đè khi kết nối, `erase(ip)` khi đóng (`HostServer.cpp:51-63`);
  `CountLoginByHWID` duyệt bảng này (`HostServer.cpp:138-152`). Hai GS cùng máy ⇒ chỉ GS kết nối sau được đếm;
  khi GS "ẩn" đóng, nó **xoá luôn** mục của GS đang hiện ⇒ IP đó không còn GS nào được đếm.
- **Chưa xác minh** số GS thực tế (cây này không có `*_cfg.ini`); bàn giao nhắc `GameServer1_cfg.ini` tồn tại
  nên khả năng ≥ 2 GS là thật. Cách kiểm: đếm tiến trình `GameServer.exe` và dòng `host connect create` trong
  log S3Relay.

### 3.4 🟡 Mục cũ của CHÍNH tài khoản mình bị đếm vào ngưỡng

- Dọn rác ở EnterGame (`HostConnect.cpp:123-143`) chạy **sau** câu hỏi ở logiclogin. Nếu S3Relay còn mục cũ
  cùng tài khoản, `num_login` gồm cả nó ⇒ với `MaxLogin=1` người đó bị đá khi đăng nhập lại.
- Hiếm: Bishop chặn đăng nhập trùng tới khi có LeaveGame gateway, mà LeaveGame gateway và LeaveGame relay
  được gửi cùng khối (`KSOServer.cpp:3873-3931`) nên thứ tự nhân quả bảo vệ. Ngoại lệ là nhánh **uỷ thác**:
  gửi LeaveGame cho relay mà **không** gửi gateway (`KSOServer.cpp:3941-3954`). Tự hết nếu làm P3.

### 3.5 🟡 Bộ đếm là BYTE

- `num_login` BYTE (`Headers/KTongProtocol.h:768`); `CountLoginByHWID` cộng BYTE qua từng GS
  (`HostServer.cpp:138-152`). 256 phiên cùng mã (mã dùng chung do 2.1, `KHONG-KHAI-MA-MAY`, trại bot lúc quan sát)
  ⇒ quay về 0 = qua ngưỡng; log quan sát trên 255 sai.

### 3.6 🟡 Đối chiếu trả lời chỉ bằng mã máy

- `bKhopMaMay` (`KSOServer.cpp:1903`): khe `nIdx` được cấp lại cho **phiên khác cùng máy** (tài khoản thứ hai
  của cùng người) thì trả lời trễ đá nhầm phiên đó, log ghi sai tài khoản. `m_dwParam` của gói hỏi đang cố định = 1,
  có thể mang số thứ tự đăng nhập — nhưng S3Relay hiện **không vọng lại** `m_dwParam` (`TongConnect.cpp:501-507`).

### 3.7 🟡 LeaveGame chỉ mang tên tài khoản

- `tagLeaveGame2` không có `lnID` (`KProtocol.h:1694-1699`). LeaveGame trễ của phiên cũ tới sau EnterGame của
  phiên mới cùng tài khoản, cùng GS ⇒ xoá phiên mới (không đếm + mất định tuyến bạn bè/chat, `SomeoneLogout`).
  Hiện được bảo vệ gián tiếp như 3.4; khẳng định "nhất quán với MỌI thứ tự sự kiện" trong bàn giao là quá lời.

### 3.8 ℹ️ Cấu hình và mã chết

- `BoQuaHangC=1` mặc định vẫn **miễn** mọi mã bắt đầu bằng `C`, kể cả `C000…` mà client giả gửi — trái với
  kết luận "hạng không phải giấy miễn" (`KSOServer.cpp:1918-1919`).
- Mỗi GameServer đọc `[LimitLogin]` từ cfg riêng ⇒ ngưỡng/chế độ có thể lệch giữa các GS.
- `MAYID_MAX_DONG = 64`: hàng đợi đóng cứng đầy thì chỉ còn đá mềm; client sửa đổi bỏ qua `s2c_exitgame`
  ở lại (`KSOServer.cpp:3443-3444`).
- Bishop `LimitAccountFile` / `GetCountHWID_Login` và bản trong GameServer **không ai gọi** (mã chết);
  `sHWID` trong gói đăng nhập tài khoản bị Bishop bỏ qua ⇒ chỉ có một lớp đếm duy nhất ở S3Relay.

---

## 4. Đối chiếu các khẳng định trong bàn giao 15/09

| Khẳng định | Đọc lại mã |
|---|---|
| Ba bảng S3Relay không khoá ⇒ tranh chấp dữ liệu | **Không xảy ra hôm nay**: đơn luồng (§1). Chỉ nguy hiểm nếu ai bỏ `_WORKMODE_SINGLETHREAD`. |
| Sổ sách tự nhất quán với MỌI thứ tự sự kiện | **Quá lời**: LeaveGame chỉ có tên tài khoản (3.7), dọn rác chạy sau câu hỏi (3.4). |
| Mã chỉ đổi khi chính nguồn đã chọn biến mất | **Đúng với UUID, sai với MAC** (2.3), một phần với ổ đĩa (2.2). |
| Rớt mạng rồi vào lại trước 60 s là kịch bản gây mồ côi hay gặp nhất | Bishop từ chối đăng nhập trùng tới khi có LeaveGame gateway ⇒ **khó xảy ra như mô tả**; đường uỷ thác mới là đường thật. |
| Hạng không phải giấy miễn | Mã máy chủ **vẫn miễn hạng C** theo mặc định (3.8). |
| Các vá tràn bộ đệm, `strcpy_s`, NULL, `dataLength`, `&sHWID[0]` | **Đúng**, đứng vững khi đọc lại. |
| Hai client cùng máy ra cùng mã, thoát thì đếm về 0 | Đúng với một GS, một S3Relay, không mở đồng thời — chưa phủ 3.1 / 3.3. |

---

## 5. Phương án (chưa làm gì, chờ chủ chọn)

### P1 — Client PC: sửa nhận diện UUID mẫu (2.1) · nhỏ, chỉ client
- So cả **hai thứ tự byte** của mẫu `0002 0003 …`, bổ sung các mẫu hay gặp; thêm luật entropy:
  ≥ 8/16 byte bằng 0 ⇒ coi là mẫu (UUID thật sinh ngẫu nhiên gần như không bao giờ có 8 byte 0; UUID
  Gigabyte/ASRock "điền dở" lấy từ MAC vẫn qua được).
- UUID bị loại ⇒ rơi xuống hạng B như thiết kế, nên làm cùng P2.

### P2 — Client PC: đổi ưu tiên hạng B thành UUID > MAC > ổ đĩa, chọn MAC ổn định (2.2, 2.3)
- MAC: ưu tiên card có **gateway mặc định** (`IP_ADAPTER_INFO.GatewayList`), loại thêm `ndis` / `usb` / `tether`.
- Ổ đĩa: chỉ lấy **đĩa hệ thống** (`IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS` của ổ chứa Windows), bỏ
  `RemovableMedia` / `BusTypeUsb`.
- Đánh đổi: MAC dễ đặt tay hơn sê-ri đĩa; nhưng cả ba đều do client khai nên không đổi bản chất.

### P3 — Máy chủ: khép cửa sổ đua (3.1 + 3.4) · GameServer + S3Relay, KHÔNG đổi gói
- Hỏi **sau** khi ghi sổ: xếp câu hỏi vào hàng đợi trễ 2–3 nhịp sau `c2s_entergame` (tái dùng cơ chế
  `gs_aDongTre`), so `num_login > MaxLogin` (đã gồm chính mình). Dọn rác cùng tài khoản đã chạy ⇒ 3.4 tự hết.
- Để không đá **cả hai** khi hai phiên vào cùng lúc: S3Relay thêm số thứ tự ghi vào `PARAMINFO` (nội bộ) và chỉ
  đếm mục ghi **trước** mục của người hỏi ⇒ "N người đầu ở lại", kết quả xác định. S3Relay cần biết mục của
  người hỏi: dùng `m_dwParam` (đang cố định 1) mang `lnID` — trường có sẵn, **không đổi layout gói**.
- Nếu chủ chấp nhận đổi giao thức (Gate 2): S3Relay trả lời ngay trong EnterGame bằng một gói mới là sạch nhất.

### P4 — Máy chủ: chống mất đếm im lặng (3.2)
- Khi link Tong `enumServerConnectCreate`: gửi lại `tagEnterGame2` cho mọi khe `enumPlayerPlaying`
  (S3Relay dọn trùng theo tài khoản nên gửi lại an toàn). Ghi log cảnh báo khi không gửi được câu hỏi.

### P5 — S3Relay: đếm qua MỌI kết nối GS (3.3)
- `CountLoginByHWID` duyệt `m_mapId2Connect` (như `CNetServer::Route`) thay vì `m_mapIp2Connect`,
  hoặc khoá theo (IP, id). Xác minh số GS thực tế trước.

### P6 — Cấu hình khi bật cưỡng chế (3.5, 3.8)
- `BoQuaHangC=0`; thống nhất `[LimitLogin]` giữa các cfg; kẹp `num_login` ở 255 trong S3Relay; cân nhắc nâng
  `MAYID_MAX_DONG`.

### P7 — Client PC: báo lý do khi bị đá (2.5)
- Máy chủ gửi một `KSystemMessage` qua đường `s2c_msg` sẵn có ("Máy này đã có N tài khoản đang chơi") ngay
  trước `s2c_exitgame`; client hiện tại tự hiển thị được, không cần đổi gói.

### P8 — Nhánh mobile (2.6)
- Trước khi gộp: rào `#ifdef JX_MOBILE` để dùng shim cũ, hoặc viết `JX_GetMachineId` bản POSIX. Ngoài phạm vi
  phiên PC, chỉ ghi nhận.

**Đề xuất thứ tự:** P1 + P3 + P5 (xác minh trước) **trước khi** đặt `ChiQuanSat=0`; P4 + P6 cùng đợt; P2, P7 sau.

---

## 6. Chưa xác minh được

- Số GameServer đang chạy và có chung IP với nhau không (không có cfg trong cây nguồn).
- Mẫu UUID thực tế của bo mạch ở các phòng máy đang gặp lỗi (cần `wmic csproduct get uuid` +
  `jx_machineid.log` của vài máy).
- Link Tong/Transfer của GameServer có tự nối lại sau khi S3Relay khởi động lại hay không.
- Bản mobile: "ba máy đo cho ba mã khác nhau" là quan sát của phiên trước, tôi không đo lại.
