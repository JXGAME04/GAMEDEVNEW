# -*- coding: utf-8 -*-
"""Sinh 04_api_gap.md tu 04_api_gap.json"""
import io, os, sys, json

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
HERE = os.path.dirname(os.path.abspath(__file__))
D = json.load(io.open(os.path.join(HERE, "04_api_gap.json"), encoding="utf-8"))
SH = json.load(io.open(os.path.join(HERE, "shadow.json"), encoding="utf-8"))

A = {n: v for n, v in D.items() if v["nhom"] == "A"}
B = {n: v for n, v in D.items() if v["nhom"] == "B"}
C = {n: v for n, v in D.items() if v.get("canh_bao_nhom_C")}
E = {n: v for n, v in D.items() if v["nhom"] == "D"}
FEATS = [("satthu", "Săn boss sát thủ (Killer Boss)"),
         ("phonglangdo", "Phong Lăng Độ (fengling_ferry)"),
         ("vuotai", "Vượt ải (challengeoftime)")]

o = []
w = o.append


def esc(s):
    return s.replace("|", "\\|").replace("\n", " ")


w("# 04 — BẢNG KHE HỞ API ENGINE cho 3 hoạt động port từ bản Linux (JX2) sang JX1")
w("")
w("| Mục | Nội dung |")
w("|---|---|")
w("| Ngày lập | 24/08/2026 |")
w("| Nguồn danh sách tệp | `D:\\GAMEDEVNEW\\ReverseTools\\port_3hd\\closure3.json` (bao đóng Include) |")
w("| Cây Lua tham chiếu | `D:\\ServerLinux\\server1\\script` + `\\vng_script` — 5.230 tệp `.lua` |")
w("| Bảng đăng ký JX1 | `D:\\GAMEDEVNEW\\Sources\\Core\\Src\\ScriptFuns.cpp` (`GameScriptFuns[]` dòng 14257, `WorldScriptFuns[]` dòng 15325) + `LuaFuns.cpp` (`SysFuns[]` dòng 43) + các `.cpp` khác trong `Core\\Src` |")
w("| Ảnh nhị phân | `D:\\ServerLinux\\server1\\jx_linux_y` (ELF32 i386, đã xoá section header) |")
w("| Bản đồ tên → địa chỉ | `D:\\GAMEDEVNEW\\ReverseTools\\jx_linux_y.luamap.full.txt` (1.560 tên) |")
w("")
w("## 0. Kết quả một dòng")
w("")
w("**280** tên hàm ENGINE bị 3 hoạt động gọi tới. **235 JX1 đã có**, **42 phải viết mới**, "
  "**8 cảnh báo tên-giống-nghĩa-khác**, **3 tên bị xếp nhầm** (thực ra không phải hàm engine).")
w("")
w("```")
w("  Tổng tên được gọi trong bao đóng   : 521")
w("   − trừ từ khoá + thư viện chuẩn Lua 4.0")
w("   − trừ hàm do CHÍNH cây script Linux định nghĩa (8.044 tên toàn cục)")
w("  ⇒ hàm ENGINE                       : 280")
w("       (A) JX1 ĐÃ CÓ                 : 235")
w("       (B) CHƯA CÓ — CẦN VIẾT MỚI    :  42")
w("       (D) không phải hàm engine     :   3   ContriValueEntryLogic, JudgePLAddTitle, Require")
w("       (C) cảnh báo nghĩa khác       :   8   (7 nằm trong 235 của nhóm A, riêng JoinMission thì không)")
w("```")
w("")

# ---------------------------------------------------------------- buoc 4
w("## 1. Bảng tóm tắt theo tính năng (bước 4)")
w("")
w("| Tính năng | Tệp trong bao đóng | Hàm engine dùng | (A) đã có | (B) phải viết | Trong đó nằm ở TỆP LÕI của tính năng |")
w("|---|---|---|---|---|---|")
nfile = {"satthu": 67, "phonglangdo": 81, "vuotai": 103}
for k, ten in FEATS:
    a = len([1 for v in A.values() if k in v["dung_boi"]])
    b = sorted(n for n, v in B.items() if k in v["dung_boi"])
    seed = sorted(n for n, v in B.items() if k in v["trong_tep_loi"])
    w("| **%s** | %d | %d | %d | **%d** | %s |"
      % (ten, nfile[k], a + len(b), a, len(b), ", ".join("`%s`" % s for s in seed) or "—"))
