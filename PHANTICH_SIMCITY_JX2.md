# PHÂN TÍCH & DỊCH NGƯỢC — TÍNH NĂNG **SIMCITY** (HỆ NGƯỜI CHƠI GIẢ LẬP) TRÊN SERVER JX2/KIẾM THẾ LINUX

Ngày: 15/08/2026 · Phạm vi: **chỉ đọc, chỉ phân tích** — không sửa bất kỳ tệp nào trong `D:\ServerLinux` hay `E:\SourceTuanLe`.

Nguồn khảo sát:
1. **Bản Linux đang chạy**: `D:\ServerLinux\server1` (binary `jx_linux_y`, 8.931.808 B, đã strip section header).
2. **Bản đầy đủ do chủ game cung cấp**: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\SOURCESUPDATE_KINHMACH_ONLTEST0608\simcity\` gồm 4 tệp: `jxser.tar.gz` (66,8 MB), `settings.zip` (704 KB), `vdk.dll` (5,05 MB), `zlib.dll` (87 KB).

Tổng công: 2 vòng đa tác nhân (32 tác nhân, ~6,6 triệu token, 1.786 lượt gọi công cụ) + 4 tổ phản biện độc lập.

---

## 0. TÓM TẮT ĐIỀU HÀNH — 12 điểm chốt

| # | Kết luận | Bằng chứng |
|---|---|---|
| 1 | **SimCity = hệ NGƯỜI CHƠI GIẢ LẬP (fake player)**, không phải hệ xây thành. Bot là **KNpc** (NPC server), không phải KPlayer | `sim.entity.lua:72` gọi `AddNpcEx`; không có API tạo player giả trong 1.521 hàm Lua |
| 2 | Hệ gồm **3 tầng**: native `vdk.so` → 40 tệp Lua (~400 KB) → 283 tệp dữ liệu `.txt` | mục 2 |
| 3 | **`vdk.so` là trái tim của SimCity** — một LD_PRELOAD binary patcher tự viết, vá 26 điểm code + 21 điểm con trỏ trong `jx_linux_y`, và **bơm thêm 51 hàm API Lua** | banner giải mã XOR 0x5C: `Bot Simcity Player = OK`, `Mod VLTK Code By V.D.K` |
| 4 | **Không có `vdk.so` thì không tạo nổi một con bot.** Bot dùng npcId 2000–2023, engine gốc chặn cứng ở 1999; `vdk.so` vá 1999→4999 | `KNpc::GetNpcCopyFromTemplate+0xc: cmp dword [ebp+0xc],0x7cf` |
| 5 | **`vdk.so` KHÔNG tương thích bản `jx_linux_y` trên `D:`** — 21 chữ ký byte khớp **21/21** với binary trong tar, **0/21** với binary trên `D:` | mục 3.6 |
| 6 | Bản tar mang theo **`jx_linux_y` 32 MB còn nguyên `.symtab` (10.259 ký hiệu) + DWARF 21 MB** — kho báu để dịch ngược, khác hẳn bản `D:` đã strip | mục 1.3 |
| 7 | `vdk.dll` **không phải** engine SimCity: tên nội bộ `fProtectClient.dll`, VERSIONINFO `JX1Mod by V.D.K / HoiQuanVoLam`, nhập `Engine.dll` ⇒ module **phía client**, bị đóng gói aPLib | mục 3.7 |
| 8 | `zlib.dll` là zlib 1.1.4 + minizip **nguyên bản, ký số bởi CÔNG TY CỔ PHẦN VNG**, digest khớp 100% — không liên quan | mục 3.8 |
| 9 | Tệp `gateway/s3relay/script/simcity.lua` **trùng tên nhưng không thuộc hệ bot** — chỉ 40 dòng, 2 hàm mở Tống Kim / Phong Hoả, mtime 2026-04-07 (trước đợt viết bot 3 tháng) | mục 9.3 |
| 10 | **Chú thích xác suất trong `config.lua` là SAI** — `CHANCE_AUTO_ATTACK/JOIN_FIGHT/ATTACK_PLAYER` ghi "1/8000, 1/3000" nhưng mã thật cho **xác suất 100%** | mục 5.4.1 |
| 11 | Bot **có ảnh hưởng cục diện Tống Kim thật**: `fightingScore` của bot → `g_simBotPointS/J` → quyết định **phe nào ra Nguyên Soái trước** | `battles/marshal/simtk.lua:49-55` |
| 12 | Bot **cày kinh nghiệm hộ người chơi**: bot trong tổ đội giết quái → exp cộng thẳng cho chủ nhóm | `activitysys/g_npcdeath.lua:29-49` |

---

## 1. PHƯƠNG PHÁP & HẠ TẦNG DỊCH NGƯỢC

### 1.1 Bản `D:` — chỉ còn xác dữ liệu

Quét byte toàn bộ **13.861 tệp** của `D:\ServerLinux`: chuỗi `simcity` chỉ xuất hiện ở **2 chỗ**:

- `server1\script\global\mel\mission\tongkim.lua:32-36` → `RemoteExc("\script\simcity.lua","Mo_TongKim",{1|2|3})`
- `server1\settings\item\004\magicscript_old.txt` → item 5130 "Lệnh bài chiến loạn" và 5131 "Lệnh bài kéo xe" trỏ tới `\script\global\vinh\simcity\controllers\main.lua` / `keoxe.lua`

Chuỗi `simcity` **không có trong bất kỳ binary nào** trên `D:` (`jx_linux_y`, `GameExtContent.so`, `GameExtConnect.so`, `KG_Angel.so`, `S2SSyncRelayD`, `libheaven.so`, `librainbow.so`).

Còn lại là **dữ liệu**: `D:\ServerLinux\server1\settings\global\vinh\simcity\` (283 tệp). Mã Lua + `vdk.so` đã bị gỡ khỏi cây `D:`.

### 1.2 Bộ đồ nghề tự dựng

Binary trên `D:` bị **strip sạch section header** (`e_shoff = 0`), nên phải khôi phục bằng đường khác:

| Công cụ (trong scratchpad) | Chức năng |
|---|---|
| `sctool.py` | Đọc ELF qua **program header**; khôi phục **1.521 hàm API Lua** (quét cặp `{char* tên, void* hàm}` liên tiếp ≥5); lấy **7.160 ranh giới hàm** từ `.eh_frame_hdr`; 248 PLT stub; `dis` / `xref` / `str` / `bytes` |
| `symmap.py` | Ánh xạ **tên Lua → ký hiệu C++ thật** từ `.symtab` của bản tar |
| `so_syms.py`, `pe_info.py` | Đọc ELF `.so` và PE `.dll` |

Lớp Lua C API đã giải được (dùng cho mọi dịch ngược sau đó), trên bản `D:`:

```
0x08232490 = lua_gettop(L)
0x082338b0 = lua_tonumber(L, idx) -> double (trả trong st0)
0x08233850 = lua_tostring(L, idx) -> const char*
0x08232d40 = lua_pushnumber(L, double)
0x082337a0 = lua_pushstring(L, const char*)
0x08232590 = lua_type(L, idx)
0x082325d0 = lua_typename(L, t)      -- bảng tên tại 0x0826da00
```

**Bảng tag Lua 4.0 của bản build này** (đọc từ bảng `lua_typename`, không đoán):

| tag | -1 | 0 | 1 | 2 | 3 | 4 | 5 | 7 |
|---|---|---|---|---|---|---|---|---|
| tên | `no value` | `userdata` | `nil` | **`number`** | `string` | `table` | `function` | `lightuserdata` |

Mẫu mã điển hình của một hàm API:

```asm
call lua_gettop ; cmp eax,N ; jle fail      => yêu cầu > N tham số
mov [esp+4],i ; call lua_tonumber           => đọc tham số thứ i kiểu số
fistp qword [ebp-X] ; mov reg,[ebp-X]       => ép double -> int
imul reg,0x1a4c ; add reg,[0x0836eae0]      => mảng KNpc, sizeof=0x1a4c=6.732 B, số lượng tại [0x0830ca58]
fld1 (hoặc fld1+fchs) ; call lua_pushnumber ; mov eax,1 ; ret   => trả 1 hoặc -1
```

### 1.3 Kho báu trong bản tar: binary **chưa strip**

`jxser/server1/jx_linux_y` — **32.158.677 B**, MD5 `89968af8…`, khác hoàn toàn bản `D:` (8.931.808 B, MD5 `cf6b2a69…`, entry `0x804ac30` vs `0x804b7c0`).

```
[12] .text          addr=0x804ac30  size=0x1fe328
[25] .bss           addr=0x82b85c0  size=0x11e6834
[29] .debug_info    size=0x145843f  (21,3 MB)
[33] .debug_str     size=0x276401
[37] .symtab        size=0x029cb0   -> 8.049 ký hiệu FUNC
```

⇒ Mọi hàm API Lua đều tra ngược được **tên C++ thật**. Ví dụ:

| Tên Lua | VA | Ký hiệu C++ |
|---|---|---|
| `AddNpcEx` | `0x0812b900` | `LuaAddNpcEx(lua_State*)` |
| `NpcWalk` | `0x081293b0` | `LuaNpcWalk(lua_State*)` |
| `NpcChat` | `0x0813d730` | `LuaNpcChat(lua_State*)` |
| `SetNpcParam` | `0x081d5350` | `Lua_SetNpcScriptParam(lua_State*)` |
| `GetNpcParam` | `0x081d5890` | `Lua_GetNpcScriptParam(lua_State*)` |
| `SetNpcScript` | `0x08118d70` | `LuaSetNpcActionScript(lua_State*)` |
| `ChangeNpcFeature` | `0x08139fb0` | `LuaChangeNpcFeature(lua_State*)` |
| `TabFile_Load` | `0x081601a0` | `LuaTabFile_Load(lua_State*)` |
| `PET_Create` | `0x081eba70` | `LuaPET_Create(lua_State*)` |

Bảng đầy đủ 1.421 dòng: `scratchpad\luaapi_symbols.md`.

---

## 2. KIẾN TRÚC — BA TẦNG

```
        ┌──────────────────────────────────────────────────────────────┐
