# PHÂN TÍCH — HỆ LIÊN ĐẤU: BẢN LINUX GỐC (leaguematch/WLLS) ĐỐI CHIẾU BẢN ĐANG CHẠY CỦA DỰ ÁN

Ngày: 20/08/2026 · **CHỈ PHÂN TÍCH — chưa sửa gì, chưa build, chưa commit.**

Phạm vi hỏi: (1) dịch ngược bản Linux tìm các hàm liên quan tính năng Liên Đấu, so với bản Liên Đấu hiện tại của dự án khác nhau gì; (2) nếu port sang dự án thì phải viết thêm bao nhiêu hàm; (3) sơ đồ cách hoạt động giữa hai bản.

> **Đường dẫn dùng trong tài liệu này**
> | Vai trò | Đường dẫn |
> |---|---|
> | Mã nguồn C++ của dự án | `D:\GAMEDEVNEW\Sources` |
> | **Máy chủ đang chạy thật** (script + settings + Maps + Pak) | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` |
> | Bản tham chiếu — GameServer Linux | `D:\ServerLinux\server1` (ELF `jx_linux_y`) |
> | Bản tham chiếu — Relay liên máy chủ | `D:\ServerLinux\gateway\s3relay` (ELF `s3relay_y`) |
> | Bản tham chiếu — client đã bung | `D:\ServerLinux\Patch` (`game_y_unpacked.bin`) |
>
> ⚠️ `E:\SourceTuanLe\dulieugame\server` là **cây cũ, không chuẩn** — cùng bố cục, cùng số tệp, lệch nội dung. Mọi số liệu dưới đây lấy từ cây chạy thật.

---

## 0. KẾT LUẬN NGẮN (đọc mục này là đủ để quyết)

1. **"Liên Đấu" bản Linux = `leaguematch`, viết tắt trong mã là `WLLS`** (武林联赛 — *Võ Lâm Liên Sai*). Chuỗi tiếng Việt `"Vũ lâm liên đấu"` nằm ngay trong `leaguematch\head.lua:43`.
2. **Bản gốc là hệ 2 tầng.** Lịch mùa giải + ghép cặp + kho dữ liệu đội **chạy trên RELAY**, còn trận đấu chạy trên GameServer. 7 tệp trong `server1\script\leaguematch\` **không chạy trên GameServer** — chúng là bản sao chết, giống hệt từng byte bản relay (md5 khớp 7/7).
3. **Quy mô chênh nhau ~3,6 lần.** Bản Linux **45 tệp / 5.902 dòng Lua**; bản dự án **10 tệp / 1.635 dòng**.
4. **Hai bản gần như không dùng chung gì về kiến trúc.** Bản dự án là hệ **tự viết 2021** (tác giả ghi trong tệp là *"Fong Kieu"*), đơn đấu 1v1, xếp hạng bằng cách **ghi tệp `.lua` ra đĩa**. Bản gốc là hệ **chiến đội** trên hạ tầng `League` của engine, 7 thể loại, mùa giải theo tháng.
5. **Nhưng có một khối chép nguyên xi:** bảng cấm vật phẩm `WLLS_FORBID_ITEM` của dự án trùng **78/78 mục, đúng từng vị trí, từng con số** với bản Linux (`head.lua:147-230`), giữ cả 2 dòng chú thích gốc `--tinhpn 20100720` và `--Updated by DinhHQ - 20110425`. Chính cái tên biến `WLLS_` là dấu vân tay. ⇒ tác giả bản dự án **đã có trong tay mã leaguematch** nhưng chỉ lấy đúng bảng dữ liệu đó rồi tự viết lại toàn bộ phần còn lại.
6. **Câu trả lời cho "phải viết thêm bao nhiêu hàm": hệ Liên Đấu bản Linux dùng 158 hàm engine, dự án đã có 121 (76,6%), còn thiếu 37.** Trong 37 đó, làm bản tối thiểu chỉ cần **12 hàm**, bản đủ chiều sâu cần **26 hàm**.
7. **Tin tốt lớn nhất: hạ tầng nặng nhất đã có sẵn.** Đợt port công thành chiến JX2 ("ĐỢT E") đã mang sang **đủ 28/28 hàm `LG_*`/`LGM_*`**, 3 hàm `Ladder_*`, `LG_ApplyDoScript`, `GlobalExecute`, và **có persist thật** (`settings\jx2league.txt`, `settings\jx2ladder.txt`). Hệ Mission của engine cũng đủ (`KMission.cpp`, camp, timer). **Bản đồ trùng ID 100%** và **đã có sẵn dữ liệu thật trong `Pak\maps.pak`** — không phải vẽ map mới.
8. **🔴 Ba thứ chặn cứng, phải xử lý TRƯỚC dòng code đầu tiên** (mục 8): lịch mùa giải đã chết từ 28/12/2016; `LuaRunMission` sai chính tả đường dẫn + deref NULL ⇒ **sập GameServer ngay trận đầu**; và hai hệ Liên Đấu sẽ **giành nhau map 396/397/399**.

---

## 1. DỊCH NGƯỢC BẢN LINUX — KẾT QUẢ

Chỉ bản Linux mới cần dịch ngược (không có mã nguồn). Phía dự án đọc thẳng `D:\GAMEDEVNEW\Sources`.

### 1.1 Sửa được một lỗi của công cụ dịch ngược sẵn có

`D:\GAMEDEVNEW\ReverseTools\re_elf_luamap.py` **bỏ sót 216 tên hàm**. Nguyên nhân: trình liên kết GCC gộp hậu tố chuỗi (`SHF_MERGE|SHF_STRINGS`), nên nhiều tên hàm là con trỏ trỏ vào **giữa** một chuỗi dài hơn, còn công cụ chỉ nhận khi trỏ vào **đầu** chuỗi.

Bằng chứng byte — tại offset `0x212530` của `jx_linux_y`:

```
... n g e T e a m F e a t u r e \0 M s g 2 F a c t i o n \0 G M M s g 2 P l a y e r \0 M s g 2 T e a m \0 ...
                                                             ^^^^ ^-- "Msg2Player" bắt đầu ở đây
