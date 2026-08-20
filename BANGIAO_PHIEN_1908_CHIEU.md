# BÀN GIAO PHIÊN 19/08/2026 CHIỀU–TỐI — hệ BOT KPlayer (trang bị, Dã Tẩu, bán sạp)

> **ĐỌC TỆP NÀY TRƯỚC KHI GÕ dòng nào cho hệ bot.** Phiên sáng cùng ngày ở
> `BANGIAO_PHIEN_1908.md` (hệ lưu dữ liệu bot + ép bot trong map). Sổ tay dự án:
> `TIENTRINH_SIMCITY_BOT.md`.
>
> Phiên này gồm **7 đợt vá** (`f34a5cbb` → `e243c38c`), tất cả đã build xanh, deploy,
> push. Bản cuối **đang chạy thật** trên GameServer khởi động 19/08 17:42.

---

## 1 · TRẠNG THÁI LÚC BÀN GIAO (19/08 17:50)

| Thứ | Trạng thái |
|---|---|
| `bin\server\CoreServer.dll` | md5 `6bcd1a663621671e3a68a257e999bf64`, **khớp** `Sources\Core\x64\ServerRelease\CoreServer.dll` |
| GameServer | khởi động **17:42:20** → **đang chạy bản cuối** |
| Goddess / Bishop / S3Relay | vẫn từ 18/08 23:16 (không có thay đổi cho chúng) |
| Commit cuối của tôi | `e243c38c` (đã push `main`) |
| `bot.log` | đã xoay: `bot.log.1908_sang` (1,0 GB), `bot.log.1908_chieu` (79,7 MB), log hiện tại bắt đầu 17:42 |
| Bản lùi DLL | `CoreServer.dll.bak_1908_1740` (và các mốc 1032/1102/1320/1335/1623) |

**⚠️ Có PHIÊN SONG SONG** (dự án auto Dã Tẩu cho WAuto) commit xen kẽ vào cùng repo và
**cùng build `Core.vcxproj`** (`KDaTauTables.h` nằm trong `Sources/Core/Src`). Trước khi
kết luận "DLL không có mã của tôi", grep chuỗi đặc trưng thay vì so mtime.

### Bằng chứng bản cuối chạy đúng (log 17:42–17:50)

```
[BotDT] chon 50 bot lam Da Tau (yeu cau 50; 105/1000 bot du cap 80)
[BotDT] DuongLong185 bo nhiem vu loai 4 (kieu 1, can 15 cuon) - doi de khac, khong ton gi
[BotDT] DuongLong185 NHAN nhiem vu loai 5 - vua luyen cap vua lam
```
35 lượt nhập môn, 49 lượt nhận nhiệm vụ, 7 lượt đổi nhiệm vụ miễn phí. **Chưa có lượt
TRẢ nào** — bình thường, vì loại 5/6 chờ 5 phút mới thử trả lần đầu và run mới 8 phút.

---

## 2 · BẢY ĐỢT VÁ TRONG PHIÊN

| Commit | Nội dung |
|---|---|
| `f34a5cbb` | Bot không mài mòn trang bị + `[BotCuu]` tự cứu kẹt lưới + phát lại vũ khí khi tay không |
| `949e2841` | Sổ tay mục 9 |
| `292c32b5` | Trang bị theo cấp (Kim Phong 9 món, ngựa Túc Sương, vũ khí cấp 10 ở cấp 81) + chia đều bãi + Quế Hoa Tửu + **fix tiềm năng chỉ tiêu 1 lần** |
| `de799143` | Bot chạy Dã Tẩu (bản 1) + buff theo lệnh bài + lên/xuống ngựa theo chiêu + party full 8 + đội trưởng tản xa |
| `fa595ddd` | Dã Tẩu v2 (chạy bộ tới NPC, chỉ loại 4/6) + **bot ra thành bán sạp thật** |
| `f42782dd` | Loại 4 gom đủ mới về trả + **hủy nhiệm vụ riêng bot miễn phí** (`tl_dealtask`) |
| `e243c38c` | **Dã Tẩu = máy trạng thái 10 pha** + **sạp bán đồ XANH** (sau đợt điều tra 13 agent) |

---

## 3 · SÁU LỖI GỐC TÌM RA (giá trị lâu dài — đừng để mắc lại)

### 3.1 🔴 `g_FileName2Id` băm **phân biệt hoa/thường**

