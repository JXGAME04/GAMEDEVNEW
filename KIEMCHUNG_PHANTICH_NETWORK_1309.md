# KIỂM CHỨNG LẠI `PHANTICH_NETWORK_2.0_LINUX_VS_DUAN_1309.md`

**Ngày 13/09/2026 · đợt 4 · CHỈ ĐỌC, KHÔNG SỬA MÃ** (theo yêu cầu của chủ: "đọc và phân tích lại, không fix code").

Đây **không phải bản tóm tắt** tài liệu cũ. Đây là bản đối chiếu từng khẳng định của tài liệu với **mã nguồn đang chạy**
và với **nhị phân Linux/2.0**, để tách ra: cái gì đã chắc, cái gì sai phải bỏ, cái gì tài liệu chưa soi tới.

Cách kiểm:

- **Mã nguồn:** git HEAD `a22873b8`, cây sạch; các tệp mạng (`Heaven/`, `Rainbow/`, `Common/`, `KSOServer.cpp`)
  lần cuối đổi ở `9538e72c` (07/09) ⇒ mọi số dòng tài liệu trích vẫn khớp mã hiện tại.
- **Nhị phân:** chạy lại `verify_net_bin.py` (capstone 5.0.7) — ánh xạ `PT_LOAD` của `jx_linux_y`, đọc bảng hàm ảo,
  so bảng khoá với `gamecl.exe` 2.0 từng byte, tháo mã thân hàm mã hoá.

---

## 1. Đã kiểm lại và ĐÚNG (không cần đọc lại phần này của tài liệu)

| Khẳng định | Bằng chứng đã đo lại |
|---|---|
| `CIOBuffer::AddData` âm thầm vứt cả khối khi đầy | `IOBuffer.cpp:90-97` — dòng `throw` bị chú thích, chỉ `return` |
| Khoá KSG **không cuộn** giữa các gói | `KSG_EncodeDecode.h:25-27, 36-38` — chép `*puKey` ra `uKey` cục bộ rồi mới gọi |
| Bắt tay `ACCOUNT_BEGIN` 32 byte, ServerKey +8, ClientKey +12, giấu bằng `~` | `Cipher.h:19-27`, `ServerStage.cpp:1650-1671` (2+32 = 34 byte trên dây) |
| `CPU×2+2` luồng IOCP | `SocketServer.cpp:42-52` |
| `TCP_NODELAY` đặt trên mọi socket nhận; **không** `SO_REUSEADDR` | `SocketServer.cpp:274-278`; grep cả `Common/` không có `SO_REUSEADDR` |
| Xả mỗi nhịp game, 18 fps | `KSOServer.cpp:3341`, `GAME_FPS` = 18 ở `:115` |
| Đường GỬI **đã có** xả cưỡng bức khi sắp đầy (đính chính §19.2 là đúng) | `ServerStage.cpp:700-716`, `m_nNetworkBufferMaxLen = bufferSize-32` `:247` |
| `_Rand` = LCG `214013/2531011` gieo `time(NULL)` | `ServerStage.cpp:214-221` |
| Client game thật sự dùng `Rainbow.dll` + `CreateInterface` | `NetConnectAgent.cpp:22, 70-75` |
| Client gửi **từng gói một**, mỗi gói một khung `[WORD][gói]` | `ClientStage.cpp:171-205` |

### 1.1 Phần mổ nhị phân — kiểm lại toàn bộ, ĐÚNG

Đây là phần tôi soi kỹ nhất vì nó là chỗ dễ sai nhất mà không ai phát hiện:

```
bang khoa VA 0x082DB760 -> tep 0x877760          (tai lieu ghi 0x877760)      DUNG
so muc tai VA 0x082E101C = 5679                  (tai lieu ghi 5679)          DUNG
bang khoa tim thay trong gamecl 2.0 tai 0x413C80; trung 22716/22716 byte      DUNG 100%
than 0x0804DA90 vs ban sao 0x0804D9F0: GIONG HET                              DUNG
```