```

`Msg2Player` **không tồn tại riêng** — nó là `GMMsg2Player + 2`. Tương tự: `GetTask`/`SetTask` nằm trong `NW_GetTask`/`NW_SetTask` (`0x214AD0`), `Talk` trong `ForbitTalk`, `GetLevel` trong `PET_GetLevel`, `WriteLog` trong `TONG_WriteLog`, `GetName` trong `TONG_GetName`.

Sau khi xử lý tail-merge: `jx_linux_y` ra **1.560 tên** (tệp `jx_linux_y.luamap.txt` cũ chỉ có 1.344), `s3relay_y` ra **473 tên**.

### 1.2 So hai ELF — ai chạy cái gì

| | Số tên |
|---|---|
| Có ở **cả hai** (GS + relay) | 392 |
| **Chỉ** GameServer | 1.168 |
| **Chỉ** relay | 81 (≈12 trong đó là nhiễu chuỗi: `emei`, `shaolin`, `port`, `TCP`…) |

**Nhóm chỉ có ở relay** — đây chính là manh mối "cái gì chạy ở relay":

```
GlobalExecute · Msg2PlayerByName · OutputMsg · WriteStringToFile
GetGblInt/SetGblInt/DelGblInt · GetGblStr/SetGblStr/DelGblStr        <- kho biến toàn cụm (RLGLB_*)
Random · GetSysCurrentTime · GetCurrentDate
LoadLadder · SyncAllLadder                                           <- nạp/đẩy bảng xếp hạng
TaskName/TaskTime/TaskInterval/TaskCountLimit/TaskSetMode/...        <- TASK CENTRE (đồng hồ chủ)
IsGameServerReady · IsMapOnGameSvr · ConnectIdx2GameServerId · GetHostPlayerCount
nhóm SDB (SaveStringToSDB / GetIntegerFromSDB / ...)                 <- CSDL dùng chung
Battle_* · CW_GetCityStatus · StartCityWar · GetTongIDByName ...
```

**Ngược lại, chỉ có ở GameServer:** `GetGlbValue`/`SetGlbValue`, `StartGlbMSTimer`/`StopGlbMSTimer`, `Msg2SubWorld`, `AddLocalNews`, toàn bộ hệ Mission.

**Đáng chú ý:** cả **28 hàm `LG_*`/`LGM_*` đều tồn tại ĐỒNG THỜI ở hai bên**, không hàm nào lẻ. Địa chỉ ảo (trích ngắn):

| Hàm | GS | Relay |
|---|---|---|
| `LG_CreateLeagueObj` | `0x0815D740` | `0x0812AE6C` |
| `LG_ApplyAddLeague` | `0x0815D210` | `0x0812B12A` |
| `LG_GetLeagueTask` | `0x0815E330` | `0x0812C350` |
| `LG_ApplyDoScript` | `0x0815B700` | `0x0812C6B9` |
| `LGM_ApplyRemoveMember` | `0x0815BAD0` | `0x0812BBAF` |

Lý do: **GS giữ bản sao chỉ-đọc, relay giữ DB thật.** RTTI xác nhận: bên GS là `KLeagueManagerAgentT<...KLeagueSendDataToRelay>`, bên relay là `KLeagueManagerRelay` + `KLeagueAccepterEvent<KLeagueManager, KLeagueSendDataToGS>`. Chuỗi `GAME_LEAGUE` trong `s3relay_y` được dựng thành ShareKey 12 byte rồi đọc/ghi qua `KShareDatabase` — tức bảng MySQL `ShareData(ShareKey, Param1, Param2, Data)` với `Param1 = nLeagueType`, `Param2 = String2ID(tên league)`.

### 1.3 Bao nhiêu hàm engine mà hệ Liên Đấu thật sự dùng

Quét 45 tệp `.lua` của `leaguematch`, lọc bỏ 194 hàm Lua tự định nghĩa và 13 hàm thư viện Lua khác:

| | Số hàm |
|---|---|
| **Tổng hàm engine hệ Liên Đấu dùng** | **158** |
| — chỉ chạy được trên GS | 108 |
| — có ở cả hai | 37 |
| — chỉ chạy được trên relay | 12 |
| — `GetGateWayClientID` (đặc biệt) | 1 |

---

## 2. SƠ ĐỒ — CÁCH BẢN LINUX HOẠT ĐỘNG

### 2.1 Kiến trúc tổng thể (2 tầng)

```
┌─────────────────────────────────── RELAY (s3relay_y) ────────────────────────────────────┐
│                                                                                          │
│  relaysetting\task\tasklist.ini  ──[Task_59]──▶ task\leaguematch.lua                      │
│                                                    │ Include                             │
│                                                    ▼                                     │
│                                          \script\leaguematch\task.lua                    │
│                                                    │                                     │
│   ĐỒNG HỒ CHỦ ── TaskInterval(15 phút) ──▶  TaskContent()                                │
│                                                    │                                     │
│         ┌──────────────────────────────────────────┼───────────────────────────┐         │
│         ▼                     ▼                    ▼                           ▼         │
│   wlls_calc_phase()     wlls_set_mid()      wlls_match_stat()          wlls_set_phase()  │
│   (timetable.lua        (đổi mùa/thể loại   (CHỐT XẾP HẠNG            (phát pha xuống)   │
│    → mùa, pha, MatchID)  → dọn đội cũ)       trận vừa xong)                  │           │
│                                                                              │           │
│   KHO DỮ LIỆU THẬT:                                                          │           │
│     • League (đội)  → KShareDatabase → MySQL ShareData                        │          │
│     • RLGLB_* 121..145 → GetGblInt/SetGblInt (RAM relay, xoá mỗi lần boot)    │          │
│     • bền thật → league-task loại 500 "WLLS" qua gb_GetTask/gb_SetTask        │          │
│     • Ladder → LoadLadder + SyncAllLadder                                     │          │
└───────────────────────────────────────────────────────────────────────────────┼──────────┘
          ▲                                                                     │
          │  LG_ApplyDoScript(nLgType, szLgName, szMember,                      │  GlobalExecute
          │                   "\script\leaguematch\<tệp>.lua",                  │  ("dw wlls_setphase(...)")
          │                   "<hàm>", "<tham số>", "", "")                     │  → PHÁT CHO MỌI GS
          │  ← 10 call site, TẤT CẢ đều kết thúc bằng `, "", ""` (không callback)│
          │                                                                     ▼
┌─────────────────────────────────── GAMESERVER (jx_linux_y) ─────────────────────────────┐
│                                                                                         │
│  \script\missions\leaguematch\  (38 tệp / 4.872 dòng)                                   │
│                                                                                         │
│   wlls_gmscript.lua : wlls_setphase / wlls_player_join / wlls_gw_say  ◀── 3 ĐIỂM HẠ CÁNH │
│                                                                                         │
│   GLB_WLLS_* 820..826  (GetGlbValue/SetGlbValue — RAM của GS)                            │
│     820 PHASE · 821 SID · 822 MATCHID · 823 TYPE · 824 NEXT · 825 TIME · 826 CLOSE       │
│                                                                                         │
│   Mission 26 (toàn cục)  ─ timer 50 (10s, báo danh) / timer 51 (5s, thi đấu)             │
│   Mission 24 (sân chuẩn bị, MỖI map)                                                    │
│   Mission 25 (sân thi đấu,  MỖI map)                                                    │
│                                                                                         │
│   NPC: officer(lập đội/thưởng) · helper(tìm đồng đội) · signup(vào sân) ·                │
│        housecarl · chefu(xa phu) · yaoshang(cửa hàng) · chuwuxiang(rương)                │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 Vòng đời một trận — 9 giai đoạn

