# BÀN GIAO — WAuto Tống Kim: LƯU CẤU HÌNH NGAY + VỀ THÀNH → RƯƠNG (06/09/2026 tối)

> Chủ game báo: *"khi tống kim xong về thành với lưu rương chưa hoạt động · về không đúng thành ·
> chưa lưu rương · cần bạn đọc hết lại WAuto phần lưu - tôi muốn khi nào config mới là auto tự lưu liền"*.

Tiếp nối `BANGIAO_WAUTO_TK_RUONG_0409.md` (tính năng rương 04/09 chưa từng chạy được vì lỗi ở mục 2.1).

---

## 1. Trạng thái — CHỜ SWAP, ba tệp `.moi` ở `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client`

| Tệp | md5 | cỡ (byte) | Nội dung |
|---|---|---|---|
| `WAuto.exe.moi` | `807e4007` (sha256 `739560f1`) | 465.920 | **lưu ngay** mọi ô tick / ô chọn + mặc định 3 trường rương TK (mục 2) |
| `CoreClient.dll.moi` | `9cb2f51f` (sha256 `1f25d254`) | 2.578.432 | build từ `main` `95317dc4` = **bản của phiên Vận tiêu 18:13** (`59161dae`, đã cất thành `CoreClient.dll.moi.vtcn_1813_59161dae`) **+** vá về thành + nhật ký (mục 3) |
| `Game.exe.moi` | `3da2169e` (sha256 `f0b16230`) | 1.481.216 | của phiên Vận tiêu, bản **18:43** (cấp 90 + bài hướng dẫn), thay bản 18:21 `fb443d6d` — **không đụng** |

Bản đang chạy: `CoreClient.dll` `b166ec89` · `Game.exe` `a4083afa` · `WAuto.exe` `66b9856b`.

### Cách đổi

1. Thoát hẳn `Game.exe` **và** `WAuto.exe`.
2. Chạy `bin\client\ChoiGame.bat` — bản mới của bat (06/09) đổi **cả** `WAuto.exe.moi`, không phải đổi tay nữa.
3. Mở WAuto → nhóm **Hoạt động** → tab **TK** → **tick lại một lần** `Về thành xong thì tới rương`,
   chọn `Rương cửa`, tick `Cất trang bị trong túi vào rương`. Lý do: tệp `APdata\2513089250.dat`
   hiện có (7.640 byte) đang ghi `bTKRuong=0, nTKRuongHuong=0 (Trung tâm), bTKRuongCat=0` vì bản
   WAuto cũ chưa bao giờ lưu được ba ô này. Từ bản mới, bấm là lưu ngay.

---

## 2. WAuto — vì sao "lưu rương chưa hoạt động" và "config mới không tự lưu"

### 2.1 Gốc (đã chứng minh bằng tệp cấu hình)

`SaveRoleData` (đọc **toàn bộ** giao diện → `gnode.apdata` → ghi `APdata\<ID>.dat`) chỉ được gọi từ
các khối `case IDC_...:` **liệt kê tay** trong `WM_COMMAND`. Hai ô tick thêm ngày 04/09 —
`IDC_CHECKBOX_9_RUONG` (475) *Về thành xong thì tới rương* và `IDC_CHECKBOX_9_RCAT` (478) *Cất trang
bị* — **không có** trong danh sách đó (combo `IDC_COMBO_9_RH` 477 thì có). Hậu quả:

- bấm ô không ghi `.dat`, và **không cập nhật `gnode.apdata`** — chính là khối được `memcpy` vào
  `IPCGameLoop` gửi sang game mỗi 54 ms → CoreClient luôn nhận `bTKRuong = 0` → về tới thành là trả
  máy, không bao giờ sang pha `TKP_RUONG`;
- chọn lại nhân vật / game kết nối lại → `LoadRoleData` + `UpdateUI` trả ô tick về 0.

