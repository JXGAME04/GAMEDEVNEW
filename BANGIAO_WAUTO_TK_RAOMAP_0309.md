# BÀN GIAO 03/09/2026 (chiều) — WAuto Tống Kim: RẢO MAP tìm địch, không tự lên ngựa, chặn chiêu bị từ chối

Phiên: `wauto` (Claude Fable 5.1). Chủ giao (03/09 chiều):
- *"WAuto tống kim vẫn còn ưu tiên chạy toạ độ cố định, không tìm đối thủ khác màu"* → **"tôi muốn WAuto sẽ tự di chuyển tìm đối thủ trong Tống Kim hết bản đồ luôn thay vì chạy toạ độ cố định"**.
- *"lúc di chuyển tự ý lên ngựa trong khi tab PK và tab Chiến đấu đã để xuống ngựa rồi"*.
- Kèm phát hiện của phiên `wauto-6a`: chiêu 1967 bị client từ chối 282 lần/12,5 phút (mỗi lần cắt đường chạy).

## 1. Tệp swap (md5 8 ký tự đầu) — CHỈ đổi CoreClient.dll

| Tệp | md5 | Cỡ | Ghi chú |
|---|---|---|---|
| `bin\client\CoreClient.dll.moi` | **`5600d7a9`** | `2.525.696` | **(03/09 đêm — ĐỢT 4, xem mục 8)** 5 yêu cầu của chủ: ra trại thì tiến về **khu xuất quân địch** khi chưa thấy ai (sau điểm cuối thấy địch); **luật ngựa mới**: đi xa tự lên ngựa, gặp địch xuống, quanh có địch không lên. Build từ main `a6c6f237` + `ReverseTools\goi_va_wauto_tk_ngua_xq_0309.py`; tập cha của `59f90510`. **Chỉ đổi CoreClient.dll.** |
| `bin\client\CoreClient.dll` | `59f90510` | `2.523.648` | **ĐANG CHẠY** (chủ swap 20:51) — ĐỢT 3 (mục 7): trận chưa bắt đầu (trap chặn cổng) thì đứng chờ; tầng săn bị tường chắn thì nhắm đúng ô địch, điểm nhắm phải `FindPath == 1`, đứng yên trong tầm PK 3 s là bỏ; ô rảo tránh trap "vào trại". Bộ vá `ReverseTools\goi_va_wauto_tk_chocong_0309.py`. |
| `bin\client\CoreClient.dll.truoc` | `4c69d7ad` | `2.521.600` | đợt 2 (mục 6), chạy ~20:05 → 20:51. Đợt 1 `a3cecb53` chạy 18:10 → 20:05. |
| `bin\client\Game.exe` | `bd5cb88e` | đang chạy | KHÔNG cần đổi (không chạm S3Client). |
| `bin\client\WAuto.exe` | `46fdc93f` | đang chạy | KHÔNG cần đổi (không có ô cấu hình mới — tận dụng ô sẵn có). |
| `bin\server\CoreServer.dll.moi` | `b68899b2` | 18.298.368 | của wauto-6a (S13k máy chủ), swap theo lịch của họ; độc lập với bộ này. |

**Checklist swap (chỉ client):**
1. Thoát hẳn `Game.exe` (WAuto.exe có thể giữ).
2. Chạy `bin\client\ChoiGame.bat` → nó đổi `CoreClient.dll.moi` → `CoreClient.dll`.
3. Kiểm nhanh: md5 `CoreClient.dll` = `4c69d7ad…`; vào trận Tống Kim, `jx_auto.log` phải có dòng `[TK-RAO] dung bang o rao map=379: N o` (N ≈ vài trăm) và các dòng `[TK-RAO] o k/N -> (x,y)`; đợt 2 thêm `[TK-RAO] ket o k ... - bo o nay, chon o khac` (khi gặp ô không tới được) và `[CHIEU-CAM] ... bi tu choi TAM THOI ... khong cam`.
4. Nếu chủ muốn lùi: `CoreClient.dll.truoc` (bat tự giữ, = `a3cecb53` đợt 1) hoặc đặt lại `e10abd7a`.

