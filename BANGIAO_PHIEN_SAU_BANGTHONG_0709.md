# BÀN GIAO CHO PHIÊN SAU — Băng thông đông người + đường gửi Heaven + hai lỗi client (07/09/2026, 19:30)

Tệp này viết cho một phiên **không có ngữ cảnh gì**. Đọc hết mục 1 → 4 trước khi gõ dòng lệnh đầu tiên.
Chi tiết số liệu từng đợt nằm ở `D:\GAMEDEVNEW\BANGIAO_BANGTHONG_DONGNGUOI_0609.md` mục 6 → 8.22 (mục này gọi tắt là **BG-GỐC**).
Phiên 19:00–19:30 đã làm xong **4.3 bước 1 + 2** (đường gửi Heaven.dll) và ghi ở BG-GỐC 8.22; bản này cập nhật theo đó.

---

## 1. Trạng thái lúc bàn giao (19:30)

### 1.1 Bản đang chạy thật

| Tệp | Băm | Lúc | Nội dung |
|---|---|---|---|
| `bin\server\CoreServer.dll` | **7361e2dd** | 18:05 | bản XEPHANG build từ origin/main sau 02eb802d = **⊇ DELTA a→l** + LNCK + TUKICH + TRANPHAI60 + BHLV/BHWS/BHMAP + XEPHANG (đã kiểm đủ chuỗi NS-BO/NS-TT/PS-BO/DMG-GON/BC-DEM/BC-LOAI) |
| `bin\server\GameServer.exe` | d07555ad | 18:05 | XEPHANG (có xả mỗi tick của DELTA b) |
| `bin\server\heaven.dll` | 096fdeb2 | 00:50 | **cũ** (chưa có [GUI]) |
| `bin\client\CoreClient.dll` | **43ba6ef9** | 17:48 | DELTA a→m (vá m = 3 nhãn nằm bẹp) |
| `bin\multiserver\Goddess.exe` | 73f10c62 | 01/09 | **cũ**, bảng xếp hạng còn rỗng tới khi đổi tay |
| `bin\multiserver\S3Relay.exe` | 99cf217d | 14:14 | cũ |
| `bin\client\Represent3.dll` | | 04/09 12:17 | có [REP3 03/09 SOC] |

### 1.2 Khe `.moi` đang chờ chủ swap

| Khe | Băm | Của ai | Ghi chú |
|---|---|---|---|
| `bin\server\heaven.dll.moi` | **9fc84e88** | phiên này ([GUI]) | `ChayGameServer.bat` tự đổi (`call :capnhat heaven.dll` đã có sẵn). Không cần đổi GameServer.exe |
| `bin\multiserver\Goddess.exe.moi` | 2ffeb6e4 | phiên XEPHANG | **đổi tay** (bat không quản `multiserver`), xem `BANGIAO_XEPHANG_0709.md` mục 1 |
| `bin\multiserver\S3Relay.exe.moi` | 18de36e5 | phiên BH100 | (nếu còn) swap cùng CoreServer có BHLV |

Khe `CoreServer.dll.moi` và `CoreClient.dll.moi` **đang trống**.

### 1.3 Cấu hình

- `bin\client\config.ini` `[Client] Rep3CacheMB=1500` (17:45, bản lưu `config.ini.truoc_rep3cache`) — **đã có hiệu lực**, xem 4.1.
- `bin\server\config.ini` **nay có mục `[Server]`** (19:15, bản lưu `config.ini.truoc_gui`) với đúng hai khoá `GuiKhoaRieng=1`,
  `MoPhongNhanBan=0`; mọi khoá DELTA khác vẫn chạy mặc định trong mã.

### 1.4 Git

Nhánh `delta-0709` trong worktree `D:\GAMEDEVNEW_wt_delta` = `origin/main` = **9538e72c**.
Cây chạy thật là `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin`, **không phải** nơi build.

---

## 2. Đường lối chủ đã chốt (đọc kỹ, đây là ràng buộc)

