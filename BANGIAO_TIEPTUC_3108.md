# BÀN GIAO — MẠCH "GỠ HỆ CŨ / DỌN SCRIPT / VIÊM ĐẾ"

Chốt 30/08, ghi lại 31/08.

> ⚠️ **CÓ NHIỀU PHIÊN CÙNG LÀM DỰ ÁN NÀY.** Tệp này chỉ nói về **mạch việc của
> tôi**. Hai mạch khác đang treo, có bàn giao riêng — đọc trước khi động vào
> vùng của họ kẻo giẫm chân:
> - `BANGIAO_PHIEN_BOT_TK_3008.md` — bot / Tống Kim / đồng bộ toạ độ
> - `BANGIAO_PHIPHONG_TIEPTUC_3008.md` — Phi Phong
>
> Đặc biệt: bản `CoreServer.dll` đang chạy **có cả phần việc của phiên khác**
> (xem mục 3). Trước khi dựng lại DLL phải liệt kê tệp nguồn nào mới hơn DLL
> đang chạy, không thì cuốn theo việc chưa soi của người khác.

Bốn tệp chi tiết bổ trợ cho mạch này: [GO_HECU](BANGIAO_GO_HECU_3008.md) ·
[5VIEC](BANGIAO_5VIEC_3008.md) · [DONSCRIPT](BANGIAO_DONSCRIPT_3008.md) ·
[HOANTAT](BANGIAO_HOANTAT_3008.md).

---

# 1. 🔴 VIỆC ĐANG DANG DỞ — LÀM TIẾP TỪ ĐÂY

## 1.1 Viêm Đế Bảo Tàng: 3 triệu chứng chủ game báo, ĐÃ VÁ 2, CHỜ NGHIỆM THU

Chủ game báo (30/08 chiều):

| # | Triệu chứng | Trạng thái |
|---|---|---|
| A | Đánh hết quái **chỉ còn 1 con**, đánh không mất máu nhưng **nó vẫn đánh được mình** (quái thường, không phải boss) | vá gián tiếp, **chưa nghiệm thu** |
| B | Quái **không tự bu vào**, ít di chuyển | vá gián tiếp, **chưa nghiệm thu** |
| C | Đánh boss xong hiện bảng xúc xắc, **bấm cần không nhận đồ** | vá 2 lỗi, **chưa nghiệm thu** |

### Gốc đã tìm ra

**Lỗi 1 — `tbReady` bị bảng rỗng đè mất 18 phương thức.** ĐÃ VÁ.

Hai tệp cùng khai `tbReady = {}`:
- `missions\yandibaozang\readymap\ready.lua:39` → rồi gắn **18 phương thức**
- `missions\yandibaozang\readymap\include.lua:42` → gắn **0** phương thức

Chuỗi nạp của bộ test: `test_hoatdong_admin.lua:17` nạp `ready.lua` (đủ phương
thức) → **dòng 21** nạp `npc.lua:10` → `readymap\include.lua:42` → `tbReady = {}`
**xoá sạch**. Bảng rỗng vẫn khác nil nên mọi chốt `if tbReady == nil` đều lọt.

Vá: guard `if (tbReady == nil) then tbReady = {} end` (khuôn đã dùng cho `PetSys`).

**Vì sao lỗi này giải thích cả A, B, C:** hàm chết là
`tbReady:InitMatchMission` — chính nó gọi
`lib:DoFunInWorld(nMapIndex, OpenMission, YDBZ_MISSION_MATCH)` để **mở mission**
trên bản đồ, và đặt `YDBZ_TEAM_COUNT` / `YDBZ_SIGNUP_*`.

Mission không mở ⇒ `GetMissionV`/`SetMissionV` vô tác dụng ⇒
- bộ đếm quái luôn đọc về 0 → `0 + 1 == 60` không bao giờ đúng → **đợt quái không
  bao giờ chốt**, boss ải không sinh (đây là "chỉ còn 1 con")
- `YDBZ_add_npc:289` đọc `GetMissionV(YDBZ_NPC_TYPE[...])` → 0 →
  `YDBZ_map_npc[0]` = **nil** → sinh quái đợt sau hỏng hẳn
- đường chia đồ đọc rác → chính là spam `AddItemSet2 nItemGenre=57216`

**Lỗi 2 — `saizi.lua:50` truyền CHUỖI vào `CallPlayerFunction`.** ĐÃ VÁ.

