# BÀN GIAO 15/09 — iOS: nút Thoát không đóng app, và lưu cấu hình khi vào nền

> **Gửi phiên Android/PC.** Đợt này đụng vào tệp dùng chung. Có **một** thay đổi cố ý ảnh hưởng tới
> **bản Android**; phần còn lại rào riêng cho iOS. Đọc mục 2 trước, đó là phần liên quan tới bạn.

## 1. Vì sao phải sửa

Apple không cho ứng dụng tự đóng mình, và SDL 3.2.30 đã bỏ lệnh `exit()` khi `main` trả về
(`src/video/uikit/SDL_uikitappdelegate.m:398-409`). Hậu quả đo được trên iPhone thật: bấm nút Thoát →
log ghi `[IOS] JxPosixMain tra ve 0` → **tiến trình còn sống nhưng không còn cửa sổ**. Người duyệt của
Apple bấm vào đó sẽ thấy app treo, bị từ chối theo quy định 2.1.

Chủ game chốt hướng xử lý: **giữ nút Thoát, nhưng bấm không đóng app**.

## 2. PHẦN ẢNH HƯỞNG BẢN ANDROID (rào `JX_MOBILE`)

**Lưu cấu hình giao diện khi app sắp vào nền.**

| Tệp | Nội dung |
|---|---|
| `Sources/S3Client/Ui/UiShell.h` | khai báo `UiLuuKhiVaoNen()` |
| `Sources/S3Client/Ui/UiShell.cpp` | cài đặt: thoát sớm nếu **không** ở trạng thái `UI_LIVING_S_INGAME`, còn lại gọi `g_UiBase.SavePrivateConfig()` |
| `Sources/S3Client/Platform/KSdlApp.cpp` | đăng ký `SDL_AddEventWatch` ngay sau `SDL_Init`, bắt `SDL_EVENT_WILL_ENTER_BACKGROUND` (dùng chung). Riêng `SDL_EVENT_TERMINATING` chỉ bật cho iOS — xem điểm 4 |

**Vì sao cho dùng chung:** Android cũng mất bố cục giao diện, thanh kỹ năng và phím tắt khi người chơi
vuốt tắt app, vì đường lưu cấu hình chỉ chạy trong `UiExit()`. Đây là lỗi có sẵn ở cả hai nền tảng,
không phải nhu cầu riêng của iOS.

**Ba điểm kỹ thuật đáng nhớ:**

1. SDL **không** đẩy các sự kiện vòng đời vào hàng đợi thường — `src/events/SDL_events.c` ghi rõ
   *"We won't actually queue this event"* rồi gọi thẳng danh sách theo dõi. Nên `SDL_PollEvent` **không bao
   giờ** thấy chúng; bắt buộc phải dùng `SDL_AddEventWatch`.
2. Điều kiện phải là `UI_LIVING_S_INGAME`, **không** phải "khác DEAD". Đây là quy ước sẵn có của
   `UiExit()` và của nhánh mất kết nối trong `UiHeartBeat()`. Lưu lúc đang ở màn đăng nhập hoặc chọn nhân
   vật là **nguy hiểm**: `SavePrivateConfig()` xoá sạch rồi ghi lại từ các cửa sổ đã bị huỷ, tức có thể
   xoá đúng cái bố cục nó sinh ra để giữ.
3. Cố ý **không** gọi `CleanTempDataFolder()` ở đây: đang chơi mà dọn thư mục tạm có thể xoá nhầm tệp
   đang dùng. Việc dọn tạm vẫn để nguyên trong `UiExit()`.
4. **`SDL_EVENT_TERMINATING` chỉ bật cho iOS, cố ý.** Trên Android, lúc người chơi vuốt tắt app thì
   `nativeSendQuit` → `WM_CLOSE` → `UiExit()` **đã lo việc lưu rồi**. Bật thêm nhánh này là ghi đè tệp ini
   lần thứ hai, ngay trong cửa sổ 1 giây của `mSDLThread.join(1000)` lúc app bị huỷ — mà `KIniFile::Save`
   ghi đè toàn bộ tệp, nên quá hạn là tệp cụt. Nhánh vào-nền (`WILL_ENTER_BACKGROUND`) mới là phần Android
   thực sự còn thiếu: bấm Home hay vào màn gần đây hiện **không** lưu gì.
5. Hàm theo dõi chỉ ghi một tệp ini, **không** gọi lại SDL và không giữ khoá nào, nên an toàn ở cả hai nền
   tảng. Lưu ý khác biệt: trên iOS nó chạy lúc bơm sự kiện nên không chen ngang giữa khung, còn trên Android
   nó **có thể** chạy giữa khung qua `Android_WaitActiveAndLockActivity`.

