# BÀN GIAO — WAuto: tab "Chiêu KH" + Tống Kim tự tìm địch (02–03/09/2026)

> Đọc mục **1** và **2** là đủ để tiếp tục. Mục 5 là chỗ dễ dẫm lại vết cũ nhất.

---

## 1. Trạng thái — đang chờ CHỦ GAME TEST

Toàn bộ khối việc này **chưa được test lần nào**. Chủ game nói nguyên văn 02/09:
*"tạm thời tôi chưa test được"*, và từ đó tới nay chỉ thêm việc chứ chưa có phản hồi test.

### Tệp chờ swap (đặt sẵn ở `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client`)

| Tệp | md5 | cỡ (byte) | Nội dung |
|---|---|---|---|
| `CoreClient.dll.moi` | `7efb5720` | 2.466.816 | commit `4d7b1df6` — gồm TẤT CẢ việc bên dưới |
| `WAuto.exe.moi` | `d0efa1f5` | 403.968 | giao diện: bỏ 3 ô cũ, thêm tab 13 "Chiêu KH" |

`Game.exe` (`0411771f`) **không đổi**, không cần đụng.

### Checklist swap

1. Thoát **hẳn** `Game.exe` **và** `WAuto.exe` (kiểm Task Manager, không còn tiến trình nào).
2. Chạy `ChoiGame.bat` — nó tự đổi tên `CoreClient.dll.moi` → `CoreClient.dll`.
3. `ChoiGame.bat` **KHÔNG** đổi `WAuto.exe.moi` (nó chỉ biết `WAuto.dll.moi`).
   Phải **đổi tay**: xoá/đổi tên `WAuto.exe` cũ rồi rename `WAuto.exe.moi` → `WAuto.exe`.
4. Mở WAuto, vào tab **"Chiêu KH"** (tab thứ 13) — nếu tab hiện ra là đã swap đúng
   `WAuto.exe`. Nếu không thấy tab đó thì bước 3 chưa xong.
5. Cấu hình cũ trong `APdata\<ID>.dat` **vẫn dùng được** — WAuto tự di trú (xem mục 5.3).

---

## 2. Việc cần làm ở phiên sau

| # | Việc | Ghi chú |
|---|---|---|
| 2.1 | **Chờ kết quả test của chủ game** | 5 mảng: bảng chiêu kết hợp, tiền chiêu, điều kiện Nộ/Âm Luật, Táp Đạp Lưu Tinh, Tống Kim tự tìm địch |
| 2.2 | **3 câu hỏi chờ chủ game quyết** | Xem mục 6 — đều là đổi hành vi người dùng thấy được nên **không tự làm** |
| 2.3 | Ổ "Mật khẩu rương" đang RỖNG | Nên bước "Cất đồ" của Hậu cần không chạy — việc cũ, xem `BANGIAO_HAUCAN_DUNGYEN_0209.md` |

---

## 3. Đã làm gì (theo commit)

Tất cả đã push lên `main`.

| Commit | Nội dung |
|---|---|
| `7cab3e26` | **Hậu cần đứng yên ở thành** — `FindPath` trả **2** (đích không tới được → ghi `m_nTargetX/Y` = ô TRUNG GIAN) trong khi 3 bước đi-tới-trạm đòi `HaveTarget` trùng từng đơn vị → huỷ bước 300 ms/lần. Vá bằng `g_uHomePath` + đi lại có tiết lưu. Chi tiết: `BANGIAO_HAUCAN_DUNGYEN_0209.md` |
| `748257c3` | **BỎ 3 ô** (kỹ năng tay trái / tay phải / tự vệ) + **THÊM tab 13 "Chiêu KH"**: bảng 6 khe bắn tuần tự (mỗi khe có "trễ (ms)" riêng) + tiền chiêu |
| `e9bd182b` | Chiêu kết hợp + tiền chiêu đọc **điều kiện tầng Nộ (1976) / Âm Luật (2116)** |
| `15c56493` | Ô chọn chiêu thiếu chiêu có hồi chiêu / chiêu ăn tầng — thêm danh sách `SkillKAr` |
| `d3166279` | Chiêu **AttackRadius = 0** (Táp Đạp Lưu Tinh 2118) làm máy đánh đi theo quái mãi không bắn |
| `c5660cd5` | **6 lỗi thật** do vòng phản biện đối kháng (33 phát hiện → 18 chốt) |
| `af2caa82` | "Đủ nộ rồi mà phải chờ lâu mới bắn" — ô "Hồi chiêu (giây)" không được chặn kiểu 3 |
| `3b23abe8` | Tống Kim: tầng săn `TK_SanNguoi` — tự tìm người/bot khác phe rồi tới đánh, thay cho việc chạy tới toạ độ sinh binh đoàn |
| `b02736bd` | Tống Kim **r2**: nhận địch bằng **CAMP (màu phe)** y như bot, bỏ phép kẹp tầm |
| `e32e68b2` | **Máy PK nhận cấu hình tab "Chiêu KH"** |
| `4d7b1df6` | **Vá 2 lỗi của khối chống-kẹt `[S9-BOMUCTIEU]`** — xem mục 5.7 |

