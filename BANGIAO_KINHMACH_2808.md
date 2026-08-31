# BÀN GIAO — HỆ KINH MẠCH 12 MẠCH × 32 HUYỆT (đợt 27–28/08/2026)

> **Đọc file này TRƯỚC KHI đụng bất cứ thứ gì thuộc hệ kinh mạch.**
> Soạn 31/08/2026. Trạng thái: **toàn bộ đã đặt + đã push git**, chờ chủ game test.
> File chi tiết đợt trước: `BANGIAO_TOMTAT_DONGBO_2708.md`, `BANGIAO_BANDONGHANH_2708.md`.

---

## 0. TÓM TẮT 30 GIÂY

| Hạng mục | Trạng thái |
|---|---|
| Giao diện kinh mạch + xung huyệt + Khí Doanh | ✅ xong, khớp bản chuẩn VLTK |
| Mua gói Khí Doanh Đan Điền 1/7/30 ngày | ✅ chạy thật (đồng + Khí Doanh Đan) |
| Thuộc tính 384 huyệt + thưởng cấp 32 | ✅ đã cân bằng theo yêu cầu chủ |
| 5 lỗi engine chiến đấu (ngũ hành, phản đòn…) | ✅ đã vá, đã build, đã push |
| Nguồn ra nguyên liệu | ✅ 4 nguồn (Tống Kim, Viêm Đế, Boss HK, Phong Lăng Độ) |
| **Việc còn dở** | **xem mục 7 — quan trọng nhất là NGUỒN Chân Nguyên còn quá ít** |

**Nhị phân đang chạy** (kiểm 31/08): `CoreServer.dll` d3486293 · `CoreClient.dll` d53d10a6 · `Game.exe` c51c7bbb — **bản 30/08 của phiên Bạn Đồng Hành, đã bao gồm đủ mọi bản vá kinh mạch** (đã đối chứng bằng dấu vết `GetMeridian`, `[KM-BLOCK]`, `[KM-CRIT]`, nhãn cấp‑32 không còn chữ "(Dương)"). **Không cần build lại.**

---

## 1. BẢN ĐỒ TỆP

### Mã nguồn C++ (`D:\GAMEDEVNEW\Sources\`)

| Tệp | Nội dung kinh mạch |
|---|---|
| `Core\KMeridian.cpp` | Áp/gỡ thuộc tính huyệt; bảng `KM_L32_LOAI` / `KM_L32_TRI` (thưởng cấp 32) |
| `Core\Src\KPlayer.cpp` | `CapNhatKhiDoanh()` (buff Khí Doanh, task 4450); nhánh mua trong `c2sSetMeridian` |
| `Core\Src\KNpc.cpp` | Công thức ngũ hành, cổng xúc xắc hoá giải/trọng kích, phản đòn, nhịp mỗi giây |
| `Core\Src\KNpcAttribModify.cpp` | 10 bộ xử lý me2X/X2me + các bộ xử lý hoá giải/trọng kích |
| `Core\Src\ScriptFuns.cpp` | Ràng buộc Lua `GetMeridian` (LF endings — không phải CRLF!) |
| `S3Client\Ui\UiCase\UiMeridian.cpp` | Toàn bộ giao diện (3 cửa sổ) |
| `S3Client\Ui\Elem\WndLine.cpp` | Vá lỗi đơ game (biến chưa khởi tạo) |

### Kịch bản Lua (`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\`)

| Tệp | Vai trò |
|---|---|
| `player\setmeridian.lua` | **Lõi** — xung huyệt, bảo đáy, quy đổi, mua Khí Doanh |
| `player\meridian_data.lua` / `meridian_lang.lua` | Bảng dữ liệu + chuỗi hiển thị |
| `item\event\kinhmach\channguyendan.lua` | Dùng viên Chân Nguyên (đã chuẩn hoá, có trần ngày) |
| `item\test_kinhmach_admin.lua` | Bộ test admin (vào qua Lệnh bài Admin) |
| `timertask\task03.lua` | Móc thưởng **Tống Kim** |
| `missions\yandibaozang\npc_death.lua` | Móc thưởng **Viêm Đế ải 6** |
| `missions\boss\bigboss.lua` | Móc thưởng **Boss Hoàng Kim** |
| `event\jiefang_jieri\200904\shuizei\shuizei.lua` | Móc thưởng **Phong Lăng Độ** |