Bằng chứng: `APdata\2513089250.dat` ghi lúc 18:05:44 (nhân vật đang test): `bTongKim=1`,
`nTKVeThanh=4` (Tương Dương Phủ), **`bTKRuong=0`**, `nTKRuongHuong=0`, `bTKRuongCat=0`.
(Đọc bằng bộ phân tích struct `autoData` theo `ipc_shared.h`, `sizeof = 7.640`.)

### 2.2 Sửa (bộ vá `ReverseTools\goi_va_wauto_luu_ngay_0609.py`, commit `6527b8be`)

1. **Khối lưu chung** đặt ngay trước `switch (LOWORD(wParam))` của `WM_COMMAND`: bất kỳ control có ID
   trong dải cấu hình `[IDC_STRING_0_L, IDC_INDEX_END)` = [161, 700) mà là **Button kiểu checkbox /
   radio** (`BN_CLICKED`, kiểm `GWL_STYLE & BS_TYPEMASK`) hoặc **ComboBox** (`CBN_SELCHANGE`, kiểm
   `GetClassNameW`) → `SaveRoleData` ngay. Nút bấm thường bỏ qua. `CheckDlgButton` /
   `ComboBox_SetCurSel` do mã gọi không phát thông báo nên không lặp. Ô nhập chữ đã có cơ chế sẵn
   (EN_CHANGE hẹn 750 ms + EN_KILLFOCUS, 27/08). Các khối `case` cũ vẫn giữ — chúng gọi
   `SaveRoleData` lần hai, vô hại. **Từ nay thêm ô mới không còn phải nhớ liệt kê `case`.**
2. `LoadRoleData`: đặt mặc định `bTKRuong=0 / nTKRuongHuong=5 (gần nhất) / bTKRuongCat=1` ở cả nhánh
   *chưa có tệp* lẫn nhánh *tệp cũ ngắn hơn* (`uOldSize <= offsetof(autoData, bTKRuong)`). Trước đây
   `memset(0)` ở đầu hàm xoá hết giá trị của constructor `autoData()` nên hai ô này ra 0 — trái với
   bàn giao 04/09.

Kiểm toán bằng script (281 control trong `.rc` đối chiếu `Resource.h` + `WAuto.cpp`): ngoài hai ô trên,
mọi checkbox/combo khác đều đã có `case`; sau vá, khối chung bao trùm tất cả.

---

## 3. CoreClient — "về không đúng thành"

### 3.1 Không còn vết của trận chủ báo

`jx_auto.log` được **tạo lại mỗi lần mở game** (18:05:26 hôm nay) và **xoay vòng khi ~17 MB**
(18:28 mất 470.000 dòng); log của trận chủ than phiền (phiên 15:42) đã mất. `TK_Msg` / `TK_Pha`
chỉ hiện chat, không ghi log → kể cả còn log cũng không đọc được pha. **Đã thêm** `AUTOLOG`
`[TK-MSG]` (mọi thông báo Tống Kim), `[TK-PHA] cũ -> mới map=… t=…`, `[LD-MSG]` (Liên đấu /
`LD_DiThanh`, dùng chung cho về thành TK + CTC).

### 3.2 Một lỗi thật tìm được khi đọc mã (bộ vá `ReverseTools\goi_va_tk_vethanh_0609.py`, commit `95317dc4`)

`TKP_END` ở map báo danh 324 nhờ Xa Phu *"Những thành thị đã đi qua"* đưa về thành đã chọn.
Danh sách đó do `station.lua::StationFun` dựng: `GetStation(i, CurStation)` →
`LuaGetPlayerStation` (`ScriptFuns.cpp:10262`) chỉ liệt kê trạm có `g_GetPriceToStation(CurStation, id) > 0`.
`tong_kim_tcap/xaphu.lua` đặt `CurStation = 1` (Phượng Tường) và `GetCurStation()` không đổi vì
map 324 không có trong `STATION_ARRAY`; `StationPrice.txt` hàng Phượng Tường → Phượng Tường = **-1**.

