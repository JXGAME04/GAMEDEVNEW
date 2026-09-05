# BÀN GIAO — TỐNG KIM: THÔNG BÁO GIẾT ĐỊCH GIỮA MÀN HÌNH (sửa lỗi) + BẢNG ĐIỂM "Tống VS Kim" (mổ 2.0) — 04/09/2026, phiên `wauto-4a`

Chủ (04/09 chiều): *"bạn mổ nhị phân bản 2.0 có phần thông báo giết địch ở tống kim ngay giữa màn hình như trên hình dự án đang có
nhưng bị lỗi — mổ bản 2.0 lấy cái đó cho tôi và tìm thêm cái hiện số điểm như trên hình gắn vào tống kim của dự án"*.

## 0. Trạng thái lúc bàn giao

| Việc | Tệp | Trạng thái |
|---|---|---|
| Client `Game.exe` (S3Client) | `KUiFlashMessage` viết lại phần vẽ, `KUiMsgCentrePad` chép chuỗi, `KUiTongKimScore` (mới), `GameSpaceChangedNotify`, `UiShell`, vcxproj | build từ worktree `D:\GAMEDEVNEW_wt_mail` (= origin/main + nhánh) → **`bin\client\Game.exe.moi`** (xem mục 6 md5) |
| Client ini/spr (tệp rời, ngoài pak) | `bin\client\Ui\Ui3\UiFlashMessage.ini` (ghi lại), `UiTongKimScore.ini` (mới), `bin\client\spr\Ui3\UiGameMain\UiTongKim\bangdiem.spr` (mới) | **đã đặt** trên cây chạy thật (bản cũ `.truoc_tkchat_0409`) |
| Script máy chủ (nạp lúc boot → **cần restart**) | `tinhnang\tong_kim_tcap\{lib_tktc,tongtu,kimtu}.lua`, `maps\tongkim\trap\{tongratrai,kimratrai}.lua`, `timertask\task03.lua` | **đã vá** trên cây chạy thật (sao lưu `.truoc_tkchat_0409`); không cần build CoreServer |
| Gương trong git | `serverscript_jx2\tongkim_chat\{server,client}\...` + công cụ `ReverseTools\tongkim_chat\` | commit trên `mail-0309` → cherry-pick `origin/main` |

Chủ cần: (1) `ChoiGame.bat` để nuốt `Game.exe.moi` (CoreClient.dll KHÔNG đổi); (2) khởi động lại máy chủ để nạp script TK mới.
Thiếu (1): dòng giết địch vẫn như cũ (bị cắt), bảng điểm không hiện; thiếu (2): dòng vẫn có 2-3 dấu cách quanh chức vụ, bảng điểm không có dữ liệu.

## 1. Lỗi dòng giết địch — gốc và cách sửa

Dòng `"<ai> đánh trọng thương Kim <chức vụ> <ai> nhận được N tích lũy."` do `tongtu.lua`/`kimtu.lua:87` phát bằng `Msg2MSAll` → `KMission::Msg2All`
→ `KPlayerChat::SendSystemInfo(1, idx, "Hệ Thống", ...)` (= dòng chat hệ thống). Client: `KUiMsgCentrePad::ChannelMessageArrival`
(`UiMsgCentrePad.cpp:265-283`) bắt mọi dòng có chữ "đánh trọng thương" và đẩy sang **`KUiFlashMessage`** (`UiFlashMessage.ini`) — đây là widget
vẽ giữa-trên màn hình. Ba lỗi thật (đối chiếu ảnh chủ gửi):

1. **Cắt ở mép phải** ("…nhận được 330 tích l"): cửa sổ rộng 440/600 px, `PaintWindow` vẽ căn TRÁI tại `x = Left + IndentH` với
   `OutputRichText(..., m_nVisionWidth)` một dòng → câu dài hơn 578 px bị cắt.
2. **Dòng nhảy hàng**: ô trống nào nhận tin thì vẽ ở hàng đó → tin mới có thể nằm TRÊN tin cũ.
3. **Khoảng trống quanh chức vụ**: mẫu câu máy chủ `"%s %s <color> %s"` + tên chức vụ có dấu cách đuôi (`"Hiệu úy "`) → 2 dấu cách trước, 3 sau.
   Ngoài ra `UiMsgCentrePad.cpp` chép `strlen-1` byte (rơi ký tự cuối).

Sửa (`[TKCHAT 04/09]`):
- `UiFlashMessage.cpp PaintWindow`: xếp các ô đang hiện theo `uDisplayStartTime` (cũ trên, mới dưới), vẽ liên tiếp từ trên xuống,
  **căn giữa** theo bề rộng thật (`sTkChatDoRong`: đếm đúng cách `KTextProcess::GetSimplexText` — byte > 0x80 = 2 nửa-ô và nuốt 2 byte,
  `KTC_COLOR` 4 byte = 0, `RESTORE` 1 byte = 0; một nửa-ô = `nFontSize/2` px), `nLineWidth = 0` (không cắt). Chiều cao dòng `font + 4`.
- `UiFlashMessage.ini`: `Left=0`, `Width=800/1024` (cả màn hình), `Height=66` (3 dòng), `Font=14`, `DisplayDuration=6000`.
- `UiMsgCentrePad.cpp`: chép đủ chuỗi + gộp dấu cách liên tiếp.
- `tongtu.lua`/`kimtu.lua`: mẫu câu `"%s%s<color> %s"` (bỏ dấu cách thừa; cả câu riêng `KIMKILLTONG`/`TONGKILLKIM` dòng 10).

Bẫy: `TGetEncodedTextOutputLenPos` (Engine `Text.cpp:1437`) có lỗi `&&` thay vì `||` ở nhánh RESTORE → đếm sai, không dùng để đo.

## 2. Mổ client 2.0 — bảng điểm là gì

- `gamecl.exe` 2.0 (ảnh bung UPX `gamecl.img`, `gamecl_fixed.bin` của phiên trước): lớp **`KUiHuaihehepan`** (淮河河畔 = tên map/hoạt động
  "Tranh đoạt BOSS" của 2.0: *"tiêu diệt Tống Kim Tiểu Binh tích lũy điểm sĩ khí, 19:30 hàng ngày dựa vào điểm sĩ khí hai bên…"*).
  Lua xuất: `OpenHHHPShiQiZhiUI(bHiện, nTống, nKim)`, `UpdateHHHPUiData(nTống, nKim)`; giao thức `OPENUI_SHIQIZHI` / `UPDATE_SHIQIZHI`
  (`\script\lib\clientcmd.lua`: `ClientCmd:SetSQZVisiable(tb)` → `OpenHHHPShiQiZhiUI(tb[1],tb[2],tb[3])`, `UpdateShiQiZhiUI(tb)`).
- Dịch ngược (`ReverseTools\tongkim_chat\dis20.py`): `LoadScheme` = `sprintf("%s\%s", GetCurSchemePath(), "淮河河畔.ini")`, `[Main]`,
  con `[Info]` = thanh cân bằng có ảnh `"%s_fence"`, nhãn `[Label_songPoint]`/`[Label_jinPoint]`; `UpdateData(a, b)`: ghi hai số, phần trăm
  `= a*100/(a+b)` (bằng nhau → 50) dịch con trỏ thanh. Họ hàng: `即时战报界面.ini` (= `KUiBattleReport` của dự án), `宋金快报.ini`, `战役开始界面.ini`.
- **Tài nguyên KHÔNG có trong client 2.0**: tra băm `\ui\ui3_1024\...` (+ 7 gốc, 12 thư mục con) và quét NỘI DUNG mọi mục ≤ 40 KB của 13 pak
  (`scan_small.py`, 110 s) đều không thấy `淮河河畔.ini`/`宋金快报.ini` (60 mục cờ 0x11 của `resource.pak` là SPR). ⇒ dựng bảng theo CẤU TRÚC đó với ảnh tự vẽ.
- Bàn giao mổ đấu giá/chiến lệnh trước: `BANGIAO_DAUGIA_CHIENLENH_0409.md`; công cụ pak: `ReverseTools\pak_vltk\` (+ `vltk2\`).

## 3. Bảng điểm trong dự án (`[TKDIEM 04/09]`)

Client `KUiTongKimScore` (`UiTongKimScore.{h,cpp}`, `KWndImage`): ảnh `bangdiem.spr` 300×64 (Tống vàng cam / VS tím hồng / Kim xanh, vẽ bằng
`lam_bangdiem.py` Pillow → SPR), hai `KWndText` số điểm `[TongPoint]`/`[KimPoint]`, hai `KWndShadow` `[BarTong]`/`[BarKim]` chia `[Bar]` theo
`% = tống×100/(tống+kim)` (bằng nhau = 50, kẹp 3..97). `Left=-1` = tự căn giữa. Bấm xuyên qua (`PtInWindow` 0).

Kênh dữ liệu: dùng **`S2C_BATTLE_BOX` có sẵn** (`ScriptFuns.cpp LuaUpdateBattleBox` 5 tham số → `"tong|kim|diem"`, `nKind`), KHÔNG đổi giao thức,
KHÔNG build CoreServer/CoreClient: `nKind 6` = cập nhật (hiện), `nKind 9` = ẩn. Client nhận ở `GameSpaceChangedNotify.cpp case GDCNI_UPDATE_BATTLE_BOX`
(`KUiTongKimScore::OnBattleBox` chạy trước `KUiBattleReport::UpdateRankWorld` — 6 cũng là kind của panel phải cũ, không hại). Tự ẩn khi bắt đầu nạp
map mới (`GDCNI_SWITCHING_SCENEPLACE`, nParam=1).

Máy chủ (`lib_tktc.lua` cuối tệp): `TK_GuiDiemChoToi(nKind)` (một người), `TK_GuiDiemPhe(nKind)` (duyệt ô 1..600, chỉ ô mà `PIdx2MSDIdx(MSDIdx2PIdx(i)) == i`;
`UpdateBattleBox` tự bỏ người mất kết nối). Gọi: sau mỗi lần giết (`tongtu/kimtu` sau `SetMission(M_TICHLUY..)`), khi ra khỏi hậu doanh
(`*ratrai.lua` sau `SetPMParam(..,1,1)`), ẩn ở `PlayerEndTongKim` (`task03.lua`) trước `NewWorld`. Điểm = `GetMissionV(M_TICHLUYA)` (Tống) /
`M_TICHLUYB` (Kim) — đúng biến các script giết đang cộng.

## 4. Kiểm thử đã làm / còn phải nhìn tận mắt

- Build `Game.exe` (S3Client, Win32) sạch; `CoreClient` "Client Release" cũng dựng (không đổi mã Core).
- Sprite: `bangdiem_preview.png` (ReverseTools\tongkim_chat\png) — header SPR theo đúng `KSprite.h` (32 byte, `Reserved[6]`).
- 6 script Lua qua `lua4.exe` (chỉ dừng ở `Include` = parse OK).
- **Chưa thấy trong game** (không chạy được client ở đây): cần chủ vào trận TK: dòng giết địch phải nằm giữa, không cắt, xếp cũ trên mới dưới;
  bảng "Tống VS Kim" hiện ở giữa-trên khi ra khỏi hậu doanh / sau lần giết đầu, số cộng dồn theo phe, thanh lệch về phe nhiều điểm, ẩn khi rời trận.
  Nếu nền bảng hiện ĐẶC (không mờ): engine hiểu alpha RLE theo thang 0..31 — đổi `ghi_spr` sang thang đó (hiện ghi 0..255 như tool `p15_vietsub_spr.py`).

## 5. Bẫy ghi lại

- `build.py` mặc định `--project Core`: `--config "Client Release"` chỉ dựng CoreClient.dll; `Game.exe` phải `--project S3Client` (cấu hình theo vcxproj).
- Tool `p15_vietsub_spr.py` đọc header 28 byte + "4 byte đệm" — struct thật `SPRHEAD` 32 byte (`Reserved[6]` WORD); palette bắt đầu ở 32.
- Font Georgia thiếu glyph "ố" → ô vuông; dùng Arial Bold cho chữ Việt trên ảnh.
- Heredoc Bash nuốt `\U`/`\u` trong chuỗi Python → dùng `chr(92)`.
- `lua4.exe` không `-s` = stack 128 (không phải 1024); máy chủ `Lua_Create(0)` cũng 128.
- Trong `task03.lua` neo `PlayerIndex = MSDIdx2PIdx(MS_TONGKIM, dataindex)` xuất hiện 2 lần (dòng 87 có `;`) — neo phải theo nguyên dòng.

## 6. Nhị phân

- **`bin\client\Game.exe.moi` = 052fe11f** (1.462.272 B, 19:20 04/09; thay b590accc 19:03 cũng của phiên này; S3Client "Release|Win32" từ worktree
  `mail-0309` = origin/main 056e3ea6+ gộp + đợt này). Dấu kiểm: `grep -c -a "UiTongKimScore.ini"` = 1, `"ImgLeft"` = 1, `"UiFlashMessage.ini"` = 1. Khe lúc đặt còn `CoreClient.dll.moi` (17:58) + `WAuto.exe.moi`
  của phiên khác — KHÔNG đụng; đợt này không đổi Core nên Game.exe mới chạy với CoreClient hiện có lẫn bản .moi đó (cùng origin/main).
- Live lúc đặt: `Game.exe` 7e353a0c (14:41). Build: `python build.py --config Release --platform Win32 --project S3Client` (mục 5).

## 7. "Phải giống bản 2.0" — ĐÃ GIỐNG (chủ 19:00: *"Nó là tống kim bang hội… vào map tống kim bang hội là có liền"*)

Mổ nhầm lúc đầu: bảng của 2.0 KHÔNG phải `KUiHuaihehepan` (C++, tài nguyên không đóng gói) mà là **UI Lua động `tong_battle_2023` (Tống Kim Bang
Hội)**: ini cửa sổ `WndType=WndImage` `ScriptFile=\script\ui\tong_battle_2023\fight_bar.lua` (slistcl.pak uid **A652C52E** / E1F499A0, 736 B):
- `[Main]` 469×122 tại (276,45), nền `\spr\Ui4\主界面\宋金战斗界面vng\宋金战斗界面.spr`; `[ImgLeft]` chữ Tống `宋.spr` (107,33); `[ImgRight]` chữ Kim
  `金.spr` (321,35); `[TxtLScore]` (73,72) 100×20 căn giữa màu 255,128,0 "Điểm: 0"; `[TxtRScore]` (291,71) màu 255,128,192.
- Ba sprite rút từ `update.pak` 2.0 (uid 71164AFA / 688DAF8F / 9201F55D; bản không `vng` 3303E9EE/E83D99C3/D6A00DAC là chữ Trung) → chép sang
  `bin\client\spr\Ui3\UiGameMain\UiTongKim\{nen20,tong20,kim20}.spr` (tên ASCII; SPR cùng định dạng, nạp thẳng). Ảnh ghép đối chiếu:
  `ReverseTools\tongkim_chat\spr20\preview_2_0_ghep.png` — trùng ảnh chủ gửi.
- `UiTongKimScore.ini` ghi lại theo đúng toạ độ trên (`Left=-1` tự căn giữa: 800 → 276 như 2.0), `Prefix=Điểm: ` (TCVN3) cho hai dòng điểm; thanh cân
  bằng tắt (`[Bar] Width=0`) vì 2.0 không có. Lớp `KUiTongKimScore` thêm 2 `KWndImage` (`[ImgLeft]/[ImgRight]`) + tiền tố.
- Script `fight_bar.lua`/`remind_wnd.lua` KHÔNG có trong pak client 2.0 này (quét nội dung `TxtLScore`/`fight_bar` toàn bộ update/slistcl/common
  + res1/res2 ≤ 256 KB) → ở 2.0 bảng chỉ hiện tĩnh "Điểm: 0 / 0" (đúng ảnh chủ); dự án cấp dữ liệu thật qua kênh `UpdateBattleBox` (mục 3).
- Dòng giết địch dời xuống `Top=175` (`UiFlashMessage.ini`) để không đè lên bảng (45..167).
- Bản đầu (ảnh tự vẽ `bangdiem.spr` + thanh cân bằng) giữ lại làm tuỳ chọn: đổi `[Main] Image` về `bangdiem.spr` 300×64 và bật `[Bar]`.

`宋金快报.ini` (tổng kết cuối trận) + `风云宋金.ini` (BXH Phong Vân) rút từ client JX1 cũ (`update01.pak`) để ở `ReverseTools\tongkim_chat\` — chưa làm.


## 8. 19:40 — Chủ: "tôi thấy chưa có điểm" + "vẫn còn thông báo giết cũ" (ảnh trong game)

### 8a. Không có điểm — gốc: `KWndText` thường KHÔNG có bộ đệm
- `KWndText::SetText` chỉ ghi khi `m_pText != NULL`; `KWndText::Init` chỉ cấp bộ đệm khi ini có `Text=`; ini `UiTongKimScore.ini`
  không có → `SetText("Điểm: 673320")` bị bỏ qua im lặng → bảng hiện, số trống.
- Sửa (`UiTongKimScore.h`): `m_TongPoint`/`m_KimPoint` → `KWndText32` (bộ đệm 32 byte trong lớp, như `KUiTargetInfo`/`KUiTimeBox`).
- Chủ 19:50 xác nhận ảnh: `Điểm: 673320` / `Điểm: 3334620` đã hiện; đuôi rác cũng hết.

### 8b. Đuôi rác `.-2` `.MFj` `.V[j` trên dòng giết địch
- `UiMsgCentrePad.cpp:274` chép bằng `strlen(pMsgBuff)`; gói chat KHÔNG kết thúc NUL → đọc lố sang byte sau → rác.
- Sửa: chép đúng `nMsgLength` (tham số hàm), trần `sizeof(sMsg)-1`.

### 8c. Trạng thái
- `Game.exe.moi` = **9f695c30** (19:42, S3Client Release Win32; chủ đã swap). Git: mail-0309 `407a0d0f` → origin/main `5ca8bbb7`.
- Bẫy git: `git commit -- <tệp> -m "..."` SAI (git coi `-m` là pathspec) → `git commit -m "..." -- <tệp>`.
- Chủ: *"vẫn còn thông báo giết người cũ — tôi muốn xóa cũ làm cái mới như bản 2.0"* → mục 9.

## 9. "Cái mới như bản 2.0" cho dòng giết địch — mổ tiếp 2.0 (chưa có mẫu để dựng)
- Exe 2.0 KHÔNG có chuỗi TCVN3 "đánh trọng thương"/"Hệ Thống" → 2.0 không bắt tin giết địch trong C++ như dự án (`UiMsgCentrePad.cpp:265`).
- Tống Kim bang hội 2.0 (`tong_battle_2023`) có 3 cửa sổ Lua: `fight_bar.ini` (bảng điểm — đã port), **`remind_wnd.ini`** (cửa sổ nhắc: máy chủ
  gửi `emSCRIPT_PROTOCOL_TONG_BATTLE_UPDATE_INFO` → `TB_UpdateWndInfo(table)`, `..._CLOSE_WND` → `TB_CloseWndInfo`; bảng giao thức
  slistcl.pak D0E42D05:459-471), `player_info.ini`. Gốc ini = `\ui\ui3_1024\` (băm `fight_bar.ini` = A652C52E; `\ui\ui3_800\` = E1F499A0).
- 🔴 `remind_wnd.ini/.lua`, `player_info.ini/.lua`, `fight_bar.lua` KHÔNG có trong 13 pak (tra băm đúng gốc + quét nội dung); không có tệp rời.
  (`\script\Ui\tong_battle_2023\player_info.lua` uid 087C460B trong map.pak là TRÙNG BĂM — nội dung là bản đồ.)
- Widget engine 2.0 còn lại: `KUiTipsMessage` (`TipsWindow.ini` — client JX1 cũ có: 312,50 400×16 chữ xanh 0,255,0 nền bóng đen, cuộn) — 2.0 không
  đóng gói ini này; `PopBlackTips(text)` (Lua 2.0, dự án đang shim thành `Msg2Player`) = gói `#`+text → dải đen chữ vàng giữa màn hình
  (`BlackTips.ini` cũ: 125,350 550×30, `blacktipsback.spr`, LifeTime 8) — dự án ĐÃ có bản tương đương: `KUiInformation3` (`UiInformation3.ini`
  125,200 550×30 vàng, LifeTime 5, kích bằng `GDCNI_OPEN_TALK_EX` ← `KProtocolProcess.cpp:4952`).
- → Cần ảnh chụp 2.0 lúc có dòng giết địch để dựng đúng (vị trí, màu, nền, số dòng, thời gian hiện). Ini cũ rút về `ReverseTools\tongkim_chat\tips_*.ini`.
