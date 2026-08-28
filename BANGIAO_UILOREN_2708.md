# BÀN GIAO — HỒI SINH BOX GIAO DIỆN LÒ RÈN 7 KHUNG (ép đồ tím + ép Hoàng Kim môn phái)

> Ngày 27/08/2026. Tài liệu này do phiên sau **dựng lại từ transcript + output workflow** của phiên
> `cfb4f299-f5e4-47c6-a437-773fc54534bb` — phiên đó **chết lúc ~14:06 vì hết hạn mức tuần**
> (weekly limit, reset 2/09 9h sáng Pacific) ngay sau khi bấm chạy phản biện cuối, **chưa kịp ghi chú**.
> Nền tảng trước đó (port 14 Lua + 4 bảng từ client VLTK, v28–v35): xem ghi nhớ `jx1-loren-vltk-client-2708`.

---

## 0-BIS. ĐỢT 2 — PHẢN BIỆN ĐÃ CHẠY XONG + VÁ 20 MIẾNG (27/08, ~15h20)

**Phiên sau đã chạy đủ vòng phản biện 3 hướng mà phiên cũ nợ**, tự phân xử mâu thuẫn giữa các
hướng bằng cách đọc mã, vá 20 miếng, build sạch cả 3 nhị phân và đặt lại (băm khớp).
Bản cũ giữ ở `*.truoc_phanbien`, nguồn cũ ở `*.truoc_uiloren2`.

### Kết quả phản biện

| Hướng | Kết luận |
|---|---|
| An ninh / trục lợi | **Sạch** — không nhân đồ, có thu phí `Pay(COMPOUND_COST)`, trần ngày + log đã port, `case 7` an toàn với gói tự chế, bộ lọc `KFoundryResDemand` **chặt hơn bản gốc**; hệ hiện chỉ admin mở được |
| Mất đồ người chơi | Đồ trong 6 phòng **không mất** khi logout/chết/đổi map (lưu DB nguyên `nPlace`, relog nạp lại đúng phòng). Bắt được 1 lỗi 🔴 kinh tế (xem dưới) |
| Client vẽ / đồng bộ | Tràn 8↔3 đã hết ở cả 6 cặp; bắt thêm 5 lỗi 🟡/🟢 chưa ai biết |

### 🔴 Lỗi dup nguyên liệu xếp chồng — ĐÃ VÁ

`KItemCompound.cpp:918-935` tính giá trị nguyên liệu **× cả chồng** (dịch ngược Linux `0x08153F21`),
nhưng `RemoveItemByIndex(idx)` một đối bên JX1 chỉ trừ **1 đơn vị** (`KJx2WarInfra.cpp:896-916`).
Bản VLTK gốc gọi một đối vì **engine của họ xoá cả chồng** — lệch ngữ nghĩa khi port.
`KhopTietDoan` (`:429`) chặn đồ chồng ≥2 ở mọi ô **trừ 18 mục `ConsumeItem_*`** (`Stackable=1`:
Thần bí khoáng thạch, Thuỷ tinh 238-240, Mảnh thiên thạch 1317-1325…) ⇒ **đúng các ô đó là dup thật**.

> 🔑 **Phân xử quan trọng**: một hướng phản biện kết luận bộ Lua trên máy chủ là "bản Trung 2005"
> và đề nghị **chép đè bằng bản viết tay**. **SAI** — kiểm băm 7/7 tệp: bộ đang chạy **chính là bản
> VLTK gốc đã nắn mã v34** (backup `.truoc_va5` còn nguyên). Chỉ vá **2 dòng**, không chép đè.
> `fantasygold*_upgrade.lua` vốn đã đúng dạng 2 đối từ trước.

### 20 miếng đã áp (`ui_loren\vA/vB/vC_*.py`, đều điền tập, giữ CRLF, byte cao không đổi, FFFD=0)

