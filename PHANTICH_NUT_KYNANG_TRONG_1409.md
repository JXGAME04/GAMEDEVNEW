# PHÂN TÍCH 14/09 — Ô KỸ NĂNG TRỐNG TỰ ĐIỀN "KỸ NĂNG ẢO" SAU KHI CHUYỂN PHÁI

> Chủ báo: *"các nút trống chưa để kỹ năng, khi chuyển phái nhận kỹ năng thì tự add kỹ năng ảo vào các nút kỹ năng
> trống"*. Phân tích ở mục 1-5; chủ duyệt **PA A + dọn nút chết** nên **mục 6 = đã thi công** (một tệp, chỉ mobile).
>
> Nhánh soi: `claude/skill-button-empty-check-8a9f74`, gốc `mobile-0809` @ `c41ffdbd`. Mã trong worktree này và trong
> `D:\GAMEDEVNEW_wt_mobile` **giống hệt nhau** (`diff` sạch).

---

## 1. Kết luận một câu

Ô kỹ năng phụ trên điện thoại **không lưu trạng thái "trống"**. Ô nào người chơi chưa tự gán thì mỗi 2 giây nó **lấy
đại phần tử thứ `i` của danh sách kỹ năng đánh trái đang sống** để hiện. Chuyển phái = danh sách đó bị thay sạch và dài
ra, nên các ô đang trống lập tức "mọc" kỹ năng mà người chơi không hề đặt.

Gốc lỗi: [JxCanDieuKhien.cpp:965](Sources/S3Client/Platform/JxCanDieuKhien.cpp:965) — nhánh dự phòng trong
`KyNang_CuaNut`.

---

## 2. Cơ chế — đọc theo mã

### 2.1 Ô phụ lấy kỹ năng ở đâu

`KyNang_CuaNut(nNut)` ([JxCanDieuKhien.cpp:946](Sources/S3Client/Platform/JxCanDieuKhien.cpp:946)) quyết định ô phụ
`nNut` (1..8) hiện kỹ năng nào, theo đúng ba bậc:

| Bậc | Điều kiện | Hành vi | Dòng |
|---|---|---|---|
| 1 | `s_KNGan[nNut-1].uId != 0` — người chơi **tự gán** | dùng đúng kỹ năng đó | 955–959 |
| 2 | `s_nKNTrong[nNut-1] == 1` — người chơi **tự gỡ** | ô trống thật | 960–964 |
| 3 | còn lại (**chưa bao giờ đụng vào**) | **lấy `s_KNBang[nNut-1]`** = phần tử thứ `nNut-1` của danh sách sống | **965–973** |

Bậc 3 chính là chỗ sinh "kỹ năng ảo". Nó không phải một phép gán — không ghi vào tệp, không có ai xác nhận — chỉ là
**ánh xạ theo vị trí** vào một danh sách thay đổi liên tục.

### 2.2 Danh sách sống đó là gì

`KyNang_DocBang()` ([JxCanDieuKhien.cpp:909](Sources/S3Client/Platform/JxCanDieuKhien.cpp:909)) gọi lại **mỗi
`KYNANG_LAM_MOI_MS` = 2000 ms** ([dòng 85](Sources/S3Client/Platform/JxCanDieuKhien.cpp:85)):

```
s_nKNCo1 = g_pCoreShell->GetGameData(GDI_LEFT_ENABLE_SKILLS, &s_KNBang, 0);
```

→ `KSkillList::GetLeftSkillSortList` ([KSkillList.cpp:700](Sources/Core/Src/KSkillList.cpp:700)):

- **Chỉ số 0** = `GetCurActiveWeaponSkill()` (kỹ năng vũ khí đang cầm) → nên **ô phụ 1 mặc định trùng ô chính**.
- Từ chỉ số 1: duyệt `m_Skills[1..MAX_NPCSKILL]` **theo thứ tự ô trong mảng**, nhận kỹ năng có `SkillLevel > 0`, đủ
  cấp yêu cầu, thuộc loại đánh trái.

Hai hệ quả:

1. Thứ tự danh sách = thứ tự **ô trống đầu tiên** mà `KSkillList::Add`/`FindFree` cấp cho kỹ năng — hoàn toàn là chi
   tiết nội bộ, không phải ý người chơi.
2. Chỉ cần **một** kỹ năng lên cấp 0 → 1, hoặc đổi vũ khí, là cả dãy dịch một bậc → **mọi ô chưa gán đổi kỹ năng cùng lúc**.