## 2. Đã sửa gì (chỉ `Sources\Core\Src\CoreShell.cpp` + 2 hàm inline trong `KSubWorld.h`)

### 2.1 Tống Kim: thứ tự mục tiêu mới + rảo map (`TKP_FIGHT`, `TK_RaoDi`)
Trước: (1) địch gần nhất trong tầm PK có đường nhìn — **gồm lính NPC** vì ô "Đánh quái" bật, tướng trước nếu "Ưu tiên" = Hiệu Úy… → (2) săn người khác màu trong vùng đồng bộ → (3) **chạy tới một điểm trong bảng toạ độ sinh binh đoàn** (`g_TKBinhA/B`, `TK_ChonDiem`).

Nay:
1. Tướng địch (chỉ khi ô "Ưu tiên" tab Tống Kim = Hiệu Úy / Phó Tướng / Đại Tướng) — `TK_ChonDich(..., 1)`.
2. **Người khác màu** trong tầm PK có đường nhìn — `TK_ChonDich(..., 2)` → giao máy PK.
3. Người khác màu ở xa / khuất trong vùng đồng bộ — `TK_SanNguoi` → tầng săn đi tới (như 03/09 sáng).
4. Lính NPC địch trong tầm (ô "Đánh quái") — `TK_ChonDich(..., 0)` → giao máy PK.
5. Không thấy ai: **`TK_RaoDi`** — rảo map theo lưới A* của client:
   - Lưới `maps\<id>.fp` client nạp khi vào map (608×672 ô cho map 379, 110.393 ô đi được). `TK_RaoDung` chia thành ô rảo 20×20 ô lưới (640 mps ≈ tầm PK), mỗi ô giữ một điểm đi được gần tâm nhất; bỏ ô trong bán kính 1.440 mps quanh **hai** hậu doanh. Kết quả vài trăm ô, dựng một lần mỗi map (log `[TK-RAO] dung bang o rao`).
   - Chọn ô **chưa thăm** (hoặc thăm quá 8 phút) có điểm = khoảng cách + ⅓ khoảng cách tới hậu doanh **địch** (nghiêng về nửa map địch). Kiểm có đường bằng `SubWorld[0].FindPath`; không có đường → bỏ ô một vòng. Hạn 45 s/ô; tới nơi (< 200 mps) hoặc quá hạn → đánh dấu đã thăm; đi ngang qua ô nào (< 420 mps) cũng tính đã thăm. Hết ô → mở lại tất cả.
   - **Điểm cuối cùng thấy người khác màu** (`TK_RaoThayDich`, ghi ở tầng săn và khi giao máy PK một người) được quay lại trước, trong 3 phút.
   - Trong lúc rảo, mỗi nhịp 300–400 ms vẫn chạy lại thứ tự 1–4 nên thấy địch là bỏ rảo ngay.
   - Chưa có lưới (map chưa nạp xong) → lùi về cách cũ (bảng toạ độ).
   - Thông báo một lần/2 phút: *"Không thấy địch quanh đây - rảo map tìm địch."*

### 2.2 Không tự lên ngựa (`DT_DuocLenNgua`, `DT_WalkTo`)
Gốc: [CoreShell.cpp:3209](Sources/Core/Src/CoreShell.cpp) `DT_WalkTo` gọi `DT_Ride` mỗi lần đi đường (trừ pha farm Dã Tẩu) — máy Tống Kim / Công Thành / Liên đấu / Hoạt động / Sát thủ đều dùng chung hàm này; máy PK (ô "Xuống ngựa") và máy đánh (ô ngựa tab Chiến đấu = xuống) lại xuống mỗi 2 s → log 03/09: 91 lần đổi ngựa/229 mẫu (≈ 2,5 s/lần) giữa lúc đánh.
Nay `DT_WalkTo` chỉ lên ngựa khi: cấu hình không nói xuống (`bPKDownHorse == 0` và `nSelFHorse != 2`), không máy nào đang **trong trận** (`nTKHold/nCTHold/nLDHold/nHDHold/nSTHold != 2`), và không ở pha farm Dã Tẩu. Mỗi `*_Process` ghi con trỏ cấu hình vào `s_pApDiDuong` ở đầu nhịp. Đi đường tới NPC / Xa Phu / về thành vẫn lên ngựa như cũ khi cấu hình cho phép.

