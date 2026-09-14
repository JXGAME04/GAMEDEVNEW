# BÀN GIAO — WAuto: 5 lỗi chủ game báo 14/09/2026

Commit `5411b57d` · nhánh `claude/wauto-auto-horse-bugs-ad45c9`
`CoreClient.dll` mới md5 **`47f06ab4`** (2.672.128 byte) — **đã đặt sẵn** `CoreClient.dll.moi`
ở `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client`.

DLL đang chạy là `fe052ae2` (bản 12/09). Bản mới **chứa trọn** bản đó: từ 12/09 tới nay
không commit nào đụng mã client (14 commit sau đều là NET/Bishop phía máy chủ).

## Cách swap

1. Thoát **hẳn** `Game.exe` (kiểm Task Manager).
2. Chạy `ChoiGame.bat` — nó tự đổi `CoreClient.dll.moi` → `CoreClient.dll`.
3. `WAuto.exe` **không đổi** (lần này không sửa gì bên WAuto).

---

## 1. Gốc của từng lỗi + cách vá

### 1.1 + 1.2 🔴 Ngựa lên xuống liên tục / "Thiếu Lâm đánh skill đao trên ngựa bị ép xuống"

Hai gạch đầu dòng này là **một lỗi**.

Ô **"Tự động"** (`nSelFHorse == 0`) ở tab Chiến đấu viết ngược:

```cpp
if(pSkill->IsNeedDownHorse())  { nếu đang cưỡi  -> XUỐNG }
else                           { nếu chưa cưỡi -> LÊN  }   // <-- vế này sai
```

Đo trên `settings\skills.txt`: cột `HorseLimit` **chỉ có hai giá trị** — `0` (1372 dòng =
không ràng buộc) và `1` (312 dòng = bắt buộc xuống). **Không một dòng nào bằng 2.**
Nên vế `else` = **LÊN NGỰA cho 81 % số chiêu**.

Bảng Chiêu KH của Thiếu Lâm trộn hai loại:

| Chiêu | Id | EqtLimit | HorseLimit | Máy làm gì (bản cũ) |
|---|---|---|---|---|
| Ma Ha Vô Lượng | 19 | 1 (đao) | 0 | **LÊN ngựa** |
| Vô Tâm Trảm | 32 | 1 (đao) | 0 | **LÊN ngựa** |
| Kinh Lôi Trảm | 34 | 1 (đao) | 0 | **LÊN ngựa** |
| Bát Phong Trảm | 37 | 1 (đao) | 0 | **LÊN ngựa** |
| Kim Cang Phục Ma | 10 | −2 | 1 | **XUỐNG ngựa** |
| Hoành Tảo Lục Hợp | 11 | 2 (côn) | 1 | **XUỐNG ngựa** |
| Long Trảo Hổ Trảo | 17 | −2 | 1 | **XUỐNG ngựa** |
| Trảm Long quyết | 29 | 4 | 1 | **XUỐNG ngựa** |

⇒ khe đao kéo **lên**, khe kế tiếp kéo **xuống** — đúng triệu chứng "tự động lên xuống ngựa",
và người cầm đao thấy "đang đánh trên ngựa thì bị ép xuống" (chiêu đao vốn dùng trên ngựa được).

Nhánh này còn **không có phanh nào**: bắn `PA_RIDE` mỗi nhịp ~300 ms trong khi `TIME_RIDE = 5000` ms
⇒ spam dòng *"Bạn quá mệt mỏi, không thể tiếp tục lên xuống ngựa!"* (`CoreShell.cpp:23636`).

**Vá:** chép đúng khuôn của **máy auto có sẵn trong client** — `KPlayerAuto.cpp:1732`, chế độ `F_Auto`:

```cpp
if (pISkill->GetHorseLimit() && m_bFightSelect == F_Auto)   // HorseLimit == 0 -> KHÔNG ĐỤNG NGỰA
```

Thêm **hai phanh**: `uHorseTime` 2 giây (giống hai ô "Lên ngựa"/"Xuống ngựa") **và** đồng hồ thật
`m_TimeHorse / TIME_RIDE` (giống `DT_Ride`, `TK_XuongNgua`, `KPlayer.cpp:12661`) ⇒ hết spam.

