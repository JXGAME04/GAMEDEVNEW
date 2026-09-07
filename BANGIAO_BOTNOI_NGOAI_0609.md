# BÀN GIAO 06/09 chiều — Bot chia đường NỘI / NGOẠI công: bật lại đường nội, chốt Thiên Vương + Đường Môn luôn cầm vũ khí

Chủ (06/09 ~14:40): *"tìm hiểu phần chia bot theo nội ngoại - hiện tại đa số bot là ngoại công"* · *"do phần add vũ khí cho bot - bot không có vũ khí là nội công nhưng hiện tại đa số bot là có vũ khí nên không đánh skill nội"* · *"riêng bot hệ kim thiên vương thì toàn bộ là ngoại công phải có vũ khí"* · *"riêng bot hệ mộc đường môn toàn bộ phải có vũ khí mới đánh được"* · *"nên lưu ý 2 phái đó tránh xóa vũ khí nhầm dẫn tới đánh không được kỹ năng"*.

Nhánh `botnoi-0609` (worktree `D:\GAMEDEVNEW_wt_botnoi`), build từ `origin/main` 50e19ac3 (đã gộp relay + vận tiêu) + một commit, đẩy thẳng `origin/main` theo luật 04/09.

## 1. Vì sao "đa số bot là ngoại công" — đo được, không đoán

Thiết kế 28/08 (chủ chốt "random tỉ lệ bot 2 đường nội - ngoại cho cân bằng; NỘI CÔNG là KHÔNG cần vũ khí"): `dwID` lẻ = đường NỘI (tay không, `pb_PickSkill` ưu tiên chiêu có đòn phép), `dwID` chẵn = NGOẠI (cầm vũ khí). Ba chỗ quyết định vũ khí đều đi qua `pb_BotNoiThat()`: phát vũ khí nhập môn (`pb_GiveFactionWeapon`), tháo vũ khí lúc đăng nhập / lên cấp (`pb_TrangBiTheoCap` bước 3b), miễn "phát lại vũ khí khi tay không" (khối 10 giây trong `pb_DriveBot`).

| # | Gốc | Bằng chứng |
|---|---|---|
| 1 | `#define PB_BAT_DUONG_NOI 0` từ [NOI-HOAN 31/08] → `pb_BotNoiThat()` luôn trả 0 → đường nội **chưa bao giờ chạy** (31/08 tôi tự ngắt vì "chủ chưa duyệt"). | `grep -c "\[BotNoi\]"` = **0** trên 330 MB `bot.log` + `bot.log.1` (05-06/09). |
| 2 | Khối "phát lại vũ khí khi tay không" (10 s/lần, giãn 60 s, trần 5 lần/phiên) bốc lại pool ngẫu nhiên → bot bốc trúng "đường quyền" (`PB_WPN_NONE`) vẫn bị ép cầm vũ khí; xác suất còn tay không sau 5 lần ≈ 3 % (pool 50/50). | 05/09: **31** dòng `duong QUYEN, khong nhan vu khi` nhưng **69** dòng `tay khong giua doi -> phat lai vu khi`. |
| 3 | 1000 bot đang chạy đều là bot cũ nạp lại, cấp 114-118, `nGaveWeapon = 1`, vũ khí nằm sẵn trong blob → chỉ bước 3b mới tháo được, mà 3b bị gốc 1 chặn. | `[BotLuu]` = 4000 dòng (4 lần restart × 1000). |

Hệ quả đo được:
- `[BotDiem]` (chia điểm tiềm năng, `bNoiCong = không cầm vũ khí`): **1395/1395** lần đều mẫu NGOẠI `SM=2 TP=1 SK=2` hoặc Đường Môn `TP=3 SK=2`; **0** lần mẫu NỘI `SM=1 TP=0 SK=4`, **0** lần Võ Đang nội `NC=3`.
- `[BotDanh]` "chiêu N": 336 / 359 / 355 / 372 / 328 / 321 / 45 / 342 / 368 / 339 / 319 — toàn chiêu 90 **cần đúng họ vũ khí** (rank 2 thắng chiêu −2). Chiêu nội 90 chỉ lác đác: 357 (199), 362 (54), 337 (19), 380 (7); 318 / 365 / 375 = **0**.

## 2. Phái nào thật sự có chiêu nội tay không (quét dữ liệu thật)

`ReverseTools/quet_noi.py` chạy đúng bộ lọc của `pb_CoChieuNoiTayKhong` (ngũ hành khớp, tầm > 0, không aura/self, nhắm địch, kiểu Missles/Melee, EqtLimit −1/−2, đủ cấp, có đòn magic/cold/fire/lighting — độc bị loại theo 303-DOC) trên `skills.txt` × `SKILLNORMAL` + `SKILL90_ARRAY` (`factionhead.lua`), bot cấp 110:

