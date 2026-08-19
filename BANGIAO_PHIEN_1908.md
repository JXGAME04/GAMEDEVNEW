# BÀN GIAO PHIÊN 19/08/2026 — server JX1: hệ LƯU DỮ LIỆU BOT + ÉP BOT TRONG MAP

> **Đọc tệp này TRƯỚC KHI GÕ bất cứ dòng nào trong phiên sau.**
> Phiên này làm 3 việc lớn phía **server** (`CoreServer.dll` / `GameServer.exe`), tất cả
> đã build xanh, deploy, push — và **chủ game đã restart GameServer lúc 19/08 09:17, bản mới
> ĐANG CHẠY THẬT**. Hệ lưu đã được log xác nhận hoạt động (mục 1.4). Việc còn lại: xác nhận
> nốt phần ép biên theo mục 5.

Tài liệu anh em: `TIENTRINH_SIMCITY_BOT.md` (sổ tay toàn dự án — mục 6, 7, 8 là của phiên
này), `BANGIAO_PHIEN_1808.md` (phiên trước, phía client).

---

## 1 · TRẠNG THÁI NGAY LÚC BÀN GIAO

### 1.1 Binary đang chạy

| Thứ | Trạng thái |
|---|---|
| `bin\server\CoreServer.dll` | bản **19/08 07:07**, md5 `711a6bc2dfae5060bf4d0c6d8f3a42d0` — khớp `Sources\Core\x64\ServerRelease\CoreServer.dll` |
| `bin\server\GameServer.exe` | bản **18/08 21:24**, md5 `b2d1123d1f7e61cbf9e546d654eb3118` |
| Tiến trình `GameServer.exe` | khởi động **19/08 09:17:01** → **đã nạp cả hai bản mới** |
| Cụm multiserver (Goddess/Bishop/S3Relay) | vẫn từ 18/08 23:16 — **chưa restart**, không sao (không có thay đổi nào cho chúng) |
| Bản lùi | ⚠️ các tệp `.bak_*` tôi tạo **đã bị xoá** (chủ game dọn?) — hiện **không có đường lùi tại chỗ**; muốn lùi thì build lại từ commit cũ |

**⇒ Toàn bộ thay đổi của phiên ĐANG chạy thật từ 19/08 09:17.**

### 1.2 Máy chủ chạy Ở ĐÂU

