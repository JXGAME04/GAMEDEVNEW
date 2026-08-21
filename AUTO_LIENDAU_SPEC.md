# DAC TA VIET AUTO LIEN DAU (WLLS) — cho phien sau lam auto
# Viet 21/08/2026 boi phien da PORT tinh nang nay (doc truoc: BANGIAO_LIENDAU_THICONG.md)

Muc dich: du kien cho MOT PHIEN KHAC viet auto tu dong tham gia lien dau
(WAuto phia client cho nguoi choi, va/hoac bot KPlayer phia server).
MOI CON SO trong file nay trich truc tiep tu script dang chay tren
`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` (SRV) — co ghi
file:dong de doi chieu. KHONG doan lai; cai gi doi qua config thi doc config.

## 0. NGUON SU THAT (doc bang vn_edit.py --read, file TCVN3)

- Lich + mua giai:      SRV\script\leaguematch\timetable.lua
- Hang so he:           SRV\script\missions\leaguematch\head.lua (GLB 820-826,
                        task id, timer, cap, item cam, bang WLLS_TAB)
- 7 the loai:           SRV\script\missions\leaguematch\macthtype\*.lua
- Thoai NPC (label):    SRV\script\missions\leaguematch\npc\*.lua
- **CAU HINH VAN HANH:  SRV\script\leaguematch\wlls_config.lua** — van hanh co
  the DOI gio mo (GIO_MO), phut/luot (PHUT_MOI_LUOT), dong/mo 2 hang, the loai
  tung mua (LOAI_CO_DINH / LOAI_THEO_MUA), he so diem/thuong. AUTO PHAI doc
  file nay (hoac duoc cap tham so trung voi no) truoc khi tinh gio.
- Bo test: item "Lenh bai Admin" (6,1,4814) -> muc "Lien dau: bo test" — ep
  pha 4 de test auto BAT KY LUC NAO, khong phai cho 18h.

## 1. LICH THOI GIAN (mac dinh, khi config GIO_MO = nil)

- **Mua giai theo THANG**: mo ngay 08 -> dong ngay 28 (yymm08..yymm28,
  timetable.lua). Ngay 29 -> 07 thang sau = PHA 1 (nghi giua mua — chi nhan
  thuong xep hang + danh hieu, KHONG lap doi/danh).
- **Khung gio mo tran trong mua** (_tmp_calender, timetable.lua:2-28):
  - Thu 2..Thu 5:  18:00, 4 luot.
  - Thu 6/7/CN:    18:00 4 luot + **20:00 4 luot**.
- **1 luot = 15 phut** (WLLS_MATCHTIME, leaguematch\head.lua:42; config
  PHUT_MOI_LUOT). 4 luot = 18:00/18:15/18:30/18:45, het 19:00.
- Trong 1 luot (head.lua:112-117):
  - **4 phut dau = chuan bi** trong khu chuan bi (PREP_FREQ 10s x PREP_TOTAL 24).
  - vao san: dem nguoc 10 giay (FIGHT_PREP 2 x 5s) roi danh.
  - **tran toi da 10 phut** (FIGHT_TOTAL 120 x 5s). Het gio -> xu theo tong
    sat thuong PHAI CHIU (it hon thang; do o dong tru mau that, sau khien).
- matchid = yymmdd*100 + so_luot_trong_ngay (1..4 hoac 1..8 cuoi tuan);
  luot *99 = tran test do admin ep pha.
- Pha (GLB_WLLS_PHASE, id 820): 1 nghi giua mua / 2 trong mua-ngoai gio /
  3 giu cho vao lai / 4 dang mo tran. Driver tick 30s, sang quy 15' moi goi
  TaskContent (gsdriver.lua) -> pha tu doi theo dong ho.