| Phái | Chiêu nội tay không | Kết luận |
|---|---|---|
| 0 Thiếu Lâm | 271 Long Trảo (rq 50), 318 Đạt Ma Độ Giang (rq 80) — cả hai **EqtLimit −1 = PHẢI tay không** | có đường nội (đường quyền) |
| 1 Thiên Vương | không | **giữ vũ khí** — đúng lời chủ |
| 2 Đường Môn | **351 Loạn Hoàn Kích** (chiêu 90, EqtLimit −2, có đòn hỏa/băng/lôi) | ⚠️ nếu chỉ dựa bộ quét, DM cấp ≥ 80 **sẽ bị tháo vũ khí** → phải **chặn cứng** (mục 3). Ghi chú 31/08 "TV/DM an toàn" đã lỗi thời vì chiêu 90. |
| 3 Ngũ Độc | chỉ đòn độc (67/70/64/356/73/72/390) → bị loại 303-DOC | giữ vũ khí (xem câu hỏi 2) |
| 4 Nga Mi | 80 / 82 / 91 / 380 (−2) | có đường nội |
| 5 Thúy Yên | 102 / 113 / 111 / 337 (−2) | có đường nội |
| 6 Cái Bang | 122 / 128 / 357 (−2) | có đường nội |
| 7 Thiên Nhẫn | 145 / 138 / 148 / 362 (−2) | có đường nội |
| 8 Võ Đang | 153 / 164 / 165 / 365 (−2) | có đường nội |
| 9 Côn Lôn | 179 / 182 / 375 (−2) | có đường nội |
| 10 Hoa Sơn, 11 Vũ Hồn, 12 Tiêu Dao | không có chiêu tay không nào | giữ vũ khí (hiện 0 bot) |

## 3. Sửa — `Sources/Core/Src/KPlayerBot.cpp`, 4 hunk (`ReverseTools/goi_va_botnoi_bat_0609.py`, idempotent, chỉ ASCII)

- **H1** `PB_BAT_DUONG_NOI 0 → 1` + hàm mới `pb_PhaiLuonCamVuKhi(nFaction)` = `nFaction == 1 || nFaction == 2` (Thiên Vương, Đường Môn) — chặn **cứng**, không phụ thuộc dữ liệu chiêu.
- **H2** `pb_BotNoiThat()`: sau kiểm `dwID` lẻ, kiểm `pb_PhaiLuonCamVuKhi(m_cFaction.m_nCurFaction)` **trước** khi quét chiêu → TV/DM không bao giờ đi đường nội ở cả ba chỗ quyết định vũ khí.
- **H3** Khối phát lại vũ khí: `!pb_BotNoiThat(nIdx)` chuyển xuống điều kiện **cuối** (chỉ quét chiêu khi bot đã tay không, đã quá 60 s, còn lượt) — ngữ nghĩa không đổi, bớt quét vô ích 1000 bot × 10 s.
- **H4** Log `[BotNoi] <tên> phai <phái> cap N: thao vu khi` ghi thêm tên phái để nghiệm thu "không có TV/DM".

Không đổi: tỉ lệ lẻ/chẵn ~50/50 (chốt 28/08); pool vũ khí nhập môn; thứ tự ưu tiên trong `pb_PickSkill`; tỉ lệ chia điểm; cơ chế bot Tống Kim / Dã Tẩu. Tệp `#ifdef _SERVER` toàn bộ → **chỉ máy chủ**, không cần swap client.

## 4. Sau restart chuyện gì xảy ra

- ~**340** bot (dwID lẻ của 7 phái Thiếu Lâm / Nga Mi / Thúy Yên / Cái Bang / Thiên Nhẫn / Võ Đang / Côn Lôn, 685 bot ÷ 2) trong 10 giây đầu sau đăng nhập chạy `pb_TrangBiTheoCap` (cổng `nTrangBiLevel = 0`) → bước 3b **huỷ vũ khí** (`RemoveItemIdx`) → `nAtkSkill = 0` → chọn lại chiêu tay không (`nWant = −1`: chỉ còn EqtLimit −1/−2, ưu tiên đòn phép) → chiêu nội 90 (337 / 380 / 357 / 362 / 365 / 375 / 318).
- Khối phát lại vũ khí **miễn** các bot này (`!pb_BotNoiThat` = 0) → không bị phát lại.
- Thiên Vương (115 bot) / Đường Môn (99 bot) / Ngũ Độc (101 bot): **không đổi gì**.
- Điểm tiềm năng của bot đang chạy đã tiêu hết theo mẫu ngoại (SM 50 %) → bot nội hiện có mang chỉ số ngoại; chỉ điểm từ cấp mới chia theo mẫu nội (`SM=1 TP=0 SK=4`, Võ Đang `NC=3 SK=2`). Engine có `KPlayer::ResetBaseAttribute` (KPlayer.cpp:4542, Lua `ResetBaseAttribute(type, n)`) để tẩy — **chưa làm**, chờ chủ (câu hỏi 3).
- Bot không uống bình mana (`[BotUong]` chỉ HP); chiêu nội tốn mana như chiêu 90 ngoại đang dùng, bộ lọc COST của `pb_PickSkill` đã so với trần mana nên không chọn chiêu không bao giờ trả nổi.
- **Lùi**: đặt `PB_BAT_DUONG_NOI 0` + build lại; vũ khí đã huỷ không thu hồi được nhưng khối phát lại tự phát vũ khí nhập môn cấp 1 trong ≤ 5 phút (lên cấp 10 ở lần đổi cấp kế qua bước 4).