1. **Không giới hạn phương án.** Bản test chỉ mình chủ chơi, được đổi giao thức, không phải giữ tương thích với client cũ ngoài đời.
2. **Không hy sinh trải nghiệm.** Cấm cắt gói, cấm giảm tầm nhìn, cấm bỏ số sát thương, cấm hạ tần suất đồng bộ để lấy băng thông.
   Giảm byte cho **cùng một lượng thông tin**, không phải gửi ít thông tin hơn.
3. **Không hiểu thì hỏi chủ**, đừng đoán rồi vá bừa. Chỗ đụng luật chơi thì bắt buộc hỏi.
4. **Đo trước, sửa sau.** Mỗi lần sửa phải kèm bộ đếm để lần kéo log sau đọc là biết đúng hay sai.

---

## 3. Đã làm gì và kết quả đo

### 3.1 Chuỗi DELTA a → m (Core, băng thông tới client) — xem BG-GỐC 8.1–8.21

Nguyên tắc: gói đồng bộ cũ gửi **toàn bộ** trạng thái mỗi lần dù chỉ một trường đổi → **băm nội dung, chỉ gửi khi đổi thật**, tách
phần đổi nhanh sang gói gọn riêng. Ba mã gói mới 221 (`NPC_POS_SYNC` 28 B, ô 156), 222 (`DAMAGESHOW_GON` 13 B, ô 157), 223
(`NPC_STATE_SYNC` 39 B, ô 158) + hello `c2s_deltahello = 175` (ô 110 máy chủ) v1/v2/v3; máy chủ chỉ phát mã mới khi **mọi** client
đã báo phiên bản đủ (`NS_SoClientCu*` trong `KNpc.cpp`). Kết quả vá l: quyết định phát gói đầy đủ −85 %, gói 77 biến mất khỏi bảng byte;
thành phần byte tới client: 221 gọn 61 % · 75 ngoại hình 12 % · 222 số sát thương 8,5 % · 148 chiêu 5 % · 223 trạng thái 5 % · 86 lệnh chạy 4 %.

### 3.2 [GUI] Heaven.dll — đường gửi cho 500 người thật (phiên này, commit 9538e72c, BG-GỐC 8.22)

Gốc (BG-GỐC 8.20): hôm nay chỉ có **một** kết nối thật (1.000 bot chạy trong lòng máy chủ, `m_nNetConnectIdx = -1`), nên đường gửi
chưa bao giờ bị thử. Ngoại suy 500 người cùng chiến trường: ~83.000 lần `PackDataToClient` trong một nhịp 55 ms, mỗi lần lấy **hai**
khoá (`m_csCM` chung toàn bảng client rồi `csWriteAction` riêng); `SendPackToClient(-1)` giữ `m_csCM` suốt vòng duyệt 500 client; luồng
IOCP nhận gói cũng phải lấy `m_csCM`. Đó là chỗ vỡ đầu tiên, trước cả băng thông.

Đã làm (chỉ `Sources/MultiServer/Heaven/ServerStage.cpp/.h`, script tái áp `ReverseTools/goi_va_gui1_khoarieng_nhanban_0709.py`):

