# BÀN GIAO — WAuto: 5 lỗi chủ game báo 14/09/2026

Commit `5411b57d` (đợt 1) + `a4787f35` + `962c6994` + `35aa446c` (đợt 4 — sau các manh mối của chủ game)
Nhánh `claude/wauto-auto-horse-bugs-ad45c9`.
`CoreClient.dll` md5 **`5801ca5a`** — đã đặt sẵn `CoreClient.dll.moi` ở
`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client`. Thoát hẳn `Game.exe` rồi chạy
`ChoiGame.bat`. `WAuto.exe` không đổi.

DLL đang chạy là `fe052ae2` (12/09); bản mới chứa trọn bản đó (từ 12/09 tới nay không
commit nào đụng mã client).

> ⚠️ **Đợt 1 của tôi sai gốc ở 2 trong 5 lỗi.** Chủ game bác đúng. Mục 1.1 và 1.5 dưới đây
> là kết luận **đã đo lại**, không phải suy luận. Giữ nguyên phần sai trong lịch sử commit
> để lần sau không dẫm lại.

---

## 1. Gốc từng lỗi

### 1.1 🔴 Ngựa lên xuống liên tục — và "Thiếu Lâm đánh skill đao bị ép xuống ngựa"

**Luật ngựa thật của game nằm ở DUY NHẤT cột `HorseLimit` của `settings\skills.txt`.**
`KSkill::CanCastSkill` ([KSkills.cpp:320](Sources/Core/Src/KSkills.cpp:320)) không đọc gì khác:
`1` = từ chối nếu đang cưỡi, `2` = từ chối nếu chưa cưỡi, `0` = **không ràng buộc gì**.
Bảng của client và của máy chủ **giống hệt nhau** (1683/1683 dòng, **0 dòng lệch HorseLimit**).

**Lỗi của bản cũ:** ô "Tự động" (`nSelFHorse == 0`) viết *"chiêu cần xuống thì xuống, **còn lại
thì LÊN ngựa**"*. Mà `skills.txt` chỉ có HorseLimit `0` (1372 dòng) và `1` (312 dòng) — không
dòng nào `= 2` — nên vế "còn lại" = **lên ngựa cho 81 % số chiêu**, quyết **theo từng khe**.
Bảng Chiêu KH trộn hai loại thì khe này kéo lên, khe kia kéo xuống. Khối đó lại **không có
phanh nào** → bắn `PA_RIDE` mỗi ~300 ms trong khi `TIME_RIDE` = 5 s → spam *"Bạn quá mệt mỏi..."*.

**Vá (đợt 3):** ô "Tự động" giờ **quét cả vòng chiêu tấn công chính** rồi mới quyết —
`WA_VongChieuDoiNgua()`, **bỏ chiêu chưa học** (nó không bao giờ bắn ra):

| Vòng chiêu tấn công chính | WAuto làm |
|---|---|
| có **ít nhất 1** chiêu đánh được trên ngựa (`HorseLimit 0` hoặc `2`) | **lên ngựa** |
| **mọi** chiêu đều `HorseLimit 1` | **xuống ngựa** (cưỡi thì không đánh được gì) |

"Vòng chiêu tấn công chính" = chiêu đang định bắn + chiêu chuột trái + 6 khe Chiêu KH + ô
"Đổi chiêu". **Không** tính cứu mạng / cứu mana / chiêu boss / tiền chiêu — chúng thỉnh thoảng
mới bắn, không được quyền trói ngựa cả trận. Đó chính là chỗ bản cũ ép xuống: các ô đó **đè lên**
`nMainSkill` trước khối ngựa, nên nhịp nào chúng bắn là nhịp đó bị kéo xuống, nhịp sau quay lại
chiêu đao thì kéo lên — không cần bảng Chiêu KH cũng đủ dao động.

> Đánh đổi: khe `HorseLimit 1` trong bảng Chiêu KH sẽ **không bắn** khi đang cưỡi. An toàn —
> `WA_ChieuBiCam` trả "bị cấm TẠM THỜI (lý do 2: ngựa)" và nơi gọi đặt `bBanRoi = true`
> *"coi như đã bắn để sang khe kế"* ([CoreShell.cpp:19949](Sources/Core/Src/CoreShell.cpp:19949)),
> **không** cấm chiêu 30 giây, không kẹt máy.

Quyết theo cả vòng chiêu nên **không thể** kéo lên rồi kéo xuống giữa hai khe. Giữ hai phanh:
`uHorseTime` 2 giây **và** đồng hồ thật `m_TimeHorse / TIME_RIDE`.