`Engine/Src/KFilePath.cpp:442` băm từng byte, **không hạ chữ thường**. Gọi
`ExecuteScript("\\Script\\Global\\station.lua")` (chữ hoa) → id khác id engine đăng ký →
`g_GetScript` trả NULL → `KPlayer::ExecuteScript` (`KPlayer.cpp:6829`) **return FALSE im
lặng**: không log, không ScriptError.

Triệu chứng thật: **27.651 dòng "nhờ Xa Phu" trong 20 phút mà bot không hề dịch chuyển**.

> **LUẬT: mọi đường dẫn script trong C++ phải viết THƯỜNG; sau build grep DLL để chắc.**

### 3.2 🔴 Đồ TÍM = `nPoint ≠ 0`, không liên quan số opt

`KItem::GetColorItem()` (`KItem.cpp:3200`) → `IsPurple()` (`:3222`) = `m_CommonAttrib.nPoint`.
`ItemSet.AddItemSet2` **tham số thứ 12 chính là `nPoint`** — truyền số-lượng-opt vào đó là
`SetPoint()` → tím 100%. Nhánh `nPoint > 0` còn **bỏ qua `Gen_MagicAttrib`**, đọc
`magicattrib.txt` theo **SỐ DÒNG** (dòng 1–5 đều là vật liệu khảm nam) → "đồ tím có ô khảm".

**Đường ra đồ XANH (chính là đường quái rớt đồ):** `ItemSet.Add` (không có tham số `nPoint`
⇒ luôn 0) + mảng `pnMagicLevel` **có giá trị** → `Gen_MagicAttrib` → `m_aryMagicAttrib[0] ≠ 0`.

- **Đồ xanh BẮT BUỘC phải có opt** — 0 opt ra đồ **TRẮNG**, không phải xanh.
- Mảng phải khai `[MAX_ITEM_MAGICLEVEL]` (16) + `ZeroMemory`: `Gen_MagicAttrib` lặp tới
  `i < 16` và có đọc `pnaryMALevel[8]`. (Nợ: `KNpc.cpp:8241` khai `[6]` — lỗi thật trên
  đường rớt đồ của quái, để đợt riêng.)

### 3.3 🔴 `ChangeWorld` chỉ kiểm khung region, **KHÔNG kiểm vật cản**

`pb_RaBai` cộng thẳng ô lệch ±8 ô rồi thả bot → map hành lang hẹp (79 Tương Dương Mật Đạo)
bot rơi **vào thân tường**, kẹt vĩnh viễn. Nay mọi điểm đặt chân đi qua `pb_ODat`
(quét xoắn ốc tìm ô engine bảo trống **và** lưới A* không chặn).

### 3.4 🔴 `KItemList::Abrade` tháo trang bị khi độ bền = 0

Bot đánh ~10 giờ → độ bền 0 → engine tháo trang bị ném vào túi → dọn túi **xóa mất** →
210 bot tay không, đánh không sụt máu, chết liên tục. Nay `Abrade` return sớm cho bot.

### 3.5 🔴 `pb_AllocAttribPoints` chỉ được gọi MỘT LẦN lúc vào phái

Chú thích cũ ghi "nhánh IN_FACTION sẽ tiêu tiếp" — **nhánh đó không tồn tại**. Điểm tiềm
năng dồn đống → bot cấp 82 chỉ 440 HP. Nay tiêu mỗi lần lên cấp.

### 3.6 🔴 Bấm lại menu = bốc lại từ đầu

`PB_SetDaTau` cũ xóa sạch `nDaTauChon` rồi bốc mới → bot đang giữa nhiệm vụ mất cờ →
`pb_RaBai` kéo về bãi. Chứng minh: CaoVinh338 teleport map 122 lúc 16:30:57, bị bỏ chọn
16:31:40, về bãi map 198 **đúng giây đó**. Nay chỉ thêm/bớt phần chênh lệch và **không bao
giờ cắt bot đang `course == 1`**.

---

## 4 · MÁY TRẠNG THÁI DÃ TẨU (bản đang chạy)

```
TOI_NPC → THOAI(Task_Confirm / Task_TaskProcess) → LOC
   ├─ loại 4 (≤5 cuộn, kiểu 1) → TOI_XAPHU → GODATAU → FARM_NV
   └─ loại 5/6                 → FARM_BAI (vừa luyện cấp vừa làm)
→ VE_TRA(Task_Accept) → THUONG(finish_*) → TOI_NPC …
→ đủ 40/ngày → NGHI 30 phút → về bãi luyện cấp
```

