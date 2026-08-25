# 14 — Bổ sung vòng 2: ĐỤNG ĐỘ ĐỊNH DANH giữa 3 hoạt động (bản Linux) và JX1

> Hướng quét 5. Mục tiêu: **tìm chỗ bỏ sót**, không viết lại vòng 1.
> Mọi khẳng định dưới đây kèm `tệp:dòng` hoặc địa chỉ ELF. Chỗ không chắc ghi rõ **CHƯA XÁC MINH**.
> Bảng đầy đủ: **`port_3hd\id_dungdo.csv`** (340 dòng, đã sắp TRÙNG NẶNG lên đầu).

---

## 0. Tóm tắt một trang

| Chỉ số | Giá trị |
|---|---|
| Tổng định danh đã đối chiếu | **340** |
| **TRÙNG NẶNG** (cùng số, JX1 đang dùng cho **việc khác**) | **48** |
| TRÙNG NHẸ (cùng số, cùng ngữ nghĩa — chỉ cần không đăng ký hai lần) | 191 |
| AN TOÀN (JX1 chưa ai dùng số đó) | 99 |
| Thiếu API (không phải va chạm số, nhưng chặn chạy) | 2 nhóm khoá `AddStatData` |

**48 chỗ TRÙNG NẶNG chia làm 4 nhóm:**

| Nhóm | Số lượng | Hậu quả nếu port thẳng |
|---|---|---|
| **Vật phẩm** `(genre,detail,particular)` | **42** | Phát **nhầm vật phẩm** cho người chơi — hỏng kinh tế, không có thông báo lỗi |
| **NPC template** 1032 / 1033 / 1034 | 3 | Ải cao cấp sinh ra **Boss New Dragon** thay vì tiểu Boss vượt ải |
| **Map** 337-339 và 480-489 | 2 dòng (13 map) | Hai hệ *cùng tên tính năng* giẫm lên nhau trên **cùng map** |
| **Task id 88** | 1 | Chuyển sinh đọc bộ đếm **Dã Tẩu** làm điểm tiềm năng ⇒ **hỏng chỉ số nhân vật** |

---

## 1. Phát hiện lớn nhất của vòng 2 — cùng **số**, khác **vật**

### 1.1 Vật phẩm: 42/97 bộ số đang trỏ sang vật phẩm KHÁC ở JX1

Vòng 1 (`item_can_them.csv`, 84 dòng) đã đánh dấu cột `TrungIdJX1 = CO` nhưng **không nói JX1 đang
dùng số đó cho vật phẩm nào**, và **bỏ sót toàn bộ `missions\boss\bigboss.lua`** (bảng thưởng dùng
chung của cả satthu lẫn vuotai). Vòng 2 quét lại được **97 bộ số** (nhiều hơn 13) và tra tận tên.

Bảng đối chiếu lấy từ `settings\item\magicscript.txt` hai bên
(JX1: `E:\…\bin\server\settings\item\magicscript.txt`; Linux: `D:\ServerLinux\Patch\settings\item\004\magicscript.txt`
— **lưu ý bảng vật phẩm bản Linux nằm trong thư mục con `004`**, đọc thẳng `settings\item\*.txt` sẽ ra rỗng).

**Những cặp nguy hiểm nhất — tên gần giống nên duyệt tay rất dễ bỏ qua:**

| Bộ số | Bản Linux (thứ 3 hoạt động muốn) | **JX1 cùng số đang là** | JX1 có cùng tên ở |
|---|---|---|---|
| `6,1,399` | **Sát Thủ lệnh** | **Sát thủ giản** | `6,1,398` |
| `6,1,400` | **Sát thủ giản** | **Sư đồ thiếp** | `6,1,399` |
| `6,1,906` | Quả Huy Hoàng **(cao)** | Quả Huy Hoàng **(trung)** | `6,1,907`, `6,1,3440` |
| `6,1,907` | **Quả Hoàng Kim** | Quả Huy Hoàng (cao) | `6,1,908` |
| `6,1,215` | Càn Khôn Tạo Hóa Đan **(đại)** | Càn Khôn Tạo Hóa Đan **(trung)** | `6,1,214` |
| `6,1,2742` | **Bảo Rương Vượt ải** | Bảo Rương Tử Mãng Khí Giới | `6,1,3360` |

`6,1,399` là ví dụ điển hình: `add_shashouling()` phát 8 lần
(`script\task\tollgate\killer\lib_killlevel.lua:73,77,81,85,89,93,97,103`,
`AddItem(6,1,399,<20…90>,series,0)`). Đem sang JX1 nguyên si thì người chơi nhận **Sát thủ giản**
(vật phẩm mang đến gặp Nhiếp Thí Trần) thay vì **Sát Thủ lệnh** — và JX1 *đang có* hệ Boss Sát Thủ
bản VN phát đúng `{6,1,398}` (`script\tinhnang\vuot_ai\sugiasatthu.lua:22-23`,
`script\tinhnang\boss_satthu\drop.lua:49`) ⇒ hai nguồn phát hai vật phẩm khác nhau cho **cùng một
việc**.

**Không có quy luật dịch số cố định.** Kiểm chứng: `6,1,68/69/72/73/74` **trùng khít** hai bên,
`6,1,71` cũng là **cùng một vật phẩm** (cùng `\spr\item\medecine\xiancaolu.spr`, cùng mô tả
"Nội 1 giờ, kinh nghiệm tăng gấp đôi") chỉ khác chữ — JX1 đổi tên thành "Tiên Thảo Lộ Thường" và
chèn thêm "Tiên Thảo Lộ Trung" ở `6,1,70`. Nhưng `6,1,2347` lệch **+9** (`→ 6,1,2356`),
`6,1,2743` lệch **+618** (`→ 6,1,3361`), `6,1,3810` lệch **+618** (`→ 6,1,4428`).
⇒ **Cấm đoán bằng công thức; phải tra từng dòng** (cột `JX1_DangDungChoGi` của CSV đã tra sẵn).

Phân bố đầy đủ 97 bộ số: **42 TRÙNG NẶNG** · **20 TRÙNG NHẸ** (JX1 trống số đó nhưng có vật phẩm
cùng tên ở số khác — chỉ cần ánh xạ, ví dụ `6,1,30228 "Chân Nguyên Đơn (trung)" → 6,1,4846`) ·
**35 AN TOÀN** (trong đó **27 bộ JX1 chưa có gì cả** — phần lớn dải `6,1,30xxx`, vật phẩm đời sau
của bản Linux — cộng 8 bộ trùng khít hai bên).

### 1.2 NPC template 1032 / 1033 / 1034 — JX1 đã ghi đè bằng "Boss New Dragon"

`settings\npcs.txt` **không có cột id**; engine tra bằng **`nNpcTemplateId + 2`**
(`Sources\Core\Src\KNpcTemplate.cpp:71` `int nNpcTempRow = nNpcTemplateId + 2;`, cùng `KNpc.cpp:5320`;
`KTabFile::GetString` đánh số dòng **từ 1** — `Sources\Engine\Src\KTabFile.cpp:210,217` `GetValue(nRow-1, …)`)
⇒ **NPC id `N` nằm ở dòng `N+2`** (dòng 1 là tiêu đề, template đánh số **từ 0**).
*[đã sửa theo đối chất — bản cũ ghi `N+1`. Các **TÊN** trong bảng dưới **vẫn đúng** (đã đối chiếu tận mắt),
nhưng cột số dòng trong `id_dungdo.csv` (1033 / 1034 / 1035) **lệch 1**, số đúng là **1034 / 1035 / 1036**
ở cả hai bản.]*
Đối chiếu toàn bộ 140 id của 3 hoạt động:

```
NPC 1032 : Linux "(cao cấp) tiểu Boss nam 7"  →  JX1 "Boss New Dragon 165"
NPC 1033 : Linux "(cao cấp) tiểu Boss nam 8"  →  JX1 "Boss New Dragon 166 8"
NPC 1034 : Linux "(cao cấp) tiểu Boss nữ 1"   →  JX1 "Boss New Dragon 167"
```

`tbRangeId[2] = {{1026,1033},{1034,1037}}` (`script\missions\challengeoftime\include.lua:111-114`)
chạm đúng 3 id này. Vòng 1 đã bắt được (`03_vuotai.md:829-833`) — vòng 2 **xác nhận lại** và bổ
sung một hệ quả vòng 1 chưa nêu:

> 🔴 **JX1 đang tự mâu thuẫn với chính nó**: `script\tinhnang\vuot_ai\lib_vuotai.lua:180-182`
> (`BOSS_VUOTAI`) vẫn khai `1032 = "Càn Khôn"`, `1033 = "Vi Đà"`, `1034 = " "` — tức Vượt Ải bản VN
> của JX1 **hiện đang sinh ra Boss New Dragon** khi chạy. Đây là lỗi **có sẵn**, không do đợt port
> này gây ra, nhưng nó khoá lựa chọn: **không được sửa `npcs.txt` về lại tên cũ** nếu Boss New
> Dragon đang là nội dung sống.

