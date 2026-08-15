# LỊCH TRÌNH TEST FULL — BANG HỘI + CÔNG THÀNH + LÃNH ĐỊA + TÁC PHƯỜNG + DANH HIỆU + THUẾ

Ngày lập: 14/08/2026 · Bộ test: `bin\server\script\test\bangthanh_f.lua`
Vào bằng: **LỆNH BÀI ADMIN → "Test bang hoi va cong thanh"**

Bộ test này đã qua một vòng phản biện 24 tác nhân đối chiếu mã nguồn C++ và script gốc: 16 lỗi được xác nhận và đã vá trước khi giao, 3 cáo buộc bị bác bỏ. Những chỗ bộ test **cố ý không nghiệm thu được** đều ghi thẳng trên nhãn nút.

---

## 0. CHUẨN BỊ (làm 1 lần, trước mọi buổi)

| # | Việc | Đúng là |
|---|---|---|
| 0.1 | Sao lưu `bin\server\settings\jx2citywar.txt`, `jx2league.txt`, `jx2ladder.txt` + backup DB bang | có file backup |
| 0.2 | Xoá `bin\server\script\ScriptError.log` và `bin\server\scriptjx2\tong_vn\ScriptError.log` | 2 file biến mất |
| 0.3 | Mở `bangthanh_f.lua`, sửa `g_TX_TESTTONG = "TESTGAME"` thành **đúng tên bang test của anh** | — |
| 0.4 | **Restart GameServer + Relay + client cùng lúc** | client vào được |
| 0.5 | Dùng lệnh bài → "Test bang hoi va cong thanh" | ra menu 6 dòng |

> **Chốt an toàn**: mọi nút *ghi dữ liệu* đều kiểm tên bang trước. Bang của anh khác `g_TX_TESTTONG` thì máy **chặn** và báo. Riêng nhóm nút chạm vào 7 thành / toàn server còn kiểm thêm số người online (trần 5).

> **Ba bẫy khi dùng menu:**
> 1. Đang mở menu mà lỡ **kích vào NPC khác** thì cú bấm kế nhảy sang script NPC đó (menu như đứng hình). Cứ dùng lại lệnh bài.
> 2. Hộp thoại chỉ vẽ **6 nút** một lúc; nút thứ 7 trở đi phải kéo thanh trượt. Bộ test đã chia trang để không màn nào quá 6 nút.
> 3. Phần chữ mô tả chỉ vẽ **6 dòng và không cuộn được** — dòng thứ 7 mất luôn. Nếu anh thấy màn nào cụt chữ, chụp lại gửi tôi.

---

## BUỔI 1 — Hạ tầng + chụp mốc (~15 phút, KHÔNG sửa dữ liệu)

| # | Thao tác | Kết quả đúng | Sai thì gửi tôi |
|---|---|---|---|
| 1.1 | `Trang 2` → `7 Tien ich` → `KIEM HA TANG - chi doc` | `ham tac phuong: co day du` · `map lanh dia nap: 11 tren 11` · `league 508 thanh 1: >0` · `lich cong thanh TB_CTC6: CO` | dòng nào khác ⇒ chụp cả 6 dòng |
| 1.2 | `1 Xem nhanh` — **chụp làm mốc** | 6 dòng có số | — |
| 1.3 | `3 Bang hoi` → `Ho so + kinh te (1)` rồi `Kinh te (2)` — chụp | 12 con số | Ngân quỹ khác cửa sổ bang ⇒ gửi cả 2 |
| 1.4 | `Trang 2` → `5 Tac phuong` → `Xem khu 1-4` rồi `5-7` — chụp | 7 dòng | — |
| 1.5 | `2 Cong thanh` → `Xem 7 thanh (1-4)` rồi `5-7` — chụp | 7 dòng + thuế | — |
| 1.6 | Soi lại 2 file `ScriptError.log` | **vẫn không tồn tại** | có file ⇒ gửi nguyên nội dung + nói vừa bấm nút nào |

---

## BUỔI 2 — Kinh tế + quyền + nhật ký (~30 phút, 1 nhân vật)

Đường đi: `3 Bang hoi`

