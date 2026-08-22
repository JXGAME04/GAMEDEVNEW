# PHÂN TÍCH LOG "ĐÁNH MISS" + "BỎ SÓT ĐỒ" — 21/08/2026 tối

Nguồn: `bin\server\jx_auto_server.log(.1)` (pid 26492, 82,4tr→86,3tr ms ≈ 65 phút, **đã lọc tên CaiBang** ở nhóm S1/S2/S3)
+ `bin\client\jx_auto.log(.1)` (6 lần vào game, pid cuối 33088). Bản chụp để đối chiếu lại nằm ở scratchpad phiên
(`srv1.log`, `srv_now.log`, `cli0.log`). Nhân vật: **CaiBang** = chỉ số NPC **91213**, pidx=1, lv 151, **ổn định cả phiên**.

Bối cảnh quan trọng mà log cho thấy (khác với mô tả "tự treo luyện công"):
- **Dã Tẩu đang BẬT và ở pha đánh quái** (`nDT=2`: 15.039/17.757 mẫu `DATAU-GATE`).
- **CaiBang đang có cờ TỔ ĐỘI** (`m_cTeam.m_nFlag≠0` — suy ra từ việc 174 yêu cầu nhặt đi vào nhánh tổ đội, không có dòng `SPICK-BELONG`).
- 38 phút đầu là PvP vào đám bot (8.332 lượt chiêu 363 chạm bot), 27 phút sau mới luyện công.
- Máy chủ đang chạy ~1000 bot; client chỉ có 256 khe NPC / 500 khe đạn.

## 1. ĐÃ CHỐT (>90%)

### 1.1 Đòn cận chiến vào quái KHÔNG BAO GIỜ TRƯỢT ở máy chủ
- `KNpc.cpp:3898-3913`: người chơi đánh cận chiến vào quái → **bỏ qua hẳn `CheckHitTarget`** (nhánh `S1-MELEE-NOROLL`).
- Đo: 1.144 đòn cận chiến vào quái (`usear=1 melee=1 autohit=1`) → **0 trượt**. Cả phiên chỉ **7/8.128** đòn trượt và đều là đánh vào bot (sàn 5% của `MAX_HIT_PERCENT=95`).
- 337/337 cặp `S1-PHYS-PRE/POST` vào quái: **máu giảm 100%**, trung vị 2.172. `S2-DODGE-*` = **0 dòng**, `S2-ARMOR-EAT` = 0.
- Phản biện độc lập xác nhận (khớp từng dấu thời gian ±1 ms, mô phỏng lại tiết chế ra đúng 294/344/364 dòng ở 3 lần đo).

### 1.2 Cái người chơi thấy là "đánh hụt" = đòn rơi vào XÁC quái (25%)
- `KNpc.cpp:3883-3887`: mục tiêu (hoặc chính mình) đang `do_death/do_revive` → `ReceiveDamage` trả TRUE **không sát thương, không gửi số, không gửi "né"**.
- Đo giai đoạn luyện công: **343/1.380 đòn (24,9%)** có `life<=0` ngay ở `S1-WHO`, 343/343 đều dừng ở đó. Nguyên nhân: auto tiếp tục vung chiêu vào con quái vừa chết (bot chung quanh cướp kill, client biết trễ).

### 1.3 Auto bắn 15 lệnh/giây, máy chủ chỉ thực hiện ~3 đòn/giây — 80% lệnh bị NUỐT, nhưng KHÔNG làm mất DPS
- `KNpc.cpp:840-958 ProcCommand`: khi nhân vật đang trong hoạt ảnh (`m_Doing=do_attack`) lệnh `do_skill` mới rơi vào nhánh `else` → **huỷ** (`m_Command.CmdKind=do_none`, không xếp hàng).
- Đo: 1.303 mẫu `S3-CMD-SWALLOW` → **0% `do_stand`**; 980 mẫu thi hành → 100% `do_stand/do_run`. Hai tập rời nhau tuyệt đối. 8.120 lệnh chiêu 361 → 1.577 đòn thật (19,4%); nhịp đòn thật trung vị 325 ms.
- Đây là hành vi có sẵn của engine: đòn thật bị giới hạn bởi hoạt ảnh, lệnh dư chỉ tốn băng thông. **Không phải nguyên nhân "miss".**

### 1.4 Khối "nhặt cuốn genre 6" của auto Dã Tẩu gửi nhặt SAI (lỗi trong mã Dã Tẩu, commit f606e540)
- `CoreShell.cpp:5418-5432` (pha `DTP_FARM`): mọi vật phẩm `genre==6` (`item_magicscript`) trong bán kính **500** → gọi thẳng `CheckObject` → gửi gói nhặt **mà không chạy tới** và **bỏ qua danh sách "không nhặt theo tên"** của người chơi.
- Hậu quả đo được: "Túi Hoạt Động" (đang ở trong danh sách không-nhặt, cùng genre 6 với cuốn Dã Tẩu) bị gửi nhặt **63 lần** cho một món, máy chủ từ chối `OBJ_TOO_FAR` (server đo 263–536, trần 200) trong khi nhân vật đang đi xa dần; 34 món "Túi Hoạt Động" bị vơ nhầm cả phiên. Mỗi lần từ chối máy chủ còn gửi thông báo "đồ quá xa" về client.

