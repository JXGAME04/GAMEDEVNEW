# BAO CAO TONG HOP — 4 TRIEU CHUNG (26/08)

Nguon: 5 mui dao doc lap + 2 vong phan bien. Toi da **tu do lai** cac con so chiu luc va **tu doc lai** cac dong ma nguon quan trong nhat truoc khi viet (chi tiet ghi "toi kiem lai" o tung muc).

Kiem soat chat luong truoc moi phep dem: nhan tran that su `[AUTOLOG] bo qua` = **0 dong** o ca 4 tep (`jx_server_i.log`, `jx_client_i.log`, `dot9/sv.log`, `dot9/cli.log`) => moi ty le duoi day la **so dem chinh xac**, khong phai uoc luong.
(Canh bao: `grep -c "bo qua"` cho ra 2526 trong `jx_server_i.log` nhung do la chu trong than nhan `[E2-RECV-RANDMOVE] ... bo qua sat thuong` — duong tinh gia. Dung `grep -c "\[AUTOLOG\] bo qua"`.)

---

## TOM TAT MOT TRANG

| Trieu chung | Trang thai | Goc |
|---|---|---|
| **Truot toi-lui** (NPC/nguoi choi khac truot muot toi rooi lui) | **DA TIM RA GOC** | `KNpcFindPath::GetDir` tra **cung ma 0** cho "DA TOI DICH" va "BI CHAN" |
| **Miss can chien** (danh vao khong khi) | **DA TIM RA GOC** | Toa do **cua chinh nhan vat** giua client va server lech, va client **co y** khong bao gio nan lai |
| **Chet cham ve thanh** | **DA DO XONG — KHONG PHAI LOI ENGINE** | Engine hoi sinh het 0,87 giay; cai "cham" la **diem hoi sinh o cach cho chet 296 o**, phai chay ve 16-18 giay |
| **Nam bep** (nam duoi dat sau hoi sinh) | **CHUA TIM RA GOC** | 0/47 cu chet trong log tai hien duoc; 4 gia thuyet manh nhat da bi bac bo bang ma nguon |

Mot cau cho chu game: **hai trieu chung nang nhat (miss + truot) deu la mot chuyen — client va server khong dong y voi nhau ve vi tri cua nhan vat, va ca hai ben deu duoc lap trinh de "khong nan lai".**

---

# 1. TUNG TRIEU CHUNG

## A. TRUOT TOI-LUI — **DA TIM RA GOC** (do tin cay: chac)

### Co che (toi da tu mo tung dong)

1. `KNpcFindPath.cpp:52` — `if (!CheckDistance(nXpos>>10, nYpos>>10, nDestX, nDestY, nMoveSpeed)) { ...; return 0; }`
2. `KNpcFindPath.cpp:250-253` — `CheckDistance` tra `(dx*dx+dy*dy) >= nDistance*nDistance`. Vay dau `!` nghia la **khoang cach con NHO HON mot buoc chan = DA TOI DICH**.
3. Nhung cung ham do con tra `0` o dong **154 va 165** khi **THAT SU bi vat can chan**.
4. `KNpc.cpp:4621` goi `GetDir`; `KNpc.cpp:4640` gop **ca hai y nghia** vao mot nhanh `else if (nRet == 0)`.
5. Trong nhanh do: neu la **chinh nhan vat minh** thi `KNpc.cpp:4648-4652` goi `DoStand()` roi return (dung); con **moi NPC khac** (tuc moi nguoi choi khac tren man hinh) thi `KNpc.cpp:4656-4657`:
   `m_nNeedFixPos++;` roi `DoStandBlocked();`
6. `KNpc.cpp:2138-2148` — `DoStandBlocked()` **chi doi hoat anh** (`m_ClientDoing`), **khong dat `m_Doing`**, khong dat lai dich. Vi `m_Doing` van la `do_run` nen cua vao `ServeMove` (`KNpc.cpp:4592`) van mo, tick sau lai `ret=0`, lai `++`. Bo dem tang mai.
7. Bo dem do bat cua ghi de toa do o `KProtocolProcess.cpp:2135`, va cho ghi de nay **chi ha co khi goi sync doi vi tri that** (`KProtocolProcess.cpp:2144-2153`). NPC dung dung cho => khong bao gio duoc ha co.
8. Ban `_SERVER` lam **dung**: `KNpc.cpp:4674-4683` `if(nRet==1){...} else { DoStand(); return; }`. Chi ban client la sai.

**Vong day thun:** co bat -> goi sync ke tiep ghi de toa do NPC bang vi tri CU cua server (keo lui) -> tick sau duong thong, `KNpc.cpp:4627` xoa co, NPC chay toi lai -> toi dich -> `4656` bat co lai -> lap 3-5 lan/giay.

**Vi sao **bay gio** moi thay ro:** lop noi suy ve (`CoreShell.cpp:18223`, snap tai `PAINT_INTERP_SNAP_DIST = 64 mps = 2 o`) bien moi cu keo <= 2 o tu **mot cu nhay 1 khung gan nhu vo hinh** thanh **mot cu truot muot trai deu ca tick**. Mat nguoi bam theo chuyen dong muot tot hon han nhay dot ngot.

### Bang chung so

- **So hoc tung dong tren `[E4_MOVE_PATH]`** (nhan in ca `ret`, `cur`, `des`, `speed` nen tinh nguoc duoc GetDir da re nhanh nao):
  - `dot9/cli.log`: `ret=0` n=160 -> **158 dong (98,8%) la DA TOI DICH**, chi 2 dong bi chan that.
  - `jx_client_i.log`: `ret=0` n=328 -> **324 (98,8%)** da toi dich.
  - Kiem dinh don vi (bat buoc): `ret=1` n=1116 -> **0 dong** co d < speed. Dung nhu ma nguon doi hoi.
  - Ty trong `ret=0`: client 160/1116 = **14,3%** so voi server 125/4878 = **2,6%**.
