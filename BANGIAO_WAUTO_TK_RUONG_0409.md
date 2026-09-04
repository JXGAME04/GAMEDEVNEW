# BÀN GIAO — WAuto Tống Kim: HẾT TRẬN VỀ THÀNH XONG THÌ TỚI RƯƠNG (04/09/2026)

> Chủ game giao: *"thêm tính năng tự chọn hết trận về ở tab tống kim về thành nào thì lưu rương
> thành đó có thêm tùy chọn lưu rương đông - tây - nam - băc - trung tâm tùy thành có sẵn ở
> thần hành phù đối chiếu làm theo"*.

Đọc mục **1** (swap) là đủ để dùng. Mục 3 là bảng dữ liệu và cách kiểm chứng, mục 5 là bẫy.

---

## 1. Trạng thái — CHỜ SWAP (chưa test thật)

Hai tệp `.moi` ở `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client`. **Phải đổi cả hai cùng lúc**
vì `struct autoData` (IPC WAuto ↔ Game.exe ↔ CoreClient.dll) dài thêm 12 byte ở cuối.

| Tệp | md5 | cỡ (byte) | Nội dung |
|---|---|---|---|
| `CoreClient.dll.moi` | `c87a0d9c` | `2.544.640` | **BẢN GỘP**: nhánh `mail-0309` `5eae0886` (đấu giá + hộp thư của phiên khác, = bản đang chạy) **+** commit rương `e4e0fa89`. Build từ worktree `D:\GAMEDEVNEW_wt_tkruong` (nhánh `tkruong-0409`, cherry-pick sạch, không xung đột). |
| `WAuto.exe.moi` | `7b081174` | `462.848` | **gồm cả đợt 7 giao diện** (đã đè bản `e8aeda79` chưa kịp đổi) + 4 ô mới ở tab TK |

Bản đang chạy: `CoreClient.dll` **`bd259907`** (nhánh `mail-0309`, chủ đổi lúc 10:50), `WAuto.exe` `742b6a9c` (đợt 5 giao diện).

> ⚠️ **HAI PHIÊN CÙNG ĐẶT `.moi`.** Lúc 10:49 tôi đặt `CoreClient.dll.moi` `c9a3fb7a` (main + rương);
> phiên đấu giá/thư đè lên bằng bản của họ lúc 10:49:10 và `ChoiGame.bat` đổi bản đó vào lúc 10:50.
> Vì vậy bản `.moi` hiện tại là **bản gộp** để không mất việc của bên nào. Nếu phiên kia lại đặt
> `CoreClient.dll.moi` mới, việc rương sẽ **biến mất khỏi bản đó** — khi ấy build lại từ nhánh
> `tkruong-0409` sau khi cherry-pick/merge nhánh của họ, hoặc merge `tkruong-0409` vào `main`.

Cây nguồn `D:\GAMEDEVNEW` ở nhánh `main` `e4e0fa89`, sạch. Nhánh gộp: `tkruong-0409` `8673afde`.

### Cách đổi

1. Thoát hẳn `Game.exe` **và** `WAuto.exe`.
2. Chạy `bin\client\ChoiGame.bat` — nó tự đổi `CoreClient.dll.moi` → `CoreClient.dll`.
3. `ChoiGame.bat` **KHÔNG** đổi `WAuto.exe.moi`: đổi tay `WAuto.exe` → `.truoc`, rồi `WAuto.exe.moi` → `WAuto.exe`.
4. Mở WAuto → nhóm **Hoạt động** → tab **TK** → cuối tab phải thấy ba dòng mới:
   `[v] Về thành xong thì tới rương`, `Rương cửa: [...]`, `[v] Cất trang bị trong túi vào rương`.
5. Cấu hình cũ `APdata\<ID>.dat` vẫn dùng được (ba trường mới nằm ở **cuối** struct). Mặc định:
   tính năng **TẮT**, cửa = **Gần nhất**, cất trang bị = **bật** (chỉ có tác dụng khi bật tính năng).

---

## 2. Người chơi dùng thế nào

Tab **TK**, ngay dưới ô *Hết trận về*:

| Ô | Ý nghĩa |
|---|---|
| `[v] Về thành xong thì tới rương` | Sau khi đã về tới thành đã chọn, auto đi tiếp tới rương thay vì trả máy ngay. |
| `Rương cửa` | Trung tâm / Đông / Tây / Nam / Bắc — **chỉ hiện những cửa thành đó thật sự có** — hoặc *Gần nhất (tự chọn)*. |
| `[v] Cất trang bị trong túi vào rương` | Cất mọi **trang bị** trong hành trang vào rương. Thuốc, phù, nguyên liệu giữ nguyên trong túi. |

Chạm vào rương chạy script obj `OpenBox(); SetRevPos(nn);` nên **vừa mở rương vừa đặt lại điểm hồi sinh**
về đúng cửa đã chọn — giống hệt khi người chơi tự bấm vào rương đó.

Đổi thành ở ô *Hết trận về* thì danh sách *Rương cửa* tự nạp lại cho đúng thành.

---

## 3. Bảng dữ liệu — sinh tự động, đã kiểm chứng chéo

`Sources\Core\Src\KTongKimTables.h` cuối tệp: `g_TKRuong[7][5]` (toạ độ ô) + `g_TKRuongRev[7][5]` (revId để đối chiếu).
Sinh bởi `ReverseTools\gen_tk_ruong.py` từ hai nguồn của **máy chủ đang chạy**:

- `bin\server\script\item\ib\shenxingfu.lua` — `RUONG_ARRAY` (toạ độ rương, **danh sách thô, không mang hướng**)
  và `THANH_ARRAY` (menu Thần Hành Phù: thành nào có cửa nào, kèm revId).
- `bin\server\settings\RevivePos.ini` — revId → toạ độ điểm hồi sinh của chính cửa đó.

Ghép: **cửa X của thành Y = rương gần điểm hồi sinh của cửa X nhất**. Kết quả 29/29 mục khớp,
khoảng cách 3–40 ô, song ánh (không rương nào bị gán hai cửa, không rương nào thừa).

Thành có ít cửa: **Đại Lý** chỉ Trung tâm + Bắc; **Biện Kinh** không có Nam; **Lâm An** không có Trung tâm và Tây.

**Kiểm chứng độc lập:** ba script obj của Lâm An (`ÁÙ°²¸®´¢ÎïÏä1/2/3.lua`) gọi `SetRevPos(67/68/69)`;
bảng sinh ra gán đúng rương ấy cho Nam/Đông/Bắc (rev 67/68/69). Ngược lại, giả thiết *"thứ tự
`RUONG_ARRAY` = thứ tự tên tệp obj"* cho kết quả sai tới 424 ô, nên **bắt buộc ghép bằng khoảng cách**.

---

## 4. Mã đã sửa

`Sources\Core\Src\ipc_shared.h` (và bản sao ở `E:\Src_Auto_Ngoai\WAuto\WAuto`) — ba trường **ở cuối** `autoData`:
`bTKRuong`, `nTKRuongHuong` (0 TT / 1 Đ / 2 T / 3 N / 4 B / 5 gần nhất), `bTKRuongCat`.

`Sources\Core\Src\CoreShell.cpp`:

- enum pha: `TKP_RUONG` chèn giữa `TKP_VETHANH` và `TKP_DONE` (không nơi nào so sánh thứ tự pha).
- `TK_TimRuongObj(x, y, r)` — quét `ObjSet` tìm `Obj_Kind_Box` đang **đóng** gần điểm nhất.
- `TK_CatDo(...)` — cất một trang bị mỗi nhịp, dùng lại `DT_EnsureUnlock` / `DT_ChestRoomFor` / `DT_BagToBox`.
- `TKP_VETHANH`: về tới thành mà bật tính năng thì sang `TKP_RUONG` thay vì trả máy.
- `TKP_RUONG`: đi tới rương (`DT_WalkTo`, sai số 150 mps) → chạm (`CheckObject`) → cất đồ → trả máy.
- Bảng tên pha (cột *Việc* của danh sách WAuto) thêm `"Tống Kim: tới rương của thành"`.

WAuto: `Resource.h` ID 475–478 · `WAuto.rc` 4 ô ở y 271/283/295, `s_aTabDay[9]` 268 → 306 ·
`WAuto.cpp` bảng `s_aTKRCo[7][5]` + `WA_NapRuongHuong()`, lưu/nạp, `ShowTab`, 4 ghi chú rê chuột,
mục *VỀ THÀNH XONG THÌ TỚI RƯƠNG* trong note tab TK.

**Sửa kèm hai lỗi có sẵn** (phát hiện khi làm, đều nằm đúng chỗ đang sửa):

1. **Rơi `case` ở `WM_COMMAND`**: cả dải `IDC_COMBO_0_CH … IDC_COMBO_15_VE` rơi thẳng vào khối của
   `IDC_COMBO_16_AC` (chèn sai chỗ ở đợt Ác chính 03/09), nên **đổi bất kỳ combo nào cũng ghi tên mục
   đang chọn vào `szAcChinhTen`** — cấu hình Ác chính bị đè bằng ví dụ "Lâm An Phủ". Đã chuyển khối
   `IDC_COMBO_16_AC` xuống sau khối lưu chung.
2. Ghi chú tab 10 chứa `script\leaguematch\timetable.lua` → `\l` `\t` bị hiểu là escape (cảnh báo C4129,
   ghi chú in ra sai). Đổi sang dấu chéo xuôi.

---

## 5. Bẫy

1. **Cất đồ không cần đứng cạnh rương** — `c2sdnmbr_exchangeitem` chỉ đòi `m_CUnlocked` và không ở
   fight-mode. Việc đi tới rương là để **đặt điểm hồi sinh** đúng cửa, không phải để cất được đồ.
2. **Chạm rương thì phải ở trong 200 mps** (`defMAX_EXEC_OBJ_SCRIPT_DISTANCE`) và obj phải đang **đóng**.
3. Ngoài thành (`m_FightMode`) server từ chối mọi thao tác rương — auto chỉ cất khi đã ở trong thành.
4. Rương khoá mà tab **Hậu cần** chưa có mật khẩu thì không cất được; auto báo rồi trả máy, đồ để lại trong túi.
5. `SetRevPos(0)` trong vài script obj cũ là **vô hiệu** (`RevivePos.ini` map 1 không có khoá `0`);
   nguồn đúng để đối chiếu là `THANH_ARRAY` của Thần Hành Phù, không phải số trong tên tệp obj.
6. Bộ vá `goi_va_wauto_tk_ruong_0409.py` viết tiếng Việt giữa cặp `@@…@@` rồi mã hoá TCVN3 bằng bảng
   của skill — **không gõ tay byte TCVN3**. Đây là cách duy nhất được phép với nguồn JX1.

Sao lưu trước khi vá: `*.truoc_tkruong_0409` (7 tệp) ở `Sources\Core\Src` và `E:\Src_Auto_Ngoai\WAuto\WAuto`.
