# A4 — SOÁT ÁNH XẠ 42 ID VẬT PHẨM (106 chỗ đã sửa)

Phiên soát: 25/08. Chỉ ĐỌC, không sửa gì.
Nguồn đối chiếu:
* Bảng vật phẩm Linux: `D:\ServerLinux\Patch\settings\item\004\magicscript.txt`
  (đã kiểm: **giống hệt byte** với `D:\ServerLinux\server1\settings\item\004\magicscript.txt`, md5 `bed0c18f…`).
* Bảng vật phẩm JX1: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt`
  (JX1 dùng bảng **phẳng**, không có thư mục ngôn ngữ `004\` ⇒ nguồn đối chiếu đúng).
* Cột dùng: Linux `[0]名称 [1]ItemGenre [2]DetailType [3]ParticularType [13]脚本名 [12]是否叠放 [17]参数4(有cấp độ)`;
  JX1 `[0]Name [1]Genre [2]DetailType [3]ParticularType [9]Script [12]nMaxStack`.
  (Đã xác minh cột [9] của JX1 đúng là Script bằng các mục có tệp thật, ví dụ `6,1,3430 Tín Sứ Bảo Rương → \script\item\xinshirenwu\xinshibaoxiang.lua`.)

**KẾT QUẢ TỔNG: 26/42 bộ có vấn đề** — 4 CHẶN, 18 NẶNG, 4 NHẸ/CHƯA XÁC MINH.
Ngoài 42 bộ còn **4 lỗ hổng ánh xạ theo lớp** (mục 3) mà đợt vá bỏ sót hoàn toàn.

Tin tốt: **KHÔNG có đường sai (false positive)**. Đã diff từng byte 101 tệp Linux gốc ↔ bản
triển khai JX1: đúng **118 dòng khác** (106 chỗ đổi số vật phẩm + 11 `AddNpc→AddNpcEx` +
1 dòng bỏ `Include`), và **cả 106 chỗ đều là bộ ba vật phẩm thật** (`tbProp` / `AddItem` /
`DropItem` / `CalcItemCount` / `ConsumeItem` / khoá `["g,d,p"]` của bảng cấm). Kiểm riêng 3 tệp
được nhắc tên: `bigboss.lua` (19 chỗ), `lenhbai_def.lua` (24 chỗ), `vngforbidspecialitem.lua`
(6 chỗ) — không chỗ nào là toạ độ / tham số hàm / số của bảng khác.

---

## 1. CHẶN

### CHẶN-1 — Hai NPC 769 “Nhiếp Thí Trần” **chồng đúng toạ độ** ở 7 thành, hai nền kinh tế Sát Thủ lệnh **khác cấp độ, không dùng chung được**

JX1 **đã có sẵn** hệ Săn boss Sát Thủ / Vượt Ải từ 2021–2025 (`script\tinhnang\boss_satthu\`,
`script\tinhnang\vuot_ai\`, `script\tinhnang\phonglangdo\` — mtime 2021…2025-12), và hệ đó
**vẫn đang chạy**: `script\startgame.lua:202  addnpcsatthu()` chưa bị tắt (chỉ
`addnpcbosssatthu()` dòng 101 và `addnpcphonglangdo()` dòng 102 bị comment).

NPC Nhiếp Thí Trần bản JX1 (id 769) được đặt ở `script\startgame\thanh\*.lua`, gắn script
`OTHER_SGSTHU = "\script\tinhnang\vuot_ai\sugiasatthu.lua"` (`script\lib\lib_map.lua:8`):

| thành | tệp JX1 : dòng | toạ độ JX1 | toạ độ bản port (`autoexec_npc_hd3.lua`) |
|---|---|---|---|
| Thành Đô (11) | `startgame\thanh\thanhdo.lua:27` | 3210, 4974 | dòng 8 — **trùng khít** |
| Phượng Tường (1) | `startgame\thanh\phuongtuong.lua:22` | 1506, 3198 | dòng 9 — **trùng khít** |
| Biện Kinh (37) | `startgame\thanh\bienkinh.lua:24` | 1647, 3050 | dòng 10 — **trùng khít** |
| Lâm An (176) | `startgame\thanh\laman.lua:30` | 1372, 3010 | dòng 11 — **trùng khít** |
| Đại Lý (162) | `startgame\thanh\daily.lua:18` | 1573, 3228 | dòng 12 — 1573,3227 (lệch 1 ô) |
| Tương Dương (78) | `startgame\thanh\tuongduong.lua:31` | 1511, 3204 | dòng 13 — 1512,3206 |
| Dương Châu (80) | `startgame\thanh\duongchau.lua:31` | 1700, 2963 | dòng 14 — **trùng khít** |

Hai NPC cùng id 769, **cùng tên** “Nhiếp Thí Trần”, đứng chồng nhau. Người chơi bấm trúng con
nào là ngẫu nhiên. Đây đúng là cái bẫy đã ghi trong ký ức dự án (NPC trùng tên → chọn nhầm).

Nghiêm trọng hơn là **cấp độ vật phẩm không tương thích**:

| | hệ JX1 sẵn có | hệ vừa port |
|---|---|---|
| phát Sát Thủ lệnh | `tinhnang\boss_satthu\drop.lua:49` `DropItem(…,6,1,398,**10**,random(0,4),0)`; `tinhnang\vuot_ai\drop.lua:49` cùng vậy | `task\tollgate\killer\lib_killlevel.lua:73,77,81,85,89,93,97,103` `AddItem(6,1,398,**20/30/40/50/60/70/80/90**,series,0)` |
| ghép giản | `npcchucnang\nhieptran.lua:144,153,159` — bắt buộc **cấp 10** và **đủ 5 ngũ hành khác nhau** | `killer\nieshichen.lua:160-198` — 5 lệnh **cùng cấp bất kỳ**, ngũ hành tuỳ ý |
| vé vào Vượt Ải | `tinhnang\vuot_ai\sugiasatthu.lua:243,304,323,407` — giản **cấp 10** | `challengeoftime\npc\dragonboat_main.lua:29,59,65,136,142` — giản **cấp 20…80** (sơ cấp) hoặc **đúng 90** (cao cấp) |

Hệ quả cụ thể: Sát Thủ lệnh rơi từ boss JX1 (cấp 10) **không ghép được** bằng NPC bản port thì
được (cùng cấp 10 ⇒ ra giản cấp 10), nhưng giản cấp 10 đó **bị Vượt Ải bản port từ chối**
(`dragonboat_main.lua:58` quét `for i=20,80,10`). Ngược lại lệnh/giản bản port (cấp 20…90)
**không bao giờ** vào được NPC/Vượt Ải bản JX1 (chỉ nhận cấp 10). Hai vòng kinh tế cắt rời nhau.

Ghi chú kèm: `script\item\hd3_admin.lua:30` phát `AddItem(6,1,398,90,**0**,0)` × 5 — cả 5 cùng
ngũ hành 0 (Kim). Chỉ chạy được với NPC bản port; NPC JX1 sẽ báo “đặt vào 5 sát thủ lệnh mỗi hệ
một cái” và từ chối.

### CHẶN-2 — Hai phần thưởng lõi (`Bảo Rương Vượt ải`, `Sát Thủ Bí Bảo`) phát ra được nhưng **không mở được**: bảng vật phẩm JX1 trỏ tới tệp script **không tồn tại**

| vật phẩm JX1 | Script trong `settings\item\magicscript.txt` | tệp có thật? | ai phát ra |
|---|---|---|---|
| `6,1,3360` Bảo Rương Vượt ải | `\script\item\baoruongvuotai.lua` | **KHÔNG CÓ** | `missions\challengeoftime\award.lua:90` (bản port) |
| `6,1,2356` Sát Thủ Bí Bảo | `\script\item\satthubibao.lua` | **KHÔNG CÓ** | `task\tollgate\killer\lib_killlevel.lua:105` (bản port) |

Đợt port **đã chép sẵn** hai tệp mở rương tương ứng và đã ánh xạ số đúng trong đó
(`missions\challengeoftime\item\chuangguanbaoxiang.lua`, `task\tollgate\killer\shashou_mibao.lua`)
nhưng **không đấu dây**: bảng vật phẩm vẫn trỏ đi chỗ khác ⇒ hai tệp đó là mã chết.

Bối cảnh trung thực: hai tệp thiếu này **có trước** đợt port (JX1 có 52/275 mục script thiếu tệp,
đa số là đồ mùa vụ đã ngừng). Nhưng đợt port vừa **hồi sinh nguồn phát** cho chúng, nên lỗ hổng
này từ “ngủ” thành “sống”. **CHƯA XÁC MINH** engine xử lý ra sao khi script vật phẩm thiếu tệp
(im lặng hay lỗi Lua).

---

## 2. NẶNG

### NANG-1 — **Va chạm chuỗi** trong `b2_patch.py`: 2 bộ bị ánh xạ HAI LẦN ⇒ phát nhầm vật phẩm

`thicong\b2_patch.py:83-92` chạy 42 luật **tuần tự** trên cùng một chuỗi. Khi đích của luật A lại
là nguồn của luật B **đứng sau**, kết quả của A bị B nuốt tiếp:

| luật A | tạo ra | luật B (chạy sau) | kết quả thật | đúng phải là |
|---|---|---|---|---|
| `6,1,2006 → 6,1,2015` (dòng 8 json) | `6,1,2015` | `6,1,2015 → 6,1,2024` (dòng 9) | **`6,1,2024`** | `6,1,2015` |
| `6,1,2117 → 6,1,2126` (dòng 12) | `6,1,2126` | `6,1,2126 → 6,1,2135` (dòng 13) | **`6,1,2135`** | `6,1,2126` |

Đã đối chiếu byte thật trên cây sống:

```
script\task\tollgate\killer\mibao_head.lua:18
  Linux: {szName = "Khiêu chiến lễ bao", tbProp = {6,1,2006,1,1,0}, nRate = 20},--ok
  JX1  : {szName = "Khiêu chiến lễ bao", tbProp = {6,1,2024,1,1,0}, nRate = 20},--ok
        6,1,2024 ở JX1 = "Truy công lệnh"  (đúng phải là 6,1,2015 = "Khiêu chiến Lễ bao")

script\missions\boss\bigboss.lua:52
  Linux: {tbProp = {6,1,2117,1,0,0}, nCount=10},
  JX1  : {tbProp = {6,1,2135,1,0,0}, nCount=10},
        6,1,2135 ở JX1 = "Nhất Kỷ Càn Khôn Phù"  (đúng phải là 6,1,2126 = "Long Huyết Hoàn")