| # | Việc | Ghi chú |
|---|---|---|
| 1 | Bảng node cố định `m_ppNode[]` cấp trong ctor | tra node không cần khoá; cùng con trỏ với `m_theClientManager` |
| 2 | Đường gửi chỉ giữ `csWriteAction`, đường nhận chỉ giữ `csReadAction` | `pSocket` đọc **và dùng** dưới khoá riêng |
| 3 | Nơi đặt/tha `pSocket` lấy `m_csCM` + **cả hai** khoá riêng | `_HelperAddClient` (khoá mã trước, `pSocket` sau cùng), `_HelperDelClient`, `ShutdownClient`. Thứ tự khoá toàn cục `m_csCM < csWriteAction < csReadAction`, không đảo chiều |
| 4 | `SendPackToClient(-1)`: chụp danh sách client dưới `m_csCM` (vài µs) rồi xả từng client | chỉ `Allocate()` khi có dữ liệu (trước: cấp + tha cho mọi client mỗi nhịp dù rỗng) |
| 5 | Bộ đếm `[GUI-DO]` mỗi 10 s → `bin\server\jx_gui_server.log` | mở-ghi-đóng từng dòng, không giữ handle; ~500 B/10 s |
| 6 | Phép thử nhân bản `[Server] MoPhongNhanBan=N` | **đọc lại mỗi 10 s, không cần khởi động lại**; mỗi lần gửi thật làm thêm N−1 lần y hệt vào node giả (tra node, khoá, memcpy; đệm đầy thì Allocate + đầu gói + mã hoá KSG + Release), chỉ **không WSASend**; `[GUI-NB]` in tổng ước tính mỗi nhịp |
| 7 | Cổng lùi `[Server] GuiKhoaRieng=0` (đọc lúc khởi động) | lấy lại `m_csCM` trước khoá riêng trên mọi đường nóng, đúng thứ tự cũ |

Số nền ngay trước swap (19:22, Tống Kim trận 500, chủ đứng chỗ thưa): TICK 9,17 ms TB / max 24,7 / p95 12 (16,5 %); BroadCast
2,25–2,42 triệu/10 s; gửi thật 266–2.760/10 s; client 2–6 KB/s.

---

## 4. Việc đang treo — làm theo thứ tự này

### 4.1 Rep3CacheMB=1500 — trần đã hết kẹp, chờ chủ xác nhận hết mất hiệu ứng

`[REP3] cache texture ... ngan sach 1500 MB` từ 17:48 và 18:58. Từ 18:58 → 19:21 cache texture 238 → 402 MB, **đỉnh 685 MB**
(trước kẹt 508–511/512) trong Tống Kim. Số mục bỏ ~31/s đều (`bo` là **cộng dồn**).

Cơ chế thật trong nguồn hiện tại (`TextureResMgr::CheckBalance`, [REP3 03/09 SOC], Represent3.dll live 04/09 12:17 build sau commit
695db480 03/09): chỉ **một** chế độ — mỗi lượt bỏ đúng **1 khung** của **1** tài nguyên nghỉ > 10 s, nhịp `m_uCheckPoint` = 25 ms;
khi **vượt ngân sách** thì ngoài nhịp định kỳ, **mỗi lần nạp ảnh** gọi thêm CheckBalance (dòng 457). Tại trần, hiệu ứng nghỉ > 10 s bị
bào khung nhanh gấp bội rồi nạp lại không kịp. (Mô tả "8 khung / nghỉ 1 s" ở BG-GỐC 8.21 là chế độ `bOver` **cũ** đã bị bỏ 03/09.)

Kiểm khi chủ báo: nếu **vẫn** mất hiệu ứng mà cache không ở trần → bệnh chỗ khác: đo `LoadImage FAIL` trong `jx_rep3.log` và luồng nạp.
Câu hỏi 3 cho chủ (nâng kẹp 512 trong mã theo VRAM trống) vẫn chờ.

### 4.2 Nghiệm thu vá m (nằm bẹp) — chưa có lần chết nào sau swap 17:48

```bash
grep -a "S7-SAUHOISINH\|S7-NAMBEP" "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/jx_auto.log" | tail -30
```

Mỗi lần chết phải có đúng 3 dòng `[S7-SAUHOISINH]` ở +1 s, +3 s, +6 s. Đọc theo bảng:

| Dòng +3 s cho thấy | Nghĩa là | Hướng sửa |
|---|---|---|
| `doing=1 cdoing=1 resdoing=1` | đứng bình thường | không phải lần này, chờ lần sau |
| `doing=10` (chết) hoặc `cdoing=8` | **logic client** kẹt ở trạng thái chết | tìm ai đặt lại; nếu có `[S7-NAMBEP-CHAN]` thì gói đồng bộ đầy đủ là thủ phạm và vá j đã chặn đúng |
| `doing=1 cdoing=1` nhưng `resdoing=8` hoặc `resaction=36` | **lớp vẽ** không đổi tư thế | soi `KNpcRes::SetAction`; nghi ảnh SPR tư thế đứng không nạp được → thêm nhãn in tên tệp SPR |
| có `[S7-NAMBEP-LAU]` | kẹt trên 10 giây, chắc chắn là bệnh | đọc ba trường `doing/cdoing/resdoing` trong dòng đó |

