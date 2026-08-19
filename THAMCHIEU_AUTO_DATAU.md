# THAM CHIẾU KỸ THUẬT: AUTO DÃ TẨU

> Phụ lục của `BANGIAO_AUTO_DATAU_WAUTO.md` (đọc tệp đó trước).
> Nội dung dưới đây được **trích tự động từ mã nguồn thật** (18-19/08/2026) — mọi khẳng định
> kèm `file:line`. Nếu sửa mã, số dòng sẽ lệch: dùng `grep` theo tên hàm/biến thay vì tin số dòng.

---

## PHẦN A — BẢN ĐỒ ENGINE (CoreShell.cpp)

Nguon duy nhat: `D:\GAMEDEVNEW\Sources\Core\Src\CoreShell.cpp` — khoi `2506` (`// ==== AUTO DA TAU (18/08/2026) ====`) den `3940` (`// ==== HET AUTO DA TAU ====`).
Diem vao: `CoreShell.cpp:8906-8908` `case ATYPE_DATAU: return DT_Process(nPlayerIdx, (const autoData*)nParam, uCurTime);`
Diem goi: `D:\GAMEDEVNEW\Sources\S3Client\S3Client.cpp:822-831` (chi goi khi `pApData->bDaTau == 1` — so sanh `==1` vi WAuto.exe cu gui struct ngan, duoi buffer la rac).

---

## 1. Bang cac PHA (enum DTPHASE) — khai bao `CoreShell.cpp:2516-2533`

| Pha (gia tri) | case tai | Nhiem vu | Chuyen sang pha nao (dieu kien chinh xac) |
|---|---|---|---|
| `DTP_IDLE` (0) | `3103-3123` | Diem xuat phat: kiem tra dang o thanh Da Tau khong (`g_DTNpc[i].nMapId == nMap`) | Khong tim thay map trong bang -> `DTP_RETURN` (`3112`, `nDTStep=DTI_NONE`, return 1). Co map -> `DTP_GOTONPC` (`3118`, `nDTStep=DTI_NONE`, `nDTRetry=0`, return 1) |
| `DTP_GOTONPC` (1) | `3148-3190` | Di den NPC Da Tau (template 108) va mo thoai | Khong co dong bang cho map hien tai -> `DTP_RETURN` (`3160`). Tim thay NPC va khoang cach <=128 mps -> chup 4 seq (`uDTDlgSeen/uDTTalkSeen/uDTFinSeen/uDTBoxSeen` `3172-3175`) + `DialogNpc` -> `DTP_WAITDLG` (`3177`, delay 700ms). Chua toi -> `DT_WalkTo(...,96)` (`3181`) hoac di toa do bang `pRow->nX*32, nY*32` bán kính 160 (`3184`); toi noi ma khong thay NPC 20 lan -> `DT_Hold` 5 phut (`3187`) |
| `DTP_WAITDLG` (2) | `3192-3408` | Doi + PHAN LOAI noi dung hoi thoai (day la bo nao) | `cap.uFinSeq` doi -> `DTP_REWARD` (`3199`). `cap.uBoxSeq` doi -> `DTP_GIVEBOX` (`3207`, `nDTRetry=0`). Marker `DTM_MSG_LIMIT` -> hold vinh vien + `nDTHoldFreeze=1` (`3244-3251`). `DTM_MSG_BAGFULL` -> hold 5' (`3254`). Fail-req -> `DTP_USEPD` neu T5/PD (`3261`), hoac thu ung vien ke -> `DTP_GOTONPC` (`3270`), hoac T5-exp -> `DTP_FARM` (`3280`), con lai -> `DT_Skip` (`3285`). `DTM_OPT_CANCEL2` -> tra loi + `DTI_CANCELWAIT` (`3293/3301`) hoac chon dap an cuoi + hold 15' (`3305-3306`). `DTM_OPT_CANCEL1A/1B` -> `DTP_GOTONPC` (`3315`). `DTM_OPT_GODATAU` -> `DTP_XAFUTALK_DONE` (`3323`, delay 1500). `DTM_OPT_CONFIRM` -> `DTP_GOTONPC` (`3331`). `DTM_OPT_TASKPROCESS` -> `DTP_GOTONPC` (`3341`). `DTM_OPT_ACCEPT` -> `DT_ParseQuest` roi: loai bi TAT -> huy hoac hold (`3359-3370`); `nDTStep==DTI_TURNIN` -> tra loi + `DTI_TURNWAIT` (`3376-3378`, doi hoi >=5 o trong `3374`); `nDTStep==DTI_CANCEL` -> `DTI_CANCELWAIT` (`3388`); mac dinh -> `DTP_EXEC` (`3395`). Khong khop marker nao -> `nDTUnknown+1`, `DTP_GOTONPC` (`3405`) |
| `DTP_EXEC` (3) | `3410-3542` | Quyet dinh viec theo `nDTQType` da parse | T1/2/3: co item trong tui -> `nDTStep=DTI_TURNIN` + `DTP_GOTONPC` (`3458-3459`); item o ruong -> `DT_BoxToBag` roi lap lai (`3452`); T1 khong co do va detail 5/6/8 -> `DTP_GOSHOP` (`3470`); nguoc lai `DT_Skip`. T4: `bFight` tat -> hold 10' (`3489`); dang o map dich -> `DTP_FARM` + return 2 (`3501-3503`); khong -> `DTP_GOXAFU` (`3505`). T5 stat=2 (exp) -> `DTP_FARM`, return 0 (`3518-3520`); stat=4 (phuc duyen) -> `DTP_USEPD` (`3526`); stat 3/5/6 -> `DTI_TURNIN` + `DTP_GOTONPC` (`3530-3531`). T6 -> `DTI_TURNIN` + `DTP_GOTONPC` (`3536-3537`). Loai la -> hold 10' (`3541`) |
| `DTP_GOSHOP` (4) | `3544-3568` | T1: di den toa do tiem tap hoa (`g_ShopStation[nMap][0]`), rut tien neu <100.000 va `nDTWDMoney>0` | Map khong co toa do -> `DT_Skip` (`3549`). `DT_WalkTo(...,250)` tra 1 -> `DTP_SHOPTALK` (`3564`, `nDTRetry=0`) |
| `DTP_SHOPTALK` (5) | `3570-3624` | T1: mo thoai chu tiem (`DT_FindNpcName("t\271p h", …)` `3601/3603`) va chon muc giao dich lan luot theo `nDTShopTry` | Cua so shop mo (`CoreDataChanged(GDCNI_UI_ACT,2,0)`) -> `DTP_BUY` (`3576`). Het dap an (`nPick>=nAns`) -> `DT_Skip` (`3590`). >10 lan mo that bai / khong thay chu tiem -> `DT_Skip` (`3615`, `3622`) |
| `DTP_BUY` (6) | `3626-3689` | T1: quet `BuySell` tim mon khop `DTBuyRow` roi `SendClientCmdBuy` (`3677`) | Shop dong -> quay `DTP_SHOPTALK` (`3631`). Da co item -> dong shop + `DTI_TURNIN` + `DTP_GOTONPC` (`3641-3644`). Thieu tien -> rut ruong hoac `DT_Skip` (`3660-3668`). Tui day -> hold 5' (`3675`). Shop khong ban mon can: `nDTShopTry>=4` -> `DT_Skip` (`3685`), khong -> `DTP_SHOPTALK` (`3686`) |
| `DTP_GOXAFU` (7) | `3691-3721` | T4: di den xa phu (`g_MoveStation[nMap][0]`, ten NPC chua `"xa phu"` `3698`) va mo thoai | Khong co toa do -> `DT_Skip` (`3696`). Gan <=128 -> `DialogNpc` -> `DTP_WAITDLG` (`3708`, menu se khop `DTM_OPT_GODATAU`). Toi noi ma khong thay xa phu 15 lan -> `DT_Skip` (`3718`) |
| `DTP_XAFUTALK_DONE` (8) | `3723-3746` | T4: da chon "den noi lam nhiem vu Da Tau", cho server chuyen map | `nMap == nDTMapId` -> `DTP_FARM`, `nDTEngaged=2`, reset `nDTRetry/nDTXaFuTry`, ARM `uDTFarmStall=uCurTime` (`3729-3734`), return 2. Qua 12 tick khong chuyen -> `nDTXaFuTry++`; >5 -> `DT_Skip` (`3740`), nguoc lai quay `DTP_GOXAFU` (`3741`) |
| `DTP_FARM` (9) | `3748-3816` (T4) va `3074-3095` (T5-exp, xu ly TRUOC switch) | T4: danh quai + nhat cuon quanh anchor, doc tien do tu `cap.szMsg`. T5-exp: tha may cho auto thuong cay | T4: roi khoi map -> `DTP_EXEC` (`3754`). Stall >20' -> `DT_Skip` (`3774`). `nDTProg >= nDTReqNum` -> `DTI_TURNIN` + `DTP_RETURN` (`3778-3779`), return 1. Con lai return 2 (`3815`). T5-exp: stall >20' -> `DT_Hold` 15' (`3084`); `g_dDTExpGain >= nDTReqNum` -> `DTI_TURNIN` + `DTP_RETURN` (`3088-3089`) return 1; con lai return 0 (`3094`) |
| `DTP_RETURN` (10) | `3125-3146` | Ve thanh Da Tau bang Tho Dia Phu | Da o thanh -> `DTP_GOTONPC` (`3132`, `nDTRetry=0`). Khong o thanh + khong `m_FightMode` -> hold 10' (`3139`). `nDTRetry>8` -> hold 10' (`3141`). Con lai: `AutoUseItem(item_townportal)`, that bai thi `SendClientCmdOpenShop` mua tu xa (`3142-3143`), delay 3000ms |
| `DTP_GIVEBOX` (11) | `3818-3861` | T1/2/3: dat item vao o `pos_affairitem` roi bam OK (`SendUiCmdScript(1, cap.szBoxFunc)`) | Hop da dong (`!cap.nBoxOpen`) -> `DTP_WAITDLG` (`3824`). Khong tim ra item -> bam OK rong -> `DTP_WAITDLG` (`3837-3838`). Chua dat item (`nDTStep != DTI_TURNWAIT+100`) -> `MoveItem` + dat co (`3850-3851`), o lai pha. Da dat -> bam OK, `nDTStep=DTI_TURNWAIT`, `nDTItemIdx=0`, `DTP_WAITDLG` (`3855-3858`) |
| `DTP_REWARD` (12) | `3863-3877` | Bam ruong thuong: `nFinType<=4` -> `SendUiCmdScript(3, DT_FIN3[nDTReward1])`, nguoc lai `SendUiCmdScript(4, DT_FIN4[nDTReward2])` (`3866-3869`) | Luon -> `DTP_GOTONPC` (`3873`), reset `nDTStep=DTI_NONE`, `nDTQType=0`, `nDTItemIdx=0`, `nDTRetry=0`, delay 1500ms |
| `DTP_USEPD` (13) | `3879-3933` | T5 phuc duyen: dung Phuc Duyen Lo (`AutoUseItem(6,1,121+i)` `3892`) roi thu tra | Dung duoc -> `DTI_TURNIN` + `DTP_GOTONPC` (`3899-3900`). `nDTUsedPD>40` -> `DT_Skip` (`3883`). Dung >=2 lan ma `nFuYuan` khong tang -> `DT_Skip` (`3887`). Khong co item: keo tu ruong (`3909-3917`); `nDTUsedPD==0` -> thu tra 1 lan (`3927-3929`); nguoc lai `DT_Skip` (`3932`) |
| `DTP_HOLD` (14) | `3051-3065` (xu ly dau ham) | Treo (ket / loai bi tat / du 40 / loi) | `uDTHoldUntil` het han -> `DTP_IDLE` (`3055`). Con han -> `nDTEngaged = (nDTHoldFreeze && !bDTTrainAfter) ? 1 : 0` roi return luon (`3062-3063`). Sang ngay moi (`nDTDoneDay != nToday`) cung mo lai -> `DTP_IDLE` (`3041-3048`) |
| `default` | `3935-3937` | Gia tri la -> `DTP_IDLE`, return `nDTEngaged` |

