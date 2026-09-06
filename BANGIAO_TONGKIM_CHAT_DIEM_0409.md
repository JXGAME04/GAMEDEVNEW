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


## 10. 20:30 — Chủ: "các dòng thông báo lúc đánh chết sẽ chạy giữa màn hình LÊN rồi biến mất" → làm lại KUiFlashMessage kiểu trôi lên
- Không cần mẫu ảnh nữa: chủ mô tả hành vi 2.0 = sinh ở giữa màn hình, trôi lên, biến mất. Bản cũ (đứng yên 3 hàng, hiện lại tới 3 lần) bỏ hẳn.
- `UiFlashMessage.cpp/.h` `[TKCHAT 04/09c]`:
  - `PaintWindow`: mỗi ô đang hiện vẽ tại `y = RiseFromY() - (now - start) * RiseSpeed / 1000`, căn giữa theo bề rộng thật (giữ `sTkChatDoRong`);
    `FadeMs` cuối tối dần bằng `sTkChatMoMau` (giảm 24 bit màu, giữ alpha 0xFF của `GetColor`; chữ không có alpha thật).
  - `ResetSlot`: lịch sinh `m_uNextStartTime` — dòng sau cách dòng trước đúng 1 hàng (`m_nLineHeight*1000/RiseSpeed` ms) → không đè nhau.
  - `AddMessage`: FIFO (`AddToTail`; trước chèn đầu → khi dồn tin mới hiện trước tin cũ); hàng đợi dồn quá `MaxQueueDelay` → bỏ tin mới.
  - `Breathe`: hết `DisplayDuration` HOẶC trôi qua mép trên cửa sổ → biến mất; tin NORMAL `free` (trước `AddToTail` → cùng dòng hiện lại tới `MAX_NORMAL_SHOW_TIMES`=3 lần).
  - `LoadScheme`: đọc `RiseFrom/RiseSpeed/FadeMs/MaxQueueDelay` ở đúng section đang dùng (`Main`/`Main1024`).
- Ini live + gương `serverscript_jx2\tongkim_chat\client\Ui\Ui3\UiFlashMessage.ini`: 800×600 `Top=90 Height=260 RiseFrom=320`; 1024×768 `Top=120 Height=320 RiseFrom=410`;
  chung `RiseSpeed=35 FadeMs=1500 DisplayDuration=6000 MaxQueueDelay=1500 Font=14 TextColor=0,255,0`. Chỉnh cảm giác: `RiseSpeed` (nhanh/chậm), `RiseFrom` (cao/thấp), `DisplayDuration`.
- `Game.exe.moi` = **77d148a9** (20:27). Chờ chủ `ChoiGame.bat`. Không đụng máy chủ.
- Kịch bản vá: `ReverseTools\tongkim_chat\va_flash_troilen.py` (neo LF/CRLF tự dò — tệp .h là CRLF, `vn_edit --read` không lộ `\r`).


## 11. 05/09 00:40 — Chủ (ảnh 12 dòng chồng): "3 dòng thôi + trôi nhanh hơn", "fix xong game lag hơn nhiều", "Tống VS Kim hay bị nháy"

### 11a. Nháy bảng điểm — GỐC: ẩn nhầm theo cờ nạp VÙNG
- Tôi ẩn bảng ở `GDCNI_SWITCHING_SCENEPLACE`; cờ này (`KScenePlaceC::SetLoadingStatus`) bật cả trong `SetFocusPosition` mỗi khi nhân vật
  chạy quá `SPWP_TRIGGER_LOADING_RANGE` (nạp vùng kế trong CÙNG map) → bảng ẩn, cập nhật điểm sau đó hiện lại = nháy.
- Sửa: chuyển sang `GDCNI_SWITCHING_MAPMODE` (chỉ bắn trong `KScenePlaceC::OpenPlace` = đổi map thật; `GameSpaceChangedNotify.cpp` case :880).
- Thêm: `SetScore` chỉ `Show + BringToTop` lần đầu (trước mỗi lần cập nhật đều xáo danh sách cửa sổ).

### 11b. Lag — 2 gốc
- **Máy chủ**: `TK_GuiDiemPhe` gửi 1 gói cho TỪNG người (tới 600) sau MỖI lần giết → Tống Kim đông, hàng chục lần giết/giây = hàng nghìn gói/giây,
  client xử lý bảng liên tục. Sửa Lua (`lib_tktc.lua` `[TKDIEM 05/09]`): tối đa 1 lần / 36 khung (2 s, `GetGameTime` script JX1 = frame 18/s),
  lần giết kế tiếp gửi tiếp; lệnh ẩn (kind 9) không hạn chế. **Cần restart máy chủ** (Lua nạp lúc boot). Gương: `serverscript_jx2\tongkim_chat\server\...`.
- **Client**: bản trôi lên vẽ tới 14 dòng/khung (Height 260 / 18) + `TEncodeText` + đo rộng MỖI khung cho MỖI dòng. Sửa `[TKCHAT 05/09]`:
  `MaxLines` (ini, mặc định 3): cách sinh ≥ `DisplayDuration/MaxLines` → tối đa 3 dòng cùng lúc; mã hoá + đo rộng MỘT lần lúc sinh
  (`DisplaySlot.szEnc/nEncLen/nRongPx`), `PaintWindow` chỉ vẽ.

### 11c. Ini mới (live + gương) — nhanh hơn
- 800×600: `Top=140 Height=200 RiseFrom=320`; 1024×768: `Top=230 Height=200 RiseFrom=410`; chung `RiseSpeed=60 DisplayDuration=2500 FadeMs=700 MaxLines=3 MaxQueueDelay=1200`.
  → mỗi dòng sống 2,5 s, đi 150 px, dòng sau cách dòng trước 833 ms (50 px). Kịch bản vá: `ReverseTools\tongkim_chat\va_flash_3dong.py`.
