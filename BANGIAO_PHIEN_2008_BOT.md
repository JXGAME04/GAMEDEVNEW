# BÀN GIAO PHIÊN 19/08 TỐI → 20/08 RẠNG SÁNG — HỆ BOT KPlayer (xích bãi → đường Xa Phu → skill 90)

> **ĐỌC TỆP NÀY TRƯỚC KHI GÕ dòng nào cho hệ bot.** Sổ tay toàn dự án:
> `TIENTRINH_SIMCITY_BOT.md` (phiên này viết mục **11, 12, 13** — chi tiết từng đợt nằm ở đó,
> tệp này là bản chỉ huy). Phiên chiều cùng ngày: `BANGIAO_PHIEN_1908_CHIEU.md`.
>
> Phiên gồm **5 đợt vá** (`3a8265d4` → `90181bd3`), tất cả build xanh + push + DLL đã nằm
> `E:\...\bin\server`. **⚠️ SERVER CHƯA RESTART với các bản sau 19:20** — việc ĐẦU TIÊN
> của phiên sau là nghiệm thu sau restart (mục 4 dưới).

---

## 1 · TRẠNG THÁI LÚC BÀN GIAO (20/08 ~03:30)

| Thứ | Trạng thái |
|---|---|
| `bin\server\CoreServer.dll` | md5 `3489c90efaf655811d371fbb230ffdd4` (03:2x) — chứa TOÀN BỘ 5 đợt + F11 server |
| GameServer đang chạy | bản restart **19:20 19/08** = chỉ có ĐỢT 1 (xích bãi). Mọi thứ từ đợt 2 trở đi **chờ restart** |
| Bản lùi DLL (xếp lớp) | `.bak_1908_1824` (F11) → `_1910` (xích) → `_syncfix` → `_2008_0300` (Xa Phu) → `_2008_031x` (skill90 bản show_kynang90) |
| Commit cuối | `90181bd3` (đã push main) |
| Lua LIVE đã sửa | `script/global/station.lua` (+BOT_LC/botlc_go), `script/global/npcchucnang/xaphu.lua` (+menu), `script/global/hocvocong.lua` (+bot_hoc90), `script/item/simcity_admin.lua` (+menu Về-Thành) — **tất cả đã mirror vào `serverscript_jx2/jx1_edits/`** (2 bản giống hệt, giữ luật này) |
| bot.log | vẫn tệp từ 17:42 19/08 (restart không xoá); nên đổi tên chốt mốc trước restart tới |
| Phiên song song | F11 TaskGuide + WAuto Dã Tẩu commit xen kẽ cùng repo — **grep chuỗi trong DLL, đừng so mtime**; coi chừng họ `git add` ké file mình đang sửa dở (đã xảy ra 1 lần) |

## 2 · NĂM ĐỢT VÁ (chi tiết = TIENTRINH mục 11-13)

| Commit | Nội dung | Trạng thái |
|---|---|---|
| `3a8265d4` | **Xích bãi 2000/2320** (map 79 void=đi-được cả 2 lớp dữ liệu) + `[BotLach]` kiểm lưới + sạp ô-tốt BFS 27×27 + `pb_ChamHangSap` làm mới trang sức 15p so le + dọn hàng khi đóng sạp + DT cấp 70 + trần cuộn 8 (loại 4 hết mã-chết) | **ĐÃ CHẠY THẬT** 19:20 — log xác nhận `[BotXich]` hoạt động, loại 4 được nhận 8-cuộn map 21/122 |
| `90a9d6e3` | Chặn spam console `Packing world sync data failed` (guard `nClient < 0` đầu `KSubWorld::SendSyncData` — 2 caller đều vứt giá trị trả về) | chờ restart |
| `edd85f89` | **Neo tự sửa** `pb_LayNeo` (BFS, vòng-1 đòi chạm-ô-gốc chống trôi vào void) + xích map-nhiệm-vụ 3200 + di tản ưu-tiên-chỗ-vắng + **chế độ VỀ THÀNH 15 map** (menu lệnh bài) + sạp 8-12 món + link kênh TG đồ xanh opt 1-5 + vá túi-đầy-pos_hand + trộn RNG + IsBotPathMap +7 map (99/100/101/153/174/20/121) | chờ restart |
| `cd327bdf` | **BỎ "bay tự chế" — đường XA PHU**: s_bai = 22 bãi tọa độ Thần Hành Phù; `pb_RaBai` 2 pha (phù về thành nhà → đi bộ tới NPC Xa Phu → `ExecuteScript station.lua botlc_go` như người bấm menu; fallback teleport khi thu>700/gọi>5); menu Xa Phu mới "Len ban do luyen cong (20 - 90)" cho cả người chơi; Về-Thành **lưu rương** `SetRevivalPos` + tự chép DEATH-pos + thành-nhà-động `nThanhNhaMap` | chờ restart |
| `c43e328d`+`90181bd3` | **Skill 90**: bot cấp ≥80 gọi `bot_hoc90(m_nCurFaction+1)` (hàm riêng trong hocvocong.lua — **CHỈ bộ 90**, không kèm skill 120 + chiêu 210 như hàm NPC gốc, theo chốt của chủ game) | chờ restart |

