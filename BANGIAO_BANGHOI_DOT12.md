# BÀN GIAO BANG HỘI ĐỢT 12 (14/08/2026) — 4 đợt A/B/C/D + 3 lỗi UI + khung nhật ký

Chuỗi commit: `d0c21b7` → `ef6c3b0` (28 commit). Khảo sát nền: `KHAOSAT_KHOILUONG_DOT12.md`.
**PHẢI RESTART: GameServer + S3Relay + client cùng lúc** (gói RECRUIT/RECORD/ZHAOMU + TOP/COP mới).

## Đợt A — vá gấp + đại thần
- WS_UP/ADD/OPEN qua relay: 4 điều kiện gốc + trừ quỹ + trần cấp (hết miễn phí vô hạn). COP_WS_SETLV kẹp ≤ cấp thật.
- League stub hết đệ quy (kho TJX_LG_TASK riêng, trả 0).
- Mặt nạ quyền 14 đúng blueprint; gate bổ nhiệm 1002.
- Đại thần: RemoveSkillState + GetTongDuty + minister.lua (hào quang khi login; ifMagic aura=0/skill=1).

## Đợt B — lãnh địa + việc vừa
- 11 map 586-597 nạp (WorldSet Count=44, đánh số lại; asset có sẵn 100% trong pak).
- COP_ENTER_MAP 36 "Vào bổn bang" (điểm vào 1718,3313); NPC/7 xưởng sinh lúc OnGame (remap SetNpcScript qua đường tuyệt đối).
- G-27 lưu chiêu mộ trừ 1000 vạn; D-3 trần tuần (tràn → field 19); F-1 nhật ký 16 dòng; G-28 PushViewTo đẩy ngược trang.

## Đợt C — kinh tế + sản xuất CHẠY BẰNG LUA GỐC
- Boot nạp trọn scriptjx2\tong_vn (cờ MODEL_GAMESERVER=1/state; g_GetScript không tự nạp!).
- TabFile theo TÊN (một ô toàn cục là lỗi chặn — workshop_setting chết giữa file).
- TWS_ApplyUse = vòng USE_R→USE_G_2 thật (kiểm status lua_call — fail-open là lỗ phát đồ free).
- TONG_DailyMaintainAll 06h05 chạy MAINTAIN_R/WEEKLY_MAINTAIN_R nguyên văn; relay nhường consume/17/stunt + snapshot 22-35; dedup TOP_WEEKLY; Lua AddDay/AddWeek no-op (relay sở hữu 20/21).
- sSendFieldCmd áp lạc quan (chống double-spend); item remap +1 (1022 hồng bao/1023 boss); Describe/PartnerBag/ChangeNpcFeature/securitylock stub; TONGTSK C++ dời 1011/1014/1292/1293.
- COP_KICK gọi MEMBER_KICK_G_1(flag 2)→MEMBER_KICK_R nguyên văn (trần 20/ngày, 60/40).

## Đợt D — 2 cửa sổ + hàng đợi đơn
- Bổ nhiệm: KUiTongAssignBox chế độ JX2 (nAct 3 mở cửa sổ, COP_SET_FIGURE 1/2/3).
- Phát tiền: KUiTongGrant + UiTongGrant.ini (blueprint gốc SAI đường sprite ImgTitle — đã sửa); COP_GRANT_GROUP 37 ("e|c|m" + cờ bit0-4); lượng = đơn vị nhập.
- Hàng đợi đơn: ZHAOMU 33/42 + TongZhaoMuDB (khóa tên, index bang) + replica GS 64 + RECRUIT_SYNC đuôi wStart/wTotal + client phân trang 8/trang (gốc 25 — deviation ghi rõ).

## UI theo ảnh chủ game
- Panel thành viên: lớp phủ KTJX2Shade nền xanh mờ (con — cha vẽ bị PageBg đè!), 6 dòng đúng mẫu, pitch 14px, màu online/offline/chọn theo blueprint.
- Nút 1 "chiêu mộ đen" = sprite thiếu + thiếu Color → nút chung 5 chữ + bind ImgSubPageMask (nạp ngay khi vào trang).
- 3 hàng Tin tức cá nhân bind đủ; buffer nhật ký 1300→2048 (gói 16 dòng 1672B!); nhật ký = MessageListBox 541×310 + thanh kéo; đổi phe ghi nhật ký.

## Bẫy mới cho đợt sau
1. g_GetScript KHÔNG tự nạp — script ngoài \script phải vào cây lúc boot.
2. TabFile giờ theo tên; gọi 1 tham số = ô cũ.
3. Heredoc bash nuốt `\` trước số → anchor chứa octal phải dựng bằng bytes([92]).
4. wnds.h không include-guard — .h cửa sổ mới include wndimage/wndtext/wndedit/wndbutton như UiTongJX2.h.
5. KLuaScript::SafeCallBegin không ghi out-param (D8) — luôn `int nTop = 0`.
6. Relay 20/21 (Day/Week) độc quyền; GS Lua AddDay/AddWeek là no-op CÓ CHỦ ĐÍCH.