TẦNG 3  │ DỮ LIỆU  settings/global/<vinh|sang|vdk>/simcity/  283 tệp   │
        │  names.txt · chat.txt · skills.txt · pets.txt ·              │
        │  npcid2faction.txt · maps/{thanhthi,attractions,haudoanh,    │
        │  trangtri}                                                    │
        └──────────────────────────────▲───────────────────────────────┘
                                       │ TabFile_Load / GetCell
        ┌──────────────────────────────┴───────────────────────────────┐
TẦNG 2  │ LUA  script/global/nobitaxd/vdk/simcity/  40 tệp (~400 KB)   │
        │  components/ sim.core(49K) sim.movement(58K) sim.fight(21K)  │
        │              sim.entity(13K) sim.fun sim.timer                │
        │  class/     sim_citizen sim_theosau group_fighter(chết)      │
        │  libs/      data walk walk_chientranh common                  │
        │  plugins/   pthanhthi pchientranh ptongkim pvatnuoi ...       │
        │  controllers/ main thanhthi tongkim keoxe batanh ...          │
        └──────────────────────────────▲───────────────────────────────┘
                                       │ 51 hàm API mới (NpcRun, BotDoSkill, ...)
        ┌──────────────────────────────┴───────────────────────────────┐
TẦNG 1  │ NATIVE  vdk.so (54 KB) — LD_PRELOAD binary patcher           │
        │  · nâng trần NPC template 2000 → 5000                         │
        │  · hook 26 điểm code + 21 điểm con trỏ trong jx_linux_y      │
        │  · hook KLuaScriptSet::RegisterFunctions để bơm 51 hàm Lua   │
        └──────────────────────────────▲───────────────────────────────┘
                                       │
        ┌──────────────────────────────┴───────────────────────────────┐
TẦNG 0  │ GAMESERVER  jx_linux_y (ELF32 i386, ET_EXEC @ 0x08048000)    │
        │  1.421 hàm API Lua gốc: AddNpcEx, NpcChat, SetNpcParam, ...  │
        └──────────────────────────────────────────────────────────────┘