---

## 2. enum DTINTENT (`ea.nDTStep`) — khai bao `CoreShell.cpp:2536-2543`

| Ten | Gia tri | Y nghia | Noi dat / noi doc |
|---|---|---|---|
| `DTI_NONE` | 0 | Gap NPC binh thuong (nhan/xem nhiem vu) | dat `3119`, `3269`, `3313`, `3339`, `3870` |
| `DTI_TURNIN` | 1 | Den de TRA nhiem vu -> chon option accept (`DTM_OPT_ACCEPT`) | dat `3088`, `3458`, `3530`, `3536`, `3643`, `3778`, `3899`, `3927`; doc `3274`, `3372` |
| `DTI_CANCEL` | 2 | Den de HUY nhiem vu | dat trong `DT_Skip` `3022`; doc `3382` |
| `DTI_CANCELWAIT` | 3 | Da chon huy, dang cho hop xac nhan | dat `3293`, `3301`, `3365`, `3388` |
| `DTI_TURNWAIT` | 4 | Da chon tra, cho ket qua (give-box / thuong / fail) | dat `3377`, `3856` |
| **`DTI_TURNWAIT + 100`** | **104** | **GIA TRI DAC BIET — khong phai y dinh, ma la CO "da dat item vao o nop"** trong `DTP_GIVEBOX`. Chu thich goc: `// dung nDTStep lam co "da dat item"` | so sanh `3844`, dat `3851`, xoa ve `DTI_TURNWAIT` `3856` |

---

## 3. Bang HAM HELPER `DT_*`

| Ham | file:line | Chuc nang |
|---|---|---|
| `DT_Msg` | `CoreShell.cpp:2550` | Gui thong bao ra khung chat kenh `"[DaTau]"`, chan spam bang `uDTStatusTime` (giãn 1200ms) |
| `DT_Today` | `2564` | Tra so ngay dang `YYMMDD` tu `GetLocalTime` (dung cho `nDTDoneDay`) |
| `DT_Has` | `2571` | `strstr` don gian — chuoi co chua marker khong |
| `DT_HasName` | `2578` | Khop ten item/tiem/map CO RANH GIOI: ky tu ngay truoc ten phai la `'>'` (het the `<color=yellow>`) — tranh ten ngan trung vao ten dai |
| `DT_NumAfter` | `2594` | Lay so nguyen dau tien sau marker, bo qua the `<color=...>`; tra -1 neu khong co |
| `DT_Split` | `2608` | Tach `"cau hoi\|a1\|a2..."` TAI CHO, tra so dap an, `szBuf` con lai la cau hoi |
| `DT_FindAns` | `2621` | Tim index dap an dau tien chua marker; -1 neu khong co |
| `DT_Hold` | `2629` | Treo may: in ly do, `nDTPhase=DTP_HOLD`, dat `uDTHoldUntil`, `nDTHoldFreeze=0`, `nDTEngaged=0`, tra 0 |
| `DT_FindNpcTpl` | `2642` | Tim NPC `kind_dialoger` theo template; uu tien NPC ten dung `"Da Tau"` TCVN3 (`2659`) |
| `DT_FindNpcName` | `2668` | Tim NPC `kind_dialoger` co ten (thuong hoa ASCII) chua chuoi con, quanh (nAtX,nAtY) trong nRadius |
| `DT_WalkTo` | `2691` | Di bo trong map (`SubWorld[0].FindPath`, giãn 2500ms bang `uDTPath`); tra 1 khi da toi trong nNear mps |
| `DT_Answer` | `2711` | Tra loi hoi thoai theo index 0-based: `CoreDataChanged(GDCNI_UI_ACT,1,0)` roi `OnSelectFromUI(UI_SELECTDIALOG)` |
| `DT_MatchRule` | `2723` | Kiem item khop luat: `magic>0` -> G/D/P + thuoc tinh magic trong `[mn,mx]`; `magic==0` -> khop du 5 truong (lvl/five `-1` = bo qua) |
| `DT_ItemProtected` | `2758` | Chan item bi khoa (`Lock/HLock/-2`) va — khi se BI TIEU HUY — do trang bi mau tren `green_item` |
| `DT_FindItemRule` | `2770` | Quet `m_ItemList` tim item khop luat trong tui (+ruong neu `bBox`); uu tien do co san trong tui (`2788`) |
| `DT_GetItemPos` | `2802` | Lay `ItemPos` (place/x/y) cua 1 item theo idx |
| `DT_BoxToBag` | `2821` | Keo item tu ruong ve tui bang goi tin thuan `c2s_dynamic_structure / c2sdnmbr_exchangeitem` — server chi can `m_CUnlocked`, KHONG can dung gan ruong |
| `DT_EnsureUnlock` | `2838` | Mo khoa ruong bang `szBoxPass` (tab Hau can), gui `SendClientCPUnlockCmd`, giãn 1200ms; tra true khi da mo |
| `DT_ParseQuest` | `2854` | Phan tich cau nhiem vu course 1 -> `nDTQType` 1..6 + `nDTCand[]` / `nDTReqNum` / `nDTMapId` / `nDTBook` / `nDTStatType` |
| `DT_FindCandItem` | `2984` | Tim item khop dung 1 ung vien hien tai (`nDTCand[nDTCandCur]`) cho T1/T2/T3 |
| `DT_Skip` | `3016` | Xu ly "ket": `nDTSkipMode==1` -> chuyen sang di HUY (`DTI_CANCEL` + `DTP_GOTONPC`, tra 1); nguoc lai `DT_Hold` 15 phut |
| `DT_Process` | `3032` | **May chinh** — switch tren `nDTPhase` |

Bang phu: `DT_FIN3[3] = {"finish_exp","finish_money","quest_random"}` (`2545`), `DT_FIN4[3] = {"finish_point","finish_lucky","finish_item"}` (`2546`).
Bien toan cuc: `g_dDTPrevExp` (`2547`), `g_dDTExpGain` (`2548`) — 1 client 1 nhan vat.

---