`CallPlayerFunction` đòi tham số 2 là **một hàm** (`ScriptFuns.cpp:2232-2258`:
`lua_pushvalue(L,2)` rồi `rawcall`). Mọi chỗ dùng đúng đều truyền hàm
(`weeklyrank.lua:9` `CallPlayerFunction(idx, GetTask, 751)`).
Vá: `CallPlayerFunction(list[i], Msg2Player, msg)` + bỏ qua phần tử ≤ 0.

### ⚠️ MỐC THỜI GIAN QUYẾT ĐỊNH — đọc kỹ

```
30/08 16:08   readymap\include.lua   (vá tbReady)
30/08 16:42   CoreServer.dll         <- lần khởi động máy chủ gần nhất
30/08 16:44   saizi.lua              (vá CallPlayerFunction)  ← SAU khi khởi động
```

⇒ **Bản vá `tbReady` ĐÃ sống. Bản vá `saizi.lua` CHƯA sống.**
Phải khởi động lại (hoặc nạp lại script) rồi mới test được C.

### Việc phiên sau

1. Nhờ chủ **khởi động lại GameServer**.
2. Test theo đúng thứ tự này, hỏi chủ từng bước:
   - vào trận một mình → **quái có bu vào không** (B)
   - đánh → **có mất máu không** (A)
   - giết hết một đợt → **có sinh boss ải + có dòng "đã thành công vượt ải thứ N"
     không** ← đây là dấu hiệu rõ nhất cho biết mission đã mở
   - giết boss → bảng xúc xắc → bấm cần → **có nhận đồ không** (C)
3. Nếu A vẫn còn sau khi B đã hết: **KHÔNG phải lỗi mission**. Khi đó chuyển
   hướng sang **bản sao NPC mồ côi phía client** — đánh vào cái bóng, con thật ở
   chỗ khác đánh lại. Dự án đã có cả chuỗi việc về đúng loại lỗi này
   (S6-GANNHANH 28/08, xem `jx1-goc-truot-va-miss-2608`).
   **Đừng vá mò vào đường đồng bộ NPC khi chưa phân định.**
4. Nếu còn spam `AddItemSet2 nItemGenre=57216`: đã truy được nó phát ra từ
   `KItemDice.cpp:578` (`AddDiceItemInfo`). Đường Viêm Đế **chứng minh được**
   truyền genre = 6 đúng (`head.lua:145/151` → `y1[3][1]` = 6), nên nếu vẫn còn
   thì **nguồn nằm chỗ khác** — `AddItemSet2` có 6 nơi gọi. Phải hỏi chủ nó xuất
   hiện đúng lúc làm thao tác gì.

## 1.2 Việc chờ chủ quyết

| Việc | Ghi chú |
|---|---|
| ~1.000 tệp bẫy/obj bản đồ TQ | **ĐÃ QUYẾT KHÔNG LÀM** — xem mục 4.2, đừng đề xuất lại |
| Sắp xếp lại thư mục | cố ý không làm — đường dẫn ghi cứng ở C++ và trong pak đã nén |
| 237 bản sao lưu rải trong cây | để nguyên — lịch sử của chủ |
| 43 mã vật phẩm trong bảng thưởng không khớp món nào | tồn từ đợt trước |
| `SERVER_TEST=1` + 2 NPC "Hỗ Trợ Test" phát 1 tỷ lượng / 100k Xu / cấp 200 | tồn từ đợt trước, **chưa báo lại** |

---

# 2. ĐÃ XONG NGÀY 30/08

## 2.1 Bỏ tính năng cũ trùng bản port Linux/JX2

7 hệ: Vượt Ải · Phong Lăng Độ · Boss Sát Thủ · Lôi Đài bang hội · Công Thành
(phần lịch) · Dã Tẩu · Liên Đấu. **Tổng cả ngày: 188 tệp / ~10.900 dòng** dời
sang `_dara`, **129 mục nhật ký hoàn tác**.

Căn cứ xác định bản nào là cũ — ba nguồn khớp nhau: ghi chú trong chính cây
script; tài liệu `.md` + git; và **trạng thái công tắc thật** trong
`script\cauhinh\ch_lich.lua` (bản port `BAT_HD3`/`BAT_VIEMDE`/`BAT_CTC_JX2` = 1,
bản cũ `BAT_*_VIET` = 0).