### Dữ liệu (`bin\server\settings\` — **phải đồng bộ sang `bin\client\`**)

- `meridian_level.txt` — 385 dòng, md5 `f6a21a18` (server = client ✅)
- `item\magicattrib_ge.txt` — 7.206 dòng, md5 `60a7c652` (server = client ✅)
- `item\magicscript.txt` — chứa hàng 4870/4871/4872/4873

### Công cụ (`D:\GAMEDEVNEW\ReverseTools\kinhmach_ui_2708\`)
Toàn bộ bản vá đánh số `p1`…`p29` (chạy lại được nhiều lần, tự sao lưu). Thư mục này **nằm trong `.gitignore`** — chủ đã yêu cầu chỉ push mã nguồn.

---

## 2. MÃ SỐ CẦN NHỚ

| Thứ | JX1 | Bản Linux | Ghi chú |
|---|---|---|---|
| Hộ Mạch Đơn | `6,1,4844` | 3203 | |
| Đại Hộ Mạch Đơn | `6,1,4870` | 4418 | **1 viên = 1.000 Hộ Mạch Đơn** |
| Định Mạch Đan / (Lv1) | `4871` / `4872` | — | mạch mới 9‑12 |
| Khí Doanh Đan | `6,1,4873` | (không có) | ta tự thêm |
| Huyết Long Đằng / Đơn | `4848` / `4849` | 30289 / — | **chưa có script** |
| Chân Nguyên Đan / Đơn trung / đại | `4752` / `4846` / `4847` | 4134 / 30228 / 30229 | +10 / +5 / +10 điểm |
| Truy Công Lệnh (Phong Lăng Độ) | `6,1,2024` | | |
| Điểm Chân Nguyên (mạch 1‑8) | **task 362** | task 4000 | `TASK_CHANGNGUYENDAN` |
| Điểm Huyền Nguyên (mạch 9‑12) | **task 4318** | | |
| Hạn buff Khí Doanh | **task 4450** | | giây unix |
| Ngày / điểm đã hấp thu viên | **task 4451 / 4452** | | do ta đặt |

⚠️ **Mã Linux ≠ mã JX1** — Linux 4804 là "Chân nguyên đơn" nhưng JX1 4804 là "Tinh Thiết Tỏa". Luôn tra `magicscript.txt` trước khi dùng.

---

## 3. NHỮNG GÌ ĐÃ LÀM (theo thứ tự)

### 3.1 Giao diện (đợt 27/08 → 28/08)
Ba cửa sổ khớp bản chuẩn VLTK: bảng kinh mạch 12 nút (4 mạch mới khác màu), cửa sổ xung huyệt mở **cạnh** bảng chính, trang Khí Doanh. Đã sửa hàng loạt lỗi hiển thị chủ chỉ ra: chữ đè nút, tiêu đề tràn, dấu tích ngược màu, nút bị bóp nhỏ, đường nối tên huyệt (khôi phục **không** gây đơ — gốc đơ là `KWndLine` có biến toạ độ chưa khởi tạo, vòng vẽ chạy trên rác).

### 3.2 Mua gói Khí Doanh Đan Điền
Ba gói **1 / 7 / 30 ngày** (600 / 4.000 / 16.000 đồng + 1/6/24 Khí Doanh Đan). Không thêm giao thức mới: gửi qua trường `Type` của `c2s_setmeridian` bằng mã ngưỡng 100/101/102. Hạn lưu ở task 4450, cộng dồn. Ba dòng trạng thái trên trang hiện đúng buff thật + đếm ngược "X ngày Y giờ".

### 3.3 Cân bằng thuộc tính (theo yêu cầu chủ)
- Đổi 3 loại "(dương)" sang bản thường: `lifemax_yan_p→lifemax_p` (339), `allres_yan_p→allres_p` (144), `fasthitrecover_yan_v→fasthitrecover_v` (28). **Hai loại KHÔNG đổi được** (engine không có bản không‑dương, đổi là mất tác dụng im lặng): `anti_allres_yan_p`, `anti_sorbdamage_yan_p`.
- Hạ nhóm hoá giải/trọng kích còn ~30%: hoá giải 77→30, phá hoá giải 77→30, trọng kích 77→30, kháng trọng kích 79→30, hiệu quả trọng kích 70→21, kháng hiệu quả 70→21.
- Thưởng cấp 32: **Sinh Lực Tối Đa 15.000 → 5.000**; đổi cả 6 thuộc tính "(dương)" sang bản thường **và bỏ chữ "(Dương)" trong nhãn hiển thị** (7 chỗ).

