# TIẾN TRÌNH SIMCITY / BOT NGƯỜI CHƠI GIẢ LẬP — SỔ TAY TOÀN DỰ ÁN

> Cập nhật cuối: **18/08/2026 ~20:45** · head lúc ghi: `a829482b` (+ các commit crash-log phía client mới hơn của chủ game)
> Tài liệu liên quan: `PHANTICH_SIMCITY_JX2.md` (phân tích bản gốc), `PORT_SIMCITY_JX1_KEHOACH.md` (kế hoạch port), `BANGIAO_DATAU_TASKLINK.md`, `DANHSACH_DATAU_PORT.md`.

---

## 1. HAI HỆ BOT — PHÂN BIỆT TUYỆT ĐỐI

| | Bot **SimCity** (`SC_*`) | Bot **KPlayer thật** (`PB_*`) |
|---|---|---|
| Tệp chính | `Sources/Core/Src/KSimCity.cpp/.h` (~1650 dòng) | `Sources/Core/Src/KPlayerBot.cpp/.h` (~4300 dòng) |
| Bản chất | `KNpc` kind_player, KHÔNG có KPlayer | `KPlayer` THẬT nạp từ roledb (tài khoản `1`..`1000`, mẫu `hinodl`) |
| Dùng cho | Trang trí thành thị: đi dạo, ngồi bán, chat bong bóng | Luyện cấp, đánh quái, nhặt đồ, tổ đội, chat kênh, PM |
| An toàn | Người chơi KHÔNG giao dịch được (không KPlayer trong region list) | CÓ túi đồ thật → **CẤM** đặt `m_BaiTan` lên loại này (người chơi mua được đồ thật) |
| Điều khiển | Lệnh bài admin → `SC_Menu` | `SC_Menu` → mục "Bot NGUOI CHOI THAT" → `PB_Menu` |

Lệnh bài: `script/item/lenhbaiadmin.lua` → `script/item/simcity_admin.lua` (repo: `serverscript_jx2/jx1_edits/`, PHẢI giữ giống bản live).

## 2. DÒNG THỜI GIAN & COMMIT MỐC (18/08)

