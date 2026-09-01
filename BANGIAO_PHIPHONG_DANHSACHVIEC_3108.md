> **Nguon:** ban tong hop cua dot mo 12 tac tu ngay 31/08 (5 chieu do + 5 phan bien doc lap
> + soat sot + tong hop). 40 khoang cach do duoc, **28 dung vung / 12 bi bac bo**, 31 gap bo sot
> moi. 2,45 trieu token, 841 luot tra cuu ma, 0 tac tu loi.
>
> **LUAT DUNG TEP NAY:** dot mo nay de ra **3 duong tinh gia nghiem trong**, ca ba deu **tu nhan
> la "chan han"** va deu sai vi cung mot loi tra cuu: `CPP-02`, `CPP-03`, `DATA-01`. Nguoi phan
> bien con nan lai don thuoc cua `INLAY-01` (ban goc **gay hai** - se cam kham hoan toan phi phong
> moi tinh), `UI-02` (tung dong xu), `UI-03`, `SEC-01`, `MODE-01`, `DATA-08` (viec de xuat la CO HAI).
>
> ⇒ **Khong thi cong theo bat ky muc nao ma chua tu mo tep doc lai neo.**
>
> Viet khong dau vi la ban do may sinh. Doc kem `BANGIAO_PHIPHONG_TIEPTUC_3108.md` — ban tom tat
> co dau, chi giu nhung gi da tu kiem chung bang tay.

---

# BANG GIAO PHI PHONG — DANH SACH VIEC CO THU TU (ban gop, da tham dinh)

Pham vi: he PHI PHONG / AN / TRANG SUC / MAT NA port tu Linux (JX2/Kiem The) sang JX1.
Tat ca neo duoi day da qua tham dinh doc lap; nhung khang dinh khong tru duoc da bi bo hoac danh dau ro.
Nhan muc: **[B]** = bat buoc de dat "giong 100% Linux" hoac chan lo hong that · **[T]** = tien nghi/my quan · **[Q]** = phai co quyet dinh cua chu game truoc.

---

## BANG TOM TAT DOT THI CONG

| Dot | Noi dung | Build | Swap |
|---|---|---|---|
| 0 | Luat lam viec + chuan bi | khong | khong |
| 1 | 9 viec Lua thuan — loi DANG XAY RA | khong | khong |
| 2 | BUILD CORESERVER #1: ITEM-01 (lo hong kinh te) + don phien | CoreServer | CoreServer.dll |
| 3 | Du lieu + tiem + nguon roi + mask.txt | khong | khong |
| 4 | Nhanh AN (lech 1 cap + loi vao) | khong | khong |
| 5 | BUILD CLIENT #1: UiMantleInlay + UiAffairItem (khong doi giao thuc) | S3Client | Game.exe |
| 6 | CHON LO KHAM: server + client + Lua, swap dong thoi | CoreServer + S3Client | CoreServer.dll + Game.exe |
| 7 | Dong bo m_nPfPack sang client (doi khuon goi tin) | CoreServer + CoreClient + S3Client | ca ba, dong thoi |
| 8 | Quyet dinh / ghi chep / no con lai | khong | khong |