🔑 **Cách sửa an toàn — bắt buộc làm theo:** 384 huyệt trỏ vào vùng mã **2897–6230** của `magicattrib_ge.txt`, vùng này **dùng chung với trang bị Hoàng Kim**. Không bao giờ sửa đè dòng cũ. Cách đúng: **thêm dòng mới ở cuối bảng** (sao chép dòng gốc, đổi cột 5 = loại, cột 6 = trị) rồi đổi **cột 7** của `meridian_level.txt` trỏ sang dòng mới.

### 3.4 Năm lỗi engine đã vá (đều đã push)

| Commit | Lỗi | Gốc |
|---|---|---|
| `35aef67e` | Buff Khí Doanh hết hạn giữa phiên không tự gỡ | hàm cập nhật chỉ chạy lúc đăng nhập + lúc mua → thêm vào nhịp mỗi giây có sẵn |
| `6eae8564` | Thưởng cấp 32 sai giá trị + còn chữ "(Dương)" | (mục 3.3) |
| `3b485087` | **Full mạch đánh Thiếu Lâm là tự chết** | công thức ngũ hành **tự chế**: `%chiêu + (Cường−Nhược)/10` cộng thẳng vào sát thương, **không kẹp trần** → full mạch +697% mọi đòn khắc hệ, phản đòn dội ngược. Thay bằng bản chuẩn Linux `0x0807BAA0`: chỉ **dịch chuyển kháng** của nạn nhân |
| `bf4c0149` | Phản đòn mạnh bất thường (3 chỗ lệch) | ① điều kiện nhân `PKRate` (PvP ×20%) có 2 vế vô nghĩa khiến **riêng đòn phản né mất phép nhân** = mạnh gấp 5 tương đối; ② nhánh xa **lỗi dấu** `-= nMin * -kháng` = cộng thêm; ③ nhánh cận dùng biến chưa kẹp 0..95 |
| `6fc1fb22` | Phản đòn tính cả phần sát thương thừa | **lệch chuẩn CÓ CHỦ ĐÍCH theo yêu cầu chủ**: gốc phản = `min(sát thương, máu đối thủ còn lại)`. Cả Linux lẫn JX1 gốc đều tính cả overkill — xưa không lộ vì hiếm khi vượt máu |

Ngoài ra (vá trước đó, cùng đợt): cổng xúc xắc hoá giải chỉ tung **khi quan hệ là ĐỊCH** (`relation_enemy`) — trước đó tung vô điều kiện nên vòng sáng/buff của đồng đội bị "hoá giải" mất; sai lệch chỉ số +1 khi đặt cấp mạch; `MAX_TASK` 4200→4600; thêm ràng buộc Lua `GetMeridian`; 10 bộ xử lý thuộc tính me2X/X2me còn thiếu.

### 3.5 Nguồn ra nguyên liệu (28/08)

| Nguồn | Điều kiện | Nhận | Tệp móc |
|---|---|---|---|
| **Tống Kim** | ≥ 3.000 điểm tích lũy/trận | 2 CNĐ trung (+10) | `timertask\task03.lua` |
| **Viêm Đế** | qua **ải thứ 6** | 6 CNĐ đại (+60), cả đội | `yandibaozang\npc_death.lua` |
| **Boss Hoàng Kim** | hạ Độc Cô Thiên Phong (19h45) | 2 CNĐ trung (+10) | `missions\boss\bigboss.lua` |
| **Phong Lăng Độ** | nộp 1 Truy Công Lệnh | 5 CNĐ đại (+50) | `shuizei\shuizei.lua` |
| Rương Tín Sứ (có sẵn từ trước) | mở rương | CNĐ trung ×7@10%, đại ×7@5%, HMD ×50@10% | `xinshirenwu\xinshibaoxiang.lua` |
| Tống Kim (có sẵn từ trước) | thắng/thua/hoà | 500 / 300 / 100 Hộ Mạch Đơn | `lib_tktc.lua` |