| Bộ | Tệp | Nội dung |
|---|---|---|
| `vA_server_loren.py` | `compound_header.lua`, `ore_upgrade.lua` (**cả 2 cây** E: và `ra\lua`) | truyền `GetItemStackCount(idx)` → ăn đúng số đã tính tiền |
| | `KItemCompound.cpp` | bỏ qua nguyên liệu **đang khoá** trong vòng lọc (`:1007`) — chặn huỷ đồ khoá vĩnh viễn |
| `vB_client_ui_loren.py` (11 miếng) | `UiCompoundItem.cpp` | **L1** thẻ Khảm nạm hết tắc (`nDetail = -1` → rơi `default`, tới được lệnh gửi) · **L2** bỏ nhánh "nạp" ở **cả 6 pad** → 1 cú bấm = 1 lần ép, hết treo ảnh hiệu ứng · **L4** đóng cửa sổ thu hồi **cả 6 phòng** thay vì mỗi thẻ đang mở · **L5** mở lại NPC không còn sáng lệch thẻ · **L6** `LoadScheme` khung 3 trỏ nhầm bảng · **L7** `case 2` Distill thiếu `break` |
| `vC_uishell_loren.py` (4 miếng) | `UiShell.cpp` | thêm cửa sổ lò rèn vào bộ đóng chung → **ESC đóng được**, giao dịch/đánh cược không chồng cửa sổ, thoát game **destroy thật** (hết ô đồ ma xuyên phiên) |

### ⚠️ MỘT MIẾNG BỊ HOÃN CÓ CHỦ Ý

Miếng "đóng phiên lò rèn phía máy chủ" (`m_dwCompoundItemId = 0` trên đường `c2s_recoveritem`)
**đã bị gỡ khỏi bộ vá**: cả 6 pad gửi `GOI_RECOVER_ITEM` từ nút **Huỷ bỏ** mà **không đóng cửa sổ**
(`UiCompoundItem.cpp:869, 1346, 1901, 2484, 2866, 3661`) ⇒ bấm Huỷ một lần là cổng
`if (m_dwCompoundItemId > 0)` (`KProtocolProcess.cpp:6510`) khoá, **mọi cú bấm ép sau im lặng**.
Mã miếng vá giữ nguyên trong `vA_server_loren.py` (biến `M3_CU/M3_MOI`) để dùng lại.
**Hướng đúng cho đợt sau**: thêm callback đóng cửa sổ gọi `EndCompoundItem`.
Hệ quả khi chưa vá: mở lò rèn một lần rồi rời NPC vẫn ép được tới khi relog — **không mất đồ,
không trục lợi** (vẫn đủ phí + lọc nguyên liệu + trần ngày).

### Còn mở sau đợt này

