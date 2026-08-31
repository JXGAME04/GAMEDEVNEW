# BÀN GIAO 30/08 (đợt 2) — 5 VIỆC CHỦ GAME GIAO

Tiếp theo [BANGIAO_GO_HECU_3008.md](BANGIAO_GO_HECU_3008.md). Vẫn theo nguyên tắc
cũ: **không xoá gì** — mọi thứ dời sang `bin\server\_dara\`, nhật ký
`_dara\NHATKY_DOI.txt`; sửa tại chỗ thì để bản gốc cạnh (`.truoc_boldhc`,
`.truoc_dondatau`, `.truoc_portlxw`, `.truoc_cuu210`); mỗi tệp đều đọc lại kiểm
byte, so cân bằng từ khoá Lua, và chạy qua trình phân tích cú pháp Lua 4.0.1
thật (`syncheck.exe` dựng từ chính `Sources\Library\LuaLib`).

---

## 1. ✅ Lôi Đài Hỗn Chiến — BỎ HẲN

Gỡ đủ 5 điểm, **216 dòng**:

| Nơi | Việc |
|---|---|
| `timerserver.lua` | xoá hàm `LoiDaiHonChien` (201 dòng), nhánh gọi `BAT_LOIDAI_HONCHIEN` (9), hàm phụ `LDHC_CFG` (6) |
| `cauhinh\ch_lich.lua` | bỏ `BAT_LOIDAI_HONCHIEN`, `LDHC_PHI_BAODANH` |
| `cauhinh\ch_thuong.lua` | bỏ 4 khoá `LDHC_*` |
| `startgame\thon\balanghuyen.lua:84` | comment `AddNpcNew` NPC báo danh |
| `tinhnang\loidaihonchien\` | dời 3 tệp sang `_dara` |

**Không đụng** (đã kiểm, đều dùng chung): `TASK_DSK` (điểm sự kiện — 6 tệp khác
dùng), vật phẩm 4844 Hộ Mạch Đơn, mảnh hoàng kim 753-770/903-942, và
`settings\MapList.ini`.

**Đính chính một điều tôi nói sai lúc đầu:** tôi đã nói bản đồ 210 chỉ Lôi Đài
Hỗn Chiến dùng. Sai — `missions\bw\bwhead.lua:9` khai `BW_COMPETEMAP = {209,
210, 211}`. Kiểm kỹ thì hệ Tỷ Võ **chỉ thực sự dùng `BW_COMPETEMAP[1]` = 209**
(9/9 chỗ tham chiếu), nên gỡ không ảnh hưởng — và may là tôi không hề sửa
`MapList.ini`.

### 🔴 Kèm một lối thoát cho người có thể đang kẹt

NPC báo danh cũ (`mainloidai.lua:42`) **chỉ kiểm giờ, không kiểm công tắc** —
nên suốt thời gian `BAT_LOIDAI_HONCHIEN = 0` vẫn có thể có người vào được bản đồ
210 (bị đặt `SetMask(2019)` / `SetPunish(1)` / `SetPKMode(2,1)`). Đoạn đưa họ ra
nằm trong đúng cái hàm tôi vừa gỡ.

Đã thêm `CuuNguoiKetLDHC()` vào `player\playerlogin.lua`: ai đăng nhập mà đang ở
bản đồ 210 thì được đưa về Ba Lăng Huyện và dọn sạch trạng thái — chuỗi lệnh
**chép nguyên** từ nhánh kết thúc trận của bản cũ. Khi chắc không còn ai kẹt thì
gỡ hàm này đi được.

*(`m_dwDeathScriptId` chỉ nằm trong bộ nhớ — `KPlayer.cpp:296` đặt 0 lúc khởi
tạo, không có mã lưu/nạp — nên thoát game là script tử vong tự sạch.)*

---

## 2. ✅ Hai rương hậu phương (bản đồ 222/223) — ĐỂ NGUYÊN

Theo lệnh anh. Không đụng `lib_ctc.lua:229-230`.

---

## 3. ✅ Long Huyết Hoàn (mã vật phẩm **2126**, dòng 2128 trong bảng) — ĐÃ LÀM CHO DÙNG ĐƯỢC

**Nó phục vụ hoạt động nào:** mua thêm 1 lượt tham gia **Thách Đấu Thời Gian
(Vượt Ải)**.

**Vì sao hỏng:** script cũ (Fong Kiều 2021) chạy trên bộ đếm của hệ Vượt Ải
**cũ** — `T_NVVATRONGNGAY` (43) và `TSK_LONGHUYETHOAN` (65). Hệ Vượt Ải đang chạy
là bản Linux `missions\challengeoftime`, đếm bằng bộ task **hoàn toàn khác**.
Sau khi hệ cũ bị gỡ thì không ai tăng task 43 nữa → câu chặn ở dòng 23 luôn đúng
→ **bấm vào chỉ ra thông báo, vật phẩm vô dụng**.

Vật phẩm **vẫn lấy được**: bán ở Kỳ Trân Các 10 xu (`goods.txt:632`), cộng 2
bảng quay sự kiện (20/10, Noel).

**Điều bất ngờ tốt:** bản Linux **đã có sẵn** cơ chế Long Huyết Hoàn, và mã task
**trùng y hệt** cây ta:

| Task | Số | Nơi khai | Ai đọc |
|---|---|---|---|
| `TSK_REMAIN_COUNT` | 1550 | `challengeoftime\include.lua:21` | `dragonboat_main.lua:151` |
| `TSK_JOIN_DATE` | 1551 | `:20` | `:147` |
| `TSK_Longxuewan_Date` | 2641 | `:126` | `dragonboat_main.lua:154` |
| `TSK_Longxuewan_Use` | 2642 | `:127` | — |
| `TSK_Longxuewan_avail` | 4018 | `:128` | `dragonboat_main.lua:155-160`, `translife_5.lua:109` |

Tức **bên nhận đã sẵn sàng từ trước, chỉ thiếu bên phát**. Đã port
`script\item\longxuewan.lua` từ `D:\ServerLinux\server1\script\item\longxuewan.lua`.

**Ba chỗ phải sửa khi port** (không chép nguyên xi được):
1. Vỏ hàm: Linux `main()` không tham số, engine JX2 tự trừ vật phẩm; JX1 dùng
   `main(nItemIndex)` và **phải tự gọi `RemoveItemByIndex`**. Lấy khuôn từ một
   tệp đã port sẵn trong cây: `item\ruong_datau_tasklink.lua`.
2. Linux ghi cứng `SetTask(1550, 1)`; cây ta cho chỉnh bằng cấu hình
   (`HD3_VA_LUOT_NGAY`) nên tôi dùng `HD_CFG`.
3. **Cố ý khác bản Linux một chỗ:** Linux còn chặn `GetExtPoint(0) == 0`.
   `GetExtPoint(0)` là **số Xu** của người chơi (`ScriptFuns.cpp:218-231`; đối
   chiếu `quanly.lua:182 local TienXu = GetExtPoint()`) — tức bản Linux chặn
   người chơi không có Xu. Đó là quyết định **kinh tế**, không phải lỗi kỹ thuật,
   và bản JX1 xưa nay không chặn. Tôi để sẵn dòng `LXW_CAN_XU` đã comment —
   anh bỏ dấu `--` là bật.

Giới hạn giữ theo bản Linux: **2 viên/ngày**.

---

## 4. ✅ Rương Dã Tẩu (1341) — KHÔNG THUỘC HOẠT ĐỘNG NÀO

Truy tới cùng: đây **không phải** vật phẩm của hệ Dã Tẩu cũ lẫn mới.

- `settings\item\magicscript.txt:1343` khai đủ, trỏ `\script\item\ruongdatau.lua`
- Tệp đó **không tồn tại trên bất kỳ cây nào**: máy chủ đang chạy, `_dara`,
  `D:\GAMEDEVNEW`, `D:\ServerLinux` — kể cả lịch sử git đều 0 kết quả.
- **Không ai phát nó**: grep mã 1341 trên toàn `script\`, `settings\droprate\`,
  `settings\task\`, `goods.txt`, `buysell.txt`, `lottery.txt` = 0. Ngay cả hệ Dã
  Tẩu **cũ** (các tệp tôi vừa dời ra) cũng không phát.
- Bấm vào không xảy ra gì: `g_GetScript` trả NULL, hàm thoát im lặng, không trừ
  vật phẩm, không ghi log.

Rương Dã Tẩu **đang dùng thật** là vật phẩm **2383** "Bảo rương thần bí của Dã
Tẩu" — hệ tasklink phát ở `global\seasonnpc.lua:122` (mốc 40 nhiệm vụ, 5 cái),
mở bằng `item\ruong_datau_tasklink.lua`.

**Xử lý: để nguyên dòng 1343.** Bảng `magicscript.txt` tra theo **chỉ số dòng**
(`KItemGenerator.CPP:1660` → `KBasPropTbl.cpp:1058`), xoá một dòng là **xô lệch
toàn bộ 3.591 dòng phía sau** — đúng cái bẫy đã từng làm hệ Phi Phong phát nhầm.
Cái giá của việc để nguyên là bằng không.

> Nhân tiện đo lại cả bảng: **4933/4933 dòng khớp `PT = số dòng − 2`, hết lệch**
> (ghi chú cũ "lệch 35 dòng từ 4881" đã lỗi thời, tôi đã sửa lại ghi nhớ).

---

## 5. ✅ Dọn triệt để Dã Tẩu cũ — LÀM XONG CHẶNG A, CHẶNG B CHỜ SWAP

### Vướng mắc

`Sources\Core\Src\KNpc.cpp:1698` gọi **cứng** `tinhnang\datau\danhquai.lua` **mỗi
lần người chơi giết quái** — và tệp đó nay còn mang **móc đếm giết quái của Bạn
Đồng Hành** (`danhquai.lua:5` Include `bdh_killhook.lua`, `:20` gọi
`BDH_OnKillNpc`). Đây là đường mã nóng nhất máy chủ.

*(Hai chỗ khác — `KPlayer.cpp:4956` và `:4966` — nằm trọn trong khối `/* */` mở
ở 4941, đóng ở 4974 → đã chết.)*

### Chặng A — xong, không cần dừng máy chủ

1. Tạo `script\global\onkillnpc.lua` — nhà mới cho móc "người chơi giết NPC",
   chỉ còn phần Bạn Đồng Hành, không dính gì Dã Tẩu.
2. Rút `danhquai.lua` 63 dòng → **bộ chuyển tiếp 11 dòng**. Nhờ vậy DLL đang
   chạy (còn trỏ đường dẫn cũ) vẫn hoạt động bình thường.
3. Gỡ nhánh dự phòng hệ cũ trong `global\station.lua::godatau()` (dòng 359-390)
   + dòng `Include lib_datau.lua`.
   **An toàn vì** `lib_ham.lua:261-267` reset **mỗi ngày** các task hệ cũ
   (`T_TIMDOCHI` 12, `T_TIMMATCHI` 13, `T_TIENDONV` 14, `T_DanhQuai` 89…) → không
   nhân vật nào kẹt quá 24 giờ.
4. Dời **12 tệp** còn lại của `tinhnang\datau` sang `_dara`.

### Chặng B — đã vá + dựng, **chờ anh swap**

`KNpc.cpp:1698` nay trỏ thẳng `\script\global\onkillnpc.lua` / `OnPlayerKillNpc`.
Dựng sạch **cả hai cấu hình** (`KNpc.cpp` là tệp dùng chung client+server):

- `Server Release|x64` → `CoreServer.dll` (md5 `5f879a7e3dfe31b7def75f9605196beb`)
- `Client Release|Win32` → `Core.dll` (chỉ để chứng minh tệp dùng chung vẫn biên
  dịch được; **không cần thay** vì thay đổi nằm trong `#ifdef _SERVER`)