## 5. Nghiệm thu (bot.log sau restart)

```bash
grep -c "\[BotNoi\]" bot.log
```
≈ 340 (một lần cho mỗi bot lẻ đủ điều kiện).
```bash
grep "\[BotNoi\]" bot.log | grep -c "Thien Vuong\|Duong Mon"
```
**PHẢI = 0**.
- `[BotVuKhi] ... tay khong giua doi -> phat lai` **không** xuất hiện cho các bot vừa bị tháo.
- `[BotDanh]` / `[BotCast]`: xuất hiện chiêu 337 / 380 / 357 / 362 / 365 / 375 / 318; không có `BI TU CHOI (eqt=-1` hàng loạt.
- Lên cấp: `[BotDiem]` có mẫu `SM=1 TP=0 SK=4` và Võ Đang `NC=3 SK=2`.
- Soát hồi quy: Tống Kim bot nội vẫn đánh (chiêu tay không có tầm — 337 tầm 60, 318 tầm 90); nếu `[BotDanh] ... 10 giay khong sut mau` tăng vọt ở 7 phái nội thì báo.

## 6. Nhị phân

| tệp | md5 | kích thước | swap |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | **d7c406dcf0662dc97c758af89093a31d** | 18.447.872 | tắt GameServer → `ChayGameServer.bat` |

Build từ `origin/main` 50e19ac3 + commit nhánh `botnoi-0609` (đủ nhãn bản live + bản `.moi` của phiên relay: `[BotTanHuong]`, `AUC_MsgTong`, `CL_Cong`, `[DECHONG]`, `[S13-TELE-CU]`, `[RELAY`, `UpdateBattleBoxAll`, `TKDich`, `SapNpc`, `st_ledger`, `[RoleChk2]`) **+** `[BotNoi]` (nhãn chỉ có khi đường nội được biên dịch thật). Bản `.moi` trước đó của phiên relay (f807aa12, 06/09 14:23) giữ lại tên `CoreServer.dll.moi.relay_f807aa12_1423`. `GameServer.exe.moi` (06/09 13:19) giữ nguyên. Không swap client.

## 7. Câu hỏi còn mở cho chủ

1. *"hệ kim Thiên Vương"* tôi hiểu = **chỉ Thiên Vương** (Thiếu Lâm vẫn có đường quyền 271/318 tay không). Nếu ý chủ là cả hệ Kim (Thiếu Lâm cũng luôn cầm vũ khí) → thêm `nFaction == 0` vào `pb_PhaiLuonCamVuKhi`, một dòng.
2. **Ngũ Độc** hiện không có bot nội vì chiêu tay không chỉ mang đòn độc (bị loại theo đo 303-DOC 30/08 "độc không bào mòn quái"). Muốn Ngũ Độc có đường nội thì phải cho phép đòn độc → cần chủ quyết.
3. Có **tẩy điểm** ~340 bot nội đang chạy (đang mang chỉ số ngoại) bằng `ResetBaseAttribute` không?

## 8. Liên quan
`BANGIAO_PHIEN_BOT_TK_3008.md` (máy trạng thái bot), `ReverseTools/goi_va_botnoingoai.py` (28/08 chia đường), `goi_va_botnoi_theophai.py` (28/08 TV/DM theo dữ liệu), `goi_va_noi_hoan.py` (31/08 ngắt), `goi_va_damageattrib_thua.py` (31/08 mảng thưa).

---

## 9. ĐỢT 2 (06/09 ~15:10) — chủ trả lời 3 câu hỏi mục 7

Chủ: *"thiếu lâm có đường quyền"* · *"ngũ độc phải có nội công"* · *"cho bot nội tẩy lại điểm và tăng điểm lại"*.

### 9.1 Thiếu Lâm — giữ nguyên
Không chặn cứng Thiếu Lâm; bot lẻ đi đường quyền 271/318 (tay không) như đợt 1.

### 9.2 Ngũ Độc có đường nội — đòn ĐỘC không vật lý là chiêu nội (chỉ Ngũ Độc)
Dữ liệu thật (`wudu.lua`): chiêu tay không của Ngũ Độc đều IsPhysical = 0, EqtLimit −2, chỉ mang `poisondamage_v`:

| id | tên | rq | poison cấp 1 → 20 (sát thương / tick, số khung, khung / tick) | mana |
|---|---|---|---|---|
| 63 | Độc Sa Chưởng | 10 | 2 → 26 / 60 / 10 | 10 |
| 68 | U Minh Quỷ Lụy | 30 | 11 → 40 / 60 / 10 | 40 |
| 71 | Thiên Cương Địa Sát | 60 | 50 → 135 / 60 / 10 | 60 |
| 353 | Âm Phong Thực Cốt (chiêu 90, bot có cấp 20) | 80 | 20 → **121** / 60 / 10 | 30 → 80 |