⇒ Chọn **Phượng Tường Phủ** — chính là **mục mặc định** của ô *Hết trận về* — thì Xa Phu 324 **không
bao giờ liệt kê**. Mã cũ chuyển bước 4: *"Trở lại chỗ lúc nãy"* → `ReturnFromPortal` về map luyện
công trước trận; map đó thường **không có Xa Phu**, không có phù về thành thì `LD_DiThanh` trả -1
ngay → *"Không đi tới được thành đã chọn - trả máy tại chỗ"* → người chơi đứng ở map cũ hoặc ở
thành mà Thổ Địa Phù đưa tới = **"về không đúng thành"**.

Sửa: bước 3 không thấy thành đã chọn → chọn **một thành khác có trong danh sách** (theo thứ tự
`g_LDVeMap`; 7 thành nối nhau qua Xa Phu, `StationPrice.txt` mọi cặp thành đều có giá), báo
*"Xa Phu điểm báo danh không liệt kê thành đã chọn - đi qua thành khác rồi nhờ Xa Phu ở đó về tiếp"*,
log `[TK-VE]`; `TKP_VETHANH` ở thành trung gian dùng `LD_DiThanh` (Thần Hành Phù nếu có, không thì
Xa Phu) đi nốt. Hết cách mới rơi về đường cũ.

Với cấu hình hiện tại của nhân vật test (Tương Dương, có trong danh sách, giá 30 lượng) đường Xa Phu
phải chạy thẳng; nếu lần tới vẫn sai thành, `jx_auto.log` sẽ có đủ `[TK-PHA]` / `[TK-MSG]` / `[LD-MSG]`
để chỉ đúng chỗ. **Chưa test thật** (trận 18:09 chạy trên DLL cũ, chưa swap).

### 3.3 Đường về thành đúng — tóm tắt để đối chiếu log

```
TKP_END (map 324) --Xa Phu 'Những thành thị đã đi qua' + tên thành--> thành đã chọn
   | không có tên thành (Phượng Tường LUÔN thế)
   v (mới 06/09)  --chọn thành khác trong danh sách--> thành trung gian
TKP_VETHANH: nMap != đích -> LD_DiThanh (Thần Hành Phù 6/1/1271: Thành thị > tên thành > cửa;
             không phù: Xa Phu 'Những thành thị đã đi qua'); quá 150 s -> trả máy
             nMap == đích -> bTKRuong ? TKP_RUONG : TKP_DONE
TKP_RUONG: đi tới rương cửa đã chọn (g_TKRuong) -> chạm (OpenBox + SetRevPos) -> cất trang bị
```

---

## 4. Cách kiểm sau swap

- WAuto: tick một ô bất kỳ → `dir APdata\<ID>.dat` đổi giờ ngay; trong game, ô mới có tác dụng ở
  nhịp kế (54 ms).
- Client: `findstr /C:"[TK-PHA]" /C:"[TK-MSG]" /C:"[TK-VE]" /C:"[LD-MSG]" bin\client\jx_auto.log`.
  Kỳ vọng sau hết trận: `[TK-PHA] … -> TKP_END` → `[TK-MSG] Xong Tống Kim - nhờ Xa Phu đưa về thành đã
  chọn` → `[TK-PHA] … -> TKP_VETHANH` → `[TK-MSG] Đã về tới thành đã chọn - đi tiếp tới rương` →
  `[TK-PHA] … -> TKP_RUONG` → `[TK-MSG] Đã tới rương - đặt lại điểm hồi sinh ở cửa này` → `… cất
  trang bị vào rương xong`. Log ghi TCVN3 (mở bằng bảng mã TCVN3 hoặc đọc qua `vn_edit.py --read`).

---

## 5. Bẫy

1. **Nhiều phiên đặt `.moi` chung một thư mục.** Lúc 18:13 phiên Vận tiêu đặt `CoreClient.dll.moi`
   `59161dae` + 18:21 `Game.exe.moi`. Tôi đã thay `CoreClient.dll.moi` bằng bản `main` HEAD (siêu tập:
   `main..chinam-vantieu-0609` = 0 commit, nguồn client sau 18:13 chỉ có `5a4d0d24` của chính họ +
   30 dòng của tôi), giữ bản cũ cạnh đó và **đã nhắn phiên đó**. Ai đặt lại `.moi` phải merge
   `origin/main` trước.
