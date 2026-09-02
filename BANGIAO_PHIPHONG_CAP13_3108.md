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

| Tệp | md5 (12 đầu, bản 31/08d — ĐÈ mọi bản trước) | Ghi chú |
|---|---|---|
| `bin\server\CoreServer.dll.moi` | `151C930D4B0B` | superset: vá ô chết `aaf5bb24` + `[REFOAN-VS]` `837a29b4` + PF13 + 31/08b + 31/08c |
| `bin\client\CoreClient.dll.moi` | `9DADA039BBAF` | tooltip (Cấp N ×1, khối tím như mẫu, hở dòng) + hết mất dòng ẩn |
| `bin\client\Game.exe.moi` | `79623AEAEF28` | panel khảm 14 ô + KGameObjDesc 4096 (phải đi cùng CoreClient) |

⚠️ 31/08b đổi cỡ `KGameObjDesc` (header dùng chung CoreClient ↔ Game.exe) — **hai file client PHẢI swap cùng nhau**, không được lệch bản.

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

## 6c. ĐỢT VÁ 31/08c — 5 LỖI CHỦ BÁO LẦN 2 (t119-t120, nhãn `[PF13 31/08c]`)

1. **"(Cấp 13) (Cấp 13)" in 2 lần** — GỐC: khối `[cấp]` cũ cuối tiêu đề overload 2 kết bằng `if (pszTemp[0]) strcat` mà pszTemp còn nguyên chuỗi của tôi (mantle bị cổng `detail <= equip_horse` loại nên không ai ghi đè pszTemp). Vá: `pszTemp[0] = 0` ngay sau khi dùng. **Cấp giờ suy từ SỐ LỖ KHẢM** (`GetMaxStoneNum()` — trùng khít cấp VNG toàn chuỗi: Tuyệt Thế 1 lỗ = Cấp 1 … Sồ Phượng 13 lỗ = Cấp 13), KHÔNG suy từ `nLevel` (có món mang nLevel mã hoá kiểu cũ — Kình Thiên hiện "[cấp 11]"). Overload 1 (món `nGoldId≠0` đi đường này) cũng vá: phi phong đi nhánh riêng, không qua bộ giải mã `[Cấp]` %10/%100 cũ.
2. **"Dòng ẩn hiện rồi biến mất"** — GỐC (tác tử lần đủ 2 đầu): mỗi lần item được DỰNG LẠI (relog/đổi map/AddKIL/`SetLevelItem`… gửi `ITEM_SYNC bIsNew`; server load roledb) thì `Gen_Gold/PlatinaEquipment` dựng lại `m_aryMagicAttrib` từ `nGeneratorLevel` với **cổng gác chỉ nhìn `[8]`** (giá trị khe 0) + **`break` tại khe trống đầu tiên** → khe ẩn 6-7 bị chặt/rơi về nhánh bảng = 0. Comment `KItemCompound.cpp:1472` về gate "[6]" là **comment nói dối** (mã thật [8]). 4 vá, KHÔNG đổi layout gói nào: ① cổng gác quét cả 8 khe (type>0 && có value word) + khe trống thì gán rỗng rồi `continue` — sửa ở CẢ BA bản sao `Gen_GoldEquipment`/`Gen_PlatinaEquipment`/`UpgradePlatinaEquip`; ② handler client `s2cSyncItem` memcpy 8→16 int (`KProtocolProcess.cpp:1642` — gói vốn mang đủ 16, bản cũ vứt nửa giá trị); ③ server `SyncItem` gửi kèm gói `s2c_syncmagic` 197B (khuôn PFSYNC) cho item có khe ẩn — client dù bị Gen dựng sai cũng bị đè lại đúng ngay sau đó trên cùng luồng TCP; ④ (từ t117) Lua chỉ ghi khe 6-7, giữ nguyên khe 0-5.
3. **Màu khối thuộc tính** — trả về **tím 200,120,255 đúng ảnh mẫu VLTK** (Green của đợt 31/08b là tôi sửa lố).
4. **Đá khảm chưa hiện thuộc tính** — vẫn không tìm thấy lỗi tĩnh (đường đi + dữ liệu + INI đều sạch); thêm **lưới an toàn**: GetDesc trả rỗng thì in thô "Thuộc tính khi khảm (lỗ 10 sao): +N" — nếu build này viên đá hiện dòng thô ⇒ MagicDesc thiếu mẫu cho mã đó; nếu vẫn trống hoàn toàn ⇒ khối không được gọi, phiên sau gắn log.
5. **Chúc phúc hở dòng** — "Đột phá điểm chúc phúc" giờ cách phần dưới 1 dòng trống; danh sách lỗ thêm dấu cách trước `<color>` đóng (RULE 0).

## 6d. ĐỢT 31/08d — KIỂM CHỨNG CHUẨN DÒNG ẨN + 5 việc chủ giao thêm (t123-t126)

