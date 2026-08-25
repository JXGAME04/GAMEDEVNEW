# A7 — ĐỐI CHIẾU 5 YÊU CẦU CỦA CHỦ GAME (soát độc lập, 25/08)

> Phạm vi: chỉ ĐỌC. Không sửa `Sources`, không sửa `E:\SourceTuanLe`, không sửa script/settings.
> Mọi khẳng định dưới đây đều có `tệp:dòng`. Chỗ chưa kiểm được ghi rõ **CHƯA XÁC MINH**.
> Cây sống: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` (dưới đây viết tắt `bin\server`).
> Bản Linux: `D:\ServerLinux\server1` (gốc A) + `D:\ServerLinux\Patch` (gốc B).

---

## 0. KẾT LUẬN NGAY — yêu cầu nào XONG / CHƯA XONG

| # | Yêu cầu chủ game | Bàn giao tự chấm | **A7 chấm lại** | Lý do |
|---|---|---|---|---|
| 1 | Thay tính năng + mã nguồn **giống 100 % bản Linux** | ✅ | **❌ CHƯA XONG** | 3/3 hoạt động **không chơi được** (A7-C1), Vượt Ải **không bao giờ mở** (A7-C2); ~100 bộ ba vật phẩm chưa ánh xạ; chế độ chuangguan30 chưa port |
| 2 | Nối `cauhinh_hoatdong.lua`, **config phần thưởng** từng hoạt động + note tiếng Việt | ✅ | **❌ CHƯA XONG** | **0 khoá phần thưởng**; 9/14 khoá HD3_* là **khoá chết** (không tệp nào đọc) |
| 3 | Tính năng nào cần lên **Chỉ nam nhiệm vụ (F11)** thì thêm vào | ✅ | **❌ CHƯA XONG** | Kết luận "chỉ Sát Thủ cần F11" **ĐÚNG**, nhưng **chưa thêm** — không có mục nào trong `uitasklist.ini` / `UiTaskGuide.cpp` |
| 4 | Task hoạt động **không trùng** tính năng khác | ✅ | **🟡 GẦN XONG** | 28 id lõi sạch thật; nhưng bỏ sót **ladder 10250** (đè bảng xếp hạng Tống Kim) và **task 1550** (thật sự có va, không phải "ngữ cảnh riêng") |
| 5 | Test từng hoạt động vào **Lệnh Bài Admin**, càng đầy đủ càng tốt | ✅ | **❌ CHƯA ĐỦ** | Thiếu ~15 nút thiết yếu; 2 nút hiện có **gây hại** (nhân bản NPC / kẹt GM trong map thuyền); 1 nút phát **sai vật phẩm** |

**Tình trạng thực tế nếu swap DLL + restart ngay bây giờ: cả 3 hoạt động đều KHÔNG chơi được**, và bản vá còn **làm hỏng ngược tính năng Tín Sứ đã phát hành 21/08** (A7-C1 mục d).

---

# 1. YÊU CẦU 1 — "giống 100 % bản Linux"

## 1.1 CÁC LỖI **CHẶN** MỚI PHÁT HIỆN (không có trong bàn giao)

### 🔴 A7-C1 — **CHẶN. `AddNpc→AddNpcEx` biến MỌI NPC của 3 hoạt động thành ĐỒNG MINH của người chơi ⇒ không đánh được. Kèm theo làm HỎNG NGƯỢC Tín Sứ đã phát hành.**

Chuỗi bằng chứng:

1. `Sources\Core\Src\ScriptFuns.cpp` (khối `LuaAddNpcEx`, ~dòng 6985-6995):
```cpp
// tham so 7: phe NPC (ban goc la mot co byte rieng - xem chu thich tren)
if (nTop >= 7 && Lua_IsNumber(L, 7))
{
    int nCamp = (int)Lua_ValueToNumber(L, 7);
    if (nCamp >= 0 && nCamp < camp_num)
        Npc[nNpcIdx].SetCurrentCamp(nCamp);      // <<< ghi ĐÈ camp lấy từ npcs.txt
```
`AddNpcSet2` đã đặt `m_CurrentCamp = cCamp` từ `npcs.txt` (`KNpcSet.cpp:405`), rồi dòng trên **ghi đè**.

2. `Sources\Core\Src\KNpcSet.cpp:139-156` `GenOneRelation`:
```cpp
if (Camp1 == camp_begin || Camp2 == camp_begin)   return relation_ally;   // camp 0
...
if (Camp1 == Camp2)                                return relation_ally;
if (Kind1 == kind_normal || Kind2 == kind_normal)  return relation_enemy;
```
`camp_begin = 0`, `camp_justice = 1` (`GameDataDef.h:484-495`).

3. Ba điểm phát bệnh:

| Hoạt động | Điểm gọi (cây sống) | Tham số 7 | camp NPC | camp người chơi | Quan hệ |
|---|---|---|---|---|---|
| **(A) Sát Thủ** | `script\task\tollgate\killbosshead.lua:189` (`Tab3[i][6]`, mọi dòng bảng đều `0` — vd `killbosshead.lua:6`) | **0** | camp_begin(0) | camp_free(4) | **ALLY** |
| **(B) PLD — 30 Thuỷ tặc** | `script\missions\fengling_ferry\mission.lua:22` | **0** | camp_begin(0) | camp_justice(1) | **ALLY** |
| **(B) PLD — boss 725 / 1692** | `script\missions\fengling_ferry\fld_smalltimer.lua:37,44,51,60` | **1** | camp_justice(1) | camp_justice(1) — `fld_head.lua:127 SetCurCamp(1)` | **ALLY** |
| **(C) Vượt Ải — toàn bộ quái/boss** | `script\missions\challengeoftime\npc.lua:431-440` (bản Linux gốc, không bị vá) | **1** | camp_justice(1) | camp_justice(1) — `mission_match.lua:67 SetCurCamp(camp)` với `camp=1` từ `dragonboat_main.lua:163 JoinMission(MISSION_MATCH,1)` | **ALLY** |

⇒ **Người chơi không thể gây sát thương, NPC cũng không đánh người chơi.** Cả 3 hoạt động vô nghĩa.

4. 🔴🔴 **Hồi quy (regression) trên tính năng ĐANG CHẠY**: `b2_patch.py` thay `AddNpc→AddNpcEx` trên **toàn bộ** `killbosshead.lua` (3 chỗ), trong đó **2 chỗ thuộc hệ TÍN SỨ đã phát hành 21/08**:
   - `killbosshead.lua:3399` `add_bossnpc` — bảng `AddNpc_turesureboss` (Bảo Rương Thiên Bảo Khố)
   - `killbosshead.lua:3410` `add_messengernpc` — bảng `AddNpc_allbugbear` (tiểu quái ải)
   - Đường gọi sống: `script\task\tollgate\addtollgatenpc.lua:16,17,19`
   Trước vá: `AddNpc(...)` của JX1 lấy tham số 6 = **nSeries**, camp giữ nguyên `npcs.txt` (Camp=5 = camp_animal ⇒ ENEMY, chạy đúng). Sau vá: tham số 6 trở thành **camp** ⇒ camp 0 ⇒ **ALLY** ⇒ toàn bộ quái/boss Tín Sứ **không đánh được nữa**.
   Lưu ý `killbosshead.lua` **không nằm trong 101 tệp chép** — nó là tệp JX1 ĐANG SỐNG bị b2 vá đè (chênh +42 byte = 3×14 byte).

**Đề xuất vá (1 dòng, phía C++)** — chỉ áp camp cho script JX1 cũ, script JX2 chỉ lấy `bNoRevive`:
```cpp
if (nTop >= 7 && Lua_IsNumber(L, 7))
{
    int nCamp = (int)Lua_ValueToNumber(L, 7);
#ifdef _SERVER
    if (g_IsJx2Script(L))
    {
        if (nCamp != 0) Npc[nNpcIdx].m_bNoRevive = 1;   // đúng ngữ nghĩa Linux
    }
    else
#endif
    if (nCamp >= 0 && nCamp < camp_num)
        Npc[nNpcIdx].SetCurrentCamp(nCamp);              // giữ nguyên hành vi JX1
}
```
(3 script JX1 đang truyền `1` ở vị trí này — `bosscharm.lua:99`, `seasonnpc_item.lua:57`, `spider_web.lua:42` — vẫn giữ nguyên vì `g_IsJx2Script` = 0.)
Riêng `killbosshead.lua` còn phải **thêm tiền tố `\script\task\tollgate\` vào `g_IsJx2Script`** — đã có sẵn (`KSortScript.cpp:117`), nên vá trên phủ luôn Tín Sứ.

---

### 🔴 A7-C2 — **CHẶN. Vượt Ải KHÔNG BAO GIỜ MỞ: `DynamicExecute` trỏ vào script nằm dưới `\settings` — thư mục KHÔNG được nạp vào cây script.**

- `script\tinhnang\3hoatdong\hd3_driver.lua:23` `HD3_VA_TRIGGER = "\\settings\\trigger_challengeoftime.lua"`
- Dùng ở `hd3_driver.lua:92` (lịch mỗi giờ) và `hd3_driver.lua:123` (`HD3_Adm_VA_Now` — nút admin "Báo danh NGAY").
- `Sources\Core\Src\ScriptFuns.cpp:2415-2421` `LuaDynamicExecute`:
```cpp
KLuaScript* pScript = (KLuaScript*)g_GetScript(szLow);
if (!pScript) { g_DebugLog("[WLLS] DynamicExecute: script chua nap, bo qua: ..."); return 0; }
```
- `Sources\Core\Src\KSortScript.cpp:56,65` — `g_IniScriptEngine()` chỉ nạp **`\script`** và **`\scriptjx2\tong_vn`**. `KSortScript.cpp:59` ghi rõ: *"g_GetScript KHONG tu nap"*.
- Kiểm cây sống: **chỉ có đúng 2 tệp `.lua` nằm dưới `settings\`** (`settings\trigger_challengeoftime.lua`, `settings\trigger_include.lua`) và **không có bản sao nào dưới `script\`**.

⇒ mỗi giờ, `HD3_Tick` ghi 1 dòng log rồi **không làm gì**. Nút admin cũng vậy. **Hoạt động (C) chết 100 %.**

**Cách sửa**: chép/di chuyển `trigger_challengeoftime.lua` + `trigger_include.lua` sang `script\missions\challengeoftime\` (nhớ sửa `Include` trong `trigger_challengeoftime.lua:5`), rồi đổi `HD3_VA_TRIGGER`. (Đối chiếu: các đợt port trước đều đặt driver dưới `\script` — `startgame.lua:107-111`.)

---

### 🔴 A7-C3 — **CHẶN cửa vào PLD giờ tốn phí: `6,1,2745` ở JX1 là "Thùng gỗ", không phải "Lệnh Bài Thủy Tặc".**

- Linux `Patch\settings\item\004\magicscript.txt` dòng 3355: `6,1,2745` = **"Lệnh Bài Thủy Tặc"**
- JX1 `bin\server\settings\item\magicscript.txt`: `6,1,2745` = **"Thùng gỗ"**; "Lệnh Bài Thủy Tặc" thật nằm ở **`6,1,3363`**
- `remap_resolved.json` có `2742→3360`, `2743→3361`, `2744→3362` nhưng **THIẾU `2745→3363`**
- Điểm gãy: `script\missions\fengling_ferry\fld_head.lua:274` `if (itemgenre ~= 6 or particular ~= 2745)`, và `:280,:290,:295`
- `script\item\hd3_admin.lua:53` cũng phát nhầm: `AddItem(6, 1, 2745, 0, 0, 0)` → phát "Thùng gỗ"

⇒ Người chơi cầm Lệnh Bài Thủy Tặc thật (3363) **không lên thuyền được** trong 5 khung giờ 10/14/16/18/20 h.

---

### 🔴 A7-C4 — **CHẶN cửa vào PLD bằng 200 Mật đồ: `6,1,196` ở JX1 là "Thưởng Thiện lệnh".**

- Linux `6,1,196` = **"Mật đồ thần bí"**; JX1 `6,1,196` = **"Thưởng Thiện lệnh"**, "Mật đồ thần bí" thật ở **`6,1,195`**
- `script\missions\fengling_ferry\fld_head.lua:197` `if (itemgenre == 6 and detailtype == 1 and parttype ==196)`
- Không có trong `remap_resolved.json`

⇒ Đường vào phụ "200 quyển Mật đồ thần bí" (rao ở `fldmap_boat1.lua:22`) **không dùng được**.

---

## 1.2 BẢNG "LỆCH SO VỚI BẢN LINUX" — đánh giá từng chỗ

| # | Chỗ lệch | Bắt buộc? | Đánh giá A7 |
|---|---|---|---|
| L1 | `AddNpc → AddNpcEx` + chèn `random(0,4)` (14 chỗ) | **Bắt buộc** về chữ ký | Ý tưởng đúng, **thi hành SAI** ⇒ A7-C1. Phải sửa phía C++, không phải phía script |
| L2 | Ánh xạ 42 ID vật phẩm (106 chỗ) | Bắt buộc | **Làm mới ~30 %.** Còn **≈100 bộ ba** chưa xử lý — xem 1.3 |
| L3 | Bỏ `Include` `\script\global\路人_礼官.lua` (`boss.lua:11`) | Bắt buộc (tệp không tồn tại ở mọi gốc) | **Đúng.** Mà `boss.lua` là **mã chết** (`thuytacdaulinh()` bị comment hết) nên vô hại |
| L4 | Vị trí 6 thuyền phu + 7 NPC 769 lấy từ toạ độ JX1 | Bắt buộc (Linux không có bảng vị trí rời) | **Chấp nhận được**, đã ghi chú. Nhưng toạ độ NPC 769 trong `autoexec_npc_hd3.lua:8-14` **lấy từ bản Linux** (`autoexec_npc.lua`) chứ không phải JX1 ⇒ **CHƯA XÁC MINH** 7 điểm này có nằm trên ô đi được của map JX1 không. Cần test |
| L5 | Toạ độ 160 boss sát thủ (`killbosshead.lua:6-179`) | — | **CHƯA XÁC MINH** — chép nguyên từ Linux, chưa ai kiểm ô hợp lệ trên map JX1. Đây chính là lý do menu admin **bắt buộc** phải có nút "dịch chuyển tới boss" (Linux có sẵn: `gmcommand_3.lua:468 KillBoss_Transfer`) |
| L6 | Các hàm C++ stub trả giá trị khác (`GetItemQuality`, `GetGlodEqIndex`, `GetPlatinaLevel`…) | Bắt buộc (JX1 không có hệ phẩm chất vàng/bạch kim) | **Đúng và trung thực** |
| L7 | `HD3_PLD_GIO` = 12 giờ chẵn | **KHÔNG bắt buộc** | Linux relay chạy **mỗi giờ** phút :00. Đây là lệch **có chủ đích nhưng chưa ghi rõ là lệch** trong bàn giao. Nên ghi chú |
| L8 | 52/137 tệp bao đóng **không chép** vì JX1 đã có | Bắt buộc | **Kiểm lại: an toàn.** 48/52 **giống hệt byte**; 4 tệp khác: `item\heart_head.lua` (JX1 nhỏ hơn 4953 byte — chỉ được `Include`, không hàm nào bị gọi ⇒ vô hại), `misc\eventsys\eventsys.lua` (+1016), `task\newtask\newtask_head.lua` (+1083, vẫn có `KILLER_MAXCOUNT=8` và `DescLink_NieShiChen`), `killbosshead.lua` (+42 = chính bản vá b2) |
| L9 | Bảng rơi đồ của quái/boss PLD | **Không thể port** | JX1 `settings\npcs.txt` **không có cột `DropRateFile`** (87 cột). ⇒ Thuỷ tặc/boss PLD sẽ rơi theo bảng rơi riêng của JX1, **không giống Linux**. Nên ghi vào bàn giao là "KHÔNG THỂ PORT" |

## 1.3 ÁNH XẠ VẬT PHẨM — bảng 42 mục **chưa đủ**, còn ≈100 bộ ba sai

Đã quét toàn bộ 105 tệp HD3, đối chiếu **tên** vật phẩm giữa `D:\ServerLinux\Patch\settings\item\004\magicscript.txt` và `bin\server\settings\item\magicscript.txt`:

**Nhóm A — vẫn giữ số Linux nhưng JX1 là vật phẩm KHÁC (≈60 bộ ba)**, trọng yếu:

| Bộ ba | Tên bản Linux | JX1 đang là | Đúng phải là | Điểm gãy |
|---|---|---|---|---|
| `6,1,196` | Mật đồ thần bí | Thưởng Thiện lệnh | `6,1,195` | `fld_head.lua:197` (**A7-C4**) |
| `6,1,2745` | Lệnh Bài Thủy Tặc | Thùng gỗ | `6,1,3363` | `fld_head.lua:274,280,290,295`; `hd3_admin.lua:53` (**A7-C3**) |
| `6,1,124` | Phúc Duyên Lộ (Đại) | **Quế Hoa Tửu** | `6,1,123` | `challengeoftime\award.lua:15`; `npc.lua:266` |
| `6,1,123` | Phúc Duyên Lộ (Trung) | Phúc Duyên Lộ (Đại) | `6,1,122` | `award.lua:16`; `npc.lua:267` |
| `6,1,122` | Phúc Duyên Lộ (Tiểu) | Phúc Duyên Lộ (Trung) | `6,1,121` | `award.lua:17`; `npc.lua:268` |
| `6,1,2113` | Thần Nông Chân Đơn | Hồng bao Sum vầy | `6,1,2122` | `event\change_destiny\head.lua:19` (chuỗi Đổi Mệnh của cả Sát Thủ + PLD) |
| `6,1,2114` | Nghịch thiên cải mệnh quyết | Hồng bao An khang | `6,1,2123` | `change_destiny\head.lua:20` |
| `6,1,2070` | Hùng Tâm Kiếm | Thiên Tứ Bảo Rương | `6,1,2079` | `event\birthday_jieri\200905\chuangguan\chuangguan.lua:59` |
| `6,1,1604 / 1605 / 1617` | Anh Hùng Thiếp / Hình nhân / Viêm Đế Lệnh | Thiệp chúc bang chủ / Thiệp chúc sư đệ / Viêm Đế gia bào hoàn | `1613 / 1614 / 1626` | `missions\yandibaozang\head.lua:52,118,53` |
| `6,1,115…124`, `218…235`, `885…889`, `910`, `1074`, `1083`, `1266`, `1389`, `1390`, `1448`, `2318…2322`, `2374…2377`, `2005` | (lệch 1 bậc) | — | (số JX1 cùng tên đã liệt kê trong bảng đầy đủ) | `vng_feature\forbiditem\vngforbidspecialitem.lua`, `vng_event\thapnienlenhbai\lenhbai_def.lua`, `event\jiefang_jieri\...\head.lua` |

**Nhóm B — Linux có, JX1 KHÔNG CÓ (40 bộ ba, đều ≥ 30000; bảng JX1 tối đa particular = 4864)**, trọng yếu:

| Bộ ba | Tên | Điểm gãy | Ứng viên JX1 cùng tên |
|---|---|---|---|
| `6,1,30557` Túi Dược Phẩm | **bảng thưởng chính Sát Thủ cấp 90** | `kill_level.lua:104` (nRate 68.26 %) | `6,1,4813` |
| `6,1,30528/30530/30531/30532/30533/30534/30535/30536/30538` Đồ Phổ Đằng Long… | **bảng thưởng Sát Thủ cấp 90** | `kill_level.lua:107-115` | phần lớn **không có** |
| `6,1,30228` Chân Nguyên Đơn (trung) | thưởng đăng ký PLD giờ tốn phí | `fld_head.lua:123` | `6,1,4846` |
| `6,1,30009/30010/30533/30538/30557` | thưởng Mật Phòng Vượt Ải | `chuangguang30.lua:39-52` | một phần |
| `6,1,30191` Chìa khoá vàng, `6,1,30289` Huyết Long Đằng, `6,1,30301` Hỗn nguyên chân đơn… | Bảo Rương Vượt Ải | `challengeoftime\item\chuangguanbaoxiang.lua:19,59-74` | `2953 / 3051,3946,4848 / 4857` |

> Chú ý: `item_can_them.csv` của phiên trước đã **gợi ý** một số ánh xạ (`ANH XA -> (6,1,4846)`, `(6,1,4813)`, `(6,1,4857)`…) nhưng **không mục nào được đưa vào `remap_resolved.json`** ⇒ chưa áp.
> `id_dungdo.csv` chỉ soát **97 vật phẩm**, còn 105 tệp HD3 dùng **172 bộ ba** ⇒ soát cũ **thiếu ~44 %**.
>
> Cũng lưu ý: 5 dòng `THIEU` kiểu `4,489,1 / 4,353,1 / 4,238…240,1` trong `item_can_them.csv` là **báo động giả** — JX1 `settings\item\questkey.txt` có đủ (`4,489` = "Lệnh bài Phong Lăng Độ", `4,353` = "Tinh Hồng Bảo Thạch"…).

## 1.4 NPC — 3 mẫu bị JX1 chiếm chỗ (chưa xử lý)

| NPC id | Bản Linux | JX1 đang là | Ảnh hưởng |
|---|---|---|---|
| **1032** | (cao cấp) tiểu Boss nam 7 | **Boss New Dragon 165** | `include.lua:111 tbRangeId[2] = {{1026,1033},{1034,1037}}` — dải "hoá thân người chơi" cấp cao |
| **1033** | (cao cấp) tiểu Boss nam 8 | **Boss New Dragon 166 8** | như trên |
| **1034** | (cao cấp) tiểu Boss nữ 1 | **Boss New Dragon 167** | như trên |

⇒ Vượt Ải **cao cấp**: 3/12 NPC "hoá thân người chơi" sẽ ra nhầm Boss New Dragon. Có trong `npc_can_them.csv` (`KHAC_TEN`) nhưng **không được xử lý**. 137/140 mẫu NPC còn lại **trùng tên, an toàn**.

## 1.5 CÁC MẢNG CỦA BẢN LINUX — ĐÃ PORT / CHƯA PORT / KHÔNG THỂ PORT

### (A) SĂN BOSS SÁT THỦ — đối chiếu `01_satthu.md` §2

| Bước luồng chơi | Trạng thái |
|---|---|
| Sinh 7 NPC 769 (7 thành) — `01_satthu.md:63` | ✅ **ĐÃ PORT** (`hd3_driver.lua:38-43`, `autoexec_npc_hd3.lua`) |
| Sinh 160 NPC boss trên 40 map — `:62` | ✅ **ĐÃ PORT** (`hd3_driver.lua:44-46`) — nhưng bị A7-C1 |
| Nhận nhiệm vụ cấp 90 (`killer90` + `showboss/showbossnext`) — `:100-110` | ✅ **ĐÃ PORT** |
| Nhóm cấp 20-80 bị comment ở Linux | ✅ **GIỮ ĐÚNG NGUYÊN BẢN** |
| Giết boss → `kill_level.lua OnDeath` → `SetMemberTask` cho cả tổ đội | ✅ **ĐÃ PORT** — nhưng **KHÔNG BAO GIỜ CHẠY** vì A7-C1 |
| Phát Sát Thủ lệnh theo ngũ hành boss (`NPCINFO_GetSeries`) | ✅ có alias engine; ngũ hành nay do `random(0,4)` sinh ⇒ đúng ý Linux |
| Bảng thưởng cố định cấp 90 (`tbAward`, 10 000 000 exp) — `:207-210` | 🟡 **PORT NHƯNG HỎNG** — 10/11 dòng vật phẩm là `6,1,30xxx` **không tồn tại ở JX1** (1.3 nhóm B) |
| Hợp thành 5 Sát Thủ lệnh → 1 Sát Thủ Giản — `:223-241` | ✅ **ĐÃ PORT**, số đã ánh xạ đúng (`399→398`, `400→399`, cả dạng `parttype == 399`) |
| Mở "Sát Thủ Bí Bảo" (cần 6 Huyền Thiên Chuỳ) — `:243-256` | ✅ **ĐÃ PORT** (`2347→2356`, `2348→2357` đúng) |
| `Ladder_NewLadder(10119)` bảng xếp hạng tích luỹ | ✅ **ĐÃ PORT**, id sạch |
| Điểm năng động `huoyuedu` "shashourenwu" (2 lần/ngày, +3) | ✅ **ĐÃ PORT** (`huoyuedu.txt` + `huoyuedu.lua` đã chép) |
| `activitysys` `FinishKillerBoss` + `EventSys KillerBoss` | ✅ **ĐÃ PORT** |
| Nhiệm vụ Đổi Mệnh `completeMission_Killer` (cần 4 lần) | 🟡 **PORT NHƯNG SAI ĐỒ** — `change_destiny\head.lua:19,20` dùng `2113/2114` (1.3) |
| `achievementsys\type\killertask.lua` — thành tựu | ❌ **CHƯA PORT** — không có trong 101 tệp, `closure3.json` không phủ |
| Lệnh GM Linux `KillBoss_Transfer` (dịch chuyển tới boss) — `01_satthu.md:263` | ❌ **CHƯA PORT** — xem yêu cầu 5 |
| **Chỉ nam F11** (`taskui_killer.lua` + `taskguide.txt` dòng 9) | ❌ **CHƯA PORT** — xem yêu cầu 3 |
| `jiefangri_award()` (sự kiện 2011) | ✅ **ĐÃ PORT**, code chết ở cả bản gốc — đúng |

### (B) PHONG LĂNG ĐỘ — đối chiếu `02_phonglangdo.md` §1

| Bước | Trạng thái |
|---|---|
| 6 NPC thuyền phu bờ Nam, mỗi người 1 BOATID | ✅ **ĐÃ PORT** (`hd3_driver.lua:55-67`, `hd3_thuyenphu.lua`) — lỗi `GetNpcValue()` **đã được vá đúng** thành `GetNpcValue(NpcIndex)` (`hd3_thuyenphu.lua:9`); `SetNpcValue/GetNpcValue` = `m_nNpcParam[0]`, không đụng `NpcParam[1]` của boss sát thủ ✔ |
| Mở 3 thuyền 337/338/339, mission 15 — `fenglingdu_main` | ✅ **ĐÃ PORT** (`hd3_driver.lua:86`) — đường `\script\...` **hợp lệ**, khác Vượt Ải |
| Cửa báo danh 10 phút, sức chứa 100/thuyền | ✅ **ĐÃ PORT** (hằng số cứng trong `fld_head.lua:108,153`) |
| Vé vào: Lệnh bài PLD `4,489` | ✅ **ĐÃ PORT**, id đúng |
| Vé vào: 200 Mật đồ thần bí | ❌ **HỎNG** — A7-C4 |
| Vé vào giờ tốn phí: Lệnh Bài Thủy Tặc | ❌ **HỎNG** — A7-C3 |
| Thưởng đăng ký giờ tốn phí (2 Chân Nguyên Đơn trung, task 3070 ≤5/ngày) | 🟡 **PORT NHƯNG HỎNG ĐỒ** — `6,1,30228` không có ở JX1 |
| Sinh 30 Thuỷ tặc + 3 đợt boss + 2 đại đầu lĩnh giờ tốn phí | ✅ **ĐÃ PORT** — nhưng bị A7-C1 |
| Bảng toạ độ sinh quái `渡船刷怪点.txt` (63 điểm) | ✅ **ĐÃ PORT** (gốc B) |
| Rơi đồ boss (2 Thí Giả Chi Ấn `1094→1095`, Truy công lệnh `2015→2024`, 0,5 % Hải long châu `2115→2124`) | ✅ ánh xạ **đúng** |
| Cập bến `Landing()` → map 336 + 2 Bảo Rương Thuỷ Tặc (`2743→3361`) | ✅ ánh xạ **đúng** |
| `DisabledUseTownP` thật (vá stub `KJx2WarInfra.cpp:258`) | ✅ **ĐÃ PORT** — cải thiện thật so với trước |
| Điểm năng động PLD (task 2863, bỏ qua thuyền 3) | ✅ **ĐÃ PORT** |
| NPC đối thoại ngày "Tiểu Chiêu" (`talkdailytask.lua`) | ✅ **ĐÃ PORT** |
| activitysys `FinishFengLingDu` / `config\1004` (đếm tuần 3105) / `config\23` (2817) | 🟡 **MỘT PHẦN** — `config\32` và `config\41` đã chép; `config\23` và `config\1004` **KHÔNG có trong manifest** ⇒ 2 mốc thưởng ngày/tuần của PLD **CHƯA PORT** |
| Bảng rơi `npcdroprate_boatthief.ini` / `boatboss_droprate.ini` | ❌ **KHÔNG THỂ PORT** — `npcs.txt` JX1 không có cột `DropRateFile` (1.2 L9) |
| **Chỉ nam F11** | ✅ **KHÔNG CẦN** — bản Linux `Patch\settings\task\taskguide.txt` không có dòng nào cho PLD (đã kiểm) |

### (C) VƯỢT ẢI — đối chiếu `03_vuotai.md` §1-2

| Bước | Trạng thái |
|---|---|
| Trigger mỗi giờ mở mission 22 trên 32 map | ❌ **CHẾT HOÀN TOÀN** — A7-C2 |
| Điểm vào NPC Nhiếp Thí Trần (`want_playboat`) | ✅ **ĐÃ PORT** — `nieshichen.lua:5,11,19` Include đủ `dragonboat_main` / `rank_perday` / `npcNhiepThiTran`; `DescLink_NieShiChen` có từ `newtask_head.lua:13` ✔ |
| Kiểm điều kiện tổ đội, trừ 2 Sát Thủ Giản | ✅ **ĐÃ PORT** (`dragonboat_main.lua:4-81,126-143`) |
| 28 ải + 7 bảng đội hình `lineup*.txt` | ✅ **ĐÃ PORT** — 7/7 tệp khớp `npc.lua:12-18` |
| 32 map 464-495 | ✅ **CÓ ĐỦ** trong `Maps\WorldSet_GameServer.ini` (kiểm trực tiếp) |
| Thưởng từng ải / hoàn thành / ải ẩn (`award.lua`) | 🟡 **PORT NHƯNG SAI ĐỒ** — Phúc Duyên Lộ lệch 1 bậc (1.3) |
| Bảo Rương Vượt Ải (`chuangguanbaoxiang.lua`) | 🟡 **PORT NHƯNG THIẾU ĐỒ** — nhiều `6,1,30xxx` |
| Bảng xếp hạng ngày `Ladder 10235` + `rank_perday` (Thiên Niên Linh Dược `6,1,2125`) | ✅ **ĐÃ PORT**, id `2125` **đúng tên** ở JX1 |
| Móc `storm_addpoint(2, …)` (`award.lua:174`) | ✅ **CHẠY ĐƯỢC** — `npc_death.lua:7,10` Include cả `award.lua` lẫn `event\storm\function.lua` trong cùng state ✔ |
| **Chế độ `chuangguan30` (Mật Phòng, map 957)** | ❌ **CHƯA PORT / KHÔNG CHẠY**: `Maps\WorldSet_GameServer.ini` **KHÔNG có map 957** (đã kiểm; `03_vuotai.md:349` đã cảnh báo, thi công **bỏ qua**). Kèm theo **thiếu bảng** `settings\maps\liandandong\npc_3.txt` (`chuangguang30.lua OnAddBoss`). Hệ quả: NPC tiếp dẫn `npc\transfer.lua` sẽ `NewWorld(957,…)` → `KNpc::ChangeWorld` trả 0, người chơi **đứng im**, không có thông báo |
| `bigboss.lua` (BigBoss Vượt Ải) | ✅ đã chép — nhưng bảng thưởng `bigboss.lua:66-110` đầy `6,1,30xxx` không tồn tại |
| **Chỉ nam F11** | ✅ **KHÔNG CẦN** — xem yêu cầu 3 |

### Mảng CHUNG chưa port

| Mảng | Trạng thái |
|---|---|
| `achievementsys` (thành tựu Sát Thủ) | ❌ **CHƯA PORT** |
| `activitysys\config\23` + `config\1004` (mốc ngày/tuần PLD) | ❌ **CHƯA PORT** |
| `uiactivityguide` "Hướng dẫn hoạt động" (client) — Linux có mục cho **cả 3** hoạt động | ❌ **KHÔNG THỂ PORT** hiện tại: JX1 client **không có** thư mục `uiactivityguide` (kiểm `bin\client\Ui\Ui3\`) |
| Bảng rơi đồ theo `DropRateFile` | ❌ **KHÔNG THỂ PORT** |

---

# 2. YÊU CẦU 2 — CONFIG `cauhinh_hoatdong.lua`

## 2.1 🔴 **9 / 14 khoá HD3_* là KHOÁ CHẾT** (NẶNG)

Quét toàn cây sống (`grep -rn 'HD_CFG("HD3_'`) — **chỉ 5 khoá được đọc**:

| Khoá | Ai đọc | Có tác dụng thật? |
|---|---|---|
| `HD3_PLD_GIO` | `hd3_driver.lua:85` | ✅ có |
| `HD3_VA_GIO` | `hd3_driver.lua:91` | ✅ có (nhưng nhánh chết vì A7-C2) |
| `HD3_VA_GIO_XEPHANG` | `hd3_driver.lua:98` | ✅ có |
| `HD3_ST_CAP_TOITHIEU` | `hd3_admin.lua:18` | ❌ **chỉ để in chữ** trên menu admin |
| `HD3_ST_MAX_NGAY` | `hd3_admin.lua:18` | ❌ **chỉ để in chữ** |

**9 khoá không tệp nào đọc** — đổi số **không có tác dụng gì**:

| Khoá chết | Giá trị thật bị cứng ở đâu |
|---|---|
| `HD3_ST_CAP_TOITHIEU` (hiệu lực) | `nieshichen.lua:123` `killerCoundTakedTask(90, 350)` |
| `HD3_ST_MAX_NGAY` (hiệu lực) | `script\task\newtask\newtask_head.lua:20` `KILLER_MAXCOUNT = 8` |
| `HD3_PLD_CAP_TOITHIEU` | không có chỗ nào kiểm cấp khi lên thuyền (Linux chỉ kiểm môn phái, `fld_head.lua:40-43`) — **khoá bịa** |
| `HD3_PLD_SUC_CHUA` | `fld_head.lua:153` `GetMSPlayerCount(MISSIONID, 1) >= 100` |
| `HD3_PLD_GIO_TONPHI` | `fld_head.lua:305-311` `tb_sptime = {[10]=1,[14]=1,[16]=1,[18]=1,[20]=1}` |
| `HD3_VA_CAP_TOITHIEU` | `challengeoftime\include.lua:85-86` `tbLevels` |
| `HD3_VA_PHUT_BAODANH` | `include.lua:12-13` `TIME_SIGNUP = 10` |
| `HD3_VA_PHUT_NHIEMVU` | `include.lua:14` `LIMIT_FINISH = 30*60` |
| `HD3_VA_NGUOI_TOIDA` | `include.lua:17` `LIMIT_PLAYER_COUNT = 8` |

> Bàn giao mục 4 ghi *"mọi khoá driver/admin dùng đều có trong config (không có khoá chết)"* — đúng theo **một chiều**, nhưng chiều ngược lại (config → script) **9/14 khoá vô dụng**, và điều đó nguy hiểm hơn: chủ game sửa `HD3_PLD_GIO_TONPHI` rồi tưởng đã đổi giờ tốn phí.

## 2.2 🔴 **KHÔNG CÓ KHOÁ PHẦN THƯỞNG NÀO** — yêu cầu chính chưa làm (NẶNG)

Chủ game nói rõ *"config hoạt động **phần thưởng** mỗi hoạt động"*. Khối `[6]` (dòng 255-303) **không có một khoá thưởng nào**. Bảng thưởng hiện nằm rải ở:

| Hoạt động | Bảng thưởng nằm ở | Sửa được từ config? |
|---|---|---|
| **(A) Sát Thủ** | `lib_killlevel.lua:70-108` (exp 15 000…140 000 theo cấp + Sát Thủ lệnh + 50 % Bí Bảo)<br>`kill_level.lua:88-121` `tbAward` (25 dòng random + **10 000 000 exp**)<br>`mibao_head.lua:10-21` (11 dòng hộp Bí Bảo)<br>`settings\droprate\boss\bosstask_lev90.ini` | ❌ KHÔNG |
| **(B) PLD** | `bossdeath.lua:6,22,31,38` (2 Thí Giả Chi Ấn, Truy công lệnh, 0,5 % Hải long châu)<br>`shuizeideath.lua:5-22` (49 % Truy công lệnh)<br>`mission.lua:120` (2 Bảo Rương Thuỷ Tặc)<br>`fld_head.lua:121-125` (2 Chân Nguyên Đơn trung) | ❌ KHÔNG |
| **(C) Vượt Ải** | `award.lua:7-45` `map_random_awards` (30+ dòng), `:58-115` `tbAward_Success`/`tbAward_batch`, `:121-131` ải ẩn<br>`chuangguang30.lua:37-54` `tbVnItemAwardEx`<br>`item\chuangguanbaoxiang.lua:19-74`<br>`rank_perday.lua:13` (Thiên Niên Linh Dược) | ❌ KHÔNG |

**Đề xuất tối thiểu để đóng yêu cầu 2** — thêm vào `tbCHD` các khoá dưới đây, và **sửa 9 khoá chết thành khoá SỐNG** bằng cách để script Linux đọc `HD_CFG(...)` thay vì hằng số cứng (chỉ 9 dòng):

```
-- (A) Sát Thủ
HD3_ST_EXP_C90        = 140000,       -- exp gốc mỗi boss cấp 90 (lib_killlevel.lua:104)
HD3_ST_EXP_HOANTHANH  = 10000000,     -- exp cố định khi hoàn thành (kill_level.lua tbAward[2])
HD3_ST_TYLE_BIBAO     = 50,           -- % rơi Sát Thủ Bí Bảo
HD3_ST_SO_LENH        = 1,            -- số Sát Thủ lệnh mỗi lần
-- (B) PLD
HD3_PLD_SO_AN_BOSS    = 2,            -- Thí Giả Chi Ấn / boss  (bossdeath.lua:6 SIGNET_DROPCOUNT)
HD3_PLD_TYLE_HAILONG  = 5,            -- 0,5 % Hải long châu (đơn vị 1/10 %)
HD3_PLD_SO_RUONG_BEN  = 2,            -- Bảo Rương Thuỷ Tặc khi cập bến
HD3_PLD_TYLE_TRUYCONG = 49,           -- % Truy công lệnh từ Thuỷ tặc
-- (C) Vượt Ải
HD3_VA_EXP_HESO       = 100,          -- % nhân exp thưởng mỗi ải
HD3_VA_SO_RUONG_15_28 = 2,            -- rương ải 15 / ải 28 (chế độ 2011)
HD3_VA_EXP_NIESHICHEN = 10000000,     -- exp khi hạ Tiểu Nhiếp Thi Trần
```
Mỗi khoá cần **1 dòng ghi chú tiếng Việt** + nhãn `[LIVE]/[RESTART]` như phần còn lại của tệp.

## 2.3 🔴 Nhãn `[LIVE]` **SAI** cho 3 khoá lịch (NẶNG)

`HD3_PLD_GIO`, `HD3_VA_GIO`, `HD3_VA_GIO_XEPHANG` được đọc trong **state của `timerserver.lua`** (`timerserver.lua:32` Include `hd3_driver.lua` lúc nạp tệp, `:74` gọi `HD3_Tick`).
Nút "Nạp lại CONFIG" gọi `HD_NapLaiCauHinh()` (`cauhinh_hoatdong.lua:338-341`) = `Include(...)` — **chỉ nạp lại vào state của Lệnh Bài Admin**, không tới `timerserver`.
⇒ 3 khoá này thực chất là **[RESTART]**, nhãn hiện tại đánh lừa người vận hành.

## 2.4 NHẸ — ghi chú config chưa khớp thực tế

- `cauhinh_hoatdong.lua:262` ghi *"engine tự sinh lúc boot (hd3_driver **HD3_Boot**)"* — hàm thật tên `HD3_DriverInit` (`hd3_driver.lua:36`).
- `cauhinh_hoatdong.lua:302` ghi *"Chế độ chuangguan30 (map 957)"* như thể đang chạy — thực tế **map 957 chưa nạp** (1.5 C).
- `HD3_VA_GIO_XEPHANG` chú thích "HHMM = 0 -> 00:00" nhưng so sánh là `nHHMM == giá trị` (`hd3_driver.lua:98`) ⇒ đặt `3` sẽ thành **00:03** chứ không phải 03:00. Nên ghi rõ "phải điền đủ 4 số, vd 0300".

---

# 3. YÊU CẦU 3 — CHỈ NAM NHIỆM VỤ (F11)

## 3.1 Tự kiểm trên bản Linux (không dựa vào kết luận cũ)

`D:\ServerLinux\Patch\settings\task\taskguide.txt` — **17 dòng**, liên quan:

| Dòng | ID | Tên | Script UI |
|---|---|---|---|
| 9 | **8** | **Nhiệm vụ Sát thủ** | `\UI\taskui_killer.lua` → `showkillertaskdesc` |
| 10 | 9 | Thách thức thời gian | `\UI\taskui_messenger.lua` → `showmessengerdesc` |

- `Patch\ui\taskui_killer.lua:8` đọc `GetTask(1082)` + `settings\task\tollgate\killer\killer.txt` ⇒ **đây đúng là hệ Săn Boss Sát Thủ**.
- Dòng 10 "Thách thức thời gian" **KHÔNG phải Vượt Ải**: `Patch\ui\taskui_messenger.lua:21` chú thích gốc `--闯关任务之信使任务` và đọc `GetTask(1201..1204)` ⇒ đó là **TÍN SỨ Thiên Bảo Khố** (đã port 21/08), chỉ trùng tên tiếng Việt.
- Grep toàn `Patch\ui\` + `taskguide.txt` cho `1550/1551/2852/1505/3070/2863/fengling/challengeoftime`: **0 kết quả**.

**⇒ Kết luận của phiên chính ĐÚNG: chỉ Sát Thủ cần F11; PLD và Vượt Ải không có mục F11 ở bản Linux.**

## 3.2 🔴 Nhưng phát hiện thêm: bản Linux có **UI thứ hai** mà bàn giao không nhắc

`Patch\ui\ui3_1024\uiactivityguide\activityinfo.ini` — "Hướng dẫn hoạt động", 20 mục, có **cả 3** hoạt động:

| Mục | ActivityId | Tên | IniSection |
|---|---|---|---|
| `[2]` | 3 | Thời gian khiêu chiến | `shijiandetiaozhan` (= Vượt Ải) |
| `[3]` | 2 | Độ Thuyền | `fenglingdu` (= PLD) |
| `[7]` | 11 | Nhiệm vụ Sát thủ | `shashourenwu` |

JX1 client **không có** thư mục `uiactivityguide` (kiểm `bin\client\Ui\Ui3\` — chỉ có `uitaskguide`).
⇒ Phân loại: **KHÔNG THỂ PORT** trong đợt này, nhưng **phải ghi vào bàn giao** để chủ game biết bản Linux có gì mà ta chưa có.

## 3.3 🔴 F11 cho Sát Thủ: **CHƯA LÀM** ⇒ yêu cầu 3 **CHƯA XONG** (NẶNG)

Bàn giao mục 6 chấm ✅ "đã phân tích", nhưng chủ game yêu cầu *"thì **thêm vào**"*, không phải "phân tích rồi thôi".

Việc còn lại (đã có khuôn sẵn từ đợt Tín Sứ/Bang Chiến/Bách Nhân):
1. `Sources\S3Client\Ui\UiCase\UiTaskGuide.cpp:29-32` — thêm `#define TASKGUIDE_SATTHU_TASKID 10`.
2. Thêm `void BuildSatThuText();` (`UiTaskGuide.h:55`) theo khuôn `BuildTinSuText()` (`UiTaskGuide.cpp:513`); nội dung lấy từ `Patch\ui\taskui_killer.lua:8-27` (task 1082 = 0 → danh sách 7 thành; > 0 → `killer.txt` cột `BossName` + `BossInfo` dòng `1082+1`).
3. Rẽ nhánh ở `UiTaskGuide.cpp:379-393` (chọn mục) và `:170-188` (`OnTaskValueChanged`, lọc `nTaskId == 1082 || 1192 || 1193 || 1217`).
4. Thêm một section vào `\UI\uitasklist.ini` (pak client) với `Name=` + `TaskId=10` — bảng mục F11 nạp từ đây (`UiTaskGuide.cpp:318-339`).
5. Build lại `Game.exe` + `CoreClient.dll`.

Điều kiện kỹ thuật đã sẵn: `nieshichen.lua` ghi task qua `nt_setTask` → `SetTask + SyncTaskValue` (`newtask_head.lua:31-34`) nên client **có** giá trị 1082/1192/1193/1217.
⚠️ **CHƯA XÁC MINH**: ảnh thoại `\spr\npcres\enemy\enemy154\enemy154_pst.spr` (`newtask_head.lua:13`) có trong pak client JX1 hay không — nếu thiếu thì hộp thoại Nhiếp Thí Trần mất ảnh (NHẸ).

---

# 4. YÊU CẦU 4 — KIỂM TOÁN TASK ID ĐỘC LẬP

Phương pháp khác `audit_ids.py`: (a) **tự trích** id từ chính 105 tệp đã chép (regex trên `Get/SetTask`, `*TaskDaily`, `Get/SetTaskTemp`, `Ladder_*`, `Get/SetGlbValue`) + (b) quét **hằng số tên kiểu TSK_/TASK_/LADDER_/MISSION_** rồi (c) đối chiếu ngược với **toàn bộ** `script\`, `scriptjx2\`, `settings\` **trừ** 105 tệp đó.
Script: `…\scratchpad\a7\idscan.py`, `idscan2.py`, `idscan3.py`.

## 4.1 Kết quả: 28 id lõi mà phiên chính nêu — **XÁC NHẬN SẠCH**

`2622 2623 2624 2626 2627 2628 2636 2637 2638 2639 2641 2642 4018 2852 1505 2863 2871 2880 2882 2817 3105 1764 1765 3079 10119 10179 10180 10235` → **0 điểm đụng** ở phần còn lại của cây JX1.
`mission 15` và `mission 22`: `settings\task\missions.txt:16,23` đã trỏ đúng, và **không script JX1 nào khác** đặt `MISSIONID/MISSION_* = 15|22`.

## 4.2 🔴 Bỏ sót 1 — **Ladder 10250 bị chép đè** (NẶNG)

| Bên | Điểm |
|---|---|
| Tệp mới chép | `script\battles\battlehead.lua:1091` `Ladder_ClearLadder(10250)`; `:1097` `Ladder_NewLadder(10250, …)` |
| JX1 đang dùng | `script\startgame\tinhnang\tongkim\songjin_shophead.lua:190` `Ladder_GetLadderInfo(10250, i)` (**bảng xếp hạng Tống Kim**) và bản sao `script\global\…\songjin_shophead.lua:190` |

Hiện `battlehead.lua` **chưa ai `Include`** ⇒ chưa nổ. Nhưng nó là **quả mìn**: chỉ cần một tệp nào đó Include về sau, `Ladder_ClearLadder(10250)` sẽ **xoá sạch bảng xếp hạng Tống Kim**.
Bàn giao mục 7 chỉ audit 4 ladder (10119/10179/10180/10235) — **bỏ sót 10250**.

## 4.3 🔴 Bỏ sót 2 — **task 1550: kết luận cũ SAI** (NẶNG)

Bàn giao mục 7 viết: *"task 1550 có ở `event\storm\function.lua` nhưng đó là hệ khác dùng cùng số theo ngữ cảnh riêng — Vượt Ải đọc/ghi trong **mission-scope riêng**, không giao thoa"*.
**Sai**: `GetTask/SetTask` là **biến nhiệm vụ của NHÂN VẬT**, không có mission-scope.

| Bên | Điểm |
|---|---|
| Vượt Ải | `challengeoftime\include.lua:20` `TSK_REMAIN_COUNT = 1550`; ghi ở `dragonboat_main.lua:148,151`; đọc ở `npcNhiepThiTran.lua:67,71` |
| Hệ khác | `script\event\storm\function.lua:396` và `:417` `SetTask(1550,0)` (chú thích gốc `杀手进行次数`) |

Rủi ro **THẤP** trên thực tế: `storm_clear()` là hàm GM, và **không tệp nào Include `event\storm\head.lua` để chạy tự động** (đã grep). Nhưng phải ghi đúng bản chất — nếu GM chạy `storm_clear`, người chơi bị **reset lượt Vượt Ải trong ngày**.

## 4.4 Các id khác đã soát và **kết luận an toàn**

| ID | Bên HD3 | Bên JX1 | Kết luận |
|---|---|---|---|
| `1122` | `battles\weeklyrank.lua:144 SetTask(1122,…)`; `activitysys\config\41\extend.lua:193,197` | `event\tongwar\head.lua`, `leaguematch\npc\officer.lua`, `test\bangthanh_f.lua` | **CÙNG NGỮ NGHĨA** (id danh hiệu đang bật) ⇒ tương thích tình cờ. `weeklyrank.lua` là mã chết |
| `751` | `task\metempsychosis\translife_5.lua:97 GetTask(751)` (chỉ ĐỌC) | điểm tích luỹ Tống Kim | **CÙNG NGỮ NGHĨA**, chỉ đọc ⇒ an toàn |
| `4000` | `vng_event\thapnienlenhbai\lenhbai_def.lua:380 SetTask(4000, …)` (đạt tới qua `award.lua:4 → mainfuc.lua:5`) | `scriptjx2\lib\awardtype\zhenyuanpoint.lua:6 TASK_ID = 4000` (điểm Chân Nguyên) | 🟡 **CÙNG SỐ, CÙNG Ý** (cộng điểm Chân Nguyên) — nhưng chỉ chạy khi dùng item Thập Niên Lệnh Bài. **CHƯA XÁC MINH** hai bên có cùng thang điểm |
| `5100` | `global\thanh\npc\npc_chuyensinh.lua:23,37,45` | không tệp JX1 nào dùng | An toàn (và tệp này **không ai Include**) |
| `88` | `task\metempsychosis\task_func.lua:84 GetTask(88)` | `global\skills_table.lua` dùng `GetTask(1..10)` cho môn phái — **không phải 88** | An toàn |
| `200` (TaskTemp) | `fld_head.lua:117`, `mission_match.lua:68`, `chuangguang30.lua` | 14 tệp JX1 (bw, citywar, tongwar…) | **CÙNG NGỮ NGHĨA** (`player_tmp_task_def.txt:90` = cho phép đổi phe) ⇒ đúng như bản gốc, an toàn |
| `1..13` trong `change_destiny\head.lua:56-69` | — | — | **BÁO ĐỘNG GIẢ**: đó là **chỉ số trong `nPlayerTask = {2622,2623,2624,2626,2627,2628}`** (`head.lua:54`) + bảng bit `tbTask` (`:71-86`), không phải task id |

## 4.5 Loại id chưa ai soát: `gb_task` (biến toàn cục chuỗi)

`rank_perday.lua:11` `nTIMERANK = "challengeoftime_ranklist"` → `gb_SetTask/gb_GetTask` khe 1-4.
Đã grep: **không tệp JX1 nào dùng khoá chuỗi này** ⇒ an toàn. (Ghi lại để lần sau còn nhớ đây cũng là một không gian tên cần soát.)

---

# 5. YÊU CẦU 5 — LỆNH BÀI ADMIN

Đường vào: `script\item\lenhbaiadmin.lua:38` Include `hd3_admin.lua`, `:107` mục menu → `HD3_AdminMenu` (`hd3_admin.lua:8`). Cơ chế `dofile` lại mỗi lần dùng ⇒ sửa không cần restart ✔.

## 5.1 🔴 3 nút hiện có bị LỖI

| Nút | Điểm | Vấn đề | Mức |
|---|---|---|---|
| "Sinh lại NPC 769 + 160 boss (boot)" | `hd3_admin.lua:27` `HD3_DriverInit()` | **Không dọn NPC cũ.** Mỗi lần bấm sinh thêm 160 boss + 7 NPC + 6 thuyền phu ⇒ **nhân bản NPC**, bấm 3 lần là 480 boss. Cần `ClearMapNpc` hoặc cờ chống gọi lại | **NẶNG** |
| "Dịch chuyển: vào map thuyền 337" | `hd3_admin.lua:51` `NewWorld(337, 1646, 3233)` | Vào thẳng map thuyền **không qua `AddMSPlayer`** ⇒ không thuộc mission 15 ⇒ `Landing()` không đưa ra; map 337 lại bị cấm vật phẩm dịch chuyển (`header\forbidmap.lua`) ⇒ **GM kẹt trong thuyền**. Phải kèm nút "Thoát map thuyền" | **NẶNG** |
| "Nhận Lệnh Bài Thủy Tặc (6,1,2745)" | `hd3_admin.lua:53` | Phát **"Thùng gỗ"** — A7-C3 | **NẶNG** |
| "Báo danh NGAY (mở mission)" | `hd3_admin.lua:63` → `hd3_driver.lua:123` | **Không làm gì** — A7-C2 | **CHẶN** |

## 5.2 NHẸ

- `hd3_admin.lua:20,28,60`: nhãn ghi *"Ba Lăng Huyện"* / *"thành Ba Lăng"* nhưng `NewWorld(1, 1506, 3198)` — map **1** trong `Maps\WorldSet_GameServer.ini` là **`Phượng Tường`**.
- `hd3_admin.lua:68`: `Msg2Player("Đã nạp lại CONFIG (script\header\cauhinh_hoatdong.lua).")` — Lua 4 nuốt `\h` và `\c` ⇒ in ra `scriptheadercauhinh_hoatdong.lua`. Phải viết `\\`.
- `hd3_admin.lua:30`: `AddItem(6,1,398, 90, 0, 0)` — 5 lệnh đều ngũ hành **0 (Kim)**; hợp thành sẽ luôn ra Sát Thủ Giản hệ Kim, không test được nhánh `givesword` random ngũ hành (`nieshichen.lua:201-206`).
- "Nạp lại CONFIG" (`hd3_admin.lua:66-70`) **không** áp cho lịch (2.3) — nhãn cần sửa.

## 5.3 CÁC NÚT CÒN THIẾU (đối chiếu luồng chơi thật)

### (A) Săn Boss Sát Thủ
1. **Dịch chuyển tới boss đang nhận** — quan trọng nhất, vì chưa ai xác minh 160 toạ độ Linux có hợp lệ trên map JX1 (1.2 L5). Bản Linux **có sẵn** hàm mẫu: `gmcommand_3.lua:468 tbCommand3:KillBoss_Transfer(nLevel, nChoice)`.
2. **Ép nhận nhiệm vụ boss số N** (`nt_setTask(1082, N)`) — bỏ qua hộp thoại 2 trang.
3. **Xoá nhiệm vụ đang có** (`SetTask(1082,0)`) — hiện chỉ reset 1193.
4. **Đặt/xem task 1217** (tổng tích luỹ) để test mốc bảng xếp hạng 10119.
5. **Nhận 1 Sát Thủ Bí Bảo + 6 Huyền Thiên Chuỳ** (`6,1,2356` + `6,1,2357`×6) để test `shashou_mibao.lua`.
6. **Nhận 5 Sát Thủ lệnh NGŨ HÀNH NGẪU NHIÊN** (thay nút hiện tại hoặc thêm).
7. **Xem bảng xếp hạng 10119** (`Ladder_GetLadderInfo`).

### (B) Phong Lăng Độ
8. **Ép thuyền rời bến NGAY** (`RunMission(15)` — sinh 30 Thuỷ tặc, không phải đợi 10 phút).
9. **Ép sinh boss 725 / 1692** (test A7-C1 và bảng rơi).
10. **Ép cập bến NGAY** (`CloseMission(15)` → `Landing()`).
11. **Xem trạng thái mission**: `GetMissionV(MS_TIMEACC_1MIN)`, `GetMissionV(MS_TIMEACC_20SEC)`, `GetMSPlayerCount(15,1)` cho từng thuyền 337/338/339.
12. **Nhận 200 Mật đồ thần bí** (test đường vào phụ).
13. **Ép "giờ tốn phí" bật/tắt** (biến tạm ghi đè `check_new_shuizeitask`).
14. **Thoát khỏi map thuyền** (bù cho nút 5.1).

### (C) Vượt Ải
15. **Mở mission trực tiếp** không qua `\settings\...` (bù A7-C2): `DynamicExecute` vào một tệp dưới `\script`.
16. **Bỏ qua điều kiện tổ đội** — hiện muốn test phải gom đủ người thật (`COT_CheckTeamRequirement`, `dragonboat_main.lua:4-81`).
17. **Ép sang ải kế tiếp / giết sạch quái ải hiện tại** (`create_batch_npc` / `batch_finish`).
18. **Xem trạng thái mission**: `VARV_STATE`, `VARV_NPC_BATCH`, `VARV_NPC_COUNT`, `VARV_BOARD_TIMER`, `VARV_BATCH_MODEL`, `VARS_TEAM_NAME`.
19. **Ép chế độ chuangguan30** + **dịch chuyển vào map 957** (sau khi thêm 957 vào WorldSet).
20. **Kết thúc sớm / huỷ trận** (`CloseMission(22)`).
21. **Nhận 2 Sát Thủ Giản** một lần (điều kiện vào là 2, nút hiện chỉ cho 1).
22. **Đặt task 2636-2639** (thành tích 2 ngày gần nhất) để test `rank_award` mà không phải chờ sang ngày.

### Chung
23. **Xem toàn bộ khoá HD3_* đang hiệu lực** (in `tbCHD` ra `Msg2Player`) — nhất là sau khi 9 khoá chết được nối vào thật.

---

# 6. PHỤ LỤC — DANH SÁCH LỖI THEO MỨC ĐỘ

## CHẶN (không chạy được / sập / mất dữ liệu)

| Mã | Mô tả | Tệp:dòng chính |
|---|---|---|
| A7-C1 | `AddNpcEx` tham số 7 ghi đè camp ⇒ **mọi NPC 3 hoạt động thành đồng minh, không đánh được**; kèm **hồi quy Tín Sứ** | `ScriptFuns.cpp` `LuaAddNpcEx` (~6989); `KNpcSet.cpp:139-156`; `killbosshead.lua:189,3399,3410`; `mission.lua:22`; `fld_smalltimer.lua:37,44,51,60`; `challengeoftime\npc.lua:431-440` |
| A7-C2 | Vượt Ải không bao giờ mở — `DynamicExecute` vào script dưới `\settings` (không được nạp) | `hd3_driver.lua:23,92,123`; `KSortScript.cpp:56,65`; `ScriptFuns.cpp:2415-2421` |
| A7-C3 | Vé PLD giờ tốn phí sai vật phẩm (`6,1,2745` = Thùng gỗ; đúng là `6,1,3363`) | `fld_head.lua:274,280,290,295`; `hd3_admin.lua:53`; `remap_resolved.json` |
| A7-C4 | Đường vào "200 Mật đồ thần bí" sai vật phẩm (`6,1,196` = Thưởng Thiện lệnh; đúng là `6,1,195`) | `fld_head.lua:197` |

## NẶNG (sai hành vi)

| Mã | Mô tả | Tệp:dòng |
|---|---|---|
| A7-N1 | ~100 bộ ba vật phẩm chưa ánh xạ (60 sai tên + 40 không tồn tại) — mất/nhầm phần thưởng cả 3 hoạt động | xem §1.3 |
| A7-N2 | Map **957** chưa có trong WorldSet ⇒ chế độ chuangguan30 chết; thiếu `settings\maps\liandandong\npc_3.txt` | `Maps\WorldSet_GameServer.ini`; `chuangguang30.lua`; `npc\transfer.lua:36-56` |
| A7-N3 | 9/14 khoá `HD3_*` là khoá chết | `cauhinh_hoatdong.lua:255-303` |
| A7-N4 | Không có khoá phần thưởng nào ⇒ yêu cầu 2 chưa làm | `cauhinh_hoatdong.lua` khối `[6]` |
| A7-N5 | Nhãn `[LIVE]` sai cho 3 khoá lịch | `cauhinh_hoatdong.lua:271,288,296`; `HD_NapLaiCauHinh` `:338` |
| A7-N6 | NPC 1032/1033/1034 ở JX1 là "Boss New Dragon" ⇒ Vượt Ải cao cấp ra nhầm 3/12 NPC | `settings\npcs.txt`; `include.lua:111` |
| A7-N7 | Nút admin "Sinh lại NPC" nhân bản NPC | `hd3_admin.lua:27` |
| A7-N8 | Nút admin "vào map thuyền 337" làm GM kẹt | `hd3_admin.lua:51` |
| A7-N9 | F11 Sát Thủ chưa thêm ⇒ yêu cầu 3 chưa xong | `UiTaskGuide.cpp:29-32,379-393`; `\UI\uitasklist.ini` |
| A7-N10 | Ladder 10250 trong `battlehead.lua` đè bảng xếp hạng Tống Kim (mã chết — mìn) | `battlehead.lua:1091,1097` vs `songjin_shophead.lua:190` |
| A7-N11 | Task 1550 thật sự va với `storm_clear()` (kết luận cũ sai bản chất) | `include.lua:20` vs `event\storm\function.lua:396,417` |
| A7-N12 | `activitysys\config\23` + `config\1004` (mốc ngày/tuần PLD) chưa port | không có trong `b1_manifest.txt` |
| A7-N13 | `achievementsys\type\killertask.lua` (thành tựu Sát Thủ) chưa port | không có trong `b1_manifest.txt` |

## NHẸ

| Mã | Mô tả | Tệp:dòng |
|---|---|---|
| A7-L1 | Nhãn "Ba Lăng Huyện" sai — map 1 = Phượng Tường | `hd3_admin.lua:20,28,60` |
| A7-L2 | `\h` `\c` bị Lua 4 nuốt trong chuỗi thông báo | `hd3_admin.lua:68` |
| A7-L3 | `HD3_PLD_GIO` 12 giờ chẵn ≠ Linux (mỗi giờ) — lệch có chủ đích nhưng chưa ghi | `cauhinh_hoatdong.lua:272` |
| A7-L4 | `IncludeLib("NPCINFO")` không có trong bảng 21 module ⇒ ghi log "module lạ" mỗi lần nạp | `kill_level.lua:9`; `ScriptFuns.cpp:2484-2503` |
| A7-L5 | 5 Sát Thủ lệnh admin đều hệ Kim | `hd3_admin.lua:30` |
| A7-L6 | Ghi chú config sai tên hàm `HD3_Boot` (thật là `HD3_DriverInit`) | `cauhinh_hoatdong.lua:262` |
| A7-L7 | `HD3_VA_GIO_XEPHANG` so sánh HHMM nhưng chú thích chỉ nói "0 → 00:00" | `hd3_driver.lua:98`; `cauhinh_hoatdong.lua:296` |
| A7-L8 | Rác chép vào: `script\battles\*` (8 tệp), `vng_event\thapnienlenhbai\*`, `global\thanh\npc\*` (6 tệp) — **không tệp nào Include**; `add_npc.lua:204,205` gọi `add_dialognpc` / `add_newtasknpc` **không tồn tại** ⇒ nếu ai gọi sẽ lỗi nil | `b1_manifest.txt` mục 22-27, 67-75 |
| A7-L9 | `startgame.lua:23` ghép 2 dòng chú thích của 2 đợt port | `startgame.lua:23` |

## CHƯA XÁC MINH (cần test thật)

1. 160 toạ độ boss sát thủ (`killbosshead.lua:6-179`) và 7 toạ độ NPC 769 (`autoexec_npc_hd3.lua:8-14`) có nằm trên ô đi được của map JX1 không.
2. Ảnh thoại `\spr\npcres\enemy\enemy154\enemy154_pst.spr` có trong pak client JX1 không.
3. Thang điểm task 4000 (Chân Nguyên) giữa `lenhbai_def.lua:380` và `zhenyuanpoint.lua` có cùng đơn vị không.
4. Bảng rơi thật của quái/boss PLD sau khi bỏ `DropRateFile` — sẽ rơi gì.

---

## 7. THỨ TỰ SỬA ĐỀ XUẤT (để 3 hoạt động chạy được)

1. **A7-C1** — vá `LuaAddNpcEx` (1 khối `if`, phía C++). *Không sửa được bằng script.* Đây là điều kiện cần cho cả 3 hoạt động **và** để trả lại Tín Sứ.
2. **A7-C2** — chuyển 2 tệp `settings\trigger_*.lua` sang `script\missions\challengeoftime\`, sửa `hd3_driver.lua:23` + `Include` bên trong.
3. **A7-C3 + A7-C4** — thêm `6,1,2745→3363` và `6,1,196→195` vào `remap_resolved.json`, chạy lại `b2_patch.py`, sửa `hd3_admin.lua:53`.
4. **A7-N1** — mở rộng bảng ánh xạ theo §1.3 (dùng cột "ứng viên JX1 cùng tên" đã tính sẵn), chạy lại b2.
5. **A7-N2** — thêm `WorldNNN=957` + tăng `Count` trong `Maps\WorldSet_GameServer.ini`, chép `settings\maps\liandandong\npc_3.txt`.
6. **Yêu cầu 2** — nối 9 khoá chết + thêm ~11 khoá phần thưởng (§2.2), sửa nhãn `[LIVE]`→`[RESTART]`.
7. **Yêu cầu 5** — bổ sung 23 nút (§5.3) + sửa 3 nút lỗi (§5.1).
8. **Yêu cầu 3** — làm F11 Sát Thủ (§3.3).
9. **A7-N6** — quyết định xử lý NPC 1032/1033/1034 (đổi `tbRangeId` hoặc thêm 3 mẫu NPC mới vào `npcs.txt`).

---

## ĐỐI CHẤT (tác tử độc lập)

> Vòng đối chất 25/08 — người kiểm chứng KHÔNG phải người viết A7, cũng không phải phiên thi công.
> Phương pháp: mặc định coi mọi phát hiện của A7 là SAI cho tới khi tệp gốc / bảng dữ liệu / mã C chứng minh ngược lại.
> Đã kiểm **24 phát hiện**. Kết quả: **17 ĐÚNG · 4 SAI · 3 THỔI PHỒNG hoặc HẠ THẤP**. Cộng thêm **5 chỗ chính A7 bỏ sót**.
> Bằng chứng được dựng lại từ đầu: đọc `magicscript.txt` theo **cột** `ItemGenre/DetailType/ParticularType` (không theo số dòng),
> đọc `npcs.txt` theo cột `Kind/Camp`, đọc thẳng `GenOneRelation` thay vì tin bảng tóm tắt.

### Bảng đối chất

| # | Phát hiện của A7 | Bằng chứng gốc (tự dựng lại) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | **A7-C1 cơ chế**: `AddNpcEx` tham số 7 ghi đè camp nên NPC thành đồng minh | `ScriptFuns.cpp:6985-6989` gọi `SetCurrentCamp(nCamp)` vô điều kiện; `KNpcSet.cpp:1706-1712` `GetRelation` tra bảng theo `m_CurrentCamp` **lúc chạy**; `KNpcSet.cpp:143` `Camp1==camp_begin` hoặc `Camp2==camp_begin` cho `relation_ally`; `KNpc.cpp:146` người chơi `m_CurrentCamp = camp_free(4)`; `npcs.txt` các mẫu 761/770/771/772/724/725/1692/849 đều `Kind=0 Camp=5` | **ĐÚNG — CHẶN** | giữ nguyên mức CHẶN |
| 2 | **A7-C1 phạm vi Sát Thủ**: 160 boss camp 0 nên thành ally | `killbosshead.lua:189` `AddNpcEx(...,Tab3[i][6],...)`; quét bảng `addkillertasknpc` (dòng 4-179): **160/160 hàng sống có cột 6 = 0** | **ĐÚNG** | — |
| 3 | **A7-C1 hồi quy Tín Sứ** qua `killbosshead.lua:3399` (`add_bossnpc`) và `:3410` (`add_messengernpc`) | `addtollgatenpc.lua:16,17,19` gọi `add_killertasknpc(AddNpc_turesureboss)`, `add_killertasknpc(AddNpc_turesurebug)`, `add_messengernpc(AddNpc_allbugbear)`; dòng 18 `add_bossnpc(AddNpc_flyboss)` **đã bị comment**. Bảng `AddNpc_allbugbear` (dòng 365-2606) có **0 hàng sống** nên `:3410` vô hại. Hai bảng thật (`turesureboss` 9 hàng, `turesurebug` 9 hàng, cột 6 = 0) chạy qua **`:189`** | **SAI (gán nhầm đường)** — hồi quy vẫn CÓ THẬT nhưng qua dòng **189** | "hồi quy Tín Sứ qua `killbosshead.lua:189`; `:3399` là mã chết (call site bị comment), `:3410` vô hại (bảng rỗng)" |
| 4 | **A7-C1 hậu quả Tín Sứ**: "toàn bộ quái/boss Tín Sứ không đánh được" | `npcs.txt` NPC **844 "Bảo rương" = Kind 3 (kind_dialoger), Camp 6**; `GenOneRelation` câu đầu tiên: `Kind==kind_dialoger` trả `relation_dialog` **trước mọi luật camp**, nên 9 Bảo rương KHÔNG bị ảnh hưởng. Chỉ **NPC 849 "Thiên Bảo Hộ thủ 90" (Kind 0, Camp 5)** — 9 con — bị đổi thành ally | **THỔI PHỒNG** | "9 Hộ Thủ Giả (NPC 849) của Tín Sứ thành đồng minh; 9 Bảo rương (NPC 844, kind_dialoger) không đổi" |
| 5 | **A7-C1 cách sửa đề xuất**: bọc `if (g_IsJx2Script(L))` quanh `SetCurrentCamp` | `LuaIncludeFile` (`ScriptFuns.cpp:2021`) dùng **`lua_dofile(L, ...)` — CÙNG state của tệp GỌI**; `g_IsJx2Script` tra tên qua `g_GetScriptNameByState` (`KSortScript.cpp:113-140`). `HD3_DriverInit` được `startgame.lua:23` Include và `:103` gọi, nên state là **`script\startgame.lua`**, KHÔNG nằm trong danh sách 21 tiền tố JX2. Tín Sứ y hệt (`startgame.lua:18` Include `tinsu_addnpc.lua`) | **SAI — bản vá đề xuất KHÔNG chữa được đúng 2 ca A7 coi là quan trọng nhất** | Vá theo **điểm gọi**, không theo state. (a) rẻ và an toàn nhất: **trả 14 chỗ về `AddNpc` gốc** (xem #6); (b) hoặc thêm tham số thứ 10 `bNoRevive` cho `AddNpcEx`; (c) hoặc bỏ hẳn nhánh `SetCurrentCamp` trong `AddNpcEx` và để 3 script JX1 (`bosscharm.lua:99`, `seasonnpc_item.lua:57`, `spider_web.lua:42`) gọi `SetNpcCurCamp` riêng |
| 6 | **A7 mục L1**: "`AddNpc` sang `AddNpcEx` là **bắt buộc** về chữ ký; ý tưởng đúng, thi hành sai" | `LuaAddNpc` (`ScriptFuns.cpp:6877-6907`) nhận `(nId, nLevel, **nSubWorldIdx**, nX, nY, **nSeries**, szName)`. **Toàn bộ 36 điểm gọi `AddNpc` của bản Linux trong 3 hoạt động** (`ReverseTools\port_3hd\src_utf8\...`) đều là `AddNpc(id, level, SubWorld, x, y, P6, name, flag)` — **CÙNG thứ tự với JX1**. `battlehead.lua:437` (bản Linux) đặt tên biến P6 là **`l_removedeath`**, và `challengeoftime\npc.lua:426` chú thích gốc cho tham số 7 là `不重生` (không hồi sinh). Vậy P6/P7 = **bNoRevive**, không phải series, cũng không phải camp | **SAI — việc đổi sang `AddNpcEx` là KHÔNG CẦN THIẾT và chính nó đẻ ra A7-C1** | "L1 là lệch **tự tạo**: bản Linux của 3 hoạt động dùng `AddNpc` **cùng thứ tự tham số với JX1**, chỉ khác nghĩa cột 6 (Linux = removedeath, JX1 = series). Bỏ 14 lần đổi và `random(0,4)` là hết A7-C1, đổi lại chấp nhận ngũ hành NPC = 0 hoặc 1" |
| 7 | **A7-C2**: Vượt Ải không bao giờ mở vì trigger nằm dưới `settings\` | `KSortScript.cpp:56,65` chỉ nạp `\script` và `\scriptjx2\tong_vn`; `LuaDynamicExecute` (`ScriptFuns.cpp:2415-2420`) `g_GetScript(szLow)` trả NULL rồi `return 0`; cây sống chỉ có `settings\trigger_challengeoftime.lua` và `settings\trigger_include.lua`, không có bản sao dưới `script\`; `hd3_driver.lua:23,92,123` đúng như mô tả | **ĐÚNG — CHẶN** | giữ nguyên |
| 8 | **A7-C3**: `6,1,2745` ở JX1 là "Thùng gỗ", đúng phải là `6,1,3363`; kèm chú "Linux dòng 3355" | Tra theo **cột**: Linux `(6,1,2745)` = **"Lệnh Bài Thủy Tặc"**; JX1 `(6,1,2745)` = **"Thùng gỗ"**; JX1 "Lệnh Bài Thủy Tặc" = `(6,1,3363)`. `fld_head.lua:274,280,290,295` và `hd3_admin.lua:53` đúng như nêu. **Số dòng 3355 sai** (hàng của 2745 nằm ở dòng 3352 bản Linux) | **ĐÚNG** (chỉ chú thích số dòng sai) | bỏ "dòng 3355", ghi "tra theo cột ParticularType" |
| 9 | **A7-C4**: `6,1,196` ở JX1 là "Thưởng Thiện lệnh", đúng là `195` | Linux `(6,1,196)` = "Mật đồ thần bí"; JX1 `(6,1,196)` = "Thưởng Thiện lệnh"; JX1 "Mật đồ thần bí" = `(6,1,195)`; `fld_head.lua:197` `parttype ==196` | **ĐÚNG — CHẶN** | giữ nguyên |
| 10 | **A7-N1 nhóm B**: "40 bộ ba `6,1,30xxx` không tồn tại ở JX1" | Tự quét 101 tệp manifest, 193 bộ ba phân biệt: **39 bộ ba `6,1,30xxx`** hoàn toàn vắng trong `magicscript.txt` của JX1 (30006/30008/30009/30010/30011/30126/30191/30215/30216/30218/30227/30228/30229/30246/30289/30301/30350/30386/30408/30446/30449/30474/30505/30506/30507/30528-30538/30557/30563/30593). `30289 Huyết Long Đằng` dùng **9 chỗ**, `30557 Túi Dược Phẩm` 4 chỗ | **ĐÚNG** | sửa "40" thành "39" |
| 11 | **A7-N1 nhóm A**: Phúc Duyên Lộ lệch 1 bậc; 2113/2114 sang 2122/2123 | Linux `(6,1,122/123/124)` = Tiểu/Trung/Đại; JX1 `(6,1,121/122/123)` = Tiểu/Trung/Đại, còn JX1 `124` = "Quế Hoa Tửu". Linux `2113/2114` = Thần Nông Chân Đơn / Nghịch thiên cải mệnh quyết; JX1 tương ứng `2122/2123` | **ĐÚNG** | — |
| 12 | **A7 §1.5(C)**: `rank_perday` dùng `2125` và "id 2125 đúng tên ở JX1" | Bản Linux gốc `src_utf8\vuotai\missions\challengeoftime\rank_perday.lua:13` dùng **2116**; bản đã port dùng **2125**; JX1 `(6,1,2125)` = "Thiên Niên Linh Dược" — đúng vật phẩm của Linux | **ĐÚNG** (ánh xạ 2116 sang 2125 hợp lệ) | — |
| 13 | **A7-N2**: map **957** chưa có trong `WorldSet_GameServer.ini`; thiếu `settings\maps\liandandong\npc_3.txt` | `Maps\WorldSet_GameServer.ini` có `Count=910`, tìm `=957` cho **0 kết quả**; thư mục `settings\maps\liandandong\` **không tồn tại**; `challengeoftime\npc\transfer.lua:56` gọi `NewWorld(957,...)` | **ĐÚNG** | — |
| 14 | **A7-N3**: 9/14 khoá `HD3_*` là khoá chết | Tìm `HD_CFG("HD3_` toàn cây sống cho đúng **4 điểm gọi / 5 khoá**: `HD3_PLD_GIO`, `HD3_VA_GIO`, `HD3_VA_GIO_XEPHANG` (`hd3_driver.lua:85,91,98`) và `HD3_ST_CAP_TOITHIEU` + `HD3_ST_MAX_NGAY` (`hd3_admin.lua:18`, chỉ để in chữ) | **ĐÚNG** | — |
| 15 | **A7-N6**: NPC 1032/1033/1034 ở JX1 là "Boss New Dragon 165/166/167" | `settings\npcs.txt` hàng 1032 = "Boss New Dragon 165", 1033 = "Boss New Dragon 166 8", 1034 = "Boss New Dragon 167"; "(cao cấp) tiểu Boss nữ" thật nằm ở **1035**; `challengeoftime\include.lua:111` `tbRangeId[2] = {{1026,1033},{1034,1037}}` | **ĐÚNG** | — |
| 16 | **A7-N10**: "Hiện `battlehead.lua` **chưa ai Include**" nên ladder 10250 là mìn chưa nổ | Tìm `Include` tới `battles\battlehead.lua` cho **5 điểm gọi SỐNG**: `battles\battleinfo.lua:1`, `startgame\tinhnang\tongkim\songjin_shophead.lua:2` (**shop Tống Kim**), bản sao trong `global\...\songjin_shophead.lua:2`, và **2 tệp HD3 vừa chép**: `missions\challengeoftime\npc.lua:5` cùng `missions\fengling_ferry\boss.lua:2`, cộng 2 trap Công Thành Chiến | **SAI về lý do** — kết luận "chưa nổ" vẫn đúng nhưng do lý do khác | "`Ladder_ClearLadder(10250)` nằm trong `bt_sortbthonour()` (`battlehead.lua:1028`), hàm này có **0 điểm gọi** trong toàn cây. Mìn nằm ở chỗ đó, KHÔNG phải ở việc chưa ai Include" |
| 17 | **A7-N11**: task 1550 va với `event\storm\function.lua`, xếp **NẶNG** | `challengeoftime\include.lua:20` `TSK_REMAIN_COUNT=1550` so với `event\storm\function.lua:396,417` `SetTask(1550,0)`; cả hai câu đều nằm trong `storm_clear(gameid)` (`:392`) và hàm này có **0 điểm gọi** trong toàn cây | **THỔI PHỒNG** (thân bài A7 tự nhận "rủi ro THẤP" nhưng phụ lục lại xếp NẶNG) | hạ xuống **NHẸ / mìn**, cùng loại với A7-N10 |
| 18 | **A7-N8**: nút "vào map thuyền 337" làm GM kẹt, một phần vì **map 337 bị cấm vật phẩm dịch chuyển** (`header\forbidmap.lua`) | `script\header\forbidmap.lua`: `CheckAllMaps` chỉ chặn cứng **`mapid == 984`**; 337/338/339 **không có** trong bất kỳ danh sách nào (`TRAINMAPS` có 336 và 340, không có 337) | **SAI ở vế "bị cấm dịch chuyển"**; vế "không `AddMSPlayer` nên không thuộc mission 15 và `Landing()` bỏ qua" vẫn ĐÚNG | bỏ mệnh đề forbidmap, giữ mức **NẶNG** vì lý do mission; và xem thêm "Bỏ sót BS-3" ở dưới |
| 19 | **A7-N7**: nút "Sinh lại NPC" nhân bản NPC | `hd3_admin.lua:27` gọi `HD3_DriverInit()` (`hd3_driver.lua:36-52`): Include lại rồi `add_dialognpc_hd3` + `add_killertasknpc` + `HD3_PLD_AddBoatNpc`, **không có `ClearMapNpc`/`DelAllNpc`, không cờ chống gọi lại** | **ĐÚNG** | — |
| 20 | **A7-L1**: nhãn "Ba Lăng Huyện" sai, map 1 là Phượng Tường | `Maps\WorldSet_GameServer.ini:5` ghi `World000=1 --- Phượng Tường`; `hd3_admin.lua:20,28` (`HD3_ADM_ST_Tele` gọi `NewWorld(1,1506,3198)`), `:60` | **ĐÚNG** | — |
| 21 | **A7-L2**: chuỗi ở `hd3_admin.lua:68` bị Lua 4 nuốt dấu gạch chéo ngược | Lua 4.0 `read_string` gặp escape lạ thì giữ ký tự và **bỏ dấu gạch chéo**, nên in ra `scriptheadercauhinh_hoatdong.lua`. Đã chạy `syncheck.exe`: tệp **hợp lệ cú pháp**, chỉ sai chữ hiển thị | **ĐÚNG — NHẸ** | — |
| 22 | **A7-L4**: `IncludeLib("NPCINFO")` không có trong bảng module | `ScriptFuns.cpp:2485-2490` bảng `szMod[21]` không chứa `"NPCINFO"`; `:2551` ghi log "module la ... bo qua". `kill_level.lua:9` đúng như nêu. Vô hại vì `NPCINFO_GetSeries` đã đăng ký trực tiếp (`ScriptFuns.cpp:15434`) | **ĐÚNG — NHẸ** | — |
| 23 | **A7 §3.1**: `taskguide.txt` dòng 9 là Sát Thủ; dòng 10 "Thách thức thời gian" **là Tín Sứ chứ không phải Vượt Ải** | `Patch\settings\task\taskguide.txt` dòng 9 = `8 / Nhiệm vụ Sát thủ / \UI\taskui_killer.lua`; dòng 10 = `9 / Thách thức thời gian / \UI\taskui_messenger.lua`. Giải mã GBK `Patch\ui\taskui_messenger.lua:21`: `function showmessengerdesc(nTaskGenre) --闯关任务之信使任务`, đọc `GetTask(1201..1204)`, tức **Tín Sứ**. Trùng tên tiếng Việt với Vượt Ải là bẫy thật | **ĐÚNG** — kết luận "chỉ Sát Thủ cần F11" đứng vững | — |
| 24 | **A7 §3.3**: khuôn C++ để làm F11 Sát Thủ | `UiTaskGuide.cpp:29-32` (`DATAU 6 / TINSU 7 / BANGCHIEN 8 / BACHNHAN 9`, id trống kế tiếp đúng là **10**), `BuildTinSuText()` ở `:513`, rẽ nhánh ở `:170-188` và `:379-393` — khớp từng dòng | **ĐÚNG** | — |
| 25 | **A7 §4.1**: 28 id lõi sạch | Tự tìm lại 12 id đại diện (`2852 1505 2863 3105 2871 2880 2882 2817 10119 10179 10180 10235`) trên toàn cây trừ các tệp HD3: **0 va**. `settings\task\missions.txt` dòng 16 trỏ mission 15 sang `fengling_ferry\mission.lua`, dòng 23 trỏ mission 22 sang `challengeoftime\mission_match.lua`; `lib\lib_task.lua:284-288` có `MS_VUOTAI=3`, `MS_PLANGDO=4` nên không đụng 15/22 | **ĐÚNG** | — |
| 26 | **A7 §1.5(B)**: `DisabledUseTownP` thật "ĐÃ PORT — cải thiện thật so với trước" | Engine đăng ký `DisabledUseTownP`, **`GetDisabledUseTownP`**, `IsDisabledUseHeart` (`ScriptFuns.cpp:15300,15415,15416`). Nhưng tìm toàn cây script: **0 tệp gọi `GetDisabledUseTownP`**. Và chú thích `KJx2WarInfra.cpp:1654-1655` nói *"đã thêm map 3 hoạt động vào `forbidmap.lua`"*, trong khi `script\header\forbidmap.lua` **chỉ có `mapid == 984`**, không có 337/338/339 cũng không có 464-495 | **HẠ THẤP — phải nâng thành lỗi**: cờ chỉ **ghi**, không ai **đọc**, nên vẫn dùng được Hồi thành phù trong thuyền PLD và trong ải | "`DisabledUseTownP` mới là **write-only**. Cần (a) thêm 337/338/339 và 464-495 (và 957) vào `CheckAllMaps` của `forbidmap.lua`, hoặc (b) cho `townportal_l.lua` và `heart_head.lua` gọi `GetDisabledUseTownP()`. Mức **NẶNG**" |

### Tổng kết đối chất

| Kết luận | Số | Mã |
|---|---|---|
| ĐÚNG (giữ nguyên) | 17 | #1 #2 #7 #8 #9 #10 #11 #12 #13 #14 #15 #19 #20 #21 #22 #23 #24 #25 |
| SAI | 4 | #3 (gán nhầm đường Tín Sứ) · #5 (bản vá đề xuất không hiệu lực) · #6 (L1 "bắt buộc") · #16 (N10 "chưa ai Include") · #18 (vế forbidmap) |
| THỔI PHỒNG | 2 | #4 (phạm vi hồi quy Tín Sứ) · #17 (N11 xếp NẶNG) |
| HẠ THẤP | 1 | #26 (`DisabledUseTownP` chấm đạt nhưng thực chất hỏng) |

---

## Bỏ sót của chính vòng soát

### BS-1 (NẶNG) — **JX1 ĐÃ CÓ SẴN cả 3 hoạt động bản Việt; A7 không soát trùng một chữ nào**

Cây sống có `script\tinhnang\boss_satthu\`, `script\tinhnang\phonglangdo\`, `script\tinhnang\vuot_ai\` — ba hệ **cùng tên tính năng** với đợt port (`lib_vuotai.lua:1-2` ghi "Author: Fong Kieu, 28/11/2016").

* **Trùng MAP**: `lib_vuotai.lua:33-43` có `MAP_VUOTAI = {480 ... 489}`; bản Linux `challengeoftime\include.lua:96-101` có `tbLevelMaps[2] = {480 ... 495}` — **10 map chồng nhau**.
* **Khác mission nhưng cùng subworld**: cũ `MS_VUOTAI = 3` (`lib\lib_task.lua:284`), mới `MISSION_MATCH = 22`. Không va id, nhưng `timertask\task01.lua:141` gọi `DelAllNpc(SubWorld)` — nếu hệ cũ được bật lại nó sẽ **xoá sạch NPC của mission 22**.
* **Trùng lối vào**: NPC cũ `global\npcchucnang\nhieptran.lua` và `tinhnang\vuot_ai\sugiasatthu.lua` (`lib\lib_map.lua:8` `OTHER_SGSTHU`) so với NPC 769 mới; thuyền phu cũ `tinhnang\phonglangdo\thuyenphu.lua` và `thuyenphubac.lua` so với `hd3_thuyenphu.lua` mới.
* **Hiện trạng thật (đã kiểm)**: cả ba **đang tắt** — `startgame.lua:100` `-- addnpcbosssatthu()`, `:102` `-- addnpcphonglangdo()`, `timerserver.lua:82,83` `-- sukien_vuotai(...)` và `-- sukien_phonglangdo(...)`. Chỉ dòng 102-103 mang nhãn `[3HD 25/08]`; hai dòng còn lại không rõ do đợt nào tắt.
* **Việc phải làm**: ghi vào bàn giao "3 hệ VN cũ đã bị thay thế, CẤM bật lại `sukien_vuotai` / `sukien_phonglangdo` / `addnpcbosssatthu`", và cân nhắc xoá hẳn hoặc đổi dải map của hệ cũ.

### BS-2 (NẶNG) — **`script\battles\battlehead.lua` là tệp MỚI THÊM và nó chui vào state của 4 tính năng ĐANG SỐNG**

`b1_copy.py` (docstring, dòng 10) ghi rõ "KHÔNG BAO GIỜ đè tệp đã có ở đích — chỉ chép MISSING", và `battlehead.lua` **có trong `b1_manifest.txt:23`**. Suy ra trước 25/08 tệp này **không tồn tại** và mọi `Include` tới nó **thất bại im lặng**. Nay nó tồn tại nên:

* `startgame\tinhnang\tongkim\songjin_shophead.lua:2` (**shop Tống Kim đang chạy**), bản sao trong `global\...\songjin_shophead.lua:2`, `battles\battleinfo.lua:1`, và 2 trap Công Thành Chiến — tất cả **bây giờ mới thực sự nạp** `battlehead.lua`.
* `battlehead.lua:1-17` kéo theo **11 Include** (`event\storm\function.lua`, `event\great_night\...`, `missions\boss\bigboss.lua`, `battles\lang.lua`, `lib\common.lua`, `battles\battle_rank_award.lua`, `bonusvlmc\head.lua`, `misc\vngpromotion\ipbonus\...`, `event\jiefang_jieri\...`, `battles\doubleexp.lua`, `battles\weeklyrank.lua`) và đặt các biến toàn cục `FRAME2TIME`, `BAOMING_TIME`, `FIGHTING_TIME`, `TIMER_1`, `TIMER_2` (`:19-25`) **vào state của shop Tống Kim**.
* Rủi ro: đè tên hàm/biến toàn cục, tăng thời gian nạp, và mọi lỗi trong 11 tệp đó nay nổ ngay trên đường NPC shop Tống Kim.
* **A7 khẳng định ngược lại** (mục A7-L8: "`script\battles\*` ... không tệp nào Include") nên đây vừa là bỏ sót vừa là kết luận sai.
* Việc phải làm: thử NPC shop Tống Kim sau khi restart. Nếu không cần thì cân nhắc **gỡ** `script\battles\*` (chỉ `challengeoftime\npc.lua:5` thực sự cần, còn `fengling_ferry\boss.lua` là mã chết).

### BS-3 (NẶNG) — **`forbidmap.lua` chưa hề thêm map 3 hoạt động, trái với chú thích trong mã C++ vừa viết**

`KJx2WarInfra.cpp:1653-1655` viết: *"item hồi thành của JX1 chặn theo map ở `script\header\forbidmap.lua` (**đã thêm map 3 hoạt động** — cùng khuôn Thành Bảo 984)"*. Kiểm `script\header\forbidmap.lua:94-99`: chỉ có `if mapid == 984 then return 1`. `townportal_l.lua:48` gọi `CheckAllMaps(nSubWorldID)`, nên trong thuyền 337/338/339 và 32 map ải 464-495 người chơi **vẫn dùng được Hồi thành phù / Tiểu hồi thành**. Cộng với đối chất #26 (`GetDisabledUseTownP` không ai đọc), cơ chế cấm rời hoạt động **hỏng ở cả hai lớp**. A7 chấm mục này là đạt.

### BS-4 (CHƯA XÁC MINH, cần soát) — **A7 chỉ soát vật phẩm `6,1,*`, bỏ trắng nhánh `6,0,*`**

Quét độc lập 101 tệp: ngoài 39 bộ ba `6,1,30xxx`, còn **26 bộ ba `6,0,*`** được dùng, mà JX1 **không có một hàng nào genre 6 / detail 0** (đã kiểm toàn bộ `settings\item\*.txt`: 0 hàng). Điểm gãy thật nằm ở bảng thưởng: `missions\boss\bigboss.lua:53,54` (`{6,0,6,...}` x20 và `{6,0,3,...}` x20 — Phi Tốc hoàn / Đại Lực hoàn), `missions\yandibaozang\head.lua:119-128` (10 món "Viêm Đế ... hoàn"), `vng_event\thapnienlenhbai\lenhbai_def.lua:464`. Các chỗ trong `vng_feature\forbiditem\vngforbidspecialitem.lua:76-88` chỉ là danh sách cấm nên vô hại. **Phải bổ sung vào bảng ánh xạ A7-N1.**

### BS-5 (đã làm hộ, KẾT QUẢ TỐT) — **không ai chạy kiểm cú pháp Lua 4 trên chính 101 tệp đã chép**

Đã chạy `syncheck.exe` (nhân Lua 4.0 của chính dự án) trên **91 tệp `.lua`** trong manifest, cộng 4 tệp tự viết, cộng `startgame.lua`, `timerserver.lua`, `cauhinh_hoatdong.lua`, `lenhbaiadmin.lua`, `killbosshead.lua` và 2 tệp `settings\trigger_*.lua`: **0 lỗi cú pháp**. Tin tốt, nhưng A7 lẽ ra phải tự kiểm trước khi chấm yêu cầu 1, vì một tệp hỏng cú pháp là CHẶN tức thì.

### Ghi thêm (NHẸ, ngoài phạm vi đợt port nhưng phát hiện lúc soát)

`script\missions\clearskill\head.lua:144` gọi `IsDisabledUseTownP()` — engine **không đăng ký** tên này (chỉ có `DisabledUseTownP` và `GetDisabledUseTownP`), nên đường NPC Phân Tầng (`global\npcchucnang\phantang.lua:3`) sẽ lỗi "nil value" khi chạm nhánh đó. Lỗi có **từ trước** đợt port, nhưng vì đợt này vừa đăng ký `GetDisabledUseTownP` nên thêm 1 dòng alias `{"IsDisabledUseTownP", LuaHD3_GetDisabledUseTownP}` là vá luôn được.

---

## THỨ TỰ SỬA — bản đã hiệu chỉnh sau đối chất

1. **A7-C1** — **KHÔNG** vá bằng `g_IsJx2Script` (không hiệu lực, xem đối chất #5). Cách rẻ và đúng nhất: **hoàn nguyên 14 lần đổi `AddNpc` sang `AddNpcEx` cộng `random(0,4)`**, vì bản Linux dùng đúng thứ tự tham số của JX1; chấp nhận ngũ hành NPC bằng giá trị cột 6 (0 hoặc 1). Nếu vẫn muốn giữ `AddNpcEx` thì phải bỏ hẳn nhánh `SetCurrentCamp` trong hàm và trả camp cho 3 script JX1 gọi `SetNpcCurCamp` riêng.
2. **A7-C2** — chuyển `settings\trigger_challengeoftime.lua` và `trigger_include.lua` sang `script\missions\challengeoftime\`, sửa `hd3_driver.lua:23` cùng `Include` bên trong; bỏ tiền tố `"\settings\trigger_"` khỏi `g_IsJx2Script` (`KSortScript.cpp:130`) vì không còn dùng.
3. **A7-C3 và A7-C4** — thêm `2745 sang 3363`, `196 sang 195` vào `remap_resolved.json`, chạy lại `b2_patch.py`, sửa `hd3_admin.lua:53`.
4. **BS-3 và đối chất #26** — thêm 337/338/339 và 464-495 (và 957 khi có) vào `CheckAllMaps`, hoặc nối `GetDisabledUseTownP()` vào `townportal_l.lua` và `heart_head.lua`.
5. **A7-N1 và BS-4** — mở rộng bảng ánh xạ: 39 bộ ba `6,1,30xxx`, 26 bộ ba `6,0,*`, và nhóm lệch 1 bậc.
6. **BS-1** — chốt và ghi bàn giao về 3 hệ VN cũ (`tinhnang\boss_satthu`, `tinhnang\phonglangdo`, `tinhnang\vuot_ai`), khoá không cho bật lại.
7. **BS-2** — kiểm thử shop Tống Kim sau restart, vì `songjin_shophead.lua` nay mới thật sự nạp `battlehead.lua`.
8. **A7-N2** (map 957), rồi **Yêu cầu 2** (khoá thưởng và nối 9 khoá chết), rồi **Yêu cầu 5** (nút admin), rồi **Yêu cầu 3** (F11 Sát Thủ), rồi **A7-N6** (NPC 1032-1034).
9. **A7-N10 và A7-N11** — hạ xuống NHẸ, chỉ cần ghi chú "mìn: `bt_sortbthonour()` và `storm_clear()` hiện có 0 điểm gọi".