#### ✅ Chỗ ép xuống ngựa — ĐÃ TÌM RA: `TK_XuongNgua`, chỉ chạy trong Tống Kim

Manh mối của chủ game *"khi vào Tống Kim mới bị"* + *"vào Tống Kim WAuto dùng phần PK"* là thứ
khoá được vụ này.

`TK_XuongNgua()` ([CoreShell.cpp:8300](Sources/Core/Src/CoreShell.cpp:8300)) — **chỉ tồn tại trong
mạch Tống Kim**, đúng "chỉ bị khi vào Tống Kim" — xuống ngựa **vô điều kiện** mỗi lần "gặp địch",
**không hề hỏi** chiêu của nhân vật có đánh được trên ngựa không. Bốn chỗ gọi trong `TKP_FIGHT`:

| Gọi khi | Dòng |
|---|---|
| đang ôm mục tiêu (`ea.uNpcID`) | 11200 |
| đánh cùng mục tiêu với ắc chính | 11225 |
| địch săn vào Tầm nhìn PK + `NGUA_DICH_THEM` | 11260 |
| vừa chọn được địch, trước khi giao cho máy PK | 11274 |

Trong trận Tống Kim địch ra vào tầm nhìn liên tục ⇒ **xuống** → `DT_DuocLenNgua` cho **lên lại**
sau `NGUA_NGHI` 6 giây → gặp địch lại **xuống**… = đúng *"tự động lên xuống ngựa"*. Và người cầm
đao Thiếu Lâm (cả 11 chiêu đao đều `HorseLimit 0` — **đánh trên ngựa được**) thấy đúng như
*"đang đánh đao trên ngựa thì bị ép xuống"*.

Đây là **luật 03/09 của chính chủ game** — *"xuống ngựa chỉ khi gặp địch"* — đúng ở thời điểm đó,
nhưng nay chủ chốt lại: *"Thiếu Lâm đao tất nhiên là skill trên ngựa rồi"*.

**Vá:** `TK_XuongNgua` nhận thêm `pAp`, chỉ xuống khi **vòng chiêu tấn công chính thật sự đòi
xuống**. Vòng chiêu đánh được trên ngựa ⇒ **giữ nguyên trên ngựa**, và **không** ghi
`s_uNguaXuongT` (không có lần xuống nào để mà phải chờ 6 giây).
Hai ô "Xuống ngựa" của người chơi (tab Chiến đấu `nSelFHorse == 2`, tab PK `bPKDownHorse`)
**vẫn thắng thế** — tick là xuống, không tự đổi ý người chơi.

**Máy PK thì không có logic ngựa theo chiêu nào cả** — chỉ có ô `bPKDownHorse`, và ô đó cũng
xuống vô điều kiện mỗi 2 giây. Khối đó trước nay **câm tịt**, không soi được; nay thêm log
`[PK-NGUA]` + phanh `TIME_RIDE`. **Hành vi không đổi** (ô của người chơi).

> 🔎 Nghiệm thu nhanh khi đánh Tống Kim — lọc `jx_auto.log`:
> `[TK-NGUA] GAP DICH nhung GIU TREN NGUA` ⇒ đã đúng.
> `[PK-NGUA] o 'Xuong ngua' tab PK dang BAT` ⇒ thủ phạm là **ô cấu hình**, bỏ tick đi.

**Những thứ đã loại trừ bằng số đo (giữ lại để khỏi dò lại):**

1. **Mọi chiêu đao Thiếu Lâm / Thiên Vương đều `HorseLimit = 0`** — 11 chiêu: `6` · `19` · `24` ·
   `32` · `34` · `37` · `321` · `322` · `1058` · `1077` · `1084`. Bảng client và bảng máy chủ
   **giống hệt** (1683/1683 dòng, 0 dòng lệch). Game **không** cấm đánh trên ngựa.
2. `[SKILL-SET] combo=0` trong log 14/09 — bảng Chiêu KH **đang tắt**, nên câu chuyện "khe đao
   kéo lên, khe sau kéo xuống" ở đợt 1 là **sai**.
3. Log 14/09 **không có nhân vật Thiếu Lâm** — cả hai tiến trình đều dùng skill `372`
   *Ngạo Tuyết Tiêu Phong* (Côn Lôn, đao, `HorseLimit 1`); ở đó `[CHIEU-CAM] lý do 2` 13 lần
   chứng minh chính client từ chối chiêu khi đang cưỡi ⇒ xuống ngựa là **bắt buộc**, không phải lỗi.
4. Máy chủ **không bao giờ tự ép** — `CheckRideHorse` chỉ chạy khi client xin.

