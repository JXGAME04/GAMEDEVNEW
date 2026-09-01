# BÀN GIAO — HỆ PHI PHONG: KHOẢNG CÁCH CÒN LẠI SO VỚI BẢN LINUX

**Chốt 31/08.** Mục tiêu chủ game đặt ra: **làm giống 100% bản Linux**. Phiên này **chỉ ghi việc**, không thi công tiếp.

Tệp này **nối tiếp** `BANGIAO_PHIPHONG_TIEPTUC_3008.md` — bẫy đã trả giá, cấu trúc bảng khảm, nguồn chuỗi gốc nằm ở đó, **không chép lại**. Ở đây chỉ có: hôm nay làm gì, cái gì đã đo chắc, cái gì đã bị bác bỏ, và việc còn lại.

📗 **Danh sách việc ĐẦY ĐỦ, có thứ tự, đã thẩm định: `BANGIAO_PHIPHONG_DANHSACHVIEC_3108.md`** (77 KB, 8 đợt thi công, mỗi việc có neo tệp:dòng + cách nghiệm thu). Tệp anh đang đọc là **bản tóm tắt** — chỉ giữ những gì tôi **tự kiểm chứng bằng tay**.

---

## 🔴 CẢNH BÁO TRƯỚC MỌI VIỆC KHÁC — LỖ HỔNG SINH ĐỒ VÔ HẠN

**`ITEM-01`. Tôi đã tự kiểm cả ba mắt xích, không qua tác tử:**