2. **Nhánh `mail-0309` còn 55 commit client chưa gộp `main`** (đấu giá, hộp thư, cửa sổ Thông tin trận
   TK…). Kết quả kiểm chuỗi nhị phân ghi ở mục 6. Nếu bản đang chạy có mã đó thì mọi build từ `main`
   (của tôi và của phiên Vận tiêu) đều thiếu — cần gộp `mail-0309` vào `main` trước khi build tiếp.
3. **`memset(&apdata, 0)` trong `LoadRoleData`** xoá mọi mặc định của constructor `autoData()`:
   thêm trường mới phải đặt mặc định ở **cả hai** nhánh của `LoadRoleData`, không chỉ trong `ipc_shared.h`.
4. **Đọc cấu hình thật của người chơi trước khi đoán**: `APdata\<ID>.dat` là struct thô — parse theo
   `ipc_shared.h` (pack 1) ra được đúng từng ô; đã làm ở mục 2.1 và nó chỉ thẳng lỗi.
5. Tên cấu hình build client ở `D:\GAMEDEVNEW` là **`Client Release|Win32`** (không có ` - US`);
   `build.py --config "Client Release" --platform Win32 --project Core --solution-dir D:/GAMEDEVNEW`.
   WAuto: `MSBuild WAuto.vcxproj -p:Configuration=Release -p:Platform=Win32` trong
   `E:\Src_Auto_Ngoai\WAuto\WAuto` → `Release\WAuto.exe`.

## 6. Kết quả kiểm bổ sung

- **Nhánh `mail-0309` không gây rủi ro cho build từ `main`**: `git diff main..mail-0309 -- Sources` = 27 tệp,
  **+26 / −2.169 dòng**; 26 dòng "thêm" chỉ là mã cũ (bot nội/ngoại `PB_BAT_DUONG_NOI`, `lua4compat`,
  `KJx2SharedStore`, `[PORT5] RemoteExecute`) mà `main` đã thay bằng bản mới hơn — không có tính năng client
  nào chỉ nằm trong `mail-0309` (55 commit của nó đã vào `main` bằng nội dung, chỉ khác mã băm).
- **Trận 18:09 trên DLL cũ `b166ec89`** (cấu hình `bTKRuong=0`, đích Tương Dương): tới 18:42 vẫn đang đánh
  (`[HD-GATE] nTK` 2↔1 = chết/hồi sinh). Một bộ canh `jx_auto.log` chạy nền thêm 10 phút; nếu trận kết thúc
  trong khoảng đó, diễn biến sau `TKP_END` (map đến) sẽ được ghi thêm ở đây. DLL cũ không có `[TK-PHA]`
  nên chỉ suy được từ `[S6-ME] loadmap` + `[DT-STATE] map=` / `[MOVE-NOMODE] curmap=`.

### 6.1 Trận 18:09 đã kết thúc — đo được trên DLL cũ (`jx_auto.log`, giờ máy 18:43)

| t (ms) | Sự kiện |
|---|---|
| 1026204188 | rời 379 → về map báo danh 324 (`SYNCNPC-SETPOS npc=92533`), TK giữ máy (`nTK=1`) ≈ 30 s: Xa Phu 324 |
| 1026208415 | **`[DT-STATE] map=78` = đã về tới Tương Dương Phủ, đúng thành đã chọn** (`nTKVeThanh=4`). Cùng nhịp, TK trả máy (`bTKRuong=0`) và máy **Hậu cần** bắt đầu: `[HC-STATE] buoc=0 … bat: ban=1 sua=1 rut=1 cat=1 giutien=1 xafu=1/0 bando=1/10` |
| 1026211423 | `[HC-STATE] buoc=3` (bán rác xong: ô trống 1 → 22) |
| 1026214654 | `[HC-STATE] buoc=9 sub=2` — bước 9 = ra Xa Phu lên map luyện công (`bGoMap=1`, `nSelMap=10`), mục tiêu = NPC Xa Phu (54146,103434) |
| 1026217555 | `SYNCNPC-SETPOS` + `[MOVE-NOMODE] curmap=93` — **về lại map luyện công 93**, 9 giây sau khi tới Tương Dương |

