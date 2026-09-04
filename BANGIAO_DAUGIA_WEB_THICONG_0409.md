# BÀN GIAO — ĐẤU GIÁ THẾ GIỚI cấu hình từ WEB ADMIN (04/09/2026, phiên `wauto-e7`)

Tài liệu này là bản bàn giao **toàn bộ** công việc ngày 04/09 của phiên `wauto-e7` quanh hệ ĐẤU GIÁ: buổi sáng sửa lỗi
đấu giá/thư (A23 → A38), buổi chiều làm tính năng **đấu giá thế giới do web admin cấu hình** (W1 → W8), kèm hai sự cố và cách
xử lý. Ba tài liệu liên quan, đọc theo thứ tự cần:

| Tài liệu | Nội dung |
|---|---|
| `BANGIAO_DAUGIA_WEB_0409.md` | **Giao kèo** web ↔ máy chủ: tên bảng, cột, cú pháp `award`, trạng thái, bố cục trang admin. Nguồn duy nhất về tên cột. |
| `BANGIAO_DAUGIA_THICONG_0409.md` | Thi công hệ đấu giá A+B+C (ký gửi / thế giới / bang hội) các ngày trước + mục 9 buổi sáng 04/09. |
| `BANGIAO_DAUGIA_CHIENLENH_0409.md` | Phân tích đấu giá 2.0 + Chiến Lệnh (nền thiết kế). |

---

## 0. Trạng thái lúc bàn giao (15:15 04/09) — đọc trước

- **Máy chủ đang chạy `CoreServer.dll = 8181a940`** (GameServer.exe khởi động 14:43:29; chủ đã chạy `ChayGameServer.bat`). Client
  đang chạy `CoreClient.dll b73372be` + `Game.exe 7e353a0c` (của wauto-55, không liên quan phần này). Khe `.moi` trống.
- **Đấu giá thế giới từ web ĐÃ CHẠY THẬT**: `hethong.log` 15:00:22 — `WEB dot 1 luc 15:00 04/09: 3 mon len san, 0 bo qua, nhom dang bat 5,
  het luc 180 phut nua` (auction_item id 35 Chân Nguyên Đơn (đại) 100/900 · 36 Võ Lâm Mật Tịch 100/1000 · 37 Lục Thủy Tinh 10/100, tiền
  Ngân lượng). Đợt kết thúc ~18:00, người giữ giá cao nhất nhận món qua thư.
- **Chiến Lệnh cần chủ khởi động lại máy chủ THÊM MỘT LẦN**: 7 bảng `st_*` bị kịch bản thử của phiên web xoá lúc 14:45:21 (sau lần khởi
  động 14:43); máy chủ chỉ tạo bảng một lần mỗi tiến trình nên hiện mọi ghi `st_*` rơi im. Sau khởi động lại: web bấm hai nút nạp mặc
  định + tạo lại mùa (mở 07/09, đóng 19/10, ân hạn 7, level_score 400, vip_bonus 1200, vip_price_xu 500, reset_hour 5). Không cần binlog.
- **SẬP 15:05:38** (dump `DumpInfo\g-v1.0-20260904-150538-7336419196.dmp`): 783 bot đăng nhập hàng loạt sau lần restart 15:03, AV
  trong `PB_OnRoleData → KPlayerSet::Add → KPlayer::LoadDBPlayerInfo` (KPlayerDBFuns.cpp:67, khối kỹ năng role-data bot LyMinh812 có
  offset rác 0x28000000). Không liên quan đấu giá web (mã web chỉ SELECT cfg lúc đó; diff so chienlenh chỉ 3 tệp đấu giá, không đổi
  struct). wauto-55 điều tra [BotLuu]. PDB: `Sources\Core\x64\Server Release\CoreServer.pdb` (thư mục có dấu cách). Máy chủ chạy lại
  15:08:42 vẫn 8181a940; **giữ khe trống tới khi rõ gốc**.
- Mã: nhánh `mail-0309` HEAD `224b2621`; 5 commit tính năng đã cherry-pick sạch lên **`origin/main` = `ab9cefaf`**. Mọi bản CoreServer
  sau này dựng từ `origin/main` là có đấu giá web; **nhưng phải gộp thêm `origin/chienlenh-0409`** (máy chủ Chiến Lệnh chưa vào main).

---

## 1. Yêu cầu của chủ và luật đã chốt

Chủ (04/09 ~13:30): *"đấu giá thế giới kết nối với mysql web; tôi sẽ viết phần admin"* → rồi: *"kết nối với phiên web và yêu cầu làm phần
admin đầy đủ để tránh 2 phiên làm khác nhau; tư vấn giao diện dễ hiểu; cho chọn danh sách item kèm giá, đến hẹn thì random item trong
nhóm đã config rồi cập nhật lên; như 3h cập nhật 1 lần; trong 3h ai giá cao nhất thì thắng, item gửi về thư; chạy phản biện tránh bug."*

Luật chốt (đã áp dụng):
- Web chỉ **cấu hình** (nhóm vật phẩm + lịch), **không** ghi `auction_item`, **không** tạo bảng. Máy chủ tự tạo bảng, tự dựng vật phẩm.
- Chu kỳ mặc định **180 phút**, mỗi đợt **3 món**, bốc **ngẫu nhiên theo trọng số, không trùng** trong một đợt.
- Kiểu Anh hai giá như ký gửi: khởi điểm = `start_price`, bước 10 %, `buy_price` tuỳ món (0 = không có Mua ngay), người giữ giá cao
  nhất không tự nâng. Hết giờ: có người trả → thư `aucitem:<id>` cho người thắng, **tiền vào hệ thống**; ế → thu hồi (không mất gì).
