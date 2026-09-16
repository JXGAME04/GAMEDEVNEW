# GIỚI HẠN TÀI KHOẢN ĐĂNG NHẬP — hiện trạng và kỹ thuật mới cho APK / iOS

**Ngày 14/09/2026 · CHỈ PHÂN TÍCH, CHƯA SỬA MÃ.**
Phạm vi: **chỉ cây `D:\GAMEDEVNEW`** (+ worktree mobile `D:\GAMEDEVNEW_wt_mobile`, nhánh `mobile-0809`).
Mọi số dòng dưới đây đọc từ chính hai cây đó.

---

## 0. Kết luận một trang

1. **Không phải hai lớp.** Chủ nói "có check IP và check HW", nhưng trong mã chúng bị **nối thành MỘT chuỗi**
   `"<HWID client khai> <IP thật>"` (`KSOServer.cpp:3898-3906`) rồi đếm bằng so sánh chuỗi **chính xác**.
   ⇒ Đổi **một trong hai** là ra khoá mới, bộ đếm về 1. Ghép kiểu này làm cơ chế **yếu đi**, không mạnh lên:
   nó lấy phần dễ phá nhất của cả hai, thay vì bắt thoả mãn cả hai.

2. 🔴 **Bản mobile: HWID gần như là một hằng số chung cho mọi máy.** `GetCurrentHwProfileA` trong
   `KPosixWin32.cpp:1010` băm đúng hai thứ: `gethostname()` và thư mục dữ liệu ứng dụng. Trên Android thư mục
   dữ liệu **giống hệt nhau ở mọi máy** cùng tên gói, còn hostname hầu hết là `localhost`. ⇒ Mọi người chơi
   Android nhiều khả năng mang **cùng một HWID**. Hậu quả không phải "lách được" mà là **phạt oan**: những người
   dùng chung một IP thoát (nhà, phòng net, 4G CGNAT) sẽ cùng một khoá, người thứ 6 bị đá.

3. **HWID phía PC là GUID hồ sơ phần cứng Windows** (`GetCurrentHwProfile`, `Login.cpp:945`,
   `NetConnectAgent.cpp:212`). Đây **không phải** giá trị suy từ phần cứng: nó là một khoá trong registry,
   sinh một lần lúc cài Windows, **giống nhau cho mọi người dùng trên máy**, và **đi theo khi nhân bản máy ảo**.
   Đổi nó = sửa một khoá registry.

4. **Trong client có sẵn công tắc mở khoá giới hạn**, đang bị chú thích:
   `NetConnectAgent.cpp:212-213` có hai dòng `hwIDStr += "-"; hwIDStr += GenerateRandomString(8);`
   kèm chú thích "mo gioi han log vao game". Bỏ chú thích = mỗi lần đăng nhập một HWID khác = **vô hiệu hoá
   hoàn toàn** giới hạn. Ai có bản client sửa đổi cũng làm được đúng việc đó.

5. **Nguyên lý không đổi:** mọi thứ **client tự tính rồi gửi** đều giả được. Chỉ ba thứ tin được:
   **IP thật của socket**, **tên tài khoản lấy từ CSDL**, và **thời điểm do máy chủ tự đo**.

6. **Kỹ thuật mới thật sự** cho APK và iOS không phải "vân tay thiết bị mạnh hơn", mà là **chứng thực phần cứng
   do Google/Apple ký, máy chủ tự kiểm** — Android *Key Attestation*, iOS *App Attest* + *DeviceCheck*.
   Đây là loại tín hiệu **client không tự tạo ra được**, khác hẳn mọi vân tay hiện nay.

7. **Điều kiện tiên quyết, chưa làm thì mọi thứ phía client đều vô nghĩa:** APK phải được **ký bằng khoá riêng
   của chủ** và **tắt `debuggable`**. Bản đang phát nếu còn ký bằng khoá gỡ lỗi công khai thì ai cũng sửa rồi ký
   lại được, không có dấu vết.

---

## 1. Cơ chế đang chạy — đọc từ mã

### 1.1 Đường đi một lần đăng nhập