## 4. WATCHDOG / chong ket

| Bien | Noi tang | Noi reset ve 0 | Nguong | Hanh dong khi vuot |
|---|---|---|---|---|
| `nDTRetry` (dem chung, y nghia doi theo pha) | `3140` RETURN; `3186` GOTONPC; `3221` WAITDLG; `3445` EXEC-unlock; `3614`/`3621` SHOPTALK; `3717` GOXAFU; `3736` XAFUTALK | `3120`, `3133`, `3161`(gian tiep), `3208`, `3231`, `3460`, `3507`, `3565`, `3577`, `3645`, `3731`, `3742`, `3780`, `3874`, `3024` | RETURN **>8**; GOTONPC **>20**; WAITDLG **>60** (moi 16 vong ~4s thi go lai NPC, `3223`); unlock ruong **>8**; SHOPTALK **>10**; GOXAFU **>15**; XAFUTALK **>12** | RETURN: hold 10' "khong ve duoc thanh" (`3141`). GOTONPC: hold 5' (`3187`). WAITDLG: hold 5' "NPC khong tra loi" (`3222`). Unlock: `DT_Skip` (`3446`). SHOPTALK/GOXAFU: `DT_Skip` (`3615`/`3622`/`3718`). XAFUTALK: tang `nDTXaFuTry` roi ve `DTP_GOXAFU` (`3739-3742`) |
| `nDTUnknown` | `3401` (`= nUnkSave + 1`) | `3235` dat tam 0, nhung nhanh unknown KHOI PHUC `nUnkSave` roi tang — chu y `3232-3233`: **khong reset vo dieu kien** (tai lap bug DT-1: NPC lien tuc tra hoi thoai la thi chot khong bao gio no) | **>6** | hold 5' "hoi thoai khong nhan dang duoc" (`3403`) |
| `nDTXaFuTry` | `3739` (khi `nDTRetry>12` o XAFUTALK) | `3506` (vao `DTP_GOXAFU` tu EXEC), `3732` (khi da toi map) | **>5** | `DT_Skip` "xa phu khong chuyen map (kiem tra nhiem vu)" (`3740`) |
| `uDTFarmStall` | Dat moc thoi gian: `3079-3080` (T5-exp co exp tang), `3498` (EXEC T4), `3516` (EXEC T5-exp), `3733` (XAFUTALK -> FARM, chu thich: `ARM watchdog T4 (0 se TAT han - DT-3)`), `3769` (T4 co tien do cuon) | Khong reset ve 0 — chi ARM lai bang `uCurTime` | `uCurTime - uDTFarmStall > 20 phut` — kiem tai `3083` (T5-exp) va `3773` (T4) | T5-exp: `DT_Hold` 15' "cay exp qua lau khong tien" (`3084`). T4: `DT_Skip` "farm qua lau khong tien trien - bo qua" (`3774`) |
| `uDTHoldUntil` | Dat trong `DT_Hold` `2635` (`uMs ? uCurTime+uMs : 0`; 0 = treo vinh vien, dung cho "du 40/ngay") | `3047` (sang ngay moi), `3056` (het han) | So sanh `uCurTime > uDTHoldUntil` (`3053`) | Het han -> `DTP_IDLE` (`3055`) |
| `nDTHoldFreeze` | Dat 1 duy nhat tai `3248` (hold hop le "du 40 nhiem vu") | `DT_Hold` luon dat 0 (`2636`) — moi hold loi/treo deu NHA MAY | boolean | `3062`: `nDTEngaged = (nDTHoldFreeze && !bDTTrainAfter) ? 1 : 0` — chi hold "du 40" + nguoi choi TAT "len map luyen cong" thi dung yen; hold khac tra 0 cho auto thuong chay |
| `nDTUsedPD` (T5 phuc duyen) | `3897`, `3929` | `3524` (vao `DTP_USEPD` tu EXEC) | **>40**; va `>=2` ma `nFuYuan <= nDTFuYuanPrev` | `DT_Skip` "dung qua nhieu Phuc Duyen Lo" (`3883`) / "Phuc Duyen Lo khong tac dung" (`3887`) |
| `nDTShopTry` (T1) | `3592` moi lan thu 1 option tiem | `3471` (vao `DTP_GOSHOP`) | **>=4** o `DTP_BUY` (`3684`); hoac `nPick >= nAns` (`3589`) | `DT_Skip` "tiem khong ban mon can mua" / "tiem khong co muc giao dich phu hop" |
| `nDTDoneDay` | `3246` (gap `DTM_MSG_LIMIT`) | `3042` khi `DT_Today()` doi | so sanh ngay | Mo lai may (`DTP_HOLD -> DTP_IDLE`, `uDTHoldUntil=0`) |
| `uDTNext` | Nhip may chung: `3099` `uDTNext = uCurTime + 250` sau moi tick; nhieu cho set delay dai hon (700/800/900/1200/1500/3000ms) | — | `3097` `if (uDTNext > uCurTime) return nDTEngaged;` | Chan tick — giu nguyen `nDTEngaged` cu |

Cac chan an toan dau ham (khong phai watchdog nhung dung tuyen): `3067-3069` dang `do_death`/`do_revive` -> return 0; `3070-3071` dang giao dich (`CheckTrading`) -> return 0.

---

## 5. Gia tri tra ve cua `DT_Process` — anh huong den auto thuong

Chu thich goc `CoreShell.cpp:2512-2514`.

| Tra ve | Nghia | Anh huong (S3Client.cpp) |
|---|---|---|
| **0** | Tha may — auto Da Tau khong gianh quyen | `S3Client.cpp:993` cho `ATYPE_MOVE` chay (chi khi `nDT == 0`); `1069` cho `ATYPE_RETURN` chay (`pApData->bReturn && nDT == 0`); `ATYPE_FIGHT` chay voi `pApData` goc |
| **1** | Dang lam viec o thanh | Bo `ATYPE_MOVE` (`993`) VA bo `ATYPE_FIGHT` (`995`: `nDT != 1`) VA bo `ATYPE_RETURN` (`1069`) — may hoan toan do Da Tau dieu khien |
| **2** | Dang farm map nhiem vu (T4) | Bo `ATYPE_MOVE` va `ATYPE_RETURN`; VAN cho `ATYPE_FIGHT` nhung truyen ban sao `sDTData` (`S3Client.cpp:828-830`) voi `bSkipGoldboss = 0` — vi Mat Chi chi roi tu boss vang |

Trong `DT_Process`, gia tri tra ve luon dong bo voi `ea.nDTEngaged` (`3098` tra `nDTEngaged` khi chua toi nhip; `3937` default cung tra `nDTEngaged`).

---

## 6. Thong bao `[DaTau]` gui ra chat

Tat ca di qua `DT_Msg` -> `ChannelMessageArrival(0, "[DaTau]", ...)` (`2559`), giãn toi thieu 1200ms.

| file:line | Noi dung |
|---|---|
| `CoreShell.cpp:3084` | `[DaTau] cay exp qua lau khong tien - treo 15 phut` |
| `3087` | `[DaTau] du kinh nghiem, quay ve tra nhiem vu` |
| `3111` | `[DaTau] khong o thanh Da Tau - dung Tho Dia Phu ve thanh` |
| `3117` | `[DaTau] uu tien Da Tau: di den NPC nhan/tra nhiem vu` |
| `3139` | `[DaTau] ket: khong o thanh, khong dung duoc Tho Dia Phu` |
| `3141` | `[DaTau] ket: khong ve duoc thanh (het Tho Dia Phu?)` |
| `3187` | `[DaTau] khong thay NPC Da Tau o toa do` |
| `3216` | `[DaTau] bi NPC phat, cho ~10 phut` |
| `3222` | `[DaTau] NPC khong tra loi, treo 5 phut` |
| `3247` | `[DaTau] du 40 nhiem vu hom nay - nghi den mai` |
| `3254` | `[DaTau] tui day (<5 o trong) - don tui roi auto chay lai` |
| `3282` | `[DaTau] chua du exp, cay tiep...` |
| `3285` | `[DaTau] khong dap ung duoc nhiem vu nay` |
| `3306` | `[DaTau] het luot huy, treo nhiem vu` |
| `3357` | `[DaTau] khong hieu noi dung nhiem vu (?)` |
| `3369` | `[DaTau] loai nhiem vu nay dang TAT - treo` |
| `3375` | `[DaTau] can >=5 o trong de tra nhiem vu` |
| `3392` | `[DaTau] khong thay nut huy` |
| `3403` | `[DaTau] hoi thoai khong nhan dang duoc` |
| `3420` | `[DaTau] khong khop duoc dong bang du lieu` |
| `3442` | `[DaTau] do can nam trong ruong nhung chua cho phep lay` |
| `3446` | `[DaTau] khong mo khoa duoc ruong (kiem tra mat khau)` |
| `3451` | `[DaTau] tui khong du cho de lay do tu ruong` |
| `3476` | `[DaTau] can mua vu khi/ngua - hay bo san mon nay vao ruong (xem ten trong nhiem vu)` |
| `3480` | `[DaTau] khong co do can tim trong tui/ruong - nen tich luy trang suc` |
| `3482` | `[DaTau] khong co do 'khoe' phu hop trong tui/ruong` |
| `3487` | `[DaTau] khong doc duoc map/so luong loai 4` |
| `3489` | `[DaTau] loai 4 can bat 'Danh quai' o tab Chien dau` |
| `3517` | `[DaTau] nhiem vu exp: tha cho auto thuong cay, du se tu ve tra` |
| `3541` | `[DaTau] loai nhiem vu la` |
| `3549` | `[DaTau] thanh nay khong co toa do tiem tap hoa` |
| `3590` | `[DaTau] tiem khong co muc giao dich phu hop` |
| `3599` | `[DaTau] mat toa do tiem` |
| `3615` | `[DaTau] khong mo duoc tiem tap hoa` |
| `3622` | `[DaTau] khong thay chu tiem tap hoa` |
| `3668` | `[DaTau] khong du tien mua do nhiem vu` |
| `3675` | `[DaTau] tui day, khong mua duoc` |
| `3685` | `[DaTau] tiem khong ban mon can mua` |
| `3696` | `[DaTau] thanh nay khong co toa do xa phu` |
| `3718` | `[DaTau] khong thay xa phu` |
| `3740` | `[DaTau] xa phu khong chuyen map (kiem tra nhiem vu)` |
| `3774` | `[DaTau] farm qua lau khong tien trien - bo qua` |
| `3777` | `[DaTau] du so cuon, quay ve tra nhiem vu` |
| `3883` | `[DaTau] dung qua nhieu Phuc Duyen Lo ma chua du` |
| `3887` | `[DaTau] Phuc Duyen Lo khong tac dung (server chua co script?)` |
| `3932` | `[DaTau] het Phuc Duyen Lo ma chua du diem` |