1. **Thẻ Khảm nạm đang mượn bố cục thẻ Trích lấy** — bản gốc có tệp riêng (`装备镶嵌`: nền `分页镶嵌`,
   nút "Khảm nạm trang bị", 18 ô khác vị trí, không có nút Huỷ). **5 ảnh còn thiếu đã rút xong**
   vào `pak_vltk\ra_vltk\spr_khung\`: `A57403C0` `616BDFBF` `240D5214` `93A84648` `43294427`.
2. **Thẻ Đồ phổ HKMP (Atlas)** — nút đang `Disable=1`, chưa có lớp cửa sổ. **Máy chủ đã sẵn ~80%**:
   `COMPOUND_ATLAS = 6`, `compoundscript.txt` → `atlas.lua` (đã port), 2 nhóm khoá
   `CompoundGold_Cryolite/Atlas`, và `s_anSoO[6] = 8` **khớp `compoundpart_num = 8`** ⇒
   **không cần thêm giá trị `pos_` mới** (tái dùng khuôn phòng 8 ô). Bố cục gốc: 6 ô nguyên liệu
   (`Box1..6`) + `AtlasBox` + `CryoliteBox` + nút `融合` (Chế tạo) + nút `预览` (Xem trước tỉ lệ).
3. 🟡 **`g_ComItem`** (`UiCompoundItem.cpp:41, :370`): menu chuột phải truyền `&g_ComItem` — một thể
   hiện toàn cục **không bao giờ `Initialize()`** — trong khi mọi cửa sổ khác của client truyền `this`,
   và bộ xử lý `:319` đòi `uParam == this` ⇒ chọn menu xong **rơi vào hư không**. Chưa vá.
4. `RuleInfo` thẻ Trích lấy bị cắt (gốc 562 byte > `Buff[512]`); 8 ô nguyên liệu mất khoảng lùi
   `UnitBorder=2` của lưới gốc.

---

---

## 0-TER. ĐỢT 3 — TEST THẬT VỚI CHỦ GAME (27/08 chiều tối)

Chủ game test trực tiếp, báo 5 lỗi. Mỗi lỗi đều **truy đến gốc bằng cách đọc bản Linux**,
không đoán. Ba lần đầu tôi vá sai vì đoán — ghi lại đây để phiên sau không lặp.

| # | Chủ game báo | Gốc thật | Vá |
|---|---|---|---|
| 1 | Rê chuột vào Chu Sa = **dis client** | Intro khoáng ở bản gốc dài **186–191 byte** (client VLTK chạy tốt) nhưng `KBASICPROP_MAGICSCRIPT::m_szIntro` của JX1 chỉ **128 byte** → `KTabFile` cắt ở byte 127, **cắt trúng giữa thẻ `<color=`** → `TEncodeCtrl` đi tìm `>` ra ngoài phạm vi, bắt nhầm `>` của thẻ phía sau → `nParamLen ≈ 21` → `memcpy` vào `static char Color[13]` (`Text.cpp:700`) → **tràn ngăn xếp, sập cứng, không kịp ghi log** | `vK` chặn biên 3 chỗ chép (`Text.cpp`) · `vM-M1` nới `m_szIntro` → 256 |
| 2 | Đá "dư dấu `<`" | cùng gốc #1 (chuỗi cắt cụt) | như trên |
| 3 | Đá **chưa có hệ** | `KItem.cpp:1463-1470` ép `nfkSerial = series_nil` cho **mọi** genre 6 trừ 398/399. Bản gốc IN ngũ hành cho viên **ẨN** (`prop_ore.lua:21-23`, `primitive_ore.lua:14-16`) | `vM-M3` |
| 4 | Đồ tím **không có vòng sáng** | Client **đã có sẵn** hệ vòng sáng 4 màu vẽ bằng mã (`vongsang.cpp`: `DrawBorder` chạy quanh + `DrawBorder2` nhấp nháy), nhánh tím `ehuyenkim` nằm sẵn ở `WndObjContainer.cpp:562`. Cổng bật là `GetColorItem()`, mà hàm này nhận đồ tím qua `IsPurple()` = `m_CommonAttrib.nPoint` (đồ tím **kiểu cũ**); đồ tím lò rèn dùng `nItemNature = NATURE_VIOLET`, `nPoint = 0` → trượt | `vL` thêm nhánh NATURE_VIOLET vào `GetColorItem`/`GetKind`; `vO` giữ phẩm chất trên 3 đường đồng bộ |
| 5 | Bỏ đủ liệu vẫn báo **"không đúng luật ghép"** | `KItemCompound.cpp:213` gộp `Distill_Equip` + `Distill_OrgMine` vào **một nhóm "hoặc"**, chú thích ghi *"nguồn: đồ HOẶC khoáng"* — **hiểu sai bản gốc**. `magic_distill.lua:48` đòi **CẢ HAI**: `if( nEquipIdx <= 0 or g_nDistillMagicPos <= 0 ) then return RESULT_LACK_RESOURCE`. Khoá cùng nhóm thay thế nhau + mỗi liệu chỉ phục vụ 1 nhóm ⇒ trang bị chiếm chỗ, viên khoáng thành "ô thừa", mà `s_anKhoaThua[3] = -1` ⇒ RULE_ERROR | `vP` tách 3 nhóm; **nhớ sửa cả bảng đếm `{s_aryNhom3, 2}` → 3** ở `sLayNhom` |
| 6 | "Lò rèn gặp lỗi không rõ" + `ScriptError` **stack Overflow** ở `calcProbLoop` | ĐANG DỞ — xem dưới | `vR` chốt độ sâu + bật log gốc |

### 🔴 LỖI #6 CÒN DỞ — stack overflow `calcProbLoop`

`ScriptError.log`: `magic_distill.lua / Compound` → `error: stack Overflow`,
traceback `calcProbLoop` (itemvalue_header.lua:136) lặp nhiều tầng → `TransItemVal:38`
→ `defFinalCompound:109`.

**Đã loại trừ bằng số** (đừng kiểm lại): bảng giá trị **KHÔNG thiếu** — `settings\item \`
và `001\` đều đủ 10 tệp `itemvalue\*` + `magicattriblevel(.._index).txt`, trùng từng byte
với bộ VLTK; `ITEM_VERSION = 1` nên `makeItemFilePath` tìm `001\` — có thật;
`itemvalue_header.lua` trên máy chủ **trùng băm với bản VLTK** (`aa81d0c147de`);
`magic_distill.lua` khớp bản Linux (chỉ lệch mã đã nắn).

**Cơ chế tràn**: `calcProbLoop` đệ quy chia đôi, chỉ hội tụ khi dãy giá trị là số thật.
Nếu một phần tử là **NaN** thì cả ba nhánh so sánh (`:72`, `:77`, `:93`) đều false →
luôn rơi vào `else` → đệ quy với **đúng khoảng cũ** → vô hạn.

**Điểm nghi chính (CHƯA chứng minh)**: `compound_header.lua:97` gọi
`ITEM_CalcItemValue` với **10 tham số** (`info[1], info[3]..info[11]`), trong khi
`getCompoundParam()` nằm ở **`info[12]`** nên **không bao giờ được truyền**;
`LuaCmp_ITEM_CalcItemValue` (`KItemCompound.cpp:1155`) đọc `szParam` ở **vị trí 11**
→ luôn nhận `""`. Cần xem `CalcItemValueByInfo` với `szParam` rỗng trả ra gì.
⚠️ Lưu ý hàm đó chỉ xử lý `nParamNum <= 2` hoặc `>= 8`; rơi vào 3..7 thì trả 0.

**Đã làm để không sập nữa + lấy số liệu** (`vR_chan_tran_calcprob.py`):
- `CALCPROB_MAX_DEPTH = 64` + tham số `nDepth`: quá ngưỡng thì chia đều xác suất rồi
  thoát, ghi `WriteLog`. Dãy ≤ 32 phần tử nên độ sâu thật ~32 — chốt không chạm
  đường chạy đúng.
- **Bật lại 6 dòng `Msg2Player` gỡ rối vốn có sẵn trong bản gốc**
  (`compound_header.lua:88-90, 98, 100`) → in `TotalSrcSum` và `DesValue1..10`.
  Bấm một lần là thấy con số nào bất thường. Tắt lại bằng `python vR_chan_tran_calcprob.py --tat`.

### Bộ vá đợt 3 (đều điền tập, giữ byte, có bản lùi riêng)

`vJ` (bỏ, không áp — tự biên Intro, SAI cách) · `vK` chặn tràn tên màu · `vL` vòng sáng tím ·
`vM` nới Intro + đọc đúng trường mã thuộc tính + hiện ngũ hành ô ẩn · `vN` bộ test phát
nguyên khoáng theo `trangbitim.lua` gốc · `vO` giữ phẩm chất 3 đường đồng bộ ·
`vP` tách 3 nhóm luật Trích lấy · `vQ` (không cần — bảng đã đủ) · `vR` chốt tràn + log.

### 🔑 Nền tảng tra được từ bản Linux (dùng lại cho mọi việc sau)

- **"Hiện"/"Ẩn" (明/暗) là TÊN 6 Ô KHẢM**, không phải thuộc tính bị giấu.
  pos 1/3/5 = hiện 1/2/3 · pos 2/4/6 = ẩn 1/2/3.
- Khác biệt **duy nhất**: ô **ẨN (pos chẵn) bắt buộc ngũ hành khoáng = ngũ hành trang bị**
  (`equip_enchase.lua:67`, `magic_distill.lua:51`); ô hiện không ràng buộc.
  ⇒ **vì thế viên ẩn mới cần đủ 5 hệ**, viên hiện chỉ 1 (`trangbitim.lua:136-157`).
- **Hai dãy vật phẩm, đều lệch −1 khi sang JX1** (số vị trí `pos` thì KHÔNG đổi,
  nên **tính chẵn/lẻ của mã ĐẢO** — bẫy dễ vá sai):
  nguyên khoáng RỖNG Linux 149..154 → **JX1 148..153** (`pos = ptc - 147`);
  khoáng ĐÃ mang phép Linux 200..205 → **JX1 199..204** (`pos = ptc - 198`).
- **Bản gốc KHÔNG BAO GIỜ phát trực tiếp dãy khoáng đã mang phép** — quét cả cây Linux,
  không một `AddItem`/bảng rơi đồ nào cấp 200..205. Chúng chỉ sinh từ Trích lấy
  (`magic_distill`) hoặc gộp 3 viên (`ore_upgrade`).
- Đồ tím đúc ra có **1..5 ô** (`equip_compound.lua:50-56`, `-1` = ô rỗng khảm được),
  lấp theo thứ tự 1→6 ⇒ món 5 ô = 3 hiện + 2 ẩn. **Ô thứ 6 (ẩn 3) không sinh ra từ
  đường đúc chuẩn** — chưa tìm được đường nào tạo nó trong cây Linux.


## 0. TRẠNG THÁI MỘT DÒNG

**Mã đã hàn xong 100%, build sạch, đã đặt lên máy chủ + client (băm khớp), menu test đã móc.
NHƯNG vòng phản biện đối kháng cuối cùng CHƯA HỀ CHẠY (cả 4 agent chết vì hết hạn mức — kết quả RỖNG),
và CHƯA khởi động lại / CHƯA test trong game.**

⚠️ Câu cuối phiên cũ nói "đang chạy phản biện đối kháng lần cuối" — **đừng tin là đã có phản biện**.
Đọc `wz7bt76sb.output` thì thấy: `chot: null, chi_tiet: []`, cả 4 agent (mất-đồ · an-ninh · ui-vẽ · chốt)
đều lỗi "You've hit your weekly limit". Mã đang nằm trên đĩa là mã **chưa qua phản biện cuối**.

---

## 1. ĐANG NẰM TRÊN ĐĨA (đã kiểm 27/08 chiều, băm/chuỗi đã soi)

Cây chạy thật `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE`:

| Tệp | Giờ | Ghi chú |
|---|---|---|
| `bin\server\CoreServer.dll` | 13:56 | build 13:52 từ D — **có `S12-TELE` + danh sách trắng `LR_UI_*` + `EndCompoundItem`** |
| `bin\client\CoreClient.dll` | 13:56 | build 13:52 — **có `S12-THEO`** |
| `bin\client\Game.exe` | 13:57 | build 13:56 — **có `khamnam/Tinhluyen/Khamnammain` + `LR_UI_MotOre`** |
| `bin\client\Ui\Ui3\khamnam\{Khamnammain,Tinhluyen,Chetao,Lay}.ini` | 13:54 | 4 khung sinh từ 9 tệp gốc VLTK |
| `bin\client\spr\ui3\铸造装备\` (tên GBK) | 13:5x | **16/16 ảnh** — 5 ảnh thiếu đã rút thêm từ pak VLTK theo uid |
| `bin\server\script\item\compound\compound_ui.lua` | 13:55 | 6 callback + `LR_UI_Mo()` (TCVN3, Lua 4) |
| `bin\server\script\item\test_loren_admin.lua` | 13:55 | dòng 20 Include compound_ui; dòng 78 "Mở thợ rèn ngay tại đây/LR_MoThoRen"; dòng 309 "Mở cửa sổ lò rèn thật/LR_UI_Mo" |

**Sao lưu để lùi:** nhị phân cũ `*.truoc_ui7khung` (3 tệp, cạnh chỗ đứng); nguồn cũ
`*.truoc_uiloren` (10 tệp trong `D:\GAMEDEVNEW\Sources\...`).

> 🔑 **Nhị phân hiện tại GỘP MỌI PHIÊN 27/08**: S9→S12 (WAuto) + kinh mạch (bản 13:40 của phiên kinh
> mạch đã được ôm vào build 13:53+) + UI lò rèn. Khởi động lại MỘT lần là test được cả ba mối.
> Câu "bản trên đĩa là của phiên WAuto" trong `BANGIAO_TOMTAT_DONGBO_2708.md` viết lúc 13:18 —
> **đã lỗi thời** từ 13:56.

---

## 2. BỐN BỘ VÁ MÃ + MỘT BỘ SINH KHUNG (64 miếng, 10 tệp — TẤT CẢ ĐÃ `--ghi`)

Công cụ ở `D:\GAMEDEVNEW\ReverseTools\loren\ui_loren\` — mọi patcher đều điền tập/idempotent
(chạy lại báo "ĐÃ CÓ - bỏ qua"), giữ CRLF + byte TCVN3/GBK, mỏ neo lệch là báo lỗi to không ghi.

| Patcher | Tệp đích | Nội dung |
|---|---|---|
| `v36_khoi_phong_server.py` | `KItemList.h/.cpp` | 19 miếng: 6 mảng phòng đồ `m_CompOneItem…m_EnchaseItem` + 6 getter + 18 hàm Check/Add/Un theo khuôn `pos_tremble`; mở khối thu hồi đồ (sửa luôn 2 tên hàm chết `Add→AddKIL`, `ItemSet.Add→AddI`) |
| `v37_nut_bam_server.py` | `KPlayer.h/.cpp`, `ScriptFuns.cpp`, `KProtocolProcess.cpp` | 12 miếng: `m_dwCompoundItemId` theo khuôn tremble; `LuaOpenCompoundItem` lưu id + `EndCompoundItem` mới; mở 6 case 4..9 của `LuaGetIdItem` (kẹp 7/7/7/10/1/10); **`UiCommandScript` case 7** với danh sách trắng 6 tên `LR_UI_MotOre/HaiOre/BaOre/Distill/Forge/Enchase` |
| `v38_client_core.py` | `CoreShell.h/.cpp`, `GameSpaceChangedNotify.cpp` | 6 miếng: 6 enum `GDI_COMPONE_ITEM…GDI_ENCHASE_ITEM` + GetGameData + SWITCH_OBJECT P1/P2 + RECOVER + notify 6 container về `KUiCompoundItem` |
| `v39_client_ui.py` | `UiCompoundItem.cpp` | 27 miếng: nối 6 nút sang `GOI_ADD_UI_CMD_SCRIPT, 7, "LR_UI_*"`; mở 6 khối `UpdateAllItem`; mở `GOI_RECOVER_ITEM` ở CloseWindow/OnCancel; `bCanSet=TRUE`; **sửa 2 lỗi CÓ SẴN từ 2021**: khung 3 SetContainerId chép nhầm `UOC_COMPTWO`→`UOC_COMPTHREE`, vòng lặp Forge `_ITEM_COMP_COUNT(3)`→`_ITEM_FORGE_COUNT(2)` (tràn mảng thật) |
| `v40_khung_ini.py` | sinh `ra_khamnam\*.ini` + chép spr | 4 khung ini: đủ từng section/khoá đúng mã đọc; **JX1 đã đánh số lại `ReturnInfo`** (Tinhluyen 1..16, Lay 1..21 — khác hẳn gốc VLTK) nên bảng thông báo viết lại theo mã, không chép nguyên gốc |

**Lỗi nghiêm trọng kiểm chéo bắt được — ĐÃ SỬA TRƯỚC KHI ÁP** (sửa tay `v38` lúc 13:51):
v38 gốc ghi **8** phần tử cho GDI_COMPONE/TWO/THREE trong khi UI khai mảng **3** (`_ITEM_COMP_COUNT`)
→ tràn stack client mỗi lần refresh pad. Đã ép 3 case đó lặp 3 (ô 3..7 UI không bao giờ dùng — Ore1..3 là toàn bộ).

Sau khi áp: **0 byte FFFD trên cả 10 tệp** (không tái diễn tai nạn FFFD sáng 27/08 — vụ đó cứu bằng git,
xem `v35_cuu_byte_fffd.py` + các bản `.hong_fffd_2708`).

---

## 3. ĐƯỜNG VÀO TEST (sau khi khởi động lại máy chủ + phát client mới)

1. Phát cho máy test: `CoreClient.dll` + `Game.exe` + thư mục `Ui\Ui3\khamnam\` + thư mục spr GBK `铸造装备`.
2. Khởi động lại GameServer (nhị phân + script server đã đổi).
3. Trong game: **Lệnh bài Admin → Bộ test lò rèn (`LR_Root`) → "Mở thợ rèn ngay tại đây" → "Mở cửa sổ lò rèn thật" (`LR_UI_Mo`)**.
4. Cửa sổ 7 thẻ đúng đồ hoạ VLTK mở ra; kéo đồ vào ô, bấm nút → server `FoundryCompound` xử thật.
   Nhớ: Huyền Tinh phải là **1 viên rời không xếp chồng** (đúng bản gốc, mỗi ô một món).

## 4. VIỆC CÒN LẠI + RỦI RO PHẢI SOI KHI TEST

1. 🔴 **Chạy lại phản biện đối kháng** (3 hướng như phiên cũ định: mất đồ người chơi · an ninh/trục lợi ·
   client vẽ/đồng bộ) — hoặc chấp nhận test thẳng rồi soi kỹ. Prompt gốc còn nguyên trong
   `wz7bt76sb.output` (xem §5).
2. 🟡 **Thẻ Khảm nạm (Enchase) nghi còn tắc ở cổng thứ hai**: `ProcessEnchase` ~dòng 3255 gán cứng
   `nDetail=0` rồi đòi `nOption[0]==53`, mà các dòng đọc `GDI_ITEM_MAGICTYPE` vẫn bị chú thích
   (enum chưa tồn tại) → có thể **luôn báo lỗi ReturnInfo 16** khi bấm. v39 chỉ được phép mở `bCanSet`
   nên chưa đụng — nếu test dính đúng vậy thì cần miếng vá riêng (khai `GDI_ITEM_MAGICTYPE` thật ở v38
   hoặc vá nhánh nDetail).
3. 🟢 Nút thẻ Khảm nạm mang hình/tooltip "Trích lấy" (mã C++ đọc cứng section `DistillBtn` — quirk gốc, chỉ xấu).
4. 🟢 `RuleInfo` Distill 558B > Buff[512] — đã cắt gọn lúc triển khai (transcript bước 9897).
5. 🟢 Phòng server compone/two/three có 8 ô nhưng UI chỉ bày 3 ô — vô hại (ô 3..7 không bao giờ có đồ).
6. 🟢 `CheckXxx` cố ý **cho qua mọi loại vật phẩm** — `KFoundryResDemand` mới là nơi lọc thật khi bấm nút (đúng kiến trúc gốc, xem ghi nhớ).

## 5. BẰNG CHỨNG GỐC (đọc lại được bất cứ lúc nào)

Thư mục task của phiên chết: `C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\cfb4f299-f5e4-47c6-a437-773fc54534bb\tasks\`

| Tệp | Nội dung |
|---|---|
| `wughw5gxy.output` (55KB, 13:18) | **Bản đồ toàn hệ UI lò rèn** (client 7 khung + server 6 phòng) + kế hoạch nối dây + phát hiện 2 lỗi 2021 |
| `wqe6s7j7a.output` (33KB, 13:50) | **Báo cáo 5 tổ vá + kiểm chéo đầy đủ** (mọi điểm ghép đã đối chiếu từng byte; nguồn chính của §2) |
| `wz7bt76sb.output` (5KB, 14:06) | Vòng phản biện cuối — **rỗng, cả 4 agent chết vì weekly limit** |

Transcript đầy đủ: `C:\Users\nguye\.claude\projects\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\cfb4f299-f5e4-47c6-a437-773fc54534bb.jsonl` (19,6MB — các mốc: dòng ~9857 sửa lỗi 8↔3, ~9861 áp 4 patcher, ~9869/9883 build, ~9891 rút 5 ảnh, ~9911 đặt script+menu, ~9925/9933 thay 3 nhị phân + kiểm băm, 9947 chết).