```
CLIENT                          GAMESERVER                        S3RELAY
  |                                  |                                |
  | c2s_logiclogin { guid, sHWID[64] }                                |
  |--------------------------------->|                                |
  |            KSOServer.cpp:3891     |                                |
  |            szHwID = sHWID                                          |
  |            szHwID += " " + IP thật của socket   <-- :3902-3906     |
  |            AttachPlayer(...) -> m_nPlayerHWID[64]                  |
  |                                  |  enumC2S_TONG_GET_LOGIN_LIMIT   |
  |                                  |  m_szName = szHwID              |
  |                                  |-------------------------------->|
  |                                  |            TongConnect.cpp:505   |
  |                                  |            num_login =           |
  |                                  |            CountLoginByHWID(...) |
  |                                  |<--------------------------------|
  |                                  | enumS2C_TONG_LOGIN_LIMIT        |
  |            KSOServer.cpp:1811     |                                |
  |            num_login > MaxLogin ? -> RemovePlayerForLimit(nIdx)    |
```

### 1.2 Các con số và vị trí

| Thứ | Giá trị / vị trí |
|---|---|
| Ngưỡng | `[LimitLogin] MaxLogin` — **5** trên máy đang chạy; mặc định trong mã là **3** (`KSOServer.cpp:348, 514`) |
| Khoá đếm | `"<HWID> <IP>"`, dựng ở `KSOServer.cpp:3898-3906` |
| Đếm ở đâu | `CHostServer::CountLoginByHWID` (`HostServer.cpp:138`) cộng dồn **qua mọi GameServer**, mỗi kết nối đếm bằng `CHostConnect::CountLoginByHWID` (`HostConnect.cpp:660`) |
| Phép so | `infoParam.hwid == hwid` — **so chuỗi chính xác**, không chuẩn hoá, không nhóm |
| Cưỡng chế | `RemovePlayerForLimit(nIdx)` = **đá ra**, không có chế độ chỉ quan sát |
| Nguồn HWID (PC) | `GetCurrentHwProfile().szHwProfileGuid` |
| Nguồn HWID (mobile) | shim tự viết, `KPosixWin32.cpp:1010` |

### 1.3 Điểm **đang làm đúng**, đừng phá

- **IP là IP thật của socket**, lấy ở máy chủ (`getIpFromClientInfo`), client không khai được. Đây là phần tốt nhất
  của cơ chế hiện tại.
- **Đếm tập trung ở S3Relay across mọi GameServer** — đúng hướng; mở thêm máy chủ không làm thủng giới hạn.

---

## 2. Bốn lỗi tìm được trong mã hiện tại

### L1 🔴 Ghép chuỗi làm yếu đi, không mạnh lên

Khoá là `HWID + " " + IP`. Muốn thoát chỉ cần **đổi một trong hai**: đổi HWID (client sửa đổi, registry, máy ảo)
**hoặc** đổi IP (4G bật tắt, VPN, proxy). Nếu muốn "cả hai phải thoả" thì phải **đếm riêng hai trục** rồi lấy
mức chặt hơn, chứ không phải nối chuỗi.

### L2 🔴 Bản mobile: HWID gần như hằng số (xem §3)

### L3 Nhánh "không lấy được HWID" là mã chết

`KSOServer.cpp:3897` và `:3920` viết `if(&pLL->sHWID[0])`. Đó là **địa chỉ** của phần tử mảng, **luôn khác NULL**
⇒ điều kiện luôn đúng, nhánh `else` in "dont get HWID from client" **không bao giờ chạy**. Client gửi HWID rỗng
vẫn đi tiếp bình thường, và khoá đếm khi đó là `" <IP>"` — tức là **rơi về đếm thuần theo IP** mà không ai biết.

### L4 Cắt cụt khoá khi lưu

`szHwID` dài tới 64 + 1 + 15 = **80 ký tự**, nhưng `m_nPlayerHWID[64]` (`KPlayer.h:695`) chỉ chứa 63 ⇒
`KPlayerSet.cpp:1043` **cắt mất phần IP** nếu HWID client khai dài. Chuỗi gửi sang S3Relay thì vẫn đầy đủ, nên
**hai nơi đang giữ hai giá trị khác nhau** cho cùng một người. Cái dùng để đếm là bản đầy đủ, nên chưa gây sai
kết quả đếm, nhưng `GetHWID` phơi ra Lua (`ScriptFuns.cpp:13513`) trả về bản cụt.

### L5 (phía client, nhỏ) Con trỏ chưa khởi tạo

`char* szHwID;` rồi `if (GetCurrentHwProfile(...)) { szHwID = ... }` rồi `if(szHwID[0])`
(`NetConnectAgent.cpp:211-223`, `Login.cpp:944-951`). Nếu hàm lấy hồ sơ thất bại, `szHwID` chưa gán mà vẫn bị
đọc ⇒ đọc bộ nhớ rác, có thể sập client. Hiếm trên Windows, nhưng trên shim mobile thì đường này do ta viết.

---