| # | Thao tác | Kết quả đúng | Sai thì gửi tôi |
|---|---|---|---|
| 2.1 | `Nap quy - cong hien - ngay` → `+1000 van ngan quy` → `Xem lai kinh te (1)` | Ngân quỹ **+10.000.000** | 10 giây không đổi ⇒ Relay không hồi đáp, gửi console Relay |
| 2.2 | `+100000 quy kien thiet` | KT +100.000 **hiện ngay** | không đổi ⇒ báo |
| 2.3 | `Dat quy kien thiet = 0` | KT = **0** | ra ~4.294.967.xxx ⇒ **báo gấp** (bản vá tràn số chưa vào binary) |
| 2.4 | `NGAY +8` → `Kinh te (2)` | `Ngay` tăng đúng 8 | không tăng ⇒ báo, vì buổi 3 sẽ kẹt |
| 2.5 | `Thanh vien + quyen` → `6 thanh vien dau` | 4 người đầu + tổng số người | — |
| 2.6 | `Quyen cua TRUONG LAO dau` | 4 dòng 0/1 | ra `CHUA CO TRUONG LAO` ⇒ **phong 1 trưởng lão bằng cửa sổ bang rồi làm lại** (bang chủ luôn ra 1, không dùng kiểm được) |
| 2.7 | `Cap quyen 9001 tac phuong` → bấm lại `Quyen cua TRUONG LAO dau` | 9001 đổi 0 → 1 | sau 10 giây vẫn 0 ⇒ báo |
| 2.8 | `+500 cong hien cho TA` → `6 thanh vien dau` | cống hiến +500 | — |
| 2.9 | `Nop 200 cong hien vao quy` → `Xem lai kinh te (1)` | Cống hiến dự trữ +200, cống hiến cá nhân −200 | báo "lệnh bị từ chối" ⇒ gửi mã |
| 2.10 | `Muc tieu tuan + bao tri` → `Ghi 20 dong nhat ky` → **mở cửa sổ Bang, trang Nhật ký** | 16 dòng cuối, chữ không cụt, không văng | cụt / văng ⇒ báo thấy mấy dòng |
| 2.11 | `Bao tri NGAY - chi bo dem` → đợi vài giây → `Kinh te (2)` | `Ngay` +1 | không tăng ⇒ báo |

> Hai nút bảo trì cá nhân **chỉ đẩy bộ đếm ở relay**, cố ý không chạy phần kinh tế (trừ phí duy trì, chốt mục tiêu tuần). Phần kinh tế chỉ chạy qua nút `TOAN SERVER - kinh te ngay` ở buổi 7.

---

## BUỔI 3 — Lãnh địa + cấp kiến thiết (~30 phút, 1 nhân vật)

Đường đi: `4 Lanh dia + kien thiet`

| # | Thao tác | Kết quả đúng | Sai thì gửi tôi |
|---|---|---|---|
| 3.1 | `Xem lanh dia` | bang mới: `f45=0 f46=0` | ra `MAP CHUA NAP` ⇒ báo số map |
| 3.2 | `Chon khu vuc CHUNG` → `586` → `Xem lanh dia` | `f45=586`, **f46 vẫn 0** (đúng) | — |
| 3.3 | `Vao lanh dia` | vào map 586: **7 toà xưởng cấp 1 + 3 Tế Đàn + Xa phu + Rương + 2 Tổng quản khu chiến đấu**. **KHÔNG** có 7 NPC "Tổng quản …phường" — đúng hiện trạng | thiếu toà xưởng ⇒ báo |
| 3.4 | `Cap kien thiet + tuyet ky` → `Nang cap kien thiet` → đợi vài giây → `Xem lanh dia` | cấp 0 → 1 | báo thiếu quỹ ⇒ quay lại 2.2 nạp thêm |
| 3.5 | Lặp: `NGAY +8` → `Nang cap` cho tới cấp 3 | lên được cấp 3 | — |
| 3.6 | **Ghi lại xem có lên được cấp 3 không** | Bản port **đã bỏ 3 điều kiện gốc** (chờ 7 ngày, cấp 2→3 đòi lãnh địa riêng, cấp 4→5 đòi chiếm thành) nên **sẽ lên được** | đây là **chỗ lệch so với bản gốc đã biết**, cần anh xác nhận có muốn khôi phục 3 điều kiện không |
| 3.7 | `Tuyet ky bang` — **chụp gửi tôi** | 4 khoá 1011 / 1014 / 1021 / 1036 | — |
| 3.8 | `Tao lanh dia RIENG theo mau` → `Mau 587` → đợi → `Xem lanh dia` | `f45 = f46 = 587` | ra 0 hoặc 1 ⇒ báo |
| 3.9 | `Vao lanh dia` (587) | vào map, chỉ NPC nền, **không có toà xưởng** — đúng hiện trạng | — |
| 3.10 | `Chon khu vuc CHUNG` → `586` | f45 = 586 | — |

