# BÀN GIAO — GIỚI HẠN ĐĂNG NHẬP THEO MÃ MÁY

**Phiên 14–15/09/2026 · dự án `D:\GAMEDEVNEW` · ĐÃ THI CÔNG, ĐÃ SWAP, ĐÃ TEST CHẠY ĐÚNG**

> ⚠️ **Chỉ dự án này.** Các ghi chú cũ về "giới hạn account" (`CheckMultiClientLimit`, `AccountLimit.ini`,
> `Enforce`, `HwidKick`, `[Whitelist]`) thuộc cây **`D:\USVOLAM`** — **cơ chế KHÁC HẲN và không tồn tại ở đây**.
> Đừng tra cứu sang đó.

---

## 1. Chủ báo gì, và gốc bệnh thật sự là gì

Chủ: *"hiện tại đang có check ip và check hw — nhưng khi qua fw thì tất cả người chơi đều cùng ip của fw nên
chỉ còn check được mã máy — nhưng mã máy thì hay bị trùng nhau nên sẽ dẫn tới người thì vào được người thì không"*.

**Gốc bệnh:** mã máy cũ lấy từ `GetCurrentHwProfile().szHwProfileGuid`. Giá trị đó **không phải** đọc từ phần
cứng — nó là **một khoá trong registry**, sinh một lần lúc cài Windows. Phòng net ghost đĩa từ một máy mẫu thì
**cả dàn máy mang cùng một GUID**. Khi cả phòng lại ra Internet qua một tường lửa (chung IP), máy chủ thấy cả
phòng là **MỘT máy** ⇒ quá ngưỡng là chặn, người vào được người không.

**Phát hiện thêm:** hai lớp "IP" và "HW" thật ra **không phải hai lớp**. Mã cũ **nối chúng thành MỘT chuỗi**
`"<mã máy> <IP>"` rồi so chuỗi chính xác. Ghép kiểu đó làm cơ chế **yếu đi**: chỉ cần đổi **một trong hai** là
ra khoá mới. Cùng một máy mà đổi mạng (4G xoay vòng, CGNAT) là thành hai khoá, lách được.

---

## 2. Đã sửa gì — 6 commit, tất cả trên `origin/main`

| Commit | Giờ | Nội dung |
|---|---|---|
| `312511e0` | 14/09 22:54 | Tài liệu nghiên cứu `PHANTICH_GIOIHAN_DANGNHAP_MOBILE_1409.md` (chỉ phân tích) |
| `2b62cb5a` | 14/09 23:25 | **Client PC**: mã máy từ phần cứng thật + **bỏ hẳn đường mở giới hạn** |
| `5d99579b` | 14/09 23:50 | **Máy chủ**: vá 2 lỗi sập từ xa, 3 lỗi đếm/đá nhầm, thêm chế độ chỉ quan sát |
| `88c7cc9b` | 15/09 00:12 | **S3Relay**: chống kẹt bộ đếm vĩnh viễn khi người chơi thoát |
| `a493995e` | 15/09 00:17 | Sửa 7 lỗi do **phản biện đối kháng** tìm ra trong chính bản vá trước |
| `ea004ed9` | 15/09 00:38 | Log quan sát, chọn nguồn mã máy ổn định, vá lỗi **mất danh sách bạn** |

9 tệp, +1048 / −69 dòng.

### 2.1 Client PC — mã máy mới

Tệp mới `Sources/S3Client/Login/KMachineId.cpp` (+ `.h`). Lấy mã theo **THỨ TỰ ƯU TIÊN**, chỉ dùng **một nguồn
đầu tiên lấy được** — đây là những thứ **không đi theo khi nhân bản đĩa**:

1. **UUID hệ thống trong SMBIOS** (gắn với bo mạch chủ, bền nhất) → hạng `A`
2. **Sê-ri ổ đĩa VẬT LÝ** (khác hẳn volume serial vốn bị sao khi ghost) → hạng `B`
3. **MAC card mạng thật** (loại card ảo, loại MAC đặt tay/ngẫu nhiên hoá) → hạng `B`
4. không có gì → nguồn lui (registry / volume serial) → hạng `C`

Chuỗi trả về **33 ký tự**: `<hạng><32 chữ số hex>`. Vừa trong `sHWID[64]` và còn chỗ cho máy chủ ghi log IP.