### 2.3 Chặn chiêu client sẽ từ chối (`WA_ChieuBiCam`)
Đo bảng `settings\skills.txt`: **1967 Trấn Biên Thùy = Vũ Hồn hệ THUẪN, `EqtLimit` 8**; 1977/1985 (hệ Đao) `EqtLimit` 7. Nhân vật cầm đao → `KSkill::CanCastSkill` từ chối 1967 vì sai vũ khí (không phải chiêu tự thân như dự đoán — `TargetSelf 0`, `TargetEnemy 1`). Máy PK/đánh vẫn gửi mỗi vòng bảng kết hợp → 282 lần/12,5 phút, mỗi lần client `Exit → DoStand` cắt đường chạy.
Nay trước khi gửi chiêu nhắm mục tiêu (3 chỗ: máy PK 2 nhánh, máy đánh thường), hỏi trước bằng đúng `pSkill->CanCastSkill(mình, -1, mục tiêu)`; từ chối → cấm chiêu đó 30 giây (`[CHIEU-CAM]` trong log), coi như "đã bắn" để bảng kết hợp sang khe kế; hết hạn thử lại (đổi vũ khí / xuống ngựa là dùng được). Không đổi cấu hình của chủ.

### 2.4 `KSubWorld.h` (client)
Thêm 2 hàm inline chỉ đọc: `LuoiOCoDiDuoc(mpsX, mpsY)` (1 đi được / 0 vật cản / −1 chưa có lưới) và `LuoiPhamViMps(x0,y0,x1,y1)`. Không thêm trường → bố cục lớp không đổi, Game.exe không cần build lại.

## 3. Bộ vá idempotent
`ReverseTools\goi_va_wauto_tk_raomap_0309.py [--thu]` — áp lại được nhiều lần, tự bỏ qua hunk đã có. Tệp nguồn TCVN3 → đọc/ghi latin-1.

## 4. Cách test
1. Bật auto Tống Kim + tab PK "Đánh người" (và "Đánh quái" nếu muốn đánh lính), "Xuống ngựa" tuỳ ý.
2. Trong trận: không thấy địch → nhân vật đi lần lượt các vùng chưa qua, nghiêng về phía địch, không vào hậu doanh; thấy người khác màu → lao vào; chết hồi sinh → quay lại chỗ cuối thấy địch trước.
3. Log cần có: `[TK-RAO] dung bang o rao`, `[TK-RAO] o k/N -> (x,y)`, `[TK-RAO] ve diem cuoi thay dich`, `[TK-SAN] san id=`, `[CHIEU-CAM]` (nếu bảng kết hợp có chiêu sai vũ khí). Không còn `ngua=` đổi liên tục trong `ME-FIGHTMODE` khi đã đặt xuống ngựa.

## 5. Bẫy / lưu ý
1. Rảo map dựa vào lưới `.fp` client: map nào chưa có lưới thì máy lùi về bảng toạ độ cũ (log không có `[TK-RAO]`).
2. `TK_SAN_CAMTRAI` 1.440 mps quanh hậu doanh dùng chung cho cả cấm săn lẫn cấm rảo.
3. `WA_ChieuBiCam` gọi `CanCastSkill` của client nên khi sai vũ khí sẽ hiện 1 thông báo hệ thống mỗi 30 s (trước đây hiện mỗi lần gửi, ~23 lần/phút).
4. Tính năng **"Tìm ac chính / Đánh cùng mục tiêu ac chính"** (auto Thái) là việc KHÁC, chưa làm — xem phân tích trong memory `jx1-thailan-tim-acchinh-cochec-0309`.

## 6. ĐỢT 2 (03/09 tối) — chủ báo *"vào tống kim WAuto di chuyển ra 1 góc rồi đứng yên - kéo ra vị trí khác thì chạy về lại góc đó"* → `CoreClient.dll.moi 4c69d7ad`