137/140 id còn lại **trùng tên khít** ⇒ giữ nguyên.

### 1.3 Map — JX1 **đã có cả ba tính năng** bản VN, và chúng chiếm đúng map

Vòng 1 có nêu (`06_phia_jx1.md:494-500`, `02_phonglangdo.md:663-690`) nhưng **chưa quy ra bảng số**.
Vòng 2 chốt con số:

| | Bản Linux muốn | JX1 **đang** dùng | Bằng chứng JX1 |
|---|---|---|---|
| Vượt Ải | `tbLevelMaps[2] = {480…495}` | `MAP_VUOTAI = {480…489}` — **trùng 10 map** | `script\tinhnang\vuot_ai\lib_vuotai.lua:33-44` |
| Phong Lăng Độ | `boatMAPS = {337,338,339}` | `MAP_DUATHUYEN_PLD = {337,338,339}` — **trùng cả 3** | `script\tinhnang\phonglangdo\lib_phonglangdo.lua:108-112` |
| Săn boss sát thủ | boss rải theo `killer.txt` | `BOSS_SATTHU` 20 boss trên map 321/225/93/75/340 | `script\tinhnang\boss_satthu\lib_boss_st.lua:7-29` |

Cửa vào bằng mission:

```
settings\task\missions.txt  (id = SỐ DÒNG, xem chú thích script\missions\mission_trong.lua:2)
  3  -> \script\missions\mission03.lua   = "Mission vượt ải"        (MS_VUOTAI  = 3, lib_task.lua:284)
  4  -> \script\missions\mission04.lua   = Phong Lăng Độ            (MS_PLANGDO = 4, lib_task.lua:285)
 15  -> \script\missions\mission_trong.lua   <= chỗ Phong Lăng Độ JX2 muốn (MISSIONID = 15)
 22  -> \script\missions\mission_trong.lua   <= chỗ Vượt Ải JX2 muốn (MISSION_MATCH = 22)
```

⇒ **Ô mission 15 và 22 còn trống, cấp được.** Nhưng map thì **không** — hai hệ cùng tính năng sẽ
chạy chồng lên nhau.

**Trạng thái bật/tắt của bản VN (đã kiểm lại tận mắt):**

* Lịch: `script\timerserver.lua:79` `-- sukien_vuotai(nHr,nMi)` và `:80` `-- sukien_phonglangdo(nHr,nMi)`
  — **đều đang bị chú thích**.
* Nhưng `script\startgame.lua:100` `addnpcphonglangdo()` **KHÔNG bị chú thích** (dòng 99
  `-- addnpcbosssatthu()` thì có) ⇒ **6 NPC "Thuyền phu" (`lib_phonglangdo.lua:147-157`, npcId 240)
  vẫn spawn mỗi lần khởi động** trên chính 3 map bến thuyền.

⇒ Đề xuất: gỡ hẳn `addnpcphonglangdo()` **trước**, hoặc dời `tbLevelMaps[2]` sang **`496…511`**.
*[đã sửa theo đối chất — bản cũ đề xuất `490…500` "còn dư 5 ô": **SAI cả sức chứa lẫn phạm vi**.
`tbLevelMaps[2]` có **16 map** (`challengeoftime\include.lua:97-102` = `480…495`) mà `490…500` chỉ có **11 ô**.
Khối `特殊用地\杀手的试炼` trong `settings\MapList.ini` **không dừng ở 500**: nó chạy `464…511` (**48 mục**).
`490…495` đã mang tên "Thách thức thời gian (Cao cấp 11…16)" và **đã đăng ký sống** trong
`Maps\WorldSet_GameServer.ini` (`World469=490` … `World474=495`); `496…511` là khối riêng
"Thử luyện tài nghệ (Cao cấp 1…16)" (`World475=496` … `World490=511`) — **đúng 16 ô**, vừa khít
`tbLevelMaps[2]`, và không tệp `.lua` nào của `script\tinhnang\` đang dùng.]*

### 1.4 Task id 88 — chỗ **vòng 1 chưa hề chạm tới**

`task_id_doi_chieu.csv` của vòng 1 chỉ kiểm **19 id** và kết luận "0 xung đột thật". Vòng 2 quét
**79 task id** (56 số trực tiếp + 18 hằng số hệ Phong Bão + 1 `SetTaskTemp` + 4 id qua hàm bọc),
tìm thêm **1 va chạm thật sự nghiêm trọng**:

| | Bản Linux | JX1 |
|---|---|---|
| **Task 88** | 4 byte đóng gói (Sức/Thân/Ngoại/Nội) do nhiệm vụ thưởng thẳng, đọc bằng `GetByte(Utask88,1..4)` — `script\task\metempsychosis\task_func.lua:84-91` (`zhuansheng_clear_prop`) | `T_TimVatPham` = 88 — bộ đếm **Dã Tẩu "tìm vật phẩm"** — `script\lib\lib_task.lua:111`, dùng ở `script\global\npcchucnang\datau.lua:43,48` |

`task/metempsychosis/task_func.lua` nằm trong **bao đóng của cả 3 tính năng** (`src_utf8\INDEX.md`).
Port thẳng ⇒ hàm reset tiềm năng của chuyển sinh sẽ lấy **số vật phẩm Dã Tẩu** làm điểm Sức/Thân/
Ngoại/Nội ⇒ **hỏng chỉ số nhân vật, không có thông báo lỗi**.

Ngoài ra vòng 2 tìm thêm **8 task id vòng 1 bỏ sót**, trong đó **7 nằm ở tệp LÕI**:

```
2636 tsk_rank_lastdate    2637 tsk_rank_lastscore      <- lõi Vượt Ải
2638 tsk_rank_2thdate     2639 tsk_rank_2thscore          script\missions\challengeoftime\rank_perday.lua:4-7 ; npc_death.lua:35-43
3070 TaskDaily Chân Nguyên Đơn (PLĐ)                      script\missions\fengling_ferry\fld_head.lua:121,122
3071 bit 25  (bittask, Vượt Ải)                           challengeoftime\npc\dragonboat_main.lua:30 ; vng_feature\challengeoftime\npcNhiepThiTran.lua:14
3079 TaskDaily Vượt Ải                                    challengeoftime\npc\dragonboat_main.lua:166,167
2661 TSK_BIGBOSS_REWARD                                   missions\boss\bigboss.lua:11
```
Cả 8 **JX1 chưa ai dùng ⇒ AN TOÀN**, nhưng phải đăng ký vào `script\lib\lib_task.lua` để đợt sau
không cấp lại (đúng bài học của đợt "trùng task id 24/08"). Ba id `3070 / 3071 / 3079` chỉ lộ ra khi
quét thêm **hàm bọc** — xem mục 3.1.

---

## 2. Kiểm lại vòng 1 (chống sai dây chuyền)

Đã mở tận mắt **4 hàng** của `task_id_doi_chieu.csv` và **1 kết luận** của `01_satthu.md`:

| Hàng vòng 1 | Kết quả kiểm lại |
|---|---|
| `200` → `item\hoatdong_admin.lua:**125**` | ❌ **vòng 1 trích SAI vị trí lệnh gọi** — dòng 125 chỉ là chuỗi hội thoại `"Gỡ kẹt trận treo của TÔI (task 200 va 2340-2342)/HD_BW_GoKet"` (có chữ "200" nên lọt lưới grep), lệnh thật `SetTaskTemp(200, 0)` nằm ở **dòng 135**. *[đã sửa theo đối chất — bản vòng 2 cũ ghi "dòng 125 là chuỗi `\"Quay lại/HD_AdminMenu\"`" (**sai**, chuỗi đó ở **dòng 126**) và ghi lệnh ở "**dòng 134**" (**sai**, dòng 134 là `function HD_BW_GoKet()`; lệnh ở **dòng 135**).]* |
| `200` → cột `SoChoJX1DaDung = 23`, nhưng cột vị trí chỉ liệt kê 8 | ✅ **CON SỐ 23 ĐÚNG** (đếm lại `grep "TaskTemp\s*(\s*200\b"` trên `bin\server\script`, bỏ tệp `.truoc_*` và bỏ dòng chú thích ⇒ đúng **23**). ⚠️ Nhưng **danh sách vị trí thiếu 15 chỗ**: `citywar_city\mission.lua:189`, `leaguematch\head.lua:519,533`, `tong\collectgoods\head.lua:224,260,294`, `tong_disciple\head.lua:187,220,252`, `tong_springfestival\head.lua:218,257,287`, `tongwar\match\head.lua:132,502`, `tongwar\trap\tongwar_trap.lua:116`. |
| `1550` → `event\storm\function.lua:396;417` | ✅ **ĐÚNG** (`SetTask(1550,0)` cả hai dòng, chú thích GBK 杀手进行次数). |
| `1082/1192/1193/1217` "hoàn toàn trống" | ✅ **ĐÚNG** — quét lại 3.780 tệp của `bin\server\script` + `scriptjx2` + `serverscript_jx2`: 0 hit. |
| `01_satthu.md:826` "skill 541/547/548 — **CÓ ĐỦ 3** (tra theo cột SkillId)" | ✅ **ĐÚNG, và cách tra là điểm mấu chốt.** `settings\skills.txt` của JX1 **lệch một dòng** so với bản Linux, nhưng engine tra bằng **cột `SkillId`** (`g_SkillManager.GetSkill(nSkillId, …)`, `Sources\Core\Src\ScriptFuns.cpp:12813`) ⇒ `AddSkillState(541,…)` vẫn ra đúng "Hoàn thành nhiệm vụ Sát thủ". **Không được tra skills.txt theo số dòng.** *[đã sửa theo đối chất — hai chi tiết minh hoạ trong bản cũ **SAI**: (a) "dòng 541 của JX1 là `SkillId=542` 'Tín sứ lệnh ở mộc'" — thật ra **dòng 541 = `SkillId` 540** ("Vòng tròn miễn dịch"); `SkillId=541` "Hoàn thành nhiệm vụ Sát thủ" nằm ở **dòng 542**, `SkillId=542` "Tín sứ lệnh ở mộc" ở **dòng 543** (quy tắc: dòng `L` mang `SkillId = L−1` vì dòng 1 là tiêu đề `SkillName|Property|SkillId|…`); `SkillId=547` ở dòng 548, `548` ở dòng 549. (b) "JX1 hiện **đang gọi y hệt**" — **không y hệt**: Linux `kill_level.lua:82` = `AddSkillState(541,1,0,54)` (4 tham số, 54); JX1 `boss_satthu\death.lua:22` = `AddSkillState(541,1,0,108*2,-1)` (5 tham số, thời lượng **216**). Cùng skill id, **khác thời lượng gấp 4**.]* |