- "Mở đợt ngay" (web đặt `next_round = 0`): mở đợt mới, **đợt cũ không bị cắt**.
- Vật phẩm đặt tên người bán `@WEB`, tên phiên `Đợt N HH:MM dd/mm`.

Ba luật chủ có thể đổi sau, không phải đổi bảng: (a) cắt sớm đợt cũ khi mở đợt ngay — hiện KHÔNG; (b) chu kỳ mặc định; (c) Mua ngay tuỳ món.

---

## 2. Kiến trúc và luồng chạy

```
Web admin (PHP, DauGia.php)                MySQL jx1_game                     GameServer (CoreServer.dll + auction_manager.lua)
  ├─ ghi auction_web_pool (nhóm+giá) ──►  auction_web_pool  ◄── đọc lúc mở đợt (AUCWEB_Pool), ghi drawn_*/item_name/err
  ├─ ghi auction_web_cfg (lịch) ───────►  auction_web_cfg   ◄── đọc mỗi 30 s (AUCWEB_Cfg); giành đợt = UPDATE nguyên tử (ClaimRound)
  └─ đọc auction_item (lịch sử) ◄──────  auction_item      ◄── chèn dòng atype=2 seller='@WEB' (AUC_PutOn), AUC_Tick chốt/hết hạn
                                          mail              ◄── AUC_Settle gửi thư aucitem:<id> cho người thắng
Client: tab Thế giới đọc auction_item qua AUC_List (không cần vòng quét) → hiện "Đợt N ..." ; nhận thư như mọi phần thưởng khác.
```

Vòng quét: `AucWeb_Tick` (30 s, riêng) → `AucWeb_Body`: `AUC_Ready`/`AUCWEB_Ready` → `AUCWEB_Cfg` → `enabled=1` và `next_round<=now`
→ nhóm rỗng? (không giành, báo một lần) → `AUCWEB_ClaimRound(now, now+period)` → `AucWeb_Round`: `AUCWEB_Pool(2000)` → lặp tối đa
`nPer*4` lần: `AucWeb_Pick` (trọng số) → `AucWeb_PutOne`: kiểm giá/tiền → `AUC_MakeRec(award)` → `AUC_PutOn(...)` → `AUCWEB_Drawn`
(hoặc `AUCWEB_Err`) → xong lô: `AucWeb_Bao` (báo người đang xem, một lần) → `AUCWEB_Msg(last_msg)`; 0 món → `AUCWEB_SetNext(now+60)`.

---

## 3. Bảng dữ liệu (tóm tắt — chi tiết và luật ở giao kèo)

- `auction_web_pool` (web sở hữu): `award` (một mục `item:`/`gold:` cú pháp thư) · `label` (web tự dùng) · `currency` 1 lượng / 2 xu ·
  `start_price` · `buy_price` · `weight` 1..1000 (≤0 → 1) · `enabled` · máy chủ ghi `drawn_count` `drawn_time` `drawn_auc` `item_name`
  (TCVN3) `err` (ASCII, '' = ổn) · `ctime` `mtime`. Máy chủ đọc tối đa 2.000 nhóm đang bật mỗi đợt.
- `auction_web_cfg` (một dòng `id=1`, máy chủ INSERT IGNORE): `enabled` · `period_min` 10..1440 · `items_per_round` 1..30 · `next_round`
  (máy chủ đặt; web chỉ đặt 0) · `last_round` · `round_no` · `last_msg` (ASCII) · `mtime`.
- `auction_item` (có sẵn, không đổi lược đồ): dòng web có `atype=2`, `seller='@WEB'`, `activity='Đợt N HH:MM dd/mm'`, `kind=1`,
  `guaranteed_price=start`, `base_price=buy` (hoặc =start), `end_time` = giờ mở đợt kế. `state`: 0 đang đấu · 1 đã bán đang giao · 3 kết
  thúc (`buyer<>''` đã giao, `buyer=''` ế thu hồi). Không có 2. Khi state 0, `buy_price` chỉ là bản sao giá đang trả.
- CSDL: `jx1_game` (mục `[gamedb]` trong `bin\server\DataBase.ini`, MySQL 3306). Web nối bằng `root@127.0.0.1` (nợ kỹ thuật của web).

---

## 4. Mã máy chủ C++ (`Sources\Core\Src`)

| Tệp | Thay đổi |
|---|---|
| `KAuctionServer.cpp` | Khối `[DAUGIA-WEB 04/09]` chèn trước `LuaAUC_Ready`: `sEnsureWebTable` (DDL 2 bảng + INSERT IGNORE cfg, có kiểm kết quả), `sSetNum/sSetStr`, `LuaAUCWEB_Ready`, `LuaAUCWEB_Cfg`, `LuaAUCWEB_ClaimRound` (UPDATE … WHERE id=1 AND enabled=1 AND next_round<=? → affected_rows), `LuaAUCWEB_Pool` (LIMIT ≤ 2000, không lọc weight), `LuaAUCWEB_Drawn`, `LuaAUCWEB_Err`, `LuaAUCWEB_Msg`, `LuaAUCWEB_SetNext`, `sMakeRecFail`, **`LuaAUC_MakeRec`**. `LuaAUC_CountSeller` thêm `AND atype<>2`. Thêm `#include "KSubWorldSet.h"`, `<ctype.h>`. |
| `KAuctionServer.h` | Khai báo 9 hàm trên (trong `#ifdef _SERVER`). |
| `ScriptFuns.cpp` | Đăng ký 9 tên Lua sau `AUC_Rollback` (tệp CRLF + TCVN3 — sửa bằng python latin-1, neo phải theo CRLF). |

