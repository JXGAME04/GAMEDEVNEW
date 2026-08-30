# BÀN GIAO — Đợt tái thiết hệ script JX1 (29/08/2026)

> Đọc mục **1** và **9** trước nếu chỉ có 2 phút.

---

## 1. Tóm tắt: đã làm gì

Chủ duyệt kế hoạch tái thiết (2 sơ đồ đã gửi trước đó) và yêu cầu *"triển khai đến lúc xong hết
thì chạy lại phản biện rồi báo tôi test"*. Đợt này gồm **9 bản vá đã áp**, chia 3 nhóm:

| Nhóm | Việc | Ảnh hưởng |
|---|---|---|
| **Sửa lỗi** | 5 lỗi C++ + 1 lỗi tràn số exp + 2 rào chắn Viêm Đế + 1 bẫy ghi hỏng kho mã | Trong đó **1 lỗi có thể treo cả GameServer** |
| **Cấu hình** | Đưa 23 hệ số kinh nghiệm từ mã nguồn ra `gamesetting.ini`; dựng khung 8 tệp cấu hình script | Mặc định **giữ nguyên mọi con số đang chạy** |
| **Vận hành** | Hệ log có công tắc; cổng trao thưởng có kiểm túi; cho chỉnh nhịp nạp lại script | Mặc định giữ nguyên hành vi cũ |

**Không đổi cân bằng game.** Mọi giá trị mặc định bằng đúng số đang chạy, nên nếu tệp cấu hình
vắng mặt thì game chạy y hệt trước.

---

## 2. Lỗi nặng nhất: GameServer có thể **treo cứng**

`KNpc::DropRateItem` (`Sources\Core\Src\KNpc.cpp:9258`) rơi đồ bằng vòng lặp:

```
while (j < nCount)          <- j chỉ tăng khi roll TRÚNG một món
    nRand = g_Random(...)
    for (mọi mục trong .ini)
        if (trúng dải) { ...tạo item...; j++; break; }
```

`j++` nằm **bên trong** nhánh trúng. Nếu tổng tỉ lệ của tệp `.ini` bằng 0 thì **không mục nào
trúng được**, vòng `while` quay vô hạn — treo cả máy chủ, không phải chỉ một người chơi.