> 🔴 Bẫy suýt mắc: `npcs.txt` **không có** cột id (id = số dòng) còn `skills.txt` **có** cột `SkillId`.
> Hai bảng cùng thư mục nhưng **quy tắc tra ngược nhau**.

---

## 3. Bảng theo từng loại định danh

### 3.1 Task id (79 mục: 76 task thường + 2 task tạm + 1 cụm bit)

| Mức | Số lượng | Danh sách |
|---|---|---|
| **TRÙNG NẶNG** | **1** | `88` |
| TRÙNG NHẸ (cùng ngữ nghĩa, JX1 đã có) | 29 | `200`(temp), `751`, `1036`, `1122`, `1201`, `1202`, `1203`, `1550`, `2361`, `2446`, `2509` + 18 id hệ Phong Bão `1661…1680` |
| AN TOÀN | 49 | `199, 1082, 1192, 1193, 1217, 1505, 1551, 1760-1765, 1830, 1937-1944, 2307, 2308, 2547, 2580, 2636-2639, 2641, 2642, 2661, 2698, 2784, 2807, 2852, 2885, 2929, 2931-2936, 3070, 3071(bit 25), 3079, 4000, 4017` |

#### 🔴 Điểm mù của cách quét — **5 task id chỉ lộ ra qua hàm bọc**

Quét theo `GetTask/SetTask/nt_setTask/gb_SetTask` **KHÔNG bắt được** những task id đi qua hàm bọc.
Phải quét thêm `PlayerFunLib:*TaskDaily` và `tbVNG_BitTask_Lib`. Năm id sau **vòng 1 không có
trong `task_id_doi_chieu.csv`**, mà **ba trong số đó nằm ngay trong tệp lõi**:

| Id | Dùng bởi | Ghi chú |
|---|---|---|
| **3070** | `script\missions\fengling_ferry\fld_head.lua:121,122` — `CheckTaskDaily(3070,5,"nomsg","<")` + `AddTaskDaily(3070,1)`, mốc Chân Nguyên Đơn khi đăng ký PLĐ tốn phí | **LÕI** Phong Lăng Độ |
| **3071 bit 25** (1 bit) | `challengeoftime\npc\dragonboat_main.lua:30` và `vng_feature\challengeoftime\npcNhiepThiTran.lua:14` — `getBitTask{nTaskID=3071, nStartBit=25, nBitCount=1}` | **LÕI** Vượt Ải |
| **3079** | `challengeoftime\npc\dragonboat_main.lua:166,167` — `AddTaskDaily/GetTaskDailyCount` | **LÕI** Vượt Ải |
| **2661** | `missions\boss\bigboss.lua:11` `BigBoss.TSK_BIGBOSS_REWARD = 2661`, dùng ở `:183,249,254,259,276,291` | LÕI (bảng thưởng dùng chung) |
| **2807** | `activitysys\config\32\variables.lua:6` `TSK_DAILY_TASK_COMPLETE_COUNT = 2807`, dùng ở `config\32\dailytask.lua:44,64` | HỖ TRỢ |

Cả 5 **AN TOÀN** (đã grep `(GetTask|SetTask|nt_setTask|nt_getTask|TaskDaily|BitTask)\s*\(\s*(3070|3071|3079|2661)` trên
`bin\server\script` + `scriptjx2` = 0 hit; mọi lần xuất hiện của các số này trong JX1 đều là **toạ độ**,
ví dụ `startgame\thanh\thanhdo.lua:70` `AddNpcNew(...,3070*32,...)`).

* Cách mã hoá `AddTaskDaily`: **`ngày(%y%m%d) * 256 + số lần`** trong **một** ô task
  (`activitysys\playerfunlib.lua:422-434`) ⇒ giá trị lên tới ~65 triệu, vẫn vừa `int`.
* Hàm nền cho bit task **JX1 đã có**: `GetBitTask` / `SetBitTask` đăng ký ở
  `Sources\Core\Src\ScriptFuns.cpp:14294` (thân hàm `:2077`, `:2105`), và
  `script\vng_lib\bittask_lib.lua` của JX1 **giống hệt** bản Linux ⇒ chỉ cần chép tệp gọi.
* ⚠️ Bit 25 của task 3071 phải được **ghi vào sổ**: hệ bit rất dễ bị đợt sau cấp chồng bit mà không ai thấy.

Chi tiết đáng lưu ý:

* **`200` (`SetTaskTemp`)** — JX1 dùng **đúng cùng nghĩa** (cờ "đang trong mission, cấm cừu sát") ở
  **23 chỗ** thuộc 7 hệ (bw, citywar_arena, citywar_city, leaguematch, tong×3, tongwar).
  Giữ nguyên số, nhưng **mọi lối ra đều phải hạ cờ về 0**: hiện 7 hệ dùng chung MỘT ô,
  ai quên hạ thì hệ sau đọc nhầm. (Bản Linux `fld_head.lua:133` cũng **đã comment** một nhánh
  `-- SetTaskTemp(200,1);` — dấu hiệu chính bản gốc từng lấn cấn chỗ này; nhánh còn sống là
  `fld_head.lua:117`, chỉ chạy khi `check_new_shuizeitask() == 1` **và** `BOATID ~= 1`.
  Bên Vượt Ải: đặt ở `chuangguang30.lua:85`, `mission_match.lua:68`; hạ ở `chuangguang30.lua:140`,
  `mission_match.lua:43`.)
