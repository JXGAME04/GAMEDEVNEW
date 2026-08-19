# ReverseTools — bo cong cu dich nguoc ban goc JX1

May nay KHONG co objdump / readelf / nm / strings. Chi co **python + capstone**.
Ba cong cu duoi day thuan python, chay duoc ngay.

| tep | dung de |
|---|---|
| `re_pe_crt.py` | **KIEM THU BAT BUOC** truoc khi trien khai .dll/.exe client — bat loi tron CRT Debug/Release |
| `re_elf_luamap.py` | lay lai TEN HAM cua may chu Linux tu bang dang ky Lua |
| `re_disasm.py` | dich nguoc theo dia chi ao, cho ca ELF32 lan anh PE da giai nen |
| `jx_linux_y.luamap.txt` | ket qua trich san: 42 bang, 1.344 ten ham |

Doc `../BANGIAO_PHIEN_1808.md` muc 5 de biet cach dung va quy doi dia chi.

## Ban goc o dau

| ban | duong dan | quy doi dia chi |
|---|---|---|
| client Windows, da giai nen | `D:\ServerLinux\Patch\game_y_unpacked.bin` | VA = 0x401000 + do lech tep |
| client Windows, con nen | `D:\ServerLinux\Patch\game_y.exe` | UPX bien the NRV2E_LE32 |
| may chu Linux | `D:\ServerLinux\server1\jx_linux_y` | qua bang PT_LOAD (5 doan) |
