# BÀN GIAO — VIỆC CÒN TỒN, LÀM TIẾP PHIÊN SAU (03/09/2026)

> Đọc file này TRƯỚC. Chi tiết kỹ thuật của các đợt trước nằm ở `BANGIAO_VUHON_TIEUDAO_0209.md` mục 22-25.

---

## 0. TRẠNG THÁI NHỊ PHÂN HIỆN TẠI

| Tệp | md5 | Ghi chú |
|---|---|---|
| `bin\server\CoreServer.dll` | `489d587d` | đang chạy, có gói `s2c_detonate` |
| `bin\client\CoreClient.dll` | `9d4b6029` | đang chạy, **đã có** cả bản vá bóng mờ `[VHTD 02/09x]` và tooltip `[VHTD 02/09y]` |
| `bin\client\Game.exe` | `0411771f` | đang chạy |
| `bin\client\WAuto.exe.moi` | — | **của luồng WAuto khác**, chưa swap, không phải việc của tôi |

**Không có `.moi` nào của tôi đang chờ.** Đợt 03/09 không build gì.

Đã kiểm nhị phân: client đang chạy **có** chuỗi `Khi bị công kích có` và `HS-DETONATE`.

---

## 1. VIỆC ƯU TIÊN 1 — CHIÊU LƯỚT GIỰT LÙI (chưa vá, cần ĐO trước)

### Đã chốt được gì

Chiêu lướt là chiêu duy nhất mà bấm chiêu cũng chính là dịch chuyển. Client **tự cho nhân vật bay ngay**, không chờ máy chủ. Máy chủ tính lại độc lập và **KHÔNG gửi xác nhận cho chính người vừa bấm**.

**Con số then chốt — ngưỡng nắn NHỎ HƠN tầm lướt:**

| | Khoảng cách (mps) |
|---|---|
| Ngưỡng nắn vị trí `KProtocolProcess.cpp:2529` (`if (nLech >= 256)`) trong `SyncNpcMinPlayer()` | **256** = 8 ô |
| 2118 Tạp Đạp Lưu Tinh | **280** (phẳng mọi cấp) |
| 995 Huyền Ảnh Mê Tung Bộ | **425** |
| 710 Mê Ảnh Tung | 120 (cấp 1) → 405 (cấp 21), vượt ngưỡng từ ~cấp 9 |

Ngưỡng 256 đặt hồi 25/08 vì sai số dự đoán chạy bộ bình thường p90 = 83 mps. Nhưng hai chiêu lướt chính **luôn vượt**.

`NormalSync` **không** nắn người chơi (`KProtocolProcess.cpp:2322` có `nIdx != CLIENT_PLAYER_INDEX`). Đường nắn duy nhất cho bản thân là ngưỡng 256 ở trên, ghi log `[S8-NAN]`.

`GetBarrier` hai bên **dùng hai nguồn khác nhau** (`KSubWorld.cpp`): server tra `m_Region[..].GetBarrier(nMapX, nMapY, nOffX>>10, nOffY>>10)`; client tra `g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY)`. Khác độ mịn → có thể dừng khác điểm.

### VIỆC PHẢI LÀM ĐẦU TIÊN: ĐO, KHÔNG VÁ

Bản đang chạy **đã có sẵn** các nhãn log. Bật AutoLog, cho một nhân vật có chiêu lướt (Đường Môn / Tiêu Dao) đánh **20-30 phút**, rồi đối chiếu theo mốc giờ:

- Client: `[S8-NAN]`
- Server: `[S4-CAST]`, `[S2-NETSKILL-IN]`, `[E4_SKILL_REJ_BUSY]`, `[E4_SKILL_COOLDOWN]`

Ba câu cần trả lời:
1. Mỗi `[S8-NAN]` lệch ~280 hoặc ~425 có **luôn thiếu** một `[S4-CAST]` tương ứng không?
2. Cửa nào đóng nhiều nhất: đang bị đánh / hồi chiêu / thiếu nội / vướng bẫy?
3. Tỉ lệ cú lướt bị lệch là bao nhiêu %?

**Không có số này thì mọi bản vá đều là đoán mò.** Phản biện đã bác cả 4 đề xuất vá vì thiếu đúng số này.

### 🚫 CẤM TUYỆT ĐỐI