**Câu hỏi chủ: "dưới 10 sao hiện 1 dòng ẩn, đủ 10 sao mới hiện 2 dòng — đúng không?" — KIỂM CHỨNG 3 NGUỒN:**
1. **Linux (D:\ServerLinux)**: quét 12.005 file lua/txt/ini + ELF `jx_linux_y` — **KHÔNG có khái niệm dòng ẩn phi phong** (cột ẩn 55-56 goldequip rỗng toàn bộ; sao chỉ khuếch đại giá trị đá + điều kiện đột phá; mẫu gần nhất: platina cặp index (0级)/(10级) — 10 sao NÂNG GIÁ TRỊ chứ không thêm dòng).
2. **Bảng client VLTK**: goldequip để trống cả 8 khe magic cho toàn dải phi phong ⇒ dòng ẩn do server VNG roll, điều kiện không nằm trong data client.
3. **Trang VNG** (nguyên văn chú thích ảnh): *"[Phi Phong cấp 13 – Long Ngâm (10 sao) mới có hiện dòng ẩn 2]"* ⇒ **luật chủ nêu ĐÚNG**: dòng 1 có trước, dòng 2 mở ở 10 sao.

**Thiết kế chốt (t123 C++ + t126 Lua):** roll đủ 2 dòng NGAY khi lên Long Ngâm lần đầu (đột phá — hoặc lưới cũ: 9→10 sao/GM/tẩy cho món có sẵn); **dòng 1 hiện + phát huy từ đầu; dòng 2 chỉ hiện + phát huy từ 10 sao** — gate ở: (a) đầu vòng magic GetDesc overload 2 (`i > 6 && sao < 10 → continue`), (b) `nCountE = sao>=10 ? 2 : 1` trong Apply/Remove. Phi phong luôn đi nhánh màu SÁNG khe ẩn (magenta 255,0,255 gốc engine), guard `nGoldActiveAttrib--` khỏi âm.

**5 việc kèm:**
- **Bỏ khối [Thuộc tính ẩn] đáy tooltip** (chủ: dư thừa) — hoá ra vòng magic gốc overload 2 (2961+) VỐN vẽ khe 6-7 sáng/mờ theo `nGoldActiveAttrib` (tác tử tìm ra ở 3048-3087) ⇒ đợt 31/08b từng làm dòng ẩn hiện HAI lần.
- **Tooltip viên đá**: khối tự-giới-thiệu DỜI từ cuối hàm lên **ngay sau intro** overload 1 (~1920) — vị trí đã chứng minh hiển thị bằng ảnh chủ chụp (phần cuối hàm không hiện được với đá, cơ chế nuốt chưa rõ — mọi đường tĩnh đã rà sạch: DLL sống chứa khối ✓ md5 khớp ✓ disk-first mode 0 nên pak KHÔNG che settings ✓ data đủ ✓). PF_AppendDesc nhánh genre-9 giờ return trần (tránh in đôi).
- **"(Dương)" (t124)**: map data-driven từ MagicDesc.Ini + enum thực + bảng ProcessFunc — **7 mã đá đổi sang bản cùng tên có handler** (kháng ngũ hành 228-232→101-105, triệt tiêu 237→193, hồi phục 245→113); **9 mã không có bản tương ứng đăng ký** (anti_*_yan_p, lifemax_yan_v 233, manamax_yan_v 235, anti_sorbdamage 269) → **xoá chữ "(Dương)" khỏi mô tả** MagicDesc.Ini 2 cây (9 dòng, chỉ mã đá/phi phong/pool đang dùng — không đụng hệ khác). Sinh lực tối đa giữ mã 233 (bản 85 lifemax_v KHÔNG có handler — đổi là câm), chỉ bỏ chữ (Dương) ✓ đúng yêu cầu hiển thị.
- **Giảm 90% (t124 + vá bổ sung)**: 34/34 viên đá cột giá trị 15-24 tính lại TỪ BẢN GỐC backup (`max(1, round(v/10))` — Cường Công 228→23, Triệt tiêu 50→5); HP 11 dòng phi phong ÷10 (20000..70000 → 2000..7000). ⚠️ **CẤM chạy lại t124** (guard HP `v>=1000` không idempotent tuyệt đối); muốn chỉnh nữa thì làm từ backup `.truoc_duong_3108`/`.truoc_giam90_3108`.
- Ghi chú nền tảng: `KPakFile::Open` mode 0 (mặc định, `g_SetPakFileMode` không ai gọi) = **ĐĨA THẮNG PAK** — pak client tuy chứa magicattrib_ge/goldequip/MagicDesc cũ nhưng không che bản đĩa.

## 6e. CUỘC ĐIỀU TRA 01/09 — "thuộc tính có tác dụng thật không, có đúng Linux không"

Chủ giao mở điều tra. Quy mô: **13 tác tử, 2,7 triệu token, 653 lượt công cụ** (workflow `wf_4e536be5-66d`, chạy 2 đợt vì đợt đầu chạm hạn mức phiên). Phương pháp 4 cửa cho mỗi mã: có handler → handler ghi field gì → **có ai ĐỌC field trong công thức chiến đấu không** → công thức có đúng nghĩa không; rồi tác tử phản biện thử bác bỏ từng kết luận; song song mổ ELF Linux + bảng Linux đối chiếu.

### Kết quả tổng: 57 mã đang dùng, 56 có handler sống, KHÔNG mã nào "câm vì không ai đọc"
Tự kiểm chéo (không qua tác tử): 11 field mới của đợt PF13 đều có đúng 1 điểm đọc, nằm trong `CalcDamage`/`ReceiveDamage`/`ProcessState`, đều trong `#ifdef _SERVER` → chạy thật trên máy chủ.