* **`1550`** — bản Linux dùng ở **hai nơi**: `TSK_REMAIN_COUNT` của Vượt Ải (`include.lua:20`) và
  `SetTask(1550,0)` trong `storm\function.lua:396,417` với chú thích 杀手进行次数. Ban đầu trông
  như trùng, nhưng map Vượt Ải trong `MapList.ini` mang tên GBK **`特殊用地\杀手的试炼`** (= "sát thủ
  đích thí luyện") ⇒ **cùng một tính năng**, không phải xung đột. JX1 đã có y hệt.
* **Trần**: `MAX_TASK = MAX_TEMP_TASK = 4200` (`Sources\Core\Src\KPlayerTask.h:18-19`);
  `KPlayerTask.cpp:69,79` **chặn im lặng** id ≥ 4200 (không sập, chỉ **mất dữ liệu**).
  Id cao nhất 3 tính năng dùng là `4018` ⇒ còn chỗ. Trong dải `41xx` JX1 **đã cấm** `4125`
  (`missions\leaguematch\head.lua:81`), `4126,4127` (`missions\citywar_arena\head.lua:22-23`),
  `4129,4130,4131` (`citywar_arena\head.lua:42`) và `4132…4138` (`lib\lib_task.lua`, `header\taskid.lua`;
  đợt vá trùng task id 24/08). *[đã sửa theo đối chất — bản cũ bỏ sót `4125` và `4129/4130/4131`, và kết luận
  "**dải trống lớn nhất để cấp mới: `4139…4199` (61 ô)**" là **SAI**. Quét lại toàn bộ `0..4199` trên 3.619 tệp
  `.lua` của cây JX1 (ngữ cảnh `Get/SetTask*`, `nTaskID=`, gán hằng số): dải trống lớn nhất là
  **`3601…4016` = 416 ô** (kiểm chứng riêng: 0 hit `(Get|Set)Task*(36xx…4016)` trên cả 3 gốc script), kế đó
  `3081…3398` = 318 ô, `2101…2239` = 139 ô, `1582…1714` = 133 ô, `1275…1399` = 125 ô, `4018…4124` = 107 ô.
  `4139…4199` chỉ 61 ô, không nằm trong top 6. **Nên cấp mới từ `3601…4016`.]*
  **Dải trống lớn nhất để cấp mới: `4139…4199` (61 ô)**.
* ⚠️ `script\event\storm\function.lua:398` có vòng `for i = 1661, 16710 do SetTask(i,0) end` —
  phần `i ≥ 4200` bị chặn im lặng. Vô hại, nhưng cho thấy hệ Phong Bão **giả định không gian task
  rộng hơn `MAX_TASK`**; nếu sau này nâng `MAX_TASK` thì vòng này sẽ quét 16.710 ô mỗi lần gọi.

### 3.2 Mission id / MS id / mission timer / mission value

| Loại | Số | JX1 | Mức |
|---|---|---|---|
| Mission | **15** (Phong Lăng Độ, `fld_head.lua:10`) | `missions.txt` dòng 15 = `mission_trong.lua` (trống) | AN TOÀN |
| Mission | **22** (Vượt Ải, `include.lua:6`) | dòng 22 = `mission_trong.lua` (trống) | AN TOÀN |
| MS id | 15 và 22 (dùng lại chính mission id: `AddMSPlayer(MISSIONID,1)` `fld_head.lua:138`) | `lib_task.lua:282-289` mới cấp `MS_* = 1..6, 11, 12` | AN TOÀN — **nhưng phải khai vào `lib_task.lua`** |
| Mission timer | 41 / 42 / 43 (Vượt Ải), 28 / 29 (Phong Lăng Độ) | Timer id là **nhãn trong khe**, không phải chỉ số mảng (`ScriptFuns.cpp:11382` `m_cTimerTaskSet.Add()` rồi `SetTimer(interval, id)`). Trần là **số lượng**: `MAX_TIMER_PERMISSION = 10` (`KMission.h:23`) | AN TOÀN (3 và 2 timer) |
| Mission value | `VARV_*` cao nhất = 33 (`include.lua:31+`) | `MAX_MISSION_VALUE_COUNT = 100` (`KMission.h:122`), `MAX_MISSIONARRAY_VALUE_COUNT = 100` (`KMissionArray.h:7`) | AN TOÀN |

> 🔴 **Cảnh báo phạm vi ô MissionV** (vòng 1 chưa nêu): `SetMissionV`/`GetMissionV` của JX1 lưu vào
> **`SubWorld[idx].m_MissionArray`** (`ScriptFuns.cpp:11007` và `:11049`) — tức ô là **của từng
> SubWorld**, KHÔNG phải của từng mission. Phong Lăng Độ JX2 dùng `MS_STATE=1 / MS_TIMEACC_1MIN=2 /
> MS_TIMEACC_20SEC=3` trên map 337-339, mà Phong Lăng Độ bản VN cũng sống trên đúng 3 map đó ⇒ nếu
> chạy song song thì **hai mission ghi đè ô của nhau**. (JX1 cũng đặt `MS_STATE = 1` ở
> `missions\bw\bwhead.lua:27`, `citywar_arena\head.lua:15`, `citywar_city\head.lua:31` — nhưng khác map nên hiện không sao.)

### 3.3 Ladder id

| Id | Dùng bởi | JX1 | Mức |
|---|---|---|---|
| `10119` | săn boss sát thủ — `killer\kill_level.lua:44`; Vượt Ải **đọc** để mượn tên NPC ải ẩn (`include.lua:334` ← `npc.lua:125,230`) | không tệp `.lua` nào của JX1 dùng | AN TOÀN |
| `10179` / `10180` | Vượt Ải sơ cấp / cao cấp — `npc_death.lua:65,67` | trống | AN TOÀN |
| `10235` | `DailyRankLadderId` — `rank_perday.lua:9`, ghi ở `npc_death.lua:88` | trống | AN TOÀN |
| `10182` / `10186` | hệ Phong Bão — `event\storm\head.lua:55,59` | JX1 **đã có y hệt** `event\storm\function.lua:240,242,428,430` | TRÙNG NHẸ |

JX1 hiện đã cấp: `10118, 10182, 10186, 10187, 10196, 10197, 10225, 10226, 10227, 10228, 10250,
10251, 10261, 10999` (kho `settings\jx2ladder.txt` mới chỉ vật chất hoá `10118` và `10999`).

Ràng buộc engine phải nhớ: `LuaLadder_NewLadder` **chỉ nhận id kiểu SỐ** và **> 10000**
(`Sources\Core\Src\KJx2SharedStore.cpp:534` `Lua_IsNumber(L,1)`, `:538` `uId <= JX2LADDER_MIN_ID` với
`JX2LADDER_MIN_ID = 10000` ở `:441`), giữ **top 10** (`JX2LADDER_TOP`, `:440`). Cả 4 id trên hợp lệ.

> Vòng 1 (`01_satthu.md:752-759`) nói ladder `10119` "bị 2 hệ ghi chung". Kiểm lại: cả hai chỗ ghi
> đều nằm **bên bản Linux** (`script\global\pgaming\xephang\bangxephang.lua:27` `Ladder_ClearLadder(10119)`
> đúng, `:80` `Ladder_NewLadder(10119, …, Player:GetLevel(), 1)` ghi **cấp nhân vật**). Thư mục
> `global\pgaming\` **KHÔNG tồn tại trong JX1** ⇒ đây là lỗi **có sẵn của bản gốc**, không phải va
> chạm với JX1. Chỉ thành vấn đề nếu đợt sau port luôn `bangxephang.lua`.

### 3.4 Khoá chuỗi (không phải số)

| Khoá | Dùng bởi | JX1 | Mức |
|---|---|---|---|
| `gb_task "challengeoftime_ranklist"` | `rank_perday.lua:11,34-47` | JX1 đang dùng `"WLLS"`, `"TONGWAR_STATION"`, `COLLG_NAME`, `FESTIVAL_SHREWMOUSE`, `"hạt Huy Hoàng"` — không có khoá này | AN TOÀN |
| `RELAYTASK_LEAGUEID = 500` (không gian tên của gb_task) | `script\lib\gb_taskfuncs.lua:11` | JX1 dùng **đúng 500** ở cả 3 bản: `script\gb_taskfuncs.lua:10`, `script\lib\gb_taskfuncs.lua:11`, `scriptjx2\lib\gb_taskfuncs.lua:11` — hai tệp **y hệt nhau từng dòng** | TRÙNG NHẸ |
| 11 khoá `AddStatData(...)` | `fld_death.lua:30,32,34`; `mission.lua:48,50,52`; `fld_head.lua:291`; `shashou_mibao.lua:23`; `lib_killlevel.lua:145` (bảng `tbMaiDian`, 3 khoá); `chuangguanbaoxiang.lua:169`; `talkdailytask.lua:159` | Khoá không đụng gì — **nhưng hàm `AddStatData` KHÔNG tồn tại trong `D:\GAMEDEVNEW\Sources` (0 hit toàn cây)** | AN TOÀN về số / **THIẾU API** |

> 🔴 Bổ sung: **script JX1 hiện tại đã gọi `AddStatData` mà không có hàm** —
> `script\global\特殊用地\宋金报名点\npc\songjin_shophead.lua:183` và
> `script\startgame\tinhnang\tongkim\songjin_shophead.lua:183`. Nhánh đó chưa chạy nên
> `ScriptError.log` chưa ghi (0 hit), nhưng đây là **quả bom hẹn giờ có sẵn**.
> Bên Linux `AddStatData` **là hàm engine thật**: chuỗi có trong `jx_linux_y` và được tham chiếu tại
> `0x082E3B78`. Phải viết hàm (hoặc stub trả 0) trước khi port, nếu không 3 tính năng sẽ chết script
> tại 11 dòng trên.

### 3.5 Skill / camp

* `AddSkillState(541,1,0,54)` (`killer\kill_level.lua:82`) — **AN TOÀN**, xem mục 2.
* `547` / `548` (miễn dịch boss / x2 exp) — JX1 `skills.txt` cột `SkillId` có đủ, cùng nghĩa.
* `SetCurCamp(1)` / `AddMSPlayer(…, camp=1)` — camp 1 là giá trị phe **chung**, không phải định danh
  được cấp phát. **AN TOÀN**.

---

## 4. Việc phải làm, theo thứ tự

1. **Quyết trước khi viết một dòng code**: có gỡ 3 tính năng bản VN của JX1 không?
   (`script\tinhnang\{vuot_ai, phonglangdo, boss_satthu}\`, mission 3 và 4, `MS_VUOTAI=3`,
   `MS_PLANGDO=4`). Nếu **không gỡ** thì phải dời map Vượt Ải sang `490…500` và **không được** mở
   Phong Lăng Độ JX2 trên 337-339.
2. **Gỡ `addnpcphonglangdo()`** ở `script\startgame.lua:100` (đang spawn 6 NPC Thuyền phu mỗi lần
   khởi động, dù lịch đã tắt).
3. **Lập bảng ánh xạ vật phẩm** cho 42 bộ số TRÙNG NẶNG + 24 bộ "JX1 có cùng tên ở số khác"
   — cột `CachXuLy` của `id_dungdo.csv` đã ghi sẵn số đích. 23 bộ còn lại phải bổ sung dòng mới vào
   `settings\item\magicscript.txt`.
4. **Xử lý NPC 1032/1033/1034** — hỏi chủ game: Boss New Dragon có đang sống không?
   Nếu có thì phải dời `tbRangeId[2]`; nếu không thì ghi đè 3 dòng `npcs.txt` (đồng thời sửa luôn
   lỗi có sẵn của `lib_vuotai.lua`).
5. **Task 88**: bỏ hàm `zhuansheng_clear_prop` hoặc dời sang dải `4139…4199`.
6. **Viết `AddStatData`** (hoặc stub) vào `ScriptFuns.cpp` trước khi bật 3 tính năng.
7. **Đăng ký** vào `script\lib\lib_task.lua`: 49 task id AN TOÀN (kể cả `3071 bit 25`) +
   `MS_* = 15, 22` + 4 ladder id (`10119, 10179, 10180, 10235`), để đợt sau không cấp trùng.

---

## 5. Chỗ CHƯA XÁC MINH

* **Bảng rơi đồ** `settings\droprate\boss\bosstask_lev20..90.ini` (8 tệp) chứa bộ số vật phẩm nhưng
  **chưa được đối chiếu từng dòng** — chúng gần chắc chắn dính cùng kiểu lệch id như mục 1.1.
  Cần một đợt quét riêng. *[đã sửa theo đối chất — bản cũ ghi "chỉ có ở `D:\ServerLinux\Patch`": **SAI**.
  Cả 8 tệp có mặt ở **cả `server1` lẫn `Patch`** và `cmp` cho kết quả **byte-identical**; `chi_co_o_patch.txt`
  cũng không liệt kê chúng. `settings\maps\challengeoftime\lineup*.txt` (7 tệp) cũng vậy.]*
* Bảng `settings\maps\challengeoftime\lineup*.txt` (`npc.lua:11-19`) — chưa đối chiếu.
* `killbosshead.lua` (3.421 dòng, bảng 160 boss) mới quét được các id qua `Tab3/Tab4/Tab6`; **chưa
  giải hết bảng** để lấy toàn bộ toạ độ/map từng boss.
* `AddStatData` bên Linux: đã xác minh **có chuỗi + có tham chiếu** tại `0x082E3B78`, nhưng **chưa
  dịch ngược thân hàm** ⇒ chưa biết nó ghi đi đâu (log? Relay?).
* ~~Đã kiểm `scriptjx2\jx2compat.lua.DISABLED` **đang tắt**, nhưng **chưa xác minh** thư mục
  `scriptjx2\tong\` có thật sự được nạp lúc chạy hay không.~~ *[đã sửa theo đối chất — **ĐÃ GIẢI ĐƯỢC**:
  `Sources\Core\Src\KSortScript.cpp:53` nạp `"\script"` và `:65` nạp `"\scriptjx2\tong_vn"` — **chỉ hai
  thư mục đó**. `scriptjx2\tong\` **KHÔNG được nạp** và không tệp `.lua` nào trong cây trỏ tới nó ⇒
  thư mục chết. Ba hàng TRÙNG NHẸ `2361 / 2446 / 2509` **vẫn đúng kết luận** nhưng **sai bằng chứng**:
  phải trích `scriptjx2\tong_vn\tong_header.lua:37` (2361), `:43` (2446) và
  `scriptjx2\tong_vn\tong_award_head.lua:10` (2509) — đó mới là cây được nạp.]*

---

## 6. Tệp sinh ra ở vòng 2

| Tệp | Nội dung |
|---|---|
| `id_dungdo.csv` | **Bảng chính** — 340 dòng, 7 cột (`Loai, So, DungBoi_Linux, JX1_DangDungChoGi, MucDo, CachXuLy, PhamVi`), TRÙNG NẶNG xếp đầu |
| `_r2_lin_hits.json` | 1.137 lượt dùng định danh, quét từ 251 tệp `src_utf8` |
| `_r2_jx1_idx.json` | Chỉ mục 969 khoá định danh của các tệp JX1 (`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script` + `…\bin\server\scriptjx2` + `D:\GAMEDEVNEW\serverscript_jx2`) *[đã sửa theo đối chất — bản cũ ghi "3.780 tệp" và ghi gốc là `bin\server\script`, nhưng `D:\GAMEDEVNEW\bin\server\` **chỉ có** `Coreserver.dll` / `.map` / `release64`, không hề có `script\` hay `scriptjx2\`. Đếm thật trên cây E: **3.619** tệp `.lua`, **3.770** `.lua`+`.txt`, **3.854** mọi tệp — không con số nào bằng 3.780.]* |
| `_r2_items.json` | 97 bộ số vật phẩm × (tên Linux, tên JX1 cùng số, id JX1 cùng tên) |
| `_r2_npc.json` | 140 NPC id × (tên Linux, tên JX1, trạng thái) |

---

## ĐỐI CHẤT (tác tử độc lập)

> Người kiểm chứng **không phải** người viết báo cáo này. Nguyên tắc: **mặc định coi mọi khẳng định là SAI
> cho đến khi tệp gốc / binary chứng minh ngược lại**. Mọi dòng dưới đây đều đã mở tận mắt tệp gốc trên đĩa.
> **Cây JX1 dùng để kiểm** = `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\` (cây máy chủ ĐANG CHẠY),
> vì `D:\GAMEDEVNEW\bin\server\` **không có** thư mục `script\` — xem khẳng định K23.
> Số dòng của bản Linux trích từ `port_3hd\src_utf8\` (giữ nguyên số dòng 1:1 với tệp gốc).

**Đã kiểm 24 khẳng định · SAI 9 · SAI NHẸ 1 · ĐÚNG 14 · bỏ sót tìm thêm 4.**

| # | Khẳng định | Bằng chứng gốc | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| K1 | Task 88 bản Linux = 4 byte Sức/Thân/Ngoại/Nội, `task\metempsychosis\task_func.lua:84-91` | `src_utf8\satthu\task\metempsychosis\task_func.lua:84` `local Utask88 = GetTask(88)`; `:87-90` `AddStrg/AddDex/AddVit/AddEng(... + GetByte(Utask88,1..4))` | **ĐÚNG** | (giữ; chính xác hơn: `:84` + `:87-90`) |
| K2 | JX1 dùng 88 = `T_TimVatPham` (Dã Tẩu), `lib_task.lua:111`, `datau.lua:43,48` | `script\lib\lib_task.lua:111` `T_TimVatPham = 88` (nằm ngay dưới chú thích `-- DA TAU` ở `:109`); `script\global\npcchucnang\datau.lua:43` và `:48` đều `GetTask(T_TimVatPham)` | **ĐÚNG** | — (đây là va chạm **thật**, mức TRÙNG NẶNG là đúng) |
| K3 | `npcs.txt` **không có cột id**, id = số dòng, "NPC id `N` nằm ở dòng `N+1`" | `Sources\Core\Src\KNpcTemplate.cpp:71` `int nNpcTempRow = nNpcTemplateId + 2;`; `KNpc.cpp:5320` `g_NpcSetting.GetString(nNpcSettingIdx + 2, …)`; `Sources\Engine\Src\KTabFile.cpp:210,217` `GetValue(nRow - 1, …)` (đánh số **từ 1**). Header `Name Kind Camp …` ở dòng 1 | **SAI** (quy tắc) | **`N+2`**, không phải `N+1`. Kéo theo: cột số dòng trong `id_dungdo.csv` ghi 1033/1034/1035 là **lệch 1**, đúng phải là **1034/1035/1036** |
| K4 | NPC 1032/1033/1034: Linux "tiểu Boss nam 7 / nam 8 / nữ 1" → JX1 "Boss New Dragon 165 / 166 8 / 167" | Linux `settings\npcs.txt` dòng **1034/1035/1036** = "(cao cấp) tiểu Boss nam 7 / nam 8 / nữ 1"; JX1 `settings\npcs.txt` dòng **1034/1035/1036** = "Boss New Dragon 165 / Boss New Dragon 166 8 / Boss New Dragon 167". Áp `N+2` ⇒ đúng id 1032/1033/1034 | **ĐÚNG** | — (chỉ số dòng dẫn chứng sai, xem K3; **tên và id thì đúng**) |
| K5 | `tbRangeId[2] = {{1026,1033},{1034,1037}}` ở `challengeoftime\include.lua:111-114` | `src_utf8\vuotai\missions\challengeoftime\include.lua:111` `[2] = {`, `:112` `{1026, 1033},`, `:113` `{1034, 1037}`, `:114` `},` | **ĐÚNG** | — |
| K6 | `lib_vuotai.lua:180-182` của JX1 vẫn khai 1032 "Càn Khôn", 1033 "Vi Đà", 1034 " " ⇒ Vượt Ải bản VN **đang sinh Boss New Dragon** | `script\tinhnang\vuot_ai\lib_vuotai.lua:180` `{1032,{1},…,"Càn Khôn"}`, `:181` `{1033,…,"Vi Đà "}`, `:182` `{1034,…," "}` — ghép với K4 ⇒ đúng | **ĐÚNG** | — (lỗi có sẵn, xác nhận) |
| K7 | `MAP_VUOTAI = {480…489}` (`lib_vuotai.lua:33-44`) và `MAP_DUATHUYEN_PLD = {337,338,339}` (`lib_phonglangdo.lua:108-112`) | `lib_vuotai.lua:33` `MAP_VUOTAI = {` … `:44` `}` (đúng 480…489); `lib_phonglangdo.lua:108-112` (337 "Ben thuyen 1", 338, 339) | **ĐÚNG** | — |
| K8 | Lịch tắt (`timerserver.lua:79,80`) nhưng `startgame.lua:100 addnpcphonglangdo()` **KHÔNG tắt** ⇒ 6 NPC Thuyền phu (tpl 240) vẫn spawn | `script\timerserver.lua:79` `-- sukien_vuotai(nHr,nMi)`, `:80` `-- sukien_phonglangdo(nHr,nMi)`; `script\startgame.lua:99` `-- addnpcbosssatthu()` (có `--`), `:100` `addnpcphonglangdo()` (**không** có `--`); `lib_phonglangdo.lua:147-157` = 6 lệnh `AddNpcEx1({240},…)` | **ĐÚNG** | — (đây là việc phải làm số 2, xác nhận) |
| K9 | Đề xuất dời `tbLevelMaps[2]` sang `490…500`, "MapList khai 464→500 đều là `杀手的试炼`, còn dư 5 ô" | `include.lua:97-102` ⇒ `tbLevelMaps[2]` có **16** map (`480…495`), `490…500` chỉ **11** ô. `settings\MapList.ini` có **48** mục `特殊用地\杀手的试炼`, trải `464…511` (**không dừng ở 500**). `Maps\WorldSet_GameServer.ini:474-479` `World469=490 … World474=495` = "Thách thức thời gian (Cao cấp 11…16)" (đã sống); `:480-495` `World475=496 … World490=511` = "Thử luyện tài nghệ (Cao cấp 1…16)" | **SAI** | Khối thay thế đúng là **`496…511`** (16 ô, vừa khít 16 map). `490…500` **không đủ chỗ** và `490…495` đã có tên/đăng ký |
| K10 | 6 cặp vật phẩm nguy hiểm nhất (399/400/906/907/215/2742) và các id "cùng tên ở JX1" | Tra 2 bảng `magicscript.txt` theo bộ `(Genre,DetailType,ParticularType)`: `6,1,399` LIN "Sát Thủ lệnh" / JX1 "Sát thủ giản" (JX1 "Sát Thủ lệnh" ở `6,1,398`); `400` LIN "Sát thủ giản" / JX1 "Sư đồ thiếp"; `906` LIN "Quả Huy Hoàng (cao)" / JX1 "(trung)" (JX1 (cao) ở `907` và `3440`); `907` LIN "Quả Hoàng Kim" / JX1 "Quả Huy Hoàng (cao)" (JX1 "Quả Hoàng Kim" ở `908`); `215` LIN "Càn Khôn Tạo Hóa Đan (đại)" / JX1 "(trung)" (JX1 (đại) ở `214`); `2742` LIN "Bảo Rương Vượt ải" / JX1 "Bảo Rương Tử Mãng Khí Giới" (JX1 "Bảo Rương Vượt ải" ở `3360`) | **ĐÚNG (6/6)** | — |
| K11 | "Không có quy luật dịch số cố định": `2347→2356` (+9), `2743→3361` (+618), `3810→4428` (+618); `68/69/72/73/74` trùng khít; `71` cùng vật khác tên | Tra bảng: JX1 `6,1,2356`="Sát Thủ Bí Bảo"(=LIN 2347) ✔; JX1 `6,1,3361`="Bảo Rương Thủy Tặc"(=LIN 2743) ✔; JX1 `6,1,4428`="Tinh Thiết Khoáng"(=LIN 3810) ✔; 68/69/72/73/74 tên khớp hai bên ✔; LIN `71`="Tiên Thảo Lộ" / JX1 `71`="Tiên Thảo Lộ Thường", JX1 chèn `70`="Tiên Thảo Lộ Trung" ✔. Kiểm thêm 4 đích của CSV: `1075→1076`, `1094→1095`, `1392→1401`, `1672→1681`, `30228→4846` ("Chân Nguyên Đơn (Trung)") — **đều đúng** | **ĐÚNG** | — (lưu ý: bản tóm tắt gửi điều phối viết "`6,1,68/69/**71**/72/73/74` trùng khít"; thân báo cáo ghi đúng là 71 **khác tên**) |
| K12 | Bảng vật phẩm Linux nằm ở `settings\item\**004**\magicscript.txt` | `D:\ServerLinux\{server1,Patch}\settings\item\` **không có** `magicscript.txt` ở gốc (chỉ `magicattrib.txt`, `platina_*`); tệp thật ở `…\item\004\magicscript.txt` (1.151.203 B) | **ĐÚNG** | — (bổ sung: tệp có ở **cả** `server1` lẫn `Patch`, không phải chỉ Patch) |
| K13 | `add_shashouling()` phát `6,1,399` **8 lần** ở `lib_killlevel.lua:73,77,81,85,89,93,97,103` | `src_utf8\satthu\task\tollgate\killer\lib_killlevel.lua` — `AddItem(6,1,399,…)` đúng 8 lần, đúng 8 số dòng đó | **ĐÚNG** | — |
| K14 | JX1 phát `{6,1,398}` ở `vuot_ai\sugiasatthu.lua:22-23` và `boss_satthu\drop.lua:49` | `sugiasatthu.lua:22` `ITEM_STL = {6,1,398}`, `:23` `ITEM_STG = {6,1,399}`; `drop.lua:49` `DropItem(nNpcIndex, 6, 1, 398, 10, random(0,4), 0) --sat thu lenh` | **ĐÚNG** | — |
| K15 | Vòng 1 ghi `200` → `hoatdong_admin.lua:125`; vòng 2 "sửa" thành "dòng 125 là chuỗi `\"Quay lại/HD_AdminMenu\"`, lệnh ở **dòng 134**" | `script\item\hoatdong_admin.lua:125` = `"Gỡ kẹt trận treo của TÔI (task 200 va 2340-2342)/HD_BW_GoKet",`; `:126` = `"Quay lại/HD_AdminMenu"})`; `:134` = `function HD_BW_GoKet()`; `:135` = `SetTaskTemp(200, 0)` | **SAI** (bản sửa của vòng 2 sai 2 chi tiết) | Dòng 125 là chuỗi **"Gỡ kẹt trận treo…"** (có chữ "200" ⇒ lọt lưới grep của vòng 1); lệnh thật ở **dòng 135** |
| K16 | `200` được JX1 dùng ở **23 chỗ**, và vòng 1 thiếu 15 vị trí | `grep -rnE "(Set\|Get)TaskTemp\s*\(\s*200\b"` trên `bin\server\script`, bỏ `.truoc_*` và dòng chú thích ⇒ **đúng 23**; 15 vị trí bổ sung mà vòng 2 liệt kê **khớp 15/15** | **ĐÚNG** | — |
| K17 | skills.txt JX1 "lệch một dòng"; "dòng 541 của JX1 là `SkillId=542` 'Tín sứ lệnh ở mộc'"; "JX1 **đang gọi y hệt** ở `boss_satthu\death.lua:22`" | `settings\skills.txt` dòng 1 = tiêu đề `SkillName Property SkillId …`; **dòng 541 → `SkillId`=540**; **dòng 542 → `SkillId`=541** "Hoàn thành nhiệm vụ Sát thủ"; **dòng 543 → `SkillId`=542** "Tín sứ lệnh ở mộc"; dòng 548 → 547 "Sát thủ - Vòng tròn miễn dịch"; dòng 549 → 548 "Vòng tròn tăng đôi kinh nghiệm". Linux `kill_level.lua:82` = `AddSkillState(541,1,0,54)`; JX1 `death.lua:22` = `AddSkillState(541,1,0,108*2,-1)` | **SAI** (2 chi tiết) — kết luận lớn "tra theo cột SkillId, có đủ 3" thì **ĐÚNG** | Quy tắc: **dòng `L` mang `SkillId = L−1`**; 541 ở dòng 542, 542 ở dòng 543. Và **không** "y hệt": khác số tham số (4 vs 5) và **thời lượng 54 vs 216** |
| K18 | `SetMissionV`/`GetMissionV` lưu vào `SubWorld[idx].m_MissionArray` (`ScriptFuns.cpp:11007`, `:11049`) ⇒ ô **của từng SubWorld**, không phải từng mission | `ScriptFuns.cpp:15231` `{"SetMissionV", LuaSetMission}` → thân ở `:10991`, ghi tại `:11007` `SubWorld[nSubWorldIndex].m_MissionArray.SetMission(nValueId, szValue)`; `:14739` `{"GetMissionV", LuaGetMissionValue}` → thân `:11035`, đọc tại `:11049` `…m_MissionArray.GetMissionValue(nValueId)`. Bằng chứng quyết định: **`KMissionArray.h:13,14`** — chính **MẢNG** giữ `m_MissionValueC[100]` / `m_MissionValue[100][16]`, không phải từng `KMission` | **ĐÚNG** | — (đây là phát hiện có giá trị nhất của mục 3.2, xác nhận vững) |
| K19 | Trần engine: `MAX_TIMER_PERMISSION=10` (`KMission.h:23`), `MAX_MISSION_VALUE_COUNT=100` (`KMission.h:122`), `MAX_MISSIONARRAY_VALUE_COUNT=100` (`KMissionArray.h:7`), timer thêm bằng `m_cTimerTaskSet.Add()` (`ScriptFuns.cpp:11382`), `MAX_TASK=MAX_TEMP_TASK=4200` (`KPlayerTask.h:18-19`), chặn im lặng (`KPlayerTask.cpp:69,79`) | Mở từng tệp: **6/6 số dòng khớp chính xác** | **ĐÚNG** | — |
| K20 | "**Dải trống lớn nhất để cấp mới: `4139…4199` (61 ô)**"; JX1 đã cấm `4126,4127` và `4132…4138` | Quét 3.619 tệp `.lua` (3 gốc script) theo ngữ cảnh task: trong `41xx` JX1 dùng **4125** (`leaguematch\head.lua:81`), 4126, 4127, **4129, 4130, 4131** (`citywar_arena\head.lua:42`), 4132…4138. Dải trống lớn nhất trong `0..4199` là **`3601…4016` = 416 ô** (kiểm chứng riêng: `grep -E "(Get\|Set)Task[A-Za-z]*\(\s*(3[6-9]\d\d\|400\d\|401[0-6])"` trên cả 3 gốc = **0 hit**), rồi `3081…3398` (318), `2101…2239` (139), `1582…1714` (133), `1275…1399` (125), `4018…4124` (107) | **SAI** | Dải trống lớn nhất là **`3601…4016` (416 ô)**; `4139…4199` (61 ô) không lọt top 6. Danh sách id đã cấm còn **thiếu 4125, 4129, 4130, 4131** |
| K21 | `AddStatData` **không tồn tại** trong `D:\GAMEDEVNEW\Sources` (0 hit) nhưng JX1 đã lỡ gọi ở `songjin_shophead.lua:183`; bên Linux là hàm engine thật, tham chiếu `0x082E3B78` | `grep -rl AddStatData D:\GAMEDEVNEW\Sources` ⇒ **0 tệp**. Lệnh gọi: `script\global\特殊用地\宋金报名点\npc\songjin_shophead.lua:183`, `script\startgame\tinhnang\tongkim\songjin_shophead.lua:183` (**và thêm 2 tệp nữa** trong `serverscript_jx2\jx1_edits\songjin\`). ELF: chuỗi `AddStatData` ở `0x0825CECC`, **được tham chiếu tại `0x082E3B78`** ✔ | **ĐÚNG** | Bổ sung: có **4** tệp gọi, không phải 2 (thêm `serverscript_jx2\jx1_edits\songjin\songjin_shophead_baodiem.lua:183` và `…_tongkim.lua:183`) |
| K22 | 3 hàng TRÙNG NHẸ `2361/2446/2509` "dựa vào giả định `scriptjx2\tong\` **có** được nạp" (để CHƯA XÁC MINH) | `Sources\Core\Src\KSortScript.cpp` — `g_IniScriptEngine()` chỉ gọi `:53` `LoadAllScript("\\script")` và `:65` `LoadAllScript("\\scriptjx2\\tong_vn")`. Không tệp `.lua` nào trỏ tới `\scriptjx2\tong\`. Nhưng **cùng ba hằng số cũng có ở cây được nạp**: `scriptjx2\tong_vn\tong_header.lua:37` (2361), `:43` (2446), `scriptjx2\tong_vn\tong_award_head.lua:10` (2509) | **SAI bằng chứng / ĐÚNG kết luận** — và **giải được** chỗ CHƯA XÁC MINH | `scriptjx2\tong\` **KHÔNG** được nạp (thư mục chết). TRÙNG NHẸ vẫn đúng, nhưng phải trích `tong_vn\` |
| K23 | `_r2_jx1_idx.json` = chỉ mục **3.780 tệp** JX1 (`bin\server\script` + `bin\server\scriptjx2` + `D:\GAMEDEVNEW\serverscript_jx2`) | `D:\GAMEDEVNEW\bin\server\` chỉ chứa `Coreserver.dll`, `Coreserver.map`, `release64` — **không có** `script\` lẫn `scriptjx2\`. Cây thật: `E:\…\TESTLOFFF_ONLINE\bin\server\script` (2.870 tệp) + `scriptjx2` (196) + `D:\GAMEDEVNEW\serverscript_jx2` (788). Đếm: **3.619** `.lua`, **3.770** `.lua`+`.txt`, **3.854** mọi tệp | **SAI** | Ghi rõ gốc `E:\SourceTuanLe\…`; con số **3.780 không khớp** cách đếm nào |
| K24 | `settings\droprate\boss\bosstask_lev20..90.ini` (8 tệp) **chỉ có ở** `D:\ServerLinux\Patch` | `cmp` từng cặp: cả 8 tệp có ở **cả `server1` lẫn `Patch`** và **byte-identical**; `chi_co_o_patch.txt` **không** liệt kê chúng. `maps\challengeoftime\lineup*.txt` (7 tệp) cũng có ở cả hai và identical | **SAI** | Bỏ chữ "chỉ có ở Patch". Việc "chưa đối chiếu từng dòng" thì vẫn đúng |

**Các khẳng định khác đã kiểm và ĐÚNG (không lập hàng riêng):**
`missions.txt` dòng 16 = `15 → mission_trong.lua` và dòng 23 = `22 → mission_trong.lua` (mission 15/22 trống) ✔ ·
`rank_perday.lua:4-7` = 2636-2639, `:9` = `DailyRankLadderId 10235`, `:11` = `"challengeoftime_ranklist"` ✔ ·
`fld_head.lua:10` `MISSIONID = 15`, `:117` `SetTaskTemp(200,1)` (trong `if check_new_shuizeitask()==1` + `if BOATID ~= 1`), `:121-122` `CheckTaskDaily(3070,5,…)`/`AddTaskDaily(3070,1)`, `:133` `-- SetTaskTemp(200,1);`, `:138` `AddMSPlayer(MISSIONID,1)` ✔ ·
`dragonboat_main.lua:30` `getBitTask{nTaskID=3071, nStartBit=25, nBitCount=1}`, `:166-167` `AddTaskDaily(3079,1)`/`GetTaskDailyCount(3079)` ✔ ·
`bigboss.lua:11` `BigBoss.TSK_BIGBOSS_REWARD = 2661` ✔ ·
`storm\function.lua:396` `SetTask(1550,0)`, `:398` `for i = 1661, 16710 do`, `:417` `SetTask(1550,0)` ✔ ·
`lib_task.lua:282-289` `MS_* = 1..6, 11, 12`, `MS_VUOTAI=3` ở `:284`, `MS_PLANGDO=4` ở `:285` ✔ ·
`MS_STATE = 1` ở `bw\bwhead.lua:27`, `citywar_arena\head.lua:15`, `citywar_city\head.lua:31` ✔ ·
`lib_boss_st.lua:7-29` = 20 boss trên map 321/225/93/75/340 ✔ ·
Ladder: `KJx2SharedStore.cpp:440` `JX2LADDER_TOP 10`, `:441` `JX2LADDER_MIN_ID 10000`, `:534` `Lua_IsNumber(L,1)`, `:538` `uId <= JX2LADDER_MIN_ID` ✔; JX1 không dùng `10119` (mọi hit là chuỗi con của toạ độ 6 chữ số trong `startgame\npcpos.lua`) ✔; `bangxephang.lua:27` `Ladder_ClearLadder(10119)` và `:80` `Ladder_NewLadder(10119, Player:GetName(), Player:GetLevel(), 1)` **đều bên Linux**, `script\global\pgaming\` **không tồn tại** trong JX1 ✔ ·
`RELAYTASK_LEAGUEID = 500` ở `script\gb_taskfuncs.lua:10`, `script\lib\gb_taskfuncs.lua:11`, `scriptjx2\lib\gb_taskfuncs.lua:11` ✔ ·
`GetBitTask`/`SetBitTask` đăng ký ở `ScriptFuns.cpp:14294-14295` ✔ (thân hàm ở `:2078`/`:2106`, `:2077`/`:2105` là dòng chú thích) ·
`ScriptFuns.cpp:12813` `g_SkillManager.GetSkill(nSkillId, nSkillLevel)` ✔ ·
Số học của bảng: `id_dungdo.csv` = **340** hàng, TRÙNG NẶNG **48** = 42 ITEM + 3 NPC_TPL + 2 MAP + 1 TASK ✔; ITEM 97 = 42 + 20 + 35 ✔; TASK 76 + TASK_TEMP 2 + TASK(bit) 1 = 79 ✔.

---

### Bỏ sót của chính vòng 2

**B1. Phong Lăng Độ ĐẶT cờ `TaskTemp 200` mà HAI đường HẠ cờ của chính nó đều bị comment.**
Vòng 2 chỉ nêu `fld_head.lua:133` (một lệnh **ĐẶT** bị comment). Quét lại toàn bộ bao đóng:

```
src_utf8\phonglangdo\missions\fengling_ferry\fld_head.lua:117   SetTaskTemp(200,1);      <- ĐẶT (còn sống)
src_utf8\phonglangdo\missions\fengling_ferry\fld_head.lua:133 --	SetTaskTemp(200,1);    <- ĐẶT (đã comment)
src_utf8\phonglangdo\missions\fengling_ferry\mission.lua:71   --	SetTaskTemp(200,0);    <- HẠ  (ĐÃ COMMENT)
src_utf8\phonglangdo\missions\fengling_ferry\mission.lua:116  --		SetTaskTemp(200, 0)  <- HẠ  (ĐÃ COMMENT)
src_utf8\phonglangdo\battles\battlehead.lua:960                 SetTaskTemp(200,0);      <- HẠ (đường thoát CHUNG của battle)
```

⇒ Trong thư mục `fengling_ferry\` **không còn một đường hạ cờ nào sống**. Cờ chỉ được hạ nhờ
`battlehead.lua:960` — một lối thoát dùng chung, không thuộc Phong Lăng Độ. Đây **chính là** kịch bản
"ai quên hạ thì hệ sau đọc nhầm" mà mục 3.1 cảnh báo trên lý thuyết, nhưng nó **đang có thật ngay trong
tính năng sắp port**, và JX1 dùng chung ô 200 cho **7 hệ**. Phải bổ sung lệnh hạ cờ ở mọi lối rời bến thuyền
(logout / hết giờ / chết / rời map) trước khi bật.
(So sánh: Vượt Ải **có đủ cặp** — đặt `chuangguang30.lua:85`, `mission_match.lua:68`; hạ `chuangguang30.lua:140`,
`mission_match.lua:43` — vòng 2 nêu đúng phần này.)

**B2. Thân hàm `AddStatData` bên Linux KHÔNG cần để "CHƯA XÁC MINH" — địa chỉ đã nằm sẵn trong luamap.**
`jx_linux_y.luamap.full.txt` có dòng `0x080FF550  AddStatData`. Dịch ngược (`re_disasm.py … 0x080FF550 60`)
cho thấy đây là **hàm thật, không phải stub**:

* Symbol C++ đọc được trong nhánh lỗi: `int LuaAddStatData(lua_State*)` (chuỗi tại `0x0825D750`), tham số
  tên `pcszName` (`0x082594E6`), khuôn lỗi `KGLOG_PROCESS_ERROR(%s) at line %d in %s` (`0x0825035C`), dòng **24089**.
* `0x080FF562` gọi `0x8232490` (đếm tham số) rồi `lea eax,[eax-1]; cmp eax,1; jbe` ⇒ **chỉ nhận 1 hoặc 2 tham số**.
* `0x080FF58B` gọi `0x8233850` (lấy chuỗi, tham số 1). Nếu `argc == 2` thì `0x080FF5D6` gọi `0x82338B0`
  (lấy số, tham số 2) và ép về nguyên qua `fistp qword` → lấy 32 bit thấp.
* `0x080FF5B3` gọi `0x081D0420` với `(this = 0x0978C0A0, pcszName, nValue, 0)` — tức đẩy vào **một singleton
  thống kê**, không phải Lua stack.
* `0x081D0444` gọi `strlen` rồi `add eax,1; cmp eax,0x40; ja <lỗi>` ⇒ **tên khoá tối đa 63 ký tự**.
* **Mọi nhánh đều kết bằng `xor eax,eax; ret`** ⇒ hàm **không trả giá trị nào** cho Lua.

⇒ Kết luận thi công (mạnh hơn mục 3.4): viết **stub trả 0** vào `ScriptFuns.cpp` là **an toàn tuyệt đối** —
không script nào đọc giá trị trả về. Chữ ký cần đăng ký: `AddStatData(szKey [, nValue])`, `szKey` ≤ 63 ký tự.

**B3. Map 957 của `chuangguang30` KHÔNG tồn tại lúc chạy — đây là chặn tiến độ thật, nhưng bị xếp TRÙNG NHẸ.**
`challengeoftime\include.lua:28` `CHUANGGUAN30_MAP_ID = 957`. Bên JX1:

* `settings\MapList.ini:6233-6236` **có** khai `957=…`, `957_name`, `957_NewWorldScript`, `957_NewWorldParam`;
* nhưng `Maps\WorldSet.ini` và `Maps\WorldSet_GameServer.ini` (`[Init] Count=910`) **không có một dòng nào chứa 957**.

⇒ GameServer không dựng SubWorld cho map 957 ⇒ `chuangguang30` (ải 30, nhánh CHUANGGUAN30) **không có map để vào**.
Hàng `MAP | 957` trong `id_dungdo.csv` bị gán **TRÙNG NHẸ** và thân báo cáo **không hề nhắc**, trong khi đây là
việc-phải-làm ngang hàng với `AddStatData`. Phải thêm 957 vào `WorldSet_GameServer.ini` (và tăng `Count`) trước khi bật.

**B4. Có sẵn một ca `task id ≥ MAX_TASK` bị nuốt im lặng NGAY TRONG cây được nạp — vòng 2 chỉ cảnh báo vòng lặp storm (vô hại).**
`scriptjx2\tong_vn\tong.lua:866` `local a, b = GetTask(5877), GetCurServerTime()` và `:874` `SetTask(5877, b)`.
`5877 ≥ MAX_TASK (4200)` ⇒ `KPlayerTask.cpp:69` trả 0 và `:79` return ⇒ **`GetTask` luôn trả 0, `SetTask` không ghi gì**,
không sập, không log. Và `scriptjx2\tong_vn\` **là thư mục ĐƯỢC NẠP** (`KSortScript.cpp:65`) — khác hẳn
`scriptjx2\tong\` (chết, xem K22). Đây là lỗi mất dữ liệu **đang sống** của bản VN, cùng họ với bài học
"trùng task id 24/08", và nằm ngay trong phạm vi quét của vòng 2 (`_r2_jx1_idx.json` có phủ `scriptjx2`).
