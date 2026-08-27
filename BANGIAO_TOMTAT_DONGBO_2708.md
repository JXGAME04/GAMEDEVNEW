# TÓM TẮT TOÀN BỘ CHUỖI FIX ĐỒNG BỘ DI CHUYỂN/MA/BÚNG — bàn giao gọn cho phiên sau (27/08 ~04h)

> Chi tiết đầy đủ: `BANGIAO_DICHUYEN_GIATLUI_2508.md` mục 9.17–9.26. File này chỉ là bản rút gọn.

## 1. TRẠNG THÁI BINARY (quan trọng nhất)

| Đâu | Bản | Trạng thái |
|---|---|---|
| `bin\client\CoreClient.dll` | **7b22c8c6** (phiên WAuto build 13:12 — **ĐÃ ÔM S12**, kiểm nhãn `S12-THEO`=1) | chờ relog |
| `bin\server\CoreServer.dll` | **f9b4eb37** (phiên WAuto — **ĐÃ ÔM S12**, nhãn `S12-TELE`=1) | chờ RESTART GameServer |

⚠️ Hai phiên build chung một cây nên bản MỚI NHẤT trên đĩa luôn ôm fix của cả hai — **trước khi nghiệm thu chỉ cần kiểm nhãn trong DLL (`grep -c S12-THEO/S12-TELE`), đừng tin md5 trong bàn giao cũ**. Bản S12 nguyên gốc của phiên này: client `795eb6a4` + server `78515a3d` (backup `.cu_2708_truoc_s12_{8ffd4243|449e3ecc}` = S11 đã nghiệm thu đạt).

## 2. CHUỖI GỐC ĐÃ CHỮA (mỗi dòng một gốc — TẤT CẢ có số nghiệm thu)

1. **Trượt tới-lui NPC** = bản sao client MÙ ĐÍCH: gói ADD không mang đích (lỗi gốc 2003, comment `need check later -- spe 03/05/27`), 3 bản vá đoán-đích S9 của tôi còn làm tệ hơn (tự sinh quay đầu 6,1×). **Fix S10**: server gửi kèm gói `s2c_npcrun/walk` CÓ SẴN mang `m_DesX/Y` thật ngay sau gói ADD (cuối `KNpc::SendSyncData`), xoá sạch khối đoán S9. → đảo chiều **6,1× → 1,4×**, lệch p90 **35 mps**.
2. **Đánh vào không khí / đuổi ma vào tường** = bản sao MA (gói chết/gỡ bị rớt bởi **ngân sách broadcast 100 người/lượt** — còn trừ cả cho bot không kết nối). **Fix S10-MA + S11**: server trả `s2c_npcremove` khi bị đánh ID không tồn tại; **gói CHẾT+GỠ miễn ngân sách** (`NPC_EVENT_BROADCAST_LIMIT=100000` — CẤM miễn run/walk/hurt/skill); 5 bộ chọn mục tiêu đọc `m_mAutoExcludeNpcID`; FIX-6 loại 60s + hỏi server; `RequestNpcFail` gỡ ma (DecRef CÓ GÁC death/revive — vô điều kiện là tràn bảng đếm BYTE); gác server chống trả-lời-xuyên-map. → **0 đợt bám mục tiêu, 0 xác chạy, ma nhìn thấy = 0, chết→trại 0,73s**.
3. **Chính mình búng 8 ô** = 8/9 cú KHÔNG phải lệch lưới mà là **script TK teleport (`SetPos` cùng map không báo chính chủ) + server tự dắt chạy ~1.100 mps**, lệnh dắt bị ConformIdx vứt. **Fix S12** (CHƯA nghiệm thu — chờ restart): `[S12-TELE]` SetPos gửi ngay gói tự-sync cho riêng chủ; `[S12-THEO]` sau mỗi `S8-NAN` mở cửa sổ 3000ms nghe lệnh run/walk cho chính mình (gác diệt echo: `HaveTarget()` hoặc `m_nSendMoveFrames<5` thì không áp; đồng hồ `timeGetTime`; reset tại SyncWorld).

Ngoài ra đã lên trước đó: FIX-1 (tách mã GetDir tới-đích/bị-chặn), FIX-3 (máy đánh lấy khoảng cách xấu hơn 2 góc nhìn — từ chối xa −87%, nhịp đòn 832→481ms), FIX-5 (`KNpcRes::Init` quên reset `m_nDoing`), S10-KEO/SNAP/GAC (lưới bản sao đứng), S9-KET (bắt nằm-bẹp thật — chưa nổ lần nào).

## 3. VIỆC CÒN MỞ (theo thứ tự)