```

**Điểm mấu chốt về cơ chế nạp script**: engine **nạp và chạy TOÀN BỘ cây `\script` mỗi lần khởi động** —
`g_InitCore → g_IniScriptEngine @0x08067060 → KLuaScriptSet::LoadScriptInPath(g_ScriptSet,"\script") @0x0821df80`.
Log thật (`Logs/KSG_ScriptOutputLog_20260705.txt`): **6.574 tệp/lần boot**, và **40/40 tệp trong `simcity/` đều chạy**.
Cái làm cho các tệp "không ai Include" trở nên vô hại là **cơ chế pack/namespace** của Lua (`lua_usepack @0x0822c6d0`), **không phải** vì chúng không chạy. Hệ quả: **tác dụng phụ engine ở mức top-level vẫn xảy ra thật** — ví dụ `AddTimer` trong `simcity/main.lua:22-23`.

---

## 3. DỊCH NGƯỢC MODULE NATIVE `vdk.so`

`jxser/server1/vdk.so` · 54.084 B · MD5 `40420c2c3950ed50744d57bbe782e288` · Build-ID `801fd048…`

```
ELF ET_DYN i386, entry=0, 8 phdr, 17 shdr
LOAD .text   off 0x001000 vaddr 0x001000 filesz 0x00a6e4  R-X
LOAD .rodata off 0x00c000 vaddr 0x00c000 filesz 0x000cac  R--
LOAD .bss    off 0x00cf2c vaddr 0x00df2c memsz  0x9bbeec  (10,2 MB)
DYNSYM: 0 xuất — chỉ nhập __divdi3, __udivdi3, strlen
.init_array[0] -> 0x7210      .fini_array[0] -> 0x2f70
```

### 3.1 Danh tính — banner mã hoá XOR 0x5C

Vùng `0x0c8c0..0x0cc3f` lưu chuỗi XOR 0x5C (byte NUL giữ nguyên). Giải mã:

```
*** MOD NAY SHARE FOR FREE, NEU BAN MUA LA DA BI LUA DAO !!! ***
TeamUi Mana                            = OK / PARTIAL (only n/4 hooks — some sigs mismatch)
Expanded Buff Cap (6+)                 = ...
Bot Simcity Player                     = OK / PARTIAL / SKIP (sig mismatch)
Fixed Title Occasionally Disappearing  = OK / SKIP (sig mismatch @ 0x0816EF78 - bug returns!)
MaxNPCS                                = ...
=================== Mod VLTK Code By V.D.K ===================
```

⇒ Module **tự khai tên tính năng trung tâm đúng là "Bot Simcity Player"**, tác giả **V.D.K** — khớp 100% với đường dẫn `script/global/nobitaxd/vdk/simcity`.

### 3.2 Ba nguyên thủy vá — đều dùng `int 0x80` (syscall trực tiếp, không qua libc)

Đây là lý do module chỉ nhập `strlen`: nó gọi thẳng `mmap` (eax=90) và `mprotect` (eax=125).

| Nguyên thủy | VA | Kỹ thuật |
|---|---|---|
| **DETOUR** (có trampoline) | `0x1030` | `mmap(0,0x1000,RWX,PRIVATE\|ANON)` → chép N byte gốc sang trang mới → nối `E9 rel32` quay về `target+N` → lưu con trỏ trampoline → ghi `E9 rel32` lên `target` |
| **JMP5** (không trampoline) | `0x2a50` | So khớp 5 byte chữ ký → `mprotect(page,RWX)` → ghi `E9 rel32` → trả 1/0 |
| **CONST** (thay hằng số) | `0x6b20` | Quét `[addr-8, addr+9]` tìm dword `== edx` → `mprotect(RWX)` → ghi đè → `mprotect(R-X)` |

### 3.3 Nâng trần NPC template 2000 → 5000 — **điều kiện sống còn**

`.init_array[0] @ 0x7210` làm 3 việc:

**Bước 1** — cấp phát lại bảng NPC template:
```asm
0x07233  mov dword [esp+0x1c], 0xdbba00   ; length = 14.400.000
0x0723b  mov dword [esp+0x20], 3          ; PROT_READ|PROT_WRITE
0x07243  mov dword [esp+0x24], 0x22       ; MAP_PRIVATE|MAP_ANONYMOUS
0x07211  mov eax, 0x5a  ...  0x07262  int 0x80    ; sys_old_mmap
```
rồi **đổi con trỏ tại 21 điểm mã** trong `jx_linux_y`: 14 điểm trỏ `g_pNpcTemplate` (`0x08320a20`), 7 điểm trỏ `+0x1e0`. Các điểm đó nằm trong: `InitNpcSetting`, `g_ReleaseCore`, `KNpc::ChangeFeature_Disguise` (6), `KNpc::LoadDataFromTemplate`, `KNpc::GetNpcCopyFromTemplate` (9), `KSkill::CastCreateNpc`, `KNpc::AppendBossProp`, `KNpc::Revive`.

Bố cục mảng: `g_pNpcTemplate[2000][6][120]` con trỏ = 5.760.000 B → `[5000][6][120]` = **14.400.000 B = 0xDBBA00**, **khớp chính xác kích thước mmap**.

**Bước 2** — 3 hằng số:

| Vị trí | Gốc | Mới | Ý nghĩa |
|---|---|---|---|
| `KNpc::GetNpcCopyFromTemplate+0xc` (`cmp dword [ebp+0xc],0x7cf`) | 1999 | **4999** | trần chỉ số template |
| `InitNpcSetting+0x21` (đối số 3 của `memset`) | 960.000 | **2.400.000** | 2000×480 → 5000×480 |
| `g_ReleaseCore+0x21` | đọc biến | `mov eax,5000` | số template khi giải phóng |

**Vì sao đây là sống-còn**: `settings/npcs.txt` có 2.025 dòng (id 0…2023); bot SimCity dùng đúng dải **2000–2023** ("Temple 1".."Temple 24"). Không có `vdk.so` thì `AddNpcEx(2000..2023, …)` bị chặn ở trần 1999 ⇒ **SimCity chết ngay con bot đầu tiên**, kể cả khi Lua và dữ liệu đầy đủ.

### 3.4 Bảng hook (26 điểm code + 21 điểm con trỏ)

**20 DETOUR:**

| Địa chỉ | Hàm trong `jx_linux_y` | Trampoline (.bss) |
|---|---|---|
| `0x080bfa80` | `KPlayer::SendData(void const*, unsigned)` | `0x9c9e0c` |
| `0x0809f6f0` | `KNpc::ProcessState()` | `0x3a234` |
| `0x080d2580` | `KPlayer::LeaveTeam(...)` | `0x3a230` |
| `0x080d0700` | `KPlayer::TeamKickOne(...)` | `0x3a228` |
| `0x080f31d0` | `KProtocolProcess::c2sViewEquip(int, uchar*)` | `0x3a22c` |
| `0x080e36a0` | `KPlayerStall::c2sstallrequestadv(uchar*)` | `0xe208` |
| `0x080e2590` | `KPlayerStall::c2sstallbuyitem(uchar*)` | `0xe204` |
| `0x080f2f50` | `KProtocolProcess::c2sTeamProtocol(int, uchar*)` | `0x3a294` |
| `0x0815e760` | `BattleSpace::KBattle::ChangeDataOnType(...)` | `0xe1e0` |
| `0x0809dad0` | `KNpc::OnDeathProcess()` | `0xe1cc` |
| `0x0809d770` | `KNpc::DoRevive()` | `0x30ee64` |
| `0x08094840` | `KNpc::SetRideHorse(int)` | `0x30ee6c` |
| `0x0821ccd0` | **`KLuaScriptSet::RegisterFunctions(TLua_Funcs*, int)`** | `0x3a28c` |
| `0x0815e570` | `BattleSpace::KBattle::ChangeLadder(TLadderInfo*)` | `0x9c9e14` |
| `0x080a1dd0` | `KNpc::ReceiveDamage(...)` | `0x30ee60` |
| `0x080f11f0` | `KProtocolProcess::c2sSendTextCmd(int, uchar*)` | `0x3a290` |
| `0x080f2210` | `KProtocolProcess::TradeApplyStart(...)` | `0xe1f4` |
| `0x080f21e0` | `KProtocolProcess::TradeMoveMoney(...)` | `0xe1f8` |
| `0x080f1e40` | `KProtocolProcess::c2sTradeReplyStart(...)` | `0xe1fc` |
| `0x080f21b0` | `KProtocolProcess::TradeDecision(...)` | `0xe200` |

**4 JMP5** (nhóm `TeamUi Mana`): `KNpc::BroadCastState()+0x138`, `+0x1b3`, `KNpc::SyncCastState(...)+0x50`, `KNpcSet::SyncNpc2Player(...)+0x12a`.

**1 sửa rel32 của lệnh CALL**: tại `KProtocolProcess::c2sViewEquip+0x5f` — lệnh gốc gọi `KPlayer::FindAroundPlayer(unsigned long)` (`0x080c4250`), sau khi vá chuyển hướng sang `vdk.so+0x2210`.

**1 NOP-out 5 byte**: tại `KPlayerTitle::SaveTitles(TDBStateData*)+0x28`, xoá lệnh `call KPlayerTitle::ActiveTitle(...)` — đây là tính năng `Fixed Title Occasionally Disappearing`.

`.fini_array[0] @ 0x2f70` **gỡ hook, trả byte gốc** khi tiến trình kết thúc ⇒ tác giả viết cho phép bật/tắt sạch, không phải mã phá hoại.

### 3.5 Bơm 51 hàm API Lua

Hàm thay thế tại `vdk.so:0x5741` gọi trampoline (`RegisterFunctions` gốc) rồi lặp 51 lần:

```asm
0x05775  mov esi, [0x82b9ecc]      ; g_ScriptSet+0x12c -> lua_State* L
0x05790  mov ebp, 0x822d390        ; lua_pushcclosure
0x05795  mov edi, 0x822d110        ; lua_setglobal
;  lua_pushcclosure(L, fn, 0)  ->  lua_setglobal(L, "BotLadderClear")  ... x51
```

**Bảng 51 hàm** (cột cuối = số lần được gọi trong 5.191 tệp `.lua` của server):

| # | Tên | VA | Gọi | # | Tên | VA | Gọi |
|---|---|---|---|---|---|---|---|
| 1 | `BotLadderClear` | `0x06cf0` | 5 | 27 | `SetBotFaction` | `0x03210` | 1 |
| 2 | `HasPlayerSay` | `0x01790` | 1 | 28 | `SetBotWeaponView` | `0x032f0` | 1 |
| 3 | `PollSayForBot` | `0x08fd0` | 1 | 29 | `EnforceBotHp` | `0x03db0` | 2 |
| 4 | `BotSayLocal` | `0x017d0` | 0 | 30 | `SetNpcLevel` | `0x03bf0` | 10 |
| 5 | `PollDuel` | `0x01800` | 1 | 31 | `BotPlayerMove` | `0x042e0` | 1 |
| 6 | `PollTradeStay` | `0x01890` | 1 | 32 | `SetNpcAtkSpeed` | `0x03cb0` | 5 |
| 7 | `TradeStayClear` | `0x027e0` | 3 | 33 | `SetBotSpeed` | `0x03f50` | 5 |
| 8 | `SendTradeItem` | `0x07030` | 1 | 34 | `BotMountSync` | `0x040b0` | 2 |
| 9 | `PollParty` | `0x01910` | 3 | 35 | `SetNpcRideHorse` | `0x04470` | 3 |
| 10 | `PartyClear` | `0x01f90` | 1 | 36 | **`NpcRun`** | `0x054b0` | **18** |
| 11 | `PartyClearPlayer` | `0x060e0` | 2 | 37 | `SetNpcFightTarget` | `0x04640` | 1 |
| 12 | `PartyRebind` | `0x01d50` | 2 | 38 | `GetNpcLastAttacker` | `0x04700` | 4 |
| 13 | `BotForceCast` | `0x036a0` | 0 | 39 | `GetPlayerPkMode` | `0x047b0` | 5 |
| 14 | `GetNpcDoing` | `0x01990` | 4 | 40 | `GetNpcAreaRaw` | `0x048a0` | 0 |
| 15 | `BotDuelArm` | `0x05240` | 2 | 41 | `SetNpcDuelAI` | `0x04a50` | 0 |
| 16 | `BotDuelDisarm` | `0x053d0` | 7 | 42 | `SetNpcDuelEnd` | `0x04ba0` | 1 |
| 17 | `SimEnemyAround` | `0x068d0` | 2 | 43 | `BotDoSkill` | `0x04c80` | 8 |
| 18 | `BotLadderAdd` | `0x05ff0` | 3 | 44 | `BotDashTo` | `0x04f90` | 1 |
| 19 | `BotLadderBroadcast` | `0x06de0` | 3 | 45 | `GetNpcRideHorse` | `0x04570` | 1 |
| 20 | `SetBotPoints` | `0x05680` | 1 | 46 | `NpcSit` | `0x055f0` | 1 |
| 21 | `GetBotPoints` | `0x016e0` | 1 | 47 | `BotDismountSkill` | `0x06b80` | 0 |
| 22 | `SetNpcTitle` | `0x03120` | 7 | 48 | `SetBotStallTier` | `0x06140` | 1 |
| 23 | `SetNpcStall` | `0x033a0` | 2 | 49 | `AddNpcStateInfo` | `0x038f0` | 0 |
| 24 | `SetNpcCombat` | `0x062a0` | 12 | 50 | `BotShowAura` | `0x039f0` | 3 |
| 25 | `SetNpcPeace` | `0x03530` | 1 | 51 | `BotAuraKeepAliveAll` | `0x03b30` | 1 |
| 26 | `SetNpcBang` | `0x03820` | 7 | | | | |

**45/51 hàm có người gọi**, và 13/17 tệp gọi nằm trong thư mục `simcity`:
`sim.core.lua` 74 lượt · `sim_citizen.lua` 17 · `sim.movement.lua` 13 · `group_fighter.class.lua` 10 · `sim.fight.lua` 9 · `sim.entity.lua` 8 · `simsevencity.lua` 4 · `g_npcdeath.lua` 2 · `sim.fun.lua` 2 · `pworld.lua` 2 · `smalltimer.lua` 1 · `login.lua`/`logout.lua` 1.

**Bảng từ khoá chat tiếng Việt** (`0x0c2b3..0x0c324`) — dùng bởi `HasPlayerSay`/`PollSayForBot` để bot **đọc câu chat của người chơi thật và trả lời theo từ khoá**:

```
mua · ban · gia · bao nhieu · doi · boss · sat thu · giup · ho tro · thoi ·
khong · cut · deo · hem · ok · dong y · duoc · chien · ukm · chao · alo · hello
```

### 3.6 Ràng buộc bản build — **21/21 vs 0/21**

`vdk.so` kiểm 21 chữ ký byte trước khi vá; sai thì in `SKIP (sig mismatch)`.

| Binary | Kích thước | MD5 | Kết quả |
|---|---|---|---|
| `jxser/server1/jx_linux_y` (tar) | 32.158.677 | `89968af8cc013748ba8dccc4ad5f58bd` | **21/21 KHỚP** |
| `D:\ServerLinux\server1\jx_linux_y` | 8.931.808 | `cf6b2a697bf7fb65550a4ab396f239fb` | **0/21 KHỚP** |

Mẫu kiểm (7 điểm tôi tự đo lại độc lập): `[0x8067951]==0xa1`, `[0x8094840]==0x55`, `[0x809d770]==0x55`, `[0x816ef78]==0xe8`, `[0x80a1dd0]==0x55`, `[0x815e570]==0x55`, `[0x821ccd0]==0x55` → bản `D:` cho `0x89 / 0x04 / 0x12 / 0xc0 / 0x00 / 0xeb / 0xc6`.

### 3.7 `vdk.dll` — module CLIENT, không phải engine bot

```
PE32 i386, ImageBase 0x10000000, build 2026-07-05 07:02:23 UTC
Export Directory (tên nội bộ): fProtectClient.dll  ->  EntryProc (1 hàm duy nhất)
Import: kernel32(GetModuleHandleA) USER32(GetClientRect) GDI32(CreateDIBSection)
        WS2_32(WSACloseEvent) MSIMG32(AlphaBlend)
        Engine.dll(?GetInteger@KIniFile@@QAEHPBD0HPAH@Z)