```
[0] BOOT GS ── autoexec.lua:163 wlls_autoexe()
      ├─ AddNpc officer ×7 thành, helper ×7 thành
      ├─ AddNpc signup/yaoshang/chefu vào hội trường, housecarl vào sân chuẩn bị
      ├─ 8 tệp macthtype/*.lua tự nạp → điền WLLS_TAB[1..7]
      └─ TabFile_Load("\settings\maps\championship\champion_gmpos.txt")   ← TỆP BẢNG DUY NHẤT

[1] LẬP CHIẾN ĐỘI  (bất kỳ lúc nào, ngoài thành)
      NGƯỜI CHƠI ──▶ NPC officer ──▶ AskClientForString(tên đội ≤16 ký tự)
                        │
                        └─▶ LG_ApplyDoScript(5, tênĐội, tênNV, "league.lua", "wlls_create", ...)
                                   │
                       RELAY ──────┴─▶ LG_CreateLeagueObj → LG_SetLeagueInfo → LG_ApplyAddLeague
                                       LGM_CreateMemberObj → LGM_SetMemberInfo → LGM_ApplyAddMember
                                       LGTASK_STYPE = thể loại · LGMTASK_JOB = 1 (đội trưởng)
      Mời thành viên: đội trưởng tập tổ đội → "wlls_add" → chặn nếu vượt max_member (1/2/3)

[2] RELAY MỞ KỲ BÁO DANH   (pha 4)
      TaskContent (mỗi 15') → GlobalExecute("dw wlls_setphase(sid,type,4,mid,next,tbOpen)")
      GS: CloseGlbMission(26) → OpenGlbMission(26) → StartGlbMSTimer(26, 50, 10*18)
          OpenMission(24) trên từng sân chuẩn bị
      GS ──▶ RELAY: "wlls_GsState <sốNgườiOnline> <ds sân> 1"
      RELAY: nFreeCount = floor((800 − sốNgười) / sốSân)   → bảng chỗ trống 200 đội/sân

[3] VÀO HỘI TRƯỜNG
      officer → lưu điểm xuất phát vào TASK 1715/1716/1717 → NewWorld(hội trường, 1523, 3024)

[4] VÀO SÂN CHUẨN BỊ  (chỉ khi pha = 4)
      NPC signup → hỏi 2 lần → wlls_en_check() quét 60+ vật phẩm cấm
                 → LG_ApplyDoScript("joinmatch.lua", "wlls_want2join", "<mtype> <tênNV>")
      RELAY kiểm: pha==4, đội tồn tại, số trận ≤48, còn chỗ
                 → GlobalExecute("dw wlls_player_join(...)")
      GS: RemoveSkillState mọi buff cấm → NewWorld(sân chuẩn bị, 1596, 2977)
          OnNewWorld → DisabledStall(1)/ForbitTrade(1)/ForbitStamina(1) → AddMSPlayer(24, camp)

[5] GHÉP CẶP  (sau 24 nhịp × 10s = 4 phút)
      StopGlbMSTimer(26,50) → pha = 5 → StartGlbMSTimer(26, 51, 5*18)
      wlls_buildup_vs():  hệ số thắng = (win*3 + tie)/total
                          → chia 10 khoảng 0.3 → xếp giảm dần → trộn ngẫu nhiên theo 5 đoạn
                          → đổi chỗ để TRÁNH GẶP LẠI 3 đối thủ gần nhất (EMY1/2/3)
                          → số đội LẺ ⇒ đội cuối được ĐI QUYỀN (thắng trắng)
      wlls_addplayer_combat(): NewWorld(sân thi đấu, toạ độ từ champion_gmpos.txt)
                          AddMSPlayer(25, camp) · SetDeathScript(playerdeath.lua)
                          ST_StartDamageCounter()          ← BỘ ĐẾM SÁT THƯƠNG
      CloseMission(24)

[6] THI ĐẤU  (120 nhịp × 5s = 10 phút)
      2 nhịp đầu = chuẩn bị (10s) → RunMission(25): SetPKFlag(2), SetFightState(1)
      Nhịp chẵn: báo số người còn lại của phe, tổng sát thương đã chịu, thời gian còn lại
      Chết  → DelMSPlayer(25,0) + về hội trường
      Rời   → chuyển sát thương của mình sang đồng đội (ST_IncreaseDamageCounter)
              nếu đội hết sạch người ⇒ đội địch thắng NGAY

[7] KẾT THÚC TRẬN
      Còn nhiều người hơn        → THẮNG
      Bằng người → BÊN CHỊU ÍT SÁT THƯƠNG HƠN thắng      ← đây là lý do cần ST_*
      Bằng cả sát thương          → HOÀ

[8] CỘNG ĐIỂM
      điểm = win*5*level + tie*2*level
      trần 48 trận/mùa; muốn thêm phải dùng Huyết Chiến Lệnh Kỳ (4 trận/lần, tối đa 18 lần)
      người OFFLINE được ghi nợ vào LGMTASK_WIN/TIE/TOTAL, trả bù lúc đăng nhập

[9] CUỐI MÙA  (pha 1)
      officer → thưởng xếp hạng (vật phẩm + điểm vinh dự) + danh hiệu top 4 (20 ngày)
      Cửa hàng: Sale(146) đổi bằng điểm vinh dự · Sale(173) đổi bằng uy danh
```

### 2.3 Mô hình dữ liệu bản Linux

**Chiến đội = đối tượng League loại `WLLS_LGTYPE = 5`, định danh bằng TÊN CHUỖI người chơi tự nhập** (≤16 ký tự). Không có ID số bền vững — mọi API tra theo cặp `(LGType=5, tên đội)`.

| Nhóm | Số chỉ số | Ví dụ |
|---|---|---|
| `WLLS_LGTASK_*` (gắn ĐỘI) | 17 | 4 = điểm · 5 = thứ hạng · 6 = thắng · 8 = tổng trận · 13/14/15 = 3 đối thủ gần nhất |
| `WLLS_LGMTASK_*` (gắn THÀNH VIÊN) | 7 | 1 = chức vụ · 2 = trạng thái · 3 = vào đội ở mùa nào |
| Task bền của nhân vật | `1715..1735`, `2500`, `2501`, `4125` | 2500 = điểm tích luỹ · 2501 = điểm vinh dự |

**7 thể loại thi đấu** (`leaguematch\head.lua:46-96`):

| # | Tên | max_member | ladder gốc | mtypes | groups |
|---|---|---|---|---|---|
| 1 | Song đấu | 2 | 10196 | 2 | 4 |
| 2 | Thi đấu 1 người (theo môn phái) | 1 | 10201 | 22 | 1 |
| 3 | Song đấu sư đồ | 2 | 10223 | 2 | 4 |
| 4 | Đấu ba người | 3 | 10225 | 2 | 4 |
| 5 | Đơn đấu tự do | 1 | 10235 | 2 | 4 |
| 6 | Song đấu (cùng hệ) | 2 | 10238 | 10 | 1 |
| 7 | Song đấu hỗn hợp | 2 | 10248 | 2 | 4 |