**KHÔNG bỏ kiểm tra bẫy trong `DoBlurMove`** (`KNpc.cpp` case 0, `GetBarrier` + `GetTrap` mỗi bước `MIN_BLURMOVE_SPEED`). Đó là **thứ duy nhất** ngăn lướt xuyên dải bẫy, mà bẫy đang làm **vạch kích hoạt / cổng** cho: tường thành **Công Thành Chiến**, **Tống Kim**, **Bách Nhân Lôi Đài**, **Viêm Đế**, cửa map luyện. Bỏ đi = lỗ hổng ăn gian, kịch bản không chạy.

### Các đề xuất ĐÃ BỊ BÁC (đừng làm lại)

| Đề xuất | Vì sao bác |
|---|---|
| Máy chủ gửi thêm gói xác nhận vị trí sau mỗi cú lướt | Gói đó đi qua đúng cửa ngưỡng 256 → với 710 cấp thấp (120) **không bao giờ có tác dụng**; khi có tác dụng thì mở lại cửa sổ 3 giây "hai người cùng lái" — đúng hố đã lấp 28/08 |
| Bỏ kiểm tra bẫy | Lỗ hổng ăn gian, xem trên |
| Sửa chặn biên 9 vùng | Gán nhầm dòng — dòng đó kiểm vùng **xuất phát** chứ không phải **đích**; nhánh còn lại không bao giờ chạy |
| Thêm khối đổi mục tiêu thành toạ độ cho 995 | Làm **chết hẳn** chiêu 995: cửa lọc chỉ lọt khi đồng đội cách ≤ 20, mà hàm lướt lại từ chối mọi cú lướt ≤ 20 → mất nội, đứng yên |

---

## 2. VIỆC ƯU TIÊN 2 — BÓNG MỜ CÒN DÍNH SAU ĐỔI PHÁI (chưa chốt gốc)

### Chủ đo được (bằng chứng thắng mọi suy luận)

> "Hết 5 phút vẫn hiện, phải thoát hẳn game mới mất."

Đo trên bản client đã có bản vá `[VHTD 02/09x]`.

### Đã tìm được lỗi thật, nhưng CHƯA đủ giải thích

`KNpcAttribModify.cpp:1385` `WalkRunShadow` dùng **cộng dồn**:
```cpp
pNpc->m_WalkRun.nTime += pMagic->nValue[1];   // += (cộng dồn)
```
trong khi `FrozenAction` ngay trên dùng **gán**:
```cpp
pNpc->m_FrozenAction.nTime = pMagic->nValue[1];   // = (gán)
```

Và `m_WalkRun.nTime` **KHÔNG nằm trong khối reset** của `KPlayer::UpdataCurData()` (`KPlayer.cpp:2810` — khối reset chỉ đặt lại các trường `m_Current*`), mà cuối hàm gọi `ReCalcState()` → `ReCalcStateEffect()` **áp lại toàn bộ thuộc tính buff đang chạy**. `UpdataCurData` có **~50 điểm gọi** (đổi trang bị, lên cấp, đăng nhập, buff vào ra, kinh mạch...).

Đây **đúng cùng lớp lỗi** đã có ghi chú sẵn trong chính hàm đó cho hai mảng ngũ hành (xem comment `[NGUHANH 01/09]` ngay trên `ReCalcMeridian()`).

### VÌ SAO CHƯA VÁ

Khi node trạng thái hết hạn (`KNpc.cpp:1549`, `m_LeftTime == 0`), nó áp lại `m_State[i]` — mà node **lưu giá trị ĐẢO DẤU** (`KNpc.cpp:8254-8256`: `nValue[i] = -pTemp->nValue[i]`) → `nValue[1] = -5400 < 0` → nhánh else → **`m_WalkRun.nTime = 0`**.

Tức về lý thuyết **5 phút là sạch bất kể cộng dồn bao nhiêu** → mâu thuẫn với quan sát của chủ ⇒ **còn một đường nữa chưa tìm ra**. Không vá theo giả thuyết đã bị bác.

### PHÉP THỬ PHÂN ĐỊNH (rẻ nhất, làm trước)

