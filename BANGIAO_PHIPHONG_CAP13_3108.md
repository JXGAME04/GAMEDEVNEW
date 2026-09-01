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
| 11 | **Hình mang trên lưng** (goldequipres + bảng mantle + spr VLTK) | ✅ xem mục 5 |

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

## 5. HÌNH MANG TRÊN LƯNG — ✅ XONG (t113, thuần dữ liệu, không cần build lại)

Chuỗi vẽ: `goldequipres.txt` (dòng goldequip → số hiệu thô, `GetGoldItemRes` trừ 2) → `m_MantleType` (sync PlayerSync BYTE) → `KNpcRes::SetArmor` phần 4 → bảng `settings\NpcRes\LadyMantle.txt`/`ManMantle.txt` → spr. **Ánh xạ chốt bằng mã** (`KNpcResNode.cpp:203` `GetString(j+2,…)`): số thô R → HÀNG DỮ LIỆU R−1; file đặt tên theo CẤP (`nv_pifeng13` = cấp 13 = Long Ngâm).

Phát hiện: JX1 Lăng Tuyệt raw 37 → hàng 36 **NGOÀI bảng 35 hàng** = áo choàng tàng hình; Long Ngâm/Sồ Phượng raw 36/35 trỏ hàng "treo kiện đặc hiệu" = sai hình. Hàng 12-20 hai bảng **khác CÓ CHỦ Ý** (JX1 thay treo kiện môn phái bằng áo choàng riêng) — không đụng.

Đã làm:
1. Nối 7 hàng VLTK (36-42: Phi Phong 12/13/14 + phi phong 15/16 + Tiêu Dao ×2) vào `LadyMantle.txt` + `ManMantle.txt` (35→42 hàng, backup `.truoc_hinhlung_3108`); Info tables đã đủ 42 hàng sẵn.
2. Trích **832/868 spr** từ pak VLTK → file rời `bin\client\spr\npcres\woman|man\...` (36 thiếu đều thuộc 2 hàng Tiêu Dao 41-42 KHÔNG dùng trong chuỗi; hàng 36-40 đủ 100%). Pak JX1 vốn có sẵn một phần (`nv_pifeng12/13` trong updatejx15/16) — pak ưu tiên, file rời bù.
3. `goldequipres.txt` 2 cây (md5 `36121985b879`): 5939/5940→**7** (Phệ Quang/Khấp Thần dùng chung hình Ngự Phong như VLTK), 6734→**38**, 6735→**39**, 6736→**40** (6733→37 vốn đúng, giờ trỏ hàng 36 = nv_pifeng12 ✓).

## 6. CHỜ SWAP + NGHIỆM THU

Nhị phân đã đặt `.moi` (commit `071192bd`, build 18:43-18:44 — chủ tự chạy `ChayGameServer.bat`/`ChoiGame.bat`; PHẢI cùng lúc như đợt chiều, gói 215 không đổi cỡ nên bản chiều→tối không thêm ràng buộc mới):

| Tệp | md5 (12 đầu) | Ghi chú |
|---|---|---|
| `bin\server\CoreServer.dll.moi` | `B6AA3FB08486` | superset: chứa cả vá ô chết `aaf5bb24` + 7 rào `[REFOAN-VS]` `837a29b4` của phiên song song |
| `bin\client\CoreClient.dll.moi` | `E861E84E6518` | đè bản `.moi` cũ `50AF3C60656C` (đợt chiều) — đã gộp, không mất gì |
| `bin\client\Game.exe.moi` | `5E1BD319B5AC` | panel khảm 14 ô |

Lua đã nằm trên đĩa (nạp lúc boot): `mantleupgrade_head.lua`, `mantleupgrade_npc.lua` (backup `.truoc_13lo_3108`, `.truoc_chuoicao_3108`, `.truoc_an_3108`), `test_phiphong_admin.lua` (`.truoc_pf13_3108`). Bảng: `goldequip.txt` (`.truoc_capcao_3108`), `magicscript.txt` (`.truoc_pf13_3108`) — cả 2 cây, md5 trùng.

