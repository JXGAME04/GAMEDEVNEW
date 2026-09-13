# MỔ NHỊ PHÂN PHẦN MẠNG — BẢN 2.0 & BẢN LINUX so với DỰ ÁN

**Ngày 13/09/2026 · CHỈ PHÂN TÍCH, KHÔNG SỬA GÌ** (theo yêu cầu của chủ).

Nguồn đã mổ:

| Bản | Tệp | Cỡ (byte) |
|---|---|---|
| Linux | `D:\ServerLinux\server1\libheaven.so` | 182.713 |
| Linux | `D:\ServerLinux\server1\librainbow.so` | 84.048 |
| Linux | `D:\ServerLinux\server1\jx_linux_y` (máy chủ game) | 8.931.808 |
| Linux | `D:\ServerLinux\server1\S2SSyncRelayD` | 6.802.744 |
| 2.0 | `Vo Lam Truyen Ky 2.0\rainbow.dll` | 33.152 |
| 2.0 | `gamecl.exe` (đã bung nén, `gamecl_unpacked.bin`) | 34.588.706 |
| Dự án | `Sources\MultiServer\{Heaven,Rainbow,Common}` + `bin\{server,client}\*.dll` | — |

Công cụ tự viết trong phiên: `elfsym.py` (đọc ELF đã tước tiết diện, chỉ dựa program header + `PT_DYNAMIC`)
và `dis2.py` (tháo mã bằng capstone, **giải tên hàm ngoài qua PLT `jmp [ebx+off]` → `DT_JMPREL`**),
để trong thư mục nháp của phiên.

---

## 0. Kết luận một trang

**Ba bản là cùng một dòng mã gốc, nhưng dự án ta đứng ở THẾ HỆ CŨ HƠN của lớp mạng.**

- Bản Linux và bản 2.0 dùng **cùng một thư viện**: `librainbow.so` và `rainbow.dll` 2.0 **đều xuất đúng một hàm
  `CreateClientManager`**, cùng bộ lớp `KClientManager / KClient / KThreadLock / KBuffer / KPackBuffer / ICoder`.
- Dự án ta dùng thế hệ trước: `heaven.dll` / `Rainbow.dll` xuất **`CreateInterface`** (kiểu COM, `IServerFactory`),
  lõi là `CIOCPServer` / `CGameClient` dựng trên khung IOCP `OnlineGameLib::Win32` (CIOBuffer, CSocketServer).
- **Giao diện hàm thì giống hệt nhau** — Linux `KServer` có đúng các tên `PackDataToClient`, `SendPackToClient`,
  `GetPackFromClient`, `ShutdownClient`, `MultiSend` như `CIOCPServer` của ta. Tức là phần thân máy chủ game bên trên
  gần như không đổi; **chỉ lớp vận chuyển bên dưới bị viết lại**.
- Chứng cứ họ hàng rõ nhất: `jx_linux_y` chứa đúng câu báo lỗi của ta —
  `Initialization failed! Don't find a correct rainbow.dll` (bản ta ở `KSOServer.cpp:570` cho heaven.dll),
  và nạp thư viện bằng `dlopen("./lib%s.so", "heaven"/"rainbow")`, lớp bọc tên `KHeavenLib` / `KRainbowLib`.

**Bốn khác biệt kỹ thuật đáng giá nhất** (chi tiết ở §3–§6):

| | Linux + 2.0 | Dự án |
|---|---|---|
| Mô hình I/O máy chủ | **epoll**, 1 luồng mạng, chờ 10 ms | **IOCP**, `CPU×2+2` luồng |
| Nhịp xả gói xuống client | **≤ 10 ms**, độc lập nhịp game | **1 nhịp game ≈ 55 ms** (18 lần/giây) |
| Bộ đệm mỗi kết nối | **co giãn** (4 KB → gấp đôi, `realloc`) | **cố định** 16 KB, đầy thì đóng/chờ |
| Khung gói trên dây | 2 byte cỡ **cho TỪNG gói** | 2 byte cỡ **cho CẢ LÔ mỗi lần xả** |