**Việc của phiên Android:** cân nhắc hai điểm mình không kiểm được từ phía iOS —
SDL trên Android chạy game ở luồng riêng và có thể chặn ở `onPause` (`SDL_ANDROID_BLOCK_ON_PAUSE`), nên
cần xác nhận việc ghi tệp trong bộ theo dõi sự kiện không gây ANR; và kiểm xem bản Android đã có sẵn
đường lưu nào lúc vào nền chưa, tránh lưu hai lần. **Nếu thấy không hợp, đổi `JX_MOBILE` thành `JX_IOS`
là bản Android trở lại y như cũ**, không cần sửa gì thêm.

## 3. PHẦN RIÊNG iOS (rào `JX_IOS`, bản Android không đổi)

| Tệp | Nội dung |
|---|---|
| `Sources/Engine/Src/Platform/KPosixWin32.cpp` | `PostQuitMessage()` không đẩy sự kiện thoát |
| `Sources/S3Client/Ui/UiShell.cpp` | `UiPostQuitMsg()` không đặt cờ chết |
| `Sources/S3Client/Ui/UiCase/UiInit.cpp` | nút Thoát ở màn đầu: không làm gì (**không** gọi `CloseWindow()`, gọi là màn hình đen) |
| `Sources/S3Client/Ui/UiCase/UiConnectInfo.cpp` | hộp lỗi kết nối: về màn chọn máy chủ |
| `Sources/S3Client/Ui/ShortcutKey.cpp` | `LuaExit()` cho script: không làm gì |
| `Sources/S3Client/Platform/KSdlApp.cpp` | `GameLoop()` trả false: không thoát |
| `Sources/S3Client/S3Client.cpp` | auto `bLaunch == 2`: bỏ qua, **không** `return` |
| `Sources/S3Client/Platform/JxWAutoNoiBo.cpp` | xoá `bOutWhenDis` / `bOutTimer` / `bOutWhenTP` |

**Không đụng** hộp ESC trong game: nó vốn đã gọi `ReturnToIdle` rồi mở lại màn đầu, không đóng tiến trình.

**Vì sao ba mục cuối không cho dùng chung:** trên Android app **được phép** tự đóng, nên chặn lệnh thoát
và xoá ba cờ tự thoát của auto sẽ **xoá mất tính năng đang chạy đúng**. Hai mục auto chỉ là hệ quả của việc
iOS không còn thoát: lệnh thoát thành lệnh rỗng nhưng vẫn `return`, làm mỗi nhịp auto bị cắt ngang và auto
đứng im lặng. Android không có vấn đề đó.

## 4. Kiểm rào: bạn chạy sẽ thấy ĐẠT, nhưng bản Android **có đổi** — đọc kỹ mục này

**Chạy `python3 ios/kiem_rao.py` sau khi kéo về, bạn sẽ thấy `DAT`. Đừng kết luận là Android không đổi.**
Kiểm rào so cây làm việc với **HEAD**, mà thay đổi này giờ đã nằm trong HEAD, nên không còn gì để nó báo.

Lúc còn chưa commit, nó báo đúng như sau:

```
KIEM RAO (ANDROID): HONG - 3 cho phai xem lai
  !! Sources/S3Client/Platform/KSdlApp.cpp
  !! Sources/S3Client/Ui/UiShell.cpp
  !! Sources/S3Client/Ui/UiShell.h
KIEM RAO (WINDOWS): DAT - ban WINDOWS khong doi
```

Phần chênh lệch in ra **chỉ gồm khối `#ifdef JX_MOBILE` của mục 2** — không có gì khác lọt sang Android.
Bản Windows ĐẠT thật, vì chế độ `--pc` lọc bỏ cả `JX_MOBILE`.

**Muốn tự xem lại phần ảnh hưởng bản Android**, dùng đúng tính năng so với một mốc khác của kiểm rào:

```
python3 ios/kiem_rao.py 98ec561a        # 98ec561a = commit [PALDO 15/09], ngay truoc hai commit nay
```

Hoặc xem thẳng bản vá:

```
git show c64c2428 -- Sources/S3Client/Ui/UiShell.h Sources/S3Client/Ui/UiShell.cpp Sources/S3Client/Platform/KSdlApp.cpp
```