Sections: 5 section KHÔNG TÊN (entropy 7,82–7,98) + UPX0 (vsz 0x6d6000, raw 0) + .boot (0x45aa00)
Entry point nằm TRONG .boot -> stub giải nén aPLib
VERSIONINFO: CompanyName=HoiQuanVoLam · FileDescription=JX1Mod by V.D.K · OriginalFilename=vdk.dll
```

`?GetInteger@KIniFile@@QAEHPBD0HPAH@Z` = `KIniFile::GetInteger(char const*, char const*, int, int*)` — API **phía client** (server Linux không có `Engine.dll`). Cùng `CreateDIBSection` + `AlphaBlend` + `GetClientRect` ⇒ vẽ giao diện 2D có alpha.

Quét 5.049.856 B: 9.513 chuỗi ASCII, **không có chuỗi `simcity` nào**; 21 chuỗi UTF-16 đều thuộc VERSIONINFO.

**Phát biểu chính xác**: `vdk.dll` là **module client do cùng tác giả V.D.K**, bị đóng gói protector nên **không đọc được nội dung**. Nó **không phải** engine SimCity (SimCity chạy 100% phía server). Tuy nhiên **chưa loại trừ** khả năng nó mang phần hiển thị phía client cho bot, vì: (a) nằm cùng gói giao hàng 4 tệp; (b) build 2026-07-05, cách `vdk.so` (2026-07-04 13:29) ~18 giờ; (c) mã Lua ghi rõ `sim.fun.lua:13`:
```lua
NpcChat(tbNpc.finalIndex, msg)   -- [2026-06-21] tam revert NpcChat (bong bong). CH_NEARBY can fix client DLL (BotSayLocal de sau)
```
và đúng là `BotSayLocal` có trong bảng 51 hàm nhưng **0 lời gọi Lua**.

### 3.8 `zlib.dll` — nguyên bản VNG, không liên quan

zlib 1.1.4 + minizip, 65 hàm xuất chuẩn, không thừa không thiếu. Có chữ ký Authenticode:
`Subject: CONG TY CO PHAN VNG, Ho Chi Minh City, District 7, VN` · `URL: https://volam.zing.vn` · `Issuer: DigiCert Trusted G4 Code Signing RSA4096 SHA384 2021 CA1`.
Tự tính lại digest: `sha1 = 6f136dd7…`, `sha256 = 08fe0226…` — **đều có trong khối chữ ký ⇒ tệp không bị sửa một byte nào**.

---

## 4. CÁC HÀM NATIVE CỦA GAME MÀ SIMCITY DÙNG

### 4.1 Ví dụ dịch ngược đầy đủ — `NpcWalk`

`NpcWalk` @ `0x08118e50` (bản `D:`) / `0x081293b0` = `LuaNpcWalk(lua_State*)` (bản tar):

```asm
0x08118e65  call lua_gettop ; cmp eax,2 ; jle fail      ; yêu cầu > 2 tham số
0x08118e7e  call lua_tonumber(L,1)  -> esi = nNpcIdx
0x08118ea7  call lua_tonumber(L,2)  -> x
0x08118eba  call lua_tonumber(L,3)  -> y
0x08118ec1  test esi,esi ; jle fail
0x08118ec3  cmp  esi,[0x0830ca58] ; jge fail            ; 0 < idx < số NPC tối đa
0x08118ece  imul esi,esi,0x1a4c ; add esi,[0x0836eae0]  ; KNpc*
0x08118f0f  shl  eax,5   -> [esp+0xc]                   ; y * 32
0x08118f19  shl  eax,5   -> [esp+8]                     ; x * 32
0x08118f20  call 0x8078aa0(npc, 2, y<<5, x<<5, 0)
0x08118f28  fld1 ; call lua_pushnumber ; mov eax,1 ; ret    ; trả 1
fail:       fld1 ; fchs ; call lua_pushnumber ; ret          ; trả -1
```

⇒ **`NpcWalk(nNpcIdx, nX, nY) -> 1 | -1`**, toạ độ nhận theo **ô lưới** (engine nhân 32).

**Đơn vị toạ độ — đã chốt bằng 4 chứng cứ độc lập:**

| Chứng cứ | Nội dung |
|---|---|
| `GetWorldPos` @ `0x08128f30` | `sar eax,5` trước khi `lua_pushnumber` ⇒ Lua thấy pos/32 |
| `NpcWalk` @ `0x08118e50` | `shl eax,5` ⇒ nhận ô lưới |
| `SetNpcPos` / `AddNpcEx` | **không** có `shl …,5` ⇒ nhận **mps** — đó là lý do mọi script viết `nX*32` |
| Dữ liệu | Khoảng cách attraction → node gần nhất: **trung vị 1,00 ô**; 179/210 ≤ 5 ô |

### 4.2 Bảng API game mà SimCity dùng (trích)

| Hàm | Vai trò trong SimCity |
|---|---|
| `AddNpcEx(nNpcId, nLevel, nSeries, nMapIdx, nX32, nY32, 1, szName, 0)` | **nơi duy nhất** sinh bot của khung `SimCitizen`/`SimTheoSau` (`sim.entity.lua:72`) |
| `SetNpcParam(idx, 1\|3\|4, v)` / `GetNpcParam` | 3 ô lưu trạng thái: slot 1 = id trong `fighterList`, slot 3 = lớp (1 citizen, 2 keoxe), slot 4 = cờ "là bot SimCity" |
| `SetNpcScript(idx, "…/sim.timer.lua")` | gắn script bắt sự kiện `OnDeath` |
| `DelNpc` / `NpcIdx2PIdx` | `DelNpcSafe` chỉ xoá khi `NpcIdx2PIdx <= 0` (tránh xoá nhầm thân người chơi) |
| `ChangeNpcFeature` | đổi ngoại trang bot |
| `NpcChat` | bong bóng thoại |
| `GetNpcPos`, `GetNpcKind`, `GetNpcName`, `SetNpcCurCamp` | truy vấn/gán phe |
| `TabFile_Load/GetCell/GetRowCount/Search/UnLoad` | nạp toàn bộ 283 tệp dữ liệu |
| `AddTimer(nTick, "hàm", đối tượng)` | nhịp `mainLoop` (18 tick) và `worldLoop` (54 tick) |
| `NpcDropMoney` | rớt tiền khi bot chết (đang tắt bằng `CHANCE_DROP_MONEY = 0`) |

**Đáng chú ý — SimCity KHÔNG dùng:** `SetNpcPos`, `SetMoveSpeed`, `NpcNewWorld` (grep toàn thư mục = 0 kết quả), `PET_*` (hệ thú cưng dùng đường khác — xem 5.8), và **không một `BT_*`/`BATTLE_*` nào áp lên bot**.

---

## 5. CÁC HOẠT ĐỘNG CỦA HỆ GIẢ LẬP

### 5.1 Vòng đời một bot

