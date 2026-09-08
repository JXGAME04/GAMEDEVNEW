# PHƯƠNG ÁN KIẾN TRÚC — hiệu ứng, RAM client, tick máy chủ (08/09/2026, 10:30)

Chủ 10:1x: *"tối ưu càng ngày càng tệ đi — cần phương án tốt hơn, không phụ thuộc vào những gì mã nguồn có."*
Tài liệu này là phương án **thiết kế lại** ba mảng, không phải vá tiếp. Mọi số nền lấy từ trận Tống Kim 09:22–09:42 08/09
(`BANGIAO_BANGTHONG_DONGNGUOI_0609.md` 8.26, `BANGIAO_HIEUUNG_KHAOSAT_0709.md` 7.3, `BANGIAO_RAM_CLIENT_0809.md`).

---

## 0. Ba sự thật phải nói trước

1. **Giật máy chủ 09:17–09:28 KHÔNG đến từ game và KHÔNG đến từ mã mới.** Trong 12 phút đó log máy chủ có CÙNG mật độ với phút
   thường (225 vs 213 dòng/s: cùng số chiêu, đạn, sát thương; bot.log cùng ~3.100 dòng/phút suốt 09:13–09:41), nhưng TICK, SW_ACTIVATE,
   LUA_CALL và phần còn lại đều chậm **~3,5× đồng đều** (8 → 24–31 ms) rồi về 8 ms trong một phút — dấu hiệu tiến trình máy chủ bị
   **tranh CPU/RAM** với thứ khác trên cùng máy, không phải một pha nào nặng lên. Đã loại trừ: tôi không chạy lệnh nào 08:56–09:24
   (transcript), không có build nào ghi tệp 09:15–09:29 trên D:\GAMEDEVNEW* / J:\CayChay, không có sự kiện Defender/Update. Máy
   24 CPU / 32 GB nhưng MemCompression 2,6 GB, MsMpEng (Defender) 94 luồng, SQL Express + Reporting Services chạy nền — chưa chỉ được
   thủ phạm. Hai phút trễ 04:21/04:25 thì **đúng là tôi**: trùng khớp lúc chạy `do_ram_client.py` (quét toàn bộ nhớ Game.exe).
   Luật mới: **không build, không quét log/RAM khi trận đang chạy** (`feedback-khong-build-khi-tran-dang-chay`); và thêm vào dòng
   `[PERF]` chỉ số **CPU tiến trình nhận được / thời gian tường** (GetProcessTimes) + % idle toàn máy để lần sau phân biệt ngay
   "game nặng" với "bị tranh CPU" (1 build, 0 rủi ro).
2. **Máy chủ thật sự khoẻ:** 1.000 bot + người chơi, tick 8 ms (ngân sách 55 ms), SW_ACTIVATE 5,5 ms, đường gửi 0,01 ms/nhịp.
   Không có việc gì cấp bách phía máy chủ.
3. **Hiệu ứng và RAM là hai bài toán KIẾN TRÚC**, không phải bộ đếm: client hiện đang **tự mô phỏng lại** chiêu và đạn
   (hoạt ảnh, khung 60 %, hồi chiêu, bể đạn, người phóng, mục tiêu) rồi hy vọng trùng máy chủ — mỗi chỗ lệch là một hiệu ứng mất.
   Và texture đang nằm **hai lần trong RAM** (bản của game + bản sao của driver D3D9).

---

## 1. HIỆU ỨNG — "máy chủ nói gì, client vẽ nấy" (sự kiện thay cho mô phỏng)

### 1.1 Vì sao 3 % chiêu "chưa rõ" — và vì sao chủ nói "skill buff" là có cơ sở

Client đếm `start` khi nhận gói 95 và **xếp lệnh** `do_skill`; lệnh đó vào `KNpc::DoSkill` (client) và có thể **bị từ chối im lặng**
ở 6 chỗ chưa có bộ đếm nào (`KNpc.cpp:3027-3110`):