---

## 7. Luong xu ly tung LOAI nhiem vu

Nhan dang loai o `DT_ParseQuest` (`2854-2981`), theo thu tu kiem marker: T1 (`DTM_T1_MUA` `2865`) -> T6 (`DTM_T6_MANH` `2886`) -> T4 (`DTM_T4_QUYEN` `2892`) -> T2/T3 (`DTM_T23_TIM` `2909`) -> T5 (`DTM_T5_NANGCAP` `2951`).

### Loai 1 — MUA do o tiem (`nDTQType=1`)
1. Parse: quet `g_DTBuy[]`, giu toi da 8 ung vien khop CA `szShop` va `szItem` bang `DT_HasName`, loc trung theo (genre/detail/particular/level/five) — `2868-2883`.
2. `DTP_EXEC` (`3415`): `DT_FindCandItem` tim san trong tui/ruong; co -> `DTI_TURNIN` + `DTP_GOTONPC` (`3458`); o ruong -> `DT_EnsureUnlock` + `DT_BoxToBag` (`3443-3453`).
3. Khong co: neu `r.nDetail` la 5/6/8 (tiem tap hoa) -> `DTP_GOSHOP` (`3470`); vu khi/ngua -> `DT_Skip` (`3475`).
4. `DTP_GOSHOP` (`3544`) di toa do + rut tien -> `DTP_SHOPTALK` (`3570`) mo thoai chu tiem, thu tung option -> `DTP_BUY` (`3626`) quet `BuySell` va `SendClientCmdBuy` (`3677`).
5. Mua xong -> dong shop, `DTI_TURNIN`, `DTP_GOTONPC` (`3641-3644`) -> `DTP_WAITDLG` -> `DTP_GIVEBOX` (`3818`) dat item vao `pos_affairitem` + OK -> `DTP_REWARD` (`3863`).

### Loai 2 — TIM do trong tui/ruong (`nDTQType=2`)
1. Parse (`2909-2949`): doc `nMin`/`nMax` tu `DTM_MIN_MARK`/`DTM_MAX_MARK`. Co min/max -> khop `g_DTFind[i].nMagic>0` + dung khoang + ten + marker magic (`2925-2928`); khong co -> khop `nMagic==0` theo ten, loc trung 5 truong (`2932-2947`).
2. `DTP_EXEC` (`3416`): `DT_FindCandItem` -> `DT_FindItemRule` voi `bWillLose=true` (do se BI TIEU HUY nen `DT_ItemProtected` chan do mau tren xanh la, `2763-2765`).
3. Tim thay o ruong -> `DT_BoxToBag`; trong tui -> `DTI_TURNIN` + `DTP_GOTONPC` (`3458-3459`).
4. Khong co -> `DT_Skip` "khong co do can tim trong tui/ruong" (`3479`).
5. Tra: `DTP_GIVEBOX` (`3818`) -> `DTP_REWARD`. Neu server bao fail-req, may thu ung vien ke (`3265-3273`).

### Loai 3 — XEM do "khoe" (`nDTQType=3`)
1. Parse (`2913-2921`): khop `g_DTShow[i]` theo dung `nMin`/`nMax` + marker `szMagic`.
2. `DTP_EXEC` (`3417`): `DT_FindCandItem` -> `DT_FindItemRule` voi `bWillLose=false` (chi cho xem, KHONG mat do — `3009`), khong loc genre/detail/particular (`-1,-1,-1`).
3. Co do -> `DTI_TURNIN` + `DTP_GOTONPC`; khong -> `DT_Skip` (`3481`).
4. `DTP_GIVEBOX` co nhanh rieng cho T3: neu chua chot item thi tu tim lai bang `DT_FindCandItem` (`3828-3843`).

### Loai 4 — DANH QUAI nhat cuon (Dia Do / Mat Chi) (`nDTQType=4`)
1. Parse (`2892-2907`): `nDTBook` = 1 neu `DTM_T4_DIADO`, 2 neu `DTM_T4_MATCHI`; `nDTReqNum` = so sau `DTM_T4_TIMGIUP`; do `g_DTQuestMap[]` de lay `nDTMapId` va anchor `nX*32, nY*32`.
2. `DTP_EXEC` case 4 (`3484-3509`): doi `bFight` bat (`3488`), mua san Tho Dia Phu (`3493-3497`), ARM `uDTFarmStall`; dang o map dich -> `DTP_FARM` return 2, khong -> `DTP_GOXAFU`.
3. `DTP_GOXAFU` (`3691`) -> `DialogNpc` xa phu -> `DTP_WAITDLG` khop `DTM_OPT_GODATAU` (`3320-3325`) -> `DTP_XAFUTALK_DONE` (`3723`) cho chuyen map.
4. `DTP_FARM` (`3748`): doc tien do tu `cap.szMsg` (marker `DTM_MSG_TAM_PRE` + `DTM_MSG_TONGCONG`, `3763-3771`); neo danh quai quanh anchor bang `nCurMoveRet=3` + `nTempX/nTempY` (`3791-3796`) khi cach <=1500, xa hon thi `DT_WalkTo`; nhat cuon roi tren dat genre 6 trong ban kinh 500 bang `CheckObject` (`3799-3813`) ke ca khi bo loc nhat cua nguoi choi bo qua.
5. Du `nDTProg >= nDTReqNum` -> `DTI_TURNIN` + `DTP_RETURN` (`3778-3779`) -> ve thanh -> `DTP_GOTONPC` -> `DTP_REWARD`.

### Loai 5 — NANG CAP chi so (`nDTQType=5`, phan nhanh theo `nDTStatType`)
Parse `2951-2980`: `nDTStatType` = 2 (exp), 3 (danh vong), 4 (phuc duyen), 5 (PK), 6 (Tong Kim); `nDTReqNum` doc sau marker tuong ung.
- **stat=2 (exp)**: `DTP_EXEC` (`3512-3521`) chup `g_dDTPrevExp`, `g_dDTExpGain=0`, vao `DTP_FARM` va **return 0 — tha may hoan toan cho auto thuong cay**. Khoi do dem nam TRUOC switch (`3074-3095`), cong don delta exp, du `nDTReqNum` -> `DTI_TURNIN` + `DTP_RETURN`. Neu server bao fail luc tra (mat baseline sau crash) -> reset dem, quay lai `DTP_FARM` (`3274-3284`).
- **stat=4 (phuc duyen)**: `DTP_USEPD` (`3879`) dung `AutoUseItem(6, 1, 121+i)` i=0..2, moi lan dung xong lai di tra (`3899-3900`); fail-req tu `DTP_WAITDLG` quay lai `DTP_USEPD` (`3258-3263`).
- **stat=3/5/6 (danh vong / PK / Tong Kim)**: khong lam gi them — `DTI_TURNIN` + `DTP_GOTONPC` thu tra 1 lan (nho da du), fail thi `DT_Skip` (`3529-3532`).

### Loai 6 — GOP MANH (`nDTQType=6`)
1. Parse (`2886-2891`): chi doc `nDTReqNum` = so sau `DTM_T4_TIMGIUP`; khong co ung vien item.
2. `DTP_EXEC` case 6 (`3534-3539`): dat thang `nDTStep = DTI_TURNIN`, `DTP_GOTONPC` — chu thich goc: `thu tra; thieu manh -> fail -> skip`.
3. `DTP_WAITDLG` chon option accept (`3376`) -> neu du manh, server mo cua so ruong -> `DTP_REWARD` (`3196-3201`); neu thieu, khop `DTM_MSG_FAILREQ` -> `DT_Skip` (`3285`).