#### ⚠️ Bản kê ĐẦY ĐỦ những gì có trong DLL này

Bộ phản biện chỉ ra một điểm quy trình mà tôi đã bỏ qua: **bản dựng cuốn theo mọi
thứ đang có trong cây nguồn, không chỉ hai bản vá của tôi.** Đối chiếu thời điểm
sửa với bản DLL đang chạy (30/08 10:11), có **4 tệp** mới hơn:

| Tệp nguồn | Sửa lúc | Thuộc về |
|---|---|---|
| `KNpc.cpp` | 11:22 | ✅ bản vá của tôi (móc `onkillnpc`) |
| `KPlayer.cpp` | 11:33 | ✅ bản vá của tôi (`bGlobal = false`) |
| `KItem.h` | 10:37 | ⚠️ **không thuộc đợt này** |
| `KItem.cpp` | 10:40 | ⚠️ **không thuộc đợt này** |

Hai tệp `KItem.*` là phần **mô tả Phi Phong** — thêm `PF_StarPrefix()` (in "N sao"
trước tên món) và khối mô tả ô khảm/điểm chúc phúc trong bảng thông tin vật phẩm.
Nó khớp với đợt port Phi Phong 29/08 vẫn đang chờ swap.

**Không phải lỗi, nhưng anh cần biết mình đang thay cái gì.** Nếu anh muốn một
bản DLL *chỉ* chứa hai bản vá của tôi thì nói một tiếng — tôi tạm hoàn `KItem.*`
về bản trước rồi dựng lại.