---

## 4. Bản đồ mã (để khỏi phải mò lại)

Tất cả trong `Sources\Core\Src\CoreShell.cpp` trừ chỗ ghi khác.

| Thứ | Ở đâu |
|---|---|
| Biến tĩnh của Chiêu KH | ~391: `g_nComboKhe`, `g_uComboNghi`, `g_nComboSkillCuoi`, `g_uTCMuc`, `g_uTCLan`, `g_uTCHoiT`, `g_uNoGuard` |
| Cổng "chiêu này bắn được chưa" | `WA_ChieuSanSang()` ~12129 — đã học + hết hồi chiêu + **đủ tầng** |
| Máy đánh thường (tab Chiến đấu) | `case ATYPE_FIGHT` ~15383; chọn chiêu 15523‑15728; bắn + ghi sổ 15783‑15858 |
| Máy PK (tab PK) | `case ATYPE_PKFIGHT` ~15892; chống-kẹt S9 ~16083; chọn chiêu ~15963+; bắn ~16118+ |
| Vẽ vòng tầm đánh | `case ATYPE_DRAWVISION` ~15869 — **chỉ chạy ở chế độ PK** |
| Tầng săn Tống Kim | `TK_SanNguoi` / `TK_ChonDich` / `TK_SanBo` / `TK_XaBang` / `TK_ChonDiem` (grep) |
| Phát lệnh máy nào | `Sources\S3Client\S3Client.cpp:1031` (`!bOnPK` → FIGHT) và `:1061‑1084` (PK) |
| Struct cấu hình | `ipc_shared.h` — **3 bản y hệt** phải sửa cùng lúc (xem 5.3) |

### Log để kiểm khi test

| Lọc | Cho biết |
|---|---|
| `[COMBO-STATE]` | máy đánh **thường**: khe đang tới lượt, chiêu chọn, còn nghỉ mấy ms, tiền chiêu, **số tầng Nộ / Âm Luật đang có** |
| `[PK-COMBO]` | y như trên nhưng của máy **PK** (Tống Kim dùng máy này) |
| `[FIGHT-R0]` / `[PK-R0]` | chiêu tầm 0 bắn tại chỗ theo toạ độ |
| `[TK-SAN]` | tầng săn Tống Kim: `campminh=` / `campdich=` / `camp=` của mục tiêu — **đối chiếu thẳng với màu tên trong game** |
| `[HC-STATE]` | chu trình Hậu cần (phù về bán rác) |
| `[S9-BOMUCTIEU]` | máy PK bỏ mục tiêu vì "áp sát mãi không gần thêm" → **cấm 60 giây** |

---

## 5. BẪY đã đo — đừng dẫm lại

### 5.1 🔴 Chiêu **tầm 0** trong máy PK tự cấm sạch bãi chiến

Máy PK chỉ có hai phép so `nDist < nSkillRadius` (16128) và `nDist <= nSkillRadius` (16149).
Chiêu `AttackRadius = 0` — **Táp Đạp Lưu Tinh 2118** của Tiêu Dao — làm cả hai **sai vĩnh
viễn**. Tệ hơn: khối chống-kẹt `[S9-BOMUCTIEU]` ở **16086** dùng chung phép so đó nên hiểu
là "áp sát mãi không gần thêm" → sau 4 giây `m_mAutoExcludeNpcID[dwID] = +60000` rồi
`return 0`.

Ô cấm ấy được **ĐỌC bởi cả ba tầng Tống Kim**: `TK_ChonDich` (~7640), `TK_SanNguoi`
(~7752), `TK_XaBang` (~7920). Mỗi vòng khe đốt một đối thủ, mà một trận Tống Kim chỉ dài
vài phút.

