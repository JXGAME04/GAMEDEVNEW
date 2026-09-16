# Bộ công cụ mổ nhị phân — THƯ VIỆN ĐỘNG (.so) và PE có nhập theo số thứ tự

Bộ này **bổ sung** cho các công cụ đã có ở `ReverseTools/` chứ không thay thế. Chia việc như sau:

| Loại tệp | Dùng công cụ nào |
|---|---|
| **Chương trình ELF32 nguyên khối** (`jx_linux_y`, `S2SSyncRelayD`) | `ReverseTools/re_disasm.py`, `ReverseTools/re_elf_luamap.py`, `ReverseTools/mo_nhi_phan_0609/elfre.py` |
| **Ảnh PE đã giải nén** (`game_y_unpacked.bin`) | `ReverseTools/re_disasm.py` (tự đặt gốc 0x401000) |
| **Thư viện dùng chung có ký hiệu động** (`libheaven.so`, `librainbow.so`) | **thư mục này** — `elfsym.py` / `dis2.py` / `dis3.py` / `scanso.py` |
| **PE nhập hàm theo SỐ THỨ TỰ** (`rainbow.dll`) | **thư mục này** — `pe_imports.py` |
| Nhị phân Go ELF64 | **thư mục này** — `gopcln.py` / `godis.py` / `gostr.py` / `goref.py` |

Vì sao cần riêng: `elfre.py` đọc ELF32 **không có section header** bằng cách quét thô, nó **không**
đọc bảng ký hiệu động, nên với `.so` nó không cho ra tên hàm. Còn `re_pe_imports.py` chỉ in tên DLL
để nhận dạng CRT, **không giải số thứ tự**, nên `rainbow.dll` nhập `setsockopt` theo ordinal thì grep
chuỗi không bao giờ thấy.

## Cần gì

```bash
pip install capstone
```

Đã thử với capstone 5.0.7. Mọi kịch bản dùng `sys.path.insert(0, ".")` nên **phải chạy từ trong thư mục này**.

## Từng công cụ, kèm lệnh đã chạy thật (16/09)

### `elfsym.py` — đọc ký hiệu của `.so` đã tước section header

Không chạy trực tiếp, dùng như thư viện. Đọc `PT_DYNAMIC` để lấy `DT_SYMTAB` / `DT_STRTAB` / `DT_NEEDED`.

```python
import sys; sys.path.insert(0, '.')
from elfsym import ELF32
e = ELF32(r'D:/ServerLinux/server1/librainbow.so')
n = e.nsyms()                                   # 138
ten = [e.sym(i)[0] for i in range(n)]
print('CreateClientManager' in ten)             # True
print(e.needed())                               # ['libpthread.so.0', 'libstdc++.so.6', ...]
```

### `dis2.py` — tháo mã MỘT HÀM THEO TÊN, giải tên hàm ngoài qua PLT

Đây là phần giá trị nhất: mã PIC gọi hàm ngoài bằng `jmp [ebx+off]`, công cụ lần ngược
`DT_JMPREL` để in ra `; -> pthread_join`, `; -> socket`.

```python
import sys; sys.path.insert(0, '.')
from dis2 import Lib, pretty
lib = Lib(r'D:/ServerLinux/server1/librainbow.so')
print(len(lib.by_name))                          # 81 ham co ten
pretty(lib, '_ZN14KClientManager12UnInitializeEv', keep=['call'])
```

### `dis3.py` — thêm giải chuỗi tham chieu kiểu PIC (`lea reg,[ebx±off]`)

Dùng khi cần thấy chuỗi literal trong thân hàm. Cùng lớp `Lib`, thêm `lib.strat(va)`.

### `scanso.py` — quét MỌI hàm trong `.so`, in dòng khớp mẫu

Công cụ mở màn tốt nhất khi chưa biết tìm gì:

```bash
python scanso.py D:/ServerLinux/server1/librainbow.so socket
```

Ra ngay `_ZN7KClient9ConnectToEjt ... ; -> socket`, tức tìm được hàm nào mở socket mà không cần đoán.

### `strs.py` / `gbk.py` — trích chuỗi kèm file-offset và địa chỉ ảo

```bash
python strs.py D:/ServerLinux/server1/librainbow.so | head -40
python gbk.py D:/ServerLinux/server1/jx_linux_y ra_gbk.txt 0x8048000 0x8300000
```

`gbk.py` chỉ giữ đoạn toàn chữ Hán, dùng để tìm bảng tên tiếng Trung. Chữ **tiếng Việt** thì phải
qua `ReverseTools/port_3hd/dec2.py` (tệp Linux trộn hai bảng mã trong cùng một dòng).

### `xr.py` — tìm mọi nơi nạp một hằng số tuyệt đối (cho ET_EXEC)

Dùng cho `jx_linux_y`: biết địa chỉ một bảng, tìm ai đọc bảng đó.

### `pe_imports.py` — bảng nhập PE32 KỂ CẢ nhập theo số thứ tự

```bash
python pe_imports.py D:/ServerLinux/Patch/rainbow.dll
```

Ra: `WS2_32.dll send(#19), recv(#16), WSAGetLastError(#111), ... ioctlsocket(#10), socket(#23)`.

### `gopcln.py` / `godis.py` / `gostr.py` / `goref.py` / `rng.py` — nhị phân Go ELF64

Dùng cho mục tiêu khác (không phải máy chủ JX). `rng.py` cần `godis.py`.

## Một chỗ đã phải sửa khi cứu bộ này về (16/09)

`elfsym.nsyms()` bản gốc: khi tệp **không có `DT_HASH`**, nó quét mù tới 200 000 mục và gọi `sym()`
không chặn bờ, nên đọc quá cuối tệp rồi chết `ValueError` — xảy ra ngay trên `librainbow.so`.
Nay chặn bằng hai mốc: `(DT_STRTAB − DT_SYMTAB) / DT_SYMENT` và độ dài tệp. Ai chép bộ này đi nơi
khác nhớ mang theo bản đã sửa.