- `Game.exe.moi` = **bc08ffb4** (05/09 09:30). Chủ: `ChoiGame.bat` (client) + **restart máy chủ** (Lua tiết chế gói điểm).

## 12. 05/09 09:35 — Chủ: "tắt hoàn toàn thông báo giữa màn hình để xem còn lag không" (đang rất lag)
- `[TKCHAT 05/09b]` công tắc ini `Enable` (đọc ở section đang dùng): 0 = `MessageArrival` bỏ qua ngay (không xếp hàng), `OpenWindow` không `Show`
  (cửa sổ ẩn → không Paint/Breathe). Bật lại: `Enable=1` trong `bin\client\Ui\Ui3\UiFlashMessage.ini`, không cần build.
- Ini live + gương đặt `Enable=0` cả `[Main]`/`[Main1024]`. `Game.exe.moi` = **c9212b50** (09:34). Dòng "Hệ Thống" trong khung chat vẫn còn.
- **Đo 09:36 (3 s, 24 lõi)**: GameServer 4,3 % ≈ **1 lõi chạy kịch** (vòng lặp chính bão hoà) — 9,3 GB RAM, 60 luồng; Game.exe client 1,9 % (≈ nửa lõi);
  Goddess/Rainbow/Bishop/S3Relay ~0. → "rất lag" nằm ở MÁY CHỦ, không phải vẽ chữ trên client.
- Nếu tắt rồi vẫn lag: (1) máy chủ chưa restart → `TK_GuiDiemPhe` vẫn 600 gói/lần giết; (2) dòng chat giết địch `Msg2MSAll` (tongtu/kimtu:87)
  cũng 600 gói/lần giết — có từ trước, có thể tiết chế/gửi cho người liên quan; (3) tải nền bot (1000 bot) — ngoài phạm vi TK chat.


## 13. 05/09 10:05 — PHÂN TÍCH LOG "rất lag" (chủ: chỉ phân tích, chưa sửa)

### 13a. Nguồn số đo
- `bin\server\jx_perf_server.log` ([PERF] mỗi 60 s: tick/tre/online, KHOILUONG region+NPC, TICK / SCRIPT_TIME / SW_MAINLOOP / SW_ACTIVATE… tb/max/p95/chiếm);
  `bot.log` (lịch bot vào trận TK "RA TRAN", chết/phút); `jx_auto_server.log` (debug từng phát đánh, 243 dòng/s); client `jx_paint.log` ([SEC] fps, tick), `jx_rep3.log`.
- `SCRIPT_TIME` CHỈ đo script hẹn giờ (`CoreServerShell.cpp:1164 pTimeScript`), KHÔNG gồm script chết/giết → script `OnDeath` nằm trong `SW_MAINLOOP`.

### 13b. Máy chủ có 2 chế độ (tick ngân sách 55 ms = 18 tick/s)
| cửa sổ | tick tb | p95 | % tick trễ | MAIN tb | % phút bão hoà |
|---|---|---|---|---|---|
| 04/09 20:05-20:50 không TK (nền, 1001 online) | 6,8 ms | 9 | 0,1 | 4,4 | 0 |
| 04/09 01:30-03:20 TK đêm (Lua cũ) | 11,2 | 17 | 0,2 | 7,7 | 0 |
| 04/09 16:30 TK, 17:50 TK (TRƯỚC 19:29) | 8,9 / 8,7 | 12 / 15 | 0,2 / 0,5 | 5,9 / 5,6 | 0 |
| 04/09 19:30 TK (SAU restart 19:29 = Lua bảng điểm chưa tiết chế, CoreServer 17:35) | **36,3** | 143 | **26,9** | 33,0 | 0 |
| 04/09 20:50 TK, 22:50 TK (CoreServer 20:47) | **49,6 / 47,2** | 163 / 159 | **32,8 / 30,5** | 45,8 / 43,4 | **81 / 78** |
| 05/09 09:20-09:43 TK (trước restart 09:44) | **57,0** | 184 | **39,4** | 53,2 | **92** |
| 05/09 09:48-10:05 TK (SAU restart 09:44 = Lua đã tiết chế 2 s) | 12,0 | 25 | 3,7 | 7,8 | 0 |
- Bão hoà = tick tb 55-78 ms, max 400-1250 ms, 40-48 % tick trễ → mọi thao tác trễ 0,2-1 s = "rất lag". Khối lượng region/NPC mỗi tick KHÔNG tăng khi bão hoà
  (reg 1100-1200, npc 2100-2300) → thời gian MỖI NPC tăng ×4, không phải nhiều NPC hơn.
- Client (`jx_paint.log` [SEC]): 52-61 fps, tick thế giới 10-24 ms/giây, tickmax 3-5 ms; `jx_rep3` 63 fps → client KHÔNG phải nút thắt.
- GameServer RAM 9,3 GB WS / 11,6 GB private ổn định (+1 MB/30 s) → không rò; 1 luồng game 7,2 s CPU/30 s (24 % lõi) khi bình thường, ≈ 1 lõi kịch khi bão hoà.

### 13c. GỐC: `TK_GuiDiemPhe` (Lua `[TKDIEM 04/09]` của tôi, sống từ restart 19:29) chạy MỖI LẦN người chơi chết
- Bot TK chết ~600 lần/phút (bot.log "da chet") = ~10 lần/giây. Mỗi lần: vòng `for i = 1, 600`: `MSDIdx2PIdx` (O(1)) + **`PIdx2MSDIdx` = `KMission::GetMissionPlayer_DataIndex` → `FindSame` quét
  danh sách `m_UseIdx` tới 600 phần tử** → 360.000 bước/lần giết (O(N²)); + 4 lời gọi Lua→C mỗi vòng (2.400/lần giết, mỗi lời gọi `GetSubWorldIndex` + `m_MissionArray.GetData`);
  + 600 `PackDataToClient` (mỗi gói lấy khoá TOÀN CỤC `m_csCM` chung với luồng mạng IOCP + khoá kết nối; đầy bộ đệm thì `_SendDataEx` gửi ngay trên luồng game).
