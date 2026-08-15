# BÀN GIAO ĐỢT E — CÔNG THÀNH CHIẾN (14/08/2026, phiên hoàn tất E3→E7)

> Nối tiếp `BANGIAO_CONGTHANH_DOTE_DODANG.md` (E1-E2). Phiên này làm **E3→E7 + phản biện từng giai đoạn**,
> mỗi fix đều điều tra đối chiếu bản gốc Linux (`D:\ServerLinux`) trước khi gõ mã, đúng lệnh chủ game.
> Spec chuẩn: `KEHOACH_CONGTHANH_DOTE.md` + `DIEUTRA_CONGTHANH_BINARY.md` + `DIEUTRA_LEAGUE_SPEC.md`
> + **`PHULUC_HAM_CONGTHANH.md` (MỚI — bảng đích danh ~110 hàm, tick khi phản biện)**.

## 0. CHUỖI COMMIT PHIÊN NÀY (sau `69f3c57` bàn giao dở dang)

| Commit | Nội dung |
|---|---|
| `3b5a92b` | **E3** KJx2CityWar nhóm CITY 7 thành (18 hàm, mirror `settings\jx2citywar.txt`, sync KSubWorld mọi instance) + de-hardcode map 78 (2 setter + KPlayer::GetCityOwnTong) + vá 5 phát hiện phản biện E3 + vá 5 phát hiện review KJx2League (nợ III.1 — 0 CHẶN) |
| `60af727` | **E4** engine trận: BT_ 19 + Title_ 7 + Arena idle 11 + ArenaCredits(task 2894) + hạ tầng ~34 hàm (GiveBox native, string-builder, obstacle động, MissionS riêng...) — **đã hấp thụ 6 CHẶN + 3 CAO của phản biện E4** |
| `683cfaf` | **E5** chép 59 script gốc + vá tối thiểu (MISSIONID 6→7/5→9, item 1499→**1508**, BT type1 751→**2895**, shim add_citybonus_task, IL→IncludeLib) + nối dây JX1 (xa phu menu, station_ctc, boot league) |
| `948d7b4` | **E6** đồng hồ 5 pha `timerserver_ctc.lua` + bản cutover `timerserver_CUTOVER.lua` + đổi chủ thành trọn gói (Title Thái Thú 152+city + field 48) + 4 nút GM ép pha |
| `8f1cf7e` | **E7** đặt thuế Thái Thú qua thoại NPC quản thành (0 protocol mới; gate y gốc 22h-23h/trần 20/1 lần-ngày) |

Build PASS xuyên suốt: Core `Server Release|x64` + Core `Client Release|Win32` + GameServer `Release|x64`.
Binary đã deploy `bin\server\Coreserver.dll` + `bin\client\CoreClient.dll` (post-build tự chép).
**GameServer.exe / S3Relay.exe / Game.exe KHÔNG đổi** — đợt E không đụng protocol client.

## 1. E8 — THỦ TỤC CUTOVER (chủ game tự chạy khi muốn bật)

1. **Restart GameServer (+Relay như thường lệ)** — binaries mới đã nằm sẵn ở `bin`. Sau restart:
   - Kiểm `ScriptError.log` sạch; console có dòng `[citywar] boot: league 4/508/509 + GlbMission 8 OK`.
   - NPC test (`script\test\citywar_e3.lua` — gắn vào NPC Hỗ Trợ Test): "Xem 7 thành" phải ra đủ 7 tên.
   - Bang hội đợt 12 vẫn chạy (bảo trì 06h05, phường, kick, chiêu mộ).
2. **Thả đồng hồ**: chép `serverscript_jx2\citywar_e\timerserver_CUTOVER.lua` → `bin\server\script\timerserver.lua`
   (hiệu lực **tức thì** ~1 phút, KHÔNG cần restart lại — vì vậy chỉ thả SAU khi GS đã chạy bản mới).
   ⚠ KHÔNG thả file này khi server còn chạy CoreServer CŨ (tick sẽ nil-call mỗi phút).