```
[1] SINH    pthanhthi:_createSingle / autoCreateNpc  ->  SimCitizen:New(config)
              -> initCharConfig: bốc tên, phái, series(ngũ hành), giới tính
                 (nSettingsIdx: -1 = NAM, -2 = NỮ), kỹ năng, vũ khí
              -> gắn 4 hệ thống con (strategy pattern):
                    movementSys · funSys · entitySys · fightSys
              -> cấp nListId (tái dùng removedIds trước, rồi mới counter)
[2] TẠO     sim.entity.lua:72  AddNpcEx(nNpcId, _spawnLv, series, mapIdx, X*32, Y*32, 1, name, 0)
              _spawnLv = level nếu 1..119, ngoài khoảng ép về 95
              lọc: GetNpcKind(idx) ~= 0  ->  DelNpcSafe  (chỉ nhận NPC "đánh được")
              SetNpcParam(idx,1,id) · SetNpcParam(idx,3,1|2) · SetNpcParam(idx,4,1)
              SetNpcScript(idx, sim.timer.lua)
[3] SỐNG    mainLoop mỗi 18 tick (=1 giây) -> SimCitizen:ATick() -> OnTimer(mỗi bot)
[4] CHẾT    sim.timer.lua:OnDeath -> SimCitizen:OnDeath -> funSys:OnDeath
              nếu còn "con" sống thì TRÁO THÂN (hoán đổi finalIndex)
              nếu noRevive = 0 -> hồi sinh tại chỗ
[5] DỌN     removeAll(map) khi người chơi cuối rời bản đồ (sau 10 giây)
```

### 5.2 Nhịp chạy

```lua
main.lua:22-23   AddTimer(REFRESH_RATE,   "mainLoop",  SimCitizen)   -- 18 tick = 1 giây
                 AddTimer(REFRESH_RATE*3, "worldLoop", SimCityWorld) -- 54 tick = 3 giây
config.lua:39    REFRESH_RATE = 18
```
```lua
sim.core.lua:1002-1015
    function SimCore:ATick(rate)
        if self.totalFighters <= 2000 then
            for _, f in self.fighterList do self:OnTimer(f, rate) end
            return
        end
        for _, f in self.fighterList do
            if f.processGroup == self.currentProcessGroup then self:OnTimer(f, rate) end
        end
        self.currentProcessGroup = self.currentProcessGroup == 1 and 2 or 1
    end
```

🔴 **Hiệu ứng phụ chưa được ghi ở đâu**: `ATick()` gọi **không tham số** ⇒ `rate = nil` ⇒ `sim.core.lua:821 local tickRate = rate or 1` ⇒ luôn `= 1`, **kể cả khi đang chia pha**. Vượt 2.000 bot thì mỗi bot chỉ chạy 2 giây/lần nhưng `tick_breath` vẫn chỉ +1 ⇒ **mọi mốc thời gian của bot (hồi chiêu, thời lượng đánh, nghỉ, quét mục tiêu) tự động dài gấp đôi**.

### 5.3 Di chuyển

**Toàn bộ di chuyển đi qua `NpcRun` (hàm của `vdk.so`)** — đếm thật: `sim.movement.lua` 11 lượt + `sim.core.lua` 7 lượt = **18**, khớp bảng 51 hàm.
`NpcWalk` (hàm gốc của game) **đã bị tắt tay**:
```lua
sim.movement.lua:242  if random(1, 100) <= 0 then NpcWalk(tbNpc.finalIndex, myPosX+random(-2,2), myPosY+random(-2,2)) end
```

**Đi theo lộ trình preset** = con trỏ ±1 kiểu con thoi (`sim.movement.lua:464-489`), tới đầu/cuối thì đảo `pathDirection`.
**Chuyển chặng** mới là chỗ tìm đường thật:
```lua
sim.movement.lua:335  local paths = SimCityGraphToChienTranh:find_all_paths(nodes, currentNodeName, nextNodeName, 0)
walk_chientranh.lua:241-245  if use_dfs == 1 then depth_first_search(10, ...) else breadth_first_search(10, ...) end
walk_chientranh.lua:145      self:shuffle(neighbors)     -- xáo trộn để mỗi bot đi khác nhau
walk_chientranh.lua:174      local max_path_length = 100
```
⇒ **DFS/BFS ngẫu nhiên hoá, tối đa 10 đường, dài tối đa 100 node** — không phải A* (không heuristic, không cost), nhưng **có** thuật toán tìm đường.

**Ý nghĩa 2 cột chưa rõ trước đây — nay chốt bằng mã:**
```lua
sim.movement.lua:975-980
    if node.isExact == 1 then
        NpcRun(tbNpc.finalIndex, nX, nY)                       -- tới ĐÚNG ô
    else
        local targetPos = randomRange({nX, nY}, tbNpc.walkVar or 2)
        NpcRun(tbNpc.finalIndex, targetPos[1], targetPos[2])   -- rung ngẫu nhiên ±2 ô
    end
```
```lua
data.lua:135   nodeType = nodeType,   -- 0: normal, 1: war
```
Đo dữ liệu thật trên 148 tệp / 15.866 dòng node: `is_exact=0` **15.854** dòng, `is_exact=1` chỉ **12** dòng (3 tệp) ⇒ **99,92 % lộ trình đi kiểu rung ±2 ô**. `type=1` 3.441 dòng / `type=0` 12.425 dòng.

**Các ngưỡng khoảng cách** (`config.lua`): `DISTANCE_CAN_CONTINUE=5` (dùng, 2 nơi), `DISTANCE_FOLLOW_PLAYER=28`, `DISTANCE_SUPPORT_PLAYER=8`, `DISTANCE_FOLLOW_PLAYER_TOOFAR=30`, `DISTANCE_VISION=15`. `DISTANCE_CAN_SPIN=2` và `SPINNING_WAIT_TIME=0` **không được dùng** trong `sim.movement.lua`.

### 5.4 Chiến đấu

#### 5.4.1 🔴 Chú thích xác suất trong `config.lua` là SAI

Chú thích:
```lua
config.lua:1  CHANCE_AUTO_ATTACK  = 1    -- 1/8000 co hoi chuyen sang chien dau
config.lua:2  CHANCE_JOIN_FIGHT   = 1    -- 1/3000 co hoi tham gia danh nhau khi di ngang qua dam danh nhau
config.lua:3  CHANCE_ATTACK_PLAYER = 1   -- 1/3000 co hoi danh nguoi neu den gan nguoi choi dang chien dau
```
Mã thật:
```lua
sim.movement.lua:829  if (tbNpc.CHANCE_JOIN_FIGHT   and random(0, tbNpc.CHANCE_JOIN_FIGHT)   <= 2) then
sim.movement.lua:839  if ((tbNpc.CHANCE_ATTACK_PLAYER and random(0, tbNpc.CHANCE_ATTACK_PLAYER) <= 2) or ...)
sim.movement.lua:847  if (tbNpc.CHANCE_ATTACK_NPC   and random(1, tbNpc.CHANCE_ATTACK_NPC)   <= 2) then
```
`random(0,1) ∈ {0,1}` và `random(1,1) = 1` ⇒ **cả ba đều luôn `<= 2` ⇒ xác suất 100 %**, không phải 1/3000 hay 1/8000. Chú thích là di sản của phiên bản cũ.
👉 **Ai chỉnh 3 số này để "giảm bot đánh nhau" sẽ không thấy tác dụng gì cho tới khi đặt ≥ 3.**

#### 5.4.2 Cổng thật chặn bot đánh nhau

```lua
libs/common.lua:344-359  function SimCityCanFight(tbNpc)
    if SIMBOT_NOFIGHT_MAPS[tbNpc.nMapId] then return 0 end
    if wi.cityPeace == 1 and SimCityIsInCity(wi, nX32/32, nY32/32) == 1 then return 0 end
    local _peCD = _pe and _pe > 0 and GetPlayerPkMode(PIdx2NpcIdx(_pe)) ~= 0
    if tbNpc.mode ~= "train" and not (tongkim == 1 and tkWarStarted == 1) and not _peCD and not duelPlayerId then return 0 end
```
`sim.movement.lua:820` bọc toàn bộ khối kích-hoạt-đánh bằng `SimCityCanFight(tbNpc) == 1`.
⇒ **Bot thành thị mặc định câm lặng**; xác suất 100 % ở trên chỉ bung ra khi **có người chơi khác phe ĐANG BẬT PK ở gần**.

#### 5.4.3 Nhịp phát chiêu

```lua
sim.core.lua:980-984  if mod(tbNpc.tick_breath, 10*18/REFRESH_RATE) == 0 then ... fightSys:Update(...)
sim.fight.lua:55-56   if tbNpc.isPlayerEnemyAround == 0 and (random(1,1000) > 50) then return end
```
- **Bot ⟷ bot**: `Update` 10 giây/lần × 5 % ⇒ trung bình **1 chiêu / 200 giây**.
- **Bot ⟷ người**: `Update` **mỗi giây, bỏ qua van 5 %**, chỉ bị `tick_canCast` (2 giây) chặn.
⇒ Chênh lệch thực tế **~100 lần**.

#### 5.4.4 Lỗi trong mã chiến đấu