---

## PHẦN B — BẢN ĐỒ UI WAUTO

Trích 100% từ mã nguồn thật. Đường dẫn tuyệt đối:

| Vai trò | File |
|---|---|
| ID control (ASCII) | `E:\Src_Auto_Ngoai\WAuto\WAuto\Resource.h` |
| Định nghĩa dialog (UTF-16 LE) | `E:\Src_Auto_Ngoai\WAuto\WAuto\WAuto.rc` |
| Logic UI (UTF-16 LE) | `E:\Src_Auto_Ngoai\WAuto\WAuto\WAuto.cpp` |
| Struct chia sẻ | `D:\GAMEDEVNEW\Sources\Core\Src\ipc_shared.h` |
| Engine tiêu thụ | `D:\GAMEDEVNEW\Sources\Core\Src\CoreShell.cpp` (`DT_Process`) |

> Số dòng của `WAuto.rc` / `WAuto.cpp` là số dòng **CHÍNH XÁC** (không phải ước lượng): bản UTF-8 quy đổi bằng `io.open(path, encoding='utf-16').read()` giữ nguyên từng dòng, không thêm/bớt dòng nào.

---

## 1. Bảng 19 control của tab 8 "Dã Tẩu"

Dải ID **393 → 411** (liền mạch), tất cả nằm dưới `IDC_INDEX_END = 412` (`Resource.h:279`). Khai báo tại `Resource.h:259-278`, layout tại `WAuto.rc:333-351`. Toạ độ .rc theo thứ tự `x, y, w, h` (dialog units).

| # | Tên ID | Số | Loại control | Nhãn tiếng Việt | Toạ độ (.rc) | Dòng .rc | Field autoData |
|---|---|---|---|---|---|---|---|
| 1 | `IDC_CHECKBOX_8_ON` | 393 | `BS_AUTOCHECKBOX` | Bật auto Dã Tẩu | 4, 112, 152, 10 | 333 | `bDaTau` |
| 2 | `IDC_CHECKBOX_8_T1` | 394 | `BS_AUTOCHECKBOX` | Mua vật phẩm | 4, 124, 50, 10 | 334 | `bDTType[0]` |
| 3 | `IDC_CHECKBOX_8_T2` | 395 | `BS_AUTOCHECKBOX` | Tìm vật phẩm | 56, 124, 50, 10 | 335 | `bDTType[1]` |
| 4 | `IDC_CHECKBOX_8_T3` | 396 | `BS_AUTOCHECKBOX` | Khoe vật phẩm | 108, 124, 48, 10 | 336 | `bDTType[2]` |
| 5 | `IDC_CHECKBOX_8_T4` | 397 | `BS_AUTOCHECKBOX` | Địa đồ chỉ | 4, 135, 50, 10 | 337 | `bDTType[3]` |
| 6 | `IDC_CHECKBOX_8_T5` | 398 | `BS_AUTOCHECKBOX` | Nâng chỉ số | 56, 135, 50, 10 | 338 | `bDTType[4]` |
| 7 | `IDC_CHECKBOX_8_T6` | 399 | `BS_AUTOCHECKBOX` | Sơn Hà Xã Tắc | 108, 135, 48, 10 | 339 | `bDTType[5]` |
| 8 | `IDC_STRING_8_SKIP` | 400 | `LTEXT` (SS_CENTERIMAGE) | Khi bỏ qua: | 4, 149, 50, 10 | 340 | — (nhãn) |
| 9 | `IDC_COMBO_8_SKIP` | 401 | `COMBOBOX` CBS_DROPDOWNLIST | (2 mục) | 56, 148, 100, 40 | 341 | `nDTSkipMode` |
| 10 | `IDC_STRING_8_CANCEL` | 402 | `LTEXT` | Cách hủy: | 4, 162, 50, 10 | 342 | — (nhãn) |
| 11 | `IDC_COMBO_8_CANCEL` | 403 | `COMBOBOX` CBS_DROPDOWNLIST | (3 mục) | 56, 161, 100, 60 | 343 | `nDTCancelMode` |
| 12 | `IDC_STRING_8_RW1` | 404 | `LTEXT` | Thưởng (Exp/Bạc): | 4, 175, 66, 10 | 344 | — (nhãn) |
| 13 | `IDC_COMBO_8_RW1` | 405 | `COMBOBOX` CBS_DROPDOWNLIST | (3 mục) | 72, 174, 84, 60 | 345 | `nDTReward1` |
| 14 | `IDC_STRING_8_RW2` | 406 | `LTEXT` | Thưởng (Điểm/Vật): | 4, 188, 66, 10 | 346 | — (nhãn) |
| 15 | `IDC_COMBO_8_RW2` | 407 | `COMBOBOX` CBS_DROPDOWNLIST | (3 mục) | 72, 187, 84, 60 | 347 | `nDTReward2` |
| 16 | `IDC_CHECKBOX_8_BOX` | 408 | `BS_AUTOCHECKBOX` | Lấy đồ/tiền từ rương (dùng mật khẩu tab Hậu cần) | 4, 201, 152, 10 | 348 | `bDTUseBox` (mật khẩu lấy từ `szBoxPass`, `ipc_shared.h:205`) |
| 17 | `IDC_STRING_8_WD` | 409 | `LTEXT` | Rút tiền (vạn): | 4, 213, 66, 10 | 349 | — (nhãn) |
| 18 | `IDC_EDITOR_8_WD` | 410 | `EDITTEXT` `ES_NUMBER` | (ô số) | 72, 213, 30, 10 | 350 | `nDTWDMoney` |
| 19 | `IDC_CHECKBOX_8_TRAIN` | 411 | `BS_AUTOCHECKBOX` | Xong nhiệm vụ tự lên map luyện công | 4, 225, 152, 10 | 351 | `bDTTrainAfter` |

**Nút tab**: `IDC_TABBTN_9` = 422 (`Resource.h:289`), nhãn **"Dã Tẩu"**, `BS_AUTORADIOBUTTON | BS_PUSHLIKE`, toạ độ **4, 96, 152, 12** — `WAuto.rc:53-56`. Lưu ý ID 422 nằm **NGOÀI** dải ẩn/hiện (>`IDC_INDEX_END`) nên nút không bị ShowTab ẩn.

---

## 2. Bảng 4 combo (thứ tự index chuẩn)

Nạp mục trong `WM_INITDIALOG` của dialog chính: `WAuto.cpp:3431-3448`.

| Combo | Index | Nhãn hiển thị | Field autoData | Ý nghĩa với engine (CoreShell.cpp) |
|---|---|---|---|---|
| `IDC_COMBO_8_SKIP` (3431) | 0 (mặc định, `CB_SETCURSEL 0` dòng 3434) | Treo (tạm dừng) | `nDTSkipMode` | Gặp nhiệm vụ không làm được → ngừng Dã Tẩu, treo máy |
| | 1 | Hủy nhiệm vụ | | `CoreShell.cpp:3019` và `:3362` — bật nhánh chủ động huỷ nhiệm vụ (`nCanIdx >= 0`) |
| `IDC_COMBO_8_CANCEL` (3435) | 0 (mặc định) | Chỉ dùng lượt hủy | `nDTCancelMode` | Chỉ tiêu thụ lượt huỷ miễn phí, không phá chuỗi |
| | 1 | Cho phép hủy thường (reset chuỗi) | | `CoreShell.cpp:3297` — chấp nhận huỷ thường, chuỗi nhiệm vụ bị reset |
| | 2 | Ưu tiên 100 mảnh SHXT | | `CoreShell.cpp:3290` — ưu tiên giữ/đi tới mốc 100 mảnh Sơn Hà Xã Tắc |
| `IDC_COMBO_8_RW1` (3440) | 0 (mặc định, dòng 3444) | Kinh nghiệm | `nDTReward1` | `DT_FIN3[0] = "finish_exp"` (`CoreShell.cpp:2545`, dùng ở `:3867` qua `SendUiCmdScript(3, ...)`) |
| | 1 | Bạc | | `DT_FIN3[1] = "finish_money"` |
| | 2 | Ngẫu nhiên | | `DT_FIN3[2] = "quest_random"` |
| `IDC_COMBO_8_RW2` (3445) | 0 | Điểm | `nDTReward2` | `DT_FIN4[0] = "finish_point"` (`CoreShell.cpp:2546`, dùng ở `:3869` qua `SendUiCmdScript(4, ...)`) |
| | 1 | May mắn (lượt hủy) | | `DT_FIN4[1] = "finish_lucky"` — cộng lượt huỷ |
| | 2 (**mặc định**, `CB_SETCURSEL 2` dòng 3449) | Vật phẩm | | `DT_FIN4[2] = "finish_item"` |

Ngoài ra 6 checkbox loại nhiệm vụ ánh xạ thẳng: `CoreShell.cpp:3359` — `if (!pAp->bDTType[ea.nDTQType - 1])` ⇒ **`nDTQType` 1..6 tương ứng `bDTType[0..5]`** theo đúng thứ tự nhãn ở bảng 1.