- **Vi du vang, npc=92822** (nguyen van, `dot9/cli.log`):
  - `t=61462344 [S6-CMD] lenh=run npc=92822 idx=103 ap=1 dich=(47152,103660)`
  - `t=61463914 [E4_MOVE_PATH] npc=103 id=92822 ret=0 dir=0 cur=(48301056,106147840) des=(47152,103660) speed=18 doing=3` -> `cur>>10 = (47169,103660)`, cach dich **17 mps** trong khi mot buoc chay la **18 mps** => "toi dich" ngay tick dau.
  - `t=61479238 [S6-SYNC] npc=92822 idx=103 ... fix=304 doing=3 nan=1` — **fix leo tu 0 len 304 qua 963 tick / 53,4 giay / 146 dong sync, cl == sv tung chu so, khong nhuc nhich.**
- **Do lon cai mat nhin thay** (`dot9/cli.log`, 682 giay): **5632 cu keo <= 2 o = 8,3 cu/giay**, p50 = 19 mps, p90 = 44 mps; tong quang duong bi keo = **3952 o**. Them **1329 cu > 2 o bi SNAP = 1,95/giay** (chinh la trieu chung "bien mat roi hien lai").
- **Huong keo:** 1734/2779 = **62,4% nguoc huong dang di**, chieu len huong di trung binh **-10,8 mps**.
- **Vi sao chu game van thay te dù so do drift da giam:** so **SU KIEN** nan lai **TANG**: `jx_client_i.log` 23,92 lan/giay -> `dot9/cli.log` **30,02 lan/giay** (+25%). Dot va truoc do cham vao **bien do** chu khong cham vao **so lan**.
- Ty trong trang thai ket: **7,9%** cua (NPC x giay) o trang thai `doing=do_run` + `fix>=11` + `cl==sv`.

### Ban va toi thieu (rui ro thap)

1. `KNpcFindPath.cpp:52` — doi `return 0;` thanh `return 2;` (ma rieng cho "da toi dich").
2. `KNpc.cpp:4640` — them nhanh **truoc** nhanh `nRet == 0`:
   `else if (nRet == 2) { DoStand(); return; }` — dung y het ban server o `4674-4683`.
3. `KNpc::DoStand()` — them `m_nNeedFixPos = 0;` canh dong `m_Doing = do_stand;`.

**An toan:** toi da grep ca cay nguon — `GetDir` **chi co DUNG MOT noi goi** (`KNpc.cpp:4621`), nen doi ma tra ve khong pha cho nao khac.
**Khong** ha `PAINT_INTERP_SNAP_DIST` — do la che trieu chung, khong phai chua goc.

---

## B. MISS CAN CHIEN — **DA TIM RA GOC** (do tin cay: chac)

### Pheu day du (toi tu dem lai toan bo, cua so vang t=59907351..61218620, 21,9 phut)

```
client gui        6546 goi danh   [S6-ATK]
server nhan       6546 goi        [S3-PKT-IN]     -> khong mat goi nao, tre p50 = 4 ms
  - 649  ( 9,9%)  muc tieu server KHONG CON   [S3-TGT-FIND] found=0
  - 2330 (35,6%)  bi nuot giua hoat anh       [S3-CMD-SWALLOW]
  - 2509 (38,3%)  bi ghi de trong hang doi mot khe
  - 1052          toi duoc phan quyet tam danh
      trong do 557 (52,9%) BI TU CHOI "XA QUA" [S2-MELEE-TOOFAR-RUN]
               495 (47,1%) thanh don that     [S4-CAST]
  - trong 495 don that, 127 (25,7%) KHONG gay bat ky sat thuong nao
```
Ket qua cuoi: tren 6546 lan bam, chi **368 don co sat thuong** = **5,6%**.

### Da loai tru duoc (khong phai cac thu nay)

- **Khong phai cong thuc ne don:** 539/539 dong `[S1-WHO]` deu `missrate=0`; toi dem lai: **0 dong** co missrate khac 0.
- **Khong phai tre mang:** ghep 1-1 6546 cap `[S6-ATK]` <-> `[S3-PKT-IN]`, lech p50 = 4 ms, p90 = 9 ms, max = 116 ms, khong mat goi.
- **Khong phai server tut nhip:** `dot9/perf.log` `SW_MAINLOOP n=1080 tb=4.26ms max=10.96ms p95=6ms chiem=7,7%`.
- **Khong phai vu khi/ngua/noi luc:** `[S2-CANCAST-DENY]` = **0 lan** (toi dem lai).

### Goc that

**Toa do cua CHINH NHAN VAT giua client va server lech, va client duoc lap trinh de KHONG BAO GIO tu nan.**

Toi tu doc va xac nhan: trong `KProtocolProcess::SyncNpcMin` co **hai** cho ghi de vi tri, va **ca hai deu loai tru chinh minh**:
- `KProtocolProcess.cpp:2104` — `if (Npc[nIdx].m_RegionIndex != nRegion && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)`
- `KProtocolProcess.cpp:2135` — `if (Npc[nIdx].m_nNeedFixPos > 0 && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)`

Va **ai la nguoi quyet dinh danh** thi cung la client: tren server `KNpcAI::Activate` gap `IsPlayer()` chi goi `ProcessPlayer()`, ma ban `_SERVER` cua ham do **chi co `TriggerObjectTrap(); TriggerMapTrap();`** — may chu **khong bao giờ tu duoi bam thay nguoi choi**. May duoi bam nam o client (`KNpcAI::FollowPeople`, trong `#ifndef _SERVER`; nhanh trong tam `KNpcAI.cpp:1114`, nhanh chay lai gan `KNpcAI.cpp:1139`).

=> Client tin minh **da trong tam** nen no ban lien tuc va **khong he chay**; server do bang toa do cua no thay **xa qua** nen tu choi. Cua tu choi: `KSkills.cpp:359-360` `if (distance > GetAttackRadius() + 20) return -1;` (voi chieu 361: nguong = 110 mps).

**Do lech thuc do duoc** (nhan `[S6-ME]`, khong tiet che; chi so sanh trong cung mot dong co reg client == reg server nen khong dinh bay tai dung khe region):
- Toan cua so n=8128: p50 = 31 mps, p75 = 68, p90 = 109, p99 = 216, **max = 291 mps (9 o)**.
- **Rieng cac khoanh khac bi tu choi** (n=512 trong +-300 ms quanh mot TOOFAR): **p50 = 66 mps (2 o)**, p90 = 155 mps.
- **478/512 = 93,4%** truong hop do lech **mot minh no** da du de giai thich phan quyet "xa qua".