| Mốc | Nội dung |
|---|---|
| (các đợt trước) | Port SC_ NPC bot (GD1-GD4), PB_ bot vào phái + đánh quái + di chuyển A* server (`FindPathServer`, lưới `maps/<id>_srv.fp`), trap-immunity, chọn chiêu đúng hệ/vũ khí, túi máu/TTL |
| `2bca25f1` | Nhặt đồ + dọn túi (giữ thuốc/túi/phù/TTL) + chống xuyên biên đợt 1 + kiểm phạm vi |
| `f740082b` | **11 lỗi do 4 vòng phản biện bắt** (FindEmptyPlace hàm chết → FindRoom; chỉ nhặt đồ CỦA MÌNH; đường cụt A*; điểm chết ngoài tầm; TeamDismiss cấm dùng...) |
| `88aa5d44` | Nhặt-TRƯỚC-đánh-sau (giết xong ra nhặt liền) |
| `e0686eb6` | Fix lag A* (nối vùng nháp `m_aGCost` + mảng thế-hệ) + `[BotPerf]` + PM trễ 2-6s/cấm 8-15s/chống lặp |
| `4e1231bb` | **Tổ đội**: 40% bot lập nhóm ≤4 người, bám đội trưởng ≤200 MPS, đổi map tự rời/giải tán, xáo 10 phút |
| `08691d75` | Chat kênh THẾ GIỚI (id kênh học lúc chạy qua hook NOTIFYCHANNELID; bơm `SendSystemInfo` từng người thật) |
| `87b9631f` | Mở trần **1000 bot** (PB_MAX_BOTS) |
| `074982fd` | Kênh thế giới dùng **chatworld.txt 119 câu bản gốc** + **POST LINK ĐỒ THẬT** (token 40 số) |
| `1784f89d` | **Trang trí thành thị** (SC GD5: SC_SetBotStall/Sit/CityNodes/PickSpawn/RandomName + menu `SC_City`) |
| `c05d673b` | Menu gọi 1/100/1000 bot |
| `d2a4c142` | Lệnh JOIN/FIGHT thành **CHẾ ĐỘ bền vững** (one-shot bỏ sót bot đến muộn) + giãn bãi + trần 120/bãi + giãn log |
| `5db79a8c` | So le đổi map ép về **cửa sổ 60 giây** (trước đó bot #999 chờ 17 phút → "đứng thành") + lưới đặt chân 9×7 |
| `acefe7cf` | **Hết leo vách**: ô vật cản GÓC = vật cản trên lưới A* + nâng `kMagic 0x53465002` (77 cache tự dựng lại) |
| `081be3a3` | `[SvPerf]` nhiệt kế 4 tầng khung server + `PB_LogNgoai` |
| `c22149db`/`6c1b4c27` | Truy + **FIX gốc tổ đội không ghép: `g_Random` engine.lib ĐÓNG BĂNG THEO GIÂY** → trộn chỉ số (`+ s_botCount*37`) vào phép gieo + điểm tản ra |
| `a829482b` | **Giãn dân**: quanh 20 ô >20 bot → di tản điểm quái xa (≥1200 MPS) luyện tiếp |

## 3. TÍNH NĂNG HIỆN CÓ + TRẠNG THÁI

**Đã xác nhận chạy (qua log/test):** sinh 1000 bot dần (2/nhịp), tản ra, vào phái đúng ngũ hành/giới tính, nhận vũ khí theo phái, tự cộng tiềm năng, chọn chiêu đúng hệ + có sát thương, nâng kỹ năng theo cấp, đánh–đuổi–roam bằng A*, hồi sinh, uống bình/mở túi/TTL, nhặt đồ của mình + dọn túi (giữ: thuốc, 4813, **4821**, 437, phù thường genre 5, phù vĩnh viễn mine 1/4, 1271, 70/71/1182, đồ task), chat bong bóng (kênh phụ cận ĐÃ TẮT theo yêu cầu), PM trễ/chống lặp, chat kênh thế giới câu rao vặt, `[BotPerf]`/`[SvPerf]`/`[BotNhom] tk` chẩn đoán.

**CHƯA KIỂM CHỨNG (việc đầu tiên phiên sau — kéo `bin\server\bot.log`):**
1. Tổ đội sau fix RNG: kỳ vọng `roll gieo=` ra số khác nhau → `tk ... muon nhom ≈ 400` → `vao nhom cua ...` → thành viên bám theo.
2. Giãn dân `[BotDan]`.
3. Hết leo vách (sau khi 77 lưới `_srv.fp` tự dựng lại — lần đầu vào map tốn vài giây).
4. Link đồ trên kênh thế giới hiển thị đúng trên client.
5. **Trang trí thành thị `SC_City` — CHƯA TEST LẦN NÀO** (đứng trong thành 37/78/176/162/80/1/11 hoặc 8 thôn → lệnh bài → SimCity → "Trang tri thanh thi (map nay)").

**Backlog:** kênh THÀNH THỊ (không tồn tại trong cây nguồn — chỉ CH_WORLD/CH_SONG/CH_JIN; hook `[BotKenh]` sẽ tự liệt kê nếu ini client trong pak có); tách bãi cấp 10 quanh Hoa Sơn (cả 1000 bot cấp 10 dồn 1 map — server chỉ 15–19%/lõi, lag là client vẽ; có thể lấy điểm từ `maps/thanhthi/2_*.txt`); chốt giữ/xóa 4822/4823 Bảo Tiên Lộ + 121–123 Phúc Duyên Lộ; PK/Tống Kim; tên nữ cho bot nữ; GỠ log `[BotNhom] roll` khi tổ đội xác nhận OK.

## 4. KẾT QUẢ DỊCH NGƯỢC BẢN GỐC (18/08)

### 4.1 `vdk.so` (54.084 byte, ELF32, trong `jxser.tar.gz` tại `E:\...\SOURCESUPDATE_KINHMACH_ONLTEST0608\simcity\`)
- KHÔNG import hàm engine nào (chỉ `__divdi3/__udivdi3/strlen`) — là **patcher chữ ký**: 188 địa chỉ tuyệt đối trỏ vào `jx_linux_y` bản 32MB TRONG TAR (bản `D:\ServerLinux\...\jx_linux_y` 8.9MB LỆCH 33/34 chữ ký — mod tự SKIP).
- 51 hàm Lua inject (bảng tên tại .rodata 0xC000): BotSayLocal, SendTradeItem, SetNpcStall, SetBotStallTier, NpcSit, PollTradeStay, HasPlayerSay/PollSayForBot (22 từ khóa VN tại 0xC2BF)...
- **`BotSayLocal` = HÀM RỖNG** (0x17D0: pop 2 tham số rồi `ret`) — khớp ghi chú tác giả trong `sim.fun.lua:13` "tạm revert NpcChat (bong bóng)". **Bản gốc trong tay KHÔNG có chat kênh** — chỉ còn file liệu.
- **`SendTradeItem` KHÔNG phải post đồ lên chat**: tạo item tạm → `gMakeSyncItem_Info` → gói protocol **0x5F** 80 byte gửi cho ĐÚNG MỘT người (hiện trong cửa sổ giao dịch, trạng thái `PollTradeStay==2`), rồi hủy item. Lệnh người chơi gõ: `TRADE:<id>`, `DUEL:<n>`, `TCLR:<n>` (hook `c2sSendTextCmd` chỉ ĐỌC vào, không gửi).
- Banner XOR 0x5C tại 0xC900: "Mod VLTK Code By V.D.K", trạng thái từng patch (nhiều mục SKIP sig mismatch).

### 4.2 Lua + dữ liệu bản gốc
- Cây Lua: `jxser/server1/script/global/nobitaxd/vdk/simcity/` (components/class/plugins/libs). `pthanhthi.lua` 829 dòng = trang trí thành thị: THANHTHI_SIZE=300 + THON_SIZE=50, spawn khi CÓ người vào map (despawn khi người cuối rời), 45–65 sạp/thành **tụm quanh địa danh** (`attractions.txt`, node <8 ô, lệch ±12 ô) + 20–30 quanh Dã Tẩu, drip 1 batch/3 giây, đi dạo đồ thị node 90% đi tiếp/10% dừng (gần địa danh đảo 5%/95%), sạp `isStanding=1` không bao giờ di chuyển + TẮT stuck-respawn.
- `chat.txt` 2745 câu 21 nhóm; `chatworld_msg.txt` **119 câu kênh thế giới** (20 câu có chỗ trống `{item}`); `stall_adv.txt` 21 biển sạp; `names.txt` 737 tên. **TẤT CẢ CÙNG BẢNG MÃ với mã nguồn dự án** (chuỗi "giá" = `67 69 b8` trùng 16 file nguồn) → chép thẳng, KHÔNG chuyển mã. Đã chép vào `settings/simcity/` live + `serverscript_jx2/jx1_edits/simcity_data/`.
- `maps/thanhthi/` 271 file node (`X_Y \t links \t is_exact \t type`) + `*_preset.txt` tuyến tuần tra + `thanhthi.txt` chỉ mục + `attractions.txt` địa danh — đã có sẵn cả live lẫn repo.
- `vdk.dll` (Windows, 5MB) bị **UPX** — chưa mổ; nghi là nơi chứa bản chat-kênh đời sau. `jx_linux_y` 32MB còn `.symtab` 10.699 symbol (IsDisabledChatWorld/City = predicate Lua, không phải hàm gửi).

### 4.3 Bẫy engine JX1 đắt giá (đã trả học phí)
1. **`g_Random` của server ĐÓNG BĂNG THEO GIÂY** — đến từ `engine.lib` DỰNG SẴN (`Core.vcxproj` → `Lib/x64/Engine Server Release`), KHÔNG phải `Engine/Src/KRandom.cpp`. Gieo hàng loạt trong 1 giây = dính chùm (1000 bot cùng trượt thăm 40%). **Luật: trộn chỉ số phần tử `(g_Random(N) + idx*37) % N`.**
2. **Ô vật cản GÓC (Obstacle_LT..RB)**: engine chỉ cho đi NỬA Ô (kiểm chéo `KRegion.cpp:927`), lưới A* cũ coi cả ô đi được → viền vách = "cầu thang" cho bot leo. Đổi predicate thì PHẢI nâng `kMagic` cache.
3. **Client JX1 có sẵn LINK ĐỒ trong chat**: token `[40 số, mỗi số một dấu phẩy kể cả số cuối]` (NUM_INFO_ITEM_CHAT=39; thứ tự: id,genre,detail,parti,series,level,luck,version,randomseed,idx,price,x,y,point,stack,enchance,goldid,yearexp,lock,durability,hlock,nature,maxopt,genLevel[0..15]; builder `UiPlayerBar.cpp:2067`, parser `UiMsgCentrePad.cpp:323`).
4. `m_BaiTan`/ShopName chỉ đi trong PLAYER_SYNC lúc client vào region → đặt NGAY sau AddBot; `SC_SetBotSit` phải khóa `m_nTimeIdleValue=0`.
5. Kênh chat: id cấp LÚC CHẠY theo tên (hook `PLAYERCOMM_NOTIFYCHANNELID` tại `KNewProtocolProcess.cpp`); bơm = `SendSystemInfo(nType=1 từng người)` — **nType=0 CẤM** (không lọc bot); channelid=-1 = kênh GM.
6. Tổ đội: vào nhóm qua `GetInviteReply` trực tiếp; **CẤM `TeamDismiss`** (chạm `Player[-1]`); blob mẫu `m_dwLeadLevel=0` → kẹp ≥1; team mở rộng chủ quyền nhặt đồ cả nhóm; exp share 768 MPS.
7. `ScriptFuns.cpp` KHÔNG include `KSimCity.h` — hàm SC_ mới phải khai `extern` tại chỗ (~dòng 12704).
8. Nhặt đồ: `FindEmptyPlace` là hàm chết (dùng `FindRoom`); chỉ nhặt `m_nBelong == chính mình` (đồ -1 gồm cả đồ người thật vứt); `FindPath_Block` trả **2 = đường cụt**.

## 5. VẬN HÀNH / QUY TRÌNH

- **Build**: chỉ build thẳng `Core.vcxproj` cấu hình `Server Release|x64` với `/p:SolutionDir=D:\GAMEDEVNEW\Sources\` — **ĐỪNG build qua .sln** (ánh xạ sai x64→Win32) và **đừng build từ cây nguồn khác** (không có code bot).
- **Deploy**: chép `Sources\Core\x64\ServerRelease\CoreServer.dll` → `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\CoreServer.dll` (đổi tên bản cũ trước nếu server đang chạy), verify md5 hai bên.
- **🔴 BẪY 16:29 18/08**: một bản build tay (17.726.976 byte, KHÔNG có chuỗi `PB_AddBot`) từng đè DLL → lệnh bài lỗi `PB_BotCount (a nil value)`. Chẩn đoán: `grep -c PB_BotCount CoreServer.dll` = 0 nghĩa là bị đè → chép lại.
- **Log**: `bin\server\bot.log` — các thẻ: `[BotPerf]` (nhịp bot/A*/quét đồ mỗi 10s), `[SvPerf]` (4 tầng khung server), `[BotNhom]` (roll/tk/vào nhóm), `[BotDan]` (giãn dân), `[BotBien]`, `[BotDame]`, `[BotNhat]/[BotXoaDo]`, `[BotKenh]` (kênh chat học được + câu đã nói), `[BotPM]`...
- **Sửa nguồn**: file TCVN3 → script python latin-1; chuỗi có backslash → BẮT BUỘC qua Write tool (bẫy Bash heredoc nuốt nửa backslash — đã cắn 2 lần trong 18/08).
- **Trình tự test chuẩn**: restart → lệnh bài → Gọi 1000 → "Cho bot VAO PHAI" → "BAT danh quai" (hai lệnh giờ là CHẾ ĐỘ, bấm sớm muộn đều được) → chạy 3–5 phút → kéo log.

---

## 6. HE LUU DU LIEU BOT (18/08 toi - "tat server mo lai bot van giu cap/do/vi tri")

**Thiet ke** (bot duoc luu NHU NGUOI THAT, di dung duong luu 20 nam):
- Spawn xong gui `PB_ASK_LOCKROLE` -> GameServer gui `c2s_roleserver_lock` toi Goddess
  (thieu khoa thi `_SaveRoleInfo` VUT bai luu - `IsRoleLockBySelf`).
- Luu DINH KY **30 GIAY/bot NHU NGUOI CHOI** (yeu cau chu game 18/08 dem; truoc do 10 phut).
  PB_Breathe toi da 2 goi/nhip = 36 goi/giay, du 1000 bot nhip 30s (can 33,3/s); dong hon
  thi nhip tu gian (bo dieu tiet). AN TOAN da kiem bang ma: BDB `DB_TXN_NOSYNC`
  (DBTable.cpp:33 - put khong fsync), GUI Goddess cat tran 100 dong ListBox
  (IDBRoleServer.cpp:731). Phi: file log Goddess phinh ~170MB/ngay khi du 1000 bot
  (muon thi xoay vong). Dem "luu %d goi" in kem [BotPerf] moi 10s de quan trac.
  Luu NGAY sau khi vao phai xong; go bot (`pb_KillBot`) luu `bLeave=true` giua
  PrepareRemove va RemoveQuiting (dung trinh tu PlayerLogoutGateway), that bai thi gui UNLOCK.
- `KPlayer::Save()` mo cong `netidx==-1` CHI khi `PB_IsBot` (PB_IsBot da so ca dwID).
- DAU BOT: `SavePlayerDataAtOnce` dong `PB_BLOB_DAU 0xB07B07` vao truong chet
  `BaseInfo.irevivaly` (nguoi that luon ghi 0, khong ai doc khi nap, Goddess chi in log).
  Blob mang dau + nSect hop le = "bot cu": KHONG xoa ky nang, phai = nSect,
  nAi = IN_FACTION, nGaveWeapon = 1 (de 0 la pb_GiveFactionWeapon HUY vu khi dang cam!).
- LUAT CAP 20: blob bot cu co `ifightlevel < 20` -> ep cUseRevive=1 + irevivalid=53 +
  irevivalx=19 (ve diem hoi sinh map 53); tu cap 20 dung nguyen cho da luu
  (`cUseRevive=0` + `ientergame*` do SavePlayerBaseInfo ghi - `SetLoginType(0)` duoc ep
  truoc MOI lan luu de script SetLogoutRV khong lat co).
- Menu lenh bai: "LUU du lieu bot ngay" -> `PB_SaveAll()` don 5 goi/nhip (~11s/1000 bot);
  "Go het bot" gio luu tung con truoc khi go (PB_KILL_PER_TICK ha 20 -> 5).
- Mat khoa role (vd Goddess restart): Goddess tra -1 -> GameServer doc byte ket qua
  (truoc day BI NUOT), bao Core `PB_OnSaveFailed` log + tu gui lai LOCKROLE; nhip 30s
  ke tiep tu luu bu (khong hen som - hen som voi nhip 30s la phep tru ra so am).
  Log `[BotLuu]` ghi du: nap lai bot cu / Goddess tu choi / khoa that bai / luu XONG.

**Phan bien 6 lang kinh da bat & da sua trong dot nay:** PB_IsBot thieu doi chieu dwID
(nguoi that chiem khe bot bi kick se bi dong dau oan); guard `nLen>=30` chan ca lenh
luu/mo khoa (bot ten 30-31 byte); nuot ket qua luu -1; `s_nLuuTatCaCon` ket khi ClearBot
cat ngang SaveAll; strcmp -> strcmpi + bo entry chet o vong chong spawn trung; tai-clone
mau tu bot co dau (taobot_bdb da them `irevivaly=0`, server them dieu kien nSect hop le);
bo qua bot dang chet/mat NPC khi luu dinh ky; NULL-deref `LoadPlayerStateSkillList`
(loi engine san co, sap server khi blob mang state-skill khong con trong bang - da va
chung cho ca nguoi that).

**RANG BUOC VAN HANH (quan trong):**
1. `GameServer_cfg.ini [Overload] MaxPlayer` PHAI >= 1500 khi bat bot (live dang 1500;
   ha xuong la nguoi that bi day qua tran `nIndex > m_nMaxPlayer` het duong luu).
2. Dai tai khoan PB_AddBot(1,1000) PHAI la tai khoan DO taobot_bdb TAO - dung tro vao
   tai khoan nguoi that ten thuan so; dung dang nhap tai khoan 1..1000 bang client that
   trong luc dang goi bot.
3. Tat server "sach": bam "LUU du lieu bot ngay" -> doi dong `[BotLuu] ... XONG` trong
   bot.log -> doi them ~10 giay (Goddess ghi not) -> tat. Khong bam gi thi mat toi da
   ~30-60 giay luyen cuoi (nhip luu dinh ky 30s nhu nguoi choi).
4. Muon doi NHAN VAT MAU cho taobot_bdb: build lai tool (da them dong xoa dau
   `p->BaseInfo.irevivaly = 0;` sau memcpy - tools/taobot_bdb, binary cu CHUA co).
5. Con no (ghi nhan, chua sua): reply saverole tre co the SetSaveStatus(SAVE_IDLE) len
   khe nguoi that vua tai su dung (mat 1 chu ky autosave, tu lanh sau 30s - loi lop cu,
   1000 bot khuech dai); khoi PB_ trong GameScriptFuns nam ngoai #ifdef _SERVER (build
   CLIENT von da hong tu truoc); chay >= 2 GameServer thi unlock cua Goddess khong kiem
   chu khoa (live 1 GS - khong sao).

**Trinh tu test he luu:** restart server -> Goi 1000 -> VAO PHAI -> BAT danh quai ->
doi 5-10 phut (xem `[BotLuu]` bao luu ngay-sau-vao-phai) -> "LUU du lieu bot ngay" ->
doi XONG + 10s -> tat CA GameServer LAN Goddess -> bat lai -> Goi 1000 -> ky vong:
`[BotLuu] <ten> nap lai bot cu: cap X phai Y (giu nguyen cho cu / duoi cap 20 -> ve map 53)`,
bot >= 20 dung dung cho cu voi nguyen do/skill, khong can vao phai lai; BAT danh quai
la chien tiep. Kiem tra cheo: console Goddess in `SaveRoleInfo:<ten>` moi lan luu.

## 7. PHAN TICH LAG 1000 BOT (18/08 toi - so lieu tu bot.log 10h)

- `[SvPerf]` (moi 10s): BotKPlayer median 1404ms (14%CPU 1 loi, max 3245ms luc relogin);
  MainLoop engine 595ms (0 bot) -> ~840ms (1000 bot); MessageLoop ~10ms; SimCity 0.
  Server GIU DU 18fps (tong ~15-21ms/khung trong quy 55ms) => lag nguoi choi thay la
  CLIENT VE hang tram sprite, khop ket luan truoc; giai phap van la gian dan (da chay).
- DA SUA O(n^2): 3 cho trong pb_DriveBot quet ca mang s_bots tim chi so chinh minh
  (~18-36 trieu vong lap/giay voi 1000 bot) -> phep tru con tro `(int)(&b - s_bots)`.
  Ky vong BotKPlayer ms giam ro trong [BotPerf] dot chay toi.
- A* ~1000 lan/giay (10 lan/bot/10s, ty le tuyen tinh theo so bot) - binh thuong sau
  fix e0686eb6. bot.log dang phinh ~33 dong/giay ([BotDame] 6,4/s, [BotDanh] 5,9/s,
  [BotHoang] 3,7/s; moi dong 1 lan fopen/fclose) - DE XUAT dot sau: khi he da on dinh,
  ha bot log [BotDame]/[BotDanh]/[BotHoang] hoac them cong tac do on.
- RNG to doi: fix an tu 16:39 (gieo= 30 gia tri phan biet/batch, ~33% muon nhom);
  19:11 co 339/1000 bot trong nhom. Con 103 dong "VAO NHOM THAT BAI mem=0/tran=3 mo=1"
  (GetInviteReply tu choi) - dang ngo, viec dot sau.

## 8. EP BOT KHONG RA KHOI MAP (19/08 sang - "bot van di chuyen ra khoi map")

**Chan doan (5 agent doc ma + phap y bot.log 2,2 trieu dong):** cac rao 18/08 dang chan
dung (0 toa do am toan log). Ba cua thoat con lai:
1. `FindPathServer` KEP dich ngoai map ve mep luoi roi tra "duong tron ven"
   (KSubWorld.cpp cu :3296-3301) -> PB_WalkTo di het waypoint roi CHANG CUOI `do_run`
   thang vao toa do THO ngoai map. Nguon dich ngoai map: scatter chi kep can duoi,
   `nBuocRa` 8-11 o khong kiem, aim lam tron 128 khong kiem lai.
2. **VUNG RONG du lieu**: region KHONG co tep vat can bi
   `memset 0` = "toan di duoc" o CA luoi A* lan engine -> bot di bo hop le ra vung
   (DINH CHINH 19/08: con so "292/480" trong chu thich ma la SAI khi doc nhu ty le -
   hai phep dem thuoc HAI THU MUC ROI NHAU; tong that 2638 tep _S vs 480 tep _C.
   Ket luan van dung: CO map chi dong goi ban _C nen fallback _Region_C la can.)
   nhin-nhu-ngoai-map, khong de lai dong log nao (khop hien tuong chu game thay).
3. `ChangeWorld`/`SetPos` chi kiem khung region, khong kiem vat can.

**Da sua (3 vong phan bien 3+1 lang kinh, build xanh):**
- `PB_WalkTo` TU CHOI ngay dich ngoai map (Mps2Map nR<0 -> -1; moi caller co san nhanh -1).
- Chang cuoi KHONG lao vao dich tho nua: kep dich vao BLOCK CUOI cua duong A*
  (`BlockNearestMps`); dich bi thay the ma da toi sat diem kep -> bao thua (-1).
- Luoi A* bot: region KHONG doc duoc du lieu vat can -> VAT CAN TOAN BO (chi ap
  `#ifdef _SERVER`); co fallback moi doc `_Region_C.dat` (cung enum SCENE_FILE_INDEX,
  cung payload 2048B - 3 loader san co da doc _C nhu vay) roi moi toi `_OBSTACLE.DAT`.
  Log `[PathSrv] map X: N/M region KHONG co du lieu vat can`; map 100% thieu in
  CANH BAO "BOT KHONG DI DUOC tren map nay".