Phiên trước ghi "server ở máy khác `206.82.7.181:6667`". Phiên này xác minh: **có một
GameServer chạy LOCAL** từ `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\`
(PID đổi theo lần chạy), kèm `Goddess.exe` / `Bishop.exe` / `S3Relay.exe` ở
`bin\multiserver\`, và `bot.log` đang được ghi liên tục. Bot chạy trên cụm local này.
Trước khi kết luận "bản vá không ăn", kiểm bằng `Get-Process GameServer | Select StartTime`
so với giờ deploy binary.

### 1.3 Git

`561e2163` là commit cuối của phiên này, đã push `main` (HEAD sau đó có thêm commit tài liệu Dã Tẩu của chủ game). Chuỗi commit của phiên (cũ → mới; **xen kẽ commit dự án WAuto/Dã Tẩu** `f606e540`/`1d24b9fc`/`8a9ae8f5` giữa `41244838` và `561e2163`):

| Commit | Nội dung |
|---|---|
| `1413babf` | vá NULL-deref `LoadPlayerStateSkillList` (lỗi engine sẵn có, cứu cả người thật) |
| `5a1ea695` | **hệ lưu dữ liệu bot** (khoá role Goddess, dấu blob, luật cấp 20, fix O(n²)) |
| `d2d0fd22` | sổ tay mục 6 + 7 |
| `41244838` | **nhịp lưu 10 phút → 30 giây** như người chơi |
| `561e2163` | **ép bot không ra khỏi map** + sổ tay mục 8 |

---

### 1.4 ✅ BẰNG CHỨNG HỆ LƯU ĐÃ CHẠY THẬT (đọc từ `bot.log`)

Qua 2 lần restart (18/08 23:17 và 19/08 09:17):

* **1889 lượt** `[BotLuu] <ten> nap lai bot cu: cap X phai Y`
* **880 lượt** `(duoi cap 20 -> ve map 53)` + **1009 lượt** `(giu nguyen cho cu)` → **luật cấp
  20 hoạt động đúng cả hai nhánh**
* Bot đã luyện tới **cấp 84** (đuôi phân bố: 53 con cấp 72, 31 cấp 73, … 1 con cấp 84) và
  **giữ được cấp qua restart** — trước phiên này chúng luôn về "bản đầu"

⇒ Việc 1 (hệ lưu) và việc 2 (nhịp 30 giây) coi như **đã kiểm chứng**. Việc 3 (ép biên) mới
chạy từ 09:17, chưa đủ dữ liệu — xem mục 5.

## 2 · VIỆC 1 — HỆ LƯU DỮ LIỆU BOT (`5a1ea695`)

**Yêu cầu:** "gọi bot vào đi luyện cấp xong tắt server mở lại thì bot không lưu dữ liệu
mà quay lại bản đầu"; dưới cấp 20 gọi ra ở map 53, từ cấp 20 đang ở đâu ở nguyên đó.

### 2.1 Vì sao trước đây mất — 3 tầng chặn (đã đọc mã, không đoán)

1. `KPlayer::Save()` (KPlayer.cpp:1032, điều kiện ở :1040) **trước khi vá** chặn ngay dòng
   đầu khi `m_nNetConnectIdx == -1` → bot không bao giờ tạo được blob lưu. (Nay dòng đó đã
   có lối thoát `&& !PB_IsBot(m_nPlayerIndex)`.)
2. Vòng lưu của GameServer (`SavePlayerData()`, KSOServer.cpp:3333) chỉ quét **khe kết nối
   mạng** — bot không có khe nào.
3. Goddess `_SaveRoleInfo` (ClientNode.cpp:431) **đòi role phải bị khoá bởi chính node đó**
   (`IsRoleLockBySelf`), bot spawn không đi qua đường login nên chưa từng gửi
   `c2s_roleserver_lock` → Goddess vứt bài lưu, trả `-1`.

Cộng thêm: `pb_KillBot` cố ý không lưu (thiết kế cũ "bot là nhân vật tạm").

### 2.2 Kiến trúc đã dựng

* **Khoá role khi spawn**: `PB_ASK_LOCKROLE` → GameServer gửi `c2s_roleserver_lock`
  (khuôn y hệt người thật vào game, KSOServer.cpp:2906). Khoá tự giải khi GameServer ngắt
  (`~CClientNode` → `UnlockAllRole`).
* **Dấu bot** `PB_BLOB_DAU = 0xB07B07` đóng vào trường chết `BaseInfo.irevivaly` tại
  `SavePlayerDataAtOnce`. An toàn tuyệt đối: người thật **luôn ghi 0** vào trường này
  (KPlayerDBFuns.cpp:925), **không ai đọc** nó lúc nạp (đã grep toàn Core), Goddess chỉ in
  log (IDBRoleServer.cpp:626-630). Dấu đóng **trước** khi GameServer tính CRC nên CRC khớp.
* **"Bot cũ" = có dấu VÀ `nSect` hợp lệ (0..9)** → giữ kỹ năng/phái/đồ, `nAi = IN_FACTION`,
  `nGaveWeapon = 1`. **`nGaveWeapon = 1` là bắt buộc**: để 0 thì `pb_GiveFactionWeapon`
  sẽ **huỷ vũ khí đang cầm** trong đồ đã lưu rồi phát vũ khí nhập môn cấp 1.
* **Luật cấp 20 lúc nạp**: `ifightlevel < 20` → ép `cUseRevive=1 + irevivalid=53 +
  irevivalx=19` (điểm lui sẵn có của `LoadPlayerBaseInfo`); từ 20 dùng `ientergame*` đã lưu.
  `SetLoginType(0)` ép **trước mỗi lần lưu** (script citywar `SetLogoutRV(1)` có thể lật cờ
  làm mất vị trí).
* **Nhịp lưu**: xem mục 3.
* **Mất khoá tự lành**: GameServer nay **đọc byte kết quả** `saverole_result` (trước đây bị
  nuốt hoàn toàn) — `-1` + là bot → `SSOI_PBOT_SAVE_FAILED (0x4A504217)` → `PB_OnSaveFailed`
  ghi log + GameServer tự gửi lại `LOCKROLE`, nhịp 30s kế tiếp lưu bù.

### 2.3 Ràng buộc vận hành (bắt buộc nhớ)

1. `GameServer_cfg.ini [Overload] MaxPlayer` **phải ≥ 1500** khi bật bot (live đang 1500).
   Hạ xuống là người thật bị đẩy quá trần `nIndex > m_nMaxPlayer` → **không đường lưu**.
   `GameServer1/2_cfg.ini` để 290 — **không được chạy bot trên hai GS đó**.
2. Dải `PB_AddBot(1,1000)` chỉ trỏ vào tài khoản do `taobot_bdb` tạo. **Đừng đăng nhập tài
   khoản 1..1000 bằng client thật** lúc đang gọi bot (cửa sổ tranh chấp role).
3. Tắt server "sạch": bấm **"LƯU dữ liệu bot ngay"** → chờ dòng `[BotLuu] ... XONG` trong
   `bot.log` → chờ thêm ~10 giây (Goddess ghi nốt) → tắt. Không bấm gì thì mất tối đa
   ~30–60 giây luyện cuối.

---

## 3 · VIỆC 2 — NHỊP LƯU 30 GIÂY NHƯ NGƯỜI CHƠI (`41244838`)

Ban đầu tôi đặt 10 phút/bot vì sợ nghẽn Goddess; chủ game hỏi "không viết lưu tự động như
người chơi được à" → đã kiểm chứng bằng mã là an toàn và nâng lên **30 giây/bot**:

* BDB mở với **`DB_TXN_NOSYNC`** (Goddess `DBTable.cpp:33`) → `put` không fsync, chỉ vài µs.
* GUI Goddess `AddOutputString` **tự cắt trần 100 dòng** ListBox (IDBRoleServer.cpp:731; hàm `AddOutputString`).
* Bộ điều tiết **2 gói/nhịp = trần 36 gói/giây**, nhu cầu 1000 bot là 33,3/giây; đông hơn
  thì nhịp tự giãn. Phản biện mô phỏng đúng vòng lặp: hội tụ **31,2 giây/bot**, đỉnh đúng
  36 gói/giây, không con nào bị bỏ đói.
* Đếm **`luu N goi`** in kèm `[BotPerf]` mỗi 10 giây để quan trắc.
* **Bẫy đã cắn khi đổi hằng số**: `PB_OnSaveFailed` từng "hẹn lưu lại sớm 60 giây" bằng phép
  `nowT - GAME_FPS*(PB_LUU_MOI_GIAY - 60)` — với nhịp 30 thì ra **số âm** → unsigned wrap →
  đến hạn ngay lập tức → spam lưu. Đã bỏ hẹn-sớm.

Phí phải trả: file log Goddess phình ~170 MB/ngày khi đủ 1000 bot (chấp nhận được, muốn thì
xoay vòng).

---

## 4 · VIỆC 3 — ÉP BOT KHÔNG RA KHỎI MAP (`561e2163`)

**Yêu cầu:** "bot vẫn di chuyển ra khỏi map — hoặc hàm di chuyển lên map làm bot bay ra
ngoài map — phải ép bot không được di chuyển hay bay ra khỏi map."

### 4.1 Chẩn đoán (5 agent đọc mã + pháp y `bot.log` 2,2 triệu dòng)

Pháp y log: **0 toạ độ âm, 0 toạ độ 5 chữ số** trong toàn file → các rào 18/08 chặn đúng
phần của chúng, hiện tượng chủ game thấy đi qua **3 cửa không để lại log**:

1. **A* "nói dối" với đích ngoài map.** `FindPathServer` gặp đích ngoài lưới thì **kẹp về ô
   mép** rồi trả **"đường trọn vẹn"** (nay ở KSubWorld.cpp:3365-3366, hàm bắt đầu :3349) thay vì báo lỗi →
   `PB_WalkTo` tưởng đích hợp lệ, đi hết waypoint rồi **chặng cuối `do_run` thẳng vào toạ độ
   THÔ ngoài map**, phát lại mỗi nhịp. Nguồn sinh đích ngoài map: điểm tán ra chỉ kẹp cận
   dưới, `nBuocRa` 8–11 ô sau đổi map không kiểm, điểm nhắm làm tròn lưới 128 không kiểm lại.
2. **Vùng rỗng dữ liệu — cửa lớn nhất.** Region **không có tệp vật cản** bị `memset 0` =
   "toàn đi được" ở **cả lưới A* lẫn engine** → bot **đi bộ hợp lệ** ra vùng
   nhìn-như-ngoài-map, không dòng log nào.
   ⚠️ **Đính chính số liệu:** chú thích cũ trong `KSubWorld.cpp` (và bản nháp tài liệu này)
   ghi "292/480 region có `_S`" — **đọc như tỉ lệ là SAI**: hai phép đếm thuộc **hai thư mục
   rời nhau** (292 là của riêng map `剑门关vn`, vốn có 0 tệp `_C`; 479/480 tệp `_C` nằm ở map
   `fongkieu`, vốn có 0 tệp `_S`). Tổng thật trên cây chạy: **2638 tệp `_S` vs 480 tệp `_C`**.
   Kết luận thì **vẫn đúng**: *có* map chỉ đóng gói bản `_C`, nên fallback đọc `_Region_C` là
   cần thiết.
3. `ChangeWorld` / `SetPos` chỉ kiểm khung region, không kiểm vật cản.

### 4.2 Đã vá

* `PB_WalkTo` **từ chối ngay** đích ngoài map (`Mps2Map` `nR < 0` → `-1`); mọi caller đều có
  sẵn nhánh xử lý `-1`.
* **Chặng cuối không lao vào đích thô nữa**: kẹp vào **block cuối** của đường A*
  (`BlockNearestMps`) — block đã xác nhận đi được. Đích **bị thay thế** mà bot đã tới sát
  điểm kẹp → báo thua (`-1`) để caller bốc điểm khác. (Chỉ xét khi điểm kẹp **khác** đích
  thô, nên đích nằm trong block cuối vẫn đi bình thường.)
* **Lưới A***: region không đọc được dữ liệu vật cản → **vật cản toàn bộ**, chỉ áp
  `#ifdef _SERVER` (cache client `%d.fp` khoá bằng `FINDPATH_VERSION` không đổi, áp chung sẽ
  lệch ngữ nghĩa). Thêm **fallback đọc `_Region_C.dat`** (cùng `SCENE_FILE_INDEX`, cùng
  payload 2048B — đã kiểm 3 loader sẵn có đọc _C y hệt) rồi mới tới `_OBSTACLE.DAT`.