> Các lệnh nâng/hạ cấp, tạo/xoá lãnh địa **không hiện kết quả ngay** — đó là thiết kế, không phải lỗi. Bấm lại `Xem lanh dia` sau vài giây mới thấy giá trị mới.

---

## BUỔI 4 — Tác phường (~45 phút) — **làm trong khung 12h30 → 22h**

**Phần A — nghiệm thu LUẬT: bắt buộc dùng CỬA SỔ BANG, tab Tác phường.** Nút trong lệnh bài đi tắt qua relay nên **không nghiệm thu luật được**.

| # | Thao tác (cửa sổ bang) | Kết quả đúng | Sai thì gửi tôi |
|---|---|---|---|
| 4.1 | Lập khu **Lễ vật** | trừ đúng **400 vạn** quỹ KT; cấp KT 0 thì báo "đã đạt giới hạn" | lập được khi cấp KT 0 / không trừ tiền ⇒ **báo: chốt trần số khu hỏng** |
| 4.2 | Mở khu → Nâng cấp khu | trừ đúng tiền; vượt trần cấp KT thì báo "cần nâng đẳng cấp kiến thiết trước" | vượt trần được ⇒ **báo: chốt trần cấp hỏng** |
| 4.3 | Đặt cấp sử dụng = 99 | bị kẹp về cấp thật | không kẹp ⇒ **báo** |

**Phần B — dựng cảnh + sản xuất (lệnh bài: `Trang 2` → `5 Tac phuong`)**

| # | Thao tác | Kết quả đúng | Sai thì gửi tôi |
|---|---|---|---|
| 4.4 | `Xem khu 1-4` | số khớp cửa sổ bang | lệch ⇒ gửi cả 2 |
| 4.5 | `DUNG CANH lap + nang 7 khu` → Đồng ý → `Xem khu 1-4` và `5-7` | 7 khu `lv10 dung10 MO SL100000` | — |
| 4.6 | **Dọn trống ít nhất 4 ô túi** trước khi sang bước sau | — | — |
| 4.7 | `SAN XUAT thu` → `Le vat 6 - hong bao` | túi có **"Bang Hội thần mật hồng bao"**, sản lượng giảm | không ra đồ ⇒ gửi mã trả; sai tên đồ ⇒ báo |
| 4.8 | `SAN XUAT thu` → `Binh giap 1 - lenh bai Boss` | túi có "Boss Triệu Hoán Phù" | — |
| 4.9 | `SAN XUAT thu` → `Hoat dong 7 - lenh bai Boss` | túi có lệnh bài gọi Boss | ra 0 món mà vẫn báo mã 1 ⇒ báo (đúng lỗi vừa vá) |
| 4.10 | `Thi luyen 4 (do lo)` và `Thien Y 5 (do lo)` | **đều KHÔNG ra đồ — đúng hiện trạng đã biết**; ghi lại khu nào **có trừ** cống hiến/sản lượng | khu nào **ra đồ** ⇒ báo (tốt hơn dự kiến) |
| 4.11 | `Mo - dong - do bo - san luong` → `Bao tri khu 6` → `Xem khu 5-7` | sản lượng tăng | — |
| 4.12 | Soi `scriptjx2\tong_vn\ScriptError.log` | dự kiến **có** lỗi của các khu hỏng | gửi nguyên file |

> Nút `SAN XUAT thu` **cố ý đi tắt**: không kiểm khung giờ 12h30–22h, không kiểm cờ chống spam, **không kiểm ô túi trống**. Vì vậy mục 4.6 (dọn túi) là bắt buộc, nếu không vật phẩm rơi mất. Muốn nghiệm thu khung giờ thật thì phải nói chuyện với NPC Tổng quản trong lãnh địa.
>
> Mã trả **1 không bảo đảm có đồ** — luôn mở túi kiểm tận mắt.

---

## BUỔI 5 — Công thành (~45 phút) — **cần 2 bang / 2 nhân vật**

