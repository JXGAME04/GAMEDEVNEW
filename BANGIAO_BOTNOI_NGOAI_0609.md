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