**Mùa giải** (`timetable.lua:32-137`): 84 mùa liên tục `[40]..[123]`, mỗi mùa = **ngày 08 → 28 của một tháng**, trần **48 trận/đội/mùa**. Lịch: 18:00 (4 trận) mỗi ngày, thêm 20:00 (4 trận) Thứ Sáu/Bảy/CN. Mỗi trận 15 phút.

**Hai hạng cấp:** Kiệt Xuất (cấp 80-119, map 396-415) và Võ Lâm (cấp ≥120, map 540-579).

---

## 3. SƠ ĐỒ — CÁCH BẢN DỰ ÁN HOẠT ĐỘNG

Nguồn: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\tinhnang\lien_dau\` — 10 tệp / 1.635 dòng + 24 tệp dữ liệu.

```
┌──────────────────────── MỘT GAMESERVER DUY NHẤT ────────────────────────┐
│                                                                         │
│ [0] BOOT                                                                │
│   startgame.lua:14  Include lib_liendau.lua                             │
│   startgame.lua:99  addnpcliendau()                                     │
│       ├─ NPC 308 "Sứ giả liên đấu"  map 78 Tương Dương (1546,3117)      │
│       ├─ NPC  87 "Sứ giả kiệt xuất" map 78 (1557,3119)                  │
│       ├─ 3× "Quan viên hội trưởng"  map 396                             │
│       └─ 2× Xa phu + rương + hiệu thuốc   map 396                       │
│   (thêm 1 cặp sứ giả ở map 53 Ba Lăng Huyện)                            │
│                                                                         │
│   settings\TimerTask.txt:12       → 11 → timer_liendau.lua              │
│   settings\task\missions.txt:12   → 11 → missions_liendau.lua           │
│   🔴 timerserver.lua:68  `-- sukien_liendau(...)`  ĐANG BỊ TẮT          │
│                                                                         │
│ [1] LẬP "CHIẾN ĐỘI"  (1 lần/tháng, ngày 8-28, cấp ≥80, MIỄN PHÍ)        │
│       chỉ là SetTask(T_ThanhLapChienDoi, 1 hoặc 2)                      │
│       1 = trung cấp (GetReBorn < 5) · 2 = cao cấp (GetReBorn ≥ 5)       │
│       ⇒ "chiến đội" luôn có ĐÚNG 1 THÀNH VIÊN — thực chất là 1v1        │
│       ⇒ hệ CHỦ ĐỘNG PHÁ tổ đội: LeaveTeam() + SetCreateTeam(0)          │
│       ghi 1 dòng {tên, cấp, 0, 0, môn phái, bang} rồi GHI RA ĐĨA:       │
│           data\<tháng>\danhsachtrung.lua  /  danhsachcao.lua            │
│           qua TaoBang() + SaveData() = openfile/write/closefile Lua 4   │
│                                                                         │
│ [2] VÀO HỘI TRƯỜNG map 396                                              │
│                                                                         │
│ [3] BÁO DANH 1 LƯỢT  (phút 0-4 / 15-19 / 30-34 / 45-49 của GIỜ 17)      │
│       chặn nếu T_LuuSoTran ≥ 48 · chặn nếu cùng hạng ≥ 94 người         │
│       SetCurCamp(4) · LeaveTeam() · NewWorld(396, khu báo danh)         │
│                                                                         │
│ [4] OnMissionTimer()  — chạy MỖI 5 GIÂY, máy trạng thái 6 pha           │
│       0 BaoDanh → 1 VaoNoiChanBi → 2 VaoKhuVucThiDau                    │
│                 → 3 ThoiGianChoBatDau → 4 BatDauChienDau → 5 DangDau    │
│                                                                         │
│ [5] GHÉP CẶP  (pha 2→3)                                                 │
│       aHeoNo = random(3,5)   ← CHỌN NGẪU NHIÊN 1 TRONG 3 CỘT LÀM KHOÁ   │
│              3 = điểm liên đấu · 4 = số trận thắng · 5 = số trận tham gia│
│       bubble-sort GIẢM DẦN theo cột đó → ghép 2 người KỀ NHAU           │
│              (1,2) → đài 1 · (3,4) → đài 2 · ...                        │
│       NewWorld(397 trung cấp / 399 cao cấp, TAB_POSTLIENDAU[k])         │
│       người lẻ cuối danh sách ⇒ THẮNG TRẮNG                             │
│                                                                         │
│ [6] THI ĐẤU ~9 phút 35 giây                                             │
│       giết đối thủ  → thắng ngay                                        │
│       hết giờ       → so tổng sát thương (QuyDinhThangThuaTheoDame)     │
│       bằng nhau     → hoà                                               │
│                                                                         │
│ [7] ĐIỂM:  thắng +30 · thua-theo-dame +15 · hoà +15 · CHẾT +0           │
│       kèm 10.000.000 × EXP_RATE(20) = 200 TRIỆU EXP mỗi trận thắng      │
│       ReLoader() GHI ĐÈ TOÀN BỘ tệp bảng sau MỖI kết quả               │
│                                                                         │
│ [8] CUỐI MÙA (ngày 1-7 tháng sau) — NPC "Sứ giả kiệt xuất"              │
│       thưởng = ĐIỂM VINH DỰ (T_LIENDAU = task 37) + DANH HIỆU 96..103   │
│       KHÔNG có vật phẩm, không có vàng                                  │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 4. SO SÁNH TRỰC DIỆN

| Trục | **Bản Linux (leaguematch/WLLS)** | **Bản dự án (lien_dau)** |
|---|---|---|
| Quy mô | 45 tệp / **5.902 dòng** | 10 tệp / **1.635 dòng** |
| Tầng | **2 tầng** — relay giữ DB, GS đánh trận | **1 tầng** — tất cả trong GameServer |
| Đơn vị thi đấu | **Chiến đội thật** 1/2/3 người, có đội trưởng, mời/rời/giải tán | **1v1**; "chiến đội" chỉ là 1 giá trị task, luôn 1 người; **chủ động phá tổ đội** |
| Thể loại | **7** (song đấu, môn phái, sư đồ, ba người, tự do, cùng hệ, hỗn hợp) | **1** (đơn đấu). Hằng `LienDauSongDau = 3` có khai báo nhưng **không dùng ở đâu** |
| Phân hạng | Theo **cấp độ**: Kiệt Xuất 80-119 / Võ Lâm ≥120 | Theo **số lần chuyển sinh**: `GetReBorn` <5 = trung cấp, ≥5 = cao cấp |
| Lưu trữ đội | Đối tượng `League` của engine → MySQL `ShareData` | **Ghi tệp `.lua` ra đĩa** bằng `openfile/write/closefile` |
| Trạng thái mùa | `GetGlbValue` (GS) + `GetGblInt` (relay) + league-task 500 | `GetGlbMissionV`/`SetGlbMission` (kho mission toàn cục) |
| Xếp hạng | `Ladder_*`, dải id 10196..10249, tới **hạng 512** | Sort thủ công trong Lua, ghi ra tệp; **chỉ chạy khi có người bấm menu NPC** |
| Ghép cặp | Theo **hệ số thắng** `(win*3+tie)/total`, chia 10 khoảng, trộn, **tránh gặp lại 3 đối thủ gần nhất** | Sort theo **một cột chọn ngẫu nhiên trong 3 cột**, ghép 2 người kề nhau, **không có lịch sử đối đầu** |
| Luật hoà | So **tổng sát thương** qua `ST_*DamageCounter` (hàm engine) | So `TaskTemp[50]` — engine chỉ cộng ô này trên map 209 và **397**; **map 399 không có** ⇒ hạng cao cấp không bao giờ thắng được bằng sát thương |
| Bản đồ | 396-415 + 527-579 + 991-993 (bộ ba hội trường / sân chuẩn bị / sân thi đấu ×20+) | Đúng **3 map**: 396 / 397 / 399 |
| Lịch | Mùa giải theo tháng, ngày 8-28, 18h+20h, 48 trận/mùa | **1 khung giờ/ngày** (`TIME_LIEN_DAU = {17,17}`), 4 trận, ngày 8-28 |
| Vật phẩm | 19 mã (Huyết Chiến Lệnh Kỳ, 5 Chân Kinh, 4 Lệnh bài vinh dự, 3 bảo rương…) | **Không phát vật phẩm nào** |
| Client | Dùng 2 cửa sổ có sẵn: `KUiTaskGuide` (Chỉ nam nhiệm vụ) + `KUiTreeRank` (`battlerank`) | **100% thoại NPC**, không đụng client |
| Điểm chung duy nhất | — | `WLLS_FORBID_ITEM` **trùng 78/78 mục**, chép nguyên cả chú thích gốc |