- Ước lượng từ số đo: +27..+45 ms/tick ở 10 lần giết/s ⇒ **≈ 30-45 ms cho MỖI lần gọi** (≈ 60 µs/vòng). Sau tiết chế 1 lần/2 s: +2 ms/tick (12,0 vs 8,7-8,9) — khớp.
- So sánh: `Msg2MSAll` (chat giết địch, có từ trước) cũng 600 `SendSystemInfo`/lần giết nhưng duyệt thẳng `m_UseIdx` (O(N), không FindSame, 1 lời gọi Lua) → TK trước 19:29 vẫn 8,7 ms.
- Chuỗi nhân quả: 19:29 nạp Lua → TK 19:30 36 ms (27 % trễ) → 20:47 thay CoreServer (không phải gốc, cộng thêm ~3 ms) → 20:50/22:50/09:20 bão hoà → 09:28 vá tiết chế, 09:44 restart → 12 ms.

### 13d. Thứ yếu (chưa sửa, ghi nhận)
- Chat giết địch `Msg2MSAll`: 600 gói/lần giết × 10/s = 6.000 gói/s, mỗi client nhận 10 dòng/s — chi phí còn lại lớn nhất theo lần giết (~1-2 ms/lần).
- Log debug: máy chủ `jx_auto_server.log` 243 dòng/s (67 MB/30 phút; `[AutoLog] On=1` config.ini), `bot.log` 5 KB/s; client `jx_auto.log` **86 KB/s** (5 MB/phút, [S6-SYNC] mỗi khung) — không gây bão hoà nhưng tốn CPU/đĩa; nên tắt khi đo thật.
- `SCRIPT_TIME max` ≈ 95 ms đều đặn mỗi phút TK (script hẹn giờ theo phút — task03/timertask?) và 220-400 ms lúc 1000 bot đăng nhập dồn (09:47-09:49) → khựng 0,1-0,4 s.
- `SW_MAINLOOP max` 1000-1600 ms lẻ tẻ cả lúc bình thường (18:20 1246 ms, 21:22 1593 ms) → khựng ~1 s hiếm, chưa rõ nguồn (autosave/nạp map/DB?).
- Bot: `[BotTrap]` 900 dòng/phút "đạp trap → BỎ QUA" = bot dẫm trap liên tục (trap TK gọi script mỗi lần) — chi phí nhỏ nhưng vô ích.

### 13e. Đề xuất (CHƯA làm — chờ chủ)
1. Bảng điểm: đưa vòng gửi vào C++ (`KMission::Msg2All`-style duyệt `m_UseIdx`, gói `S2C_BATTLE_BOX` chung) → 1 lời gọi Lua, O(N), không FindSame; giữ tiết chế 2 s. Hoặc Lua chỉ duyệt
   `m_UseIdx` qua hàm C mới `MSNextIdx`.
2. Chat giết địch: gộp theo 1-2 s hoặc chỉ gửi cho người trong vùng nhìn thấy / người liên quan (đã hỏi chủ 09:36).
3. Tắt `[AutoLog]` máy chủ + client khi đo hiệu năng; xoay `bot.log`.
4. Mổ 95 ms script hẹn giờ mỗi phút và khựng 1 s `SW_MAINLOOP` (log riêng theo tick khi > 200 ms).


### 13f. 05/09 10:40 — ĐO THẬT + CƠ CHẾ XÁC NHẬN (chủ: "phân tích nguyên nhân")
**Đo (ghép `bot.log` số lần chết/phút với `SW_MAINLOOP` mỗi phút, trừ nền 4.787 ms/phút):**
| giai đoạn | chết/phút | MAIN tb | ms MAIN cho MỖI lần chết (hiệu số) | hồi quy theo phút |
|---|---|---|---|---|
| A. TK 16:30 + 17:50 hôm qua — chỉ chat giết địch | 474 | 5,6–5,9 ms | **2,6–3,3 ms** | 6,5 ms (n=52) |
| B. 4 trận TK 19:31→09:43 — chat + vòng bảng điểm MỖI lần chết | 413–572 | 33,9–55,1 ms | **77–98 ms** | 86,4 ms (n=113) |
| C. TK 09:49→10:05 — bảng điểm tiết chế 1 lần/2 s | 497 | 8,2 ms | 8,1 ms | 7,6 ms (n=17) |
→ Vòng `TK_GuiDiemPhe` tốn **~90 ms cho mỗi lần gọi**; ở 10 lần chết/giây = 900 ms/giây > ngân sách 18 tick × 55 ms → bão hoà. Tiết chế 0,5 lần/giây → +45 ms/giây ≈ +2,5 ms/tick (khớp C).

