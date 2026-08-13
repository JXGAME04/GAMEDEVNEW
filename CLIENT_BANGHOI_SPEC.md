# DAC TA THI CONG CUA SO BANG HOI JX2 TREN CLIENT JX1

> Lap 13/08/2026. Server da xong 100% (150 TONG_/TONGM_ + 29 TWS_ + timer).
> Tai lieu nay la ban do day noi DA KHAO SAT THAT de dung 17 cua so client.
> Ban thiet ke bo cuc JX2: `clientui_jx2/ui3_1024/` + `ui3_800/` (30 file .ini).
> Anh SPR: 64/67 CO SAN trong pak client (xem `tools/anh_banghoi_co.txt`), 3 file thieu ve lai.

## 1. KIEN TRUC CLIENT JX1 (da xac minh tung diem)

| Thanh phan | Vi tri | Ghi chu |
|---|---|---|
| Exe client | `Sources/S3Client/S3Client.vcxproj` | link Core client (Win32) + Engine + Represent2 |
| Cua so tong hien co | `Sources/S3Client/Ui/UiCase/UiTong*.{h,cpp}` (5 cua so) | mau chuan: `UiTongManager.cpp` (1791 dong) |
| Scheme ini cua so | `bin/client/Ui/Ui3/UiTongManager.ini` | thu muc scheme = `bin\client\Ui\Ui3\` (file roi, KHONG trong pak) |
| Khung control | `KWndImage` cha + `KWndLabeledButton/KWndText32/KWndList2/KWndEdit512/KWndScrollBar` | khai bao thanh vien + `LoadScheme(ini)` |
| Mo cua so | `UiShell.cpp:891`, `UiPlayerBar.cpp:766` (nut bang hoi), `UiGame.cpp:292` | `KUiTongManager::OpenWindow(...)` |

## 2. DUONG DU LIEU CLIENT <-> GS (da xac minh)

### Client -> GS (lenh)
- Struct: `STONG_PROTOCOL_HEAD { WORD m_wLength; BYTE m_btMsgId; }` + than lenh.
- `m_btMsgId` = `enumTONG_COMMAND_ID_*` (`Headers/KProtocolDef.h:492-513`, them moi TRUOC `enumTONG_COMMAND_ID_NUM`).
- Ben gui: cac ham client trong `Sources/Core/Src/KPlayerTong.cpp` (phia `#ifndef _SERVER`).
- Ben nhan GS: `KSwordOnLineSever::ProcessPlayerTongMsg` (`Sources/MultiServer/GameServer/KSOServer.cpp:4016`) — switch theo `m_btMsgId`, chuyen vao Core qua `m_pCoreServerShell->GetGameData(SGDI_TONG_*, ...)`.

### GS -> Client (dong bo)
- Goi co `ProtocolType = s2c_extendtong` + `S2C_TONG_HEAD { ... BYTE m_btMsgId; }`,
  `m_btMsgId` = `enumTONG_SYNC_ID_*` (`KProtocolDef.h:470-488`, them TRUOC `_NUM`).
- GS gui: `m_pServer->PackDataToClient(nNetID, &sSync, len)`; lay netid:
  `m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, nPlayerIdx, 0)`.
- Client nhan: `KProtocolProcess::s2cExtendTong` (`Sources/Core/Src/KProtocolProcess.cpp:4254`)
  -> switch `m_btMsgId` -> bao len UI bang `CoreDataChanged(GDCNI_*, ptr, n)`
  (cau Core->UiShell; UiShell dispatch toi cua so, xem cach `TongInfoArrive` duoc goi).

## 3. NGUON DU LIEU JX2 TREN GS

- `g_TongJX2` (`Sources/Core/Src/KTongJX2.h`) — ban sao DAY DU moi bang:
  field map (48 he thong + nhiem vu + tac phuong 20000+/30000+), thanh vien
  (figure/sex/field/quyen), thong bao `szAnnounce`. GS TRA LOI CLIENT TUC THI
  tu ban sao — khong can cho relay.
- Lenh GHI tu client: kiem quyen tren ban sao (`TONGM_CheckRight` logic, bang
  ID quyen muc 4.3 tai lieu phan tich) roi `g_NewProtocolProcess.PushMsgInTong`
  cac goi `enumC2S_TONG_JX2_*` (da co du: FIELD/MONEY/MEMBER_FIELD/RIGHT/STRING/TONG_OP).
- Quyen gop tien that: GS tru tien `KPlayer` (xem `LuaPlayerPayMoney` lam mau)
  roi push AddBuildFund/AddMoney/AddOffer — mau day du trong `tools/tongjx2_menu.lua`.

## 4. KE HOACH 17 CUA SO -> 6 LOP JX1 (moi lop 1 trang JX2 chinh)