### 1.2 🔴 Về mở rương xong không tự tắt rương

Chạm rương ⇒ máy chủ chạy `OpenBox()` (`ruongchua.lua:59`) ⇒ `s2c_openstorebox` ⇒ client mở
`KUiStoreBox` + `KUiItem`. `grep GDCNI_FK_AUTO_ITEM CoreShell.cpp` = **0 kết quả** — WAuto chưa
bao giờ gửi lệnh đóng; auto có sẵn thì có (`KPlayerAuto.cpp:3695`, `nParam -2`).

**Vá:** `TK_DongRuong()` gọi đúng đường đó, đặt ở **cả ba lối ra** của pha `TKP_RUONG`.
Lối cất đồ của Hậu cần không mở cửa sổ này nên không dính.

### 1.3 🔴 Tống Kim xong, phe Kim chạy vào góc

Bản 12/09 đã loại trừ đúng tên NPC và lưới A*. Còn **hai đường**:

**(a) Rò rỉ biến đếm.** Bước "dọn túi" đầu pha `TKP_END` và bước "tìm Xa Phu" dùng **chung**
`ea.nTKTry`; nhánh thoát `++ea.nTKTry > 40` không đặt lại 0 ⇒ có lượt bước tìm Xa Phu bắt đầu
với `nTKTry = 41` và **bỏ cuộc ngay nhịp đầu**.

**(b) Bỏ cuộc = trả máy.** `nTKHold = 0` khi **còn đứng trên map 324** ⇒ `ATYPE_MOVE` chạy theo
toạ độ tab "Di chuyển" (toạ độ map farm, vô nghĩa ở 324) ⇒ **chạy vào góc**. Đây chính là triệu chứng.

**Vì sao chỉ phe Kim** (`lib_tktc.lua:798-801`, `battle_transprot`):

| Phe | Máy chủ thả ở ô | Xa Phu ở ô | Khoảng cách |
|---|---|---|---|
| Tống | 1541, 3178 | 1535, 3153 | ~823 mps |
| Kim | 1570, 3085 | 1568, 3075 | **~326 mps** |

`TK_ToiNpc` chỉ trả −1 (bắt đầu đếm bỏ cuộc) khi đã ở trong **320 mps** của mốc. Phe Kim vào đếm
gần như ngay; phe Tống còn cả 26 ô để danh sách NPC kịp đồng bộ.

**Vá:** đặt lại `nTKTry`; nới ngân sách 20 → 50 nhịp (8 → 20 giây); nới bán kính lối thoát
`TK_NpcThoaiGan` 96 → 160 mps; và **không trả máy khi còn trên map báo danh** — thử lại từ đầu,
dòng chat chặn 1 lần/phút.

> ⚠️ Hệ quả: nếu Xa Phu **thật sự** không có, nhân vật **đứng ở map 324** thay vì chạy đi.
> Tự lành ngay khi NPC đồng bộ xong. Log: `[TK-NPC] ... GIU MAY tren map bao danh`.

### 1.4 🔴 "Túi Hoạt Động" có trong danh sách mà lúc nhặt lúc không

**Gốc thật — không phải cái tôi vá ở đợt 1.**

`Túi Hoạt Động` có **`Genre = 6`** (`server\settings\item\magicscript.txt`:
`Name / Genre / DetailType` = `Túi Hoạt Động / 6 / 1`).

Mà pha **farm Dã Tẩu** ([CoreShell.cpp:7139](Sources/Core/Src/CoreShell.cpp:7139)) nhặt **mọi obj
`genre == 6`** trong 500 mps và **cố ý bỏ qua bộ lọc nhặt** — chú thích gốc ghi thẳng
*"kể cả khi bộ lọc nhặt của người chơi bỏ qua"* — lại **không ghi một dòng log nào**.

⇒ **trong pha farm Dã Tẩu thì nhặt, ngoài pha thì hai đường nhặt thường chặn đúng** =
đúng "lúc nhặt lúc không".

**Log 14/09 chứng minh:** tên đó có **7.621 dòng `PICK-NAME-IN`** và **0 dòng `PICK-TYPE-IN`** —
tức đường nhặt **thường** chưa hề cho nó lọt lần nào. Mọi lần chủ game thấy nó bị nhặt đều đi
qua khối genre 6 câm lặng kia.

**Vá:** pha farm Dã Tẩu tôn trọng danh sách "không nhặt" + thêm log `[DT-CUON]`.
Gom **cả ba** đường lọc tên vào một hàm `WA_ObjBiCamNhat()` để không bao giờ lệch nhau nữa.