> ⚠️ **Đổi hành vi người dùng thấy được:** ô "Tự động" **không còn tự leo lên ngựa** khi đánh.
> Ai muốn như cũ thì chọn mục **"Lên ngựa"** ở chính ô đó. Nếu chủ muốn khác, nói một câu là đổi.

### 1.3 🔴 Về mở rương xong không tự tắt rương

Chạm vào rương ⇒ máy chủ chạy `OpenBox()` (`ruongchua.lua:59`) ⇒ gói `s2c_openstorebox`
⇒ client **mở** `KUiStoreBox` + `KUiItem` (`GameSpaceChangedNotify.cpp:557-560`).

`grep GDCNI_FK_AUTO_ITEM CoreShell.cpp` ⇒ **0 kết quả**: WAuto chưa bao giờ gửi lệnh đóng.
Máy auto có sẵn thì có: `CoreDataChanged(GDCNI_FK_AUTO_ITEM, 0, -2)` (`KPlayerAuto.cpp:3695`).

**Vá:** thêm `TK_DongRuong()` gọi đúng đường đó, đặt ở **cả ba lối ra** của pha `TKP_RUONG`
(không cất đồ / cất xong / rời pha bằng `TK_Pha` khi bị kéo map hoặc quá hạn pha).
Không thêm lệnh UI mới, không đụng protocol.

> Lối cất đồ của **Hậu cần** không mở cửa sổ này (nó chỉ gửi mật khẩu rương + `GOI_EXCHANGEITEM`),
> nên chỉ mạch Tống Kim dính.

### 1.4 🔴 Tống Kim xong, phe Kim chạy vào góc thay vì tới Xa Phu

Bản vá 12/09 (`2b76154a`) đã loại trừ đúng: tên NPC khớp cả hai phe, lưới A* khu Kim thông,
`TK_GheVongKim` đã vô hiệu từ 26/08. Nhưng còn **hai đường** chưa bịt:

**(a) Rò rỉ biến đếm.** Bước "dọn túi" đầu pha `TKP_END` và bước "tìm Xa Phu" dùng **chung**
`ea.nTKTry`. Nhánh thoát `++ea.nTKTry > 40` đặt `nTKStep = 2` mà **không** đặt lại `nTKTry = 0`
⇒ có lượt bước tìm Xa Phu bắt đầu với `nTKTry = 41` và **bỏ cuộc ngay ở nhịp đầu** thay vì chờ 8 giây.

**(b) Bỏ cuộc = trả máy.** `ea.nTKHold = 0` khi **vẫn đang đứng trên map báo danh** ⇒ `S3Client`
thả `ATYPE_MOVE` chạy theo toạ độ tab **"Di chuyển"** (toạ độ của map farm, vô nghĩa ở map 324)
⇒ nhân vật chạy thẳng vào **góc bản đồ**. Đây **chính là** triệu chứng.

**Vì sao chỉ phe Kim** (đo từ `lib_tktc.lua:798-801`, hàm `battle_transprot`):

| Phe | Máy chủ thả ở ô | Xa Phu ở ô | Khoảng cách |
|---|---|---|---|
| Tống | 1541, 3178 | 1535, 3153 | **~823 mps** |
| Kim | 1570, 3085 | 1568, 3075 | **~326 mps** |

`TK_ToiNpc` chỉ trả −1 (bắt đầu đếm bỏ cuộc) khi đã **ở trong 320 mps** của mốc. Phe Kim rơi vào
đếm gần như ngay lập tức; phe Tống còn cả quãng 26 ô để danh sách NPC kịp đồng bộ. Cùng một lỗi,
chỉ phe Kim đủ gần để lộ.

**Vá:**
- đặt lại `nTKTry = 0` ở nhánh dọn túi bỏ cuộc;
- nới ngân sách 20 → **50 nhịp** (8 → 20 giây);
- nới bán kính lối thoát `TK_NpcThoaiGan` 96 → **160 mps** (5 ô; hai NPC thoại gần nhau nhất
  quanh mỗi mốc vẫn cách ≥ 12 ô nên không sợ bấm nhầm);