**Cum dong dinh** (nguyen van, hai tep, cung dong ho):
```
client t=59925569 [S6-ME] nhanh=GIUNGUYEN cl=(7,12,30720,16384) reg=1 sv=(9,10,21504,29696) reg=1 doing=1
        (lap Y HET o 59925680 / 59925809 / 59926026 / 59926138 / 59926346 - dong bang 1,15 giay)
server t=59925557 [S2-MELEE-TOOFAR-RUN] npc=91423 id=92422 skill=361 tgt_idx=91814
        dist=135 radius=90 cong=110 chay_toi=(45803,107983) map=(9,10) doing=1 ngua=1
```
Client tu cho minh o mps (254,400), server cho la (309,349) — lech **75 mps = 2,3 o**; server can 135 <= 110. Trong 1,15 giay do client ban ra **18 goi** `[S6-ATK]` va **khong phat mot lenh chay nao**. `nhanh=GIUNGUYEN` = client giu nguyen toa do minh.

### Ba viec phu cong huong (deu that)

- **Nguong ban cua WAuto qua sat:** `CoreShell.cpp:14723-14727` ep `nNearDist = max(nPKNearDist, 75)`; **1166/1166** dong `[PK-EMIT]` deu la `radius=90 near=75`. Bien an toan chi **110 - 75 = 35 mps**, trong khi do lech luc tu choi p50 = 66 mps. Auto ban thang vao vung server chac chan tu choi, va **khong bao gio biet minh bi tu choi**.
- **Luoi an toan cua server DA HONG (nhung sua no khong lam nhan vat nhuc nhich):** `KNpc.cpp:2502` dat lenh `SendCommand(do_run, ...)` roi `return`; luong quay ve `ProcCommand` va cham `KNpc.cpp:1014` `m_Command.CmdKind = do_none;` (dong nay o **cap ham**, sau khi ca hai khoi da dong — toi doc lai `KNpc.cpp:1006-1015`, dung y vay) => **lenh bi xoa ngay**. Do that: neu lenh song thi phai co 557 goi `s2c_npcrun`, thuc te chi **5/557 (0,9%)** trong 100 ms va toi da 7/557 o moi cua so den 2000 ms; rieng nhom `doing=1` chi **1/443**. Tuc **98,7% lenh bi xoa**.
  **Nhung**: `KPlayer::ConformIdx` (`KPlayer.cpp:989-990`) tra FALSE cho chinh minh, nen client **vut bo** moi goi `s2c_npcrun` gui cho no. Va co sua cho lenh chay that thi cung chi keo **ban sao ben server**, man hinh nguoi choi khong doi. => **Day la vet phu, khong phai goc.**
- **649 goi danh vao muc tieu server khong con** = 40,9 giay, chi 3 ID muc tieu, gom 3 cum. `KProtocolProcess.cpp:5237-5240` — khong tim thay thi server **im lang**, khong bao ve client; client van ve NPC do (`[S6-ATK]` 0 dong `thay=0`, 0 dong `tgreg=-1`) nen auto ban mai.

### Thu tu va (bat buoc theo dung thu tu)

1. **Nan toa do ban than co dieu kien** — trong `SyncNpcMinPlayer`, nhanh "GIUNGUYEN": khi `nRegion >= 0` VA lech >= 2 o VA `m_Doing == do_stand` thi ghi de toa do theo server. **Chi nan khi DUNG YEN** de khong tai dien loi "giut lui" da ghi trong `BANGIAO_DICHUYEN_GIATLUI_2508.md`. Truoc khi va: dat mot nhan khong tiet che in `m_Doing` + do lech + `m_RunSpeed` de xac nhan lech chi ton tai khi dung.
2. **Ha nguong ban cua auto** — `CoreShell.cpp:14724`, doi `75` xuong khoang **48** (1,5 o) de bien an toan >= 60 mps, phu duoc p50-p75 cua do lech. Viec re nhat, rui ro thap nhat, lam duoc ngay.
3. **Bao ve client khi muc tieu khong con** — `KProtocolProcess.cpp:5237`.
4. **Chi khi da xong (1)** moi dong den `KNpc.cpp:1014` (co giu lenh `m_bKeepCmd`), kem cua chan phat lai toi da ~2 goi/giay — client gui lai `do_skill` moi ~80 ms (14 dong TOOFAR trong 1,15 giay), khong chan se bom ~12 goi/giay toi moi nguoi trong region.

**Sau moi buoc phai do lai dung 3 con so:** so `[S2-MELEE-TOOFAR-RUN]`, ty le `[S4-CAST]`/`[S3-PKT-IN]`, ty le cast khong co `[S1-WHO]`.

### Con **mot mang chua giai** trong trieu chung nay

**25,7% don DA THI HANH ma khong gay bat ky sat thuong nao** (127/495 — toi tu dem lai; rieng chieu 361 la 127/472 = 26,9%). **Chua biet** la vien dan khong duoc sinh ra hay sinh roi bay hut. Da loai tru: khong phai ne don, khong phai het khe dan, **khong theo khoang cach** (0-9 mps hut 11%, 30-39 hut 41%, 60-69 hut 12%, 100-109 hut 28% — khong co xu huong).
Vi du ba don lien tiep hut o cu ly gan nhu dinh nhau, mau muc tieu con **tang** (hoi mau):
`t=59979448 [S4-CAST] npc=91423 skill=361 ... tgt=91463(id=92462 ... life=2733) dist=10 radius=90` -> t=59980401 life=2721 -> t=59981180 life=2733.

---

## C. CHET CHAM VE THANH — **DA DO XONG. ENGINE KHONG CHAM; CAI CHAM NAM O DIEM HOI SINH**

Toi tu do tren 3 tep server + 3 tep client (nhan `[S7-*]` **khong tiet che** nen dem duoc ca su vang mat).