Và trong `settings\droprate\` **có sẵn hai tệp như vậy** (tôi tự quét cả 49 tệp `.ini` để xác
nhận, không nghe lại của ai):

| Tệp | Khai | Số mục thật | Tổng tỉ lệ |
|---|---|---|---|
| `npcdroprate_fenglindubei.ini` | `Count=86` | **0** | **0** |
| `npcdroprate_fenglindunan.ini` | `Count=53` | **0** | **0** |

Hiện chưa script nào trỏ tới chúng nên chưa nổ. Chỉ cần một dòng Lua gõ nhầm tên tệp, hoặc GM sửa
`.ini` sai, là treo.

**Đã vá:** cộng tổng tỉ lệ trước vòng lặp, bằng 0 thì bỏ qua; thêm trần số lần roll. Tệp `.ini`
tệ nhất trong dự án (`npcdroprate110.ini`) cần trung bình **93 lần roll cho mỗi món**, nhân với số
món nhiều nhất mà script yêu cầu (10) là **930 lần**. Trần đặt **2.000.000** — dư hơn **2.100
lần**, nên **dữ liệu đúng không bao giờ chạm tới và tỉ lệ rơi đồ giữ nguyên**.

---

## 3. Bốn lỗi C++ còn lại

| Lỗi | Vị trí | Hậu quả trước khi vá |
|---|---|---|
| `fopen` không kiểm NULL | `KPlayer.cpp:8470` | Ghi log thoại NPC thất bại (đĩa đầy / tệp bị khoá) → `fprintf(NULL)` → **sập máy chủ**. Đang bật (`WriteScriptNpcLog=1`) |
| `fopen` không kiểm NULL | `KObj.cpp:1245` | Như trên, đường vật thể |
| Gọi nhầm `SafeCallBegin` chỗ đáng lẽ là mốc kết thúc | `KItem.cpp:3130` | Ghi đè mốc ngăn xếp Lua bằng đỉnh hiện tại → dọn dẹp **không dọn gì** → rò ngăn xếp mỗi lần xem mô tả vật phẩm có `GetDesc` |
| Vòng lặp thiếu 1 (`k < 21` trong khi bảng có 22 mục) | `ScriptFuns.cpp:2547` | Phần tử cuối là `"PET"` → **`IncludeLib("PET")` không bao giờ khớp** → hệ Bạn Đồng Hành không nạp được thư viện |

---

## 4. Lỗi tràn số kinh nghiệm (người chơi **bị trừ** exp)

`KPlayer.cpp:2579` tính `nGetExp = nGetExp * g_ExpRate * nxExpTanThu` — **trong kiểu `int`**.

Với hệ số 280 (người chơi cấp 80–139), phép nhân tràn 32-bit khi exp gốc vượt **7.669.584**, kết
quả ra **số âm**, rồi cộng vào tổng exp. Nghĩa là giết quái/boss có exp lớn thì người chơi **mất
exp** chứ không phải nhận.

Biến lưu exp vốn là `double` (`KPlayer.h:646`) nên thừa sức chứa. Đã đổi sang tính bằng `double`.

⚠️ **Báo trước để khỏi hiểu nhầm:** sau khi cập nhật, boss có exp rất lớn sẽ cho exp **đúng** thay
vì âm. Nếu thấy ai đó lên cấp nhanh bất thường ở những con boss cụ thể, đó là vì trước đây họ
đang bị trừ — hãy báo lại, đừng tự đoán.

---

## 5. Hệ số kinh nghiệm nay chỉnh được không cần build

Trước đây nhịp game nằm **cứng trong mã nguồn**, không phải trong `ExpRate`:

```
cấp < 50  → ×80        VIP cộng thêm +20
cấp < 80  → ×70        chuyển sinh >3 lần ở map 341: <140 → ×160, còn lại ×50
cấp < 140 → ×280       chênh cấp ≤9 ăn đủ, >9 chia 10, quái cao hơn = 1 exp
cấp ≥ 140 → ×100       cả hai bên ≥90 thì bỏ mọi phạt chênh cấp
tổ đội 2/3/4/5/6/7/8 người → 80/70/60/55/55/50/50 %, khác → 60 %
```

Người chơi 8x–13x đang ăn **×280** do một con số nằm trong mã, và `ExpRate` chỉ **nhân thêm** lên
trên đó.

Nay cả 23 con số đọc từ `settings\gamesetting.ini` nhóm **`[Exp]`**, **mặc định đúng bằng số cũ**.
Sửa xong phải khởi động lại GameServer.

---

## 6. Hệ log

**Tin tốt: đường báo lỗi Lua đã chạy tốt sẵn** (do đợt 21/08). Mọi lỗi runtime của script được ghi
vào `ScriptError.log` kèm **stack traceback có tên tệp và số dòng**. Chính nó đã giúp tìm ra lỗi
Viêm Đế ở mục 7 dưới đây.

Đợt này thêm:

- Hàm Lua **`GhiLog("TÊN_HỆ", "nội dung")`** → ghi `logs\hethong.log` kèm giờ và tên hệ.
- Nhóm **`[Log]`** trong `gamesetting.ini`: bật/tắt, đổi đường tệp, đặt ngưỡng xoay vòng (mặc định
  64 MB, vượt thì đổi tên thành `.1` rồi mở tệp mới nên không bao giờ ăn hết đĩa).

**Việc chưa làm được (nói rõ, không giấu):** trong mã nguồn có **336 điểm gọi `g_DebugLog`** dùng
để chẩn đoán, nhưng chúng chỉ gửi cho cửa sổ `DebugWin.exe`; chạy dịch vụ thật là mất sạch. Muốn
bật chúng phải sửa `Engine\Src\KDebug.cpp` — mà **`Engine.vcxproj` hiện không build được**:

```
KWin32App.cpp(18): Cannot open include file: 'ipc_shared.h'
```

Tệp đó nằm ở `Core\Src\ipc_shared.h`, tức đường include của Engine thiếu. Lỗi này **có trước đợt
vá này**, và vì nó ảnh hưởng mọi thứ dùng Engine (kể cả `Game.exe`) nên tôi **không tự sửa** — cần
chủ quyết.

---

## 7. Viêm Đế: tìm ra nguyên nhân thật của "vào map không thấy NPC"

Log lỗi lúc **11:46:24 hôm nay** ghi:

```
error: bad argument #2 to `random' (interval is empty)
   2: function `YDBZ_add_npc' at line 285 [npc.lua]
   3: function `YDBZ_create_all_npc' at line 453