`AUC_MakeRec(szAward) → szHex, szTên, "g,d,p,l,s,k", nCells, nStack, szLỗi` — dựng vật phẩm THẬT không cần người chơi:
1. Đọc chuỗi: tiền tố `item:`/`gold:`, tối đa 12 số, không âm, không `;`. `item:` cần ≥ 6 số; `gold:` ≥ 1.
2. Từ chối: `lock`/`expSec` ≠ 0; genre ngoài `sCanRebuild` (0,1,4,5,6,8,9); văn cương `n>1`; level trang bị thường (detail ≤ ngựa) ngoài
   1..10; level thuốc ngoài 1..5; `n` > 9999.
3. Sinh vào `KItem` trên ngăn xếp (ZeroMemory) **đúng đường của hộp thư**: trang bị thường → `Gen_Equipment` 10 tham số (server tự bốc
   `uRandomSeed` → thuộc tính ngẫu nhiên mỗi đợt) + `SetMaxOptMultiply(genXOpt(luck))`; detail ≥ `equip_signet` (13-16) → overload 9 tham số
   `Gen_Equipment(NATURE_NORMAL, …)`; hoàng kim → `Gen_Equipment(NATURE_GOLD, row, 0,0,0, NULL, 0, ver)` (khuôn `AddItem2(2,0,record,0,0,0)`);
   thuốc/nhiệm vụ/hồi thành/bí kíp/văn cương/tinh thạch → `Gen_Medicine/Gen_Quest/Gen_TownPortal/Gen_MagicScript/Gen_Fusion/Gen_StarStone`.
4. Kiểm `w*h > 0` và tên ≠ '' (nhánh hoàng kim của Gen_Equipment trả TRUE cả khi dòng sai); kiểm xếp chồng bằng **`m_CommonAttrib.bStack`**
   (không dùng `nMaxStack` — thuốc/nhiệm vụ luôn 0); trần chồng = `nMaxStack` nếu > 0, không thì 500.
5. `ItemSet.AddI` → `sItemToRec` → `sRecToHex` → chép tên/ô/chồng → `ItemSet.Remove`; rồi **tự dựng lại lần hai** từ chính chuỗi hex
   (`sHexToRec` + `sRecToItem`) so tên/ô/chồng → khớp mới trả hex. Đây là bằng chứng món trao được sau này (đường `AUC_GiveRec` dùng đúng
   `sRecToItem`).

Chỗ dễ vỡ: `AUC_COLS` 22 cột khớp cứng — không thêm cột vào `auction_item`; `CREATE TABLE IF NOT EXISTS` không thêm được cột vào bảng đã có
(MySQL không có `ADD COLUMN IF NOT EXISTS`) → tính năng mới = bảng mới. Kết nối đồng bộ `pSync` không tự nối lại — vòng quét 30 s giữ nó sống.

---

## 5. Script Lua (bộ sinh `ReverseTools\mail\p12_daugia.py` → `bin\server\script\auction_house\auction_manager.lua`)

**Cấm sửa tay tệp `.lua` sinh ra** — sửa bộ sinh rồi chạy `python p12_daugia.py` (ghi cả cây chạy thật `E:\...` lẫn bản chụp
`serverscript_jx2\mail\...`). Từ W4, `p12` **không còn gọi `p3_lua.py`** (sinh `mailmanager.lua`) — xem sự cố mục 9.

Khối `[DAUGIA-WEB 04/09]` (đặt TRƯỚC khối AUCPOLL trong `build_manager()`):
- Hằng: `AUCWEB_SELLER="@WEB"`, `AUCWEB_FRAMES=30*18`, `AUCWEB_GLB=9003`, `AUCWEB_MAX_PRICE=2000000000`, `AUCWEB_POOL_MAX=2000`.
- `AUC_LaChuMon(r)` = `seller == GetName()` **và** `seller ~= "@WEB"` — dùng ở trả giá / mua ngay / rút món / `bMine` (tên `@WEB` tạo được
  làm nhân vật vì Goddess chỉ chặn byte ≤ 32).
- `AucWeb_LoiLua(sz)` = handler ghi "LOI Lua: <thông điệp>" (tham số 4 của `call`).
- `AucWeb_Kep`, `AucWeb_Pick` (trọng số; tổng > 30000 thì ghép hai `random` vì `random()` Lua 4.0 chỉ 32767 mức), `AucWeb_PutOne`,
  `AucWeb_Round`, `AucWeb_Bao` (bọc `AUC_NotifyNewItem`, trả 1), `AUC_GioHet`, `AucWeb_Body` (**mọi nhánh `return 1`** — `call(...,"x")`
  trả nil cả khi hàm không trả gì), `AucWeb_Tick` = `call(AucWeb_Body, {}, "x", AucWeb_LoiLua)`.
