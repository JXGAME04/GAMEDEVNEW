# NGA MY / THÚY YÊN — NỘI CÔNG ĐÁNH RA CÓ GÂY "BĂNG" (ĐÓNG BĂNG / LÀM CHẬM) KHÔNG? MỔ LINUX SO DỰ ÁN — 07/09/2026

Câu hỏi chủ game: "mổ nhị phân bản Linux xem phái Nga My – Thúy Yên nội công đánh ra đối phương có dính hiệu ứng băng sát không?"

Công cụ như các đợt trước (`dis_all.txt`, `fnfull.py`). Chỉ phân tích, **chưa đổi gì**.

## 1. Trả lời ngắn

**Có, nhưng chỉ khi đang bật tâm pháp 90 của phái.** Trong Linux, thời gian đóng băng của một đòn = `thời gian băng của chiêu + coldenhance_p của người đánh`. Toàn bộ bảng chiêu Nga My / Thúy Yên (cả nội lẫn ngoại, kể cả 150) **không có** thời gian băng riêng (`colddamage_v` chỉ khai `[1]` min và `[3]` max, thiếu `[2]`), nên:

| Tình huống | Linux | Dự án |
|---|---|---|
| Nội công Nga My/Thúy Yên **không** bật tâm pháp 90 | không băng (thời gian 0) | không băng |
| Nga My bật **Phật Pháp Vô Biên** (252, `coldenhance_p 8→37`) | băng **8→37 khung** (0,4–2,1 s) mỗi đòn nội trúng | giống Linux |
| Thúy Yên bật **Băng Cốt Tuyết Tâm** (114, `coldenhance_p`) | băng **10→140 khung** (0,6–7,8 s) | **8→80 khung** (0,4–4,4 s) — dữ liệu dự án thấp hơn |
| Đòn băng bị kháng hết (sát thương băng = 0) | vẫn băng | **không** băng (JX1 `CalcDamage` trả FALSE khi sát thương ≤ 0) |

Thời gian thực tế trên nạn nhân = `khung × (100 − min(freezetimereduce_p của nạn nhân, 77)) / 100`, chỉ đặt khi nạn nhân **chưa** đang băng, và nạn nhân có `ignorenegativestate_p` thì tung xúc xắc huỷ — ba luật này hai bản giống nhau (đã làm chuẩn đợt [BANGSAT 01/09] và [SK120 07/09]).

## 2. Engine Linux — đường đi của "băng"

| Bước | Linux | Dự án |
|---|---|---|
| `coldenhance_p` (chỉ số 161) | handler `0x08097FC0`: `KNpc+0x1430 += v0` | `KNpcAttribModify::ColdEnhanceP → m_CurrentColdEnhance` — giống |
| Gộp vào chiêu (`AppendSkillEffect 0x0807CE70`, helper ô băng `0x0807C950`) | nếu ô băng của chiêu là `colddamage_v` (0x3c) và `min > 0`: `đích.v0 = min; đích.v1 = KNpc+0x1430 + chiêu.v1; đích.v2 = max` — **cho mọi chiêu, nội lẫn ngoại**; ngoại còn gộp băng vũ khí `v1 = max(v1, +0x1430 + vũ khí.v1)`; nội chỉ cộng `addcoldmagic_v` (+0x121c/+0x1224) + nội công vào min/max, **không đụng v1** | `KNpc.cpp:5374‑5405`: `v1 = chiêu.v1 + m_CurrentColdEnhance` (5378), ngoại `max(...)` (5403) — giống (đã làm chuẩn `[HOASON 02/09g]`) |
| Đặt băng (`ReceiveDamage`, `0x0808A6D1 → 0x0808B1E0`) | sau ô vật lý và ô băng, nếu `m_FreezeState.nTime(+0x1d8) ≤ 0` và `v1 > 0`: `nTime = v1 × (100 − min(+0x1420, FreezeTimeReduceMax)) / 100`; rồi `ign% (+0x1474) > rand(100)` ⇒ log `IgnoreNegState(Freeze)…Ignore` và `nTime = 0`. **Không xét** sát thương băng có > 0 hay không (`CalcDamage 0x08089C90` trả 1 khi `min+max ≤ 0`, chỉ trả 0 khi mục tiêu chết/không hợp lệ) | `KNpc.cpp:4891‑4913`: giống công thức, nhưng nằm trong `if (CalcDamage(cold…))` và `CalcDamage` JX1 trả FALSE khi `nDamage ≤ 0` với người đánh là người chơi (`4182‑4186/4419‑4423/4457‑4462`, có báo "né") ⇒ không băng khi sát thương băng bằng 0 |
| Kiểu chiêu | không phân biệt `IsPhysical`/`IsMelee` cho việc băng | giống |

