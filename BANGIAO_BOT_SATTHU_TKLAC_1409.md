# BÀN GIAO 14/09/2026 — BOT: TRÁNH BOSS SÁT THỦ + RA MAP BÁO DANH SAU BẢO TRÌ GIỮA TRẬN TỐNG KIM

> Chủ game giao (nguyên văn):
> 1. *"tôi cần bạn fix lại bot khi luyện công không đánh boss sát thủ"*
> 2. *"tôi cần bạn fix lại khi bot đang đánh tống kim mà bảo trì nữa chừng vào lại thì phải ra map báo danh chứ không được đứng trong map tống kim"*
> 3. *"tôi cần bạn fix cẩn thận tìm chính xác vị trí fix không ảnh hưởng các tính năng khác"*
> 4. *"fix xong phải phản biện"*
>
> Chủ chốt hướng việc 1 khi được hỏi: **bot TRÁNH HẲN boss Sát Thủ**.
>
> Bản vá: `ReverseTools/goi_va_bot_satthu_tklac.py` (5 hunk, **chỉ** `Sources/Core/Src/KPlayerBot.cpp`, +108 dòng, ASCII, idempotent, `--thu` = chỉ kiểm neo).
> Build: Core `Server Release|x64` PASS + Core `Client Release|Win32` PASS (0 error C, link thật).
> Không đổi giao thức, không đụng script Lua, không đụng dữ liệu, không đụng client.

---

## 1. VIỆC 1 — BOT LUYỆN CÔNG VÀ BOSS SÁT THỦ

### 1.1 Gốc (đo trên bản đang chạy, không đoán)

Boss nhiệm vụ Sát Thủ = 160 con sinh lúc boot bởi `HD3_DriverInit` → `add_killertasknpc(addkillertasknpc)` (`script/tinhnang/3hoatdong/hd3_driver.lua:135`, bảng ở `script/nhiemvu/tollgate/killbosshead.lua`), rải trên 40 bản đồ, mỗi con được gán `SetNpcScript(..., "\script\task\tollgate\killer\kill_level.lua")`.

Thuộc tính trong `settings/npcs.txt` (template 760–819):

| Nhóm cấp | Máu (`LifeParam3`) | Hồi máu (`LifeReplenish`) |
|---|---|---|
| 20 | 22.000 | 0\|20 |
| 50 | 100.000 | 0\|200 |
| 80 | 900.000 | 0\|700 |
| **90** | **1.300.000** | **0\|1000** |

Đo `bot.log` (40 MB cuối, 4 giờ ngày 14/09):

- **233 lượt** bot bỏ mục tiêu có máu tối đa **1.300.000** = boss Sát Thủ cấp 90.
- **14 boss** khác nhau, mỗi con bị **6–12 bot** thay nhau cắn.
- Máu boss **hồi lại**: con `90345` đi từ 772.318 lên 1.138.415; con `90353` từ 1.213.944 lên 1.260.795.

Chuỗi nhân quả: `pb_Fight` có luật chống-đánh-vô-ích — *10 giây không sụt nổi 10% máu thì bỏ mục tiêu và cấm 45 giây*. Với boss 1,3 triệu máu, 10% = 130.000 sát thương trong 10 giây, bot không bao giờ đạt, nên **mọi** bot đều nhả boss sau đúng 10 giây; boss tự hồi máu trong lúc không ai đánh. Vòng lặp vô tận: bot phí hàng giờ quanh một con không bao giờ hạ được, boss của người chơi làm nhiệm vụ thì bị chiếm chỗ. Đúng cảnh chủ thấy: *"bot không đánh boss sát thủ"*.

### 1.2 Vá

**Nhận diện boss** — hàm mới `pb_LaBossSatThu` (`KPlayerBot.cpp`, đặt ngay trước `pb_FindTarget`):
1. máu tối đa < 20.000 → không phải (boss nhỏ nhất 22.000; quái thường dừng ở bước này, không tốn gì thêm);
2. `ActionScript` của NPC chứa `"kill_level"`.