| # | Thao tác | Kết quả đúng | Sai thì gửi tôi |
|---|---|---|---|
| 5.1 | `Trang 2` → `7 Tien ich` → `Sang trang 2` → `Phat Khieu chien lenh` → Đồng ý | túi có **1 cái** | 0 cái ⇒ báo ngay |
| 5.2 | `Di chuyen nhanh` → `53 Ba Lang` → kích **NPC Sứ Giả Công Thành** → nộp lệnh | vào danh sách đấu giá | không thấy NPC ⇒ báo (boot chưa sinh NPC) |
| 5.3 | `2 Cong thanh` → `Bang bao danh thanh nay` | thấy tên bang vừa nộp | — |
| 5.4 | `Bo nhiem - khieu chien` → `Bang TA lam CHU thanh nay` → Đồng ý → `Xem 7 thanh (1-4)` | chủ thành = bang anh | — |
| 5.5 | `Trang 2` → `6 Danh hieu` → `Danh hieu cua TA` | có danh hiệu **15x Thái Thú** | không có ⇒ báo id thành + tên bang |
| 5.6 | **Nhân vật 2 / bang 2**: `Bang TA KHIEU CHIEN thanh nay` → Đồng ý | `Xem 7 thanh` hiện `[C]` | — |
| 5.7 | `Bo nhiem - khieu chien` → `Ket tran` → `CONG thang` → Đồng ý | đổi chủ sang bang 2, bang 2 nhận Title | không đổi ⇒ báo |
| 5.8 | Làm lại 5.6 rồi `Ket tran` → `THU thang` | giữ nguyên chủ, hết `[C]` | — |
| 5.9 | `Ep pha` → `20h khai chien` → Đồng ý | `[C]` bật, trong 5 phút mission chiến trường mở | — |
| 5.10 | `Ep pha` → `18h mo bao danh` → Đồng ý | cờ `[B]` bật | — |
| 5.11 | `Ep pha` → `19h chot khieu chien` → Đồng ý | thành vô chủ được gán chủ; **cờ `[B]` của 6 thành kia bị hạ** (đúng như relay gốc chạy hằng ngày) | — |
| 5.12 | `Ep pha` → `0h don ngay` → Đồng ý | dọn sạch, console không lỗi | — |

> **Đồng hồ 5 pha KHÔNG tự chạy** — chỉ chạy khi ép tay ở menu này. Nếu anh đứng đợi 18h/19h/20h mà thành không tự đổi trạng thái thì đó là hiện trạng, không phải lỗi mới.
>
> Ép pha đúng vào khung giờ thật có thể khiến đồng hồ thật bắn lại pha đó. Nên test ép pha **ngoài** các khung 18h–20h và 0h.

---

## BUỔI 6 — Danh hiệu + thuế (~20 phút)

Đường đi: `Trang 2` → `6 Danh hieu + thue`

| # | Thao tác | Kết quả đúng | Sai thì gửi tôi |
|---|---|---|---|
| 6.1 | `Danh hieu cua TA` | liệt kê danh hiệu + id đang bật | — |
| 6.2 | `Cap - bat - go danh hieu` → `Cap quan ham 89 Tong binh` | vào tab, **không có buff** — đúng dữ liệu hiện tại | — |
| 6.3 | `Tat danh hieu dang bat` → `Danh hieu cua TA` | `Dang bat: 0` | — |
| 6.4 | `Man goc linh danh hieu` | mở menu gốc phân trang 5 mục | menu không ra ⇒ báo |
| 6.5 | `Xem thue 7 thanh` rồi `Thue thanh 5-7` | 7 con số | — |
| 6.6 | Đứng trong thành mình làm chủ, **lúc 22h–23h**, `Dat thue thanh dang dung` → `Dat 15 phan tram` | `Ma tra 0 - OK` | mã 1 ⇒ chụp cả màn (nó in sẵn bang chủ / Thái Thú / tên anh / giờ máy chủ) |
| 6.7 | Bấm lại lần 2 cùng ngày | `Ma tra 3 - hom nay da dat thue roi` | — |
| 6.8 | Thử **ngoài 22h–23h** | `Ma tra 2 - ngoai khung gio` | — |
| 6.9 | Nhân vật thường (không phải Thái Thú) đặt thuế | `Ma tra 1` | — |
| 6.10 | Bán 1 món ở sạp trong thành có thuế, nhân vật 2 mua | tiền về bang chủ thành theo % | không trừ ⇒ báo (cần 2 nick) |

> Danh hiệu nằm trong RAM, **mất khi restart** — đúng thiết kế hiện tại. Bật danh hiệu **có gán buff chiến đấu thật** lên nhân vật, nên đừng bật lung tung trên nhân vật đang PK.

---

## BUỔI 7 — Toàn server + dọn dẹp (~20 phút, **làm khi server vắng người**)