- kMagic cache `_srv.fp` 02 -> 03 + BO guard "khong ghi de": guard cu lam 77 map tu
  dung lai luoi MOI LAN BOOT tu 18/08 ma cache moi khong bao gio duoc ghi. Kem kiem
  KICH THUOC TEP TONG + kep parentId khi nap cache (tep cut giua chung = rac).
- Chase: diem lam tron 128 vang ngoai khung -> dung diem tho (khong cam oan quai mep).
- Follow doi truong: HUNG gia tri tra ve PB_WalkTo (caller duy nhat tung vut no) -
  thua thi nghi 5 giay (3 dong don dep nam TRONG cong nghi), trong luc nghi van
  danh/nhat; [BotLach] SetPos reset them follow/loot/nFollowNghiToi.
- Chang cuoi block gop rong (toi 512x1024 MPS): chi dem "khong tien" khi bot thuc su
  KHONG dich chuyen (neo lastMoveTick) - khoi bao thua gia giua dong bang.

**Van hanh sau deploy:** restart server -> lan boot dau MOI map tu dung lai luoi
(cache cu bi vut vi kMagic 03) va GHI DE cache moi - boot thu 2 phai thay dong
"nap cache" tro lai. Doc cac dong `[PathSrv]` de biet map nao thieu du lieu vat can
nhieu/toan bo; map bai luyen nao dinh "CANH BAO ... BOT KHONG DI DUOC" thi bo sung
tep _Region_S/_C hoac rut khoi bang s_bai. Loi engine nen ghi nhan CHUA va (anh
huong ca nguoi that, doi dot rieng): Mps2Map chia so am lot bien voi map rect.left==0;
ServeJump cong offset truoc khi chup old -> nhay lien tuc vao bien troi toa do.