> **Vì sao ƯU TIÊN chứ không TRỘN:** bản nháp đầu trộn mọi nguồn tìm được, nên mã phụ thuộc vào **tập** nguồn —
> chỉ cần một nguồn xuất hiện hay biến mất (cắm/rút SSD USB, cắm dây mạng USB, chia sẻ mạng từ điện thoại) là
> **mã đổi** ⇒ lách được. Chú thích cũ viết "cắm thêm ổ đĩa không làm đổi mã máy" là **SAI**, tác tử phản biện
> đã chứng minh bằng đo thật. Ưu tiên một nguồn thì mã chỉ đổi khi chính nguồn đó biến mất.

**Bỏ hẳn đường mở giới hạn:** xoá hàm `GenerateRandomString` — nó chỉ tồn tại để phục vụ hai dòng bị chú thích
kèm ghi chú *"mo gioi han log vao game"* (nối chuỗi ngẫu nhiên vào mã máy = mỗi lần đăng nhập một mã khác = vô
hiệu hoá hoàn toàn giới hạn). Xoá để không ai bật lại bằng cách bỏ hai dấu chú thích.

**Hai bẫy đã tránh:** tiền biên dịch của client đặt `_WIN32_WINNT = 0x0400` (NT 4.0) nên không khai báo hàm đọc
SMBIOS ⇒ tệp này đặt `NotUsing` PCH. Và **liên kết TĨNH hàm đó sẽ làm `Game.exe` KHÔNG MỞ ĐƯỢC trên Windows XP**
⇒ gọi qua `GetProcAddress`, máy cũ chỉ mất một nguồn chứ không hỏng game.

### 2.2 Máy chủ — bỏ IP khỏi khoá, thêm chế độ quan sát

- **Bỏ phần nối IP khỏi KHOÁ ĐẾM.** IP vẫn được ghi vào log để điều tra, chỉ không nằm trong khoá.
  *Lưu ý:* khoá này đi vào `m_nPlayerHWID`, mà chỗ **GHI** vào bảng đếm đọc lại chính nó ⇒ sửa một chỗ là hai
  bên **tự động khớp**. **Đừng bao giờ sửa một bên** — lệch chuỗi là bộ đếm luôn ra 0, giới hạn tắt câm.
- **Chế độ chỉ quan sát** + hai dòng log mỗi lần đăng nhập và mỗi lần đếm.
- **Đóng kết nối TRỄ 2 nhịp.** Lệnh đá chỉ *nạp* gói `s2c_exitgame` vào bộ đệm, còn lệnh đóng lại *xoá sạch* bộ
  đệm đó ⇒ đóng ngay là gói bị vứt, người chơi bị đá mà không hiểu vì sao. Dùng **bộ đếm lùi** chứ không dùng
  mốc nhịp, vì `m_nGameLoop` quay về 0 mỗi 1.512.000 nhịp.

### 2.3 S3Relay — chống kẹt bộ đếm vĩnh viễn (việc chủ yêu cầu kiểm kỹ nhất)

Ba bảng `m_mapAcc` / `m_mapRole` / `m_mapParam` phải nhất quán, nhưng lúc vào game mã cũ **chỉ ghi đè**
`m_mapAcc[acc]`:

```
lần 1:  m_mapAcc[X] = {khe 100}      m_mapParam[100] = {mã máy}
lần 2:  m_mapAcc[X] = {khe 200}      ← MẤT DẤU số 100
rời:    chỉ xoá được m_mapParam[200] → m_mapParam[100] MỒ CÔI VĨNH VIỄN
```

Mỗi mục mồ côi làm bộ đếm của **đúng mã máy đó** tăng thêm một và **không bao giờ giảm**. Tình huống hay gặp
nhất: **rớt mạng đột ngột rồi vào lại trước khi hết 60 giây chờ ping**. Vài lần là máy đó hết đăng nhập được.
Chiều ngược lại cũng hỏng: khe kết nối được tái sử dụng, nên lúc rời game có thể **xoá nhầm mục của người mới**.

**Đã sửa:** lúc vào game dọn **cả hai chiều** trước khi ghi; lúc rời game **chỉ xoá mục vẫn còn thuộc về mình**.
Sổ sách tự nhất quán với **mọi thứ tự sự kiện**, không phụ thuộc đoán đúng kịch bản.

