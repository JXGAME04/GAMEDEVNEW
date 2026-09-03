# BÀN GIAO — WAuto Hậu cần: "phù về bán rác thì đứng yên ở thành không lên lại"

> 02/09/2026 tối. Nguyên văn chủ game: *"WAuto đang bị lỗi phù về bán rác thì đứng yên
> ở thành không lên lại"*.
>
> Commit `7cab3e26` (1 tệp: `Sources/Core/Src/CoreShell.cpp`).
> **Chỉ CoreClient.dll đổi. CoreServer.dll và Game.exe KHÔNG đổi.**

---

## 1. Gốc lỗi

`KSubWorld::FindPath` có **ba** giá trị trả về, không phải hai:

| trả về | nghĩa | `m_nTargetX/Y` được đặt thành |
|---|---|---|
| `1` | tìm được đường tới đúng đích | **toạ độ mình xin** |
| `2` | **đích KHÔNG tới được → đi tới ô gần nhất** | **ô TRUNG GIAN** (`KSubWorld.cpp:1022-1032`) |
| `≤ 0` | không có đường / lưới chưa nạp xong | (StopPath) |

Nhánh `2` nằm ở [`KSubWorld.cpp:701-715`](Sources/Core/Src/KSubWorld.cpp) — `if (!found) … return 2;`.

Ba bộ phận **đi bộ tới trạm** của chu trình Hậu cần (bước 6 mua thuốc · 7 mua phù ·
**9 đi xa phu**) lại đòi `HaveTarget()` ([`KSubWorld.h:190`](Sources/Core/Src/KSubWorld.h))
trả về **trùng từng đơn vị** với toạ độ trạm:

```cpp
if(v[nCurShop].x != x || v[nCurShop].y != y)
{
    g_ScenePlace.RemoveFlag();
    Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;   // <-- HUỶ BƯỚC
    return 0;
}
```

Khi `FindPath` trả `2`, phép so này **không bao giờ trùng** ⇒ mỗi 300 ms (`uTNextReturn`)
đường vừa tính xong lại bị huỷ, quay về `nSubStep = 0`, tính lại, huỷ tiếp… **vô hạn, im
lặng** → nhân vật giật một bước rồi đứng giữa thành.

Và vì `nHomeStep` không bao giờ qua được bước 9, **bước 10 "Đi bản đồ" (Thần Hành Phù —
cái đưa lên map luyện) không bao giờ tới lượt** ⇒ đúng triệu chứng *"không lên lại"*.

Chu trình cũng **không tự thoát được**: `nHomeStep` chỉ được đặt lại khi **cờ chiến đấu
đổi** ([`CoreShell.cpp:14562`](Sources/Core/Src/CoreShell.cpp)) — đứng yên trong thành thì
cờ không bao giờ đổi.

**Vì sao máy Dã Tẩu không dính:** `DT_WalkTo` ([`CoreShell.cpp:3196-3208`](Sources/Core/Src/CoreShell.cpp))
lệch đích thì **chỉ TÍNH LẠI đường, giãn 2,5 giây** — không huỷ bước. Bản vá này bê đúng
khuôn đó sang.

### Cấu hình đang chạy khớp đúng kịch bản

Giải mã `APdata\2513089250.dat` (ghi 18:56 ngày 02/09, 7388 B = đúng cỡ `autoData`):

```
bReturn=1  bSellItem=1  bRepair=1  bWithdraw=1  bSaveItem=1
bBuyLife=0 bBuyMana=0 bBuyPois=0 bBuyTP=0          <- bước 6, 7 bị bỏ qua ngay
bGoStation=1 (nSelStation=0 "Quay lại điểm cũ")     <- bước 9 CHẠY -> kẹt tại đây
bGoMap=1     (nSelMap=8)                            <- bước 10 không bao giờ tới lượt
```

Ở Tương Dương (map 78) điểm phù về là ~(50464,103616), trạm Xa Phu gần nhất
(54146,103434) — cách 3.686 mps, phải băng cả thành: đúng loại đường hay ra `FindPath == 2`.

---

## 2. Đã sửa gì (commit `7cab3e26`)

Chỉ `Sources/Core/Src/CoreShell.cpp`, **3 chỗ × 3 bộ phận (bước 6/7/9)**:

| Chỗ | Trước | Sau |
|---|---|---|
| `nSubStep == 1` (đường về trung tâm) | lệch đích → `RemoveFlag(); nSubStep = 0; return 0;` | lệch đích → **tính lại đường, giãn 2,5 s**, giữ nguyên bước |
| `nSubStep == 2` (đường đến trạm) | lệch đích → huỷ bước như trên | **tính lại đường tới đúng trạm, giãn 2,5 s**, rồi **CHẠY TIẾP** xuống phần đo khoảng cách (`nDist < 300`) để vẫn nhận ra "đã tới nơi" khi đường chỉ đến được gần trạm |
| `nSubStep == 0` (chọn trạm) | — | thêm `g_uHomePath = 0;` để nhịp tính-lại đầu tiên không phải chờ 2,5 s |

Biến giãn nhịp là **biến tệp** `static UINT g_uHomePath` đặt cạnh bảng trạm
(`CoreShell.cpp:385`), đúng khuôn các bộ đếm `g_nDTXaFuDi` / `g_uDTThpT` của máy Dã Tẩu.
**Cố ý KHÔNG thêm trường vào `ExtAuto`/`KPlayer.h`** — làm vậy sẽ đổi `sizeof(KPlayer)` và
kéo theo phải build lại các module server.

Thêm **một dòng nhật ký `[HC-STATE]`** (giãn 3 giây, chỉ chạy khi bật "Tự quay lại"):

```
[HC-STATE] buoc=9 sub=2 map=78 toi=(50464,103616) tram=0 cotgt=1 tgt=(50120,103300)
           xatgt=470 sync=0 path=1800 mokhoa=0 | bat: ban=1 sua=1 rut=1 cat=1
           thuoc=0/0/0 phu=0 giutien=1 xafu=1/0 bando=1/8
```

Hậu cần trước giờ là máy auto **DUY NHẤT** không có dòng log trạng thái nào (dòng
`g_DebugLog("homest…")` gốc bị tắt ở `CoreShell.cpp:16623`), kẹt ở bước nào chỉ còn nước
đoán. Nay nhìn `buoc=` + `sub=` + `cotgt=`/`tgt=` là biết ngay.

### KHÔNG đụng vào

- Bộ lọc bán rác (tab Hậu cần + danh sách Lọc tab Nhặt đồ) — giữ nguyên trạng.
- Thứ tự 13 bước, các ô cấu hình, `ipc_shared.h`, giao thức, script máy chủ.
- `nHomeStep = 100` (5 chỗ) và trạng thái kết thúc `13` — **vẫn là ngõ cụt**, xem mục 5.

---

## 3. Nhị phân — CHỜ SWAP

Bản mới nằm cạnh bản đang chạy trong `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin`:

| Tệp | md5 | Của phiên nào |
|---|---|---|
| `client\CoreClient.dll.moi` | `227895cf1ac6cc76d6dbc63a8f9e3800` (2.456.576 B) | **phiên này** (build từ HEAD nên đã gồm cả FUSCHAT) |
| `client\Game.exe.moi` | `f6a2229c290afce3c815023dd54ea6f4` | phiên `wauto-6e` (FUSCHAT) |
| `server\CoreServer.dll.moi` | `2f9bb7de798bc3310e1c4fc5f624c92c` | phiên `wauto-6e` (FUSCHAT) |

Ba tệp này là **một bộ đồng bộ** (struct `ChatItem` của FUSCHAT đi trong gói
`s2c_diceitem`) ⇒ **phải swap CẢ BA cùng lúc**.

> ⚠️ **Đây là MỘT bộ chung cho BA luồng việc, không phải ba bộ khác nhau** — cùng bộ 3 tệp
> này được ghi ở `BANGIAO_VUHON_TIEUDAO_0209.md` mục 18.4 và `BANGIAO_DUNGLUYEN_HKMP_0209.md`
> mục 6.5. Swap một lần là ăn cả ba. Chia trọn vẹn theo tệp nguồn:
> `CoreServer.dll` ← `KItemDice.cpp` + `KPlayerBot.cpp` + `GameDataDef.h` ·
> `CoreClient.dll` ← `CoreShell.cpp` + `GameDataDef.h` ·
> `Game.exe` ← `UiPlayerBar` + `UiMsgCentrePad` + `GameDataDef.h`.

### Checklist swap