### 4.1 Ba thứ bản Linux có mà dự án hoàn toàn không có

1. **Bảng tìm đồng đội** (`npc\helper.lua`, 467 dòng) — dùng một League loại riêng để lưu hồ sơ tìm đội, lọc theo giới tính/môn phái/đẳng cấp/tỉ lệ thắng.
2. **Bộ đếm sát thương của engine** (`ST_StartDamageCounter` / `ST_Stop` / `ST_Get` / `ST_Increase`) — nền tảng của luật phân thắng khi hoà quân số, và có cơ chế **chuyển sát thương sang đồng đội khi một người rời trận**.
3. **Vòng đời mùa giải + thống kê chốt hạng** (`task.lua` `wlls_match_stat`) — chốt xếp hạng ngay khi MatchID đổi.

---

## 5. PHẢI VIẾT THÊM BAO NHIÊU HÀM

### 5.1 Con số gốc

```
Hàm engine hệ Liên Đấu bản Linux dùng ................. 158
Dự án ĐÃ CÓ .......................................... 121   (76,6 %)
CÒN THIẾU ............................................  37
```

Đối chiếu bằng bảng đăng ký `{ "Tên", LuaHàm }` trong `Sources\Core\Src\ScriptFuns.cpp` (dự án đăng ký **923 tên**, qua 3 bảng: `GameScriptFuns` 907, `WorldScriptFuns` 9, `SysFuns` 14).

**Đã có sẵn — không phải viết lại (đây là phần đắt nhất, đợt port công thành chiến JX2 đã làm):**