⇒ **"Về không đúng thành" thật ra là: về ĐÚNG thành rồi bị máy Hậu cần (tab Hậu cần: *Tự quay lại* + *Đi map
luyện công*) đưa ngay về map luyện công trong ~9 giây**, nên người nhìn chỉ thấy nhân vật đứng ở map 93.
Đường chọn thành / Xa Phu 324 chạy đúng với Tương Dương. Vá Phượng Tường ở mục 3.2 vẫn đúng (lỗi tiềm ẩn
cho mục mặc định), không phải nguyên nhân của lần này.

Sau khi swap WAuto mới và tick lại ba ô rương, thứ tự sẽ là: về thành → **TK giữ máy đi tới rương, đặt điểm
hồi sinh, cất trang bị** → trả máy → Hậu cần bán/sửa/rút → Xa Phu về map luyện công (đúng cấu hình Hậu cần
của chủ). **Nếu chủ muốn nhân vật ĐỨNG LẠI trong thành sau Tống Kim** thì hoặc bỏ tick *Đi map luyện công*
ở tab Hậu cần, hoặc yêu cầu thêm tuỳ chọn "sau rương thì đứng lại" — chưa làm vì đây là quyết định luật chơi.

### 6.2 Phối hợp với phiên Vận tiêu (18:45)

Phiên Vận tiêu đã kiểm `CoreClient.dll.moi` của tôi (thấy đủ dấu hiệu mã v2 của họ + `[TK-MSG]`) và **đồng ý
không đặt lại**; từ `5a4d0d24` họ không sửa `Sources\Core` nữa nên bản này đầy đủ. Họ **đã thay `Game.exe.moi`**
bằng bản 18:43 (`3da2169e`, chỉ mã S3Client, không liên quan WAuto). Máy chủ: `bin\server\CoreServer.dll.moi`
`6439ba82` (sha256 `61bc3e0f`) là của họ, giữ nguyên. Lưu ý sổ hash: tôi ghi **md5**, họ ghi **sha256[:8]**
— cùng tệp khác hàm băm, đừng tưởng lệch bản.

---

## 7. (19:05) Tính năng mới tab PK — BỎ QUA MỤC TIÊU ĐANG CÓ KHIÊN BẢO VỆ

> Chủ giao: *"thêm tính năng ở tab pk bỏ qua đối tượng đang có khiên bảo vệ AddSkillState(963, 1, 0, 18*3)"*.

### 7.1 Khiên là gì, client biết bằng cách nào

- Mọi script dịch chuyển cấp **vòng tròn bất tử 3 giây**: `SetProtectTime(18*3)` + `AddSkillState(963, 1, 0, 18*3)`
  — `station.lua` (Xa Phu), `shenxingfu.lua` (Thần Hành Phù), trap cổng thành, **trap ra trại Tống Kim**
  `maps/tongkim/trap/kimratrai.lua` / `tongratrai.lua`, `trinhsat.lua`, `playerlogin.lua`; `dichuyenmap.lua` 18*6;
  pubg 18*180. Skill 963 = *Thánh Quang Bình Thuẫn (chỉ hiệu ứng)*, `StateSpecialId = 159`. Đánh vào không trúng.
- Client nhận khiên của người khác qua gói **NPC_NORMAL_SYNC**: `m_nProtectedTime` (`KProtocolProcess.cpp:2453`) và
  `StateInfo` → `m_btStateInfo` (`:2465`, có 159). **Bẫy:** client KHÔNG tự đếm lùi `m_nProtectedTime` (`KNpc.cpp:1626`
  nằm trong `#ifdef _SERVER`) nên giá trị có thể "đứng" tới lần đồng bộ sau → `PK_CoKhien` tự ước **hạn hết khiên** =
  lúc thấy + số khung còn lại / 18, chỉ tính lại khi giá trị đồng bộ **đổi**; hết hạn thì đánh bình thường dù cờ còn
  đứng — không bao giờ bỏ qua ai mãi mãi.