| Lỗi | Vị trí | Hệ quả |
|---|---|---|
| `tbNpc.fighting` — trường không tồn tại (đúng phải là `isFighting`) | `sim.fight.lua:45` | cổng chặn vô hiệu (Lua 4: `nil == 0` là false) |
| `if nil and tbNpc.selfDefDuel == 1 …` | `sim.core.lua:493` | cả khối 493-500 (gồm `SimBotTaunt`) không bao giờ chạy |
| `config.capHP = config.capHP or 1` | `sim.core.lua:33` | điều kiện `capHP ~= "auto"` **luôn đúng** ⇒ **máu bot luôn `random(60000,120000)`**; 4 mức "Đệ tử tinh anh / Cao thủ nhất lưu / Tuyệt đỉnh cao thủ / Võ lâm chí tôn" trong menu GM **không tạo khác biệt nào** |
| `TIME_FIGHTING = 6000` > chu kỳ cuộn `tick_breath` (1800) | `config.lua:20-23` vs `sim.core.lua:926-931` | thời lượng đánh **không do `TIME_FIGHTING` quyết định** |
| `SIMBOT_AGGRO_PLAYER` định nghĩa trong `group_fighter.class.lua` (khác pack) | `sim.movement.lua:41` | luôn `nil` ⇒ **chỉ bot `mode=="train"` mới chủ động nhắm người chơi** |
| Tráo thân quên gán `child.nNpcId = tmp.nNpcId` | `sim.entity.lua:244-274` | sau lần tráo đầu, `child.nNpcId` lệch khỏi thân xác thật |
| Rò rỉ id + `totalFighters` khi `worldInfo == nil` | `sim_citizen.lua:15-40, 62-70` | có thể **ép server vào chế độ chia pha sớm** (bot chạy chậm gấp đôi) dù chưa tới 2.000 bot |

### 5.5 Sinh/dọn theo người chơi (EventSys)

```lua
vdk/main.lua:22-26   for id, map in SimCityMap do
                         EventSys:GetType("EnterMap"):Reg(id, SimCityThanhThi.onPlayerEnterMap, SimCityThanhThi)
                         EventSys:GetType("LeaveMap"):Reg(id, SimCityThanhThi.onPlayerExitMap, SimCityThanhThi)
                         EventSys:GetType("EnterMap"):Reg(id, SimCityVatNuoi.onPlayerEnterMap, SimCityVatNuoi)
                     end
pthanhthi.lua:481-492
    if (IsTongKimMap(nW) ~= 1 and worldInfo.name ~= "" and worldInfo.playerTrackerCount >= 1 and self:countMap(nW) == 0) then
        self:createNpcSoCapByMap(nW)
    end
    if worldInfo.playerTrackerCount == 0 and worldInfo.isTrainMap ~= 1 then
        if IsTongKimMap(nW) ~= 1 then self:removeAll(nW) else SimCityChienTranh:removeAll(nW) end
    end
```
- Người vào map → chờ **3 giây** rồi sinh bot; người cuối rời map → chờ **10 giây** rồi dọn sạch.
- Điều kiện then chốt: **`countMap(nW) == 0`** — bản đồ còn dù 1 bot thì lượt sinh bị bỏ hẳn.
- Số lượng: `THANHTHI_SIZE = 300` (thành), `THON_SIZE = 50` (thôn), `LUYENCONG_AUTOADD = 1` (bot luyện công ở map dã ngoại).

⚠️ **`AddTimer` trong `main.lua` chạy ở mức nạp tệp, độc lập với Activity 801** (log: `main.lua` nạp ở dòng 2655, Activity 801 mãi dòng 5435). ⇒ **"Tắt Activity 801 để tắt SimCity" là sai** — chỉ ngắt auto-spawn theo EnterMap; vòng `SimCitizen:ATick()` vẫn quay mỗi giây và bot đã tạo vẫn sống.

### 5.6 Tống Kim / chiến trường

- Bot **KHÔNG báo danh qua hệ Battle**. `grep "BT_[A-Za-z]"` trong thư mục simcity: chỉ 2 chỗ và **không chỗ nào áp lên bot**.
- Sinh bot Tống Kim:
```lua
battles/marshal/smalltimer.lua:132-134  if (lsf_level == 3 and t == 1) then simTK:add_npc_simcity(BT_GetGameData(GAME_MAPID)) end
battles/marshal/simtk.lua:245-249       function simTK:add_npc_simcity(idMap)
                                            self:call_npc_simcity(idMap, 2000, 2023, 100, 1)   -- 100 vòng × 2 phe = 200 lượt
```
⇒ **chỉ Tống Kim cao cấp** (`lsf_level == 3`) và **ở nhịp `OnTimer` đầu tiên**.
- `haudoanh.txt`: bot hậu doanh là **hệ tách rời**, **không bao giờ tiến lên**.
- `TONGKIM_SPAWN_MINSTAY/MAXSTAY` dùng đúng 1 chỗ (`sim.movement.lua:642`) và **bị ghi đè mỗi nhịp** bởi `sim.core.lua:712-718`; công tắc thật là `simtk:markWarStarted` gọi từ `battles/marshal/mission.lua:179`.

🔴 **Bot CÓ ảnh hưởng cục diện trận thật:**
```lua
battles/marshal/simtk.lua:49-55
    if (fb.stage or 0) < 1 and el >= 960 then
        local ps = g_simBotPointS or 0; local pj = g_simBotPointJ or 0
        fb.loserCamp = (ps <= pj) and 1 or 2
        local idx = AddNpc(dat[3], dat[4] or 95, fb.mapIdx, dat[1]*32, dat[2]*32, 1, "Nguyen Soai", 1)
```
`g_simBotPointS/J` tính từ **tổng `fightingScore` của bot** (`sim_citizen.lua:146-163`) × hệ số giờ vàng/cuối tuần.
Đường dẫn đầy đủ: bot-đánh-bot → `execAddScoreToAroundNPC` (`sim.fun.lua:100-132`) → `fightingScore` → `g_simBotPoint*` → **thời điểm & phe của Nguyên Soái**.

### 5.7 "Kéo xe" = đoàn tuỳ tùng theo sau người chơi

`SimTheoSau` (`role = "keoxe"`, `PARAM_TYPE = 2`) — bot đi theo **một người chơi cụ thể**, hành vi ở `SimMovement.KeoXe`. Bot kéo xe **mượn tên bang thật của người chơi** (`sim_theosau.lua:45-59`).
Item 5131 "Lệnh bài kéo xe" trỏ tới `controllers/keoxe.lua`.

### 5.8 Thú cưng, tiểu thiếp, ngoại trang

- `pvatnuoi.lua` (725 dòng): **cửa hàng bán pet cho người chơi thật**, không phải bot dắt pet. Đọc `pets.txt` (npcIdx, category, npcName, cost). **Không dùng `PET_*`** — lưu bằng cách nhồi 3 pet vào 5 biến Task 32-bit.
- `ptieuthiep.lua` (417 dòng): NPC nữ đi theo người chơi, có menu tiện ích. `ptieuthiep.lua:400-415` có vòng `while j < 20` sinh **20 quái quanh người chơi** — **không qua `SimCitizen`, không gắn `PARAM_LIST_ID` ⇒ không nằm trong `fighterList`, không ai dọn**.
- `pngoaitrang.lua`: đổi ngoại trang bằng `ChangeNpcFeature`; `init()` **hỏng** vì tên tệp GBK bị phá mã hoá.

### 5.9 Chat

Có **hai đường**:
1. **Đường ngẫu nhiên** (`CHANCE_CHAT = 10/1000 mỗi giây`) nằm trong `funSys:Update` — **không ai gọi** (`grep funSys` = 3 kết quả: 1 gán + 2 gọi `OnDeath`) ⇒ **hiện không chạy**.
2. **Đường phản hồi từ khoá** qua `vdk.so`: `HasPlayerSay` / `PollSayForBot` đọc câu chat người chơi và so với bảng 22 từ khoá (mua/bán/giá/bao nhiêu/boss/chào/…) ⇒ **đây mới là đường chat đang sống**.

### 5.10 Bot cày kinh nghiệm hộ người chơi

```lua
activitysys/g_npcdeath.lua:29-49
-- [PARTY XP 2026-07-01] bot trong nhom giet quai -> gan PlayerIndex = chu nhom + cong exp tay
    if (not PlayerIndex or PlayerIndex <= 0) and GetNpcLastAttacker and PollParty then
        local _botIdx = GetNpcLastAttacker(nNpcIndex)
        local _pp = PollParty(_botIdx)
        local _p4 = GetNpcParam(nNpcIndex, 4)
        if _pp and _pp > 0 and not (_p4 == 1 or _p4 == 2) then     -- KO cộng exp khi giết sim-bot (chống farm)
            PlayerIndex = _pp
            CallPlayerFunction(_pp, AddOwnExp, floor(vGetNpcExp(min(_npcLv,_plLv)) * _rate))
```
Đây là **sửa hàm chết-NPC toàn cục của server** — bot vào tổ đội người chơi, đánh quái, exp cộng thẳng cho chủ nhóm.

