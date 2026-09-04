# BÀN GIAO 01/09 chiều — CHÍ TỬ chuẩn Linux · CẮT KINH MẠCH · ÉP ẤN · NGUỒN NGUYÊN LIỆU PHI PHONG

Tất cả sửa **dữ liệu + Lua** ở cây chạy thật `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` (mỗi tệp có bản `.truoc_*_0109` cạnh bên). Phần C++ chí tử đã build trước (commit `d22b24ca`, `.moi` CoreServer `CEE660A0FF78` / CoreClient `A660678EE4EA`). **Restart là ăn hết.**

## 1. Chí tử — chuẩn Linux (commit `d22b24ca` + dữ liệu)
- C++ (đã build): xác suất chỉ theo chiêu (không trừ kháng); sát thương = **1/4 máu hiện tại × (100 − kháng chí tử)/100**, trừ thẳng máu, **1 lần/đòn**, tự thêm khâu chết. Bỏ random 30-50% + DamageReduction. Giữ `KNpc.cpp:4568` (Linux đấu chéo 71/72 là lỗi của Linux — không chép).
- Dữ liệu: **bỏ chí tử khỏi Thiên Ngoại Lưu Tinh** (JX1 tự thêm 1→60%, Linux không có): `skills.txt` dòng 363 cột 92 `fatallystrike_p` → rỗng (giữ cột 93 = `tianwai_liuxing` đúng khuôn Linux); `tianren.lua` xoá dòng `fatallystrike_p={{{1,1},{20,60}}}`. Backup `.truoc_thienngoai_0109`.
- Còn lệch nhỏ chưa sửa (chủ chưa yêu cầu): Tất Hoả Liêu Nguyên JX1 10→30%, Linux 10→25%.

## 2. Cắt kinh mạch — giữ động lực nâng mạch
Cách cắt: **scale tỉ lệ từng hàng** (giữ nguyên đường cong tăng dần — mỗi huyệt vẫn thấy tăng), làm tròn, **sàn 1** (không huyệt nào thành "cấp chết"). Sửa `magicattrib_ge.txt` cột 6+7 của **321 hàng** kinh mạch (384 huyệt tham chiếu 361 hàng; tổng tính trên tham chiếu, mỗi hàng scale đúng 1 lần). Backup `.truoc_cat_kinhmach_0109`.

| Mã | Ý nghĩa | Trước | Sau |
|---|---|---|---|
| 243 | tăng sát thương kỹ năng | 925 | **101** |
| 224 | xuyên kháng | 208 | 61 |
| 269 | xuyên triệt tiêu | 718 | 152 |
| 114 | kháng tất cả | 144 | 60 |
| 246/247 | ngũ hành cường/nhược | 6770 | 2002 |
| 86 | sinh lực tối đa % | 339 | 79 |
| 97/98/99/100 | 4 chỉ số | 730-740 | 199-201 |
| 226/227/252/270 | hoà giải / trọng kích | 30 | **24** (sàn 1 × 24 huyệt — không thể xuống 10 nếu không tạo cấp chết) |
| 264/305 | | 21 | 11 |
| 51 | | 15 | 11 |

**KHÔNG đụng** (giữ lợi ích nâng mạch): 190 hồi thuốc +176%, 113/196 phục hồi, **khối thưởng cấp-32** `KMeridian.cpp:79-93` (KM_L32: +200/+200 ngũ hành, +5000 máu, +5 kháng ×5 — cột mốc, hardcode, muốn đổi phải build). 4 hàng dùng chung trang bị 2937/2938/2940/2943 (type 190) không đụng.
Ước tính (cùng phương pháp phản biện): tỉ số sát thương full/0 mạch **~430 → ~14 lần**, sức mạnh tổng ~1900 → ~26. Full mạch vẫn đánh gấp ~5, trâu gấp ~1.9.
Cảnh báo đã biết: sát thương lên **quái** tụt (quái ResistMax=0 nên xuyên kháng âm không bị chặn) — rà máu boss sau restart.

