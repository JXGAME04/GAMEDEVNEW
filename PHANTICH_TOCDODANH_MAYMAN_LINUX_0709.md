# TỐC ĐỘ ĐÁNH NGOẠI CÔNG & MAY MẮN — MỔ NHỊ PHÂN LINUX (`jx_linux_y`) SO VỚI DỰ ÁN — 07/09/2026

Yêu cầu chủ game: (1) "mổ nhị phân kiểm tra công thức tính tốc độ đánh ngoại công bản Linux so sánh với bản dự án"; (2) "kiểm tra bản dự án đã có may mắn điểm và may mắn % chưa".

Công cụ như các đợt trước (`ReverseTools/mo_nhi_phan_0609/`, `dis_all.txt`, `fnfull.py`, `linux_magic_handlers.txt`). Chỉ phân tích, **chưa đổi mã**.

## 1. Kết luận nhanh

| | Kết quả |
|---|---|
| **Tốc độ đánh** | Công thức khung đòn **giống nhau về bản chất**: `khung = AttackFrame × 100 / (100 + tốc độ)`. Linux có 3 điểm JX1 không có: (a) `attackspeed_yan_v` là **kênh riêng**, tốc độ hiệu lực = **max(thường, yan)** — JX1 gộp cả hai vào một biến (**cộng dồn**); (b) Linux chặn mẫu số ≤ 0 → 1 và khung ≤ 0 → 1 ở **mọi** chỗ, JX1 chỉ chặn ở `DoSkill`, 3 chỗ `DoAttack/do_special1/jumpattack` không chặn (tốc độ ≤ −100 ⇒ chia 0); (c) gói đồng bộ client Linux gửi `max(thường, yan)`. Hiện dữ liệu JX1 **không còn dòng nào** dùng `attackspeed_yan_v`/`castspeed_yan_v` (0/1 606 skill, item chỉ còn tên trong MagicDesc.ini) và giá trị âm nhỏ nhất là −1 ⇒ hai lệch (a)(b) **đang ngủ**, không phải nguyên nhân gì hiện tại. Gốc `BASE_ATTACK_SPEED = 0` cả hai bản; cột `AttackSpeed` trong `npcs.txt` = số **khung** mỗi đòn (mặc định 20) cả hai bản. |
| **May mắn** | Theo `MagicDesc.ini` (hai bản giống nhau): **"May mắn: +N%" = `lucky_v`** (idx 135) — dự án **có**: handler `LuckyV` → `m_nCurLucky`, dùng khi rơi đồ. **"May mắn tổ đội: +N điểm" = `lucky_v_partner`** (Linux idx 188) — dự án **KHÔNG có**: tên không nằm trong `KMagicDesc.cpp`/enum (idx 188 của JX1 là `dec_percasttimehorse`), không handler ⇒ vật phẩm/kỹ năng mang thuộc tính này bị bỏ qua khi nạp. Ngoài ra cách **dùng** may mắn khi rơi đồ hai bản khác nhiều (mục 4). |

## 2. Tốc độ đánh — Linux

| Mục | Linux | Địa chỉ |
|---|---|---|
| Handler `attackspeed_v` (115) | `Npc.m_CurrentAttackSpeed (+0x1a34) += v0` | `0x08098CF0` |
| Handler `attackspeed_yan_v` (239) | `Npc.m_CurrentAttackSpeedYan (+0x1a38) += v0` — **trường riêng** | `0x08095FE0` |
| `castspeed_v` / `castspeed_yan_v` | `+0x1a3c` / `+0x1a40`, cùng kiểu | `0x08098CB0` / `0x08096000` |
| Khung đòn (DoAttack, do_special1, jumpattack, ProcCommand do_attack) | `s = max(atk, atk_yan); s += 100; if (s <= 0) s = 1; khung = AttackFrame(+0x1918) × 100 / s; if (khung <= 0) khung = 1; nCurrentFrame = 0` | `0x080787A9`, `0x080808A8`, `0x08084954`, `0x08088221` |
| Đồng bộ client | gửi `max(atk, atk_yan)` và `max(cast, cast_yan)` | `0x0809FE6C` |
| Gốc | người chơi: `+0x1628 = 0` (`0x080A801A`); NPC mẫu: 0 (`0x0807E00F`); lúc tính lại thuộc tính: cả hai trường = gốc (`0x0807EEFB`, `0x080829DD`); NPC còn cộng thêm cột `+0x20` của bảng cấp NPC (`0x0809DE9D`) — bảng này sinh từ `npclevelscript/property.lua`, hai bản đều **không có** hàm tốc độ ⇒ 0 |