Viên Chân Nguyên đã chuẩn hoá: trung **+5**, đại **+10**, Chân Nguyên Đan **+10**, **trần hấp thu 3.000 điểm/ngày** (số chuẩn Linux). Bản cũ cộng +10 đồng loạt, không trần, còn có dòng `dofile` tự gọi lại chính nó.

❌ **Đã BỎ** boss "Kẻ Trộm Chân Nguyên" (chủ thấy không hợp lý) — `p29_bo_ketrom.py` gỡ sạch; 2 tệp script đổi tên `.bo_p29`, muốn khôi phục thì đổi tên lại.

---

## 4. KINH TẾ — CON SỐ THẬT

**Chi phí full 1 mạch cũ (1‑8), 32 huyệt:**
`483.200 điểm Chân Nguyên` + `9.100 Hộ Mạch Đơn` (cấp 1‑16) + `465 Đại Hộ Mạch Đơn` (cấp 17‑32)

**Mạch mới (9‑12):** `373.860 Huyền Nguyên` + `13.586 Định Mạch Đan` — **chưa có nguồn ra nào**.

**Thu nhập/ngày hiện tại** (người chơi chăm): ~**520–700 điểm Chân Nguyên** + ~1.500 Hộ Mạch Đơn.

→ **Chân Nguyên là nút cổ chai: ~2 năm cho 1 mạch.** Trần 3.000/ngày chưa hề chạm tới — thiếu là thiếu **nguồn**, không phải thiếu trần.

⚠️ Sau khi bỏ boss Kẻ Trộm, **Đại Hộ Mạch Đơn mất nguồn định kỳ duy nhất** — cấp 17‑32 chỉ còn đường quy đổi 1.000 Hộ Mạch Đơn = 1 viên.

---

## 5. BẢY CÁI BẪY ĐÃ TRẢ GIÁ (đừng vấp lại)

1. **RULE 0 — không bao giờ gõ tay byte tiếng Việt làm mỏ neo tìm kiếm.** Đã vấp 2 lần trong đợt này. Dùng `vn_edit.py`, hoặc neo **thuần ASCII**, hoặc sinh byte bằng `unicode_to_tcvn3_bytes`.
2. **`UiMeridian.cpp` có BOM UTF‑8** → tiếng Việt mới trong tệp này phải là **thoát bát phân**; các tệp `.lua`/`.cpp` khác dùng **TCVN3 thô**. Chú thích trong `.cpp` chỉ ASCII (ký tự Trung từng làm sập assert).
3. **Vùng mã 2897–6230 dùng chung với trang bị Hoàng Kim** — chỉ được thêm dòng mới + trỏ lại (mục 3.3).
4. **`Say`/`SayEx` có trần 512 byte dùng chung** cho tiêu đề + mọi nhãn; đầy là **cắt im lặng** các mục cuối. Ký tự `|` trong nhãn làm vỡ danh sách. Menu cấp vật phẩm hiện ~431/512.
5. **Hai phiên build cùng lúc trên cùng cây = trộn `.obj` hai phiên bản → sập kiểu ODR.** Đổi header dùng chung ⇒ phải `-t:Rebuild` cả ba nhị phân.
6. **Bảng bên client có thể là bản CŨ.** Đã bắt được `magicattrib_ge.txt` client 5.583 dòng trong khi server 7.082 (và dùng xuống dòng LF). **Luôn so md5 hai bên.**
7. **Nghiệm thu nhị phân bằng chuỗi THẬT (log/tên hàm), không bằng chuỗi chú thích** — chú thích không vào nhị phân. Đã vấp một lần.

---

## 6. CÔNG CỤ