**Đã vá** (`e32e68b2`): thêm nhánh bắn theo toạ độ `[PK-R0]`, và mở ngoại lệ cho watchdog.
⚠️ **Ngoại lệ ấy sau đó phải sửa lại** — viết `|| nSkillRadius <= 0` để **reset** đồng hồ là sai,
nó mở lại đúng căn bệnh `[FIX-6]` sinh ra để chữa. Bản đúng ở `4d7b1df6` — **xem mục 5.7**.

### 5.2 🔴 Ba luật khi bê logic từ `ATYPE_FIGHT` sang `ATYPE_PKFIGHT`

1. **Không bê `return 1`.** Máy PK **không dùng** `uFDelayTime`; nó có bộ đếm riêng
   `s_uS9ApID / s_uS9ApT / s_nS9ApD` (16083) nên lớp bù `uFDelayTime` của máy thường **vô
   nghĩa** ở đây. Cửa sổ nghỉ giữa 2 khe chỉ được **bọc đúng câu lệnh bắn**; nhánh
   `do_walk` / `do_run` phải nguyên vẹn, không thì đứng yên ngoài tầm → dính 5.1.
2. **Biến tĩnh `g_*` phải DÙNG CHUNG, đừng tách theo máy.** `g_uNoGuard` là phanh của
   **tài nguyên vật lý** (một thanh tầng Nộ/Âm Luật) — tách ra là bắn trên **số tầng CŨ**.
   `g_nComboKhe` chỉ là **điểm bắt đầu quét** chứ không phải con trỏ khe (15571‑15582 quét
   cả 6 khe). Một nhân vật chỉ có một bảng chiêu, và `S3Client.cpp:1031/1061` không bao giờ
   chạy hai máy trong cùng một nhịp.
3. **Mọi khối đè lên `nMainSkill`** (boss / LS / MS / Cast bùa / tiền chiêu) **phải hạ**
   `bComboCast = false; nKheBan = -1;` — không thì khe bị đẩy và bị áp "trễ (ms)" của một
   khe **thực ra chưa hề bắn**.

### 5.3 🔴 `ipc_shared.h` có **BA bản y hệt** + không được xoá trường giữa

Ba nơi phải sửa **cùng lúc**:
`E:\Src_Auto_Ngoai\WAuto\WAuto` · `D:\GAMEDEVNEW\Sources\Core\Src` · `D:\GAMEDEVNEW\WAutoUI`

12 trường Chiêu KH **thêm ở CUỐI** struct (7388 → 7472 byte). Ba trường cũ `nSkillIdL`
(@272), `nSkillIdR` (@276), `nSkillIdP` (@284) **giữ làm chỗ chết** — xoá một trường ở
giữa là dịch mọi offset phía sau và **phá sạch `APdata\<ID>.dat` của mọi người chơi**.
`LoadRoleData` di trú theo `offsetof(autoData, bCombo)`.

`WAuto.rc` và `WAuto.cpp` là **UTF-16LE**; `Resource.h` và `ipc_shared.h` là ASCII.

### 5.4 🔴 Tống Kim: camp là màu phe, và **không có tổ đội**

Đọc `bin\server\script\tinhnang\tong_kim_tcap\mobinhtk.lua` (script báo danh của **người
chơi**):

| hàm | camp | task |
|---|---|---|
| `common_tong` (393, 437) | `SetCurCamp(1)` | `T_CHECKPHETK = 1` |
| `common_kim` (323, 367) | `SetCurCamp(2)` | `T_CHECKPHETK = 2` |

⇒ **camp 1 = Tống, camp 2 = Kim**. Client nhận qua `NpcSync.CurrentCamp` (`KNpc.cpp:6343`),
và **chính camp đó quyết màu tên** (`KNpc.cpp:6930` — camp 1 cam, camp 2 hồng tím).

Cả hai hàm gọi `LeaveTeam()` + `SetCreateTeam(0)` ngay lúc báo danh (321‑322, 391‑392)
⇒ **Tống Kim KHÔNG có tổ đội**. Đừng lấy `GetRelation` để né camp — nỗi lo "tổ đội ghi đè
camp" là bịa (tôi đã sai chỗ này một vòng build).