- **quan trọng nhất:** khi vẫn đứng trên map báo danh thì **không trả máy** — thử lại từ đầu.
  Đứng yên cạnh Xa Phu còn hơn chạy vào góc. Dòng chat chặn 1 lần/phút.

> ⚠️ Hệ quả: nếu Xa Phu **thật sự** không có, nhân vật sẽ **đứng ở map 324** thay vì chạy đi.
> Tự lành ngay khi danh sách NPC đồng bộ xong. Nếu chủ thấy thà chạy đi còn hơn đứng, nói một câu.
> Log để soi: `[TK-NPC] TKP_END khong thay Xa Phu sau 20 giay - GIU MAY ...`

### 1.5 🔴 Vật phẩm trong danh sách đen "lúc nhặt lúc không"

Bộ lọc so `strcmp` **đúng bằng** giữa `Object[].m_szName` (tên obj **trên đất**) và tên người chơi chọn.
Hai tên **không cùng nguồn**:

- danh sách lấy từ **tên trong túi** (`ATYPE_GETITEMNAME` → `Item[].GetName()`) — không có đuôi;
- tên obj trên đất được máy chủ ghi **kèm số lượng** khi món chồng đống rơi thành chồng:

```cpp
if (GetGenre() != item_equip && GetStackNum() > 1)
    sprintf(szName, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
```

Có ở **tất cả** đường rơi đồ: `KNpc.cpp:10368 / 10436 / 10609`,
`KItemList.cpp:3094 / 3459 / 4901 / 5478 / 5579`, `KItemCompound.cpp:1526`, `KJx2WarInfra.cpp:1727`.

⇒ cùng một món: rơi **1 cái** → tên trùng → bỏ qua; rơi **≥ 2 cái** → tên thành `"X x 3"` →
**không trùng** → **vẫn nhặt**. Đúng "lúc nhặt lúc không".

**Vá:** `WA_TenObjTrungCam()` — trùng khi bằng nhau **hoặc** bằng nhau sau khi bỏ đuôi
`" x <chữ số>"`. Áp cho **cả hai** lượt lọc (`ATYPE_PICKUP` và lượt "theo đuổi nhặt").
Không sợ nhầm món khác: phải trùng hết phần đầu **và** phần còn lại đúng dạng `" x "` + toàn chữ số.

**Sửa thêm phép đo:** dòng `PICK-SKIP-NAME` / `PICK2-SKIP-NAME` trước đây ghi **kể cả khi
không bỏ qua**. Log 14/09: **10.963 dòng "SKIP-NAME"** trong khi **4.949 món vẫn được nhặt** —
không thể soi ra lọc sai. Nay chỉ ghi khi thật sự bỏ qua.

---

## 2. Ba lỗ CÒN LẠI của danh sách đen — chưa vá, chờ chủ quyết

Đều là **đổi hành vi người dùng thấy được** nên không tự làm.

**L1 — Tiền không bao giờ chặn được.** `AddMoneyObj` gửi tên **rỗng** (`KObjSet.cpp:201`), client
giữ tên mặc định trong `settings\obj\ObjData.txt` dòng 268 = **chữ Hán GBK `一点钱`**. Không ai gõ
được chuỗi đó vào danh sách. (Có tài khoản đã thử: `APdata\3054654269.dat` có mục "Tiền đồng".)
Muốn chặn tiền thì phải thêm một ô riêng, không dùng được danh sách tên.

**L2 — Pha farm Dã Tẩu cố tình bỏ qua bộ lọc.** `CoreShell.cpp:7149` nhặt mọi obj `genre == 6`
(bí kíp) trong 500 mps, **không** hỏi danh sách đen — có chú thích "kể cả khi bộ lọc nhặt của
người chơi bỏ qua". Nếu món trong danh sách là bí kíp thì lúc farm Dã Tẩu vẫn bị nhặt.

**L3 — Auto "nhặt đồ" có sẵn trong game.** Nếu bật ô nhặt đồ của bảng auto trong game
(`m_bPickItem`, `KPlayer.cpp:11562`) thì bộ máy đó nhặt sạch, **không biết** danh sách đen của WAuto.

