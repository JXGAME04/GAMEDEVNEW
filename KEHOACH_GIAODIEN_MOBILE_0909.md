# KẾ HOẠCH: MANG GIAO DIỆN ĐIỀU KHIỂN MOBILE (VNKU / USVOLAM) VÀO BẢN ANDROID NÀY

> Viết 09/09 sau khi chủ chỉ hai nguồn tham khảo. **USVOLAM và NHACTAI chỉ ĐỌC — không được sửa gì bên đó.**
> Đọc kèm `LOTRINH_MOBILE_0909.md` (việc còn lại nói chung) và `BANGIAO_ANDROID_PHA4_0809.md` (§6–§8).

## Phát hiện quan trọng nhất

**Bản mobile của chủ (VNKU) dựng trên CHÍNH hệ giao diện của client này, không phải Cocos.**

Bằng chứng: `C:\Users\nguye\Downloads\NHACTAI\VNKU_ui_ini\ini\Ui\Ui3\UiAssignSkill.ini` viết đúng
định dạng `KWndWindow::Init` đọc:

```ini
[Main]
Left=995
Top=485
Width=120
Height=250
Image=\spr\Ui3\UiSkillControl\assign_skill_70x70.spr
[Btn_MainSkill]
Left=20
Top=20
Width=141
Height=51
Image=\spr\UiNew\UiSkills\nut_phim_chinh.spr
```

Và `D:\USVOLAM\Sources\S3Client\Ui\UiCase\` có sẵn **lớp C++ cùng hệ `KWnd`** cho các cửa sổ mobile.

**Hệ quả:** không phải viết lại gì. Việc còn lại là **chép lớp + chép ảnh + chép ini + nối vào**.
Có hai dự án Cocos (`E:\Source Jx1m Cocos V3 full`, `D:\USVOLAM\Jx1mClientMobile`) — đó là bản
khách khác, **chỉ dùng để hiểu hành vi** (ví dụ cần điều khiển), không chép mã.

## Nguồn tài nguyên

| Thứ | Ở đâu | Ghi chú |
|---|---|---|
| Ảnh giao diện mobile (`.spr`) | `C:\Users\nguye\Downloads\NHACTAI\VNKU_ui\spr\` | **1.959 tệp**, đúng định dạng `.spr` engine này đọc |
| Tệp bố cục (`.ini`) | `C:\Users\nguye\Downloads\NHACTAI\VNKU_ui_ini\ini\Ui\Ui3\` | đúng định dạng `KWndWindow::Init` |
| Lớp C++ | `D:\USVOLAM\Sources\S3Client\Ui\UiCase\` | cùng hệ `KWnd`, chép sang là dùng được |
| Hành vi mobile (cần điều khiển, nút kỹ năng) | `D:\USVOLAM\Jx1mClientMobile\Classes\gamescene\KHRocker.cpp`, `KSkillRocker.cpp` | **chỉ đọc để hiểu**, mã Cocos không chép |

## Đã làm 09/09

- **Cần điều khiển ảo** — `Sources/S3Client/Platform/JxCanDieuKhien.cpp`, vẽ bằng ảnh thật
  `\spr\Ui3\UiSkillControl\joystick_bg.spr` + `joystick_ctrl.spr` (đã chép vào `D:\jx1_android_data\spr\ui3\uiskillcontrol\`).
  Kéo ngón ở nửa trái = cần; chạm vẫn là chuột trái. Mỗi vòng lặp gọi `iCoreShell::Goto(nDir, 0)`,
  8 hướng của vòng 64 hướng — **lấy đúng cách `HRocker` của bản mobile làm**. Có ảnh chủ xác nhận đã hiện.
- **Chạm giữ = chuột phải ở đâu cũng vậy** → mặc / tháo / dùng vật phẩm chạy được.
- **Nối `g_pfnJxGetKeyState`** → Shift/Ctrl/Alt mới có tác dụng (trước đó `GetKeyState` **luôn trả 0**,
  nên Ctrl+phải = menu người chơi, Shift+trái = đánh ép… đều chết, kể cả khi cắm bàn phím rời).
  Có sẵn `JxSdl_DatPhimDinh(mặt nạ)` để một nút ảo giữ hộ phím bổ trợ.

## Hệ tự căn chỉnh giao diện (`FitFlags`) — đã mang sang, nhưng **opt-in**

`KWndWindow` nay có `SetFitFlags(...)` + `FitToScreen()` mang từ USVOLAM ("đợt UI-1/UI-2"):
mỗi tệp `.ini` giao diện được vẽ theo khung chuẩn **1024×768**, cửa sổ nào đăng ký thì được đặt lại
theo màn hình thật (bám lề trái/phải/trên/dưới, căn giữa, hoặc kéo căng). Rào `#ifdef JX_ANDROID`.

**Khác USVOLAM ở hai điểm, đều do đo thật rồi mới đổi:**

1. **Mặc định tự suy ra neo** thay vì "dịch cả khung vào giữa". USVOLAM chỉ bật hệ neo khi màn hình
   **rộng VÀ cao** hơn khung chuẩn; màn hình điện thoại thì ngược lại — rộng hơn nhưng **thấp hơn
   nhiều** (604 / 616 so với 768). Dịch vào giữa kéo thanh công cụ dưới đáy **lên 82 điểm ảnh**
   (đã nhìn tận mắt). Nay: cửa sổ ở một phần ba đầu → bám lề trên/trái, một phần ba cuối → bám lề
   dưới/phải, ở giữa → dịch vào giữa.
