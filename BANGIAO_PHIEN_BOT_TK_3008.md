# BÀN GIAO PHIÊN 28→30/08 — ĐỒNG BỘ TOẠ ĐỘ + BOT/TỐNG KIM (đọc file này TRƯỚC TIÊN)

> Chi tiết từng đợt: `BANGIAO_DICHUYEN_GIATLUI_2508.md` mục **9.28 → 9.49b**.
> File này là bản rút gọn để phiên sau vào việc ngay.

---

## 1. TRẠNG THÁI BINARY (kiểm lúc 30/08 ~16:45)

| Đâu | Bản trên đĩa | Tiến trình đang chạy | Việc cần |
|---|---|---|---|
| `bin\server\CoreServer.dll` | **`d3486293`** (30/08 16:42) | GameServer khởi động 16:27 = bản **`7f0909fc`** | 🔴 **CẦN RESTART GameServer** |
| `bin\client\CoreClient.dll` | **`d53d10a6`** (30/08 16:25) | Game.exe khởi động 16:27 | ✅ đã relog, đang chạy đúng bản |

**Cách kiểm nhanh bản trên đĩa có đủ vá không** (đừng tin md5 trong bàn giao cũ — hai phiên Claude build chung cây, bản mới nhất luôn ôm cả hai):
```
grep -c DOCTHUAN bin/server/CoreServer.dll      # 1 = có vá 303 (mới nhất phía server)
grep -c S6-GANNHANH bin/client/CoreClient.dll   # 1 = có vá gắn-lại-nhanh (mới nhất phía client)
```
Backup theo tên `CoreServer.dll.cu_<ngày>_truoc_<tên vá>_<md5 cũ>`.

---

## 2. CHUỖI SCRIPT TÁI ÁP (file dùng chung CHƯA COMMIT — chạy ĐÚNG THỨ TỰ)

`KProtocolProcess.cpp`, `KProtocol.cpp`, `KNpc.cpp`, `KNpcSet.cpp`, `KPlayerBot.cpp` bị hai phiên
dùng chung nên **không commit**. Mọi script trong `ReverseTools/` đều **idempotent** (chạy lại vô hại,
báo lỗi to nếu neo trượt), latin-1, CRLF-aware:

**Nhánh đồng bộ toạ độ (client):**
`goi_va_S10_dichthat.py` → `goi_va_S10_ma.py` → `goi_va_S11_chongma.py` → `goi_va_S12_bung8o.py`
→ `goi_va_S12b_cuaso_vanh.py` → `goi_va_S12c_autothang.py` → `goi_va_S6_gannhanh.py`

**Nhánh bot/TK (server, `KPlayerBot.cpp`):**
`goi_va_botthp_toado.py` → `goi_va_botnoingoai.py` → `goi_va_botnoi_theophai.py` → `goi_va_datau_thp.py`
→ `goi_va_sap_raideu.py` → `goi_va_tk_ket_hangdoi.py` → `goi_va_tk_san_doithu.py` → `goi_va_tk_san_tran10.py`
→ `goi_va_tk_san_tanra.py` → `goi_va_tk_chongcamp.py` → `goi_va_tk_chia_hoaluc.py` → `goi_va_tk_censu_cuu.py`
→ `goi_va_tk_tan_nhom.py` → `goi_va_tk_chia_map.py` → `goi_va_botcast_repick.py` → `goi_va_dm20_donthuong.py`
→ `goi_va_303_docthuan.py`