### 3 viên đá VÔ DỤNG (giá trị luôn về 0 khi chạy) — cần chủ quyết
| Viên | Mã | Gốc bệnh |
|---|---|---|
| P=16 Hoa Lệ | 254 `manareplenish_p` | công thức lấy % CỦA chỉ số hồi nội lực phẳng, mà nền người chơi `PLAYER_MANA_REPLENISH = 0` (KPlayer.h) → 0 × X% = 0 |
| P=15 Thiểm Diệu | 190 `lifereplenish_p` | cùng bệnh, `PLAYER_LIFE_REPLENISH = 0` |
| P=7 Chí Mật | 113 `fasthitrecover_v` | `giam_tho_thuong = (m_CurrentHitRecover/10)*10` (KNpc.cpp:1800) cắt cụt bội 10; nền 6 + đá 2 = 8 → 0. Phải ≥ +4 mới nhảy bậc |

Hệ quả kèm: mã **196** `anti_hitrecover` (viên P=6 Xuyên Thích) cũng vô hiệu khi đánh mục tiêu có hit-recover < 10 — tức hầu hết người chơi.

### 1 LỖI THẬT — ĐÃ SỬA (t130, thuần dữ liệu, không cần build lại)
**P=1 Phác Tố**: Linux dùng mã **237** `sorbdamage_yan_p` (đơn vị **phần nghìn**, trần 500, cộng dồn với ~150 dòng trang bị "Dương"). Đợt t124 map 237→**193** `sorbdamage_p` (đơn vị **phần trăm**, công thức `KNpc.cpp:3794` nhân 10) → hậu quả: viên này **thoát khỏi đợt giảm 90%** (sao 10 = 50‰ = đúng bằng Linux, trong khi 33 viên kia còn 10%), **mất trần 500**, và **mất cộng dồn** (công thức lấy `max()` giữa hai nhánh chứ không cộng) nên nó *tranh chấp* với trang bị Dương thay vì cộng vào. Đã trả về **237** ở cả 2 cây (`magicattrib_ge` md5 `4c428bd8551c`) và vẫn xoá chữ "(Dương)" khỏi mô tả theo yêu cầu chủ (`MagicDesc.Ini` md5 `f23180a59f8c`).

### Đối chiếu Linux: ánh xạ mã ĐÚNG 100%
Tác tử mổ ELF `jx_linux_y` lấy được **nguyên bảng tên→mã 341 mục** của engine Linux (mảng trong `.bss` tại VA 0x0830E640) → đối chiếu xác nhận **18/18 ánh xạ** của các đợt t100/t124 trùng khít ngữ nghĩa (Linux 219=`anti_hitrecover`, 258=`anti_poisontimereduce_p`, 259=`do_hurt_p`, 263-267=`anti_*res_yan_p`, 271=`anti_enhancehit_rate`, 248=`manareplenish_p`…). 34/34 dòng bảng đá JX1 có **tên hàng trùng từng ký tự** với Linux → không viên nào trỏ nhầm hàng; 5 viên kháng hệ + viên hồi phục nắn xuống mã cơ bản đều **dùng chung handler** với mã `_yan` gốc ⇒ tương đương tuyệt đối. Công thức Linux (rút từ chuỗi log trong ELF): `anti_hitrecover`, `anti_stuntimereduce` đều **trừ thẳng**; kháng lưu theo bộ ba **Yin/Yan/Max** riêng.

### Hệ quả của đợt giảm 90%: 5 viên mất hết đường cong sao
P=9 Viên Nhuận, P=10 Kiên Cường, P=11 Đoạn Liệt, P=12 Ổn Cố, P=18 Quỷ Bí — trị gốc chỉ 4-12 nên sau khi chia 10 và làm tròn thì **sao 1 = sao 10 = 1**: nâng đá từ 1 lên 10 sao **không tăng gì**. 6 viên khác (P4/5/6/7/15/16) gần phẳng (8 cấp đầu như nhau).

### Phát hiện DI SẢN của dự án (không do đợt PF13 — đã truy commit, KHÔNG tự sửa)
1. **Kháng bị áp HAI LẦN** trong `CalcDamage`: khối phi tuyến `nDamage *= 2/(rate/100+2)` (3744-3748) rồi lại `nDamage -= nDamage*nRate/100` (3805). Cả hai đều có từ commit cũ (`9bc7936a` và `43bca2e0`) ⇒ mọi thuộc tính kháng/xuyên kháng mạnh hơn thiết kế Linux.
2. **Trần kháng thực tế là 75**, không phải 95 — `KPlayer::UpdataCurData` ghi đè `m_Current*ResistMax = BASE_FANGYU_ALL_MAX(75) + chuyển sinh`. Điểm kháng vượt 75 bị vứt bỏ trong chiến đấu (chỉ còn hiện số cosmetic).
3. **Kháng hệ vô tác dụng hoàn toàn khi đánh quái/pet** (`*ResistMax` mặc định 0 trong `KNpcTemplate`).
4. Công thức lợi ích giảm dần: +100 điểm kháng chỉ giảm **33%** sát thương (không phải 100%) — tooltip "#d1+%" gây hiểu nhầm nặng.
5. Nhịp hồi máu/nội lực thật là **0,556 s** (GAME_UPDATE_TIME 10 / GAME_FPS 18), tooltip ghi "mỗi nửa giây" ⇒ thực nhận ít hơn ~10%.
6. Mã **152** `fatallystrikeenhance_p` (dòng ẩn "tấn công chí mạng"): cờ `bIsFS` không được truyền cho lần `CalcDamage` sát thương **vật lý** ⇒ build thuần vật lý gần như vô hiệu; thêm nữa mô tả trong `MagicDesc.Ini` trùng nguyên văn với `deadlystrike_p` (nên sửa chuỗi).
7. Mã **161** `coldenhance_p` (viên P=20 Băng Hàn): mô tả ghi "%" nhưng mã **cộng thẳng** đơn vị thời gian.
8. Mã **97** `strength_v` có một nhánh chết (`m_nMeridianStrength` không ai đọc) — 3 nhánh anh em (Dexterity/Vitality/Energy) đều có người đọc.

