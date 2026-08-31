# BÀN GIAO — HỆ PHI PHONG / ẤN / TRANG SỨC / MẶT NẠ (chốt 30/08 tối)

> Đọc tệp này **TRƯỚC**, rồi mới đọc `BANGIAO_PHIPHONG_AN_TRANGSUC_MATNA_2908.md` (phần port dữ liệu gốc).

---

## 0. VIỆC ĐẦU TIÊN PHẢI LÀM

**Chép `Game.exe` đang chờ.** Nó đã build sạch nhưng chưa swap được vì chủ game bật lại client.

```
D:\GAMEDEVNEW\Sources\S3Client\Release\Game.exe    f5894dc1a6a7   (16:32)
   ->  E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Game.exe
```

Điều kiện: **tắt `Game.exe`**. Không cần tắt `GameServer`. Nếu tệp báo *"Device or resource busy"* dù không tiến trình nào nạp — dùng mẹo **đổi tên tệp cũ đi rồi copy**.

Bản `Game.exe` này chứa **3 bản vá chưa ai thử**:
1. `m_Pad` chuyển tiếp thông điệp → **bỏ đồ vào ô** và **2 nút** mới hoạt động
2. Căn giữa cửa sổ ngoài → hết lệch
3. Nới bề rộng tiêu đề/thẻ/nhãn → hết cắt chữ

Chi tiết 3 lỗi này ở mục 4.

---

## 1. TRẠNG THÁI NHỊ PHÂN

| Tệp | Đang chạy | Bản build mới nhất |
|---|---|---|
| `bin\server\CoreServer.dll` | `d34862933427` | `d34862933427` ✅ khớp |
| `bin\client\CoreClient.dll` | `d53d10a6cd81` | `5b1cf96556f3` ⚠️ **lệch** |
| `bin\client\Game.exe` | `c51c7bbb7eea` | `f5894dc1a6a7` ⚠️ **lệch — phải chép** |

⚠️ `CoreClient.dll` lệch là do **phiên khác build lúc 16:42**. Cùng cây nguồn nên gồm cả vá của tôi — nhưng **PHẢI KIỂM LẠI BẰNG BYTE**, đừng tin suông (xem mục 6).

Sao lưu để lùi: `*.truoc_phiphong_2908` nằm cạnh từng tệp.

---

## 2. ĐÃ XONG VÀ ĐÃ NGHIỆM THU

- **33/33 bảng vật phẩm nạp đủ.** `bin\server\bpt_load_error.log` vẫn giữ mốc **00:19:11** của lần boot hỏng cũ; các lần boot sau **không ghi gì** = sạch.
- **Server hết sập.** Chốt NULL ở `KItemGenerator.CPP:1856` và `:2083` (đã gỡ mã xác minh có `test rax,rax`/`je` trong nhị phân).
- **Dữ liệu port**: `goldequip` 286 dòng, `platinaequip` +971 dòng (65→71 cột), `mask/mantle/signet/shipin`, `starstone.txt` 34 dòng, 9 nguyên liệu `4881..4889`.
- **Bộ test admin**: lệnh bài → **"Bộ test Phi Phong"** (`PP_Root`, 12 mục, 460/512 byte).
- **Bảng mô tả vật phẩm**: tiền tố sao, điểm chúc phúc, từng lỗ khảm, thuộc tính mỗi viên đá.
- **Bảng khảm**: 2 lớp (khung ngoài + ruột), 14 ô, 27 ảnh khung, nhãn từng ô.

---

## 3. 🔴 SÁU CÁI BẪY ĐÃ TRẢ GIÁ ĐỂ BIẾT

Đọc kỹ, đây là phần đắt nhất của phiên này.

### 3.1 `magicscript.txt`: ParticularType PHẢI bằng số dòng − 1
`Gen_MagicScript` (`KItemGenerator.CPP:1661`) làm `GetMagicScript(nParticularType)` = `GetRecord(i)` — **tra theo CHỈ SỐ**. Chèn một dòng sai chỗ là đẩy lệch **mọi** vật phẩm phía sau. Bảng có khoảng trống mã thì phải chèn dòng độn `"Vị trí trống"`.
*(`FindRecord(nType)` mới tra theo mã — đừng nhầm hai hàm.)*

