# ReverseTools — bo cong cu dich nguoc ban goc JX1

May nay KHONG co objdump / readelf / nm / strings. Chi co **python + capstone**.
Cac cong cu duoi day thuan python, chay duoc ngay.

> 🔴 **LUAT 20/08:** chi dich nguoc **ban Linux / ban khong co ma nguon**.
> Binary cua DU AN (`CoreServer.dll`, `GameServer.exe`, `engine.dll`, `Game.exe`) thi **doc thang ma nguon**
> `D:\GAMEDEVNEW\Sources` — soi binary cua chinh minh la vong vo va de ket luan sai.

| tep | dung de |
|---|---|
| `re_pe_crt.py` | **KIEM THU BAT BUOC** truoc khi trien khai .dll/.exe client — bat loi tron CRT Debug/Release |
| `re_elf_luamap.py` | (ban cu) lay lai TEN HAM may chu Linux tu bang dang ky Lua — ⚠️ **BO SOT 216 TEN**, xem duoi |
| **`re_elf_luamap_full.py`** | **ban SUA, dung ban nay** — co xu ly tail-merge chuoi cua GCC |
| **`re_lua_api_gap.py`** | doi chieu "cay script ban Linux goi ham gi" voi "du an dang ky ham gi" ⇒ danh sach ham CON THIEU |
| `re_disasm.py` | dich nguoc theo dia chi ao, cho ca ELF32 lan anh PE da giai nen |
| `jx_linux_y.luamap.txt` | ket qua ban CU: 42 bang, 1.344 ten |
| **`jx_linux_y.luamap.full.txt`** | ket qua ban SUA: **1.560 ten** (GameServer Linux) |
| **`s3relay_y.luamap.full.txt`** | ket qua ban SUA: **473 ten** (Relay lien may chu) |
| `liendau_api_gap.txt` | ket qua chay `re_lua_api_gap.py` cho he Lien Dau (leaguematch) |

Doc `../BANGIAO_PHIEN_1808.md` muc 5 de biet cach dung va quy doi dia chi.
Rieng he Lien Dau: doc `../BANGIAO_LIENDAU.md`.

## 🔴 Vi sao `re_elf_luamap.py` bo sot 216 ten

Trinh lien ket GCC gop hau to chuoi (`SHF_MERGE|SHF_STRINGS`). Rat nhieu ten ham la con tro tro vao
**GIUA** mot chuoi dai hon, trong khi cong cu cu chi nhan khi con tro tro vao **DAU** chuoi.

Bang chung byte — `jx_linux_y` offset `0x212530`:

```
... ngeTeamFeature\0 Msg2Faction\0 GMMsg2Player\0 Msg2Team\0 ...
                                     ^^ "Msg2Player" bat dau tu day = GMMsg2Player + 2
```

`Msg2Player` **khong ton tai rieng**. Tuong tu:
`GetTask`/`SetTask` nam trong `NW_GetTask`/`NW_SetTask` (`0x214AD0`),
`Talk` trong `ForbitTalk`, `GetLevel` trong `PET_GetLevel`,
`WriteLog` trong `TONG_WriteLog`, `GetName` trong `TONG_GetName`.

`re_elf_luamap_full.py` do tim **MOI vi tri byte** cua `ten + NUL` roi moi tra con tro, nen lay du.

## Cach dung

```bash
# trich bang dang ky Lua (ban SUA)
python re_elf_luamap_full.py D:/ServerLinux/server1/jx_linux_y        > jx_linux_y.luamap.full.txt
python re_elf_luamap_full.py D:/ServerLinux/gateway/s3relay/s3relay_y > s3relay_y.luamap.full.txt

# doi chieu chenh lech API (mac dinh: cay leaguematch)
# sua bien ROOTS trong tep de doi sang cay script khac
python re_lua_api_gap.py > liendau_api_gap.txt
```

⚠️ Tren Windows phai dat `PYTHONIOENCODING=utf-8` truoc khi chay, neu khong se loi `charmap codec`.

## Ban goc o dau

| ban | duong dan | quy doi dia chi |
|---|---|---|
| client Windows, da giai nen | `D:\ServerLinux\Patch\game_y_unpacked.bin` | VA = 0x401000 + do lech tep |
| client Windows, con nen | `D:\ServerLinux\Patch\game_y.exe` | UPX bien the NRV2E_LE32 |
| may chu Linux (GameServer) | `D:\ServerLinux\server1\jx_linux_y` | qua bang PT_LOAD (5 doan) |
| may chu Linux (Relay) | `D:\ServerLinux\gateway\s3relay\s3relay_y` | qua bang PT_LOAD |