- Đăng ký timer kiểu nhịp tim (`AUCWEB_DANGKY` + `GlbValue 9003` cũ hơn 120 s → `AddTimer`).
- Thay đổi ngoài khối: `AucPoll_Tick` bọc `AUC_Tick` bằng `call(...,"x", AucWeb_LoiLua)` và `AUC_Tick` `return 1`; `AUC_OnRequestGetBack`
  chặn `seller == "@WEB"`; `AUC_FinishEnglish` dùng `AUC_Buy(r.id, r.buyer, r.cur)` thay `AUC_SetState(id,1,1)` (ghi luôn giá chốt).

Vì sao phải bọc `call`: `KJx2League.cpp` `KJx2ScriptTimer_Breathe` gọi callback bằng `lua_dostring`; lỗi Lua → không có trị trả về →
`bContinue = 0` → **timer bị xoá vĩnh viễn** (chỉ ghi `[PORT5] ScriptTimer LOI`), khối đăng ký lại chỉ chạy lúc nạp tệp → phải khởi
động lại máy chủ. Nhật ký 14:41-14:43 hôm nay ("LOI Lua trong AucWeb_Body" mỗi 30 s khi DLL cũ chưa có `AUCWEB_Ready`) chứng minh lớp bọc
giữ được timer sống.

Kiểm cú pháp/chạy thử offline: `ReverseTools\lua4\lua4.exe` (dựng theo `ReverseTools\lua4\README.md`, không commit) + bộ giả lập
`scratchpad\test_aucweb.lua` (9 kịch bản: mở đợt, chưa tới giờ, mở đợt ngay, tắt lịch, cfg nil, nhóm rỗng, tick, lỗi trong AUC_Tick được
đỡ, chặn rút `@WEB`). Chạy: `lua4.exe test_aucweb.lua` — mọi `AucWeb_Body` phải trả 1. Lỗi `attempt to call global 'Include'` khi chạy
thẳng tệp sinh = đã qua parse (lỗi chạy, không phải cú pháp).

---

## 6. Build và swap

- Worktree: `D:\GAMEDEVNEW_wt_mail`, nhánh `mail-0309`. Build: `python C:\Users\nguye\.claude\skills\swordonline-dev\scripts\build.py
  --config "Server Release" --platform x64` (repo này KHÔNG có "Server Release - US") → `Sources\Core\x64\ServerRelease\CoreServer.dll`.
- Trước build: `git fetch` + gộp `origin/main` **và `origin/chienlenh-0409`** (máy chủ Chiến Lệnh chỉ ở đó; wauto-55 đã đồng ý gộp
  `origin/main` về phía họ, nên từ giờ chỉ cần gộp `origin/main` + `origin/chienlenh-0409`).
- Dấu kiểm trong DLL (`grep -c -a`): của tôi `AUC_MakeRec` `AUCWEB_ClaimRound` `AUCWEB_SetNext` `auction_web_pool` `mon khong xep chong`
  `AND atype<>2`; Chiến Lệnh `SELECT v FROM st_config WHERE k='cfg_version'` `[CL]` `st_season` `CANH BAO moc %d: chua co thuong`; chung
  `node SimCity` `PB_SetNhapMon` `SapNpc` `TKDich` `BC-DEM` `DECHONG` `S13-DENY`.
- Swap: đặt `bin\server\CoreServer.dll.moi`, chủ chạy `ChayGameServer.bat`. Script `.lua` nạp MỘT lần lúc boot → đổi script cũng cần khởi
  động lại (riêng cây `timerserver.lua` tự nạp lại mỗi phút). Trước khi đè khe phải xem khe có bản của phiên khác không (sao lưu, nhắn).
- Commit: `git commit -- <tệp>` (pathspec) trong worktree; đẩy nhánh ngay; cherry-pick lên `origin/main` qua worktree tạm
  (`git worktree add D:\GAMEDEVNEW_wt_tmpmain origin/main` → cherry-pick → push `HEAD:main` → remove).

---

## 7. Kiểm thử đã làm và kết quả phản biện

Bộ giả lập Lua (mục 5) qua 9/9. Phản biện tự động (workflow `phanbien-daugia-web`, 4 lát cắt × 2 người xác minh mỗi phát hiện, 43 tác tử,
chỉ đọc) trên commit `7002737e`; mọi phát hiện được giữ đã sửa:

| Phát hiện (được giữ) | Sửa | Đợt |
|---|---|---|
| Bước "báo người xem" ném lỗi làm mất thông điệp đợt | `AucWeb_Bao` bọc `call` riêng | W3 |
| `p12` gọi lại `p3` sinh đè `mailmanager.lua` | `p12` chỉ chạy `p3` khi vừa vá | W4 |
| `random(1,n)` chỉ 32767 mức → trọng số lớn không bốc được | ghép hai `random` khi tổng > 30000 | W5 |
| `call(x)` nuốt thông điệp lỗi | tham số 4 `AucWeb_LoiLua` | W5 |
| Trần nhóm 500 im lặng | 2000 + cảnh báo `last_msg` | W5 |
| Bảng `err` trong giao kèo thiếu | web hiện nguyên văn; liệt kê đủ | W5 |
| Món không xếp chồng nhận `n>1`; văn cương `n>1` bị ép 1 im lặng | từ chối, `err` rõ (kiểm bằng `bStack` sau W8) | W6/W8 |
| Trang bị detail 13-16 không dựng được (overload 10 tham số thiếu case) | dùng overload 9 tham số | W6 |
| `INSERT IGNORE` dòng cfg không kiểm → bảng có mà không có cfg | kiểm, không đánh dấu sẵn sàng | W6 |
| `buy_price` chưa chắc là giá chốt khi hết giờ | `AUC_FinishEnglish` → `AUC_Buy` | W6 |
| Tên nhân vật `@WEB` tạo được → bị coi là chủ món | `AUC_LaChuMon` + `AUC_CountSeller` bỏ `atype 2` | W6/W8 |
| Nhóm cấu hình sai hàng loạt kéo dài một nhịp | tối đa `nPer*4` lần bốc | W6 |
| Nhóm rỗng vẫn giành đợt (đốt 3 giờ) | kiểm nhóm trước; 0 món → `SetNext(+60)` | W7 |
| Hai đợt cùng phút gộp một | tên có `round_no` | W7 |
| `weight ≤ 0` bị SQL loại im lặng | bỏ lọc, Lua kẹp | W7 |
| Level 0 dựng ra món khác | từ chối level ngoài bảng | W7 |
| **Hồi quy W6**: kiểm chồng bằng `nMaxStack` từ chối oan mọi thuốc/nhiệm vụ | dùng `bStack`, trần 500 | W8 |