### 2.3 Vì sao đúng lúc chuyển phái mới lộ rõ

Kịch bản chuyển phái sống là `vatpham/lenhbaitanthu.lua`:

- [`lenhbaitanthu.lua:1115`](serverscript_live/script/vatpham/lenhbaitanthu.lua:1115) `DelAllMagic()` →
  `LuaDelAllMagic` ([ScriptFuns.cpp:8148](Sources/Core/Src/ScriptFuns.cpp:8148)) gọi `RemoveAllSkill()` +
  `SendSyncData_Skill()`; client nhận `s2c_synccurplayerskill` và **`m_SkillList.Clear()`**
  ([KProtocolProcess.cpp:3362](Sources/Core/Src/KProtocolProcess.cpp:3362)) → danh sách rỗng trong tích tắc.
- Rồi `hockynang(nCurFac)` ([hocvocong.lua:880](serverscript_live/script/global/hocvocong.lua:880)) →
  `show_kynang90` ([hocvocong.lua:893](serverscript_live/script/global/hocvocong.lua:893)) cấp **kỹ năng 90/120/150 ở
  cấp 20** và khinh công 210 cấp 1 (các kỹ năng SKILLNORMAL cấp 0 thì không vào danh sách đánh trái).

Tức là sau một lượt chuyển phái, số kỹ năng đánh trái **cấp > 0** nhảy từ vài cái lên 5–7 cái ngay lập tức. Trong vòng
≤ 2 giây `KyNang_DocBang` đọc lại, `s_nKNCo1` tăng, và **mọi ô phụ chưa gán đều có kỹ năng**. Đúng như chủ mô tả.

### 2.4 Đây là lần tái phát, không phải lỗi mới

Ngày 11/09 chủ đã báo cùng một họ lỗi: *"gỡ kỹ năng là nó tự add kỹ năng khác vào ô đã gỡ"*. Bản vá `[ANDROID 11/09
OTRONG]` thêm cờ `Trong<i>=1` — nhưng **chỉ chặn ô người chơi đã gỡ**. Ô **chưa từng đụng vào** vẫn rơi vào bậc 3.
Bản vá đó chữa triệu chứng, chưa chữa gốc.

---

## 3. Loại thứ hai: ô ĐÃ gán giữ kỹ năng phái cũ (kỹ năng chết)

Chạy song song với lỗi trên, và cũng đáng gọi là "ảo":

- Bảng gán nằm ở `UserData\KyNangMobile_<mã nhân vật>.ini`
  ([JxCanDieuKhien.cpp:657](Sources/S3Client/Platform/JxCanDieuKhien.cpp:657)). Chuyển phái **không đổi mã nhân vật**
  → tệp giữ nguyên.
- `KyNang_CuaNut` bậc 1 **không hề đối chiếu** `s_KNGan[i]` với danh sách kỹ năng hiện có (đã grep toàn tệp: `s_KNGan`
  chỉ được đọc/ghi, không có chỗ nào kiểm tra tính hợp lệ).
- Biểu tượng vẫn vẽ bình thường: `CoreDrawGameObj` lấy ảnh từ `g_SkillManager` chứ không kiểm sở hữu; `FindSame` chỉ
  dùng để phủ lớp hồi chiêu ([CoreDrawGameObj.cpp:212](Sources/Core/Src/CoreDrawGameObj.cpp:212)).
- `GDI_KYNANG_MOBILE` cũng trả tên/tầm/aura từ `g_SkillManager`, không kiểm sở hữu
  ([CoreShell.cpp:2899](Sources/Core/Src/CoreShell.cpp:2899)).
- Bấm vào thì `LockSomeoneUseSkill` → `FindSame(nSkillID)` trả 0 → `SetActiveSkill(0)`
  ([CoreShell.cpp](Sources/Core/Src/CoreShell.cpp)) → **nút nhìn sống nhưng ra đòn thường**.

Nên sau chuyển phái, các ô người chơi từng gán cho phái cũ thành **nút chết trông như còn dùng được**.

(Riêng **ô chính** an toàn: `KPlayer::SetLeftSkill` chặn `nLevel <= 0` nên lệnh đặt kỹ năng phái cũ bị bỏ lặng lẽ,
ô chính luôn soi đúng kỹ năng đánh trái thật của Core.)

---

## 4. Bản PC làm ĐÚNG — đây là mã tham chiếu sẵn có