Hợp đồng trả về với `pb_DriveBot`: `0` = tự điều khiển (return ngay) · `1` = đang farm map
nhiệm vụ (chặn `pb_RaBai`) · `2` = nhả máy (luyện cấp bình thường).

### Luật lọc nhiệm vụ (quyết định then chốt)

Bot cấp ≥80 rút bậc link 11, ở đó **95,24% trọng số là dòng Num=15** (~13.500 con quái) và
phần còn lại là **Mật Chỉ** (cuộn 212, **chỉ rơi từ boss xanh** ~1/31.360 con). Cả hai đều
là ngõ cụt ⇒ bot **chỉ nhận loại 4 kiểu 1 và ≤ `PB_DT_CUON_TOI_DA` (5) cuộn**, còn lại
**đổi nhiệm vụ MIỄN PHÍ** qua `PB_BotDoiNhiemVu` → `tl_dealtask()`.

- `task 1032`: **byte thấp = KIỂU cuộn** (1 = Địa Đồ Chỉ 205, 2 = Mật Chỉ 212), **byte 2 = SỐ cuộn**.
- ⚠️ **TUYỆT ĐỐI KHÔNG gọi `PB_BotCancel`** (bọc `Task_Cancel(1)` thật): +1 vào `2797` (mất
  thưởng mốc-40), +1 vào `2420` (đốt lượt trong trần 40/ngày), có thể làm NPC giận 10 phút.
  Đường đúng duy nhất là `PB_BotDoiNhiemVu`.
- ⚠️ `tl_dealtask` **đặt lại `1025 = 0`** — đừng đổi nhiệm vụ khi đã gom được kha khá cuộn.

### Các chốt chống treo

| Chốt | Ngưỡng |
|---|---|
| Farm không ra cuộn | 20 phút → đổi nhiệm vụ |
| Loại 5/6 chưa xong | thử trả mỗi 5 phút, quá 45 phút → đổi |
| Kích Xa Phu không sang được map | 5 lần → đổi |
| Trả mà `course` vẫn = 1 | 8 lần → đổi |
| Túi < 5 ô khi trả | tự dọn túi, 5 lần vẫn thiếu → nghỉ 15 phút |
| Không tới được NPC | ~600 nhịp → nghỉ 5 phút |
| NPC giận (`1036 == 10`) | nghỉ **11 phút** (không phải 3) |
| Đổi 40 lần chưa ra nhiệm vụ vừa sức | nghỉ 5 phút |

### Quét NPC — không dùng tọa độ cứng

`Npc[].ActionScript` là **đường dẫn script nạp từ pak** (chính cái `KPlayer::DialogNpc`
dùng). Quét theo chuỗi `"seasonnpc"` = Dã Tẩu, `"xaphu"` = Xa Phu, so **không phân biệt
hoa/thường**, có bản nhớ theo bản đồ. Không tìm thấy thì lùi về bảng tọa độ + ghi log.

---

## 5 · CÔNG CỤ LỆNH BÀI (menu bot)

SimCity đã **gỡ khỏi lệnh bài** — mục cũ "SimCity - bot gia lap" nay là
**"BOT nguoi choi (KPlayer)" → `PB_Menu`**. Trong `PB_Menu`:

| Nút | Việc |
|---|---|
| Gọi 1 / 100 / 1000 bot | sinh bot |
| Cho bot VÀO PHÁI / BẬT–TẮT đánh quái | chế độ bền vững |
| BẬT–TẮT bot nói chuyện | chat |
| **Chấm TTL + Quế Hoa Tửu: BẬT/TẮT** | **mặc định TẮT** — bật thì bot mới có x2 exp + 20 may mắn |
| **Bot chạy nhiệm vụ DÃ TẨU** | 0 / 20 / 50 / 100 / 200 / 1000 |
| **Bot ra THÀNH bán sạp** | 0 / 10 / 20 / 50 / 100 / 200 |
| LƯU dữ liệu bot ngay / Gỡ hết bot | như cũ |

**Ràng buộc vận hành:**
1. Hai nút chọn số **bốc trong bot ĐANG SỐNG** → bấm **sau** khi Gọi 1000.
2. Dã Tẩu **chỉ lấy bot cấp ≥80** (`PB_DT_CAP_TOI_THIEU`). Hiện 105/1000 con đủ cấp, nên
   bấm "200 bot" sẽ ra log `chon 105 bot ... (yeu cau 200; 105/1000 bot du cap 80)` — **đúng
   thiết kế, không phải lỗi**.