* **Log mới** `[PathSrv] map X: N/M region KHONG co du lieu vat can`; map thiếu **100%** in
  `CANH BAO ... BOT KHONG DI DUOC tren map nay`.
* **Cache `_srv.fp`**: `kMagic` `0x53465002` → `0x53465003`, **và bỏ guard "không ghi đè"**.
  Guard cũ là lỗi ngầm từ 18/08: sau khi nâng magic lần trước, **77 map tự dựng lại lưới MỖI
  LẦN BOOT** mà cache mới không bao giờ được ghi. Thêm kiểm **kích thước tệp tổng**, kẹp
  `uNb`, kẹp `parentId` khi nạp (tệp cụt giữa chừng = `m_GridNode` rác = nguy cơ sập).
* **Chase**: điểm làm tròn 128 văng ngoài khung → dùng điểm thô (không cấm oan quái sát mép).
* **Follow đội trưởng**: hứng giá trị trả về `PB_WalkTo` (caller **duy nhất** từng vứt nó) —
  thua thì nghỉ 5 giây, và **3 dòng dọn dẹp nằm TRONG cổng nghỉ** (phản biện chốt bắt: để
  ngoài thì lúc nghỉ vẫn quét `pb_FindTarget` + chạy A* chase **mỗi nhịp**). `[BotLach]`
  `SetPos` reset thêm `follow`/`loot`/`nFollowNghiToi`.
