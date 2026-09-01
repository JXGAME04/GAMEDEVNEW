# BÀN GIAO — PHI PHONG CẤP 13 (LONG NGÂM) ĐỢT 31/08 TỐI [PF13]

**Nối tiếp** `BANGIAO_PHIPHONG_THICONG_3108B.md` (đợt chiều). Chủ giao: *"làm luôn 1 - 2 rồi báo cho tôi test — tôi cần giống 100% client vltk: thông tin item, tên item, thuộc tính, hình ảnh hiển thị khi mang phi phong"* + trang tham chiếu VNG https://volam.vnggames.com/su-kien/ban-cap-nhat-11-2023/phi-phong-13.html.

Mọi thay đổi mang nhãn `[PF13 31/08]`. Driver: scratchpad phiên `daec676c` t103→t112.

---

## 1. TRẠNG THÁI TỔNG

| Lớp | Nội dung | Trạng thái |
|---|---|---|
| 1 | Nắn mã thuộc tính đá (t100, 295+1 dòng, 2 cây) | ✅ áp, md5 trùng |
| 2 | 11 loại thuộc tính C++ mới (t101) | ✅ áp, build sạch |
| 3 | Bố cục 13 lỗ pfpack v2 + PF_TraRowInfo + migration DB (t102) | ✅ áp, build sạch |
| 4 | goldequip: tên **Long Ngâm** (6735), cấp 12/13/14, HP 233, chúc phúc Ngự Phong 22 (t103, chép NGUYÊN dòng VLTK) | ✅ 2 cây md5 `96a2dc34fabe` |
| 5 | Panel 14 ô + khoang 14×4 + Lua khảm 13 lỗ (t107) | ✅ áp, syncheck OK |
| 6 | Chuỗi đột phá 13 bậc + nguyên liệu theo bậc (t108) | ✅ áp, syncheck OK |
| 7 | 5 nguyên liệu mới magicscript P=4933..4937 + icon VLTK (t109) | ✅ 2 cây md5 `ae618fc31dac` |
| 8 | Thuộc tính ẨN Long Ngâm+ (t110: pool 21+19 mã ĐÃ KIỂM đăng ký) | ✅ áp, syncheck OK |
| 9 | GM tool: 3 bộ thử mới + 8 phi phong bậc cao + 5 nguyên liệu (t111) | ✅ áp, syncheck OK |
| 10 | Đệm tooltip `GOD_MAX_OBJ_PROP_LEN` 1024→2560 (13 dòng đá tràn strcat) | ✅ áp |
| 11 | **Hình mang trên lưng** (goldequipres + bảng mantle + spr VLTK) | ⏳ xem mục 5 |

## 2. THIẾT KẾ CHUỖI 13 BẬC (khớp bảng VLTK trích từ `slistcache.pak`)

Chuỗi goldequip (key Lua = dòng vật lý − 1): 5374 Tuyệt Thế(1 lỗ, cấp2) → 5375 Phá Quân → 5376 Ngạo Tuyết → 5377 Kinh Lôi → 5378 **Ngự Phong**(5 lỗ) → 5939 Phệ Quang(6) → 5940 Khấp Thần(7) → 5959 Huyền Kim(8) → 5960 Vô Cực(9) → 5961 Kình Thiên(10) → 6733 Lăng Tuyệt(11) → 6734 **Long Ngâm**(12 lỗ, cấp13) → 6735 Sồ Phượng(13 lỗ, cấp14 — tối cao).

- **Đột phá bậc cao** (menu mới "đột phá bậc cao", `TYPE_CAO_BREAK_THROUGH=6`, tỉ lệ 100% như "hợp thành" VLTK): Ngự Phong→Phệ Quang **30 Bách Luyện Thành Cương** (P4884) · →Khấp Thần 50 · →Huyền Kim 70 · →Vô Cực 100 · →Kình Thiên **10 Đại Bách Luyện** (P4933) · →Lăng Tuyệt 15 · →Long Ngâm **20 (số VNG công bố)** · →Sồ Phượng 25. Chỉ 20 viên 11→12 là số VNG chính thức, còn lại NỘI SUY — chỉnh trong `tbBreakThrough[key].nNeedItem`.
- **Đường platina Vô Cực (Linux/JX2) BỎ**: 5378 đổi từ `AdvBreak→3485` sang `CaoBreak→5939`. Menu Vô Cực/đột phá lần 2 vốn đã tắt từ trước.
- **Tăng sao theo bậc** (mô tả item VLTK): Tinh Ngọc (bậc ≤ Ngự Phong) · **Phách Tinh Ngọc** P4934 (Phệ Quang/Khấp Thần/Huyền Kim) · **Đại Tinh Thạch** P4935 (Vô Cực/Kình Thiên/Lăng Tuyệt) · **Kim Tinh** P4936 (Long Ngâm/Sồ Phượng). Tỉ lệ từng sao của 8 bậc mới NỘI SUY từ hàng Kinh Lôi — chỉnh trong `tbStarUpGradeData[key].tbRatePreStar`.
- Sau đột phá: lỗ 1..(n−1) mười sao + đá cũ khảm lại, lỗ cuối 0 sao (khuôn có sẵn, chạy đúng cho 13 lỗ vì đọc `GetEquipMaxStoneNum`).

