# BÀN GIAO 03/09/2026 (chiều) — WAuto Tống Kim: RẢO MAP tìm địch, không tự lên ngựa, chặn chiêu bị từ chối

Phiên: `wauto` (Claude Fable 5.1). Chủ giao (03/09 chiều):
- *"WAuto tống kim vẫn còn ưu tiên chạy toạ độ cố định, không tìm đối thủ khác màu"* → **"tôi muốn WAuto sẽ tự di chuyển tìm đối thủ trong Tống Kim hết bản đồ luôn thay vì chạy toạ độ cố định"**.
- *"lúc di chuyển tự ý lên ngựa trong khi tab PK và tab Chiến đấu đã để xuống ngựa rồi"*.
- Kèm phát hiện của phiên `wauto-6a`: chiêu 1967 bị client từ chối 282 lần/12,5 phút (mỗi lần cắt đường chạy).

## 1. Tệp swap (md5 8 ký tự đầu) — CHỈ đổi CoreClient.dll

| Tệp | md5 | Cỡ | Ghi chú |
|---|---|---|---|
| `bin\client\CoreClient.dll.moi` | **`4c69d7ad`** | `2.521.600` | **(03/09 tối — ĐỢT 2, xem mục 6)** sửa "ra một góc rồi đứng yên, kéo đi lại chạy về": chỉ nhận ô rảo `FindPath == 1` + đo kẹt 3 s; đứng trong map 379 mà máy TK bị xoá (tắt/bật tick auto) thì vào lại ngay; cấm chiêu 30 s chỉ khi lý do lâu dài (sai vũ khí). Build từ main `91302e35` + `ReverseTools\goi_va_wauto_tk_rao_ket_0309.py`; tập cha của `a3cecb53`. Không đổi `autoData` / `ExtAuto` / IPC / gói mạng → **chỉ đổi CoreClient.dll**. |
| `bin\client\CoreClient.dll` | `a3cecb53` | `2.520.064` | **ĐANG CHẠY** (chủ swap 18:10): build từ **main `d59340c4`** (S13k của wauto-6a) + bộ vá đợt 1 → tập cha của `e10abd7a` (S13k); chuỗi "[Công Thành]" TCVN3 còn nguyên. Không đổi `autoData` / `ExtAuto` / IPC / gói mạng. |
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

**Chưa làm / còn nghi:** (a) `FindPath` trả 2 ở tầng săn (`TK_SanNguoi`) đã có "đo kẹt" riêng 3 s + 2 lần tính lại → không đổi; (b) 1967 Trấn Biên Thùy vẫn sai vũ khí (hệ Thuẫn) — chủ nên bỏ khỏi bảng kết hợp tab Chiêu KH khi cầm đao.