## 2.2 Năm việc chủ giao

1. **Lôi Đài Hỗn Chiến** — bỏ hẳn (216 dòng + 3 tệp), kèm `CuuNguoiKetLDHC()`
   trong `playerlogin.lua` đưa người kẹt ở bản đồ 210 về Ba Lăng Huyện.
2. **Hai rương hậu phương 222/223** — để nguyên theo lệnh.
3. **Long Huyết Hoàn** (mã **2126**, dòng 2128 trong bảng) — port từ Linux.
   Bản Linux **đã có sẵn** cơ chế này, mã task trùng y hệt cây ta
   (1550/1551/2641/2642/4018); bên nhận sẵn sàng từ trước, chỉ thiếu bên phát.
4. **Rương Dã Tẩu 1341** — truy ra là **dòng thừa**: script chưa từng tồn tại
   trên bất kỳ cây nào, không ai phát. **Để nguyên dòng 1343** (bảng tra theo
   chỉ số dòng).
5. **Dọn triệt để Dã Tẩu cũ** — xong cả hai chặng, xem 2.4.

## 2.3 Sáu lỗi ĐANG XẢY RA đã vá

1. **Ba bẫy Công Thành cũ** trên bản đồ 221 búng người chơi lùi giữa trận Công
   Thành JX2 (`lib_ctc.lua:278-280` → `chancong_1/2/3.lua` đọc mission 2 đã chết).
2. **Nhặt cuộn Dã Tẩu cướp menu người chơi** — `KPlayer.cpp:4993/4995` để
   `bGlobal` mặc định, ghi đè `m_ActionScriptID` (`:7129`) là biến điều phối nút
   bấm (`:7621`/`:7626`). Log có 25 lần.
3. **Long Huyết Hoàn vô dụng** (chạy trên bộ đếm hệ đã gỡ).
4. **`tbReady` bị đè** — xem 1.1.
5. **`saizi.lua` truyền chuỗi vào `CallPlayerFunction`** — xem 1.1.
6. **Bốn công tắc không điều khiển gì**: `GLB_MANH_BOSS_SATTHU`,
   `BDH_THOIHAN_DANHHIEU_NGAY` (bị `partner_reward2.lua:15` ghi đè), nhóm
   `BAT_*_VIET`, và bảng "bù hoạt động" 5/6 bộ đếm chết.

Thêm: **mìn `storm_clear()`** đã chặn — trước đây gọi thiếu tham số là xoá trắng
task 1661→trần của nhân vật; nay phải gọi rõ `storm_clear(-1)`.

## 2.4 Dã Tẩu cũ — ĐÃ XONG HẲN

- `script\global\onkillnpc.lua` = nhà mới cho móc "người chơi giết NPC"
- `KNpc.cpp:1698` trỏ thẳng sang đó (`OnPlayerKillNpc`)
- **DLL đã swap** (kiểm trong nhị phân: có `onkillnpc.lua` + `OnPlayerKillNpc`,
  **không còn** `\script\tinhnang\datau\danhquai.lua`)
- Bộ chuyển tiếp `danhquai.lua` đã dời nốt 31/08 → **thư mục `tinhnang\datau`
  không còn tồn tại**

---

# 3. TRẠNG THÁI NHỊ PHÂN

```
CoreServer.dll       30/08 16:42   18.207.232 byte   <- ĐANG CHẠY, đã có 2 bản vá của tôi
CoreServer.dll.moi   30/08 16:36   18.207.232        md5 b2526657979a3f6fc6b66f83537f8b32
CoreServer.dll.truoc 30/08 15:52   18.205.696
```

⚠️ **Bản `.moi` hiện tại KHÔNG phải bản tôi dựng** (của tôi: 18.205.184 byte,
md5 `5f879a7e3dfe31b7def75f9605196beb`, dựng 13:10). Bản 16:36/16:42 có
`KItem.cpp` sửa lúc 14:06 — **việc của phiên khác**, tôi chưa soi. Hai bản vá C++
của tôi vẫn còn nguyên trong đó.

**Bài học quy trình:** bản dựng cuốn theo **mọi thứ đang có trong cây nguồn**,
không chỉ bản vá của mình. Trước khi giao DLL phải liệt kê tệp nguồn nào mới hơn
DLL đang chạy.

---