**Cơ chế (đọc mã):** không phải `PackDataToClient` (600 gói/lần chết của chat có sẵn chỉ ~3 ms) và không phải `FindSame` (`KLinkArray::GetNext` là liên kết đôi O(1), quét 600 ô ≈ 4 ms/lần chết). Thủ phạm:
- MỌI hàm Lua về nhiệm vụ (`MSDIdx2PIdx`, `PIdx2MSDIdx`, `GetPMParam`, `SetPMParam`, `UpdateBattleBox`, `Msg2MSAll`, …) đều viết `KMission Mission; Mission.SetMissionId(id); m_MissionArray.GetData(&Mission)` — dựng một **`KMission` tạm trên stack** chỉ để tra theo id.
- `KMission` chứa `KMissionPlayerArray = KLinkArrayTemplate<TMissionPlayerInfo, MAX_PLAYER=1500>` (~88 B/ô) + `KMissionNpcArray<…, 5000>` + `KMissionTimerArray<…, 10>`. Constructor `KLinkArrayTemplate()` (`KLinkArrayTemplate.h`): `m_FreeIdx.Init(ulSize)` + `m_UseIdx.Init(ulSize)` = **2 × `new KLinkNode[ulSize]`**, rồi `for (i = ulSize-1; i > 0; i--) m_FreeIdx.Insert(i)` + `Clear()`; destructor `delete[]` ×2. `KMission()` còn duyệt 10 timer.
- ⇒ mỗi lời gọi Lua = **6 cấp phát + 6 giải phóng heap, ~6510 vòng chèn danh sách, chạm ~207 KB bộ nhớ** ≈ 30–45 µs. Vòng bảng điểm: 600 ô × 4 lời gọi = 2.400 lời gọi/lần chết ≈ 70–110 ms → khớp số đo 77–98 ms.
- Cùng cơ chế giải thích chi phí nền của TK: mỗi lần chết `tongtu.lua` gọi ~25 hàm nhiệm vụ (GetPMParam/SetPMParam…) ≈ 1 ms + 600 gói chat ≈ 2 ms = 3 ms/lần chết (giai đoạn A).
- Hệ quả rộng hơn (chưa sửa): **mọi script dùng API nhiệm vụ đều trả giá ~40 µs/lời gọi** (Tống Kim, Phong Lăng Độ, công thành, Bang chiến, timer nhiệm vụ mỗi phút) — nghi là nguồn của `SCRIPT_TIME max ≈ 95 ms` mỗi phút TK (script hẹn giờ duyệt người chơi bằng API này).

**Đề xuất gốc rễ (chưa làm):** trong `ScriptFuns.cpp` thay `KMission Mission; …GetData(&Mission)` bằng tra theo id không dựng đối tượng (thêm `KSubWorldMissionArray::GetByMissionId(id)` duyệt `m_UseIdx` so `m_ulMissionId`, hoặc bảng `id → KMission*`) → mọi API nhiệm vụ rẻ ~100 lần; khi đó vòng bảng điểm 600 ô chỉ ~1-2 ms, chat giết địch ~2 ms. Song song vẫn nên tiết chế/gộp phát điểm và đưa vòng gửi vào C++.


### 13g. 05/09 11:10 — ẢNH HƯỞNG NẾU SỬA GỐC RỄ (chủ hỏi "fix vậy có ảnh hưởng gì không?") — chỉ phân tích
**Phương án gốc rễ = thay `KMission Mission; Mission.SetMissionId(id); m_MissionArray.GetData(&Mission)` bằng tra theo id không dựng đối tượng.**

Đã kiểm từng điều kiện an toàn:
1. **Ngữ nghĩa giống hệt**: `KMissionArray::FindSame` (`KMissionArray.h:64`) chỉ so `GetMissionId()`, KHÔNG ghi gì vào nhiệm vụ thật (khác mảng người chơi `_KMissionPlayerArray::FindSame` có ghi `m_ulPlayerIndex`). Trả `NULL` khi không có → giữ nguyên.
2. **Toàn bộ 27 chỗ đều là TRA thuần**: 24 trong `ScriptFuns.cpp` (+`StopMission` 11620), `KJx2WarInfra.cpp` 1, `KPlayerBot.cpp:9200` (`pb_TkMission`). Đối tượng tạm không được dùng vào việc gì khác. TẠO nhiệm vụ đi qua `m_MissionArray.Add()` (ScriptFuns 11447), XOÁ qua `Remove(pMission)` — không đụng.
3. **Constructor/destructor không có tác dụng phụ toàn cục**: `KMission()` chỉ `SetOwner` cho 10 timer của chính nó + memset trường riêng; `KLinkArray` dtor `delete[]` node của chính nó.
4. **Không có luồng khác chạy Lua**: máy chủ chỉ có luồng MySQL (`KMySQLDB.cpp:153`), client có luồng nạp cảnh — script luôn ở luồng chính → không có race.
5. **Phạm vi build**: `ScriptFuns.cpp` dùng chung nhưng API nhiệm vụ nằm trong vùng `_SERVER` — `CoreClient.dll` live KHÔNG chứa `GetPMParam/UpdateBattleBox/Msg2MSAll/PIdx2MSDIdx` → client không đổi hành vi; vẫn phải build cả hai (luật) và swap CoreServer + restart.
6. **Bộ nhớ**: mỗi lời gọi hiện đặt ~210 KB (`m_Data` người 1500×88 = 128 KB + NPC 5000×16 = 78 KB + timer/giá trị) trên STACK và `new[]` 101 KB node heap rồi `delete[]` → sau sửa không còn → giảm áp lực stack khi script lồng nhau (`ExecuteScript` → C → Lua) và giảm phân mảnh heap (heap CRT dùng chung với luồng IOCP). Chỉ có lợi.
7. **Điểm KHÔNG đổi**: cấu trúc `KMission`, giao thức, dữ liệu lưu, cân bằng — không đụng.