Và một điểm **ta hơn**: ta có `TCP_NODELAY` (`SocketServer.cpp:277`), **bản Linux không đặt** (chỉ `SO_REUSEADDR`).

---

## 1. Quan hệ ba dòng mã

```
                  ┌───────────────── dòng mã gốc (Kim Sơn) ─────────────────┐
                  │                                                          │
   THẾ HỆ CŨ  ────┤  heaven.dll / rainbow.dll   xuất CreateInterface          │
   (DỰ ÁN TA)     │  CIOCPServer : IServer, CSocketServer   (IOCP)           │
                  │  CGameClient : IClient, CSocketClient                    │
                  │  đệm CIOBuffer cố định · KSG_EncodeDecode (XOR cuốn khoá)│
                  │                                                          │
   THẾ HỆ SAU ────┤  libheaven.so   xuất CreateServer        (epoll)         │
   (LINUX + 2.0)  │  librainbow.so  xuất CreateClientManager                 │
                  │  rainbow.dll 2.0 xuất CreateClientManager  ← CÙNG MÃ     │
                  │  đệm KBuffer co giãn · KPackBuffer + ICoder (cắm ngoài)  │
                  └──────────────────────────────────────────────────────────┘
```

### 1.1 Bằng chứng: bảng ký hiệu

`libheaven.so` — 97 hàm định nghĩa, nhóm theo lớp:

| Lớp | Số hàm | Byte mã | Vai trò |
|---|---|---|---|
| `KServer` | 38 | 11.798 | máy chủ epoll (tương đương `CIOCPServer`) |
| `KPackBuffer` | 13 | 1.500 | khung gói + mã hoá theo gói |
| `KBuffer` | 15 | 1.248 | đệm co giãn |
| `KUniQueue` | 7 | 1.215 | hàng đợi **không trùng** chỉ số kết nối |
| `KSelecter` | 10 | 776 | bọc `epoll_create/ctl/wait` |
| `KThreadLock` / `KAutoThreadLock` / `KNullLock` | 6 | 136 | khoá |

`librainbow.so` — 81 hàm: `KClientManager` (26 hàm, 4.984 B), `KClient` (19 hàm, 2.593 B),
`KPackBuffer`, `KBuffer` **giống hệt libheaven từng byte-kích-thước** (271/256/199/157/108/107/93/64/59/58/35),
tức là cùng một tệp nguồn biên dịch vào cả hai.

Danh sách hàm `KServer` **trùng tên với `IServer` của ta**:
`PackDataToClient(uint, const void*, uint)`, `SendPackToClient(uint)`, `GetPackFromClient(uint, uint&)`,
`ShutdownClient(uint)`, `MultiSend(uint, const KDATA*, int)`, `GetClientCount`, `GetClientInfo`,
`RegisterEventHandler`, `RegisterCoder(ICoder*)`.

### 1.2 Bằng chứng: hằng số bắt tay khoá

Hai hằng `0x2E6D2399` (cộng) và `0x2E6D23CF` (xor) dùng để giấu khoá phiên:

| Tệp | Có hằng số? |
|---|---|
| Linux `libheaven.so` | **CÓ** |
| Linux `librainbow.so` | **CÓ** |
| Linux `S2SSyncRelayD` | **CÓ** |
| 2.0 `gamecl.exe` (bung nén) | **CÓ** (mã nghịch đảo: `xor 0x2e6d23cf` → `sub 0x2e6d2399` → `not` → đảo byte) |
| 2.0 `rainbow.dll` | không (dll chỉ vận chuyển; phần khoá nằm trong `gamecl.exe`) |
| Dự án `heaven.dll`, `rainbow.dll`, `Rainbow.dll` (server + client) | **KHÔNG** |

Trong `gamecl.exe` còn thấy tên lớp `KRainbowLib`, `ICoder`, `KClientThread`, `IClientCallback`, `KClientCallback`
và chuỗi `Failed to export function from rainbow` — đúng khuôn `jx_linux_y`.

---