`do_death = 10`, `do_revive = 21`, `do_stand = 1`; `cdo_stand = 1`, `cdo_death = 8`.

### 4.3 Đo đường gửi sau khi swap `heaven.dll.moi` (việc chính kế tiếp)

```bash
grep -a "GUI-DO\|GUI-NB" "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/jx_gui_server.log" | tail -6
```

1. Dòng khởi động phải có `GuiKhoaRieng=1`. Khi chủ đánh, ghi lại `moi nhip: goi X ms (max) + xa Y ms (max)` = chi phí đường gửi thật hôm
   nay (dự kiến < 0,2 ms/nhịp) và `Write N lan, X us/lan` (chỉ là PostQueuedCompletionStatus, WSASend chạy ở luồng IOCP).
2. Lúc chủ đang ở đám đông, sửa `MoPhongNhanBan=500` trong `bin\server\config.ini`, chờ 10–20 s, đọc `[GUI-NB]`, lấy
   **`duong gui uoc tinh moi nhip`** và `max`. Xong đặt lại `0`. Đọc theo bảng:

| `duong gui uoc tinh moi nhip` | Nghĩa | Bước tiếp |
|---|---|---|
| < 15 ms (cộng TICK 9 ms vẫn < 55) | luồng chính gánh được 500 người | không cần luồng gửi riêng; còn lại là băng thông ra 80–240 Mbps (máy 1 Gbps thì đủ) |
| 15–30 ms | sát ngưỡng khi có đỉnh | đưa phần **xả** (Allocate + mã hoá + Write) sang luồng riêng, phần `goi` (memcpy) giữ luồng chính |
| > 30 ms hoặc `max` > 40 | vượt | luồng gửi riêng **và** hỏi chủ về thưa đồng bộ vị trí theo khoảng cách (bước 3 dưới) |

3. Kiểm hồi quy khoá riêng: client vào/ra vài lần; `grep -a -c "Net Msg Error" jx_auto.log` = 0; `jx_crash.log` không sập; console
   GameServer không in `Socket is closed` / `Unexpected exception`. Nghi ngờ → `GuiKhoaRieng=0` + khởi động lại là về hành vi cũ, không
   cần đổi nhị phân.
4. **Thưa theo khoảng cách khi vùng đông** (quá 16 ô thì hạ nhịp đồng bộ vị trí còn một nửa, chỉ bật khi đông): gói 221 chiếm 61 % byte,
   là chỗ duy nhất còn nhiều mỡ. **Đụng trải nghiệm → phải hỏi chủ trước, chưa làm.**

Giới hạn của phép thử: chỉ đo luồng chính. Chi phí luồng IOCP (WSASend thật, tranh chấp khoá từ 500 client gửi lên) không mô phỏng được;
nhưng khoá chung đã bỏ nên tranh chấp ấy chỉ còn ở khoá riêng từng client.

### 4.4 Tách map chiến trường ra một nhân CPU riêng — chủ hỏi 17:50, trả lời: không an toàn

Vòng chính (`KSubWorldSet::MainLoop`) duyệt tuần tự từng map trong một luồng; mã có ghi chú lần trước **đã bỏ** vòng đa luồng vì
`SubWorld[i].Activate()` dùng chung `Npc[]`/`Player[]` toàn cục không khoá; đạn, tổ đội, bang hội, giao dịch, script đều xuyên map.
CPU chưa phải chỗ nghẽn (TICK 6–9 ms trên 55). Lộ trình an toàn nếu vẫn muốn: **đường gửi** sang luồng riêng trước (4.3 bước 2), rồi mới
song song hoá một pha thuần tính toán.