| Chỗ từ chối | Khi nào lệch với máy chủ |
|---|---|
| `m_Doing == do_skill \|\| do_hurt` (đang bận) | client còn trong hoạt ảnh trúng đòn trong khi máy chủ đã xong |
| `!m_FightMode` (người chơi) | cờ chiến đấu client chưa kịp đồng bộ (đã gặp 07/09) |
| `m_SkillList.CanCast(id, t)` — **hồi chiêu** | client tự ghi `SetNextCastTime` cho **cả NPC khác** lúc bắn ở khung 60 % (`OnSkill`), tức muộn hơn máy chủ đúng độ trễ mạng + lệch nhịp → chiêu kế tiếp của bot **đúng lúc hết hồi** tới client khi "còn hồi" → **từ chối**. Chiêu buff/hộ thể hồi lâu, bot dùng lại ngay khi hết hồi → chịu nặng nhất. **Đây là cơ chế khớp với nhận định "skill buff" của chủ.** |
| `CanCastSkill` trả 0 (tầm, mana, khoá) | client không biết trang bị/tâm pháp giảm tiêu hao |
| `Cost(...)` thất bại | mana client lệch |
| `SendCommand` đè lệnh cũ | hai gói 95 tới trong một nhịp → lệnh trước mất |

Kiểm chứng (nếu chủ muốn số trước khi làm 1.2): một build client đếm 6 nhánh trên **theo mã kỹ năng** (top 10) — 1 giờ + 1 trận.
Nhưng dù nguyên nhân là nhánh nào, thiết kế 1.2 xoá **cả lớp** lỗi này.

### 1.2 Thiết kế: sự kiện "ĐÃ BẮN" từ máy chủ + lớp hiệu ứng tách khỏi bảng NPC

| Bước | Máy chủ | Client | Xoá được |
|---|---|---|---|
| **A1. Gói `s2c_skillfired`** phát đúng lúc `KSkill::Cast` thật sự chạy (14 B: id NPC, id kỹ năng, cấp, mục tiêu hoặc toạ độ, số thứ tự) | 1 chỗ trong `OnSkill` máy chủ | nhận → **bắn hiệu ứng ngay**, không phụ thuộc client đang ở khung nào, bận hay hồi chiêu; chiêu của chính mình: client vẫn đoán trước, đối chiếu số thứ tự để không bắn 2 lần | hurt < 60 %, chết, huy_sync, 6 nhánh từ chối ở 1.1, khung 60 % so bằng, `nTotalFrame = 1` |
| **A2. Lớp hiệu ứng riêng** (`KHieuUng`): đạn/hiệu ứng là **vật thể hình ảnh** có toạ độ, hướng, mục tiêu tuỳ chọn, thời gian sống; không cần ô NPC người phóng còn sống; mất mục tiêu → bay tới vị trí cuối; bể cỡ theo ngân sách vẽ (8.000, cấu trúc nhẹ ~100 B) | — | thay vòng đời `KMissle` client (giữ nguyên mã vẽ sprite) | `ownerlost` 3.262/7 phút, `tgtlost`, bể đạn 500/3000 (không còn liên quan luật chơi) |
| **A3. Hiệu ứng trúng đòn theo gói sát thương 222** (đã có, 14 % byte) | — | sprite trúng đòn nổ đúng nạn nhân, đúng lúc máy chủ tính | đạn client tới sớm/muộn, nổ sai chỗ |
| **A4. Buff theo trạng thái đồng bộ 223** | — | icon/sprite buff bám trạng thái máy chủ; nới 6 ô/loại nếu cần | `buff_het_o`, buff hiện sai |

Băng thông: ~45 sự kiện/s quanh một người ≈ 0,6 KB/s mỗi client (đường 10 Gbps, không đáng kể). Gói 95 giữ nguyên để tư thế
vung chiêu vẫn bắt đầu sớm.

**Kết quả cam kết đo được:** client `fire` = máy chủ `ban` (94 %) — tức 100 % chiêu máy chủ bắn đều có hình; chỉ còn độ trễ mạng.
**Công:** máy chủ 0,5 ngày; client 3–4 ngày (handler + lớp hiệu ứng + đối chiếu chiêu của mình); test 2 trận.
**Rủi ro:** trung bình — chạm mã vẽ đạn client; đổi giao thức (máy chủ test, chủ đã cho phép). Có cổng `[Client] HieuUngSuKien=0` lùi về cũ.

---

## 2. RAM CLIENT — "texture ở VRAM, không ở RAM"

