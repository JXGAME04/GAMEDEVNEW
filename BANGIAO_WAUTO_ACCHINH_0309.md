# BÀN GIAO 03/09/2026 (đêm) — WAuto: "Tìm ac chính" + "Đánh cùng mục tiêu ac chính" (đợt 1)

Phiên `wauto` (Claude Fable 5.1). Thiết kế đã duyệt: `THIETKE_WAUTO_ACCHINH_0309.md` — chủ *"hãy làm 1-6"* (cùng máy; cùng map; tab mới; trong Tống Kim ac phụ theo ac chính; combo + chuột phải; 200/150/5 s/300 ms).
Nhánh **`acchinh-0309`** (gốc `rep3-0309` 20bfcdf5 của phiên wauto-ca vì đuôi `autoData` phải chồng lên 3 trường HIỂN THỊ của họ). Gộp vào `main` là quyết định của chủ (cùng lúc với `rep3-0309`).

## 1. Tệp swap — PHẢI lên CẢ BA cùng lúc (`autoData` thêm 14 trường ở cuối, `IPCViTri` mới)

| Tệp | md5 | Cỡ | Ghi chú |
|---|---|---|---|
| `bin\client\CoreClient.dll.moi` | `dff1bfc8` | `2.532.864` | main + rep3-0309 + Ac chính. Tập cha của mọi bản trước (có CT, TK rảo map đợt 1–5b, S13k, MAIL, REP3 hiển thị). Chuỗi kiểm: "[Công Thành]" = 1, "[AC]" ≥ 1. |
| `bin\client\Game.exe.moi` | `aed2d11d` | `1.401.856` | S3Client: cổng `nAC` (+ hiển thị REP3 của wauto-ca). Tập cha của `71dae629` (MAIL D7). |
| `bin\client\WAuto.exe.moi` | `72862beb` | `420.864` | tab thứ 16 **"Ac chính"** (nhóm Điều khiển) + 2 ô hiển thị REP3 của wauto-ca (tab Cơ bản). **`ChoiGame.bat` KHÔNG đổi WAuto.exe.moi → đổi tay.** |
| `bin\server\CoreServer.dll` | không đổi | | máy chủ không liên quan (không có gói mạng mới). |

**Checklist swap:**
1. Thoát hẳn `Game.exe` **và** `WAuto.exe`.
2. Chạy `bin\client\ChoiGame.bat` → đổi `CoreClient.dll.moi` và `Game.exe.moi`.
3. Đổi tay `WAuto.exe` → `.truoc`, `WAuto.exe.moi` → `WAuto.exe`.
4. Mở WAuto: nhóm **Điều khiển** → tab **"Ac chính"** phải hiện (Ac chính: combo · Tìm ac chính [200] mps · Tìm trong thành · Đánh cùng mục tiêu · dòng trạng thái). Không thấy tab = bước 3 chưa xong.
5. Cấu hình cũ `APdata\<ID>.dat` vẫn dùng được (di trú `offsetof(autoData, bTimAcChinh)`, tính năng mặc định TẮT).
6. Lùi: `.truoc` của cả ba (bat giữ CoreClient/Game; WAuto đổi tay).

## 2. Cách dùng
1. Mở ≥ 2 cửa sổ game trong WAuto, tick auto ở dòng nhân vật (cả ac chính lẫn ac phụ — ac chính cần tick để gửi vị trí; auto của ac chính vẫn chạy như thường).
2. **Chuột phải** tên nhân vật muốn làm ac chính ở danh sách → *"Đặt làm ac chính cho tất cả cửa sổ khác"* (hoặc vào tab Ac chính của từng ac phụ chọn tên trong combo).
3. Ở mỗi ac phụ: tab Ac chính → tick **Tìm ac chính** (khoảng cách mặc định 200 mps), tick **Đánh cùng mục tiêu ac chính** nếu muốn; **Tìm trong thành** tắt = ac chính về thành thì ac phụ không chạy theo.
4. Dòng trạng thái tab cho biết: cùng map / khác map, cách bao nhiêu, mục tiêu, sống/chết, mất kết nối.