w("")
w("### 1.1 Phân bố 42 hàm nhóm B")
w("")
all3 = sorted(n for n, v in B.items() if len(v["dung_boi"]) == 3)
w("**Dùng chung cả 3 tính năng — %d hàm** (viết một lần, cả ba cùng hưởng):" % len(all3))
w("")
w("> " + ", ".join("`%s`" % n for n in all3))
w("")
for k, ten in FEATS:
    only = sorted(n for n, v in B.items() if v["dung_boi"] == [k])
    w("**Chỉ riêng %s — %d hàm:** %s" % (ten, len(only),
      ", ".join("`%s`" % n for n in only) or "không có"))
    w("")
two = sorted(n for n, v in B.items() if len(v["dung_boi"]) == 2)
w("**Dùng bởi 2 tính năng — %d hàm:** %s" % (len(two),
  ", ".join("`%s` (%s)" % (n, " + ".join(B[n]["dung_boi"])) for n in two)))
w("")
w("> **Nhận xét quan trọng.** 25/42 hàm nhóm B **không** nằm trong tệp lõi của bất kỳ tính năng "
  "nào, mà đến từ **thư viện dùng chung** do bao đóng Include kéo vào: `lib/droptemplet.lua`, "
  "`lib/log.lua`, `lib/file.lua`, `lib/composeex.lua`, `lib/string.lua`, "
  "`missions/boss/bigboss.lua`, `activitysys/*`, `battles/battlehead.lua`. Viết xong 25 hàm này "
  "là gỡ được nút thắt cho cả ba hoạt động.")
w("")

# ---------------------------------------------------------------- nhom B bang
w("## 2. Nhóm (B) — 42 hàm engine JX1 chưa có, phải viết mới")
w("")
w("| # | Tên hàm | Địa chỉ ELF | Chữ ký suy ra | Độ tin cậy | Dùng bởi | Lần gọi |")
w("|---|---|---|---|---|---|---|")
for i, (n, v) in enumerate(sorted(B.items()), 1):
    w("| %d | **`%s`** | `%s` | `%s` | %s | %s | %d |"
      % (i, n, v["dia_chi"] or "—", esc(v["chu_ky"] or "?"),
         v["do_tin_cay"], " + ".join(x for x in v["dung_boi"]), v["so_lan_goi"]))
w("")
w("### 2.1 Cách suy chữ ký — các địa chỉ API Lua đã xác định")
w("")
w("Chữ ký suy từ mã máy i386: đếm `lua_tonumber(L,i)` / `lua_tostring(L,i)` theo thứ tự chỉ số, "
  "và đọc trị trả về của hàm C (Lua 4.0: **trị trả về của hàm C = số giá trị đã đẩy lên ngăn xếp**). "
  "Các địa chỉ API Lua dưới đây được xác định bằng cách **đọc trực tiếp thân hàm**, không đoán:")
w("")
w("| Địa chỉ | Là hàm | Bằng chứng |")
w("|---|---|---|")
w("| `0x08232490` | `lua_gettop(L)` | `(L->top − L->stack) / 12` — `TObject` của Lua 4.0 dài 12 byte |")
w("| `0x08232D40` | `lua_pushnumber(L, double)` | ghi tag `2` (`LUA_TNUMBER`) rồi `fstp qword [eax+4]` |")
w("| `0x082338B0` | `lua_tonumber(L, idx)` | kiểm tag `2`, `fld qword [ebx+4]` |")
w("| `0x08233850` | `lua_tostring(L, idx)` | kiểm tag `3` (`LUA_TSTRING`), trả `str + 0x14` |")
w("| `0x082337A0` | `lua_pushstring(L, char*)` | gọi `strlen` (PLT `0x0804AEDC`) rồi `pushlstring` |")
w("| `0x08232E70` | `lua_pushnil(L)` | nhánh \"không phải bạch kim\" của `GetPlatinaLevel` |")
w("| `0x08232BE0` | `lua_newtable(L)` | gọi ngay trước vòng đổ 6 phần tử của `GetItemAllParams` |")
w("| `0x082339B0` | `lua_tonumber → int` | bọc `lua_tonumber` + `fistp` |")
w("| `0x08107860` / `0x08107910` | `GetGlobalPlayerIndex(L)` | đọc **biến toàn cục Lua `PlayerIndex`** (chuỗi tại `0x08251227`), trả −1 nếu không phải số |")
w("")
w("Cấu trúc dữ liệu suy ra kèm:")
w("")
w("- **Bản ghi người chơi**: mảng gốc `[0x08BAEE60]`, bước **`0x8788`** byte, chỉ số hợp lệ "
  "**1…1199** (`lea ecx,[eax-1]` rồi `cmp ecx, 0x4AE`).")