### 3.2 Ô RỖNG ở cột CHUỖI giết CẢ BẢNG, và giết luôn mọi bảng phía sau
`KTabFile::GetValue` trả FALSE khi `dwLength == 0`; `GetString` trả FALSE theo; `::LoadRecord` coi đó là hỏng cả bảng; `KLibOfBPT::Init` gặp bảng hỏng là **`return FALSE` bỏ dở toàn bộ dãy còn lại, im lặng**.
**`GetInteger` thì LUÔN trả TRUE** (ô rỗng → mặc định) ⇒ **chỉ cột CHUỖI mới nguy hiểm**.
Ca thật: 9 dòng tôi thêm để trống cột 10 (`Script`) → hỏng `magicscript` (bảng 24) → 8 bảng sau không nạp → `ext_suite` rỗng → `Gen_Equipment` đọc NULL → **sập server**.
Bộ mô phỏng chạy lại được: `scratchpad/ktkm/dmp_mophong.py`.

### 3.3 Cột Linux ↔ JX1 khác nghĩa dù cùng 30 cột

| cột | LINUX | JX1 |
|---|---|---|
| 10 | 五行属性 (rỗng) | **Script** |
| 12 | 等级 | ShortKey |
| 13 | 是否叠放 | **nMaxStack** |
| 14 | 脚本名 | **PickExecute** (số) |
| 21 | 物品最大叠放值 | — |

Ánh xạ đúng: `L1..L9→J1..J9` · `L14→J10` · `L11→J11` · `L19→J12` · `L21→J13` · `J14=0` · `J15..J30` rỗng. Mẫu chuẩn: **dòng 200 "Huyền Thiết khoáng"**. Không có script thì ghi **`"0"`**, KHÔNG dùng `noscript.lua` (quy ước Linux, tệp không tồn tại bên JX1).

### 3.4 Hằng `TABFILE_*` phải là TÊN TRẦN
`KBasicPropertyTable::Load()` **tự ghép** `TABFILE_PATH` vào trước. Tôi để `TABFILE_STARSTONE` kèm cả đường dẫn → thành `\settings\item\settings\item\starstone.txt`, không bao giờ nạp được.

### 3.5 `platinaequip.txt` cột sao LỆCH 1 Ô so với `goldequip.txt`

| bảng | CanUpStar | MaxStone | MaxWish |
|---|---|---|---|
| `goldequip` (62 cột) | 60 | 61 | 62 |
| `platinaequip` (71 cột) | **69** | **70** | **71** |

Bảng platina có thêm 1 cột **không tên** ở vị trí 68 nên nhãn tiêu đề bị đẩy trái 1 ô. **Đọc theo nhãn là sai.**

### 3.6 `g_DebugLog` KHÔNG ghi ra tệp nào
`Engine/Src/KDebug.cpp:44` chỉ `SendMessage(WM_COPYDATA)` tới một cửa sổ gỡ rối; không có cửa sổ đó thì **mất hút**. Vì thế `"Can't open tab file"` của `KTabFile::Load` và mọi log `[DaTau]`/`[WLLS]` chưa bao giờ được ghi.
➡️ Tôi đã thêm chẩn đoán **ghi thẳng ra tệp** ở đúng chỗ `return FALSE` của `KLibOfBPT::Init()` → **`bin\server\bpt_load_error.log`**. Giữ lại, nó cứu cả buổi.

---

## 4. BA LỖI ĐANG CHỜ NGHIỆM THU (nằm trong `Game.exe f5894dc1a6a7`)

### 4.1 Đổi cha là ĐỨT đường thông điệp
`KWndObjectBox::DropObject` gửi `WND_N_ITEM_PICKDROP` tới **`m_pParentWnd`**. Khi tách bảng thành 2 lớp, cha của các ô đổi thành `m_Pad` (một `KWndImage` trần) → thả đồ và bấm nút đều rơi vào hư vô.
**Vá:** `class KUiMantleInlayPad : public KWndImage` với `WndProc` chuyển tiếp lên `m_pParentWnd`.

### 4.2 JX1 không đọc `PositionType`
`KWndWindow::Init` chỉ đọc `Width/Height/Left/Top/Disable/Moveable/FollowMove/FollowSize/DummyWnd`. Tệp ini gốc dùng `PositionType=1` để tự căn → JX1 bỏ qua, dùng nguyên `Left=132 Top=100` nên lệch. **Vá:** căn giữa cửa sổ ngoài bằng `SCREEN_WIDTH/SCREEN_HEIGHT`.

### 4.3 Chữ Việt dài hơn chữ Trung
Bề rộng trong ini gốc vừa cho chữ Trung: tiêu đề `112`, thẻ `64`, nhãn của tôi `48`. **Vá:** nới thành `220 / 112 / 96`, chỉ đổi bề rộng.