### Phan engine: **NHANH**

| Doan | n | min | p50 | p90 | max |
|---|---|---|---|---|---|
| Chet -> het hoat anh chet (`CHET`->`CHET2`) | 43 | 773 | **835** | 845 | 848 ms |
| Chet -> server doi vi tri (`CHET`->`REV-XONG`) | 43 | 825 | **866** | 911 | **5894** ms |
| Tu luc nhan lenh -> doi vi tri xong | 41 | 0 | **0** | 22 | 42 ms |

- **41/43 cu chet: xong duoi 1 giay.** 2 cu > 4 giay la dung **2 cu nguoi choi KHONG bam nut** => roi vao bo dem tu dong 5 giay. Do la thiet ke, khong phai loi.
- `[S7-REV-NUOT] = 0`, `[S7-REV] 57 - [S7-REV-EP] 14 = 43 = [S7-REV-XONG] 43 = [S7-CHET] 43`. **May chu khong nuot mot cu hoi sinh nao.**
- Client: chet -> nhan goi hoi sinh p50 = 882 ms, roi -> nap ban do moi p50 = 186-220 ms (max 880 ms). Tong tren man hinh ~1,1 giay.
- 15 khung chet / 18 tick moi giay = **833 ms** — khop chinh xac p50 = 835 ms.

### Cai **thuc su cham**: diem hoi sinh o **rat xa**

- **43/43 lan hoi sinh deu ve DUNG MOT diem**: `ve sw=379 mps=(54016,98304)` (= o (1688,3072) tren ban do 379). Khong mot ngoai le.
- Khoang cach tu cho chet den diem do (ghep voi `me=` cua `[S4-CAST]` ngay truoc luc chet, 43/43 cu deu co mau): **min 171 o, p50 296 o, max 419 o**.
- **Thoi gian tu luc hoi sinh xong den luc danh duoc tro lai** (`REV-XONG` -> `[S4-CAST]`/`[S2-MELEE-TOOFAR-RUN]` dau tien): **n=43, min 16,4 giay, p50 18,0 giay, p90 24,0 giay, max 44,4 giay. Tong 862 giay = 14,4 phut** trong phien 46,4 phut.
- De doi chieu: `REV-XONG` -> lan chet ke tiep p50 = 41,2 giay. Tuc **danh duoc ~23 giay roi chet, roi chay ve 18 giay**.

**Ket luan:** khong co gi de va trong C++. Muon bot cham thi phai **doi diem hoi sinh** (script chet cua ban do 379 / cau hinh diem hoi sinh doanh trai), hoac cho hoi sinh tai cho co dieu kien. Day la viec **thiet ke/Lua**, khong phai loi engine.

### Mot diem **chua chot** dang chu y

**41/41 lan bam nut deu roi vao dai 803..946 ms sau khi chet** — dung luc hoat anh chet ket thuc (773..848 ms). Nguoi that khong the bam deu nhu vay. Rat co the **nut hoi sinh chi an duoc sau khi hoat anh chet chay xong** (server thi da chap nhan bam som: `[S7-REV-EP]` = 14/43 = 32,6% "bam luc con do_death frame=14/15 -> ep het hoat anh"). Neu dung, moi cu chet an them ~0,85 giay vo ich.
**Can them nhan:** mot dong `AUTOLOG` **khong tiet che** o phia client, tai ham xu ly bam nut hoi sinh (cho goi `SendClientCmd` cua `c2s_playerrevive`), in `timeGetTime()`, `m_Doing`, `m_ClientDoing`, `m_Frames.nCurrentFrame/nTotalFrame`; va mot dong nua o cho **mo hop thoai hoi sinh**. Doi chieu hai moc do voi `[S7-CHET-CLI]` la biet ngay nut bi khoa hay nguoi choi tu doi.

---

## D. NAM BEP — **CHUA TIM RA GOC**

### Trang thai co that va dang ghi nhan (phan **con dung vung** sau phan bien)

Phia client, khi nhan vat minh chet, het hoat anh chet se goi `DoRevive` (`KNpc.cpp:1641`). Nhanh client cua `DoRevive` (`KNpc.cpp:2201-2203`, `2234-2235`) dat `m_Doing=do_revive`, `m_ProcessAI=0`, `m_ProcessState=0`, `nTotalFrame=m_DeathFrame`, `m_ClientDoing=cdo_death`; roi `OnRevive` (`KNpc.cpp:2260`) **ghim** `m_Frames.nCurrentFrame = nTotalFrame - 1` moi tick. **Trang thai nay tu no khong het** — trong client khong co bo dem het gio nao. Do la mot **trang thai hap thu**.

### Nhung: **KHONG CO BANG CHUNG DUONG TINH** nao trong toan bo log

- **47/47 cu chet** (3 tep client) deu ket thuc bang `[S7-REV-CLI2]` voi `doing=1` va `cdoing` thuoc {1,2}. **Khong mot dong nao** `cdoing=8` di kem `doing=1`.
- **25.755 mau** cua hai nhan **khong tiet che** `[S6-ME]` va `[S6-SYNC](npc=92422)`: `doing=21` xuat hien **0 lan**, `doing=10` xuat hien **0 lan**.
- Phia server: `[S7-REV-NUOT] = 0` o ca 3 tep.

### Bon gia thuyet manh nhat **DA BI BAC BO bang ma nguon** (khong duoc dung lai)