### 1.5 Phần lớn món "không nhặt được" là ĐỒ CỦA BOT (thuộc người khác 33 giây)
- `ServerPickUpItem` `KPlayer.cpp:4707-4767`: đồ rơi có `m_nBelong` = người giết (`OBJ_BELONG_TIME=600` frame ≈ 33 s); CaiBang trong tổ đội → đồ của người ngoài đội bị từ chối **im lặng** (`return FALSE`, chỉ gửi `OBJ_CANNOT_PICKUP`).
- Đo trên **150 món duy nhất** CaiBang xin nhặt: **100 nhặt được**, **42 bị từ chối vì thuộc người khác**, 6 đã mất trước khi tới, 1 túi đầy, 1 quá xa (chính là 1.4). Client **không biết `m_nBelong`** (chỉ có ở server) nên cứ chạy tới, thử 3 lần rồi bỏ (`PICK-CUR-DROP3`).

### 1.6 Phía client, auto quyết định ĐÚNG với hầu hết món nhìn thấy
- 102 món `OBJADD-RECV` (cửa sổ pid 6748): 70 đã gửi nhặt, 15 bị lọc tên/loại theo cấu hình người chơi, 10 ngoài tầm 800, 5 túi không còn chỗ (`canplace=0`), 1 mất trước khi tới.
- Phiên sáng (DLL 12:50) còn đo được **40,3 % lần kiểm túi trả `canplace=0`** — túi hết chỗ cho trang bị 2x3/1x4.

## 2. NGHI NGỜ (60–90%) — nguồn của CẢM GIÁC "miss" còn lại
- **Pool đạn client 500 khe đầy** ở chỗ đông: 4–9 % lần tạo đạn trả `Add=-1` (`cli0.log` 199/2.250; `jx_auto.log.1` 127/3.175) → mất hiệu ứng va chạm, **máu vẫn tụt**. Phiên sáng ở map vắng: 0/708.
- **Bảng NPC client 256 khe**: `SYNCPLAYERMIN-NOIDX` 1.909 lần; số sát thương chỉ vẽ khi client tìm thấy NPC nhận (`KProtocolProcess.cpp:3938`) — đo được 2,4 % gói `s2c_show_damage` không vẽ (ridx<=0).
- Lệch vị trí client↔server: `SYNCME-DRIFT` p90 ≈ 4 ô (~128 mps) > bán kính chiêu 90 → 63 lần máy chủ đổi đòn đánh thành lệnh chạy (`S2-MELEE-TOOFAR-RUN`, server đo trung vị 135 khi client nghĩ ≤ 90).

## 3. ĐÃ LOẠI TRỪ (đừng sửa nhầm)
- "Chiêu chưa học" (`S2-SKILL-NOTLEARNED` 664 dòng): nhãn đặt sai chỗ, `found_idx=18` ≠ 0 → chiêu luôn tìm thấy.
- `MISSLE-POOL-FULL`/`E3_MISSLE_ADDFAIL` ~2.700 dòng phía server: nhãn đặt trước cửa, `Add tra ve > 0` ở **3.095/3.095** → máy chủ (20.000 khe) chưa bao giờ tràn.
- Lệch ngưỡng tầm đánh client/server: client bắn khi `dist < 90`, server nhận tới `radius+20 = 110` → client **chặt hơn** server.
- Các nhãn `E4_SKILL_REJ_*`, `SKILL-REFUSE-*`: không lọc tên + tiết chế chung → chỉ 0–4 dòng của CaiBang, và cả 9 dòng đó đều **không chặn**.
- Đồ nằm ngoài tầm 800: chỉ 10/102 món; không phải nguyên nhân chính.

## 4. TRẢ LỜI 2 CÂU HỎI
**(A) Vì sao "đánh không trúng"?** Ở máy chủ, đòn cận chiến của phái ngoại công **không thể trượt** và 100 % đòn thật đều trừ máu. Thứ nhìn thấy như "hụt": ① ~25 % đòn vung vào quái đã chết (bot cướp kill, client biết trễ); ② ở chỗ ~1000 bot, client hết khe đạn/NPC nên mất hiệu ứng và đôi khi mất số sát thương dù máu quái vẫn tụt; ③ lệch vị trí làm máy chủ bắt chạy lại gần thay vì đánh.
**(B) Vì sao "bỏ sót đồ"?** ① 42/150 món là **đồ của bot** (thuộc người giết 33 s) — client không biết nên chạy tới rồi bị từ chối im lặng; ② khối Dã Tẩu nhặt genre 6 gửi sai (1.4); ③ túi hết chỗ cho trang bị to (40 % lần kiểm ở phiên sáng); ④ 10 % món ngoài 800.

## 5. ĐỀ XUẤT SỬA (theo luật: chỉ sửa đúng chỗ hỏng, không chế cơ chế mới)
1. **`CoreShell.cpp:5418-5432`** (mã Dã Tẩu): (a) tôn trọng danh sách không-nhặt `szNOPName` như vòng nhặt chính; (b) chỉ `CheckObject` khi `d < 200` (khớp `PLAYER_PICKUP_SERVER_DISTANCE`), xa hơn thì để luồng `ATYPE_PICKUP` có sẵn chạy tới. Rủi ro thấp; chỉ ảnh hưởng auto Dã Tẩu.
2. **Đổi các nhãn log còn dùng `AUTOLOG_EVERY` trên đường đánh/nhặt sang `AUTOLOG_IDX`** (S3-PKT-IN, S3-CMD-SWALLOW, S3-PROC-FINDSAME, MSL-*/MIS-*, SPICK-*) — không đổi hành vi, để lần sau đo được % chính xác.
3. (Tuỳ chủ game) Cờ tổ đội của CaiBang: nếu không cố ý vào tổ đội thì kiểm tra tab Tổ đội của WAuto; nếu cố ý thì 42 món kia là đúng luật server.
4. (Tuỳ chủ game, **đổi hành vi auto** nên chưa làm) giảm nhịp gửi lệnh chiêu 15/giây xuống theo nhịp hoạt ảnh — chỉ tiết kiệm băng thông, không tăng DPS.