## 3. THUỘC TÍNH ẨN (VNG 11/2023)

- Long Ngâm/Sồ Phượng (key ≥ 6734) đạt **10 sao qua Thợ Rèn** → tự quay 2 dòng (1 nhóm phòng thủ 21 mã + 1 nhóm tấn công 19 mã, bảng `tbPF_AnNhom1/2` trong head.lua — TỪNG MÃ đã kiểm có `ProcessFunc` thật, t110 bước 0 chặn 2 mã câm 61/72 và đã thay).
- Lưu ở khe custom-magic 0/1 (`SetMagicAttrib` — ghi đè cả 8 khe, phi phong không dùng khe này cho gì khác), client hiện ở khối tím có sẵn từ đợt chiều.
- Đột phá lên bậc mới **giữ nguyên** 2 dòng ẩn (đọc trước khi xoá item cũ).
- Menu mới "Tẩy thuộc tính ẩn": 1 **Lệnh Bài Phi Phong Long Ngâm** (P4937, miễn bạc) HOẶC 2 Đại Bách Luyện + 20000 vạn. Cũng là đường KÍCH HOẠT cho phi phong GM ép sẵn 10 sao (StarLevelUp thô không qua Thợ Rèn nên chưa có dòng).
- **LỆCH CÓ Ý so với VNG** (cần chủ duyệt): ① không có hạn 14 ngày (VNG cho thuê dòng ẩn 14 ngày — cơ chế hút tiền, bỏ); ② không có bước "kích hoạt 3/5 Đại Bách Luyện" — dòng tự sinh khi đạt 10 sao; ③ 4 giá trị quá lớn của VNG đã hạ (công kỹ năng 180→18%, băng/hỏa/lôi sát 1500→150, độc sát 180→60) — sửa trong bảng nếu muốn.

## 4. CÁC BẪY ĐÃ XỬ / PHÁT HIỆN MỚI

1. **Khoang give-box 6×4**: ô khảm 6..13 gửi `Region.h>5` bị `PlaceItem` từ chối → `AFFAIRITEM_ROOM_WIDTH` 6→**14** (GameDataDef.h — chỉ là sức chứa trong bộ nhớ, nuôi MAX_PLAYER_ITEM, không nằm trong gói tin/DB; hộp đặt đồ thường không đổi hành vi vì client chỉ gửi h 0..5).
2. **Đệm tooltip 1024**: 13 dòng đá + 13 dòng thuộc tính + khối tím ~>1300B, strcat không chặn → `GOD_MAX_OBJ_PROP_LEN` 1024→**2560** (chỉ UI client).
3. **Comment enum KMagicAttrib.h NÓI DỐI 2 vùng**: mục `normal_reserve4` (idx 189, KHÔNG có tiền tố magic_) làm regex thiếu 1 → tưởng t100 sai +1 (thật ra ĐÚNG); vùng 140–167 comment lệch hẳn +28 (stale). Kiểm mã thuộc tính = phải đếm bằng parse chặt (khuôn t110 bước 0), đừng tin comment.
4. **Vị trí phi phong trong `tbItemIdx` giờ là PHẦN TỬ CUỐI** (`nSize+1`), hết hardcode `[6]` — npc.lua truyền bảng 14 phần tử {13 đá, phi phong}.
5. Icon phi phong đủ 14 bậc TRONG pak JX1 (`pifeng_02..11` updatejx07, `12..14` updatejx15) — không phải bơm.
6. Icon 5 nguyên liệu mới trích từ pak VLTK để **file rời** trong cây client (engine fallback đĩa khi pak không có — `g_FileExists` KFilePath.cpp:406).

## 5. HÌNH MANG TRÊN LƯNG — ĐANG DỞ (việc còn lại duy nhất)