Bị bác bỏ (không sửa): "món x N không chồng dùng một lần mất hết" (IsStack = nStackNum>1, trừ từng lần); "call(x) đặt `_ERRORMESSAGE`
= nil" (chỉ khi có tham số 4); "AUC_Bid ghi giá cũ" (MySQL gán SET trái→phải); "4 tệp protocol Chiến Lệnh lọt vào commit" (bản chụp cây
chạy thật, byte-identical).

---

## 8. Vận hành hằng ngày

- **Bật**: web đặt nhóm (`enabled=1`) rồi bật lịch (`enabled=1`, `next_round=0`). Trong ≤ 30 s: `hethong.log` có `[DAUGIA] WEB len san
  id … (nhom …)` và `WEB dot N luc …: X mon len san, Y bo qua, nhom dang bat Z, het luc …`; `auction_web_cfg.last_msg` cùng nội dung.
- **Không lên món**: xem `last_msg` (`nhom rong…` / `KHONG mon nao len san (xem cot err)` / `CANH BAO: chi doc 2000 nhom`), cột `err` từng
  nhóm (chuỗi ASCII, giao kèo mục 5), và log `LOI Lua: …` (thông điệp Lua thật). Nếu `next_round` nằm 0 quá 2 phút mà `mtime` không đổi →
  máy chủ không quét (DLL cũ, MySQL ngắt, hoặc timer chết) → xem `[PORT5] ScriptTimer … LOI` trong log, khởi động lại.
- **Kiểm CSDL nhanh** (chỉ đọc): `SELECT * FROM auction_web_cfg;` · `SELECT id,award,err,drawn_count FROM auction_web_pool;` ·
  `SELECT id,activity,item_name,cur_price,buyer,state,end_time FROM auction_item WHERE atype=2 AND seller='@WEB' ORDER BY id DESC LIMIT 20;`
- **Đổi luật** (chu kỳ, số món): web sửa cfg, hiệu lực nhịp kế. Đổi mã Lua: sửa `p12`, chạy `p12`, khởi động lại máy chủ. Đổi C++: build
  + swap.
- **Tuyệt đối**: không chạy kịch bản thử có CREATE/DROP trên `jx1_game` (dùng CSDL nháp như `jx1_game_dgthu`); không chạy `p3_lua.py`
  khi chưa đưa bản vá tay của wauto-55 (2712d95b) vào nó; không sửa tay tệp `.lua` sinh ra.

---

## 9. Hai sự cố hôm nay và bài học

**(1) 14:23 — `p12` sinh đè `mailmanager.lua` trên cây chạy thật** (tôi gây ra). `p12` từ trước luôn gọi `p3_lua.py`; `p3` không chứa bản vá
tay `2712d95b` của wauto-55 (`MailManager_QuaConLai`, 50 dòng) → tệp sống mất bản vá. Máy chủ chưa ảnh hưởng (script nạp lúc boot). Khôi
phục ngay từ git HEAD (`e3de9daf`), bản sinh lỗi lưu scratchpad, `p12` W4 không gọi `p3` nữa, đã báo wauto-55 (họ sẽ đưa vá vào `p3`).
Bài học: bộ sinh chỉ được ghi tệp mà nó là nguồn duy nhất; bản vá tay vào tệp sinh phải vào bộ sinh ngay.

**(2) 14:45:21 — 7 bảng `st_*` Chiến Lệnh bị DROP khỏi `jx1_game`.** Thủ phạm: kịch bản thử `scratchpad/thu_chienlenh.php` của phiên web
(CREATE 7 bảng đầu bài + `finally` DROP, viết từ lúc bảng thật chưa có, chạy lại không đọc lại). wauto-55 tìm ra qua chuỗi `thu nghiem: tu
choi nap - need_score khong tang dan` trong `st_cfg_log` ngay trước DROP; tôi đã rà phiên mình bằng dấu thời gian (0 lệnh MySQL). Thiệt hại:
chỉ khung + dữ liệu thử (award rỗng, ledger 0, 987 dòng st_player là tải thử, mùa id 2 active 0). Phục hồi: khởi động lại máy chủ (tạo lại
bảng) + web nạp mặc định + tạo lại mùa. Bài học: bộ thử CREATE/DROP chỉ chạy trên CSDL nháp; binlog đang bật (801 tệp) là lưới an toàn.

---

## 10. Phối hợp phiên