---

## 3. Phản biện bản vá (2 vòng, tự soi)

| # | Rủi ro soi ra | Kết luận |
|---|---|---|
| 1 | Phanh 2 giây **ngắn hơn** `TIME_RIDE` 5 giây ⇒ vẫn còn cửa sổ 2–5 s bắn `PA_RIDE` và ăn dòng "quá mệt mỏi" | **Lỗi thật** → vòng 2 thêm điều kiện `GetTickCount() - m_TimeHorse >= TIME_RIDE` |
| 2 | Ngân sách tìm Xa Phu đo bằng `ea.uTKPhaseT` | **Lỗi thật**: bước dọn túi đầu pha ăn tới 16 giây của chính đồng hồ đó → vòng 2 quay lại đếm nhịp (`< 50`), đã chống rò rỉ ở vòng 1 |
| 3 | Giữ máy ⇒ dòng chat lặp mãi | **Đúng** → chặn 1 lần/phút bằng `s_uTKXaPhuBao` |
| 4 | Bỏ "lên ngựa khi HorseLimit 0" có sinh xung đột mới với `ATYPE_RETURN` (Hậu cần luôn lên ngựa mỗi 2 s)? | **Không**: xung đột chỉ có với chiêu HorseLimit 1, mà nhánh đó **đã** xuống ngựa từ trước — hành vi không đổi, chỉ thêm phanh nên **bớt** dao động |
| 5 | Ô "Lên ngựa" (`nSelFHorse == 1`) có chọi với nhánh mới? | **Không**: nhánh mới chỉ chạy khi `nSelFHorse == 0`, hai ô loại trừ nhau |
| 6 | `WA_TenObjTrungCam` bắt nhầm món khác? | **Không**: `"Kim"` vs `"Kim Sang"` → phần dư `" Sang"` không đúng dạng `" x "`+số → trả false |
| 7 | `TK_DongRuong` đóng cả cửa sổ **túi đồ** của người chơi | **Có**, đúng như máy auto có sẵn làm. Nhẹ, chỉ bắn lúc kết thúc pha rương. Báo để chủ biết |
| 8 | Vá `TK_Pha` gọi `TK_DongRuong` khi **vào** pha rương? | **Không**: điều kiện `ea.nTKPhase == TKP_RUONG && nPha != TKP_RUONG` chỉ đúng lúc **rời** pha |
| 9 | `CoreShell.cpp` có phải tệp dùng chung với server? | **Không** — `ExcludedFromBuild` ở cả 4 cấu hình Server. Client-only, không cần dựng server |

**Điểm còn mù, chủ soi giúp khi test:**
Lỗi 1.4 mới bịt được **đường dẫn tới** triệu chứng, chưa chứng minh được **vì sao** `DT_FindNpcName`
trượt tên ở khu Kim (tên "Xa phu Kim quân" **có** chứa "xa phu", lưới A* thông). Nếu sau bản này
vẫn còn, log `[TK-NPC]` sẽ nói thẳng: tên NPC thật được dùng thay, hoặc NPC gần nhất trong 640 mps.

---

## 4. Nghiệm thu (lọc `jx_auto.log`)

| Lọc | Mong đợi |
|---|---|
| `[FIGHT-HORSE]` | chỉ hiện với `limit=1` (hoặc 2). Chiêu đao `limit=0` **không được** xuất hiện dòng nào |
| `Ban qua met moi` | biến mất khỏi khung chat |
| `[TK-PHA] 10 -> ` | rời pha rương ⇒ cửa sổ rương đóng ngay |
| `[TK-NPC]` | phe Kim: thấy tên NPC được dùng, hoặc "GIU MAY tren map bao danh" — **không còn** chạy vào góc |
| `PICK-SKIP-NAME` | số dòng phải **xấp xỉ** số món thật bị bỏ qua (trước đây gấp ~2,2 lần) |
| `PICK-SEND ... x 2` | **không được** có: món chồng đống trong danh sách phải bị chặn |