1. **Nghiệm thu S12** sau restart: chuỗi 4-cú-`S8-NAN` phải biến mất → 1 snap lúc teleport + `[S12-THEO]` dắt mượt; tổng S8-NAN giảm ~8/9; KHÔNG có `[S12-THEO]` nào lúc cầm chuột thường (gác echo). `[S12-TELE]` ghi qua g_DebugLog (không phải jx_auto.log).
2. **[KEO] nắn mềm — ĐÃ HOÃN có chủ đích** (phản biện: 2 rủi ro chết người — kéo vào tường client = tự kẹt; sàn 64 < nhiễu p90=83 = giật camera; cần mô hình nợ-kéo ≤2 mps/tick). Chỉ nhắm 1/9 cú, S8-NAN vẫn đỡ.
3. **Đồng bộ lưới vật cản map 379** (fix nguồn cú 1/9): server chặn 298k ô vs client 24k (67%). Phương án ĐÃ KHẢO SÁT: sinh lại đoạn obstacle trong `%03d_Region_C.dat` từ dữ liệu `_S` (section cùng cỡ 2048 B/region), đặt **tệp LOOSE** cạnh pak — `KPakFile` mặc định đọc **đĩa trước pak sau** (không ai gọi `g_SetPakFileMode`) nên KHÔNG cần rebuild pak; BẮT BUỘC xoá cache `.fp` hai bên. Tool đo: `ReverseTools/tk_luoi_client_vs_server.py`. **Chờ chủ duyệt** (đụng dữ liệu map).
4. **PerfLog gai tick cuối trận** (gỡ ~1000 bot một đợt nay broadcast không cắt): chưa có số — PerfLog không ghi trong cửa sổ đo. Nếu chủ thấy khựng cuối trận thì soi.
5. Lỗi phụ treo: `[S2-SKILL-NOTLEARNED]` lặp 1,3s/lần (mục 9.7).

## 4. BẪY SỐNG CÒN CHO PHIÊN SAU

- **File dùng chung 2 phiên Claude, CHƯA COMMIT**: `KProtocolProcess.cpp` + `CoreShell.cpp` (ôm cả S8+xúc xắc của phiên kia). Mọi fix của tôi trên 2 file này tái áp bằng chuỗi script **THEO THỨ TỰ**: `ReverseTools/goi_va_S10_dichthat.py` → `goi_va_S10_ma.py` → `goi_va_S11_chongma.py` (C3a expect=2) → `goi_va_S12_bung8o.py`. Các file khác (KNpc.cpp/.h, KNpcSet.cpp, KRegion.h, KNpcFindPath.cpp, KNpcRes.*) ĐÃ commit.
- **Nhãn tiết chế CẤM đếm/suy-từ-vắng-mặt**: `NET-RUN`, `E4_*`, `S9-DICH`, `FIGHT-DIST`… (`AUTOLOG_EVERY`). Đếm được: `S6-*`, `S7-*`, `S10-*`, `S11-*`, `S12-THEO`, `S8-NAN` (`AUTOLOG`/`AUTOLOG_IDX`).
- **Đồng hồ**: `SubWorld[0].m_dwCurrentTime` là **FRAME ~18/s và bị gán lại theo server** — cấm dùng cho cửa sổ ms; dùng `timeGetTime()`.
- **Build**: sửa file chung → build CẢ `Server Release|x64` LẪN `Client Release|Win32`; server ra ở cả `x64\ServerRelease\` lẫn `x64\Server Release\` (space) — kiểm md5+nhãn trước khi swap; `m_Command`/`m_nDoing` là private (dùng `GetCommand()`/`GetResDoing()`); `m_nNeedFixPos` chỉ có bản client (bọc `#ifndef _SERVER`).
- **Log**: phiên kia hay reset `jx_auto.log` — cần giữ thì cp về scratchpad ngay khi kéo.
- **Quy trình đã ăn khách cả chuỗi**: điều tra (workflow song song) → thiết kế → **phản biện đối kháng bẻ bằng mã thật** → mới sửa → nghiệm thu bằng số trước/sau. Phản biện đã chặn 6 rủi ro chết người trước khi thành code trong 3 đợt liên tiếp — đừng bỏ bước này.

## 5. NGHIỆM THU CHUẨN (chạy lại để so)

Bộ thước trong scratchpad phiên cũ (viết lại nhanh được từ mục 9.17-9.25): đảo-chiều client/server cùng NPC (chuẩn ≤1,4×) · lệch p50/p90 (12/35 mps) · đợt bám-một-mục-tiêu ≥6s (chuẩn 0) · `S6-BAL` toàn orphan reg=-1 (ma nhìn thấy = 0) · chết→`S7-REV-XONG` (chuẩn <1s, 0 cú >2s) · trần log `grep "\[AUTOLOG\] bo qua"` phải = 0.