w("- **Bản ghi vật phẩm**: mảng gốc `[0x0830D300]`, bước **`0x368`** byte, số lượng `[0x0830CA5C]`, "
  "trường `[+4]` = **phẩm chất**, `[+0x1E0]` = 6 cấp thuộc tính ngẫu nhiên.")
w("")
w("### 2.2 Chi tiết từng hàm")
w("")
for n, v in sorted(B.items()):
    w("#### `%s`" % n)
    w("")
    w("- **Địa chỉ bản Linux**: `%s`" % (v["dia_chi"] or "không xuất ra Lua"))
    w("- **Chữ ký**: `%s`" % v["chu_ky"])
    w("- **Độ tin cậy**: %s" % v["do_tin_cay"])
    w("- **Mô tả**: %s" % v["mo_ta"])
    ds = v.get("disasm")
    if ds:
        w("- **Số liệu dịch ngược**: %d lệnh · `lua_gettop` = %s · tham số đọc được = %s · "
          "đẩy về = %s · trị trả về của hàm C = %s"
          % (ds["ninsn"], "có" if ds["gettop"] else "không",
             ("{" + ", ".join("p%s : %s" % (k, "/".join(x)) for k, x in sorted(ds["params"].items())) + "}")
             if ds["params"] else "{} (không đọc tham số Lua nào)",
             ds["push"] or "{}", ds["retvals"]))
        if ds.get("strings"):
            w("- **Chuỗi trong thân hàm**: %s" % ", ".join("`%s`" % esc(s) for s in ds["strings"]))
    w("- **Dùng bởi**: %s%s" % (" + ".join(v["dung_boi"]),
      ("  ·  nằm trong **tệp lõi** của: " + " + ".join(v["trong_tep_loi"])) if v["trong_tep_loi"] else ""))
    sites = list(dict.fromkeys(v["vi_tri_goi"]))[:5]
    w("- **Chỗ gọi tiêu biểu**: %s" % ", ".join("`%s`" % s for s in sites))
    w("")

# ---------------------------------------------------------------- nhom C
w("## 3. Nhóm (C) — tên giống nhưng nghĩa khác / cần kiểm chứng")
w("")
w("Đây là những tên **đã có trong bảng đăng ký JX1** nên bước 2 xếp vào nhóm A, nhưng **hành vi "
  "không giống bản JX2**. Nguy hiểm hơn nhóm B: script sẽ chạy **im lặng**, không báo lỗi.")
w("")
w("| Tên | Đăng ký trong JX1 | Vấn đề |")
w("|---|---|---|")
for n, v in sorted(C.items()):
    w("| **`%s`** | `%s` | %s |" % (n, v["dang_ky_jx1"] or "—", esc(v["canh_bao_nhom_C"])))
w("")
w("### 3.1 Những tên đáng ngờ nhưng ĐÃ KIỂM CHỨNG LÀ ĐÚNG (không thuộc nhóm C)")
w("")
w("Ba nhóm dưới đây có tên Lua khác tên hàm C nên thoạt nhìn đáng ngờ; đã đọc thân hàm và **xác "
  "nhận đúng nghĩa**:")