**Rủi ro thực tế còn lại của phương án gốc rễ:**
- Sửa 27 chỗ trong tệp bị nhiều phiên cùng sửa (`ScriptFuns.cpp`) → xung đột gộp; làm bằng kịch bản thay mẫu + kiểm `grep` không còn `KMission Mission;`, commit pathspec, build cả hai cấu hình.
- Lợi ích chưa đo được phần nền: bao nhiêu lời gọi API nhiệm vụ mỗi tick lúc bình thường (timer nhiệm vụ mỗi phút, bot `pb_TkNhip` 2 lần/tick, `PB_TrapLog` mỗi lần bot đạp trap ~15/s) — sẽ thấy trong `SCRIPT_TIME max` (95 ms) sau khi sửa.

**Xếp hạng phương án (rủi ro ↑):**
| # | Phương án | Hiệu quả | Rủi ro | Ghi chú |
|---|---|---|---|---|
| 1 | Giữ tiết chế 2 s (đã sống 09:44) | TK 12 ms | 0 | bảng điểm trễ ≤ 2 s |
| 2 | Vòng gửi bảng điểm vào C++ (duyệt `m_UseIdx` như `Msg2All`, 1 lời gọi Lua) | 2.400 lời gọi → 1; ~2 ms/lần | thấp, chỉ tính năng của tôi | có thể bỏ tiết chế hoặc giữ 1 s |
| 3 | `FindById` thay 27 chỗ dựng `KMission` tạm | mọi API nhiệm vụ ~40 µs → <0,5 µs; giảm stack/heap | thấp về ngữ nghĩa, trung bình về quy trình (tệp chung, build 2 cấu hình, restart) | lợi cho TẤT CẢ nhiệm vụ + bot |
| 4 | Sửa `KLinkArrayTemplate` khởi tạo lười | như 3 | CAO: đổi container lõi dùng bởi mảng người/NPC/timer của nhiệm vụ thật | KHÔNG khuyến nghị |
| 5 | Đối tượng dò tĩnh `static KMission s_Probe` | như 3, ít sửa chỗ | thấp (không luồng khác) nhưng kém rõ ràng | phương án dự phòng |

Khuyến nghị: làm 2 + 3 (3 bằng kịch bản, kiểm `grep`), thử trên máy chủ dev/giờ vắng, đo lại `jx_perf_server.log` trước/sau cùng cửa sổ TK.


## 14. 05/09 10:45 — THI CÔNG phương án 2 + 3 (chủ chốt "làm 2-3")
### 14a. Phương án 3 — `[MSFIND 05/09]` tra nhiệm vụ theo id, không dựng `KMission` tạm
- `KMissionArray.h`: thêm `T* FindById(unsigned long ulMissionId)` (duyệt `m_UseIdx`, so `GetMissionId()`, trả `&m_Data[nIdx]` hoặc NULL) — tương đương 100 % `GetData(&probe)` vì `FindSame` chỉ so id.
- Thay 27/27 chỗ bằng kịch bản regex có kiểm đếm (`ReverseTools\tongkim_chat\va_msfind.py`): `ScriptFuns.cpp` 25 (kể cả `StopMission`), `KJx2WarInfra.cpp` 1, `KPlayerBot.cpp` 1 (`pb_TkMission`). Kiểm: không còn `KMission X;` trong 3 tệp; số byte cao không đổi.
- Mỗi lời gọi API nhiệm vụ: bỏ ~210 KB stack + 2×3 `new[]/delete[]` (101 KB) + ~6.500 vòng chèn → còn ≤ 10 so sánh id.
### 14b. Phương án 2 — `[TKDIEM 05/09]` `UpdateBattleBoxAll(nMissionId, nTong, nKim, nKind)` (C++, `ScriptFuns.cpp` sau `LuaUpdateBattleBox`, đăng ký cạnh `UpdateBattleBox`)
- Duyệt thẳng `m_MissionPlayer.m_UseIdx` như `KMission::Msg2All`; điều kiện gửi = đúng vòng Lua cũ: ô còn dùng (`MISSION_PARAM_AVAILABLE`), còn nối, `Player[idx].m_dwID == m_ulPlayerID` (bỏ ô cũ của người đã rời); gói `S2C_BATTLE_BOX` "tong|kim|diem_riêng" (`m_nParam[6]`); trả số người đã gửi.
- Lua `lib_tktc.lua` `TK_GuiDiemPhe`: tiết chế 18 khung (1 s, trước 2 s) → `if UpdateBattleBoxAll then return UpdateBattleBoxAll(...) end` → **rơi về vòng cũ nếu CoreServer cũ** (không lỗi script khi chỉ restart mà chưa swap DLL). Gương git: `serverscript_jx2\tongkim_chat\server\script\...`.
### 14c. Build / triển khai
- Worktree mail-0309 đã merge `origin/main` 573b9130 (diff trống) → build từ main. `CoreServer.dll` x64 Server Release = **a2053172** (10:42; dấu hiệu: `UpdateBattleBoxAll`, `CL_Cong`, `AUC_MsgTong`, `[CFGW]`, `S13-TELE`).
- Khe `bin\server\CoreServer.dll.moi` có bản chờ 10:02 của phiên khác (= `[CFGW 05/09]` 6177ba7e, đã trong main → bản tôi bao trùm) → đổi tên thành `CoreServer.dll.moi.cfgw_1002_phienkhac`, đặt bản a2053172 làm `.moi`. Chủ: `ChayGameServer.bat` (swap DLL + nạp Lua mới).
- Client Win32 chỉ build kiểm biên dịch (API nhiệm vụ không có trong CoreClient) — không swap client.
- Kỳ vọng đo sau restart (cùng cửa sổ TK ~500 chết/phút): MAIN mỗi lần chết 3 → ~2 ms (còn 600 gói chat), bảng điểm 1 s/lần ~2-3 ms/lần, `SCRIPT_TIME max` mỗi phút giảm.
- 10:45 — main nhận thêm ff7d0359 `[CFGW 05/09]` (KCauHinhWeb + KCore) sau lúc merge → merge lại (1c72eb1d), build lại: **CoreServer.dll = e4d6156e** (10:45) = `.moi` hiện tại; a2053172 bỏ. Git: mail-0309 == main (2e6d93d8 + ff7d0359).