### Checklist test cho chủ (sau swap + restart)
1. Lệnh bài admin → "Bộ thử đột phá bậc cao" → Thợ Rèn → "Phi Phong đột phá bậc cao": đặt Ngự Phong 10 sao + 30 Bách Luyện → nhận **Phệ Quang** 0 sao (đá cũ còn, lỗ mới 0 sao). Đi tiếp từng bậc tới Sồ Phượng (đủ nguyên liệu trong bộ thử).
2. "Bộ thử 13 lỗ khảm" → Thợ Rèn → khảm: bảng phải hiện **13 ô + ô giữa**, thả 13 viên vào 13 ô, khảm nắm 1 lần ăn hết; tooltip phi phong liệt kê đủ 13 dòng đá (không cụt đuôi, không crash khi rê chuột).
3. "Bộ thử thuộc tính ẩn" → Thợ Rèn → "Tẩy thuộc tính ẩn": đặt Long Ngâm + 1 Lệnh Bài → 2 dòng tím xuất hiện/đổi. Đột phá Long Ngâm→Sồ Phượng: 2 dòng tím **đi theo**.
4. Tăng sao bậc cao: Phệ Quang đòi **Phách Tinh Ngọc** (bỏ Tinh Ngọc thường phải bị từ chối kèm tên nguyên liệu đúng); Long Ngâm đòi **Kim Tinh**.
5. Tên item: bậc 13 phải là **"Long Ngâm Phi Phong"** (hết "Tử Vân"); cấp yêu cầu 12/13/14 hiện đúng; 5 nguyên liệu mới có icon + mô tả VLTK.
6. Phi phong cũ (trước swap): sao/chúc phúc/5 lỗ đá GIỮ NGUYÊN (migration tự dọn 2 pack tái dụng — mục t102).

### Bổ sung checklist test (hình lưng)
7. Mang Long Ngâm lên người: sau lưng phải hiện **áo choàng rồng riêng** (nv/nan_pifeng13), Sồ Phượng hiện pifeng14, Lăng Tuyệt pifeng12; Phệ Quang/Khấp Thần hiện hình giống Ngự Phong (đúng VLTK). Thử cả nhân vật nam lẫn nữ, đứng/chạy/ngồi/lên ngựa.

## 6b. ĐỢT VÁ 31/08b — 8 LỖI CHỦ BÁO SAU TEST ĐẦU (driver t114-t118, nhãn `[PF13 31/08b]`)

Điều tra bằng workflow 6 tác tử song song (472k token) trên mã thật trước khi vá:

1. **Tăng sao thất bại nuốt 200 nguyên liệu** — GỐC: `tbRatePreStar` là **%/viên** (hàng gốc Tuyệt Thế sao 1 = 1.0 = 1%/viên → 100 viên đạt 100%); tỉ lệ nội suy t108 (0.07-0.2) thấp 10-30 lần → sao 1 Sồ Phượng cần ~1.400 viên, 200 viên = 14% → thua là mất sạch. **t114**: nắn 8 bậc lên 1.4-2.0%/viên (sao 1 ≈ 50-70 viên, sao 10 ≈ 150-250 viên đạt 100% — nguyên liệu hiếm nên MẠNH hơn Tinh Ngọc; vẫn ĐỂ CHỈNH).
2. **Lệnh Bài không hiện hình** — GỐC KÉP: t109 trích `smallfragment.spr` còn ở **dạng nén NRV2B** (điều kiện `csize==size → coi như không nén` sai với entry này; header `FB 53 50 52` ≠ `SPR\0`) VÀ đường dẫn bị chính `update01.pak` của JX1 che (pak thắng file rời). **t118**: giải nén theo NỘI DUNG (chỉ nhận khi ra `SPR\0`) → `\spr\vng\item\lenhbailongngam.spr` (không bị che) + đổi ImageName magicscript 2 cây (md5 `d09bb3f71932`). 4 icon kia đã rà: file rời `SPR\0` chuẩn ✓.
3. **Đá khảm chưa hiện thuộc tính** — đã rà TRỌN đường tĩnh (overload GetDesc nào chạy cho genre 9 → chuyển tiếp 2517 → overload 1 → PF_AppendDesc:2134 vô điều kiện; dữ liệu starstone/GE/MagicDesc.Ini đủ 34/34 mã có giá trị; `operator=(KBASICPROP_STARSTONE)` chép đủ nParticularType) — **không tìm thấy lỗi tĩnh**; khối này giờ kèm viết hoa. Chờ test lại build mới, còn tái diễn thì phiên sau gắn log in-game.
4. **Tên thiếu cấp** — Linux KHÔNG nhét cấp vào tên (đã mổ cả 2 bản goldequip Linux); làm theo cách gọi VNG: tiêu đề tooltip thêm **"(Cấp N)"**, N = cột cấp goldequip − 1 (Tuyệt Thế=1 … Sồ Phượng=13) — `KItem.cpp` overload 2 sau tên (phi phong đi overload 2 vì `nGoldId=0`, bằng chứng: tag `[x1 Thuộc Tính]` chỉ overload 2 in).
5-6-8. **Màu sai / chưa viết hoa / chưa tách phần** — PF_AppendDesc bố cục lại: khối thuộc tính **ĐÁ** đổi tím→**Green** (tím dành riêng cho dòng ẩn); **VIẾT HOA chữ cái đầu** mọi dòng thuộc tính phi phong + dòng tự-giới-thiệu của viên đá (helper `sPF_ChepHoaDau` — chỉ hoa ASCII a-z, chữ đầu có dấu giữ nguyên theo RULE 0; gốc chữ thường nằm ở chính MagicDesc.Ini); **dòng trống tách phần** giữa danh sách lỗ / khối đá / khối ẩn.
7. **Chưa thấy dòng ẩn ở 10 sao** — GỐC THẬT: phi phong hoàng kim **sinh ra ĐÃ có thuộc tính vàng ở khe 0** (`Gen_GoldEquipment` đổ từ cột magic goldequip.txt qua magicattrib_ge — tác tử đã lần đủ chuỗi) → cổng "khe 0 == 0 thì roll" không bao giờ đúng; và nếu roll thì `SetMagicAttrib` đè khe 0-5 làm MẤT thuộc tính vàng. **THIẾT KẾ LẠI**: dòng ẩn chuyển sang **khe 6-7** — đúng nhánh "Hidden magic" CÓ SẴN của engine (`i >= MAX_ITEM_NORMAL_MAGICATTRIB=6` trong Apply/RemoveMagicAttribFromNPC, cổng `nCountE` xưa nay không ai cấp — t116 mở cổng riêng cho phi phong); vòng hiển thị chuẩn chỉ vẽ khe 0-5 nên không trùng; khối tím mới đọc khe 6-7; DB/sync đủ 8 khe (iparam[16] ↔ nGeneratorLevel[16], gói ITEM_SYNC m_MagicLevel[16]). Lua (t117): `PF_GhiKheAn` GIỮ NGUYÊN khe 0-5; cổng tự nổ + tẩy + mang-theo-khi-đột-phá đều theo khe 19 (= type khe 6); đột phá chỉ chép 2 khe ẩn sang món mới (khe 0-5 của bậc mới giữ của chính nó). GM: `PP_BoAn`/`PP_EpMon` tự roll ngay (admin tool Include head.lua).
+ **Đệm tiêu đề** `GOD_MAX_OBJ_TITLE_LEN` 2048→4096 (GetDesc ghi TOÀN BỘ tooltip nối tiếp từ szTitle — tác tử chứng minh không có con trỏ riêng cho szProp — 13 dòng đá + khối ẩn vượt 2048).

Bẫy ghi thêm: (a) `iiduphong2/3/4` KHÔNG rảnh (nParam/GlowLight/MaxOptMultiply) — đừng trưng dụng; (b) `<color=R,G,B>` được engine parse thật (Text.cpp:191-225) — luật "tên màu ≥8 ký tự vẽ đen" chỉ áp cho TÊN màu; (c) mọi lời gọi GetDesc ở CoreShell đều khớp overload 2 (tham số bool), overload 1 chỉ chạy qua trạm chuyển tiếp 2516.

## 7. VIỆC KẾ TIẾP
1. Chủ duyệt 3 lệch VNG (mục 3) + số nguyên liệu nội suy (mục 2) + Phệ Quang/Khấp Thần dùng chung hình 7 (muốn hình riêng từng bậc như JX1 cũ thì trả goldequipres 5939/5940 về 8/9).
2. Cho 4 nguyên liệu mới vào tiệm 186 (onMaterialShop) nếu muốn bán cho người chơi.
3. Phi phong bạch kim Vô Cực cũ (3485/4835-4839) là di sản Linux — không vào được chuỗi mới (menu Vô Cực vẫn tắt); nếu người chơi đang giữ, cần chủ quyết đường quy đổi.