### 🔴 Kèm trong cùng bản DLL: vá một LỖI ĐANG XẢY RA hằng ngày

Bộ phản biện tìm ra, tôi đã tự kiểm lại từng dòng và có bằng chứng trong log.

`KPlayer.cpp:4993` và `:4995` gọi
`ExecuteScript("...tasklink_goods.lua", "PickUp", nObjIndex)` **không truyền tham
số cuối**. Chữ ký mặc định `bGlobal = true` (`KPlayer.h:857`), và nhánh đó ghi
`Npc[m_nIndex].m_ActionScriptID = dwScriptId` (`KPlayer.cpp:7129`) — đúng cái
biến mà **menu trả lời của người chơi điều phối qua** (`:7621`/`:7626`).

Nghĩa là: đang mở menu bất kỳ mà **đi ngang một cuộn Dã Tẩu** (vật phẩm 6/1/205
hoặc 6/1/212) là ngữ cảnh hội thoại bị cướp — cú bấm nút tiếp theo gọi vào
`tasklink_goods.lua` tìm một hàm không có ở đó, **nút im lặng không ăn**.

Bằng chứng trong `ScriptError.log` của máy chủ đang chạy:

| Thời điểm | Script bị gọi nhầm | Hàm người chơi thực sự bấm |
|---|---|---|
| 30/08 11:10:00 | `tasklink_goods.lua` | `StationFun` (Xa Phu) |
| 30/08 10:31:25 | `tasklink_goods.lua` | `g_DailogBack` (nút Quay lại) |
| 29/08 10:11:02 | `tasklink_goods.lua` | `BDH_Root` (menu Bạn Đồng Hành) |
| 28/08 18:49:24 | `tasklink_goods.lua` | `ruong` |
| 27/08 23:40:56 | `tasklink_goods.lua` | `psthanhthi` (dịch chuyển) |