### 4.5 Mỡ còn lại, theo thứ tự

| Mục | Tỉ lệ byte | Ghi chú |
|---|---|---|
| Gói 221 vị trí gọn | 61 % | Giá của đồng bộ vị trí dày. Chỉ giảm được bằng đánh đổi, chủ phải quyết |
| Gói 75 ngoại hình | 12 % | Còn cặp cờ PK (0x01/0x10 đổi cùng nhau, 831 lần) và nhóm rank/danh vọng (1.115 lần) của người thật. Xử giống vá l: tách sang gói gọn hoặc chỉ gửi ở kỳ làm mới |
| Gói 222 số sát thương | 8,5 % | Đã gọn 13 byte, hết cách trừ khi bớt thông tin |
| Gói 148/95 chiêu | 8 % | Chưa soi |
| Gói 223 trạng thái | 5 % | Vừa thêm |

---

## 5. Cách làm việc (bắt buộc theo)

### 5.1 Sửa mã nguồn JX1 — luật sống còn

Nguồn Core là **TCVN3 một byte**, có tệp bọc UTF-8. **Cấm dùng Edit/Write thẳng lên nguồn game.** Viết script Python vá theo mẫu
`ReverseTools/goi_va_*.py`: đọc/ghi `io.open(p, encoding="latin-1", newline="")`; neo và đoạn chèn chỉ **ASCII**; đổi `\n` → `\r\n`
trước khi so khớp; đếm byte cao trước/sau; kiểm số lần khớp. Sau khi vá chạy
`python "C:\Users\nguye\.claude\skills\swordonline-dev\scripts\check_encoding.py" <tệp>`. Script vá dùng xong chép vào `ReverseTools/`
và commit. (`ServerStage.cpp/.h` của Heaven thuần ASCII, nhưng vẫn vá bằng script để tái áp được.)

### 5.2 Build và đặt bản swap

**Trước khi build luôn `git fetch origin` rồi `git merge origin/main`** (3–5 phiên khác cùng sửa cây này).

Core máy chủ / client (từ handoff cũ, dạng `/p:` chạy được trong phiên trước):

```bash
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/amd64/MSBuild.exe" "D:/GAMEDEVNEW_wt_delta/Sources/Core/Core.vcxproj" "/p:Configuration=Server Release" "/p:Platform=x64" "/p:SolutionDir=D:/GAMEDEVNEW_wt_delta/" "/p:PostBuildEventUseInBuild=false" /m:8 /nologo /v:m /t:Build
```

Client: `Client Release` + `Win32`. Kết quả `Sources/Core/x64/ServerRelease/CoreServer.dll`, `Sources/Core/ClientRelease/CoreClient.dll`.

Heaven (phiên này; **Git Bash đổi `/p:` thành đường dẫn** → `MSB1008 Only one project`, phải dùng dạng gạch ngang):

```bash
MSYS_NO_PATHCONV=1 "/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/amd64/MSBuild.exe" "D:\\GAMEDEVNEW_wt_delta\\Sources\\MultiServer\\Heaven\\Heaven.vcxproj" -p:Configuration=Release -p:Platform=x64 "-p:SolutionDir=D:\\GAMEDEVNEW_wt_delta\\" -p:PostBuildEventUseInBuild=false -m:4 -nologo -v:m -t:Build
```

Kết quả `Sources/MultiServer/Heaven/x64/Release/Heaven.dll`. Heaven link `Lib\release64\common.lib` (Common post-build chép sang);
Common đổi thì build Common trước. Heaven.vcxproj có post-build copy vào `bin\server` **của cây build** (không phải cây live) — vẫn
tắt bằng `-p:PostBuildEventUseInBuild=false`.