### 1.5 Lỗi tên `" x <số lượng>"` (vá kèm — là lỗi thật, nhưng **không phải** lỗi 1.4)

Tên obj **trên đất** được máy chủ ghi kèm số lượng khi món chồng đống rơi thành chồng:
`sprintf(szName, "%s x %d", GetName(), GetStackNum())` — có ở **9 chỗ** (`KNpc.cpp:10368/10436/10609`,
`KItemList.cpp:3094/3459/4901/5478/5579`, `KItemCompound.cpp:1526`, `KJx2WarInfra.cpp:1727`), trong khi
danh sách lấy từ **tên trong túi**. ⇒ rơi 1 cái thì chặn, rơi ≥ 2 cái thì lọt.
`WA_TenObjTrungCam()` bỏ đuôi đó trước khi so.

**Sửa thêm phép đo:** dòng `PICK-SKIP-NAME` / `PICK2-SKIP-NAME` trước đây ghi **kể cả khi không
bỏ qua** (10.963 dòng "SKIP-NAME" / 4.949 món vẫn nhặt) — nay chỉ ghi khi thật sự bỏ qua.

---

## 2. Hai lỗ CÒN LẠI của danh sách đen — chưa vá, chờ chủ quyết

**L1 — Tiền không bao giờ chặn được.** `AddMoneyObj` gửi tên **rỗng** (`KObjSet.cpp:201`), client
giữ tên mặc định trong `settings\obj\ObjData.txt` dòng 268 = **chữ Hán GBK `一点钱`**. Muốn chặn
tiền phải thêm ô riêng, không dùng được danh sách tên.

**L2 — Auto "nhặt đồ" có sẵn trong game** (`m_bPickItem`, `KPlayer.cpp:11562`) nhặt sạch, **không
biết** danh sách đen của WAuto.

---

## 3. Phản biện (3 vòng)

| # | Soi ra | Kết luận |
|---|---|---|
| 1 | Phanh 2 giây ngắn hơn `TIME_RIDE` 5 giây ⇒ vẫn ăn dòng "quá mệt mỏi" | **Lỗi thật** → thêm điều kiện `GetTickCount() - m_TimeHorse >= TIME_RIDE` |
| 2 | Ngân sách tìm Xa Phu đo bằng `ea.uTKPhaseT` | **Lỗi thật**: bước dọn túi ăn tới 16 giây của chính đồng hồ đó → quay lại đếm nhịp |
| 3 | Giữ máy ⇒ dòng chat lặp mãi | → chặn 1 lần/phút |
| 4 | **Gốc lỗi nhặt đồ ở đợt 1 (`" x N"`) không khớp món chủ game nêu** | **Sai thật** → đo lại: Genre 6 + pha farm Dã Tẩu. Giữ cả hai bản vá |
| 5 | **Câu chuyện "khe đao kéo lên, khe sau kéo xuống" ở đợt 1** | **Sai thật**: log ghi `combo=0`, bảng Chiêu KH đang tắt → đổi sang quyết theo cả vòng chiêu |
| 6 | Quyết theo cả vòng chiêu có chọi với ô "Xuống ngựa" tab PK? | **Không**: máy PK và máy Chiến đấu không bao giờ chạy cùng một nhịp (`S3Client.cpp:1088` vs `:1125`) |
| 7 | `WA_TenObjTrungCam` bắt nhầm món khác? | **Không**: `"Kim"` vs `"Kim Sang"` → phần dư không đúng dạng `" x "`+số |
| 8 | `TK_DongRuong` đóng cả cửa sổ túi đồ | **Có** — đúng như auto có sẵn làm. Nhẹ, chỉ bắn lúc kết thúc pha rương |
| 9 | `CoreShell.cpp` có dùng chung với server? | **Không** — `ExcludedFromBuild` ở cả 4 cấu hình Server |

---

## 4. Nghiệm thu (lọc `jx_auto.log`)

| Lọc | Mong đợi |
|---|---|
| `[FIGHT-HORSE]` | in `rangbuoc=` + `chieuQD=`; **một trạng thái ổn định**, không lên xuống qua lại |
| `Ban qua met moi` | biến mất khỏi chat |
| `[DT-CUON] bo qua` | **Túi Hoạt Động phải hiện ở đây** khi farm Dã Tẩu |
| `[TK-PHA] 10 -> ` | rời pha rương ⇒ cửa sổ rương đóng ngay |
| `[TK-NPC]` | phe Kim: tên NPC được dùng, hoặc "GIU MAY" — **không còn** chạy vào góc |
| `PICK-SEND` | không được có tên nằm trong danh sách, kể cả dạng `"… x 2"` |