---

## 3. Các điểm nối dây (wiring) trong `WAuto.cpp`

| Nơi | Dòng | Nội dung |
|---|---|---|
| `SaveRoleData` — khối "tab 8 Da Tau" | **969-988** | Đọc 9 checkbox bằng `IsDlgButtonChecked`, 4 combo bằng `CB_GETCURSEL` (kèm kẹp `< 0 → 0` ở 980/982/984/986), ô số bằng `GetDlgItemTextA` + `atoi` (987-988) |
| `SaveRoleDataFast` | 1007 (hàm) | **KHÔNG** động tới field Dã Tẩu — chỉ ghi thẳng `apdata` xuống `%PID%.dat`; mọi thay đổi tab 8 đi qua `SaveRoleData` |
| `UpdateUI` — đổ dữ liệu lên UI | **1918-1931** | 9 × `CheckDlgButton`, 4 × `ComboBox_SetCurSel`, `_itow_s(nDTWDMoney)` → `IDC_EDITOR_8_WD` (1931) |
| `LoadRoleData` — giá trị mặc định khi chưa có `.dat` | **2009-2018** | `bDTType[0..5]=1`, `bDTUseBox=1`, `nDTReward1=0`, `nDTReward2=2`, `nDTWDMoney=50` (phần còn lại = 0 do `memset` ở 1960) |
| `LoadRoleData` — nạp file cũ | 1997-2001 | `if (uSize > sizeof(autoData)) uSize = sizeof(autoData); fread(...)` — **cắt bớt, không lỗi** ⇒ file `.dat` cũ vẫn nạp được |
| `ShowTab` — vòng ẩn toàn bộ | **2782-2785** | `for(i=IDC_STRING_0_L; i<IDC_INDEX_END; ++i) ShowWindow(..., SW_HIDE)` |
| `ShowTab` — nhánh tab 8 | **2878-2884** | `else if(nTabBtn == 8) { for(i=IDC_CHECKBOX_8_ON; i<=IDC_CHECKBOX_8_TRAIN; ++i) ShowWindow(..., SW_SHOW); }` |
| `WM_INITDIALOG` dialog chính | 3212 (bắt đầu); nạp combo tab 8 tại **3431-3449** | Thêm chuỗi cho 4 combo + `CB_SETCURSEL` mặc định |
| Handler nút tab | **3804-3809** | `case IDC_TABBTN_9:` → `int tabbtn = (LOWORD(wParam) == IDC_TABBTN_9) ? 8 : (LOWORD(wParam) - IDC_TABBTN_1); ShowTab(hDlg, tabbtn);` — **TABBTN_9 phải ánh xạ tay sang 8** vì ID 422 không liền dải TABBTN_1..8 |
| `EN_KILLFOCUS` cho ô số | `case IDC_EDITOR_8_WD:` ở **3832**, thân xử lý chung `if (HIWORD(wParam) == EN_KILLFOCUS)` ở **3850**, gọi `SaveRoleData` ở **3942** | Lưu khi ô rút tiền mất focus |
| `BN_CLICKED` cho 9 checkbox | `case` liệt kê **3973-3981**, thân chung `if (HIWORD(wParam) == BN_CLICKED)` ở **4023**, `SaveRoleData` ở **4028** | |
| `CBN_SELCHANGE` cho 4 combo | `case` liệt kê **4710-4713**, thân chung ở **4728**, `SaveRoleData` ở **4737** | |

---

## 4. Bảng 14 field Dã Tẩu trong `autoData`

`D:\GAMEDEVNEW\Sources\Core\Src\ipc_shared.h:239-248` (khối có ghi chú *"== Da Tau (18/08/2026) - PHAI o cuoi struct, truoc constructor =="*). Constructor khởi tạo tại **:403-412**.

| # | Field | Kiểu | Dòng | Ý nghĩa | Mặc định `LoadRoleData` (chưa có .dat) |
|---|---|---|---|---|---|
| 1 | `bDaTau` | `int` | 240 | Bật/tắt auto Dã Tẩu | **0** (memset) |
| 2 | `bDTType[0]` | `int` | 241 | Loại 1 — Mua vật phẩm | **1** (dòng 2009) |
| 3 | `bDTType[1]` | `int` | 241 | Loại 2 — Tìm vật phẩm | **1** (2010) |
| 4 | `bDTType[2]` | `int` | 241 | Loại 3 — Khoe vật phẩm | **1** (2011) |
| 5 | `bDTType[3]` | `int` | 241 | Loại 4 — Địa đồ chỉ | **1** (2012) |
| 6 | `bDTType[4]` | `int` | 241 | Loại 5 — Nâng chỉ số | **1** (2013) |
| 7 | `bDTType[5]` | `int` | 241 | Loại 6 — Sơn Hà Xã Tắc | **1** (2014) |
| 8 | `nDTSkipMode` | `int` | 242 | 0 = treo (ngừng DT); 1 = huỷ nhiệm vụ | **0** |
| 9 | `nDTCancelMode` | `int` | 243 | 0 = chỉ lượt huỷ; 1 = cho phép huỷ thường (reset chuỗi); 2 = ưu tiên 100 mảnh SHXT | **0** |
| 10 | `nDTReward1` | `int` | 244 | Cửa sổ Exp/Money/Random: 0/1/2 | **0** (2016) |
| 11 | `nDTReward2` | `int` | 245 | Cửa sổ Point/Lucky/Item: 0/1/2 (Lucky = tích lượt huỷ) | **2** (2017) |
| 12 | `bDTUseBox` | `int` | 246 | Cho phép lấy đồ/tiền từ rương (dùng `szBoxPass`) | **1** (2015) |
| 13 | `bDTTrainAfter` | `int` | 247 | Xong/treo → thả máy cho auto thường về map luyện công | **0** |
| 14 | `nDTWDMoney` | `int` | 248 | Vạn lượng rút từ rương khi thiếu tiền mua đồ | **50** (2018) |

Ghi chú engine: `nDTWDMoney` được nhân `* 10000` trước khi rút (`CoreShell.cpp:3557`, `:3663`); chỉ rút khi `GetEquipmentMoney() < 100000 && bDTUseBox && nDTWDMoney > 0` (`CoreShell.cpp:3551-3552`). `bDTTrainAfter` chi phối cờ `ea.nDTEngaged` (`CoreShell.cpp:3062`, `:3249`).

---

## 5. Ghi chú BẮT BUỘC cho người sửa sau

1. **ID mới phải < `IDC_INDEX_END` và liền mạch.** `ShowTab` ẩn theo dải `IDC_STRING_0_L (161) … < IDC_INDEX_END (412)` (`WAuto.cpp:2782`), còn tab 8 hiện theo dải `IDC_CHECKBOX_8_ON (393) … IDC_CHECKBOX_8_TRAIN (411)` (`WAuto.cpp:2880`). Thêm control mới ⇒ **chèn vào giữa dải 393-411 và dời `IDC_INDEX_END` lên**, hoặc thêm ngay trước 411 rồi sửa cận trên của vòng lặp. Nếu đặt ID > `IDC_INDEX_END` thì control **không bao giờ bị ẩn** khi chuyển tab (đúng như `IDC_TABBTN_9 = 422`, cố ý).
2. **`IDC_TABBTN_9` không liền dải TABBTN.** Mọi chỗ tính chỉ số tab phải giữ nhánh đặc biệt `(LOWORD(wParam) == IDC_TABBTN_9) ? 8 : (LOWORD(wParam) - IDC_TABBTN_1)` (`WAuto.cpp:3806`).
3. **`WAuto.rc` và `WAuto.cpp` là UTF-16 LE.** Tuyệt đối **không** sửa bằng Edit/Write thông thường (phá encoding, mất dấu tiếng Việt). Quy trình an toàn: đọc/ghi bằng python `io.open(path, encoding='utf-16')`, ghi lại đúng `encoding='utf-16'`. `Resource.h` là ASCII nên sửa bình thường.
4. **File `.dat` cũ vẫn nạp được** vì (a) field Dã Tẩu nằm **ở CUỐI** `autoData` ngay trước constructor (`ipc_shared.h:239-248`) và (b) `LoadRoleData` đọc `fread` theo kích thước file rồi kẹp `uSize > sizeof(autoData)` (`WAuto.cpp:1997-2001`) sau khi đã `memset` 0 (`:1960`). ⇒ **Field mới BẮT BUỘC thêm ở cuối struct**; chèn giữa struct sẽ làm lệch toàn bộ dữ liệu người dùng cũ. Field mới mặc định = 0 với file cũ, nên phải chọn ngữ nghĩa sao cho 0 = "tắt/an toàn".
5. **Bốn nơi phải sửa đồng bộ khi thêm control**: `SaveRoleData` (đọc UI → apdata, ~969-988), `UpdateUI` (apdata → UI, ~1918-1931), `WM_INITDIALOG` (nạp mục combo, ~3431-3449), và danh sách `case` của `BN_CLICKED`/`CBN_SELCHANGE`/`EN_KILLFOCUS` (3973-3981 / 4710-4713 / 3832). Thiếu case ⇒ thay đổi **không được lưu**.
6. **`SaveRoleDataFast` không ghi field tab 8** — nó chỉ dump `apdata` hiện có; nếu thêm control mới, luồng lưu vẫn phải đi qua `SaveRoleData`.
7. Struct `autoData` là **bộ nhớ chia sẻ** giữa WAuto.exe và Game.exe. Đổi kích thước/thứ tự ⇒ **phải build lại cả WAuto.exe lẫn CoreClient.dll/Game.exe**, nếu không hai bên đọc lệch offset. Lưu ý còn một bản `ipc_shared.h` thứ hai tại `E:\Src_Auto_Ngoai\WAuto\WAuto\ipc_shared.h` — hai bản **phải giống hệt nhau**.