* **Chặng cuối block gộp rộng** (tới 512×1024 MPS): chỉ đếm "không tiến" khi bot **thật sự
  đứng yên** (neo `lastMoveTick` của B5a) — khỏi báo thua giả giữa đường.

### 4.3 Về cache `.fp` — KHÔNG phải xoá tay

`kMagic` 03 làm cache cũ **tự bị vứt** khi nạp, và guard "không ghi đè" đã bỏ nên cache mới
**được ghi đè** lên tệp cũ. Lần boot đầu sau restart mỗi map tự dựng lại lưới (vài giây, chỉ
lần đầu vào map đó); **boot thứ hai trở đi phải thấy lại dòng `nap cache`** — nếu boot thứ
hai vẫn dựng lại thì việc ghi cache có vấn đề, cần điều tra.

---

## 5 · VIỆC ĐẦU TIÊN CỦA PHIÊN SAU

### 5.1 Xoay vòng `bot.log` (đã lỡ một nhịp)

`bot.log` nay **~1,0 GB**; mã ghi bằng `fopen(..., "a")` nên restart **không** xoá — lần
restart 09:17 đã không đổi tên nên log 18/08 và 19/08 đang nằm chung một tệp. Đổi tên khi
tiện (server đang chạy vẫn rename được, tệp mới sẽ tự tạo):

