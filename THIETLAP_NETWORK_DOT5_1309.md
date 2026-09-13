# THIẾT LẬP MẠNG — ĐỢT 5: MỔ SÂU TỪNG THAM SỐ, ĐỀ XUẤT THIẾT LẬP LẠI

**Ngày 13/09/2026 · đợt 5 · CHỈ PHÂN TÍCH, KHÔNG SỬA MÃ** (chủ: "mổ nhị phân sâu hơn tìm ra các thiết lập lại
network cho hiệu quả nhất", rồi nhắc "chưa fix chỉ phân tích").

Đợt 1–4 mổ **kiến trúc**. Đợt này mổ **tham số**: từng tuỳ chọn socket, hàng chờ `listen`, cỡ đệm và trần đệm,
số luồng, thời gian chờ, kích thước bể — đọc từ **bảng nhập + mã máy** của bản Linux/2.0 và từ **mã + cấu hình đang
chạy** của ta. Mọi con số đều kèm địa chỉ hoặc `tệp:dòng` để kiểm lại. Cuối tài liệu là danh sách thiết lập nên đổi,
xếp theo lợi/rủi, và danh sách **không nên** đổi.

Nguồn đã mổ (cùng bộ với đợt 1–4, thêm hai `rainbow.dll` Windows của bộ gateway):

| Bản | Tệp | Ghi chú |
|---|---|---|
| Linux | `D:\ServerLinux\server1\libheaven.so` / `librainbow.so` | có bảng ký hiệu, tháo mã theo tên hàm |
| Linux | `D:\ServerLinux\server1\jx_linux_y` | máy chủ game, đã tước ký hiệu; lần theo xref chuỗi |
| Linux | `D:\ServerLinux\server1\S2SSyncRelayD` | chỉ đọc chuỗi khoá cấu hình |
| Windows | `D:\ServerLinux\gateway\rainbow.dll` (22.528 B, 2011) · `D:\ServerLinux\Patch\rainbow.dll` (23.040 B, 2020) | cùng PDB `…\kgc\net\Rainbow\…` = thế hệ `CreateClientManager`, đọc bảng nhập theo **số thứ tự** |
| Dự án | `D:\GAMEDEVNEW` HEAD `a22873b8` (cây sạch) | `Sources\MultiServer\{Common,Heaven,Rainbow,GameServer,Goddess,Bishop,S3Relay}` |
| Dự án | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\*.ini` | cấu hình **đang chạy**, chỉ đọc |

Công cụ viết trong phiên (thư mục nháp `…\8b57a9a4-…\scratchpad\`): `net_settings_bin.py` (bảng nhập + tháo mã các hàm
mang thiết lập), `xref_cfg.py` / `xref_more.py` / `xref_last.py` / `xref_main.py` / `xref_create*.py` (lần xref chuỗi
cấu hình trong `jx_linux_y`), `pe_imports.py` (bảng nhập PE theo ordinal). Dùng lại `dis2.py`/`dis3.py`/`elfsym.py` đợt 1.

---

## 0. Kết luận một trang

**1. Toàn bộ "tuỳ chọn socket" của thế hệ Linux/2.0 đã đọc hết — và gần như không có gì.**

| | Máy chủ Linux (`libheaven.so`) | Client Linux (`librainbow.so`) | Client Windows (`rainbow.dll` kgc/net) |
|---|---|---|---|
| `setsockopt` có trong bảng nhập? | **có, đúng 1 chỗ**: `SO_REUSEADDR` trên socket nghe | **không có** | **không có** (`#21` vắng) |
| `TCP_NODELAY` / `SO_SNDBUF` / `SO_RCVBUF` / `SO_KEEPALIVE` | không | không | không |
| Chờ I/O | `epoll_wait` hết giờ **10 ms** | `usleep(10 000)` | `Sleep(10)` + `ioctlsocket(FIONBIO)` |
| `listen` backlog | **10** | — | — |

⇒ Hiệu quả của bản tham khảo **không nằm ở tuỳ chọn socket**. Không có "thiết lập bí mật" nào để chép.

**2. 🔴 Phát hiện lớn nhất đợt này — sửa lại một kết luận của đợt 1–4: bản Linux KHÔNG gom gói trong 10 ms.**
Ngưỡng "sắp tràn thì xả" ở `KBuffer+0x10` **luôn bằng 0** (chỉ hàm dựng, `Init`, `Clear` ghi vào nó, cả ba đều ghi 0;
đã quét toàn bộ 97 hàm của thư viện). Vì thế nhánh "xả ngay" trong `KServer::PackData` được lấy **ở mọi lần gọi**:
mỗi `PackDataToClient` = `send()` phần đang chờ **ngay trên luồng game**, rồi mới xếp gói mới. Vòng 10 ms chỉ quét
**gói cuối cùng** của mỗi kết nối. Client cũng y hệt (`KClient::SendPackToServer`). Tức là họ tốn **~1 syscall mỗi gói**,
và trông cậy vào **Nagle của kernel** (vì không đặt `TCP_NODELAY`) để gom trên dây.
Ta: **1 `WSASend` mỗi client mỗi nhịp** + `TCP_NODELAY` ⇒ ta **ít syscall hơn hẳn và không bị Nagle kìm**.
**Đừng "học" theo cách gửi từng gói, đừng bỏ `TCP_NODELAY`.** Chi tiết + bằng chứng ở §3.

**3. Các con số thật của bản Linux** (lần đầu đọc được): `[Overload] MaxPlayer` mặc định **1200**, không có `Precision`;
`[GameServer] Port` **6666** (giống ta từng chữ); FPS **18** (giống ta); đệm mỗi kết nối người chơi khởi đầu **4 KB**,
nhân đôi khi cần; đệm 5 liên kết máy chủ-máy chủ (**Gateway/Database/Transfer/Chat/Tong — đúng 5 liên kết của ta**)
bị **ép sàn 20 MB** (`0x1400000`, mặc định cấu hình 5 MB thành vô nghĩa); ping liên kết **10 s**, mất ping chỉ ghi log.

**4. Thiết lập của ta đáng chỉnh** (không đụng giao thức, xếp theo lợi/rủi — §6):

| # | Thiết lập | Đang | Nên | Mức |
|---|---|---|---|---|
| 1 | Hàng chờ `listen` (`SocketServer.cpp:203`) | **5** | `SOMAXCONN` | chắc chắn, rẻ |
| 2 | Đệm cache của Bishop (`Intercessor.cpp:432`) | **8.192** < cỡ đọc 10.240 | ≥ 16.384 (quy tắc: ≥ cỡ đọc + 32) | chắc chắn, sửa lỗi mất dữ liệu **cả hai chiều** |
| 3 | Bể đệm cache của GameServer (`KSOServer.cpp:120`) | **10** | ≥ 3 × MaxPlayer (≈ 1000) | chắc chắn, chống phân mảnh heap |
| 4 | `TCP_NODELAY` phía **client** (`SocketClient.cpp:173`) | không | có — **đo trước** bằng bộ `test_rainbow` | đo rồi mới quyết |
| 5 | `SO_REUSEADDR` (đề xuất §8.4 cũ) | không | **vẫn không** — trên Windows nghĩa khác; nếu muốn thì `SO_EXCLUSIVEADDRUSE` | đính chính |
| 6 | Nhịp tim liên kết máy chủ-máy chủ | không có | `SO_KEEPALIVE` + `SIO_KEEPALIVE_VALS` trên socket liên kết | vừa, tuỳ chọn |

**Không nên đổi** (§7): `SO_SNDBUF`/`SO_RCVBUF`, số luồng IOCP, cỡ đọc 10.240, khung theo lô, nhịp xả theo `MainLoop`,
`TCP_NODELAY` máy chủ, ping 60/5/15 s.

---

## 1. Bảng nhập — "được phép gọi gì" là bằng chứng mạnh nhất

Bảng nhập (PLT/GOT với ELF, import directory với PE) liệt kê **mọi** hàm ngoài mà mô-đun có thể gọi. Một tuỳ chọn socket
không thể tồn tại nếu `setsockopt` không có trong bảng.

### 1.1 `libheaven.so` — 170 mục nhập, phần socket/thời gian

```
accept  bind  close  epoll_create  epoll_ctl  epoll_wait  fcntl  listen  recv  send  setsockopt  socket
signal  time  localtime  pthread_create  pthread_join  pthread_mutex_*  realloc  memcpy  memmove  mkdir
```

Không có `getsockopt`, không có `select`/`poll`, không có `usleep`/`nanosleep` (10 ms là tham số `epoll_wait`).
`setsockopt` được gọi **đúng một lần** trong cả thư viện (§2.1).

### 1.2 `librainbow.so` — 119 mục nhập, phần socket/thời gian

```
bind  close  connect  fcntl  recv  send  socket  usleep  inet_ntoa  strncpy  realloc  pthread_*
```

**Không có `setsockopt`.** Client Linux không đặt bất kỳ tuỳ chọn nào — kể cả `SO_LINGER`.

### 1.3 `rainbow.dll` Windows (kgc/net/Rainbow, 2011 và 2020) — nhập WS2_32 theo số thứ tự

Grep chuỗi `setsockopt` trong tệp **không thấy được** vì nhập theo ordinal; phải đọc import directory (`pe_imports.py`):

```
WS2_32.dll: send(#19) recv(#16) WSAGetLastError(#111) WSACleanup(#116) WSAStartup(#115) bind(#2)
            ioctlsocket(#10) closesocket(#3) socket(#23) htons(#9) connect(#4) inet_ntoa(#12)
KERNEL32:   Sleep, InitializeCriticalSection, InterlockedCompareExchange/Exchange, GetTickCount, ...
```

**Không có `setsockopt(#21)`, không `select(#18)`, không `WSAEventSelect`, không `getsockopt(#7)`.**
Hai bản (2011, 2020) giống hệt nhau về bảng nhập; PDB `E:\dailybuilder\sourcecode\vn\Coding\kgc\net\Rainbow\Release\rainbow.pdb`
và `…\product\win32\server\rainbow.pdb` — cùng dòng với `rainbow.dll` 2.0 đã mổ ở đợt 1 (§6.1 tài liệu đợt 1 liệt kê
đúng bộ ordinal này). Cả hai đều mang hằng bắt tay `0x2E6D2399`.

---

## 2. Máy chủ Linux — từng thiết lập, kèm địa chỉ

### 2.1 Mở cổng — `KServer::OpenService(ip, port)` @0x44c0

| Địa chỉ | Lệnh | Thiết lập |
|---|---|---|
| 0x4512 | `socket(2, 1, 0)` | `AF_INET, SOCK_STREAM` |
| 0x4526–0x4548 | `[ebp-0x1c]=1; setsockopt(fd, 1, 2, &1, 4)` | `SOL_SOCKET=1`, **`SO_REUSEADDR=2`** — tuỳ chọn duy nhất |
| 0x4557 | `ModifySocket(fd)` @0x3f90 | `fcntl(F_SETFD, FD_CLOEXEC)` |
| 0x4566 | `Unblock(fd)` @0x3ff0 | `fcntl(F_SETFL, O_NONBLOCK)` — **socket nghe không chặn** |
| 0x45a5 | `bind(fd, addr, 16)` | |
| 0x45b3 | `mov [esp+4], 0xa` → `listen` @0x45c1 | **backlog = 10** |
| 0x45e5 | `KSelecter::Add(listenfd, NULL)` (vtbl+4) | socket nghe cũng nằm trong epoll, `data = NULL` là dấu nhận biết |
| 0x462a | `pthread_create` | **đúng một** luồng mạng |

### 2.2 Nhận kết nối — `KServer::HandleNewConnection` @0x5b40

```
accept(listenfd, &addr, &len=16)                          0x5b7b
ModifySocket(fd)          -> FD_CLOEXEC                   0x5b8e
lấy chỉ số từ vòng rỗi: [+0x40]=vòng, [+0x44]=sức chứa, [+0x48]=đầu, [+0x4c]=LeftFreeCount
nếu idx >= [+0x70] (nMaxConn) -> đóng, log "Invalid ConnectionIdx:%u is allocated!"
Unblock(fd)               -> O_NONBLOCK                   0x5c49
KSelecter::Add(fd, &conn)                                 0x5c6b
InitConnection(conn, fd, &addr)                           0x5ced   (bắt tay 42 byte — đợt 1 §2.6)
log "Allocate Connectionidx:%u, IP:%s, LeftFreeCount:%u"  0x5d17
callback(handler, idx, 0x100)                             0x5d3c
```

**Không có `setsockopt` trên socket vừa nhận** — không `TCP_NODELAY`, không cỡ đệm, không keepalive.

### 2.3 Vòng lặp — `KServer::Loop` @0x5d50 và `KSelecter`

- `KSelecter::Wait(events=[+0x1f4], maxevents=[+0x70]=nMaxConn, timeout=0xa)` — `mov [esp+0xc], 0xa` @0x5d72 ⇒
  **`epoll_wait` hết giờ 10 ms**, số sự kiện tối đa = số kết nối tối đa.
- `KSelecter::KSelecter(n)` @0x4840: `epoll_create(n)` @0x4882, mảng sự kiện `n × 12` byte.
- Mỗi socket sẵn sàng: `data == NULL` → `HandleNewConnection`; ngược lại khoá riêng → `RecvData` → `KUniQueue::Push`.
  Cuối vòng: `ProcessEventList` (xả những chỉ số đã xếp hàng).

### 2.4 Đệm — `KBuffer`/`KPackBuffer`

| Hàm | Địa chỉ | Việc làm |
|---|---|---|
| `KBuffer::KBuffer(max)` | 0x3470 | xoá hết, **`+0x14 = max`** (trần), `+0x10 = 0` |
| `KBuffer::Init` | 0x35a0 | `realloc(p, 0x1000)` — **khởi đầu 4 KB**, `+0xc = 0x1000`, `+0x10 = 0`, `+0x18 = 0` |
| `KBuffer::Extend` | 0x3710 | nhân đôi từ 4 KB tới khi đủ, không vượt `+0x14`; `Adjust()` rồi `realloc` |
| `KBuffer::Clear` | 0x38c0 | `+0x10 = 0` |
| `KServer::Create(n, bufSize)` | 0x6800 | `n == 0 || bufSize == 0` → NULL; `new KServer(0x20c)(n, bufSize)` |
| `KServer::KServer` | 0x6290 | `KUniQueue(n)`, vòng chỉ số `n×4` B, `vector<KCONNECTION>::reserve`, **mỗi kết nối 2 × `new KPackBuffer(bufSize)`** + `KThreadLock` |

`bufSize` là **trần** (không phải cấp phát sẵn): kết nối bình thường 4 KB, kết nối dồn ứ nở dần tới `bufSize`.

### 2.5 Nhật ký & tín hiệu — `KServer::Initialize` @0x5960

`mkdir("./Logs", 0777)`, mở `Logs/heaven_%d_%d_%4d%02d%02d.log`, ghi `heaven(version %d.%02d) log start....`,
`signal(SIGPIPE, SIG_IGN)`. Không có thiết lập mạng nào khác ở đây.

---

## 3. 🔴 Ngưỡng "sắp tràn thì xả" bằng 0 — bản Linux gửi gần như từng gói

### 3.1 Bằng chứng

**(a) Ai ghi vào `KBuffer+0x10`?** Quét **toàn bộ** hàm định nghĩa trong `libheaven.so` tìm mọi lệnh đụng `[reg+0x10]`:

```
KBuffer::KBuffer   0x3494  mov [eax+0x10], 0
KBuffer::Init      0x35e7  mov [esi+0x10], 0
KBuffer::Clear     0x3903  mov [esi+0x10], 0
```

Mọi chỗ còn lại (`KServer::*` so sánh `[reg+0x10]` với `-1`, `KUniQueue`, `vector::_M_insert_aux`) là trường
`KCONNECTION+0x10` (socket) hoặc cấu trúc khác, **không phải `KBuffer`**. `Extend`, `PushData`, `Shrink`, `GetFree`,
`PopData`, `GetData`, toàn bộ `KPackBuffer::*` — **không hàm nào ghi `+0x10`**. `librainbow.so` là cùng mã. ⇒ `+0x10 == 0` mãi mãi.

**(b) `KServer::PackData` @0x42e0 dùng nó thế nào:**

```
004302  mov eax, [esi+0x1c]        ; conn->trạng thái
004305  test eax, eax ; jne trả 0  ; kết nối hỏng thì thôi
004309  mov edx, [esi+4]           ; conn->đệm GỬI (KPackBuffer*)   (đợt 4 đã kiểm KCONNECTION+4)
00430c  mov eax, [edx+8]           ; đang dùng
00430f  add eax, ecx               ; + cỡ gói mới
004311  cmp eax, [edx+0x10]        ; so với 0
004314  jae 0x4368                 ; không dấu: (dùng + cỡ) >= 0  -> LUÔN NHẢY
004368  ... call KServer::SendData(conn, &state)     ; XẢ NGAY
        test eax ; je 0x4332 (thất bại -> xếp hàng, trả 0) ; jmp 0x4316 -> PushPack gói mới -> KUniQueue::Push
```

`MultiPackData` @0x43f9 cùng phép so sánh. `KServer::SendData` @0x4050 có chặn rỗng (`GetData` trả `ptr==0` hoặc
`len==0` → thoát không gọi `send`, 0x408e–0x409b), nên gói **đầu tiên** của một nhịp không tốn syscall.

**(c) Client y hệt** — `KClient::SendPackToServer` @0x3390:

```
0033e7  mov eax, [edi+8]           ; conn->đệm gửi
0033f0  mov eax, [ebp+0x10]        ; cỡ gói
0033f3  add eax, [ecx+8]           ; + đang dùng
0033f6  cmp eax, [ecx+0x10]        ; so với 0
0033f9  jae 0x3440                 ; -> KClientManager::SendData trước, rồi PushPack
```

### 3.2 Hành vi thật (dựng lại từ (a)+(b)+(c))

Với **N gói** cho cùng một client trong một nhịp game của bản Linux:

```
PackData(gói 1):  đệm rỗng -> SendData bỏ qua           -> xếp gói 1 -> xếp hàng idx
PackData(gói 2):  SendData -> send(gói 1) ngay, luồng GAME -> xếp gói 2
PackData(gói 3):  SendData -> send(gói 2)                  -> xếp gói 3
...
cuối vòng 10 ms:  ProcessEventList -> send(gói N)
```

⇒ **N−1 lần `send()` đồng bộ trên luồng game + 1 lần ở luồng mạng**, mỗi lần `send` kèm một lượt `KPackBuffer::Encode`
(bộ mã hoá bảng 5.679 mục) cho phần vừa xếp. Vòng 10 ms chỉ đảm nhận **gói cuối** và phần gửi thiếu (`EAGAIN`).
Trên dây, vì **không** `TCP_NODELAY`, kernel dùng Nagle: gói 1 đi ngay, gói 2…N−1 **đợi ACK của gói 1** (≤ 1 RTT, hoặc
tới khi client ACK trễ — Windows 200 ms nếu client không có gì gửi lại). Đó là lý do họ không cần đặt `TCP_NODELAY`:
Nagle chính là bộ gom của họ.

Cùng tình huống ở ta: `PackDataToClient` chỉ chép vào `pWriteBuffer` (xả cưỡng bức chỉ khi > 10.208 B), cuối `MainLoop`
**một `WSASend`** cho cả N gói, `TCP_NODELAY` bật ⇒ **1 syscall, 1 lượt mã hoá, không Nagle**; giá phải trả là gói sinh
ra giữa nhịp chờ tới cuối nhịp (đo đợt 4: 8–9 ms thường, 20–31 ms lúc xấu). Với 300 người × ~20 gói đồng bộ/nhịp,
bản Linux tốn ~6.000 `send()`/nhịp trên luồng game; ta tốn 300.

### 3.3 Những câu ở đợt 1–4 phải sửa vì phát hiện này

| Chỗ | Câu cũ | Sửa thành |
|---|---|---|
| Đợt 1 §0 bảng, dòng "Nhịp xả gói xuống client" | "Linux ≤ 10 ms, độc lập nhịp game" | ≤ 10 ms chỉ cho **gói cuối** mỗi kết nối; các gói trước đi ngay trên luồng game, rồi bị Nagle giữ ≤ 1 RTT |
| Đợt 1 §2.2 | "`KUniQueue` … là cơ chế gộp ghi tự viết" | `KUniQueue` chỉ chống trùng chỉ số cho lượt quét cuối vòng; **không gộp** |
| Đợt 1 §2.5 | "hẹn xả ở cuối vòng lặp (≤ 10 ms)" | hẹn xả **gói cuối**; gói trước đã xả ngay trong `PackData` |
| Đợt 1 §6.2 | "`SendPackToServer` chỉ gọi `SendData` ngay khi đệm sắp tràn, còn lại để vòng 10 ms gom" | gọi `SendData` **mọi lần**; client 2.0/Linux cũng gửi gần từng gói |
| Đợt 1 §7 hai dòng "Xả server→client ≤ 10 ms" / "Xả client→server gom 10 ms" | | như trên |
| Đợt 1 §8.1, §19.4 #6 "tách nhịp xả khỏi nhịp game (10 ms thay vì 55 ms)" | tiền đề "Linux gom 10 ms" | tiền đề sai; đề xuất **rút** (đợt 4 S10 đã thu về "xả thêm sau `MessageLoop`") |
| Đợt 2 §10.2 | "`+0x10` = ngưỡng xả" | trường luôn 0, chưa bao giờ được gán ⇒ ngưỡng thực = "xả mọi lần" |
| Đợt 2 §14.1 | "Linux 1 lần chép, `send()` thẳng" | đúng về chép, nhưng **N lần `send` + N lần `Encode`** mỗi client mỗi nhịp |
| Đợt 4 §5 "Nagle đã gom giúp ở tầng TCP rồi" | dùng để bỏ đề xuất gom gói client | vẫn đúng là bỏ, nhưng lý do là bản tham khảo **cũng không gom**; Nagle là chỗ dựa của họ, không phải ưu điểm |

Kết luận "**ta đang hơn ở khung theo lô + xả cưỡng bức + `TCP_NODELAY`**" (đợt 3 §19.5) **được củng cố thêm** chứ không bị lật.

---

## 4. `jx_linux_y` — cấu hình và hằng số (lần đầu đọc được)

### 4.1 Đường vào — `main` @0x804b880 → `KSOServer::InitNetwork` @0x804c910

`main` đọc argv: `-v`, `gm` (đặt cờ `[esp+0x1c] = 1`), số nguyên (`strtol`) → **cổng ghi đè**. Gọi
`InitNetwork(this, port_override, gm_flag)` @0x804b984. (Tham số thứ ba là cờ GM, **không phải** cỡ đệm.)

`InitNetwork` (đọc từ 0x804c910 tới 0x804cdf6):

| Địa chỉ | Việc |
|---|---|
| 0x804c94f | mở `package.ini` |
| 0x804c95b | mở nhật ký `Logs/network` |
| 0x804cc8e / 0x804ca8e | `dlopen("./lib%s.so")` với `heaven`, `rainbow` |
| 0x804c98b / 0x804c9b1 | `dlsym("CreateServer")` → `[this+0x34]`; `dlsym("CreateClientManager")` → gọi `CreateClientManager(6, &[this+0x24])` — **6 khe client** (5 liên kết + dự phòng) |
| 0x804ca05 | nạp **`servercfg.ini`** ("Failed to load %s!" nếu thiếu) |
| 0x804cadc | `GetInteger("GameServer", "Port", **0x1a0a = 6666**, &[this+0x3c])` — giống `KSOServer.cpp:487` của ta từng chữ; argv ghi đè nếu > 0 |
| 0x804cb1d | `GetString("FixIp", "IntranetIp", …)`; rỗng → `"Can't get server ip"` |
| 0x804cb56 | `GetInteger("Overload", "MaxPlayer", **0x4b0 = 1200**, &[this+0x38])` — **không có khoá `Precision`** (chuỗi không tồn tại trong tệp) |
| 0x804cb81 | cấp mảng trạng thái `MaxPlayer × 20` byte |
| 0x804cc67 | `ServerCore::Create(&[this+0x44], MaxPlayer, gm)` @0x80527a0 → `new (0x300b4 B)` |
| 0x804cd44 | `ServerCore->Initialize(this)` (bảng ảo 0x8250e08, khe 0 = 0x8051490) |

`ServerCore` (196 KB) chứa một mảng dựng sẵn **128 KB** (`memset(this+0x24, 0, 0x20000)` @0x80526b4; chuỗi assert
`… <= countof(m_bySendBuffer)`) — đệm lắp gói, không phải trần đệm kết nối.

**Chưa lần ra**: hằng `bufSize` mà `jx_linux_y` truyền cho `KServer::Create(MaxPlayer, bufSize)` (đường gọi qua con trỏ
`[this+0x34]` nằm sâu hơn ba tầng đã theo). Biết chắc: mỗi kết nối người chơi **khởi đầu 4 KB, nhân đôi tới trần đó**.

### 4.2 Năm liên kết máy chủ-máy chủ — `LoadLink(ini, section, out)` @0x804c320

```
GetString (ini, sec, "Ip",         mặc định "192.168.26.1", out+0, 16)     0x804c35b
GetInteger(ini, sec, "Port",       mặc định = out+0x10 (đặt trước = 0))    0x804c37d
GetInteger(ini, sec, "BufferSize", mặc định = out+0x14 (đặt trước = 0x500000 = 5 MB))   0x804c39f
cmp [out+0x14], 0x13fffff ; jg bỏ qua ; mov [out+0x14], 0x1400000          0x804c3a4–0x804c3ad
```

⇒ **`BufferSize` liên kết = max(cấu hình, 20 MB)**. Mặc định 5 MB ghi ở nơi gọi (`mov [ebp-0x28], 0x500000`
@0x804ce1c, 0x804ce84…) **bị sàn 20 MB vô hiệu hoá** — dấu vết của một lần vá "nâng sàn" sau khi vận hành gặp tràn
đệm liên kết. Nơi gọi: `Gateway` @0x804ce36, `Database` @0x804ce9e, `Transfer` @0x804cef2, `Chat` @0x804cf46,
`Tong` @0x804cf9a — **trùng khít 5 liên kết của `KSOServer.cpp:690–906`**.

### 4.3 Nhịp game, thoát, ping liên kết

- **FPS = 18**: kiểm cảnh báo `"[Warning...] GameServer' FPS=%d"` @0x804d227–0x804d294 tính `(x>>2)*18`
  (`lea eax,[edx+edx*8]; add eax,eax`); nhật ký `GameLoop= %06u` mỗi 1.024 vòng, mốc giờ `0xfd20 = 64 800 = 18 × 3 600`.
  Giống `GAME_FPS 18` của ta (`KSOServer.cpp:115`).
- Thoát: `"GameServer exit..."` → `usleep(0x4c4b40 = 5 000 000)` = **chờ 5 s** trước khi kết thúc @0x804d27f.
- **Ping liên kết** @0x81e2000: mỗi **10 000 ms** (`cmp ecx, 0x2710`) gửi gói 10 byte `{0x0F, 0x2B, seq, now}`;
  nếu lần trước chưa được trả lời → chỉ ghi `"[%s] Ping Lost! Start-Last(%d)"` (**không đóng**). Bên nhận @0x81e2080
  ghi `"[%s]receive ping[%u] interval:%u ms."`. Ta: **không có nhịp tim nào trên liên kết** Goddess/Bishop/Transfer/Chat/Tong
  (chỉ có ping người chơi `c2s_ping`/`s2c_ping`).
- `S2SSyncRelayD` (relay thế hệ mới, đọc khoá): `[Gmc]/[Bishop] Port, ReConnInterval, PingInterval`;
  `[Goddess] CheckConnInterval`; `[Host]/[Chat]/[Tong] BufferSize, ListenPort, SendRecvTimeout`;
  `[RuntimeStat] FlushIntervalSec` — họ **cấu hình được** nhịp ping/kết nối lại/hết giờ gửi-nhận; ta gắn cứng.

### 4.4 Client Linux/2.0 — `KClient::ConnectTo` @0x37a0, `KClientManager::Work` @0x4b20

```
socket(2,1,0) ; ModifySocket (FD_CLOEXEC) ; [Bind nếu có ip cục bộ]
connect(...)                 <- CHẶN, hết giờ theo hệ điều hành
UnblockSocket (O_NONBLOCK)   <- SAU khi nối
log "connect to [%s:%d]: ok" ; KPackBuffer::Init x2 ; ip/port
Work: lặp { Loop(); usleep(10 000) } ; thoát: SendAll()
Loop: mỗi kết nối: khoá -> (lần đầu: callback 0x100) -> RecvData -> SendData ; lỗi -> CloseConnection
```

Windows (`rainbow.dll` kgc/net): `ioctlsocket(FIONBIO)`, `Sleep(10)` — cùng mô hình. Không có tuỳ chọn socket nào.

---

## 5. Thiết lập của TA — bảng đầy đủ (mã `D:\GAMEDEVNEW` @a22873b8 + cấu hình đang chạy)

### 5.1 Máy chủ `heaven.dll` (`CIOCPServer` / `OnlineGameLib::CSocketServer`)

| Thiết lập | Giá trị | Nơi |
|---|---|---|
| Số luồng IOCP | **CPU × 2 + 2** | `Common/SocketServer.cpp:48` |
| Độ đồng thời IOCP | `m_iocp(0)` → `CreateIoCompletionPort(…, 0)` = **số CPU** | `SocketServer.cpp:67`, `IOCompletionPort.cpp:14` |
| Chờ hoàn thành | `GetQueuedCompletionStatus(INFINITE)` | `IOCompletionPort.cpp:59` |
| Socket nghe | `WSASocket(…, WSA_FLAG_OVERLAPPED)` | `SocketServer.cpp:183–188` |
| **Hàng chờ `listen`** | **5** | `SocketServer.cpp:203` |
| Mô hình nhận | `WSAAccept` chặn, **từng kết nối một**, trên luồng `Run()`; mỗi lần `Allocate()` một `CIOBuffer` 10 KB chỉ để chứa `sockaddr` | `:255–296` |
| `TCP_NODELAY` | **có**, mọi socket vừa nhận `[DELTA 07/09]` | `:277` |
| `SO_LINGER {1,0}` | khi đóng gấp (RST) | `:671–680`, `Socket.cpp:66–77` |
| `SO_REUSEADDR` / `SO_EXCLUSIVEADDRUSE` / `SO_KEEPALIVE` / `SO_SNDBUF` / `SO_RCVBUF` | **không có** | grep toàn `Sources` |
| Cỡ đọc mỗi `WSARecv` (`bufferSize`) | **10.240** | `Heaven/ServerStage.h:88` |
| Bể đệm đọc (free list) | tối đa **10.240** đệm × 10 KB | `ServerStage.cpp:1459` |
| Ngưỡng xả cưỡng bức đường gửi | `bufferSize − 32` = **10.208** — suy từ **cỡ đọc**, không phải từ cỡ đệm cache | `ServerStage.cpp:247`, `:700–716` |
| Đệm cache mỗi kết nối (recv/read/write, 3 cái) | GameServer **16.384** · Goddess **4 MB** · **Bishop 8.192** · S3Relay theo `relay_config.ini` (đang chạy **1 MB**; nếu thiếu khoá → mặc định **8.192**) | `KSOServer.cpp:121`, `Goddess.cpp:719`, `Intercessor.cpp:432`, `S3Relay/Global.cpp`, `ServerStage.cpp:1457` |
| Bể đệm cache (free list) | GameServer **10** (`m_snMaxBuffer`) · Goddess 20 · Bishop 1000 · S3Relay 15 | `KSOServer.cpp:120,554` … |
| Số kết nối tối đa | GameServer `MaxPlayer + Precision`: **đang chạy 290 + 10 = 300** mỗi GameServer (`GameServer1_cfg.ini`, `GameServer2_cfg.ini`; mặc định mã 450+20 `WIN32`, 1000+200 khác) · Goddess 10 · Bishop **500 gắn cứng** (`Application.cpp:175`) · S3Relay 10+1 | `KSOServer.cpp:510–521` |
| `Precision` | 10 (0 → 10) | `ServerStage.cpp:1454` |
| Cổng | `[GameServer] Port` mặc định 6666, đang chạy 6667 (GS1) | `KSOServer.cpp:487` |
| `GAME_FPS` | 18 | `KSOServer.cpp:115` |
| Nhịp xả | `SendPackToClient(-1)` cuối `MainLoop` (18 lần/giây); `MessageLoop()` mỗi vòng `Breathe` | `:3341`, `:1103` |
| Ping người chơi | gửi mỗi **5 s**, hết giờ **60 s**, lặp `m_pingCount` lần; chưa đăng nhập hết giờ **15 s** | `:3311–3330` |
| Nhịp tim liên kết máy chủ-máy chủ | **không có** | grep `Ping` trong `KSOServer.cpp` |

### 5.2 Client `Rainbow.dll` (`CGameClient` / `CSocketClient`) — dùng cho **cả game client lẫn 5 liên kết** của GameServer

| Thiết lập | Giá trị | Nơi |
|---|---|---|
| Socket | `WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, **0**)` — không overlapped; chờ bằng `WSAEventSelect(FD_CONNECT\|FD_CLOSE\|FD_READ)` | `Common/SocketClient.cpp:173`, `:103` |
| `connect` | `::connect` **chặn**, hết giờ theo Windows (~21 s) | `Socket.cpp:120–128` |
| Chờ gói khoá (34 B) | `select` **5 s** (`g_dwTimeout = 5`, chú thích cho thấy trước là 1000) | `SocketClient.cpp:547–582` |
| `Write` | `WSASend` đồng bộ; `WOULDBLOCK` → `select` ghi được **5 s** → thử lại, **lặp tới khi gửi xong hoặc shutdown** (không bỏ cuộc) — chạy trên **luồng gọi** (luồng chính của game client; **luồng game của GameServer** với 5 liên kết) | `:412–545` |
| `TCP_NODELAY` | **không có** | grep |
| `SO_LINGER {1,0}` | khi `StopConnections` | `:134–145` |
| Đệm nhận/cache | `SetEnvironment(x)` → **ép ≥ 4 MB** `[RECV 04/09]`; `CGameClient(2, 2, ≥4 MB, 10240)` | `Rainbow/ClientStage.cpp:486–493` |
| Cỡ xin cho 5 liên kết | Gateway 8 MB · Database 512 KB→4 MB · Transfer 4 MB · Chat 4 MB · Tong 1 MB→4 MB | `KSOServer.cpp:690, 739, 796, 850, 906` |
| Chống dồn ứ đường nhận | `[RECV 04/09]` vòng chờ `Sleep(1)` tối đa 30.000 lượt rồi đóng | `ClientStage.cpp:410–442` |
| Gửi | **từng gói, ngay** (`SendPackToServer` → `Write`) | `ClientStage.cpp:171–205`, `S3Client/NetConnect/NetConnectAgent.cpp:269` |

Ghi chú về cờ `0` (không overlapped): trên Win32 **giữ nguyên** — luồng I/O chờ trên EVENT nên không có `select` treo
handle; bản SDL phải đổi sang `WSA_FLAG_OVERLAPPED` vì thay EVENT bằng `select` (bộ nhớ `jx1-sdl-socket-overlapped-0809`).

---

## 6. Đề xuất thiết lập lại — xếp theo lợi/rủi (CHƯA LÀM, chờ chủ)

Tất cả **không đụng giao thức**, không đụng client cũ, trừ mục 4 chỉ dựng lại `Rainbow.dll`.

### 6.1 `listen(5)` → `SOMAXCONN` — `SocketServer.cpp:203` · chắc chắn · rẻ nhất

Winsock tôn trọng backlog đúng bằng số đưa vào: **5** kết nối đang bắt tay được xếp hàng trong khi luồng `Run()` nhận
**từng cái một** (`WSAAccept` chặn, cấp `CIOBuffer`, gọi `OnConnectionEstablished`). Máy chủ khởi động lại, đầu trận
Tống Kim, 300 client nối lại trong vài giây ⇒ hàng chờ đầy ⇒ SYN bị vứt ⇒ client gửi lại SYN sau 1–3 s ⇒ "đăng nhập
lâu", "vào trận chậm", mà nhật ký máy chủ **không ghi gì**. Bản Linux dùng 10 (kernel Linux còn nâng theo `somaxconn`).
Đề xuất `SOMAXCONN` (Windows 8+/2012+ tự chọn mức hợp lý, hoặc `SOMAXCONN_HINT(500)`). Chi phí 0, rủi ro 0.
Cách đo trước/sau: đếm thời gian từ `connect` tới nhận gói khoá của client lúc 300 người cùng vào.

### 6.2 Đệm cache Bishop `1024*8` → `1024*16` — `Bishop/Intercessor.cpp:432` · chắc chắn · sửa lỗi mất dữ liệu **hai chiều**

- Đường **nhận**: mỗi `WSARecv` mang về tới 10.240 B, `ReadCompleted` (`ServerStage.cpp:1263`) `AddData` không kiểm ⇒
  một lần đọc đầy vào đệm 8.192 B là **vứt im lặng** (đợt 4 M2).
- Đường **gửi** (mới thấy đợt này): ngưỡng xả cưỡng bức là **10.208** (`bufferSize − 32`, tính từ cỡ đọc), nhưng
  `pWriteBuffer` của Bishop chỉ **8.192** ⇒ nhánh xả **không bao giờ** kịp chạy; `AddData` vứt im lặng khi lô vượt 8.192 B.
- Quy tắc để ghi vào `ServerStage`: **`bufferSize_Cache ≥ bufferSize + 32`** (≥ 10.272); nên đặt bằng GameServer = 16.384
  (Bishop giữ 500 khe × 3 × 16 KB = 24 MB, không đáng kể). Kèm một dòng kiểm ở `CServerFactory::CreateInstance` để
  in cảnh báo nếu vi phạm — tránh lặp lại với S3Relay khi ai đó xoá khoá `buffersize` trong `relay_config.ini`
  (mặc định lúc đó là 8.192, `ServerStage.cpp:1457`).

### 6.3 Bể đệm cache GameServer `m_snMaxBuffer = 10` → `≥ 3 × MaxPlayer` — `KSOServer.cpp:120` · chắc chắn

Mỗi kết nối chiếm **3** đệm cache (recv/read/write, 3 × 16 KB) lấy từ `m_theCacheAllocator`; khi trả về, free list chỉ
giữ **10** cái (`IOBuffer.cpp:206–214`), số còn lại `delete`. Cuối trận Tống Kim 300 người thoát = **900 lần `delete`
16 KB**, vào lại = 900 lần `new` + memset. Khối 16 KB nằm ngoài Low-Fragmentation Heap ⇒ heap chính phân mảnh dần
qua nhiều ngày chạy. Đặt ≥ 900–1000 (Bishop đã dùng 1000; `0` = không giới hạn) giữ RAM ở đỉnh ≈ 300 × 48 KB = 14 MB — không đáng kể.

### 6.4 `TCP_NODELAY` phía client — `SocketClient.cpp` sau dòng 173 · **đo trước** rồi mới quyết

Client gửi **từng gói ngay** và **không** tắt Nagle: gói thứ 2 trở đi trong cùng một loạt (di chuyển + chiêu + nhặt
trong một khung hình; WAuto/bot còn dày hơn) **đợi ACK của gói 1**. ACK từ máy chủ Windows đi kèm dữ liệu của nhịp kế
(≤ 55 ms) hoặc theo đồng hồ ACK trễ 200 ms nếu máy chủ không có gì gửi. Bật `TCP_NODELAY` bỏ cái đợi này; giá là mỗi
gói thành một segment riêng (lưu lượng client nhỏ, không đáng kể). Bản 2.0/Linux cũng không bật — nhưng như §3 cho thấy,
họ dựa vào Nagle vì gửi từng gói ở cả hai phía, không phải vì đó là lựa chọn tốt hơn.
Chỉ dựng lại `Rainbow.dll` phía client; máy chủ không đổi. **Đo bằng bộ có sẵn** `ReverseTools/mobile_x64/test_rainbow/`
(`chay_apluc.ps1`, `test_rainbow.exe … 127.0.0.1 3`) — so RTT của **cặp gói liên tiếp** trước/sau, trên mạng thật
(loopback không lộ Nagle). Nếu số đo không khác thì thôi.

### 6.5 `SO_REUSEADDR`: **vẫn không đặt** — đính chính §8.4 đợt 1

Trên Linux `SO_REUSEADDR` chủ yếu cho phép `bind` lại cổng còn trong `TIME_WAIT`. Trên Windows nó cho phép **socket thứ hai
bind chồng lên cổng đang có người nghe** (chiếm cổng), và Windows vốn không chặn `bind` lại cổng nghe sau khi khởi động lại
(chỉ các kết nối đã nhận mới nằm `TIME_WAIT`). Đặt nó không mua được gì và mở một lỗ. Nếu muốn "học" thì đặt
**`SO_EXCLUSIVEADDRUSE`** trên socket nghe — cấm chiếm cổng, hiệu năng không đổi.

### 6.6 Nhịp tim liên kết máy chủ-máy chủ — tuỳ chọn, mức vừa

Bản Linux ping 5 liên kết mỗi 10 s (§4.3); ta không có gì: liên kết Goddess/Bishop/Relay nửa chết (rớt cáp, NAT quên)
chỉ lộ khi một lần gửi thất bại — mà `CSocketClient::Write` lại **lặp chờ vô hạn** (§5.2), tức luồng game GameServer
đứng. Cách rẻ, không đụng giao thức: `setsockopt(SO_KEEPALIVE)` + `WSAIoctl(SIO_KEEPALIVE_VALS, {onoff, 30 000, 5 000})`
trên socket của `CSocketClient` (áp cho cả game client, vô hại). Cách đầy đủ hơn (ping ứng dụng như Linux) là **đổi giao
thức nội bộ** — chỉ làm nếu chủ muốn.

### 6.7 Ghi lại cho lần sau (không phải thiết lập, không làm ngay)

- `CSocketClient::Write` chặn luồng gọi tới khi gửi được (§5.2). Với game client = treo giao diện khi máy chủ ngừng đọc;
  với GameServer = **treo luồng game khi Goddess/Bishop ngừng đọc**. Đây là thiết kế, không phải một con số; ghi để biết
  khi nào thấy GameServer "đứng im" mà CPU rảnh.
- Trần đệm liên kết: Linux ép sàn **20 MB**; ta ép sàn 4 MB `[RECV 04/09]` **kèm chờ dồn ứ** — ta an toàn hơn về cơ chế,
  nhỏ hơn về số. Nâng lên 8–16 MB là tuỳ, không cấp thiết.

---

## 7. KHÔNG nên đổi — và vì sao

| Thiết lập | Lý do giữ nguyên |
|---|---|
| `SO_SNDBUF` / `SO_RCVBUF` | Không bên nào đặt. Windows tự điều chỉnh cửa sổ; 37 KB/s mỗi người (bàn giao 07/09) là ~1,7 s dữ liệu trong 64 KB mặc định. Mẹo `SO_SNDBUF = 0` (bỏ một lần chép trong kernel) chỉ tiết kiệm phần đã đo ≈ 0,01 ms/nhịp (đợt 4 S8) và dễ gây nghẽn khi chỉ có một `WSASend` treo. |
| Số luồng IOCP `CPU×2+2`, đồng thời = số CPU | Hàm hoàn thành chỉ `memcpy` + `AddData` + khoá; dư luồng vô hại. Chỉ khi `[GUI-DO]` cho thấy tranh khoá thì thử `CPU×1` — đo, không sửa mò. |
| Cỡ đọc 10.240, khung `[WORD tổng][lô]`, 1 `WSASend`/client/nhịp, `TCP_NODELAY` máy chủ | §3: đây là chỗ **ta hơn** bản tham khảo. Đừng chuyển sang gửi từng gói, đừng bỏ `TCP_NODELAY`. |
| Nhịp xả cuối `MainLoop` | Tiền đề "Linux xả 10 ms độc lập" sai (§3.3). Việc duy nhất còn đáng là đợt 4 S10 (xả thêm sau `MessageLoop`) — là mã, không phải thiết lập. |
| Ping người chơi 60 / 5 / 15 s | Hợp lý; Linux dùng 10 s cho liên kết, không có số cho người chơi để so. |
| `MaxPlayer + Precision` | Linux dùng thẳng `MaxPlayer` (1200); ta 290 + 10 mỗi GameServer theo tải thật. Không có gì để chép. |
| Bảng khoá mã hoá 5.679 mục, bắt tay 42 byte, `_Rand` | như đợt 1–4: đổi giao thức / giống hệt ta. |

---

## 8. Bảng đối chiếu tổng (chỉ tham số)

| Tham số | Linux (`libheaven`/`jx_linux_y`) | 2.0 / Windows kgc (`rainbow.dll`) | Ta | Nên |
|---|---|---|---|---|
| Tuỳ chọn socket nghe | `SO_REUSEADDR` | — | không | `SO_EXCLUSIVEADDRUSE` (tuỳ) |
| Tuỳ chọn socket nhận | không | — | `TCP_NODELAY` | giữ |
| Tuỳ chọn socket client | không | không | `SO_LINGER` khi đóng | + `TCP_NODELAY` (đo) |
| `listen` backlog | 10 | — | **5** | `SOMAXCONN` |
| Luồng mạng | 1 (epoll) | 1 (Sleep 10 ms) | CPU×2+2 (IOCP) | giữ |
| Chờ I/O | 10 ms | 10 ms | INFINITE (IOCP) | giữ |
| Số gửi mỗi client mỗi nhịp | ~N (từng gói, §3) | ~N | **1** | giữ |
| Đệm kết nối người chơi | 4 KB → nhân đôi → trần (chưa đọc được) | như Linux | 16 KB cố định ×3 | giữ (Bishop 8 KB → 16 KB) |
| Đệm liên kết máy chủ-máy chủ | sàn **20 MB** | — | sàn 4 MB + chờ dồn ứ | giữ |
| Bể đệm cache | không có (realloc) | — | GS 10 / Bishop 1000 | GS ≥ 3×MaxPlayer |
| MaxPlayer mặc định | 1200, không Precision | — | 450+20 (mã), 290+10 (chạy) | giữ |
| Cổng mặc định | 6666 | — | 6666 | — |
| FPS | 18 | — | 18 | — |
| Ping người chơi | (không đọc) | — | 5 s / 60 s / 15 s | giữ |
| Ping liên kết | 10 s, mất chỉ log | — | không | keepalive TCP (tuỳ) |
| `connect` client | chặn | chặn | chặn (~21 s) | giữ |
| Hết giờ bắt tay client | — | `WaitForSingleObject` 60 s (luồng) | 5 s | giữ |
| Thoát máy chủ | chờ 5 s | — | — | — |

---

## 9. Cách kiểm chứng lại

Thư mục nháp phiên: `C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\8b57a9a4-…\scratchpad\`.

```bash
# bảng nhập + tháo mã mọi hàm mang thiết lập (libheaven/librainbow) + chuỗi cấu hình jx_linux_y
python net_settings_bin.py > net_settings_bin.out
# xref chuỗi cấu hình trong jx_linux_y (Port 6666, BufferSize sàn 20 MB, MaxPlayer 1200, FPS 18, ping 10 s)
python xref_cfg.py ; python xref_more.py ; python xref_last.py ; python xref_main.py
# bảng nhập PE theo ordinal của rainbow.dll Windows
python pe_imports.py D:/ServerLinux/gateway/rainbow.dll D:/ServerLinux/Patch/rainbow.dll
# ai ghi KBuffer+0x10 (phải chỉ thấy ctor/Init/Clear)
python -c "import sys;sys.path.insert(0,'.');from dis3 import Lib;import re;h=Lib('D:/ServerLinux/server1/libheaven.so');p=re.compile(r'\[(e[abcd]x|esi|edi) \+ 0x10\], ');[print(n,hex(a),t) for n,(v,s) in h.by_name.items() if 'KBuffer' in n for a,t,x in h.full(n,800) if p.search(t)]"
```

Mốc địa chỉ đợt này — `libheaven.so`: `OpenService` 0x44c0 (`setsockopt` 0x4548, `listen(10)` 0x45b3),
`HandleNewConnection` 0x5b40, `Loop` 0x5d50 (`epoll_wait 10 ms` 0x5d72), `KSelecter` ctor 0x4840, `KBuffer` ctor 0x3470,
`KBuffer::Init` 0x35a0 (4 KB), `KServer::Create` 0x6800, `PackData` 0x42e0 (`cmp … [edx+0x10]` 0x4311), `SendData` 0x4050.
`librainbow.so`: `ConnectTo` 0x37a0, `Work` 0x4b20 (`usleep 0x2710`), `SendPackToServer` 0x3390 (`cmp` 0x33f6).
`jx_linux_y`: `main` 0x804b880, `InitNetwork` 0x804c910, `Port` 0x804cadc, `MaxPlayer` 0x804cb56, `LoadLink` 0x804c320
(sàn 20 MB 0x804c3a4), `ServerCore::Create` 0x80527a0, bảng ảo `ServerCore` 0x8250e08, FPS 0x804d227, ping liên kết 0x81e2000.