2. **Chỉ neo cửa sổ nào TỰ ĐĂNG KÝ** (`m_FitFlags != 0`). Đã thử áp cho mọi cửa sổ một lượt như
   USVOLAM và **đo thấy hỏng**: bản này đã tự chỉnh sẵn nhiều cửa sổ theo `SCREEN_WIDTH/HEIGHT`
   ngay trong mã (ví dụ `UiPlayerBar` có nhánh riêng cho 1024 và gọi `SetSize(SCREEN_WIDTH, ...)`),
   nên neo lại từ khung chuẩn là **chỉnh hai lần** → khung trang trí thanh dưới lạc chỗ.
   Cửa sổ cũ giữ nguyên đường đã chạy; cửa sổ mới của bản mobile gọi `SetFitFlags` là dùng được.

## Các lớp cần mang sang, theo thứ tự

Đo bằng `wc -l` trên cây USVOLAM. Tất cả đều dùng `KWnd*` mà bản này đã có
(`WndImage`, `WndButton`, `WndPureTextBtn`… đã kiểm: `WndPureTextBtn` có sẵn).

| # | Lớp | cpp / h | Đáp ứng yêu cầu nào của chủ |
|---|---|---|---|
| 1 | `UiPlayerControlBar` | 167 / 43 | **nút điều khiển** — thanh nút ảo, tối đa 120 nút chữ, đặt bằng `SetItemBtnInfo/SetItemBtnPos` |
| 2 | `UiMiniSkill` | 1141 / 44 | **nút chọn kỹ năng** (kèm `UiAssignSkill.ini` + `\spr\Ui3\UiSkillControl\assign_skill_*.spr`) |
| 3 | `UiNpcBar` | 350 / 48 | **tới gần NPC hiện icon để chạm** |
| 4 | `UiPlayerLock` | 117 / 43 | **chạm vào người chơi → khoá mục tiêu / hiện lựa chọn** |
| 5 | `UiGive` | 1068 / 88 | **gửi / lấy vật phẩm** |
| 6 | `UiVirtualKeyboard` | 353 / 75 | bàn phím ảo trong game (không phụ thuộc bàn phím hệ thống) |
| 7 | `UiTeamManageBar` | — | thanh tổ đội |
| 8 | `UiInformation1`, `UiNotiInfo`, `UiNotiQuest` | — | thông báo kiểu mobile |

Ngoài ra USVOLAM còn các lớp **nội dung** (không phải điều khiển) bản này chưa có, ghi lại để chủ quyết
sau: `UiCrafting`, `UiKinhMach`, `UiDaTau`, `UiHoatDong`, `UiShopOnline`, `UiRankLienTram`, `UiSelMap`,
`UiCaveList`, `UiChatRoom`, `UiTongSKill`, `UiXemNguHanh`, `UiMocNap`, `UiMocHoatDong`, `UiExpandItem`,
`UiSetPrice`, `UiRise`, `UiInCre`, `UiNewsMessage2`, `UiNewsSysMsg`, `UiProgressBarLoading`,
`UiAutoSkill_SelSkill`, `UiChatStatus`.

## Cách mang một lớp sang (làm đúng thứ tự này)

1. Chép `UiXxx.cpp` / `UiXxx.h` từ USVOLAM sang `Sources/S3Client/Ui/UiCase/`
   — **chép byte, không mở bằng công cụ soạn thảo**: nguồn là TCVN3, mở sai là hỏng hết chữ Việt.
2. Thêm vào `Sources/S3Client/S3Client.vcxproj` (`ClCompile`), rồi chạy `python android/gen_lists.py`
   để sinh lại `android/lists/*.cmake` (**không sửa tay tệp lists**).
3. Dịch thử: `powershell -File ReverseTools\mobile_x64\build_chuoi_sdl.ps1 -Target Build`.
   Thiếu hàm nào thì đó là phần USVOLAM có mà bản này chưa có — cân nhắc chép thêm hoặc bỏ nhánh đó.
4. Chép ảnh `.spr` cần dùng từ `NHACTAI\VNKU_ui\spr\Spr\...` vào thư mục dữ liệu, **tên thư mục và tệp
   hạ chữ thường** (`JxPathPosix` hạ chữ thường phần tương đối).
5. Chép `.ini` bố cục từ `NHACTAI\VNKU_ui_ini\ini\Ui\Ui3\` vào `<dữ liệu>\ui\ui3\`.
6. Mở cửa sổ: gọi `KUiXxx::OpenWindow()` ở chỗ hợp (thường trong `UiShell.cpp UiStartGame`), **bọc
   `#ifdef JX_ANDROID`** để bản PC không đổi.
7. Chạy thử trên LDPlayer, chụp ảnh đối chiếu.

## Luật không được quên

- **USVOLAM và NHACTAI chỉ đọc.** Không ghi, không sửa, không xoá gì trong hai cây đó.
- **Bản PC phải chơi song song được**: mọi thứ thêm vào phải trong `#ifdef JX_ANDROID`
  (hoặc tệp bị `ExcludedFromBuild` ngoài `ReleaseSDL|x64`). Sau mỗi đợt phải dựng lại
  `build_chuoi_x64.ps1` **và** `build_chuoi_sdl.ps1`, cả hai 0 lỗi.
- Nguồn TCVN3 chỉ sửa bằng python `latin-1`, không dùng công cụ soạn thảo thường.