```
mv "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/bot.log" \
   "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/bot.log.1908_sang"
```

### 5.2 Test trọn hệ lưu (chưa kiểm chứng lần nào)

1. Restart GameServer → lệnh bài → **Gọi 1000** → **Cho bot VÀO PHÁI** → **BẬT đánh quái**
2. Chờ 5–10 phút; xem `[BotLuu]` (lưu ngay sau vào phái) và `[BotPerf] ... luu N goi`
3. Bấm **"LƯU dữ liệu bot ngay"** → chờ `[BotLuu] ... XONG` → +10 giây
4. **Tắt cả GameServer lẫn Goddess** → bật lại → **Gọi 1000**
5. Kỳ vọng log:
   `[BotLuu] <ten> nap lai bot cu: cap X phai Y (giu nguyen cho cu)` — bot ≥ 20 đứng đúng
   chỗ cũ với nguyên đồ/kỹ năng, **không** phải vào phái lại; bot < 20 về map 53
6. Kiểm chéo: console Goddess in `SaveRoleInfo:<ten>` mỗi lần lưu

**Lưu ý:** sau reboot bot cũ vào thẳng `PB_AI_IN_FACTION` và **đứng im chờ lệnh** — hai công
tắc chế độ (`s_nPbCheDoNhapMon`, `s_nPbCheDoDanh`) là biến static, reset về 0 mỗi lần boot.
Phải bấm **BẬT đánh quái** thì chúng mới hoạt động. Đừng nhầm là "nạp bot cũ hỏng".

### 5.3 Test ép biên + đọc bản đồ thiếu dữ liệu

```
grep -a "\[PathSrv\]" bot.log | grep -a "CANH BAO"
```

Map nào dính `CANH BAO ... BOT KHONG DI DUOC` = thiếu **toàn bộ** dữ liệu vật cản → bổ sung
tệp `_Region_S/_Region_C` cho map đó **hoặc rút khỏi bảng `s_bai`** (KPlayerBot.cpp, 32 bãi).
Cũng nên đọc dòng `[PathSrv] map X: N/M region ...` để biết map nào thiếu một phần.

Quan sát bot: không còn cảnh ép sát mép/biến mất. Nếu vẫn thấy → kéo log grep `[BotBien]`,
`[BotLach]`, `[BotDan]`, `[BotBai]` quanh mốc thời gian đó.

---

## 6 · NỢ KỸ THUẬT — ĐÃ BIẾT, CHƯA VÁ