### 7.2 Đã làm (commit `aed31625`, bộ vá `ReverseTools\goi_va_pk_khien_0609.py`)

| Chỗ | Nội dung |
|---|---|
| `ipc_shared.h` (Core + WAuto) | `int bPKBoQuaKhien` ở **cuối** `autoData`, mặc định 1 → struct **7.640 → 7.644 byte**: `WAuto.exe` và `CoreClient.dll` phải đổi **cùng lúc** |
| `CoreShell.cpp` | `PK_CoKhien(nIdx)`; máy PK `ATYPE_PKFIGHT`: mục tiêu đang giữ có khiên → bỏ; mục tiêu mới có khiên → đưa vào `m_mAutoExcludeNpcID` 3,5 s (FindTargetNpc tự bỏ qua) + log `[PK-KHIEN]`; tầng săn TK `TK_SanNguoi`, `TK_ChonDich`, `LD_ChonDich` bỏ qua người có khiên |
| WAuto tab PK | ô tick **Bỏ qua mục tiêu đang có khiên bảo vệ** (ID 479, hàng y=351 vì hàng 336 bị `ShowTab` dời cụm *Đổi với tay trái* tới — bản đầu chồng lên combo, chủ báo 19:10; `s_aTabDay[7]` 350 → 365; `do_wauto_bo_cuc.py` 0 nhãn cắt), lưu ngay (khối lưu chung), mặc định **BẬT** cho tệp mới và tệp cũ, tooltip + note tab PK |

Mặc định BẬT vì đánh người có khiên chỉ tốn lượt; chủ không muốn thì bỏ tick.

### 7.3 Trạng thái swap

Lúc 18:50 và 19:01 chủ đã chạy `ChoiGame.bat`: bản đang chạy = `CoreClient.dll` `9cb2f51f` + `Game.exe` `3da2169e`
(Vận tiêu) + `WAuto.exe` `807e4007` — tức **đợt 1 (lưu ngay + về thành) đã lên máy**. `.moi` mới chờ đợt 2:

| Tệp | md5 | sha256[:8] | cỡ |
|---|---|---|---|
| `WAuto.exe.moi` | `c692b1de` | `9cf5e921` | 466.944 |
| `CoreClient.dll.moi` | `e233e443` | `ba2f2f11` | 2.586.112 |

Cách đổi: thoát Game.exe **và** WAuto.exe → `ChoiGame.bat` (đổi cả hai). **PHẢI đổi cả hai cùng lúc** vì gói IPC
`IPCGameLoop` dài thêm 4 byte. Nếu lỡ chỉ đổi một: `S3Client.cpp:1232` (PRT_GAMELOOP) đã có lưới — gói NGẮN hơn
(WAuto cũ → CoreClient mới) được chép vào bản sao xoá trắng nên ô mới đọc ra 0 = tính năng tắt, gói DÀI hơn (WAuto mới
→ CoreClient cũ) bị bỏ 4 byte cuối; không sập, chỉ là tính năng khiên im lặng không chạy cho tới khi đổi đủ cả hai.
Tệp `.dat` 7.640 byte cũ nạp được, ô mới mặc định bật. Chưa test thật; kiểm bằng `findstr /C:"[PK-KHIEN]" jx_auto.log` khi PK gần người vừa dịch chuyển.
`origin/main` sau `aed31625` có thêm bot nội đợt 3 (server) — không ảnh hưởng client, chưa build lại.

---

## 8. (19:35) Chủ báo sau swap đợt 2: "về thành là đứng yên" + "tới NPC Dã Tẩu xong đứng yên"

Đo `jx_auto.log` pid 47140 (game mở 19:14, map 78 Tương Dương), 19:22-19:25:

| Máy | Thấy gì | Gốc | Xử lý |
|---|---|---|---|
| **Hậu cần** | `[HC-STATE] buoc=1` đứng yên hàng phút ở cùng toạ độ, túi còn 28-29 ô trống | Bước 1 (bán rác) quét túi, gặp một món trang bị ≤ xanh không khoá → `SendClientCmdSell` rồi `return 1`; 300 ms sau quét lại, món **vẫn còn** (máy chủ từ chối im lặng: `KBuySell::Sell` trả FALSE khi khoá/-2/FightMode, hoặc lệnh không tới) → gửi lại **mãi mãi**, không sang bước 2. Lúc 18:43 bước 1 qua trong 3 s vì túi không có món đó. | **Đã vá** (commit `42262294`, `goi_va_hc_ban_0609.py`): đếm số lần gửi theo dwID, tới 6 lần mà món vẫn còn thì bỏ qua món đó trong lượt này; bước 0 xoá bộ đếm. Log `[HC-BAN] gui ban id=… 'tên' genre/detail/mau/khoa lan k/6` — lần sau đọc là biết món nào và vì sao. |
| **Dã Tẩu** | `pha=1` tới NPC 108, thoại, rồi `pha=14` treo 3.598 s, `du40=260906/260906` | NPC nói đúng câu `seasonnpc.lua:62` *"Mỗi ngày làm 40 lần là đủ rồi! Ngày mai trở lại nhé!"* (mốc `DTM_MSG_LIMIT` = "Mỗi ngày làm 40 lần"). Nhân vật này **đã làm đủ 40 nhiệm vụ hôm nay** (từ 18:04 log đã treo vì đủ 40). Auto báo trong chat *"Tuyệt! Đã đủ 40 nhiệm vụ Dã Tẩu hôm nay - nghỉ, qua ngày auto tự chạy lại."* | Đúng thiết kế, **không sửa**. Qua ngày mới (`DT_Today()` đổi) tự chạy lại. |
| **Cả hai bị "khởi động lại"** mỗi 57-100 s | `[DT-STATE] pha=0 … du40=0 dlg=17/0` = `ExtAuto` bị `memset` (`ATYPE_CLEAR`) | `ATYPE_CLEAR` chỉ phát khi ô tick auto trong danh sách WAuto đổi trạng thái: bấm ô tick, hoặc **Ctrl+A trong game** (`UiGame.cpp:91` → `PRG_AUTOONOFF` → WAuto đổi tick → `PRT_TICKSTART` → xoá trạng thái). Mỗi lần như vậy Dã Tẩu quên "đủ 40" → đi lại tới NPC → hỏi → treo lại; Hậu cần về bước 0. | Không phải lỗi mã; là hệ quả của việc bật/tắt auto liên tục khi thấy nhân vật đứng. Ghi nhận để chủ biết. |

**Đợt 3 ĐÃ LÊN MÁY 19:31-19:34** (phiên Vận tiêu đo, tôi kiểm lại 19:50): `CoreClient.dll` **`b425fb53`** (sha256 `076e8550`,
2.593.280 B) + `WAuto.exe` `9ff7c4e6` + `Game.exe` `3da2169e`; game mở lại 19:34:37 (pid 29772) nhưng tới 19:50 **chưa vào
nhân vật** (chưa có dòng log nào của pid mới, bộ nhớ 185 MB = màn hình đăng nhập) nên chưa đo được vá Hậu cần. Khi vào lại,
kiểm: `findstr /C:"[HC-BAN]" jx_auto.log` — dòng đầu ghi tên món máy chủ từ chối bán, dòng `lan 6/6 - VAN CON TRONG TUI, bo qua`
là máy đã bỏ qua và đi tiếp; `[HC-STATE] buoc` phải chạy 1 → 2 → … → 9 thay vì kẹt 1. Khe `.moi` của tôi hiện TRỐNG; `Game.exe.moi`
(6210e1f5) và `bin\server\CoreServer.dll.moi` (d0dab0c9, đã gộp `42262294`) là của phiên Vận tiêu / bot nội, tôi không đụng.