3. Hai nhóm Dã Tẩu và bán sạp **loại trừ nhau**.
4. Bấm lại menu nay **an toàn** (chỉ thêm/bớt phần chênh), và có nhịp **bù quân số mỗi 60
   giây** trong `PB_Breathe` vì mỗi lần nạp lại bot đều xóa cờ.
5. Sau **restart server**, `s_nPbDaTauMax` về 0 → **phải bấm lại nút Dã Tẩu**; nhưng
   **không** cần bấm lại "BẬT đánh quái" (bot nạp lại tự lên `PB_AI_FIGHT`).

---

## 6 · CÁC TÍNH NĂNG KHÁC ĐÃ THÊM TRONG PHIÊN

- **Trang bị theo cấp** (mỗi lần lên cấp): tiêu hết tiềm năng tồn đọng · mặc dần bộ Hoàng
  Kim **Kim Phong** (goldequip id **177–185**, `id = DÒNG − 1`; cả 9 món mang `nGoldId = 36`
  = phép thử "đã mặc" sống sót qua restart; nhẫn thứ 2 phải chỉ định `itempart_ring2`) ·
  cưỡi **ngựa Túc Sương cấp 10** · đạt cấp 81 thì **nâng vũ khí lên cấp 10 cùng loại**.
  ⚠️ `Equip` phía server **không tự kiểm điều kiện** — phải gọi `CanEquip` trước.
- **Ngựa**: chiêu cấm-trên-ngựa → tự xuống, chiêu đòi-cưỡi → tự lên (đọc `GetHorseLimit`);
  di chuyển trong 10 map thành thị/thôn → cưỡi ngựa. `CheckRideHorse(FALSE)` = **lên**,
  `(TRUE)` = **xuống**.
- **Bán sạp**: bot xuống ngựa + **ngồi** (`do_sit`), chỉ ngồi **ô trống quanh NPC Dã Tẩu
  thật**, biển sạp lấy từ `settings/simcity/stall_adv.txt`, bày 3–5 **trang sức xanh** giá
  gốc ×2; người chơi mua qua **đúng đường `TradeBuyItem` của người thật** (có thuế thành).
  Hết hàng 5 phút châm đợt mới. `pb_DonTui` không xóa hàng đang bày (`nPrice > 0`).
- **Chia đều bãi luyện**: chọn bãi **ít bot nhất** đúng bậc, tràn 120 mới xuống bậc dưới.
  Đội trưởng nhóm chọn điểm roam **ít bot nhất / xa nhất** → các nhóm tự rải toàn map.
- **Tổ đội**: 100% bot muốn vào nhóm, trần 7 thành viên (nhóm 8). Bot Dã Tẩu/bán sạp
  **không** vào danh sách ghép nhóm và **không** bám theo đội trưởng.
- **Dọn rác log**: chặn mời vào nhóm **đã đầy** trước khi gọi `GetInviteReply` — trước đó
  **49% log (120.281/244.802 dòng)** là rác loại này.

---

## 7 · NỢ KỸ THUẬT / VIỆC CHƯA LÀM

| # | Việc | Ghi chú |
|---|---|---|
| 1 | **Chưa nghiệm thu trọn vòng Dã Tẩu** | Cần run ≥30 phút để thấy `TRA XONG nhiem vu` → `chon ruong thuong`. Run 17:42 mới 8 phút |
| 2 | **Chưa thấy bot nhận được loại 4** | Bậc link 11 hiếm dòng ≤5 cuộn. Nếu muốn bot làm loại 4 thật thì hoặc nâng `PB_DT_CUON_TOI_DA`, hoặc tăng tỉ lệ rơi cuộn cho bot, hoặc chấp nhận bot chỉ làm loại 5/6 |
| 3 | **Sạp chưa test lần nào ở bản mới** | Chưa có `[BotSap]` trong run 17:42 (chủ game chưa bấm). Cần xác nhận **0 dòng** `bo mon khong phai do xanh` |
| 4 | **Giáp đã mất từ trước không phục hồi được** | Không có bản ghi. Bot đó máu mỏng; muốn sạch thì gỡ tạo lại 1000 con |
| 5 | `KNpc.cpp:8241` khai `pnMagicLevel[6]` | Đọc ngoài biên trên đường rớt đồ của **quái** (ảnh hưởng người thật) — đợt riêng |
| 6 | Nợ engine cũ: `Mps2Map` chia số âm, `ServeJump` trôi offset | Từ phiên sáng, ảnh hưởng cả người thật |
| 7 | Goddess `RemoveLogProc` checkpoint 1 giờ → 10 phút | Đã sửa nguồn, **chưa build/deploy** |
| 8 | `taobot_bdb.exe` chưa build lại | Chỉ cần khi đổi nhân vật mẫu |