## 6f. ĐỢT 01/09 — CHỦ GIAO 7 VIỆC (đá + KHÁNG giống Linux + UI tẩy luyện)

Chủ duyệt mục 1+2+3 điều tra và giao thêm: kháng + dame hệ giống Linux, hiệu ứng trạng thái/nội lực chính xác Linux, 5 điểm phát hiện làm như Linux, port UI tẩy luyện VLTK, **chạy phản biện chống mất cân bằng hệ phái**.

### Việc 1+2+3 — sửa đá (t130/t132, thuần dữ liệu)
- 3 viên vô dụng: **P15 Thiểm Diệu**→mã 88 (hồi sinh lực phẳng), **P16 Hoa Lệ**→mã 92 (hồi nội lực phẳng), **P7 Chí Mật** trả giá trị gốc Linux (1..18, vượt ngưỡng cắt-cụt-bội-10).
- 5 viên mất đường cong sao (P9/10/11/12/18): trả **giá trị GỐC Linux** (bản chia 10 làm sao1=sao10).
- **P1 Phác Tố**: 193→**237** (Linux phần nghìn, trần 500, cộng dồn — map cũ làm viên này thoát đợt giảm 90%).
`starstone` md5 `fa709277e064`, `magicattrib_ge` md5 `20148c819040` — 2 cây khớp.

### Việc 4 — KHÁNG dựng lại GIỐNG LINUX (t136, mổ ELF `jx_linux_y`, đã phản biện 3 tác tử)
Mổ hàm `BeHurt 0x08089C90` rút công thức thật:
```
a = max(D_yin - A_yin, D_yan - A_yan)     // 2 kênh kháng trừ xuyên
b = D_max - A_max                          // trần mềm
r = (a<=b) ? a : b + (a-b)*(95-b)/400      // SOFT-CAP (sub_8078910, chia 400)
rate = min(r, 95)                          // trần cứng 95, KHÔNG kẹp sàn
nDamage = nDamage * (100-rate) / 100       // TUYẾN TÍNH, áp ĐÚNG 1 LẦN
```
**JX1 cũ SAI 2 điểm** (đều là DI SẢN từ commit cũ, không phải PF13): ① áp kháng **HAI LẦN** (phi tuyến `2/(rate/100+2)` ở 3744 + tuyến tính 3805) → kháng mạnh gấp bội; ② kẹp CỨNG `min(ResistMax=75)` → vứt bỏ kháng vượt 75.
**Vá** (KNpc.cpp CalcDamage, 1 kênh của JX1 làm `a`, `m_Current*ResistMax` làm `b`): bỏ khối phi tuyến (còn 1 lần tuyến tính = Linux); thay kẹp cứng bằng soft-cap `/400` + trần 95; **guard 01/09b**: chỉ soft-cap khi `ResistMax>0` (người chơi = 75+reborn, xác nhận KPlayer.cpp:2877-2881), quái ResistMax=0 giữ kẹp cứng cũ → **không regression PvE**.

**Phản biện 3 tác tử** (opus): công thức **ĐÚNG 100% khớp Linux** (thứ tự trừ-xuyên-trước-softcap, hằng số /400 & trần 95, áp 1 lần). Bảng cân bằng người chơi (ResistMax=75): kháng yếu đi đồng đều vì bỏ double-apply — sát thương NHẬN tăng tương đối: R30 +15%, R45 +22.5%, R60 +30%, R75 +37.5%. **Đây ĐÚNG Linux** (bản cũ áp 2 lần làm mọi build thủ quá tanky). `damage_magic` (phản đòn) không đổi. Rủi ro quái-tanky đã chặn bằng guard.

⚠️ **Chủ cần biết**: đây là thay đổi cân bằng LỚN — mọi người chơi thủ kháng cao sẽ "giấy" hơn (giảm ~20-37% khả năng chịu đòn ở dải kháng 45-75). Đề nghị chủ ra soát/nâng HP-kháng boss PvE nếu chúng được tune dưới thời công thức cũ.