- **`webver5-eb`** (web): `loi/DauGia.php` + `giaodien/admin_daugia.php` xong (46/47 + 13/13 + 25/25 phép kiểm trên CSDL nháp), menu "Giao
  dịch"; tái dùng `Thu::locChuoiQua()`, bảng tra `D:\Webver5\data\tim_vatpham.php` (8.647 món + 7.385 hoàng kim, ảnh `lib\vatpham\*.png`);
  không CREATE bảng; `err` hiện nguyên văn; cảnh báo > 2.000 nhóm; `item_name` máy chủ là tên chính. Đã bật thử → đợt 1 lúc 15:00.
- **`wauto-55`** (Chiến Lệnh): máy chủ Chiến Lệnh chỉ trên `origin/chienlenh-0409`; đồng ý gộp `origin/main` (đã có 5 commit của tôi) cho
  các bản CoreServer sau và nhắn trước khi đè khe; đang dựng `khoiphuc_st.sql` (chủ so, không tự chạy); sẽ đưa vá 2712d95b vào `p3_lua.py`.
- **`wauto-9b`** (AC CHÍNH): 2 commit trên main (686231c0, affbc060), đã có trong DLL 8181a940.
- Luật chung đã thống nhất: cherry-pick lên `origin/main` ngay sau mỗi commit; kiểm dấu nhị phân trước khi đặt khe; sao lưu bản ở khe trước
  khi đè; CSDL nháp cho mọi bộ thử.

---

## 11. Còn mở / chờ chủ

1. Ba luật có thể đổi (mục 1). 2. Khởi động lại máy chủ cho Chiến Lệnh + web tạo lại mùa (mục 0). 3. Tên nhân vật `@WEB` tạo được — đã
chặn mềm; muốn chặn cứng thì Goddess `ClientNode.cpp:379` (kiểm tên) — chưa làm. 4. Tồn đọng cũ của hệ đấu giá (chưa quyết): `Mail_Send` trần
100 thư xoá thư cũ kể cả có đính kèm; vật phẩm cao > 3 ô tràn ô hiển thị; `AUC_Viewers` khe cũ; `AUC_TMP` khoá theo PlayerIndex; chưa gõ
được số tiền trả giá (chỉ nút +). 5. `auctionpoll.lua`/`mailpoll.lua` trên đĩa là xác chết (không ai Include) — `p12` vẫn sinh
`auctionpoll.lua`; đừng Include lại. 6. Mùa Chiến Lệnh #1 `active=0`, mở 07/09 — chủ bật khi muốn.

---

## 12. Bẫy kỹ thuật đã xác minh (để người sau khỏi dẫm)

- Timer script chết vĩnh viễn khi callback ném lỗi (mục 5). `GlbValue` = `static std::map` RAM MỘT tiến trình (`KJx2SharedStore.cpp:672-697`);
  chú thích "lưu vĩnh viễn" trong A29b là sai.
- `AUC_PutOnItem` (Lua) **cấm gọi từ timer**: timer không đặt `PlayerIndex` → `GetName()`/`RemoveItemByIndex` móc túi người chơi cuối cùng
  chạm state. `AUC_PutOn` (C++) chỉ đọc 18 tham số + `g_MySQLDB` → an toàn. `MailManager_SendMail:548` dùng `DynamicExecuteByPlayer(PlayerIndex)`
  → gọi từ vòng quét thì tiếng "ting" tới nhầm (thư vẫn lưu đúng).
- `MAILPOLL_LASTID` là biến RAM → dòng web ghi lúc máy chủ tắt bị bỏ qua vĩnh viễn (thư sống nhờ giao theo kiểu kéo). Đấu giá web dùng cột
  trạng thái trong CSDL, không mốc RAM.
- `operator=(MEDICINE/QUEST)` không gán `nMaxStack` (luôn 0); cờ đúng là `m_CommonAttrib.bStack`. `IsStack() = nStackNum > 1`.
- Lua 4.0 engine: số là double (an toàn tới 2^53); `random(a,b)` 32767 mức; `mod(a,b)` có; `call(f,{},"x",h)` trả nil khi lỗi (cả khi
  hàm không trả gì); số 0 là TRUE.
- `Gen_Equipment` 10 tham số (khuôn AddItem) không có case detail 13-16; 9 tham số (khuôn AddItem2) có; nhánh NATURE_GOLD trả TRUE cả khi
  dòng sai → phải kiểm `w*h>0` và tên. Chỉ số dòng = `particular*10 + level - 1` (detail ≤ ngựa) / `detail*5 + level - 1` (thuốc) → level 0 =
  món khác.
- `ScriptFuns.cpp` là CRLF + TCVN3: neo patch phải theo CRLF; ghi latin-1. Chú thích `//` sau literal trong lời gọi nhiều dòng nuốt phần còn
  lại của dòng (lỗi C2187/C1075 hôm nay).
- Repo này build với cấu hình `"Server Release"` / `"Client Release"` (không có hậu tố `- US`).

---

## 13. Buổi sáng 04/09 (A23 → A38) — tóm tắt, chi tiết ở `BANGIAO_DAUGIA_THICONG_0409.md` mục 9

- Thể thức hai giá cho cả ba thẻ (A34): `guaranteed_price` = giá cơ bản/khởi điểm, `base_price` = mua ngay, `cur_price` = 0 khi chưa ai
  trả, bước 10 % giá cơ bản, trả ≥ mua ngay = chốt; A38 người giữ giá cao nhất không tự nâng; A36 bang chủ rút được món phiên bang;
  A37 chặn `nBaseIn` 0 (Lua 0 là TRUE).