Vá: truyền `bGlobal = false` cho hai lời gọi đó. Nhặt đồ là việc "bắn ra rồi
thôi", không được phép sở hữu ngữ cảnh hội thoại của người chơi.

##### Hai điều phản biện nêu mà tôi CỐ Ý KHÔNG sửa

**(a) "Bản vá chỉ bịt một trong hai cửa."** Đúng — cùng hai tệp đó còn hàm
`main(nItemIndex)` chạy khi người chơi **bấm chuột phải** vào cuộn trong túi
(`magicscript.txt:207/214`), và đường đó vẫn để `bGlobal` mặc định.

Nhưng hai đường này **khác bản chất**:

| | Nhặt tự động | Bấm chuột phải |
|---|---|---|
| Ai khởi động | **engine**, người chơi không làm gì | chính người chơi |
| Đang mở menu thì | bị cướp **oan** | người chơi tự bỏ menu để dùng đồ |

Đặt `m_ActionScriptID` khi dùng vật phẩm là **hành vi đúng của engine** — rất
nhiều script vật phẩm mở menu và cần nút bấm quay về đúng script. Sửa đường đó
là đổi hành vi của **mọi** vật phẩm trong game. Nên tôi chỉ bịt đường nhặt.

*(Thêm nữa: từ 16/08 cuộn không vào túi nữa — `KPlayer.cpp:5021` `return TRUE`
trước `AddKIL` — nên cửa chuột phải chỉ còn với cuộn tồn từ trước.)*