Ghi chú: `MagicDesc.ini` Linux ghi `coldenhance_p = "Kháng băng: +%"` (dịch sai), dự án ghi `"Thời gian đóng băng: +%"`; engine hai bản đều cộng **số khung** vào thời gian băng, không phải %.

## 3. Dữ liệu

### 3.1 Bảng chiêu (không có thời gian băng riêng ở cả hai bản)

`colddamage_v = { [1]={min}, [3]={max} }` ở mọi bảng Nga My / Thúy Yên: 80 Phiêu Tuyết Xuyên Vân, 82 Tứ Tượng Đồng Quy, 91 Phật Quang Phổ Chiếu, 111/112 Bích Hải Triều Sinh, 113 Phù Vân Tán Tuyết, 330 Thiên Phật Thiên Diệp, 331 Kim Đỉnh Phật Quang, 337 Băng Tâm Tiên Tử, 338 Phong Tuyết Băng Thiên, 380 Phong Sương Toái Ảnh, 381 Băng Tâm Ngọc Lăng, 382 Băng Tâm Tuyết Liên, 1062/1065/1090/1093/1102 (150) và các chiêu ngoại (85, 88, 99, 105, 108, 328, 336, 385, 1061/1063/1064/1089/1114). `GetSkillLevelData` (Lua, hai bản) điền `[2] = {{0,0},{20,0}}` khi thiếu ⇒ v1 = 0.

Khác nhỏ khác: 382 Băng Tâm Tuyết Liên dự án `IsPhysical=1` (Linux 0); 338 Phong Tuyết Băng Thiên dự án `{1,45}→{20,400}` (Linux `→600`); dự án có thêm 102 Phong Quyển Tàn Tuyết (`fengjuan_canxue`).

### 3.2 Tâm pháp 90 (nguồn duy nhất của thời gian băng)

| Bảng | Linux | Dự án |
|---|---|---|
| `fofa_wubian` (252 Phật Pháp Vô Biên) | `coldenhance_p {1,8},{30,37}`; + `lifemax_yan_p` (Dương) | `coldenhance_p {1,8},{30,37}` — **giống** |
| `binggu_xuexin` (114 Băng Cốt Tuyết Tâm) | `coldenhance_p {1,10},{30,140}` (hai dòng cũ bị chú thích 8→80, 9→110); `deadlystrikeenhance_p {1,5},{30,60,Conic}`; `fasthitrecover_yan_v {1,5},{30,49}` (Dương); `lifemax_yan_p`; `addcoldmagic_v {1,60},{30,315}`; `addcolddamage_v {1,30},{30,275}`; `addphysicsmagic_v {1,30},{30,275}` | `coldenhance_p {1,8},{30,80}`; `deadlystrikeenhance_p {1,5},{30,45,Conic}`; `fasthitrecover_v {1,5},{30,25}` **có trong Lua nhưng `skills.txt` không khai** ⇒ vô hiệu; `skills.txt` khai `sorbdamage_p` nhưng Lua **không có** ⇒ 0; `addcoldmagic_v/addcolddamage_v/addphysicsmagic_v` giống |

Không có vật phẩm/script nào khác cấp `coldenhance_p` ở cả hai bản (0 tệp ngoài thư mục kỹ năng).

## 4. Nếu chủ muốn "như Linux" (chưa làm)

1. Dữ liệu `binggu_xuexin` (server + client): `coldenhance_p 8→80 → 10→140`, `deadlystrikeenhance_p 45 → 60`; sửa cặp khai/dữ liệu `fasthitrecover_v` (Linux là bản Dương `fasthitrecover_yan_v {1,5},{30,49}` — dự án không dùng Dương thì khai `fasthitrecover_v` với giá trị Linux) và bỏ `sorbdamage_p` rỗng.
2. Engine: bỏ điều kiện "sát thương băng > 0" khi đặt băng (`KNpc.cpp:4891`) để băng cả khi bị kháng hết như Linux — ảnh hưởng nhỏ.