Đo `bin\client\jx_auto.log`, phiên pid 31128 (19:29–19:36, chạy đúng bản `a3cecb53`). Ba gốc, cả ba đều làm nhân vật đứng yên trong trận:

| # | Gốc (tệp:dòng) | Bằng chứng trong log | Sửa (chỉ `CoreShell.cpp`) |
|---|---|---|---|
| 1 | **`KSubWorld::FindPath` trả `2` khi đích KHÔNG tới được** — đi tới ô gần nhất rồi ghi `m_nTargetX/Y` = ô trung gian (`KSubWorld.cpp` `FindPath_Block` "goal unreachable" + 1022-1032; cùng bẫy `BANGIAO_HAUCAN_DUNGYEN_0209`). `TK_RaoDi` bước (d) chỉ loại `<= 0` nên nhận ô này; nhân vật tới ô gần nhất, dừng cách đích 290–410 mps (> ngưỡng 200 "tới nơi"), đứng chờ hết hạn 45 s; `DT_WalkTo` cứ 2,5 s tính lại đường về đúng ô trung gian → **kéo đi là bị kéo về**. | ô 678 → dừng (44507,109754) cách đích 287, đứng 20 s; ô 619 → cách 338, đứng 35 s; ô 589 → cách 411; ô mới chỉ được chọn đúng mỗi 45 s (+126,2 → +171,6 → +216,6 s) | (d) chỉ nhận ô `FindPath == 1`, thử 6 ô/nhịp; (c) và (b) thêm **đo kẹt `TK_RaoKet`**: đứng yên (< 32 mps) quá 3 s → đánh dấu đã thăm, chọn ô khác ngay, log `[TK-RAO] ket o k`; hết 6 ô vẫn `return 1` (rảo tiếp nhịp sau), **không** lùi về bảng toạ độ cố định. |
| 2 | **Tắt/bật tick auto ở dòng nhân vật trong WAuto** → `PRT_TICKSTART` (`WAuto.cpp:4806`) → `S3Client.cpp:1288` `ATYPE_CLEAR` → `CoreShell.cpp:16358` `memset(ExtAuto)` → `nTKPhase = TKP_OFF`; cửa vào cuộc đòi khung giờ hoặc loa, mà loa đã trôi và giờ TK không cấu hình (đang test) → **nTK = 0 tới hết trận**, nhân vật đứng yên (map sự kiện chặn mọi auto tự do). | `[AUTO-PASS]` ngắt 13,4 s lúc +222,9 s (chủ thao tác WAuto), rồi `[HD-GATE] nTK 2 → 0` lúc 19:34 và giữ 0 tới hết phiên; sau hồi sinh đứng ở hậu doanh (39886,113455) 10 s cho tới khi chủ kéo tay | `TK_Process`, khối "quyết định vào cuộc": **đứng trong map 379 và pha OFF → vào lại ngay** (không cần giờ/loa; cùng luật `TKP_GO` "vào lại game giữa trận"), thông báo `Đang đứng trong trận Tống Kim - vào lại đánh tiếp.` Không áp cho pha DONE (bỏ cuộc sau 3 phút kẹt — giữ quyết định cũ). **Chủ duyệt**: nếu không muốn máy tự vào lại thì báo, tôi gỡ 1 điều kiện. |
| 3 | **`WA_ChieuBiCam` cấm 30 s vì lý do TẠM THỜI** — `KSkill::CanCastSkill` trả 0 cho cả lý do theo mục tiêu (`SKILL-REFUSE-FIGHTMODE`: người chơi vừa được thêm, FightMode chưa đồng bộ; quan hệ) và tạm thời (im lặng, ngựa), không chỉ sai vũ khí. | +68 s (vừa ra khỏi trại): 1977 → `[SKILL-REFUSE-WEAPON] wpn_particular=7 yeucau=7` (vũ khí ĐÚNG) vẫn bị từ chối → 1977/1985/1967 cấm cùng lúc → đứng cạnh 93066 (152 mps) **30 s không đánh**; +98 s chỉ 1967 bị cấm lại (sai vũ khí thật) | trước khi cấm, kiểm 4 lý do tạm thời (im lặng / giới hạn ngựa / FightMode mục tiêu khác / quan hệ chưa là địch) → chỉ **bỏ lượt này, không cấm**, log `[CHIEU-CAM] ... bi tu choi TAM THOI (ly do n)`; sai vũ khí vẫn cấm 30 s như đợt 1. |