1. **Thoát hẳn** mọi `Game.exe` và `WAuto.exe` (relog nhân vật KHÔNG đủ).
2. Tắt GameServer.
3. Đổi tên `bin\server\CoreServer.dll.moi` → `CoreServer.dll` (giữ bản cũ thành
   `CoreServer.dll.cu_0209_haucan`).
4. Chạy `bin\client\ChoiGame.bat` — tệp .bat này **tự** đổi tên `Game.exe.moi` và
   `CoreClient.dll.moi` thành tên chuẩn rồi mở game.
5. Mở GameServer lại.

> Restart mà chưa làm bước 3-4 thì vẫn chạy bản cũ.
> Kiểm nhanh bản đang chạy có bản vá chưa:
> `grep -c -a -F "[HC-STATE]" CoreClient.dll` → phải ra **1**.

---

## 4. Nghiệm thu — nhìn gì khi test

1. Để túi đầy tới ngưỡng tab Cơ bản → auto dùng phù về thành → bán rác →
   **phải chạy tới Xa Phu** (Tương Dương: trạm (54146,103434)) chứ không đứng im.
2. Nếu vẫn đứng im: mở `bin\client\jx_auto.log`, lọc `[HC-STATE]`, gửi lại vài dòng.
   - `buoc=9 sub=0` đứng mãi ⇒ `FindPath` trả ≤ 0 (lưới đường của map thiếu / bị chặn).
   - `buoc=9 sub=2 cotgt=1` mà `xatgt=` không giảm ⇒ trạm thật sự không tới được.
   - `buoc=13` ⇒ chu trình đã chạy hết mà chưa lên map (xem mục 5).
3. Xem có tới Xa Phu rồi bấm đúng mục **"Quay lại nơi cũ"** không.

---

## 5. Còn lại / chờ chủ game quyết

1. 🔴 **Ô "Mật khẩu rương" trong cấu hình đang chạy đang RỖNG.** Trong
   `APdata\2513089250.dat` byte đầu của `szBoxPass` = 0 (phía sau còn rác `234566` của lần
   lưu trước). Hệ quả: bước 5 **"Cất đồ" không chạy** (đòi rương đã mở khoá), bước 3 "Rút
   tiền" và bước 8 "Giữ tiền" bắn lệnh vào rương khoá nên vô hiệu ⇒ **túi không bao giờ
   được dọn vào rương**, đầy lại rất nhanh và phù về liên tục. Chỉ cần gõ lại mật khẩu
   trong WAuto (tab Hậu cần), không phải sửa mã. Dòng `[HC-STATE] … mokhoa=0` cho biết
   rương chưa mở.
2. 🟠 **Ngõ cụt `nHomeStep = 100` và trạng thái kết thúc `13` vẫn còn** (theo đúng phạm vi
   chủ game chốt: chỉ sửa gốc `FindPath == 2`). Nghĩa là nếu **cả** Xa Phu **lẫn** Thần
   Hành Phù đều hỏng (hết Thần Hành Phù `6,1,1271`, chưa đủ cấp map, hoặc phù về rơi vào
   thành không nằm trong 15 thành có bảng trạm) thì auto **vẫn đứng trong thành**, im
   lặng. Muốn chắc chắn "luôn lên lại" thì cần thêm chốt hạn + thử lại (phương án A đã
   trình bày) — chờ chủ game quyết.
3. 🟠 **Bước 11 dereference iterator không kiểm `end()`** (`CoreShell.cpp:17812`, `17825`):
   `it = g_ShopStation.find(map); v = (*it).second;` — thành nào có bảng thuốc mà thiếu
   bảng tạp hoá / xa phu sẽ **crash client**. Hiện 15 thành đều có đủ 3 bảng nên chưa nổ.
   Chưa vá (ngoài phạm vi chủ game chốt).
4. 🟠 **Bước 10 bắn gói `movemapid` một lần rồi tăng bước vô điều kiện**
   (`CoreShell.cpp:17750`): nếu `shenxingfu.lua : GotoMapId` từ chối (không có Thần Hành
   Phù trong túi — `GetItemCount(0,6,1,1271,-1,-1) <= 0`, hoặc chưa đủ cấp) thì máy chủ
   chỉ `Msg2Player` còn auto vẫn đi tiếp như đã lên map. Chưa vá.