w("")
w("| Tên Lua | Hàm C trong JX1 | Kết luận |")
w("|---|---|---|")
w("| `AddDex` / `AddEng` / `AddStrg` / `AddVit` | `LuaSetPlayerDexterity`… (ScriptFuns.cpp:9412) | "
  "**Đúng.** Tên hàm C là \"Set\" nhưng `KPlayer::SetBaseDexterity` (KPlayer.cpp:13422) làm "
  "`m_nDexterity += nData` — tức là **cộng**, khớp với `Add*` của JX2. |")
w("| `AddProp` / `GetProp` | `LuaAddPropPoint` (15276) / `LuaGetRestPropPoint` (15279) | "
  "**Đúng.** Bản JX2 (`0x0810FD60` / `0x0810F9F0`) cộng và đọc đúng một trường `player+0x5924` "
  "= điểm tiềm năng còn lại. |")
w("| `ST_GetTransLifeCount` | `LuaGetPlayerReBornValue` (ScriptFuns.cpp:5528) | "
  "**Đúng.** Bản JX2 (`0x081C1100`) đọc byte `player+0x86B8`; JX1 đọc `m_cReBorn.GetReBornValue()`. "
  "Cùng vai trò — và đây chính là byte mà `ST_IsTransLife` (nhóm B) đọc. |")
w("")

# ---------------------------------------------------------------- nhom D
w("## 4. Nhóm (D) — 3 tên bị xếp nhầm vào \"khe hở engine\"")
w("")
w("| Tên | Kết luận |")
w("|---|---|")
for n, v in sorted(E.items()):
    w("| **`%s`** | %s |" % (n, esc(v["mo_ta"])))
w("")

# ---------------------------------------------------------------- rui ro khac
w("## 5. Rủi ro khác phát hiện kèm (không phải khe hở API nhưng sẽ làm vỡ bản port)")
w("")
w("### 5.1 Hàm toàn cục được định nghĩa NGOÀI bao đóng Include")
w("")
w("Bộ quét loại một tên khỏi \"khe hở engine\" khi cây script Linux có định nghĩa hàm toàn cục "
  "cùng tên. Nhưng nếu **tệp định nghĩa không nằm trong bao đóng** của tính năng thì khi port "
  "sang JX1 mà không kéo tệp đó theo, Lua sẽ báo *attempt to call global*.")
outside = [r for r in SH if not r["trong_bao_dong"]]
w("")
w("- Tổng tên bị loại vì script tự định nghĩa: **%d**" % len(SH))
w("- Trong đó định nghĩa nằm **ngoài** bao đóng: **%d**" % len(outside))
w("- Trong số đó, tên **cũng có trong luamap ELF và JX1 chưa có**: chỉ một tên — `JoinMission` "
  "(xem cảnh báo nhóm C ở trên).")
w("")
w("### 5.2 Các điểm CHƯA XÁC MINH")
w("")
w("| Vấn đề | Trạng thái |")
w("|---|---|")
w("| `SetNpcScript` và `SetNpcDeathScript` cùng trỏ tới `0x08101500` trong luamap | **CHƯA XÁC MINH** — hoặc công cụ lập luamap gán trùng, hoặc JX2 dùng chung một hàm. |")
w("| `ITEM_SetExpiredTime` — tham số 2 là YYYYMMDD hay số phút | **CHƯA XÁC MINH** — hai chỗ gọi trong bao đóng dùng hai đơn vị khác nhau. |")
w("| `JoinMission` — gọi hàm engine hay hàm script cùng tên | **CHƯA XÁC MINH** — `dragonboat_main.lua:163` truyền mission id, không khớp chữ ký hàm script. |")
w("| `GetNpcAroundPlayerList` — tham số thứ 3 | **CHƯA XÁC MINH** — mã máy đọc 3 số, script chỉ truyền 2. |")
w("| `OpenProgressBar` — tham số thứ 7 | **CHƯA XÁC MINH** — mã máy đọc 7, script chỉ truyền 6. |")
w("| `PET_*` — độ lệch trường trong bản ghi người chơi | Chỉ `+0x873C` (Grown) và `+0x8738` (kề bên) đọc trực tiếp được; ánh xạ Tame/Upgrade dựa vào luamap, **chưa đối chiếu chéo**. |")
w("")

