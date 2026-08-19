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