Cơ chế độc trong engine (`KNpc.cpp` ~4940 đặt trạng thái, ~1487 tick): mỗi 10 khung trừ `nValue[0]` máu trong 60..120 khung (nhân hệ số 1..2 theo kháng độc), cộng `MixPoisonDamage` từ Ngũ Độc Chưởng Pháp (62). 353 cấp 20 ≈ 121 × 6..12 tick / lần ra chiêu → **sụt máu thật**. Kết luận [303-DOC 30/08] "độc không bào mòn quái" chỉ đúng với 303 của Đường Môn ở **cấp 1** (8 / tick) — không phải luật chung; Đường Môn vẫn giữ nguyên loại 303 vì đã chặn cứng không đi đường nội.

Sửa (`ReverseTools/goi_va_botnoi_dot2_0609.py`, H5–H7):
- `pb_DonDocNoi(p)` = `!IsPhysical && pb_DonDoc`; `pb_PhaiDocLaNoi(f)` = `f == 3` (Ngũ Độc).
- `pb_CoChieuNoiTayKhong(nNpcIdx, bDocLaNoi)`: với Ngũ Độc, chiêu độc không vật lý tính là chiêu nội → `pb_BotNoiThat` = 1 cho bot lẻ Ngũ Độc → tháo vũ khí ở 3b như 7 phái kia.
- `pb_PickSkill`: Ngũ Độc **không** loại `DOCTHUAN`, và `bNoi` tính cả đòn độc → bot lẻ tay không chọn 353 (rq 80 cao nhất; 63/68/71 dự phòng theo cấp). Bot chẵn cầm đao vẫn chọn 355 Huyền Âm Trảm (rank khớp vũ khí thắng).

### 9.3 Tẩy điểm + chia lại cho bot nội — `pb_TayDiemBotNoi` (H8)
- Gọi ở `pb_TrangBiTheoCap` bước **3c**, ngay sau 3b, chỉ khi bot là nội thật (`pb_BotNoiThat`) **và đã tay không**.
- Nhận biết "đang mang chỉ số ngoại" bằng **tỉ lệ** điểm đã tiêu (điểm gốc hiện tại − bảng gốc): phái thường `SM ≥ 35 %` (mẫu nội 20 %, ngoại 50 %); Võ Đang `NC < 50 %` (mẫu nội 70 %, ngoại 0 %). Sau khi tẩy, tỉ lệ về đúng mẫu → **không lặp lại** mỗi restart / lên cấp (không so bằng tuyệt đối vì chia 5 điểm / cấp làm tròn khác chia một cục).
- Tẩy đúng đường của game: bảng gốc `as[ngũ hành]` của `chuyensinhdaisu.lua:122` / `lenhbaitanthu.lua:212` = {Sức mạnh, Sinh khí, Thân pháp, Nội công}: Kim {35,25,25,15}, Mộc {20,35,20,25}, Thủy {25,25,25,25}, Hỏa {30,20,30,20}, Thổ {20,15,25,40}; đặt qua `KPlayer::ResetBaseAttribute` (KPlayer.cpp:4542 → `ResetBase*` đặt tuyệt đối + tính lại HP/MP/công/thủ + đồng bộ); quỹ = `(cấp − 1) × 5 + điểm giữ chuyển sinh` y hệt `LuaSetBasePoint` (ScriptFuns.cpp:10695; máy chủ cộng `PLAYER_LEVEL_ADD_ATTRIBUTE` = 5 mỗi cấp, KPlayer.cpp:61 / :2690).
- Rồi `pb_AllocAttribPoints` chia lại cả quỹ (tay không → mẫu nội: 20 % SM / 10 % TP / 70 % SK; Võ Đang 70 % NC / 30 % SK) và `nAtkSkill = 0` để chọn lại chiêu theo trần mana mới.
- Log: `[BotTayDiem] <tên> phai <phái> cap N: tay diem (da tieu SM=.. SK=.. TP=.. NC=..) -> ve goc he X, chia lai P diem theo duong NOI`.
- Ví dụ bot cấp 115 (570 điểm): trước SM 228 / TP 114 / SK 228 → sau SM 114 / TP 57 / SK 399 (HP tăng ~ +171 × máu/điểm); Võ Đang: NC 399 / SK 171.

### 9.4 Nghiệm thu bổ sung (sau restart)
```bash
grep -c "\[BotTayDiem\]" bot.log
```
≈ số bot nội (≈ 340 + ~50 Ngũ Độc lẻ); mỗi bot **một** dòng, restart lần sau phải là **0** dòng mới.
```bash
grep "\[BotNoi\]" bot.log | grep -c "Ngu Doc"
```
≈ 50 (bot lẻ Ngũ Độc bị tháo vũ khí); `Thien Vuong|Duong Mon` vẫn **0**.
- `[BotDanh]`/`[BotCast]` có chiêu 353 (Ngũ Độc tay không); `[BotDiem]` lần lên cấp kế của bot nội theo mẫu nội.