# 4. BẪY ĐÃ VẤP TRONG NGÀY — ĐỌC KỸ KẺO LẶP

## 4.1 Bảy bẫy công cụ

1. **Đường dẫn trong Lua viết `\\`** (hai dấu) — so bằng một dấu là **trượt hết**.
2. **Khớp tiền tố**: `tinhnang\loidai` khớp nhầm `tinhnang\loidaihonchien`.
3. **Tên chung** (`main`, `no`, `OnTimer`, `NOW_END_SAY`) khai ở hàng trăm tệp —
   không lọc thì mọi tệp đều bị chấm là phụ thuộc.
4. **Hàm engine C++**: `IsArenaBegin`, `GetArenaBothSides`, `GetJoinTongTime`,
   `EnterBattle`… nằm trong `ScriptFuns.cpp` dạng `{ "Tên", LuaTên }`. Kiểm
   trước khi nói "hàm không tồn tại".
5. **Tệp TRỘN kiểu xuống dòng**: `station.lua` (CRLF 615 / LF 637),
   `timerserver.lua` (822/827), `startgame.lua` (219/224), `task01.lua` (76/77).
   Tách bằng `\r\n` là **gộp nhiều dòng logic vào một phần tử** → cắt lem và số
   dòng báo ra sai. Phải tách bằng `\n`, giữ `\r` cuối phần tử.
6. **Biểu thức `[\w\-]+\.lua` CẮT CỤT tên tiếng Trung** — `·` (byte GBK) không
   phải word-char. Dùng `[^\s"'\\/<>()\[\],;=]+\.lua`.
7. **`pakdump.read_entry` chỉ biết zlib** — pak nén **UCL NRV2B**; zlib thất bại
   thì nó **trả về nguyên khối đã nén** không báo lỗi → trích ra tên rác trông
   *gần giống* tên thật. Phải gọi `ucl.nrv2b_decompress_8`.

## 4.2 🔴 QUYẾT ĐỊNH: KHÔNG quét dọn hàng loạt

Trong **đúng một buổi**, công cụ cho **hai câu trả lời sai** về cùng câu hỏi
"tệp nào chết", **cả hai lần đều sai theo hướng tuyên bố tệp đang sống là chết**.

Phép thử dứt điểm: lấy 10 tệp ngẫu nhiên trong nhóm "mồ côi", tìm **nguyên byte
tên tệp** trên toàn bộ máy chủ kể cả **5 pak đã giải nén** (154.607 mục) →
**4/10 THẬT SỰ có trong `maps.pak`**. Bộ lọc sai **~40%**.

⇒ Muốn dọn tiếp: **chỉ từng cụm có tên tuổi**, đọc thật xem là tính năng gì, và
**chạy `t69_kiem_mau_thobao.py` trước**. Đừng quét ào.

## 4.3 Tám đường một tệp `.lua` có thể được nạp