### 14d. 05/09 11:10 — ĐO SAU FIX (máy chủ restart 10:50:03, pid 4660, DLL e4d6156e; TK từ 10:56, ~580 chết/phút; không lỗi script)
```
D. TK 10:57-11:08 SAU FIX 2+3 (DLL e4d6156e, tiet che 1 s) n=12 chet/ph= 584 MAIN= 5.56 tick=  7.9 p95= 10 tre= 0.1% tickmax=  95.0 SCmax= 62.3 | ms/chet hieu so=  2.1 hoi quy=11.3
C. TK 09:49-10:05 tiet che 2 s (DLL cu)              n=17 chet/ph= 497 MAIN= 8.16 tick= 12.5 p95= 26 tre= 4.0% tickmax= 369.1 SCmax=115.5 | ms/chet hieu so=  8.1 hoi quy=7.6
B. TK 09:21-09:43 vong Lua moi lan chet              n=23 chet/ph= 572 MAIN=55.07 tick= 58.8 p95=190 tre=40.9% tickmax=1255.6 SCmax= 78.7 | ms/chet hieu so= 95.8 hoi quy=83.7
A. TK 16:30-16:55 hom qua, chi chat                  n=26 chet/ph=   0 MAIN= 5.87 tick=  8.9 p95= 12 tre= 0.2% tickmax= 544.2 SCmax= 97.3 | ms/chet hieu so=  0.0 hoi quy=-
N. nen 20:05-20:49 hom qua, khong TK                 n=45 chet/ph=   0 MAIN= 4.43 tick=  6.8 p95=  9 tre= 0.1% tickmax=  73.4 SCmax= 52.1 | ms/chet hieu so=  0.0 hoi quy=-
```
- Chi phí mỗi lần chết: B 77–98 ms → C 8 ms → **D ~1,8 ms** (còn lại chủ yếu 600 gói chat giết địch); tick trễ 39 % → 5 % → **0,1 %**; p95 184 → 25 → **~10 ms**.
- `SCRIPT_TIME max` mỗi phút TK 93–97 ms → 56–86 ms (nền không TK 55–62 ms) — script hẹn giờ theo phút vẫn ~55 ms, việc riêng (mục 13d).


## 15. 05/09 11:30 — "tiếp tục": hai việc còn lại (chỉ phân tích, chưa đổi gì)
### 15a. Khựng ~55 ms MỖI PHÚT (`SCRIPT_TIME max`, cả lúc không Tống Kim)
- `CoreServerShell.cpp:1162-1172`: mỗi tick, nếu khung %% 18 == 0 và giây hệ thống == 0 → `timerserver.lua RunTime()` (script hẹn giờ, 1 lần/phút).
- `RunTime()` (`timerserver.lua:55-84`, `[NHIPNAP 29/08]` của chủ): **`dofile("script/timerserver.lua")` nạp lại chính nó + 33 `Include` = 90 tệp / 1,17 MB mỗi phút** để "sửa script ăn ngay, không cần restart". Đo ngoài engine bằng `lua4.exe` + tag method `getglobal` giả lập hàm engine: **~20 ms** chỉ riêng biên dịch (vài tệp dừng sớm → trong engine cao hơn, có thêm tra pak/đăng ký hàm). Phần còn lại của 55-86 ms = tác vụ theo phút (`BDH_JitanTick`, `CL_Tick_Wrap`, `BotAuto_Tick`, `sukien_tongkim`, bảo trì bang 6:05…); trong phút TK trước FindById 93-97 ms, sau 56-86 ms.
- Hậu quả: đúng 1 tick mỗi phút dài ~60-90 ms (ngân sách 55) → mọi người khựng nhẹ 1 lần/phút; không tích luỹ.
- **Đã có núm, không cần sửa mã**: `script\cauhinh\ch_chung.lua:137 CH_NAPLAI_PHUT = 1` (1 = mỗi phút, 5 = 5 phút/lần, 0 = tắt — khi tắt thì sửa script phải restart); khoá đã phơi lên web admin (`cauhinh_web\cfgw_meta.lua:68`, nhóm HETHONG, nhịp áp 30 s). → Quyết định của chủ: giữ 1 (tiện dev) hay 5/0 (mượt hơn).
### 15b. Chat giết địch `Msg2MSAll` (tongtu/kimtu:88)
- 600 `SendSystemInfo` → 600 `PackDataToClient` mỗi lần chết ≈ 2 ms; ở ~10 lần chết/giây = 20 ms/giây ≈ 2 %% ngân sách; mỗi client nhận ~10 dòng/giây (chat + widget).
- Phương án nếu muốn giảm: (a) chỉ gửi cho người trong 3×3 vùng quanh chỗ chết (KRegion) + người giết/bị giết — đổi trải nghiệm (không thấy kill xa); (b) gộp 1 giây gửi 1 gói nhiều dòng — client `KUiFlashMessage` cần tách dòng; (c) giữ nguyên. Khuyến nghị **(c)** vì chi phí đã nhỏ sau fix.
- 🔴 **Chủ 11:35: "phần RunTime() cẩn thận vì nó là bộ hẹn của các hoạt động"** → KHÔNG đụng `RunTime()`/`timerserver.lua`; KHÔNG đổi `CH_NAPLAI_PHUT` nếu chưa rà từng driver phụ thuộc nhịp nạp lại (trạng thái cấp tệp bị reset mỗi phút theo thiết kế `g_nMocNapLai`; cấu hình `G_CFG` tươi nhờ nạp lại; `cfgw_driver` 30 s; `hd3_driver`, `ydbz_driver`, `bot_auto`, `sukien_tongkim`). Khựng ~60 ms/phút là cái giá chấp nhận của thiết kế "sửa script ăn ngay". Mục 15a chỉ là phân tích, không phải đề xuất đổi.