`KUiSkillTree` (bản PC) không có bậc 3, và **tự dọn ô không hợp lệ**:

```
KUiSkillTree::UpdateData()                       // UiSkillTree.cpp:252
  đọc GDI_LEFT/RIGHT_ENABLE_SKILLS
  với mỗi ô tắt: nếu kỹ năng của ô KHÔNG có trong danh sách
      ms_ShortcutSkills[i].uGenre = CGOG_NOTHING;   // dòng 272-273
```

Cùng phép kiểm đó còn lặp ở các dòng 116, 146, 429, 490. Nghĩa là trên PC: ô trống là trống thật, và chuyển phái xong
ô kỹ năng phái cũ **tự rỗng**. Bản mobile khi viết lại (`JxCanDieuKhien.cpp`, toàn tệp nằm trong `#ifdef JX_MOBILE`)
đã bỏ mất phép kiểm này và thêm bậc 3.

→ Sửa bên mobile **không đụng gì tới PC**, đúng luật chủ đặt 11/09.

---

## 5. Tác dụng phụ đang xảy ra (không chỉ là nhìn xấu)

`KyNang_LuanChuyen()` ([JxCanDieuKhien.cpp:1473](Sources/S3Client/Platform/JxCanDieuKhien.cpp:1473)) quét **cả 8 ô**
qua `KyNang_CuaNut`, gom ô nào là vòng sáng rồi **tự bật luân phiên** bằng `GOI_SET_IMMDIA_SKILL`. Vì ô trống bị điền
tự động, các vòng sáng của **phái mới mà người chơi chưa hề gán** sẽ bị game tự bật/đổi liên tục ngay sau khi chuyển
phái.

Một điểm lệch nhỏ cùng họ: nút **"Gán ô phụ"** trong bảng chọn tìm ô trống bằng `s_KNGan[i].uId == 0`
([JxCanDieuKhien.cpp:1766](Sources/S3Client/Platform/JxCanDieuKhien.cpp:1766)) — không xét `s_nKNTrong` cũng không xét
ô đang hiện kỹ năng tự điền, nên nó có thể "gán vào ô trống" mà trên màn ô đó đang có biểu tượng.

---

## 6. Đã thi công (chủ duyệt 14/09: **PA A + dọn nút chết theo cách bản PC**)

Chỉ sửa **một tệp**: `Sources/S3Client/Platform/JxCanDieuKhien.cpp` — cả tệp nằm trong `#ifdef JX_MOBILE`,
nên **bản PC không đổi một byte**. Thẻ mã: `[KYNANG 14/09 TRONG]`.

### 6.1 Ô trống là trống thật

- `KyNang_CuaNut`: **bỏ hẳn bậc 3**. Chưa gán và không phải ô vừa gỡ → trả về trống. Từ đây chuyển phái,
  lên cấp, học kỹ năng mới, đổi vũ khí đều **không thể** đẩy kỹ năng vào ô người chơi để trống.
- `KyNang_DungMacDinh()` (mới): nhân vật **chưa có tệp** `UserData\KyNangMobile_<mã>.ini` thì chụp danh sách
  kỹ năng đánh hiện có thành **hàng thật** `O0..O7` rồi khoá lại bằng khoá mới `Dung=1`. Chụp đúng ánh xạ cũ
  (ô phụ `n` ← mục `n-1` của danh sách) nên nhân vật mới thấy y như trước, chỉ khác là **từ đó nó đứng yên**.
  Chốt an toàn: chỉ chụp khi đã vào thế giới, có mã nhân vật, và `s_nKNCo1 > 1` (giữa lượt chuyển phái danh
  sách có lúc chỉ còn kỹ năng vũ khí).
- Nhân vật **đã có tệp** thì **không chụp** — ô nào họ chưa đặt là trống thật (đúng lựa chọn của chủ).

### 6.2 Dọn ô giữ kỹ năng không còn học

`KyNang_DonOChet()` (mới), chạy trong `KyNang_DocBang` (2 giây/lần), làm đúng như `KUiSkillTree::UpdateData`
của bản PC nhưng chặn ba cái bẫy đã tìm ra khi soi:

| Bẫy | Nếu không chặn | Cách chặn |
|---|---|---|
| **uGenre lệch** — bảng kỹ năng đưa `CGOG_SKILL_FIGHT`, danh sách trái/phải đưa `CGOG_SKILL_SHORTCUT` | so cả uGenre → **xoá sạch mọi ô người chơi tự gán** | `KyNang_CoTrongBang` chỉ so **mã kỹ năng** |
| **Vòng sáng là `RightOnlySkill`** nên không nằm trong danh sách đánh trái | **xoá mọi ô vòng sáng** | đối chiếu **cả ba bảng**: trái + phải (`GDI_RIGHT_ENABLE_SKILLS`) + toàn bộ kỹ năng đánh (`GDI_FIGHT_SKILLS`, có cả kỹ năng cấp 0 mà người chơi vẫn gán được từ bảng) |
| **`DelAllMagic` làm danh sách RỖNG trong tích tắc** giữa lượt chuyển phái | dọn ngay lúc đó = **xoá sạch bảng gán** | bỏ qua khi `s_nKNCo1 <= 1`; và phải vắng mặt **2 lần đọc liên tiếp** (`KYNANG_VANG_DU`, ≥ 4 giây) mới xoá |

Ô bị dọn được đặt `Trong<i>=1` và ghi tệp; bộ đệm vòng sáng (`s_uKNAuraId`) tính lại.
Hai bảng phụ **chỉ đọc khi cần** (có ô vắng mặt ở danh sách trái) — trường hợp thường không tốn thêm gì.

### 6.3 Ăn theo, không phải sửa thêm

- **Vòng sáng tự bật**: `KyNang_LuanChuyen` quét qua `KyNang_CuaNut` nên nay chỉ luân chuyển vòng sáng ở ô
  người chơi **thật sự đã gán**.
- **"Gán ô phụ" tìm ô trống** (`s_KNGan[i].uId == 0`) nay **đúng** vì không còn ô "hiện kỹ năng mà chưa gán".
- Nhánh thứ hai trong `KyNang_GoKhoiO` (gỡ kỹ năng đang hiện theo danh sách mặc định) thành mã chết vô hại —
  để nguyên cho khác biệt nhỏ nhất.

### 6.4 Chưa đụng (cố ý)

- Khoá `Chinh=` (ô chính) vẫn có thể trỏ kỹ năng phái cũ. **Không hại**: `KPlayer::SetLeftSkill` chặn
  `nLevel <= 0` nên lệnh bị bỏ lặng lẽ, ô chính luôn soi đúng kỹ năng đánh trái thật của Core.
- Lỗi thứ tự toán tử `(!IsBase()) && (A) || (B)` trong `GetLeftSkillSortList`/`GetRightSkillSortList`
  (`KSkillList.cpp`) là mã **dùng chung với bản PC** — ghi nhận, không sửa trong lượt này.

### 6.5 Đã kiểm

- `clang++` NDK 25.2 (`aarch64-linux-android24`, `-std=gnu++17`, `-DJX_MOBILE -DJX_ANDROID`, `-fsyntax-only`):
  **0 lỗi, 0 cảnh báo mới** (chỉ còn cảnh báo sẵn có về chữ hoa/thường đường dẫn include).
- Mã hoá TCVN3: vá bằng kịch bản latin-1, **79 byte ≥ 0x80 trước và sau bằng nhau**, CRLF giữ nguyên.
- **Chưa dựng APK, chưa thử máy thật.**

### 6.6 Điều chủ phải biết trước khi phát hành

**Người chơi hiện tại sẽ thấy các ô họ chưa từng gán trở thành TRỐNG** (trước đây chúng tự hiện kỹ năng).
Đây đúng là điều chủ chọn, nhưng nên báo trước cho người chơi: **chạm vào ô trống là mở ngay bảng kỹ năng để
gán** (một chạm, `JxKyNang_BatDau`), không phải ba bước.

### 6.7 Đo lại sau khi lên máy thật

1. Nhân vật mới: vào game → các ô phụ có kỹ năng như trước; tệp `UserData\kynangmobile_<mã>.ini` có
   `O0..` và `Dung=1`.
2. Chuyển phái: sau ≤ 4 giây, ô phái cũ **tự rỗng**, các ô chưa gán **vẫn trống**, vòng sáng không tự bật.
3. Nhật ký: `[KYNANG] dung bo cuc mac dinh mot lan: N o` và `[KYNANG] o i giu ky nang X khong con hoc`.

---

## 7. Phương án đã cân nhắc (lưu lại)

### PA A (đề xuất) — ô trống là trống thật, chỉ dựng mặc định MỘT LẦN