**(b) `item\lenhbaiadmin.lua` có 29 lỗi cùng dạng** trong `ScriptError.log`
(`TTHD_MM_Vao853` ×10, `LR_Root` ×6, `BDH_P_CapRuong`…, lần cuối 30/08 10:07).
Đây là **một script khác** với cùng triệu chứng, nằm ngoài phạm vi 5 việc anh
giao. Ghi lại để anh biết còn một chỗ nữa đáng truy.

Đã kiểm trong nhị phân: có `\script\global\onkillnpc.lua` + `OnPlayerKillNpc`,
**không còn** `\script\tinhnang\datau\danhquai.lua` và `OnDeathMonsterDaTau`.

**Thứ tự bắt buộc:** bộ chuyển tiếp `danhquai.lua` phải **ở nguyên** cho tới khi
swap xong. Swap rồi thì nó hết việc, dời nốt là thư mục `tinhnang\datau` biến mất
hẳn.

### An toàn dữ liệu

Task hệ Dã Tẩu cũ (12, 13, 14, 75, 87-99, 317) và hệ tasklink mới (1020-1046,
1825, 2419, 2420, 2574, 2690, 2797) **không trùng nhau số nào**; chỗ chung duy
nhất là task 38 và cả hai hệ chỉ **đọc**.

---

## 6. Phát hiện thêm — BÁO ĐỂ ANH BIẾT, CHƯA ĐỘNG VÀO

`lib_ham.lua:296-305` có bảng `TB_BU_HD` — hệ **"bù hoạt động lỡ"**, tích lượt
thiếu tới 7 ngày. Đo cả 6 mục (quét theo **cả tên lẫn số hiệu**, mọi dạng ghi):

| # | Hoạt động | Bộ đếm | Ai ghi |
|---|---|---|---|
| 1 | Phong Lăng Độ | `TSK_NUMPLDNGAY` (315) | **không ai** |
| 2 | Vượt Ải | `T_NVVATRONGNGAY` (43) | **không ai** |
| 3 | Vận Tiêu | `T_NVVTTRONGNGAY` (49) | ✅ `event_vantieu\tieudau.lua` |
| 4 | Hằng Ngày | `TASK_TANTHU3` (345) | **không ai** (`lenhbaitanthu.lua:110` đã comment) |
| 5 | Vip | `TASK_NEWTHOREN6` (377) | **không ai** (`:111` đã comment) |
| 6 | Dã Tẩu | `T_SoNVTrongNgay` (87) | chỉ hệ **cũ** (`datau.lua:110`) — nay đã gỡ |