## 9. TU CUU KET LUOI + HET MAI MON TRANG BI (19/08 chieu - f34a5cbb)

Chu game bao (quan sat 09:46-09:55): "bot van dang dung ngoai map" + "mot so bot
chet ve thanh khong len lai map luyen cong". Phap y 60MB cuoi bot.log ra 2 benh:

1) KET LUOI - bot NAP LAI dung vi tri da luu tu thoi "vung rong = di duoc": nay
   vung rong = vat can toan phan -> xuat phat ket -4 vinh vien (FindFreeBlockAround
   chi quet vanh 1 o quanh block; block vung rong gop toi 16x32 o), hoac ket dao A*
   (map 193: CaoVu720/NgoToan954 thua 264 lan/50 phut tai o ~1529,2464). Dung im
   vo han = canh "dung ngoai map".
   FIX [BotCuu] (KPlayerBot.cpp, moi 10s/bot, gion cuu 60s): T1 = o dang dung + CA
   4 o lan can la vat can LUOI nhung engine bao TRONG (nam sau vung rong; o goc
   nua-trong khong bi oan; mep vung rong tu thoat bang vanh block) HOAC T2 = roam
   A* thua 5 lan LIEN TIEP -> SetPos ve diem dat chan bai (o lech 9x7, kiem
   GetBarrierMin; diem ve cung bi chan = map hong toan phan thi KHONG dich) + reset
   walk/chase/roam/follow/loot/jam + buoc-ra. API moi KSubWorld::CellObsSrv(mps):
   1/0/-1, doc obs CHINH O khong kep toa do.

2) TAY KHONG - do ben ve 0: KItemList::Abrade nhanh nDur==0 THAO trang bi nem vao
   tui ("cuc sat") -> don tui cua bot XOA -> 210 bot [BotCast] BI TU CHOI (vukhi
   detail=-1) danh khong sut mau -> cam muc tieu 45s quay vong -> chet lien tuc ve
   thanh (NgoNghia790 hoi sinh 09:13:25 chet lai 09:14:09). Bot van QUAY LAI bai
   ngay giay hoi sinh ([BotBai] cung giay) - "khong len lai" thuc ra la chet qua
   nhanh nen luc nao cung thay o thanh.
   FIX goc: Abrade return som cho bot (PB_IsBot tu kiem bien + dwID) - bot khong
   biet sua do o tho ren; nguoi that/uy thac khong doi gi. FIX don chua: tay khong
   (da co phai + tung nhan vu khi) -> pb_GiveFactionWeapon phat lai vu khi nhap mon
   [BotVuKhi], gion 60s, tran 5 lan/doi (phai quyen "tay khong" la co y), xong ep
   nAtkSkill=0 chon lai chieu.