## 2. Máy chủ Linux: `libheaven.so`

### 2.1 Mở dịch vụ — `KServer::OpenService(ip, port)` @0x44c0

```
socket(AF_INET=2, SOCK_STREAM=1, 0)
setsockopt(fd, SOL_SOCKET=1, SO_REUSEADDR=2, &1, 4)      ← chỉ mỗi tuỳ chọn này
ModifySocket(fd):  fcntl(F_GETFD=1) ; fcntl(F_SETFD=2, flags)      (đóng khi exec)
Unblock(fd):       fcntl(F_GETFL=3) ; fcntl(F_SETFL=4, flags)      (không chặn)
bind(fd, addr, 16)
listen(fd, 10)                                            ← HÀNG CHỜ CHỈ 10
pthread_create(...)                                       ← 1 luồng mạng
```

**Không có `TCP_NODELAY`, không có `SO_SNDBUF`/`SO_RCVBUF`.** Hàng chờ `listen` = 10 là rất nhỏ; khi 500 người
cùng vào (đầu trận Tống Kim, sau khi máy chủ khởi động lại) hệ điều hành sẽ bỏ bớt yêu cầu kết nối.

### 2.2 Vòng lặp — `KServer::Loop` @0x5d50

```
KSelecter::Wait(danh_sách_socket, max_kết_nối, 10)      → epoll_wait, hết giờ 10 ms
   với mỗi socket sẵn sàng:  RecvData(conn) → KUniQueue::Push(chỉ số)
HandleNewConnection()                                    → accept
ProcessEventList()                                       → KUniQueue::PopList → đóng hoặc SendData
```

`KUniQueue` là hàng đợi **chống trùng**: một kết nối dù có 50 gói trong nhịp cũng chỉ nằm một lần trong danh sách xả.
Đây là cơ chế gộp ghi tự viết trong không gian người dùng.

### 2.3 Nhận — `KServer::RecvData` @0x4160

```
KBuffer::GetFree(&len)            ; nếu 0 → Adjust() (dồn đệm) rồi thử lại; vẫn 0 → trạng thái 6 (đầy, đóng)
recv(fd, p, len, 0)
   = 0            → trạng thái 2 (đối phương đóng)
   = -1, errno 4  (EINTR)  → bỏ qua
   = -1, errno 11 (EAGAIN) → bỏ qua
   = -1, khác     → trạng thái 5
KBuffer::PushData(n)
KPackBuffer::Decode(coder, conn->khoá_nhận)
```

### 2.4 Gửi — `KServer::SendData` @0x4050

```
KBuffer::GetData(0, &len)
KPackBuffer::Encode(coder, conn->khoá_gửi)
send(fd, p, len, 0)
   errno 105 (ENOBUFS) / 11 (EAGAIN) / 4 (EINTR) → giữ nguyên, thử lại nhịp sau
KPackBuffer::PopData(số_byte_đã_gửi)           ← gửi thiếu vẫn đúng
```

### 2.5 Xếp gói — `KServer::PackData` @0x42e0 (điểm đáng học)

```
nếu conn->trạng_thái != 0                      → trả 0 (kết nối đang hỏng)
nếu đệm_đang_dùng + cỡ_gói >= sức_chứa:
        SendData(conn) NGAY                    ← xả cưỡng bức khi sắp tràn
        nếu vẫn không đủ → trạng_thái = 1
KPackBuffer::PushPack(data, len)
KUniQueue::Push(chỉ_số)                        ← hẹn xả ở cuối vòng lặp (≤ 10 ms)
```

So với ta: `CIOCPServer::PackDataToClient` **chỉ chép vào `pWriteBuffer`**, không có nhánh "sắp tràn thì xả ngay";
tràn thì báo lỗi.

### 2.6 Bắt tay khoá 42 byte — `KServer::InitConnection` @0x52c0

Ngay khi `accept`:

```
conn->khoá_gửi  = _Rand()
conn->khoá_nhận = _Rand()
RandMemSet(42, đệm)                 ← 42 byte NGẪU NHIÊN toàn bộ
đệm[0] = 0x20 ; đệm[1] = 0x00
đệm[8..11]  = giấu(khoá_gửi)
đệm[17..20] = giấu(khoá_nhận)
KPackBuffer::PushPack(đệm, 42)
KPackBuffer::PassCodeOffset()       ← gói bắt tay NẰM NGOÀI dòng mã hoá
```

Phép giấu (đọc trực tiếp từ mã máy):

```
x = đảo_byte(khoá)        ; (x<<24) | (x>>24) | (x & 0x00FFFF00)
x = ~x
x = x + 0x2E6D2399
x = x ^ 0x2E6D23CF
```

Phía 2.0, `gamecl.exe` tại `0x18d5a4`/`0x18d5b2` có đúng phép nghịch đảo (`xor 0x6d23cf` … `sub 0x2e6d2399` … `not` …
đảo byte). Phía Linux, `KClientManager::InitializeKey` @0x3d70 **kiểm tra gói đầu tiên phải đúng 42 byte**
(`cmp [ebp-0x1c], 0x2a`) rồi mới bóc khoá, gọi `PassCodeOffset()`, `PopPack()`.

---

## 3. Máy chủ dự án: `CIOCPServer`

Đối chiếu từng mục trên:

| Mục | Linux `KServer` | Dự án `CIOCPServer` |
|---|---|---|
| Nạp | `dlopen("./libheaven.so")` → `CreateServer` | `CLibrary("heaven.dll")` → `CreateInterface(IID_IServerFactory)` (`KSOServer.cpp:80,548`) |
| Khởi tạo | `KServer::Create(số_kết_nối, cỡ_đệm)` | `SetEnvironment(MaxPlayer+Precision, Precision, 10, 16384)` (`KSOServer.cpp:554`) |
| Số luồng | 1 luồng mạng | `CPU × 2 + 2` luồng IOCP (`SocketServer.cpp:48`) + 1 luồng phụ thêm/xoá client |
| Chờ sự kiện | `epoll_wait(…, 10 ms)` | `GetQueuedCompletionStatus` (không hết giờ) |
| Hàng chờ listen | **10** | tham số `backlog` của `CSocketServer` (`Socket.cpp:94`) |
| Tuỳ chọn socket | `SO_REUSEADDR` | `TCP_NODELAY` trên mọi socket nhận (`SocketServer.cpp:277`), `SO_LINGER` khi đóng gấp |
| Khoá | `KThreadLock` (pthread mutex) mỗi lần `PackDataToClient` | `m_csCM` + `csWriteAction`/`csReadAction` riêng từng client, tuỳ khoá `[Server] GuiKhoaRieng` (`ServerStage.cpp:265`) |
| Xả | trong luồng mạng, ≤ 10 ms | `SendPackToClient(-1)` trong **luồng game**, mỗi nhịp (`KSOServer.cpp:3341`) |

Về chuyện xả: chú thích `[DELTA 07/09]` ngay tại `KSOServer.cpp:3339` ghi rõ trước kia xả 2 nhịp một lần (gói chờ
0–111 ms), nay mỗi nhịp → **18 lần WSASend mỗi giây mỗi client**. Bản Linux xả tới **100 lần mỗi giây**, và quan trọng
hơn là **không phụ thuộc nhịp game**: nhịp game có tụt xuống 10 fps thì gói vẫn ra trong 10 ms.

---

## 4. Bộ đệm và khung gói

### 4.1 `KBuffer` (Linux/2.0) — co giãn

`KBuffer::Extend` @0x3710:

```
cần = (đang_dùng + thêm) × 2
nếu cần > trần (m_ulMaxSize) → thất bại
cỡ = sức_chứa_hiện_tại, nếu = 0 thì 0x1000 (4 KB)
lặp: cỡ = cỡ × 2  cho tới khi cỡ >= cần
Adjust()          ← dồn dữ liệu về đầu
realloc(đệm, cỡ)
```