1. `KItemList.cpp:1945-1957` — `sIsJx2ItemScript` liệt kê 9 tiền tố hưởng quy ước JX2 *"`main()` trả ≠ 1 thì engine tự trừ 1 món"*. **Không có `\script\global\mantlesystem\`** (`grep -c mantlesystem KItemList.cpp` = **0**).
2. `item_starore.lua:45-54` và `item_starstone.lua:31-38` — nhánh **thành công rơi khỏi `main()` không `return`** và **không tự xoá mình**; chúng dựa hẳn vào quy ước trên.
3. `KItemList.cpp:2039` — nhánh `else` gọi `ExecuteScript(...)` rồi **bỏ qua trị trả về**; cả khối `:2011-2043` **không có một lệnh `Remove`/`SetStackNum` nào**.

⇒ **PT 4885 (Tinh Ngọc Nguyên Thạch) và PT 4887 (Tinh Thần Khoáng) KHÔNG BAO GIỜ bị tiêu hao.** Bấm 1 viên Khoáng → được 1 Tinh Thần Thạch, **viên Khoáng vẫn còn**. Bấm Nguyên Thạch → chỉ trừ 1 Tinh Hoả Than, ra 15-90 Tinh Ngọc (+5% Mãnh Thiên Tinh Ngọc, +2% Tinh Thần Khoáng), **viên Nguyên Thạch vẫn còn**. Một viên + N Tinh Hoả Than = **Tinh Ngọc không giới hạn**. Không truy ngược thu hồi được, vì không có dòng log trừ đồ nào cả.

**Mức độ thật — tôi đã kiểm, nhẹ hơn báo cáo của bộ mổ:** hiện **người chơi thường KHÔNG với tới được**. Grep toàn cây: ngoài `mantlesystem` và lệnh bài GM, **không script nào phát 4885/4887**; hai tệp giftcode khớp chỉ là **trùng chuỗi số ngẫu nhiên** trong mã quà, không phải mã vật phẩm.

> 🔴 **Nhưng lỗ hổng tự lên nòng đúng lúc anh mở nguồn** — mà mở nguồn chính là việc `A8` / đợt 3 trong danh sách. **Phải vá `ITEM-01` TRƯỚC khi mở bất kỳ đường phát nguyên liệu nào.**

**Vá:** thêm đúng **một phần tử** `"\\script\\global\\mantlesystem\\"` vào mảng `szJx2[]`. Cần build `CoreServer.dll` + swap.
**Vá tạm bằng Lua nếu chưa build được:** thêm `RemoveItemByIndex` ở cuối nhánh thành công của hai tệp — ⚠️ **PHẢI GỠ khi áp bản C++, nếu không TRỪ HAI LẦN.**
**Nghiệm thu:** GM cho 1 viên PT 4887, bấm → nhận 1 Tinh Thần Thạch **và viên khoáng phải biến mất**. Kiểm PT 4883 (`item_tianxingyu.lua` tự `ConsumeItem`) **không bị trừ hai lần**.

---

## 0. VIỆC ĐẦU TIÊN CỦA PHIÊN SAU

Bản vá 31/08 là **thuần Lua, đã nằm trên đĩa nhưng CHƯA SỐNG**. Script chỉ nạp lúc boot (`KSortScript.cpp:88`). Chạy lệnh GM (bí danh `ReLoadSct`, `KGMCommand.cpp:19-20`):

```
RLS \script\global\mantlesystem\mantleupgrade_npc.lua
RLS \script\item\test_phiphong_admin.lua
```

`npc.lua` `Include` `head.lua` (dòng 1) nên nạp lại `npc.lua` là ăn cả hai. Không dùng lệnh GM thì chờ lần khởi động sau — **cấm tự tắt GameServer**.

---

## 1. HÔM NAY (31/08) ĐÃ LÀM GÌ

Triệu chứng chủ game báo: *"bỏ đủ nguyên liệu vào thì khảm nó báo thiếu nguyên liệu"*.

**Cách chốt được gốc** (ghi lại vì phương pháp này dùng lại được): câu *"Hãy đặt vào Tinh Thần Thạch"* có ở **hai** nơi cùng chữ — `head.lua:723` và `npc.lua:182`. Phân biệt bằng **thứ tự chốt**: shim kiểm *Phi Phong trước*; chủ game **không** bị báo thiếu Phi Phong ⇒ hộp gom đồ, `GetGiveItemUnit`, hằng `ITEM_TYPE_*`, `GetItemType` **đều chạy đúng** ⇒ chỉ còn một khả năng: 5 viên bỏ vào **không phải G=9**.

- Đá khảm thật = `starstone.txt`, **G=9 D=1**, tên **có tiền tố** (*Phục Tô / Anh Dũng / Cương Ngạnh… Tinh Thần Thạch*), 34 viên.
- `magicscript` **P=4888 là NGUYÊN LIỆU G=6 D=1, tên đúng bằng "Tinh Thần Thạch"**.

⚠️ **Trùng tên là THIẾT KẾ GỐC — cấm đổi tên hay xoá.** Linux 004 có `P=4553` cùng tên, cùng G=6. Linux cho nó icon riêng `星辰石.spr`, nhưng **ảnh đó không có trong cả client JX1 lẫn client VLTK** (đã quét 42+34 pak), nên 29/08 trỏ tạm sang `星辰石-金.spr` — **trùng khít 7 viên hệ Kim thật**. Lệnh bài lại phát 200 món này ở mục "nguyên liệu" ⇒ rất dễ bỏ nhầm.

**Lỗ hổng thật nằm ở mã tự viết**: bản Linux **báo rõ** (`head.lua:712`), còn shim `doMantleMosaicStoneBox` **bỏ qua im lặng** món không hợp lệ.

### 4 vá đã áp

| # | Tệp | Nội dung |
|---|---|---|
| 1 | `mantleupgrade_npc.lua` | Báo rõ món lạ **kèm tên món**; báo rõ đá còn xếp chồng |
| 2 | `mantleupgrade_npc.lua` | `PF_InlayMoLai()` **mở lại phiên sau MỌI lần chạy** |
| 3 | `mantleupgrade_head.lua:757` | `if nP ~= nil` → `if tbItemIdx[i] ~= 0` |
| 4 | `test_phiphong_admin.lua` | `PP_SODA` 20 → 1 |

**Vì sao vá 2 bắt buộc:** `KProtocolProcess.cpp:6502` **xoá `m_dwGiveBoxId` ngay sau lần bấm đầu** → lần bấm sau rơi vào hư vô (nút như chết). Và đường **thành công** chỉ ăn ĐÁ (`head.lua:797`), **Phi Phong ở lại `pos_affairitem`** — Linux không sao vì client giữ panel mở qua gói `MANTLE_INLAYRESULT`, panel JX1 không nghe gói đó.

**Vì sao vá 3 bắt buộc:** `if nP ~= nil` là niềm tin của bản Linux (`GetItemProp` trả nil cho ô trống). JX1 `LuaGetItemProp` (`KJx2WarInfra.cpp:771-793`) **luôn đẩy đủ 6 số** ⇒ ô trống cho `nP=0`, trùng `tbHasStone[i]=0` của lỗ trống → báo oan *"cùng loại không thể thay thế"*.

✅ 3 tệp qua `syncheck.exe` (Lua 4.0 thật), `check_encoding` FFFD = 0, byte cao đúng như dự tính.
📌 **3 tệp Lua này KHÔNG nằm trong kho git nào** — chỉ có ở cây vận hành `E:\...\bin\server`. Không có gì để commit.

---

## 2. NỀN ĐÃ ĐO CHẮC — ĐỪNG ĐO LẠI

Tự kiểm bằng tay, không qua tác tử:

| Hạng mục | Kết quả | Cách đo |
|---|---|---|
| Hàm Lua Linux thiếu ở JX1 | **0** | `npc.lua` 17/17, `head.lua` 22/22; JX1 chỉ *thêm* 5 hàm |
| API Lua bản Linux gọi mà JX1 không có | **0/61** | 58 binding C, 2 hàm Lua JX1 tự thêm, `CreateTaskSay` có ở `lib\worldlibrary.lua:50` |
| 65 bảng vật phẩm server↔client | **khớp hết** | kể cả `goldequip`, `platinaequip`, `magicscript`, `starstone` |
| 3 nhị phân build↔chạy | **khớp hết** | `CoreServer 74b55a4f` · `CoreClient f24c214e` · `Game.exe c1054e9b` |
| 5 chuỗi bảng mô tả trong `CoreClient.dll` **đang chạy** | **còn đủ** | đọc byte TCVN3 trong nhị phân ở cây chạy |
| 3 cột sao/lỗ/chúc phúc có được nạp không | **CÓ** | `PF_ApplyRowInfo` được gọi ở `KItemGenerator.CPP:1930` (gold) và `:2082` (platina) |

⇒ Hai việc treo trong bản giao 30/08 — *kiểm chuỗi DLL* và *Bạch Hổ Ấn 7380 lệch* — **đều đã xong**.

⇒ **Khoảng cách còn lại hẹp và thuần hành vi**, gốc ở chỗ mượn hộp giao đồ JX1 thay cho giao thức panel gốc của Linux.

---

## 3. ĐÃ BÁC BỎ — ĐỪNG ĐÀO LẠI

Đợt mổ 12 tác tử (5 người đo + 5 người phản biện + soát sót + tổng hợp) đẻ ra 40 mục; **7 mục sau đã bị bác bỏ bằng mã**, ghi lại để phiên sau khỏi mất công:

| Mục | Khẳng định sai | Vì sao sai |
|---|---|---|
| `CPP-03` | *"`GetEquipMaxStoneNum` luôn trả 0 → toàn bộ đường khảm chết hẳn"* | `SetMaxStoneNum` **có** được gọi trong `KItemGenerator` |
| `CPP-02` | *"`GetMaxEquipWishValue` luôn 0 → tỉ lệ đột phá luôn 100%"* | cùng lỗi grep như trên |
| `DATA-01` | *"3 cột sao của goldequip+platinaequip KHÔNG BAO GIỜ được nạp"* | **tự phân xử**: `PF_ApplyRowInfo` gọi ở `KItemGenerator.CPP:1930` và `:2082` |
| `LUA-04` | *"`GetServerOpenDays` tham chiếu bảng không tồn tại"* | các mệnh đề rời thì đúng nhưng kết luận sai |
| `UI-RING-05` | *"8 ô hoa văn bị bỏ trống"* | số liệu đúng nhưng kết luận sai |
| `UI-MSG-06` | *"20 câu thông báo dịch sẵn trong ini chưa dùng"* | mô tả ini lệch, kết luận sai |
| *(tràn chuỗi)* | *"`Value1[256]` bị tràn — lỗi ĐANG XẢY RA"* | **tự phân xử**: cả hai đường dùng `sWStrCpy` **có chặn biên** (`KJx2WarInfra.cpp:827-829` và `:2413-2415`) — **cắt chữ, không tràn** |
| `DATA-02` | *"cột sao platina lệch, phải là 69/70/71"* | Đọc bảng **đúng 100%**, nhưng mã **đã dùng đúng 69/70/71** rồi (`PF_PCOL_*`) — không phải khoảng cách. Kiểm chéo: *Phi phong Tuyệt Thế* ở `goldequip:5375` cột 60/61/62 = `1\|1\|4`, ở `platinaequip:5301` cột 68..71 = `200\|1\|1\|4` |
| `DATA-05` | *"2 nguyên liệu khác mức xếp chồng so với Linux"* | Số liệu đúng, tác động ~0. P=4888 là **nguyên liệu G=6**, không phải đá khảm (đá thật G=9 đọc từ `starstone.txt`) |
| `DATA-06` | *"`mantle.txt` / `shipin.txt` / `signet.txt` không được engine nạp"* | Ba bảng **CÓ** được nạp: `KBasPropTbl.CPP:38-40` + mảng `TABFILE_EQUIPMENT[]` chỉ số 12/13/14 trong 17 mục, vòng `Init` `:154-159` |
| `DATA-07` | *"96 dòng `mask.txt` còn tên Trung là do đợt port"* | Con số đúng nhưng quy trách nhiệm sai — `md5` của `mask/mantle/shipin/signet/starstone` **trùng Linux từng byte** |

Ngoài ra `MODE-01` bị **diễn giải sai**: hai dòng menu Vô Cực ở `npc.lua:51-52` **bản Linux gốc cũng chú thích sẵn** — JX1 chép trung thành, **không phải khoảng cách port**. Và `SEC-01` bị **thổi phồng**: `szLockState` chưa gán là lỗi có sẵn của bản gốc, nhưng dòng 699 là câu **áp chót**, mất đúng **một dòng nhật ký**, và hôm nay **không thể chạy tới** vì menu đang tắt.

---

## 4. KHOẢNG CÁCH CÒN LẠI — XẾP THEO ĐỢT THI CÔNG

Gom theo **số lần phải build + swap**, vì mỗi lần swap là một lần phiền chủ game.

### 4.1 ĐỢT A — thuần Lua, KHÔNG build, làm được ngay

| Mã | Việc | Neo |
|---|---|---|
| `A1` | **Lời hướng dẫn hộp bị cắt câm.** Ô `Value1` chỉ 256 byte; `npc.lua:71` truyền **285 byte**, `npc.lua:107` truyền **298 byte** → cụt giữa câu. Rút gọn hai chuỗi. | `KProtocol.h:1935-1942` |
| `A2` | **Tiêu đề sát trần.** Ô `Value` chỉ **32 byte**; 4 lời gọi trong `npc.lua` (dòng 107/125/162) đang dùng **30 byte**. Thêm 3 chữ là cụt. Đừng nới chuỗi tiêu đề. | như trên |
| `A3` | **Nút "Tiệm nguyên liệu Phi Phong" là nút chết.** `Sale(186, 25, 100, cb, path)` — `LuaSale` (`ScriptFuns.cpp:2697-2711`) **chỉ đọc 2 đối số**, gọi `OpenSale(..., 185, 25)`; `KBuySell::OpenSale` chặn `nShop >= m_Height`, mà `buysell.txt` chỉ 102 hàng ⇒ **thoát ngay, không báo gì**. Việc: thêm hàng tiệm vào `buysell.txt`, sửa `npc.lua:255` cho đúng cú pháp JX1. ⚠️ JX1 **không có callback mua hàng** nên `onShopCallBack` (`npc.lua:357`) sẽ thành hàm mồ côi — đồ mua về **không bị khoá**. 🔴 **Đơn thuốc đã bị nắn:** phải truyền **≥ 3 tham số**, không phải 2 — gọi 2 tham số là **nuốt mất `moneyunit`**; và **không** có chốt kiểm `0..6` như báo cáo đầu nói (nhánh `OpenSale(int,int,int)` không kiểm). | `KBuySell.cpp:654-658` |
| `A4` | **Sửa `head.lua:79`** — chuỗi lỗi chỉ người chơi tới *"tính năng Nâng cấp Vô Cực"* vốn **không có trong menu** (xem 4.5). | `head.lua:71-79` |
| `A5` | **`head.lua:699`** bỏ `szLockState` khỏi `format`, hoặc dùng lại `nBindState` có sẵn ở `head.lua:678`. Ưu tiên thấp, làm cùng đợt nếu mở menu Vô Cực. | |
| `A6` | **Đếm nguyên liệu lạm phát vì khai triển chồng.** `GiveBoxCollect` trải stack thành N mục (`KJx2WarInfra.cpp:865-868`), nhưng script còn cộng `GetItemStackCount` từng mục ⇒ đếm thành N². Sửa **3 dòng** `head.lua:304/429/625` thành `+ 1`. **Không** đụng `head.lua:714` (dòng đó vẫn đúng). Nhớ kiểm lại `head.lua:441-443` và `:639-641` — cùng gốc, còn **chặn đường đột phá**. | |

> ⚠️ `A6` làm **lệch mã so với bản Linux**. Đây là hệ quả bắt buộc của việc mượn hộp giao đồ JX1. Phải ghi chú ngay trong mã.

#### 🔴 A7 — ĐỢT PORT 29/08 ĐÃ ÂM THẦM ĐỔI SỐ LIỆU VẬT PHẨM (`DATA-04`, **làm trước tiên**)

Đây là **vi phạm Gate 4** (đổi cân bằng như tác dụng phụ), người phản biện xác nhận **còn nặng hơn báo cáo gốc**. `goldequip.txt` có **286 dòng đổi**, trong đó **19 dòng đổi CỘT SỐ có nghĩa**:

| Neo | Đã bị đổi thành gì |
|---|---|
| `:5379` **Phi phong Ngũ Phong** | **cột 62 — TRẦN ĐIỂM CHÚC PHÚC — bị XOÁ: `22` → rỗng.** Chính cột hệ phi phong đọc |
| `:5931..:5933` **Tân Nham Hạnh Ấn Giám (Hạ/Trung/Thượng)** | **XOÁ SẠCH** cột 14..19 (`116/10/10/115/10/10` → rỗng) |
| `:3226..:3235` **Ngũ Hành Ấn** | mất hậu tố *"Cấp 1..10"* (10 viên cùng một tên) **và** cột 36 đổi `90` → `120` |
| `:3478/:3479`, `:3481/:3482` | **thêm mới** cột 35 = `47`, cột 36 = `54000` / `120000` (trước đó rỗng) |
| `:3551` **Long Đảm** | cột 15 `30`→`5`, cột 16 `30`→`25` |
| `:4484-4493`, `:4854-4863` | hoán nhãn *"[Không thể gia hạn]"* — **theo đúng bản Linux**, nhưng cần chủ game chốt giữ bản nào |

**Việc:** so từng ô với `goldequip.txt.truoc_phiphong_2908` **cùng thư mục**, trả lại những cột số không cố ý đổi, đặc biệt `:5379` cột 62. Sửa xong **chép đồng bộ sang client** (`bin\client\settings\item\goldequip.txt` hiện trùng `md5` với server — đừng làm lệch).

#### A8 — Người chơi thường KHÔNG CÓ đường nào lấy Phi Phong (`DATA-03`)

Không shop, không rơi đồ, không hộp quà — **chỉ lệnh bài GM mới tạo ra Phi Phong**. Linux phát qua `magicscript` *"Tuyệt Thế Phi Phong Bảo Hạp"* (`Linux magicscript.txt:4976`, PT 4658) trỏ tới `script\item\item_jueshipifengbox.lua`.

⚠️ **Hai điểm người phản biện nắn lại, đọc trước khi thi công:**
- **Chỉ cần THÊM 1 DÒNG magicscript, không phải 5.** Bốn dòng "anh em" `4977..4980` bên Linux là **dòng chiếm chỗ** (`占位符`) trỏ tới `noscript.lua` — 4 mục tương ứng trong `item_jueshipifengbox.lua` là **mã chết**.
- **Cột script của JX1 là CỘT 10, không phải cột 14 như Linux.** (`KBasPropTbl.CPP:1030-1044`: `szIntro(9), szScript(10), nPrice(11), bShortKey(12), nMaxStack(13)…`)
- `ParticularType` vẫn phải **= số dòng − 2**, đúng như 9 dòng `4881..4889` đang có.

Việc đầu tiên là **hỏi chủ game** phát qua đường nào: hộp quà giống Linux, bán ở NPC Thợ Rèn, hay rơi đồ. Cùng gốc: **nguồn rơi nguyên liệu hiện là SỐ KHÔNG** (không phải 3 như báo cáo đầu nói).

#### 🔴 A9 — MỌI HOÀNG KIM ẤN LỆCH LÊN 1 CẤP (bẫy im lặng)

Quy ước chuẩn JX1: *số trong script = chỉ số bản ghi + 1 = **số dòng tệp − 1***. Hệ Phi Phong dùng đúng quy ước đó (`LuaPF_AddGoldEquipByRow` **trừ 1**). Nhưng hệ Ấn đi qua đường khác: `wuxingyin.lua:196` → `lib\awardtype\item_jx1.lua:27-28` gọi `AddItem2(2, 0, tbProp[2], …)` — **không trừ 1**.

Đo được: `goldequip.txt:3206-3215` là 10 dòng *Hoàng Kim Ấn (Cường hoá)*, `:3216` là *Nhược hoá*. `wuxingyin.lua` đánh số cường hoá `3205..3209`. Vậy `3205` → dòng 3207 (**cường hoá thứ 2**), và **`3214` → dòng 3216 = NHƯỢC HOÁ** — Ấn bị làm **yếu** thay vì mạnh nhất. Món nhận về vẫn có tên hợp lệ, chỉ *"thuộc tính không đúng như bảng"*.

⚠️ **PHẢI THỬ BẰNG GM TRƯỚC KHI SỬA** (lệnh bài → `WXY_MoBang()` → nâng cấp 1 → so tên + thuộc tính với `goldequip.txt:3206`). Nếu đúng lệch: `item_jx1.lua:28` → `(tbProp[2] or 0) - 1`. Phạm vi an toàn: grep `nQuality *= *1` toàn cây chỉ ra `wuxingyin.lua`.

#### 🔴 A10 — `mask.txt`: RỦI RO DỮ LIỆU LỚN NHẤT CẢ ĐỢT PORT

Không mục nào trước đó nêu. Đợt port 29/08 **không chỉ thêm dòng mới** — so từng ô với `mask.txt.truoc_phiphong_2908` cho thấy **cả 840 dòng cũ ĐỀU ĐỔI**. (`md5` của `mask/mantle/shipin/signet/starstone` giờ **trùng Linux từng byte** — tức bảng JX1 cũ bị **thay trọn** bằng bảng Linux.) Đúng tinh thần *"giống 100% Linux"*, nhưng **cần anh biết và chốt**, vì mặt nạ là đồ người chơi đang dùng.

---

## 🔎 PHÁT HIỆN CẦN ANH QUYẾT: TĂNG SAO KHÔNG CHO MỘT ĐIỂM CHỈ SỐ NÀO

Đường cộng thuộc tính thật là `PF_ModifyStoneAttrib` (`KItem.cpp:165-192`), lấy `p->nValue[nLv-1]` với `nLv = GetStoneLevel(i)` = **cấp của TỪNG LỖ**. Cấp lỗ chỉ được đặt ở **hai chỗ, đều là đường ĐỘT PHÁ** (`head.lua:501` và `:686`). Đường **TĂNG SAO** (`ProcessStarUpGrade`, `head.lua:336-386`) chỉ gọi `StarLevelUp` rồi `SyncItem` — **không chạm cấp lỗ, không chạm thuộc tính**.

Mà tăng sao là **chế độ sống chính** của NPC, mỗi lần tốn **100 vạn bạc** + Tinh Ngọc, **có thể thất bại** — đổi lại chỉ được **một tiền tố tên món** và **quyền đi tiếp vào Đột Phá** (đòi 10 sao).

⚠️ **Không kiểm chứng được** bên Linux có liên kết sao ↔ cấp lỗ hay không — không đọc được engine từ hai cây được phép. **Đừng khẳng định.**
Nếu anh muốn sao **có** ảnh hưởng chỉ số: cách rẻ nhất là sau `StarLevelUp` thành công gọi thêm `SetStoneLevelOnEquip`. 🔴 **Đó là ĐỔI CÂN BẰNG LỐI CHƠI — tôi không tự quyết, phải anh duyệt tỉ lệ.**

### 4.2 ĐỢT B — một lần build `Game.exe` (client phải tắt)

| Mã | Việc | Neo |
|---|---|---|
| `B1` | **Bấm Huỷ / nút X / ESC KHÔNG trả đồ về túi.** `UiMantleInlay.cpp:77-81` đặt `m_pSelf=NULL` rồi `:85` kiểm `if (m_pSelf)` ⇒ `:86 OnCancel()` **không bao giờ chạy**. Cả 3 đường đóng (`:257` Huỷ, `:259` X, `:269` ESC) đều truyền `bDestroy=true`. Sửa: gọi `OnCancel()` **trước** khi `Destroy()`/gán NULL, xoá hai dòng `:85-86`. **Đồ không mất vĩnh viễn** (vẫn ở `pos_affairitem`, nói chuyện lại NPC là `UpdateData` vẽ lại và kéo ra được) — nhưng chủ game sẽ tưởng mất. 🔴 **Đính chính:** `OnCancel` **KHÔNG** chỉ gửi 1 gói cho cả hộp — nó là **vòng lặp từng món** (`CoreShell.cpp:13292-13314`). | khuôn gốc `UiAffairItem.cpp:68-76` |
| `B2` | **Cùng lỗi ấy còn nguyên ở BẢN GỐC** `UiAffairItem.cpp:70-76` — vá cùng đợt. | |
| `B3` | **Thẻ "Tinh Thần Thạch"** (`[AddStarstoneBtn]`) là `CheckBox=1` nhưng `WndProc` không xử lý ⇒ **bấm một cái là thẻ tắt vĩnh viễn**. | |
| `B4` | **Nhãn dưới ô tràn ra ngoài viền trái.** `obj_10` ở `(12,12)`, nhãn rộng 96 đặt tại `x = 12+12-48 = -24`, mà `m_Pad` có `Left=4` ⇒ âm tuyệt đối. Đè lên khung hướng dẫn và hai ô hoa văn. | `UiMantleInlay.cpp` |
| `B5` | **`OpenWindow` vứt bỏ tham số `pszTitle`** — nhận vào nhưng không dùng lần nào. | `UiMantleInlay.cpp:33-67` |
| `B6` | **`Region.v` bị ghi cứng 0** (`UiMantleInlay.cpp:326` và `:344`) ⇒ đồ ở hàng dưới của hộp giao **hiện nhưng không nhặt ra được**. | |
| `B7` | ❓ **`PF_UI_INPUT` có thể chọn nhầm 5 ô.** Người đo bảo phải là `{5,6,7,8,9,13}` (ngũ giác quanh ô Phi Phong) thay vì `{10,11,12,0,1,13}`; người phản biện chỉ ra **tệp ini có HAI vòng năm ô** nên đơn thuốc là **tung đồng xu**. **Bắt buộc mở panel nhìn tận mắt + đọc ảnh nền `\spr\Ui3\星辰石镶嵌.spr` trước khi sửa.** | `UiMantleInlay.cpp:147` |

### 4.3 ĐỢT C — một lần build `CoreServer.dll` — **gói "chọn lỗ khảm"**

Đây là **khoảng cách chính** so với Linux: bản gốc cho người chơi **chọn đích danh lỗ**; shim hiện tại **tự xếp vào lỗ trống** nên **không thay được viên đã khảm**.

✅ **Đã xác nhận đi trọn vòng** (`XN-01`): số hiệu ô client gửi (`Region.h`) **đi tới tận `PlayerItem.nX`** — **client không phải sửa gì**. Nó bị vứt đúng một chỗ: `KJx2WarInfra_GiveBoxCollect` bỏ qua `nX/nY`.

**Đặc tả vá (6 sửa đổi, làm trọn gói):**

1. `KJx2WarInfra.cpp:804-807` — thêm `std::vector<int> vSlots;` cạnh `vItems`.
2. `KJx2WarInfra.cpp:854` — thêm `s.vSlots.clear();` ngay sau `s.vItems.clear();`.
3. `KJx2WarInfra.cpp:867-868` — đẩy **song nhịp trong cùng thân vòng `u`** để hai mảng không lệch khi chạm trần `JX2GIVE_MAX`.
4. Sau `:889` — thêm `LuaGetGiveItemSlot(Lua_State*)`, khuôn y hệt `LuaGetGiveItemUnit`, trả `-1` khi không có.
5. `ScriptFuns.cpp:15671` — đăng ký `{"GetGiveItemSlot", LuaGetGiveItemSlot}`.
6. `mantleupgrade_npc.lua:222-236` — thay vòng "xếp vào lỗ trống" bằng `tbItemIdx[GetGiveItemSlot(i)+1] = nIdx`.

🔴 **LUẬT CỨNG của đợt này** (`RUI-01`): chỉ **ĐƯỢC push thêm `vSlots`**. **CẤM** đổi thứ tự duyệt `GetFirstItem()/GetNextItem()`, **CẤM** đổi cách khai triển chồng ở `:864-868`, **CẤM** đổi giá trị trả về của `GiveBoxCollect`. Có **29 điểm gọi / 11 tệp `.lua`** đang dựa vào hành vi hiện tại.

🔴 **THỨ TỰ SWAP** (`BS-5`): `GetGiveItemSlot` là **binding mới**. Lua 4.0 gọi hàm nil là `ScriptError` ⇒ **phải swap `CoreServer.dll` TRƯỚC, rồi mới `RLS` script.** Ngược lại là chết đường khảm.

**Nghiệm thu tối thiểu 5 luồng** (đừng bớt): đột phá Phi Phong (`mantleupgrade_npc.lua:268`) · `equip_system.lua:167` · `seasonnpc.lua:415` · `lib\composeex.lua:185` · `missions\yandibaozang\yandiduihuan.lua:108` (chỗ này gọi `GetGiveItemUnit(nCheckedId)` theo **chỉ số động** — nhạy nhất).

**Hai bẫy PHẢI vá KÈM trong cùng đợt, không tách** — vì chính đợt C mở ra chúng:

- 🩸 `MAT-01` **BẪY ĂN ĐÁ**: chọn lỗ vượt số lỗ đã mở thì `InlayStarStone` (`KJx2WarInfra.cpp:2270`) **từ chối lặng lẽ** nhưng `head.lua:797` **vẫn xoá viên đá**. Vá hai lớp: (a) thêm vòng kiểm `i > nMaxStoneNum` ngay sau `head.lua:738`; (b) `head.lua:795-797` đổi thành **chỉ `RemoveItemByIndex` khi `InlayStarStone` trả về thành công**.
- 🩸 `BS-1` **BẪY LỖ 0 SAO**: cùng họ, **xảy ra thường xuyên hơn**. `CheckInlayStarStone` chỉ kiểm cấp sao của lỗ **đã có đá**. Vá **chung một vòng `for`** với `MAT-01` — thêm một dòng là xong.

⚠️ **Đơn thuốc gốc của `INLAY-01` GÂY HẠI, đừng áp nguyên**: thêm `and GetStoneLevelOnEquip(nMantle,i) > 0` sẽ khiến **lỗ trên cùng không bao giờ khảm được** (`head.lua:501` cố ý để nó cấp 0 chừng nào còn là lỗ cuối), và **phi phong mới tinh — mọi lỗ đều cấp 0 — thì cấm khảm hoàn toàn**. Cách đúng: **ưu tiên** lỗ trống cấp > 0 (vòng 1), rồi mới tính lỗ trống cấp 0 (vòng 2).

### 4.4 ĐỢT D — đổi khuôn gói tin (**rủi ro cao, cân nhắc kỹ**)

`PROTO-SYNC-03`: `ITEM_SYNC` **không mang `m_nPfPack`** ⇒ client **mù hoàn toàn** về số sao / số lỗ / đá đã khảm.

- Cách 1: thêm `int[4]` vào cuối `ITEM_SYNC` (`KProtocol.h`), gán ở `KItemList.cpp:4696`, client `SetPfPack`. ⚠️ **`sizeof(ITEM_SYNC)` đổi theo** ⇒ server và client **bắt buộc swap cùng lúc**; lệch bản là **lệch MỌI gói `s2c_syncitem`**, không riêng phi phong. Vi phạm tinh thần Gate 2.
- Cách 2 (**khuyến nghị**): dùng **gói RIÊNG** kiểu `ITEM_SYNC_MAGIC` (`KProtocol.h:1033-1038`), chỉ gửi cho vật phẩm có `GetMaxStoneNum() > 0`.

ℹ️ `BS-3`: **người chơi ĐÃ CÓ đường xem lỗ nào có đá gì** — `KItem::PF_AppendDesc` (`KItem.cpp:216-256`) in thẳng vào bảng mô tả. Nên đợt D là **tiện nghi**, không phải chặn.

### 4.5 CẦN CHỦ GAME QUYẾT

| Việc | Vì sao phải hỏi |
|---|---|
| **Mở nhánh Vô Cực / đột phá lần 2?** | Bản Linux gốc **cũng tắt sẵn**. Mở ra là mở một đường **chưa từng chạy trên JX1** (platina + 10 lần nâng cấp + chép đá). 🔴 **CẤM chọn phương án "xoá cho gọn"** — đó là tự ý lệch khỏi mốc Linux. |
| **Gắn Hoàng Kim Ấn vào `thoren.lua`?** | `WXY_MoBang()` đã có sẵn (`wuxingyin.lua:295`), gọi được từ lệnh bài (`test_phiphong_admin.lua:465`), **chưa gắn NPC**. Người chơi thấy ngay. |
| **Đường lấy Phi Phong cho người chơi thường** (`DATA-03`, chưa thẩm định) | Nếu đúng thì hiện chỉ có lệnh bài GM mới ra được Phi Phong. |

---

## 5. CÁCH ĐIỀU TRA TIẾP

### Bộ công cụ đã dựng sẵn

`C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\93867973-c89d-4811-8422-13404ccb0fa6\scratchpad\ktkm\`

| Tệp | Dùng để |
|---|---|
| `luac\syncheck.exe` | **Kiểm cú pháp Lua 4.0 THẬT** (không phải Lua 5). Dựng lại bằng `build_sc.bat`. Lỗi `IncludeLib` là *runtime*, nghĩa là **tệp đã biên dịch xong** — không phải lỗi cú pháp. |
| `pp_tim_anh4888.py` | Dò một `.spr` trong **mọi pak** của cả hai client (băm tên duyệt `char` **có dấu**) |
| `pp_do_chuoi_hop.py` | Đo chuỗi truyền cho hộp giao đồ so với bề rộng ô gói tin |
| `pp_kiem_chuoi_dll.py` | Đọc byte TCVN3 trong nhị phân **ở cây chạy** để xác minh vá đã sống |
| `pp_ket_tinh.py` → `ket_tinh.txt` | **170 KB** — toàn văn 40 mục + phản biện + 24 gap bỏ sót |

### Đợt mổ đa tác tử

Kịch bản: `...\93867973-...\workflows\scripts\phiphong-do-khoang-cach-linux-wf_f83f11a4-1a1.js`
Bản ghi: `...\subagents\workflows\wf_f83f11a4-1a1\`

Chạy lại bằng `Workflow({scriptPath: "<đường dẫn trên>"})`. **`resumeFromRunId` chỉ dùng được trong cùng phiên**, phiên sau phải chạy mới.

**Trạng thái khi giao:** 5 chiều đo + 5 phản biện **đã xong** (40 mục: **29 đứng vững, 11 bị bác bỏ**). Hai bước cuối — *soát sót* và *tổng hợp* — **chưa chạy xong**, nên có thể còn gap chưa ai chạm tới. Chạy lại kịch bản là ra.

### ⚠️ Mức độ tin cậy — đọc kỹ

Đợt mổ này sinh **3 dương tính giả nghiêm trọng**, mỗi cái đều **tự nhận là "chặn hẳn"** và đều **sai vì lỗi grep**: `CPP-03`, `CPP-02`, `DATA-01` (cả ba cùng khẳng định sai rằng hàm nạp cột sao "không có nơi gọi nào"). Người phản biện cũng **nắn lại đơn thuốc** của `INLAY-01` (bản gốc **gây hại**), `UI-02` (tung đồng xu), `UI-03`, `SEC-01`, `MODE-01`, `DATA-08`.

⇒ **Luật cho phiên sau: không thi công theo bất kỳ mục nào mà chưa tự mở tệp đọc lại neo.**

### Phương pháp đã tỏ ra hiệu quả

1. **Chốt nơi báo lỗi bằng THỨ TỰ CHỐT**, không bằng nội dung chuỗi (nhiều nơi trùng chữ).
2. **Đọc byte trong nhị phân ở cây CHẠY** — đừng tin build suông.
3. **Tự phân xử khi hai tác tử mâu thuẫn** — đã bắt được 3 dương tính giả nghiêm trọng (`CPP-03`, `DATA-01`, *tràn chuỗi*), mỗi cái đều tự nhận là "chặn hẳn".
4. **Đo bằng công cụ, đừng đếm tay** — bẫy sprite 29/08 sinh ra vì chỉ kiểm tệp rời, quên 42 pak.

### ⚠️ Bẫy trong chính các đơn thuốc

`BS-2`: một đơn thuốc viết `"L\307 kh\271m"` — **sai mã TCVN3**, thi công y nguyên sẽ ra *"Lầ khạm"*. **Mọi chuỗi tiếng Việt phải qua `vn_edit.py` / `unicode_to_tcvn3_bytes`, cấm tự gõ `\xNN`.**

---

## 6. BẪY VẬN HÀNH (đã dính, đừng dính lại)

- **CẤM tự tắt/restart `GameServer`.** Báo trước khi swap, luôn sao lưu.
- **Phiên khác đang làm cùng cây nguồn.** Đã va chạm 3 lần. ⇒ Kiểm tiến trình build trước khi build; **tìm dòng theo MÃ vật phẩm, đừng theo số dòng**.
- **`sed`/heredoc của bash nuốt backslash.** Mọi chuỗi có `\` phải qua tệp viết bằng công cụ ghi tệp.
- **Tệp TCVN3 chỉ sửa bằng `safe_edit.py` / `vn_edit.py`**; kiểm byte cao không đổi + FFFD = 0.
- **Neo thay thế phải đủ dài.** `tbCurMantle:ProcessInlayStarStone();\nend` khớp **2 lần** (hàm gốc Linux kết thúc y hệt) — suýt vá nhầm.
- **Script chỉ nạp lúc boot.** Sửa `.lua` xong phải `RLS`, nếu không là vá nằm chết trên đĩa.
- 🔴 **CẤM xoá dòng thừa trong `bin\client\package.ini`.** `package.ini` khai 36 khoá `0..35` nhưng **4 tệp không tồn tại** (`sprvlngaothe2.pak`, `settings.pak`, `ui.pak`, `script.pak`). Hiện tại **hoàn toàn vô hại** — `XPackFile::Open` thất bại rồi bị bỏ qua. Nhưng `KPakList::Open` (`KPakList.cpp:144-149`) duyệt khoá `0,1,2,…` và **`break` ngay khi thiếu một số** ⇒ xoá dòng `3=sprvlngaothe2.pak` mà **không đánh số lại liên tục** sẽ làm client **ngừng nạp cả 32 pak còn lại** = mất gần hết tài nguyên. Muốn dọn thì phải đánh số lại `0..31` **và chạy thử client trước khi giao**.
- **Ảnh hệ phi phong không thiếu cái nào** (43/43 có trong pak). Riêng `\spr\item\obj_yingchundai.spr` thiếu thật — **có sẵn từ trước đợt port**, không phải do port.

---

## 7. GHI NHỚ LIÊN QUAN

`jx1-phiphong-port-2908` · `jx1-crash-extsuite-null-2908` · `jx1-magicscript-3bay-3008` · `jx1-bang-chuoi-vltk-slistcache` · `jx1-say-menu-512byte` · `jx1-pak-vltk-ucl-nrv2b` · `jx1-pak-hash-char-co-dau` · `jx1-nap-san-moi-script` · `jx1-tcvn3-chu-hoa-mat-dau`