**Nhưng không thất thoát gì**: quét toàn cây, **không nơi nào đọc `TASK_HD_MISS_*`
để phát thưởng** (chỗ duy nhất nhắc tới là `lenhbaitanthu.lua:107`, đã comment).
Tức hệ này chỉ đếm rỗng — nửa xây dựng dở.

Muốn nó chạy thật thì phải cho các hệ mới ghi bộ đếm — **đó là mở một dòng phần
thưởng mới**, nên tôi để anh quyết.

---

## 6b. 🔴 MỘT QUẢ MÌN CÓ SẴN — không phải do đợt này, nhưng anh nên biết

`script\event\storm\function.lua:392-400`:

```lua
function storm_clear(gameid)
	if (gameid == nil) then
		Msg2Player("Xóa hết các tin tức tích luỹ của nhân vật này!")
		SetTask(1550,0);
		SetTask(1036,0);
		for i = 1661, 16710 do
			SetTask(i,0);
		end
```

Gọi `storm_clear()` **không tham số** là **xoá trắng mọi task từ 1661 tới trần
(5199)** của nhân vật đó — nuốt luôn 2641/2642/4018 (Long Huyết Hoàn vừa nối),
1825/2419/2420/2690 (tasklink Dã Tẩu), và cả dải chuyển sinh.

Hiện trạng: **không nơi nào gọi nó** (grep toàn cây chỉ ra đúng dòng định nghĩa),
nên chưa nổ. Nhưng nó **được nạp sẵn** vào 4 trạng thái Lua đang chạy —
`battles\battlehead.lua:4`, `global\seasonnpc.lua:9`,
`challengeoftime\mission_match.lua:6`, `challengeoftime\npc_death.lua:10` — tức
bất kỳ ai gọi được Lua trên các trạng thái đó đều chạm tới được.

Tôi **không tự sửa** vì nó nằm ngoài 5 việc anh giao và là công cụ GM. Muốn an
toàn thì chỉ cần bắt nó đòi một tham số xác nhận — nói một tiếng là tôi vá.

---

## 7. Checklist thay DLL

1. Báo trước cho người chơi, **tắt cụm máy chủ** theo quy trình của anh.
2. Vào `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\`, xác nhận có
   `CoreServer.dll.moi` (18.205.184 byte, md5 `5f879a7e3dfe31b7def75f9605196beb`).
3. Chạy `ChayGameServer.bat` — nó tự đổi tên `.moi` thành `CoreServer.dll`.
4. Sau khi máy chủ lên, **thoát hẳn client rồi vào lại** (không dùng nút đổi
   nhân vật).
5. Kiểm nhanh:
   - Giết vài con quái → nhiệm vụ **cuộn tu luyện Bạn Đồng Hành** vẫn đếm.
   - Mở Kỳ Trân Các mua **Long Huyết Hoàn** (10 xu), bấm dùng → phải báo
     *"Thu được thêm cơ hội 1 lần tham gia hoạt động thách đấu thời gian!"*,
     rồi gặp Nhiếp Thị Trấn thấy có thêm lượt.
   - Xa Phu → **Dã Tẩu**: nhân vật đang làm nhiệm vụ tasklink vẫn đi được.
   - NPC **Lôi Đài Hỗn Chiến** ở Ba Lăng Huyện đã biến mất.
6. Nếu có ai kẹt ở bản đồ 210: bảo họ **thoát game vào lại** — sẽ tự được đưa về
   Ba Lăng Huyện.
7. Xong xuôi, báo tôi để dời nốt `tinhnang\datau\danhquai.lua` (lúc đó mới hết
   việc).

**Hoàn tác:** `doi_tep.hoan_tac("<đường dẫn>", ghi=True)`; tệp sửa tại chỗ thì
chép đè bản `.truoc_*` nằm cạnh; DLL thì đổi tên `CoreServer.dll.truoc` về.