---

## 8 · QUY TRÌNH NGHIỆM THU PHIÊN SAU

1. Đổi tên `bot.log` (chốt mốc sạch).
2. Restart GameServer → **Gọi 1000** → **BẬT đánh quái**.
3. Bấm **Dã Tẩu** một lần (50–100) và **bán sạp** một lần (20–50).
4. Chạy **≥30 phút** rồi đọc:

```bash
grep -a "\[BotDT\]\|\[BotSap\]" bot.log | tail -60
```

**Điều kiện đạt:**
- (a) có `TRA XONG nhiem vu loai N` → tiếp theo là `chon ruong thuong`;
- (b) nếu có bot nhận loại 4: có dòng `nhat cuon (n/N)` với **n > 0**;
- (c) `[BotSap]` có N dòng `mo sap` và **0 dòng** `bo mon khong phai do xanh`;
- (d) không còn bot đứng bất động ngoài rìa map / trong tường.

---

## 9 · CẠM BẪY (bắt buộc nhớ)

1. **`KPlayerBot.cpp` là ASCII thuần** nhưng `KItemList.cpp` / `KSubWorld.cpp` / `ScriptFuns.cpp`
   là **TCVN3** → chỉ sửa qua **python latin-1**, sau mỗi đợt **đếm byte ≥ 0x80 so với
   `git show HEAD:<file>`, phải khớp tuyệt đối**.
2. **EOL không đồng nhất**: `KPlayerBot.cpp` LF · `KSubWorld.cpp`/`KItemList.cpp` CRLF ·
   `ScriptFuns.cpp` **vùng đăng ký Lua là LF** · `seasonnpc.lua` **TRỘN** (CRLF + LF, đuôi LF).
   Luôn xem byte thật trước khi tạo chuỗi anchor.
3. **Chuỗi có backslash phải qua Write tool → chạy file**, không viết inline trong Bash.
4. **Cắt log theo TỪ KHÓA, không theo kích thước** — cắt 25 MB từng làm mất 10 phút cuối và
   sinh kết luận sai hoàn toàn.
5. **Log in TRƯỚC lời gọi không chứng minh gì.** Dòng `tra nhiem vu` cũ in trước
   `ExecuteScript`; nay phải đọc lại `course` SAU lệnh mới biết trả được hay không.
6. **Macro phải khai trước chỗ dùng** — `PB_DT_*` từng khai sau `PB_SetDaTau` (C2065).
7. `KNpc::m_nPlayerIdx` là **private** — muốn biết chủ của NPC thì truyền `nIdx` vào hàm.
8. Build: `MSBuild Core.vcxproj -p:Configuration="Server Release" -p:Platform=x64
   -p:SolutionDir="D:\GAMEDEVNEW\Sources\"`. **Đừng lọc output bằng `head`** — lỗi nằm ở cuối.
9. **PostBuild của Core chỉ chép sang `D:\GAMEDEVNEW\bin\server`** — sang cây E phải chép tay
   (rename bản cũ trước, md5 hai bên, grep chuỗi mới trong DLL).
10. **Phiên song song cùng build `Core.vcxproj`** — đừng so mtime, hãy grep chuỗi.

---

## 10 · PHƯƠNG PHÁP (giữ cho phiên sau)

Đợt cuối chạy **13 agent điều tra** (6 hướng độc lập → 6 phản biện đối kháng → tổng hợp),
2,3 triệu token, và **đáng tiền**: tìm ra 11 nguyên nhân độc lập của một triệu chứng duy
nhất, trong đó 7 nguyên nhân tôi đã bỏ sót khi tự đọc mã. Luật giữ nguyên từ phiên sáng:
**mọi kết luận phải trích được `file:line` đã thật sự đọc**; ai nói "nghi ngờ" mà không
chứng minh được bằng mã thì ghi nhận chứ không sửa theo.