1. **"Goi dong bo FULL dien lai hoat anh chet"** — bac. May chu **khong bao gio** gui goi mang `Doing = do_revive/do_death`: `KNpc::NormalSync` chan ngay dau ham, `KNpc.cpp:5764` `if (m_Doing == do_revive || m_Doing == do_death || !m_Index || m_RegionIndex < 0) return;`. Ca `s2c_syncnpcmin` (5772) lan `s2c_syncnpcminplayer` (5954) deu sinh trong ham do.
2. **"Trang thai nam chi co MOT duong ra"** — bac. Con it nhat 2 duong nua: **bi danh** (`KProtocolProcess.cpp:654` -> `DoHurt`, danh sach tu choi o `KNpc.cpp:1703` **khong co** `do_revive`; `OnHurt` `KNpc.cpp:1827-1832` goi `DoStand` + `m_ProcessAI=1`) va **doi ban do** (`KProtocolProcess.cpp:2745` `SendCommand(do_stand)` + `2752 SetProcessAI(TRUE)` -> nhanh nAI **co** `case do_stand` o `KNpc.cpp:908-910`). Duong doi ban do chay rat hay gap vi hoi sinh goi `ChangeWorld` ngay sau (`KPlayer.cpp:6829`).
3. **"Chi can mot goi hoi sinh bi mat la nam vinh vien"** — bac. Moi lan hoi sinh may chu gui **HAI goi doc lap** (`KPlayer.cpp:6822` gui thang + `6823` phat 9 vung); do that 1,96-2,00 goi/cu chet, hai goi den **cung mot mili-giay**. Duong truyen la **TCP** (`NetWork/GameServer.cpp:68` `SOCK_STREAM`) — khong mat goi le, mat la rot mang.
4. **"`DoStand()` return som la cai bay da len nong"** — bac. Trang thai kich hoat (`m_Doing==do_stand` VA `m_ClientDoing==cdo_death`) la **bat kha dat**: `cdo_death` chi duoc ghi o 2 cho (`KNpc.cpp:1560`, `2235`), ca hai deu kem `m_Doing = do_death`(1510)/`do_revive`(2201); `do_stand` chi duoc ghi o 2 cho (`KNpc.cpp:147`, `2108`), ca hai deu kem `m_ClientDoing` tu the dung. Log xac nhan: **47/47 lan `cdoing=8` di kem `doing=21`, 0 lan di kem `doing=1`**. Va o day **khong duoc va** — se la ma chet, con tao ao giac "da va" cho lan doc sau.

### Con lai nhung gia thuyet nao

- **(G1) Duong thi chieu:** `KNpc.cpp:2673` `CLIENTACTION ClientDoing = pSkill->GetActionType();` roi `2676-2680` **chi chan `>= cdo_count` (=15)** nen `cdo_death` (=8) **lot qua**; neu `CastMeleeSkill` that bai thi `2685` goi `DoStand()` va `2691 return 1` **ma khong dat `m_Doing`** => dang dung ma ket o tu the CHET. Doi hoi mot dong bang chieu co `CharActionId = 8` — **la loi DU LIEU, chua kiem bang `magicscript`**. Viec kiem re: dem so dong co cot action type = 8.
- **(G2) NPC mo coi:** `KNpc::Activate` return o `KNpc.cpp:676-677` khi `m_RegionIndex == -1` — **truoc ca** `m_DataRes.SetAction()` (738) lan `SetPos()` (750/755) => **dong bang ca tu the lan vi tri**. Muc do: `t=61911357 [S6-BANG] dung=228/256 mocoi=208 ...` = **91% NPC dang khong duoc cap nhat hoat anh**; va chinh nhan vat minh bi mo coi o **moi** lan nap lai ban do (31/31, 46/46, 16/16). Con mot **ngo cut that su** o `KProtocolProcess.cpp:2225-2231`: da mo coi ma vi tri server roi vao region chua nap thi ham `return`, **khong goi `LoadMap`** => mo coi vinh vien. **Chua lan nao no trong 3 tep log nay** (khong co dong `[S6-ME] nhanh=vaolandau` nao).

### Nhan log **can them** de chot lan sau (dat chinh xac, chi loc cho chinh nhan vat)

1. **`[S8-VE]` — quan trong nhat, la nhan DUY NHAT cho thay lop VE.** Dat trong `KNpc::Activate` ngay **truoc** `KNpc.cpp:738` `m_DataRes.SetAction(m_ClientDoing);`, trong `#ifndef _SERVER`, tiet che bang static rieng 1000 ms, chi cho `m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex`. In: `doing`, `cdoing`, `m_DataRes.m_nDoing`, `m_DataRes.m_nAction`, `frame`, `reg`, `cell`.
   Doc ket qua: `resdoing=8` => loi o lop ve; `cdoing=8` => loi o `KNpc`; ca ba deu =1 ma van nam => loi o `Represent`.
2. **`[S8-NAM]`** — ai da dat nam: dat ngay sau `KNpc.cpp:1560` (trong `DoDeath`) va ngay sau `KNpc.cpp:2235` (trong `DoRevive`), khong tiet che.
3. **`[S8-MOCOI-ME]`** — trong `KNpc::Activate` ngay truoc `return` o `KNpc.cpp:677`: dem tick lien tiep, in khi = 2 va khi vuot 18 (1 giay); dat lai ve 0 sau dong 738 kem `[S8-MOCOI-HET]`.
4. **`[S9-REV-CUU]`** — neu quyet dinh dung hang rao du phong (xem duoi), phai co nhan **khong tiet che** ngay tai cho va de con dem duoc no **co bao gio chay khong**. Bang chung hien tai noi la **khong bao gio**.

### Hang rao du phong (chi lam neu chu game muon, va phai coi la **PHONG NGUA** chu khong phai va loi dang xay ra)

Trong `KProtocolProcess::SyncNpcMinPlayer` (ham chay tu **2201 den 2372** — khong phai dong 2171 nhu ban nhap de xuat nham), o nhanh thu ba "GIUNGUYEN" (`~2323-2352`): neu `nNpcIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex` **va** `m_Doing == do_revive` thi goi `ProcNetCommand(do_revive)`.
**Khong** them dieu kien "goi bao con song": goi `NPC_PLAYER_TYPE_NORMAL_SYNC` (`KProtocol.h:530-541`) **khong co truong mau**, con `m_byDoing` thi server de trong (`KNpc.cpp:5960` bi chu thich, bien stack khong `ZeroMemory` => 2 byte **rac** len duong day moi goi — day cung la mot loi rieng nen vá).
**Luu y nhip:** nhanh nay chay p50 **111 ms** / trung binh 145 ms / **max 3.223 ms** (dot9: max 4.555 ms), khong phai "~55 ms".

---

# 2. XEP HANG VIEC NEN LAM

