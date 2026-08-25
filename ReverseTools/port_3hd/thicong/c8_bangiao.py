# -*- coding: utf-8 -*-
"""C8 - cap nhat BANGIAO_3HOATDONG_2508.md sau dot C (25/08 sang):
   - thay muc "Viec CON LAI" (da lam xong ca 3)
   - them phu luc DOT C (C1..C7) + trang thai binary moi
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

P = r"D:\GAMEDEVNEW\BANGIAO_3HOATDONG_2508.md"
d = io.open(P, encoding="utf-8", newline="").read()
if "ĐỢT C" in d:
    print("da cap nhat roi"); raise SystemExit
nl = "\r\n" if "\r\n" in d else "\n"

# ---- 1) thay muc "Viec CON LAI" cu ----
i = d.index("## Việc CÒN LẠI (chưa làm, có chủ đích)")
old_tail = d[i:]
new_tail = "\n".join([
    "## Việc còn lại của vòng soát — ĐÃ LÀM XONG trong ĐỢT C (xem phụ lục dưới)",
    "",
    "1. ~~F11 cho Săn Boss Sát Thủ~~ → **ĐÃ LÀM** (C7, xem phụ lục).",
    "2. ~~Bảng thưởng chưa đưa ra config~~ → **ĐÃ LÀM** (C5+C6: 8 khoá thưởng).",
    "3. ~~Khoá `HD3_*` chết~~ → **ĐÃ NỐI** vào script thật (C5: 6 khoá).",
    "",
    "---",
    "",
    "# PHỤ LỤC 2 — ĐỢT C HOÀN THIỆN (25/08 sáng, sau vòng soát)",
    "",
    "Script thi công: `ReverseTools\\port_3hd\\thicong\\c1..c8_*.py` + `b3_admin.py` v2.",
    "",
    "## C1+C2 — vá gốc A7-C1 (AddNpc → phe ĐỒNG MINH)",
    "",
    "Vòng soát phát hiện: JX1 `LuaAddNpcEx` tham số 7 = **camp** (gọi `SetCurrentCamp`),",
    "còn bản Linux tham số 6 = bNoRevive; cách vá cũ dựa `g_IsJx2Script` **không ăn** vì",
    "`HD3_DriverInit` chạy trong state của `startgame.lua` (không thuộc danh sách JX2).",
    "Giải pháp: **2 hàm C++ mới, KHÔNG đụng camp** (`KJx2WarInfra.cpp`, khối `[3HD 25/08]`):",
    "",
    "| Hàm | Nghĩa tham số (GIỐNG HỆT bản Linux) |",
    "|---|---|",
    "| `HD3_AddNpc(id, lg, mapIdx, x32, y32, noRevive, name)` | series TỰ SINH `g_Random(5)` như Linux `rand()%5` |",
    "| `HD3_AddNpcEx(id, lg, mapIdx, x32, y32, series, noRevive, name)` | series chỉ định |",
    "",
    "- `noRevive ≠ 0` ⇒ `Npc[n].m_bNoRevive = 1` (đúng nghĩa Linux, KHÔNG phải camp).",
    "- Không gọi `SetCurrentCamp` ⇒ giữ camp từ `npcs.txt` ⇒ **boss đánh được** (hết lỗi đồng minh).",
    "- C2 đổi TOÀN BỘ điểm gọi trong script 3HD sang 2 hàm mới (driver, autoexec, killer,",
    "  fengling, challengeoftime). Đăng ký tại `ScriptFuns.cpp` (trong `#ifdef _SERVER`).",
    "",
    "## C3 — remap vật phẩm đợt 2 (A7-N1)",
    "",
    "- `c3_remap2.py`: **98 ánh xạ** áp vào ~37 tệp (đợt b2 mới phủ một phần).",
    "- ⚠️ **~40 vật phẩm `6,1,30xxx` KHÔNG có hàng JX1 tương đương** (Đồ Phổ Đằng Long,",
    "  nguyên liệu chế đồ JX2...) — GIỮ NGUYÊN id gốc, rơi ra sẽ là item lỗi nếu bảng",
    "  item JX1 không có; danh sách trong `thicong\\remap_resolved.json` khoá `KHONG TIM DUOC`.",
    "  Muốn dùng phải thêm hàng vào bảng item hoặc đổi sang item JX1 khác (quyết định chủ game).",
    "",
    "## C4 — map 957 (Mật Phong của ải — chế độ chuangguan30)",
    "",
    "- `Maps\\WorldSet_GameServer.ini`: `World910=957`, `Count=911` (backup `.truoc_3hd_2508`).",
    "- Chép `settings\\maps\\liandandong\\npc_3.txt` từ gốc A Linux.",
    "- `MapList.ini` ĐÃ CÓ sẵn block 957 (kể cả `NewWorldParam USETOWNP_OFF|HEART_OFF`).",
    "",
    "## C5+C6 — nối config THẬT + 8 khoá phần thưởng (Yêu cầu 2 trọn vẹn)",
    "",
    "Khuôn: `<biến> = HD_CFG(\"KHOA\", <mặc định gốc Linux>)`; bảng thưởng dùng `nil` = bảng gốc.",
    "",
    "| Khoá mới | Nối vào |",
    "|---|---|",
    "| `HD3_VA_PHUT_BAODANH / PHUT_NHIEMVU / NGUOI_TOIDA / LUOT_NGAY` | `challengeoftime\\include.lua` |",
    "| `HD3_ST_MAX_NGAY` | `killer\\nieshichen.lua` (KILLER_MAXCOUNT) |",
    "| `HD3_PLD_SUC_CHUA`, `HD3_PLD_GIO_TONPHI` | `fengling_ferry\\fld_head.lua` |",
    "| `HD3_ST_THUONG` (nil=bảng gốc) | `killer\\kill_level.lua` (thưởng nhóm 90) |",
    "| `HD3_PLD_SO_AN_BOSS / TILE_HAILONG / TILE_TRUYCONG / THUONG_CAPBEN` | `bossdeath` / `shuizeideath` / `mission.lua` |",
    "| `HD3_VA_THUONG_HOANTHANH / HANG_NGAY / RUONG` (nil=bảng gốc) | `award` / `rank_perday` / `chuangguanbaoxiang` |",
    "",
    "- C6 sửa nhãn `[LIVE]`→`[RESTART]` cho khoá chỉ nạp lúc boot; 3 khoá cấp → `[HIEN THI]`;",
    "  thu hẹp `tbRangeId` cao cấp `{1026,1033}→{1026,1031}` + `{1034,1037}→{1035,1037}`",
    "  (JX1 1032-1034 = Boss New Dragon, hệ khác đã chiếm chỗ) — chốt A7-N6.",
    "",
    "## C7 — F11 Chỉ nam nhiệm vụ: mục \"Săn Boss Sát Thủ\" (Yêu cầu 3 trọn vẹn)",
    "",
    "- `UiTaskGuide.cpp/.h`: `TASKGUIDE_SATTHU_TASKID 10`, 2 nhánh rẽ, `BuildSatThuText()`",
    "  (đọc task 1082 = chỉ số boss 1..160, 1193 = số lần giết hôm nay/8).",
    "- `UiTaskGuideStr.h`: 7 chuỗi `ST3_*` TCVN3 thô.",
    "- `bin\\client\\Ui\\uitasklist.ini`: mục `[5] Name=+Săn Boss Sát Thủ TaskId=10`.",
    "- Đường đồng bộ có sẵn: killer dùng `nt_setTask` → `SyncTaskValue` → client `UI_TASKVALUE`.",
    "",
    "## b3_admin v2 — lệnh bài admin sửa theo phản biện",
    "",
    "3 menu con (Sát Thủ / PLD / Vượt Ải), sửa: Phượng Tường (không phải Ba Lăng),",
    "bỏ `/` trong nhãn nút, thêm nút Thoát, SetBoss/Gian2/SetRank/ShowCfg.",
    "",
    "## Binary MỚI đặt cạnh (25/08 09:13) — CHỜ RESTART, CHƯA TEST",
    "",
    "| Tệp | Cỡ | Ghi chú |",
    "|---|---|---|",
    "| `bin\\server\\CoreServer.dll.moi_2508_3hoatdong` | 18.057.216 | CÓ `HD3_AddNpc/Ex` |",
    "| `bin\\client\\CoreClient.dll.moi_2508_3hoatdong` | 2.334.208 | |",
    "| `bin\\client\\Game.exe.moi_2508_3hoatdong` | 1.264.128 | CÓ `BuildSatThuText` (F11) |",
    "",
    "🔴 **DLL server ĐANG CHẠY (boot 08:46) CHƯA CÓ `HD3_AddNpc`** ⇒ NPC 3HD hiện KHÔNG",
    "spawn (gọi nil lặng lẽ trong state startgame). Restart với DLL mới thì hết.",
    "",
    "Lệnh swap lúc server/client TẮT:",
    "```",
    "cd /d E:\\SourceTuanLe\\SourceVs22\\TESTLOFFF_ONLINE",
    "ren bin\\server\\CoreServer.dll CoreServer.dll.cu_2508_truoc3hd",
    "ren bin\\server\\CoreServer.dll.moi_2508_3hoatdong CoreServer.dll",
    "ren bin\\client\\CoreClient.dll CoreClient.dll.cu_2508_truoc3hd",
    "ren bin\\client\\CoreClient.dll.moi_2508_3hoatdong CoreClient.dll",
    "ren bin\\client\\Game.exe Game.exe.cu_2508_truoc3hd",
    "ren bin\\client\\Game.exe.moi_2508_3hoatdong Game.exe",
    "```",
    "",
    "## Kiểm sau đợt C",
    "",
    "- Build: `Server Release|x64` + `Client Release|Win32` + `Game.exe` (Release|Win32,",
    "  cần `/p:VcpkgEnableManifest=false`) — **0 error C / 0 error LNK**.",
    "- `syncheck.exe` (Lua 4.0 engine): **89/89 tệp Lua** trong gương đạt cú pháp.",
    "- Gương ↔ cây sống: **115/115 tệp giống byte** (114 dưới `bin\\server` + `uitasklist.ini`",
    "  dưới `bin\\client`, đối chiếu md5 tay).",
    "- `check_encoding.py`: FFFD=0 trên mọi tệp C++ đã sửa.",
    "- ScriptError.log boot 08:46: **0 lỗi HD3** (162 lỗi `qianbaoku` là của hệ Tín Sứ cũ,",
    "  có từ 23/08 — KHÔNG thuộc đợt này; 160 lỗi `kill_level OnRevive` chỉ ở boot 01:49,",
    "  trước khi vá).",
    "",
]).replace("\n", nl) + nl
d = d[:i] + new_tail

# ---- 2) dinh chinh 2 con so cu trong phu luc 1 ----
d = d.replace("Gương repo khớp cây sống **112/112 tệp, 0 lệch byte**",
              "Gương repo khớp cây sống **115/115 tệp, 0 lệch byte** (sau đợt C)")
d = d.replace("**13/13 tệp** qua `syncheck.exe`", "**89/89 tệp** qua `syncheck.exe` (sau đợt C)")

io.open(P, "w", encoding="utf-8", newline="").write(d)
print("da cap nhat BANGIAO, %d dong" % d.count(nl))