`Shrink()` co lại khi rỗng. Nghĩa là: **kết nối bình thường tốn 4 KB; kết nối đang dồn dữ liệu tự nở ra** tới trần
cấu hình. `jx_linux_y` đọc khoá `BufferSize` (chuỗi tại 0x207bb7, cạnh `Port` và `heaven`) — trần này chỉnh được
bằng tệp cấu hình.

Hằng `0x1000` xuất hiện 5 lần và `0xFFFE` 1 lần trong `.text` của `rainbow.dll` 2.0 → đúng cùng mã.

### 4.2 `CIOBuffer` (dự án) — cố định

- Đệm cache: `m_snBufferSize = 1024*16` = 16 KB, giữ sẵn 10 cái (`KSOServer.cpp:120-121`).
- `CIOCPServer` mặc định `bufferSize_Cache = 8192*2`, `bufferSize = 1024*10` (`ServerStage.h:89`).
- `CGameClient` mặc định `bufferSize_Cache = 8192`, `bufferSize = 1024*10` (`ClientStage.h:57-58`).
- Khi đầy: `ProcessDataStream` in `Too much data! found error and close this socket!` rồi `Shutdown()` +
  `Empty()` (`ServerStage.cpp:1302-1315`). Phía client đã được vá `[RECV 04/09]` để **chờ** thay vì vứt khối
  (`ClientStage.cpp:410-437`) — đúng sự cố đã ghi trong bộ nhớ `jx1-goddess-gameserver-mat-khung-rainbow-0409`.

**Đây là khác biệt về độ bền:** bản Linux gặp dồn ứ thì cấp thêm bộ nhớ, bản ta gặp dồn ứ thì đóng kết nối.

### 4.3 Khung gói trên dây — KHÁC NHAU

`KPackBuffer::PushPack` @0x3c80 (Linux/2.0):

```
nếu len > 0xFFFE → từ chối
ghi WORD (len + 2)        ← cỡ gồm cả 2 byte đầu
ghi len byte dữ liệu
```

`KPackBuffer::EnumPack` @0x39b0 đọc lại: `WORD cỡ` ở đầu, cần `cỡ > 2` và đủ byte, trả về `(con trỏ+2, cỡ-2)`.

Dự án ta (`ServerStage.cpp:802-805`, `ClientStage.cpp:187-192`):

```
headlength = 2 + tổng_số_byte_đang_dồn      ← MỘT đầu gói cho CẢ LÔ
ghi WORD headlength
ghi toàn bộ lô
```

Nghĩa là:

```
LINUX/2.0 :  [len1][gói1][len2][gói2][len3][gói3] …     2 byte × số gói
DỰ ÁN     :  [tổng][gói1 gói2 gói3 …]                    2 byte × số lần xả
```

Ranh giới gói bên trong lô của ta do tầng giao thức JX1 tự định (mỗi gói có byte lệnh + cỡ theo loại).
**Cách của ta tốn ít băng thông hơn** — đúng hướng các đợt tối ưu `[DELTA]`/`[BANGTHONG]` đã làm. Nhưng cũng vì thế
**một byte lệch trong lô làm hỏng cả lô**, còn bản Linux chỉ hỏng đúng một gói.

Trần một gói `0xFFFE` của Linux áp cho **từng gói**; trần `WORD` của ta áp cho **cả lô** — khi 500 người cùng bản đồ,
lô gửi cho một client có thể chạm 65.535 byte dễ hơn nhiều so với một gói đơn.

---

## 5. Mã hoá

| | Linux + 2.0 | Dự án |
|---|---|---|
| Nơi đặt | `KPackBuffer::Encode/Decode(ICoder*, khoá)` | `KSG_EncodeBuf/DecodeBuf` gọi thẳng |
| Thuật toán | **cắm ngoài** — `RegisterCoder(ICoder*)`, thân nằm trong `jx_linux_y`/`gamecl.exe` | XOR cuốn khoá 4 byte (`KSG_EncodeDecode.cpp:14-38`) |
| Phạm vi | **từng gói một**, chỉ phần thân (sau 2 byte cỡ) — `Encode` duyệt `EnumPack` rồi gọi `coder->vtbl[0]` | **cả lô**, sau 2 byte cỡ |
| Khoá | ngẫu nhiên 2 chiều, trao qua gói 42 byte | ngẫu nhiên 2 chiều, trao qua gói `ACCOUNT_BEGIN` |
| Miễn trừ | `PassCodeOffset()` để gói bắt tay không bị mã hoá | gói bắt tay gửi thẳng bằng `pSocket->Write` trước khi bật khoá |