```

Bằng chứng độc lập rằng 2126 mới là Long Huyết Hoàn: chính JX1 đang dùng
`script\event\20thang10\eventlib2010.lua:79` và `script\event\eventnoel\eventlibnoel.lua:79`
= `{6,1,2126},--long huyet hoan`.

Hậu quả kép ở `bigboss.lua`: dòng 39 (2126→2135) và dòng 52 (2117→2135) **ra cùng một vật phẩm**,
còn “Long Huyết Hoàn” biến mất khỏi bảng rơi; “Khiêu chiến Lễ bao” của Sát Thủ mật bảo bị thay
bằng “Truy công lệnh” (vật phẩm giao nhiệm vụ của Phong Lăng Độ) ⇒ **rò rỉ vé PLD**.

Cặp `6,1,400 → 6,1,399` / `6,1,399 → 6,1,398` cũng có va chạm cùng kiểu nhưng **thoát nạn nhờ
thứ tự**: luật `399→398` (dòng 38 json) chạy trước `400→399` (dòng 39). Đã diff xác nhận đúng.
Đây là an toàn **do may**, không do thiết kế — nếu sắp lại thứ tự bảng là hỏng.

Cách vá đúng: thay vòng lặp tuần tự bằng **một lượt duy nhất** (`re.sub` với hàm tra bảng), hoặc
thay `6,1,2006→6,1,2015` và `6,1,2117→6,1,2126` bằng một mốc trung gian.

### NANG-2 — `genre 6 / detail 0` **KHÔNG TỒN TẠI Ở JX1**, đợt vá bỏ sót toàn bộ (23 chỗ, trong đó **10 chỗ phát thưởng thuộc cả 3 hoạt động**)

Đếm thật: bảng JX1 có **0** mục `Genre=6, DetailType=0`; bảng Linux có **33** mục. Tên các mục
`6,0,N` của Linux **trùng khít** với `6,1,N` của JX1 ⇒ ánh xạ đúng là **đổi DetailType 0 → 1**,
giữ nguyên particular. Đợt vá chỉ sinh mẫu regex theo đúng genre/detail của nguồn nên hoàn toàn
không chạm tới lớp này.

Chỗ **phát thưởng** (đang phát vật phẩm không tồn tại):

| tệp : dòng | hiện tại | phải là |
|---|---|---|
| `script\missions\boss\bigboss.lua:54` | `{tbProp = {6,0,3,1,0,0}, nCount=20}` | `6,1,3` |
| `script\missions\boss\bigboss.lua:53` | `{tbProp = {6,0,6,1,0,0}, nCount=20}` | `6,1,6` |
| `script\task\tollgate\killer\kill_level.lua:105` | `6,0,3` Đại Lực hoàn | `6,1,3` |
| `script\task\tollgate\killer\kill_level.lua:106` | `6,0,6` Phi Tốc hoàn | `6,1,6` |
| `script\task\tollgate\killer\mibao_head.lua:16` | `6,0,3` | `6,1,3` |
| `script\task\tollgate\killer\mibao_head.lua:17` | `6,0,6` | `6,1,6` |
| `script\missions\challengeoftime\chuangguang30.lua:45` | `6,0,3` | `6,1,3` |
| `script\missions\challengeoftime\chuangguang30.lua:46` | `6,0,6` | `6,1,6` |
| `script\missions\yandibaozang\head.lua:166` | `6,0,3` | `6,1,3` |
| `script\missions\yandibaozang\head.lua:167` | `6,0,6` | `6,1,6` |

Chỗ **bảng cấm** (đang cấm vật phẩm không tồn tại ⇒ luật cấm vô hiệu):
`script\vng_feature\forbiditem\vngforbidspecialitem.lua:76,77,78,79,80,81,82,83,84,85,86,87,88`
(`6,0,1 6,0,2 6,0,3 6,0,4 6,0,5 6,0,6 6,0,7 6,0,8 6,0,9 6,0,10 6,0,60 6,0,61 6,0,62`
→ lần lượt `6,1,1 … 6,1,10`, `6,1,60 6,1,61 6,1,62`; đã kiểm tên trùng khít từng mục).

Đối chứng: JX1 `6,1,3 = "Đại Lực hoàn"` script `\script\item\potion15.lua` (tệp CÓ),
`6,1,6 = "Phi Tốc hoàn"` cùng script. Tức bên JX1 hai thứ này là thuốc dùng được bình thường —
chỉ số genre/detail của bản Linux là sai chỗ.

**CHƯA XÁC MINH**: `AddItem` với bộ ba không có trong bảng gây lỗi Lua hay chỉ trả 0/không phát.
Nếu chỉ trả 0 thì mức là “mất thưởng”; nếu ném lỗi thì đây là CHẶN.

### NANG-3 — 8 bảng rơi `.ini` của Săn boss Sát Thủ **không được ánh xạ chút nào** (b2 chỉ lọc `.lua`)

`thicong\b2_patch.py:76` — `if not rel.lower().endswith(".lua"): continue`. Trong 101 tệp
manifest có 20 tệp **không phải .lua** (8 `bosstask_lev*.ini`, `killer.txt`, `huoyuedu.txt`,
7 `lineup*.txt`, 3 bảng toạ độ GBK) và tất cả bị bỏ qua.

`killer.txt` / `huoyuedu.txt` / `lineup*.txt` / bảng toạ độ GBK: đã kiểm — **không chứa id vật phẩm**
(chỉ `Id/BossName/BossInfo`, task id, và cặp `XPOS/YPOS`) ⇒ không sao.

Nhưng **8 tệp `settings\droprate\boss\bosstask_lev{20,30,40,50,60,70,80,90}.ini`** là bảng rơi
thật (`Genre=/Detail=/Particular=` mỗi khoá một dòng nên regex `6,1,N` không bao giờ khớp).
Trong 65 bộ của chúng có **5 bộ lệch tên** giữa hai bảng:

| bộ (cả 8 tệp) | tên Linux | tên JX1 cùng số | số JX1 đúng |
|---|---|---|---|
| `6,1,122` | Phúc Duyên Lộ (Tiểu) | Phúc Duyên Lộ (**Trung**) | `6,1,121` |
| `6,1,123` | Phúc Duyên Lộ (Trung) | Phúc Duyên Lộ (**Đại**) | `6,1,122` |
| `6,1,124` | Phúc Duyên Lộ (Đại) | **Quế Hoa Tửu** | `6,1,123` |
| `6,1,130` | Đại Bạch Câu hoàn | **Túi thư của Long Ngũ** | `6,1,129` |
| `6,1,71` | Tiên Thảo Lộ | Tiên Thảo Lộ **Thường** | (giữ 71 — xem NHE-3) |

`6,1,130` đặc biệt xấu: “Túi thư của Long Ngũ” là **đồ nhiệm vụ** của JX1
(script `\script\item\pack.lua`, hiện cũng thiếu tệp) ⇒ boss Sát Thủ sẽ rải đồ nhiệm vụ ra đất.

### NANG-4 — Ngoài 42 bộ còn **70 bộ nữa** trong 101 tệp có **tên JX1 khác tên Linux** nhưng không được ánh xạ; **14 bộ nằm ở vị trí phát thưởng**

Phương pháp: quét mọi bộ ba xuất hiện trên dòng có `tbProp|AddItem|DropItem|GiveAward|
Consume*|Calc*|GetItemCount|ForbidIn|["g,d,p"]` trong 101 tệp Linux gốc; loại 42 bộ đã ánh xạ;
so tên hai bảng. Kết quả: 168 bộ xuất hiện → 42 đã ánh xạ, **5 bộ trùng tên (an toàn)**,
**70 bộ tồn tại ở JX1 nhưng tên khác**, **52 bộ không tồn tại ở JX1** (xem NANG-5).

14 bộ ở vị trí **phát thưởng** (nguy hiểm nhất — phát nhầm đồ):

| bộ | tên Linux | tên JX1 CÙNG SỐ (thứ thực sự phát ra) | chỗ dùng |
|---|---|---|---|
| `6,1,1083` | Hồi thành phù (lớn) | Hồi thành phù (nhỏ) | `lenhbai_def.lua:401` |
| `6,1,1390` | Bắc Đẩu Trường Sinh Thuật - Cơ Sở Thiên | Lễ bao đại Bạch Câu Hoàn đặc biệt | `lenhbai_def.lua:408, 524` |
| `6,1,2005` | Tống Kim lễ bao | `<<未翻译>>` | `qianqiu_yinglie\head.lua:198, 218` |
| `6,1,2070` | Hùng Tâm Kiếm | Thiên Tứ Bảo Rương | `birthday_jieri\200905\chuangguan\chuangguan.lua:59` |
| `6,1,2424` | Đại Thành Bí Kíp 90 | Biện Kinh Huyết Chiến Lệnh | `lenhbai_def.lua:400` |
| `6,1,2425` | Đại Thành Bí Kíp 120 | Lâm An Huyết Chiến Lệnh | `lenhbai_def.lua:414` |
| `6,1,2805` | Viêm Đế Bí Bảo | Bánh Tét | `yandibaozang\head.lua:181` |
| `6,1,2921` | Võ Học Kinh Nhu | `越南自定义道具252` | `lenhbai_def.lua:425` |
| `6,1,3277` | Vô Cực Tiên Đơn | `越南自定义道具608` | `metempsychosis\translife_5.lua:17` |
| `6,1,3453` | Thiệp đồng hành | Tử Mãng Bảo Châu | `lenhbai_def.lua:447` |
| `6,1,3557` | Ngọc Chuyển Sinh | `黄金图谱礼盒－侠骨` | `lenhbai_def.lua:525` |
| `6,1,3558` | Phục Sinh Hoàn | `黄金图谱礼盒－柔情` | `lenhbai_def.lua:526` |
| `6,1,4323` | Hộ Mạch Bao | Nhược Thủy Văn Tinh- Bính [Cấp 6] | `lenhbai_def.lua:409, 426, 441` |
| `6,1,71` | Tiên Thảo Lộ | Tiên Thảo Lộ Thường | `bigboss.lua:55`, `mibao_head.lua:11` |

56 bộ còn lại nằm ở bảng cấm `vngforbidspecialitem.lua` (dòng 51–103) và vài chỗ chú thích —
mức thấp hơn nhưng **luật cấm đang cấm sai vật phẩm** (ví dụ dòng 51 `["6,1,218"]` ghi “Công Tốc
hoàn” nhưng `6,1,218` ở JX1 là “Bào Tốc hoàn”; cả dải 218…235 lệch đúng 1 nấc).

Nhận xét phạm vi: `lenhbai_def.lua`, `qianqiu_yinglie`, `birthday_jieri`, `metempsychosis`,
`yandibaozang` **không thuộc 3 hoạt động** — chúng bị kéo vào do bao đóng `Include`. Nhưng chúng
đã nằm trên cây sống nên nếu có gì kích hoạt là phát nhầm ngay. Hai chỗ **thuộc lõi Sát Thủ**
(`bigboss.lua:55`, `mibao_head.lua:11`) thì chắc chắn chạy.

### NANG-5 — **52 bộ vật phẩm không tồn tại trong bảng JX1** (không có mục nào), nhiều bộ ở vị trí phát thưởng của cả 3 hoạt động

Trích các bộ có mặt trong bảng thưởng đang chạy:

| bộ | tên Linux | chỗ dùng (ví dụ) |
|---|---|---|
| `6,0,3`, `6,0,6` | Đại Lực hoàn / Phi Tốc hoàn | xem NANG-2 (thực ra chỉ lệch DetailType) |
| `6,1,30006` | Đồ Phổ Tinh Sương Y | `chuangguanbaoxiang.lua:73` |
| `6,1,30008` | Đồ Phổ Tinh Sương Yêu Đái | `yandibaozang\head.lua:172` |
| `6,1,30009`, `6,1,30010` | Đồ Phổ Tinh Sương Hộ Uyển / Hạng Liên | `chuangguang30.lua:51, 52` |
| `6,1,30011` | Đồ Phổ Tinh Sương Ngọc Bội | `yandibaozang\head.lua:173` |
| `6,1,30126` | Bát Thuật Chân Kinh | `lenhbai_def.lua:402, 537` |
| `6,1,30215`, `6,1,30216`, `6,1,30218` | Cống Hiến / Kiến Thiết / Chiến Bị Lễ Bao | `bigboss.lua:90, 91, 92` |
| `6,1,30227/30228/30229` | Chân Nguyên Đơn tiểu/trung/đại | `activitysys\config\32\variables.lua:11-13`, `fengling_ferry\fld_head.lua:123`, `chuangguanbaoxiang.lua:60, 61` |
| `6,1,30289` | Huyết Long Đằng (cấp 5…12) | `variables.lua:16`, `chuangguanbaoxiang.lua:64-66`, `lenhbai_def.lua` ×5 |
| `6,1,30301` | Hỗn nguyên chân đơn | `variables.lua:14`, `chuangguanbaoxiang.lua:62` |
| `6,1,30350` | Hộp Mặt Nạ Chiến Trường | `yandibaozang\head.lua:156, 157` |
| `6,1,30386` | Xích Lân Chi Bảo | `lenhbai_def.lua:431` |
| `6,1,30563`, `6,1,30593` | Trang Sức Tinh Linh / Chi Quang | `bigboss.lua:68, 69` |

Đây là dải `30xxx` (đồ phổ / trang sức / đơn dược đời sau của bản Linux) mà JX1 **hoàn toàn
không có**. Ba dòng thưởng lõi bị ảnh hưởng trực tiếp: `bigboss.lua:68,69,90,91,92`
(Săn boss Sát Thủ), `chuangguanbaoxiang.lua:60-66,73` (rương Vượt Ải),
`fld_head.lua:123` (Phong Lăng Độ). Danh sách đầy đủ 52 bộ: chạy lại
`audit\_a4_cls.py` (kèm ở mục 6).

### NANG-6 — 12+ vật phẩm đích **phát ra được nhưng vô dụng**: bản Linux có script sử dụng, JX1 để `Script = 0`

Không phải “JX1 không dùng script cho đồ” — JX1 có 275 mục có script và các thuốc/hộp cùng loại
đều có (`6,1,3 → potion15.lua`, `6,1,71 → xiancaolu.lua`, `6,1,121 → fuyuanlu_small.lua`,
`6,1,1401 → hoplevatvuotai.lua`). Nên `Script=0` ở các mục dưới là thật sự không dùng được:

| bộ Linux → JX1 | vật phẩm | script bản Linux | JX1 | nguồn phát (bản port) |
|---|---|---|---|---|
| `6,1,1075 → 1076` | Hộp lệ vật [quả Huy Hoàng] | `xinnianlibao\xinniangift.lua` | `0` | `bigboss.lua:38, 44` |
| `6,1,2006 → 2015` | Khiêu chiến Lễ bao | `jiefang_jieri\200904\tiaozhan_libao.lua` | `0` | `mibao_head.lua:18` |
| `6,1,2527 → 2536` | Hồi thiên tái tạo lễ bao | `caiyao\huitianlibao.lua` | `0` | `bigboss.lua:84` |
| `6,1,1781 → 1790` | Cẩm nang thay đổi trời đất (chứa 60 món) | `caiyao\huitianjinglang.lua` | `0` | `mibao_head.lua:21` |
| `6,1,2115 → 2124` | Hải long châu | `hailongzhu.lua` | `0` | `bigboss.lua:51`, `bossdeath.lua:42` |
| `6,1,2116 → 2125` | Thiên Niên Linh Dược | `qiannianlingyao.lua` | `0` | `rank_perday.lua:13` |
| `6,1,2952 → 3570` | Kích Công Trợ Lực Hoàn | `ib_shop\gongjizuliwan.lua` | `0` | `bigboss.lua:86` |
| `6,1,2953 → 3571` | Âm Dương Hoạt Huyết Đơn | `ib_shop\yinyanghuoxuewan.lua` | `0` | `bigboss.lua:87` |
| `6,1,3022 → 3640` | Thiên Linh Đơn | `goldboss_adjust_2011\...` | `0` | `bigboss.lua:97` |
| `6,1,4149 → 4755` | Sách kĩ năng cấp 150 cấp 22 | `skill\lvlup150_limit.lua` | `0` | `bigboss.lua:64` |
| `6,1,906 → 3440` | Quả Huy Hoàng (cao) | `huihuangzhiguo3.lua` | `0` | `mibao_head.lua:19` |
| `6,1,1094 → 1095` | Thí Giả Chi ấn | `signet.lua` | `0` | `bossdeath.lua:22` (Phong Lăng Độ) |
| `6,1,3810 → 4428` | Tinh Thiết Khoáng | `item_jingtiekuang.lua` | `0` | 4 chỗ — **CHƯA XÁC MINH** (nguyên liệu, có thể dùng qua giao diện ghép) |
| `6,1,3811 → 4429` | Tinh Tinh Khoáng | `item_jingjingkuang.lua` | `0` | 4 chỗ — **CHƯA XÁC MINH** |

### NANG-7 — 3 vật phẩm đích **JX1 đã có script riêng khác hẳn bản Linux** ⇒ tệp vừa chép là mã chết, bảng thưởng chạy theo JX1

| bộ | JX1 chạy tệp nào | bản port chép tệp nào (không được gọi) |
|---|---|---|
| `6,1,1392 → 1401` Hộp lễ vật vượt ải | `\script\item\hoplevatvuotai.lua` (CÓ tệp) | Linux gọi `\script\item\giftbox.lua` (không chép) |
| `6,1,2743 → 3361` Bảo Rương Thủy Tặc | `\script\item\baoruongthuytac.lua` (CÓ tệp) | Linux gọi `\script\item\activityitem.lua` |
| `6,1,4134 → 4752` Chân Nguyên Đan | `\script\item\event\kinhmach\channguyendan.lua` (CÓ tệp) | Linux gọi `activitysys\config\13\zhenyuandan.lua` |

Tức bảng phần thưởng khi mở hộp/rương sẽ là **bảng của JX1**, không phải bảng bản Linux mà đợt
port vừa dịch. Không sập, nhưng thiết kế phần thưởng khác đặc tả.

### NANG-8 — 6 số đích **đang được hệ khác của JX1 dùng song song** (hai nguồn cùng phát một vật phẩm)

| số đích | vật phẩm | hệ JX1 sẵn có cũng phát/tiêu | hệ vừa port |
|---|---|---|---|
| `6,1,398` | Sát Thủ lệnh | `tinhnang\boss_satthu\drop.lua:49`, `tinhnang\vuot_ai\drop.lua:49`, `event\storm\custom.lua:155`, `script\<GBK 西域北区\江津村>\…\ôâàï.lua:331` | `lib_killlevel.lua` ×8 |
| `6,1,399` | Sát thủ giản | `npcchucnang\nhieptran.lua:128,159`, `vuot_ai\sugiasatthu.lua:131,163` | `nieshichen.lua:204` |
| `6,1,2024` | Truy công lệnh | `tinhnang\phonglangdo\lib_phonglangdo.lua:20 ITEM_TRUYCL` | `bossdeath.lua:31`, `shuizeideath.lua:13` |
| `6,1,1182` | Tiên Thảo Lộ đặc biệt | `global\vatpham.lua:134`, `tinhnang\phonglangdo\drop.lua:104`, `ruong_datau_tasklink.lua:48-50`, 4 tệp event | `bigboss.lua:56` |
| `6,1,3361` | Bảo Rương Thủy Tặc | `tinhnang\phonglangdo\drop.lua:108` | `fengling_ferry\mission.lua:120` |
| `6,1,3362` | Chìa Khóa Như ý | `item\xinshirenwu\xinshibaoxiang.lua:10` (mở **Tín Sứ Bảo Rương** `6,1,3430`) | `chuangguanbaoxiang.lua:18` (mở Bảo Rương Vượt ải) |

Riêng `6,1,3362`: một chìa khoá dùng cho hai loại rương ⇒ người chơi cày Vượt Ải có thể mở rương
Tín Sứ và ngược lại. Cần chủ game quyết định là cố ý hay không.

Ghi chú tích cực: chính sự trùng khớp này **chứng minh bảng ánh xạ 42 bộ là đúng** — hệ JX1 sẵn
có độc lập cũng dùng đúng những số đó cho đúng những vật phẩm đó (398/399 Sát Thủ,
2024 Truy công lệnh, 1182 Tiên Thảo Lộ đặc biệt, 3361 Bảo Rương Thủy Tặc, 2126 Long Huyết Hoàn).

---

## 3. NHẸ / CHƯA XÁC MINH

* **NHE-1** — `6,1,1781 → 1790` nhập nhằng: JX1 có **hai** mục cùng tên “Cẩm nang thay đổi trời
  đất” (`6,1,1790` và `6,1,3656`), **cả hai đều `Script=0`, `nMaxStack=0`**. Lựa 1790 không sai
  hơn 3656, nhưng cả hai đều là đồ chết (đã tính ở NANG-6). Đối chiếu: JX1 `ruong_datau_tasklink.lua:42`
  cũng phát 1790.
  Ngược lại `6,1,1765 → 1182` (hai ứng viên 1182/1774) chọn **đúng**: 1182 có script
  `xiancaolu_special.lua` + `nMaxStack=100` + được 15 chỗ khác của JX1 dùng; 1774 là mục chết.
* **NHE-2** — Hai bộ chỉ khác hoa/thường, cùng vật phẩm, không sao:
  `6,1,2823 "Huy Chương Chiến Công" → 1827 "Huy chương chiến công"`,
  `6,1,2825 "Cây Bút" → 2183 "Cây bút"`.
* **NHE-3** — `6,1,71`: Linux “Tiên Thảo Lộ”, JX1 “Tiên Thảo Lộ **Thường**”. Nhiều khả năng cùng
  một thứ (JX1 `6,1,71` có đúng script `xiancaolu.lua` như Linux) ⇒ để nguyên. **CHƯA XÁC MINH**.
* **NHE-4** — `6,1,2348 → 2357` (Huyền Thiên Chùy): trong 101 tệp chỉ có **chỗ tiêu thụ**
  (`shashou_mibao.lua:16, 21` — cần 6 cái để mở Sát Thủ Bí Bảo), **không có chỗ phát**. Quét cả
  cây Linux: chỉ `item\xinnianlibao\xinniangift.lua:172` phát (không port). Quét cả cây JX1:
  `6,1,2357` chỉ xuất hiện ở `ruong_datau_tasklink.lua:13, 21` — cũng chỉ tiêu thụ. Kết luận:
  **không hệ nào ở JX1 phát ra Huyền Thiên Chùy**. Việc này hiện chưa gây hại vì
  `shashou_mibao.lua` là mã chết (CHẶN-2), nhưng nếu đấu dây lại thì Sát Thủ Bí Bảo sẽ không mở
  được.
* **NHE-5** — Chồng chất: `lenhbai_def.lua` trao `nCount=12000` Hộ Mạch Đơn nhưng
  `6,1,3821 nMaxStack=500` ⇒ 24 ô túi cho một phần thưởng. Ngoài phạm vi 3 hoạt động.
* **NHE-6 (ngoài phạm vi, không do đợt port)** — Vài tệp JX1 **có sẵn** đang dùng số kiểu-JX2
  chưa ánh xạ, tên hiển thị đúng nhưng số sai: `missions\leaguematch\head.lua:198, 201, 207, 211`
  (`6,1,1075`, `6,1,1781`, `6,1,2347`, `6,1,2527`) và
  `tinhnang\congthanhchien\lib_ctc.lua:352` (`AddItem(6,1,1075,…)`). Không phải lỗi phiên này,
  nhưng cùng một loại bệnh — nên ghi để chủ game biết.

---

## 4. BẢNG ĐẦY ĐỦ 42 BỘ

Producer = chỗ **phát ra** (`AddItem`/`DropItem`/`tbProp`/`GiveAward`);
Consumer = chỗ **tiêu thụ/kiểm tra** (`Consume*`/`Calc*`/`GetItemCount`/`GetItemProp`/so số trần);
Cấm = số dòng trong `vngforbidspecialitem.lua`.

| # | Nguồn (Linux) | Tên | Đích (JX1) | Tên JX1 | Producer | Consumer | Cấm | Kết luận |
|---|---|---|---|---|---|---|---|---|
| 1 | `6,1,215` | Càn Khôn Tạo Hóa Đan (đại) | `6,1,214` | Càn Khôn Tạo Hóa Đan (đại) | bigboss.lua:85, chuangguang30.lua:44, yandibaozang\head.lua:165 | – | 0 | **OK** (script khớp `sj_zaohuadan_b.lua`, `SetItemStackCount(…,10)` = nMaxStack JX1) |
| 2 | `6,1,399` | Sát Thủ lệnh | `6,1,398` | Sát Thủ lệnh | lib_killlevel.lua:73,77,81,85,89,93,97,103 | nieshichen.lua:168 (số trần) | 0 | **CHẶN-1** cấp độ 20–90 vs hệ JX1 cấp 10; NANG-8 dùng chung |
| 3 | `6,1,400` | Sát thủ giản | `6,1,399` | Sát thủ giản | nieshichen.lua:204 | dragonboat_main.lua:29,59,65,136,142; npcNhiepThiTran.lua:54 | 0 | **CHẶN-1** như trên |
| 4 | `6,1,906` | Quả Huy Hoàng (cao) | `6,1,3440` | Quả Huy Hoàng (cao) | mibao_head.lua:19 | – | 0 | NANG-6 (JX1 `Script=0`) |
| 5 | `6,1,907` | Quả Hoàng Kim | `6,1,908` | Quả Hoàng Kim | bigboss.lua:37, 96; mibao_head.lua:20 | – | 0 | **OK** (script `goldenseed.lua` khớp) |
| 6 | `6,1,1075` | Hộp lệ vật [quả Huy Hoàng] | `6,1,1076` | Hộp lệ vật [quả Huy Hoàng] | bigboss.lua:38, 44 | – | 1 | NANG-6 (hộp, `Script=0` ⇒ không mở được) |
| 7 | `6,1,1094` | Thí Giả Chi ấn | `6,1,1095` | Thí Giả Chi ấn | bossdeath.lua:22 | – | 0 | NANG-6 |
| 8 | `6,1,1392` | Hộp lễ vật vượt ải | `6,1,1401` | Hộp lễ vật vượt ải | award.lua:226 | – | 0 | NANG-7 (JX1 dùng `hoplevatvuotai.lua`) |
| 9 | `6,1,1672` | Bắc Đẩu truyền công thuật | `6,1,1681` | Bắc Đẩu truyền công thuật | bigboss.lua:49 | – | 0 | **OK** (Linux vốn `noscript`) |
| 10 | `6,1,1765` | Tiên Thảo Lộ đặc biệt | `6,1,1182` | Tiên Thảo Lộ đặc biệt | bigboss.lua:56 | – | 0 | OK; NANG-8 dùng chung (15 chỗ JX1) |
| 11 | `6,1,1781` | Cẩm nang thay đổi trời đất | `6,1,1790` | Cẩm nang thay đổi trời đất | mibao_head.lua:21 | – | 1 | NANG-6 + NHE-1 |
| 12 | `6,1,2006` | Khiêu chiến Lễ bao | `6,1,2015` | Khiêu chiến Lễ bao | mibao_head.lua:18 | – | 0 | **NANG-1 SAI — thực tế thành `6,1,2024`**; và cả 2015 cũng `Script=0` |
| 13 | `6,1,2015` | Truy công lệnh | `6,1,2024` | Truy công lệnh | bossdeath.lua:31, shuizeideath.lua:13 | shuizei.lua:74 (số trần) | 0 | ánh xạ đúng; NANG-8 dùng chung với PLD bản JX1 |
| 14 | `6,1,2115` | Hải long châu | `6,1,2124` | Hải long châu | bigboss.lua:51, bossdeath.lua:42 | – | 0 | NANG-6 |
| 15 | `6,1,2116` | Thiên Niên Linh Dược | `6,1,2125` | Thiên Niên Linh Dược | rank_perday.lua:13 | – | 0 | NANG-6 |
| 16 | `6,1,2117` | Long Huyết Hoàn | `6,1,2126` | Long Huyết Hoàn | bigboss.lua:52 | – | 0 | **NANG-1 SAI — thực tế thành `6,1,2135`** |
| 17 | `6,1,2126` | Nhất Kỷ Càn Khôn Phù | `6,1,2135` | Nhất Kỷ Càn Khôn Phù | bigboss.lua:39 | – | 0 | **OK** (nhưng đụng độ với dòng 52, xem NANG-1) |
| 18 | `6,1,2127` | Bắc Đẩu Luyện Kim Thuật (Q1) | `6,1,2136` | Bắc Đẩu Luyện Kim Thuật (Q1) | bigboss.lua:31 | – | 0 | **OK** |
| 19 | `6,1,2219` | Càn Khôn Song Tuyệt Bội | `6,1,2228` | Càn Khôn Song Tuyệt Bội | bigboss.lua:65, lenhbai_def.lua:440 | – | 0 | **OK** (script `qiankunshuangjuepei` khớp) |
| 20 | `6,1,2347` | Sát Thủ Bí Bảo | `6,1,2356` | Sát Thủ Bí Bảo | lib_killlevel.lua:105 | – | 1 | **CHẶN-2** (`satthubibao.lua` không tồn tại) |
| 21 | `6,1,2348` | Huyền Thiên Chùy | `6,1,2357` | Huyền Thiên Chùy | – | shashou_mibao.lua:16, 21 | 0 | ánh xạ đúng; NHE-4 không có nguồn phát |
| 22 | `6,1,2350` | Tử Mãng Lệnh | `6,1,2359` | Tử Mãng Lệnh | mibao_head.lua:12 | – | 0 | **OK** |
| 23 | `6,1,2351` | Huyền Viên Lệnh | `6,1,2360` | Huyền Viên Lệnh | mibao_head.lua:13 | – | 0 | **OK** |
| 24 | `6,1,2352` | Thương Lang Lệnh | `6,1,2361` | Thương Lang Lệnh | mibao_head.lua:14 | – | 0 | **OK** |
| 25 | `6,1,2353` | Vân Lộc Lệnh | `6,1,2362` | Vân Lộc Lệnh | mibao_head.lua:15 | – | 0 | **OK** |
| 26 | `6,1,2527` | Hồi thiên tái tạo lễ bao | `6,1,2536` | Hồi thiên tái tạo lễ bao | bigboss.lua:84 | – | 1 | NANG-6 (lễ bao `Script=0`) |
| 27 | `6,1,2571` | Thư của Long Nhi | `6,1,2580` | Thư của Long Nhi | talkdailytask.lua:108 | talkdailytask.lua:174, 178 | 0 | **OK** (chuỗi phát–kiểm–tiêu đủ 3 chỗ, cùng ánh xạ) |
| 28 | `6,1,2742` | Bảo Rương Vượt ải | `6,1,3360` | Bảo Rương Vượt ải | award.lua:90 | chuangguanbaoxiang.lua:159 | 0 | **CHẶN-2** (`baoruongvuotai.lua` không tồn tại) |
| 29 | `6,1,2743` | Bảo Rương Thủy Tặc | `6,1,3361` | Bảo Rương Thủy Tặc | mission.lua:120 | – | 0 | NANG-7 + NANG-8 |
| 30 | `6,1,2744` | Chìa Khóa Như ý | `6,1,3362` | Chìa Khóa Như ý | chuangguanbaoxiang.lua:18 | – | 0 | NANG-8 (dùng chung với Tín Sứ Bảo Rương) |
| 31 | `6,1,2823` | Huy Chương Chiến Công | `6,1,1827` | Huy chương chiến công | lib_killlevel.lua:118 | – | 0 | **OK** (NHE-2) |
| 32 | `6,1,2825` | Cây Bút | `6,1,2183` | Cây bút | lib_killlevel.lua:119 | – | 0 | **OK** (NHE-2) |
| 33 | `6,1,2826` | Phù Hiệu | `6,1,3444` | Phù Hiệu | lib_killlevel.lua:120 | – | 0 | **OK** |
| 34 | `6,1,2952` | Kích Công Trợ Lực Hoàn | `6,1,3570` | Kích Công Trợ Lực Hoàn | bigboss.lua:86 | – | 1 | NANG-6 |
| 35 | `6,1,2953` | Âm Dương Hoạt Huyết Đơn | `6,1,3571` | Âm Dương Hoạt Huyết Đơn | bigboss.lua:87 | – | 1 | NANG-6 |
| 36 | `6,1,3022` | Thiên Linh Đơn | `6,1,3640` | Thiên Linh Đơn | bigboss.lua:97 | – | 0 | NANG-6 |
| 37 | `6,1,3203` | Hộ Mạch Đơn | `6,1,3821` | Hộ Mạch Đơn | variables.lua:15, chuangguanbaoxiang.lua:63, lenhbai_def.lua ×18 | – | 0 | ánh xạ đúng; NHE-5 (nCount 12000 > nMaxStack 500) |
| 38 | `6,1,3454` | Thuốc tăng trưởng | `6,1,4072` | Thuốc tăng trưởng | bigboss.lua:95, lenhbai_def.lua:449,473,500,529,565 | – | 0 | **OK** |
| 39 | `6,1,3810` | Tinh Thiết Khoáng | `6,1,4428` | Tinh Thiết Khoáng | kill_level.lua:92, chuangguang30.lua:40, chuangguanbaoxiang.lua:68, yandibaozang\head.lua:153 | – | 0 | NANG-6 (CHƯA XÁC MINH — nguyên liệu) |
| 40 | `6,1,3811` | Tinh Tinh Khoáng | `6,1,4429` | Tinh Tinh Khoáng | kill_level.lua:91, chuangguang30.lua:39, chuangguanbaoxiang.lua:67, yandibaozang\head.lua:152 | – | 0 | NANG-6 (CHƯA XÁC MINH) |
| 41 | `6,1,4134` | Chân Nguyên Đan | `6,1,4752` | Chân Nguyên Đan | chuangguanbaoxiang.lua:59 | – | 0 | NANG-7 (JX1 dùng `channguyendan.lua`) |
| 42 | `6,1,4149` | Sách kĩ năng cấp 150 cấp 22 | `6,1,4755` | Sách kĩ năng cấp 150 cấp 22 | bigboss.lua:64 | – | 0 | NANG-6 (sách `Script=0` ⇒ không học được) |

**Tên vật phẩm: 42/42 trùng khít** (so chuẩn hoá khoảng trắng + không phân biệt hoa thường).
Không bộ nào “chỉ gần giống” ngoài hai bộ khác hoa/thường ở NHE-2. Đây là phần **làm đúng** của
đợt trước.

---

## 5. ĐƯỜNG SÓT (false negative) — đã quét, kết quả

* **Dạng số trần** (`== N`, `= N`, `parttype == N`, `np == N`): quét toàn bộ 101 tệp **bản Linux
  gốc**, chỉ có **đúng 2 chỗ** và cả 2 đã được vá tay:
  `killer\nieshichen.lua:168` (`parttype == 399` → 398) và
  `jiefang_jieri\200904\shuizei\shuizei.lua:74` (`np == 2015` → 2024). **Không sót thêm.**
* **Biến trung gian / chuỗi ký tự** dạng `ITEM_X = {6,1,N}` hoặc `"6,1,N"`: dạng bảng
  `["6,1,N"]` trong `vngforbidspecialitem.lua` đã được regex bắt (6 chỗ). Không có dạng
  `ITEM_XXX = {6,1,N}` nào trong 101 tệp port.
* **Khoảng trắng lạ** `6 , 1 , 399`: regex `(\s*,\s*)` đã bao. Không tìm thấy chỗ nào bị bỏ.
* **Đã kiểm lại từng chỗ trong 106 chỗ** — không có chỗ nào là toạ độ / tham số / số bảng khác.
  Các số 400 / 215 / 3022 xuất hiện ở nơi khác (`BONUS_1VS1 = 400`, `RandRate=400`,
  toạ độ `(215,189)` trong `killer.txt`, `SetPlayerTitle(215,…)`, `HD3_VA_GIO = {…400…}`)
  đều **không** bị regex chạm vì không ở dạng bộ ba.
* **Sót thật** nằm ở 4 lớp regex không thể bắt: NANG-2 (`6,0,N`), NANG-3 (`.ini` dạng
  `Genre=/Detail=/Particular=`), NANG-4 (70 bộ ngoài danh sách 42), NANG-5 (52 bộ không có ở JX1).

---

## 6. VIỆC CẦN LÀM (đề xuất, phiên chính tự quyết)

Ưu tiên theo thứ tự:

1. **Sửa va chạm chuỗi** (NANG-1): đổi `b2_patch.py` sang một lượt `re.sub` tra bảng, hoặc vá tay
   2 chỗ: `mibao_head.lua:18` `2024 → 2015`; `bigboss.lua:52` `2135 → 2126`.
   Nhớ vá **cả hai cây**: cây sống `E:\…\bin\server` và gương `D:\GAMEDEVNEW\serverscript_jx2\3hoatdong`
   (đã kiểm: 6 tệp mẫu hiện **giống hệt nhau**, md5 khớp).
2. **Quyết hệ nào sống** (CHẶN-1): hoặc tắt `addnpcsatthu()` (`script\startgame.lua:202`) +
   gỡ 7 NPC `OTHER_SGSTHU` ở `script\startgame\thanh\*.lua`, hoặc bỏ `autoexec_npc_hd3.lua` và
   dùng lại hệ JX1. **Không được để cả hai** — hai vòng kinh tế cấp 10 vs 20–90 không nối được.
3. **Đấu dây 2 rương lõi** (CHẶN-2): trỏ `6,1,3360` → `\script\missions\challengeoftime\item\chuangguanbaoxiang.lua`
   và `6,1,2356` → `\script\task\tollgate\killer\shashou_mibao.lua` trong
   `settings\item\magicscript.txt`; hoặc viết `baoruongvuotai.lua` / `satthubibao.lua`.
   Nếu chọn `shashou_mibao.lua` thì phải giải quyết NHE-4 (không ai phát Huyền Thiên Chùy).
4. **Vá lớp `6,0,N` → `6,1,N`** (NANG-2): 10 chỗ phát thưởng + 13 dòng bảng cấm.
5. **Vá 5 bộ trong 8 tệp `bosstask_lev*.ini`** (NANG-3): `122→121, 123→122, 124→123, 130→129`.
6. **Rà 14 bộ phát thưởng ở NANG-4** và **52 bộ ở NANG-5** — quyết định bỏ dòng thưởng hay
   ánh xạ tiếp.
7. **Rà 12 vật phẩm `Script=0`** ở NANG-6 — hoặc chép script bản Linux vào JX1 và điền cột
   Script, hoặc thay bằng vật phẩm JX1 tương đương đang sống.

### Kịch bản tái lập
Các script kiểm dùng trong báo cáo này đã chép vào
`D:\GAMEDEVNEW\ReverseTools\port_3hd\audit\_a4_*.py`
(`a4_tbl.py` so tên 42 bộ, `a4_sites.py` dò va chạm chuỗi, `a4_diff.py` diff 118 dòng,
`a4_scan.py` quét cả cây JX1, `a4_bare.py` dò số trần, `a4_cls.py` phân loại 70+52 bộ sót,
`a4_script.py` so cột Script, `a4_stack.py` so nMaxStack, `a4_table.py` sinh bảng 42 dòng).
Tất cả **chỉ đọc**, chạy với `PYTHONIOENCODING=utf-8`.

---

## ĐỐI CHẤT (tác tử độc lập)

Phiên đối chất 25/08. Người soát **không phải** người viết báo cáo trên. Nguyên tắc: **mặc định
mọi phát hiện là SAI cho tới khi tệp gốc / bảng dữ liệu / mã C++ chứng minh ngược lại**.
Chỉ đọc, không sửa mã nguồn — chỉ ghi thêm mục này.

**Phát hiện then chốt của vòng đối chất** (nó lật ngược một phần kết luận ở trên):

> `KItemGenerator::Gen_MagicScript` (`D:\GAMEDEVNEW\Sources\Core\Src\KItemGenerator.cpp:1358-1370`)
> tra bảng bằng **`GetMagicScript(nParticularType)`** → `KBPT_MagicScript::GetRecord(i)`
> (`KBasPropTbl.cpp:1033-1043`) = `((KBASICPROP_MAGICSCRIPT*)m_pBuf) + i`, tức **ParticularType
> được dùng làm CHỈ SỐ DÒNG**, và **DetailType bị bỏ qua hoàn toàn** (sau đó `*pItem = *pMagicScript`
> chép cả DetailType từ *bảng*, không lấy từ Lua).
> Đo thật: `settings\item\magicscript.txt` của JX1 có **4865/4865 dòng thoả `ParticularType == chỉ số dòng`**
> (bản Linux thì **4203/4995 dòng KHÔNG thoả** — hai engine tra bảng khác nhau).
> ⇒ Ở JX1, `{6,0,N}` và `{6,1,N}` sinh ra **cùng một vật phẩm**.
> Kịch bản tái lập: `audit\_dc_v7_rowidx.py` (danh sách đầy đủ ở cuối mục).

Hệ quả tương tự cho trang bị: `Gen_Equipment` (`KItemGenerator.cpp:309`) dùng
`i = nParticularType` khi `nDetailType > equip_horse(10)`, và `i = nParticularType*10 + nLevel - 1`
khi `nDetailType <= 10`. Đây là cơ sở để kiểm mặt nạ / ngựa ở mục "Bỏ sót" bên dưới.

### Bảng đối chất

| # | Phát hiện (báo cáo trên) | Bằng chứng gốc (đối chất) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | **CHẶN-1** 7 NPC 769 JX1 vẫn sống, 5/7 trùng khít toạ độ | `script\startgame.lua:185,187,189,191,193,195,197` gọi đủ `addnpctuongduong/bienkinh/daily/duongchau/laman/thanhdo/phuongtuong`; 7 dòng `AddNpcEx1({769},…,OTHER_SGSTHU,nil,6)` ở `startgame\thanh\*.lua`; toạ độ khớp đúng như bảng | **ĐÚNG** | giữ CHẶN |
| 2 | **CHẶN-1** hai vòng kinh tế cấp 10 vs 20–90 | `tinhnang\boss_satthu\drop.lua:49` + `tinhnang\vuot_ai\drop.lua:49` = `DropItem(…,6,1,398,10,…)`; `npcchucnang\nhieptran.lua:144,153` bắt **cấp 10 và đủ 5 ngũ hành**; `lib_killlevel.lua:73…103` = `AddItem(6,1,398,20…90,…)`; `dragonboat_main.lua:58` `for i=20,80,10`, `:65` cấp 90. Thêm: JX1 `6,1,398` có `nMaxStack=0` nên `Gen_MagicScript` **không** ép `SetLevel(10)` ⇒ cấp 20–90 thật sự tồn tại | **ĐÚNG** | giữ CHẶN |
| 3 | **CHẶN-1** cách sửa: tắt `addnpcsatthu()` (dòng 202) | Sai địa chỉ: `addnpcsatthu()` (`startgame\khac\satthu.lua`) đặt **10 con BOSS ở map 321**, *không* đặt NPC 769. NPC 769 do `addnpc<thành>()` đặt | **ĐÚNG (lỗi) nhưng CÁCH SỬA SAI ĐỊA CHỈ** | tắt **cả hai**: (a) 7 dòng `OTHER_SGSTHU` trong `startgame\thanh\*.lua` cho NPC, (b) `addnpcsatthu()` dòng 202 cho boss — vì HD3 cũng tự đặt boss |
| 4 | **CHẶN-2** `6,1,3360`/`6,1,2356` trỏ tệp không tồn tại | Đọc trực tiếp `magicscript.txt`: `3360 → \script\item\baoruongvuotai.lua` (**không có**), `2356 → \script\item\satthubibao.lua` (**không có**); đối chứng `3430 → xinshirenwu\xinshibaoxiang.lua` (**có**) | **ĐÚNG** | giữ, nhưng xem #5 |
| 5 | **CHẶN-2** "CHƯA XÁC MINH engine xử lý ra sao khi thiếu tệp" | Xác minh được: `KItemList.cpp:1793,1829` → `KPlayer::ExecuteScript(szScript,…)` → `g_GetScript(dwScriptId)`; `if (pScript)` sai ⇒ **không làm gì, không lỗi Lua, không sập** | **ĐÚNG nhưng THỔI PHỒNG nhãn** | mức thật = **NẶNG-CAO** (bấm rương không phản hồi), không phải "sập/mất dữ liệu". Vẫn ưu tiên vá vì đó là phần thưởng lõi của 2/3 hoạt động |
| 6 | **NANG-1** va chạm chuỗi ⇒ `mibao_head.lua:18` = `6,1,2024` | Diff byte độc lập 101 tệp: đúng **118 dòng khác**; cặp `2006→2024` xuất hiện **x1**. `magicscript.txt` JX1: `2024 = "Truy công lệnh"`, `2015 = "Khiêu chiến Lễ bao"`. `remap_resolved.json` để `"6,1,2006"` **trước** `"6,1,2015"` ⇒ dict giữ thứ tự chèn ⇒ va chạm thật | **ĐÚNG** | `mibao_head.lua:18`: `2024 → 2015` |
| 7 | **NANG-1** `bigboss.lua:52` = `6,1,2135` | Diff: cặp `2117→2135` x1. JX1: `2117 = "Xảo quả"`, `2126 = "Long Huyết Hoàn"` (script `longxuewan.lua`, **có tệp**), `2135 = "Nhất Kỷ Càn Khôn Phù"`. `bigboss.lua:39` và `:52` hiện **cùng ra 2135** | **ĐÚNG** | `bigboss.lua:52`: `2135 → 2126` |
| 8 | **NANG-1** cặp `399/400` "thoát nạn nhờ thứ tự" | Giao của tập đích và tập nguồn = {2015, 2126, 399}; trong json `"6,1,399"` đứng **trước** `"6,1,400"` ⇒ an toàn. Diff xác nhận `399→398` x9, `400→399` x8 | **ĐÚNG** | giữ cảnh báo |
| 9 | **NANG-2** `genre 6 / detail 0` "không tồn tại ở JX1", **10 chỗ phát thưởng đang phát vật phẩm không tồn tại** | Xem khối "Phát hiện then chốt": engine **bỏ qua DetailType**, `{6,0,3}` = dòng 3 = "Đại Lực hoàn", `{6,0,6}` = dòng 6 = "Phi Tốc hoàn". **Không chỗ nào hỏng** | **SAI / THỔI PHỒNG** | bỏ 10 dòng "phát thưởng" khỏi NANG-2. Không cần vá `bigboss.lua:53,54`, `kill_level.lua:105,106`, `mibao_head.lua:16,17`, `chuangguang30.lua:45,46`, `yandibaozang\head.lua:166,167` |
| 10 | **NANG-2** 13 dòng bảng cấm `["6,0,N"]` vô hiệu | Khoá ghép bằng `pack_ItemDetail(g,d,p)` = `g..","..d..","..p` (`script\item\forbiditem.lua:88`) từ genre/detail **thật của vật phẩm** (luôn là 6,1,N) ⇒ khoá `6,0,N` không bao giờ khớp: **đúng**. NHƯNG `tbVNGForbidItem:CheckItemUsable` chỉ được gọi từ `forbiditem.lua:136 Check_ItemUsable`, mà **`Check_ItemUsable` KHÔNG CÓ NGƯỜI GỌI** (grep toàn cây `.lua` = 1 hit là chính dòng định nghĩa; grep toàn `D:\GAMEDEVNEW\Sources` = 0 hit) | **ĐÚNG nhưng THỔI PHỒNG** | hạ xuống **NHẸ** — cả tệp `vngforbidspecialitem.lua` là mã chết |
| 11 | **NANG-3** 8 tệp `bosstask_lev*.ini` không được ánh xạ, 5 bộ lệch | `b2_patch.py:76` lọc `.lua`: đúng. 8 tệp `.ini` byte-giống hệt bản Linux. Định dạng khớp parser JX1 (`KNpcTemplate.cpp:19-44`, `Genre/Detail/Particular/RandRate`), `ITEM_DropRateItem` **có đăng ký** (`ScriptFuns.cpp:15433`) và đang được gọi thật (`global\thanh\npc\bosssatthuhead.lua:13-34`, `bosssatthudeath.lua:9`, `kill_level.lua:69`). Kiểm 65 bộ: đúng **5 bộ genre-6 lệch tên**, ứng viên cùng tên: 122→**121**, 123→**122**, 124→**123**, 130→**129**, 71→không có | **ĐÚNG** | giữ NẶNG, sửa đúng 4 bộ (71 giữ nguyên) |
| 12 | **NANG-4** 70 bộ lệch tên, 14 bộ ở vị trí phát thưởng | Kiểm lại bằng phương pháp mạnh hơn (so **nhãn `szName` trên chính dòng đó** với **tên thật của JX1 tại dòng `ParticularType`**): 14 bộ trong bảng đều đúng, nhưng **danh sách còn thiếu** — xem "Bỏ sót" M2 | **ĐÚNG nhưng THIẾU** | bổ sung `award.lua:15-17`, `npc.lua:266-268`, `yandibaozang\head.lua:52,53,118-128,139,145` |
| 13 | **NANG-4** "56 bộ còn lại nằm ở bảng cấm … luật cấm đang cấm sai vật phẩm" | Cùng lý do #10: bảng cấm là mã chết | **THỔI PHỒNG** | hạ xuống NHẸ |
| 14 | **NANG-5** 52 bộ `30xxx` không tồn tại ở JX1 ⇒ "mất thưởng" | Tồn tại: **đúng** (dải 30xxx > 4865 dòng của bảng JX1). Nhưng hậu quả **nặng hơn** mô tả: `KItemSet::AddItemSet2` (`KItemSet.cpp:197-247`) **luôn trả về slot > 0** sau `FindFree()`; `Gen_MagicScript` gặp `GetRecord()==NULL` thì `_ASSERT(FALSE)` (no-op ở Release) rồi `return FALSE` **mà không đặt gì**; slot đã `Reset()` (`KItem.cpp:1119-1122` → `KItem::Reset()` `KItem.cpp:33-47` memset toàn bộ `m_CommonAttrib`) ⇒ **một vật phẩm RỖNG (genre 0 / detail 0 / particular 0 / tên rỗng) được `AddKIL` vào túi** (`ScriptFuns.cpp:4894` → `:4904 AddKIL`), túi đầy thì `ObjSet.Add` **rơi ra đất** | **ĐÚNG — nhưng HẠ THẤP mức độ** | nâng lên **NẶNG-CAO**: không chỉ mất thưởng mà **sinh vật phẩm rác vào túi/đất và được lưu xuống DB** |
| 15 | **NANG-6** 12+ vật phẩm đích `Script=0` | Đọc trực tiếp cột `[9]` của `magicscript.txt`: 1076, 1095, 1790, 2015, 2124, 2125, 2536, 3440, 3570, 3571, 3640, 4428, 4429, 4755, 3821, 2357, 398, 399, 3362 — **tất cả = `0`**; đối chứng 3, 6, 71, 121, 122, 123, 124, 1401, 3361, 4752, 2126 đều **có** script và **có tệp** | **ĐÚNG** | giữ NẶNG |
| 16 | **NANG-7** 3 vật phẩm JX1 đã có script riêng | `1401 → hoplevatvuotai.lua` (**có**), `3361 → baoruongthuytac.lua` (**có**), `4752 → event\kinhmach\channguyendan.lua` (**có**) | **ĐÚNG** | giữ NẶNG |
| 17 | **NANG-8** 6 số đích dùng chung | `6,1,398` xuất hiện ở 8 tệp gồm cả `tinhnang\boss_satthu\drop.lua` lẫn `lib_killlevel.lua`; `6,1,3362` chỉ ở 2 tệp: `item\xinshirenwu\xinshibaoxiang.lua:10` và `challengeoftime\item\chuangguanbaoxiang.lua:18` | **ĐÚNG** | giữ; **ghi chú**: lo ngại "một chìa mở hai rương" hiện **chưa xảy ra** vì `chuangguanbaoxiang.lua` đang là mã chết (CHẶN-2) — chỉ thành thật sau khi đấu dây lại |
| 18 | **NHE-3** giữ `6,1,71` | Quét toàn bảng JX1: **không có** mục genre 6 nào tên đúng "Tiên Thảo Lộ"; `6,1,71` JX1 = "Tiên Thảo Lộ Thường", script `xiancaolu.lua` (có tệp) — giống Linux | **ĐÚNG** | giữ NHẸ |
| 19 | **NHE-4** không ai phát Huyền Thiên Chùy | Grep toàn cây JX1 cho `6,1,2357`: chỉ 4 chỗ, **tất cả đều tiêu thụ/đếm** (`ruong_datau_tasklink.lua:13,21`, `shashou_mibao.lua:16,21`) | **ĐÚNG** | giữ NHẸ |
| 20 | **NHE-5** `nCount=12000` Hộ Mạch Đơn ⇒ "24 ô túi", "ngoài phạm vi" | Bộ trao thưởng thật của JX1 là `script\lib\awardtype\item_jx1.lua:17,23-31`: `nAmount = (nAwardCount or 1) * (tbItem.nCount or 1)` rồi **`for i = 1, nAmount do AddItem(...) end`** ⇒ `nCount=12000` = **12 000 lần gọi `AddItem`** trong một tick, mỗi lần túi đầy còn tạo thêm một `Object` rơi ra đất. Máy chủ chỉ có **một luồng logic** (ký ức dự án 24/08). `lenhbai_def.lua` **có đường sống**: `missions\challengeoftime\award.lua:4` → `thapnienlenhbai\mainfuc.lua:5` → `lenhbai_def.lua` | **HẠ THẤP nghiêm trọng** | nâng lên **NẶNG** (nguy cơ đơ máy chủ + rác map). Sửa: kẹp `nCount` theo `nMaxStack`, hoặc dùng `SetItemStackCount` một lần thay vì vòng lặp |
| 21 | "**KHÔNG có đường sai**, 106 chỗ đều là bộ ba vật phẩm thật" | Diff byte độc lập 101 tệp Linux ↔ cây sống: **118 dòng** (105 dòng "chỉ đổi số" + 1 dòng đổi số làm lệch độ dài + 11 `AddNpc→AddNpcEx` + 1 bỏ `Include`). Soi tay các cặp rủi ro nhất (`215→214` x3, `400→399` x8, `3203→3821` x20, `907→908` x3, `2015→2024` x3): **tất cả đều là `tbProp`/`AddItem`/`DropItem`/`ConsumeItem`/`CalcEquiproomItemCount`** — không có toạ độ/tham số nào bị chạm | **ĐÚNG** | giữ nguyên (đây là phần làm tốt) |
| 22 | "**Tên vật phẩm 42/42 trùng khít**" | Kiểm lại toàn bảng + đối chứng gián tiếp: chính hệ JX1 sẵn có độc lập cũng dùng đúng 398/399/2024/1182/3361/2126 cho đúng những vật phẩm đó | **ĐÚNG** | giữ nguyên |
| 23 | "Gương repo `serverscript_jx2\3hoatdong` md5 khớp cây sống" | Kiểm 101 tệp manifest + 4 tệp mới + 5 tệp sửa (`startgame.lua`, `timerserver.lua`, `cauhinh_hoatdong.lua`, `lenhbaiadmin.lua`, `settings\task\missions.txt`): **109 giống, 0 khác, 0 thiếu** | **ĐÚNG** | giữ nguyên |
| 24 | Mục 5: "**Dạng số trần chỉ có đúng 2 chỗ, không sót thêm**" | Quét lại độc lập trên 101 tệp Linux gốc: 30 vị trí có số trần trùng 42 nguồn; soi từng cái — 26 là bộ ba mà `b2_patch` **đã** bắt (regex `6\s*,\s*1\s*,\s*N` vẫn khớp khi đứng sau tham số khác, ví dụ `CallPlayerFunction(…, CalcItemCount, 3, 6, 1, 400, 90)`), 2 là hai chỗ đã vá tay, 2 còn lại là **ID kỹ năng** `AddMagic(400)/HaveMagic(400)` ở `task_func.lua:62,71` — **không phải vật phẩm**, và kiểm `settings\skills.txt` thì skill 400 = "Kiếp Phú Tế Bần" **giống nhau ở cả hai bản** | **ĐÚNG (về vật phẩm)** | giữ; thêm ghi chú lớp ID kỹ năng đã kiểm và **sạch** (210/400/541 đều trùng tên) |

**Tổng đối chất: 24 phát hiện — 18 ĐÚNG, 1 SAI/THỔI PHỒNG hoàn toàn (NANG-2 phần phát thưởng),
2 THỔI PHỒNG một phần (bảng cấm ở NANG-2 và NANG-4; nhãn CHẶN của CHẶN-2), 2 HẠ THẤP (NANG-5, NHE-5),
1 đúng-lỗi-nhưng-sai-cách-sửa (CHẶN-1).**

### Bỏ sót của chính vòng soát

**M1 — Cả lớp `genre ≠ 6` chưa hề được soát** (`audit\_a4_cls.py:45`: `if k[0] != 6 or k not in lnx: continue`).
Trong 101 tệp có **19 bộ genre 0/2/3/4** nằm ở vị trí `tbProp`/`AddItem`. Kết quả kiểm lại (dùng đúng
công thức tra bảng của engine, không dùng khoá `(g,d,p)`):

* **Mặt nạ `0,11,446 / 447 / 450 / 482 / 647 / 828` — KHÔNG SAO.** `mask.txt` JX1 có **839 dòng**,
  `i = ParticularType` ⇒ đều nằm trong bảng và **trùng tên từng chữ** với bản Linux.
  (Lưu ý: nếu tra bằng khoá `(0,11,p)` như `_a4_cls.py` thì 6 bộ này sẽ **báo nhầm** thành "không tồn tại".)
* **Ngựa `0,10,19 / 20 / 21` — LỖI THẬT, mức NẶNG.** Với `nDetailType <= equip_horse(10)` engine dùng
  `i = ParticularType*10 + nLevel - 1` ⇒ 190 / 200 / 210, trong khi `horse.txt` JX1 chỉ có **135 dòng**
  ⇒ `GetRecord` trả NULL ⇒ **vật phẩm rỗng** (xem #14).
  Chỗ dùng: `script\task\tollgate\killer\kill_level.lua:101,102,103`
  ("Phong Vân Bạch Mã / Chiến Mã / Thần Mã", mốc thưởng 141–160 của Săn boss Sát Thủ).
  Sửa: bỏ 3 dòng đó, hoặc bổ sung 3 con ngựa vào `settings\item\horse.txt` (phải chèn **đủ 10 dòng/cấp**
  cho mỗi particular, không được chèn 1 dòng).
* **Đồ nhiệm vụ `4,238 / 4,239 / 4,240 / 4,353` — KHÔNG SAO** (`questkey.txt` hai bên trùng tên:
  Lam/Tử/Lục Thủy Tinh, Tinh Hồng Bảo Thạch).
* `2,0,11` và `3,6,1` chỉ xuất hiện ở `CalcItemCount`, không phát ⇒ không sao.

**M2 — Cả lớp "dòng thưởng KHÔNG chứa từ khoá"** chưa được soát. Bộ lọc của `_a4_cls.py:41` và
`_a4_all97.py:47` bắt buộc dòng phải có `tbProp|AddItem|DropItem|GiveAward|Consume*|Calc*|GetItemCount|ForbidIn|["g,d,p"]`.
Bảng thưởng ngẫu nhiên của **lõi Vượt Ải** viết dạng `{trọng_số, {"tên", 6, 1, N, 1, 0, 0}}` — **không có từ khoá nào**
⇒ lọt lưới. Kiểm lại bằng cách so **nhãn `szName` trên chính dòng đó** với **tên thật của JX1 tại dòng `ParticularType`**:

| tệp : dòng | bộ | nhãn ghi trên dòng | vật phẩm THẬT sẽ phát ra | trọng số |
|---|---|---|---|---|
| `missions\challengeoftime\award.lua:15` | `6,1,124` | Phúc Duyên Lộ (Đại) | **Quế Hoa Tửu** | 0.2 |
| `missions\challengeoftime\award.lua:16` | `6,1,123` | Phúc Duyên Lộ (Trung) | **Phúc Duyên Lộ (Đại)** | 0.19587 |
| `missions\challengeoftime\award.lua:17` | `6,1,122` | Phúc Duyên Lộ (Tiểu) | **Phúc Duyên Lộ (Trung)** | 0.18 |
| `missions\challengeoftime\npc.lua:266,267,268` | như trên | như trên | như trên | (bảng thứ hai, cùng nội dung) |

Cộng lại **≈ 57,5 % trọng số** của bảng thưởng ngẫu nhiên Vượt Ải đang phát **sai vật phẩm** — đây là
**đúng cùng một họ lệch 1 nấc** đã bắt được ở NANG-3 (bảng `.ini`), chỉ khác là nằm trong `.lua`
mà `b2_patch` **có** xử lý nhưng bảng 42 bộ **không có** 122/123/124. Mức: **NẶNG**.
Sửa: `124→123`, `123→122`, `122→121` tại 6 dòng trên (và giữ nguyên `71`).

Cùng phương pháp còn lộ ra một họ nữa (ngoài phạm vi 3 hoạt động nhưng đã nằm trên cây sống):
`missions\yandibaozang\head.lua:52,53,118,119,120,121,122,123,124,125,126,127,128,139,145`
— dải `6,1,1604…1617` lệch **2 nấc** (ví dụ nhãn "Hình nhân" `6,1,1605` nhưng JX1 `1605` = "Thiệp chúc sư đệ",
"Hình nhân" thật ở `1614`). Mức: NẶNG nếu Viêm Đế Bảo Tạng được bật, NHẸ nếu không.

**M3 — Cả lớp ID nhiệm vụ (task id) chưa được soát.** Quét `SetTask/GetTask/nt_setTask/nt_getTask/SetTaskTemp`
trên 101+4 tệp port so với phần còn lại của cây JX1:

* **5 id dùng chung**: `88` (`task_func.lua:84` ↔ `global\skills_table.lua:1303`), `200`
  (`battlehead.lua:960`, `chuangguang30.lua:85,140`, `mission_match.lua:43,68`, `fld_head.lua:117`
  ↔ `item\hoatdong_admin.lua:135`, `missions\bw\bwhead.lua:137`), `751` (`translife_5.lua:97` ↔ `event\storm\function.lua:86`),
  `1122` (`weeklyrank.lua:144` ↔ `event\tongwar\head.lua:281`), `1550` (`npcNhiepThiTran.lua:67,71` ↔ `event\storm\function.lua:396`).
  `200` là **temp task** nên rủi ro thấp; `1122`/`1550` là task **lưu** ⇒ phải rà. Mức: **NẶNG / CHƯA XÁC MINH** (chưa truy hết ngữ nghĩa từng id).
* **`task 5100` vượt `MAX_TASK = 4200`** (`Sources\Core\Src\KPlayerTask.h:18`; `KPlayerTask::SetSaveVal`
  chặn `nNo >= MAX_TASK` rồi `return` im lặng): `script\global\thanh\npc\npc_chuyensinh.lua:37,45`
  (`GetTask(5100)` / `SetTask(5100, cs+1)`) ⇒ bộ đếm chuyển sinh **vĩnh viễn = 0**.
  Hiện **ngủ** vì 8 dòng đặt NPC 2530 trong `script\global\thanh\npc\add_npc.lua:13-20` đều bị comment.
  Mức: **NHẸ (ngủ)** — nhưng phải ghi vì tệp đã nằm trên cây sống.

**M4 — "CHƯA XÁC MINH engine xử lý ra sao" ở NANG-2 và NANG-5 vốn tra được từ mã C++.**
Vòng soát trước để ngỏ hai câu hỏi bản lề (`AddItem` với bộ ba không có trong bảng: lỗi Lua hay trả 0?)
rồi dựa vào đó để **không** quyết mức. Cả hai đều trả lời được bằng `KItemSet.cpp:197-247` +
`KItemGenerator.cpp:1358` + `KItem.cpp:1119`: **không lỗi Lua, không trả 0, mà tạo vật phẩm rỗng**.
Chính câu trả lời này vừa **bác** NANG-2 vừa **nâng** NANG-5.

**M5 — Phe (camp) của 7 NPC vừa đặt không được so.** Báo cáo trên so toạ độ nhưng không so tham số phe:

* bản JX1: `AddNpcEx1({769},1,nil,<map>,x,y,"",OTHER_SGSTHU,nil,**6**)` → `script\lib\lib_map.lua:108`
  `SetNpcCurCamp(nNpcId, 6)` = `camp_event` (`Sources\Core\Src\GameDataDef.h:492`).
* bản port: `script\global\autoexec_npc_hd3.lua:22` `AddNpcEx(it[1],1,random(0,4),SId,x,y,**0**,it[6])`
  → `ScriptFuns.cpp:6982-6986` `SetCurrentCamp(0)` = `camp_begin`.

Hai NPC cùng tên đứng cạnh nhau nhưng **khác phe** ⇒ khác màu tên/hành vi PK. Mức: **NHẸ / CHƯA XÁC MINH**
(chưa đo tại chỗ tác động của `camp_begin` với NPC đối thoại).

### Danh sách CHỐT sau đối chất (chỉ những lỗi THẬT)

| Mức | Lỗi | Vị trí | Sửa |
|---|---|---|---|
| CHẶN | Hai hệ Sát Thủ/Vượt Ải chạy song song, NPC 769 chồng toạ độ, kinh tế cấp 10 vs 20–90 | `startgame.lua:202`; `startgame\thanh\*.lua` (7 dòng `OTHER_SGSTHU`); `global\autoexec_npc_hd3.lua:8-14` | Chọn **một** hệ; nếu giữ HD3 thì gỡ 7 dòng `OTHER_SGSTHU` **và** tắt `addnpcsatthu()` |
| CHẶN | Hai rương phần thưởng lõi không mở được | `settings\item\magicscript.txt` dòng của `6,1,3360` và `6,1,2356` | Trỏ Script sang `\script\missions\challengeoftime\item\chuangguanbaoxiang.lua` và `\script\task\tollgate\killer\shashou_mibao.lua` (kèm giải quyết NHE-4) |
| NẶNG | Va chạm chuỗi: phát nhầm "Truy công lệnh" thay "Khiêu chiến Lễ bao" (rò vé PLD) | `script\task\tollgate\killer\mibao_head.lua:18` | `6,1,2024` → `6,1,2015` |
| NẶNG | Va chạm chuỗi: mất "Long Huyết Hoàn", trùng "Nhất Kỷ Càn Khôn Phù" | `script\missions\boss\bigboss.lua:52` | `6,1,2135` → `6,1,2126` |
| NẶNG | **(mới)** Bảng thưởng ngẫu nhiên Vượt Ải phát sai ~57,5 % trọng số | `missions\challengeoftime\award.lua:15,16,17` + `npc.lua:266,267,268` | `124→123`, `123→122`, `122→121` |
| NẶNG | 4 bộ lệch 1 nấc trong 8 bảng rơi `.ini` (có cả đồ nhiệm vụ "Túi thư của Long Ngũ") | `settings\droprate\boss\bosstask_lev{20…90}.ini` | `122→121`, `123→122`, `124→123`, `130→129` |
| NẶNG | **(mới)** 3 con ngựa ngoài bảng ⇒ sinh vật phẩm rỗng | `task\tollgate\killer\kill_level.lua:101,102,103` (`0,10,19/20/21`) | Bỏ 3 dòng, hoặc bổ sung đủ 10 dòng/cấp vào `horse.txt` |
| NẶNG | 52 bộ `30xxx` không có trong bảng JX1 ⇒ **vật phẩm rỗng vào túi / rơi ra đất / lưu DB** | `bigboss.lua:66-70,83,90-92,100-110`; `kill_level.lua:104,107-115`; `chuangguang30.lua:43,49-52`; `chuangguanbaoxiang.lua:19,60-74`; `fld_head.lua:123`; `variables.lua:11-16`; `yandibaozang\head.lua:156,157,164,170-173` | Bỏ dòng thưởng hoặc thay bằng vật phẩm JX1 tương đương |
| NẶNG | 14 bộ lệch tên chưa ánh xạ ở vị trí phát thưởng | bảng NANG-4 (giữ nguyên) | ánh xạ tiếp hoặc bỏ dòng |
| NẶNG | 12+ vật phẩm đích `Script = 0` ⇒ phát ra nhưng không dùng được | bảng NANG-6 (giữ nguyên) | điền cột Script + chép tệp, hoặc đổi vật phẩm |
| NẶNG | 3 vật phẩm đích chạy script JX1 khác hẳn bản Linux | `6,1,1401`, `6,1,3361`, `6,1,4752` | chấp nhận hoặc đổi số |
| NẶNG | **(nâng mức)** `nCount=12000` ⇒ 12 000 lần `AddItem` trong một tick, máy chủ một luồng | `vng_event\thapnienlenhbai\lenhbai_def.lua:417,422` (đường sống qua `award.lua:4`) | kẹp `nCount` theo `nMaxStack`, hoặc sửa `lib\awardtype\item_jx1.lua` dùng `SetItemStackCount` |
| NẶNG / CHƯA XÁC MINH | **(mới)** 5 task id dùng chung với hệ JX1 (`88, 200, 751, 1122, 1550`) | xem M3 | rà từng id, đổi sang dải trống (ký ức dự án: `4126-4199`) |
| NẶNG / CHƯA XÁC MINH | **(mới)** Họ `6,1,1604…1617` lệch 2 nấc | `missions\yandibaozang\head.lua:52,53,118-128,139,145` | ánh xạ +9 (1605→1614 …) hoặc tắt Viêm Đế Bảo Tạng |
| NHẸ | 6 số đích dùng chung hai hệ (398/399/2024/1182/3361/3362) | NANG-8 | chủ game quyết; `3362` chỉ thành vấn đề sau khi đấu dây rương |
| NHẸ | **(mới)** `task 5100 > MAX_TASK 4200`, hiện đang ngủ | `global\thanh\npc\npc_chuyensinh.lua:37,45` | đổi sang id < 4200 trước khi bật NPC 2530 |
| NHẸ | **(mới)** NPC 769 bản port đặt `camp 0` trong khi bản JX1 dùng `camp 6` | `global\autoexec_npc_hd3.lua:22` | truyền `6` thay `0` nếu giữ bản port |
| NHẸ | Bảng cấm `vngforbidspecialitem.lua` (13 dòng `6,0,N` + 56 bộ lệch) | toàn tệp | **mã chết** — không ai gọi `Check_ItemUsable`; sửa sau |
| NHẸ | `6,1,71`, hoa/thường 1827/2183, `1790` nhập nhằng, Huyền Thiên Chùy không nguồn phát | NHE-1…NHE-4 | giữ nguyên |
| — | **BỎ**: NANG-2 phần "10 chỗ phát thưởng `6,0,N`" | — | **không phải lỗi** — engine bỏ qua DetailType |

### Kịch bản tái lập của vòng đối chất

Đã chép vào `D:\GAMEDEVNEW\ReverseTools\port_3hd\audit\` với tiền tố `_dc_`
(chỉ đọc, chạy với `PYTHONIOENCODING=utf-8`):
`_dc_v1.py` (đọc thẳng bảng vật phẩm + kiểm tệp script có thật),
`_dc_v2_ini.py` (8 tệp `bosstask_lev*.ini`),
`_dc_v3_diff.py` (diff byte 101 tệp, phân loại 118 dòng, thống kê từng cặp số),
`_dc_v4_gap.py` (bộ ba genre ≠ 6 + `questkey.txt`),
`_dc_v5_equip.py`/`_dc_v7_rowidx.py` (**tra bảng theo CHỈ SỐ DÒNG như engine** — mask/horse/magicscript),
`_dc_v6_precise.py` (trích bộ ba theo **vị trí tham số**, không dùng regex trộn),
`_dc_v8_task.py` (đụng độ task id),
`_dc_v9_bare.py` (số trần),
`_dc_v10_stack.py` (`nCount` vs `nMaxStack`),
`_dc_v11_label.py` (**so nhãn `szName` với tên thật của JX1** — cách bắt được M2).