Phan bien tu bat truoc build: guard Abrade khong duoc doc Player[m_PlayerIdx] truoc
khi kiem bien (list khong thuoc player co that - guard CheckCanEquip); 5 truong moi
PB_Bot phai init khi tai dung khe (rac nAStarThua>=5 = cuu oan ngay, nVuKhiThu=5 =
cam phat vinh vien); T1 mot-o cuu oan o goc -> doi 4 lan can.

GHI NHAN chua lam: giap DA mat khong phuc hoi duoc (khong co ban ghi) - bot do
thanh glass HP thap, chet nhieu hon binh thuong nhung van luyen duoc; max HP lech
lon giua cac phai (440 vs 10000 o cap ~80) la phan bo tiem nang, viec khac neu can.

TEST sau restart (GameServer dang TAT luc 10:32, DLL moi da nam san bin\server):
bat GameServer -> Goi 1000 -> BAT danh quai -> doi 5-10 phut ->
  grep -a "\[BotCuu\]"  bot.log   # cuu duoc bao nhieu con ket
  grep -a "\[BotVuKhi\].*tay khong giua doi" bot.log   # phat lai vu khi
  grep -a "\[PathSrv\]" bot.log | grep -a "CANH BAO"   # map hong du lieu (lan dau co log nay)
  grep -a "BI TU CHOI" bot.log | tail                    # phai GIAM dan ve 0
Quan sat mat thuong: khong con bot dung bat dong ngoai ria map; so bot lang vang
o thanh giam manh (chi con con vua chet dang cho 60s doi map cua pb_RaBai).


## 10. PHIEN 19/08 CHIEU-TOI: TRANG BI + DA TAU + BAN SAP (7 dot, f34a5cbb -> e243c38c)

**DOC `BANGIAO_PHIEN_1908_CHIEU.md` TRUOC KHI GO** - tai lieu do la ban day du.
Muc nay chi la chi muc nhanh.

**Tinh nang moi:** trang bi theo cap (tieu tiem nang moi lan len cap + bo Hoang Kim
Kim Phong 177-185 + ngua Tuc Suong cap 10 + vu khi cap 10 khi dat 81) · len/xuong ngua
theo GetHorseLimit cua chieu + cuoi ngua trong thanh · party full 8 nguoi · chia deu bai
luyen (chon bai it bot nhat) · doi truong roam diem it bot nhat · buff TTL + Que Hoa Tuu
bat/tat bang lenh bai (mac dinh TAT) · BOT CHAY DA TAU (may trang thai 10 pha) · BOT RA
THANH BAN SAP THAT (nguoi choi mua duoc) · bo SimCity khoi lenh bai.

**SAU LOI GOC tim ra trong phien (gia tri lau dai):**
1. `g_FileName2Id` (Engine/KFilePath.cpp:442) bam PHAN BIET HOA/THUONG -> duong dan
   script viet hoa = ExecuteScript IM LANG khong chay (27.651 dong log chet).
2. Do TIM = `nPoint != 0` (KItem.cpp:3222). `AddItemSet2` tham so thu 12 la nPoint.
   Do XANH = `ItemSet.Add` (khong co nPoint) + mang nMagicLevel co gia tri.
   Do xanh BAT BUOC phai co opt - 0 opt ra do TRANG.
3. `ChangeWorld` chi kiem khung region, KHONG kiem vat can -> bot dap vao than tuong
   (map 79). Nay moi diem dat chan qua pb_ODat (quet xoan oc tim o trong that).
