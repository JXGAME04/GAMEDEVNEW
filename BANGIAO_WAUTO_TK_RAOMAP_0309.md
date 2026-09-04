# BÀN GIAO 03/09/2026 (chiều) — WAuto Tống Kim: RẢO MAP tìm địch, không tự lên ngựa, chặn chiêu bị từ chối

Phiên: `wauto` (Claude Fable 5.1). Chủ giao (03/09 chiều):
- *"WAuto tống kim vẫn còn ưu tiên chạy toạ độ cố định, không tìm đối thủ khác màu"* → **"tôi muốn WAuto sẽ tự di chuyển tìm đối thủ trong Tống Kim hết bản đồ luôn thay vì chạy toạ độ cố định"**.
- *"lúc di chuyển tự ý lên ngựa trong khi tab PK và tab Chiến đấu đã để xuống ngựa rồi"*.
- Kèm phát hiện của phiên `wauto-6a`: chiêu 1967 bị client từ chối 282 lần/12,5 phút (mỗi lần cắt đường chạy).

## 1. Tệp swap (md5 8 ký tự đầu) — CHỈ đổi CoreClient.dll

| Tệp | md5 | Cỡ | Ghi chú |
|---|---|---|---|
| `bin\client\CoreClient.dll.moi` | `a3cecb53` | `2.520.064` | build từ **main `d59340c4`** (S13k của wauto-6a) + bộ vá này → tập cha của `e10abd7a` (S13k) mà wauto-6a vừa đặt; chuỗi "[Công Thành]" TCVN3 còn nguyên. Không đổi `autoData` / `ExtAuto` / IPC / gói mạng. |
| `bin\client\Game.exe` | `bd5cb88e` | đang chạy | KHÔNG cần đổi (không chạm S3Client). |
| `bin\client\WAuto.exe` | `46fdc93f` | đang chạy | KHÔNG cần đổi (không có ô cấu hình mới — tận dụng ô sẵn có). |
| `bin\server\CoreServer.dll.moi` | `b68899b2` | 18.298.368 | của wauto-6a (S13k máy chủ), swap theo lịch của họ; độc lập với bộ này. |

**Checklist swap (chỉ client):**
1. Thoát hẳn `Game.exe` (WAuto.exe có thể giữ).
2. Chạy `bin\client\ChoiGame.bat` → nó đổi `CoreClient.dll.moi` → `CoreClient.dll`.
3. Kiểm nhanh: md5 `CoreClient.dll` = `a3cecb53…`; vào trận Tống Kim, `jx_auto.log` phải có dòng `[TK-RAO] dung bang o rao map=379: N o` (N ≈ vài trăm) và các dòng `[TK-RAO] o k/N -> (x,y)`.
4. Nếu chủ muốn lùi: `CoreClient.dll.truoc` (bat tự giữ) hoặc đặt lại `e10abd7a`.

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