---

## 3. Mười một lỗi đã vá

| Mức | Lỗi | Nơi |
|---|---|---|
| 🔴 **SẬP TỪ XA** | `strcpy_s(sLogin.m_szName /*[64]*/, szHwID)` với chuỗi ghép tới 79 ký tự ⇒ CRT **thoát tiến trình**. Client sửa đổi sập được máy chủ bằng **một lần đăng nhập** | KSOServer `ProcessLoginProtocol` |
| 🔴 **SẬP TỪ XA** | `strcpy(szHwID, pLL->sHWID)` — `sHWID` là trường **CUỐI** gói, client điền đủ 64 byte không NUL ⇒ đọc tràn sang phần còn lại đệm nhận, chép quá đuôi mảng ngăn xếp | nt |
| 🔴 **MẤT DỮ LIỆU** | `m_mapPlayers[someone]` + `assert(loaded)` ở **HAI** chỗ. Bản phát hành không có khẳng định nên **đi tiếp với danh sách bạn RỖNG** rồi ghi đè **xuống CSDL** ⇒ mất sạch danh sách bạn thật | S3Relay `FriendMgr.cpp` |
| 🔴 **KẸT VĨNH VIỄN** | Mục đếm mồ côi khi cùng tài khoản vào lại chưa kịp có gói rời (xem §2.3) | S3Relay `HostConnect.cpp` |
| NẶNG | `(BYTE)m_MaxLogin` cắt cụt: `MaxLogin=256` ⇒ `0` ⇒ **đá sạch mọi người**; `MaxLogin=1000` (cách tắt quen dùng) thành ngưỡng 232 | KSOServer |
| VỪA | `nNetIdx <= 0` bỏ sót **khe số 0** (chỉ số client bắt đầu từ 0, dấu hiệu "không socket" là −1) ⇒ mỗi máy chủ có đúng một người không bao giờ bị đóng cứng | nt |
| VỪA | `TreNhipDong=1` làm đóng socket **trước** lần xả ⇒ vứt mất gói báo thoát — đúng bệnh mà cơ chế sinh ra để tránh | nt |
| VỪA | Tái sử dụng khe mạng ⇒ **đóng nhầm người vô can** khi máy chủ đầy 100 % | nt |
| VỪA | `getIpFromClientInfo(NULL)` = `strlen(NULL)` khi client cắt kết nối ngay sau gói đăng nhập | nt |
| VỪA | Không kiểm `dataLength` ⇒ gói dài 1 byte vẫn bị đọc 64 byte mã máy + GUID từ **rác** của gói kế tiếp | nt |
| NHẸ | `if(&pLL->sHWID[0])` lấy **địa chỉ** mảng nên **luôn đúng** ⇒ nhánh báo lỗi là **mã chết** (có ở 2 chỗ) | nt |

> Bốn lỗi giữa bảng do **tác tử phản biện đối kháng** tìm ra **trong chính bản vá của tôi**. Nó đã biên dịch và
> **chạy thật** mô-đun mã máy trên máy này để kiểm, không chỉ đọc mã.

---

## 4. Đang chạy gì (đã swap 15/09 00:38)

| Tệp | Đang chạy | Lùi được về |
|---|---|---|
| `bin\client\Game.exe` | **ab88ad04** | e7f34350 |
| `bin\server\GameServer.exe` | **69de2967** | 34efa408 |
| `bin\multiserver\S3Relay.exe` | **745cf3cb** | 65f6342a |
| `bin\server\heaven.dll` | dd096b86 (13/09) | không đổi đợt này |

Lùi = đổi tên tệp `.truoc` về tên gốc. **Lưu ý:** chuỗi lùi của S3Relay chỉ về được bản 00:13, **không còn bản
gốc 07/09**; nếu cần bản đó thì dựng lại từ commit `9ba3ebce`.

---

## 5. Cấu hình — mục `[LimitLogin]`

🔴 **Máy chủ nạp `GameServer_cfg.ini`**, KHÔNG phải `GameServer1_cfg.ini` (đã xác minh bằng log).