Thu tu thi cong (uu tien theo gia tri):
1. `KUiTongJX2Main` — cua so chinh + tab (thay the vai tro 帮会主界面.ini):
   trang THONG TIN (帮会主界面信息页面): ten/cap/exp/tien 64/7 quy/ngay-tuan/thong bao.
2. Trang THANH VIEN (帮会列表界面): danh sach phan trang tu ban sao GS
   (ten, chuc vu, cong hien khoa 7, online) + nut duoi (quyen 1901) + doi chuc.
3. Trang QUYEN HAN (帮会权限分配页面.ini — 11KB, marquee JX2): luoi 12 quyen
   x truong lao; cap/thu (chi bang chu/1000 -> dich figure 1 — dung 4.4).
4. Trang TAC PHUONG (帮会改造系统.ini): 7 khu, lap/mo/dong/nang (quyen 9001).
5. Trang THONG BAO + CHIEU MO (帮会招募文案页面): xem/sua announce (bang chu).
6. `KUiTongJX2Grant` (帮会发钱界面): phat ngan luong/cong hien (bang chu).

Cua so JX1 giu nguyen: tao bang (`UiTongCreateSheet` — flow SGDI_TONG_APPLY_CREATE
hoat dong san), doi chuc danh, mat khau.

## 5. GIAO THUC MOI CAN THEM (client-view)

```
// KProtocolDef.h (them TRUOC _NUM moi enum):
enumTONG_COMMAND_ID_JX2VIEW,   // {head, BYTE btPage, DWORD dwParam} xin du lieu trang
enumTONG_COMMAND_ID_JX2OP,     // {head, BYTE btOp, DWORD dwTarget, int nP1, int nP2}
                               //  op: 0 kick 1 addright 2 delright 3 setann(text rieng)
                               //  4 donate 5 ws_add 6 ws_open 7 ws_close 8 ws_up
                               //  9 setstunt 10 upgrade 11 degrade 12 grant(tien/offer)
enumTONG_SYNC_ID_JX2,          // bien dai: {S2C_TONG_HEAD, btPage, payload trang}
```
- GS: `ProcessPlayerTongMsg` case JX2VIEW -> `GetGameData(SGDI_TONG_JX2VIEW,
  (intptr_t)bufOut, MAKELONG(page,playerIdx))` -> Core dung payload tu g_TongJX2
  -> GS PackDataToClient. Case JX2OP -> `GetGameData(SGDI_TONG_JX2OP, ...)`
  -> Core kiem quyen + push relay + tra ma loi -> GS gui sync bao ket qua.
- Core: them 2 op SGDI (dinh nghia canh `SSOI_TONG_JX2_SYNC` trong
  `CoreServerShell.h`, NGOAI enum, truoc `#ifndef CORE_EXPORTS`).
- Client: `s2cExtendTong` case `enumTONG_SYNC_ID_JX2` -> parse -> struct view
  -> `CoreDataChanged(GDCNI_TONG_JX2, ...)` -> UiShell -> `KUiTongJX2Main::DataArrive`.
  GDCNI_* khai bao o dau: xem cac GDCNI_TONG hien co (grep `GDCNI_` trong
  `Sources/Core/Src/CoreShell.h` / `KUiDataDef.h`).

## 6. LUAT KY THUAT BAT BUOC

- File .cpp moi trong Core: `#include "KCore.h"` DAU TIEN (PCH "Use"), boc `#ifdef _SERVER`
  cho phan server / `#ifndef _SERVER` cho phan client trong cung file neu can.
- File .cpp moi trong S3Client: khong PCH dac biet (xem cac UiTong* — include truc tiep).
- File GBK/TCVN chi sua bang `safe_edit.py`; file moi tu viet = ASCII thuan.
- Them enum CHI o CUOI (truoc `_NUM`); GS + Relay + Client trien khai DONG THOI.
- Scheme ini moi: mau theo `bin/client/Ui/Ui3/UiTongManager.ini` (format JX1),
  toa do/bo cuc tham chieu `clientui_jx2/ui3_1024/*.ini` (format JX2 — CHI de doc so).
- Dang ky compile: `S3Client.vcxproj` (ClCompile/ClInclude, mau `<ClCompile Include="Ui\UiCase\UiTongManager.cpp">`).
- Build kiem: Core "Client Release|Win32" + S3Client "Release|Win32" + (server khong doi van build lai de chac).

## 7. VIEC TAO BANG + SCRIPT TRON GOI (server da san sang)

- Tao bang: flow JX1 nguyen ban (NPC -> UiTongCreateSheet -> APPLY_CREATE ->
  relay Create -> `OnClientConnectCreate` dump JX2). Sau tao: goi `TONG_ApplyInit`
  tu script de reset du lieu JX2 (mau trong `tongjx2_menu.lua`).
- Script NPC quan ly day du: `tools/tongjx2_menu.lua` (8 nhanh, da trien khai
  `bin/server/script/test/`). Co the dung lam NPC chinh thuc den khi cua so xong.