## 3. Tốc độ đánh — JX1 (`KNpc.cpp`) và lệch

| Chỗ | JX1 | So với Linux |
|---|---|---|
| `KNpcAttribModify.cpp:81‑84` | `attackspeed_v` **và** `attackspeed_yan_v` cùng trỏ `AttackSpeedV` ⇒ `m_CurrentAttackSpeed += v0` cho cả hai | Linux: 2 trường, hiệu lực = max ⇒ nếu sau này có dữ liệu yan, JX1 **cộng dồn** (mạnh hơn Linux) |
| `KNpc.cpp:3117‑3121` (DoSkill vật lý) | `d = speed + 100; if (d <= 0) d = 1; f = AttackFrame×100/d; f -= f % 2; if (f <= 0) f = 1` | có chặn, thêm làm chẵn (JX1 riêng, vô hại) |
| `KNpc.cpp:3171` (DoAttack), `3268` (do_special1), `9124` (jumpattack) | `AttackFrame × 100 / (100 + speed)` **không chặn** | speed ≤ −100 ⇒ chia 0 (sập) / âm; hiện min dữ liệu −1 nên chưa xảy ra |
| Đồng bộ (`KNpc.cpp:6762/6930/7063`) | gửi `m_CurrentAttackSpeed` (tổng) | tương đương khi không có yan |
| `KPlayer.cpp:77` `BASE_ATTACK_SPEED 0` ("mặc định 20" là chú thích cũ) | 0 | Linux 0 — giống |
| `KNpcTemplate.cpp:86` cột `AttackSpeed` → `m_AttackFrame` (mặc định 20) | | Linux `+0x1918` cùng nghĩa — giống |

**Đề xuất (nếu chủ muốn, 1 lần build server + client)**: (1) thêm `m_CurrentAttackSpeedYan/CastSpeedYan`, handler yan ghi trường riêng, 4 chỗ tính khung + đồng bộ dùng `max`; (2) chặn `≤ 0 → 1` ở 3 chỗ còn thiếu. Hiện tại không có tác dụng vì dữ liệu không dùng yan, nên tôi **chưa làm**, tránh thêm một vòng `.moi` ngay trước lượt swap.

## 4. May mắn

### 4.1 Thuộc tính

| Tên | Mô tả trong `MagicDesc.ini` (hai bản) | Linux | JX1 |
|---|---|---|---|
| `lucky_v` (135) | **"May mắn: #d1+%"** | handler `0x0809A180` → `KPlayer::AddLucky` (`+0x5958 = m_nCurLucky`, `0x080A7C30`), chỉ người chơi | **có**: `KNpcAttribModify::LuckyV` → `m_nCurLucky` |
| `lucky_v_partner` (188) | **"May mắn tổ đội: #d1- điểm"** | handler `0x08095CF0`: chỉ NPC Kind 2 (bạn đồng hành/pet) có đối tượng đồng hành `+0x1698` ⇒ `float +0x68 += v0` (may mắn của bạn đồng hành) | **không có**: tên vắng trong `KMagicDesc.cpp` (JX1 326 tên, Linux 336); idx 188 của JX1 = `dec_percasttimehorse`; `KPlayerPartner.cpp`/`KPlayerPet.cpp` không có trường may mắn |
| may mắn dạng `_p` | không có ở cả hai bản (chữ `%` trong mô tả `lucky_v` chỉ là cách hiển thị) | | |

### 4.2 Cách dùng khi rơi đồ