Vì sao dùng `ActionScript` chứ **không** dùng cột `LevelScript` (`task_killboss.lua`): cột đó có **77 template**, trong đó có **Tống Tiên phong / Kim Tiên phong** (`npcs.txt` 1068/1073) là NPC **trong trận Tống Kim** — lọc theo cột đó sẽ chặn nhầm và làm hỏng Tống Kim. Còn `kill_level.lua` đã quét cả cây script: **chỉ** `add_killertasknpc` gán, không NPC nào khác dùng.

**Ba điểm chạm:**

| # | Chỗ | Việc |
|---|---|---|
| a | `pb_FindTarget`, đầu hàm | tách biến `bMapTK` (đang đứng trên map 379) — `bTrongTK` (đã ra trận) giữ nguyên nghĩa cũ |
| b | `pb_FindTarget`, vòng quét ứng viên | bỏ qua boss Sát Thủ, **trừ khi** đang ở map Tống Kim (chiến trường không có boss Sát Thủ nhưng đầy NPC máu lớn — miễn hẳn để không tốn lần so chuỗi nào trong trận) |
| c | `pb_Fight`, khối kiểm mục tiêu đang giữ | nếu mục tiêu đang cầm là boss Sát Thủ và bot không ở Tống Kim → nhả ngay, cấm 45 giây, ghi log `[BotDanh] ... tranh boss Sat Thu ...` |

Điểm (c) để bot đang cắn boss lúc nạp bản mới nhả ra ngay, không phải chờ hết đồng hồ 10 giây.

---

## 2. VIỆC 2 — BẢO TRÌ GIỮA TRẬN TỐNG KIM, BOT ĐỨNG TRONG CHIẾN TRƯỜNG

### 2.1 Gốc

Máy trạng thái Tống Kim của bot (`b.nTk`) nằm trong **bộ nhớ tiến trình**; vị trí bot nằm trong **blob roledb**. Bảo trì giữa trận → bot nạp lại đúng giữa map chiến trường **379** với `b.nTk = 0`, và còn mang theo camp phe Tống/Kim + khoá cờ PK + điểm hồi sinh trong trận từ blob (`pb_TkTraTrangThai` chưa từng chạy cho nó).

Đường duy nhất kéo bot ra khỏi map là `pb_RaBai`, mà `pb_RaBai` chỉ chạy trong nhánh `PB_AI_FIGHT` của `pb_DriveBot`. Bot cũ nạp lại được đặt `PB_AI_IN_FACTION` (`PB_OnRoleData`), chỉ lên `PB_AI_FIGHT` khi công tắc *bật đánh quái* được bấm — công tắc này là biến trong bộ nhớ nên cũng mất sau bảo trì. Kết quả: **bot đứng im giữa chiến trường** cho tới khi chủ bấm lại lệnh bài.

### 2.2 Vá

Khối mới `[TKLAC]` trong `pb_DriveBot`, đặt **sau** khối tự hồi sinh và **trước** khối Tống Kim (`if (b.nTk)`), nên không đụng bot đang trong trận thật:

```
b.nTk == 0  &&  đang đứng trên map 379
   → chờ so le 1–31 giây theo chỉ số bot (tránh hàng trăm bot đổi map cùng một khung)
   → pb_TkTraTrangThai  (trả camp, cờ PK, điểm hồi sinh — đúng đường người chơi thật)
   → ChangeWorld về map báo danh 324, đáp đúng điểm battle_transprot của lib_tktc
       Tống (1541,3178) / Kim (1570,3085), rải đều hai điểm, mỗi con một ô riêng
   → thất bại thì 10 giây sau thử lại; log [BotTK] ... -> tra trang thai + ve map bao danh 324
```

Khối này còn là lưới an toàn cho một đường cũ: khi pha 5 phù về bãi trượt 3 lần và `ChangeWorld` về thành cũng trượt, bot từng ở lại map 379 với `nTk = 0` vĩnh viễn.

---