| # | Việc | Ghi chú |
|---|---|---|
| 1 | **Goddess `RemoveLogProc` checkpoint 1 giờ → 10 phút** | Đã sửa **nguồn** (IDBRoleServer.cpp) nhưng **chưa build/deploy**: `Goddess.exe` live là bản 19/04, không chứng minh được build từ cây này. Với nhịp ghi 30s, txn log BDB phình ~0,5–1 GB/giờ giữa hai lần dọn; crash giữa chừng thì `DB_RECOVER` replay cả giờ log lúc boot. **Phải đối chiếu nguồn ↔ binary Goddess trước khi ship** (Goddess chỉ có cấu hình Win32; `OutDir = bin\multiserver` **chỉ đúng cho Debug|Win32**, còn Release|Win32 ra `.\Release\`; cây D không có `bin\multiserver` nên build ở D không chạm bản live ở E). |
| 2 | **`taobot_bdb.exe` chưa build lại** | Nguồn đã thêm `p->BaseInfo.irevivaly = 0;` sau `memcpy` (xoá dấu bot khi nhân bản mẫu). Binary cũ chưa có. Chỉ cần trước khi **đổi nhân vật mẫu**; server đã tự vệ bằng điều kiện `nSect` hợp lệ. |
| 3 | **Lỗi engine: `Mps2Map` chia số âm** | `int x = Rx / (…)` cắt về 0 nên MPS âm nhỏ (−511..−1) cho `x = 0`; map có `rect.left == 0` thì lọt kiểm tra → `SetPos`/`ChangeWorld` đặt NPC với `m_MapX` âm. **Ảnh hưởng cả người thật** → đợt riêng. |
| 4 | **Lỗi engine: `ServeJump` trôi offset** | Cộng `m_OffX/m_OffY` **trước** khi chụp `nOldOffX/Y` (ngược với `ServeMove`), nên nhánh khôi phục trả lại offset đã cộng → nhảy liên tục vào biên làm toạ độ trôi. **Ảnh hưởng cả người thật** → đợt riêng. |
| 5 | **`GetInviteReply` từ chối 103 lần** | Log `[BotNhom] VAO NHOM ... THAT BAI (mem=0/tran=3, mo=1)` — lý do mâu thuẫn (nhóm mở mà báo đầy). Đáng ngờ, chưa truy. |
| 6 | ~~Khối `PB_` nằm ngoài `#ifdef _SERVER`~~ — **NỢ GIẢ, đã bác bỏ** | Kiểm chứng 19/08: toàn bộ khối `PB_` **đã nằm trong** `#ifdef _SERVER` (extern ở 12731–12737 trong khối mở tại 12359; bảng đăng ký 13082–13089 trong khối mở tại 12875). Nếu build client vẫn lỗi link thì **nguyên nhân ở chỗ khác — đừng tìm theo hướng này**. |
| 7 | **Hạ bớt log bot** | `bot.log` ~33 dòng/giây, mỗi dòng một lần `fopen`/`fclose`. Khi hệ đã ổn định: hạ `[BotDame]` (6,4/s), `[BotDanh]` (5,9/s), `[BotHoang]` (3,7/s) hoặc thêm công tắc độ ồn. |
| 8 | **Reply `saverole_result` đến trễ** | Có thể `SetSaveStatus(SAVE_IDLE)` lên khe người thật vừa tái sử dụng (mất 1 chu kỳ autosave, tự lành sau 30s). Lỗi lớp cũ của engine, 1000 bot khuếch đại tần suất. |
| 9 | **Nhiều GameServer chung Goddess** | `_LockOrUnlockRole` nhánh mở khoá **không kiểm chủ khoá** (khác `UnlockRoleSelf`). Live 1 GS nên vô hại; chạy ≥2 GS thì phải sửa. |

---

## 7 · QUY TRÌNH (đã kiểm trong phiên này)

### 7.1 Sửa mã — encoding

* `KPlayerBot.cpp` / `KPlayerBot.h` / `simcity_admin.lua` / `taobot_bdb.cpp`: **ASCII thuần**.
* `KPlayer.cpp` (4361 byte cao), `CoreServerShell.cpp` (930), `ScriptFuns.cpp` (1801),
  `KSOServer.cpp` (265), `KSubWorld.cpp` (198), `CoreServerShell.h` (756),
  `KPlayerDBFuns.cpp` (88), `TIENTRINH_SIMCITY_BOT.md` (2737): **TCVN3** → sửa **chỉ** qua
  script python latin-1 hoặc `safe_edit.py`.
* **Kiểm sau mỗi đợt**: đếm byte ≥ 0x80 so với `git show HEAD:<file>` — phải **khớp tuyệt đối**.
* **EOL không đồng nhất**: `KPlayerBot.cpp` LF, `KSubWorld.cpp`/`KPlayer.cpp`/`KSOServer.cpp`
  CRLF, `simcity_admin.lua` LF. **Luôn xem byte thật (`cat -A`) trước khi tạo chuỗi `--old`**
  — phiên này mất 1 vòng vì đoán nhầm CRLF.
* **Bẫy heredoc**: chuỗi có `\` phải qua **Write tool → chạy file**, không viết inline trong
  Bash (đã cắn lại một lần trong phiên này).

### 7.2 Build

```
MSBuild Core.vcxproj      -p:Configuration="Server Release" -p:Platform=x64 -p:SolutionDir="D:\GAMEDEVNEW\Sources\"
MSBuild GameServer.vcxproj -p:Configuration=Release        -p:Platform=x64 -p:SolutionDir="D:\GAMEDEVNEW\Sources\"
```

* MSBuild ở `C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\`.
* **Đừng lọc output bằng `head`** — dòng lỗi nằm ở cuối; ghi ra file log rồi grep
  `"error C|error LNK"` (phiên này suýt tưởng build xanh trong khi có `C2065`).
* Cảnh báo `MSB8012 TargetPath ... x64\Server Release ... vs ... x64\ServerRelease` là **bình
  thường**; DLL thật nằm ở `x64\ServerRelease\`.
* PostBuild của Core chỉ chép sang `D:\GAMEDEVNEW\bin\server\` — **không** đụng cây E.
* Goddess **không có** cấu hình x64 (chỉ Win32) — build x64 sẽ trả về lỗi, đúng dự kiến.
* Lỗi đã cắn: khối `static` dùng trong `PB_RemoveAll` phải khai báo **trước** nó (C2065).

### 7.3 Deploy

Server đang chạy vẫn **rename được** file:

```
mv  bin/server/CoreServer.dll  bin/server/CoreServer.dll.bak_<ngay_gio>
cp  Sources/Core/x64/ServerRelease/CoreServer.dll  bin/server/CoreServer.dll
md5sum <hai bên>   # phải khớp
grep -ac "<chuoi moi trong ban va>" bin/server/CoreServer.dll   # phải = 1
```

Header thật của Core là `D:\GAMEDEVNEW\Lib\S3DBInterface.h` (resolve qua `/I ..\Network`);
`Sources\Core\lib\s3dbinterface.h` là **file ma, không ai include** — đừng đọc nhầm nó
(layout khác hẳn: `szName[100]`, thứ tự trường khác).

---

## 8 · PHƯƠNG PHÁP ĐÃ DÙNG (giữ cho phiên sau)

Mỗi việc đều chạy **phản biện đối kháng nhiều lăng kính** trước khi build, và nó **đáng
tiền**: hệ lưu qua 3 vòng (6+2 lăng kính) bắt 10+ lỗi thật, trong đó nặng nhất là `PB_IsBot`
thiếu so `dwID` (người thật chiếm khe bot bị GM kick sẽ bị đóng dấu oan vào DB) và
`LoadPlayerStateSkillList` NULL-deref (sập server lúc nạp nhân vật); ép biên qua 2 vòng
(3+1 lăng kính) bắt lỗi follow dọn dẹp ngoài cổng nghỉ.

Luật rút ra: **mọi kết luận phải trích được `file:line` đã thật sự đọc**; phản biện viên nào
nói "nghi ngờ" mà không chứng minh được bằng mã thì ghi nhận chứ không sửa theo.