**Nhờ một người chơi khác đứng nhìn chủ chạy:**
- Cả hai **cùng thấy** bóng mờ ⇒ lỗi **phía máy chủ** (cờ thật sự bật) → đào tiếp đường state node.
- **Chỉ mình chủ thấy** ⇒ lỗi **phía client** → đào đường `m_nBlurState` / gói đồng bộ.

Hai hướng sửa hoàn toàn khác nhau.

### Hướng đào tiếp nếu là phía máy chủ

- `KNpc.cpp:1546` `if (pTempNode->m_LeftTime == -1) continue;` = trạng thái **vĩnh viễn**, bị bỏ qua mãi mãi, **không bao giờ gỡ**. **Chưa xác minh** 1358 có rơi vào nhánh này không → kiểm đầu tiên.
- `AddStateSkillList` (`KNpc.cpp:8225-8243`): re-cast cùng kỹ năng **cùng cấp** chỉ làm mới `m_LeftTime` rồi `return`, **không** áp lại thuộc tính → re-cast **không** gây cộng dồn. Đã loại.

### Đề xuất ĐÃ BỊ BÁC

| Đề xuất | Vì sao bác |
|---|---|
| Gọi lệnh gỡ trạng thái trong hàm xoá kỹ năng | Hàm đó còn dùng để **làm mới kỹ năng theo cấp** ở ~10 chỗ, và một script gọi ~200 lần liên tiếp → thành công cụ "gỡ buff chùa", gỡ luôn buff dài hạn đã lưu DB |
| Đặt lại bộ đếm bóng mờ mỗi lần tính lại chỉ số | Node hết hạn vẫn ép về 0 → chưa chứng minh chữa được bệnh; dòng đó chạy **cả trên client** nơi biến này là **cờ nhận từ gói tin**, không phải đồng hồ → có thể gây nháy hình |
| Thêm bóng mờ vào lệnh dọn trạng thái khi chết/hồi sinh | Hàm đồng bộ đã chặn ngay đầu: chết/hồi sinh thì thoát luôn, không gửi cờ |

---

## 3. VIỆC CHỜ CHỦ QUYẾT (không tự làm)

| # | Việc | Loại | Build? |
|---|---|---|---|
| 3.1 | **`randmove` cùng lỗi cộng dồn** như bóng mờ (`KNpcAttribModify.cpp:1392`, 6 kỹ năng dùng). Chưa sửa vì chạm **thời lượng khống chế** = cân bằng | mã | có |
| 3.2 | **`skills.txt` máy chủ TRÙNG MÃ** SkillId **1561** và **1562** (dòng 1562/1602 và 1563/1603: `TOPTK1` vs `Tuý Tiên Tá Cốt` / `VIP 90`); client **chỉ trùng 1560** → hai bên bất đồng về hai kỹ năng này | dữ liệu | không |
| 3.3 | **Lực tay nội công 1382 tụt sâu?** Đợt 12 cắt +180% → +60%. Mở lại một khối (`longxuan_jianqi1` trong `huashan.lua`, bỏ `--` ở đầu 4 dòng) = **+60%**. Sửa **cả hai bản** server + client | dữ liệu | không |
| 3.4 | **Nâng 1360 Thượng Tùng Nghênh Khách?** Hiện 1196 là **đúng VLTK** (hệ số 20%). Nâng `cangsong_yingke.physicsenhance_p` là **lệch VLTK**, và bảng dùng chung **1360/1361/1362** nên cả ba cùng tăng | dữ liệu | không |
| 3.5 | **Tạp Đạp Lưu Tinh hạ chi phí Âm Luật?** Công cụ `vhtd_data_patch10_costsp.py` đã viết, **chưa chạy**. Dữ liệu ta trùng VLTK 100%, cấp 20 chỉ đòi 6 (không phải 10) | dữ liệu | không |
| 3.6 | **1363 Thái Nhạc lên 9 kiếm?** Hiện cố định 3 = **đúng VLTK**. Muốn 9 phải thêm `skill_misslenum_v` → **lệch VLTK**, sát thương gấp 3 | dữ liệu | không |
| 3.7 | **Tỷ lệ 1364 giữ hay trả chuẩn?** Ta 10% / 5 giây, VLTK 3% / 15 giây (ta **rộng hơn 3 lần**). Hoặc đổi thành nổ **khi đánh ra** (`autoreplyskill` → `autoattackskill`) = lệch thiết kế | dữ liệu | không |
| 3.8 | **995 `TargetAlly` = 1** (khác 710/732/2118 đều = 0) — cửa duy nhất cho cú lướt nhận "mục tiêu" thay vì "toạ độ". Đặt về 0 đóng cửa đó nhưng **mất khả năng nhắm đồng đội** | dữ liệu | không |
| 3.9 | **Lệch 1379 Khí Quán Trường Hồng client/server**: server cộng theo **tỷ lệ** nội lực, client chỉ cộng khi **đầy**. Vá sẽ làm ô lực tay **nhảy số liên tục theo thanh nội lực** | mã | có |