# ---------------------------------------------------------------- nhom A
w("## 6. Nhóm (A) — %d hàm JX1 đã có" % len(A))
w("")
w("| Tên Lua | Đăng ký trong JX1 | Địa chỉ bản Linux | Dùng bởi |")
w("|---|---|---|---|")
for n, v in sorted(A.items()):
    w("| `%s` | %s | `%s` | %s |"
      % (n, v["dang_ky_jx1"] or "—", v["dia_chi"] or "—",
         " + ".join(x for x in v["dung_boi"])))
w("")

# ---------------------------------------------------------------- phu luc
w("## 7. Phụ lục — công cụ và tệp kết quả")
w("")
w("Tất cả nằm trong `D:\\GAMEDEVNEW\\ReverseTools\\port_3hd\\`:")
w("")
w("| Tệp | Vai trò |")
w("|---|---|")
w("| `api_gap_3hd.py` | Quét bao đóng → tách tên hàm engine → đối chiếu bảng đăng ký JX1. Sinh `api_gap_raw.json`. |")
w("| `bdump_3hd.py` | Dịch ngược toàn bộ hàm nhóm B từ ELF. Sinh `bdump.txt` (mã máy + chuỗi tham chiếu) và `callstat.txt` (tần suất đích `call`, dùng để nhận diện API Lua). |")
w("| `bsig_3hd.py` | Suy chữ ký từ mã máy (đếm `lua_tonumber`/`lua_tostring` theo chỉ số, số giá trị trả về). Sinh `bsig.json`. |")
w("| `bsites_3hd.py` | In chỗ gọi Lua kèm ngữ cảnh. Sinh `sites.txt`. |")
w("| `shadow_3hd.py` | Tìm tên bị che khuất bởi hàm script định nghĩa ngoài bao đóng. Sinh `shadow.json`. |")
w("| `finalize_3hd.py` | Gộp tất cả + chú thích tay → `04_api_gap.json`. |")
w("| `mkmd_3hd.py` | Sinh chính tệp `04_api_gap.md` này. |")
w("")
w("### Chạy lại toàn bộ")
w("")
w("```")
w("set PYTHONIOENCODING=utf-8")
w("cd D:\\GAMEDEVNEW\\ReverseTools\\port_3hd")
w("python api_gap_3hd.py  &&  python bdump_3hd.py  &&  python bsig_3hd.py")
w("python bsites_3hd.py > sites.txt  &&  python shadow_3hd.py")
w("python finalize_3hd.py  &&  python mkmd_3hd.py")
w("```")
w("")
w("### Bốn bộ lọc đã áp dụng khi tách \"hàm engine\"")
w("")
w("1. Bỏ 20 từ khoá Lua 4.0 và ~96 tên thư viện chuẩn Lua 4.0. Lưu ý Lua 4.0 **chưa có** bảng "
  "`string` / `table` / `math` — mọi thứ đều là hàm toàn cục: `strfind`, `getn`, `tinsert`, "
  "`floor`, `format`…")
w("2. Bỏ tên bị che bởi **biến cục bộ / tham số / biến vòng lặp** trong chính tệp đó "
  "(`local f = ...; f()`, `function g(cb) cb() end`, `for k, value in ...`). Không lọc bước này "
  "sẽ lọt `id`, `name`, `func`, `pos`, `series`, `proceed`, `value`, `org`, `fnCallback`.")
w("3. Bỏ tên do cây script Linux định nghĩa bằng `function <tên>(` hoặc `<tên> = function(`. "
  "**Chỉ tính hàm toàn cục trần**: `function Player:OpenProgressBar(...)` là **phương thức** của "
  "bảng `Player`, không định nghĩa global `OpenProgressBar` — nếu tính nhầm sẽ bỏ sót 3 hàm "
  "engine thật (`OpenProgressBar`, `SendScriptData`, `GetRoomItems`).")
w("4. Xoá nội dung chuỗi và chú thích trước khi quét, nhưng **giữ nguyên số dòng** để mọi chỗ gọi "
  "báo cáo được `tệp:dòng` chính xác.")
w("")

io.open(os.path.join(HERE, "04_api_gap.md"), "w", encoding="utf-8").write("\n".join(o) + "\n")
print("Da ghi 04_api_gap.md  (%d dong)" % len(o))