| Khoá | Mặc định | Ý nghĩa |
|---|---|---|
| `MaxLogin` | 3 (mã) | Số phiên cùng lúc cho phép trên một mã máy. `<= 0` = **TẮT** |
| `ChiQuanSat` | **1** | **1 = CHỈ GHI LOG, KHÔNG ĐÁ AI.** Đặt `0` để bật cưỡng chế |
| `BoQuaHangC` | 1 | Mã máy hạng `C` vẫn **được đếm** và **ghi log**, chỉ không bị đá |
| `TreNhipDong` | 2 | Số nhịp chờ giữa lúc gửi gói thoát và lúc đóng socket (1 tự nâng thành 2) |

🔴 **Hạng A/B/C KHÔNG phải giấy miễn.** Bản nháp đầu của tôi định "hạng C thì không đá" — **SAI, đó là cửa hậu**:
hạng cũng do client khai, gửi `C000...` là thoát sạch giới hạn. Hạng **chỉ để ghi log và theo dõi**. Thấy tỉ lệ
hạng `C` tăng vọt = dấu hiệu **client giả**, không phải dấu hiệu máy yếu.

---

## 6. Bằng chứng test thật (log máy chủ, 15/09 00:4x–00:5x)

```
[MAYID] id=A27EADC94CFCBAD3CB54E1FA92D025ACE  hang=A nguon_dung=uuid-bo-mach   ← client sinh mã
[MAYID] dang nhap: mamay [A27EADC94CFCBAD3CB54E1FA92D025ACE] hang A IP 127.0.0.1  khe 0
[MAYID-DEM] mamay [A27EADC94CFCBAD3CB54E1FA92D025ACE] dang co 0 phien (nguong 6)
...
[MAYID] dang nhap: mamay [A27EADC94CFCBAD3CB54E1FA92D025ACE] hang A IP 127.0.0.1  khe 6
[MAYID-DEM] mamay [A27EADC94CFCBAD3CB54E1FA92D025ACE] dang co 1 phien (nguong 6)   ← HAI client cùng máy
```

Bốn chặng đã chứng minh:

1. Client sinh mã đúng, **hạng A**, nguồn `uuid-bo-mach`.
2. Mã đi **nguyên vẹn** tới máy chủ (chuỗi trong log máy chủ khớp từng ký tự với log client).
3. **Hai client trên cùng một máy ra CÙNG một mã** ⇒ bị đếm chung ⇒ `dang co 1 phien`. Đây là mục đích của
   việc giới hạn, và nó hoạt động.
4. **Thoát ra thì bộ đếm về 0.** Hơn 30 chu kỳ vào/thoát liên tiếp, **không để lại một mục rác nào**. Đây chính
   là thứ chủ bảo kiểm kỹ, và nó sạch.

Sức khoẻ máy chủ trong lúc test: nhịp 1080–1081/phút, trễ **0,0 %**, hơn 1000 người trong game, không lỗi mới.

**Ba máy Android** (`10.0.0.140`, `.127`, `.187`) cho **ba mã khác nhau** ⇒ được tính riêng, không chặn nhầm
nhau. Điều này **tốt hơn** tôi lo trong tài liệu nghiên cứu (§3 của `PHANTICH_...` nói mã mobile có thể là hằng
số chung). **Đính chính:** ba mẫu chưa đủ kết luận cho các máy cùng đời để tên mặc định.

---

## 7. 🔴 Những điều PHẢI nhớ trước khi bật cưỡng chế

1. **Hiện giới hạn đang TẮT** (`ChiQuanSat=1`) — lỏng hơn cả trước khi đụng vào. Đây là cố ý: chưa có số liệu
   thật thì mọi ngưỡng đều là phỏng đoán.
2. **ĐỪNG bật khi vẫn còn người dùng client CŨ.** Client cũ gửi mã kiểu `{...}` sinh từ registry ⇒ cả dàn máy
   ghost đĩa **vẫn trùng nhau** ⇒ tái phát đúng triệu chứng ban đầu. Mã cũ **không** bắt đầu bằng `C` nên
   `BoQuaHangC` **không che được họ**.
3. **Chọn ngưỡng bằng SỐ LIỆU.** Chạy quan sát vài ngày, đọc các dòng `[MAYID-DEM]` xem bao nhiêu mã máy thường
   chạy 2, 3, 5 phiên, rồi lấy ngưỡng ở phân vị rất cao. Đừng chọn bằng cảm tính.