---

## 6. DỮ LIỆU — LƯỢC ĐỒ & SỐ LIỆU THẬT

### 6.1 Đồ thị đường đi

| Hạng mục | Số liệu (đếm bằng script) |
|---|---|
| `maps/thanhthi.txt` | 274 dòng dữ liệu, **151 WorldID**, 149 dòng `nodes` + 125 dòng `preset` |
| Tệp trong `maps/thanhthi/` | **270** = 148 `*_nodes.txt` + 122 `*_preset.txt` |
| Dòng node | **15.866** (15.838 node riêng biệt, 28 dòng trùng tên) |
| Cung có hướng | **35.271** · cạnh vô hướng **17.640** |
| Cung **một chiều** | **9** (0,026 %) — đều là lỗi dữ liệu ở `580_phonghoatong`, `904_noname` |
| Node tự trỏ chính nó | 804 |
| Node trỏ tới node không tồn tại | **0** · node mồ côi **0** · node cụt 562 |
| Phân bố bậc | bậc 2 chiếm **79,7 %** (12.626 node) ⇒ phần lớn là mắt xích trên tuyến |
| Phạm vi toạ độ | X 126…4.289 · Y 698…6.358 (ô lưới) |

Lược đồ cột:
- `*_nodes.txt` : `node_name` (dạng `X_Y`, 100 % khớp `^-?\d+_-?\d+$`) · `linked_nodes` (phân cách `,`) · `is_exact` (0/1) · `type` (0/1)
- `*_preset.txt`: `PathName` · `node_name` — **danh sách phẳng theo thứ tự**, các dòng cùng `PathName` nối tiếp tạo thành lộ trình

### 6.2 Các tệp còn lại

| Tệp | Số dòng | Nội dung |
|---|---|---|
| `names.txt` | 738 | tên nhân vật giả ("Trương Tam Phong", "ĐậpTraiĐậpKiếm"…) |
| `chat.txt` | 2.619 | câu thoại phân theo cột **Type** trạng thái (`fighting`, `rep_chui`, …) |
| `npcid2faction.txt` | 61 | npcId → phái / series (ngũ hành) / genere (giới tính) |
| `skills.txt` | 40 | phái, skillId, tên, maxLevel, `noCast`, `castBasic`, `cost` |
| `pets.txt` | 68 | npcIdx, category (Văn lang hùng tộc / Ưng trảo môn / Cuồng lang tộc…), tên, giá |
| `maps/attractions.txt` | 211 | worldId, pX, pY, mô tả (NPC Xa Phu / Thợ rèn / Dược Điếm…), npcId |
| `maps/haudoanh.txt` | 165 | mapId, camp (`haudoanh1`/`haudoanh2`), nodename |
| `maps/trangtri/78_tuongduong.txt` | 71 | NPC trang trí Tương Dương |

Hai giá trị ngoại lai trong `attractions.txt` **là lỗi gõ dư chữ số**: `(37, 16620, 3019)` lệch 14.548 ô và `(162, 15599, 3262)` lệch 13.754 ô (bbox map 37 chỉ X 1480…2082).

---

## 7. BA BIẾN THỂ DỮ LIỆU — BẢN `D:` THUỘC ĐỜI NÀO