### 9.5 Nhị phân đợt 2
| tệp | md5 | kích thước | swap |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | **e86f3de72e727f1fdfcb0c07866d38ec** | 18.448.896 | tắt GameServer → `ChayGameServer.bat` (thay bản đợt 1 d7c406dc) |

Nhãn kiểm: đủ nhãn đợt 1 + `[BotTayDiem]`, `[NGUDOC-NOI]` không phải chuỗi; kiểm `duong NOI` (2) + `BotTayDiem` (1).

---

## 10. ĐỢT 3 (06/09 tối) — 4 việc mới của chủ

Chủ: *"tôi muốn viết thêm hàm nâng mạch cho toàn bộ bot"* · *"bot cấp 110 sẽ cho mặc ngựa chiếu dạ"* · *"bot có vũ khí sẽ cho random tỉ lệ nMagicLevel từ 7 - 8"* · *"bot lên 120 sẽ có skill 120 full skill"*.

Bản vá: `ReverseTools/goi_va_botnoi_dot3_0609.py` (C++: `KPlayerBot.cpp` H9–H14, `KPlayerBot.h`, `ScriptFuns.cpp`) + `goi_va_botnoi_dot3_lua_0609.py` (Lua: `hocvocong.lua`, `simcity_admin.lua` — áp cho **cả** cây chạy thật `bin\server\script` **và** gương git `serverscript_live\script`, hai bên bằng nhau byte-đối-byte; `kiem_54.py` 0 lỗi).

### 10.1 Hàm nâng kinh mạch cho toàn bộ bot — `PB_NangMach(nCap [, nMach])`
- Hệ kinh mạch: 12 mạch × 32 cấp (`MAX_MERIDIAN` / `MAX_MERIDIAN_LEVEL`, `meridian_level.txt` 384 hàng); người chơi xung huyệt qua `setmeridian.lua` (chỉ tốn tài nguyên, **không** gate theo cấp nhân vật).
- Hàm C mới đi đúng đường `SetMeridian` của Lua (`ScriptFuns.cpp:315`): `m_cMeridian.setMeridian` (mã mạch đếm từ 0) → `ApplyMaridianToNPC` / `RemoveMaridianFromNPC` (đếm từ 1, [KM 27/08b]) → `UpdataCurData`; xong gọi `PB_SaveAll` để ghi blob (cấp mạch nằm ở `szStringduphong2`, `KPlayer.cpp:3103` áp lại lúc đăng nhập) → **bền qua restart**. Trả số bot có mạch đổi; log `[BotMach] nang mach M (0 = ca 12) len cap C: n/N bot doi`.
- Đăng ký Lua `PB_NangMach` (`ScriptFuns.cpp` cạnh `PB_SaveAll`). **Menu lệnh bài** (`simcity_admin.lua` → Bot người chơi thật → *Nâng kinh mạch bot*): cấp 32 / 24 / 16 / 8 / xoá (0); bấm là áp cho mọi bot đang sống. Gọi tay: `PB_NangMach(32)` hoặc `PB_NangMach(16, 3)` (chỉ Xung mạch).
- **Chưa** tự động theo cấp bot (chủ chưa chốt mức); muốn tự động thì thêm khoá web `BOT_MACH_CAP` vào `bot_auto.lua` (mỗi phút gọi `PB_NangMach`, rẻ vì mạch đã đúng thì không làm gì) — chờ chủ chốt cấp.
- Lưu ý sức mạnh: memory 01/09 đo full mạch (sau khi cắt bảng 01/09) ≈ ×14 sức mạnh nhân vật → bot mạch 32 mạnh hơn hẳn bot hiện tại; chủ chọn mức phù hợp người chơi.

### 10.2 Bot cấp 110 cưỡi Chiếu Dạ — `pb_TrangBiTheoCap` bước 3
- `horse.txt` dòng 61: *Chiếu Dạ Ngọc Sư Tử* = detail 10 (`equip_horse`), **particular 5, cấp 10** (dòng 56 là bản cấp 5; dòng 130 *Hoàng Kim* particular 12 không dùng). Túc Sương = particular 2.
- Cấp < 110: Túc Sương cấp 10 như cũ; **cấp ≥ 110**: Chiếu Dạ cấp 10. Ngựa cũ (không Hoàng Kim) bị **tháo huỷ** rồi mặc ngựa mới — kiểm `CanEquip` ngựa mới **trước** khi huỷ, đang cưỡi thì `CheckRideHorse(TRUE)` xuống ngựa trước. Log `[BotTrangBi] … cuoi ngua Chieu Da Ngoc Su Tu cap 10`. 1000 bot hiện tại (114-118) sẽ đổi ngựa ngay lần `pb_TrangBiTheoCap` đầu sau restart.