1. Bỏ bậc 3 trong `KyNang_CuaNut`: chưa gán và không có `Trong` → **trả về trống**.
2. Lần đầu một nhân vật vào thế giới mà **chưa có tệp** `KyNangMobile_<uid>.ini`: chụp danh sách kỹ năng hiện có, ghi
   thành `O0..O7=` **thật** rồi lưu tệp. Từ đó bảng ô cố định, chỉ đổi khi người chơi tự đổi.
   - Chốt an toàn: chỉ chụp khi đã vào thế giới và `s_nKNCo1 > 1` (tránh chụp nhằm lúc danh sách đang rỗng).

- Giữ nguyên tiện lợi "vào là đánh được ngay" cho người mới.
- Sau đó **tuyệt đối không tự thêm gì nữa**, kể cả chuyển phái, lên cấp, học kỹ năng mới, đổi vũ khí.
- Người đang chơi (đã có tệp) không bị đụng bảng gán.

### PA B — chặn hẹp, chỉ ở thời điểm chuyển phái

Phát hiện số hiệu phái đổi → ghi `Trong<i>=1` cho mọi ô chưa gán.
Rẻ và ít rủi ro nhất, nhưng **chắp vá**: các dịp khác (lên cấp học kỹ năng mới, nhận kỹ năng sự kiện, đổi vũ khí) vẫn
làm ô chưa gán đổi kỹ năng.

### PA C — dọn ô chết theo đúng cách bản PC (nên làm KÈM A hoặc B)

Trong `KyNang_DocBang`, đối chiếu `s_KNGan[i]` với `s_KNBang` như `KUiSkillTree::UpdateData`; không còn trong danh sách
→ xoá ô + đặt `Trong<i>=1` + ghi tệp.

> ⚠️ **Bẫy bắt buộc phải chặn**: giữa lượt chuyển phái, `DelAllMagic` làm danh sách **rỗng trong tích tắc**. Nếu đối
> chiếu ngay lúc đó sẽ **xoá sạch bảng gán của người chơi**. Phải có chốt: chỉ dọn khi đã vào thế giới, `s_nKNCo1 > 1`,
> và kỹ năng vắng mặt qua **ít nhất 2 lần đọc liên tiếp** (≥ 4 giây) mới xoá.

### Việc phụ đi kèm (nếu chủ duyệt)

- "Gán ô phụ" chọn ô trống phải xét cả `s_nKNTrong` và ô đang hiện kỹ năng.

### Đo lại sau khi sửa

Chuyển phái trên máy thật → xem 8 ô: ô chưa gán phải **trống** (chỉ còn vòng tròn), ô đã gán cho phái cũ phải **tự
rỗng**, vòng sáng không tự bật.

---

## 8. Bảng tệp/dòng đã soi

| Tệp | Dòng | Vai trò |
|---|---|---|
| `Sources/S3Client/Platform/JxCanDieuKhien.cpp` | 946–974 | `KyNang_CuaNut` — **gốc lỗi ở 965–973** |
| | 85, 909–943 | chu kỳ đọc lại danh sách 2 s |
| | 674–726, 728–753 | đọc/ghi `KyNangMobile_<uid>.ini` (`O<i>`, `Trong<i>`, `Chinh`) |
| | 1473–1511 | luân chuyển vòng sáng (quét cả ô tự điền) |
| | 1674–1710 | `KyNang_GoKhoiO` — chỗ đặt cờ `Trong` |
| | 1760–1775 | "Gán ô phụ" tìm ô trống |
| `Sources/Core/Src/KSkillList.cpp` | 700–760 | `GetLeftSkillSortList` — thứ tự danh sách |
| `Sources/Core/Src/CoreShell.cpp` | 1787, 2899 | `GDI_LEFT_ENABLE_SKILLS`, `GDI_KYNANG_MOBILE` |
| `Sources/Core/Src/CoreDrawGameObj.cpp` | 212–245 | vẽ biểu tượng kỹ năng, không kiểm sở hữu |
| `Sources/Core/Src/KProtocolProcess.cpp` | 3356–3380 | `s2cSyncAllSkill` — `Clear()` rồi nạp lại |
| `Sources/S3Client/Ui/UiCase/UiSkillTree.cpp` | 252–274 | **bản PC làm đúng** (mã tham chiếu) |
| `serverscript_live/script/vatpham/lenhbaitanthu.lua` | 1115–1212 | luồng chuyển phái |
| `serverscript_live/script/global/hocvocong.lua` | 880–914 | `hockynang` / `show_kynang90` (cấp 20) |