| # | Viec | Gia tri | Rui ro | Cong suc |
|---|---|---|---|---|
| **1** | **Tach "da toi dich" khoi "bi chan"** — `KNpcFindPath.cpp:52` `return 2` + `KNpc.cpp:4640` nhanh `nRet==2 -> DoStand()` + `m_nNeedFixPos=0` trong `DoStand` | **Rat cao** — chua thang trieu chung truot; 8,3 cu truot/giay bien mat | **Thap** (GetDir chi 1 noi goi, va co ban `_SERVER` lam mau o `4674-4683`) | 1 buoi |
| **2** | **Ha nguong ban cua WAuto** `CoreShell.cpp:14724` (75 -> ~48) | Cao — an ngay vao 52,9% don bi tu choi | **Rat thap** (chi doi mot hang so ben client, khong dung engine) | 30 phut |
| **3** | **Doi cho 3 nhan gia** (`KSkills.cpp:357` xuong duoi 359; `KMissleSet.cpp:65` vao than `if` o 66; `KSkills.cpp:1226`+`1228` vao than `if` o 1229) | Cao — khong doi gameplay nhung **chan chan doan sai lan sau** (da lua 3 lan) | Khong | 30 phut |
| **4** | **Nan toa do ban than co dieu kien** trong `SyncNpcMinPlayer` (chi khi DUNG YEN, lech >= 2 o) | **Rat cao** — day la goc that cua miss | **Trung binh-cao** — dung ho loi "giut lui" da tung xay ra. Bat buoc: dat nhan do TRUOC, va thu tren mot tai khoan truoc | 1-2 ngay |
| **5** | **Them nhan cho MISS-7** (`KSkills.cpp` sau dong 1225 truoc 1229 + nhanh `DoCollide` cua `KMissle.cpp`) roi **do lai** | Cao — con 25,7% don khong sat thuong chua biet goc | Khong (chi log) | 2 gio + 1 phien do |
| **6** | **Bao client khi muc tieu khong con** `KProtocolProcess.cpp:5237` | Trung binh — 7,1% thoi gian danh nhau | Thap | 0,5 ngay |
| **7** | **Doi diem hoi sinh ban do 379** (Lua/cau hinh) | Trung binh-cao voi cam giac choi: dang mat **18 giay chay ve x 43 lan = 14,4 phut / 46 phut** | Thap (script) | 0,5 ngay |
| **8** | **Them 4 nhan `[S8-*]` cho "nam bep"** roi cho trieu chung tai dien | Trung binh — hien khong bat duoc | Khong | 0,5 ngay |
| **9** | `KNpc.cpp:1014` co giu lenh (`m_bKeepCmd`) + chan phat lai | **Thap** — chi lam SAU muc 4; **mot minh no khong lam nhan vat nhuc nhich tren man hinh** vi client vut goi `s2c_npcrun` gui cho chinh no | Trung binh | 0,5 ngay |
| **10** | Vá 2 loi rieng nhat duoc: `m_byDoing` rac (`KNpc.cpp:5960`), va **hang dung** `KProtocolProcess.cpp:1977` `if (m_Doing != do_death \|\| m_Doing != do_revive)` | Thap (hien vo hai) | Thap — **nhung TUYET DOI khong doi `\|\|` thanh `&&`**, se chan mat duong `do_revive` | 1 gio |

**KHONG LAM:** vá `KNpc::DoStand()` cho trieu chung nam bep (ma chet); ha `PAINT_INTERP_SNAP_DIST`; nang `MAX_NPC`.

---

# 3. NHUNG CON SO CU PHAI RUT LAI

Cac ket luan sau **da bi phan bien bac bo bang ma nguon hoac bang phep dem lai** — dung dung lai chung, va dung vá theo chung.