- A33: gốc "vòng quét chưa từng chạy" = `sFindScriptIdxByState` lặp `i < nCurrentScriptNum` bỏ sót script đang nạp → mọi `AddTimer` ở thân
  chunk trả 0; đã vá C++ (`KJx2League.cpp`), sau khởi động lại có dòng `HET HAN`.
- Thư: thu nhỏ vật phẩm nhiều ô về một ô bằng `DrawSpritePartAlpha` (Represent3 `DrawImage2DStretch` nhánh SPR) chỉ trong thư
  (`ResizeBigItem=1` ở `mail_award_item.ini`); đấu giá giữ nguyên cỡ (ô 58×78, `m_IconBg.Hide()`); số tiền viết theo vạn (không "ức"); mô tả
  món qua `AUC_RecDesc(rec)` (hoàng kim nhận dạng `nNature=NATURE_GOLD + nRow`). Chủ chốt: *"phần mail đã oke rồi giờ không chỉnh nữa"*.
- Bản client tương ứng đã swap trước 13:30 (mọi việc client ≤ commit `f77588fe` trên `origin/main`).

---

## 14. Checklist cho phiên kế tiếp

1. Chủ khởi động lại máy chủ → kiểm `SHOW TABLES LIKE 'st_%'` có 7 bảng → web nạp mặc định + tạo lại mùa → Chiến Lệnh hiện.
2. Xem đợt 1 chốt lúc ~18:00: log `BAN id 35..37` / `LUU PHACH`, thư `aucitem:` tới người thắng, `state` 3, đợt 2 mở ngay sau.
3. Nếu chủ đổi luật (a)(b)(c) mục 1: (a) cần sửa Lua `AucWeb_Round` (kết thúc sớm dòng `@WEB` state 0 bằng `AUC_SetPrice(...end=now)`) —
   chưa có; (b)(c) chỉ cấu hình web.
4. Đưa vá 2712d95b vào `p3_lua.py` (wauto-55) trước khi bất kỳ ai chạy `p3`.
5. Bộ nhớ phiên: `memory\jx1-daugia-web-0409.md`, `jx1-daugia-thicong-0409.md`.
6. `[DAUGIA-CHAT]` (mục 15): khởi động lại máy chủ (script mới) + swap DLL có `AUC_MsgTong`; kiểm trong game: đợt web mở → 1 + N dòng "Hệ Thống"; trả giá phiên bang → dòng "Tin bang" nằm trong thẻ **bang** (DLL cũ thì nằm khung chính).

---

## 15. `[DAUGIA-CHAT 04/09]` — Thông báo mỗi thay đổi đấu giá vào KÊNH CHAT (phiên `wauto-4a`, 15:40-16:20)

Chủ: *"viết thêm thông báo mỗi khi thay đổi đấu giá trên thế giới vào kênh chat - đấu giá bang vào kênh chat bang; gôm thông tin để
người chơi biết"*. Mẫu câu và bảng sự kiện: giao kèo mục 9. Commit `mail-0309` **c8264cf5** → `origin/main` **4f7f2e2e**.

**Vì sao phải đụng C++ (chỉ cho phần bang).** Khung chat client xếp dòng theo `channelid` trong gói `chat_channelchat`
(`GameSpaceChangedNotify.cpp KClientCallback::ChannelMessageArrival` → `KUiMsgCentrePad::NewChannelMessageArrival`: id đã kích hoạt
→ khung chính + phòng theo `ResourceIndex` (6 = **Bang**, 4 = Khác, 3 = Phái, 1 = Phòng); `channelid = -1` → kênh GM/Hệ thống).
Kênh bang tên `\O<tong name id>` (`UiMsgCentrePad_Left.ini [CH_TONG] FormatName=\O<Tong#>`), **id do S3Relay cấp bằng bộ đếm tăng dần**
(`ChannelMgr.cpp GenChannID`, đổi mỗi lần relay chạy) → GameServer không tính được. Nó chỉ *đi ngang* GameServer trong gói
`playercomm_s2c_notifychannelid` trên đường xuống client (`KNewProtocolProcess::P_ProcessPlayerCommExtend`, chính chỗ bot học kênh thế
giới bằng `PB_GhiNhoKenh` — nhưng bảng bot chỉ 24 ô, không tra theo bang). Nhật ký `[BotKenh] thay kenh "\O305419896" id=81` xác nhận.
Hệ quả: có thành viên online là có id (client nào cũng hỏi kênh bang lúc vào game); chưa ai vào thì cũng không ai cần nhận.

**Mã C++** (`Sources/Core/Src`, 84 dòng): `KAuctionServer.cpp` khối `[DAUGIA-CHAT]` — `s_mapKenhBang` (tong id → channel id),
`AUC_GhiNhoKenhBang(szKenh, dwId)` (chỉ nhận `\O` + toàn chữ số), `LuaAUC_MsgTong(nTong, szMsg)` (lặp `PlayerSet`, thành viên
`m_cTong.GetTongNameID() == nTong` → `KPlayerChat::SendSystemInfo(1, idx, "Tin bang", msg, len ≤ 250, channelid|-1)`, trả số người
nhận; **không cần PlayerIndex** — `Msg2Tong` gốc đòi PlayerIndex hợp lệ đang trong bang nên không gọi được từ vòng quét),
`LuaAUC_KenhBang(nTong)` (đối chiếu). `KAuctionServer.h` khai báo; `ScriptFuns.cpp` đăng ký `AUC_MsgTong`/`AUC_KenhBang` sau
`AUCWEB_SetNext`; `KNewProtocolProcess.cpp` thêm một dòng gọi `AUC_GhiNhoKenhBang` ngay sau `PB_GhiNhoKenh` (trong `#ifdef _SERVER`,
chỉ đọc gói). Client không đổi (đã dựng Client Release Win32 kiểm compile: PASS).