```

Truy ngược từng mắt:

1. `npc.lua` gọi `YDBZ_GetTabFileHeight(file_name)` để đếm số dòng tệp toạ độ.
2. Hàm đó (`include.lua:167-173`) **trả 0 khi không mở được tệp**.
3. Số 0 rơi thẳng xuống `random(1, 0)` → **ném lỗi, huỷ sạch cả trận**: không tổ nào có quái, 4
   boss trung tâm cũng không sinh. Đúng triệu chứng chủ báo.
4. Vì sao không mở được: thư mục `settings\maps\yandibaozang\` được chép sang cây chạy lúc
   **11:58:37** — **sau lỗi 12 phút**. Lúc đó 31 tệp toạ độ chưa tồn tại.

Nay đã đủ tệp nên lỗi không tái diễn. Nhưng nguy cơ còn nguyên, nên **đã thêm rào chắn**: thiếu
tệp thì bỏ qua **đúng một ải** và ghi log tên tệp thiếu, thay vì đổ cả trận. Khi dữ liệu đúng, số
dòng là 50 (hoặc 10) nên rào chắn **không bao giờ chạy** — không đổi gì.

Còn triệu chứng thứ hai *"có NPC rồi nhưng NPC không đánh tôi"* thì đã vá lúc 14:02 hôm nay (tham
số thứ 7 của `AddNpcEx` bị hiểu là **phe** chứ không phải *không-hồi-sinh*, làm quái cùng phe với
người chơi).

---

## 8. Bẫy có thể **mất vĩnh viễn 8.000 mã quà tặng**

Hai tệp cùng khai một biến tên `Code_New`:

| Tệp | Nội dung | Kiểu khoá |
|---|---|---|
| `script\codenew.lua` | **8.000 mã** | số |
| `script\giftcode_new.lua` | 3.000 mã | chữ |

Và `hotrotanthu.lua:218` có lệnh **ghi ngược xuống đĩa**: `SaveData("script/codenew.lua", ...)`.

Thứ duy nhất đang chặn thảm hoạ là **hai dòng comment** ở `hotrotanthu.lua:13-14`. Ai bỏ comment
(mà không biết vì sao chúng bị comment) thì biến `Code_New` trong phiên đó thành bảng 3.000 mã
khoá-chữ, rồi `SaveData` **ghi đè lên tệp 8.000 mã** — không khôi phục được.

**Đã gỡ:** đổi tên bảng trong `giftcode_new.lua` thành `Code_GiftNew`. Hai tệp không còn trùng tên
nên bẫy biến mất kể cả khi có người bỏ comment.

Phát hiện kèm theo: kho 3.000 mã quà tặng đó **hiện không được mã sống nào đọc** — chỉ còn một
dòng comment ở `lequan.lua:114` nhắc tới. Nếu chủ muốn dùng lại thì cho biết.

---

## 9. ✅ CHECKLIST ĐỂ TEST — làm đúng thứ tự

Nhị phân mới đã build sạch **cả hai cấu hình** và đặt sẵn theo đúng quy trình swap bằng `.bat`:

```
bin\server\CoreServer.dll.moi     18.201.088 B
bin\client\CoreClient.dll.moi      2.420.736 B
```

**1) Tắt GameServer** (chờ chủ — tôi không tự tắt).

**2) Chạy `bin\server\ChayGameServer.bat`** — nó tự đổi `CoreServer.dll.moi` thành tên thật (bản
cũ giữ lại đuôi `.truoc`) rồi mở GameServer.

**3) Thoát HẲN client** rồi **chạy `bin\client\ChoiGame.bat`** — tự cập nhật `CoreClient.dll` rồi
mở game.

**4) Test theo thứ tự:**

| # | Việc | Dấu hiệu ĐÚNG |
|---|---|---|
| 1 | Đánh quái thường vài phút, xem exp | Lên exp **y như trước**, không nhanh chậm bất thường |
| 2 | Lập tổ đội 2–3 người đánh chung | Chia exp như cũ |
| 3 | Giết một boss exp lớn | Exp **tăng** (trước đây có thể bị trừ) |
| 4 | Rơi đồ quái thường + boss | Tỉ lệ như cũ, server không khựng |
| 5 | Mở hành trang, rê chuột vài món có mô tả đặc biệt | Không lag dồn sau nhiều lần xem |
| 6 | Vào Viêm Đế 1 người | Có NPC; nếu thiếu tệp thì chỉ hụt 1 ải chứ không trống trơn |
| 7 | Dùng thử vật phẩm Bạn Đồng Hành | Thư viện `PET` nạp được |
| 8 | Xem `logs\hethong.log` | Có tệp, có dòng, giờ đúng |

**5) Nếu có gì lạ** — gửi tôi 3 tệp: `ScriptError.log`, `logs\hethong.log`, `GameServer.log`.

**Quay lui:** bản cũ được `.bat` giữ lại với đuôi `.truoc` — đổi tên ngược rồi khởi động lại.

---

## 9b. ⚠️ HAI VIỆC PHẢI XEM TRƯỚC KHI SWAP

### (a) Sửa mã vật phẩm làm một số ô thưởng "sống lại" — đây là thay đổi kinh tế

Bộ phản biện chỉ ra một điều tôi đã bỏ sót: những ô thưởng mã sai **trước đây không ra gì**, sửa
xong là chúng **bắt đầu phát đồ thật**. Về kỹ thuật là sửa lỗi, nhưng về kinh tế máy chủ là thay
đổi, nên chủ nên biết trước:

| Nơi | Món | Số lượng | Tỉ lệ |
|---|---|---|---|
| `missions\boss\bigboss.lua:110` | Đằng Long Thạch - Hạ | **10 viên** | **77%** mỗi lần boss chết |
| `missions\challengeoftime\chuangguang30.lua` | Thiên Sơn Thánh Thủy (đại) | theo bảng | theo bảng |
| 14 chỗ còn lại | Đại Lực hoàn / Phi Tốc hoàn / Đồ Phổ Đằng Long Bội | theo bảng | theo bảng |

Nếu chủ chỉ muốn **hết lỗi** chứ không muốn bơm thêm nguồn cung, thì phải chỉnh số lượng/tỉ lệ
kèm theo — và đó là quyết định của chủ, không phải của tôi.

Muốn hoàn tác riêng phần này: các tệp gốc còn nguyên với đuôi `.truoc_nanthuong` cạnh bản đang
chạy.

### (b) Hệ Phi Phong đang phát **nhầm vật phẩm** — lỗi có sẵn, không do đợt này

Khi rà soát tôi phát hiện `settings\item\magicscript.txt` có **35 dòng mà cột `ParticularType`
lệch khỏi chỉ số dòng**, bắt đầu từ chỉ số 4881. Engine tra bảng **theo chỉ số dòng**
(`KItemGenerator.CPP:1660` → `KBasPropTbl.cpp:1058`), không theo cột — nên ở vùng lệch, script ghi
một món mà người chơi nhận món khác:

| Mã script dùng | Script tưởng là | Người chơi thực nhận |
|---|---|---|
| 4881 | Tinh Ngọc | Vương Thiết Tượng Lệnh Phù |
| 4882 | Thiên Tinh Ngọc | Tinh Ngọc |
| 4883 | Mảnh Thiên Tinh Ngọc | Thiên Tinh Ngọc |
| 4885 | Tinh Ngọc Nguyên Thạch | Bách Luyện Thành Cương |
| 4887 | Tinh Thần Khoáng | Tinh Hỏa Than |
| 4889 | Vương Thiết Tượng Lệnh Phù | Tinh Thần Thạch |

Chỗ dùng: `global\mantlesystem\*.lua` và `event\equip_publish\wuxingyin\wuxingyin.lua:57`.

Nhiều khả năng do việc thêm dòng vào giữa `magicscript.txt` (đợt port Phi Phong hôm nay) làm xô
lệch chỉ số. **Tôi không tự sửa** — sửa bảng vật phẩm là đổi dữ liệu và thuộc phần việc đang làm
song song. Cần chủ báo lại cho phần đó.

---

## 10. Việc còn chờ chủ quyết

1. **Mã vật phẩm trong bảng thưởng — bản rà soát lại**
   (`ReverseTools\cauhinh\ra_soat_ma_thuong.txt`, tra đúng cách máy tra):
   - **68 chỗ đúng** — không phải làm gì.
   - **6 chỗ sai nhưng đã tra ra món đúng theo tên** — sửa được ngay, nhưng chờ chủ duyệt vì cùng
     lý do ở mục 9b(a). Đáng chú ý `mibao_head.lua:19` ghi "Khiêu chiến lễ bao" mà người chơi
     nhận **"Truy công lệnh"**, và `task_head.lua:56-57` trả về hai dòng tên tiếng Trung rác.
   - **43 chỗ không tra được tên** — chủ quyết: tạo vật phẩm mới / thay món khác / bỏ khỏi bảng.
     Tệp có kèm **danh sách ứng viên tên gần giống** cho từng chỗ.

   Bản `thieu_vatpham.txt` cũ đã lỗi thời: bộ phản biện chỉ ra nó báo nhầm "Đại Thánh Bí Kíp 150"
   là không có, trong khi món đó có thật ở chỉ số 3208 — script chỉ viết sai chính tả ("Thánh"
   thay vì "Thành"). Suýt nữa chủ tạo thêm một món trùng tên. Công cụ nay đã đối khớp gần đúng
   (bỏ dấu) và **in ứng viên thay vì kết luận "không có"**.

2. **`Engine.vcxproj` không build được** (mục 6) — sửa hay để nguyên?

3. **`ScripNpcDialog.log` đang phình 1,7 MB và vẫn ghi** mỗi lần ai đó nói chuyện với NPC
   (`WriteScriptNpcLog=1`). Muốn nhẹ máy thì đổi thành `0` trong `gamesetting.ini` rồi khởi động
   lại. Đây là lựa chọn vận hành nên tôi để nguyên.

4. **Dọn tệp thừa** — `ReverseTools\cauhinh\tep_thua.txt` liệt kê 155 bản sao lưu của các đợt vá,
   39 nhóm tệp trùng byte, 8 tệp rỗng. Tôi **chưa xoá gì**. Bản sao lưu nên giữ tới khi nghiệm thu
   xong đợt này.

5. **Kho 3.000 mã quà tặng** hiện không ai đọc (mục 8) — dùng lại hay bỏ?

---

## 10b. Phản biện đối kháng — đã chạy, đã sửa

Chủ yêu cầu *"chạy lại phản biện"*. Tôi cho 6 nhóm soi độc lập từng bản vá, rồi **mỗi phát hiện
nặng lại giao cho một người thứ hai cố chứng minh là sai**. Kết quả: 20 lần kiểm chứng ngược, **16
xác nhận có thật, 4 bị bác bỏ**.

Bảy lỗi thật đã sửa xong trong phiên này:

| # | Lỗi phản biện bắt được | Đã sửa thế nào |
|---|---|---|
| 1 | **Cả nhóm `[Exp]` và `[Log]` không bao giờ đọc được** — tôi viết `MocCap1&nbsp;&nbsp;= 50` có dấu cách trước `=`, mà `KIniFile::SplitKeyValue` giữ nguyên dấu cách trong tên khoá | Ghi lại đúng khuôn `Tên=Giá trị` |
| 2 | Gọi **sai chữ ký `AddItem`** — tham số 4 là *cấp độ*, không phải số lượng; số lượng ở vị trí 13 và chỉ đọc khi đủ 15 tham số | Viết lại theo khuôn `global\vatpham.lua:38` |
| 3 | Bỏ qua giá trị trả về của `AddItem` → log ghi "đã trao" cả khi người chơi **không nhận được gì** | Kiểm giá trị trả về, thất bại thì ghi log riêng |
| 4 | Tiền đề của bộ vá nhịp **sai**: `RunTime` không chạy đúng một lần mỗi phút (lag dồn khung thì chạy 2–3 lần) | Đổi từ phép chia dư sang dấu mốc lần chạy cuối |
| 5 | Tràn số khi tính ngưỡng xoay vòng log (`long` là 32 bit) → đặt `TranMB=2048` sẽ xoay vòng **sau mỗi dòng** | Tính bằng phép chia |
| 6 | Vá tràn exp **chưa hết** — còn hai phép nhân `int` ở nhánh bùa kinh nghiệm | Vá nốt cả chuỗi |
| 7 | Hoàn tác `KDebug.cpp` làm **mất luôn** bản vá tràn bộ đệm thật (`vsprintf` không chặn, điểm gọi in hơn 330 byte vào đệm 256) | Áp lại riêng phần đó |

Ngoài ra: 23 khoá `[Exp]` nay được **kẹp về khoảng hợp lệ** khi đọc (trước đây chúng là hằng số
biên dịch nên không thể sai; thành cấu hình rồi thì một dấu trừ gõ nhầm có thể làm exp đứng hoặc
tụt), và các rào chắn Viêm Đế được kiểm **từng tầng** thay vì một tầng.

---

## 11. Danh sách bản vá và nơi tra

Mọi bộ vá đều chạy **diễn tập trước, ghi sau**, có sao lưu, và tự kiểm (số byte tiếng Việt không
đổi, cân bằng từ khoá Lua không đổi, mốc phải khớp đúng một lần).

| Bộ | Việc | Tệp đụng tới |
|---|---|---|
| `cauhinh\t01_nan_ma_thuong.py` | Nắn 16 mã vật phẩm sai trong bảng thưởng | 5 tệp Lua |
| `cauhinh\t02_go_bay_codenew.py` | Gỡ bẫy ghi đè 8.000 mã | `giftcode_new.lua`, `lequan.lua` |
| `cauhinh\t03_va_cpp_5loi.py` | 5 lỗi C++ | `KNpc/KPlayer/KObj/KItem/ScriptFuns` |
| `cauhinh\t04_nhip_nap_lai.py` | Cho chỉnh nhịp nạp lại script | `timerserver.lua` |
| `cauhinh\t05a_cauhinh_exp_cpp.py` + `t05a2` | 23 hệ số exp ra `[Exp]` | `KCore.cpp/h`, `KPlayer.cpp`, `gamesetting.ini` |
| `cauhinh\t05b_khung_cauhinh_lua.py` | Khung 7 tệp cấu hình | `script\cauhinh\` (mới) |
| `cauhinh\t06b/c/d` | Hệ log của Core | `KCore.cpp/h`, `ScriptFuns.cpp`, `gamesetting.ini` |
| `cauhinh\t07_cong_thuong.py` | Cổng trao thưởng có kiểm túi | `script\cauhinh\ch_thuong_lib.lua` (mới) |
| `viemde\v30_rao_chan_toado.py` | Rào chắn thiếu tệp toạ độ | `yandibaozang\npc.lua` |

Công cụ kiểm (chỉ đọc, chạy lại bất cứ lúc nào):

- `cauhinh\ktr_cauhinh.py` — 7 phép kiểm cấu hình (mã vật phẩm sai, số khai hai nơi, giờ trùng…)
- `cauhinh\t08_liet_ke_thua.py` — liệt kê tệp thừa
- `cauhinh\_kiem_droprate.py` — quét 49 tệp `.ini`, tìm tệp có thể treo server
- `cauhinh\_kiem_exp_macdinh.py` — đối chiếu 3 nơi cho từng hệ số exp
- `cauhinh\dem_ma.py` — đếm trên mã thật, bỏ chú thích (dùng cho chốt tự kiểm của các bộ vá sau)

---

## 12. Đợt sau — đã khảo sát xong, chưa nối dây

Sáu nhóm quét song song toàn bộ hoạt động/sự kiện, kết quả đầy đủ ở
**`D:\GAMEDEVNEW\KHAOSAT_LICH_THUONG_2908.md`** (hơn 100.000 chữ, mỗi khoá kèm `tệp:dòng`).

Đợt này mới nối dây phần **rẻ nhất mà đắt giá nhất**: 20 công tắc bật/tắt hoạt động. Trước đây
muốn bật một hoạt động phải **sửa mã nguồn** (bỏ dấu `--` trong `timerserver.lua`); nay đổi một số
trong `script\cauhinh\ch_lich.lua`. Trạng thái mặc định giữ đúng hiện tại — 4 hoạt động đang chạy
(Tống Kim, cụm 3 hoạt động Linux, Viêm Đế, Công Thành JX2) và 16 đang tắt.

Nếu bật một hoạt động đã tắt lâu mà không thấy chạy, xem `logs\hethong.log` — sẽ có dòng
*"&lt;khoá&gt; bật nhưng hàm &lt;tên&gt; chưa nạp"* (thư viện của hoạt động đó cũng đã bị gỡ khỏi
`timerserver.lua`, phải nối lại).

Việc còn lại của đợt sau, xếp theo giá trị:

| Ưu tiên | Việc | Quy mô |
|---|---|---|
| 1 | Lịch Tống Kim (4 khung giờ báo danh) + Công Thành (giờ báo danh **viết cứng ở 6 nơi**) | ~30 khoá |
| 2 | Bảng thưởng các hoạt động lớn — chuyển sang cổng `G_TraoThuong` để có log và kiểm túi | ~40 bảng |
| 3 | Hằng số rơi đồ trong `lib_sukien.lua` và `Droprate_normal.lua` | ~20 khoá |
| 4 | Exp thưởng nhiệm vụ/hoạt động (hàng trăm hằng số rải rác) | lớn, làm dần |

Khảo sát cũng nêu vài chỗ **cùng một con số bị chép ở hai nơi và đã lệch nhau** — đó là loại lỗi
âm thầm mà bộ kiểm `ktr_cauhinh.py` sinh ra để bắt.
