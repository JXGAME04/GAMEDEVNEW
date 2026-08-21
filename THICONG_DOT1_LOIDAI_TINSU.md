# PHIẾU THI CÔNG ĐỢT 1 — LÔI ĐÀI BANG HỘI + TÍN SỨ

> Bắt đầu 21/08/2026. Tài liệu này **cập nhật liên tục trong lúc thi công**.
> Nền: `PHANTICH_LOIDAI_TINSU_BANGHOI.md` + `DIEUKIEN_THAMGIA_9_HOATDONG.md`.

## Lệnh gốc của chủ game (21/08)

1. Làm **Lôi Đài Bang Hội** + **Tín Sứ**; bản dự án trùng thì **gỡ đi, thay bản Linux vào**.
2. **Giống 100% bản Linux từ item đến hình ảnh**, chỉ đổi **giới hạn cấp độ → 90** và **bỏ trùng sinh**.
3. Tính năng khác bản Linux có sẵn và đầy đủ thì **làm luôn**.
4. **Ưu tiên: hoạt động bang hội + BOSS BANG HỘI.**
5. Thiếu item thì **làm thêm item**.
6. Làm luôn **bang hội chiếm lĩnh trên bản đồ + thông tin thuế mỗi thành**.
7. **Nhiệm vụ lúc nhận phải hiện thông tin ở Chỉ Nam Nhiệm Vụ (F11).**

Chốt phạm vi: Tín Sứ **chép đúng hiện trạng Linux** (2 tuyến Thành Đô ↔ Đại Lý, 1 ải Thiên Bảo Khố
map 395). **Chủ game tự restart** sau mỗi đợt.

---

## 1. ITEM TÍN SỨ — ĐÃ GIẢI QUYẾT XONG ✅

Quét toàn bộ `task/tollgate/messenger` + `item/messenger` bản Linux: **chỉ 9 item id được dùng thật**.
Đối chiếu sang dự án **bằng TÊN** (không bằng số — bẫy "lệch 1 chỉ số" đã ghi trong memory):

| Linux id | Tên | **Dự án id** | Ảnh `.spr` | Ghi chú |
|---|---|---|---|---|
| 402 | Thần bí Đại Hồng Bao | **401** | — | lệch −1 |
| 885 | Tín Sứ Mộc yêu bài | **884** | `\spr\item\task\item_xinshimu.spr` | **trùng đường dẫn ảnh** |
| 886 | Tín Sứ Đồng yêu bài | **885** | `…item_xinshitong.spr` | trùng |
| 887 | Tín Sứ Ngân yêu bài | **886** | `…item_xinshiyin.spr` | trùng |
| 888 | Tín Sứ Kim yêu bài | **887** | `…item_xinshijin.spr` | trùng |
| 889 | Ngự Tứ Tín Sứ yêu bài | **888** | `…item_xinshiyuci.spr` | trùng |
| 2566 | Hành Hiệp Lệnh | **2575** | `\spr\item\script\xingxialing.spr` | trùng |
| 2812 | Tín Sứ Bảo Rương | **3430** | `\spr\item\script\xinshibaoxiang.spr` | trùng |
| 2813 | Thiên Bảo Khố Lệnh | **3431** | `\spr\item\script\tianbaokuling.spr` | trùng |
| 30229 | Chân Nguyên Đơn (Đại) | **4847** | `\spr\item\kinhmach\channguyendon.spr` | bản kinh mạch của dự án |

Nhóm phụ (Ngũ Hành Phù dùng trong ải, gọi từ `\script\item\` ngoài cây messenger):

| Linux | Tên | Dự án |
|---|---|---|
| 2806 | Ngũ Hành Phù | **3424** |
| 2807…2811 | Triệt Kim / Mộc / Thuỷ / Hoả / Thổ Phù | **3425…3429** |

**Kết luận: KHÔNG PHẢI LÀM THÊM ITEM NÀO, KHÔNG PHẢI VẼ ẢNH NÀO.**
14/14 item đã có sẵn trong `settings\item\magicscript.txt` của dự án, **đường dẫn `.spr` trùng
từng ký tự** với bản Linux ⇒ hình ảnh trong game sẽ giống hệt.

**Việc phải làm:** đổi 9 id trong script khi chép sang + **bind cột `Script` (cột 10)** của 5 yêu bài
và 6 phù về `\script\item\messenger\toll_*.lua` (hiện đang là `0` = chưa gắn).

> Công cụ tái lập: `ReverseTools/item_remap.py <thư-mục-Linux>` — trích mọi `(6,1,N)` rồi tra ngược
> theo tên sang bảng dự án. Dùng lại được cho mọi đợt sau (bang hội, boss bang hội…).

---

## 2. LÔI ĐÀI BANG HỘI — trạng thái

Script đã port (trùng 9/10 tệp từng byte), map 213-220 + region data đủ, timer 16/17 + mission 9
đã khai. **0 hàm engine thiếu, 0 item cần.**

**Ba điểm nối dây đang bị comment tắt:**
| Tệp : dòng | Nội dung |
|---|---|
| `script/startgame/thon/balanghuyen.lua:80` | `-- AddNpcNew(373,…,"\script\tinhnang\loidai\vebinhdautruong.lua",…)` |
| `script/timerserver.lua:76` | `-- sukien_loidaibanghoi(nDyfW,nHr,nMi)` |
| `script/item/lenhbaiadmin.lua:25` | `-- Include(".../loidai/lib_loidai.lua")` |

**Phải gỡ (bản tự viết trùng chức năng):** `script/tinhnang/loidai/` + các điểm gọi trong
`missions/mission06.lua`, `timertask/task06.lua`, `timerserver.lua:743-770`.

**Đang điều tra (workflow `wf_e9950759-e3b`):** NPC `manager.lua` / `camper*.lua` của bản Linux được
đặt lên map bằng cách nào — bản Linux **không có `AddNpc` nào** trỏ tới chúng, nên phải xác định
cơ chế trước khi nối dây.

**Sửa điều kiện theo chính sách cấp 90:**
| Tệp : dòng | Hiện tại | Đổi thành |
|---|---|---|
| `missions/citywar_arena/camper.lua:81` và `:87` | `GetJoinTongTime() >= 7200` (5 ngày) | bỏ, thay `GetLevel() >= 90` |

---

## 3. NHẬT KÝ THI CÔNG

| Thời điểm | Việc | Trạng thái |
|---|---|---|
| 21/08 | Chốt phạm vi + chính sách cấp 90 / bỏ trùng sinh | ✅ |
| 21/08 | Đối chiếu item Tín Sứ (14/14 có sẵn, 0 phải làm thêm) | ✅ |
| 21/08 | Điều tra NPC placement / Chỉ Nam Nhiệm Vụ / boss bang hội / thuế thành | ⏳ đang chạy |
| — | Gỡ `tinhnang/loidai`, bật `citywar_arena` | ⬜ |
| — | Gỡ `tinhnang/thienbaokho`, chép cây `messenger` | ⬜ |