---

## 5. CẤU TRÚC BẢNG KHẢM (để khỏi mò lại)

**Hai lớp**, đúng khuôn `KUiCompoundItem` (ngoài) + `KUiCompoundOne` (ruột) có sẵn:

| Tệp ini | Vai trò |
|---|---|
| `Ui\Ui3\星辰石镶嵌界面.ini` | **Cửa sổ NGOÀI** 432×293 — ảnh viền `物品铸造底板.spr`, `CloseBtn`, `txtTitle`, `AddStarstoneBtn` |
| `Ui\Ui3\星辰石镶嵌.ini` | **Ruột** tại `(4,49)` — **toạ độ TƯƠNG ĐỐI với khung ngoài**, 14 `obj_*`, 14 `imgBottom_*`, 13 `imgUp_*`, `GuideList`, `CompoundBtn`, `CancleBtn`, `TextColor` |

**Vai trò ô CỐ ĐỊNH THEO CHỈ SỐ** — chốt từ mã server (`mantleupgrade_head.lua` `CheckInlayStarStone`):
```lua
local nSize = getn(tbItemIdx) - 1;                        -- ô 1..5 = Tinh Thần Thạch
if GetItemType(tbItemIdx[6]) ~= ITEM_TYPE_HJ_PIFENG ...   -- ô THỨ 6 = PHI PHONG
```
⇒ `Region.h` **0..4 = đá**, **5 = phi phong**. Bỏ nhầm ô là server trả *"Hãy đặt vào Phi Phong cần khảm"*.

Ánh xạ hiện tại (`PF_UI_INPUT` trong `UiMantleInlay.cpp`):
```c
static const int PF_UI_INPUT[6] = { 10, 11, 12, 0, 1, 13 };
```
`obj_13` ở chính giữa hoa văn = ô Phi Phong. 8 ô còn lại (`obj_2..obj_9`) là lỗ của hoa văn, **đã tắt thả đồ**, chỉ để xem.

---

## 6. CHUỖI GỐC — LẤY Ở ĐÂU

🔴 **`game_y.exe` của client VLTK KHÔNG có chuỗi tiếng Việt nào** (kể cả "Hành trang"). Tôi đã kết luận nhầm là "bị đóng gói" và bỏ cuộc — chủ game bắt làm lại, **chuỗi có thật**.

Chúng nằm trong **`data\slistcache.pak` mục UID `59A637DE`** — bảng key/value (`key\tvalue\r\n`, ~63 KB, nén UCL NRV2B).

```
G_STR_XING                      %d sao %s
G_STR_COLOR_XING                <color=Green>%d sao %s<color>\n
G_STR_COLOR_EMPTY_XING          <color=Green>%d sao Lỗ khảm trống<color>\n
G_STR_MANTLESYSTEM_BLESS_VALUE  <color=HBlue>Đột phá điểm chúc phúc %d/%d<color>
MSG_MANTLE_IMPLICIT_EXPIREDTIME <color=White>Kỹ năng có hiệu quả đến: %.2d:%.2d %.2d-%.2d-%d<color>\n
G_ITEM_27                       Chưa khảm nạm
```

Bộ công cụ: `scratchpad/ktkm/pp_quet_vltk.py` (quét cả thư mục, giải nén pak, thử 5 bảng mã) + `pp_trich_muc.py <pak> <uid_hex>`.
Giải nén: `ReverseTools\pak_vltk\ucl.py` → `nrv2b_decompress_8(raw, size)`. Băm tên: `name2id` duyệt `char` **CÓ DẤU**.

⚠️ Bảng chuỗi của **JX1 không có** 4 khoá trên, và **mã nguồn JX1 không tra bảng lúc chạy** — quy ước là viết TCVN3 thẳng vào mã kèm chú thích khoá (xem `KTongJX2.cpp:2638`). Tôi làm theo đúng lối đó.

🔴 Bẫy khi dò khoá: `G_STR_XING` là **chuỗi con** của `G_STR_XINGXIALING` → tìm bằng `in` cho dương tính giả. Phải tách theo `\t` rồi so **khoá đầy đủ**. *(Tôi đã báo nhầm "JX1 có sẵn G_STR_XING" vì lỗi này.)*

---

## 7. TỆP ĐÃ SỬA