---

## PHẦN C — HOOK, DỮ LIỆU & TÍCH HỢP

## 1. Bảng 6 HOOK chụp gói tin

| # | file:line | Hàm chứa | Gói tin / sự kiện | Ghi vào `g_sDTCap` |
|---|---|---|---|---|
| 1 | `D:\GAMEDEVNEW\Sources\Core\Src\KPlayer.cpp:7518-7519` | `KPlayer::OnScriptAction` → `case SCRIPTACTION_UISHOW` / `case UI_SELECTDIALOG` (nhánh `m_bParam1 == 0`) | `PLAYER_SCRIPTACTION_SYNC` (hộp thoại hỏi–đáp) | `szDlg` = nguyên văn `strContent` (chụp TRƯỚC `strstr("|")` + `TEncodeText`), rồi `++uDlgSeq` |
| 2 | `KPlayer.cpp:7644-7645` | cùng hàm, `case UI_TALKDIALOG` | `PLAYER_SCRIPTACTION_SYNC` (NPC nói) | `szTalk` = `pAnswer` (nguyên văn "câu 1\|câu 2\|..."), `++uTalkSeq` |
| 3 | `KProtocolProcess.cpp:3905-3906` | `KProtocolProcess::s2cOpenQuestFinishDlg` | `s2c_openquestfinishdlg` / `QUEST_FINISH_DLG_SYNC` (cửa sổ 3 rương) | `nFinType = pFinish->m_bType`, `++uFinSeq` |
| 4 | `KProtocolProcess.cpp:4057-4059` | `KProtocolProcess::OpenAffairBox`, `case 1` | `S2C_GIVE_BOX` nType==1 (mở hộp nộp đồ) | `szBoxFunc = GiveBoxCmd->Value2` (tên hàm nộp), `nBoxOpen = 1`, `++uBoxSeq` |
| 5 | `KProtocolProcess.cpp:4064` | `KProtocolProcess::OpenAffairBox`, `case 2` | `S2C_GIVE_BOX` nType==2 (đóng hộp) | `nBoxOpen = 0` (KHÔNG tăng seq) |
| 6 | `KProtocolProcess.cpp:4141-4151` | `KProtocolProcess::s2cExtendChat`, nhánh `protocol == chat_channelchat` | `CHAT_CHANNELCHAT_SYNC`, lọc `pCccSync->someone == "Hệ Thống"` (`"H\326 Th\350ng"`) | `szMsg` = payload (`memcpy`, cắt còn `sizeof(szMsg)-1`), `++uMsgSeq` |

Khai báo/định nghĩa biến: `KPlayer.cpp:41` `KDaTauCapture g_sDTCap;` — include `KDaTauCap.h` ở `KPlayer.cpp:39`, `KProtocolProcess.cpp:36`, `CoreShell.cpp:39`.

## 2. Cấu trúc `KDaTauCap.h` (`D:\GAMEDEVNEW\Sources\Core\Src\KDaTauCap.h`, 26 dòng)

| Trường (dòng) | Ý nghĩa | Ai GHI | Ai ĐỌC |
|---|---|---|---|
| `unsigned int uDlgSeq` (11) | tăng mỗi `UI_SELECTDIALOG` | KPlayer.cpp:7519 | CoreShell.cpp:3172, 3218, 3230, 3581, 3583, 3611, 3706 (so với `ea.uDTDlgSeen`) |
| `char szDlg[2048]` (12) | "câu hỏi\|đáp án 1\|đáp án 2\|..." TCVN3, CHƯA mã hóa/tách | KPlayer.cpp:7518 | CoreShell.cpp:3239, 3586 (`g_StrCpyLen` ra buffer rồi tách) |
| `unsigned int uTalkSeq` (13) | tăng mỗi `UI_TALKDIALOG` | KPlayer.cpp:7645 | CoreShell.cpp:3173, 3212-3214 |
| `char szTalk[1024]` (14) | "câu 1\|câu 2\|..." | KPlayer.cpp:7644 | CoreShell.cpp:3215 (`DT_Has(cap.szTalk, DTM_MSG_PUNISH)` — bắt bị phạt 10 phút) |
| `unsigned int uFinSeq` (15) | tăng mỗi `s2c_openquestfinishdlg` | KProtocolProcess.cpp:3906 | CoreShell.cpp:3174, 3196-3198 |
| `int nFinType` (16) | `m_bType`: <=4 = `KUiDaTau` (exp/money/random), >4 = `KUiDaTau1` | KProtocolProcess.cpp:3905 | CoreShell.cpp:3866 (`if (cap.nFinType <= 4)`) |
| `unsigned int uBoxSeq` (17) | tăng mỗi khi give-box MỞ (`S2C_GIVE_BOX` nType==1) | KProtocolProcess.cpp:4059 | CoreShell.cpp:3175, 3204-3206 |
| `int nBoxOpen` (18) | 1 = đang mở, 0 = đã đóng | KProtocolProcess.cpp:4058 / :4064 | CoreShell.cpp:3821 (`if (!cap.nBoxOpen)`) |
| `char szBoxFunc[64]` (19) | tên hàm nộp = `S2C_GIVE_BOX.Value2` | KProtocolProcess.cpp:4057 | CoreShell.cpp:3837, 3855 (`SendUiCmdScript(1, cap.szBoxFunc)`) |
| `unsigned int uMsgSeq` (20) | tăng mỗi thông điệp "Hệ Thống" | KProtocolProcess.cpp:4150 | CoreShell.cpp:3491, 3760-3762 (vòng `while` để không sót) |
| `char szMsg[512]` (21) | nội dung thông điệp (TCVN3) — tiến độ nhặt cuộn | KProtocolProcess.cpp:4148-4149 | CoreShell.cpp:3763-3765 (`DTM_MSG_TAM_PRE` + `DTM_MSG_TONGCONG` → `DT_NumAfter`) |

Ghi chú header (dòng 5): biến định nghĩa trong KPlayer.cpp nên compile cả client lẫn server, vô hại. Máy trạng thái đọc bằng cách SO SÁNH seq với bản sao `ea.uDT*Seen` (CoreShell.cpp:3172-3175 khởi tạo "đã thấy" lúc bắt đầu để bỏ qua dữ liệu cũ).

## 3. Tích hợp trong `D:\GAMEDEVNEW\Sources\S3Client\S3Client.cpp` (hàm ExtAutoLoop)

Khối gate — dòng 818-832:

```
if(!g_pCoreShell->GetGameData(GDI_GET_PLAYERNPC_INDEX, 0, 0)) return;      // 818
int nDT = 0;  autoData sDTData;                                            // 822-823
if(pApData->bDaTau == 1)                                                   // 824
{
    nDT = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_DATAU, (int)pApData);  // 826
    if(nDT == 2) { memcpy(&sDTData, pApData, sizeof(autoData)); sDTData.bSkipGoldboss = 0; }// 827-831
}
```

Ý nghĩa `nDT` (chú thích dòng 820-821):
- `nDT == 0`: máy Dã Tẩu "thả máy" — auto thường chạy bình thường.
- `nDT == 1`: đang làm việc ở thành (mua đồ / nói chuyện NPC / nộp nhiệm vụ) — BỎ `ATYPE_MOVE` và `ATYPE_RETURN`, và bỏ luôn `ATYPE_FIGHT`.
- `nDT == 2`: đang farm map nhiệm vụ — BỎ `ATYPE_MOVE`/`ATYPE_RETURN` nhưng VẪN đánh, đánh với bản sao dữ liệu đã sửa.

Ảnh hưởng từng ATYPE:

| Vị trí | Luật |
|---|---|
| `S3Client.cpp:992-994` | `BOOL bMoving = FALSE; if(nDT == 0) bMoving = ...ATYPE_MOVE...` → `nDT != 0` thì KHÔNG gọi `ATYPE_MOVE`, để máy Dã Tẩu độc quyền điều khiển di chuyển (tránh hai bên giành đường đi) |
| `S3Client.cpp:995-997` | `if(!bMoving && pApData->bFight && nDT != 1)` → `nDT==1` (đang ở thành) tắt hẳn đánh nhau; `nDT==0` hoặc `2` mới `ATYPE_FIGHT` |
| `S3Client.cpp:997` | tham số `(int)(nDT == 2 ? &sDTData : pApData)` → khi farm map nhiệm vụ dùng BẢN SAO |
| `S3Client.cpp:1069` | `if(pApData->bReturn && nDT == 0 && ...)` → chỉ về thành (`ATYPE_RETURN`) khi Dã Tẩu đang thả máy |