### 10.3 Vũ khí bot có dòng cấp 7-8 — `pb_MagicVuKhi`
- Mảng `nMagicLevel[MAX_ITEM_MAGICLEVEL]` của `ItemSet.Add` → `Gen_Equipment` → `Gen_MagicAttrib` (`KItemGenerator.CPP:593`): ô `[i]` (i < 6) = **cấp dòng thuộc tính** thứ i (1..10, tra bảng magicattrib theo tiền/hậu tố, loại, hệ, cấp; 0 = dừng). Bot trước đây truyền toàn 0 → vũ khí **trắng**.
- Nay `pb_MagicVuKhi`: 6 ô đầu = 7 hoặc 8 ngẫu nhiên từng ô (tương ứng bậc "Hoàng Kim"/"Bạch Kim" của bảng), 6 ô sau 0. Áp cho vũ khí nhập môn (`pb_GiveFactionWeapon`) và vũ khí cấp 10 ở bước 4.
- Bot đang chạy: vũ khí cấp 10 cũ **không có dòng** (`GetTotalMagicLevel() == 0`) được sinh lại **một lần** cùng loại/hệ/cấp với dòng 7-8 (sau đó tổng dòng > 0 → không lặp). Log `[BotTrangBi] … len vu khi cap 10 (detail d parti p, dong 7-8 tong T)`. Bot nội (tay không) không liên quan.

### 10.4 Bot cấp 120 học kỹ năng 120 — `bot_hoc120`
- `SKILL120AR` (`hocvocong.lua:817`) = **một** chiêu/phái: 709 708 710 711 712 713 714 715 716 717 (10 phái), 1365 Hoa Sơn, 1984 Vũ Hồn, 2127 Tiêu Dao; đều là bị động / tự buff (style 3/2, rq 120, MaxLevel 20) — không phải chiêu đánh nên không đụng `pb_PickSkill`.
- `bot_hoc120(nCurFac)`: `AddMagic(SKILL120AR[nCurFac], 20)` = cấp 20 = **max** ("full"), giống dòng `show_kynang90` cấp cho người chơi. Không kèm 210 khinh công / `SKILL150_ARRAY` (chủ nói "skill 120"; muốn thêm 150 thì một dòng).
- C++: bước 1c `pb_TrangBiTheoCap`: `nLevel >= 120 && !b.nHoc120` → `ExecuteScript(hocvocong.lua, bot_hoc120, phái+1)` → log `[BotSkill120]`. Bot hiện cao nhất 118 → chưa bot nào kích hoạt.

### 10.5 Nhị phân đợt 3
| tệp | md5 | kích thước | swap |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | **4ce83dd57edd1cb714f464e855c3a0b3** | 18.458.112 | tắt GameServer → `ChayGameServer.bat` (thay bản đợt 2 e86f3de7) |

Build từ `origin/main` e41c0262 (đã rebase, gồm cả các commit vận tiêu / WAuto TK / lua54 của các phiên khác) + nhánh `botnoi-0609`. Nhãn kiểm mới: `[BotMach]`, `[BotSkill120]`, `bot_hoc120`, `Chieu Da Ngoc Su Tu`, `dong 7-8 tong`, `PB_NangMach` (mỗi nhãn 1) + đủ nhãn đợt 1-2. Script Lua đã ghi thẳng cây chạy thật (`hocvocong.lua` nạp lúc boot → hiệu lực sau restart; menu lệnh bài dùng ngay nhưng báo "chưa có PB_NangMach" cho tới khi swap).

### 10.0 Kết quả đo THẬT của đợt 1 + 2 (bản live 3cb5a6a3 từ 16:01, restart 15:47 / 16:01 / 18:04)
Bản `CoreServer.dll` đang chạy (md5 3cb5a6a3, phiên lua54 build từ origin/main sau 15:20) đã có `[BotNoi]` + `[BotTayDiem]`. `bot.log` sau 15:48:
- `[BotNoi]` **383** bot bị tháo vũ khí: Thiếu Lâm 44 · Nga Mi 47 · Thúy Yên 52 · Cái Bang 40 · Thiên Nhẫn 61 · Võ Đang 58 · Côn Lôn 45 · Ngũ Độc 36; **Thiên Vương / Đường Môn = 0** ✔. Không lặp ở restart 18:04 (một lần/bot) ✔.
- `[BotTayDiem]` **378** bot tẩy điểm; `[BotDiem]` sau đó đúng mẫu nội: `SM=114 TP=57 SK=399` (570 điểm), Võ Đang `NC=399 SK=171` ✔.
- Chiêu nội đã được dùng (`[BotDanh]` sau 15:48): 337 Thúy Yên **750** · 365 Võ Đang 276 · 357 Cái Bang 256 · **353 Ngũ Độc 225** · 362 Thiên Nhẫn 191 · 380 Nga Mi 178 · 375 Côn Lôn 159 · 318 Thiếu Lâm 126 (trước 15:48 gần như 0). Chiêu vũ khí của bot ngoại vẫn chạy (45 / 355 / 336 / 328 / 359 / 372 …).
- `tay khong giua doi -> phat lai` = **0**; `[BotCast] BI TU CHOI` = **0** ✔.
- `ScriptError.log` sau 18:07 chỉ có 1 lỗi của `npc_lmbiaoche.lua` (vận tiêu, phiên khác) — không liên quan bot.

