# BÀN GIAO 07/09 rạng sáng — Fix tận gốc "mất đồ người chơi" (CaiBang)

Chủ: *"làm 1 - 2 · fix tận gốc lỗi tránh sau phát hành game bị lỗi"* (sau bản phân tích `PHANTICH_MATDO_CAIBANG_GOC_THAT_0609.md`).

Commit **`225bf978`** đã lên `origin/main` (trên `d32cc061`). Bộ vá idempotent: `ReverseTools/goi_va_matdo_goc_0709.py`.

## 1. Nhị phân chờ swap (đặt 00:37, build từ main `68d7591b` = bản đang chạy + fix MATDO + AI710L của phiên "Phân tích CaiBang skill 120")

| tệp | md5 | cỡ | thay bản đang chạy |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | `a509a089bcc2c3e6a6102fc879c7103e` | 18 475 008 | b43c85e8 (23:15) |
| `bin\client\CoreClient.dll.moi` | `4e25e8e4517666e1caa0f03cac75becf` | 2 609 664 | 964da287 (23:15) |

Swap: tắt GameServer → `ChayGameServer.bat`; client → `ChoiGame.bat`. Bản đầu (dfd18306 / 8a491314, 00:28, main `225bf978`) đã được thay bằng bản gộp sau khi phiên AI710L báo họ cũng định đặt `.moi`; họ đã xác nhận không đè và đã kiểm `KNpcAI.obj` của bản gộp có đủ 3 hàm mới. Cả hai là superset của bản đang chạy (cùng main).

Lưu ý khi nghiệm thu: cùng lần restart này còn kích hoạt các sửa đổi của phiên AI710L trên cây chạy thật (sao lưu `*.truoc_sk120_0709`): `settings/skills.txt` server+client (876/1406/1493/1322 SkillStyle 14→0, 1236 PreCastSpr, 720 fastwalkrun_p) và script `nhanvat/kynang/{tianren,gaibang,wudu}.lua` + `script/skill/` client. Chi tiết: `PHANTICH_CAIBANG_DAN_VA_QUAI_DOT2_0609.md` Phần I (commit `3710e2b9`). Khác lạ về kỹ năng/AI quái sau swap thuộc phần đó, không phải MATDO.

## 2. Đã sửa gì (3 tệp)

Gốc: `m_Hand` LỆCH (chỉ số không còn là entry `pos_hand`) → `InsertEquipment` gọi `Remove(m_Hand)` không xét kết quả rồi tạo vật rơi trỏ vào chỉ số đó = item của NGƯỜI KHÁC → `KObj::Release` giải phóng → chủ thật mất đồ, entry treo thành "bản ghi ma".

1. **`KItemList::Hand()` tự chữa** ([KItemList.cpp](Sources/Core/Src/KItemList.cpp) helper trước `AddKIL`): chỉ trả chỉ số THẬT SỰ đang ở tay (entry `pos_hand`); lệch → 0 + log. Mười chỗ "ném món trên tay" (`ScriptFuns.cpp` ×6, `KPlayer.cpp` `RecoveryBox`/tách chồng, `KItemCompound.cpp`, `ExchangeItem` ×2) đều lấy qua `Hand()` nên được chặn một lượt.
2. **`InsertEquipment`**: chỉ ném khi `Hand()` hợp lệ VÀ `Remove()` xác nhận danh sách này đang giữ món.
3. **`ExchangeItem` (server, chống hack)**: `m_Hand` không có trong danh sách → xoá ngay (trước treo tới khi vào lại).
4. **`KObjSet::Add` (server)**: từ chối tạo vật rơi cho chỉ số item rỗng (`GetID()==0`) hoặc ngoài biên.
5. **`AddKIL` khi nạp DB** (thay H3/H4 của `e06a12b6`): tay / ô trang bị / dự phòng / mọi lưới (hành trang, rương, 3 rương mở rộng, hành trang mở rộng):
   - trùng chỗ mà **cùng một món** với món đang ở đó (`CungMotMon`: thuộc tính + seed + số chồng + độ bền) = bản ghi KÉP → **bỏ + log** (không nhân bản);
   - khác → **cứu** sang chỗ trống (`CuuMonKhiNap`: chính container đó → hành trang → rương) + log;
   - hết chỗ mới bỏ, có log. Món "đang mặc" không hợp ô (`Fit`) hoặc không phải trang bị cũng đi đường cứu.