Giả mã §11.2 của tài liệu khớp **từng lệnh** với mã máy:

| Giả mã tài liệu | Lệnh thật |
|---|---|
| `s = khoa` | `mov esi, [edx]` @0x804daaa |
| `s = BANG[(s + (n-1-i)) % 5679]` | `lea eax,[ebx+esi]` `div [0x82e101c]` `mov esi,[edx*4+0x82db760]` @0x804dab8-dac6 |
| `+ 0x2E6D23C1` | `add esi, 0x2e6d23c1` @0x804dacd |
| `word[i] ^= s` | `xor [ecx], esi` @0x804dad3 |
| đuôi `BANG[du % 5679] ^ s` | @0x804dae6-dafb |
| `khoá = khoá*31 + 0x08088405` | `shl 5 / sub / add 0x8088405` @0x804db14-db1c |

Và một điểm tài liệu nói đúng mà đáng nhấn: **bản Linux cũng KHÔNG cho khoá cuộn giữa các gói** — thunk
`0x0804D530` truyền `lea eax,[ebp+0x14]`, tức **địa chỉ bản sao khoá trên ngăn xếp**, nên phép cuộn khoá ghi vào
chỗ bỏ đi. Giống hệt `KSG_EncodeBuf` của ta. Hai bên yếu như nhau ở điểm này.

Ma trận hằng số §11.5 đo lại cũng khớp:

```
libheaven.so     23C1=-  8405=-  2399=Y  23CF=Y   (chi giu phan BAT TAY)
librainbow.so    23C1=-  8405=-  2399=Y  23CF=Y
jx_linux_y       23C1=Y  8405=Y  2399=-  23CF=-   (chi giu phan MA HOA NOI DUNG)
S2SSyncRelayD    23C1=Y  8405=Y  2399=Y  23CF=Y
gamecl 2.0       23C1=Y  8405=Y  2399=Y  23CF=Y
```

**Một lỗi nhỏ duy nhất ở §18:** bảng hàm ảo `KCoder2` không nằm ở `0x0825004C` — ô đó chứa **con trỏ typeinfo**
(`0x08250058`); hai khe thật ở `0x08250050` (= `0x0804d530`) và `0x08250054` (= `0x0804d500`). Địa chỉ hàm thì đúng,
chỉ nhãn "bảng hàm ảo" lệch 4 byte.

---

## 2. Những chỗ tài liệu SAI — phải sửa trước khi có ai hành động theo

### S1. "Đệm đầy thì đóng kết nối" — SAI (xuất hiện ở §0 bảng, §4.2, §7, §8.2, §19.1)

`ProcessDataStream` (`ServerStage.cpp:1286-1349`) chỉ in `Too much data!` và `Shutdown()` khi **đồng thời**:
`messageSize == 0` (WORD đầu khung đúng bằng 0) **và** `used == GetSize() - 1` (đệm thiếu đúng 1 byte là đầy).
Dồn ứ bình thường **không bao giờ** chạm nhánh này. Câu "`Too much data!` là nguyên nhân rớt kết nối khi dồn ứ"
(§8.2) do đó sai, và mục §19.1 dòng 3 ("một thông điệp dài hơn đệm đọc ⇒ đóng kết nối") cũng sai.

Điều thật sự xảy ra khi khung khai độ dài lớn hơn đệm: dữ liệu dồn tới đầy, `AddData` bắt đầu vứt im lặng,
`GetPackFromClient` **không bao giờ** trả được gói ⇒ kết nối **kẹt câm**. Ở GameServer còn có ping 60 s đá ra
(`KSOServer.cpp:3311, 3328-3336`); ở Goddess/Bishop tôi chưa thấy cơ chế tương đương (chưa soi kỹ).

### S2. §2.5 còn nguyên câu sai đã tự đính chính ở §19.2