| Bản | Vị trí | Ghi chú |
|---|---|---|
| `vinh` | `D:\ServerLinux\server1\settings\global\vinh\simcity\` | bản đang chạy |
| `sang` | `settings.zip → settings/global/sang/simcity/` | |
| `vdk` (zip) | `settings.zip → settings/global/vdk/simcity/` | có thêm `chatworld_msg.txt`, `stall_adv.txt` |
| `vdk` (tar) | `jxser/server1/settings/global/vdk/simcity/` | bản server |

Kết quả đo:
- `vdk`(zip) 283 tệp vs `vdk`(tar) 281 tệp: **280 tệp giống hệt từng byte**, 1 khác (`names.txt`), 2 chỉ có trong zip.
  👉 **`settings.zip` thực chất là bản sao của settings SERVER** (mới hơn 2 tệp), không phải dữ liệu client như tên gọi gợi ý.
- 9 tệp khác nội dung giữa `vinh` và `vdk`: `chat.txt` (85.294 → 90.251 B), `npcid2faction.txt` (1.118 → 1.562 B), `names.txt`, `attractions.txt` (211 → 215 dòng), `thanhthi.txt` (275 → 276 dòng), `skills.txt` (khác 7 dòng), + `11_noname_preset.txt` (712 dòng, **chỉ có ở vdk**).
- **Mã Lua trỏ vào thư mục `vdk`** (`libs/data.lua`), không phải `vinh`.

⇒ **Bản `vinh` trên `D:` là đời CŨ hơn bản `vdk`.** Muốn phục dựng phải dùng bộ `vdk`.

---

## 8. BẢNG LỖI & BẪY PHÁT HIỆN ĐƯỢC

| # | Vấn đề | Mức | Vị trí |
|---|---|---|---|
| 1 | Chú thích xác suất `config.lua:1-3` sai hoàn toàn (100 % thay vì 1/3000) | 🔴 | `config.lua` vs `sim.movement.lua:829/839/847` |
| 2 | `tickRate` luôn = 1 kể cả khi chia pha ⇒ mọi mốc thời gian bot dài gấp đôi khi >2.000 bot | 🔴 | `sim.core.lua:821`, `main.lua:10` |
| 3 | Máu bot luôn `random(60000,120000)` — 4 mức GM vô nghĩa | 🟠 | `sim.core.lua:33-38` |
| 4 | `tbNpc.fighting` sai tên trường ⇒ cổng chặn phát chiêu vô hiệu | 🟠 | `sim.fight.lua:45` |
| 5 | `if nil and …` tắt tay nhánh tự vệ của duel | 🟠 | `sim.core.lua:493` |
| 6 | Tráo thân quên `child.nNpcId` | 🟠 | `sim.entity.lua:244-274` |
| 7 | Rò rỉ `nListId` + `totalFighters` khi tạo thất bại | 🟠 | `sim_citizen.lua:15-40` |
| 8 | `pbatanh.lua` (`role="vantieu"`) **không nằm trong `plugins/index.lua`** ⇒ `SimCityBaTanh` = nil trong pack của `controllers/batanh.lua`; và `role="vantieu"` **không có nhánh** trong 4 hệ con ⇒ rơi về `Citizen` | 🟠 | `pbatanh.lua`, `sim.movement.lua:1375-1383` |
| 9 | `ptieuthiep.lua:400-415` sinh 20 quái không gắn `PARAM_LIST_ID` ⇒ **không ai dọn** | 🟠 | `ptieuthiep.lua` |
| 10 | `pngoaitrang:init()` hỏng vì tên tệp GBK bị phá mã hoá | 🟡 | `pngoaitrang.lua` |
| 11 | `npcdeath_simcity.lua` (8.371 B) **mồ côi** — dễ hiểu nhầm là đường thưởng đang chạy (đường thật là `ptongkim.lua:87-221`) | 🟡 | `battles/marshal/` |
| 12 | `simsevencity.lua` (3.587 dòng) **vá nóng** `SimTheoSau.New` và `SimCityKeoXe.ATick` từ bên ngoài | 🟠 | `missions/sevencity/` |
| 13 | `vinh_OnGlobalNpcDeath` định nghĩa nhưng không ai gọi ⇒ khối "SimCity chết thì đừng rớt gì" **không có hiệu lực** | 🟡 | `g_npcdeath.lua:169-177` |

---

## 9. RỦI RO VẬN HÀNH

| # | Rủi ro | Mức | Bằng chứng |
|---|---|---|---|
| G1 | **Farm điểm + đồ Tống Kim vô hạn** — mỗi bot chết cho điểm tích luỹ, liên trảm, **và rớt vật phẩm thật** (46 món); bot `noRevive = 0` ⇒ hồi sinh liên tục; 200 bot/vòng | 🔴 | `ptongkim.lua:115, 223-241, 193-194`; `simtk.lua:114, 246` |
| G2 | **Nghi vấn mỏ kinh nghiệm** — template 2000-2023 có `ExpParam=100`, bot `level=95`, hồi sinh tức thì. Chốt chặn chống farm **chỉ có cho đường "bot giết quái"**, **không có** chốt cho "người chơi giết bot". *Chưa chắc*: chưa dịch ngược `CalcExp` — **phải đo trước khi bật** | 🔴 | `settings/npcs.txt:2002`; `config.lua:9`; `sim.entity.lua:190` |
| G3 | **Vòi vật phẩm không kiểm toán được** — bot tự bỏ đồ vào ô giao dịch và đưa cho người chơi; **nội dung món đồ do `vdk.so` quyết định, không xuất hiện ở bất kỳ dòng Lua nào** | 🔴 | `sim.core.lua:866  if SendTradeItem then SendTradeItem(tbNpc.finalIndex) end` |
| G4 | **Hỏng câm khi lệch bản binary** — mọi lời gọi API bot đều bọc `if X then …`, khi `vdk.so` sai chữ ký thì hàm biến mất, script **không báo lỗi**, bot chỉ đứng im/không mặc đồ | 🔴 | mục 3.6 |
| G5 | **Không có công tắc tắt thực sự** — timer khởi động ở mức nạp tệp; menu "Tự động thêm (tắt)" chỉ dọn bot của **1 map hiện tại** | 🟠 | `main.lua:22-23`; `pthanhthi.lua:150-184, 412-420` |
| G6 | **Tải máy chủ** — `mainLoop` mỗi giây duyệt toàn bộ `fighterList`; chỉ chia pha khi **>2.000**; ~270 bản đồ đều đăng ký `EnterMap/LeaveMap`; 300 bot/thành × 7 thành + 50/thôn × 8 thôn + 200/trận TK | 🟠 | `main.lua:9-15`; `sim.core.lua:1002-1015` |
| G7 | **Mạo danh** — 738 tên bốc ngẫu nhiên, **không kiểm trùng nhau, không kiểm trùng người chơi thật**; bot Tống Kim được gán chức **"Bang Chủ"**; bot kéo xe **mượn tên bang thật** | 🟠 | `pnpcinfo.lua:454-456`; `sim_citizen.lua:139`; `sim_theosau.lua:45-59` |
| G8 | **Bot chửi người chơi** (`rep_chui`, 6 câu) khi bị truy đuổi rồi chạy vào vùng hoà bình | 🟡 | `sim.core.lua:285-305`; `chat.txt` |
| G9 | **Dễ bị phát hiện** — template "Temple 1..24", `NpcResType = boss003…boss040` (bộ tài nguyên BOSS), `Camp=5`, `Treasure=24` | 🟠 | `settings/npcs.txt:2002-2025` |
| G10 | **Bot chiếm bảng xếp hạng phím F** — `BotLadderAdd` đẩy tối đa 10 mục, bot xếp chung với người thật | 🟠 | `sim_citizen.lua:188-204` |
| G11 | **Bot bám nhầm người chơi qua re-login** (index người chơi bị tái sử dụng) — đã phải vá bằng `PartyClearPlayer` ở login/logout | 🟠 | `login.lua:61`, `logout.lua:26` |
| G12 | **Xung đột chính danh với hệ chống bot** — server có `antibotconfig.ini` + `KSG_AntiBotLog` soi **người chơi**, không soi NPC ⇒ bot SimCity miễn nhiễm; nhưng chính sách "cấm auto" mất tính chính danh khi máy chủ tự chạy 2.000+ nhân vật giả | 🟡 | `antibotconfig.ini` |

---

## 10. NẾU MUỐN CHẠY SIMCITY TRÊN BẢN `D:` — ĐIỀU KIỆN KỸ THUẬT

Đây là kết luận kỹ thuật, **không phải khuyến nghị triển khai**:

1. **Không thể chỉ thả mã Lua vào.** `NpcRun` được gọi **không có cổng bảo vệ nil** ở **11/11** chỗ trong `sim.movement.lua` và **5/7** chỗ trong `sim.core.lua`:
```lua
sim.core.lua:460  NpcRun(tbNpc.finalIndex, _tx, _ty)      -- KHÔNG có cổng
sim.core.lua:582  NpcRun(tbNpc.finalIndex, _bandX, _bandY)
sim.core.lua:587  NpcRun(tbNpc.finalIndex, pX, pY)
sim.core.lua:595  NpcRun(tbNpc.finalIndex, fx, fy)
sim.core.lua:669  NpcRun(tbNpc.finalIndex, _bx, _by)
```
⇒ Mọi bot sẽ ném `attempt to call a nil value` **ngay nhịp `Move` đầu tiên**.

2. **Không thể thả `vdk.so` của tar vào `D:`.** 0/21 chữ ký khớp ⇒ in `SKIP (sig mismatch)` cho mọi tính năng, không vá gì cả, và trần NPC vẫn là 1999 ⇒ `AddNpcEx(2000..2023)` thất bại.

3. **Hai đường khả dĩ**, đều tốn công:
   - **(a)** Dùng đúng bản `jx_linux_y` 32 MB trong tar (đã có `.symtab` + DWARF) — nhưng phải kiểm toàn bộ khác biệt so với bản `D:` đang chạy trước khi đổi binary server.
   - **(b)** Định vị lại **toàn bộ 47 địa chỉ hardcode** (26 điểm hook + 21 điểm con trỏ) của `vdk.so` sang bản `D:`. Bản `D:` đã strip nên phải dò bằng mẫu byte/chữ ký hàm.

4. **Relay của bản `D:` thiếu `simcity.lua`.** `D:\ServerLinux\gateway\s3relay\script\` có 15 mục, **không có** `simcity.lua` (cũng thiếu `protocol.lua`, `startmissions.lua`, `xephang`); bản tar có 17 mục. ⇒ Hai nút GM "Mở Tống Kim" và "Phát động Phong Hoả Liên Thành" sẽ **`RemoteExc` vào hư không** (im lặng, không báo lỗi cho GM).

---

## 11. ĐÍNH CHÍNH & NHỮNG GÌ CHƯA CHẮC

**Đã đính chính trong quá trình làm:**
- Kết luận sớm "`vdk.so` không có chuỗi nào, chưa rõ là gì" là **sai vì đọc thiếu**: `.rodata` tại `0xC000` chứa 51 tên hàm dạng plaintext + 22 từ khoá chat + banner XOR.
- Lập luận "grep không thấy `Include` ⇒ mã chết" (dùng trong 5 báo cáo trung gian) **sai về cơ chế**: server nạp và chạy **100 % tệp `.lua`** mỗi lần boot; cái làm chúng vô hại là **pack/namespace**, không phải "không chạy". Kết luận cuối vẫn đúng với **biến Lua**, nhưng **sai với tác dụng phụ engine**.
- `settings.zip` được mô tả là "phía client" — đo thực tế cho thấy **280/283 tệp giống hệt settings SERVER** trong tar; chỉ `chatworld_msg.txt` và `stall_adv.txt` là riêng.

**Chưa chắc (ghi rõ để không hiểu nhầm):**
- Cơ chế nạp `vdk.so` **suy ra là `LD_PRELOAD`** (chuỗi `vdk.so` không tồn tại trong `jx_linux_y`, module không xuất ký hiệu nào để `dlsym`). Script khởi động không có trong gói ⇒ chưa có bằng chứng trực tiếp.
- Chưa tách được từng mảng trong `.bss` 10,2 MB thành "bảng trạng thái bot" cụ thể (không có ký hiệu). Chắc chắn nó là vùng **trạng thái runtime**, và 10,2 MB tương xứng ~5.000 bot.
- Chưa định danh **thương hiệu** protector của `vdk.dll` (chỉ chắc là nén aPLib, stub `.boot`, tên giả `UPX0`). Muốn biết vai trò thật phải bung động.
- G2 (mỏ kinh nghiệm): **chưa dịch ngược `CalcExp`** để khẳng định engine có trả exp khi người chơi giết bot.
- 28 dòng node trùng tên: đọc theo "gộp danh sách kề" cho 9 cung một chiều, đọc theo "dòng sau đè dòng trước" cho 90. Mã nạp thật (`data.lua`) cần kiểm lại để chốt — số liệu trong tài liệu này dùng ngữ nghĩa **gộp**.

---

## PHỤ LỤC — CÔNG CỤ ĐÃ DỰNG (dùng lại được)

Tất cả nằm trong scratchpad phiên làm việc:

| Tệp | Công dụng |
|---|---|
| `sctool.py` | Bộ đồ nghề chính cho `jx_linux_y` **đã strip**: `info` / `luatab` / `dis` / `xref` / `str` / `strat` / `plt` / `bytes` / `funcs`. Đổi binary bằng biến môi trường `SCBIN` |
| `symmap.py` | Ánh xạ **tên Lua → ký hiệu C++** từ `.symtab` bản tar → xuất `luaapi_symbols.md` (1.421 dòng) |
| `jxsym.py` | Tra cứu `VA → tên hàm C++` tức thì trên bản có symtab |
| `sigcheck.py` | Đối chiếu 21 chữ ký của `vdk.so` với bất kỳ bản `jx_linux_y` nào |
| `vdkso.py` / `vdkdis.py` / `vdkstr.py` / `vdkreg.py` / `vdkhook.py` | Bộ dịch ngược `vdk.so` (ELF, disasm, giải XOR, bảng đăng ký, bảng hook) |
| `pe_info.py` / `pe2.py` | Đọc PE: section, entropy, export, import, VERSIONINFO, chữ ký số |
| `vdk_full.asm` | Disassembly đầy đủ 12.000 lệnh `.text` của `vdk.so` |

Cách dùng nhanh:
```bash
python sctool.py luatab "Npc"
python sctool.py dis NpcWalk --max 200
python symmap.py "^(AddNpcEx|NpcChat)$"
```