| # | Thao tác | Kết quả đúng |
|---|---|---|
| 7.1 | `3 Bang hoi` → `Muc tieu tuan + bao tri` → `TOAN SERVER - kinh te ngay` → Đồng ý | ra số bang đã chạy > 0. Ra **0** thì màn hình sẽ nói rõ 2 khả năng — gửi tôi màn đó |
| 7.2 | Sau 7.1: `Ho so + kinh te (1)` | quỹ chiến bị bị trừ phí duy trì, **không được ra ~4,29 tỷ** |
| 7.3 | `Trang 2` → `7 Tien ich` → `Sang trang 2` → `RESET 7 thanh ve vo chu` → Đồng ý | 7 thành vô chủ |
| 7.4 | `2 Cong thanh` → `Xem 7 thanh` | tất cả `(vo chu)` |
| 7.5 | `4 Lanh dia` → `Cap kien thiet + tuyet ky` → `Xoa lanh dia` → Đồng ý → `Xem lanh dia` | `f45=0 f46=0` |
| 7.6 | Chơi bình thường 10 phút: đánh quái, mua bán, chat, vào Tống Kim | không văng, không lỗi console |
| 7.7 | Soi 2 file `ScriptError.log` lần cuối | gửi tôi nếu có |

> Nút reset **không thu hồi được danh hiệu Thái Thú của người khác** và **không xoá field 48** của các bang chủ cũ (hàm danh hiệu chỉ tác động lên người đang bấm). Muốn sạch hoàn toàn thì restart server.

---

## NHỮNG VIỆC **KHÔNG CÓ NÚT** — phải test tay

| Việc | Vì sao |
|---|---|
| Lập / mở / nâng / đặt cấp dùng tác phường **đúng luật** | Đường Lua ghi thẳng vào ô dữ liệu, bỏ qua kiểm tra ở relay ⇒ kết quả **giả**. Nghiệm thu chỉ bằng **cửa sổ Bang, tab Tác phường** (mục 4.1–4.3) |
| Đổi chức vụ · toàn bộ Liên minh · duyệt/từ chối đơn · lưu văn án · phát ngân lượng theo chức · "Vào bổn bang" | không có hàm gọi được từ Lua — chỉ đi bằng cửa sổ client |
| **Đọc** nhật ký bang / tên liên minh / văn án chiêu mộ bằng lệnh bài | hàm đọc là rỗng; phải mở cửa sổ Bang |
| Nhận thưởng mục tiêu tuần | NPC Tế Đàn đang bị tắt 5 mục |
| Trần 20 lượt đuổi người/ngày + phạt | nút đuổi **cố ý không đưa vào** bộ test (đi tắt, bỏ qua luật) |
| Lôi đài công thành | hàm đăng ký lôi đài **không tồn tại** ở cả C++ lẫn Lua — người chơi nộp tiền xong hàm chết giữa chừng. **Đây là việc còn nợ, chưa làm.** |

---

## HAI BẢN VÁ C++ CẦN BUILD LẠI MỚI CÓ HIỆU LỰC

Vòng phản biện tìm ra một lỗi **ngoài bộ test**, ảnh hưởng vận hành thật:

`scriptjx2\tong_vn\tong.lua` gọi trừ quỹ bằng **số âm** ở 4 chỗ (dòng 152, 274, 380, 517), nhưng nhánh cộng-không-dấu ở relay không kẹp, nên **bang nào có quỹ nhỏ hơn phí duy trì sẽ bị bơm vọt thành ~4,29 tỷ** ngay lần bảo trì kế tiếp.

Đã vá 2 chỗ, cả hai **compile PASS 0 lỗi**:
- `Sources\MultiServer\S3Relay\KTongJX2Relay.cpp:97` — kẹp về 0 khi trừ quá số dư
- `Sources\Core\Src\KTongJX2.cpp:406` — bản sao lạc quan kẹp y hệt để client không lệch số với relay

**Bản vá chưa vào binary đang chạy.** Anh cần build lại `Core` (x64) và `S3Relay` (Win32) từ máy có đủ thư viện, rồi thay `CoreServer.dll` + `S3Relay.exe`. Trước khi thay xong thì mục 2.3 và 7.2 trong lịch trình vẫn sẽ ra số 4,29 tỷ.

---

## CÁCH BÁO LỖI CHO TÔI (giúp sửa nhanh gấp đôi)

Mỗi lần báo, gửi đủ 4 thứ:
1. **Số mục** trong bảng trên (ví dụ "2.3").
2. **Ảnh chụp** màn thoại — chụp cả phần chữ mô tả, không chỉ nút.
3. **Console GameServer** đoạn ngay sau khi bấm.
4. Nội dung 2 file `ScriptError.log` nếu chúng xuất hiện.