3. Test ép pha bằng NPC test (đứng trong 1 thành): ÉP 18h (mở báo danh) → nộp Khiêu chiến lệnh ở NPC
   Tân Thủ Thôn (infocenter) → ÉP 19h (bốc thăm: vô chủ → bổ nhiệm ngay; có chủ → khiêu chiến) →
   ÉP 20h (khai chiến — timer 18 poll ≤5' sẽ OpenMission 7, map 221 dựng trụ/cổng/NPC) →
   đánh + kết trận cả 2 chiều → ÉP 0h (dọn). Sau restart giữa chừng: state ĐANG-ĐÁNH tự hạ (boot demote).

## 2. LỊCH VẬN HÀNH (khi chạy thật — múi giờ máy GS)

- **0h00** dọn league 508/509 + trạng thái sót · **18h00-18h59** mở báo danh thành theo lịch (hạ bảng ladder 10261)
  · **19h** chốt: bốc thăm bang nộp NHIỀU Khiêu chiến lệnh nhất (đồng điểm → random; điểm = member-task league 508);
  thành VÔ CHỦ → Thái Thú NGAY không cần đánh · **20h** khai chiến thành có lịch (90 phút, map 221) · kết trận →
  đổi chủ + Title Thái Thú + field 48.
- Lịch tuần (`%w`: 0=CN): PT **T4/T5** · TĐ **T2/T3** · ĐL **T3/T4** · BK **T6/T7** · TD **T5/T6** · DC **CN/T2** · LA **T7/CN**.
- Báo danh: bang cấp ≥18, ≥37 người (citywar.ini); nộp **Khiêu chiến lệnh (6,1,1508)** tại NPC infocenter —
  kéo item vào Ô GIAO VẬT PHẨM (hộp GiveBox), +50000 exp/lệnh, trần 300 lệnh/ngày/người, kho bang trần 2 tỷ.
- Thuế: Thái Thú thoại **bia Quản Thành** (citybulletin — Tương Dương có sẵn `addobjtuongduong`) → menu 0/5/10/15/20%,
  chỉ **22h-23h**, 1 lần/ngày. Thuế + tên chủ thành áp vào MỌI instance của map thành.
- Thưởng: top-10 điểm trận +3.000.000 exp + 5 người ngẫu nhiên nhận Lễ hộp (camper GameOver);
  thưởng tuần huyền tinh (T2 9h-18h thành viên, 18h-20h bang chủ phần dư) đi qua NPC Kim Sơn chưởng môn
  (citywar_function `take_tong_*award` — cần bang THẬT chiếm thành trước).
- Dụng cụ công thành: mua bằng **Kim Nguyên Bảo (event item 343)** → 4 đạo cụ (2,1,30-33); dùng TRONG map 221,
  đúng phe, gần vị trí; lệnh bài công/thủ 14 loại (event item 354-367) bán ở infocenter cho liên minh.

## 3. DEVIATION CÓ CHỦ ĐÍCH (đối chiếu gốc — tất cả đã ghi chú tại chỗ trong mã)

1. **E7 thuế qua THOẠI** thay cửa sổ 0xA3/KUiCityManage (client ta không hiểu gói JX2; giữ đúng Q3 "không protocol mới").
   Muốn cửa sổ thật: làm theo khuôn KEHOACH E7 (UIInfo + page 8 + COP 38) ở đợt sau.
2. **Item id remap**: Khiêu chiến lệnh 1499→1508 (magicscript.txt cây ta); BT type1 751→2895 (751 = điểm songjin);
   ArenaCredits 3179→2894 (MAX_TASK 3000).
3. **4 lỗ thủng CÓ SẴN của binary gốc** được vá thật (script gốc gọi nhưng binary KHÔNG đăng ký — chứng minh trong
   PHULUC mục 4): ForbidChangePK + DisabledUseTownP (no-op nhận đối — chưa cưỡng chế), PushString/PopString (làm thật),
   IL("TITLE") trong titlefuncs (đổi IncludeLib). Ở bản gốc các lời gọi này CẮT ĐUÔI hàm đang chạy.
4. **MissionS tách kho riêng** (JX1 dùng chung mảng số+chuỗi — SetMissionV(1) đè mất tên bang thủ; GetMissionS
   đăng ký lần 2 đè bản cũ — tiền lệ THẤP-7).
5. Vật cản động: CHẶN Ô THẬT theo đường chéo + khôi phục giá trị cũ khi Clear; **không vẽ hình + client không được
   báo** → có thể giật kéo-về ở rìa rào (test E8 để ý); playerdeath đổi `OnDeath(Launcher)` → `OnPlayerDeath(victim, killer)`
   (engine JX1 gọi tên/đối khác gốc); `AddSkillState(661,5,0,0)` lúc rời trận → `RemoveSkillState(661)` (0 = vĩnh viễn trên JX1).
6. GetItemLife trả 0 khi có item → lệnh bài mua **không tự hết hạn 5 ngày** (item ta không lưu tuổi — có lợi người chơi).
7. Tick lỡ CẢ GIỜ pha (server sập 18h-19h) = bỏ pha đó tuần đó (relay gốc cũng vậy khi sập).

## 4. CHECKLIST TEST THÊM (ngoài mục 1 — từ các sàn phản biện)

- Nhánh sống dậy trong hệ bang: bảo trì bang có tổng đàn cấp 5 + menu cột totem (`tong_setting/city_totempole` giờ
  gọi được GetCityOwner/GetCityAreaName) — xem log sạch (phản biện E3 #3).
- Đứng cạnh rào chắn khi cổng chưa vỡ: có bị giật kéo về không (VỪA-3).
- Xe công thành/NPC kết liễu CỔNG: doordeath có chạy không (JX1 chỉ kích khi NGƯỜI kết liễu — VỪA-5; xe hiện là
  NPC thường không AI, tham số đã lưu).
- Rời trận bằng xa phu (chefu 222/223) + chết trong trận: điểm PK/liên trảm/rank lên bảng, GameOver thưởng đúng.
- 2 thành 37/176 lần đầu mở: NPC/spawn có thể trống (đã báo từ E1); bia Quản Thành mới có ở Tương Dương —
  các thành khác cần AddObj tương tự nếu muốn đặt thuế tại chỗ (đợt sau).
- `SetPKFlag(0)` khi rời trận tắt luôn chế độ PK người chơi tự bật (map SetFightState — THẤP-3).

## 5. CÒN LẠI / ĐỀ XUẤT ĐỢT SAU

1. Cửa sổ KUiCityManage thật (COP 38 + page 8) nếu chủ game muốn UI thay thoại.
2. Cưỡng chế ForbidChangePK/DisabledUseTownP (cần cờ per-player + chặn ở KPlayer).
3. Bia Quản Thành cho 6 thành còn lại (AddObj như `addobjtuongduong`).
4. Lôi đài 213-220: đăng ký đủ, idle như nhánh VN gốc — bật = đợt riêng.
5. Hai file persist mới tự sinh ở `bin\server\settings\`: `jx2citywar.txt` (+ `jx2league.txt`, `jx2ladder.txt` từ E2) — đừng commit.
6. Review KJx2League còn 2 điểm VỪA chưa xử (đã ghi nhận, không chặn): save-cả-file mỗi lần ghi (league 535 chỉ phình
   nếu deploy battlehead — ta KHÔNG deploy) và trần 16 timer/tick.