## 3. Nguyên lý (đúng thiết kế, không đọc bộ nhớ, không gói mạng game)
- `CoreShell.cpp` `AC_GuiViTri`: mỗi cửa sổ có tên ac chính (hoặc là ac chính) gửi `IPCViTri{dwPID, map, x, y, uMucTieu, bSong, camp}` lên WAuto 300 ms/lần (`PRG_VITRI`, cuối `PROTGAMEID`). Mục tiêu = `ea.uNpcID` của auto, không có thì `GetTargetNpc()` (m_nPeapleIdx).
- `WAuto.cpp` `AC_GhepChoNode`: trước mỗi `IPCGameLoop` (54 ms) tra cửa sổ có tên = `szAcChinhTen` của ac phụ, ghi `nACMap/X/Y, uACMucTieu, nACSong, nACCamp, uACTuoi` (ms từ tin cuối) vào `autoData` của ac phụ; `nACLaChinh` = 1 nếu cửa sổ chính là ac chính.
- `CoreShell.cpp` `AC_Process` (`ATYPE_ACCHINH`, `S3Client.cpp` gọi sau các máy sự kiện, kết quả vào `nBS` khi không máy nào khác cầm lái): ac chính hợp lệ = cùng map, còn sống, tin ≤ 5 s. **Cùng mục tiêu** (`AC_CungMucTieu`): `NpcSet.SearchID` thấy → `GetRelation == enemy` → `ea.uNpcID`, trả 2 (máy PK đánh, đuổi theo như Tống Kim giao mục tiêu). **Đi theo** (`AC_DiTheo`): cách > KC → `DT_WalkTo` tới điểm cách ac chính 150 mps (lệch ±40–80 theo `dwID` để không chồng ô), đo kẹt 3 s → tính lại đường 2 lần → nghỉ 10 s; trả 1. Đủ gần → trả 0 (auto thường: đánh quái quanh, nhặt đồ).
- **Tống Kim** (`TKP_FIGHT`): "cùng mục tiêu" chèn trước bước 1; "đi theo" chèn trước "vị trí địch máy chủ báo / rảo map" → *ac phụ theo ac chính thay cho rảo*, ac chính vẫn chạy máy TK đầy đủ.
- Ngựa: qua `DT_WalkTo` → luật ngựa đợt 4 (đường xa tự lên, quanh có địch xuống, cấu hình xuống thì không lên).
- "Tìm trong thành" tắt: map hiện tại có Xa Phu (`g_MoveStation`) = thành → không theo.

## 4. Thay đổi cấu trúc (đều ở CUỐI, tương thích ngược)
- `ipc_shared.h` (3 bản khớp: Core, `E:\Src_Auto_Ngoai`, `WAutoUI`): `PRG_VITRI`; `struct IPCViTri`; `autoData` sau `nWAMissleIndex`: `bTimAcChinh, nAcChinhKC(200), bAcChinhThanh, bCungMucTieu, bAcChinhVaoMap, szAcChinhTen[32], nACLaChinh, nACMap, nACX, nACY, uACMucTieu, nACSong, nACCamp, uACTuoi`.
- `KPlayer.h` ExtAuto cuối: `nACHold, uACNext, nACMyX/Y, uACMoveT, nACRepath, uACNghi` (chỉ CoreClient).
- `CoreShell.h`: `ATYPE_ACCHINH` sau `ATYPE_HIENTHI`.
- `Resource.h`: 632–641 (`IDC_GRP_TAB15 … IDC_STRING_16_TT`), **`IDC_INDEX_END 640 → 700`**, `IDM_ACCHINH_ALL 716`. `WA_SO_TAB 15 → 16`, nhóm "Điều khiển" thêm tab 15, `s_aTabDay` +244.

## 5. Bộ vá
`ReverseTools\goi_va_wauto_acchinh_0309.py [--thu] [--root <worktree>]` — idempotent; Core latin-1, WAuto.cpp/.rc UTF-16; tự chép mirror `WAutoUI`.

## 6. Test đề nghị
1. 2 cửa sổ cùng máy, cùng map: kéo ac chính đi 500 mps → ac phụ chạy theo, dừng cách ~150; log ac phụ `[AC] theo ac chinh (x,y) d=...`.
2. Ac chính đánh quái → ac phụ (tick Đánh cùng mục tiêu) đánh đúng con đó; log `[AC] cung muc tieu ac chinh id=...`.
3. Ac chính chết → ac phụ thôi theo (trạng thái "đã chết"); hồi sinh theo lại. Ac chính thoát game → "mất kết nối", ac phụ về auto thường; log `[AC] mat ac chinh`.
4. Ac chính về thành, ô "Tìm trong thành" tắt → ac phụ không theo; bật → theo.
5. Tống Kim 2 acc cùng phe, ac phụ tick Tìm ac chính → không rảo, bám ac chính, đánh cùng mục tiêu.

## 7. Chưa làm (đợt 2, theo thiết kế mục 8)
Khác map (Xa Phu / Thần Hành Phù sang map ac chính) — trường `bAcChinhVaoMap` đã có, chưa có UI/logic; nhiều máy tính qua mạng; ac chính dự phòng.