6. Log ghi **`logs\hethong.log`** thẻ `[MATDO]` qua `g_GhiLogHeThong` (phát hiện thêm: `g_DebugLog` chỉ gửi cửa sổ debug, máy chủ thật không thấy).

Giữ nguyên H1/H2 của `e06a12b6` (vô hại).

## 3. Nghiệm thu sau swap

```bash
grep -c "\[MATDO\]" /e/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/logs/hethong.log
```

- Lần CaiBang vào đầu tiên: mong đợi vài dòng `CUU mon ...` (bản ghi ma giáp/mũ/vũ khí của bot hiện đang ở ô 4/5/13 sẽ vào túi → bán/vứt được) và `BO mon ...` cho bản ghi rỗng ở ô 7/10. Sau đó **không được** còn dòng `CUU`/`BO` mới cho CaiBang.
- Dòng `m_Hand=... LECH` xuất hiện = có đường sinh lệch còn sót, ghi tên nhân vật → báo lại.
- Dòng `KObjSet::Add tu choi` = có nơi vẫn ném chỉ số rỗng → báo lại.
- Kiểm tổng: `python ReverseTools/matdo_theo_vitri.py CaiBang 40` — "DANG MAC" phải đúng số ô thật, "RUONG"/"hanh trang" không tụt ngoài bán/dùng.

## 5. Nghiệm thu thực tế (fix ĐÃ SỐNG từ 00:56, đo tới 03:40)

Phiên DELTA swap lúc 00:50 bản `4b89f185` build từ main sau `225bf978` → fix mất đồ chạy thật từ 00:56:55 (đã kiểm chuỗi `[MATDO]` trong DLL đang chạy và cả CoreClient 5c359b16). Khe `.moi` sau đó thuộc phiên SK120 (`1a33f617`, superset); bản của tôi được giữ tên `.moi.matdo_a509a089_0037` / `.moi.matdo_4e25e8e4_0037`.

| chỉ số (00:57 → 03:40) | kết quả |
|---|---|
| đăng nhập qua đường nạp mới (`AddKIL`) | 1.001 (1.000 bot + CaiBang) |
| dòng `[MATDO]` trong `logs\hethong.log` | **1**: `CaiBang: CUU mon Xà Khiếp (dt 0 cấp 7 seed 108862496) từ ô trang bị (2,13,0) → rương (0,2)` = đúng bản ghi ma đã chứng minh là đồ bot |
| `m_Hand LECH`, `KObjSet::Add tu choi`, `ban ghi KEP`, `BO mon` | 0 |
| bot.log: bot phát lại vũ khí / `LECH` / `tay dang giu` | 77 / 0 / 0 |
| CaiBang mất món thật (`matdo_quet.py`) | 0 (chỉ auto bán rác hàng (x,0)/(x,2) như mọi ngày; rương giữ 2 món) |

Món "Xà Khiếp" sau khi vào rương đã được auto xử lý (không còn trong bản lưu 01:01), là đồ rác của bot nên không cần giữ. Lưu ý đọc số liệu: bản lưu `role_history` đầu tiên sau đăng nhập (00:58:25) là bản CŨ trước khi tắt máy chủ (thứ tự bản ghi trùng 00:48); bản 01:01:25 mới là trạng thái sau nạp (thứ tự đảo ngược, không còn bản ghi ma).

## 4. Còn mở (chờ chủ)

- **Khôi phục đồ CaiBang** từ `role_history` id **693958** (19:03:00 06/09, trước bão: 14 mặc + 31 rương + 35 túi). Phải tắt nhân vật, mất đồ nhặt sau mốc đó. Chưa làm.
- `InsertEquipment` còn lỗi cũ nhỏ: tên/màu vật rơi lấy từ món MỚI (`nIdx`) thay vì món bị ném (`nIndex`) — chỉ hiển thị, chưa sửa.