## 3 · LỖI GỐC / TRI THỨC ĐẮT TIỀN TÌM RA PHIÊN NÀY (đừng để mắc lại)

1. 🔴 **Dữ liệu vật cản nhiều map ghi vùng-void = 0 = "đi được" ở CẢ engine LẪN lưới A*** (map 79 chỉ vẽ tường mê cung) — không rào tọa độ nào bắt được; quái cũng lang thang ra đó và roam-theo-quái kéo bot theo. Trị bằng luật sản phẩm (xích) + đường đi chính thống (Xa Phu), không trị được bằng dữ liệu.
2. 🔴 **Tọa độ neo tay trong `s_bai` cũ nằm trên đảo lưới** → `[BotCuu]` thả về đảo rồi cứu lại đúng chỗ mỗi 60s (PhamVu342/Lão Hổ Động). Từ nay MỌI điểm đáp phải là tọa độ script chính thống (THP/waypoint/go_HSBattle).
3. 🔴 **`SendSellItemCount` TỰ hạ `m_BaiTan = 0`** khi khách bấm sạp hết hàng (KPlayer.cpp:10371) → code mở sạp phải set `m_BaiTan = 1` MỖI kỳ châm hàng, không chỉ lúc mở.
4. 🔴 **`FindPathServer` ca không-có-đường loang toàn thành phần liên thông** (map 1 = 777k ô) — CẤM dùng làm probe connectivity hàng loạt; dùng BFS cửa sổ trần cứng (`pb_SapLoang` 27×27).
5. 🔴 **`InsertEquipment` khi túi đầy KHÔNG báo lỗi**: nhét vào pos_hand + VỨT món đang cầm xuống đất thành Object công khai; `FindSame` vẫn thấy đồ ở tay → nhánh kiểm `q2<=0` là ảo. Kiểm `nPlace != pos_equiproom` sau FindSame.
6. 🔴 **`g_Random` đóng băng theo giây + cùng-nMax-cùng-giá-trị**: 2 lần gọi cùng khung trả CÙNG số → series ≡ cấp-opt mãi mãi, số opt bimodal {1,6}, đi dạo chéo 45° dồn cục. Mọi phép gieo phải trộn biến đếm/chỉ số với hệ số nguyên tố khác nhau từng trục.
7. 🔴 **`SetRevivalPos` chỉ ghi LOGIN-pos; CHẾT đọc DEATH-pos** (chỉ được chép từ login lúc NẠP nhân vật) — muốn hiệu lực ngay phải tự chép `GetDeathRevivalPos()`.
8. 🔴 **`KSkillList::Add` không bao giờ HẠ cấp chiêu** (chỉ nâng khi cao hơn, KSkillList.cpp:434-447) → AddMagic lặp an toàn.
9. 🔴 **Thần Hành Phù thật đang HỎNG 3 dòng menu "Sa Mạc 1/2/4"** (trỏ waypoint 226/227/228 — 226 là map 79, 227/228 không tồn tại trong WayPoint.txt). Người chơi bấm 3 dòng đó đi sai/không đi được — VIỆC PHIÊN SAU nếu chủ game muốn sửa cho người thật.
10. 🔴 **Hai bản `RevivalID.ini` chỉ là bảng tra cho script; nguồn thật = `settings/RevivePos.ini`** (key hợp lệ per map; map 53 → 19 khớp hệ lưu bot).
11. 🔴 **Loại 4 Dã Tẩu là mã chết với trần cuộn <8** ở MỌI cấp (levellink: 50-79→bậc 6 nhẹ nhất Num=8; 80+→bậc 11 chỉ Num=15/Mật Chỉ). `PB_DT_CUON_TOI_DA` hiện = 8, đừng hạ.
12. 🔴 **Bẫy bash-inline TÁI PHẠM** (backtick trong python inline bị command-substitution nuốt → doc hỏng đã vá `e2c9cd68`): chuỗi đặc biệt LUÔN qua Write tool → chạy file, kể cả "đoạn text ngắn thôi".
13. Map 20 + 121 xưa nay không có lưới A* (2/10 thành nhà Dã Tẩu!) — đã thêm; bệnh "không tới được NPC thành 20/121 nghỉ 5 phút" của DT sẽ tự hết sau restart.

## 4 · VIỆC PHIÊN SAU — NGHIỆM THU SAU RESTART (theo thứ tự)