| Nhóm | Trạng thái |
|---|---|
| 28/28 hàm `LG_*` / `LGM_*` | ✅ `KJx2League.cpp` (1.035 dòng), **có persist thật** `settings\jx2league.txt` (fopen + MoveFileEx REPLACE, có bản dự phòng qua `KGameKV`) |
| 3/3 hàm `Ladder_*` | ✅ `KJx2SharedStore.cpp`, persist `settings\jx2ladder.txt` |
| `LG_ApplyDoScript`, `GlobalExecute` | ✅ (nhưng chạy **cục bộ**, xem rủi ro #1) |
| `OpenGlbMission` / `StartGlbMSTimer` / `StopGlbMSTimer` | ✅ shim tự viết trong `KJx2League.cpp:914-1034` |
| Hệ Mission đầy đủ: 24 hàm, camp (`m_ucPlayerGroup`), 9 timer/mission | ✅ `KMission.cpp/h`, `KSubWorld::m_MissionArray` |
| Bản đồ 396-579 | ✅ `MapList.ini` trùng id **100%** với bản Linux, và **có dữ liệu thật trong `Pak\maps.pak`** (đã giải mã chỉ mục bằng `g_FileName2Id`) |

> **Giả thiết "hệ Mission là khối lượng lớn nhất" bị BÁC BỎ.** Nhưng cần đính chính: `g_GlobalMissionArray` 50 khe **chưa bao giờ được dùng** — `Activate()` bị chú thích ở `KSubWorldSet.cpp:101`; nó chỉ còn là kho key-value cho `GetGlbMissionV`/`SetGlbMissionV`. Bộ ba `OpenGlbMission/StartGlbMSTimer/StopGlbMSTimer` **không phải cổng engine gốc** mà là shim dùng `std::vector` riêng, không liên quan lớp `KMission`.

### 5.2 Chia theo câu hỏi

| | Hạng mục | Tối thiểu | Vừa | Đầy đủ |
|---|---|---|---|---|
| **A** | Hàm Lua-API C++ viết mới | **12** | **26** | **38** |
| **B** | Hàm/hạ tầng C++ nội bộ mới | 2 | 4 | 9 |
| **C** | Hàm C++ đã có **phải sửa** | 3 | 4 | 6 |
| **D** | Lua phải port | ~14 tệp / ~1.850 dòng | ~37 tệp / ~4.550 dòng | 45 tệp / 5.902 dòng |
| **E** | Thay đổi giao thức client↔server | **0** | **0** | 0 gói tin, +2 cửa sổ UI |
| **F** | Thay đổi cấu trúc lưu trữ | 1 mục | 2 mục | 4 mục |
| | **Tổng công việc C++** | **17 mục** | **34 mục** | **53 mục** |
| | **Tổng dòng ước lượng** | ~2.150 | ~5.250 | ~9.400 |
| | **Công sức** | 5-8 ngày công | 12-18 ngày công | 30-45 ngày công |

### 5.3 Danh sách 37 hàm thiếu, phân nhóm

| Nhóm | Hàm | Ghi chú |
|---|---|---|
| **Bắt buộc, rẻ** (12) | `GetGblInt`, `SetGblInt` | ánh xạ thẳng vào `LuaGetGlbValue`/`LuaSetGlbValue` — dải RLGLB 121..145 không đụng dải 820..826 |
| | `Random` | 🔴 **BẪY ĐẾM:** dự án chỉ có `RANDOM` **viết hoa** (`ScriptFuns.cpp:13835`). Bảng băm tên hàm Lua **phân biệt hoa-thường** — mọi lần đếm phải dùng `-CaseSensitive`, nếu không sẽ báo nhầm "đã có" |
| | `CloseGlbMission` | có `Open`/`Start`/`Stop` nhưng khuyết đúng cái này |
| | `SyncTaskValueMore` | dự án chỉ có `SyncTaskValue` 1 id; bản gốc đồng bộ cả dải 1720..1732 + 2500..2501 |
| | `AskClientForString` | ⚡ **không cần đổi giao thức** — chỉ là lớp bọc ~25 dòng quanh `OpenGetString` + `GetStringFromUI` đã có sẵn (`ScriptFuns.cpp:13322-13325`), đường gói tin `S2C_INPUT_BOX` đã chạy |
| | `Number2Int`, `CalcItemCount`, `ITEM_GetImmediaItemIndex`, `CountFreeRoomByWH`, `DisabledStall`, `ForbitTrade` | bọc quanh hạ tầng đã có (`pos_immediacy`, `FindRoom`, `GetItemCount`) |
| **Chiều sâu** (14) | `CalcEquiproomItemCount`, `ForbitStamina`, `GetSkillState`, `GetLastAddFaction`, `GetBoxLockState`, `CheckGlobalTradeFlag`, `ST_CheckTextFilter`, `Time2Tm`, `LoadScript`, `GetGateWayClientID` | phần lớn có hạ tầng sẵn, chỉ thiếu lớp đăng ký Lua |
| | `GetRespect`, `AddRespect`, `SetRespect` | hạ tầng Respect **đã có** trong engine (16 hit) |
| | **4 hàm `ST_*DamageCounter`** | 🔴 **phải dựng hạ tầng C++ MỚI** — thêm `m_nDamageCounter` vào `KPlayer/KNpc` + hook đường tính sát thương. Đây là thứ duy nhất trong cả hệ phải làm từ số 0 |
| **Nên BỎ** (9) | `GetStringTask`, `SetStringTask` | 🔴 đòi **thêm mảng chuỗi vào định dạng lưu nhân vật** — xung đột trực tiếp với đợt di trú `roledb` sang MySQL đang làm. Chỉ `helper.lua` (bảng tìm đồng đội) cần ⇒ bỏ tính năng đó |
| | `LoadLadder`, `SyncAllLadder` | thừa hoàn toàn với 1 GS |
| | `TaskName`, `TaskTime`, `TaskInterval`, `TaskCountLimit` | đây là **task centre của relay**; thay bằng `StartGlbMSTimer(26, id, 15*60*18)` đã có |
| | `GetGlbMSRestTime` | **không tồn tại ở cả bản Linux** — chính script đã tự bọc `if GetGlbMSRestTime then` |

### 5.4 Hàm C++ đã có nhưng PHẢI SỬA

| Hàm | Vấn đề | Vị trí |
|---|---|---|
| 🔴 `LuaRunMission` | `sprintf("\\script\\misions\\mision%02d.lua")` — **thiếu chữ `s` ở cả hai từ**, bỏ qua `g_MissionTabFile`; và deref `pScript->m_LuaState` **không kiểm NULL** trong khi `g_GetScript` trả NULL được | `ScriptFuns.cpp:10394-10400` |
| `LuaStartMissionTimer` | luôn `Add()` slot mới, không chống trùng; chỉ 9 slot dùng được (`MAX_TIMER_PERMISSION=10`, index 0 bị bỏ) | `ScriptFuns.cpp:10505-10533` |
| `GlobalExecute` | chỉ `lua_dostring` trên **chính state đang gọi**; phải bóc tiền tố `dw ` / `dwf <đường dẫn> ` của bản gốc | `KTongJX2.cpp:4023-4034` |
| `OpenMission` | gọi hàm Lua tên `"BeginMission"`, còn script Linux định nghĩa `"InitMission"` | `ScriptFuns.cpp:10365` |
| Callback timer mission | engine gọi `"OnMissionTimer"`, script Linux đặt tên `"OnTimer"` | `KMission.cpp:336` |
| Nhịp `Breathe` của GlbTimer | không set `SCRIPT_SUBWORLDINDEX` ⇒ mọi hàm Mission gọi từ `OnTimer` sẽ **no-op im lặng** | `KJx2League.cpp:996-1033` |

---

## 6. KIẾN TRÚC: BỎ TẦNG RELAY CÓ AN TOÀN KHÔNG

**Bỏ được, và bỏ tầng bất đồng bộ là AN TOÀN.** Bằng chứng: **10/10 call site** `LG_ApplyDoScript` trong `missions\leaguematch` đều kết thúc bằng `, "", ""` — **không ai dùng callback**. `KJx2League.cpp:844-893` đã thực thi cục bộ đồng bộ.

**Phần relay THỪA hoàn toàn:** `LoadLadder`, `SyncAllLadder`, `RemoteExecute`/`DynamicExecute`, `IsGameServerReady`/`ConnectIdx2GameServerId`/`GetHostPlayerCount`, toàn bộ tầng `KLeagueManagerAgentT → relay → ILeagueDatabase`.

**Phần relay BẮT BUỘC phải gộp vào GS:**
- đồng hồ chủ 15 phút → thay bằng `StartGlbMSTimer(26, id, 15*60*18)`
- kho `RLGLB_*` 121..145 → ánh xạ thẳng vào `GetGlbValue`/`SetGlbValue` (không va chạm dải 820..826)
- kho bền `gb_GetTask`/`gb_SetTask` league 500 → chạy được ngay vì `LG_*` đã có persist

---

## 7. 🔴 BẢY RỦI RO / BẪY — ĐỌC TRƯỚC KHI GÕ

**1. LỊCH MÙA GIẢI ĐÃ CHẾT.** `WLLS_SEASON_TB` kết thúc ở mùa `[123] = 161228` (28/12/2016), sau đó là 2 mùa giả `999998`/`999999`. Với ngày hôm nay `wlls_calc_phase()` **luôn trả pha 1 = nghỉ vĩnh viễn**. Port nguyên xi ⇒ tính năng không bao giờ chạy, và sẽ mất nhiều ngày điều tra "sao không thấy gì". Bắt buộc viết lại bảng mùa giải (~150 dòng) hoặc đổi sang công thức tính mùa theo tháng.

**2. `LuaRunMission` sẽ SẬP GAMESERVER.** `combat\mission.lua:19` gọi `RunMission(25)`. Hàm này vừa sai chính tả đường dẫn vừa deref NULL. **Sập ngay trận đầu tiên.** Phải sửa trước.

**3. ĐỆ QUY TRONG CÙNG LUA STATE (rủi ro kiến trúc số 1).** Bỏ tầng bất đồng bộ khiến chuỗi GS→relay→GS thành đệ quy đồng bộ:

```
signup.lua:98  LG_ApplyDoScript(...)
   └─▶ KJx2League.cpp:844  thực thi CỤC BỘ, ĐỒNG BỘ
        └─▶ joinmatch.lua:227  GlobalExecute("dw wlls_player_join(...)")
             └─▶ wlls_gmscript.lua:147  NewWorld()   ← ĐỔI BẢN ĐỒ NGAY GIỮA LÚC HỘP THOẠI NPC ĐANG MỞ
```

Bản gốc có độ trễ mạng chen vào nên `NewWorld` rơi vào tick sau. **Bắt buộc chèn hàng đợi hoãn 1 tick**, nếu không sẽ treo hộp thoại hoặc lệch `PlayerIndex`.

**4. HAI HỆ SẼ GIÀNH NHAU BẢN ĐỒ.** Bản tự viết đang chiếm map **396/397/399**, Mission **11**, Timer **11**, và task bền **37**. Bản gốc dùng map 396-415/540-579 và task 2500/2501. Phải quyết **thay thế hẳn** hay **chạy song song trên bộ map khác** (540-579 còn trống) trước dòng code đầu tiên. Lưu ý task 37 = `TASKVALUE_STATTASK_HONOR` = đơn vị tiền của shop 93 — chạy song song sẽ làm lệch điểm vinh dự.

**5. VA CHẠM TÊN TOÀN CỤC.** Gộp 7 tệp relay vào cây GS thì **hai tệp cùng tên `head.lua`** sẽ cùng định nghĩa `FALSE` và `_M` — bản nạp sau thắng. Log khởi động GS bản gốc cho thấy GS **đã nạp cả 7 tệp relay** này và chúng là mã chết ở đó; khi gộp 1-GS chúng thành mã **sống**. Phải đổi tên thư mục và kiểm từng hàm trùng.

**6. XUNG ĐỘT ID.** Cả **19 mã vật phẩm** WLLS (1254-1257, 2126, 2212-2220…) và **danh hiệu 81-88/100-103/200-207** đều **đã bị vật phẩm/danh hiệu khác chiếm** trong bảng của dự án. Không được chép bảng — phải ánh xạ lại trong script. May là 12/19 món tương đương **đã tồn tại dưới ID khác** (Lệnh bài vinh dự 1259-1262, Chân Kinh 2222-2226…).

**7. THIẾU TỆP DỮ LIỆU CHẶN.** `settings\maps\championship\champion_gmpos.txt` (1.127 B, 101 dòng, 100 toạ độ vào đấu trường) **không tồn tại** trong cây chạy. Đây là **tệp bảng DUY NHẤT** cả hệ leaguematch nạp. Thiếu nó thì `TabFile_GetCell` trả rỗng ⇒ `NewWorld(map, nil, nil)`.

### Bẫy phụ đã ghi nhận

- **`MapList.ini` của dự án đã trỏ 46 map** vào `\script\missions\leaguematch\...` — thư mục **không tồn tại**. Đây là lỗi tồn đọng: 46 bản đồ đang chạy không có `NewWorldScript`. Khi port, **chỉ cần đặt đúng tệp vào là 46 mục tự sống lại**, không phải sửa `MapList.ini`.
- **`Ladder` của dự án chỉ giữ TOP 10** (`JX2LADDER_TOP = 10`) trong khi bản Linux xếp tới hạng 512.
- **`GetGlbValue`/`SetGlbValue` chỉ nằm trong RAM** (`KJx2SharedStore.cpp:463-492`, không có `fopen` nào) ⇒ khởi động lại giữa mùa là mất pha thi đấu. Bản gốc chấp nhận được vì relay đẩy lại pha mỗi 15 phút; với 1 GS thì phải có cờ cứu hộ lúc boot.
- **`nLid` không ổn định qua khởi động lại** — `sLeagueLoad` cấp lại lid bằng `++s_nNextLid` theo thứ tự dòng trong tệp. Luôn tra lại bằng `LG_GetLeagueObj(type, tên)`, đừng lưu lid.
- **`LGM_ApplyRemoveMember` bỏ qua đối số thứ 6 `removelg`** (`KJx2League.cpp:758-788`) — `league.lua:104,128,132` đặt `removelg=1` để engine tự xoá league khi thành viên cuối rời đội. Dự án không xoá ⇒ **league rỗng tồn tại vĩnh viễn** trong `jx2league.txt`.
- **Hai stub `LG_GetMemberObj` / `LG_GetMemberJoinTime`** — chú thích trong mã ghi "0 call site Lua" là **sai sự thật**: có 2 call site (`shitu\shitu.lua:287`, `missions\statinfo\timer_goodssale.lua:92`). Không ảnh hưởng leaguematch nhưng đang gây hỏng logic khoá 24 giờ sư đồ.
- **`sLadderSave` không kiểm `ferror`** trước `MoveFileEx` (`KJx2SharedStore.cpp:357-358`) — đĩa đầy sẽ để bản ladder cụt đè lên bản tốt. `sLeagueSave` thì có kiểm.
- **`faction.lua` khai TRÙNG bộ map Hoa Sơn** `{991,993,992}` cho cả mtype 11 lẫn 22 (`:74-79` và `:140-145`) ⇒ `map_index` bị ghi đè.
- **`SetMissionV` `strcpy` vào `char[100][16]` không kiểm độ dài** (`KMissionArray.h:36-41`) — truyền chuỗi >15 ký tự từ Lua sẽ tràn sang ô kế tiếp.
- **`GetMissionV` và `SetMissionV` lệch điều kiện chỉ số**: `SetMission` chặn `< 0` (id 0 ghi được) còn `GetMissionValue` đòi `> 0` (id 0 đọc luôn ra 0) ⇒ ô chỉ số 0 là hố đen.
- **`script\header\liendau.lua` là MÃ CHẾT** — trông rất giống điểm khởi đầu có sẵn nhưng **không tệp nào Include nó**, và nó gọi `GetDataInt`/`GetDataGr` vốn **không tồn tại ở bất kỳ `.lua`, `.dll` hay `.exe` nào**. Đừng tưởng đây là chỗ bắt đầu.

---

## 8. LỖI TỒN ĐỌNG PHÁT HIỆN KÈM (trong bản Liên Đấu đang chạy — ngoài phạm vi hỏi)

| # | Lỗi | Vị trí |
|---|---|---|
| 1 | **Hạng cao cấp không bao giờ thắng được bằng sát thương.** `TaskTemp[50]` chỉ được engine cộng trên map **209** và **397**; map **399** không nằm trong điều kiện ⇒ mọi trận cao cấp hết giờ đều `myDame == enemyDame == 0` ⇒ luôn rơi vào nhánh hoà (+15 điểm) | `KNpc.cpp:3586-3592` (call site **duy nhất** của `SetClearVal(50,...)`) |
| 2 | **Luật cấm vật phẩm hiện KHÔNG chạy.** `CheckForbidItem()` gọi 3 hàm không tồn tại (`ITEM_GetImmediaItemIndex`, `CalcEquiproomItemCount`, `CalcItemCount`) — cả 3 dòng đều đang bị `--` | `lib_liendau.lua:326, 337, 342` |
| 3 | **Bảng xếp hạng có thể không bao giờ được sắp.** `CapNhatHang()` chỉ được gọi từ `CheckFull()`, tức chỉ khi người chơi bấm menu "kiểm tra tình hình". Cả tháng không ai bấm ⇒ thưởng phát theo **thứ tự đăng ký**. Dữ liệu thực tế xác nhận: `data\11\danhsachtrung.lua` dòng [1] có 0 trận thắng còn [2] có 4 | `lib_liendau.lua:124` |
| 4 | **`return` sớm trong vòng lặp đếm ngược** ⇒ chỉ người chơi ĐẦU TIÊN nhận được thông báo, và `PlayerIndex` không được phục hồi | `timer_liendau.lua:64-78` |
| 5 | **Người chết bị kẹt lại map đấu trường** — khối xử lý người chết không có `NewWorld()`, chỉ khối người thắng mới có | `playerdie.lua:21-34` vs `:42-58` |
| 6 | **`MAX_PLAYER` định nghĩa hai lần khác giá trị** — `lib_ham.lua:6` = 400, `lib_map.lua:58` = 280; nạp sau thắng ⇒ mọi vòng lặp chỉ quét 280 khe. Riêng `QuyDinhThangThuaTheoDame()` lại dùng `GetCountPlayerMax()` ⇒ **không nhất quán** | |
| 7 | **`Include` tệp không tồn tại** — `data\<tháng>\danhsachthamgia.lua` không có ở bất kỳ thư mục tháng nào | `quanvienhoitruong.lua:8` |
| 8 | **`sukien_liendau()` đang bị tắt** ⇒ mất thông báo toàn server, và đoạn **xoá dữ liệu bảng xếp hạng tháng trước không chạy** — sau 1 năm dữ liệu cũ sẽ "sống lại" chồng lên mùa mới của cùng tháng đó | `timerserver.lua:68` |
| 9 | **Chiến lược tối ưu là KHÔNG ĐÁNH.** Thua vì ít sát thương vẫn được 15 điểm + 10 triệu exp; chỉ CHẾT mới 0 điểm. Đứng yên tới hết giờ ⇒ 48 × 15 = 720 điểm/mùa mà không cần chiến đấu | `timer_liendau.lua:292, 434` |
| 10 | **Hiệu năng I/O:** `ReLoader()` ghi đè **toàn bộ** tệp bảng sau **mỗi** kết quả trận của **mỗi** người chơi — tối đa 752 lần mở/ghi/đóng tệp mỗi giờ, đồng bộ trong luồng game chính | `lib_liendau.lua:206-233` |

---

## 9. KHUYẾN NGHỊ

Chọn **phương án Vừa**, làm 2 đợt:

- **Đợt A (≈ phương án tối thiểu, 5-8 ngày):** sửa 3 hàm C++ nguy hiểm **trước** (`LuaRunMission` là sập chắc chắn) → thêm 12 hàm → chép `champion_gmpos.txt` → port khung + 1 thể loại → **viết lại lịch mùa giải**. Nghiệm thu: 1 trận Song đấu chạy trọn vẹn, xếp hạng lên, thưởng cuối mùa trả đúng.
- **Đợt B (+7-10 ngày):** mở 6 thể loại còn lại, 2 hạng cấp, `ST_*` đếm sát thương, 7 vật phẩm.

**Bỏ hẳn ở cả hai đợt:** `npc\helper.lua` (kéo theo `StringTask` → đổi định dạng lưu nhân vật, xung đột đợt MySQL `roledb`), `LoadLadder`/`SyncAllLadder`, 4 hàm `Task*`, cửa sổ xếp hạng phía client (dùng menu NPC với `Ladder_GetLadderInfo` là đủ).

---

## 10. ĐỘ TIN CẬY & NHỮNG GÌ CHƯA KIỂM ĐƯỢC

**Đã kiểm tới byte:** mọi địa chỉ VA trong ELF; md5 7/7 tệp relay vs server1; chỉ mục `Pak\maps.pak` (giải mã bằng `g_FileName2Id` + chuẩn hoá `g_GetPackPath`); bảng đăng ký Lua trong `ScriptFuns.cpp`; toàn bộ `KJx2League.cpp` 1.035 dòng và `KJx2SharedStore.cpp` 494 dòng; số dòng từng tệp Lua hai bên; trùng khớp 78/78 `WLLS_FORBID_ITEM`.

**Chưa kiểm được:**
- **Chưa chạy thử trong game** — mọi kết luận về hành vi đều từ đọc mã.
- Pak bản đồ phía **client thật** (`maps_client.pak` trong `bin\server` thiếu 2 thư mục liên đấu, nhưng đó chỉ là bản sao — chưa chắc là pak client đang phát hành).
- Ý nghĩa và tính bắt buộc của các tệp `Maps\*_srv.fp` (thiếu toàn bộ cho map liên đấu).
- Engine dự án có nhận diện 2 cờ `FORBIT_ITEM_TYPE` là `PKEX` và `LEAGUE` hay không (`settings\map_type.txt` của dự án có dòng `LEAGUEMATCH` nhưng **thiếu 2 cờ này** so với bản gốc).
- Dự án có hệ Sư đồ (League loại 1 + TASK 1403) để chạy thể loại 3 hay không.
- `KLinkArrayTemplate.h` chưa đọc ⇒ chưa chắc 10 khe mission là 10 dùng được hay 9 (với timer thì đã xác định là **9** vì constructor `KMission` lặp từ `i = 1`).
- Chưa đối chiếu **chữ ký từng hàm** `LG_*` của dự án với bản Linux về số đối số/thứ tự trả về (mới xác nhận `LG_GetLeagueInfo` trả 3 giá trị, đúng như `head.lua:110` mong đợi).

---

## PHỤ LỤC — ĐỊA CHỈ & ĐƯỜNG DẪN HAY DÙNG

| Thứ | Nơi |
|---|---|
| Script Liên Đấu bản Linux — GameServer | `D:\ServerLinux\server1\script\missions\leaguematch\` (38 tệp / 4.872 dòng) |
| Script Liên Đấu bản Linux — Relay | `D:\ServerLinux\server1\script\leaguematch\` = `D:\ServerLinux\gateway\s3relay\script\leaguematch\` (7 tệp / 1.030 dòng, md5 khớp 7/7) |
| Điểm nạp thật phía relay | `D:\ServerLinux\gateway\s3relay\relaysetting\task\leaguematch.lua:1` |
| Bật/tắt tác vụ relay | `relaysetting\task\tasklist.ini` — `ExcutedCount=-1` nghĩa là **TẮT** |
| Bản Liên Đấu của dự án | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\tinhnang\lien_dau\` (10 tệp / 1.635 dòng + 24 tệp data) |
| Hạ tầng League C++ | `D:\GAMEDEVNEW\Sources\Core\Src\KJx2League.cpp` (1.035 dòng), `KJx2SharedStore.cpp` (494) |
| Bảng đăng ký hàm Lua | `D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp` — 923 tên qua 3 bảng |
| Đặc tả hệ League đã có sẵn | `D:\GAMEDEVNEW\DIEUTRA_LEAGUE_SPEC.md` (14/08) |
| Công cụ dịch ngược ELF | `D:\GAMEDEVNEW\ReverseTools\re_elf_luamap.py` — ⚠️ **bỏ sót 216 tên**, xem mục 1.1 |
| Persist League / Ladder | `<bin\server>\settings\jx2league.txt` · `jx2ladder.txt` |