Đặt bản swap = chép đè thành `<tên>.moi` trong `bin\server` / `bin\client`; chủ chạy `ChayGameServer.bat` / `ChoiGame.bat`.
`ChayGameServer.bat` đổi `CoreServer.dll`, `GameServer.exe`, `heaven.dll`, `engine.dll`, `Lua54Dll.dll`; **không** đổi `multiserver\`.

### 5.3 Chung khe `.moi` với phiên khác — đã va chạm nhiều lần

Trước khi đè một `.moi` không phải của mình:

```bash
python "<scratchpad>/chk2.py" <bản live> <bản .moi của họ> <bản mình vừa build>
```

`missing in mine` phải bằng 0 (bỏ qua chuỗi rác nhị phân). Thiếu chuỗi tính năng thật thì **chưa được đè**: fetch lại origin/main,
build lại, kiểm lại. Rồi đổi tên bản của họ, đặt bản mình vào, **nhắn cho phiên đó** bằng `mcp__ccd_session_mgmt__send_message`.
Các phiên cùng làm hôm nay: bang hội (BH100/BHLV/BHWS + S3Relay), tự kích chuột (TUKICH), WAuto bán rác (HC-CAT), giao diện toạ độ
(UITOADO), xếp hạng (XEPHANG — Goddess đổi tay).

### 5.4 Kéo log và đo

Công cụ (chép từ phiên trước vào scratchpad của phiên này, gốc ở
`C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\aa06ca26-0f29-44cd-a5f8-b461ca7f1eea\scratchpad`):

- `ptich_tran_tk.py <pid> <t0> <t1>` — đọc `jx_auto_server.log.1` + `.log`, cửa sổ 10 s, bảng byte theo mã gói, đỉnh KB/s, quyết định
  đồng bộ, 12 kỳ `[PERF]` gần nhất. `0 0` = toàn bộ theo pid.
- `chk2.py` — so chuỗi ba tệp nhị phân (luật siêu tập 5.3).

| Tệp | Ở đâu | Đọc gì |
|---|---|---|
| `jx_auto_server.log` (+ `.log.1`) | `bin\server` | mọi nhãn máy chủ, xoay vòng 64 MB |
| **`jx_gui_server.log`** | `bin\server` | **`[GUI-DO]` / `[GUI-NB]` đường gửi Heaven (mới, sau swap)** |
| `jx_perf_server.log` | `bin\server` | `[PERF]` nhịp tick, có `pid=` |
| `logs\hethong.log` | `bin\server` | mốc Tống Kim, `[BOTAUTO]` |
| `jx_auto.log` (+ `.log.1`) | `bin\client` | nhãn client, xoay vòng 64 MB |
| `jx_paint.log` | `bin\client` | `[SUM]` lượt vẽ, giật |
| `jx_rep3.log` | `bin\client` | bộ nhớ ảnh (`bo` cộng dồn), `LoadImage FAIL` |
| `jx_crash.log` | `bin\client` | mốc khởi động, sập |

Nhãn: `[NS-BO]` quyết định đồng bộ /10 s · `[NS-TT]` gói 223 · `[PS-BO]` gói 75 · `[DMG-GON]` · `[BC-DEM]`/`[BC-LOAI]`/`[BC-NGUOI]`
phát tán · `[S7-*]` chết/hồi sinh · `[S9-*]` lớp vẽ · `[S6-*]` đồng bộ NPC client · `[GUI-DO]`/`[GUI-NB]` đường gửi.

**Bẫy khi đọc:** so KB/s giữa hai đợt chỉ có nghĩa khi mật độ giống nhau (dùng `BroadCast/10s` và số gói 222 để biết đông hay thưa).
Con số không phụ thuộc chỗ đứng là các bộ đếm máy chủ (`day`, `gui`, `bo`).

### 5.5 Bẫy đã dính, đừng dính lại

1. **Bash heredoc nuốt một nửa dấu gạch chéo ngược** (`\\r` → `\r` = xuống dòng, đã dính lần nữa phiên này khi ghi tài liệu). Tài liệu
   hay script có dấu gạch chéo thì dùng công cụ ghi tệp, không dùng heredoc.
2. **In tiếng Việt ra console Windows** báo lỗi cp1252 → `export PYTHONIOENCODING=utf-8`.
3. **Biến toàn cục dùng ở cả hai bản** phải định nghĩa ngoài mọi khối `#ifdef _SERVER`.
4. **Hàm dùng trước khi định nghĩa** trong cùng tệp phải khai báo trước.
5. **Thêm mã gói mới**: khai báo `Headers/KProtocolDef.h`, cấu trúc `KProtocol.h` (nhớ **ba** bản sao `TGAME_STAT_DATA` chung include
   guard — xem BANGIAO_XEPHANG), cỡ vào **đúng ô** `KProtocol.cpp`, đăng ký hàm xử lý, nâng phiên bản hello.