---

# 5. THI CÔNG (07/09, chủ chọn "làm như Linux") — commit `[BANGSAT2 07/09]` **34b58589** origin/main

| # | Việc | Nội dung | Như Linux |
|---|---|---|---|
| E1 | Engine `KNpc.cpp` (gói `ReverseTools/goi_va_bangsat_0709.py`, cùng mã CoreServer + CoreClient) | ô băng: `bBangTrung = CalcDamage(cold…)`; đặt băng khi `bBangTrung` **hoặc** mục tiêu còn hợp lệ (`m_Doing != do_death/do_revive`, `m_RegionIndex >= 0`, không phải xe tiêu `Owner`); thêm điều kiện `pTemp->nValue[1] > 0` | `ReceiveDamage 0x0808A6D1 → 0x0808B1E0`: băng không phụ thuộc sát thương băng > 0; `CalcDamage 0x08089C90` trả 1 khi `min+max ≤ 0`, trả 0 chỉ khi chết/hồi sinh/không hợp lệ. Giữ chặn riêng JX1 cho xe tiêu (không có ở Linux) |
| D1 | `cuiyan.lua` `binggu_xuexin` (server + client byte‑một, sao lưu `*.truoc_bangsat_0709`, gương `serverscript_live`) | `coldenhance_p 8→80 ⇒ 10→140`; `deadlystrikeenhance_p 45 ⇒ 60 (Conic)`; `fasthitrecover_v 5→25 ⇒ 5→49` | Linux `10→140`, `60`, `fasthitrecover_yan_v 5→49` (Dương gấp vào Âm vì dự án không dùng Dương, đúng khi đứng một mình) |
| D2 | `skills.txt` dòng 114 Băng Cốt Tuyết Tâm (server + client) | ô `LvlSetting6` `sorbdamage_p` (không có dữ liệu Lua ⇒ 0) ⇒ `fasthitrecover_v` (LvlData6 vẫn `binggu_xuexin`) | Linux khai 7 ô (thêm `lifemax_yan_p`, `fasthitrecover_yan_v`); `lifemax_yan_p` (Dương +21 % HP) **không** chép — ngoài câu hỏi băng, chủ quyết riêng |

Kiểm: `check_encoding.py` KNpc.cpp giữ số byte cao; `kiem_54.py` 2 tệp 0 lỗi; cuiyan.lua client = server. Cần **restart** (Lua/skills) và **swap `.moi`** (engine).

## 5.1 Kết quả mong đợi (Thúy Yên bật Băng Cốt Tuyết Tâm, nạn nhân không có giảm thời gian băng)

| Cấp buff | Trước (khung / giây) | Sau = Linux |
|---|---|---|
| 1 | 8 / 0,44 s | 10 / 0,56 s |
| 15 | 43 / 2,4 s | 73 / 4,0 s |
| 20 | 55 / 3,1 s | 94 / 5,2 s |
| 30 | 80 / 4,4 s | 140 / 7,8 s |

Nga My (Phật Pháp Vô Biên 8→37 khung) không đổi. Đòn băng bị kháng hết (sát thương 0, báo né) nay vẫn đóng băng như Linux.

**Trạng thái 10:42 07/09**: build từ origin/main `34b58589` (worktree `D:/GAMEDEVNEW_wt_ai710l`, compile 0 lỗi, link thật; gộp DELTA f `a6d62a3c`, CL `10a55613`): `bin/server/CoreServer.dll.moi` = **8008e14d** (18 482 176), `bin/client/CoreClient.dll.moi` = **7bcf5119** (2 611 712) — thay khe DELTA f (bc6b2b80/9916d00f, giữ tên `.moi.delta_f_*`); kiểm chuỗi `gon_them` (đợt f) và `CastFrame` (TOCDO) có trong khe. Phải swap **cả server lẫn client** (gói vị trí 28 byte của DELTA f). Live vẫn db5d064f/563c127e. Đã báo DELTA + MATDO.