**Lưu ý về giới hạn của kiểm rào**, nên biết để khỏi tin quá:
- Nó **không nhìn** `ios/CMakeLists.txt`, chỉ so thư mục `Sources/`.
- Chế độ `--pc` **bỏ qua** tệp không nằm trong `.vcxproj` nào: `JxWAutoNoiBo.cpp`, `KPosixWin32.cpp`,
  `UiShell.h`. Nên ĐẠT không có nghĩa là đã phủ hết.
- Nó **không dò trùng lặp**: một khối bị chèn hai lần vẫn ĐẠT và vẫn dựng được.

## 5. Hạ tầng phát hành iOS (chỉ trong `ios/`, không ảnh hưởng ai)

`ios/CMakeLists.txt` đổi nhiều, **chú ý khi gộp nhánh**: thêm tệp khai báo quyền riêng tư
`ios/PrivacyInfo.xcprivacy`; tắt camera/HIDAPI/âm thanh/cảm biến của SDL; đặt `INSTALL_PATH` để đóng gói
Archive được; thêm `-g` cho mọi target để dSYM đủ (trước khi sửa dSYM chỉ có **2** tệp nguồn, sau khi sửa
có **466**); `CFBundleIconName` và số bản dựng lấy từ biến CMake. Thêm `ios/dung_ban_phat_hanh.sh` để đóng
gói bản phát hành ở **cây dựng riêng** — chạy Archive trong `build/ios-dev` sẽ phá cây bản thử.

## 6. Ba cái bẫy đã trả giá, ghi lại để khỏi lặp

1. **Mọi dòng thêm vào tệp dùng chung, kể cả chú thích, phải nằm trong nhánh chỉ-iOS.** Để lọt một dòng
   chú thích vào nhánh `#ifndef JX_IOS` là kiểm rào báo HỎNG. Dạng an toàn: `#ifdef JX_IOS` chứa chú thích,
   `#else` giữ mã cũ. Dòng trống thừa cũng bị bắt.
2. **Script vá phải nhận biết "đã vá" bằng dấu riêng của từng bản vá**, không dựa vào việc neo còn hay mất.
   Ba bản vá vào `KSdlApp.cpp` đều chèn *sau* một dòng neo vẫn còn, nên lần chạy thứ hai vá đè và trình biên
   dịch báo `redefinition`. Chạy script hai lần liên tiếp, lần hai phải ra `0/N`.
3. **Đừng dò chữ trên đầu ra `codesign -d --entitlements`**: nó in toàn bộ quyền trên **một dòng**, nên
   `grep -A1 get-task-allow | grep '<true/>'` bắt trúng khoá khác. Gói phát hành thật luôn có
   `beta-reports-active = true`. Dùng `plutil -extract get-task-allow raw`.

## 7. Đã kiểm

Kiểm rào như mục 4; Debug và Release dựng sạch; hai script vá chạy lại ra `0/5` và `0/7`; Archive mới qua
tự kiểm với dSYM 466 tệp nguồn. **Chủ game đã test trên iPhone thật ngày 15/09 và xác nhận đạt**: nút Thoát
bấm không đóng app, ESC → Thoát game vẫn về màn đầu, vuốt tắt rồi mở lại không mất bố cục, âm thanh bình thường.

## 8. Chưa làm, để đợt sau

1. **App vẫn vẽ khi đã vào nền.** `m_bActive` đặt TRUE trong hàm dựng và không bao giờ được gán lại
   (`KSdlApp.cpp` định tuyến mất tiêu điểm vào `WM_ACTIVATEAPP` nhưng `KWin32App.cpp` không có nhánh xử lý).
   iOS giết app vì gửi lệnh GPU trong nền. Lỗi có sẵn, nhưng nặng hơn từ khi app không còn thoát.
2. **Thư mục tạm không còn được dọn trên iOS**, vì `UiExit()` không chạy nữa.
3. Nên bắt thêm `SDL_EVENT_LOW_MEMORY`, và chỉ lưu khi có thay đổi — iOS báo "sắp vào nền" mỗi lần kéo
   Trung tâm điều khiển hay có thông báo, ghi tệp mỗi lần là thừa.
4. `KIniFile::Save` cắt tệp rồi mới ghi, không ghi tạm rồi đổi tên; bị giết giữa chừng thì tệp cụt.
5. **Chú thích cũ trong `macos/CMakeLists.txt` (khoảng dòng 11–12) nói tệp này định nghĩa `JX_MOBILE` và
   `JX_IOS`, nhưng dòng 56–57 cho thấy không phải.** Tôi không sửa vì ngoài phạm vi đợt này, nhưng chú thích
   sai kiểu đó rất dễ làm người sau phân loại nhầm chung/riêng — nên sửa khi có dịp.