### Việc 5 — DAME CÁC HỆ (ngũ hành): CẦN CHỦ QUYẾT, chưa làm
Mổ ELF: **Linux BỎ bảng tương-khắc-cứng**, thay bằng hệ **data-driven** — mỗi item mang thuộc tính `me2Xdamage_p[5]` (tấn công +% theo hệ) và `X2medamage_p[5]` (phòng thủ −% theo hệ); `delta = atk.me2X[def.hệ] − def.X2me[atk.hệ]`, `dmg *= (1 + delta/100)`. Cộng `five_elements_enhance/resist_v` chuẩn hoá theo cấp `bonus = dmg*(100+(enh−res)*100/(cấp*8+200))/700` và `add_damage_p` (buff % chung). **JX1 hiện dùng bảng tương-khắc-cứng với `seriesdamage_p/2`** — KHÁC HẲN. Port đầy đủ cần: thêm 2 mảng 5 phần tử vào KNpc + 10 handler + **toàn bộ dữ liệu item me2X/X2me (JX1 KHÔNG có)**. Đây là dự án con lớn + cần data → **đề nghị chủ quyết**: (a) giữ hệ ngũ hành JX1 hiện tại, hay (b) port đầy đủ Linux (cần thời gian + dữ liệu item).

### Việc 6 — hiệu ứng trạng thái + nội lực: phần lớn ĐÃ khớp Linux
Đối chiếu (đã mổ Linux): **choáng** JX1 `base*(100-net)/100`, net = reduce−antiReduce trừ thẳng, trần 75% — **KHỚP Linux**. **Băng** JX1 tuyến tính trần 75 — gần khớp (Linux dùng config FreezeTimeReduceMax thay 75). **Sorb** JX1 cơ số 1000 trừ AntiSorb — **KHỚP Linux** (chỉ thiếu kẹp trần 500 cho nhánh SorbP kinh mạch — minor). **Poison2Mana / mana shield**: Linux mana-shield chặn theo %đòn (JX1 chặn phẳng nValue) — KHÁC, cần mổ thêm. **Poison time** JX1 `factor=1+(75−giảm)/75` (nhân đôi base khi không kháng) — Linux chưa mổ rõ vị trí. → 2 điểm này để đợt sau (cần mổ thêm), không đụng khi chưa chắc.

### Việc 7 — 5 điểm phát hiện
- **Kháng áp 2 lần** → ✅ ĐÃ SỬA (chính việc 4).
- **Trần 75 không phải 95** → ✅ xử lý bằng soft-cap (kháng vượt 75 giờ có lợi giảm dần đúng Linux).
- **Nhịp hồi máu 0,556s** (GAME_FPS=18): là hằng số toàn engine, KHÔNG nên đụng — chỉ tooltip ghi "nửa giây" gây hiểu nhầm. Để nguyên.
- **Mã 152 chí mạng không áp sát thương vật lý** + **mã 161 mô tả "%" nhưng cộng thẳng** → 2 điểm nhỏ, xử ở đợt kèm.

## 6g. ĐỢT 01/09 (tiếp) — UI TẨY LUYỆN + phát hiện NGŨ HÀNH