- `sDTData`: bản sao `autoData` (memcpy toàn struct, dòng 829) chỉ dùng cho lần `ATYPE_FIGHT` này, không sửa dữ liệu bộ nhớ chia sẻ gốc.
- `bSkipGoldboss = 0` (dòng 830, chú thích "Mật Chỉ chỉ rơi từ boss"): tắt tùy chọn né boss vàng của người dùng khi đang farm nhiệm vụ, vì vật phẩm loại 4 (Mật chỉ) chỉ rơi từ boss.
- Vì sao `pApData->bDaTau == 1` chứ không `if(pApData->bDaTau)`: chú thích dòng 824 — "WAuto.exe cũ gửi struct ngắn, đuôi buffer là rác"; bản WAuto cũ không có trường này nên vùng nhớ tương ứng chứa rác khác 0, so sánh `== 1` mới an toàn. Trường khai báo ở `D:\GAMEDEVNEW\Sources\Core\Src\ipc_shared.h:240` (`int bDaTau; // bat/tat auto Da Tau`), khởi tạo `= 0` ở dòng 403.

## 4. Bảng dữ liệu nhúng `D:\GAMEDEVNEW\Sources\Core\Src\KDaTauTables.h` (55.030 byte, 698 dòng, ASCII thuần — TCVN3 escape bát phân)

| Mảng (dòng khai báo) | Số dòng dữ liệu | Cột / ý nghĩa |
|---|---|---|
| `g_DTBuy[]` (44-80), `g_nDTBuyCount` | **35** | `DTBuyRow { nRow; nGenre, nDetail, nParticular, nLevel, nFive; szShop; szItem }` — nhiệm vụ loại 1 "Mua giúp ta": tiệm nào, món nào (nguồn `tasklink_buygoods.txt`) |
| `g_DTFind[]` (85-612), `g_nDTFindCount` | **526** | `DTFindRow { nRow; nGenre, nDetail, nParticular, nLevel, nFive; nMagic, nMin, nMax; szItem; szMagic }` — loại 2/3 tìm đồ; `nLevel/nFive == -1` = bỏ qua (ô "n"); `nMagic == 0` = khớp 5 trường (nguồn `tasklink_findgoods.txt`) |
| `g_DTShow[]` (616-663), `g_nDTShowCount` | **45** | `DTShowRow { nRow; nMagic, nMin, nMax; szMagic }` — thuộc tính cần trưng (nguồn `tasklink_showgoods.txt`) |
| `g_DTQuestMap[]` (667-683), `g_nDTQuestMapCount` | **14** | `DTMapRow { nMapId, nX, nY; szName }` — map nhiệm vụ loại 4; **tọa độ đơn vị Ô (cell), phải NHÂN 32 ra mps** (chú thích dòng 665) |
| `g_DTNpc[]` (686-698), `g_nDTNpcCount` | **10** | `DTNpcRow { nMapId, nX, nY }` — NPC Dã Tẩu ở 10 thành (hằng `DTNPC` viết tay trong script, theo `AUTO_DATAU_SPEC.md` mục 1) |

Marker chuỗi TCVN3: **36** hằng `static const char DTM_*[]` (dòng 6-41), gồm `DTM_OPT_*` (lựa chọn hội thoại), `DTM_MSG_*` (thông báo), `DTM_T1..T6_*` (mẫu câu 6 loại nhiệm vụ), `DTM_MIN_MARK` / `DTM_MAX_MARK`, `DTM_SENDER_HETHONG`.

### Cách SINH LẠI
- Chạy: `python D:\GAMEDEVNEW\ReverseTools\gen_datau_tables.py` (không tham số) → ghi thẳng `D:\GAMEDEVNEW\Sources\Core\Src\KDaTauTables.h` (dòng 5, 16, 201). In dòng tổng kết `OK: ghi ... (buy= find= show= map= marker=)`.
- File nguồn nó đọc (dòng 8-16):
  | Biến | Đường dẫn | Dùng cho |
  |---|---|---|
  | `TASKDIR` | `serverscript_jx2\datau_tasklink\settings\task\tasklink_{buygoods,findgoods,showgoods}.txt` | 3 bảng dữ liệu (tab-separated, đọc theo TÊN CỘT, `nRow` = số dòng vật lý 2-based như Lua) |
  | `SEA` | `...\script\global\seasonnpc.lua` | marker `OPT_CONFIRM / OPT_TASKPROCESS / OPT_ACCEPT / OPT_CANCELCONF / OPT_CANCEL1A/1B / OPT_CANCEL2 / OPT_NORMALCANCEL / MSG_PUNISH / MSG_LIMIT / MSG_FAILREQ / MSG_FAILSHXT / MSG_BAGFULL / MSG_CLAIMED` |
  | `HEAD` | `...\script\task\newtask\tasklink\tasklink_head.lua` | marker mẫu câu 6 loại: `T1_MUA, T23_TIM, T2_1CAI, T3_XEMXONG, T4_*, T5_*, T6_MANH, MIN_MARK, MAX_MARK` |
  | `GOODS` | `...\script\item\tasklink_goods.lua` | `MSG_TAM_PRE, MSG_TONGCONG, MSG_TAM_SUF` (tiến độ nhặt cuộn) |
  | `XAPHU` | `serverscript_jx2\jx1_edits\xaphu.lua` | `OPT_GODATAU` ("Đến nơi làm nhiệm vụ dã tẩu") |
  | `MAPIDX` | `...\script\task\newtask\map_index.lua` | `g_DTQuestMap` (lọc theo `QUEST_MAPS` 14 id, dòng 146) |
  | `NAMEDEF` | `Sources\Core\Src\CoreUseNameDef.h` | `SENDER_HETHONG` = "Hệ Thống" |
- Cơ chế an toàn: mọi marker đều `assert` (`find_lit_before` / `first_string_in_func` / `assert_in`, dòng 31-50) — nếu Lua đổi text thì script **ném AssertionError chứ không sinh file sai**. Byte TCVN3 được xuất dạng octal 3 chữ số (`c_escape`, dòng 21-29) nên header là ASCII thuần, không bị công cụ sửa file phá mã hóa. Ghi file bằng `wb` với CRLF, encode `ascii` (dòng 200-201). Số map phải đủ 14, ngược lại assert (dòng 152).

## 5. Ghi chú: khi server đổi text hội thoại Lua

1. Đồng bộ lại thư mục `D:\GAMEDEVNEW\serverscript_jx2\datau_tasklink\...` (và `jx1_edits\xaphu.lua`) đúng bản Lua server đang chạy.
2. Chạy `python D:\GAMEDEVNEW\ReverseTools\gen_datau_tables.py`. Nếu nó `AssertionError` → chính chuỗi đó đã đổi: sửa literal tương ứng trong script (dòng 91-116) cho khớp text mới, rồi chạy lại.
3. Build lại **CoreClient.dll** (KDaTauTables.h là header nhúng, `CoreShell.cpp:40` include; không build lại thì marker cũ vẫn nằm trong DLL). Deploy vào `bin\client` bằng RENAME, RESTART game.
4. File Lua nào chi phối marker nào:
   - `seasonnpc.lua` → toàn bộ lựa chọn hội thoại NPC Dã Tẩu (nhận / trả / hủy / hủy bằng 100 mảnh SHXT) + thông báo phạt, đủ 40 lần/ngày, thiếu yêu cầu, thiếu mảnh SHXT, túi đầy, đã lãnh thưởng. **Đổi ở đây = máy trạng thái mất khả năng nhận/trả nhiệm vụ.**
   - `tasklink_head.lua` → mẫu câu nhận diện 6 LOẠI nhiệm vụ + mốc "nhỏ nhất:/lớn nhất:". Đổi ở đây = phân loại nhiệm vụ sai.
   - `tasklink_goods.lua` → thông điệp tiến độ nhặt cuộn ("Bạn nhận được một tấm ... tổng cộng ... tấm."). Đổi ở đây = đếm cuộn hỏng.
   - `xaphu.lua` → dòng chọn "Đến nơi làm nhiệm vụ dã tẩu" ở Xa Phu (dịch chuyển).
   - `map_index.lua` → tọa độ/tên map loại 4.
   - `CoreUseNameDef.h` → tên người gửi "Hệ Thống"; nếu đổi phải sửa **cả** so sánh hằng tay ở `KProtocolProcess.cpp:4141` (`"H\326 Th\350ng"` viết cứng trong hook, KHÔNG dùng `DTM_SENDER_HETHONG`).
5. Bảng `.txt` (`tasklink_*goods.txt`) đổi thì cũng phải sinh lại — `nRow` của bảng nhúng là SỐ DÒNG VẬT LÝ trong file txt, chèn/xóa dòng làm lệch toàn bộ ánh xạ.