4. **Siết dần.** Mỗi lần hạ `MaxLogin` một bậc, chờ vài ngày. Đừng nhảy thẳng tới số cuối.

---

## 8. Bản Linux tham khảo — chủ đã chốt KHÔNG theo

Mổ `jx_linux_y`, `libheaven.so`, `librainbow.so`, `bishop_y`, `goddess_y`: **không một cơ chế nào** đếm theo
IP, MAC hay máy. Họ chỉ có:

- Trần **tổng** số kết nối (`[Overload] MaxPlayer`, mặc định trong mã 1200).
- **Một tài khoản một phiên**, khoá theo **TÊN TÀI KHOẢN**, cưỡng chế bằng **đá phiên cũ** cho phiên mới vào.

IP và MAC **có** thu thập (MAC nằm ngay trong gói đăng nhập) nhưng chỉ để **ghi log** và chuyển sang máy chủ
tài khoản. Tệp `acc_setup.ini` có khoá `Limit_Muti_Account_Online` nhưng **không nhị phân nào đọc** — nó thuộc
PaySys, không có trong bộ này. Cổng đăng nhập của họ đã được Kim Sơn **viết lại bằng Go**.

*Điểm đáng học duy nhất (chủ chưa lấy): đá phiên cũ thay vì từ chối phiên mới — người vừa gõ mật khẩu luôn vào
được, nên không bao giờ có cảnh "người vào được người không".*

---

## 9. Còn lại — chưa làm

**Ghi nhận, chưa sửa (ngoài phạm vi):**
- `DUMMY_AUTOLOCKWRITE/READ` trong `S3Relay/Global.h` định nghĩa thành `0` = **không khoá gì**. Ba bảng đếm
  không được bảo vệ; nếu các hàm xử lý chạy trên nhiều luồng thì đây là tranh chấp dữ liệu thật.
- `CHostServer::m_mapIp2Connect` khoá theo **DWORD IP** ⇒ nhiều GameServer cùng một IP (ví dụ đều `127.0.0.1`)
  thì **chỉ giữ được một**, đếm sẽ sót các GameServer còn lại.
- `TongConnect.cpp:507` `strcpy_s` chưa chặn biên (chỉ chạm được nếu nối thẳng vào cổng Tong).
- `ChatConnect.cpp` có **cùng kiểu sổ sách rò** nhưng **không** feed vào bộ đếm ⇒ chỉ là rò bộ nhớ.

**Lỗ hổng còn lại — client sửa đổi vẫn lách được:**
Mã máy do **client khai**. Sửa nhị phân để gửi 33 ký tự ngẫu nhiên mỗi lần là giới hạn **vô hiệu 100 %**. Hai
commit này nâng đáng kể độ khó cho người dùng thường và vá lỗ thật, nhưng **không** là hàng rào chống client
sửa đổi. Muốn thật thì theo §5 của `PHANTICH_GIOIHAN_DANGNHAP_MOBILE_1409.md`: **mã thiết bị do MÁY CHỦ phát**,
rồi Android *Key Attestation* / iOS *App Attest* + *DeviceCheck* — loại tín hiệu client không tự tạo ra được.

**Bản mobile:** mã máy băm từ tên máy + thư mục dữ liệu ứng dụng (`KPosixWin32.cpp:1018`). Ba máy đo thật cho
ba mã khác nhau, nhưng đây vẫn là điểm yếu nhất và nên thay bằng mã do máy chủ phát.

---

## 10. Đính chính cho `PHANTICH_GIOIHAN_DANGNHAP_MOBILE_1409.md`

Tài liệu đó viết **trước** khi thi công, nên vài chỗ đã lỗi thời:

- §1 mô tả khoá đếm là `"<HWID> <IP>"` — **đã bỏ phần IP**, nay khoá chỉ là mã máy.
- §1.2 ghi ngưỡng đọc từ `GameServer1_cfg.ini` — thực tế máy chủ nạp **`GameServer_cfg.ini`**.
- §2 L3/L4/L5 và §9 mục 4/5 — **đã vá hết**.
- §3 lo mã mobile là hằng số chung — **đo thật 3 máy cho 3 mã khác nhau**, hạ mức cảnh báo.
- §5.1 "hạng C thì máy chủ không được từ chối" — **SAI, đã bỏ**, xem §5 của tài liệu này.