**Mã nguồn** (`D:\GAMEDEVNEW\Sources\`)

| Tệp | Nội dung |
|---|---|
| `Core/Src/KItem.cpp` | `PF_StarPrefix()`, `PF_AppendDesc()`, tên đá trong `PF_GetStoneInfo`, nối vào **cả hai** bản `GetDesc` |
| `Core/Src/KItem.h` | khai báo 2 hàm trên |
| `Core/Src/KItemGenerator.CPP` | tách hằng cột `PF_GCOL_*`/`PF_PCOL_*`, **2 chốt NULL** |
| `Core/Src/KBasPropTbl.CPP` | `starstone.txt` tên trần, chẩn đoán `bpt_load_error.log` |
| `Core/Src/KBasPropTbl.h` | `GetTabFileName()` |
| `S3Client/Ui/UiCase/UiMantleInlay.{h,cpp}` | bảng khảm 2 lớp, 14 ô, 27 khung, nhãn, `UpdateData/UpdateItem` |
| `S3Client/Ui/GameSpaceChangedNotify.cpp` | nối bảng khảm vào thông báo `UOC_AFFAIR_ITEM` |

**Dữ liệu / script** (`E:\...\TESTLOFFF_ONLINE\bin\`) — mọi tệp đều có sao lưu `.truoc_*`

`settings/item/`: `magicscript.txt` (2 phía) · `platinaequip.txt` (2 phía) · `starstone.txt` · `goldequip.txt` · `mask/mantle/signet/shipin.txt` · `questkey.txt` (client)
`server/script/`: `item/test_phiphong_admin.lua` (mới) · `item/lenhbaiadmin.lua` · `global/mantlesystem/*` · `event/equip_publish/wuxingyin/wuxingyin.lua` · `global/npcchucnang/thoren.lua`

---

## 8. VIỆC CÒN LẠI

1. **Chép `Game.exe f5894dc1a6a7`** rồi nghiệm thu 3 lỗi mục 4.
2. **Kiểm lại `CoreClient.dll` đang chạy** có đủ 4 chuỗi bảng mô tả không (phiên khác vừa build đè) — nếu thiếu thì build lại `Core Client Release|Win32` rồi chép.
3. **Nâng cấp Hoàng Kim Ấn chưa gắn vào NPC.** `WXY_MoBang()` đã có sẵn cuối `wuxingyin.lua`, chỉ cần thêm 1 mục menu vào `thoren.lua`. Chưa làm vì đó là thứ người chơi thấy ngay — **chờ chủ game duyệt**.
4. **9 ảnh còn thiếu** trong 971 dòng `platinaequip` mới (nhẫn/dây chuyền/đai) — art riêng của Linux, không có trong client. Chưa xử.
5. **`Bạch Hổ Ấn (Liên Đấu)`** dòng 7380 `goldequip.txt` lệch server/client — **có sẵn từ trước**, chủ game đã duyệt đồng bộ nhưng tôi chưa làm.

---

## 9. LUẬT VẬN HÀNH (đã dính, đừng dính lại)

- **CẤM tự tắt/restart `GameServer`.** Báo trước khi swap, luôn sao lưu.
- **Phiên khác đang làm cùng cây nguồn** (pet/BDH, CFGEXP). Đã va chạm 3 lần: `KPlayer.cpp` gãy Client Release, `magicscript.txt` bị sửa song song, DLL bị build đè. ⇒ **Kiểm tiến trình build trước khi build**, và **tìm dòng theo MÃ vật phẩm chứ đừng theo số dòng**.
- **`sed`/heredoc của bash nuốt backslash.** Mọi chuỗi có `\` phải qua tệp viết bằng Write. Đã dính 5 lần trong phiên này.
- **Tệp TCVN3 chỉ sửa bằng `safe_edit.py` / `vn_edit.py`**, kiểm byte cao không đổi + FFFD = 0.
- **Đừng tin build suông** — đọc byte trong nhị phân đã nằm ở cây chạy.

---

## 10. GHI NHỚ LIÊN QUAN

`jx1-phiphong-port-2908` · `jx1-crash-extsuite-null-2908` · `jx1-magicscript-3bay-3008` · `jx1-bang-chuoi-vltk-slistcache` · `jx1-say-menu-512byte` · `jx1-pak-vltk-ucl-nrv2b` · `jx1-pak-hash-char-co-dau` · `jx1-build-song-song-obj-lech`

Bộ công cụ phiên này: `C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\93867973-c89d-4811-8422-13404ccb0fa6\scratchpad\ktkm\`
(`dmp_*.py` mổ dump · `pp_*.py` port và vá · `luac/syncheck.exe` kiểm cú pháp Lua 4.0)