## 3. 🔴 Bản mobile — phần quan trọng nhất của tài liệu này

`D:\GAMEDEVNEW_wt_mobile\Sources\Engine\Src\Platform\KPosixWin32.cpp:1010`:

```c
BOOL GetCurrentHwProfileA(LPHW_PROFILE_INFOA p)
{
    char h[256] = "android"; gethostname(h, sizeof(h));
    unsigned long a = 5381;
    for (const char* c = h; *c; c++)        a = a * 33 + (unsigned char)*c;   /* ten may */
    for (const char* c = s_szDataDir; *c; c++) a = a * 33 + (unsigned char)*c; /* thu muc du lieu */
    snprintf(p->szHwProfileGuid, ..., "{%08lX-4E44-4A58-B1B1-%012lX}", ...);
    return TRUE;
}
```

Hai đầu vào, cả hai đều **không phân biệt được máy**:

| Đầu vào | Trên Android | Trên iOS |
|---|---|---|
| `s_szDataDir` | `/data/data/<tên gói>/...` — **giống hệt mọi máy** cùng ứng dụng | sandbox theo UUID cài đặt — **đổi mỗi lần cài lại**, nhưng giống nhau giữa các máy về hình dạng |
| `gethostname()` | hầu hết trả `localhost` (Android 8+ bỏ `net.hostname`) | thường là tên máy người dùng đặt, **trùng nhau rất nhiều** ("iPhone") |

**Hệ quả trên Android:** HWID nhiều khả năng **giống nhau ở mọi máy**. Khoá đếm rút gọn còn **thuần IP**.
Với `MaxLogin=5`, mọi người chơi Android sau người thứ 5 trên **cùng một IP thoát** bị đá — kể cả khi họ là
năm người khác nhau trong một nhà, một phòng net, hay chung một cụm CGNAT của nhà mạng. Đây là **phạt oan**,
và nó im lặng: log chỉ ghi `HWID [...] limit login`.

**Hệ quả trên iOS:** ngược lại — `s_szDataDir` chứa UUID cài đặt nên **đổi sau mỗi lần cài lại**, tức là
gỡ ra cài lại là **reset bộ đếm**. Vừa phạt oan vừa lách được, ở hai đầu khác nhau.

> **Việc cần làm trước mọi thứ khác: đo.** Thêm một dòng log in `szHwProfileGuid` lúc đăng nhập trên hai máy
> Android thật khác nhau. Nếu hai giá trị bằng nhau thì kết luận trên được xác nhận và đây là lỗi **đang gây hại
> cho người chơi thật**, phải sửa trước khi bàn kỹ thuật mới.

---

## 4. Cái gì tin được, cái gì không

| Tín hiệu | Ai tạo ra | Giả được không |
|---|---|---|
| IP thật của socket | tầng TCP ở máy chủ | **Không giả được** (nhưng đổi được bằng 4G/VPN/proxy, và **dùng chung** bởi nhiều người vô can) |
| Tên tài khoản | CSDL sau khi xác thực | **Không** |
| Thời điểm máy chủ đo | máy chủ | **Không** |
| HWID hiện nay (PC và mobile) | **client tự tính rồi gửi** | **Có** — sửa client, hoặc bỏ chú thích 2 dòng có sẵn |
| ANDROID_ID, IDFV, IMEI, MAC | client đọc rồi gửi | **Có** — và giả lập có công cụ đổi hàng loạt |
| **Chứng thực Google/Apple ký** | **phần cứng an toàn của máy** | **Không**, nếu máy chủ tự kiểm chữ ký |

Dòng cuối là toàn bộ nội dung của "kỹ thuật mới hơn".

---

## 5. Kỹ thuật mới — áp dụng được cho cả APK và iOS

### 5.1 Tầng A — Mã thiết bị do MÁY CHỦ phát (rẻ, làm được ngay, chung cho hai nền)

Thay vì để client tự nghĩ ra HWID, **máy chủ phát** một chuỗi ngẫu nhiên 32 byte lần đầu chạy, client cất vào
kho an toàn của hệ điều hành và gửi lại mỗi lần đăng nhập.

- **Android:** cất bằng `EncryptedSharedPreferences`, khoá nằm trong Android Keystore.
- **iOS:** cất trong **Keychain**. Điểm mạnh: mục Keychain **sống sót qua việc gỡ ứng dụng**, nên cài lại không
  reset được bộ đếm — vá đúng lỗ iOS ở §3.