## 16. 05/09 12:30 — CỬA SỔ "THÔNG TIN TRẬN" kiểu Liên Đấu Bang 2.0 cho Tống Kim (chủ: "mổ nhị phân 2.0 lấy UI trên cho tống kim")
### 16a. Mổ 2.0
- Ảnh chủ: tiêu đề "Liên Đấu Bang", "Giai đoạn: Chiến đấu", "Còn: 1084 giây", bảng Hạng | Tên Bang | Phe | Điểm Phe, dòng "Nhấn xem Chiến B…".
- Exe 2.0 không chứa nhãn (UI Lua). Quét NỘI DUNG 13 pak (≤128 KB, chuỗi TCVN3) → 12 mục trong `slistcl.pak`: bảng hướng dẫn (mục BHLS
  "Liên Đấu Bang … 19:50-20:20"), bảng danh hiệu, lịch hoạt động, và **`\script\ui\greatteamfight\fieldreport.lua`** (8369606B):
  `UpdateStage` ("<color=yellow>Giai đoạn: <color>%s"), `Countdown` ("Còn: <color=green>%d<color> giây" — client tự đếm lui từ `nEndTime`).
  → "Liên Đấu Bang" = tên Việt của nhóm `greatteamfight` (侠峰论剑); cửa sổ trong ảnh là cửa sổ cùng nhóm mà ini/lua KHÔNG được đóng gói
  (tra băm `\ui\ui3_1024\greatteamfight\{fieldreport,fightbar,roundstate,entrybtn,signin}.ini` đều trống; chỉ có `fightresult.ini/lua`,
  `mainguide\mainpageset.ini`, `signin.lua`). Nền 2.0 rút được: 竞技场底图 397×403, 竞技场内底 381×239, 侠峰论剑底版 612×471 (thân gần đen
  20,20,20; dải tiêu đề nâu vàng 48,42,28; viền vàng mảnh) → tự vẽ `thongtin20.spr` 221×268 theo phong cách đó (giải mã SPR: `spr2png.py`).
### 16b. Dựng trong dự án (không đổi giao thức)
- Client `KUiTongKimInfo` (`S3Client\Ui\UiCase\UiTongKimInfo.*`, ini `UiTongKimInfo.ini` TCVN3): Title "Tống Kim"; `Giai đoạn:` + tên pha
  (`[Phase] P1=Báo danh P2=Chiến đấu P3=Kết thúc`); `Còn: N giây` — client đếm lui mỗi giây từ gói cuối (`Breathe`); bảng 5 hàng Hạng|Tên|Phe|Điểm
  (`[Camp] C1=Tống C2=Kim`); nút chữ trong suốt `[BtnReport]` "Nhấn xem Chiến Báo" (`KWndLabeledButton`) → mở `KUiBattleReport` (chiến báo top-10
  có sẵn). Hook: `GameSpaceChangedNotify` (`GDCNI_UPDATE_BATTLE_BOX` kind 7/8/9, `SWITCHING_MAPMODE` ẩn), `UiShell` mở/đóng, vcxproj/filters.
  Vị trí: 800 `Left=5 Top=170`, 1024 `Left=5 Top=215`.
- Máy chủ `ScriptFuns.cpp` `[TKINFO 05/09]`: `UpdateBattleInfo(mission, phase, timerId, tong, kim, rows)` (cho PlayerIndex) và `UpdateBattleInfoAll(...)`
  (cả trận, điều kiện như `UpdateBattleBoxAll`): kind 7 `"phase|rest|tong|kim"` (rest = `GetTimerRestTimer(timerId)/18`), kind 8 `"n;hang|ten|phe|diem;…"`
  từ **`m_MissionLadder`** (top-10 theo tham số ladder = 6 tích lũy, C++ tự xếp trong `KMission::SetPlayerParam` mỗi lần cộng điểm — TK mở nhiệm vụ với
  `nMSLadderParam = 6`), tên cắt 12 ký tự để ≤127 byte.
- Lua `lib_tktc.lua` `[TKINFO 05/09]`: `TK_GiaiDoan()` (timer 1 còn → 1 báo danh, else 2), `TK_TimerHien(pha)` (1 → timer 1, 2 → timer 3),
  `TK_GuiThongTinChoToi()` (gọi ở `tongratrai/kimratrai.lua` sau `TK_GuiDiemChoToi`), `TK_GuiThongTinPhe()` (tiết chế 54 khung = 3 s, gọi đầu
  `TK_GuiDiemPhe` mỗi lần giết). Cú pháp trung tính Lua 4/5.4 (`kiem_54.py`: 0 lỗi); **chỉ gọi khi hàm C++ tồn tại** → an toàn với máy chủ cũ.
- Kind 9 (ẩn, `task03.lua PlayerEndTongKim`) ẩn cả bảng điểm lẫn cửa sổ này.
### 16c. Trạng thái / triển khai
- wauto-c9 12:00: main đã Lua 5.4 (38d65e50) — worktree mail-0309 đã merge (df1c5dec), build link `Lua54Dll`.
- 🔴 wauto-c9 12:15 KHẨN: bat đổi cây script lỗi → **KHÔNG đặt Game.exe.moi / CoreServer.dll.moi link Lua54 vào bin** cho tới khi họ báo xong.
  Binary build xong giữ trong worktree (`Sources\Core\x64\ServerRelease\CoreServer.dll`, `Sources\S3Client\Release\Game.exe`).