**Lua** (`p12_daugia.py` → `auction_manager.lua`, khối `[DAUGIA-CHAT]` trước phần "nhận từ client"): `AUC_ChatGui(nType, nTong, sz)`
(thế giới → `Msg2SubWorld`; bang → `AUC_MsgTong`, DLL cũ → `Msg2Tong(nIdx, nTong, sz, -1)` qua `AUC_TimThanhVienOnline` = duyệt
`TONG_GetFirstMember/TONGM_GetOnline/TONGM_GetName` + `FindPlayer`), `AUC_ChatCat` (≤ 200 byte), `AUC_ChatThu(f, tbArg)` =
`call(f, tbArg, "x", AucWeb_LoiLua)` (mọi hàm báo `return 1`; lỗi trong câu chữ chỉ ghi log), `AUC_GioDongHo(nT)` ("HH:MM" — `GetLocalDate`
chỉ định dạng giờ hiện tại nên cộng chênh lệch), `AUC_ConLai(nEnd)`, `AUC_GiaTien`, `AUC_ChatHuongDan`, và các câu:
`AUC_ChatLenSan` (cuối `AUC_PutOnItem`), `AUC_ChatTraGia` (cuối `AUC_OnRequestOfferEnglish`), `AUC_ChatBan` (cuối `AUC_Settle`,
tham số mới `bMuaNgay`: 1 từ `OfferDutch`, 0 từ `FinishEnglish`), `AUC_ChatE` (`AUC_Expire` cả hai nhánh), `AUC_ChatRut`
(`AUC_OnRequestGetBack`), `AUC_ChatHaGia` (`AUC_Tick` nhánh Hà Lan), `AucWeb_ChatDot` (`AucWeb_Round`; `AucWeb_PutOne` trả thêm
`szName, nStart, nBuy, nCur` để gom `tbBao`). `AUC_ChatLoai` lọc: chỉ atype 1 (bang) và 2 (thế giới).

**Kiểm thử**: `ReverseTools\mail\test_aucchat.lua` (chạy `lua4.exe -s1024 test_aucchat.lua`, đọc kết quả bằng `vn_edit.py --read`):
đợt web 3 món, bang chủ lên sàn, GM Hà Lan, trả giá bang/thế giới, mua ngay (hoàn người giữ giá), hết giờ thắng (+ quỹ bang), ế thu hồi /
trả về, rút món, ký gửi im lặng, đường DLL cũ (`Msg2Tong` qua thành viên online; bang không ai online → im), cắt 200 byte, lỗi trong
hàm báo không phá giao dịch — 18 dòng chat đúng mẫu.

**Bẫy mới ghi nhận**
- `lua4.exe` KHÔNG `-s` = `lua_open(0)` = `DEFAULT_STACK_SIZE` = **128** (`llimits.h` sửa từ 1024; README lua4 nói 1024 là sai); máy chủ
  `KLuaScript::Init → Lua_Create(0)` cũng 128. Harness stub nặng (AUC_PutOn 18 tham số + bảng 21 trường) cần `-s200`; đo bằng stub nhẹ
  (`test_stack.lua`): cả đợt web + `AUC_Tick` chỉ cần 60 khe, **có hay không có** phần chat đều 60 → không thêm rủi ro tràn.
- `Msg2Tong(nTong, msg)` gọi 2 tham số (script cũ `yandibaozang`) → `nChannelID = 0` = kênh **GM** (id 0 theo `[BotKenh] thay kenh "GM" id=0`);
  muốn "Hệ thống" phải truyền `-1` tường minh. `Msg2SubWorld` = `SendSystemInfo(0,0,"Hệ Thống")` → mọi người, không cần PlayerIndex.
- Khe `.moi` bị phiên khác đặt lúc 15:54 (b67f002f, không có `AUC_MsgTong`) trong lúc tôi làm → không đè, nhắn `wauto-4c`. Họ xác nhận
  bản đó chỉ gồm commit đã push (main 9fb1f175 "gỡ ROLECHK" + chienlenh a18b5eb8) → **16:25 khe = f576ac9e** (gồm gỡ ROLECHK 9fb1f175 của
  wauto-go-rolechk), kiểm trước khi đặt: `RoleChk` = 0 (bắt buộc — = 1 là bot lại không gọi được), còn đủ `CL_Cong` `st_ledger`
  `AUCWEB_ClaimRound` `SapNpc` `TAT che do vao phai` `TKDich`; bản cũ lưu `CoreServer.dll.moi.bot_b67f002f`. Gốc sập 15:05 theo họ:
  không phải lỗi bot — gói của bot PhanNghia813 bị ghép vào thông điệp LyMinh812 ở tầng truyền Goddess→GameServer
  (`BANGIAO_ROLECHK_GO_0409.md` của wauto-4c).
- GameServer đang chạy khởi động 15:30 với script CŨ (p12 sinh lại lúc ~15:50): thông báo chỉ xuất hiện sau lần khởi động kế.