1. Đổi tên `bot.log` chốt mốc → nhờ chủ game **restart GameServer** → Gọi 1000 → BẬT đánh quái → bấm Dã Tẩu (100-200) + bán sạp (20-50). Boot đầu chậm thêm vài giây (dựng 9 lưới map mới).
2. Chạy ≥30 phút rồi soi:
```bash
cd "E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
grep -a "\[BotXe\]" bot.log | head -40          # phù về thành + gặp Xa Phu; "BO TAY" phải ≈ 0
grep -ac "bang duong XA PHU" bot.log             # số bot lên bãi đúng đường (kỳ vọng tăng dần → nghìn)
grep -a "\[BotSkill90\]" bot.log | head          # bot 80+ học CHỈ 90 (~650 con)
grep -a "\[BotNeo\]" bot.log                     # neo bị đổi (kỳ vọng ÍT vì tọa độ đã là THP); dòng DUYET TAY → soi tay bằng fp_view.py
grep -ac "\[BotCuu\]" bot.log                    # kỳ vọng GIẢM MẠNH so 62/17ph của bản cũ
grep -a "\[BotDT\]" bot.log | grep -a "TRA XONG\|ruong thuong" | head   # trọn vòng loại 4
grep -a "\[BotSap\]" bot.log | tail -20          # 8-12 món; "lam moi sap" sau 15-16p; [BotSapLoi] = 0
```
3. Bấm menu lệnh bài "Goi het bot ve THANH-THON: BAT" → `[BotVeThanh]` đủ **15 map** + "luu ruong (rev N)"; TẮT → bot tự phù về thành rồi đi Xa Phu ra bãi. Kiểm 1 bot chết ở bãi → hồi sinh đúng thành đã bị gọi về.
4. Người chơi thật bấm NPC Xa Phu → thấy mục mới "Len ban do luyen cong (20 - 90)" hoạt động.
5. Mắt thường: KHÔNG còn bot đứng/di chuyển ngoài rìa map (mọi điểm đáp giờ là tọa độ THP); bot chết ở thành tự chạy bộ tới Xa Phu rồi biến mất lên bãi.

## 5 · NỢ / RỦI RO GHI SỔ (chưa sửa, có chủ đích — cân nhắc phiên sau)

| # | Việc | Ghi chú |
|---|---|---|
| 1 | Khách mở cửa sổ sạp đúng lúc làm mới → mua trúng món mới cùng khe pool (LIFO reuse) | server tự nhất quán tiền-đồ; muốn đóng hẳn: push `SendSellItemInfo(TRUE)` cho player quanh region sau châm |
| 2 | Rò 1 slot ItemSet/lượt mua sạp — lỗi engine `c2sTradeBuy` có sẵn (server không `ItemSet.Remove`) | pool 1M, vô hại thực tế |
| 3 | `nDtNeoMap` không reset giữa 2 nhiệm vụ CÙNG map → xích quanh điểm thả cũ | bán kính 3200 đủ rộng |
| 4 | Sửa 3 dòng Sa Mạc hỏng của Thần Hành Phù cho NGƯỜI CHƠI | shenxingfu.lua TRAIN_ARRAY2 dòng {90,226/227/228} |
| 5 | Bot nhiễm skill Thiên Vương lưu trong khe 18/08 không tự sạch | thấy phái khác múa chiêu đao TV thì gỡ tạo lại 1000 con |
| 6 | `KNpc.cpp:8241` `pnMagicLevel[6]` đọc ngoài biên (đường rớt đồ QUÁI — ảnh hưởng người thật) | đợt riêng |
| 7 | Nợ engine cũ: `Mps2Map` chia số âm, `ServeJump` trôi offset | ảnh hưởng người thật, đợt riêng |
| 8 | Goddess `RemoveLogProc` checkpoint 1h→10p đã sửa nguồn, chưa build/deploy | từ phiên 18/08 |
| 9 | Bãi cũ bị loại (79/181/875/225-227/170/182/164/206/198/7/193/21/167/144/152) — nếu chủ game muốn giữ bãi nào thì phải tìm tuyến chính thống cho nó (thêm dòng THP/waypoint) rồi thêm lại s_bai + BOT_LC **đồng bộ cả 2 bảng** | thứ tự BOT_LC[2] (lua) phải khớp s_baiLc (C++) |
| 10 | Agent phản biện đợt 3 chết do session limit API — đợt Xa Phu mới chỉ TỰ phản biện, chưa có mắt độc lập | nếu sau restart có hành vi lạ ở pb_RaBai, soi lại 2 pha + fallback trước tiên |

## 6 · CẠM BẪY VẬN HÀNH (nhắc lại nhanh)

- Build: `MSBuild Core.vcxproj -p:Configuration="Server Release" -p:Platform=x64 -p:SolutionDir="D:\GAMEDEVNEW\Sources\"`; deploy = rename bản cũ rồi copy + md5 + **grep chuỗi mới trong DLL**.
- `KPlayerBot.cpp` ASCII/LF (Edit tool OK); `KSubWorld.cpp`/`ScriptFuns.cpp`/lua = TCVN3 → python latin-1, đếm high-byte trước/sau.
- Lua live sửa xong PHẢI mirror `serverscript_jx2/jx1_edits/` (2 bản giống hệt).
- Đường dẫn script trong C++ viết THƯỜNG tuyệt đối; tên HÀM lua giữ nguyên hoa/thường.
- `ExecuteScript(file, func, int, bool)` = khuôn truyền 1 số vào hàm lua (write_log_tax/botlc_go/bot_hoc90 đều dùng).
- Xích/neo: mọi nơi cần "điểm bãi" phải qua `pb_LayNeo` (đừng đọc `s_bai[].nOX` thô); mọi phép gieo mới phải trộn biến đếm.
