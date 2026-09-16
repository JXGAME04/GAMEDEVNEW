# BÀN GIAO — MỔ NHỊ PHÂN BẢN 2.0 & BẢN LINUX

**Viết 16/09/2026 cho phiên sau.** Gom lại toàn bộ đường dẫn, công cụ, kết quả đã có và việc còn lại.
Mọi đường dẫn và cỡ tệp trong mục 1 **đã kiểm lại trên máy ngày 16/09**, không chép từ tài liệu cũ.

> **Luật đang áp dụng cho việc này: CHỈ PHÂN TÍCH, KHÔNG SỬA MÃ** cho tới khi chủ chọn.
> Xem bộ nhớ `phan-tich-truoc-chua-sua`. Đợt 13/09 đã giao báo cáo, chủ chưa chọn mục nào.

---

## 1. NHỊ PHÂN — ĐƯỜNG DẪN ĐẦY ĐỦ

### 1.1 Có trên máy (kiểm 16/09)

| Vai | Đường dẫn | Cỡ (byte) | md5 |
|---|---|---|---|
| Máy chủ game Linux | `D:\ServerLinux\server1\jx_linux_y` | 8.931.808 | `cf6b2a697bf7fb65550a4ab396f239fb` |
| Thư viện logic Linux | `D:\ServerLinux\server1\libheaven.so` | 182.713 | `1c5498b1e823a27dcf7a49c4afad27d2` |
| Thư viện vận chuyển Linux | `D:\ServerLinux\server1\librainbow.so` | 84.048 | `a3157485648480e8d1a32816b5ceda7a` |
| Relay đồng bộ Linux | `D:\ServerLinux\server1\S2SSyncRelayD` | 6.802.744 | `7b8b4831e8c7eaa1fa34e8d17fecc744` |
| Client của bản Linux | `D:\ServerLinux\Patch\game_y.exe` | 1.083.450 | `6c618239a8832d8275bcf5df9316eaa5` |
| Client đã giải nén | `D:\ServerLinux\Patch\game_y_unpacked.bin` | 22.551.940 | `152f275af4f68c6307a2538e8d8a0e7f` |
| **rainbow.dll thế hệ mới** | `D:\ServerLinux\Patch\rainbow.dll` | 23.040 | `0386872a717d3992bb4125ab41c70af6` |
| rainbow.dll (gateway) | `D:\ServerLinux\gateway\rainbow.dll` | 22.528 | `3ee1fe22533a8c98bbdbe50b6323950f` |
| heaven.dll bản Linux | `D:\ServerLinux\Patch\heaven.dll` | 36.987 | `e41bba84c24bb426bc8198674d6ea7b5` |

`D:\ServerLinux` có **ba** gốc con và chúng khác nhau, đừng lẫn:

- `server1\` — máy chủ: nhị phân, `script\`, `settings\`
- `gateway\` — lịch chạy ở `...\relaysetting`
- `Patch\` — **client** + 34 bảng máy chủ mà `server1` không có, ở `Patch\settings`

### 1.2 THIẾU — phải hỏi chủ

Tài liệu 13/09 có mổ **client bản 2.0**: `Vo Lam Truyen Ky 2.0\rainbow.dll` (33.152 byte) và
`gamecl.exe` giải nén thành `gamecl_unpacked.bin` (34.588.706 byte).

**Hôm nay tìm khắp các ổ đang gắn, KHÔNG thấy cây này.** Hai tệp đó không còn trên máy.

Thay thế tạm được: hai `rainbow.dll` ở `Patch\` và `gateway\` **đều xuất đúng một hàm
`CreateClientManager`** (đã kiểm hôm nay bằng bảng xuất khẩu PE), tức cùng thế hệ với bản 2.0 và
bản Linux. Nên nếu chỉ cần mẫu vật phía Windows của lớp vận chuyển thế hệ mới thì dùng được ngay.
Nhưng **bảng khoá mã hoá 22.716 byte nằm trong `gamecl.exe`**, không có trong `.dll` — muốn kiểm lại
phần đó (§11.4 tài liệu cũ) thì bắt buộc phải xin lại cây 2.0 từ chủ.

### 1.3 Cây dự án để đối chiếu

| Vai | Đường dẫn |
|---|---|
| Mã nguồn máy chủ | `D:\GAMEDEVNEW\Sources\MultiServer\{Heaven,Rainbow,Common}` |
| Nhị phân dự án | `D:\GAMEDEVNEW\bin\{server,client}\*.dll` |
| Máy chủ JX1 chạy thật | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\` |
| Client JX1 chạy thật | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\` |

---

## 2. BỘ CÔNG CỤ — CHIA BA NHÓM

`pip install capstone` (đã thử với 5.0.7). Cả ba nhóm đều là kịch bản Python, chỉ đọc, không ghi gì
vào nhị phân.

### 2.1 Chương trình nguyên khối — `ReverseTools\` (đã có sẵn từ trước)

| Công cụ | Việc |
|---|---|
| `ReverseTools\re_disasm.py` | Tháo mã theo địa chỉ ảo. Hiểu **cả** ELF32 (`jx_linux_y`) **và** ảnh PE đã giải nén (tự đặt gốc `0x401000`). Có `--find "chuỗi"` để tìm ai tham chiếu một chuỗi |
| `ReverseTools\re_elf_luamap.py` | Trích tên hàm từ `jx_linux_y` |
| `ReverseTools\mo_nhi_phan_0609\elfre.py` | ELF32 không section header: `s` tìm chuỗi, `f` tháo một hàm, `x` tìm nơi nạp hằng số, `b` tìm dãy byte |
| `ReverseTools\re_pe_crt.py` | Nhận dạng CRT của `.dll/.exe` client (bắt buộc chạy trước khi triển khai) |
| `ReverseTools\re_minidump_sym.py` | Ghép minidump + PDB ra call stack có tên hàm |

```bash
python ReverseTools/re_disasm.py D:/ServerLinux/server1/jx_linux_y 0x0804DA90 60
python ReverseTools/re_disasm.py D:/ServerLinux/Patch/game_y_unpacked.bin 0x006429A0 40
```

### 2.2 Thư viện động và PE nhập theo số thứ tự — `ReverseTools\nhiphan\` (**cứu về 16/09**)

**Bộ này suýt mất.** Nó do phiên mổ mạng 13/09 tự viết và chỉ nằm trong thư mục tạm của phiên đó
(`C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\...\scratchpad`),
không hề được commit. Hôm nay đã chép vào kho, sửa một lỗi, thử lại trên nhị phân thật và đẩy lên.

Sổ tay đầy đủ kèm lệnh đã chạy: `ReverseTools\nhiphan\README.md`.

| Công cụ | Việc |
|---|---|
| `elfsym.py` | Đọc bảng ký hiệu động của `.so` đã tước section header (qua `PT_DYNAMIC`) |
| `dis2.py` | Tháo mã **một hàm theo tên**, và **giải tên hàm ngoài qua PLT** (`jmp [ebx+off]` → `DT_JMPREL`) — in ra `; -> socket`, `; -> pthread_join` |
| `dis3.py` | Thêm giải chuỗi tham chiếu kiểu PIC |
| `scanso.py` | Quét **mọi** hàm trong `.so`, in dòng khớp mẫu — công cụ mở màn tốt nhất |
| `strs.py`, `gbk.py` | Trích chuỗi kèm file-offset và địa chỉ ảo; `gbk.py` lọc riêng chữ Hán |
| `xr.py` | Tìm mọi nơi nạp một hằng số tuyệt đối (cho ET_EXEC) |
| `pe_imports.py` | Bảng nhập PE32 **kể cả nhập theo số thứ tự** |
| `gopcln.py`, `godis.py`, `gostr.py`, `goref.py`, `rng.py` | Nhị phân Go ELF64 (mục tiêu khác) |

**Vì sao không dùng công cụ cũ được:** `elfre.py` quét thô, **không đọc bảng ký hiệu động**, nên với
`.so` nó không cho ra tên hàm. `re_pe_imports.py` chỉ in tên DLL để đoán CRT, **không giải số thứ tự**,
mà `rainbow.dll` nhập `setsockopt`/`send`/`recv` theo ordinal — grep chuỗi không bao giờ thấy.

Hai lệnh đáng nhớ nhất:

```bash
cd ReverseTools/nhiphan
python scanso.py D:/ServerLinux/server1/librainbow.so socket
python pe_imports.py D:/ServerLinux/Patch/rainbow.dll
```

### 2.3 Đọc chữ trong tệp Linux — `ReverseTools\port_3hd\`

| Công cụ | Việc |
|---|---|
| `ReverseTools\port_3hd\dec2.py` | **BẮT BUỘC** khi đọc script/bảng bản Linux: tệp trộn **hai bảng mã trong cùng một dòng** (tiếng Việt VNI/TCVN + tiếng Trung GBK). `iconv` và `gbktool.py` đều giải sai. `dec2.decline2(bytes)` cắt từng đoạn rồi chọn bảng mã theo đoạn |
| `ReverseTools\port_3hd\recon_tinhnang.py` | Đo mặt phụ thuộc của một tính năng trước khi port |

Hướng dẫn port một tính năng Lua từ bản Linux: `HUONGDAN_DICHNGUOC_TINHNANG_LINUX.md`
(quy trình 8 bước, 13 cái bẫy đã cắn thật).

---

## 3. ĐÃ MỔ ĐƯỢC GÌ — BẢN ĐỒ TÀI LIỆU

Tài liệu chính: **`PHANTICH_NETWORK_2.0_LINUX_VS_DUAN_1309.md`** (957 dòng).

| Mục | Nội dung |
|---|---|
| §0–§1 | Kết luận một trang; ba bản là cùng dòng mã gốc, dự án ta đứng ở **thế hệ cũ hơn** của lớp mạng |
| §2 | Máy chủ Linux `libheaven.so`: `OpenService` `0x44c0`, `Loop` `0x5d50`, `RecvData` `0x4160`, `SendData` `0x4050`, `PackData` `0x42e0`, bắt tay khoá 42 byte `0x52c0` |
| §3–§4 | Máy chủ dự án `CIOCPServer`; đệm co giãn so với cố định; **khung gói trên dây khác nhau** |
| §5–§6 | Mã hoá; phía client ba bản |
| §7–§8 | Bảng đối chiếu tổng hợp; 8 điểm đáng cân nhắc |
| §9 | Cách kiểm chứng lại |
| §10 | Bản đồ cấu trúc: `KCONNECTION` 52 B, `KBuffer` 28 B, `KPackBuffer` 32 B, `KThreadLock` 28 B |
| §11 | **Thuật toán mã hoá dựng lại đầy đủ** từ `0x0804DA90`; bảng khoá 22.716 byte trùng 100 % giữa Linux và client 2.0 |
| §12–§15 | Gói bắt tay so từng byte; bộ sinh ngẫu nhiên (**giống hệt ta**); `MultiSend`; máy trạng thái đóng kết nối |
| §16 | Các mô-đun còn lại: `GameExtConnect.so` trùng md5 `KG_Angel.so`, không phải mô-đun mạng; `S2SSyncRelayD` có lớp mạng riêng, cùng họ thuật toán, khoá riêng |
| §17–§18 | Đề xuất 9–13; **mốc địa chỉ để kiểm lại** |
| §19 | Nên học gì; **một lỗi thật tìm được khi kiểm lại mã của ta** |

Các tài liệu Linux khác trong kho: `HUONGDAN_DICHNGUOC_TINHNANG_LINUX.md`,
`PHANTICH_3HOATDONG_LINUX_2408.md`, `PHANTICH_DATAU_LINUX_GOC.md`,
`PHANTICH_BANGSAT_NOICONG_LINUX_0709.md`, `PHANTICH_SKILL150_180_LINUX_0709.md`,
`PHANTICH_LNCK_KHIEN_TINH_LINUX_0709.md`, `PHANTICH_TOCDODANH_LINUX_SAU_0709.md`,
`PHANTICH_LIENDAU_LINUX_VS_DUAN.md`, `PHANTICH_BANGHOI_UI_VS_LINUX_0709.md`,
`BANGIAO_DOITEN_DOIPHAI_LINUX_0109.md`.

---

## 4. VIỆC CÒN LẠI — XẾP THEO ƯU TIÊN

### 4.1 Việc số một: một lỗi thật, đã xác minh **vẫn còn sống hôm nay**

`CIOBuffer::AddData` (`Sources\MultiServer\Common\IOBuffer.cpp:90`) khi đệm đầy thì **`return` im lặng**,
dòng `throw` đã bị chú thích. Hai nơi gọi nó với dữ liệu vừa đọc từ socket:

| Nơi | Có chống tràn không |
|---|---|
| `Sources\MultiServer\Rainbow\ClientStage.cpp:422` | **CÓ** — bản vá `[RECV 04/09]`: chờ luồng chính lấy bớt, tạo backpressure TCP, quá hạn thì đóng và báo to |
| `Sources\MultiServer\Heaven\ServerStage.cpp:1298` | **KHÔNG** — gọi thẳng `pCN->pRecvBuffer->AddData( pPackData, used )` |

Nghĩa là lỗi đã làm **sập GameServer hôm 04/09** trên tuyến Goddess→GameServer **vẫn còn nguyên trên
tuyến client→GameServer**. Điều kiện chạm: đệm nhận 16.384 B, mỗi lần đọc socket tối đa 10.240 B, mà
`pRecvBuffer` chỉ được rút mỗi nhịp game (~55 ms) một lần cho mỗi client ⇒ **hai lần đọc đầy liên tiếp
trong cùng một nhịp là tràn**, khối thứ hai bị vứt im lặng, con trỏ độ dài trỏ vào rác.

Cách sửa rẻ nhất: bê nguyên bản vá `[RECV 04/09]` từ `ClientStage.cpp` sang `ServerStage.cpp`.
Không đụng giao thức, không đụng client, chỉ dựng lại `heaven.dll`.

> **Số dòng trong tài liệu 13/09 đã lệch** (nó ghi `ServerStage.cpp:1263`, nay là `1298`).
> Đừng nhảy thẳng theo số dòng cũ, `grep -n "pRecvBuffer->AddData"` rồi hãy đọc.

### 4.2 Bảng nên lấy gì, không nên lấy gì (rút từ §19.4)

| # | Học từ bản Linux | Giá trị | Rủi ro | Đụng giao thức |
|---|---|---|---|---|
| 1 | Không bao giờ vứt dữ liệu im lặng khi đệm đầy | **Cao — lỗi sập đã biết** | Thấp | Không |
| 2 | Đệm co giãn **có trần** thay vì cố định 16 KB | Cao | Trung bình | Không |
| 3 | Nhật ký `LeftFreeCount` mỗi lần cấp/trả/hết khe | Trung bình | Rất thấp | Không |
| 4 | Xoá khoá phiên lúc đóng kết nối | Thấp | Rất thấp | Không |
| 5 | Đổi nguồn hạt ngẫu nhiên, bỏ `time(NULL)` | Trung bình | Rất thấp | Không |
| 6 | Tách nhịp xả khỏi nhịp game (10 ms thay 55 ms) | Cao cho độ trễ | **Cao** | Không |
| 7 | Bỏ một lần chép ở đường xả | Trung bình | Cao — `WSASend` bất đồng bộ | Không |
| 8 | `KNullLock` khoá rỗng lúc biên dịch | Thấp | Rất thấp | Không |

**Đừng bắt chước**: `listen(backlog = 10)` quá nhỏ; thiếu `TCP_NODELAY` (**ta đã có**,
`SocketServer.cpp:277`, bản Linux thiếu); khung gói 2 byte **mỗi gói** (khung theo lô của ta gọn hơn);
epoll một luồng (ràng buộc của Linux, không phải thiết kế hơn); bảng khoá 5.679 mục (**đổi giao thức**,
phải thay client + máy chủ + WAuto cùng lúc); `_Rand` LCG gieo `time(0)` (**giống hệt ta**).

**Chỗ dự án đang hơn** — ghi để đừng "sửa theo Linux" nhầm hướng: có `TCP_NODELAY`; khung gói theo lô;
đường gửi đã có xả cưỡng bức kèm `gs_nGoiTran`; chuỗi tối ưu `[DELTA]` không có bản tương đương bên
Linux; client đã có backpressure `[RECV 04/09]`.

### 4.3 Phần chưa mổ

- **Client bản 2.0** — thiếu tệp, xem mục 1.2. Muốn kiểm lại §11.4 (bảng khoá) thì phải xin lại.
- `S2SSyncRelayD` mới mổ ở mức nhận dạng: có `epoll_wait`, cùng hằng `0x2E6D23C1` + `0x08088405`,
  **bảng khoá khác** (chỉ trùng 87 byte đầu). Chưa dựng lại lớp mạng riêng của nó.
- `GameExtConnect.so` / `KG_Angel.so` đã loại khỏi câu chuyện mạng (là `KG_Statistic2`), nhưng
  **chưa ai mổ phần thống kê** của nó — nếu sau này cần tính năng thống kê thì đây là nguồn.

---

## 5. BẨY ĐÃ CẮN

1. **Công cụ tự viết mà không commit là mất.** Bộ `ReverseTools\nhiphan\` suýt mất vì chỉ nằm trong
   thư mục tạm của phiên. Viết xong công cụ thì commit ngay, đừng để ở thư mục nháp.
2. **`elfsym.nsyms()` bản gốc đọc quá cuối tệp** khi `.so` không có `DT_HASH` — chết ngay trên
   `librainbow.so`. Đã sửa (chặn bằng `(DT_STRTAB − DT_SYMTAB) / DT_SYMENT` và độ dài tệp).
3. **Grep chuỗi không thấy hàm nhập theo số thứ tự.** `rainbow.dll` nhập `setsockopt` theo ordinal.
   Phải dùng `pe_imports.py`.
4. **Số dòng trong tài liệu cũ lệch theo thời gian.** Luôn `grep -n` lại trước khi mở.
5. **Tệp Linux trộn hai bảng mã trong một dòng.** Phải qua `dec2.py`, `iconv` giải sai.
6. **Ba gốc con của `D:\ServerLinux` khác nhau** — script ở `server1`, lịch ở `gateway`, 34 bảng thiếu
   ở `Patch\settings`.
7. **`GameExtConnect.so` và `KG_Angel.so` là cùng một tệp đặt hai tên** (trùng md5). Đừng mổ hai lần.

---

## 6. LÀM GÌ TRƯỚC

1. Hỏi chủ có muốn sửa **mục 4.1** không. Đó là lỗi thật, đã biết cơ chế, đã có sẵn bản vá mẫu ở tệp
   bên cạnh, không đụng giao thức. Mọi thứ khác trong tài liệu đều là cải tiến, chỉ cái này là lỗi.
2. Nếu chủ muốn đi tiếp phần mổ: xin lại cây **Vo Lam Truyen Ky 2.0**, không có nó thì phần bảng khoá
   không kiểm lại được.
3. Trước khi mổ thêm bất cứ gì, chạy thử ba lệnh ở mục 2 để chắc bộ công cụ còn chạy trên máy hiện tại.