## 3. PHẢN BIỆN (tác tử độc lập đọc diff + cây script sống + `npcs.txt`) → đợt b `ReverseTools/goi_va_bot_satthu_tklac_b.py`

| Mức | Phát hiện | Xử lý |
|---|---|---|
| **CAO** | Khối `[TKLAC]` `return` vô điều kiện: nếu `ChangeWorld` về 324 hỏng (map chưa mở, không tìm được ô đặt chân) thì bot **kẹt vĩnh viễn** trên 379 — vì khối nằm trước mọi nhánh AI nên `pb_RaBai`, đường cứu còn lại, không bao giờ chạy nữa. Trước bản vá bot `PB_AI_FIGHT` vẫn được `pb_RaBai` kéo đi | Thêm **đường lui y hệt pha 5**: 3 lần trượt → về **thành nhà** (`pb_ThanhNha` + `pb_DtVeThanh`); cả đường đó cũng trượt thì **thôi `return`**, để các nhánh dưới tiếp quản |
| **CAO** | `pb_TkTraTrangThai` (một lượt vào Lua `bot_tk_thoat`, ~12 lệnh engine) **và** dòng log chạy lại **mỗi 10 giây** không có trần, nhân lên hàng trăm bot kẹt | Trả trạng thái **đúng một lần** (`b.nTkGoiThu == 0`); log lần đầu + lần thành công + lần lui |
| **VỪA** | Thiếu gác `b.nNhomNguoiIdx` / `b.nBanSap`: bot đang trong tổ đội **người chơi thật** (hoặc đang ngồi bán sạp) mà đứng trên 379 sẽ bị trả trạng thái rồi giật về 324 | Thêm hai gác đúng khuôn `pb_TkDuTuCach` |
| **VỪA** | Về tới 324 xong bot vẫn đứng im vì `b.nAi` vẫn là `PB_AI_IN_FACTION` (công tắc *bật đánh quái* mất sau bảo trì), nên cũng không được gọi quân ở trận sau | **Cố ý không sửa.** Chủ chỉ yêu cầu *"ra map báo danh"*; tự bật chế độ đánh cho bot mà chủ chưa hề bật là đổi hành vi ngoài yêu cầu (chính chú thích sẵn có ở `pb_TkDuTuCach` đã chốt ý này). Khi chủ bấm bật đánh, bot ở 324 tự đi tiếp bằng đường `pb_RaBai` |
| THẤP | Chú thích ghi sai dải template (760..819) | Sửa thành **761..820**, ghi thêm: máu là hằng số theo template, không phụ thuộc cấp |
| THẤP | `"kill_level"` là chuỗi con của `"skill_level"` (hiện chưa có đường dẫn nào như vậy) | So chặt hơn: `"killer\kill_level"` |
| THẤP | Boss `makeboss.lua` (máu 6,2–9,6 triệu, hồi 4.800–6.000) có `ActionScript` rỗng nên vẫn rơi vào vòng 10 giây / cấm 45 giây | Ngoài phạm vi hai việc chủ giao — gộp vào mục 5 để chủ quyết |

**Tác tử xác nhận đúng:** thứ tự định nghĩa hàm (`pb_CoChuoi` 3810 < `pb_LaBossSatThu` < `pb_FindTarget` < `pb_Fight`; `pb_TkTraTrangThai` < `pb_DriveBot`); mọi tên biến trong khối mới tồn tại; **ngưỡng 20.000 không bỏ sót boss nào** (máu boss là hằng số theo template vì `LifeParam=100, LifeParam1=LifeParam2=0`, con nhỏ nhất 22.000); **không chặn nhầm Tống/Kim Tiên phong** (cột `ActionScript` của chúng rỗng nên dừng trước khi so chuỗi); cả cây script sống chỉ `killbosshead.lua:190` gán `kill_level.lua`; `ActionScript` luôn kết thúc NUL; **không xung đột `b.nTkTick`** (mọi đường đặt `nTk = 0` đều đặt `nTkTick = 0` cùng lúc) và **không có nguy cơ kéo bot về 324 giữa trận thật** (bot trong trận mang `nTk` 1..5); bot chết trên 379 sau bảo trì vẫn thoát được; `bot_tk_thoat` an toàn khi bot không thuộc mission nào; hiệu năng thậm chí lãi vì phép kiểm mới đặt trước `NpcSet.GetRelation` và trong Tống Kim không gọi lần nào.