| Bước | Linux `GenRandomItem` `0x08083BB0` | JX1 `KNpc.cpp` 9840‑9962 + `KItemGenerator`/`KItemSet` |
|---|---|---|
| May mắn tổng | `m_nCurLucky + số đồng đội đứng gần` (`0x080CC620`: đếm tối đa 7 thành viên tổ đội cùng bản đồ, khoảng cách ≤ 0xFFFFF) | `m_nCurLucky` (+2 mỗi cấp bang qua `ReCalcTongBenefit`, JX1 tự thêm); **không** có đồng đội |
| Cấp vật phẩm | `rand` đều trong `[min, max]` từ `nMinItemLevelScale/Max…` — **không** phụ thuộc may mắn | `g_RandPercent(luck/10)` → max; `g_RandPercent(luck/2+50)` → ngẫu nhiên; còn lại min (JX1 thêm) |
| Cấp dòng thuộc tính (magic level) | theo cấp quái + `rand`, kẹp 1‑10, số ô = `rand(4)+3` | bậc theo may mắn: 0‑10 → `rand(3)`; 11‑20 → ≥ 3; … > 60 → ≥ 8; mỗi ô lấp với xác suất `1 − 1/(2+luck)` (JX1 thêm) |
| Trong `Gen_MagicAttrib` | quality > 3: `nDecide = rand(1e6)×100 / (10×luck + 100)`; 1‑3: `rand(1e6) / (luck/5 + 1)`; ≤ 1: `rand(100)` (`0x0806AF70`) | `(100×g_Random(1000000)) / (10×nLucky + 100)` và `GetRandomNumber(0,99)/(1+nLucky×20/100)` — **cùng gốc Linux** |
| Nhân tuỳ chọn X (`genXOpt`) | không có | may mắn ≥ 11/21/31/41 ⇒ 10/30/50/80 % nhận `g_MaxOptMultiply` (JX1 thêm) |
| Nội suy giá trị dòng `nMax + (nMin−nMax)×luck/10` (kẹp 0‑10) | chưa thấy trong `0x0806AF70` (có thể ở `Gen_Equipment`) | có (`KItemGenerator.cpp:550‑574`) |

Kết luận may mắn: dự án **có "May mắn %" (`lucky_v`)** và dùng nó **mạnh hơn Linux nhiều** (cấp vật phẩm, bậc dòng, số dòng, nhân X); **thiếu "May mắn tổ đội điểm" (`lucky_v_partner`)** và thiếu cộng may mắn theo **đồng đội đứng gần** của Linux. Nếu chủ muốn có `lucky_v_partner`: thêm tên vào một ô `normal_reserve*` của enum JX1 (không đổi số thứ tự các thuộc tính khác ⇒ không đụng giao thức), handler cộng vào may mắn bạn đồng hành, và quyết định chỗ dùng (Linux cộng vào đối tượng đồng hành, chưa thấy Linux đọc lại giá trị đó trong `GenRandomItem`).

## 5. Việc chủ quyết

1. Tốc độ đánh: có làm gói (1)+(2) mục 3 không (hiện không đổi hành vi, chỉ phòng xa).
2. May mắn: có thêm `lucky_v_partner` và/hoặc cộng may mắn theo đồng đội gần như Linux không; có giữ các tác dụng may mắn JX1 tự thêm không.


---

**Đính chính 07/09 (đợt 2, xem `PHANTICH_TOCDODANH_LINUX_SAU_0709.md`)**: `0x08088221`/`0x0808818D` là đuôi `KNpc::DoSkill` (`0x08088150`), không phải ProcCommand; "bảng cấp NPC cột +0x20" là bảng npcgold cột `AttackSpeed/CastSpeed`; nhánh "style 14" thực ra là `CharAnimId == cdo_none (14)` ⇒ khung 0 (JX1 cũng vậy). Lệch có tác dụng thật là **khung gốc**: JX1 người chơi `m_CastFrame` kẹt 20 (Linux 18), `AttackFrame` 17 vs 18.