### UI Tẩy luyện VLTK — ✅ DỰNG XONG (build sạch cả 3, chờ chủ test in-game)
Chủ chọn cơ chế **"xem trước rồi chọn"** đúng mẫu VLTK. Đã trích box ini + 6 spr (`\spr\Ui4\主界面\新五行印\ui\`), đặt `Ui/Ui3/mantlewash.ini`. **Không thêm gói server→client mới** (né Gate 2): tái dùng give-box `nType=4` để mở + `ITEM_SYNC_MAGIC` sẵn có để cập nhật item; thêm 1 `GetGameData(GDI_MANTLE_HIDDEN_DESC)` để panel đọc 2 dòng ẩn (khe 6-7) hiển thị.
- **Panel** `KUiMantleWash` (S3Client, `UiMantleWash.h/.cpp`): ô đặt phi phong + cột TRƯỚC (xám, snapshot lúc bấm Tẩy) + mũi tên + cột SAU (xanh, dòng hiện tại) + 3 nút. Client tự nhớ dòng cũ khi bấm Tẩy.
- **Nối**: `KProtocolProcess::OpenAffairBox` case 4 → `GDCNI_OPEN_MANTLE_WASH`; `GameSpaceChangedNotify` mở panel + đóng theo `END_AFFAIR_BOX` + cập nhật item; `CoreShell.h` +2 enum (`GDCNI_OPEN_MANTLE_WASH`, `GDI_MANTLE_HIDDEN_DESC`); `KJx2WarInfra` `LuaPF_OpenMantleWashBox`; `ScriptFuns` register `OpenMantleWashBox`.
- **Lua** (npc.lua): menu "Tẩy luyện thuộc tính ẩn (bảng xem trước)"; `doWashRoll` (lưu dòng cũ vào `tb.tbWashCu` + trừ nguyên liệu 1 Lệnh Bài / 2 ĐBL + 20000 vạn + `PF_RollAnAttr` ghi dòng mới + mở lại panel); `doWashKeep` (khôi phục `tbWashCu`); `doWashApply` (giữ dòng mới). Mỗi lần bấm Tẩy trừ nguyên liệu (như VLTK).
- Cơ chế: bấm Tẩy → item bị ghi dòng mới ngay (đã sync), "Giữ nguyên" khôi phục dòng cũ từ session; nếu thoát giữa chừng thì giữ dòng mới (chấp nhận được).

### Việc 5 — NGŨ HÀNH: điều tra xong, CÓ BẪY ENUM nghiêm trọng, cần đợt riêng
Mổ dữ liệu Linux: hệ ngũ hành Linux dùng `me2Xdamage_p`/`X2medamage_p` (10 mã, 2 mảng 5 phần tử) + `five_elements_enhance/resist_v`. **Dữ liệu Ấn ngũ hành ĐÃ có trong TESTLOFFF** (goldequip `Ngũ Hành Ấn mới` :5099/:5104 mang các mã này). **NHƯNG BẪY**: dữ liệu dùng bố cục mã **liền mạch Linux 276-285** (đã kiểm: dòng 5099 cột magic → GE record mã **276** `对金系伤害增加`), trong khi **engine JX1 (D:\GAMEDEVNEW) đọc 276-285 = `range_returnres_p` + `addskilldamage1-9`** — nghĩa HOÀN TOÀN KHÁC. Mã me2X/X2me thật của JX1 nằm RẢI RÁC: me2metal=253, metal2me=249, me2wood=206, wood2me=256, me2water=257, water2me=258, me2fire=259, fire2me=260, me2earth=242, earth2me=262.
⇒ **Các Ấn ngũ hành trong TESTLOFFF ĐANG BỊ HỎNG** (thuộc tính đọc sai thành addskilldamage) — đây là bug hiện có, ngoài phạm vi phi phong nhưng chủ nên biết.
**Để port hệ ngũ hành Linux** cần (dự án con lớn, rủi ro cân bằng cực cao → làm ĐỢT RIÊNG với nhiều vòng phản biện): (1) NẮN dữ liệu item 276-285 → mã JX1 tương ứng theo TÊN HÀNG (như t100 nắn đá); (2) thêm 2 mảng `me2X[5]`/`X2me[5]` vào KNpc + 10 handler đăng ký (JX1 hiện chỉ KHAI enum, KHÔNG dùng); (3) sửa CalcDamage áp `delta = atk.me2X[def.hệ] − def.X2me[atk.hệ]; dmg *= (1+delta/100)`; (4) phản biện cân bằng đa vòng. Vì JX1 hiện dùng bảng-tương-khắc-cứng, chuyển sang data-driven là thay đổi cân bằng toàn cục.

## 6h. ĐỢT 02/09 — PHI PHONG POST LÊN KÊNH CHAT THIẾU HIỂN THỊ (nhãn `[PFCHAT 02/09]`)

Chủ báo kèm 2 ảnh: mặc trên người tooltip đủ (10 sao, 13 đá, khối tím, 2 dòng ẩn); post lên kênh chat rồi bấm xem chỉ còn "0 sao Lỗ khảm trống" ×13, tên không có "10 sao", mất dòng ẩn 2.

### Cơ chế item-chat (mổ 02/09) — KHÔNG phải gói nhị phân
- Ctrl+click item → `GDI_GET_ITEM_PARAM` (CoreShell.cpp ~2421) đổ `KItem` → struct `ChatItem` (GameDataDef.h ~613, `#pragma pack 1`) → `KUiPlayerBar::SetChatItem` (UiPlayerBar.cpp ~2093) đóng thành CHUỖI `[23 trường,16 khe magic,]` — mỗi số kèm dấu phẩy, tổng **39 dấu phẩy = `NUM_INFO_ITEM_CHAT`** — thay cho `<tên>` trong tin nhắn lúc gửi (UiPlayerBar.cpp ~1298).
- Phía nhận: `UiMsgCentrePad.cpp` có **hai** bộ giải mã (kênh ~323, chat mật ~810) tìm `[`…`]` đúng số dấu phẩy rồi đọc từng số → `GDI_ITEM_CHAT` (CoreShell.cpp ~2327) dựng item **TẠM** (`ItemSet.Add` theo nature/row) để lấy tên/màu; bấm tên → `KUiChatItem` gọi `GDI_CHAT_ITEM_DESC` = `KItem::GetDesc` trên item tạm dựng lại từ `ChatItem` lưu trong nút (`KWndPureTextBtn::m_Item`) / tin nhắn (`KOneMsgInfo::sItem`).
- `CoreShell.cpp:19293` (`ChatSpecialPlayer`) chỉ đếm dấu phẩy để chặn chat NPC. Bot server cũng sinh chuỗi này: `KPlayerBot.cpp pb_TaoLinkDo` (số dấu phẩy phải khớp, không thì link bot thành chữ thô).
- `ChatItem` còn nằm TRONG gói `s2c_diceitem` (`Headers\KDiceProtocol.h`, `KItemDice::FillItemDesc`) — xúc xắc Viêm Đế dùng chung đường `GDI_ITEM_CHAT`.

### Gốc lỗi
Chuỗi **không mang `KItem::m_nPfPack[4]`** (sao / chúc phúc / 13 lỗ đá — item của chính mình nhận qua gói riêng `s2c_syncpfpack`, PFSYNC 31/08) → item tạm 0 sao, 13 lỗ trống; dòng ẩn 2 bị cổng `sao < 10` (KItem.cpp ~3393) giấu; dòng ẩn 1 vẫn hiện vì khe 6 nằm trong 16 khe magic của chuỗi. Tooltip đá/thuộc tính đá được `PF_AppendDesc` tính từ pfpack + bảng ở client nên chỉ cần đủ 4 ô là hiện y hệt.
**Bẫy kèm**: `KUiChatItem` (cửa sổ xem item chat) dùng đệm **2048** (`szTitle` trên stack + `m_ObjTitle`) trong khi `GetDesc` ghi cả tooltip nối tiếp (13 đá vượt 2048 — chính lý do 31/08b nâng `GOD_MAX_OBJ_TITLE_LEN` lên 4096). Chỉ truyền pfpack mà không nới đệm = **tràn stack Game.exe** khi bấm xem.

### Vá — 9 tệp, build sạch `-t:Rebuild` (đổi header dùng chung GameDataDef.h)
1. `GameDataDef.h`: `ChatItem` thêm `int m_nPfPack[4]` **cuối struct** (89 → 105 byte); `NUM_INFO_ITEM_CHAT` 39 → **43**.
2. `CoreShell.cpp`: `GDI_GET_ITEM_PARAM` chép 4 ô từ item thật; `GDI_ITEM_CHAT` đổ 4 ô vào item tạm (`SetPfPack`) — phủ luôn nút trong khung chat (`UpdateChatItem`), chat mật, xúc xắc. Món không phải phi phong 4 ô = 0 (KPlayerDBFuns.cpp ~736 đã dọn) nên hành vi cũ không đổi.
3. `UiPlayerBar.cpp/.h`: `SetChatItem` ghi thêm 4 số (mỗi số + dấu phẩy) sau 16 khe magic; đệm `m_ChatItemInfo` 128 → 320 (chuỗi tối đa ~264 ký tự).
4. `UiMsgCentrePad.cpp`: 2 bộ giải mã đọc thêm 4 số → `CItem.m_nPfPack` (có chặn `nLeng` khỏi tràn `szNum`).
5. `KPlayerBot.cpp pb_TaoLinkDo`: ghi thêm `0,0,0,0,` (+ cập nhật comment 44 trường).
6. `KItemDice.cpp FillItemDesc`: gói xúc xắc mang pfpack.
7. `UiChatItem.h/.cpp`: đệm 2048 → `GOD_MAX_OBJ_TITLE_LEN`, thêm `#include GameDataDef.h`, điều kiện `<= sizeof(m_ObjTitle)`.
Mô phỏng python bộ mã hoá/giải mã: 43 dấu phẩy, đọc lại đúng 16 magic + 4 pfpack; 4 ô pfpack bố cục v2 không dùng bit 31 → `%d`/`atoi` an toàn. Encoding: số byte cao 9 tệp giữ nguyên, FFFD = 0 (chỉ còn `CoreServerShell.cpp` hỏng sẵn từ trước).

### ⚠️ RÀNG BUỘC SWAP + giới hạn
- `ChatItem` nằm trong gói `s2c_diceitem` (cỡ = `sizeof`) → gói lớn thêm 16 byte → **CoreServer.dll + CoreClient.dll + Game.exe PHẢI đổi tên cùng lúc** (lệch bản = client tách gói sai khi có xúc xắc). WAuto PC (`E:\Src_Auto_Ngoai`) không dùng `ChatItem`/gói này — đã grep.
- GameServer vứt gói chat ≥ 255 byte (`KSOServer.cpp:2778`, chặn có sẵn): link phi phong dài ~150-160 ký tự → chữ kèm theo còn ~80 ký tự (kênh) / ~50 (chat mật), trước là ~125/~95. Link đồ thường dài thêm 8 ký tự.
- Tin nhắn link cũ (định dạng 39 dấu phẩy) nếu còn được kênh phát lại sau swap sẽ hiện thành chữ thô `[...]` — vô hại, tạm thời.

### Phát hiện thêm — DI SẢN, CHƯA sửa (chờ chủ quyết)
`GDI_GET_ITEM_PARAM` (CoreShell.cpp ~2450): vòng `for (i < MAX_ITEM_MAGICLEVEL=16)` ghi `m_btMagicLevel[i + 8]` với i ≥ 8 → **tràn ngoài mảng 16 short** → đè `m_wVersion` / `m_dwRandomSeed` / `m_nIdx` / `m_uPrice` (và đọc `m_aryMagicAttrib[8..15]` ngoài mảng 8) mỗi khi khe giá trị 8-10 > 10 — tức đồ hoàng kim có value ở khe 0-2. Đây là gốc của ghi chú cũ "fix loi m_dwRandomSeed sai option đồ xanh chatitem" ở bộ giải mã. Không chạm pfpack (nằm sau `m_nMaxOptMultiply`). Sửa đúng = giới hạn vòng ở 8 (`MAX_ITEM_MAGICATTRIB`); chưa làm vì ngoài phạm vi chủ giao.

### Nhị phân `.moi` CHỜ SWAP (build 02/09 06:38-06:39, obj đồng nhất một mốc)
| Tệp | md5 (12 đầu) | Bản đang chạy trước swap |
|---|---|---|
| `bin\server\CoreServer.dll.moi` | `fba0955f6893` | `0a0cc35205a0` (HEAD `5a975674` VHTD 02/09d) |
| `bin\client\CoreClient.dll.moi` | `50e1f30e72eb` | `f52ddc8e7219` |
| `bin\client\Game.exe.moi` | `8911735e768f` | `359536c5b29b` |
Bản mới = HEAD + vá này (superset, không rơi đợt nào trước).

### Checklist test cho chủ
1. Tắt GameServer → chạy `ChayGameServer.bat` (đổi `CoreServer.dll.moi` → `CoreServer.dll`); thoát hẳn Game.exe → chạy `ChoiGame.bat` (đổi `CoreClient.dll.moi` + `Game.exe.moi`). Restart mà chưa đổi tên thì vẫn chạy bản cũ.
2. Ctrl+click phi phong 10 sao đủ 13 đá → gửi kênh → bấm tên item trong khung chat: cửa sổ phải hiện "10 sao … (Cấp 13)", "Đột phá điểm chúc phúc x/44", 13 dòng "10 sao <tên đá>", khối tím thuộc tính đá, cả 2 dòng ẩn — y hệt tooltip mặc trên người. Thử cả từ máy khác nhận.
3. Chat mật kèm link phi phong: như 2.
4. Link đồ thường (vũ khí/áo hoàng kim, đồ xanh) vẫn mở được; link bot post lên kênh thế giới vẫn bấm được (không thành chữ thô).
5. Xúc xắc Viêm Đế: chia 1 món → ô hiện đúng icon + chú giải (gói đổi cỡ).

## 6i. ĐỢT 02/09 — "Tẩy thuộc tính ẩn: bấm Giữ thuộc tính báo lỗi script" (nhãn `[WASH-KEEP 02/09]`)

**Bằng chứng** `bin/server/ScriptError.log` 06:38:42: `error: stack Overflow` — traceback `getn` ← `unpack` (script/lib/common.lua:20/23) lặp 24 tầng ← `doWashKeep` (mantleupgrade_npc.lua:485), Script Name = thoren.lua (hàm chạy trong state của Thợ Rèn, nơi Include mantleupgrade_npc.lua).

**Gốc**: `unpack` của `script/lib/common.lua` là hàm ĐỆ QUY, mỗi phần tử tốn một tầng gọi. `doWashKeep` gọi `SetMagicAttrib(idx, unpack(tb.tbWashCu))` với `tbWashCu = pack(GetMagicAttrib(idx))` = **24 giá trị** (8 khe × 3: type, value0, value2 — ScriptFuns.cpp LuaGetMagicAttrib). Engine mở mỗi state Lua chỉ **100 ô stack** (`Engine/Src/KLuaScript.cpp:23 lua_open(100)`) nên 24 tầng đệ quy tràn. Các nơi khác dùng `unpack` chỉ với bảng 2–6 phần tử nên chưa bao giờ lộ. Nút "Tẩy" không lỗi vì `pack` không đệ quy; "Áp dụng" không đụng unpack.

**Vá (thuần Lua, không build)**: `mantleupgrade_npc.lua doWashKeep` truyền thẳng 24 ô `c[1]..c[24]` y như `PF_GhiKheAn` (head.lua:110) — không dùng `unpack`. Backup `mantleupgrade_npc.lua.truoc_washkeep_0209`. `syncheck` (Lua 4.0 thật) OK cho npc.lua, head.lua, thoren.lua; TCVN3 giữ nguyên (1087 byte cao).

**Có hiệu lực khi**: script được nạp sẵn lúc boot và Include vào state thoren.lua → cần **restart GameServer** (đằng nào cũng phải restart để swap `CoreServer.dll.moi` mục 6h). Không có lệnh reload sẵn cho thoren.lua (`hoatdong_admin.lua HD_RELOAD_LIST` chỉ reload danh sách cố định) — muốn có nút reload riêng thì báo, chưa tự thêm.

**Không sửa `common.lua unpack`** (dùng chung ~20 chỗ; đổi thành `call()` là đổi hành vi toàn cục). Ghi nhớ luật: trên JX1 KHÔNG dùng `unpack` với bảng > ~10 phần tử; bảng lớn thì truyền thẳng chỉ số hoặc `call(f, tb)` (Lua 4.0 có sẵn, dailogsay.lua:58 đang dùng).

**Thấy thêm trong log (không thuộc phi phong, chưa đụng)**: 06:26:25 hàng loạt `ScriptError attempt to call a nil value` ở `npcthon/npcmonphai/tieudao.lua` và `global/vhtd/npc_chao.lua` hàm `OnRevive` — thuộc đợt Vũ Hồn/Tiêu Dao 02/09.

**Test**: Thợ Rèn → Tẩy luyện thuộc tính ẩn → đặt Long Ngâm 10 sao → Tẩy → **Giữ nguyên**: không còn báo lỗi script, 2 dòng ẩn trở về đúng dòng cũ (cột trước), bảng mở lại; rồi Tẩy → Áp dụng: giữ dòng mới.

## 7. VIỆC KẾ TIẾP
1. Chủ duyệt 3 lệch VNG (mục 3) + số nguyên liệu nội suy (mục 2) + Phệ Quang/Khấp Thần dùng chung hình 7 (muốn hình riêng từng bậc như JX1 cũ thì trả goldequipres 5939/5940 về 8/9).
2. Cho 4 nguyên liệu mới vào tiệm 186 (onMaterialShop) nếu muốn bán cho người chơi.
3. Phi phong bạch kim Vô Cực cũ (3485/4835-4839) là di sản Linux — không vào được chuỗi mới (menu Vô Cực vẫn tắt); nếu người chơi đang giữ, cần chủ quyết đường quy đổi.