So với hiện nay: vẫn giả được (người dùng có thể xoá để lấy mã mới), nhưng **hơn hẳn** vì mã là **duy nhất thật
sự cho mỗi máy**, không đụng nhau, và không còn phụ thuộc IP để phân biệt người. Việc này **một mình nó** đã sửa
cả hai hướng hỏng ở §3.

### 5.2 Tầng B — Chứng thực phần cứng (mạnh, máy chủ tự kiểm, đây là "kỹ thuật mới")

**Android — Key Attestation (khuyến nghị số một cho APK).**
Ứng dụng tạo một khoá trong Keystore kèm `setAttestationChallenge(<số dùng một lần do máy chủ phát>)`. Máy Android
trả về **chuỗi chứng chỉ X.509 ký bởi gốc chứng thực của Google**. Máy chủ kiểm chuỗi đó và đọc phần mở rộng
chứng thực để biết: khoá có nằm trong vùng an toàn phần cứng không (TEE hay StrongBox), trạng thái khởi động
có được xác minh không, ứng dụng nào sở hữu khoá.

- **Giả lập (LDPlayer, BlueStacks) không tạo nổi chuỗi này** vì không có khoá phần cứng do Google ký.
  Đây là cách chặn giả lập đúng bản chất, thay cho mọi mẹo dò dấu vết giả lập.
- **Không phụ thuộc Google Play** ⇒ hợp với APK phát ngoài cửa hàng như bản của dự án.
- Số dùng một lần do máy chủ phát ⇒ **không phát lại được** bản ghi cũ.

**iOS — App Attest + DeviceCheck.**
- **App Attest** (`DCAppAttestService`): Secure Enclave ký, máy chủ kiểm theo gốc của Apple. Chứng minh
  **đúng ứng dụng của mình, chạy trên máy Apple thật, chưa bị sửa**. Tương đương Key Attestation bên Android.
- **DeviceCheck**: cho phép lưu **2 bit gắn với máy**, **sống sót qua gỡ cài lại và cả xoá sạch máy**, đọc/ghi
  qua API máy chủ của Apple. Đúng là thứ để đánh dấu "máy này đã dùng hết suất miễn phí" mà cài lại không xoá được.
  Hạn chế: chỉ 2 bit (4 trạng thái), nên dùng làm cờ, không dùng làm bộ đếm.

**Android — Play Integrity API**: mạnh, nhưng **đòi ứng dụng có mặt trong Google Play Console** và khớp chữ ký.
Với bản APK tự phát thì đây là rào vận hành lớn. **Key Attestation ở trên cho gần hết lợi ích mà không vướng rào này**
⇒ ưu tiên Key Attestation, để Play Integrity lại nếu sau này lên cửa hàng.

### 5.3 Tầng C — Trục tài khoản (trần thật)

Dù chứng thực mạnh đến đâu, người quyết tâm vẫn mua thêm máy. Trần cuối cùng nằm ở **tài khoản**:
đăng ký có chi phí (xác minh số điện thoại, nạp tối thiểu), và **một tài khoản một phiên**. Cái này nằm ở
CMS/PaySys, ngoài mã game, nhưng nó mới là thứ quyết định.

---

## 6. Lộ trình đề xuất (chưa làm, chờ chủ quyết)

| Giai đoạn | Việc | Đụng gì | Rủi ro |
|---|---|---|---|
| **0** | **Đo trước**: log `szHwProfileGuid` trên 2 máy Android thật; thêm chế độ **chỉ quan sát** (`Enforce=0`) để ghi log thay vì đá | máy chủ, một khoá ini | Rất thấp |
| **0b** | Sửa L3 (`if(&pLL->sHWID[0])` mã chết) và L4 (cắt cụt) | `KSOServer.cpp`, `KPlayerSet.cpp` | Thấp |
| **1** | **Đếm riêng hai trục** thay vì nối chuỗi: `theo HWID` và `theo IP`, mỗi trục một ngưỡng, có **danh sách miễn trừ IP** cho phòng net / 4G | S3Relay + GameServer | Trung bình |
| **2** | **Mã thiết bị do máy chủ phát** (§5.1) — Keychain trên iOS, Keystore trên Android; thay hẳn `GetCurrentHwProfile` ở đường mobile | client mobile + máy chủ | Trung bình |
| **3** | **Chứng thực phần cứng** (§5.2): Key Attestation cho APK, App Attest + DeviceCheck cho iOS; kiểm phía máy chủ | client + một dịch vụ kiểm chứng (nên đặt ở lớp web đã có, không nhét vào vòng lặp game) | Cao, nhưng đây là bước làm nên khác biệt |
| **4** | Rào tài khoản ở CMS/PaySys (§5.3) | ngoài mã game | Chính sách |