### 10.5b ĐỢT 4 — sửa "vũ khí bot 7 dòng" (chủ chụp Kim Cô Bổng cấp 10 có 7 dòng, 19:1x)
Chủ: *"vũ khí add cho bot có 7 dòng thuộc tính trong khi đồ xanh mặc định nhiều nhất 6 dòng thuộc tính"*.
- **Gốc:** `MAX_ITEM_MAGICATTRIB` của dự án = **8** (`GameDataDef.h:40`), không phải 6 như tôi giả định; `pb_MagicVuKhi` đợt 3 lặp tới hằng đó → điền 8 ô → `Gen_MagicAttrib` (bản 7 tham số, lặp tới `MAX_ITEM_MAGICATTRIB`) sinh tới 8 dòng (ảnh: 7 dòng, một ô không tìm được dòng khác loại). "tong 45" trong log = `GetTotalMagicLevel` chỉ cộng 6 ô đầu.
- **Gốc phụ:** `g_Random(2)` là LCG `seed*IA+IC % 2` → trả 0,1,0,1… → mọi vũ khí đều 7,8,7,8,7,8 (143/143 "tong 45") — không ngẫu nhiên.
- **Sửa** (`ReverseTools/goi_va_botnoi_dot4_0609.py`, 2 hunk): `PB_VK_SO_DONG 6` — `pb_MagicVuKhi` điền đúng 6 ô, ô 7-8 = 0 (bộ sinh dừng ở ô thứ 7); ngẫu nhiên lấy bit 8 của `g_Random(65536)` (mẹo băm sẵn có trong tệp). Bước 4: vũ khí đã lỡ 7-8 dòng (`nGeneratorLevel[6]` hoặc `[7]` > 0 — chỉ bản 18:53 sinh ra) được sinh lại **một lần** 6 dòng (`pb_VuKhiQuaDong`).
- Nhị phân đợt 4: `bin\server\CoreServer.dll.moi` gộp vào bản đợt 4 (mục 10.5e), thay bản 4ce83dd5 đang chạy. Nghiệm thu: `grep "dong 7-8 tong" bot.log` sau restart ≈ 143+ dòng mới, tổng dòng 42..48 **thay đổi** (không còn toàn 45); vũ khí bot xem trong game đúng 6 dòng.

### 10.5c ĐỢT 4b — KHẨN: "bot ném đồ ra lúc đổi đồ, nhặt vào hiển thị lỗi item, out vào mất" (19:1x–19:3x)
Chủ: *"bug làm mất đồ người chơi thật? bạn kiểm tra ngay"* · *"bot ném đồ ra lúc đổi đồ thì tôi nhặt vào hiển thị lỗi item rồi tôi out ra vào lại thì mất đồ"*.

**Kiểm tra mã bot có đụng đồ người thật không:** mọi lệnh huỷ/thay đồ chỉ chạy trong `pb_DriveBot` cho khe bot đã kiểm `Player[nIdx].m_dwID == b.dwID` (khe cấp lại cho người khác thì bỏ); soát toàn bộ bot.log 06/09: **0** tên ngoài danh sách bot trong các dòng `[BotNoi]` (384) / `[BotTayDiem]` (379) / `[BotTrangBi]` (776) / `[BotVuKhi]` (346) / `[BotDiem]` / `[BotSkill90]`. `PB_NangMach` thiếu kiểm dwID → đã thêm (H22).

**Gốc "ném đồ" (KItemList.cpp):** bot mặc đồ bằng `InsertEquipment` + `Equip`. `InsertEquipment` (~4603) khi túi không còn dải ô: món khoá thì thử kho, rồi đặt món vào **TAY** (`pos_hand`) và **NÉM MÓN ĐANG Ở TAY xuống đất** thành Object công khai. `Equip` (1252) đổi `nPlace` sang `pos_equip` nhưng **không xoá `m_Hand`** → `m_Hand` trỏ vào món **đã mặc**. Lần `InsertEquipment` sau (bước 4 vũ khí ngay sau bước 3 ngựa; hoặc phát lại vũ khí 60 s × 5): `Remove(m_Hand)` = **tháo + ném ngựa/vũ khí đã mặc xuống đất**, `AddKIL(pos_hand)` trả 0 vì `m_Hand != 0` → món mới không vào danh sách → bot tay không; `m_Hand` vẫn lệch → mỗi lần phát lại lại ném tiếp `Item[m_Hand]` — chỉ số cũ có thể **đã cấp cho món của người khác** → người chơi nhặt được "item lỗi", out vào mất; nguy cơ dính chỉ số món của người thật (thấp nhưng có thật). Đo 19:04: **346 bot tay không** (đúng = số bot vừa nhận ngựa mà túi đầy), 1.531 lượt phát lại, 1.049 `dang cam=0`; 299 bot không có dòng ngựa = 200 bot sạp + 99 bot ở trạng thái khác chưa chạy `pb_TrangBiTheoCap` (không phải lỗi). Lỗi này **có từ 18/08** (Kim Phong/ngựa/vũ khí cấp 81 đều qua đường này) nhưng chỉ bùng khi đợt 3 đổi ngựa + vũ khí cho 1.000 bot cùng lúc.