- Ini + spr client và Lua máy chủ đã đặt trên cây live (trơ với binary cũ).
- 12:40 — wauto-c9 12:25 báo bat đã sửa, cho phép đặt: **`CoreServer.dll.moi` = 0e5c545d (md5 d48983c4fb74fe093a4daa5b37afd3ff)**,
  **`Game.exe.moi` = a136398e (md5 62ff6f75b0a2b4e57eef19e626d1b4af)** — cả hai link Lua54, build từ main đã merge. Chủ: `ChayGameServer.bat`
  (máy chủ lên 5.4 + cửa sổ) và `ChoiGame.bat` (client). Kiểm sau: vào Tống Kim, ra khỏi hậu doanh → cửa sổ "Tống Kim" hiện giai đoạn/giây/top 5;
  bấm "Nhấn xem Chiến Báo" mở chiến báo.

### 16d. 12:55 — Sau khi máy chủ lên Lua 5.4 (12:41): lỗi 5.4 trong đấu giá + máy chủ dừng 12:44
- `logs\hethong.log` 12:42–12:43 mỗi 30 s: `[DAUGIA] LOI Lua: [string "?"]:1503: attempt to call a table value (for iterator)` — `auction_manager.lua:1503`
  `for _, nType in {A, B} do` (Lua 4) → `ipairs({A, B})` `[LUA54 05/09]` (live cây 5.4 + gương `serverscript_jx2\mail\server\scriptuction_house`).
  Bọc `call(AUC_Tick, ..., "x")` đã giữ vòng quét sống ("bo qua nhip nay"), nên hậu quả chỉ là 2 phút Hà Lan không hạ giá. Quét cả cây 5.4: không còn
  `for … in {` / `for k,v in t do` nào khác. `kiem_54.py` chỉ kiểm biên dịch, KHÔNG bắt lỗi chạy kiểu này.
- GameServer (pid 75388, lên 12:41:24) không còn chạy lúc 12:44:36; log cuối 12:43:56–12:44:00 bình thường, không dump/Event → nghi tắt chủ động (hỏi wauto-c9).
- Script Tống Kim `[TKINFO]/[TKDIEM]` còn nguyên trong cây 5.4 (`script.lua4` giữ bản cũ), `kiem_54` 0 lỗi; cửa sổ chưa thử được vì chưa có trận.


## 17. 06/09 09:48 — CLIENT SẬP khi vẽ cửa sổ Tống Kim mới — GỐC: sprite nền do tôi ghi sai bảng màu
### 17a. Bằng chứng (jx_crash.log)
```
Ma loi : 0xC0000005 (Access Violation), DOC dia chi 0x69CAAE93
[00] TextureResSpr::LoadSprFile + 0x1F9   [TextureRes.cpp dong 522]
[01] TextureResSpr::LoadImageA           [02] TextureResMgr::LoadImageA
[04] KRepresentShell3::DrawImage2DFlat   [06] KWndImage::PaintWindow  [09] UiPaint
```
`TextureRes.cpp:522` = `m_pFrameInfo[i].nWidth = pFrame->Width;` với `pFrame = pSprite + pOffset[i].Offset`.
### 17b. Gốc
- `thongtin20.spr` (nền cửa sổ, tôi vẽ 05/09) khai `Colors = 256` nhưng **chỉ ghi 138 màu = 414 byte**: `pal_img.getpalette()`
  của Pillow CHỈ trả về số màu ảnh thật sự dùng, tôi lại cắt `[:768]` mà không đệm.
- `LoadSprFile` đọc bảng màu theo `Colors` (768 byte) → bảng `SPROFFS` lệch 354 byte, rơi vào giữa dữ liệu RLE →
  `Offset = 0x4B4B4B4B = 1.263.225.675` → con trỏ hoang → sập ngay khi cửa sổ được vẽ lần đầu.
- `bangdiem.spr` (04/09) không sập vì ảnh đó dùng đủ 256 màu — lỗi tiềm ẩn, chỉ lộ khi ảnh ít màu.
- Chỉ 1 lần sập hôm nay, đúng chữ ký này. Các lần sập 05/09 12:06–12:09 là `lua_pushinteger` (chuyển Lua 5.4, việc khác).
### 17c. Sửa
- `ReverseTools\tongkim_chat\ghi_spr.py` (MỚI): ghi SPR **luôn đệm bảng màu đủ 256×3** + `kiem_spr()` đọc ngược kiểm
  (bảng màu, `SPROFFS` trong tệp, đầu khung, RLE phủ đúng số dòng); ghi xong tự kiểm, sai thì ném lỗi chứ không tạo tệp hỏng.
- `ve_thongtin20.py` (MỚI): vẽ lại nền và ghi qua bộ ghi trên. Tệp mới 61.726 byte, `kiem_spr` OK, giải mã ngược ra đúng 221×268.
- `lam_bangdiem.py`: vá cùng lỗi để không tái diễn.
- Quét **3.028 tệp .spr rời** trong `bin\client`: không còn tệp nào hỏng cấu trúc (`Spr\item\citydefence\smallfragment.spr`
  bắt đầu bằng `FB "SPR"` = dạng nén, có từ 31/08, không liên quan).
### 17d. Triển khai
- **Chỉ cần khởi động lại client** (`ChoiGame.bat`): `.spr` là tệp rời, nạp lúc vẽ. `Game.exe` live đã đúng bản a136398e, không có `.moi` chờ.
- Bài học: **mọi tệp nhị phân tự sinh (SPR/PAK/ảnh) phải đọc ngược kiểm trước khi đặt vào cây chạy thật** — engine không kiểm biên,
  một tệp sai làm sập client ngay khi vẽ.