- CLIENT KHONG doc duoc GLB truc tiep -> auto suy pha tu DONG HO he thong
  (+ config) la chinh; doi chieu bang thoai NPC + thong bao he thong
  (TB_WLLS_PHASE_MSG, wlls_gmscript.lua:12-17: "dang trong giai doan bao
  danh!" = pha 4 vua mo...).

## 2. HAI HANG DAU + DIEU KIEN THAM GIA

- **Kiet xuat (JUNIOR)**: cap 80-119 (WLLS_LEVEL_JUNIOR=80, head.lua:123).
  Su gia + hoi truong rieng. Mac dinh config dang MO (goc Linux DONG).
- **Vo lam (SENIOR)**: cap 120 tro len (WLLS_LEVEL_SENIOR=120).
- Su gia moi hang dat o thanh KHAC nhau (wlls_autoexec.lua:4-8):
  - "Su gia kiet xuat" (NPC 308, officer.lua): map 176 (1463,3247),
    37 (1777,3063), 78 (1546,3117)...
  - "Su gia lien dau " (NPC 308, officer.lua): map 80 (1753,3035),
    162 (1599,3150), 1 (1673,321x)...
  - "Su gia lien dau" (NPC 87, helper.lua — dang ky tim dong doi): 162/80/37...
- Dieu kien vao tran: co CHIEN DOI (LG type 5) dung the loai mua; so thanh
  vien <= max_member cua the loai; thanh vien khong dinh co OVER (mua truoc
  vuot cap -> mat tu cach, wlls_levelcheck head.lua:916); dang o pha 4;
  san chua day (400 doi/san); doi chua danh luot nay (LGTASK_LAST ==
  matchid thi thoi, joinmatch.lua:31/117).
- Cac cau TU CHOI khi bao danh (TB_WLLS_JOIN_SAY, wlls_gmscript.lua:3-10) —
  auto bat cac chuoi nay de biet ly do: het gio / chua co chien doi /
  san day 400 doi / da dat max tran / 2 thanh vien vao cung luc (thu lai) /
  qua dong.

## 3. BAY THE LOAI (LOAI = cot 1 bang mua, timetable.lua)

Xoay vong goc tu thang 1/2026: 2,1,2,5,3,6,2,1,4,5,3,6 (lap 12 thang).
Mua sid = 124 + (nam-2026)*12 + (thang-1). **Thang 8/2026 = sid 131 = loai 1.**
Config LOAI_CO_DINH/LOAI_THEO_MUA co the DE — auto phai doc config.

| Loai | Ten            | Nguoi/doi | Rang buoc ghep them            | Ladder L1/L2 |
|------|----------------|-----------|--------------------------------|--------------|
| 1    | Song dau       | 2         | tu do                          | 10196/10197  |
| 2    | Chien doi mon phai | (nhieu mtype) | THEO MON PHAI nhan vat  | 10198+ moi phai |
| 3    | Su do          | 2         | cap su-do (teacher.lua)        | 10223/10224  |
| 4    | Tam nhan       | 3         | tu do                          | 10225/10226  |
| 5    | Don dau        | 1         | mot minh                       | 10235/10236  |
| 6    | Song dau cung he | 2       | cung NGU HANH (Kim/Moc/Thuy/Hoa/Tho) | 5 ladder moi he |
| 7    | Nam nu phoi hop| 2         | 1 nam + 1 nu (mixeddoubles.lua)| 10248/10249  |

Auto dot dau nen nham loai 1 (Song dau — mua nay) va 5 (Don dau — de nhat,
khong can ghep nguoi). Rang buoc chi tiet moi loai: doc chinh file macthtype
tuong ung (ham npc_mylg + wlls_check_addmem).

Map 4 san moi hang (loai 1, double.lua:17-33; cac loai khac cung mau, id khac):
- Kiet xuat: hall/prep/arena = (396,560,397) (398,561,399) (400,562,401) (402,563,403)
- Vo lam:                     (540,570,541) (542,571,543) (544,572,545) (546,573,547)
- Toa do chuan: vao hall (1523,3024); khu chuan bi (1596,2977) — don vi CELL,
  dung thang cho NewWorld.

## 4. FLOW AUTO TUNG BUOC (bam theo thoai — label DA VIET LAI co dau 21/08)

B1. LAP DOI (pha 2 hoac 4; moi acc lam 1 lan moi mua):
  - Den su gia dung hang -> menu chinh officer.lua:43-48:
    * "Ta muon den khu thi dau hang ..."  -> want2signmap (B3)
    * "Chien doi hang ...cua ta"          -> mylg (lap/quan ly doi)
    * "Ta muon lanh phan thuong lien dau" -> wantaward (B6)
    * "Ta muon kiem tra diem Uy Danh"
    * "Bat tat nhan kinh nghiem khi thi dau"
  - Chua co doi -> nhanh mylg co "Ta muon lap...nhom/wlls_want2create" ->
    HOP NHAP TEN (S2C_INPUT_BOX nType=1; auto go ten <=16 byte; CAM ky tu:
    khoang trang, ", /, #, |, tab — officer.lua:308-320) -> tra loi bang goi
    input tro ve, callback nhan chuoi. De trong = lay ten nhan vat.
  - Them nguoi (loai >1 nguoi): acc phu TO DOI thuong voi doi truong; doi
    truong vao mylg -> "De doi huu cua Ta gia nhap vao doi thi dau nay" ->
    chon ten. Chi DOI TRUONG (LGMTASK_JOB=1) lam duoc.

B2. CHO PHA 4: tinh theo lich muc 1 (uu tien) — vao khung gio thi moi phut
  ranh kiem tra bang cach bam su gia (hoac admin ep pha khi test).

B3. VAO HOI TRUONG: su gia -> "Ta muon den khu thi dau hang ..." — pha <2
  bi tu choi ("tam nghi... dong cua"); OK thi teleport hall + luu vi tri cu
  vao task 1715/1716/1717 (FROMMAP/X/Y — dung de ve).

B4. BAO DANH (trong hall, pha 4): NPC "Quan vien hoi truong" (3 vi tri
  1514,3015 / 1534,3050 / 1484,2999):
  - CHUA co doi -> bi DUA VE THANH IM LANG (khong loi — auto phai hieu).
  - pha <3 -> hoi "co can ta dua nguoi ve?"; pha 3 -> "tam nghi o day".
  - pha 4 -> "Ta muon tham chien!" -> 2 man xac nhan ("Ta da san sang!",
    "Xac nhan") -> kiem DO CAM (muc 5) -> ca doi keo vao KHU CHUAN BI.
  - Cac cau tu choi: muc 2.

B5. TRAN DAU:
  - Khu chuan bi: cho ghep (relay ghep theo ty le thang (win*3+tie)/total,
    10 gio; tranh 3 doi thu gan nhat EMY1-3; can >= WLLS_MIN_TEAM=2 doi).
    NPC "Thi ve hoi truong" bao con may phut. RA KHOI = bo cuoc.
  - Vao arena: dem 10s roi danh. Trong san: cam bay sap/giao dich, the luc
    dong bang, exp co the tat qua "Bat tat nhan kinh nghiem".
  - Thang: ha guc het doi ban HOAC het 10' it sat-thuong-phai-chiu hon.
  - Nguoi CHET hoac ROI MAP bi go khoi tran ngay (fix fc957e15) — auto
    dung nhay map giua tran.
  - Xong tran tu ve hall; ve thanh bang "Xa phu" (chon 7 thanh) hoac
    Thi ve "dua ta ve".

B6. THUONG (auto nen nhan het):
  - MOI TRAN (award.lua:33-48, tu dong + thong bao Msg2Player):
    thang 10.000.000 exp + 3 uy danh; hoa 6.000.000 + 2; thua 2.000.000 + 1.
    Diem DOI: thang*5*cap_hang + hoa*2*cap_hang (config DIEM_THANG/HOA).
    OFFLINE luc ket thuc tran -> exp mat (GetUUID()==0), diem doi van tinh;
    bu offline khi login (wlls_login).
  - CUOI MUA pha 1 (ngay 29->07): su gia -> "Ta muon lanh phan thuong lien
    dau" -> "phan thuong xep hang" (top1..128: diem + Lien dau le bao 4416 +
    item 4550 theo bang award_rank) va "giai thuong danh hieu" (TOP 4, moi
    nguoi 1 lan/mua, han 20 ngay, tu deo). Doi hang Vo lam: danh hieu
    200-207 KEM hieu ung (skill 976 lv3-5...); Kiet xuat 85-88 chi chu.
  - Doi diem danh du (task 37): "doi phan thuong danh du" -> shop 93.

## 5. DO CAM MANG VAO SAN (auto PHAI cat vao ruong truoc khi bao danh)

Kiem o: hanh trang + tui mo rong + TREN TAY + 9 o dung-ngay (wlls_en_check).
Danh sach WLLS_FORBID_ITEM (head.lua:149-180+, ~30 muc, (genre,detail,particular)):
- Toan bo hoan PK (6,1,218..235): Cong Toc/Bao Toc/Pho Phong/Doc Phong/
  Bang Phong/Hoa Phong/Loi Phong/Giam Thuong/Giam Hon/Giam Doc/Giam Bang/
  Pho Cong/Doc Cong/Bang Cong/Hoa Cong/Loi Cong/Truong Menh/Truong Noi hoan.
- Yen Hong dan (6,1,115), Xa Lam dan (6,1,116), Noi Pho/Doc/Bang/Hoa/Dien
  hoan (6,1,117..121).
- Nhom (6,0,1..6+): Truong Menh/Gia Bao/Dai Luc/Cao Thiem/Cao Trung/Phi Toc
  hoan... (doc tiep head.lua:175+ cho du).
Mang theo -> bi mang "that to gan... dem duoc pham cam vao" va KHONG vao san.

## 6. TASK CLIENT DOC DUOC (sau khi server SyncTaskValue — sau moi tran + login)

- 37 danh du, 39 uy danh, 2500 tich luy ca nhan, 1720/1721/1722 thang/hoa/tong,
  4125 chuoi thang lien tiep, 1726-1732 anh xa thong tin DOI (LGTYPE/LGPOINT/
  LGRANK/LGWIN/LGTIE/LGTOTAL/LGTIME), 1734 cong tac exp, 1719 da-nhan-thuong.
- Task >=256 di kenh UI_TASKVALUE (he bang nhiem vu F11 da co san tu 4618a2d0).
- 1715/1716/1717: vi tri truoc khi vao san (auto co the doc de biet dang
  trong mach lien dau).

## 7. GOI Y KIEN TRUC AUTO (WAuto)

- May trang thai theo pha: NGOAI_MUA -> CHO_KHUNG_GIO -> LAP_DOI -> VAO_HALL
  -> BAO_DANH -> CHO_GHEP -> DANH -> NHAN_THUONG -> QUAY_LAI. Moi buoc co
  timeout + nhan dien cau tu choi (muc 2/4) de lui buoc.
- Bam menu NPC: match option theo BYTE TCVN3 trich tu script (dung go tay —
  trich bang vn_edit; cac label chuan la ban 21/08 da viet lai o tren).
- Hop nhap ten doi: xu ly S2C_INPUT_BOX (nType 1) — WAuto da co nen tu auto
  Da Tau (OpenGetString cung kenh).
- Danh nhau: dung nen autopk san co; luu y doi thu la NGUOI (khong phai quai)
  — can che do danh player trong pham vi arena; chet la bi loai.
- Test khong cho gio: nho admin ep pha 4 (Lenh bai) hoac van hanh dat config
  GIO_MO = { {0,0,96} } (mo suot ngay) roi restart.
- Bot server-side muon tham gia: co san lenh GM keo vao tran
  `dw wlls_player_join(...)` (wlls_gmscript.lua:147) — huong re nhat cho bot
  KPlayer lam doi thu; nghien cuu tham so truoc khi dung.

## 8. CANH BAO / CAM BAY DA GAP (dung lap lai)

- MOI FILE .lua MOT lua_State — dung tuong "dinh nghia o file A thi file B
  thay"; moi lien ket qua Include/DynamicExecute/dw (tre 1 tick ~1-2s).
- Thoai tu choi cua Quan vien khi chua co doi la IM LANG DUA VE THANH.
- matchid *99 la tran test admin — auto dung ghi cung dinh dang matchid.
- Lich/diem/thuong DOI DUOC qua wlls_config.lua — auto phai dong bo,
  dung hardcode 18h00 roi khoc khi van hanh doi gio.
- Loi script gio hien NGUYEN VAN trong SRV\ScriptError.log (co dong + ly do
  + traceback) — debug auto nho doc no truoc khi doan.
- Vi tri chuan trong san: dung toa do muc 3 (CELL, khong nhan 32 khi dung
  NewWorld; nhan 32 khi dung AddNpc).