| Ket luan cu | Vi sao sai | Thay bang |
|---|---|---|
| "Goi dong bo FULL cua chinh minh mang `doing=10` lam **dien lai hoat anh chet**" | `KNpc::NormalSync` `KNpc.cpp:5764` chan tu goc: server khong bao gio gui goi co `Doing = do_revive/do_death` | Trang thai nam la **trang thai hap thu tu ghim khung** (`KNpc.cpp:2260`), khong phai do goi dong bo |
| "Trang thai nam chi co **DUNG MOT** duong ra" | Con it nhat 2: bi danh (`KProtocolProcess.cpp:654` -> `KNpc.cpp:1827-1832`) va doi ban do (`KProtocolProcess.cpp:2745/2752` -> `KNpc.cpp:908-910`) | Co 3 duong ra |
| "Chi can **mot goi** hoi sinh bi mat la nam vinh vien" | May chu gui **hai** goi (`KPlayer.cpp:6822` + `6823`), do that 1,96-2,00 goi/cu chet, cung mili-giay; duong truyen **TCP** | Mat goi le khong xay ra |
| "`DoStand()` return som — **cai bay da len nong**" | Trang thai kich hoat **bat kha dat**; log: 47/47 lan `cdoing=8` deu kem `doing=21`, **0 lan** kem `doing=1` | Nhanh return som do dang lam **dung viec**: nuot lenh hoi sinh trung lap (moi cu chet nhan 2 lenh: `REV-BAM` 41 + `REV-AUTO` 16 = 57 = `[S7-REV]`) |
| "`[SKILL-REFUSE-FAR]` = don bi tu choi vi xa" | `KSkills.cpp:353-358`: dieu kien nam trong `#ifndef _SERVER` nen ban server **in vo dieu kien**. Bang chung quyet dinh: **0/62** dong phia client co `d <= 0,8R`, trong khi **2321/5277 = 44,0%** dong phia server co — neu cung ma thi xac suat ~1,4e-16 | Tu tinh lai tung dong theo `d > radius + 20`. Tu choi **that** chi co 4 dong trong `jx_server_i.log` |
| "`[MSL-SET-FULL]` / `[MISSLE-POOL-FULL]` / `[E3_MISSLE_ADDFAIL]` = het khe dan" | Ca 3 dat **truoc** cua chan. Nguyen van: `[MSL-SET-FULL] ... MAX_MISSLE=20000 dang dung=0` (0/20000 ma keu het khe); `[MISSLE-POOL-FULL] ... Add tra ve 1 -> BO QUA` (1 = thanh cong) | Bao dong gia hoan toan |
| "Dong `KNpc.cpp:1014` xoa lenh la **nguyen nhan so 1** cua miss" | Server **khong bao gio** tu duoi bam thay nguoi choi (`ProcessPlayer` ban `_SERVER` chi co 2 loi goi trap); nguoi duoi bam la **client** | Ha bac xuong "**luoi an toan cua may chu bi vo hieu**". Goc = lech toa do |
| "86,9% cap dong lien tiep van `doing=1` chung minh lenh `do_run` bi xoa" | Gia thuyet doi lap ("client don gian khong chay") tien doan y het => **bang chung khong phan biet duoc** | Dung bang chung thieu goi mang: **550/557 = 98,7%** khong co `s2c_npcrun` toi client |
| "1058 lenh toi duoc `DoSkill`" | Cong nham 6 dong `[S2-CANCAST-DENY]` von dung `AUTOLOG_IDX_EVERY(500)` = **co tiet che**; va `DoSkill` con nhieu duong return truoc do (`KNpc.cpp:2469/2471/2481/2489`) | "**1052 phan quyet CanCastSkill co ghi log** cho CaiBang (557 + 495), trong do **52,9%** la TOOFAR". Tong so luot vao `DoSkill` **khong biet duoc** |
| "95,7 giay = 16,6% cua 575 giay dang danh" | Nguong gom cum (3000 ms) khong duoc noi ra; doi nguong doi han: 250 ms -> 35,5 giay, 6000 ms -> 134,9 giay | "Nguong 3000 ms: 50 cum, 95,7 giay tren 447,7 giay danh nhau = **21,4%**" |
| "Nhip goi tu dong bo ~55 ms" | Do that: p50 = 111 ms, tb 144,8 ms, p90 330 ms, **max 3.223 ms** | Cham hon 2-80 lan |
| "fix cang cao cang trung khit **chung minh** NPC da toi dich" | Lap luan **tu vong**: `KProtocolProcess.cpp:2144-2153` chi reset khi vi tri khac nhau, nen "fix cao" la he qua cua luat reset | Dung so hoc tung dong tren `[E4_MOVE_PATH]`: **98,8%** dong `ret=0` la da toi dich |
| "291 lan nan lien tiep, quang duong keo = 0 mps" | Chuoi dai nhat that su la **290**, va trong do vi tri client **co doi** | Bo vi du do; dung vi du `npc=92822` (fix 0->304 / 53,4 giay / cl==sv tung chu so) |
| "Mang noi suy theo khe `idx` bi tai dung gay truot" | Guard `dwID` o `CoreShell.cpp:18243` chan **5014/5049**; chi con **14 lan/682 giay = 0,021 lan/giay** thuc su bi noi suy, so voi 8,3 cu truot/giay = chenh **400 lan**; `[S6-ADD]` co `npc=0` = **0 lan** | Khong phai goc |
| "Fix D lam tang `[S6-CMD] ap=0`" | 18,73% -> **17,80%** (10,06 -> 10,19 lenh/giay) — gan nhu khong doi | Bac bo. (Nhung 10 lenh/giay bi vut van la nguon phu dang xu ly sau) |
| (Da rut tu truoc, nhac lai) "73,5% NPC moi khong nhan lenh di chuyen" / "bang NPC client luon day" | Suy tu **su vang mat** cua nhan `AUTOLOG_EVERY` | Cam suy tu su vang mat cua nhom nhan do |

---

# 4. CHO MU CUA HE LOG

Nhung cho **hien khong nhin thay duoc**, xep theo muc do can bo sung.

### 4.1 Chua he thay lop VE — cho mu nghiem trong nhat
Toan bo he nhan hien tai chi chup `KNpc` (`m_Doing`, `m_ClientDoing`). **Khong co mot nhan nao** doc `m_DataRes` (`m_nDoing`, `m_nAction`, `m_SceneID`). Vi vay khi chu game bao "nam bep" thi **khong the phan biet** loi o `KNpc`, o `KNpcRes`, hay o `Represent`.
=> **Bat buoc them `[S8-VE]`** (vi tri chinh xac o muc D). Ghi chu ky thuat: `KNpcRes::Init` (`KNpcRes.cpp:56-146`) dat lai `m_nAction=0` **nhung khong dat lai `m_nDoing`**, trong khi `KNpcRes::SetAction` (`KNpcRes.cpp:991-992`) mo dau bang `if (m_nDoing == nDoing) return TRUE;` — khe NPC tai su dung co the ve bang bo anh sai. **Vá 1 dong:** them `m_nDoing = cdo_none;` canh `KNpcRes.cpp:66`.

### 4.2 **Khong co con quai nao trong ban ghi**
Suot 682 giay cua `dot9/cli.log`: `[S6-ADD]` chi co `kind=1` (5135, nguoi choi) va `kind=3` (169, NPC thoai); `[S6-SYNC]` chi co `kind=1` (168401) va `kind=3` (4478). **`kind=0` (quai) = 0 dong.**
=> Moi ket luan ve "quai bi truot / quai danh miss" **la suy tu ma nguon, khong phai do duoc**. Can mot phien do o ban do **co quai di chuyen**.

### 4.3 Nhan dat **TRUOC** cua chan = bao dong gia (da dinh 3 lan)
Ho nhan gia trong `KSkills.cpp`: `:188` `[E3_REJECT_TARGETONLY]`, `:193` `[E3_REJECT_BADIDX]`, `:230` `[E3_REJECT_SILENT]`, `:279` `[SKILL-REFUSE-WEAPON]`, `:300` `[SKILL-REFUSE-FIGHTMODE]`, `:357` `[SKILL-REFUSE-FAR]`, va `:371-376` `[E3_REJECT_RANGE_XY]` (**cung hinh dang** `#ifndef _SERVER` / dieu kien / `#endif` / `{ log }` — chua xac nhan duoc bang log vi 0 dong trong ca 3 tep).
=> **Ghi vao so tay du an:** moi nhan log dat truoc cua chan deu la bao dong gia.