6. **`day/(gon+day)` thấp không có nghĩa là gói đầy đủ ít byte** — phải nhìn bảng byte tới client.
7. **MSBuild trong Git Bash**: `/p:` bị đổi thành đường dẫn → dùng `-p:`/`-t:` + `MSYS_NO_PATHCONV=1`.

---

## 6. Bản đồ mã cho các việc trên

| Việc | Tệp và chỗ |
|---|---|
| Quyết định đồng bộ, băm nhanh/chậm, gói 221/223 | `Sources/Core/Src/KNpc.cpp`, hàm `NormalSync` |
| Gói ngoại hình 75 và bộ đếm | cùng tệp, khối `[PS-BO]` |
| Số sát thương và gói 222 | cùng tệp, hàm `SyncDamageInfo` |
| Phát tán theo vùng, van, tầm 32 ô | `Sources/Core/Src/KRegion.cpp`, hàm `BroadCast` |
| Client nhận 221/222/223, hello, cờ chiến đấu | `Sources/Core/Src/KProtocolProcess.cpp` |
| Chết và hồi sinh phía client | cùng tệp, `PlayerRevive`, `NetCommandDeath` |
| Nhãn lớp vẽ và nằm bẹp | `Sources/Core/Src/KNpc.cpp`, hàm `Activate` |
| Bộ nhớ ảnh của client | `Sources/Represent/Represent3/TextureResMgr.cpp` (`CheckBalance` 48, ngân sách 97–105, kích khi vượt 457) |
| **Đường gửi gói, khoá riêng, bộ đếm, nhân bản** | `Sources/MultiServer/Heaven/ServerStage.cpp` (`PackDataToClient`, `SendPackToClient`, `_GuiDoBaoCao`, `_NhanBan*`), `.h` (`m_ppNode`, `_Node`) |
| Xả mỗi tick (gọi `SendPackToClient(-1)`) | `Sources/MultiServer/GameServer/KSOServer.cpp:3318` |
| Socket ghi (Write chỉ post IOCP; WSASend ở luồng worker) | `Sources/MultiServer/Common/SocketServer.cpp:823`, `1104` |
| Vòng chính, duyệt map | `Sources/Core/Src/KSubWorldSet.cpp`, hàm `MainLoop` |

---

## 7. Câu hỏi đang chờ chủ trả lời

1. Có cho **thưa đồng bộ vị trí theo khoảng cách khi vùng đông** không (4.3 bước 4)? Chỗ duy nhất còn giảm được nhiều, nhưng đụng trải
   nghiệm nên chưa làm.
2. Sau khi swap `heaven.dll.moi`: cho phép **bật `MoPhongNhanBan=500` trong ~1 phút lúc chủ đang đánh đông** để lấy số thật? (Chỉ đổi số
   trong config.ini, không cần khởi động lại; tắt lại bằng `0`.)
3. Có nâng luôn trần bộ nhớ ảnh 512 trong mã (theo VRAM trống) không, hay để mỗi máy tự sửa `Rep3CacheMB`?
4. Chủ còn thấy **mất hiệu ứng kỹ năng khi đánh** sau khi có `Rep3CacheMB=1500` không? Có lần **chết về thành nằm bẹp** nào sau 17:48
   không (để đọc `[S7-SAUHOISINH]`)?