§2.5 vẫn viết "`PackDataToClient` **chỉ chép vào** `pWriteBuffer`, không có nhánh sắp tràn thì xả ngay; tràn thì báo lỗi".
Cả hai vế đều sai (`ServerStage.cpp:700-716` có nhánh xả; tràn thì **im lặng**, không báo). §19.2 đã đính chính
nhưng §2.5 không sửa — ai đọc lướt phần đầu sẽ làm lại việc đã có.

### S3. §4.3 + §8.3 "lô gửi có thể chạm trần 65.535 byte" — KHÔNG THỂ XẢY RA

Xả cưỡng bức ở **10.208 byte** (`bufferSize - 32`, với `bufferSize` mặc định `1024*10` — `ServerStage.h:88`),
và đệm IOCP đem đi gửi cũng chỉ **10.240 byte**. Một lô không bao giờ vượt ~10,2 KB. Đề xuất số 3 ("đo lô lớn nhất
mỗi giờ") vì vậy bỏ hẳn, khỏi tốn công đo.

### S4. §3/§7/§8.8/§19.4 — hàng chờ `listen` của TA là **5**, nhỏ hơn Linux

`SocketServer.cpp:203`: `listeningSocket.Listen( 5 )`. Tài liệu để trống ô này ("tham số backlog") rồi đi khuyên
"**không nên bắt chước** `listen(10)` quá nhỏ của Linux" — ngược hướng. Nếu chỗ này có việc phải làm thì là nâng
của ta, không phải tránh của họ. (Mức độ: luồng `WSAAccept` chạy riêng và rỗng việc nên hàng chờ 5 hiếm khi đầy —
cần đo trước khi đụng, đừng sửa mò.)

### S5. §19.1 liệt kê "ghi thiếu byte" như lỗi còn treo

Đã vá 04/09: `SocketServer.cpp:1117-1131` gửi nốt phần còn lại, không gửi nổi thì đóng kết nối (commit `c6ca1f55`).
Để trong bảng "ba sự cố nặng nhất" mà không ghi "đã vá" dễ làm người đọc tưởng còn.

### S6. §13/§17.9/§19.4#5 "đổi nguồn hạt ngẫu nhiên" — giá trị gần **0**, không phải "trung bình/rẻ nhất đáng làm"

Khoá phiên được gửi cho client **ngay ở gói đầu**, chỉ giấu bằng `~` (`ServerStage.cpp:1666-1667`). Ai bắt được gói
đầu là có khoá, **không cần đoán RNG**. Thay `time(NULL)` bằng `CryptGenRandom` không bịt được gì cả — nó chỉ có
nghĩa nếu đồng thời đổi cách trao khoá, mà đổi cách trao khoá = **đổi giao thức**. Đề xuất này nên rút khỏi danh
sách "rẻ và đáng làm".

### S7. §11.3 nói nhẹ hơn thực tế

"Biết 4 byte bản rõ ⇒ suy ra mặt nạ của **toàn bộ lô đó**" — thực ra vì khoá không cuộn, mặt nạ là **cố định cho cả
chiều của cả kết nối**, không chỉ một lô.

### S8. §14.1/§17.11 "9.000 lượt cấp phát/giây" — không phải cấp phát heap

`Allocate()` lấy từ danh sách rỗi giữ tới **10.240** đệm (`ServerStage.cpp:1459`), nên sau khởi động gần như không
còn `new/delete`, chỉ là một khoá + pop/push. Phần "một lần chép thừa" ở 500 người ≈ 18 MB/s memcpy = vài phần
nghìn mili giây mỗi nhịp — trong khi số đo thật của đường gửi là 0,010-0,014 ms/nhịp
(`jx_gui_server.log`, trận 08/09). Lợi ích ~0, rủi ro cao (WSASend bất đồng bộ, phải đệm đôi) ⇒ **bỏ đề xuất 11**.

### S9. §17.13/§19.4 "đổi mã hoá thì WAuto phải cập nhật theo" — không đúng với WAuto hiện nay

`WAuto.exe` không đụng gói mạng: nó nói chuyện với client qua **shared memory + sự kiện + `WM_COPYDATA`**
(`WAuto.cpp:33-34, 476, 496, 564`). Mã hoá nằm trong client. (Kết luận "đừng đổi mã hoá" vẫn giữ, nhưng vì lý do
khác: nó là đổi giao thức client+server, không mua lại được gì.)

### S10. §0/§3/§8.1 "xả mỗi nhịp ≈ 55 ms" so với "Linux ≤ 10 ms" — so lệch nhau

Gói **do chính vòng game sinh ra**, và `SendPackToClient(-1)` nằm ở **cuối `MainLoop`** (`KSOServer.cpp:3341`).
Nên thời gian gói nằm chờ = phần CPU còn lại của nhịp đó (đo thật 8-9 ms, lúc xấu 20-31 ms), **không phải 55 ms**.
Khi nhịp tụt xuống 10 fps thì gói cũng **sinh ra** muộn — xả nhanh không cứu được, khác với câu "gói vẫn ra trong 10 ms".

Chỗ **thật sự** mất 0-55 ms là chỗ khác, tài liệu không nhắc: gói của client được xử lý trong `MessageLoop()`
(`KSOServer.cpp:1103`) — hàm này chạy **mỗi vòng `Breathe`**, kể cả khi chưa tới nhịp game — nhưng câu trả lời cho nó
phải nằm chờ tới **cuối `MainLoop` kế tiếp**. Nếu muốn giảm trễ thật thì làm đúng chỗ này (xả thêm một lần sau
`MessageLoop`), rẻ và ít rủi ro hơn nhiều so với "tách hẳn luồng xả theo thời gian" (đề xuất 1 + §19.4#6).

---

## 3. Lỗi 🔴 ở §19.3: vẫn là lỗi thật, nhưng **mức độ và cách sửa đều phải chỉnh**

Phần đúng: `CIOCPServer::ReadCompleted` (`ServerStage.cpp:1263`) gọi thẳng `pRecvBuffer->AddData()` không kiểm chỗ
trống, và `AddData` vứt im lặng. Ba chỗ phải chỉnh:

**(a) Khó chạm hơn tài liệu viết.** Tài liệu nói "vòng game gọi `GetPackFromClient` mỗi nhịp **một lần** cho mỗi
client". Thực tế `KSOServer.cpp:1238-1247` là vòng `while` rút **hết** gói của client đó, và `MessageLoop` chạy mỗi
vòng `Breathe` (~1 ms khi rảnh), không phải 55 ms một lần. Muốn tràn 16 KB phải có >16 KB dồn trong một nhịp ⇒
người chơi thường không tạo nổi; cần client sửa đổi/flood, hoặc nhịp kẹt rất lâu. Con số "nhịp kẹt max 24,7 ms"
mà tài liệu đưa ra làm bằng chứng **không đủ để chạm**.

**(b) Hậu quả khác chuỗi sập 04/09.** Hướng client→GameServer đóng khung **`[WORD][một gói]` mỗi gói**
(`ClientStage.cpp:187-192`), không phải lô. Mất khối ⇒ lệch khung ⇒ hoặc `messageSize` rác làm kết nối **kẹt câm**
tới ping timeout (xem S1), hoặc một gói rác đi vào `PlayerMessageProcess`. Chuỗi sập 04/09 là chuyện khác:
`CPackager` ghép role-data ở hướng **Goddess→GameServer**, và hướng đó **đã vá**. Nói "lỗi đã làm sập GameServer
04/09 vẫn còn nguyên" là mạnh quá tay.

**(c) Cách sửa tài liệu đề nghị là SAI HƯỚNG — đây là điểm quan trọng nhất của cả bản kiểm chứng này.**
Tài liệu viết: "Sửa rẻ nhất: **bê nguyên** `[RECV 04/09]` từ `ClientStage.cpp` sang `ServerStage.cpp`".
Bản vá đó (`ClientStage.cpp:417-442`) **chờ bằng `Sleep(1)` tới 30.000 lượt ngay trong hàm hoàn thành I/O**.

- Ở Rainbow: đúng, vì cả tiến trình chỉ có **một** kết nối và luồng riêng của nó.
- Ở Heaven: `ReadCompleted` chạy trên **luồng IOCP dùng chung** (`SocketServer.cpp:1038-1097`), mà **chính các luồng
  đó cũng là nơi phát `WSASend`** (`:1246`). Chỉ cần số client kẹt ≥ số luồng (`CPU×2+2`) là **toàn bộ I/O của mọi
  người đứng im** — cả gửi lẫn nhận. Tức là bê nguyên = tự mở một đường đánh sập máy chủ bằng vài kết nối rác.

Hướng đúng (chỉ nêu, **chưa làm**): kiểm chỗ trống rồi **không đăng lại lệnh đọc** cho riêng client đó — TCP tự
tạo backpressure, đăng đọc lại khi luồng game đã rút; hoặc nới đệm có trần; hoặc đóng kết nối kèm log.
Điều tối thiểu, bắt buộc, rẻ nhất: **đừng vứt im lặng** — có một dòng log là đủ để lần sau biết nó có xảy ra hay không.

---

## 4. Ba chỗ tài liệu chưa soi tới

### M1. Đường GỬI: một gói ĐƠN dài hơn 10.238 byte ghi ra ngoài đệm

`SendPackToClient`/`SendData`/`_SendDataEx` đều làm: `Allocate()` (đệm **10.240** byte) → `AddData(đầu gói, 2)` →
`AddData(dữ liệu, used)` → `KSG_EncodeBuf(used, pBuffer->GetBuffer()+2, ...)`.
Nếu `used > 10.238`: `AddData` **vứt im lặng** (không đủ chỗ) nhưng `KSG_EncodeBuf` vẫn XOR **`used` byte** kể từ
`GetBuffer()+2` ⇒ **ghi quá đuôi vùng nhớ 10.240 byte**, đồng thời gửi đi một khung khai `used+2` byte mà thân chỉ
có 2 byte ⇒ client lệch dòng.

Chạm được không? Lối vào duy nhất là một gói đơn ≥ 10.239 byte (gộp lô không tạo nổi vì đã xả cưỡng bức ở 10.208).
Các chỗ gửi độ dài biến thiên tôi tra được đều có trần nhỏ hơn: `KTongJX2.cpp:400` `byOut[4096]`,
`KScriptProtocol.h:35` `SCRIPT_DATA_MAXLEN 4096`, chat mở rộng theo `WORD wLength` của `tagExtendProtoHeader`,
`KSubWorld::BroadCast` thì do bên gọi quyết định. ⇒ **Chưa tìm được đường chạm — xếp là lỗ hổng tiềm ẩn**, đáng đặt
một lằn ranh kiểm tra (một `if` + log) chứ không phải sự cố đang xảy ra.

### M2. Cỡ đệm nhận lệch nhau giữa các máy chủ cùng dùng `heaven.dll`

Mỗi lần đọc socket mang về tới **10.240 byte** (`bufferSize` mặc định), trong khi đệm nhận mỗi kết nối là:

| Máy chủ | Đệm nhận | Nhận xét |
|---|---|---|
| GameServer | 16.384 (`KSOServer.cpp:121`) | > cỡ đọc, an toàn ở mức khung |
| Goddess | 4 MB (`Goddess.cpp:719`) | rộng rãi |
| **Bishop** | **8.192** (`Bishop/Intercessor.cpp:432`, dùng cho **cả** server người chơi lẫn server GameServer) | **nhỏ hơn một lần đọc** ⇒ một lần đọc đầy là **luôn** bị vứt im lặng |

Và Goddess an toàn hơn tài liệu ngầm giả định, vì luồng rút mạng (`Goddess.cpp:177-209`) chỉ nối dữ liệu vào hàng
đợi, còn việc MySQL nằm ở **luồng khác** (`ClientNode.cpp:175-219`) ⇒ MySQL chậm không làm tràn đệm socket, nó chỉ
làm dài hàng đợi trong RAM.

### M3. `ProcessMessage` — đường mã thứ hai, chỉ S3Relay dùng

`ServerStage.cpp:1816-1886` (dùng khi có `RegisterMsgFilter(clientID, IMessageProcess*)` — `S3Relay/NetServer.cpp:149`):
nếu WORD đầu khung là 0 hoặc 1 thì `messageSize - sizeof(WORD)` **âm ⇒ tràn số** thành ~4 tỉ và `KSG_DecodeBuf` ghi
loạn bộ nhớ; vòng `while` cũng không tiến. Cần khung hỏng đến từ một GameServer (bên tin cậy) nên rủi ro thấp,
nhưng là lỗi thật, và nó **không có** ở đường `ProcessDataStream` thông thường.

---

## 5. Xếp lại danh sách việc (thay cho §8 + §17 + §19.4)

Nếu chủ quyết định động vào lớp mạng, thứ tự nên là:

| # | Việc | Vì sao | Đụng giao thức |
|---|---|---|---|
| 1 | **Đừng vứt im lặng ở đường nhận `heaven`** — tối thiểu là log; tốt hơn là hoãn đăng lệnh đọc | lỗi thật, im lặng nên không ai biết nó có xảy ra hay không | Không |
| 2 | **Rà cỡ đệm nhận ≥ cỡ đọc** (Bishop 8 KB < 10 KB) | lệch cỡ = mất dữ liệu chắc chắn, sửa bằng một con số | Không |
| 3 | **Lằn ranh gói đơn quá cỡ ở đường gửi** (M1) | chặn một lỗ ghi ngoài vùng nhớ trước khi ai đó thêm gói to | Không |
| 4 | **Xả thêm một lần sau `MessageLoop`** nếu muốn giảm trễ phản hồi (S10) | rẻ hơn hẳn "tách luồng xả", đúng chỗ mất thời gian thật | Không |
| 5 | Nâng `listen(5)` — **đo trước** (S4) | ta đang thấp hơn cả bản Linux mà tài liệu chê | Không |
| — | **BỎ:** đổi nguồn hạt ngẫu nhiên (S6) · bỏ một lần chép đường xả (S8) · đo trần lô 65.535 (S3) · gom gói phía client | không mua lại được gì, hoặc không thể xảy ra | — |

Giữ nguyên kết luận "**không bắt chước**" của tài liệu: bảng khoá 5.679 mục (đổi giao thức), epoll một luồng,
khung 2 byte mỗi gói, `_Rand` (giống hệt ta) — **trừ** dòng `listen(10)`, vì ta đang là 5.

Về "gom gói phía client": ngoài lý do trễ, còn một chi tiết tài liệu không nêu — client **không** đặt `TCP_NODELAY`
(cả `Common/SocketClient.cpp` chỉ đặt `SO_LINGER`), nên Nagle đã gom giúp ở tầng TCP rồi; và `CSocketClient::Write`
(`:414-535`) **gửi chặn ngay trong luồng gọi**, có vòng `select` chờ tới 5 giây khi socket nghẽn. Gom thêm ở tầng
trên chỉ cộng thêm trễ.

---

## 6. Trạng thái

**Không sửa một dòng mã nào.** Công cụ kiểm chứng nhị phân của đợt này:
`…\54365836-…\scratchpad\verify_net_bin.py` (chạy lại được: ánh xạ PT_LOAD, bảng ảo, so bảng khoá, tháo mã coder).