## 3. Ép Ấn (Hoàng Kim Ấn) — bản Linux `wuxingyin.lua`
- `thoren.lua`: `Include` có điều kiện (`if not WXY_MoBang`) — vì `main()` `dofile` lại chính tệp mỗi lần nói chuyện, tránh `tbWuxingyin:Register()` đăng ký trùng; mục menu **"Nâng cấp Hoàng kim ấn/onNangCapHoangKimAn"** sau "Tăng cấp Phi Phong"; hàm bọc gọi `WXY_MoBang()` (cùng Lua_State — bắt buộc, CẤM DynamicExecute). Backup `.truoc_ep_an_0109`.
- Luật: 1 Ấn cấp N + 1 **Vương Thiết Tượng Lệnh Phù (4889)** → cấp N+1; 35/45/55/65/75%; thất bại chỉ mất Lệnh Phù; trần cấp 6; hạn 7 ngày (cấp 2-5) / 30 ngày (cấp 6).
- **ĐÍNH CHÍNH — Ấn Chiến CÓ THẬT** (tìm sai tên lần trước): hệ `series_stamp` / Tân Ngũ Hành Ấn, DetailType 15. Server Linux **không có logic**; client VLTK **có trọn** (8 ini, script client, 3 bảng, 25 mã protocol, sprite). JX1 đã có enum/ô cho DetailType 15/16 nhưng đang dùng cho "ngoại trang mũ/áo" tự chế (Hoods/Cloak) — xung đột phải chủ quyết. Server phải tự thiết kế + 1 gói s2c mới. Chi tiết: memory `jx1-chitu-kinhmach-an-0109`.

## 4. Nguồn nguyên liệu Phi Phong + Lệnh Phù
- Gốc tắc: **Tinh Hỏa Than (4886) 0 nguồn** → `item_starstone.lua:25` chặn → cả chuỗi 4881-4888 = 0.
- `goods.txt` +2 dòng (schema JX1 11 cột, cột 8 = giá **xu**): `4886 Tinh Hỏa Than = 20 xu` (chủ chốt) · `4889 Lệnh Phù = 50 xu` (**giá tạm** — Linux bán 300 "năng lượng", JX1 không có loại tiền này; chủ chỉnh ở cột 8 dòng 772).
- `buysell.txt` +shop data row **102** "Nguyên liệu Phi Phong" (goods 770, 771).
- `mantleupgrade_npc.lua` `onMaterialShop`: bản cũ `Sale(186, 25, …)` — **shop 186 không tồn tại** (chỉ 101 shop) + moneyunit 25 không hợp lệ ⇒ tiệm cũ **chết**. Nay menu: **Mua bằng xu** `Sale(102, 1, 0)` (LuaSale chỉ đọc tham số 2 khi có ≥3 tham số — `ScriptFuns.cpp:2706`) + **Đổi Tinh Ngọc (4881)** theo giá Linux: Thiên Tinh Ngọc 4882 = 1000, Mảnh 4883 = 100, Tinh Thần Khoáng 4887 = 200 (khuôn `item_tianxingyu.lua`).
- 5/12 nguồn Nguyên Thạch đang sống (Viêm Đế, Phong Lăng Độ, Sát Thủ Boss, Vượt Ải 29, rương thưởng) — tỉ lệ đối chiếu Linux: xem mục 6 (bổ sung sau khi tác tử đối chiếu xong).
- 5 hộp "Phi Phong Bảo Hạp" Linux bán (4658-4662) JX1 chưa có item — chưa port.

## 5. Kiểm sau restart
1. Thợ rèn → "Nâng cấp Hoàng kim ấn": mở bảng đặt Ấn + Lệnh Phù; thất bại chỉ mất Lệnh Phù.
2. Thợ rèn → Tăng cấp Phi Phong → Tiệm nguyên liệu: mua Tinh Hỏa Than 20 xu; đổi 1000 Tinh Ngọc lấy Thiên Tinh Ngọc.
3. Dùng Tinh Hỏa Than mở Nguyên Thạch (item_starstone) ra Tinh Ngọc/Mảnh/Tinh Thần Khoáng.
4. Thiên Nhẫn dùng Thiên Ngoại Lưu Tinh: không còn nổ chí tử; Ma Diệm Thất Sát vẫn nổ, sát thương = 1/4 máu hiện tại.
5. Kinh mạch: bảng thuộc tính nhân vật full mạch giảm theo bảng mục 2; nâng thêm 1 huyệt vẫn thấy chỉ số tăng.