**Sửa (`goi_va_botnoi_dot4b_0609.py`, 8 hunk):**
- `pb_MacVaoNguoi` viết lại: **chỉ đặt vào ô túi** (`CheckCanPlaceInEquipment` + `AddKIL(pos_equiproom)`) → `Equip` → nhả ô lưới; **không bao giờ** qua `InsertEquipment`/tay; hết chỗ → `pb_LamChoTui` xoá rác túi (cùng bộ lọc giữ của `pb_DonTui`) rồi thử lại; mọi thất bại có log `[BotMac]` kèm yêu cầu món + chỉ số bot.
- `pb_DonChoMac`: kiểm đủ điều kiện + có chỗ **trước** khi huỷ món cũ (bước 3 ngựa, bước 4 vũ khí, vũ khí nhập môn); tay đang giữ rác thì huỷ, `m_Hand` lệch thì chỉ báo.
- `pb_GiveFactionWeapon` và sạp tạo hàng: bỏ `InsertEquipment`, dùng đường trên. Trong `KPlayerBot.cpp` không còn lời gọi `InsertEquipment` nào.
- Sau restart: 346 bot tay không tự nhận lại vũ khí nhập môn rồi lên cấp 10 (6 dòng) trong 10 giây; đồ đã rơi trên đất mất khi restart (là đồ bot, khoá). Người chơi đã nhặt "item lỗi" thì món đó mất — đó là đồ của bot, không phải đồ của họ.

### 10.5d ĐỢT 4c — "bot vào bang người chơi hiển thị thành bang chủ"
- **Gốc:** bản sao relay JX2 (`KTongJX2.h:24`) dùng chức vụ 0 bang chủ / 1 trưởng lão / 2 đội trưởng / 3 bang chúng / 4 ẩn sĩ; còn `KPlayerTong::m_nFigure` (đồng bộ lên mọi client qua `PlayerSync.TongFigure`, `KNpc.cpp:6705`; relay cho người thật cũng dùng enum này) là enum JX1 `GameDataDef.h:1616`: 0 MEMBER / 1 MANAGER / 2 DIRECTOR / 3 MASTER. `pb_BangDongBo` chép thô `t.m_nFigure = btFigure` → bot bang chúng (3) = MASTER = "Bang chủ"; bang chủ bot (0) = MEMBER. bot.log: 2.952 dòng "chức vụ 3" của bang TESTGAME.
- **Sửa** (`goi_va_botnoi_dot4c_0609.py`): đổi bảng 0→MASTER, 1→DIRECTOR, 2→MANAGER, 3/4→MEMBER trước khi gán và so sánh; log ghi cả hai mã. Bot đang ở TESTGAME tự đồng bộ lại trong ≤ 15 phút sau restart (hoặc ngay khi bản sao relay đổi).

### 10.5e Nhị phân đợt 4 (gộp 4 + 4b + 4c)
| tệp | md5 | kích thước | swap |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | **62313d60e45ce37dc6c8267c9bfb515e** | 18.460.672 | tắt GameServer → `ChayGameServer.bat` (thay bản 4ce83dd5 đang chạy — bản này còn ném đồ) |

Nhãn kiểm: đủ nhãn đợt 1-3 + `[BotMac]` (9), `chuc vu JX2` (1); `InsertEquipment` chỉ còn 1 (dòng include). **Không restart bằng bản 4ce83dd5 nữa** (mỗi restart tái kích 5 lượt ném đồ cho 346 bot).

### 10.6 Nghiệm thu đợt 3 (sau restart)
```bash
grep -c "cuoi ngua Chieu Da" bot.log
```
≈ số bot cấp ≥ 110 có vũ khí hoặc không (mọi bot) ≈ 1000, mỗi bot một lần.
```bash
grep -c "dong 7-8 tong" bot.log
```
≈ số bot ngoại (~600), tổng dòng ≥ 42 (6 dòng × 7..8).
- Lệnh bài → Nâng kinh mạch bot → cấp 32: `grep "\[BotMach\]" bot.log` một dòng `n/1000 bot doi`, sau đó `[BotLuu] xep hang luu 1000 bot`; restart lại thì mạch còn nguyên (kiểm `GetMeridian` hoặc xem chỉ số bot).
- Khi bot đầu tiên lên 120: `[BotSkill120]` một dòng/bot.