Bộ vá: `ReverseTools\goi_va_wauto_tk_rao_ket_0309.py [--thu] [--root ...]` (idempotent, latin-1). Build: `MSBuild Core.vcxproj -p:Configuration="Client Release" -p:Platform=Win32 -p:SolutionDir=D:\GAMEDEVNEW\Sources\ -p:PostBuildEventUseInBuild=false` → `Sources\Core\ClientRelease\CoreClient.dll` = `4c69d7ad`.

**Swap: chỉ `CoreClient.dll.moi`** (thoát Game.exe → `ChoiGame.bat`). Game.exe `71dae629`, WAuto.exe `46fdc93f`, CoreServer `b68899b2` giữ nguyên.

**Nghiệm thu:** vào trận, không thấy địch → nhân vật đi hết ô này sang ô khác không dừng quá 3 s giữa đường; log có `[TK-RAO] o k/N -> ...` cách nhau vài chục giây và, khi gặp ô kín, `[TK-RAO] ket o k ... - bo o nay, chon o khac` ngay sau 3 s (không còn chuỗi 45 s). Tắt rồi bật tick auto giữa trận → kênh `[Tống Kim]` báo *Đang đứng trong trận Tống Kim - vào lại đánh tiếp* và `[HD-GATE] nTK=2` trở lại trong 1 s. Vừa ra trại gặp địch → không còn `[CHIEU-CAM] ... cam 30 giay` cho 1977/1985, chỉ còn dòng `TAM THOI` (nếu có) rồi đánh ngay khi đồng bộ xong.

**Chưa làm / còn nghi:** (a) `FindPath` trả 2 ở tầng săn → **đã sửa ở đợt 3** (mục 7); (b) 1967 Trấn Biên Thùy vẫn sai vũ khí (hệ Thuẫn) — chủ nên bỏ khỏi bảng kết hợp tab Chiêu KH khi cầm đao.

## 7. ĐỢT 3 (03/09 tối, sau swap đợt 2) — chủ báo *"di chuyển chạy lạng qua lạng về"* và *"phần tự tìm đối tượng khác phe (màu tên) để tới đánh vẫn còn chạy vào góc"* → `CoreClient.dll.moi 59f90510`

Đo `jx_auto.log` phiên pid 23876 (20:15–20:24) và pid 62776 (20:36–20:45, đang chạy `4c69d7ad`). **Máy rảo đã chạy** (phiên 23876: dựng 1024 ô, +486→+507 s đi 12 ô liền; 9 lần săn được người khác màu rồi lao vào đánh). Ba việc mới:

| # | Hiện tượng chủ thấy | Gốc (tệp:dòng) | Sửa |
|---|---|---|---|
| 1 | **"lạng qua lạng về"**: +112→+143 s chạy Tây Nam ~6 s rồi bị ném về đúng (50.272, 102.272), lặp 4 lần (`[S6-ME] nhanh=loadmap svmps=(50272,102272)`), tới +143 s mới đi tiếp | `lib_tktc.lua:655-661` đặt 3 vết trap `kim_chancong.lua` + 3 vết `tong_chancong.lua` = *"Chặn cổng 3 trap khi chưa bắt đầu"*: còn hẹn giờ 1 (`timerserver.lua:620` = `TIME_BD_TK` phút báo danh) thì ai qua cổng bị `SetPos(1571,3196)`/`(1332,3443)` + Talk *"Đại chiến Tống Kim còn N giây sẽ bắt đầu"*; hết giờ: `task01.lua:62` Talk *"Tống Kim Đại chiến chính thức bắt đầu!"*. Auto ra khỏi trại rồi rảo ngay khi còn đếm ngược. | `TK_Process` đọc thoại Talk (`g_sDTCap.szTalk`): *"còn N giây"* → `s_uTKChoBatDau`, đứng chờ (không rảo/săn, vẫn ăn thuốc, máy PK vẫn đánh trả), báo *"Trận chưa bắt đầu - còn N giây, đứng chờ trước cổng."*; thoại *"chính thức bắt đầu"* → đi ngay. Dự phòng không bắt được thoại: nhảy ≥ 1.500 mps trong một nhịp (không chết) và rơi đúng điểm SetPos chặn cổng → chờ 20 s. Hai câu thoại + toạ độ trích **byte-exact từ script máy chủ** lúc chạy tool. |
| 2 | **"chạy vào góc"** (tầng săn): +54→+72 s phiên 62776: săn 92686, tới điểm nhắm cách địch **164 mps** rồi đứng sát tường **7 s** (`repath 0→1→2`), địch chạy mất mới thôi | `TK_ChonDich` chỉ giao máy PK khi có **đường nhìn** (`TK_ThayDuoc`); tầng săn nhắm điểm lùi 3 ô trên **đường thẳng** → bị tường chắn thì điểm đó nằm ngay chân tường, `FindPath` trả 2 (cùng bẫy đợt 2), đứng yên; "đo kẹt" tính lại đường 2 lần vô ích | `TK_SanNguoi`: đường thẳng bị chắn → nhắm **đúng ô địch** để A* client đi vòng; mỗi lần nhắm lại đòi `FindPath == 1` (điểm lùi không được thì thử ô địch; vẫn không → loại 20 s `[TK-SAN-BO] khong co duong`); đứng yên 3 s **trong tầm PK** → bỏ ngay, cấm 20 s (ngoài tầm vẫn 2 lần tính lại + cấm 60 s như cũ). |
| 3 | (phòng ngừa) | Trap `kimvaotrai`/`tongvaotrai` (1591,3162)/(1289,3480) + 10 ô chéo: phe mình giẫm vào là bị kéo vào hậu doanh, đồng hồ 90 s bật lại → ngồi ~80 s mới ra được; ô rảo chưa tránh | `TK_RaoDung` loại ô rảo trong 480 mps quanh tâm hai vết trap này. |

Bộ vá: `ReverseTools\goi_va_wauto_tk_chocong_0309.py [--thu] [--root ...] [--srv <bin\server\script>]`. **Không sinh lại `KTongKimTables.h`**: `TAB_TIME_TONG_KIM` trong lua đã đổi dạng `TK_CFG("TK_LICH", {...})`, bộ sinh cũ đọc không ra và bảng `g_TKGio` trong .h ({13,58}…) đã khác lua ({23,46}…) — sinh lại sẽ đổi giờ mặc định của tab Tống Kim. Việc nợ: sửa `gen_tongkim_tables.py` đọc dạng mới rồi chuyển các marker/toạ độ đợt 3 vào .h.

**Swap: chỉ `CoreClient.dll.moi`** (thoát Game.exe → `ChoiGame.bat`). **Nghiệm thu:** vào trận lúc còn báo danh → kênh `[Tống Kim]` báo *Trận chưa bắt đầu - còn N giây…* đúng một lần và nhân vật đứng trước cổng, không còn bị ném về lặp; đến giờ → *Trận đã bắt đầu - xuất quân*. Log: `[TK-CHO] thoai chan cong…`, `[TK-CHO] thoai: tran da bat dau`, `[TK-SAN-BO] khong co duong toi id=…` (địch trong vùng kín), không còn `[TK-SAN] … xa=1xx … repath=2` kéo dài.

**Còn phải hỏi chủ:** phiên 62776 chết **12 lần / 9 phút**: mỗi lần hồi sinh xong auto chạy thẳng ~4.000 mps về *"điểm cuối thấy địch"* (luật đợt chiều) = giữa đám địch, sống 5–15 s. Có nên đổi luật (đứng gần cổng mình chờ địch tới / chỉ lao vào khi thấy ≤ N địch) không? Chưa đổi vì thuộc chiến thuật của chủ. → **Chủ trả lời 21:0x bằng 5 yêu cầu (mục 8): giữ hướng "ra trại là tìm địch để đánh".**