### 4.4 Nhom nhan `AUTOLOG_EVERY` khong dung lam thong ke duoc
`KCore.h:215` — **mot** bien `static` cho **moi diem goi**, dung chung cho hang tram NPC, **khong loc ten**. Do duoc bao hoa: 60 dong `[SKILL-REFUSE-FAR]` trong `dot9/sv.log`, khoang cach lien tiep min = 1000 ms **dung bang**, 0 khoang < 1000 ms trong 60,4 giay => **60 la TRAN**, khong phai tan suat.
(Sua mot y phu tung noi qua: **khong** phai moi nhan `AUTOLOG_EVERY(1000)` deu ra 60 — `[SKILL-REFUSE-WEAPON]`=59, `[E3_RELATION]`=59, `[S3-CMD-SWALLOW]`=29, `[E3_LIST_CURLEVEL]`=12.)

### 4.5 Khong co nhan cho **hang doi lenh mot khe**
`m_Command` la mot bien don (`KNpc.cpp:4947-4950`); 2509/6546 = 38,3% goi bien mat khong dau vet = bi ghi de. **Chua chung minh duoc** rang lenh `do_run` cua nguoi choi (`KProtocolProcess.cpp:5205`) bi lenh `do_skill` de len — day la gia thuyet manh nhat ve **nguon sinh ra do lech toa do**.
=> **Can them:** `AUTOLOG_IDX(m_Index, "[S5-CMD-DEDE] cu=%d p=(%d,%d,%d) moi=%d p=(%d,%d,%d) doing=%d procai=%d")` ngay **truoc** `KNpc.cpp:4947`, chi in khi `m_Command.CmdKind != do_none`.
=> **Can them:** mot nhan khong tiet che trong `KNpc::RunTo` in **ly do dung som** (het duong / vuong vat can / het the luc).

### 4.6 Khong dem duoc **vien dan sinh ra**
`[E3_MISSLE_BORN]` va `[MISSLE-POOL-FULL]` deu la `AUTOLOG_EVERY` (khong loc ten) => khong dem duoc rieng cho CaiBang. `[S4-MSL-END]` chi ghi trong `KMissle::DoVanish` (`KMissle.cpp:1328`) nen **vien dan chet bang duong `DoCollide` khong bao gio in ra** — do la ly do con so "79/115 cast hut khong co MSL-END" **khong phai bang chung**.
=> **Can them:** `AUTOLOG_IDX` (khong tiet che) tai `KSkills.cpp` **sau** dong 1225 va **truoc** dong 1229, in `nMissleIndex`, `i`, `j`, `m_nChildSkillNum`, mps cua launcher va cua target; **va** mot `AUTOLOG_IDX` tuong tu trong nhanh `DoCollide` cua `KMissle.cpp`.

### 4.7 `AUTOLOG_IDX` **chi loc theo nguoi tung chieu**
Nen moi thu CaiBang **bi danh** deu vo hinh. Neu dot sau can ca hai chieu thi phai cho qua khi `nLauncher` **hoac** `nParam2` trung ten can theo doi.

### 4.8 Chua doc duoc cau hinh client dang chay
Khong tim thay `config.ini` nao trong `/d/GAMEDEVNEW`. Chi biet mac dinh trong ma nguon: `PaintInterp=1` (`S3Client.cpp:69`, `:511`), `PaintFps=30` (`S3Client.cpp:68`, `:506`), tran 60. **Can doc `config.ini` cua dung thu muc client dang chay** de biet bien do that cua lop noi suy.

### 4.9 Log server bi reset
`dot9/sv.log` chi con **61 giay cuoi** (t=61867614..61928840) trong khi client co 682 giay. Moi so lieu `dot9` phia server chi dung de **xac nhan xu huong** (n=10 don danh), **khong du de tinh ty le**.

### 4.10 Chua co mau doi chung "khong cuoi ngua"
**557/557** dong `[S2-MELEE-TOOFAR-RUN]` deu co `ngua=1`. Khong biet cuoi ngua co lam do lech nang them hay khong.
=> **Can:** mot phien do co chu game **xuong ngua 5 phut**, roi so lai ty le `[S4-CAST]`/`[S3-PKT-IN]` va phan bo do lech.

---

## PHU LUC — HAI CHO HAI MUI TUNG MAU THUAN, va ket luan cuoi

1. **"Dong `KNpc.cpp:1014` co phai goc cua miss khong?"** — Mui dieu tra miss noi **co (nguyen nhan so 1)**; vong phan bien noi **khong**. **Ben phan bien dung**, va lap luan quyet dinh la duong ma: tren server `ProcessPlayer` (ban `_SERVER`) **khong co ma duoi bam**, nen server chua bao gio la nguoi dieu khien chan nhan vat. Ca hai ben deu dong y **loi o dong 1014 la co that** — chi khac ve **xep hang**. Ket luan chot: **co that, nhung la vet phu; va no o vi tri thu 9 trong danh sach viec.**

2. **"Client co nan toa do cua chinh minh khong?"** — Ca ba mui deu do ra **khong** (`nhanh=GIUNGUYEN`), va toi da **tu doc lai ma nguon xac nhan**: ca `KProtocolProcess.cpp:2104` lan `:2135` deu co dieu kien `nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex`. Day la **chu y cua nguoi viet ma**, khong phai tai nan. Nen khi vá muc 4 thi phai coi la **doi thiet ke**, va phai thu ky vi day chinh la cho tung sinh ra loi "giut lui" thang 08.

3. **Diem chung cua ca 2 trieu chung nang:** cung mot cau hoi — *"khi client va server bat dong ve vi tri thi ai chiu nhuong?"*. Hien tai cau tra loi la **khong ai ca**: nhan vat cua minh thi client khong bao gio nhuong; nguoi choi khac thi client nhuong **lien tuc va sai luc** (moi khi co `m_nNeedFixPos` bat, ma co do dang bat nham vi ly do o muc A). Va o **truot**, mot dong `return 2` giai quyet duoc goc; o **miss**, phai chon lai nguyen tac nhuong.