**Nguyên lý của bot** (`KPlayerBot.cpp`): `pb_TkTimDichGanNhat` lọc
`Npc[nn].m_CurrentCamp != nCampDich` (9699), **không giới hạn tầm**, bỏ `m_CurrentLife <= 0`.
Di chuyển: đổi đích thì đi **thẳng** (10471); tiết lưu `[TK-KET2]` — mục tiêu cũ phải dịch
**> 8 ô VÀ qua 3 giây** mới nhắm lại (10455‑10457). Thiếu tiết lưu này thì mỗi nhịp một
điểm đích khác, `DT_WalkTo` tính lại đường liên tục → **nhân vật giật tại chỗ**.

Khác biệt **không tránh được**: client `MAX_PLAYER = 2` (`KPlayerDef.h:21`) nên không port
được vòng quét `Player[1..1500]` của bot — phải quét `Npc[]` qua `NpcSet.GetNextIdx`.
Không cần kẹp tầm: máy chủ vốn ngừng gửi gói NPC cách ≥ 40 ô (`S6_XaQuaTam`,
`KProtocolProcess.cpp:107`).

### 5.5 Phân công hai tab (chủ game chốt 03/09)

> *"Hệ thống di chuyển xác định mục tiêu thì phần tab tống kim — còn quét phạm vi quái thì
> tab pk sẽ chịu trách nhiệm"*

Tab **Tống Kim** = tìm mục tiêu + di chuyển. Tab **PK** = quét phạm vi + đánh.
Thứ tự chọn chiêu trong máy PK, chủ game chốt: **Cast bùa > Chiêu KH > bỏ ô "Đổi chiêu"**.

### 5.7 🔴 Watchdog và phép bắn phải dùng **chung một cái thước**

Hai lỗi vá ở `4d7b1df6`, cùng một gốc: khối `[S9-BOMUCTIEU]` đo bằng một bán kính, phép
bắn đo bằng bán kính khác.

1. **Phép kẹp tầm phải đứng TRƯỚC watchdog.** Khối kẹp xuống `max(75, nPKNearDist)` vốn
   nằm **bên trong** nhánh `bPKFollowTG`, tức **sau** watchdog. Chiêu tầm 400 + ô "Tiếp cận"
   bật (kẹp còn 75) + đứng cách 200 ⇒ phép bắn thấy `200 >= 75` nên **đi bộ**, watchdog thấy
   `200 < 400` nên **xoá cờ mỗi nhịp** ⇒ 4 giây không bao giờ đủ ⇒ đâm tường đuổi vĩnh viễn.
   Máy đánh thường không dính vì nó kẹp **trước** mọi phép so.
2. **Nhịp chiêu tầm 0 = nhịp KHÔNG TÍNH, không phải nhịp RESET.** Nhánh `[PK-R0]` bắn tại
   chỗ theo toạ độ, **không hề có** `do_walk`/`do_run` — nó không đóng góp gì vào việc áp sát
   nên coi là "đánh được rồi" là sai. **797/1684 dòng `skills.txt` có `AttackRadius <= 0`**
   (261 dòng có `StateSpecialId != 0` = loại hay nhét vào ô Cast bùa) nên xác suất dính cao.
   Để nhánh **rỗng** là đúng cả hai đầu: đồng hồ là **thời gian thật** (`uCurTime - s_uS9ApT`)
   chứ không đếm số nhịp, nên đóng băng sổ sách ở nhịp tầm 0 không làm dừng đồng hồ.

Chuỗi trạng thái đúng, 5 nhánh: `trong tầm → reset` / `tầm 0 → không tính` /
`đổi mục tiêu → nạp cờ` / `gần thêm ≥ 1 ô → gia hạn` / `quá 4 giây → cấm 60 giây + return 0`.

⚠️ **Tác dụng phụ đã báo chủ:** sau vá 1, ở cấu hình **"Tiếp cận" BẬT** + chiêu tầm xa,
watchdog **sẽ nổ** — việc trước nay nó chưa từng làm. Đuổi một người chạy trốn cùng tốc độ
mà 4 giây không rút được 1 ô sẽ bị **cấm 60 giây**. Ở cấu hình đó bot **đang không gây sát
thương** (nó đi bộ, không bắn) nên đổi mục tiêu là lời — nhưng trận thưa người sẽ thấy bot
"bỏ đi" 60 giây. Ô "Tiếp cận" **mặc định TẮT** nên phần lớn người chơi không thấy gì đổi.

### 5.6 Bẫy chung của dự án (nhắc lại)