4. `KItemList::Abrade` thao trang bi khi do ben = 0 -> don tui xoa mat -> bot tay khong.
5. `pb_AllocAttribPoints` chi goi MOT LAN luc vao phai (chu thich noi "nhanh IN_FACTION
   tieu tiep" - nhanh do KHONG TON TAI) -> bot cap 82 chi 440 HP.
6. `PB_SetDaTau` cu xoa sach co roi boc lai -> bam menu lan 2 la cat ngang bot dang
   giua nhiem vu (CaoVinh338: teleport 16:30:57 -> bo chon 16:31:40 -> ve bai ngay giay do).

**Luat Da Tau chot:** bot cap >= 80; chi nhan loai 4 KIEU 1 va <= 5 cuon (bac link 11
co 95% dong Num=15 ~13.500 quai; Mat Chi chi rot tu boss xanh ~1/31.360 con); loai 5/6
vua luyen cap vua lam; con lai DOI NHIEM VU MIEN PHI qua `PB_BotDoiNhiemVu` ->
`tl_dealtask`. TUYET DOI KHONG goi `PB_BotCancel` (Task_Cancel that: mat thuong moc-40
+ dot luot ngay). task 1032: byte thap = KIEU cuon, byte 2 = SO cuon.

**Trang thai:** ban cuoi chay that tu 19/08 17:42, log xac nhan may trang thai hoat dong
(35 nhap mon, 49 nhan nhiem vu, 7 doi nhiem vu). CHUA nghiem thu tron vong (chua thay
`TRA XONG nhiem vu` -> `chon ruong thuong`) va CHUA test sap o ban moi.


## 11. PHIÊN 19/08 TỐI #3: SẠP GÓC KẸT + BOT RA NGOÀI MAP 79 + DÃ TẨU 70 + LÀM MỚI TRANG SỨC (deploy 19:1x, chờ restart)

**4 việc chủ game giao:** (1) sạp vẫn dính góc kẹt; (2) bot map luyện công vẫn ra ngoài map, 79 nhiều nhất — điều tra; (3) hạ cấp Dã Tẩu xuống 70 để test; (4) viết hàm cập nhật random trang sức cho sạp; (5) **bắt buộc phản biện khi fix** — đã chạy 3 agent phản biện độc lập (biên dịch / logic game / vật phẩm-hiệu năng), bắt được 1 lỗi thật suýt lọt + 2 rủi ro, tất cả đã xử lý trước deploy.

### Điều tra map 79 (bằng chứng, không đoán)
- Mổ cache lưới `79_srv.fp` (viết `fp_view.py` đọc định dạng SFP03): dữ liệu vật cản map **chỉ vẽ TƯỜNG mê cung**, toàn bộ vùng trống ngoài mê cung (client nhìn là "ngoài map") ghi = 0 = **đi được** → cả engine lẫn lưới A* đều tin dữ liệu, mọi rào 18–19/08 đều cho qua. Các khối chữ nhật full-obstacle trong hình = region thiếu dữ liệu (fix 19/08 sáng hoạt động đúng).
- `bot.log` (run 17:42): quái đứng/lang thang trong vùng trống (engine cho đi), `pb_FindRoamSpot` lấy **vị trí quái** làm đích → `[BotHoang]` 27 lượt nhắm đúng ô (1686,3140) giữa vùng trống → bot đi theo quái "ra ngoài map". 62 lượt `[BotCuu]`/30 phút.
- Lỗ thứ hai đang hoạt động: `[BotLach]` (lách đám đông) SetPos chỉ kiểm engine, **không kiểm CellObsSrv** → 3 bot khác nhau cùng kẹt đúng ô (1477,3023) cạnh bãi Lương Thủy Động.
- Quan sát phụ: 18:12:46 toàn bộ map nạp lại cache lưới (SubWorld reload theo hệ mission?) — vô hại, cache nạp nhanh.

### Đã sửa (KPlayerBot.cpp, một file duy nhất)
1. **DÂY XÍCH BÃI** `PB_XICH_BAI=2000` MPS quanh điểm neo `s_bai` (ngã thả khi đuổi `PB_XICH_BAI_THA=2320`): lọc ở `pb_FindTarget` + `pb_FindRoamSpot` (cả nhánh `[BotDan]` giãn dân) + buông mục tiêu trong chase (`[BotXich]`). `pb_NeoBai` chỉ bật khi đứng ĐÚNG map bãi của mình → Dã Tẩu farm map nhiệm vụ / bán sạp / trong thành không bị xích. Không phân biệt được "vùng trống giả" bằng dữ liệu server nên ép bằng luật sản phẩm: bot luyện quanh bãi, quái ngoài xích coi như không tồn tại.
2. **`[BotLach]`** thêm kiểm `CellObsSrv==1 → bỏ ứng viên` (đóng lỗ đẩy bot vào vùng rỗng) + nới 24→48 ứng viên (bán kính 3..8 ô, gần trước xa sau) bù van thoát khi mật độ bot tăng do xích.
3. **Sạp không ngồi góc kẹt**: ô ngồi phải qua `pb_OSapTot` = nối được tới NPC theo **bản đồ loang BFS cửa sổ 27×27** (`pb_SapLoang`, trần cứng 729 ô — cố ý KHÔNG dùng FindPathServer vì ca không-có-đường sẽ loang cả thành phần liên thông map thành) + thoáng ≥5/8 ô lân cận + chưa ai chiếm; duyệt vòng bán kính 3→12 quanh NPC, thất bại lùi về `pb_ODat` cũ.
4. **`pb_ChamHangSap(nIdx, nLech, nDot, nowT)`** — hàm cập nhật random trang sức (yêu cầu #4): dọn rổ cũ (`pb_DonTrangSucSap` — xóa cả nPrice>0) rồi sinh 3–5 món xanh mới; `nDot = b.nSapDot++` trộn vào mọi phép gieo (g_Random đóng băng theo giây) → mỗi đợt ra bộ hàng khác thật sự (chu kỳ đủ: detail %3, series %5, cấp %10 qua nDot*13, opt %6). Kỳ làm mới: lần đầu / hết hàng 5 phút / **định kỳ 15 phút SO LE +0..59s theo chỉ số bot** (không so le là 200 sạp thay hàng cùng 1 khung). Log `[BotSap] lam moi sap dot N (het hang|dinh ky 15 phut, X mon)` + `[BotSapLoi]` khi đợt châm 0 món.
5. **Đóng sạp dọn hàng tồn**: `PB_SetBanSap` bấm lại → bot bị bỏ chọn được `pb_DonTrangSucSap` dọn rổ trước khi về bãi (trước đây ôm 3–5 món nPrice>0 kẹt túi vĩnh viễn vì `pb_DonTui` cố ý giữ đồ có giá).
6. **Dã Tẩu**: `PB_DT_CAP_TOI_THIEU` 80→70 **và** `PB_DT_CUON_TOI_DA` 5→8. Phát hiện phản biện từ dữ liệu (`levellink.txt` + `tasklink_findmaps.txt`): cấp 70–79 rút bậc 6, dòng loại-4 kiểu-1 nhẹ nhất là **Num=8 (map 122/21)** — với trần 5 cũ thì **nhánh loại 4 là mã chết ở mọi cấp** (bậc 11 chỉ có Num=15 + Mật Chỉ). Nâng 8 thì bot 70–79 nhận được loại 4 thật → test trọn TOI_XAPHU/GODATAU/FARM_NV; lưới an toàn sẵn: farm 20 phút không ra cuộn tự đổi miễn phí.

### Lỗi thật do phản biện bắt được (suýt lọt)
- **Sạp cháy hàng đóng vĩnh viễn**: `SendSellItemCount` (KPlayer.cpp:10371) TỰ hạ `m_BaiTan=0` khi khách bấm vào sạp đã bán hết. Bản nháp đầu chỉ set `m_BaiTan=1` lúc mở lần đầu → sạp cháy hàng bị khách bấm là chết âm thầm (log vẫn đẹp, `c2sTradeBuy` chặn `!m_BaiTan`). Đã sửa: set `m_BaiTan=1` MỖI kỳ châm, ngoài `if (!nMoLai)`.
- Bản nháp đầu dùng FindPathServer kiểm ô sạp → ca không-có-đường loang cả map thành (777k ô × 200 bot cùng khung = treo server nhiều giây) — tự bắt trước khi build, thay bằng BFS cửa sổ.

### Rủi ro ghi sổ (chưa sửa, có chủ đích)
- **Khách mở cửa sổ sạp đúng lúc làm mới**: chỉ số Item toàn cục tái dùng LIFO ngay trong 1 lời gọi → khách bấm mua "khe X" stale có thể được giao món MỚI với giá niêm yết của món mới (server tự nhất quán tiền–đồ, `GetPrice` khe không còn thì mua trượt êm; gói mua không mang giá nên không đối chiếu được). Thiệt hại trần thấp (trang sức xanh 500–2×gốc), có log_giaodich. Muốn đóng hẳn: push `SendSellItemInfo(bUpdate=TRUE)` cho player quanh region sau châm — để đợt sau nếu chủ game cần.
- **Rò slot ItemSet có sẵn của engine** (không phải do vá): `c2sTradeBuy` phía server `KItemList::Remove` không trả slot về pool (chỉ client làm) → mỗi lượt mua sạp rò 1 slot / pool 1.000.000. Vô hại thực tế, ghi để khỏi đổ oan khi soi.
- `pb_FindRoamSpot` trả rỗng không tăng `nAStarThua` → T2 không phủ ca "bot ngoài xích + không ứng viên" (tự khỏi nhờ quái di chuyển + [BotLach] nhảy ngẫu nhiên; phản biện xác nhận không treo vĩnh viễn).
- Bãi nào vùng quái chính lệch xa neo >2000 MPS sẽ bỏ phí map — cách đo rẻ sau deploy: map nào tỉ lệ `[BotXich]`/`[BotDame]` cao bất thường thì CHỈNH TỌA ĐỘ NEO trong `s_bai`, đừng nới xích.

### Vận hành / nghiệm thu (sau restart GameServer)
DLL mới md5 `818de6bebe6fa5814e32897b6b12f0f1` đã nằm `bin\server` (bản 18:24 của phiên F11 lùi ở `CoreServer.dll.bak_1908_1824`; bản mới build từ HEAD `2886e954` nên **chứa cả F11 lẫn hệ bot** — không mất công phiên nào). Build 0 error; 3 vòng phản biện đã đóng hết phát hiện chặn.

```
restart → Gọi 1000 → BẬT đánh quái → Dã Tẩu 50–100 → bán sạp 20–50 → chạy ≥35 phút
grep -a "\[BotXich\]" bot.log | head          # xích hoạt động (nhất là map 79/193)
grep -ac "\[BotCuu\]" bot.log                  # kỳ vọng GIẢM mạnh so 62/30ph
grep -a "\[BotSap\] .*lam moi" bot.log | head  # làm mới sau 15–16 phút, so le
grep -a "\[BotSapLoi\]" bot.log                # kỳ vọng 0 dòng
grep -a "\[BotDT\]" bot.log | grep -a "loai 4" # kỳ vọng có NHẬN loại 4 (8 cuộn, map 122/21)
```
Điều kiện đạt: không còn bot đứng/di chuyển ngoài map 79 sau ~10 phút (bot cũ nạp lại ngoài xích cần vài phút roam về); sạp ngồi quanh NPC Dã Tẩu chỗ thoáng, không chui góc/khe tường; sạp cháy hàng vẫn mua được sau kỳ châm kế; Dã Tẩu thấy `NHAN nhiem vu loai 4` → `godatau` → `nhat cuon (n/8)` → `TRA XONG` → `chon ruong thuong`.


## 12. PHIÊN 19/08 ĐÊM: NEO TỰ SỬA + VỀ THÀNH/THÔN + SẠP 8-12 MÓN + LINK XANH (đợt vá 2, sau restart 19:20)

**5 việc chủ game giao (đêm) + 3 việc nhắn bổ sung:** (1) kiểm Kim Phong không add lặp; (2) kiểm ngựa Túc Sương add 1 lần; (3) kiểm hàm xóa skill Thiên Vương thừa kế; (4) bot VẪN đứng ngoài map — điều tra kỹ hơn; (5) hàm lệnh bài gọi toàn bộ bot về thành/thôn chia đều; (6) bot vẫn dồn đông một chỗ; (7) sạp bày nhiều món hơn; (8) item post kênh thế giới phải là đồ xanh opt 1-5. Phản biện: 2 agent độc lập (logic hệ / vật phẩm-hiệu năng), bắt 1 lỗi chặn + 3 rủi ro, tất cả đã đóng trước deploy.

### Kết quả 3 việc KIỂM TRA (mã + log sau restart 19:20)
1. **Kim Phong**: đợt 8 (`32c43c99`, trước phiên này) đã sửa đúng gốc — phép thử `GetGoldId() == 177..185` (so id thật, không phải cột m_nSet=36). Log sau restart: 65 dòng/57 bot = chỉ mặc món thiếu, **không còn add lặp** (hiện tượng 26.395 dòng chủ game từng thấy là của DLL cũ 17:42).
2. **Túc Sương**: phép thử `detail==equip_horse && parti==2 && level>=10` trên ngựa đang mặc — log mỗi bot đúng 1 lần. **Đạt**.
3. **Xóa skill thừa kế**: hàm CÒN — `RemoveAllSkill()` [KPlayerBot.cpp ~dòng 830] chạy cho mọi blob **mới** (taobot_bdb) hoặc blob thiếu nSect hợp lệ; bot <10 chưa vào phái lưu rồi nạp lại vẫn bị quét sạch (nSect=-1 → coi là bot mới). **Khe còn lại**: bot ĐÃ vào phái mà blob nhiễm skill Thiên Vương từ khe 18/08 (lưu giữa lúc vá vòng nạp và lúc thêm RemoveAllSkill) thì nạp lại KHÔNG tự sạch — nếu còn thấy phái khác múa chiêu đao TV thì đó là bot lưu trong khe đó; cách sạch: gỡ tạo lại 1000 con.

### Điều tra "vẫn đứng ngoài map" — thủ phạm thật: NEO BÃI HỎNG
Pháp y log sau restart: PhamVu342 được `[BotCuu]` thả về đúng tọa độ neo Lão Hổ Động (1702,3350) rồi 49 giây sau "ket dao A*" tại (1710,3345) → cứu về lại chỗ cũ — **vòng lặp cứu-tại-chỗ mỗi 60s**, bot đứng im giữa vùng đen = cảnh chủ game thấy. Gốc: một số **tọa độ neo trong s_bai được chọn tay TRƯỚC khi lưới 19/08 sơn đặc region thiếu dữ liệu** — nay chúng nằm trong/cạnh đảo lưới (Hoành Sơn Phái 21 lượt cứu/17ph, Trường Bạch 11, Lão Hổ Động 10).

### Đã sửa (KPlayerBot.cpp + KSubWorld.cpp + ScriptFuns.cpp + simcity_admin.lua)
1. **NEO TỰ SỬA `pb_LayNeo`**: lần đầu dùng neo mỗi bãi, đo độ liên thông bằng BFS cửa sổ 27×27 (`pb_SapLoang` giờ trả số ô); neo loang <60 ô → quét xoắn ốc 2 vòng: **vòng 1 (r≤12) đòi ứng viên CHẠM ĐƯỢC ô gốc/4 lân cận** trong bản đồ loang của nó (chống bốc nhầm "vùng trống giả" void loang mênh mông — phản biện chỉ đúng); vòng 2 (r≤40) chỉ đòi ≥60 + log `DUYET TAY`. Cache cả đời server. `pb_RaBai` + `[BotCuu]` + xích đều đi qua neo-đã-kiểm. Sau restart grep `[BotNeo]` để duyệt neo bị đổi.
2. **Xích MAP NHIỆM VỤ loại 4** (lỗ mở ra khi nâng trần cuộn): neo = điểm Xa Phu thả (`nDtNeoMap/X/Y`), xích riêng **PB_XICH_NV 3200/3520** (rộng hơn bãi — quái rơi cuộn rải khắp map, 2000 dễ "farm 20 phút không ra cuộn"). Theo dõi tần suất dòng đó sau deploy.
3. **Di tản ưu-tiên-chỗ-vắng** (trả lời "bot vẫn dồn đông"): `pb_FindRoamSpot` thêm `bUuTienVang` — nhánh đếm-bot-ít-nhất (sẵn có của đội trưởng) giờ dùng cho `[BotDan]` thay vì bốc ngẫu nhiên (trước hay trúng điểm ngay cạnh đám đông, di tản xong vẫn đông nguyên).
4. **CHẾ ĐỘ VỀ THÀNH/THÔN** (yêu cầu #5): menu lệnh bài mới "Goi het bot ve THANH-THON: BAT/TAT" → `PB_SetVeThanh` (Lua đăng ký ScriptFuns.cpp). 15 map đích = 10 thành s_dtNpc + 5 thôn (99 Vĩnh Lạc, 100 Chu Tiên, 101 Đạo Hương, 153 Thạch Cổ, 174 Long Tuyền — tọa độ từ node simcity). Chia đều `nLech % 15` (~67 bot/map), teleport **so le 60 giây**, rải ô lệch 9×7, tới nơi cưỡi ngựa (thành) + đi dạo quanh tâm ≤18 ô. Sạp giữ nguyên sạp; Dã Tẩu tạm ngưng KHÔNG mất nhiệm vụ; hồi sinh + lưu định kỳ vẫn chạy. TẮT là bot tự về bãi. **Phản biện bắt kịp trước deploy: map 20 + 121 chưa từng có lưới A*** (2/15 đích VÀ 2/10 thành nhà Dã Tẩu — bệnh "không tới được NPC thành 20/121" có sẵn từ trước!) → đã thêm 20,121 + 5 thôn vào `IsBotPathMap` (boot đầu dựng 7 lưới mới, tốn vài giây).
5. **Sạp 8-12 món** (yêu cầu #7): `nMuon = 8 + (nLech+nDot)%5`. Đo thật: cửa sổ sạp client theo ô túi 6×10=60, gói VIEW_ITEM_SYNC trần 60 món → 12 an toàn tuyệt đối; 12 món trang sức chỉ tốn ~20/60 ô túi. Vá kèm **nhánh túi-đầy ảo**: `InsertEquipment` túi đầy không báo lỗi mà nhét vào pos_hand + VỨT món đang cầm xuống đất → giờ món tới tay là hủy + dừng đợt.
6. **Link kênh thế giới = đồ XANH opt 1-5** (yêu cầu #8): `pb_TaoLinkDo` rải `nMagicLevel` cấp 1..5 (trước để trống = đồ trắng), tự kiểm màu — không xanh thì bỏ link (câu rao vẫn gửi). Client dựng lại item từ seed+nMagicLevel trong token (cùng pipeline sạp đã chạy thật) → người chơi bấm link thấy đúng đồ xanh.
7. **Phá "chữ ký bot" của RNG đóng băng**: mọi phép gieo trong link/sạp/đi-dạo giờ trộn biến đếm/chỉ số (không thì cùng-giây-cùng-giá-trị làm series ≡ cấp-opt mãi mãi, số opt chỉ {1,6}, cả đàn đi dạo chéo 45° dồn cục).

### Ghi sổ (chưa sửa, có chủ đích)
- `nDtNeoMap` không reset giữa 2 nhiệm vụ cùng map → xích neo quanh điểm thả CŨ (bán kính 3200 đủ rộng, rủi ro thấp).
- Đổi neo bằng vòng-2 (không chạm gốc) có thể vẫn rơi vào void → log có chữ `DUYET TAY`, chủ game soi bằng `fp_view.py` trước khi tin.
- Chú ý sau restart: bot Dã Tẩu thành nhà 20/121 hết bệnh "nghỉ 5 phút xoay vòng" (lưới mới) — grep `KHONG toi duoc` để xác nhận.

### Nghiệm thu sau restart
```
grep -a "\[BotNeo\]" bot.log                      # neo nào bị đổi, có dòng DUYET TAY không
grep -a "\[BotVeThanh\]" bot.log | awk '{print $NF}' | sort | uniq -c   # đủ 15 map, ~67 con/map
grep -ac "\[BotCuu\]" bot.log                      # phải GIẢM mạnh so 62/17ph
grep -a "farm 20 phut khong ra cuon" bot.log | wc -l   # xích NV có bóp nghẹt loại 4 không
grep -a "\[BotSap\] .*mon trang suc" bot.log | tail    # 8-12 món/sạp
```
Bấm menu: BOT người chơi → "Goi het bot ve THANH-THON: BAT" — quan sát thành/thôn đông dần trong ~60 giây; TAT là bot về bãi đánh quái lại.


## 13. PHIÊN 20/08 RẠNG SÁNG: BỎ "BAY TỰ CHẾ" — BOT LÊN BÃI BẰNG ĐƯỜNG XA PHU (deploy 03:00, chờ restart)

**Chủ game giao (5 tin nhắn):** bot lúc lên map "bay thẳng ra ngoài map" vì teleport tọa độ tự viết → làm lại: **muốn lên map luyện công thì về thành → đi bộ tới Xa Phu → lên map bằng cấp** (tọa độ có sẵn trong item Thần Hành Phù); lên cấp giữa bãi thì "phù về thành" rồi đi lại; >90 luyện map 90; thêm dòng menu "lên bản đồ luyện công 20-90" ở Xa Phu cho cả người chơi; chia đều map cùng cấp; chế độ Về-Thành phải **tự lưu rương** để sau phù về đúng thành đã bị gọi về.

### Đã làm
1. **`station.lua` (live + repo)**: thêm khối `BOT_LC` — 21 map luyện 20→90, tọa độ **chép nguyên văn từ `shenxingfu.lua` TRAIN_ARRAY1/2** (dòng dạng waypoint đã tra qua `settings/WayPoint.txt`) + `LuyenCongFun` (menu người chơi) + `sellc` + `botlc_go(n)` (bot gọi). Khuôn y `selluyen1`: kiểm cấp → `NewWorld` → `SetFightState(1)` + `SetProtectTime` + `AddSkillState(963)`. **Phát hiện kèm: 3 dòng "Sa Mạc 1/2/4" của THP thật trỏ waypoint 226/227/228 sai/không tồn tại — menu gốc đang hỏng 3 dòng đó** (không đưa vào BOT_LC).
2. **`xaphu.lua` (live + repo)**: menu chính thêm "Len ban do luyen cong (20 - 90)/LuyenCongFun" (Say 9→10 mục).
3. **`s_bai` C++ thay bằng 22 bãi** có tuyến chính thống (Hoa Sơn = tuyến `go_HSBattle` sẵn có + 21 map THP; mỗi mốc 20-80 có 2 map, mốc 90 có 7 → `pb_ChonBai` ít-bot-nhất chia đều sẵn; >90 tự luyện mốc 90). **Loại 11 bãi cũ tọa độ tay** (79, 7, 193, 170, 21, 167, 182, 164, 206, 198, 181, 875, 225-227, 144, 152) — không còn đường bay tự chế. `s_baiLc[]` ánh xạ 1-1 sang `BOT_LC` (đã đối chiếu máy: 21/21 dòng khớp).
4. **`pb_RaBai` = máy 2 pha**: không ở thành → **"phù về thành nhà"** (khuôn `pb_DtVeThanh`, cửa sổ so le 60s, thành nhà động `pb_ThanhNha`); ở thành → **đi bộ tới NPC Xa Phu** (`pb_TimNpcNho "xaphu"` — khuôn Dã Tẩu) → `ExecuteScript station.lua botlc_go/go_HSBattle` như người bấm menu → đáp điểm chuẩn → bước-ra + log `[BotBai] ... bang duong XA PHU`. Đếm `nRaBaiThu/nRaBaiGoi`; quá 700 nhịp/5 lần gọi → **fallback teleport neo-đã-kiểm** (log `[BotXe] BO TAY` — không bao giờ chết đứng).
5. **Chế độ Về-Thành lưu rương**: đáp map nào là `SetRevivalPos(map, revId)` (bảng `s_veThanh` + cột `nRevId` từ `RevivePos.ini`; map 53→19 khớp giá trị hệ lưu đang dùng) **+ tự chép sang DEATH-pos** (SetRevivalPos chỉ ghi LOGIN-pos, chết đọc DEATH-pos vốn chỉ chép lúc nạp — không tự chép thì phải đợi restart mới ăn) + ghi `b.nThanhNhaMap` (nếu là 1 trong 10 thành có Xa Phu) → "phù về thành" từ đó dùng đúng thành đã bị gọi về; 5 thôn vẫn lưu rương (chết hồi sinh tại thôn) nhưng phù-về dùng thành cũ.
6. **Chặn spam console** `player Packing world sync data failed...` (đợt trước cùng đêm, commit `90a9d6e3`): guard `nClient < 0` đầu `KSubWorld::SendSyncData` — bot đổi map hàng loạt không in nữa, hành vi không đổi (cả 2 caller vứt giá trị trả về).

### Phản biện
Agent phản biện đợt 3 chết giữa chừng (hết hạn mức API) → tự phản biện thủ công 10 câu: build 0 lỗi (thứ tự khai báo OK), bảng Lua↔C++ so máy khớp, `ExecuteScript(file,func,int,bool)` là khuôn `write_log_tax` đã chạy prod, menu Say đếm đúng 10, bot đứng bãi-bị-loại chỉ đứng yên chờ phù (pb_RaBai trả 0 chặn khối đánh — không lang thang void), nhóm theo captain tới Xa Phu rồi tự tách, `pb_TimNpcNho` có cache theo map (rẻ), key rương map 1 tồn tại, hồi sinh nằm TRƯỚC mọi return trong pb_DriveBot.

### Nghiệm thu sau restart
```
grep -a "\[BotXe\]" bot.log | head -30        # phu ve thanh + gap Xa Phu + BO TAY (ky vong ~0 BO TAY)
grep -a "bang duong XA PHU" bot.log | wc -l   # so bot len bai dung duong
grep -a "\[BotNeo\]\|\[BotCuu\]" bot.log       # ky vong giam manh (toa do THP chuan)
```
Mắt thường: bot chết hồi sinh ở thành → chạy bộ tới Xa Phu → biến mất → hiện ra ở bãi đúng cấp; người chơi thật bấm Xa Phu thấy menu mới "Len ban do luyen cong (20 - 90)". Bot cũ đang đứng ở 11 bãi bị loại sẽ lần lượt phù về thành trong ~1 phút đầu rồi tỏa đi xe.