**Nguyên tắc xuyên suốt: quan sát trước, phạt sau.** Cơ chế hiện nay **không có chế độ quan sát** — nó đá thẳng.
Với lỗi HWID hằng số ở §3, rất có thể đang có người chơi thật bị đá mà không ai biết. Thêm `Enforce=0` là việc
rẻ nhất và nên làm đầu tiên.

---

## 7. Điều kiện tiên quyết cho mọi việc phía client

1. **APK phải ký bằng khoá riêng của chủ và tắt `debuggable`.** Nếu bản đang phát còn ký bằng khoá gỡ lỗi mặc
   định thì bất kỳ ai cũng sửa rồi ký lại được, và mọi biện pháp phía client chỉ là trang trí. **Cần kiểm tra
   bản APK đang phát trước khi đầu tư vào tầng B.**
2. **Chứng thực phải kiểm ở máy chủ**, không kiểm ở client. Client chỉ chuyển tiếp chứng chỉ; máy chủ mới là nơi
   phán quyết. Nếu để client tự kiểm rồi báo "tôi hợp lệ" thì quay lại đúng chỗ cũ.
3. **Không gọi mạng ra Google/Apple trong vòng lặp game.** Việc kiểm chứng thực phải nằm ở lớp web / dịch vụ
   riêng, trả kết quả về qua CSDL hoặc S3Relay, đúng như hạ tầng kịch bản đang làm.

---

## 8. Rủi ro phải tính trước

- **Phạt oan là rủi ro số một, không phải kẻ gian.** 4G ở Việt Nam dùng CGNAT: hàng nghìn thuê bao chung một IP
  thoát. Bất kỳ ngưỡng nào đặt trên trục IP đều phải có danh sách miễn trừ và phải chạy ở chế độ quan sát
  trước ít nhất một tuần.
- **Chứng thực loại người dùng hợp lệ:** máy đã mở khoá bootloader, máy cũ không có TEE, máy đã root vì lý do
  chính đáng sẽ trượt chứng thực. Nên dùng chứng thực làm **hệ số**, ví dụ máy không chứng thực được thì hạ suất
  miễn phí, chứ không chặn đăng nhập.
- **Mỗi lần siết là một lần mất người chơi.** Nhóm treo nhiều nick thường trùng với nhóm nạp tiền. Nên cân nhắc
  cưỡng chế bằng **kinh tế** (giảm dần quyền lợi từ nick thứ N) thay vì đá ra.

---

## 9. Chưa kiểm chứng được — nói thật

1. **Chưa đo `szHwProfileGuid` trên máy Android thật.** Kết luận §3 suy từ mã và từ hành vi thường thấy của
   `gethostname()` trên Android, **chưa có số đo**. Đây là việc đầu tiên phải làm.
2. **Chưa biết bản APK đang phát ký bằng khoá nào và có `debuggable` không** — chưa mở tệp APK ra xem.
3. **Chưa xác định bản iOS đã có chưa và phát hành theo đường nào** (TestFlight, chứng chỉ doanh nghiệp, hay
   tự ký). Đường phát hành quyết định App Attest có dùng được không.
4. ~~Chưa đọc `RemovePlayerForLimit`~~ — **đã đọc** (`CoreServerShell.cpp:1422`): nó **chỉ**
   `PackDataToClient(s2c_exitgame)`, tức là **nạp một gói vào bộ đệm rồi nhờ chính client tự thoát**.
   Không đóng socket, không gỡ người chơi khỏi thế giới. ⇒ **Client sửa đổi chỉ cần bỏ qua gói đó là ở lại**,
   và cả cơ chế giới hạn mất tác dụng với đúng nhóm mà nó nhắm tới. Đây là lỗ thứ năm, nặng ngang §2-L1.
   Sửa đúng cách là **hoãn đóng socket** ở vòng lặp chính sau khi gói đã thực sự ra dây, không gọi
   `ShutdownClient` ngay tại chỗ (đóng ngay sẽ nuốt mất gói và người chơi không thấy thông báo).
5. **Chưa có ai bị đá trên thực tế**: `GameServer.log` và `hethong.log` đều **0 dòng** `limit login`.
   Nghĩa là hoặc chưa ai chạm ngưỡng 5, hoặc nhánh này chưa từng chạy. Máy đang test cục bộ chỉ có bot và
   một client của chủ, nên **chưa kết luận được**; phải đo lại khi có người chơi mobile thật.