- **CẤM** Edit/Write tool trên tệp nguồn JX1 — dùng `python io.open(..., encoding='latin-1',
  newline='')` hoặc `safe_edit.py` / `vn_edit.py`. Sau mỗi đợt: `check_encoding.py`,
  **high byte phải không đổi, FFFD = 0**.
- Công cụ vá phải có `main()` guard.
- Build: `MSBuild Core\Core.vcxproj -p:Configuration="Client Release" -p:Platform=Win32
  -p:SolutionDir="D:\GAMEDEVNEW\Sources\" -m:1`
- **Hai phiên build song song cùng cây = build nuốt bản dở dang.** Nếu có phiên khác đang
  sửa, build ở `git worktree` riêng.

---

## 6. BA CÂU HỎI CHỜ CHỦ GAME QUYẾT (không tự làm)

Cả ba đều **đổi hành vi người dùng thấy được**, nên theo luật 03/09 phải hỏi trước.

**Q1 — Ô "Cast bùa" nhét chiêu tự buff thì bot đứng im buff mãi.**
Khối CS1/CS2/CS3 chỉ kiểm *"mục tiêu đã dính state chưa"*, **không** kiểm đã học / hồi chiêu.
Với chiêu buff lên **chính mình**, mục tiêu vĩnh viễn không mang state đó ⇒ `bCastState = true`
mọi nhịp ⇒ bot đứng im buff mãi. Sửa đúng: bắt CS kiểm state **trên bản thân** với chiêu tự
thân, hoặc chặn chiêu `AttackRadius <= 0` khỏi ô Cast bùa.

**Q2 — Tạp Đạp Lưu Tinh 2118 đốt tầng Âm Luật không công.**
`DoBlurMove` trả FALSE khi `nWantLength <= MIN_DOMELEE_RANGE` (20, `KNpc.cpp:2108`) hoặc bị
chắn ngay mấy bước đầu (2131‑2135) ⇒ không lướt, không sát thương — nhưng `HS_SpCost` đã
trừ ở `KNpc.cpp:2699`, **trước** chỗ kiểm `CastMeleeSkill(pSkill) == FALSE` (2869).
`WA_ChieuSanSang` chỉ kiểm "đủ tầng" nên nó chọn lại ngay khi đủ ⇒ lặp. Vá nhẹ nhất: lọc khe
khi `nDist <= 20`.

**Q3 — `TK_ChonDich` giao lại mục tiêu mỗi 300 ms mà không có khoá.**
`TK_SanNguoi` thì có (chỉ đổi khi con mới gần hơn 25%). Đổi mục tiêu liên tục cũng làm
watchdog nạp lại cờ. Chép khuôn khoá sang là việc độc lập.

---

## 7. Phản biện đã chạy — kết quả để tham khảo

| Đợt | Quy mô | Kết quả |
|---|---|---|
| 02/09 — bảng Chiêu KH | 33 phát hiện → 18 chốt | **6 lỗi thật**, vá ở `c5660cd5`. Nặng nhất: `return 1` trong cửa sổ nghỉ **cắt cả nhịp** → chặn chiêu cứu mạng, chặn đánh trả / Thổ Địa Phù, chặn cả áp sát — đúng lớp lỗi "vòng lặp im lặng" vừa vá ở Hậu cần |
| 03/09 — `CK-PK-03` trên mã đã vá | 8 tác tử | **2 lỗi thật**, vá ở `4d7b1df6` — xem mục 5.7. Một trong hai là **hồi quy của chính tôi sáng cùng ngày**: ngoại lệ `|| nSkillRadius <= 0` thêm ở `e32e68b2` để chữa một bệnh đã **mở lại bệnh khác** |
| 03/09 — máy PK nhận Chiêu KH | 32 tác tử, 20 rủi ro → 8 đem cãi | **2 sống sót**, cả hai đã xử trước khi vá (mục 5.1 và 5.2‑1). 6 cái bị bác ≥2/3 — **toàn bộ đề nghị "tách riêng biến `g_*` cho PK" đều bị bác** |

**Luật rút ra:** phản biện nội bộ **không thay được một câu hỏi cho chủ game**. Tác tử chỉ
đọc được mã; chủ game biết **luật chơi thật**. Rủi ro nào thuộc luật chơi (tổ đội, thể
thức, phe, thứ tự ưu tiên) thì **HỎI CHỦ, đừng tự né**.