### 2.1 Số nền (đo thật)
RAM riêng = ~180 MB nền + 103 MB mảng tĩnh + ~140 MB heap + **texture × (1 + 0,8 bản sao driver)**. Cache 570 MB → 690 MB; đỉnh 816 MB.
D3D9Ex **không** bỏ được bản sao (đo 08/09 09:11–09:35). Chủ chốt: không giảm trải nghiệm (không 16-bit, không hạ cache).

### 2.2 Bốn đòn bẩy, xếp theo lợi/công

| | Việc | RAM giảm | Công | Rủi ro |
|---|---|---|---|---|
| **B0** | **Thí nghiệm quyết định**: chương trình thử riêng tạo 700 MB texture bằng **D3D11** trên máy chủ này, đo RAM riêng. Nếu D3D11/WDDM 2 không giữ bản sao → B1 đáng làm; nếu vẫn giữ → bỏ B1, làm B2+B3 | — | 1 ngày | 0 |
| **B1** | Lớp vẽ Represent3 trên **D3D11** (sprite 2D: texture + quad + chữ + vài shader; swapchain flip) | **−0,8 × texture** (≈ −450 MB ở cache 570) → client ~300 MB dù cache 1,5 GB; VRAM 4 GB dư | 1–2 tuần | trung bình-cao: chữ, UI, fullscreen, mọi đường vẽ; cần cổng lùi Rep3 D3D9 |
| **B2** | **Texture bảng màu A8L8 2 B/điểm + bảng màu 256 + pixel shader** tra bảng (point sampling = vẽ 1:1 như DirectDraw gốc) — **không mất một màu nào** | texture −50 % → RAM −250 MB (và bản sao driver −250) ở cache 570 | 1–2 ngày | thấp-trung bình: test UI/chữ/hiệu ứng; chạy được trên D3D9 hiện tại, giữ nguyên khi lên D3D11 |
| **B3** | Mảng tĩnh: lưới đường 2,4 triệu ô cấp **theo bản đồ đang đứng**; trường chỉ máy chủ dùng của `KNpc` bọc `#ifdef _SERVER` | −60…−90 MB | 1–2 ngày | thấp: đụng A* xuyên map (`jx1-luoi-astar-son-dac-dat-that`), phải test Dã Tẩu/bản đồ thế giới |
| **B4** | Heap 140 MB: đo bằng heap tracing (WPA/UMDH) rồi cắt theo hệ (script, âm thanh, bảng) | chưa biết | 1 ngày đo | 0 |

**Lộ trình đề xuất:** B2 + B3 ngay (3–4 ngày) → client ~400–450 MB trong trận nặng; B0 chạy song song; nếu B0 dương → B1 → ~300 MB.

---

## 3. MÁY CHỦ — không có việc cấp bách; chỉ quy trình

- Tick 8 ms / 1.000 bot là khoẻ. Không tối ưu thêm khi chưa có số vượt 20 ms **từ chính game**.
- Quy trình: (1) không build/quét nặng khi trận chạy; (2) mỗi thay đổi kèm bảng trước/sau cùng loại trận; (3) một thí nghiệm một lần;
  (4) bộ đếm đã đủ — dừng thêm bộ đếm, chuyển sang thiết kế.
- Khi nào cần: > 2.000 bot hoặc SW_ACTIVATE > 20 ms thật → đo pha con, rồi "bot nghĩ luân phiên" (mỗi bot quyết định mỗi N nhịp, so le)
  trước khi nghĩ tới đa luồng.

---

## 4. Việc chờ chủ quyết

| # | Câu hỏi | Nếu đồng ý tôi làm |
|---|---|---|
| Q1 | Làm **1.2** (sự kiện "đã bắn" + lớp hiệu ứng)? Có cần bước kiểm chứng 1.1 (1 build, 1 trận) trước không? | A1 máy chủ + client cổng lùi, đo trận đầu: `fire` phải = `ban` |
| Q2 | RAM: bắt đầu **B2 + B3** ngay, và cho chạy **B0** (thí nghiệm D3D11 1 ngày) để quyết B1? | B2 trước (nhìn thấy ngay −250 MB), B3 sau |
| Q3 | Goddess.exe.moi (xếp hạng) vẫn chờ đổi tay | — |