## 6. Bẫy ghi lại
- `LuaSale(id, unit)` phải truyền **3 tham số** mới đọc unit.
- `buysell.txt` giá trị = **dòng goods.txt − 1** (1-based data row); `Sale(id)` → row 0-based = id − 1.
- `meridian_level.txt` cột 7 (1-based) = magicType → `magicattrib_ge.txt` dòng file = magicType + 1.
- TCVN3 không có "Ấ" hoa → nhãn menu dùng "ấn" thường (như wuxingyin.lua).
- `syncheck.exe` Lua 4.0 build được bằng `cl` với TOÀN BỘ `LuaLib\src\*.c` (+`baselib\`), bat cũ trỏ scratchpad phiên cũ.

## 6. Đối chiếu tỉ lệ 5 nguồn Nguyên Thạch với Linux (tác tử w8cbcfhws)
`other_func_outputs.lua` hai bên **giống 100%** trừ mã item (4885 vs 4550). Cơ chế `awardtemplet:GivByRandom`: nRate là % cộng dồn, phần dư = không nhận; **túi đầy = MẤT thưởng** (chỉ Msg2Player).

| Nguồn | JX1 | Linux | Kết luận |
|---|---|---|---|
| Viêm Đế boss cuối | 2 viên chắc, người kết liễu | như nhau | KHỚP (Linux thêm 1 viên ở Hộ tống Long Môn Tiêu Cục act.129 — JX1 không có) |
| Phong Lăng Độ Thủy tặc đầu lĩnh | 60%×1 / 40%×2 mỗi boss | như nhau | KHỚP |
| Sát Thủ Boss | 10%×1 | như nhau | KHỚP |
| Vượt Ải batch 29 | 2 viên chắc cả đội | như nhau | KHỚP |
| Rương thưởng | 1%×1, 4 tệp phát | 1%, 7 tệp phát | tỉ lệ KHỚP; JX1 thiếu 3 rương: Thiên Trì Mật Cảnh, Mê cung, Tống Kim mật bảo |

Khác nhỏ: `lib\awardtype\item_jx1.lua` không `Msg2Player("Nhận được N …")` như Linux `item.lua:124` → người chơi tưởng không rơi (đã thêm — xem mục 7).

Shop Linux: 186 "pifeng" bán Thiên Tinh Ngọc 1000 / Mảnh 100 / Tinh Thần Khoáng 200 / Tuyệt Thế Bảo Hạp 100 (bằng Tinh Ngọc); Tinh Hỏa Than 20 Tiền Đồng + Tinh Thần Khoáng 200 Tiền Đồng ở shop 139 "Hot"; Lệnh Phù 300 **Tinh lực** (UniPriceType 15) ở shop 174. Tinh Ngọc (tiền 25) = đếm item 4546 trong túi; nguồn: mở Nguyên Thạch (~27 Tinh Ngọc/viên), Hoạt Diệu Độ 20/30/50.

Map Linux→JX1: 4546→4881 · 4547→4882 · 4548→4883 · 4549→4884 · 4550→4885 · 4551→4886 · 4552→4887 · 4553→4888 · 2839→4889. JX1 KHÔNG có 4658-4662 (Bảo Hạp), túi 4571-4574; 4933-4937 là JX1 tự chế.

## 7. Hội thoại tẩy luyện (chủ yêu cầu thêm)
`mantleupgrade_npc.lua`: nhãn menu + tiêu đề box → "Tẩy luyện thuộc tính ẩn Phi Phong"; `onWashMantle` giờ hiện hội thoại giới thiệu (Bắt đầu / Trở về / **Kết thúc đối thoại**), thân cũ đổi thành `onWashMantleBox`. Backup `.truoc_hoithoai_wash_0109`.
**Box tẩy luyện ĐÃ LÀM LẠI đúng client VLTK** (commit `9dd11893`, `Game.exe.moi` `F71C0529BDE5`): lớp VLTK `KUiMantleImplicit` = `main.ini` (khung 582×336, tiêu đề, nút đóng, 2 tab) + `pageshift.ini` + `pageactivate.ini`, rút nguyên byte từ pak client VLTK (`C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data\{1024,slistcache,updatejx15}.pak`) + 12 spr → `Ui\Ui3\mantleimplicit_*.ini` + `\spr\vng\ui\mantleimplicit\*.spr` (đường dẫn ASCII, tránh codepage). `KUiMantleWash : KWndPageSet` (khuôn UiCompoundItem), 2 trang `KWndPage`. Tab 2 "Kích hoạt": VLTK kích hoạt dòng ẩn **có thời hạn 14 ngày** (gói C2S riêng, server trừ Hoàng Tinh Ngọc) — JX1 dòng ẩn luôn hiệu lực → trang chỉ hiển thị dòng ẩn nền "đã kích hoạt", nút tắt. Bản cũ sai: chỉ chép 1 trang, không khung/tiêu đề/tab, `Init("CloseBtn")` section không tồn tại. Bẫy: `KWndText` Init đọc `Text` tối đa 256 byte → mô tả dài đọc riêng bằng `GetString` 1024 + `UiAffair_EncodeDesc` (hỗ trợ `<enter>`) đổ vào `KWndMessageListBox`.

## 8. Chiều 01/09 — (Dương) kinh mạch, nội lực hộ thân, phản biện
- **Thuộc tính (Dương) kinh mạch**: 224 `anti_allres_yan_p` → **307 `magic_anti_allres_p`**, 269 `anti_sorbdamage_yan_p` → **309 `magic_anti_sorbdamage_p`** (thêm cuối enum, cùng handler/đơn vị; 306 và 308 để trống vì dữ liệu VLTK/pet đang chiếm). 46 hàng `magicattrib_ge.txt` đổi mã; 8 nhãn huyệt "(Dương)" của 113 sửa nhãn (thuộc tính vốn không-Dương). Tầng 2 cắt thêm: 113/196 28→11, 190 176→59 (4 hàng dùng chung trang bị → tạo hàng mới 7206-7209). Client đã đồng bộ 6 tệp lệch (magicattrib_ge, meridian_level, MagicDesc.ini, goods, buysell, skills dòng 363).
- **Nội lực hộ thân** (commit hộ thân): khiên tĩnh `staticmagicshield_p` theo Linux — bể = nội lực tối đa × pct/100, gỡ → 0; hấp thụ sát thương VỪA QUAY trước kháng/giáp; đòn < bể → không mất máu; bể cạn đúng (bug cũ không cạn). Gỡ trừ phẳng `m_ManaShield`. ⚠️ `kunlun.lua` Côn Lôn 120 1800..10050 giống Linux ⇒ bể = 18×..100× nội lực tối đa — chủ cân nhắc hạ dữ liệu. `dynamicmagicshield_v` Linux = xác suất chặn trọn + phản kích bằng chiêu (0x080864B8 → 0x8086260, [0x8fbfdd4]) — **chưa port**, JX1 vẫn "hấp thụ 10/40%".
- **Phản biện** bắt 7 lỗi đã sửa (308 đụng pet; snapshot box; MsgColor; OverFrame; ConsumeItem trả số món; tbAllPlayerProcess reset; thiếu Kết thúc đối thoại). Chi tiết memory `jx1-kiemtoan-thuoctinh-0109`.
- **Dung luyện**: đã đọc trọn Linux (`smelt_system.lua`, `smeltsystem\smeltupgrade.lua`, `fusion.txt` 352, `smeltupgradetable.txt` 352, API C `GetFusionInEquipInfo`/`SetFusionMagicSeed`/`SetFusionIsSmelted`/`UnSmeltIsBind`/`SmeltItem`; `SmeltEquip`/`UnSmeltEquip` do C++ gọi Lua `DoSmeltEquip`). Linux lưu **6 ô fusionP + 6 seed trên KItem** (+0x240/+0x258). JX1 thiếu: loader `fusion.txt` (genre 8), item Văn Cương, ô lưu trên KItem, DB, sync, box. **Chờ chủ cho phép đổi DB/giao thức.**