Bắt tay của ta (`ServerStage.cpp:1633-1671`):

```cpp
pCNMoi->uServerKey = _Rand();
pCNMoi->uClientKey = _Rand();
pCNMoi->uKeyMode   = 0;
…
RandMemSet(sizeof(SendAccountBegin), (unsigned char *)&SendAccountBegin);   // cùng tên hàm với bản Linux
SendAccountBegin.AccountBegin.ServerKey = ~uServerKey;                      // giấu bằng NOT
SendAccountBegin.AccountBegin.ClientKey = ~uClientKey;
pSocket->Write((BYTE *)&SendAccountBegin, sizeof(SendAccountBegin));
```

Client đọc ở `SocketClient.cpp:629-632`. **Vậy ta KHÔNG thiếu bắt tay khoá** — chỉ là bản Linux/2.0 giấu khoá kỹ hơn
(đảo byte + cộng + xor thay vì chỉ NOT) và đệm 42 byte rác quanh khoá.

> Ghi chú tránh hiểu lầm: hằng `uKey = 0x5A17C3E9` ở `ServerStage.cpp:581` **không phải khoá thật**. Nó nằm trong
> `_NhanBanXaMot`, tức bộ mô phỏng tải `[GUI 07/09] MoPhongNhanBan`, không đụng đường gửi thật.

---

## 6. Phía client

### 6.1 Bản 2.0 (`rainbow.dll` 33 KB + `gamecl.exe`)

- Xuất **một hàm duy nhất** `CreateClientManager`; PDB gốc `D:\newBuilder\projects\jxvn20\code\product\win32\server\rainbow.pdb`.
- Nhập WS2_32 theo số thứ tự: `send`(19), `recv`(16), `socket`(23), `connect`(4), `bind`(2), `ioctlsocket`(10),
  `closesocket`(3), `htons`(9), `inet_ntoa`(12), `WSAStartup/WSACleanup/WSAGetLastError`.
  **Không có `WSASend`/`WSARecv`/`WSASocket` → không dùng IOCP.**
- Luồng: `CreateThread` + `WaitForSingleObject` + `TerminateThread` + `Sleep`; lớp `KThreadLock` (CRITICAL_SECTION).
- Chuỗi trạng thái đóng: `CLOSED`, `SENDFAIL`, `RECVFAIL`, `INVALIDSERVER`, `BUFERROR`; nhật ký
  `connect to [%s:%d]: ok|fail`, `[%s:%d] closed: %s(%d)`, `shutdown [%s:%d]`.
- `gamecl.exe` chỉ nạp `rainbow` (chuỗi `Failed to export function from rainbow`, `NetConnectAgent`) — **không có
  heaven** ở client, giống ta.

### 6.2 Bản Linux (`librainbow.so`) — cùng mã, đọc được rõ hơn

`KClientManager::Work` @0x4b20:

```
lặp: Loop() ; usleep(0x2710 = 10.000 µs = 10 ms)
thoát: SendAll()
```

`KClient::ConnectTo` @0x37a0:

```
socket(2,1,0) ; ModifySocket (đóng khi exec)
connect(...)                     ← CHẶN, chưa bật chế độ không chặn
UnblockSocket(fd)                ← fcntl O_NONBLOCK SAU khi nối xong
Bind(...) ; KPackBuffer::Init() ×2 ; strncpy(inet_ntoa(...), 15)
```

`KClient::SendPackToServer` @0x3390: `PushPack`; **chỉ gọi `SendData` ngay khi đệm sắp tràn**, còn lại để vòng 10 ms
gom rồi gửi.