- **Kiểm cú pháp Lua 4 offline**: `ReverseTools\lua_syncheck\` — build bằng `build_syncheck.bat`, nhưng **phải liệt kê tệp `.c` tường minh** (loại `lua.c`, `luac.c`, `ltests.c`, `dump.c` vì dính `main`/trùng ký hiệu) và thêm `baselib\*.c`. Bắt buộc chạy trước khi đặt bất kỳ tệp `.lua` nào.
- **Dịch ngược Linux**: `D:\ServerLinux\server1\jx_linux_y` (ELF 32‑bit) + `kinhmach_ui_2708\dis_lx.py`.
  - Bảng tên thuộc tính bắt đầu `0x830E6F4`, **bước 4**; bảng bộ xử lý **bước 8** tại `[edx+disp]`.
  - Công thức đổi: `disp = 0x7CC + (khe_tên − 0x830EA24) × 2` (đối chứng 2 mốc: `add_damage_p` = `0x08096130`@`0x7CC`, `anti_hitrecover` = `0x08096CD0`@`0x6DC`).
  - Tìm khe tên: tìm dword chứa địa chỉ chuỗi, có tiền tố byte `C7 05`.
- **Bảng chuẩn gốc**: `D:\ServerLinux\Patch\settings\meridian\meridian_level.txt` (8 mạch × 16) + `D:\ServerLinux\server1\settings\item\004\magicattrib_ge.txt` (**chỉ bản 004** có vùng mã kinh mạch; 000‑003 chỉ 299 dòng). Đối chiếu 1.318 mã: **1.299 khớp tuyệt đối** ⇒ bảng JX1 chính là bảng chuẩn.

---

## 7. VIỆC CÒN DỞ (theo thứ tự ưu tiên)

### 🔴 Cần chủ quyết
1. **Nguồn Chân Nguyên còn quá ít** (~2 năm/mạch). Ba hướng: port **Hồn Châu** (viên +5.000/7.500/10.000 — hàng nạp, đúng chất trả phí); hoặc port **Kiếm Gia Mê Cung** (phụ bản mê cung tổ đội ≥6 người cấp ≥120, ~40 kịch bản phòng — **khối lượng lớn**, cho 10 viên đại/ngày); hoặc đơn giản nhất là **nhân hệ số giá trị viên / nới trần ngày** theo con số chủ chốt.
2. **Đại Hộ Mạch Đơn không còn nguồn định kỳ** sau khi bỏ boss. Đề xuất: gắn 1‑2 viên vào thưởng thắng Tống Kim hoặc nộp Truy Công Lệnh; hoặc mở tiệm đổi (X huy chương Tống Kim = 1 viên).
3. **Mạch 9‑12 (Huyền Nguyên + Định Mạch Đan) chưa có nguồn nào.**
4. **`anti_allres_yan_p` và `anti_sorbdamage_yan_p` chưa bỏ được "(dương)"** — engine không có bản thường. Muốn có thì phải viết thêm bộ xử lý.

### 🟡 Kỹ thuật, chưa gấp
5. **`UIMessageBox` không vẽ ra màn hình** — lỗi CHUNG của client (bang hội, tổ đội, siêu thị cùng dùng). Đã loại trừ: hàm khởi tạo có chạy, tệp giao diện có trên đĩa, ảnh nền có trong pak. **Chưa ra gốc.**
6. **Công thức hoá giải thiếu số hạng `addblockrate` động** (chuẩn có, kẹp trần 25). Cố ý chưa vá vì vá vào là **tăng** hoá giải, ngược ý cân bằng chủ vừa yêu cầu.
7. **Hệ Huyết Long Đằng/Đơn** (4848/4849) chưa có script — chủ đã hoãn.
8. "Hộp quà Kinh Mạch mới" (nguồn Huyền Nguyên bản chuẩn) chưa port.

---

## 8. CẦN LÀM NGAY SAU KHI ĐỌC

1. **Restart máy chủ** — mọi thay đổi script/dữ liệu chưa được nạp.
2. Test theo thứ tự: đánh Thiếu Lâm có La Hán Trận (không được tự chết) → giết đối thủ ít máu (phản đòn phải nhỏ) → mua gói Khí Doanh → xem bảng thuộc tính cấp 32 (Sinh Lực 5.000, không còn chữ "Dương") → Tống Kim ≥3.000 điểm → nộp Truy Công Lệnh.
3. Bộ test nhanh: **Lệnh bài Admin → Bộ test Kinh Mạch** (đặt cấp mạch, cấp điểm, cấp 9 loại vật phẩm).

**Sao lưu**: mọi tệp bị sửa đều có bản cũ cùng thư mục, đuôi `.truoc_*` (theo tên đợt vá). Nhị phân cũ: `.truoc_kmp2X_2808`.