---

## 4. VIỆC CHỦ CẦN NGHIỆM THU (từ các đợt đã swap)

1. Bấm **Thần Quang Toàn Nhiễu** khi có **Ma Vân Kiếm Khí** → khí trường **mất ngay** kèm hiệu ứng nổ, không còn chờ 20 giây.
2. Bật **Huyền Nhãn Vân Yên** chạy bộ **không đánh nhau** → vệt bóng mờ **vẫn còn**; đứng yên thì hết; **chết thử** xem xác có nhả ảnh mờ không.
3. **Vệt bóng chiêu lướt** (Tạp Đạp Lưu Tinh) dày thêm đúng 1 ảnh — kiểm xem có xấu không.
4. Có **vòng sáng xanh lá dưới chân** khi buff 1358. Không thấy thì kiểm **tuỳ chọn giảm hiệu ứng** của client trước.
5. Tooltip **Đoạt Mệnh Liên Hoàn Tam Tiễn Kiếm** ghi **"Khi bị công kích có 10%..."** và **"3 kiếm"** (không còn "3/9"). Tooltip **Cửu Kiếm Hợp Nhất** vẫn giữ "Đòn đánh có..." và "N/9 kiếm".
6. **Kiếm không bay ra**: đứng cho quái đánh mình **20-30 giây, đừng đánh lại**, buff bật. Thấy kiếm bay = bình thường. Bị đánh nhiều mà vẫn không thấy = lỗi thật, báo lại.
7. **Lực tay Hoa Sơn nội** full kỹ năng full mạch ≈ **14.440** (trước 54.994).

---

## 5. LUẬT QUAN TRỌNG PHẢI NHỚ

1. **CẤM Edit/Write tool lên tệp nguồn JX1** (TCVN3/GBK một byte). Chỉ `python io.open(..., encoding="latin-1", newline="")`. `KProtocol.h` **có BOM** — giữ nguyên, chỉ chèn ASCII.
2. **Chữ Việt mới**: ghép từ **byte có sẵn** trong dữ liệu/mã, **không tự gõ**, và tự kiểm lại bằng bảng TCVN3 trước khi ghi. (Dữ liệu gốc `MagicDesc.ini` viết sai **"Khị bị công kích"** — byte `0xDE`.)
3. **Trước khi build**: `git status` — có tệp lạ đang sửa dở thì build ở `git worktree`. Đã dính 2 lần. `Lib` không nằm trong git: `cp -r D:/GAMEDEVNEW/lib/. <wt>/lib/`.
4. **`git add -A <thư mục>` quét cả tệp của phiên khác** — luôn `git diff --cached --name-only` rồi `git restore --staged` trước khi commit.
5. **`Core.cpp` biên dịch vào CẢ hai binary.** Luôn kiểm một dòng nằm trong `#ifdef _SERVER` hay `#else` — và nhớ `#else` có thể **lật nghĩa** (ví dụ `KPlayer.cpp:390` `#ifdef`, **415** `#else` → dòng 465 là mã **client**).
6. **Đổi bảng giao thức phải kiểm THEO VỊ TRÍ** — lệch ở đây **không báo lỗi biên dịch**, chỉ desync âm thầm. Bảng đánh chỉ số bằng `protocol - c2s_gameserverbegin - 1` (base = **64**).
7. **Khi thêm công tắc hiển thị, quét MỌI chỗ khác ghi cùng cờ đó** — một lệnh chạy mỗi nhịp ở hàm khác sẽ luôn thắng (bài học `FkAutoSetBlur`).
8. **Chỉ phản biện phần có nguy cơ bug** (yêu cầu của chủ), và **chỉ chạy tác tử khi thật sự cần**.
