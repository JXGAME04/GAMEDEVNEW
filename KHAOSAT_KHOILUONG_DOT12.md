# KHẢO SÁT KHỐI LƯỢNG CÁC TÍNH NĂNG BANG HỘI CÒN LẠI (đợt 12 — 14/08/2026)

> 6 phiên điều tra độc lập, mọi con số đo từ file thật (đếm dòng/hàm/asset, đọc mã 2 phía),
> không ước bằng cảm giác. Chi tiết từng phiên nằm trong phần dưới; bảng đầu là để ra quyết định.

## 0. BẢNG QUYẾT ĐỊNH TỔNG

| Tính năng | Phương án khuyến nghị | Ngày công | Rủi ro | Phát hiện đảo chiều |
|---|---|---:|---|---|
| 🔴 **VÁ GẤP (đang lỗi/khai thác được NGAY)** | 3 việc gộp | **3–4** | Thấp | Xem mục 1 |
| **Hiệu ứng đại thần (NW)** | A — hào quang + skill khi được phong | **2–3** | Thấp | Skill 953-962 + 5 sprite + tianzi.lua **ĐÃ CÓ SẴN nguyên vẹn** trong cây ta |
| **Map lãnh địa 586-597** | A+ — 11 map chung + "Vào bổn bang" + NPC đầy đủ | **5–7** | Thấp | **100% asset map ĐÃ NẰM SẴN trong pak client + server** (6,17 MB, đúng format JX1) — chi phí asset = 0 |
| **Việc vừa đợt 1** (G-28, G-27, G-26, D-3, F-1) | trọn gói | **4** | Thấp | 4/8 việc DOT9 đã xong từ trước |
| **Kinh tế bang thật** (bảo trì ngày D-1, phạt trục xuất D-4, mục tiêu tuần C) | trọn gói | **5,5** | TB | Đánh giá cũ "15 hàm Get không có đường ghi" đã LỖI THỜI — thiếu thật là *không ai chọn mục tiêu mới* |
| **Sản xuất công phường** | B — trần/chi phí + 3 khu chỉ-cần-AddItem | **10,5–13** | TB | Script 3.838 dòng + settings **đã trong cây**; bảng item khớp **lệch đều +1 ID** (rủi ro kinh tế: THẤP) |
| **2 cửa sổ con** (bổ nhiệm + phát tiền) | E-2 rồi E-1 | **5–6** | Thấp-TB | Cửa sổ bổ nhiệm **tái dùng được `UiTongAssignBox` có sẵn**; sprite "thiếu" của phát tiền khả năng chỉ là sai đường dẫn trong blueprint |
| **Hàng đợi đơn xin vào bang ở relay** | A (làm SAU G-28) | **5–6** | TB-Cao | Bản gốc 25 đơn/trang (không phải 20), tô màu online/offline |
| **Tuyệt kỹ bang đúng chuẩn** | B (PHẢI làm DOT6 #18 trước) | **6,5** | **Cao** | **DOT6 #18 còn nguyên**: bảng số TONGTSK_ C++ lệch Lua (1101/1102/1104/1105 đè nhau) — chặn cứng |
| **Quốc chủ gate thật** (đại thần B) | sau khi có công thành ownership | **+1–1,5** | TB | CTC JX1 đã có bản ghi bang giữ thành (`danhsach_bang.lua`) — hiện đang RỖNG |
| **Công thành / thành chiến** | B — mở rộng CTC JX1 sẵn có lên đa thành + thuế + Thái Thú | **22–38** | TB | **JX1 đã có hệ CTC SỐNG 2.075 dòng** (map 221-223 + lịch trận); port full JX2 = 206–288 ngày → không đáng |
| **League LG_/LGM_** | A chỉ-khi-cần (fix 28 hàm + persistence thật) | **7–8** | TB | **97% League là cân nặng chết** (consumer chưa đem sang); phần đang chạy thì ĐANG LỖI đệ quy (vá gấp mục 1) |
| Lãnh địa B (riêng per-bang, pool ID tĩnh) | chỉ khi cần riêng tư | +9–13 | TB | Không đụng lõi engine nếu dùng pool ID tĩnh |
| Phường C (full 7 khu, hoãn FoundryItem) | sau B nếu muốn đủ | +12–14 | TB-Cao | `Sale` 4 tham số + 10 hàm durability |
| NW full (Thiên Tử, ngọc tỷ, quốc chiến...) | không khuyến nghị đợt này | 20–30 | Cao | 23 hàm NW_ + 2.422 dòng script |
| Công thành C (port nguyên 4 hệ JX2) | **KHÔNG khuyến nghị** | 206–288 | Cao | 20.170 dòng + 133 hàm engine thiếu + trùng hệ CTC đang chạy |
| League C (full + mọi consumer) | **KHÔNG khuyến nghị** | 95–120 | Cao | WLLS/VLĐH/sư đồ... đều chưa có kế hoạch |
| Nới nhật ký RECORD_LEN 96 (F-2) | **HOÃN** — rút ngắn 7 chuỗi thay vì đổi format DB | (2–2,5) | **Cao** | Khuôn "nhận 2 cỡ" KHÔNG áp được (đổi GIỮA struct) |

## 1. 🔴 VÁ GẤP — lỗi đang sống, độc lập với mọi lựa chọn (3–4 ngày)

1. **Nâng cấp tác phường MIỄN PHÍ VÔ HẠN CẤP** (DOT9 #25+#26 chưa làm): `WS_UP` chỉ `+1` vào field cấp — không trần, không trừ quỹ, không điều kiện. Người chơi có quyền 9001 bấm liên tục là max cấp mọi khu. **2,5–3 ngày** (4 điều kiện + chi phí + trần theo `tong_level_data` + đọc 2 trường trần đã nạp sẵn ở relay).
2. **Đệ quy vô hạn trong `gb_taskfuncs`** (League stub): người chơi bình chọn bản đồ công cộng hoặc bang lên cấp → `gb_AppendTask` ↔ `LG_Apply*` gọi vòng nhau, key dài thêm mỗi vòng → tràn stack Lua. Gốc: stub `LG_GetLeagueObj` trả −1 (script gốc test ==0) + `LG_Apply*` route ngược vào `gb_SetTask`. **Vá nóng 0,5–1 ngày** (kho riêng cho League stub, trả 0 khi không thấy).
3. **Mặt nạ quyền 12 → 14** (DOT9 #24): 4 quyền của bản gốc (1002/1004/1903/2003) bấm không gửi được; đang chứa 2 mã 1000/2007 bản gốc không có. **0,5 ngày** (GS + client ra cùng lúc).

## 2. THỨ TỰ KHUYẾN NGHỊ (nếu muốn tối đa giá trị/ngày công)

```
Đợt A (≈ 6-7 ngày):  VÁ GẤP (3-4) → Hiệu ứng đại thần (2-3)
Đợt B (≈ 9-11 ngày): Map lãnh địa A+ (5-7) → Việc vừa đợt 1 (4)
                     ⤷ lãnh địa mở khóa NPC tổng quản 7 phường + Tế Đàn trên map bang
Đợt C (≈ 11-13 ngày): Kinh tế bang + mục tiêu tuần (5,5) → Phường sản xuất B (5,5-7)
                     ⤷ sau đợt này: Tế Đàn phát thưởng thật, phường đổi được lệnh bài Boss/mặt nạ/hồng bao
Đợt D (≈ 11-12 ngày): 2 cửa sổ con (5-6) → Hàng đợi đơn relay (5-6)
Đợt E (lớn, chọn 1):  CÔNG THÀNH B (22-38) ← tính năng đinh, đáng nhất
                     hoặc Tuyệt kỹ B (6,5) + Lãnh địa B (9-13) + Quốc chủ gate (1-1,5)
```

**Lý do xếp lãnh địa sớm:** cổng vào SẢN XUẤT phường của bản gốc là NPC tòa xưởng ĐỨNG TRÊN MAP BANG (`add_one_building` → `SVR_CheckUse`), và NPC Tế Đàn (thưởng mục tiêu tuần) cũng vậy — không có lãnh địa thì hai tính năng kia không có chỗ đứng đúng chuẩn.

**Lý do chọn Công thành B thay vì port JX2:** JX1 đã có CTC sống (2.075 dòng VN, 3 map, lịch trận, trụ/cổng/mật đạo, thuế, top-10). Mở rộng nó đạt ~80% giá trị cảm nhận của "Thất Thành Đại Chiến" với ~12% chi phí; port nguyên JX2 tạo ra HAI hệ công thành song song và cần 2 subsystem C++ mới (`LG_` 25 hàm + `BT_` 19 hàm).

## 3. CHI TIẾT TỪNG PHIÊN ĐIỀU TRA

(6 báo cáo đầy đủ — mỗi báo cáo có inventory file/dòng, bằng chứng file:line, phân rã ngày công theo tầng, rủi ro — lưu trong lịch sử phiên; các số then chốt đã tổng hợp ở bảng mục 0. Điểm neo quan trọng:)

- **Đại thần:** thiếu đúng 2 hàm C++ nhỏ (`RemoveSkillState` — bọc `ForceClearStateSkillEffect` có sẵn; `GetTongDuty` — đọc field 51-53) + 1 file Lua ~60 dòng + 1 dòng hook `playerlogin.lua`. `danhsach_bang.lua` đang rỗng → gate quốc chủ chặt sẽ khóa bổ nhiệm tới khi có bang thắng CTC (cần fallback GM).
- **Lãnh địa:** JX1 KHÔNG có instance động (KSubWorldSet chỉ nạp lúc boot) — nhưng phương án A+/B không cần; bẫy phải sửa: các nhánh script test `> DYNMAP_ID_BASE (70000)` sẽ khóa chức năng nếu dùng ID tĩnh; nút `[BtnEnterMap]` đang bị chiếm làm việc khác, cần trả lại "Vào bổn bang".
- **Phường:** lỗ hổng nâng cấp là việc nóng; `TWS_ApplyUse` hiện nuốt lệnh (không round-trip USE_R/G) — đó là nút cổ chai của sản xuất; 6/12 chỗ `AddItem` gọi 6 tham số bị JX1 lặng lẽ bỏ qua (fix 30 phút); 2 lỗ dữ liệu thật: genre-6-detail-2 và mask 366.
- **Công thành:** relay ta KHÔNG có máy Lua/task-scheduler (DoScript.cpp chỉ 3 hàm) — mọi phương án né được điều này trừ port full JX2.
- **League:** persistence hiện tại là ảo (ghi append `gbtask_jx2.txt` không bao giờ đọc lại — file phình vô hạn); 9/19 stub sai chữ ký so với call site thật.
- **Việc vừa:** G-21/22/23 + D-2 đã XONG từ đợt 11; F-1 (12→16 dòng nhật ký) phải nâng `TJX2_UI_ROWS` 14→16 + 6 mục ini kèm theo (đúng lớp bẫy tràn mảng đã dính 2 lần).

## 4. VIỆC KHÔNG NÊN LÀM (ghi để khỏi làm nhầm)

1. Port nguyên 4 hệ công thành JX2 (206–288 ngày, trùng hệ đang chạy).
2. "Hoàn thiện 28 hàm League" như hạng mục độc lập — 97% không có ai gọi.
3. Nới RECORD_LEN (đổi giữa TTongStruct = di trú DB thật) chỉ để cứu 7 chuỗi — rút ngắn chuỗi rẻ hơn 10 lần.
4. FoundryItem (đúc trang bị) ngay đợt đầu — chỉ 2 call site, khu Binh giáp vẫn chạy 80% giá trị không cần nó.