### 6.3 Dự án (`Rainbow.dll` 2.135.552 B phía client)

`Sources\S3Client\NetConnect\NetConnectAgent.cpp:22,70`: nạp `"Rainbow.dll"` → `GetProcAddress("CreateInterface")`
→ `IClientFactory` → `IClient::ConnectTo`.

`CGameClient::SendPackToServer` (`ClientStage.cpp:171-201`):

```cpp
CIOBuffer *pBuffer = Allocate();
headlength = 2 + datalength;  pBuffer->AddData(&headlength, 2);  pBuffer->AddData(pData, datalength);
KSG_EncodeBuf(datalength, pBuffer->GetBuffer()+2, &m_uClientKey);
Write( pBuffer );                     // GỬI NGAY, mỗi gói một lần ghi
```

**Khác biệt:** client ta gửi **từng gói một** (mỗi gói = 1 lần ghi socket, 2 byte đầu riêng); client 2.0/Linux
**gom trong cửa sổ 10 ms**. Với gói đi lên (di chuyển, chiêu thức) thì lượng nhỏ, nhưng khi tự động hoá/nhiều lệnh
liên tiếp thì số lần gọi socket của ta cao hơn hẳn.

---

## 7. Bảng đối chiếu tổng hợp

| Hạng mục | Linux (`libheaven`/`librainbow`) | 2.0 (`rainbow.dll`) | Dự án (`heaven`/`Rainbow`) |
|---|---|---|---|
| Hàm xuất | `CreateServer` / `CreateClientManager` | `CreateClientManager` | `CreateInterface` |
| Lớp lõi | `KServer` / `KClientManager`+`KClient` | `KClientManager`+`KClient` | `CIOCPServer` / `CGameClient` |
| Mô hình | epoll, 1 luồng | luồng + `ioctlsocket` | IOCP, CPU×2+2 luồng |
| Chờ | `epoll_wait` 10 ms | `Sleep` trong luồng | hoàn thành (IOCP) |
| Không chặn | `fcntl O_NONBLOCK` | `ioctlsocket FIONBIO` | `WSAIoctl FIONBIO` (client) / overlapped (server) |
| `TCP_NODELAY` | **không** | không thấy | **có** (`SocketServer.cpp:277`) |
| `SO_REUSEADDR` | có | — | — (chưa thấy) |
| listen backlog | **10** | — | tham số `backlog` |
| Đệm/kết nối | 4 KB, gấp đôi, `realloc`, có trần cấu hình | như Linux | 16 KB cố định (cache), 10 KB (đọc) |
| Khi đệm đầy | nở thêm; hết trần → đánh dấu hỏng | như Linux | đóng socket / chờ (bản vá 04/09) |
| Khung gói | 2 byte **mỗi gói**, trần 0xFFFE | như Linux | 2 byte **mỗi lô xả** |
| Mã hoá | `ICoder` cắm ngoài, **theo từng gói** | thân coder trong `gamecl.exe` | `KSG` XOR, **theo cả lô** |
| Bắt tay khoá | gói 42 byte, đảo byte+`~`+cộng+xor, `PassCodeOffset` | đọc gói 42 byte | `ACCOUNT_BEGIN`, giấu bằng `~` |
| Gửi thiếu | `PopData(n)` đúng số byte đã gửi | như Linux | `CIOBuffer` + IOCP xử lý |
| Xả server→client | ≤ 10 ms, luồng mạng riêng | — | mỗi nhịp game (~55 ms) |
| Xả client→server | gom 10 ms | gom 10 ms | **gửi ngay từng gói** |
| Chống trùng hàng đợi xả | `KUniQueue` | — | duyệt `m_usedClientNode` mỗi nhịp |

---

## 8. Điểm đáng cân nhắc (CHƯA SỬA — chờ chủ quyết)

Xếp theo lợi ích/rủi ro, tất cả đều chỉ là đề xuất:

1. **Xả gói theo thời gian thay vì theo nhịp game.** Hiện `SendPackToClient(-1)` nằm trong vòng game
   (`KSOServer.cpp:3341`). Bản Linux tách hẳn ra luồng mạng 10 ms. Lợi: khi nhịp game tụt (đông người, Tống Kim),
   độ trễ mạng không tụt theo. Rủi: phải rà lại toàn bộ khoá `csWriteAction` — đây là chỗ vừa sửa `[GUI 07/09]`.
   *Phương án nhẹ hơn, ít rủi ro:* giữ nguyên kiến trúc, chỉ thêm nhánh **"đệm sắp đầy thì xả ngay"** trong
   `PackDataToClient` như `KServer::PackData` làm.

2. **Bộ đệm ghi co giãn thay vì đóng kết nối.** Chỗ `Too much data! … close this socket!`
   (`ServerStage.cpp:1304`) là nguyên nhân rớt kết nối khi dồn ứ. Bản Linux `realloc` gấp đôi tới trần cấu hình.
   Lợi: hết rớt khi đông. Rủi: cần trần cứng, nếu không một client kẹt sẽ ăn hết RAM.

3. **Trần lô 65.535 byte.** Vì ta gộp cả lô dưới một `WORD`, lô gửi cho một client khi đông người có thể chạm trần.
   Cần đo thực tế: log lô lớn nhất mỗi giờ. *Hiện chưa có số đo — đây là nghi vấn, chưa phải lỗi đã chứng minh.*

4. **`SO_REUSEADDR` khi mở cổng.** Bản Linux có, ta chưa thấy. Ảnh hưởng: khởi động lại máy chủ trong vòng
   `TIME_WAIT` có thể báo bận cổng.

5. **Gom gói phía client.** `CGameClient::SendPackToServer` gửi ngay từng gói. Gom trong 1 nhịp vẽ sẽ giảm số lần
   ghi socket. Rủi: tăng độ trễ lệnh người chơi — **có thể không đáng đổi**, cần chủ quyết.

6. **Giấu khoá yếu hơn.** Ta giấu khoá phiên bằng `~`, bản 2.0/Linux dùng đảo byte + `~` + cộng + xor và chèn khoá
   vào 42 byte rác. Đây là chống dò công cụ ngoài, không phải chống người chơi thường.
   Lưu ý: **đổi cách giấu khoá là đổi giao thức**, phải thay cả client và server cùng lúc.

7. **`TCP_NODELAY` ta đã có, bản Linux không** — không cần làm gì, ghi lại để lần sau đừng "sửa theo Linux" nhầm hướng.

8. **`listen(10)` của Linux quá nhỏ** — cũng là điều **không nên** bắt chước.

---

## 9. Cách kiểm chứng lại

Công cụ nằm trong thư mục nháp của phiên
(`…\4b437a48-…\scratchpad\`): `elfsym.py`, `dis2.py`.

```bash
python -c "import sys;sys.path.insert(0,'.');from dis2 import Lib,pretty;h=Lib('D:/ServerLinux/server1/libheaven.so');pretty(h,'_ZN7KServer11OpenServiceEjt')"
```

Đối chiếu nhanh hằng số bắt tay trong mọi tệp:

```bash
python -c "import io;d=io.open('D:/ServerLinux/server1/libheaven.so','rb').read();print(bytes.fromhex('99236d2e') in d)"
```

Các mốc mã đã dùng trong tài liệu này (địa chỉ trong `libheaven.so` trừ khi ghi khác):
`OpenService@0x44c0`, `Loop@0x5d50`, `RecvData@0x4160`, `SendData@0x4050`, `PackData@0x42e0`,
`InitConnection@0x52c0`, `KBuffer::Extend@0x3710`, `KPackBuffer::PushPack@0x3c80`, `EnumPack@0x39b0`;
`librainbow.so`: `KClientManager::Work@0x4b20`, `InitializeKey@0x3d70`, `KClient::ConnectTo@0x37a0`,
`SendPackToServer@0x3390`; `gamecl_unpacked.bin`: khoá tại `0x18d5a4`, `0x18d5b2`.