**Build:** `Server Release|x64` + `Client Release|Win32` (file dùng chung ⇒ build CẢ HAI).
MSBuild: `Core/Core.vcxproj -p:SolutionDir="D:\GAMEDEVNEW\Sources\\"`. Output server ra
`x64\ServerRelease\` (KHÔNG space).

---

## 3. ĐÃ XONG — NGHIỆM THU ĐẠT BẰNG SỐ

### 3.1 Đồng bộ toạ độ nhân vật (chuỗi S9→S12c) — mục 9.28, 9.44, 9.45
- **S12b**: mở cửa sổ nghe-lệnh NGAY tại cú hạ cánh (`[S12-CUA]`) + phân biệt echo bằng đích đã tự gửi.
- **Nghiệm thu 9.44**: thang búng 8 ô **15 → 2 cú đơn lẻ**; 3 cú phù về **0 búng**; `[S12-TELE]` đo được
  trong `jx_auto_server.log`; flap NPC chớp tắt **179-208 → 10**.
- **S12c (9.45)**: cú "nhảy bậy" còn lại = **giằng co giữa lệnh dắt của server và WAuto** (bypass sống
  suốt 3s). Sửa: bypass chỉ hiệu lực khi auto **chưa tự gửi lệnh nào kể từ lúc mở cửa sổ** ⇒ auto lên
  tiếng là auto thắng.
- **S6-GANNHANH (9.46)**: bản sao (bot/NPC/người chơi) mồ côi do recenter phải chờ vòng sync server
  **trung vị 8,4 giây** mới hiện lại ⇒ "biến mất rồi hiện lại chỗ khác". Sửa: mồ côi mà server vừa
  sync ≤2s + còn trong 38 ô + region đã nạp lại ⇒ **gắn lại ngay trong 1 tick**.

### 3.2 Bot — toạ độ & hành vi (mục 9.30-9.34, 9.42-9.43)
- **Toạ độ chuẩn = Thần Hành Phù** (`script/item/ib/shenxingfu.lua`): bảng `s_aThpDiem` 27 map +
  `pb_ThpDiem()` trong KPlayerBot. Áp cho: gác bot login đứng ô chặn (`[BotTHP]`), điểm đáp báo danh TK
  (Tống 1541,3178 / Kim 1570,3085 — lấy từ `battle_transprot`), phù về Dã Tẩu (đáp **trung tâm thành**
  rồi ĐI BỘ tới NPC).
- **Sạp rải đều (9.33)**: gốc = `g_nPbNpcChan` mặc định 0 làm phép kiểm "ô đã có sạp" thành no-op +
  chỉ duyệt 8 tia + bot ở sẵn thành ngồi tại chỗ. Sửa: tự quét danh sách sạp, duyệt trọn vành đai,
  cờ `nSapChoXong`.
- **Nội/ngoại công (9.31 + 9.34)**: `pb_BotNoiThat` = dwID lẻ **VÀ** phái thật sự có chiêu phép đánh
  tay không (quét bằng chính bộ lọc pb_PickSkill). Thiên Vương/Đường Môn tự về đường ngoại, giữ vũ khí.
- **Bot test tự động xoá sạch bot (9.29)**: `partner_test_bdh.lua` của phiên kia gọi `PB_ClearBot()`
  mỗi phút qua `timerserver.lua` — **đã comment 3 dòng `call(BDH_TestTick/TalkTick/PetTick)`**.
  Muốn chạy lại bộ test: bỏ 3 dấu `--`.

### 3.3 Tống Kim (mục 9.35-9.43)
| Vá | Gốc đo được | Cách chữa |
|---|---|---|
| 9.35 mass-quit | 85/250 phe Tống "KET pha 3" oan vì đồng hồ 120s không tính hàng-đợi-đang-tiến | nhúc nhích ≥6 ô = làm tươi đồng hồ; rải điểm Quân Y ±8 ô |
| 9.36 `[BotSan]` | bot chạy toạ độ cố định, không tìm địch | quét `Player[]` phía server (không giới hạn khoảng cách), đuổi địch **sống** gần nhất, tái định vị ~1,2s, bỏ xác |
| 9.37 trần 10 | cả đàn dồn 1 nạn nhân | bảng đếm `s_nTkSanDem` + dựng lại 1Hz trong `pb_TkNhip` |
| 9.38 tản + KET2 | đuôi hàng đợi A* đứng im >120s | hậu doanh = vùng an toàn cho đồng hồ; tiết lưu re-path ≥3s |
| 9.39 chống camp | bot hai phe cắm cửa trại địch, người ra là chết trong 10-16s | vùng cấm săn: quanh hậu doanh 45 ô + cụm điểm ra cửa 25 ô của phe địch |
| 9.40 chia hoả lực | ~500 mạng/phút ⇒ 40% quân số luôn trong trại (cảnh "kẹt trại") | bộ nhắm chia trong **nhóm 4 gần nhất** thay vì gần-nhất-tuyệt-đối |
| 9.41 census/cứu | ~16-25% bot báo danh xong không bao giờ ra (**có từ TRƯỚC mọi vá TK**) | `[TkCensus]` 10s/lần mổ pha-3; `[TkCuu3]` SetPos thoát ô bị chặn |
| 9.42 `[BotTan]` | >20 đồng phe dồn cục, tuyến sau vô dụng | quét 12 ô, >20 con thì tuyến sau bỏ mục tiêu gần, toả cánh khác |
| 9.43 chia map | 25 ứng viên gần nhất đều trong CÙNG cụm ⇒ vẫn 1 ổ | phổ ứng viên **25→60**, bán kính né đám **20→60 ô** |

### 3.4 Bot đánh nhau — chiêu thức (mục 9.47-9.49b)
- **`[CAST-LECH]` (9.47)**: 83k cú "chiêu bị từ chối"/ngày = bot hệ kiếm bốc trúng đường quyền
  (tay không) nhưng giữ chiêu kiếm cũ ⇒ đấm gió vĩnh viễn. Sửa: chiêu lệch vũ khí ⇒ chọn lại ngay.
- **`[DM20]` (9.48)**: nếu KHÔNG còn chiêu phái nào đủ cấp ⇒ **tự học + dùng đòn đánh thường id 1**
  (rq=0, mọi vũ khí). Lưới an toàn cho cấp <10. 🔴 **LUẬT CHỦ: CẤM sửa `skills.txt`** — mọi fallback
  phải viết bằng code.
- **`[303-DOC]` (9.49 + 9.49b) — GỐC THẬT của "ĐM cấp 20 không đánh được"**:
  - Chiêu **303 "Độc Thạch Cốt"** (chiêu ĐM, rq=20, mọi vũ khí) chỉ có **độc DOT**, và đo thật
    **199k cú/ngày đánh quái HP600 không sụt một giọt máu** ⇒ trên build này **độc không bào mòn quái**.
  - Bot luôn chọn nó vì bậc phụ **rqTier** (rq20 > rq10) đè lên **id 45 "Phích Lịch Đạn"** — chiêu ĐM
    **cấp 10, dùng MỌI vũ khí (phi đao/phi tiêu/tụ tiễn), sát thương vật lý thật**, mà bot **đã học sẵn**
    (`SKILLNORMAL[3]` dòng đầu).
  - Sửa: loại chiêu **phép độc-thuần** khỏi ứng viên + bỏ poison khỏi phép thử "phái có nội".
  - Đã soát cạn: sau khi bỏ 303, ứng viên hợp lệ duy nhất ở cấp 20 chính là **45** (305/306 nhắm bản
    thân; 49 không nhắm địch; 347 lệch hệ; 47/50/54 đòi cấp 30). Cấp 30 lên Đoạt Hồn Tiêu/Truy Tâm
    Tiễn/Mãn Thiên Hoa Vũ khớp họ vũ khí.

---

## 4. VIỆC PHIÊN SAU LÀM NGAY (theo thứ tự)

1. 🔴 **Restart GameServer** (bản `d3486293` đang chờ) rồi nghiệm thu **một lượt** cả cụm:
   - ĐM cấp 20: `grep "dung chieu 45" bot.log` phải có; "khong sut mau ... chieu 303" về ~0.
   - `grep BotCast bot.log | grep -c "BI TU CHOI"` phải tụt mạnh (trước 83k/ngày).
   - TK: `KET o pha 3` ~0 · `[TkCuu3]` đếm số con gỡ khỏi ô chặn · `[TkCensus] pha3=` rút về ~0 giữa
     trận · "da chet"/phút < ~200 · `[BotSan] cach N o` có nhiều dòng N>100 · `[BotTan]` nổ khi vón cục
     · nhìn trận ≥3-5 ổ đánh nhau.
   - Sạp: `grep "ngoi sap" bot.log` mỗi con một ô khác nhau.
2. **Nếu ĐM cấp 20 vẫn không đánh được sau restart**: đọc `[BotChon]` (chỉ in khi chọn hụt) và kiểm
   `grep "dung chieu" | grep "cap 2[0-9]"`. Khả năng còn lại: chiêu 45 cast được nhưng sát thương thấp
   ⇒ khi đó là chuyện cân bằng dữ liệu, phải hỏi chủ (KHÔNG tự sửa skills.txt).
3. **Còn treo, chờ chủ duyệt**:
   - **Độc không bào mòn quái** ở tầng engine (ảnh hưởng cả người chơi Ngũ Độc/ĐM) — đụng gameplay.
   - **Đồng bộ lưới vật cản map 379** (server 298k ô vs client 24k): phương án đặt tệp LOOSE cạnh pak
     (`KPakFile` đọc đĩa trước pak), phải xoá cache `.fp` hai bên. Tool đo:
     `ReverseTools/tk_luoi_client_vs_server.py`.
   - Lỗi phụ: `[S2-SKILL-NOTLEARNED]` lặp 1,3s/lần (mục 9.7).

---

## 5. BẪY SỐNG CÒN (đã trả giá trong phiên này)

- 🔴 **Heredoc bash nuốt backslash** — viết/sửa script vá phải dùng **Write tool**, không dùng
  `cat <<EOF` cho nội dung có `\n`, `\t`.
- 🔴 **Neo phải CRLF**: file nguồn dùng `\r\n`; script vá tự quy đổi (`crlf` flag) — giữ nguyên khuôn đó.
- 🔴 **Phiên Claude kia swap DLL liên tục** (có ngày 5-6 lần): **luôn `md5sum` bản đang nằm NGAY TRƯỚC
  khi swap** và đặt tên backup theo md5 THẬT, đừng chép md5 từ ghi chú cũ.
- 🔴 **Phiên kia cũng ghi đè `MEMORY.md`** — nội dung chi tiết nằm trong các file memory riêng và
  BANGIAO; đừng tin mỗi dòng index.
- 🔴 **CẤM sửa `skills.txt`** (và dữ liệu cân bằng nói chung) để chữa lỗi bot — fallback phải bằng code.
- 🔴 **CẤM tự tắt/restart GameServer** khi chưa được phép; build + swap xong thì báo chủ.
- **Nhãn tiết chế cấm đếm**: `NET-RUN`, `E4_*`, `S9-DICH`, `FIGHT-DIST` (`AUTOLOG_EVERY`).
  Đếm được: `S6-*`, `S7-*`, `S10-*`, `S11-*`, `S12-*`, `S8-NAN`, và mọi nhãn `[Bot*]` trong bot.log.
- **Đồng hồ**: `SubWorld[0].m_dwCurrentTime` là FRAME ~18/s và bị gán lại theo server — cửa sổ ms phải
  dùng `timeGetTime()`.
- **Quy trình đã ăn khách cả chuỗi**: đo log → giả thuyết → **phản biện bằng mã thật** → sửa tối thiểu
  → nghiệm thu bằng số trước/sau. Ba lần trong phiên này chẩn đoán đầu tiên SAI và chỉ log mới chỉ ra
  gốc thật (mass-quit, "kẹt trại", ĐM cấp 20) — **đừng bỏ bước đo**.