Duong dan viet tat trong ban nay:
- `SV\` = `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\`
- `CL\` = `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\`
- `SRC\` = `D:\GAMEDEVNEW\Sources\`
- `LX\` = `D:\ServerLinux\`
- `head.lua` = `SV\script\global\mantlesystem\mantleupgrade_head.lua`
- `npc.lua` = `SV\script\global\mantlesystem\mantleupgrade_npc.lua`

---

## DOT 0 — LUAT LAM VIEC + CHUAN BI (khong sua ma)

**0.1 [B] Luat TCVN3.** Moi chuoi Viet moi trong `.cpp` phai CHEP BYTE tu chuoi da co san trong cay, khong tu nho bang ma. "Lo kham" dung la `"L\347 kh\266m"` (neo: `SRC\Core\Src\KItem.cpp:253` — `"<color=Green>%d sao L\347 kh\266m tr\350ng<color>"`; kiem cheo bang `od` tren `CL\Ui\Ui3\<ten TCVN3>.ini` dong 413 = "Moi lo kham", dong 349 = "Kham nam"). **CAM** dung `\307` va `\271` — do la `ầ` va `ạ` (byte cua "Th\307n Th\271ch"), ghep ra "Lầ khạm".

**0.2 [B] Luat grep.** CAM ket luan "khong co noi goi nao" bang `grep -rn --include=*.cpp`. Mau nay phan biet HOA/thuong (bo qua `Core\Src\KBasPropTbl.CPP`) va da tai lap duoc viec bo sot IM LANG `Core\Src\KItemGenerator.cpp`. Hai ket luan "chan-han" (DATA-01, DATA-06) da bi bac bo chi vi loi nay. Ke tu gio: grep khong loc, hoac liet ke ro danh sach tep da quet.

**0.3 [B] Luat Lua_State.** JX1 = MOI tep `.lua` MOT Lua_State (`SRC\Core\Src\ScriptFuns.cpp:2213-2216`: `curpack`/`usepack` la stub tra 0, `LuaCurPack` :2217, `LuaUsePack` :2223; `Include` = `lua_dofile` vao state NGUOI GOI, `ScriptFuns.cpp:2051`). Moi tep kieu "dang ky handler/menu" cua Linux la MA CHET tru khi tung diem su dung `Include` no tuong minh. Truoc khi noi "da noi xong", chay `grep -rn <ten tep> SV\script`. Phep thu re nay da bat duoc EVT-02.

**0.4 [B] Luat thu tu swap.** DLL di TRUOC hoac CUNG LUC voi tep `.lua` goi ham binding moi. Nguoc lai = goi mot global nil = ScriptError im lang (engine nap san tung `.lua` luc khoi dong). Ap dung truc tiep cho DOT 6.

**0.5 Da xac nhan — khong can nghi ngo.** Nhi phan dang chay DA co ma phi phong: `CoreServer.dll` (31/08 12:06) chua chuoi `"%d sao "` (PF_StarPrefix, `KItem.cpp:205`) 3 lan; ban `CoreServer.dll.truoc_phiphong_2908` co 0 lan. `KItemGenerator.cpp` sua 29/08 18:17 < ngay build.

**0.6 Cong cu nghiem thu da co san.** `SV\script\item\test_phiphong_admin.lua` (492 dong): phat du 9 nguyen lieu 4881-4889, chuoi hoang kim 5374/5375/5376/5377/5378/3485, chuoi bach kim 4835-4839, ep san do 10 sao kham day, `Include` wuxingyin.lua (:25) va goi `WXY_MoBang()` (:461-465), phat da kham `AddItem(9,1,i,...)` (:188). **KHONG port them cong cu thu tu Linux** (`mantle_test_npc.lua` 33 dong khong them duoc gi).

---

## DOT 1 — LUA THUAN, LOI DANG XAY RA (khong build, khong swap)

### 1.1 [B] Dem nguyen lieu bi binh phuong vi hop giao do khai trien chong
- **Tep:** `head.lua:304` (tang sao), `head.lua:429` (dot pha thuong), `head.lua:625` (dot pha lan 2).
- **Linux:** cong `GetItemStackCount(tbItemIdx[i])` — dung, vi danh sach giao do ben Linux liet ke moi CHONG dung mot lan.
- **JX1:** `SRC\Core\Src\KJx2WarInfra.cpp:862-869` KHAI TRIEN chong (`int nUnits = Item[p->nIdx].GetStackNum(); for (u...) s.vItems.push_back(p->nIdx);`), con `LuaGetItemStackCount` (`KJx2WarInfra.cpp:297-318`) tra N moi lan ⇒ **N binh phuong**.
- **Sua thanh:** ca 3 dong doi thanh `self.nStarCount = self.nStarCount + 1;`. **KHONG dung vao** `head.lua:714` (`elseif GetItemStackCount(tbItemIdx[i]) > 1`) — dong nay VAN DUNG duoi ngu nghia khai trien. Giu nguyen `RemoveItemByIndex` o `:354/:488/:674` (engine tru 1 don vi moi lan, dung).
- **Hai tang trieu chung:** (a) tang sao — `nSuccessRate = nRatePreStar * nStarCount` (`head.lua:324-325`), khoi kiem >101% bi chu thich (`npc.lua:277-280`) ⇒ 1 chong 10 Tinh Ngoc cho ti le GAP 10 LAN ma chi ton 10 vien = **lo hong kinh te**; (b) dot pha — `head.lua:441-443` va `:639-641` so `nStarCount ~= nNeedItem` ⇒ 1 chong 2 Thien Tinh Ngoc ra 4 ≠ 1 ⇒ **chan han duong dot pha**, bao "da dat vao 4 cai" trong khi nguoi choi cam 2 vien. Bang chung chong: `SV\settings\item\magicscript.txt` dong 4884 nMaxStack=50, dong 4886 nMaxStack=100, dong 4883 = 200.
- **Nghiem thu:** (a) dat 1 chong 10 Tinh Ngoc vao hop tang sao → ti le hien ra phai la `10 * nRatePreStar`, khong phai `100 *`; (b) dat 1 chong 2 Thien Tinh Ngoc vao hop dot pha thuong → bao "da dat vao 2", va dat dung 1 vien tach ra thi qua duoc.
- **Ghi ban giao:** sau khi sua, ma so LECH so voi ban Linux. Day la he qua bat buoc cua viec muon hop giao do JX1.

### 1.2 [B] Tran o chuoi Value1 (256 byte) — cat cut mo ta o hai muc menu
- **Tep:** `npc.lua:71-73` (onBreakThroughPro, chuoi 285 byte — **muc menu DANG SONG**), `npc.lua:107-109` (onBreakThroughWuJi, 298 byte).
- **JX1:** `S2C_GIVE_BOX::Value1[256]` (`SRC\Core\Src\KProtocol.h:1940`); `sWStrCpy` (`KJx2WarInfra.cpp:36-40`) lam `strncpy(nDstSize-1)` roi cam NUL ⇒ cat o byte 255, tai `KJx2WarInfra.cpp:828` va ban sao `:2414`.
- **Sua thanh:** rut ngan hai chuoi mo ta xuong ≤ 255 byte.
- **Nghiem thu:** mo menu "Phi Phong dot pha" → cau cuoi trong hop phai het cau, khong cut giua tu.
- **Ban dai han (chi khi chu game doi):** noi Value1 len 512, doi ca hai dau, swap dong thoi. Cung ho voi bay da ghi trong bo nho "Say/SayEx tran 512 byte".

### 1.3 [B] Shim tu xep da chon nham LO 0 SAO
- **Tep:** `npc.lua:228-236` (trong `doMantleMosaicStoneBox`).
- **JX1 dang lam:** chi xet `(not tbHasStone[i]) or tbHasStone[i] == 0` — khong goi `GetStoneLevelOnEquip`. Da vao lo 0 sao thi `KItem.cpp:165-192` (`PF_ModifyStoneAttrib`) bo qua (`nLv < 1` → `continue`) ⇒ khong cong gi, nhung `head.lua:804` van bao "Kham Phi Phong thanh cong".
- **Vi sao gan nhu chac chan xay ra:** `head.lua:501` chay `for i = 1, nCurHoles-1 do SetStoneLevelOnEquip(nNewIdx, i, 10)` (GIONG HET Linux) ⇒ sau MOI lan dot pha thuong, phi phong moi LUON con lo CUOI o cap 0 — dung cai lo ma shim se chon khi cac lo truoc da day.
- **Sua thanh:** hai vong. Vong 1 chi lay lo trong co `GetStoneLevelOnEquip(nMantle, i) > 0`. Vong 2 (chi khi con da chua xep) moi tinh toi lo trong cap 0, kem CANH BAO ro rang. Neu het lo → phan biet "het lo trong" vs "chi con lo chua tang sao".
- **CAM ap ban va cu** (`and GetStoneLevelOnEquip(...) > 0` lam dieu kien duy nhat): `head.lua:501` co tinh de lo cuoi cap 0, nen cam kham lo cap 0 = cam kham LO TREN CUNG vinh vien; va voi phi phong moi tinh (moi lo cap 0) thi cam kham hoan toan.
- **Dinh chinh (dung dao lai):** KHONG phai "mat da vinh vien" — ma da van nam trong lo, lan dot pha ke tiep (`head.lua:499-503`) hoac dot pha lan 2 (`head.lua:685-689`) `SetStoneLevelOnEquip(...,10)` roi `InlayStarStone` lai ⇒ thuoc tinh song lai. La mat TAM THOI / vo hinh.
- **Nghiem thu:** GM tao phi phong 3 lo (lo 1-2 cap 10, lo 3 cap 0). Tha 1 vien → vao lo 1 hoac 2 (kiem bang tooltip, `KItem.cpp:216-256` in tung lo). Tha 3 vien → vien thu 3 vao lo cap 0 kem canh bao.

### 1.4 [B] `InlayStarStone` tu choi lang le nhung van an vien da
- **Tep:** `head.lua:795-797`.
- **JX1:** `LuaPF_InlayStarStone` (`KJx2WarInfra.cpp:2260-2276`), o `:2270-2274`: `if (nSlot < 1 || nSlot > Item[n].GetMaxStoneNum()) { Lua_PushNumber(L, 0); return 1; }` — tra 0, khong bao gi. Script goi `InlayStarStone(nMantleIdx, i, nP);` roi NGAY DONG SAU `RemoveItemByIndex(nStoneIdx);` **khong kiem tri tra ve**.
- **Sua thanh:** `if InlayStarStone(nMantleIdx, i, nP) == 1 then RemoveItemByIndex(nStoneIdx); ... else <ghi nhan truot> end`; va neu co vien nao truot thi **khong** bao "thanh cong".
- **Trang thai hom nay:** chua no vi shim lap `1..nMaxHole`. Day la lop phong thu re, lam luon de DOT 6 khong phai nho lai.
- **Nghiem thu:** (sau DOT 6) chon lo 4 tren phi phong 2 lo → bao loi va vien da CON trong tui.

### 1.5 [T] `szLockState` chua tung duoc gan
- **Tep:** `head.lua:699` — `format("SecBreakThrough\t%s\t%d\tsuccess\t%d", szLockState, ...)`.
- **Su that:** grep toan bo `LX\server1\script\` va `SV\script\`: bien nay xuat hien DUNG MOT LAN o moi cay, khong noi nao gan. **Loi co san cua ban goc Kingsoft**, khong phai loi port.
- **Muc do that (da ha):** dong 699 la cau ap chot — `Pay`, `RemoveItemByIndex`, `AddPlatinaItem`, `UpgradePlatinaItem x10`, `SetLastBreakTime`, `SetStoneLevelOnEquip`/`InlayStarStone`, `SetItemBindState`, `SyncItem`, `Talk` deu da xong o `head.lua:663-698`. Mat duy nhat: `self:WriteMantleLog(szLogInfo)` (`:700`) = MOT DONG NHAT KY. Va hom nay KHONG THE chay (chi toi qua `onBreakThroughWuJi`, dang bi chu thich).
- **Sua thanh:** bo `%s`/`szLockState` khoi chuoi format, hoac gan `local szLockState = GetItemBindState(self.nMantleIdx)` (tra SO — Lua 4.0 chap nhan `%s` voi so).
- **Bat buoc neu mo nhanh Vo Cuc (DOT 8-2).**

### 1.6 [T] Shim `SetUiGiveItemMsg` spam khung chat — 13 diem goi
- **Tep:** shim o `head.lua:824-834` (dinh nghia lai `SetUiGiveItemMsg` / `SetUiGiveItemMoreConfirmMsg` thanh `Msg2Player`).
- **Linux:** hai ham la API ENGINE that (`grep -a -o "SetUiGiveItem[A-Za-z]*" LX\jx_linux_y` tra ve dung 2 ten; `grep "function SetUiGiveItemMsg"` toan cay script Linux = RONG), ghi chu THANG VAO PANEL.
- **JX1:** khong co trong bang dang ky (`ScriptFuns.cpp` / `LuaFuns.cpp`) — ly do ky thuat ghi o `head.lua:816-822`: S2C_GIVE_BOX chi 3 o chuoi.
- **13 diem goi (khong phai 3):** `npc.lua:44, 74, 92, 110` (ngay luc MO hop, truoc khi panel hien), `:303, :308, :320, :324, :326` (onStarUpGradeCallBackCheckItem), `:335, :340, :354` (onCallBackCheckItem), cong dinh nghia `head.lua:824-834`.
- **Sua thanh:** dat bo nho "chuoi cuoi cung theo PlayerIndex" NGAY TRONG SHIM o `head.lua`, chi `Msg2Player` khi noi dung DOI; them tien to `[Phi Phong]`. **Khong sua tung noi goi** — loc trung trong callback thi cac dong luc mo hop van spam.
- **Nghiem thu:** mo hop tang sao, tha/rut 5 lan → moi lan doi noi dung ra dung 1 dong chat.

### 1.7 [B] Hai loi thoat som khong mo lai phien — nut chet vinh vien
- **Tep:** `npc.lua:174` (`not PlayerIndex`) va `npc.lua:178` (`not tbAllPlayerProcess[PlayerIndex]`) trong `doMantleMosaicStoneBox` — ca hai `return` ma KHONG goi `PF_InlayMoLai()`.
- **Vi sao chet:** `SRC\Core\Src\KProtocolProcess.cpp:6502` xoa `m_dwGiveBoxId` va `KJx2WarInfra.cpp:852` xoa `s_GivePending` ngay sau lan bam OK dau. Khong mo lai = nut "Kham nam" chet, Phi Phong ket trong o. Cong voi 5.1 (Huy/X/ESC khong tra do) thi khong con duong lay do ra ngoai cach di gap lai NPC.
- **Sua thanh:** hai nhanh do goi `PF_InlayMoLai()`, hoac it nhat `Talk` mot cau.
- **Nghiem thu:** lam rong `tbAllPlayerProcess` (relog roi bam thang "Kham nam" khong qua thoai) → co thong bao, va nut van bam duoc lan 2.

### 1.8 [B] `doMantleUpGrade` khong mo lai phien tren nhanh loi — hop chet sau mot lan bam
- **Tep:** `npc.lua:258-290` — dung chung cho tang sao, dot pha thuong, nang cap Vo Cuc, dot pha lan 2. Khong co loi goi mo lai nao, ke ca tren nhanh loi.
- **Kem theo:** `SRC\S3Client\Ui\UiCase\UiAffairItem.cpp:143-146` — `WndProc` goi `OnOk()` ma KHONG dong cua so.
- **Trieu chung:** nguoi choi bam OK, gap "Hien vang khong du..." (`npc.lua:284-287`) hoac bat ky `szErrorMsg` nao, sua lai roi bam OK lan hai → khong co gi xay ra (`m_dwGiveBoxId` = 0). Hop trong y het luc dau nhung da chet; Phi Phong chi lay lai duoc bang Huy/ESC. **Dung lop loi ma ban va 31/08 da thua nhan — nhung ban do CHI va cho duong kham.**
- **Sua thanh:** moi nhanh LOI cua `doMantleUpGrade` phai mo lai phien give-box (goi lai `GiveItemUI` cung bo tham so, theo tinh than `PF_InlayMoLai` o `npc.lua:161-165`).
- **Nghiem thu:** mo hop tang sao voi tui khong du tien → bam OK, nhan thong bao, GM nap tien, bam OK lan 2 → phai chay.

### 1.9 [T][Q] Sua CHU cho khop thuc te (3 chuoi)
- `npc.lua:49`: bo doan hua "mo server 10/20/40/60 ngay dot pha den le 2/3/4/5". Ly do: `nMinOpenDays` (`head.lua:51/57/63/69` va `:98-114`) khong ai doc; hai cho doc bi CHU THICH SAN o **CA HAI CAY** (`head.lua:417-421` va `:613-617`) — moc nay CHUA TUNG duoc cuong che o ban goc.
- `head.lua:79`: bo cau chi dan toi "Nang cap Vo Cuc" (menu `npc.lua:51-52` bi chu thich SAN TU BAN LINUX ⇒ chi dan toi chuc nang khong ton tai).
- `npc.lua:12`: neu chu game chot cap sao khong cong thuoc tinh (DOT 8-1) thi bo cau "tang sao se tang kha nang thuoc tinh".
- **CAM (dung dao lai):** khong xoa `onAdvanceToWuJi`/`onBreakThroughWuJi`/4 ham `head.lua:524, 539, 578, 655`/bang `tbSecBreakThrough` — do la tu tao lech khoi moc Linux, va `test_phiphong_admin.lua:59` con tham chieu. Khong viet lai `GetServerOpenDays` bang API JX1 roi bo chu thich `head.lua:417-421`/`:613-617` — do la THEM mot han che ma Linux KHONG co (= doi can bang loi choi).
- `GetServerOpenDays` (`head.lua:175-184`, tham chieu `tbTimeLineClass`/`tbTimeLineManager` khong ton tai) la MA CHET o CA HAI cay, than ham khong duoc danh gia luc nap ⇒ **khong the gay ScriptError**, khong phai "bom nen". Xoa la tuy chon ve sinh, khong bat buoc.

---

## DOT 2 — BUILD CORESERVER #1 (nho, khong doi giao thuc)

### 2.1 [B] **ITEM-01 — LO HONG KINH TE VO HAN, DANG CHAY**
- **Tep sua:** `SRC\Core\Src\KItemList.cpp:1945-1957` — ham `sIsJx2ItemScript`, mang `szJx2[]` liet ke 9 tien to duong dan duoc huong quy uoc JX2 (petsys, item\messenger, item\xinshirenwu, bosscharm.lua, honnguyenchandon.lua, huangzhendan.lua, biggoldenseed.lua, shenmuling.lua, shashou_mibao.lua).
- **Thieu:** `\script\global\mantlesystem\` (grep `mantlesystem` trong `KItemList.cpp` = 0 ket qua).
- **Linux:** quy uoc engine JX2 — `main()` tra ve KHAC 1 thi ENGINE tu tru 1 don vi mon vua dung. `LX\server1\script\global\mantlesystem\item_starore.lua:45-54` va `item_starstone.lua:31-38` dua han vao do: nhanh THANH CONG khong `return` (tra nil), khong tu xoa minh. (`item_starore.lua` trung md5 tuyet doi giua hai cay: `7e1d42ff`; hai tep kia chi khac MA VAT PHAM 4546→4881, 4548→4883, 4551→4886, 4552→4887.)
- **JX1:** `KItemList.cpp:2102` `case item_magicscript` → `EatMecidine` (`:1970`) → `:2019` `if (sIsJx2ItemScript(...))` **FALSE** → nhanh else `:2040` `ExecuteScript(..., "main", ...)` — **BO QUA tri tra ve**, va khoi `:2011-2043` khong co lenh `Remove`/`SetStackNum` nao.
- **Hau qua:** PT **4885** (Tinh Ngoc Nguyen Thach) va PT **4887** (Tinh Than Khoang) **KHONG BAO GIO bi tieu hao**. Bam 1 vien Khoang → duoc 1 Tinh Than Thach, vien Khoang VAN CON. Bam Nguyen Thach → chi tru 1 Tinh Hoa Than, moi lan ra 15-90 Tinh Ngoc (+5% Manh Thien Tinh Ngoc, +2% Tinh Than Khoang) ma vien Nguyen Thach khong mat. Mot vien Nguyen Thach + N Tinh Hoa Than = Tinh Ngoc khong gioi han, va sinh ra Khoang vo han. **Khong the truy nguoc de thu hoi** (khong co log tru do vi do khong bi tru).
- **Khong dinh:** `item_tianxingyu.lua` (PT 4883) tu `ConsumeItem` 10 mon va `return 1` o moi nhanh.
- **Neo bang:** `SV\settings\item\magicscript.txt` cot 10 (Script): dong 4885 → `item_tianxingyu.lua`, dong 4887 → `item_starstone.lua`, dong 4889 → `item_starore.lua` (tuong ung PT 4883/4885/4887, Genre=6).
- **Sua thanh:** them 1 phan tu `"\\script\\global\\mantlesystem\\"` vao `szJx2[]`. Khop tuyet doi voi ban goc, an toan cho ca 3 tep.
- **Neu chua build duoc hom nay:** va tam bang Lua — them `RemoveItemByIndex`/`ConsumeItem` chinh mon o cuoi nhanh thanh cong `item_starore.lua:52` va `item_starstone.lua:34`. **PHAI GO khi ap ban C++, khong thi TRU HAI LAN.** Ghi ngay vao ban giao khi va tam.
- **Rui ro:** hep — chi 3 tep trong dung thu muc mantlesystem, khong dung den 9 tien to dang chay. Nhung day la lan dau thu muc nay vao danh sach → kiem lai ca 3 tep sau swap.
- **Nghiem thu:** GM cho 1 vien PT 4887, bam → nhan 1 Tinh Than Thach VA vien khoang PHAI BIEN MAT khoi tui. Lam lai voi PT 4885 (tru them 1 Tinh Hoa Than). Kiem PT 4883 khong bi tru hai lan (10 mon vao, tru dung 10).

### 2.2 [T] Ro trang thai phien give-box
- **Tep:** `SRC\Core\Src\KJx2WarInfra.cpp:808-809` (`s_GiveSessions`, `s_GivePending`); ghi o `:833` va `:2419`; chi duoc xoa o `:849`/`:852` — tuc **chi khi nguoi choi bam OK**.
- **Su that:** nut Huy/ESC (`UiAffairItem.cpp:63-78`, `:187-194`) chi gui `GOI_RECOVERY_BOX_COMMAND` (`CoreShell.cpp:13288`) nen `s_GivePending[nPlayerIndex]` o lai vinh vien. `grep -rn KJx2WarInfra_ Sources` chi ra DUNG MOT ham (`KJx2WarInfra_GiveBoxCollect`) — khong co moc don khi nguoi choi thoat game. Ca hai map khoa theo CHI SO O nguoi choi (tai su dung) ⇒ `vItems` cua nguoi truoc con nguyen cho nguoi sau vao cung o.
- **Sua thanh:** (a) xoa ca hai map trong duong roi phien cua `KPlayer`; (b) xoa `s_GivePending` khi nhan RECOVERY_BOX.
- **Trang thai:** chua khai thac duoc (moi script deu goi trong callback) — la bay cho. Lam chung dot vi cung tep, cung lan build.
- **Nghiem thu:** dat mot dong log tam trong duong roi phien, thay no chay khi nguoi choi thoat; va sau ESC thi `GetGiveItemUnit` tra 0.

---

## DOT 3 — DU LIEU + TIEM + NGUON ROI (khong build; can chu game duyet kinh te)

### 3.1 [B] Nut "Tiem nguyen lieu Phi Phong" la NUT CHET + ra soat ca bang `Sale`
- **Linux:** `LX\server1\script\global\mantlesystem\mantleupgrade_npc.lua:149` — `Sale(186, 25, 100, "onShopCallBack(%d,%d,%d)", "\\script\\global\\mantlesystem\\mantleupgrade_npc.lua")` (5 tham so; **25 la SO O cua tiem, KHONG phai don vi tien** — dung be nguyen sang).
- **JX1:** `npc.lua:255` giu nguyen 5 tham so. Nhung `LuaSale` (`SRC\Core\Src\ScriptFuns.cpp:2697-2711`) CHI doc doi 1 va 2 roi goi `BuySell.OpenSale(player, 186-1=185, 25)`. `KBuySell::OpenSale` (`SRC\Core\Src\KBuySell.cpp:654-658`) co chan `if (nShop < 0 || nShop >= m_Height) return;`; `m_Height = BuySellFile.GetHeight()-1` (`KBuySell.cpp:173`), tep nap la `BUYSELL_FILE = "\\settings\\buysell.txt"` (`CoreUseNameDef.h:94`, `KBuySell.cpp:84`) — **102 hang ⇒ m_Height = 101 ⇒ `Sale` hop le 1..101**. 186 bi loai, khong gui goi tin nao. Doi JX1 da vap dung bay nay: `SV\script\missions\tongwar\npc\doctor.lua:8` co chu thich "buysell.txt du an 102 dong (khong co shop 131)".
- **BAY KEM (phai biet truoc khi sua):** `ScriptFuns.cpp:2704-2706` — `int nShopMoneyUnit = moneyunit_money; if (Lua_GetTopIndex(L) > 2) nShopMoneyUnit = Lua_ValueToNumber(L, 2);`. Goi `Sale(N, moneyunit)` HAI tham so thi TopIndex = 2, KHONG > 2 ⇒ moneyunit lang le ve 0 (tien van). **Phai truyen TU BA tham so tro len**: `Sale(N, moneyunit, 0)`. (Nhanh `OpenSale(int,int,int)` `KBuySell.cpp:654-671` KHONG kiem dai `moneyunit`; chi nhanh 5 tham so `:673-682` moi kiem — dung vien dan mot chot khong ton tai, nhung van phai dat dung dai vi client dung no.)
- **Viec:**
  1. Them MOT hang tiem vao **CUOI** `SV\settings\buysell.txt`: 4881 Tinh Ngoc PT, 4882 Thien Tinh Ngoc, 4883 Manh Thien Tinh Ngoc, 4884 Bach Luyen Thanh Cuong, 4886 Tinh Hoa Than. Ghi lai so hang N.
  2. `npc.lua:255` → `Sale(N, <moneyunit>, 0)`. Bo hai tham so callback/script.
  3. **[Q]** `onShopCallBack` (`npc.lua:357`) thanh ham mo coi — JX1 khong co callback mua hang nao (`LuaSale` va `LuaNewSale` `ScriptFuns.cpp:2713+` deu khong nhan ten ham goi lai). Dong `SetItemBindState(nItemIdx, -2)` khoa do vua mua cua Linux SE MAT HAN. Muon giu thi phai dat cot khoa san trong buysell/goods → **doi tinh chat vat pham ban ra, chu game phai duyet**.
  4. Ra ca bang: grep `Sale(` toan cay, loc id > 101. **Da biet:** `SV\script\task\partner\changge_people.lua` goi `Sale(110)`...`Sale(129)` (≥20 loi goi, dong 95-138+) — TOAN BO bi chan im lang, **he Ban Dong Hanh con nguyen nut chet**. Va cac loi goi 2 tham so bi nuot don vi tien: `event\trongbanghoi\congtontoan.lua:50` `Sale(53,0)`, `event\kiemmonquan\congtonthu.lua:45` `Sale(53,0)`, `global\npcchucnang\thoren.lua:1121` `Sale(31,0)`, `missions\tongwar\npc\doctor.lua:8` `Sale(12,1)` (dong nay TUONG minh ban bang don vi 1, thuc te ban bang tien van).
- **Rui ro:** chon nham chi so hang (buysell dung chi so 0-based ben trong, Lua truyen 1-based) → thu bang GM truoc khi mo cho nguoi choi.
- **Nghiem thu:** GM bam menu "Tiem nguyen lieu Phi Phong" → tiem PHAI mo, dung 5 mon, dung don vi tien; mua 1 mon → tru dung loai tien.

### 3.2 [B][Q] Khong co duong lay PHI PHONG so cap, cung khong co duong lay TINH THAN THACH
- **Linux:** `LX\Patch\settings\item\004\magicscript.txt:4976` (PT 4658) "Tuyet The Phi Phong Bao Hap", script `\script\item\item_jueshipifengbox.lua`; tep `LX\server1\script\item\item_jueshipifengbox.lua:8-14` anh xa `6,1,4658` → `tbProp {0,5374}` (= goldequip dong 5375 "Phi phong Tuyet The").
- **JX1:** `SV\settings\item\magicscript.txt` chi co 9 dong phi phong (dong tep 4883..4891 = PT 4881..4889) — toan nguyen lieu, **khong co dong hop nao**; grep `jueshipifeng` = 0. `AddGoldEquipByRow`/`AddPlatinaItem` chi o 3 diem, deu la duong DOT PHA (`head.lua:499, :568, :681`) + lenh bai admin. Tho Ren DA `Include` `mantleupgrade_npc.lua` (`thoren.lua:9`) nen nguoi choi **mo duoc bang nhung khong co gi de bo vao**.
- **Da kham (G=9, `starstone.txt` 34 dong PT 1..33)** cung chi sinh ra o `test_phiphong_admin.lua:188` (`AddItem(9,1,i,...)`).
- **Viec:**
  1. **CHI CAN 1 DONG magicscript** (khong phai 5): 4 dong "anh em" Linux 4977..4980 (PT 4659..4662) la **DONG CHIEM CHO** — tro toi `\script\item\noscript.lua`, anh `questkey\003.spr` ⇒ khong bao gio chay; 4 muc do trong `item_jueshipifengbox.lua` la ma chet. Bon phi phong con lai di duong dot pha.
  2. **Cot script cua JX1 la COT 10**, khong phai cot 14 nhu Linux. Bang JX1 15 truong (`SRC\Core\Src\KBasPropTbl.h:340-362` + `LoadRecord` `KBasPropTbl.CPP:1030-1044`: szIntro(9), **szScript(10)**, nPrice(11), bShortKey(12), nMaxStack(13), nPickExecute(14), nParam(15)). Dat nham cot 14 = hop khong chay.
  3. Port `item_jueshipifengbox.lua`, giu anh xa `tbProp {0,5374}` (`goldequip.txt:5375` trung khop Linux tung byte).
  4. **PHAI noi vao CUOI bang** magicscript (RECORD INDEX = ma item; chen giua = doi ma moi dong sau — bay da ghi trong bo nho), va chep dong bo sang `CL\settings\item\magicscript.txt` (hien hai ben trung md5).
  5. **[Q]** Chot duong phat **Tinh Than Thach**: roi tu quai / ban o tiem (3.1) / hop qua. Khong co da thi he kham VAN khong dung duoc.
- **Nghiem thu:** nhan vat thuong (khong GM) lay duoc hop, dung → nhan dung "Phi phong Tuyet The"; va lay duoc it nhat 1 vien da de kham thu.

### 3.3 [B][Q] Nguon roi nguyen lieu = **SO KHONG**, khong phai 3 (bac bo ket luan cu)
- **Tep:** `SV\script\global\mantlesystem\other_func_outputs.lua` — `RegisterFuncOutPutEvent()` dang ky 12 handler o `:172-185`, goi o chunk body `:187`. Khac ban Linux dung 1 dong (dong 8, `TB_ITEM_PROP` 4550 → 4885).
- **Su that moi:** `grep -rn other_func_outputs SV\script` = **0 KET QUA** — khong tep nao `Include` no. Theo luat 0.3, 12 handler nam trong mot Lua_State CO LAP. Diem ban su kien o state khac han: `missions\yandibaozang\npc_death.lua:308` (`OnLastBigBossDeath`), `missions\fengling_ferry\bossdeath.lua:56` va `bigbossdeath.lua:5` (`OnShuiZeiDeath`) — cac tep nay `Include` `misc\eventsys\type\func.lua` rieng, tao bang `EventType`/`MapEvnent` RONG ⇒ `eventsys.lua:63-65` `if not self.MapEvnent[szType] then return end` thoat ngay.
- **Ben Linux khong ai Include no cung dung thiet ke** — vi Linux co pack that (`eventsys.lua` dung `curpack()`/`usepack()` o `:29, :68, :70, :89, :90`).
- **Dinh chinh so lieu cu:** FengLingDu co **2** diem ban chu khong phai 3 (diem thu ba la `shuizei.lua:83` ban `OnFinishShuiZeiTask` — su kien KHAC, khong duoc dang ky). WorldBoss va BloodyBattle = 0. Nhung du sao ca 3 diem con song deu khong toi duoc handler.
- **Viec (chon 1):**
  - (a) Cho MOI tep co ban su kien `Include` truc tiep `\script\global\mantlesystem\other_func_outputs.lua`. **TRUOC KHI THEM:** grep 3 ten toan cuc ma tep do dat o chunk body (`TB_ITEM_PROP`, `MIN_FREE_ROOM_NEED`, `MSG_FREE_ROOM_NEED_ERROR`) trong tep dich, de tranh va cham ten.
  - (b) Bo `EventSys` o duong nay, goi thang ham trao thuong tu chinh tep ban su kien — giong cach ban port da lam cho `wuxingyin` bang `WXY_MoBang()`.
- **KHONG LAM:** go 9 handler "khong bao gio chay". Do la tu tao lech voi Linux va phai lam lai khi Boss The Gioi / Huyet Chien duoc port. Giu nguyen thi tep trung khit ban Linux va khong ton gi.
- **[Q] Can chu game duyet:** moi diem noi lai la MOT NGUON ROI THAT.
- **Nghiem thu:** GM giet boss cuoi Viem De → nhan duoc Tinh Ngoc Nguyen Thach, va co dong trong `logs\hd3_statdata.log`.

### 3.4 [B][Q] `goldequip.txt`: 34 dong doi TEN + **19 dong doi CHI SO co nghia**
So voi `goldequip.txt.truoc_phiphong_2908`: 286 dong doi (DT11=150, DT12=20, DT13=14, DT14=102), 34 dong doi ten, 273 dong doi cot 9. Cau "252 dong con lai chi bo cap nhay kep, vo hai" la **SAI** — 19 dong doi so:
- `:3226..3235` "Ngu Hanh An": mat hau to "Cap 1..10" (ca 10 dong cung mot ten) **VA** cot 36 doi 90 → 120. He nang cap An dung dung 10 dong nay (`event\equip_publish\wuxingyin\wuxingyin.lua`) ⇒ nguoi choi cam 10 vien deu thay mot ten.
- `:3478`/`:3479` va `:3481`/`:3482` (Phi Phong Phu Quang / Khiep Than, hoa giai sat thuong): duoc **THEM** cot 35 = 47 va cot 36 = 54000 / 120000 (truoc do rong).
- `:3551` "Long Dam": cot 15 doi 30→5, cot 16 doi 30→25.
- `:5379` "Phi phong Ngu Phong": **cot 62 = TRAN DIEM CHUC PHUC bi XOA tu 22 → rong**. Day dung la cot ma `PF_ApplyRowInfo` doc (`SRC\Core\Src\KItemGenerator.CPP:26-41`, `PF_GCOL_MAXWISH = 62`) ⇒ mon phi phong cuoi cua nhanh vang **mat tran diem chuc phuc**.
- `:5931..5933` "Tan Nham Hanh An Giam (Ha/Trung/Thuong)": bi **XOA SACH** cot 14,15,16,17,18,19 (116/10/10/115/10/10 → rong) = mat toan bo thuoc tinh.
- Hai khoi `:4484-4493` (Long Dam) / `:4854-4863` (Cuong Lan) bi HOAN nhan "[Khong the gia han]" theo dung ban Linux.
- **Viec:** tra lai hau to cap cho `:3226..3235` tu ban `.truoc_phiphong_2908`; trinh chu game 19 dong doi so, chot tung dong giu theo Linux hay theo ban JX1 cu; **uu tien `:5379` cot 62 va `:5931-5933`**.
- **Rui ro:** chi doi cot, khong doi so dong ⇒ khong lech ma vat pham. Server va client hien TRUNG md5 (`ea06786d...`) — sua ben nao phai chep sang ben kia.
- **Nghiem thu:** GM phat 10 vien Ngu Hanh An cac cap → ten phai khac nhau. GM phat "Phi phong Ngu Phong" → sau DOT 7, tooltip phai co dong diem chuc phuc x/22.

### 3.5 [B][Q] `mask.txt` — **RUI RO DU LIEU LON NHAT CA DOT PORT** (khong finding nao truoc do neu)
- **Su that:** `mask.txt` bi **THAY NGUYEN BAN** bang tep Linux, khong phai "them 64 dong". So tung o voi `mask.txt.truoc_phiphong_2908`: **840/840 dong cu deu doi**; cot 4 ParticularType doi 839 dong; cot 12 (cap) 758 dong; cot 11 (gia) 447 dong; cot 17-22 va 29-33 hang tram dong. Ban cu = 84 PT x 10 cap (cot 12 co du 1..10, moi cap 84 dong). Ban moi = **903 PT duy nhat**, 884 dong cap 1. (96 dong con ten Trung Quoc tho, 21 dong sau dong 840; md5 mask/mantle/shipin/signet/starstone trung Linux tung byte.)
- **Hau qua:** `SRC\Core\Src\KItemGenerator.cpp:412-415` tra bang mat na theo `i = nParticularType` ⇒ **moi mat na da phat cho nguoi choi theo danh so CU se hoa thanh MON KHAC sau swap.**
- **Viec:** TRUOC khi cho nguoi choi vao, quet CSDL xem con mat na (DetailType 11) nao trong tui/kho/sap khong. Neu co → hoac giu `mask.txt` cu, hoac lam bang anh xa PT cu → PT moi. **Trinh chu game quyet.**
- **Nghiem thu:** quet CSDL ra 0 mat na → an toan, ghi ban giao. Ra > 0 → phai co bang anh xa truoc khi mo server.

### 3.6 [T] Canh bao tran chi so 3 bang "trang"
`SRC\Core\Src\KBasPropTbl.CPP:38-40` (TABFILE_MANTLE/SIGNET/SHIPIN) + `:62-81` (chi so 12/13/14 trong 17 muc) + `:154-159` + `:202` + `:1406-1408`: ba bang NAY **CO** duoc nap. Nhung `mantle.txt` chi 3 dong du lieu, `signet.txt` va `shipin.txt` moi tep 1 dong ("Sa Di phuc"), trong khi `KItemGenerator.cpp:412-415` tra ban ghi theo `i = nParticularType`. Neu co duong nao sinh do "trang" DT 12/13/14 voi PT ≥ so dong → `GetRecord` roi ra ngoai pham vi (`_ASSERT` o Debug, con tro sai o Release). **Viec:** them phep kiem PT truoc khi sinh, hoac ghi ro "cam sinh do trang cho 3 o nay". Chi ghi chep, khong gap.

---

## DOT 4 — NHANH AN (Lua thuan, khong build)

### 4.1 [B] **Lam TRUOC 4.2** — lech quy uoc so dong goldequip: moi Hoang Kim An lech LEN 1 cap
- **Quy uoc chuan JX1:** so trong script = `GetGlodEqIndex` = chi so ban ghi + 1 = **so dong tep − 1**. Ba neo: (a) `SRC\Core\Src\KItemCompound.cpp:1779` `nIdx = p->GetRow() + 1`; (b) `KBasPropTbl.CPP:84-87` `nRow += 2` (ban ghi i o DONG TEP i+2); (c) he Phi Phong dung dung quy uoc do — `head.lua:284` va `:398` lay `GetGlodEqIndex`, `tbBreakThrough[5374]` = "Phi phong Tuyet The" (`goldequip.txt` DONG 5375), va `LuaPF_AddGoldEquipByRow` (`KJx2WarInfra.cpp:2343`) **TRU 1**.
- **Duong cua he An di qua tep khac:** `wuxingyin.lua:196` → `tbAwardTemplet:GiveAwardByList` → `SV\script\lib\awardtype\item_jx1.lua:27-28`: `if tbItem.nQuality == 1 and (tbProp[1] or 0) == 0 then nItemIdx = AddItem2(2, 0, tbProp[2] or 0, 0, 0, 0)` — **KHONG tru 1**. `LuaAddItem2` (`ScriptFuns.cpp:5078`) → `ItemSet.Add(..., nDetailType, ...)` → `KItemGenerator.CPP:1430 Gen_GoldEquipment` → `:1924 GetGoldEquipRecord(nIndex)` → dong tep `nIndex+2`.
- **Hau qua do duoc:** `goldequip.txt` dong 3206-3215 = 10 dong "Hoang Kim An (Cuong hoa)", dong 3216 = "Hoang Kim An (Nhuoc hoa)". `wuxingyin.lua` danh cuong hoa 3205..3209, nhuoc hoa 3215..3219. Vay 3205 → dong 3207 (cuong hoa thu 2), **3214 → dong 3216 = NHUOC HOA** (An bi lam YEU thay vi manh nhat). Bay im lang: mon nhan ve van co ten hop le, chi thay "thuoc tinh khong dung nhu bang".
- **Viec:**
  1. **THU BANG GM TRUOC KHI SUA:** `test_phiphong_admin.lua` → `WXY_MoBang()` → nang cap 1 → doc ten + thuoc tinh mon nhan duoc, so voi `goldequip.txt:3206`.
  2. Neu dung lech: `item_jx1.lua:28` → `AddItem2(2, 0, (tbProp[2] or 0) - 1, 0, 0, 0)`.
- **Pham vi:** grep `nQuality *= *1` toan cay chi ra `wuxingyin.lua` (20 dong) ⇒ sua an toan. **KHONG dung vao** `event\30thang4\EventLib.lua:176` (goi `AddItem2` truc tiep, bo so rieng — duong khac).
- **Rui ro:** neu bo so trong `wuxingyin.lua` that ra la chi so ban ghi tho thi tru 1 se lech NGUOC ⇒ bat buoc lam buoc 1. Bang chung manh nhat ung ho viec tru 1: bien Nhuoc hoa — 3215 khop dung dong 3216 duoi quy uoc `GetGlodEqIndex`, con duoi quy uoc tho thi khoi Cuong hoa tran sang Nhuoc hoa, khong the la y do thiet ke.
- **Nghiem thu:** "cuong hoa cap 1" → dung dong 3206; "cuong hoa cap 10" → dung dong 3215, **khong** ra mon "Nhuoc hoa".

### 4.2 [B][Q] Nhanh AN khong co LOI VAO cho nguoi choi
- **Linux:** `LX\server1\script\event\equip_publish\wuxingyin\wuxingyin.lua:248-276` — `tbWuxingyin:Register()` tu gan minh vao NPC bang eventsys (`:272` `pEventType:Reg("Tho ren trong thanh", "Nang cap Hoang kim an", CreateNewSayEx, {szMsg, tbOpt})`, goi `Register()` o chunk body `:276`) ⇒ moi Tho Ren tu dong moc them dong menu.
- **JX1:** `SV\script\event\equip_publish\wuxingyin\wuxingyin.lua` da port du (10510 byte vs 9188 byte Linux); ban port ghi ly do khong dung eventsys o `:278-292` va thay bang ham phang `WXY_MoBang()` (`:296-306`). Nhung grep toan cay `WXY_MoBang`/`wuxingyin`: ngoai chinh tep, **CHI** `item\test_phiphong_admin.lua` (`:25` Include, `:461-465` goi). `SV\script\global\npcchucnang\thoren.lua` `Include` `mantleupgrade_npc.lua` (`:9`) nhung **KHONG** `Include` `wuxingyin.lua`.
- **Hau qua:** toan bo nhanh AN chi GM cham toi duoc. Du lieu da san: `goldequip.txt` co 172 dong DT 13 (An), gom 10 dong "Hoang Kim An (Cuong hoa)" (`:3206-3215`), 10 dong "Ngu Hanh An" (`:3226-3235`), khoi "Nhuoc hoa" tu `:3216` — deu khong voi toi duoc.
- **Sua thanh:** them `Include("\\script\\event\\equip_publish\\wuxingyin\\wuxingyin.lua")` vao `thoren.lua` canh dong 9, va them mot dong menu goi `WXY_MoBang()`. **PHAI goi tu chinh state da Include** (khong dung `DynamicExecute`) vi `GiveItemUI` buoc callback theo `m_ActionScriptID`.
- **Nghiem thu:** nhan vat thuong noi chuyen Tho Ren → thay dong "Nang cap Hoang kim an", mo duoc bang, nang 1 cap ra dung mon (da vá 4.1).

---

## DOT 5 — BUILD CLIENT #1 (Game.exe, khong doi giao thuc, client phai tat)

### 5.1 [B] Bam Huy / X / ESC tren panel kham KHONG tra do ve tui
- **Tep:** `SRC\S3Client\Ui\UiCase\UiMantleInlay.cpp:72-88`. Nhanh `bDestroy` dat `m_pSelf = NULL` o `:80`, roi `:85` kiem `if (m_pSelf)` nen `:86` `m_pSelf->OnCancel()` **KHONG BAO GIO chay**. Ca ba duong dong truyen `bDestroy = true`: `:257` (Huy bo), `:259` (X), `:269` (ESC). Duong `CloseWindow(false)` tu `GameSpaceChangedNotify.cpp:792` khong dinh loi nay.
- **Khuon goc cua chinh JX1:** `UiAffairItem.cpp:68-76` goi `OnCancel()` **VO DIEU KIEN** sau khi gan NULL; `OnCancel` (`:187-195`) chi cham `g_pCoreShell` nen van chay va gui `GOI_RECOVERY_BOX_COMMAND`. (Ben Linux nut Huy cua panel la `[CancleBtn]` o dong 352 tep ini ruot ⇒ panel goc cung co duong huy.)
- **Sua thanh:** goi `OnCancel()` TRUOC khi `Destroy()`/gan NULL; xoa `:85-86`. **Sua LUON `UiAffairItem.cpp:70-76` trong cung dot** — do la goi ham qua con tro NULL (hanh vi khong xac dinh), hien "chay duoc" thuan tuy vi `OnCancel` khong cham bien thanh vien; ai them mot dong dung `m_Title`/`m_ItemBox` la client sap moi lan dong hop giao do.
- **CANH BAO (bac bo cau tran an cu "OnCancel chi gui 1 goi cho CA hop"):** `CoreShell.cpp:13292-13314` (case `GOI_RECOVERY_BOX_COMMAND`) **DUYET TUNG MON** o `pos_affairitem` va goi `SendClientRecoveryBox` cho MOI mon ⇒ dinh dung bay `CoreShell.cpp:3664-3672`: `KPlayer::RecoveryBox` (`KPlayer.cpp:6663-6704`) khi tui khong con cho se **NEM mon dang cam xuong dat** (`ObjSet.Add`, `KPlayer.cpp:6694`). Vi vay: hoac theo ky luat `DT_ThuHoiBox` (`CoreShell.cpp:3672-3688` — moi lan 1 mon, tu kiem `CheckCanPlaceInEquipment` truoc khi gui), hoac ghi ro la chap nhan dung rui ro san co cua `KUiAffairItem`.
- **Dinh chinh muc do:** do KHONG mat vinh vien — van o `pos_affairitem`, luu CSDL (`KPlayerDBFuns.cpp:1053`) va nap lai (`KItemList.cpp:318-321`), hien lai khi mo panel tu NPC. La "ket + hoang mang", khong phai "mat do".
- **Nghiem thu:** tha Phi Phong + 2 vien da vao panel, bam ESC → do PHAI ve tui ngay. Lam lai voi tui chi con 1 o trong → **khong duoc roi do xuong dat**.

### 5.2 [T] Panel vut bo tham so `pszTitle`
`UiMantleInlay.cpp:33-67` nhan `pszTitle` nhung khong dong den; khuon goc `UiAffairItem.cpp:42-43` co `m_pSelf->m_Title.SetText(pszTitle)`. Chuoi "Phi Phong kham Tinh Than Thach" (`npc.lua:162`) di tron ven qua `LuaPF_OpenMantleInlayBox` (`KJx2WarInfra.cpp:2413` `sWStrCpy` vao `Value`) toi `KProtocolProcess.cpp:4610` roi bi nem di; bang luon hien chuoi cung trong ini ngoai dong 32. **Sua 1 dong.** CANH BAO: `S2C_GIVE_BOX::Value` chi **32 byte** (`KProtocol.h:1939`) ma tieu de hien tai da 30 byte — con dung 2 byte truoc khi bi cat im lang; kiem lai do dai chuoi `npc.lua:162` khi sua. **Nghiem thu:** mo panel → tieu de la chuoi tu script.

### 5.3 [T] The "Tinh Than Thach" bam mot cai la tat vinh vien
Ini ngoai dong 35 `[AddStarstoneBtn]` co `CheckBox=1`, `Up=1`, `Down=0`. `KWndButton::Init` doc khoa `CheckBox` o `SRC\S3Client\Ui\Elem\WndButton.cpp:103` va bat `WNDBTN_ES_CHECKBOX` o `:105`; `OnLBtnDown` (`:294`, nhanh `:312-316`) TU BO co CHECKED va doi ve `m_nUpFrame`, roi `:326-328` moi gui `WND_N_BUTTON_CLICK`. `KUiMantleInlay::WndProc` `:253-259` chi so voi `m_OkBtn`/`m_CancelBtn`/`m_Close` ⇒ khong ai bat lai. (`UiMantleInlay.cpp:169-170` co goi `CheckButton(TRUE)` luc nap.)
**Sua:** them `else if (uParam == (unsigned int)(KWndWindow*)&m_TabStone) m_TabStone.CheckButton(TRUE);` — JX1 chi co MOT che do. Day cung la diem cam the thu hai neu sau nay port che do duc Huyen Kim. **Nghiem thu:** bam vao the → the giu trang thai duoc chon.

### 5.4 [T] Nhan chu duoi cac o tran ra ngoai vien, de len khung huong dan va o hoa van
`UiMantleInlay.cpp:221-237` sinh 6 nhan 96x14 tai `(nX + nW/2 - 48, nY + nH + 1)` + `BringToTop()` (`:236`). Tinh theo ini ruot: nhan cua `obj_10 (12,12,24,24)` bat dau `x = -24`, ma `m_Pad` co `Left=4` ⇒ lech **20px RA NGOAI** vien trai cua so ngoai; nhan `obj_12 (212,12)` trai 176..272 de len `[GuideList] Left=254` (ini ruot dong 294) **18px**; nhan `obj_13 (112,97)` chiem x 76..172, y 122..136 → de len nua tren `obj_8 (82,132)` va `obj_7 (142,132)`.
Nhan CO VE THAT: `KWndWindow` dat `WND_S_VISIBLE` ngay trong ham dung (`WndWindow.cpp:47`); `KWndText::PaintWindow` (`WndText.cpp:199-207`) ve bang toa do TUYET DOI, khong cat theo khung cha. Chu thuc te **can trai** (m_Nhan khong duoc `Init()` nen khong co `WNDTEXT_ES_HALIGN_CENTRE`, `WndText.cpp:211-217`) — dung con so tren, chi la ly do khac.
**Sua (chon 1):** (a) **bo han 6 nhan** — dung bo cuc goc; khung `m_Guide` da noi ro bang loi (`npc.lua:162`) ngay khi mo bang; hoac (b) giu nhan nhung kep `x` vao `[0, PadWidth-96]`, rut ngan chu, **bo nhan cua o giua** (`obj_13`), va **bo loi goi `BringToTop()` o `:236`**.
**Nghiem thu:** mo panel — khong con chu tran ra ngoai vien / de len khung huong dan / de len o hoa van.

### 5.5 [T] `GDCNI_END_AFFAIR_BOX` dong panel kham va GIAT DO RA, khong phan biet nguon
`GameSpaceChangedNotify.cpp:791-792` goi `KUiMantleInlay::CloseWindow(false)` **VO DIEU KIEN**; ma `CloseWindow(false)` lai chay `OnCancel()` (vi `m_pSelf` con khac NULL) ⇒ thu hoi het do trong o. Thong diep nay phat boi BAT KY script goi `EndGiveBox` (`KProtocolProcess.cpp:4603`, `S2C_GIVE_BOX nType=2`) **va** boi `ST_DongHop` cua may Sat Thu (`CoreShell.cpp:11544`) — khong cai nao biet den phien kham.
**Sua:** chan theo phien — chi dong bang kham khi thong diep den tu chinh phien da mo no. **Nghiem thu:** mo panel kham, de mot script khac dong hop giao cua no → panel kham khong bi dong, Phi Phong khong bi rut ra.

### 5.6 [T] Truc `v` bi dong cung 0
`UiMantleInlay.cpp:326` va `:344` dat `Pick.Region.v = Drop.Region.v = 0`, trong khi khoang chua la luoi **6x4** (`SRC\Core\Src\GameDataDef.h:376-378` — **khong phai 387-389**, do la `TRADE_ROOM`/`GAMBLE_ROOM`), va hop giao THUONG dung ca luoi (`UiAffairItem.cpp:279-280`).
**Hau qua:** mon o `nY != 0` (do phien give-box khac de lai) van duoc VE ra (`UpdateItem` chi doc `Region.h`, `UiMantleInlay.cpp:403`) nhung **khong keo ra duoc** — server tra o bang `FindItem(SrcPos->nX, SrcPos->nY)` (`KItemList.cpp:3426`) va se do o `y=0`. Them: hai mon trung `nX` (khac `nY`) bi GOP hinh — ca hai anh xa ve mot `obj_N`, `HoldObject` sau de len truoc ⇒ nguoi choi thay 1 bieu tuong trong khi `GDI_AFFAIR_ITEM` (`CoreShell.cpp:1415-1445`) va `GiveBoxCollect` (`KJx2WarInfra.cpp:855-869`) deu dem 2.
**Sua:** truyen `Region.v` that, hoac (re hon) khi mo panel thi don het do o `nY != 0` ve tui. **Nghiem thu:** de lai mon o hang duoi cua khoang chua roi mo panel kham → keo ra duoc, hoac mon khong bi ket.

### 5.7 [B] **KHAO SAT — dau ra la dau vao bat buoc cua DOT 6** (khong sua ma)
Mo panel bang ban dung hien tai, **CHUP LAI mot lan**, va doc anh nen `\spr\Ui3\<ten TCVN3>.spr` de xem O KHAM duoc VE o vong nao.
Ly do: ini co **HAI vong nam o quanh CUNG mot tam** `obj_13 (112,97)`:
- vong TRONG `obj_5..obj_9`: `:233 (112,47)` `:240 (157,82)` `:247 (142,132)` `:254 (82,132)` `:261 (67,82)` — lech tam (0,-50)(45,-15)(30,35)(-30,35)(-45,-15), ban kinh ~50.
- vong NGOAI `obj_0..obj_4`: `:198 (58,22)` `:205 (162,22)` `:212 (202,122)` `:219 (22,122)` `:226 (112,182)` — lech tam (-54,-75)(50,-75)(90,25)(-90,25)(0,85), ban kinh 85..93, ngu giac deu, chi xoay 180°.
Va ini ruot dong 407 (`[ReturnInfo_Starstone]`) noi "Tinh Than Thach o lo 1-7 cua Phi Phong co the trung voi lo 8-10" ⇒ **bang goc thiet ke cho phi phong MUOI lo (hai vong nam)**, trong khi JX1 chan cung 5 (`SRC\Core\Src\KItem.h:286` `PF_MAX_STONE = 5`; `:304` `GetMaxStoneNum` lay 3 bit; `:308` kep ve 5).
⇒ **Khong the suy ra vong nao la lo 1-5 tu toa do.** Phai nhin anh.
Ket qua can co: (a) chot `PF_UI_INPUT` = `{5,6,7,8,9,13}` hay `{0,1,2,3,4,13}`; (b) quyet dinh AN 8 o con lai hay chap nhan panel rong.
Dinh chinh so lieu cu: `obj_0..obj_9` bi chu thich `;Image=` (**MUOI** o), `obj_10/11/12` **khong he co khoa Image**; `obj_13` la o duy nhat con giu `Image=` bat. `[ReturnInfo_Starstone]` la dong **406-413** (tep het o 413); khoa `Border` trong `[TextColor]` o dong **374**.

---

## DOT 6 — CHON LO KHAM (BAT BUOC de dat "giong 100% Linux") — build SERVER + CLIENT, swap DONG THOI

**Bon canh:** `LX\...\mantleupgrade_npc.lua:134-138` — `for i=1,nSize do tbItemIdx[i] = GetItemIndexBydwID(tbItemdwID[i]); end`, **vi tri trong mang CHINH LA so lo**; `head.lua:774-801 ProcessInlayStarStone` lap `for i = 1, nSize` roi `InlayStarStone(nMantleIdx, i, nP)`; `tbItemIdx[6]` la Phi Phong (chot cung o `head.lua:727-731`). Dang ky: `LX\...\protocol_def_gs.lua:162-167` `MANTLE_DOINLAY {OBJTYPE_TABLE}`.
**Ben JX1 duong so hieu o DA THONG SUOT ca hai chieu** (XN-01, da kiem lai tung chang, khong bac bo duoc): `UiMantleInlay.cpp:350-357` `Drop.Region.h = Pick.Region.h = i` → `CoreShell.cpp:13512-13516` va `:13713-13717` (`case UOC_AFFAIR_ITEM: P.nPlace = pos_affairitem; P.nX = Region.h`) → `CoreShell.cpp:13798 MoveItem` → `KPlayer.cpp:3780 SendClientCmdMoveItem` → `KPlayer.cpp:5091 ServerMoveItem` → `:5126-5142` → `KItemList.cpp:3425 case pos_affairitem` → **`KItemList.cpp:3464`** `m_Items[nListIdx].nX = DesPos->nX;`. Chieu nguoc: `CoreShell.cpp:1426`/`:1440` `pInfo->Region.h = pItem->nX` → `GameSpaceChangedNotify.cpp:247-256` → `UiMantleInlay.cpp:403-410`. **Client KHONG phai sua gi cho phan co che.** Bien da co san: `KInventory.cpp:65` tu choi `nX + nWidth > 6`; moi Phi Phong va moi da kham deu Width=1 Height=1 (`starstone.txt` 34/34 dong, `mantle.txt` 3 dong).
**Diem duy nhat con thieu:** `KJx2WarInfra_GiveBoxCollect` (`KJx2WarInfra.cpp:840-871`) chi loc `p->nPlace != pos_affairitem` roi push `p->nIdx` — **vut bo `p->nX`**.

### 6.1 [B] C++ server — 6 sua doi (da kiem, dung cho)
1. `KJx2WarInfra.cpp:804-807` — them `std::vector<int> vSlots;` canh `vItems` trong `struct KJx2GiveSession`.
2. `KJx2WarInfra.cpp:854` — them `s.vSlots.clear();` ngay sau `s.vItems.clear();`.
3. `KJx2WarInfra.cpp:867-868` — day SONG NHIP trong CUNG than vong `u` de hai mang khong lech khi cham tran `JX2GIVE_MAX` (512):
   `for (int u = 0; u < nUnits && (int)s.vItems.size() < JX2GIVE_MAX; u++) { s.vItems.push_back(p->nIdx); s.vSlots.push_back(p->nX); }`
4. `KJx2WarInfra.cpp` sau `:889` — them `LuaGetGiveItemSlot(Lua_State* L)`, khuon **y het** `LuaGetGiveItemUnit` (`:874-887`): lay `i` 1-based, tra `it->second.vSlots[i-1]`, mac dinh **-1**.
5. `KJx2WarInfra.h:47` — them khai bao canh `LuaGetGiveItemUnit`.
6. `ScriptFuns.cpp:14058` — them `extern int LuaGetGiveItemSlot(Lua_State* L);`; `ScriptFuns.cpp:15671` — them `{ "GetGiveItemSlot", LuaGetGiveItemSlot },` ngay duoi `{ "GetGiveItemUnit", ... }`.

**RANG BUOC CUNG (vi pham = hong lan sang he nhiem vu va he trang bi, sap tham, kho truy):**
- CHI DUOC push them `vSlots`.
- **KHONG** doi thu tu duyet `GetFirstItem()/GetNextItem()`.
- **KHONG** doi cach khai trien chong `:864-868` (`LuaRemoveItemByIndex` `KJx2WarInfra.cpp:896` tru dung tung don vi dua vao do).
- **KHONG** doi gia tri tra ve cua `KJx2WarInfra_GiveBoxCollect` (van la `vItems.size()`).
**Ly do:** ham dung chung — `grep -rn GetGiveItemUnit --include=*.lua SV\script` = **29 diem goi / 11 tep**: `global\equip_system.lua:167` · `global\seasonnpc.lua:415, 502, 509, 542, 543, 553, 585` · `event\jiefang_jieri\200904\shuizei\shuizei.lua:73` · `lib\composeex.lua:185` · `missions\citywar_global\infocenter_head.lua:230, 260` · `missions\fengling_ferry\fld_head.lua:207, 270` · `missions\yandibaozang\yandiduihuan.lua:60, 94, 108, 111` · `item\test_loren_admin.lua:363, 472` · `task\tollgate\killer\nieshichen.lua` · `vng_event\give_support_item.lua` · va 4 luong Phi Phong khac `npc.lua:268, 314, 345, 389`.

### 6.2 [B] Lua — shim chon lo (`npc.lua:172-242`)
Thay khoi gom (`:185-210`) va khoi tu xep (`:223-242`) bang vong doc theo o:
```
local tbItemIdx = {0,0,0,0,0,0};   -- [1..5] = lo 1..5, [6] = Phi Phong
for i = 1, nCount do
   local nIdx  = GetGiveItemUnit(i);
   local nSlot = GetGiveItemSlot(i);   -- 0..4 = lo 1..5 ; 5 = o Phi Phong
   ... tbItemIdx[nSlot + 1] = nIdx;
end
```
**BA DINH CHINH bat buoc so voi ban dac ta cu:**
- (a) **Bo nhanh `szTrung` ("hai mon cung mot o")**, hoac kiem `szChong` TRUOC no. Hai mon KHONG THE cung `(nX,nY)`: `KInventory.cpp:65-90` tu choi o da co do, `KItemList.cpp:3438-3454` gop chong khi cung loai. Nhanh do chi no khi mot O chua mot CHONG — ma chong thi `:864-868` khai trien thanh N muc cung `nIdx` + cung `nX` ⇒ bao oan "hai mon cung mot o" trong khi thu pham la chong.
- (b) **Tach nhanh rieng cho Phi Phong dat nham o da.** Neu khong, no roi vao `elseif eType ~= ITEM_TYPE_XINGCS then szLa = ...` ⇒ hien "…<Phi Phong> khong phai da kham" — tu mau thuan. Phai bao "Phi Phong phai dat vao o giua".
- (c) **Hang rao:** `local bCoSlot = (GetGiveItemSlot ~= nil);` — neu `false` thi roi ve duong tu-xep cu (`npc.lua:223-242`) thay vi no ScriptError (luat 0.4).
Giu nguyen 4 thong diep loi da vá 31/08 (doc `tbItemIdx[6]` cho "Hay dat vao Phi Phong", quet `tbItemIdx[1..5]` cho "Hay dat vao Tinh Than Thach") va van goi `PF_InlayMoLai()` cuoi cung. Xoa `nMaxHole`/`tbHasStone`/`nPut` (chi khi `bCoSlot`).

### 6.3 [B] Lua — MOT vong `for`, BA hang rao trong `CheckInlayStarStone`
Chen ngay sau `head.lua:738` (dong `end` dong nhanh `nMaxStoneNum <= 0`), duyet `for i = 1, nSize do if tbItemIdx[i] ~= 0 then`:
1. **Lo chua mo** — `if i > nMaxStoneNum then szErrorMsg = "Phi Phong nay moi mo %d lo kham, lo so %d chua mo."; return 0; end`.
   Ly do: `KJx2WarInfra.cpp:2270-2274` tu choi LANG LE (tra 0) nhung `head.lua:797` van an vien da va `:804` van bao "thanh cong". (Linux `head.lua:734-737` cung chi kiem `nMaxStoneNum <= 0` — la BO SUNG so voi Linux, khong phai khoi phuc.)
2. **Lo 0 sao** — `if GetStoneLevelOnEquip(self.nMantleIdx, i) < 1 then szErrorMsg = <dung cau da co o head.lua:748>; return 0; end`.
   Ly do: `CheckInlayStarStone` chi kiem cap sao cua lo **DA CO DA** (`head.lua:743-753`: `local nP = tbHasStone[i]; if nP and nP ~= 0 then ... if nLevel == 0`). Lo TRONG cap 0 khong bi kiem gi ca — o CA HAI cay. Ma lo 0 sao **chac chan ton tai** (`head.lua:501` `for i = 1, nCurHoles-1`; `KItem.h:334-346` cap lo mac dinh 0). Ket qua: `InlayStarStone` chap nhan → an vien da → bao thanh cong → `KItem.cpp:176-178` (`if (nLv < 1 || nLv > 10) continue;`) khong cong gi. Ban Linux **co san cau thong bao** cho ca nay (ini `[ReturnInfo_Starstone]` dong 412, dung o `head.lua:748`) nhung dieu kien bao quanh khien no khong bao gio no cho lo trong.
3. **Lo dang co da** — `if tbHasStone[i] and tbHasStone[i] ~= 0 then szErrorMsg = "Lo so %d da co <ten da>, hay chon lo trong."; return 0; end`.
   Ly do: Linux CHO ghi de lo khac loai (`head.lua:756-769` chi chan khi TRUNG loai) va viec cu **mat vinh vien** (`KJx2WarInfra.cpp:2259` ghi ro "ban Linux khong co duong go ra"). JX1 hom nay chua ghi de duoc; 6.1 mo ra kha nang do lan dau. **TU CHOI thang, khong "canh bao"** — panel nay khong co co che xac nhan hai buoc (mo bang `OpenMantleInlayBox` `KJx2WarInfra.cpp:2402`, nut OK ban thang `GOI_ADD_UI_CMD_SCRIPT` `UiMantleInlay.cpp:292`; `SetUiGiveItemMoreConfirmMsg` thuoc hop `GiveItemUI`, khong phai panel nay).
   **[Q]** Hang rao 3 la **LECH CO Y** so voi Linux. Neu chu game doi giong Linux 100% thi bo no — nhung phai co cach hien thi da cu tren panel truoc (8.3).
Chuoi TCVN3: chep byte theo luat 0.1.

### 6.4 [B/T] C++ client — nhan so lo + chot `PF_UI_INPUT`
- `UiMantleInlay.cpp:234-235`: hien dang `SetText(i == PF_UI_INPUT_COUNT - 1 ? "Phi Phong" : "Tinh Th\307n Th\271ch")` — ca 5 o da cung mot nhan. Doi thanh `sprintf(sz, "L\347 kh\266m %d", i + 1)`, o thu 6 giu "Phi Phong". **CAM** `"L\307 kh\271m"` (= "Lầ khạm").
  **Chi lam CUNG DOT voi 6.1** — danh so 1..5 len panel khi may chu van bo qua vi tri la noi doi nguoi choi, te hon nhan trung.
- `UiMantleInlay.cpp:147` `PF_UI_INPUT[6] = { 10, 11, 12, 0, 1, 13 }` (3 o hang tren `(12,12)/(112,12)/(212,12)` + 2 o to 28x28 `(58,22)/(162,22)`) — **mot mo chap va, khong phai mot nhom mach lac**. Chot lai theo ket qua 5.7.
- Sua chu thich sai `UiMantleInlay.h:40-41` ("3 o cuoi la cho dat PHI PHONG") — chi `obj_13` giu vai Phi Phong.

### 6.5 Nghiem thu DOT 6 (bat buoc lam du)
- (a) GM tao phi phong 3 lo: lo 1-2 cap 10, lo 3 cap 0.
- (b) Tha 1 vien vao **o so 3** → bao "chua duoc tang sao", vien da **CON trong tui**.
- (c) Tha vao **o so 4** → bao "chua mo lo", da con.
- (d) Tha vao **o so 1** → kham vao **DUNG LO 1** (kiem bang tooltip — `PF_AppendDesc` in tung lo).
- (e) Tha lai vao **o so 1** → bao "lo da co da".
- (f) **Hoi quy 5 luong khac khong duoc hong:** `npc.lua:268` (dot pha Phi Phong), `global\equip_system.lua:167`, `global\seasonnpc.lua:415`, `lib\composeex.lua:185`, `missions\yandibaozang\yandiduihuan.lua:108` (**nhay nhat** — goi `GetGiveItemUnit(nCheckedId)` theo chi so DONG).
- **Thu tu swap:** `CoreServer.dll` **TRUOC hoac CUNG LUC** roi moi dat `.lua`; `Game.exe` swap cung dot (client phai tat).

---

## DOT 7 — DONG BO `m_nPfPack` SANG CLIENT (build CA server LAN client, swap DONG THOI)

### 7.1 [B] Client mu hoan toan ve so sao / so lo / da da kham
- **Su that:** `SRC\Core\Src\KItem.h:161` `int m_nPfPack[4]` (chu thich `:156-160`), bo tra cuu `:286-341` (`PF_MAX_STONE`=5, `GetStarLevel`, `GetMaxStoneNum`, `GetStoneId`, `GetStoneLevel`, `GetMaxWishValue`). Grep `PfPack` toan cay: chi `KItem.h`, `KItem.cpp:44` (memset trong ham dung) va `KPlayerDBFuns.cpp:713-716` (nap tu CSDL) + `:1079-1082` (ghi xuong).
- `struct ITEM_SYNC` (`KProtocol.h`, ket thuc dong 1030) **KHONG co truong nao**; `KItemList::SyncItem` (`:4671-4718`) gan `m_Mantle` o **`:4696`** nhung khong gan gi tuong duong; `KProtocolProcess::s2cSyncItem` (bat dau `:1572`) khong he goi `SetPfPack`.
- Trong khi do `PF_StarPrefix` (`KItem.cpp:198-209`) va `PF_AppendDesc` (`KItem.cpp:216-256`) duoc `GetDesc` goi (`KItem.cpp:1451`, `:2407`; cu the `:1522/1527/1567/1573/2027/2440/2897`), va `GetDesc` chay **PHIA CLIENT** (`CoreShell.cpp:981-1060` va `:1149-1161`).
- **Hau qua do duoc:** client `m_nPfPack` toan 0 ⇒ `GetMaxStoneNum`/`GetStarLevel`/`GetMaxWishValue` = 0 ⇒ `PF_AppendDesc` thoat ngay o `KItem.cpp:225`, `PF_StarPrefix` tra chuoi rong. Ten mon khong co tien to "N sao "; bang mo ta mat sach so sao / diem chuc phuc / lo kham / thuoc tinh da. Thuoc tinh **VAN an** vao nhan vat (server tu ap qua `PF_ModifyStoneAttrib`, `KItem.cpp:165-192`) ⇒ chu game se thay "kham xong ma khong thay gi thay doi" va nghi he thong hong.
- **Hai cach:**
  - (a) Them 4 truong `int` (hoac `int[4]`) vao cuoi `ITEM_SYNC`; gan trong `KItemList::SyncItem` canh `:4696` bang `Item[nIdx].GetPfPack(0..3)`; phia client, nhanh `s2c_syncitem` trong `KProtocolProcess.cpp` goi `SetPfPack(i, ...)`. Sau do `PF_StarPrefix`/`PF_AppendDesc` tu song, khong sua them dong nao.
  - (b) **RE HON VE RUI RO — de xuat chon:** dung mot goi RIENG kieu `ITEM_SYNC_MAGIC` (`KProtocol.h:1033-1038`), chi gui cho vat pham co `GetMaxStoneNum() > 0`, thay vi phinh `ITEM_SYNC` cho MOI vat pham trong game.
- **Rui ro cua (a):** `sizeof(ITEM_SYNC)` o `KProtocol.cpp:67` tu doi ⇒ **lech ban la lech MOI goi `s2c_syncitem`**, khong rieng phi phong. BAT BUOC swap dong thoi `CoreServer.dll` + `CoreClient.dll` + `Game.exe`, khong duoc swap le.
- **Nghiem thu:** GM tao phi phong 5 sao, 3 lo, kham 2 vien → ten mon co tien to "5 sao "; re chuot thay du 3 dong lo (2 dong co ten da + 1 dong "Lo kham trong") va dong diem chuc phuc.
- **Ghi chu:** chi sau DOT 7 thi cau "tang sao xong khong thay gi" moi tach bach duoc voi 8.1 (cap sao that su khong cong thuoc tinh).

---

## DOT 8 — QUYET DINH / GHI CHEP / NO CON LAI

### 8.1 [Q] Cap SAO cua Phi Phong khong tham gia BAT KY phep tinh thuoc tinh nao
Grep `GetStarLevel()` toan cay `Sources`: 

dung 4 vi tri — `KItem.h:290` (dinh nghia `return m_nPfPack[0] & 0xF`), `KJx2WarInfra.cpp:2139` (`LuaPF_GetStarLevel`), `:2153` (trong `LuaPF_StarLevelUp`), va HAI cho **chi de HIEN THI**: `KItem.cpp:202` (`PF_StarPrefix`) + `KItem.cpp:221` (`PF_AppendDesc`). **Khong duong thuoc tinh nao doc no.**
Duong cong thuoc tinh that = `PF_ModifyStoneAttrib` (`KItem.cpp:165-192`), noi doi xung vao `ApplyMagicAttribToNPC` (`KItem.cpp:327`) va `RemoveMagicAttribFromNPC` (`KItem.cpp:415`); no lay `p->nValue[nLv-1]` voi `nLv = GetStoneLevel(i)` = **CAP CUA TUNG LO**. Cap lo chi duoc dat o hai cho, **deu la duong DOT PHA**: `head.lua:501` va `head.lua:686` (`SetStoneLevelOnEquip(..., i, 10)`). Duong TANG SAO (`ProcessStarUpGrade`, `head.lua:336-386`) chi goi `StarLevelUp` (`:364`) roi `SetItemBindState` + `SyncItem` — khong cham cap lo, khong cham thuoc tinh.
**Hau qua:** tang sao la che do SONG chinh (`npc.lua:13`), moi lan ton **100 van bac** (`npc.lua:37` `nNeedMoney = 1000000`) + Tinh Ngoc, co the that bai — doi lai chi duoc mot tien to ten mon va quyen di tiep vao Dot Pha (`CheckBreakThrough` doi 10 sao). **Khong mot diem chi so nao.**
**Hoi chu game:** co muon cap sao anh huong thuoc tinh khong.
- Neu **CO**: cach re nhat va thuan Lua — sau khi `StarLevelUp` thanh cong (`head.lua:364-368`) goi them `SetStoneLevelOnEquip` cho cac lo theo cap sao moi. **DOI CAN BANG LOI CHOI, rui ro CAO, tuyet doi phai co chu game duyet ti le, khong tu quyet.**
- Neu **KHONG**: sua chuoi `npc.lua:12` (gop vao 1.9) va ghi ban giao "tang sao = dieu kien vao Dot Pha".
**Khong kiem chung duoc:** ben Linux co lien ket sao ↔ cap lo hay khong — khong doc duoc engine tu hai cay duoc phep. Dung khang dinh.

### 8.2 [Q] Co mo nhanh VO CUC / HUYEN KIM khong
`npc.lua:51-52` hai dong menu ("Nang cap Vo Cuc/onAdvanceToWuJi", "Vo Cuc dot pha lan 2/onBreakThroughWuJi") bi chu thich **SAN TU BAN LINUX** — JX1 chep trung thanh. `onAdvanceToWuJi` (`npc.lua:77-93`), `onBreakThroughWuJi` (`:95-111`) va 4 ham `head.lua:524, 539, 578, 655` la ma chet o CA HAI cay (grep toan cay script JX1, ke ca `test_phiphong_admin.lua`, khong noi nao goi). Bang `tbSecBreakThrough` (`head.lua:95-127`) va cac hang `platinaequip` 3485 / 4835-4839 **da port day du** va trung noi dung ban Linux.
Hom nay chuoi thang tien dung o "Phi phong Ngu Phong" (goldequip hang 5378): `head.lua:71-76` dat `eBreakType = "AdvBreak"` cho 5378, menu chi mo duong ProBreak, nen `CheckBreakThrough` (`head.lua:407-410`) tra loi chi dan toi mot chuc nang khong co trong menu.
**Neu mo:** phai vá 1.5 (`szLockState`) truoc, va nghiem thu ca duong platina (`AddPlatinaItem` + `UpgradePlatinaItem x10`, `head.lua:681-682`) — duong nay **chua tung chay tren JX1**.
**Phai trinh chu game, khong duoc tu sua:** `head.lua:501` (dot pha thuong) chay `for i = 1, nCurHoles-1` (bo lo cuoi) trong khi `head.lua:685-686` (dot pha lan 2) chay `for i = 1, nCurHoles` (du lo). Chenh lech nay **giong het Linux**.
**CAM** phuong an "xoa han 2 ham + 4 ham + bang `tbSecBreakThrough`": tu y lech khoi moc Linux, khong duoc loi gi, phai lam lai neu sau nay mo nhanh Vo Cuc, va `test_phiphong_admin.lua:59` con ghi chu tham chieu toi bang do.

### 8.3 [T] Ve da DANG kham len panel — GOI VIEC RIENG, khong gop vao DOT 6
Them binding client doc `Item[n].GetStoneId(i)` / `GetStoneLevel(i)` roi ve vao 5 o hoa van (`m_Obj[]`). Phu thuoc DOT 7 (client phai co `m_nPfPack` truoc mac cham).
**GHI CHU QUAN TRONG — bac bo khang dinh cu "client JX1 KHONG co duong nao doc `GetStoneId`":** nguoi choi **DA CO** duong xem. `KItem::PF_AppendDesc` (`KItem.cpp:216-256`, vong `:245-256`) in **MOI LO MOT DONG** vao bang mo ta vat pham — doc `GetStoneId(i)` + `GetStoneLevel(i)` roi in `"%d sao <ten da>"` hoac `"%d sao Lo kham trong"` (`:253`). Vong nay **khong** bi `#ifndef _SERVER` (guard do mai o `:259`, chi boc phan dong thuoc tinh), va ham duoc goi tu **ca hai** ban `GetDesc` (`KItem.cpp:2027` va `:2897`). Re chuot len Phi Phong la thay du lo nao co da gi, cap may, ke ca khi Phi Phong dang nam trong panel kham.
⇒ Day chi la **tien nghi**, KHONG bat buoc. Va da co san khuon logic de chep sang panel khi lam.
⇒ Cung vi vay: **bo han** phuong an "dung `Talk`/`Msg2Player` liet ke lo nao dang co da gi (dua `GetStarStoneOnEquip` + `GetStoneLevelOnEquip`)" — thua.

### 8.4 [T] `login_add` — tuy chon, de sau
`head.lua:815` `if login_add then login_add(PlayerLogin, 2) end`; `SV\script\global\login_head.lua` **khong ton tai**; `login_add` khong co trong `ScriptFuns.cpp`/`LuaFuns.cpp`. `PlayerLogin()` (`head.lua:809-813`) la ma chet. Khong sap, khong ScriptError (co menh de bao ve; doc mot global nil trong Lua 4.0 la hop le). Mat mat: mot dong nhac mau vang luc dang nhap ve he Phi Phong.
**Day la NO CO SAN cua ca cay JX1**, khong phai khiem khuyet rieng he Phi Phong: cung thanh ngu `if login_add then` da co o `SV\script\missions\leaguematch\wlls_login.lua:47` va `SV\script\task\metempsychosis\task_func.lua:218`. Neu lam thi **lam CHUNG cho ca 3 cho**, khong vá rieng.

### 8.5 [B] `package.ini` — **TUYET DOI KHONG XOA DONG** (viec da de xuat truoc day la CO HAI)
`CL\package.ini` khai 36 khoa (0..35); thu muc `CL\data` co 37 pak; dung **4 tep duoc khai ma khong co**: `sprvlngaothe2.pak`, `settings.pak`, `ui.pak`, `script.pak`. 32 pak con lai mo duoc (183.001 ma tep).
`KPakList::Open` (`SRC\Engine\Src\KPakList.cpp:144-149`) duyet khoa 0,1,2,... va **BREAK ngay khi thieu mot so** ⇒ xoa dong `3=sprvlngaothe2.pak` ma khong danh so lai se lam client **ngung nap ca 32 pak con lai (4..35)** = mat gan het tai nguyen. Hien tai pak thieu chi lam `XPackFile::Open` that bai roi bi bo qua — **hoan toan vo hai**.
Neu van muon don: phai **DANH SO LAI lien tuc 0..31** va chay thu client truoc khi giao.
**Anh phi phong: KHONG THIEU** — quet 75 duong `.spr` ma cac bang JX1 tro toi (magicscript 9 dong phi phong, `starstone.txt`, `goldequip`/`platinaequip` DT 11-14, `mantle`/`signet`/`shipin`), bam ten bang `KPakList::FileNameToId` (`KPakList.cpp:72`, `char` CO DAU) + ha chu thuong ASCII theo `g_GetPackPath` (`KFilePath.cpp:259-274`): 68 nam trong pak, phep thu am (ten bia) tra ve khong tim thay. Ngoai le da ghi nhan: 6 duong `\spr\iteman\an{bach,kim,moc,thuy,hoa,tho}.spr` (`goldequip.txt:7382..7387`) khong nam trong pak nao ma la **TEP ROI tren dia** — van hien duoc vi `KPakFile::Open` (`KPakFile.cpp:249-259`) doc DIA TRUOC roi moi tra pak; va `\spr\item\obj_yingchundai.spr` (`platinaequip.txt:763`) **thieu that** nhung **co san TU TRUOC dot port** (ban `platinaequip.txt.truoc_phiphong_2908` cung tro toi no).

### 8.6 [B] Ghi 3 MA TAM vao ban giao thanh mot muc rieng
1. **`LuaCurPack` / `LuaUsePack`** (`ScriptFuns.cpp:2217-2226`, dang ky `:14708-14709`) la **STUB tra 0** — chinh chu thich `:2213-2216` thua nhan "chuyen pack la no-op". Day la **GOC** cua 3.3 va se con khien moi dang ky lien-tep khac chet im lang. **Khong can dong vao** — sua thanh pack that la doi kien truc nap script, khong can thiet neu moi noi deu `Include` tuong minh.
2. **`LuaHD3_AddStatData`** (`KJx2WarInfra.cpp:1167-1197`) khong bom vao kenh thong ke nao ma ghi dong text vao `logs\hd3_statdata.log` (chu thich ghi ro "nguoi van hanh gop bang tay/script"). 5 diem dem cua he Phi Phong do het vao do: `head.lua:383` (`pifeng_star_up`), `head.lua:519` (`pifeng_level_up`), `head.lua:799` (`pifeng_stargem_in`), `item_starore.lua:53`, `item_starstone.lua:34`. Muon tra loi "bao nhieu nguoi da tang sao" phai doc tay tep log.
3. **`WXY_MoBang()`** (`wuxingyin.lua:296-306`) thay cho `eventsys` `Reg` (xem 4.2).
**Kem canh bao ngoai pham vi:** 12 tep khac trong cay JX1 co `EventSys:...:Reg(` — moi tep phai kiem RIENG xem diem ban co `Include` tep dang ky khong: `other_func_outputs.lua`, `huoyuedu\huoyuedu.lua`, `missions\bairenleitai\hundred_arena.lua`, `missions\challengeoftime\chuangguang30.lua`, `missions\leaguematch\award.lua`, `missions\tongcastle\game.lua`, `missions\tongcastle\treedeath.lua`, `petsys\summon.lua`, `task\metempsychosis\task_func.lua`, `task\metempsychosis\translife_5.lua`, `translife_6.lua`, `task\tollgate\killer\kill_level.lua`.

### 8.7 [T] Dang ky giao thuc con song — dung xoa `doMantleMosaicStone`
`SV\script\script_protocol\protocol_def_gs.lua:162-166` **DANG DANG KY**: `{"emSCRIPT_PROTOCOL_MANTLE_DOINLAY", "\\script\\global\\mantlesystem\\mantleupgrade_npc.lua", "doMantleMosaicStone", {OBJTYPE_TABLE}}`. Ham goc cua Linux tuy khong toi duoc (client C++ khong gui — grep `MANTLE_OPENUI`/`MANTLE_INLAYRESULT` tren `SRC\S3Client` = 0) nhung **KHONG DUOC XOA neu khong go dang ky cung luc**.
Cung o day, ghi nhan (vo hai): `g_ReturnInlayResult` duoc goi SONG tu `head.lua:781` va `head.lua:803` trong `ProcessInlayStarStone` ⇒ moi lan kham deu ban mot goi khong ai nghe.
Thu muc `mantlesystem` cua JX1 khong co `mantle_test_npc.lua` — **khong can port** (xem 0.6).

### 8.8 [T][Q] Goi S2C bao KET QUA kham — chi lam neu chu game doi, va phai THEM chu khong THAY
- **Linux:** `LX\server1\script\protocol.lua:44` khai `emSCRIPT_PROTOCOL_MANTLE_INLAYRESULT`; `LX\server1\script\dailogsys\dailogsay.lua:175-180` `g_ReturnInlayResult(nSuccess)` day mot so (0/1) ve client qua `ScriptProtocol:SendData`; goi o 3 cho: `head.lua:777` (`OnCheckLock` truot → 0), `head.lua:799` (kham xong → 1), `npc.lua:141` (`OnCheckItem` truot → 0). `protocol_def_gs.lua:163` chi dang ky `DOINLAY` ⇒ `INLAYRESULT` la S2C thuan.
- **JX1:** grep `MANTLE` toan `SRC` chi ra **mot** ma: `CoreShell.h:544` `GDCNI_OPEN_MANTLE_INLAY`, phat o `KProtocolProcess.cpp:4614`, nhan o `GameSpaceChangedNotify.cpp:794-799`. Khong co ma thu hai.
- **CAM (se lam hong panel):** bo `PF_InlayMoLai()` de thay bang `PF_ReturnInlayResult(0/1)` tai `npc.lua:168` va `:251`. `PF_InlayMoLai` la **CHIU LUC** — goi lai `OpenMantleInlayBox` la thu DUY NHAT nap lai phien give-box (`KJx2WarInfra.cpp:2410` dat `m_dwGiveBoxId`, `:2418` dat `s_GivePending`), ma ca hai bi xoa ngay sau lan bam OK dau (`KProtocolProcess.cpp:6502` va `KJx2WarInfra.cpp:852`). Bo no = nut "Kham nam" chet tu lan bam thu hai = **dung lai lo da vá hom 31/08**.
- **CACH RE NHAT neu chi muon doi cho hien thong bao** (tu `Talk` sang khung `m_Guide`): `OpenMantleInlayBox` **da co san tham so noi dung** (`Value1`, 256 byte) — truyen thang cau loi vao do khi mo lai. Khong them `ProtocolType`, khong them `OnResult`, khong dong vao bang kich thuoc goi `KProtocol.cpp`.
- Neu chu game van doi goi ket qua that: **THEM** `PF_ReturnInlayResult` canh `LuaPF_OpenMantleInlayBox` (`KJx2WarInfra.cpp:2402`), dang ky o `ScriptFuns.cpp:15815`, them `GDCNI_MANTLE_INLAY_RESULT` canh `CoreShell.h:544`, nhanh xu ly trong `GameSpaceChangedNotify.cpp` goi `KUiMantleInlay::OnResult(int)` (ghi cau ket qua vao `m_Guide` thay vi mo lai ca panel, roi chi `UpdateData()`). Uu tien phuong an mo rong `S2C_GIVE_BOX` them `nType=4` mang 1 so nguyen — **tranh** them `ProtocolType` moi (lech mot o trong bang kich thuoc goi `KProtocol.cpp` la lech toan bo giao thuc phia sau). Swap CA `CoreServer.dll` lan `Game.exe`.
- Ghi chu trieu chung hien nay: sau khi bam "Kham nam" panel dung yen cho toi khi goi mo-lai ve toi ⇒ nguoi choi bam them lan nua; va moi lan mo lai `KUiMantleInlay::OpenWindow` (`UiMantleInlay.cpp:49-51`) XOA sach `m_Guide` roi nap lai cau huong dan CHUNG, con `BringToTop()` (`:62`) day panel len tren, de che mat cau `Talk` vua hien.

### 8.9 [Q] Nhanh CHUA BAT DAU — ghi ro vao ban giao
- **MAT NA** (`goldequip.txt` DetailType 11, **231 dong**) va **TRANG SUC** (DetailType 14, **150 dong**): **KHONG co he thong script nao** trong cay JX1. Moi xong bang du lieu + o trang bi + hien thi. Day la hai nhanh con lai cua dot port, phien sau phai lam.
- **AN**: xong du lieu + script, thieu loi vao (4.2) va lech 1 cap (4.1).
- **PHI PHONG**: xong phan loi choi tang sao/dot pha; con thieu chon lo (DOT 6), hien thi sao/lo o client (DOT 7), va nguon vat pham (3.1/3.2/3.3).

---

## D. DA BAC BO — DUNG DAO LAI

### D.1 Muc bi bac bo hoan toan
| Ma | Noi dung bi bac bo | Ly do (neo) |
|---|---|---|
| **DATA-01** | "3 cot sao/lo/chuc phuc cua goldequip+platinaequip khong bao gio duoc nap ⇒ moi phi phong 0 lo, kham luon that bai" | Doan nap DA TON TAI va DA duoc goi: `KItemGenerator.CPP:26-41` (`PF_GCOL_CANUPSTAR/MAXSTONE/MAXWISH = 60/61/62`, `PF_PCOL_* = 69/70/71`), `:51-100` `PF_GetRowInfo` (KTabFile, cache, `nRow = nIndex+2` mirror `LoadRecord`), `:104-115` `PF_ApplyRowInfo` goi **dung ba ham** `SetMaxStoneNum`/`SetMaxWishValue`/`SetCanUpStar`; goi tai `:1930` (`Gen_GoldEquipment`) va `:2082` (`Gen_PlatinaEquipment`). **Goc sai = luat 0.2.** |
| **CPP-02** | "`GetMaxEquipWishValue` luon tra 0 ⇒ ti le dot pha LUON 100%" | `SetMaxWishValue` duoc goi tai `KItemGenerator.CPP:114`. Luu tru cung day du (`KPlayerDBFuns.cpp:713-716` / `:1079-1082` qua `iiduphong5..8`). Them mot sai du lieu: "dong 5375 cot 62 = 6" — that ra `:5375` c60/61/62 = 1/1/4, `:5376` moi la 1/2/6. |
| **CPP-03** | "`GetEquipMaxStoneNum` luon tra 0 ⇒ duong kham chet han, dot pha xoa sach da" | `SetMaxStoneNum` tai `KItemGenerator.CPP:113`, `SetCanUpStar` tai `:115`. `goldequip.txt:5375-5379` c61 = 1,2,3,4,5; `LuaPF_AddGoldEquipByRow` (`KJx2WarInfra.cpp:2327-2345`) tru 1 khop `GetGlodEqIndex` (`KItemCompound.cpp:1777-1785`). **De xuat noi `KBASICPROP_EQUIPMENT_GOLD2` la NGUY HIEM** — `LoadRecord` anh xa cot THEO VI TRI, danh sach do da co 2 muc bi chu thich (`m_nNeedToActive1/2`) roi chen `m_aryMagicAttribs[6],[7]` sau (chu thich `KItemGenerator.CPP:17-24`); lam theo la lieu hong toan bo bang trang bi vang. |
| **DATA-02** | "`platinaequip.txt` co cot du ⇒ phai hardcode hai bo chi so rieng" | Viec doc bang **DUNG 100%** (o thu 71 rong; `goldequip.txt:5375` c60/61/62 = 1\|1\|4 con `platinaequip.txt:5301` c68..71 = 200\|1\|1\|4) nhung viec can lam **DA LAM XONG y het**: `KItemGenerator.CPP:17-41` (chu thich neo `goldequip 5374..5378` + `plat 5300..5304` + `4835..4839`) va `:63-66` chon cot theo co `bPlatina`. |
| **DATA-06** | "`mantle.txt`/`shipin.txt`/`signet.txt` khong he duoc engine JX1 nap" | Duoc nap: `KBasPropTbl.CPP:38-40` (TABFILE_*), `:62-81` (chi so 12/13/14 trong 17 muc), `:154-159` (vong `Init` i<17), `:202` (`Load()`), `:1406-1408`. **Goc sai = luat 0.2** (`.CPP` viet HOA). |
| **DATA-07** | "`mask.txt` giu 840 dong cu, chi them 64 dong moi" | So tung o: **840/840 dong cu deu doi** — xem 3.5. Nguoi bao cao chi so sanh COT TEN. |
| **LUA-04** | "`GetServerOpenDays` tham chieu bang khong ton tai ⇒ bay" | Duong tinh gia. Hai cho goi bi CHU THICH SAN o **CHINH BAN LINUX** (`LX\...\head.lua:417-418` va `:613-614`), y het JX1. Ma chet o ca hai cay; than ham khong duoc danh gia luc nap ⇒ khong the ScriptError. |
| **DATA-05** | "Muc xep chong 2 nguyen lieu khac Linux (0 vs 50)" | So lieu dung nhung tac dong ~0. PT 4888 "Tinh Than Thach" cua magicscript la NGUYEN LIEU G=6, **khong phai da kham** (da kham la G=9 tu `starstone.txt`) nen muc xep chong khong dinh gi den kham. PT 4882 thi JX1 xu ly chong DUNG (sau khi vá 1.1). |
| **UI-RING-05** | "8 o vong hoa van co vai tro HIEN THI trang thai lo" | So lieu khong sai mot chu, nhung **tien de bi bac bo**: (a) ini ngoai dong 1 tu goi la "giao dien DUC TRANG BI", chu thich dong 34 la "nang cap Huyen Kim" ⇒ 8 o hoan toan co the la o NHAN DO cua the Huyen Kim; (b) trong ca cay Linux **khong ham nao** cham toi 8 o do (`onMosaicStone` `npc.lua:113-123` la handler duy nhat mo bang; 4 thao tac con lai dung `GiveItemUI` thuong); (c) so hoc khong khop: 8 o vs toi da 5 lo o JX1 vs "lo 1-7"/"lo 8-10" o ini. |
| **UI-MSG-06** | "20 cau `[ReturnInfo_*]` khop mot-doi-mot voi cau server tra ve" | May chu Linux **KHONG BAO GIO gui ma loi** — `g_ReturnInlayResult` day dung MOT so, ca 3 cho goi chi truyen 0 hoac 1. Grep `ReturnInfo` toan `LX\server1\script` = **RONG**. Loi that di bang CHUOI qua `Talk(1,'',szErrorMsg)`. Bang do la di vat cua CLIENT Kiem The (chu thich `; foundry_succeed = 0,` ini ruot `:380-402` cho thay la enum phia client). Dinh chinh: `[ReturnInfo_Starstone]` = dong **406-413**; `Border` trong `[TextColor]` o dong **374**. |
| **LUA-01 (o-kham)** | "Bang `tbItemIdx` phai luon du 6 phan tu — thieu la tinh sai/no nil" | Duong tinh gia. JX1 da lam dung (`npc.lua:226` `local tbItemIdx = {0,0,0,0,0,nMantle};`). Ba cho `tinsert` (`:268/:314/:345`) thuoc BA tinh nang khac, va **ban Linux lam Y HET** (`LX\...\npc.lua:160-163`). Khang dinh "client Linux luon gui du 6 khe" khong kiem chung duoc. |
| **FILE-01 (phan)** | "`doMantleMosaicStone` la ma chet khong ai goi" + "chep `mantle_test_npc.lua` sang JX1" | SAI: `protocol_def_gs.lua:162-166` DANG dang ky no (xem 8.7). THUA: `test_phiphong_admin.lua` (492 dong) day du hon ban Linux 33 dong. |

### D.2 Khang dinh le bi bac bo (nam trong finding con dung)
- **"Client JX1 khong co duong nao doc `GetStoneId` cua Phi Phong"** — SAI, xem 8.3 (`KItem.cpp:216-256` + `GetDesc` `:2027`/`:2897`).
- **"Client Linux nhan biet so lo da mo / chan lo chua mo"** — **suy doan**, khong co ma client Linux trong hai cay duoc phep. May chu Linux **khong** chan lo TRONG 0 sao (`LX\...\head.lua:744-752` chi soi lo DA CO DA) — y het JX1.
- **"`OnCancel` chi gui 1 goi cho CA hop, khong phai vong lap tung mon"** — SAI, xem 5.1 (`CoreShell.cpp:13292-13314`).
- **"Kham vao lo 0 sao = mat da VINH VIEN"** — thoi phong, xem 1.3 (mat tam thoi/vo hinh).
- **"JX1 con 3 nguon that su ban su kien"** — SAI, la **0** (3.3). Va **"FengLingDu 3 diem ban"** → thuc te **2**.
- **"Them 5 dong magicscript cho hop Phi Phong"** → chi **1 dong** (3.2). **"Cot script la cot 14"** → cot **10**.
- **"`Sale(N, moneyunit)` 2 tham so"** → nuot moneyunit, phai **≥3 tham so** (3.1). Va **"tham so 2 phai nam trong 0..6 vi co chot kiem tra"** → nhanh `OpenSale(int,int,int)` KHONG kiem.
- **"`AFFAIRITEM_ROOM_WIDTH = 6` nen chi 6 o, khong noi them duoc"** → SAI, `HEIGHT = 4` va truc `v` dang bi bo khong (5.6) ⇒ con nguyen 24 o dia chi duoc; tran 6 la lua chon.
- **"`PF_UI_INPUT` chac chan phai la `{5,6,7,8,9,13}`"** → chua co bang chung, la **tung dong xu** giua hai vong (5.7).
- **Ma TCVN3 `"L\307 kh\271m"`** → ra "Lầ khạm" (0.1).
- **"Xoa 4 dong khai bao pak thua trong `package.ini`"** → **CO HAI**, xem 8.5.
- **"Go 9 handler khong bao gio chay khoi `other_func_outputs.lua`"** / **"Xoa `GetServerOpenDays`, xoa nhanh Vo Cuc"** → tu tao lech khoi moc Linux, xem 3.3 / 1.9 / 8.2.
- **"Them o chuoi thu 4 vao `S2C_GIVE_BOX`"** → sai uu tien; van de chuoi that la `Value1` DANG TRAN (1.2).
- **"Sap xep lai `vItems` theo `nX`"** → cam tuyet doi (6.1) — nhung day la **rang buoc**, khong phai loi dang xay ra; danh sach cu chi ke 4 tep, thuc te **29 diem goi / 11 tep**.
- **Neo lech dong (da nan):** `KItemList.cpp:3464` (khong phai 3459-3462) · `GameDataDef.h:376-378` (khong phai 387-389) · `KItemList.cpp:4696` cho `m_Mantle` (khong phai 4692) · `LuaRemoveItemByIndex` `KJx2WarInfra.cpp:896` (khong phai 894) · `UiMantleInlay.cpp:209`/`:213` cho `EnablePickPut` (khong phai `:222`) · `LX\...\npc.lua:134-138` cho vong map 1-1 (khong phai 125-142) · `head.lua:815` cho `login_add` (khong phai 814).

---

## E. DA KIEM VA **KHONG PHAI** KHOANG CACH (dung lam lai)

1. **Duong so hieu o di tron ven toi `PlayerItem.nX`** — 7 chang da doc lai het, khong bac bo duoc; **client khong phai sua gi cho phan co che** (xem bon canh DOT 6). Cung cap them: `wndObjContainer.cpp:367-377` `KWndObjectBox::DropObject` luon dat `Pick.pWnd = Drop.pWnd = this` cung mot o ⇒ khong co ca pick/drop lech o; khuon y het `KUiCompoundOne` (`UiCompoundItem.cpp:922-970`).
2. **Duong LOG chay that** — `WriteMantleLog` (`head.lua:166-173`) → `WriteLog` → `LuaJX2_WriteLog` (`KTongJX2.cpp:3999-4004`) ghi that vao `logs\script_jx2.log`. `GetLocalDate`/`GetAccount`/`GetName`/`GetCurServerTime` deu co dang ky (`ScriptFuns.cpp:15747`, `:15044`).
3. **Task 3550 an toan** — `MAX_TASK = 5200` (`KPlayerTask.h:28`); grep toan cay script JX1 va toan `Core\Src` khong co ai dung trung.
4. **Chuoi thuoc tinh vien da lanh lan** — `starstone.txt` (35 dong) + `magicattrib_ge.txt` (7206 dong) co du ca hai ben; cot 14 va 15-24 khop dung ma `PF_GetStoneInfo` (`KItem.cpp:124-160`) doc; `PF_ModifyStoneAttrib` noi **DOI XUNG** vao `ApplyMagicAttribToNPC` (`KItem.cpp:327`) / `RemoveMagicAttribFromNPC` (`KItem.cpp:415`) — khong lech cong/tru. Ma da nhet vao truong 6 bit (`KItem.h:322-333`) an toan vi `PF_GetStoneInfo` chan `nP >= 64` va ma da doc tu `starstone.txt` cot 4 deu < 64.
5. **`GetItemQuality`** tra dung 4 cho bach kim (`ITEMQUALITY_PLATINA = 4`, `KItemCompound.h:77`) ⇒ phep so `4 == nQuality` o `head.lua:194` chay dung. `equip_mantle = 12` (`GameDataDef.h:704`) khop `nG == 0 and nD == 12` (`head.lua:191`); `item_starstone = 9` (`GameDataDef.h:661`) khop `nG == 9` (`head.lua:203`).
6. **8 bang du lieu trung md5 tuyet doi giua `bin\server` va `bin\client`** (mask/mantle/signet/shipin/starstone/goldequip/platinaequip/magicscript) — `goldequip` = `ea06786d...`. Sua ben nao phai chep sang ben kia.
7. **Cot phi phong cua `platinaequip.txt` KHONG lech** — tieu de ghi 68/69/70 nhung du lieu that o 69/70/71; `platinaequip.txt:4836-4840` ("Phi phong Vo Cuc") dat 1,2,3,4,5 o **cot 70**, khop `PF_PCOL_MAXSTONE = 70` va khop `tbSecBreakThrough` (`head.lua:95-127`, `tbItemNeed={15,20,25,30,35,0}` tra bang `nStep = GetEquipMaxStoneNum+1` o `head.lua:636`). **Tieu de cua CA HAI cay deu lech nhu nhau — dung ai "sua" cho khop tieu de.**
8. **Mat binding day du** — boc tat ca ten ham toan cuc duoc goi trong `head.lua` + `npc.lua` roi doi chieu bang dang ky `ScriptFuns.cpp`/`LuaFuns.cpp`: chi `CreateTaskSay` khong co ben C++, va no la ham Lua co san o `lib\worldlibrary.lua:50`. `pack`/`clone` o `lib\common.lua:27` va `:144`; `PlayerFunLib` o `activitysys\playerfunlib.lua`. **Khong thieu ham nao.**
9. **`item_tianxingyu.lua` (PT 4883) khong dinh loi 2.1** — tu `ConsumeItem` 10 mon (gom chinh no) va `return 1` o moi nhanh.
10. **`AddItem` trong `item_jx1.lua`** goi lap N lan nhung `AddKIL` co gop chong (`KItemList.cpp:191-215`) ⇒ phan thuong 90 Tinh Ngoc chi ton 1 o tui, khong tran.
11. **`npc.lua:194`** tu choi vien da CON XEP CHONG (`GetItemStackCount > 1`) va bao nguoi choi tu tach. Da that (G=9, `starstone.txt`) hien khong ai dat muc xep chong nen tam thoi khong sao — nhung neu sau nay ban da qua magicscript thi phai dat **khong xep chong** cho **CHUNG** (khong phai cho hai dong nguyen lieu ma DATA-05 neu).
12. **Anh phi phong 43/43 co du trong pak** — xem 8.5.
13. **Ma phi phong DA SONG trong nhi phan dang chay** — xem 0.5.

---

## F. CHOT NHANH — CAI GI BAT BUOC, CAI GI KHONG

**BAT BUOC de dat "giong 100% Linux" hoac chan lo hong that (10 viec):**
2.1 (ITEM-01 tieu hao vat pham) · 1.1 (dem chong) · 3.1 (tiem) · 3.2 (duong lay phi phong + da) · 3.3 (nguon roi) · 6.1+6.2 (chon lo) · 7.1 (dong bo `m_nPfPack`) · 5.1 (Huy/ESC tra do) · 4.1 (lech 1 cap An) · 4.2 (loi vao An).

**BAT BUOC nhu HANG RAO kem theo ban vá (khong co o Linux, nhung phai co):**
6.3 (3 hang rao lo chua mo / lo 0 sao / lo da co da) · 1.4 (kiem tri tra `InlayStarStone`) · 1.3 (uu tien lo >0 sao) · 1.7 + 1.8 (mo lai phien give-box).

**TIEN NGHI / MY QUAN (lam duoc thi tot, khong chan ai):**
1.2 (tran chuoi) · 1.5 (`szLockState`) · 1.6 (spam chat) · 1.9 (sua chu) · 2.2 (ro phien) · 3.4 (goldequip) · 3.6 (canh bao PT) · 5.2..5.6 (panel) · 6.4 (nhan so lo) · 8.3..8.8.

**CHO QUYET DINH CUA CHU GAME (7 cau hoi, chan cac viec tuong ung):**
1. Ti le/nguon roi khi noi lai `other_func_outputs` (3.3) — chan 3.3.
2. Duong phat Phi Phong so cap va Tinh Than Thach (3.2) — chan 3.2.
3. Co giu co che khoa do vua mua o tiem khong (3.1 buoc 3).
4. 19 dong `goldequip` doi so — giu Linux hay giu JX1 cu (3.4); dac biet `:5379` cot 62 va `:5931-5933`.
5. `mask.txt` — co mat na cu trong CSDL khong, co lam bang anh xa khong (3.5). **RUI RO LON NHAT.**
6. Vong o kham nao la lo 1-5, va co an 8 o con lai khong (5.7 → 6.4).
7. Cap sao co cong thuoc tinh khong (8.1); co mo nhanh Vo Cuc khong (8.2).

**RUI RO CAO NHAT theo thu tu:** 3.5 (mask.txt doi ma vat pham cua do da phat) → 2.1 (nhan ban vat pham vo han) → 7.1 (doi khuon `ITEM_SYNC` = lech moi goi `s2c_syncitem` neu swap le) → 6.1 (`GiveBoxCollect` dung chung 29 diem goi) → 1.1 (lam lech ma so voi Linux, phai ghi ban giao).