---

## 3b. BINARY CHỜ SWAP

- `bin\server\CoreServer.dll.moi` = **`a672eee21456`** (14/09 16:09, 18.505.216 byte). Khe `.moi` trước đó **trống** nên không đè bản của phiên nào.
- **Client không cần deploy**: `KPlayerBot.cpp` bị `ExcludedFromBuild` ở cả 4 cấu hình Client trong `Core.vcxproj` (dòng 810–813), nên `CoreClient.dll` không đổi. Bản `client\CoreClient.dll.moi` (12/09) của phiên khác giữ nguyên, tôi không đụng.
- Bản đang chạy `CoreServer.dll` là `d5fef156` (11/09 03:51), cũ hơn HEAD nhiều commit — `.moi` này là **superset** (HEAD `7b3d744e` + hai bản vá hôm nay), nên swap sẽ kéo theo mọi thay đổi đã đẩy từ 11/09 tới nay.

## 4. NGHIỆM THU SAU KHI SWAP

```bash
grep -a "tranh boss Sat Thu" bot.log | tail -20
```

1. **Boss Sát Thủ**: sau khi bot nạp bản mới, log `[BotDanh] ... tranh boss Sat Thu <idx> (HP .../1300000) -> bo muc tieu` xuất hiện một đợt rồi **im hẳn** (bot không chọn boss nữa). Kiểm ngược: `grep -a "bo muc tieu" bot.log | grep -c "1300000"` phải về **0** sau vài phút. Đứng cạnh một boss Sát Thủ quan sát: bot đi ngang qua, không dừng lại đánh.
2. **Tống Kim sau bảo trì**: tắt GameServer giữa trận rồi bật lại, gọi bot. Trong 1–31 giây mỗi bot còn kẹt ở map 379 phải rời sang map 324, log `[BotTK] ... dung trong chien truong 379 ma KHONG o trong tran (bao tri giua tran?) -> tra trang thai + ve map bao danh 324 (ret=1)`. Kiểm `[TkCensus]` không còn dòng `379:` khi không có trận.
3. **Không hồi quy**: trận Tống Kim kế tiếp diễn ra bình thường (bot vẫn được gọi quân, báo danh, ra trận, đánh nhau, hết trận về bãi); bot vẫn đánh quái thường và boss hoàng kim như cũ.

---

## 5. PHÁT HIỆN PHỤ — CHƯA SỬA, CHỜ CHỦ QUYẾT

Trong cùng đợt đo còn một hiện tượng **khác**, không nằm trong hai việc chủ giao nên tôi không tự sửa:

- **3.315 lượt** bot bỏ mục tiêu có máu **30.000/30.000 — không sụt một điểm nào** trong 10 giây, trải trên **1.061 NPC** khác nhau và **262 bot**, đều đặn ~10–14 lượt mỗi phút.
- Hiện tượng **vẫn tiếp diễn sau khi trận Tống Kim đóng** (15:47), nên **không phải** lính Tống Kim.
- Nghĩa là mỗi lượt bot phí 10 giây vung chiêu vào một mục tiêu nó không gây nổi 1 điểm sát thương, rồi cấm 45 giây, rồi lặp lại với con khác cùng loại.
- Ứng viên theo `npcs.txt` (máu 30.000): `Mộc nhân` (585), `Boss Thấu Thần` (580), `Đầu Thạch Xa` (524), `Tống binh` (630) / `Kim binh` (636).

Nếu chủ muốn, đợt sau tôi đo đích danh template rồi cho bot bỏ qua luôn nhóm này (cùng khuôn với boss Sát Thủ).