Chuỗi vẽ: `goldequipres.txt` (dòng goldequip → số hiệu hình, đọc `GetGoldItemRes` trừ 2) → `m_MantleType` (sync PlayerSync BYTE) → `KNpcRes::SetArmor` phần 4 → bảng `settings\NpcRes\LadyMantle.txt`/`ManMantle.txt` (35 hàng hình) → spr.

Đối chiếu t112 (`goldequipres_vltk.txt` đã trích về scratchpad `vltk_bang\`):
- 5939/5940 (Phệ Quang/Khấp Thần): VLTK dùng hình **7** (chung với Ngự Phong), JX1 đang 8/9.
- 6734/6735/6736 (Long Ngâm/Sồ Phượng/15pifeng): VLTK **38/39/40** (hình RIÊNG từng bậc), JX1 đang 36/35/34 (trỏ vào hàng "treo kiện" — sai hình).
- Muốn giống 100% phải: sửa 5 dòng goldequipres + **nhập hàng 36..38 của bảng mantle VLTK + toàn bộ spr động tác đi kèm** (mỗi hình = trọn bộ spr theo động tác × 2 giới). Bảng mantle VLTK KHÔNG tìm thấy theo tên `LadyMantle.txt` — đang quét nội dung toàn pak (t106 nền, tìm bảng chứa "pifeng") lúc kết phiên vẫn chạy. **CHƯA sửa goldequipres** — đổi số mà thiếu hàng bảng/spr là phi phong tàng hình.

## 6. CHỜ SWAP + NGHIỆM THU

Nhị phân (đặt `.moi` — chủ tự chạy `ChayGameServer.bat`/`ChoiGame.bat`; PHẢI cùng lúc như đợt chiều, gói 215 không đổi cỡ nên bản chiều→tối không thêm ràng buộc mới): md5 ghi ở mục CHECKLIST cuối (điền sau khi build xong).

Lua đã nằm trên đĩa (nạp lúc boot): `mantleupgrade_head.lua`, `mantleupgrade_npc.lua` (backup `.truoc_13lo_3108`, `.truoc_chuoicao_3108`, `.truoc_an_3108`), `test_phiphong_admin.lua` (`.truoc_pf13_3108`). Bảng: `goldequip.txt` (`.truoc_capcao_3108`), `magicscript.txt` (`.truoc_pf13_3108`) — cả 2 cây, md5 trùng.

### Checklist test cho chủ (sau swap + restart)
1. Lệnh bài admin → "Bộ thử đột phá bậc cao" → Thợ Rèn → "Phi Phong đột phá bậc cao": đặt Ngự Phong 10 sao + 30 Bách Luyện → nhận **Phệ Quang** 0 sao (đá cũ còn, lỗ mới 0 sao). Đi tiếp từng bậc tới Sồ Phượng (đủ nguyên liệu trong bộ thử).
2. "Bộ thử 13 lỗ khảm" → Thợ Rèn → khảm: bảng phải hiện **13 ô + ô giữa**, thả 13 viên vào 13 ô, khảm nắm 1 lần ăn hết; tooltip phi phong liệt kê đủ 13 dòng đá (không cụt đuôi, không crash khi rê chuột).
3. "Bộ thử thuộc tính ẩn" → Thợ Rèn → "Tẩy thuộc tính ẩn": đặt Long Ngâm + 1 Lệnh Bài → 2 dòng tím xuất hiện/đổi. Đột phá Long Ngâm→Sồ Phượng: 2 dòng tím **đi theo**.
4. Tăng sao bậc cao: Phệ Quang đòi **Phách Tinh Ngọc** (bỏ Tinh Ngọc thường phải bị từ chối kèm tên nguyên liệu đúng); Long Ngâm đòi **Kim Tinh**.
5. Tên item: bậc 13 phải là **"Long Ngâm Phi Phong"** (hết "Tử Vân"); cấp yêu cầu 12/13/14 hiện đúng; 5 nguyên liệu mới có icon + mô tả VLTK.
6. Phi phong cũ (trước swap): sao/chúc phúc/5 lỗ đá GIỮ NGUYÊN (migration tự dọn 2 pack tái dụng — mục t102).

## 7. VIỆC KẾ TIẾP
1. Xong quét t106 → nhập bảng mantle + spr hình lưng VLTK (mục 5) rồi mới sửa goldequipres.
2. Chủ duyệt 3 lệch VNG (mục 3) + số nguyên liệu nội suy (mục 2).
3. Cho 4 nguyên liệu mới vào tiệm 186 (onMaterialShop) nếu muốn bán cho người chơi.