`Include`/`dofile`/`ReLoadScript`/`DynamicExecute` · `AddNpc*` · `SetDeathScript`
/`AddTrapEx*`/`OpenGiveBox` · bảng `settings\` · **quy ước tên theo số hiệu**
(`missionNN.lua`, `taskNN.lua`) · C++ gọi cứng · bảng chuỗi trong `.h` ·
**dữ liệu bản đồ trong `Pak\*.pak` (đã nén)**.

Hai đường cuối grep **mù hoàn toàn**.

## 4.4 🔴 Engine NẠP SẴN TỪNG tệp lúc khởi động

Console in `====== [Total ScriptLoaded : 2948] ======`. **Thân chunk của mỗi tệp
chạy độc lập một lần**, không có ai `Include` nó trước.

⇒ Mọi lời gọi ở thân chunk phải **tự đủ**: hàm phải khai trong chính tệp, hoặc
trong tệp đã `Include` **ở trên**, hoặc là hàm engine. Vi phạm = `ScriptError 4`,
**tệp đó không nạp được** ⇒ mọi hàm trong nó thành nil.

Đã vấp thật: `partner_reward2.lua` gọi `BDH_CFG` (khai trong
`partner_reward.lua`). **Chạy `t71_quet_goi_nil_thanchunk.py` sau mỗi đợt sửa.**

## 4.5 Bảng bị khai lại đè mất phương thức

`tbReady` (mục 1.1) và trước đó `PetSys`. Đã quét toàn cây: **chỉ còn `tbReady`**
mắc bệnh này, và đã vá. Công cụ quét nằm trong lịch sử phiên — tìm bảng khai ở
nhiều tệp, trong đó có tệp gắn phương thức và tệp không.

---

# 5. QUY TRÌNH BẮT BUỘC

- **Không xoá — DỜI** sang `bin\server\_dara\script\<đường dẫn cũ>`. Nhật ký
  `_dara\NHATKY_DOI.txt`. Hoàn tác: `doi_tep.hoan_tac("<đường>", ghi=True)`.
- Sửa tại chỗ: **comment kèm dấu mốc**, để bản gốc cạnh đuôi `.truoc_*`.
- **Mọi tệp là ANSI/TCVN3** — đọc/ghi bằng `latin-1`. **Không tự gõ byte tiếng
  Việt**: chép nguyên văn từ tệp có sẵn, hoặc dùng `vn_edit.vn()` của bộ kỹ năng.
  TCVN3 **không mã hoá được nguyên âm HOA có dấu** (trừ Ă Â Đ Ê Ô Ơ Ư).
- Mỗi bản vá phải có: neo khớp **đúng một lần** · so **cân bằng từ khoá Lua** ·
  so **số byte tiếng Việt** · **đọc lại** sau khi ghi.
- Nghiệm thu bằng `syncheck.exe` — trình phân tích **Lua 4.0.1 thật**, dựng từ
  chính `Sources\Library\LuaLib`. Kịch bản dựng:
  `ReverseTools\lua_syncheck\build_syncheck.bat` (sửa đường dẫn scratchpad).
- Build: gọi thẳng `Core\Core.vcxproj`, **không** qua `JXAll.sln`; cần
  `-p:SolutionDir="D:\GAMEDEVNEW\Sources\"`. Hai cấu hình:
  `"Server Release"|x64` và `"Client Release"|Win32`.
- **Cấm tự tắt/khởi động lại GameServer.** Swap = đặt `<file>.moi` cạnh bản đang
  chạy, chủ chạy `ChayGameServer.bat`. Báo swap **phải kèm checklist đánh số**.
- Bash nuốt `\` và backtick — **chuỗi có backslash phải qua tệp**, không dùng
  heredoc/inline.

---

# 6. CÔNG CỤ (`D:\GAMEDEVNEW\ReverseTools\cauhinh\`)

| Tệp | Việc |
|---|---|
| `doi_tep.py` | dời-không-xoá, có chốt "còn ai gọi", `hoan_tac()` |
| `lua_ham.py` | dò ranh giới hàm/khối Lua theo cân bằng từ khoá |
| `t44_nghiem_thu.py` | **nghiệm thu toàn cây** — tham chiếu treo, hàm nil, cú pháp |
| `t58_duong_nap_thu8_mapdata.py` | giải nén pak bằng UCL, trích tên script trong dữ liệu bản đồ |
| `t69_kiem_mau_thobao.py` | **phép thử thô bạo** — tìm nguyên byte tên tệp trên toàn máy chủ + pak đã giải nén |
| `t71_quet_goi_nil_thanchunk.py` | tìm lời gọi hàm nil ở thân chunk |
| `t72_truy_chuoi_include.py` | truy chuỗi Include tìm mắt xích gãy |
| `t73_quet_chuoi_thanchunk.py` | như t71 nhưng trên cả chuỗi Include |
| `t35`–`t43` | sáu đợt gỡ hệ cũ |
| `t45`–`t56` | năm việc chủ giao + dọn |
| `t57`, `t59`, `t60`, `t62`, `t63`, `t68` | quét/dọn tệp mồ côi |
| `t64`–`t67`, `t70`, `t74`, `t75` | vá theo phản biện + vá lỗi chủ báo |

---

# 7. NGHIỆM THU HIỆN TẠI

| Phép kiểm | Kết quả |
|---|---|
| Cú pháp Lua 4.0.1 | **1.549/1.549** tệp tên ASCII đạt |
| 1.398 tệp tên tiếng Trung | cân bằng từ khoá lệch **0** |
| Lời gọi hàm nil | **0** |
| Gọi hàm nil ở thân chunk (28 tệp đã sửa) | **0** |
| Tham chiếu treo | **2**, cả hai cố ý (`hd3_driver.lua:49-50` — tên NPC cần xoá) |