## 8. ĐỢT 4 (03/09 đêm) — 5 yêu cầu của chủ → `CoreClient.dll.moi 5600d7a9`

Nguyên văn chủ: *(1) khi ra khỏi doanh trại thì Auto sẽ xác định vị trí của địch để di chuyển tới đánh; (2) trên đường di chuyển gặp địch thì dừng lại để đánh; (3) nếu đối tượng được xác định lúc đầu đã chết thì đổi qua đối tượng khác; (4) di chuyển đường xa, lúc chết về thì phải tự lên ngựa, xuống ngựa chỉ khi gặp địch; (5) đang dưới ngựa mà xung quanh có địch thì không được lên ngựa, chỉ lên ngựa khi di chuyển xa.*

| Yêu cầu | Đã có sẵn | Làm thêm ở đợt 4 (chỉ `CoreShell.cpp`) |
|---|---|---|
| (1) xác định vị trí địch | Client **chỉ thấy** địch trong vùng đồng bộ (~40 ô); thứ tự `TKP_FIGHT`: tướng → người khác màu trong tầm PK → săn người trong vùng đồng bộ → lính → "điểm cuối thấy địch" (3 phút) → rảo | `TK_RaoDi (b2)`: chưa thấy ai và không còn điểm cuối thấy địch → **mỗi lượt ra trận đi thẳng một lần tới khu xuất quân của địch** (8 điểm trap ra trại bên kia, `g_TKXuatQuanA/B` đã có trong `KTongKimTables.h`, chọn điểm gần nhất có `FindPath == 1`); tới nơi (< 480 mps) / 90 s / kẹt 3 s → rảo quanh đó. Log `[TK-XQ]`, báo *"Không thấy địch - tiến về khu xuất quân của địch."* |
| (2) gặp địch trên đường thì đánh | Có: tầng săn/máy PK chạy mỗi nhịp trước rảo, thấy địch là cướp quyền | — |
| (3) mục tiêu chết thì đổi | Có: `TK_SanNguoi` bỏ mục tiêu chết mỗi nhịp; `TK_ChonDich` bỏ `do_death` | — |
| (4)(5) ngựa | Đợt chiều: ô "Xuống ngựa" tab PK / ô ngựa tab Chiến đấu = xuống ⇒ **không bao giờ** tự lên; máy trong trận không lên | **Đổi luật `DT_DuocLenNgua`** (mọi máy đi đường qua `DT_WalkTo`): lên ngựa khi đường còn ≥ 480 mps, không ôm mục tiêu, **không có địch** (quan hệ enemy, còn sống; NPC chỉ tính khi trong trận TK và bật "Đánh quái") trong **Tầm nhìn PK + 400 mps**, và đã ≥ 6 s từ lần xuống. Hai ô cấu hình "xuống ngựa" giờ chỉ nói về lúc đánh. Pha farm Dã Tẩu vẫn không lên. Máy TK **xuống ngựa khi gặp địch** (`TK_XuongNgua`): giao mục tiêu cho máy PK, địch săn vào Tầm nhìn PK + 400, hay máy PK đang ôm mục tiêu. Log `[TK-NGUA]`. |

Ngưỡng do tôi chọn (chủ đổi được bằng cách nói số): "đường xa" = 480 mps (15 ô); "xung quanh có địch" = ô *Tầm nhìn PK* của tab PK + 400 mps; nghỉ 6 s sau khi xuống. Bộ vá: `ReverseTools\goi_va_wauto_tk_ngua_xq_0309.py [--thu] [--root]`.

**Lưu ý:** máy đánh thường (tab Chiến đấu, ô ngựa = "tự động") vẫn tự **lên** ngựa khi chiêu cho phép cưỡi (`[FIGHT-HORSE] TOGGLE`) — muốn đúng luật "quanh có địch không lên ngựa" thì đặt ô đó = *Xuống ngựa*. **Nghiệm thu:** ra trại → `[Tống Kim] Không thấy địch - tiến về khu xuất quân của địch` → lên ngựa chạy → thấy địch trong tầm → `[TK-NGUA] gap dich - xuong ngua` → đánh; hết địch, đường xa → lên lại sau ≥ 6 s.
