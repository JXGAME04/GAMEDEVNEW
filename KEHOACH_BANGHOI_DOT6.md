# KE HOACH SUA HE BANG HOI JX2 (workflow 67 agent, 13/08/2026)

## NGUYEN NHAN CHINH - DON XIN VAO BANG

KET LUAN DUT KHOAT: co BON nguyen nhan doc lap, xep theo THU TU XAY RA tren duong di cua mot la don. Hai cai dau tien la NGUYEN NHAN CHINH, moi cai deu DU SUC gay ra trieu chung mot minh, va ca hai deu xay ra gan nhu 100% moi lan.

CHINH 1 (SERVER - chan tu goc, don khong bao gio duoc tao):
Sources/Core/Src/KTongJX2.cpp:401-415. Truoc khi ghi don, ham sJX2_DoApplyJoin quet PlayerSet cuc bo tim mot thanh vien online cua bang de doc Player[nChk].m_cTong.GetRecruit(); neu tra 0 thi return 8 = "Bang hoi nay dang dong tuyen nguoi", don KHONG duoc luu. Ma bang MOI TAO co m_nRecruit = 0 (Sources/MultiServer/S3Relay/KTongControl.cpp:77 va :123) va bang chu thi luon online (chinh ho la nguoi doi xem don), nen vong lap CHAC CHAN tim thay ho va CHAC CHAN return 8. Ket qua: bang nao chua tung bam nut "Dong/mo tuyen" thi MOI don deu bi tu choi thang, khong co gi de hien. Cong nay con bi lam nang them boi loi thu ba ben duoi (nut mo tuyen hien sai trang thai nen bang chu khong biet phai bam).
Ghi chu quan trong da kiem cheo: menh de "m_nRecruit khong song qua restart relay" LA SAI - TONGDB.CPP:111 co ghi va :189 co nap lai. Loi that nam o GIA TRI MAC DINH = 0 (dong tuyen) cho bang moi, khong phai o viec mat du lieu.

CHINH 2 (CLIENT - neu don co ton tai thi cung bi xoa truoc khi ve len man hinh):
Sources/S3Client/Ui/UiCase/UiTongJX2.cpp:1367-1378. RenderRecruit ghi dong tieu de "== Don xin vao bang (N) ==" vao m_Row[0] va cac don vao m_Row[1..4], roi NGAY SAU DO goi RenderMembers() o dong 1377-1378; RenderMembers mo dau bang ClearRows() (dong 1116), ma ClearRows (739-749) xoa m_Row[i], m_RowDim[i], m_MList[i] cho CA 14 dong. Tiep do 1379-1386 lai xoa them m_MList[0..4] va m_RowDim[0..4]. Ket qua: 5 dong dau panel phai TRONG HOAN TOAN - khong don xin, cung khong thanh vien.
Loi nay xay ra 100%, khong co cach vong tranh, vi m_bHasMember LUON = 1 truoc khi vao tab Chieu mo: OpenWindow (UiTongJX2.cpp:243-248) mo o trang FUNUSE, ma RequestPage nhanh FUNUSE (dong 616-622) xin ngay PAGE_MEMBER; goi MEMBER_SYNC ve dat m_bHasMember=1 (dong 692). Day la loi MOI, do chinh dot sua cuoi (f209960 - "moi tab deu hien danh sach thanh vien") sinh ra.
=> Neu phai chon MOT cai la "CHINH NHAT" thi la CAI NAY: no tat dinh, khong bam nut nao vong tranh duoc, va no lam ca 4 don dau tien vo hinh ke ca khi cong tuyen dung da mo.

PHU 3 (nut Dong/Mo tuyen hong nen bang chu khong biet duong mo):
Sources/Core/Src/CoreShell.cpp:9206-9213 boc case GTOI_TONG_GET_RECRUIT trong "if (uParam)", ma client goi voi uParam=0 o ca hai cho (UiTongJX2.cpp:1347 va :1706), nen ham LUON tra 0. Hau qua: nhan nut luon hien "Dang dong tuyen - bam de mo" du bang da mo, va vi nOpen luon = 0 nen bam nut LUON gui ApplyTongChangeRecruit(1) - chi mo duoc, khong bao gio dong duoc. (Mat tot: bam mot lan la mo, tuc PHU 3 co the vong tranh CHINH 1 mot cach tinh co.)

PHU 4 (kien truc - lam don bien mat ve sau, va lam duyet don that bai):
Don chi nam trong RAM cua MOT GameServer: KTongJX2.h:40-43 (szApplyName/dwApplyID/wApplyLevel, chu thich tu ghi "cuc bo GS"); grep toan bo Sources/MultiServer/S3Relay = 0 tham chieu. Hau qua: (a) don MAT SACH khi GameServer khoi dong lai; (b) neu chay 2 GameServer thi nguoi xin o GS-A nop don, bang chu o GS-B khong bao gio thay; (c) duyet don doi nguoi xin PHAI dang online tren dung GS do (KTongJX2.cpp:2446-2449 return 5) nen ty le duyet thanh cong rat thap. Ban JX2 goc lam nguoc lai: don nam tren RELAY (lop KTongZhaoMuRelay trong s3relay_y), dung chung cho moi GameServer, khong gioi han so don, cat 20 don/trang khi gui ve client.

BON CAI KHAC LAM TRIEU CHUNG NANG THEM (khong phai nguyen nhan goc):
- Nguoi nop don nhan duoc "Da gui don xin gia nhap, cho bang hoi duyet" (KTongJX2.cpp:2239-2243) nen tin la da gui thanh cong, trong khi bang chu KHONG duoc bao gi ca: sJX2_DoApplyJoin khong goi SendSystemInfo, khong ghi so su kien, khong day RECRUIT_SYNC.
- Chi ve 4/8 don (UiTongJX2.cpp:1369 ep i<4), vung bam chon chi mo cho dong 1..4 (:784, :1643), va hai nut Trang truoc/Trang ke chi RequestPage(m_nPage, 0) chu khong phan trang (:1928-1931).
- Tran luu tru 8 don (KTongJX2.cpp:429) trong khi ban goc KHONG gioi han so don; bang dong nguoi se bi tu choi oan voi thong bao sai noi dung ("Thao tac khong hop le hoac muc tieu khong online").
- Nhanh xin lai danh sach thanh vien cho tab Chieu mo (UiTongJX2.cpp:631-636) la MA CHET vi nPage da bi gan lai o dong 609-610.

## DANH GIA DO HOAN THIEN

DO HOAN THIEN THUC TE ~45-50% so voi ban JX2 Linux (khong phai 55-60% nhu uoc tinh phia may chu, vi phai nhan them he so client + van hanh).

CHIA THEO MANG:
- Giao thuc + ha tang dong bo (goi tin, bang g_nTongPSSize, kich thuoc goi, 2 ban KProtocol.h khop): ~92%. Chi con bay khoa-theo-IP khi chay 2 GameServer va relay khong khoa luong.
- Bang API Lua: 100% ve SO LUONG (182 ten dang ky, phu du 179 ham goc), nhung ~28/179 ham luon tra 0 vi khong co duong ghi field, 2 ham la stub (TWS_ApplyMaintain, TONG_ChangeAllMemberFeature). Nghiep vu that ~70%.
- Luu tru DB: ~85% (field bang / thong bao / su kien / lich su / van an chieu mo deu xuong Berkeley DB). CON THIEU HAN: danh sach DON XIN (chi RAM cua tung GameServer), dwTaskTemp, dwWsNpc.
- CHIEU MO / XIN VAO BANG: ~25%. Dat SAI TANG (GameServer thay vi Relay), tran 8 don (goc khong gioi han), ve 4 don, khong phan trang, duyet doi nguoi xin online, mat khi restart GS, khong thay duoc giua nhieu GS.
- Client UI: ~55%. 7/15 cua so goc da chep section, 113/346 section trong ini KHONG duoc Init lan nao. Thieu han 8 cua so: danh sach bang rieng, xem chieu mo bang khac, bo nhiem chuc vu, uy nhiem dai than, chi dinh danh hieu, phat tien (nhap so), he cai tao/giup do, cau hinh lanh dia. Trang Tin tuc co ban (47 section Info_*) khong bao gio mo duoc.
- Phan quyen: ~60%. Mat na 12 bit trong khi blueprint co 14 nut; 4 nut chet; o kiem khong nap trang thai va khong tich duoc bang tay; 4/12 quyen khong thao tac nao kiem.
- Kinh te bang: ~40%. Bao tri ngay khong tinh chi phi duy tri (field 16 khong ai ghi) nen khong tru gi va tro cap luon 0; cong hien tuan khoa 9/11 luon 0; tran quyen gop tuan (field 42) khong ap; truc xuat khong phat tien va khong chan; COP_DONATE cong doi (vua quy kien thiet vua ngan quy).
- Tac phuong: ~35%. Co khung field, khong tran cap, khong tran so luong, khong tru quy, khong san xuat. Lop Lua goc CO SAN nhung khong ai goi va thieu file trien khai.
- Muc tieu tuan: ~50% (Lua co du, NPC Te Dan da gan, nhung 15 ham Get doc field khong co duong ghi tu C++).
- Tuyet ky: ~20% (chi luu 2 con so, tru 6000 hard-code hang NGAY thay vi hang TUAN, khong cap SkillID).
- Lanh dia bang: ~40% (map + Lua co, nhung client gui nParam1=0, nut cau hinh nAct=-1, thieu 8 ham engine).
- Lien minh bang hoi: 0% (chi co 1 ham doc field 10, khong lop, khong opcode, khong bang LevelUnionNum duoc doc).
- Dai than / thoai an (An si) / doi danh hieu / doi tran doanh / chuyen quy / nhuong bang chu: 0% opcode phia server, nut client treo.
- Van hanh / trien khai: ~30%. Duong trien khai nam ngoai git (6 goc dich, 2 cay lib song song lech byte, NPC bang hoi bi tat o file ngoai repo), khong co script dong bo -> da tung dot mot vong test.

VIEC KHONG NEN LAM (rui ro cao hon loi ich, ghi ro de khoi lam nham):
1. KHONG doi khoa m_mapIp2Connect sang ID ket noi - se pha FindTongConnectByIP va FindPlayerByIpParam. Chi duoc doi RIENG duong quang ba sang CNetServer::BroadPackage.
2. KHONG nang MaxLen thong bao len 2000 - phai doi CA hai ban KProtocol.h + KWndEdit512, loi hon thiet; giu 127.
3. KHONG mo mat na quyen len 14 bit roi dung lai - 4 quyen 1002/1004/1903/2003 khong co cho nao kiem, chi doi tu "nut chet" thanh "nut tich duoc ma vo nghia". Phai lam ca cong kiem quyen hoac dung.
4. KHONG bat GameServer thu hai len truoc khi don xin len Relay va truoc khi sua khoa-theo-IP.
5. KHONG tach ZDBTable::add khoi commit() theo kieu toan cuc - anh huong moi noi goi; chi them addNoCommit rieng cho vong bao tri.
6. KHONG lam Lien minh / Dai than / He cai tao trong dot nay - phai them lop Relay + opcode + truong goi tin moi, khoi luong bang ca he chieu mo, trong khi khong ai dang dung.
7. KHONG dung tiep bo sinh ini tu blueprint (da tung bo mat Label/Text/Tip/RightID/Menu va phai sinh lai 2694 dong) neu chua dua bo sinh do vao kho.
8. KHONG sua file GBK/TCVN bang Edit/Write thuan - bat buoc dung skill swordonline-dev.

## KE HOACH (30 viec)

### #1 [P0] Client: tach ClearRows - het xoa danh sach don xin

**FILE:** Sources/S3Client/Ui/UiCase/UiTongJX2.cpp:739-749 (ClearRows), :1110-1116 (RenderMembers), :1342-1386 (RenderRecruit), :693-695 (DataArrive)

Them ham moi ClearMemberRows() chi xoa m_MList[0..13], m_RowDim[0..13], m_MDet[0..6] (KHONG cham m_Row). Trong RenderMembers doi ClearRows() o dong 1116 thanh ClearMemberRows(). Trong RenderRecruit: bo han vong xoa 1379-1386, chuyen loi goi RenderMembers() (1377-1378) len NGAY SAU ClearRows() o 1342 va TRUOC khi ghi m_Row[0..4] (1367-1374). Trong RenderMembers them tham so int nOffset=0, ghi thanh vien vao m_MList[i+1+nOffset]; RenderRecruit goi RenderMembers(5) de thanh vien bat dau tu m_MList[5], het de len 5 dong don. Trong DataArrive case PAGE_MEMBER (693-695), khi ms_pSelf->m_nPage == TJX2_UI_PAGE_RECRUIT thi goi RenderRecruit() thay vi RenderMembers() (de khong xoa don khi goi thanh vien ve sau). File GBK - sua bang skill swordonline-dev.

### #2 [P0] Server: go cong 'bang dang dong tuyen' sai - don bi tu choi thang truoc khi luu

**FILE:** Sources/Core/Src/KTongJX2.cpp:401-415 (sJX2_DoApplyJoin) va :2599-2647 (LuaTONG_ApplyJoin)

Khoi 401-415 quet PlayerSet cuc bo tim 1 thanh vien online de doc Player[nChk].m_cTong.GetRecruit(); bang moi tao co m_nRecruit=0 (KTongControl.cpp:77,123) nen bang chu dang online = LUON return 8, don KHONG BAO GIO duoc ghi. Sua: xoa han khoi 401-415, thay bang doc co chieu mo cua chinh he JX2: DWORD dwOpen = GetField(dwTongID, 67); if (!dwOpen && GetField(dwTongID,65)==0 && GetField(dwTongID,66)==0) { /* chua cau hinh chieu mo -> VAN CHO NOP DON */ } - tuc mac dinh MO. Neu muon giu cong tac thi phai dung field bang (dong bo qua relay) chu tuyet doi khong quet PlayerSet. LuaTONG_ApplyJoin (2599-2647) hien KHONG co cong nay - sau khi sua phai giong het sJX2_DoApplyJoin: tach than chung ra 1 ham static roi ca hai cung goi.

### #3 [P0] Sua nut Dong/Mo tuyen luon tra 0 (GTOI_TONG_GET_RECRUIT bi cong if (uParam) nuot)

**FILE:** Sources/Core/Src/CoreShell.cpp:9206-9213; Sources/S3Client/Ui/UiCase/UiTongJX2.cpp:1347 va :1702-1711

CoreShell 9207 'if (uParam)' bao quanh case GTOI_TONG_GET_RECRUIT, ma client goi voi uParam=0 nen luon tra nRet=0. Bo han khoi if, giu nguyen dong 9211 nRet = Player[CLIENT_PLAYER_INDEX].m_cTong.GetRecruit(); (dong 9209-9210 khai bao pItem va dong comment phai xoa vi khong dung). Sau do nhan nut o UiTongJX2.cpp:1348-1350 se hien dung trang thai va 1707 se dao dung chieu. Ghi chu de tranh bao loi oan: ApplyTongChangeRecruit (KPlayerTong.cpp:1764-1772) chi cho MASTER/DIRECTOR, nguoi thuong bam se nhan MSG_TONG_CHANGE_CAMP_ERROR1 - dung, khong sua.

### #4 [P0] Duyet don khong con bat nguoi xin phai dang online cung GameServer

**FILE:** Sources/Core/Src/KTongJX2.cpp:2434-2462 (ACCEPT_APPLY), :373-389 (sJX2_SendAddMember), :391-438 va :2599-2647 (noi ghi don), Sources/Core/Src/KTongJX2.h:40-43

Buoc 1: them truong gioi tinh vao ban ghi don - KTongJX2.h sau dong 43 them 'BYTE btApplySex[8];'. Buoc 2: tai KTongJX2.cpp:431-436 va :2639-2644 ghi them pTong->btApplySex[a] = (BYTE)Npc[Player[nPlayerIdx].m_nIndex].m_nSex. Buoc 3: viet ban thu hai cua ham gui: static void sJX2_SendAddMemberByName(KTongJX2Tong* pTong, const char* pszName, DWORD dwNameID, BYTE btSex, DWORD dwEchoIdx) - noi dung y het 375-388 nhung lay ten/ID/sex tu tham so thay vi Player[nJoinIdx], m_dwParam = dwEchoIdx (0 khi offline). Relay giai theo TEN (S3Relay/TongConnect.cpp:322-327 goi g_cTongSet.AddMember(szPlayerName, szTongName, m_btSex)) nen khong can doi giao thuc. Buoc 4: tai 2446-2449 bo cong return 5; goi sJX2_SendAddMemberByName(pTong, pTong->szApplyName[a], pTong->dwApplyID[a], pTong->btApplySex[a], (DWORD)sFindPlayerIdxByNameID(pCmd->m_dwTarget)). Buoc 5: dong 2459-2460 dang dung Player[nJoinIdx].m_PlayerName cho so su kien - doi sang pTong->szApplyName[a] (phai copy ra bien tam TRUOC vong don mang 2451-2456).

### #5 [P0] Bao cho bang chu / nguoi co quyen khi co don moi (hien don nop xong im lang tuyet doi)

**FILE:** Sources/Core/Src/KTongJX2.cpp:429-437 (cuoi sJX2_DoApplyJoin) va :2639-2645 (LuaTONG_ApplyJoin)

Sau khi btApplyCount++ (dong 436), them: (a) ghi so su kien de bang chu doc lai duoc sau khi offline: char szLog[160]; sprintf(szLog, "%s xin gia nhap bang", Player[nPlayerIdx].m_PlayerName); sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT, szLog, (DWORD)nPlayerIdx); (b) quet PlayerSet, voi moi Player[i] co m_cTong.GetTongNameID()==dwTongID VA (btFigure==0 hoac co quyen 1901) thi KPlayerChat::SendSystemInfo(1, i, MESSAGE_SYSTEM_ANNOUCE_HEAD, szLog, strlen(szLog)). Lay btFigure/quyen tu FindMember(pTong, g_FileName2Id(Player[i].m_PlayerName)) + sJX2_HasRight. Lam giong het o ca hai duong (client va Lua).

### #6 [P0] Go ma chet trong RequestPage - tab Chieu mo va Nhat ky khong bao gio xin danh sach thanh vien

**FILE:** Sources/S3Client/Ui/UiCase/UiTongJX2.cpp:606-639

Dong 609-610 gan lai nPage truoc, nen nhanh 631-636 (else if nPage==TJX2_UI_PAGE_RECRUIT || nPage==4) khong bao gio dung. Sua: dat mot bien 'int nUiPage = nPage;' o dau ham (truoc dong 609) va doi TAT CA dieu kien so sanh sang nUiPage; giu nguyen viec gan lai nPage cho ma trang server. Cu the doi 631 thanh 'else if (nUiPage == TJX2_UI_PAGE_RECRUIT || nUiPage == 4)'. Luu y sau khi sua thi tab Chieu mo se nhan them MEMBER_SYNC - viec so 1 phai lam TRUOC hoac lam cung luc, neu khong se bung lai loi xoa don.

### #7 [P0] LO HONG: khong co ho so thanh vien = duoc coi la BANG CHU (rut sach ngan quy)

**FILE:** Sources/Core/Src/KTongJX2.cpp:2286 va :2218-2225 (sJX2_HasRight)

Dong 2286 'BOOL bMaster = (pMe == NULL || pMe->btFigure == 0);' - nguoi vua vao bang chua co trong ban sao MEMBER_SYNC se co pMe==NULL nen qua het moi cong (DRAW_MONEY, KICK, SETANN, UPGRADE, SET_FIGURE). Sua thanh: if (!pMe) return 2; ngay sau dong 2283, roi 'BOOL bMaster = (pMe->btFigure == 0);'. Dong thoi sJX2_HasRight 2220-2221 doi 'if (!pMe) return FALSE;'. Ket hop voi viec so 8 (tru truoc khi gui) de dong han lo nhan doi tien.

### #8 [P0] Nhan doi tien ngan quy: kiem tra tren ban sao roi cong tien ngay, relay ket so am ve 0

**FILE:** Sources/Core/Src/KTongJX2.cpp:2498-2536 (DRAW_MONEY/PAY_MEMBER); Sources/MultiServer/S3Relay/KTongJX2Relay.cpp:810-819

Phia GS: truoc khi goi Player[..].Earn(), phai TRU NGAY tren ban sao cuc bo de lan bam ke tiep thay so du da giam - them ham SetMapField cho field 3/4 (tach money64 hien tai roi ghi lai) ngay sau khi sSendMoneyCmd. Phia relay: khoi 810-815 dang lam nNew<0 -> nNew=0 (TAO RA tien tu khong); doi thanh: neu m_wOp==defTONG_JX2_OP_ADD va JX2_GetMoney64() + m_nValue < 0 thi BO QUA lenh (khong ghi, ghi rTRACE) thay vi ket 0. Luu y relay don luong nen khong can khoa cho rieng cho nay.

### #9 [P0] CHAN TRIEN KHAI: TTongStruct/TMemberStruct doi kich thuoc - mo len DB cu la mat sach bang hoi

**FILE:** Sources/MultiServer/S3Relay/TONGDB.CPP:172-179 (SearchTong), :236-243, :322-325 (GetTongCount), :344-351 (GetTongList); KTongJX2Relay.cpp:684, :711, :741

TTongStruct tu ~2368 len 6860 byte, TMemberStruct tu ~76 len 404 byte; ca 7 cho tren deu doi dung sizeof moi (hoac sizeof-128) nen MOI ban ghi cu bi tu choi -> GetTongCount=0 -> relay chay voi 0 bang; nguoi choi tao lai bang trung ten se GHI DE vinh vien (KTongSet.cpp:77-85 chi tra bo nho, DBTable.cpp:133 put co=0 = ghi de). Viet ham CTongDB::UpgradeOldRecords() goi mot lan ngay sau Open() (Global.cpp:371): duyet cursor bang Tong, voi ban ghi co size==2368 thi cap phat TTongStruct moi, memset 0, memcpy dung 2368 byte dau, roi add() lai; lam tuong tu cho bang Member voi size==76. LUAT VAN HANH: chua chay xong ham nay thi CAM mo cong cho nguoi choi tao bang (byte cu van con nguyen tren dia vi TONGDB.CPP:59-60 hai lenh DeleteFile da bi chu thich - con cuu duoc).

### #10 [P0] Ban do ket noi khoa theo IP - hai GameServer cung may de len nhau, mot GS khong bao gio nhan echo

**FILE:** Sources/MultiServer/S3Relay/KTongJX2Relay.cpp:632-658 (CTongServer::BroadcastPackage, vong 640-646); TongServer.h:33-34

m_mapIp2Connect khoa bang IP, ma bin/Server/GameServer_cfg.ini:17-19 va GameServer1_cfg.ini:17-19 deu [Tong] Ip=127.0.0.1 Port=5005. Sua DUY NHAT duong quang ba: bo than ham BroadcastPackage, doi thanh goi lop co CNetServer::BroadPackage(pData, size) (NetServer.h:47, NetServer.cpp:266-282) - ham nay duyet m_mapId2Connect nen khong trung khoa. TUYET DOI KHONG doi khoa cua m_mapIp2Connect vi FindTongConnectByIP (TongServer.cpp:68-81) va FindPlayerByIpParam (:132-153) van can tra theo IP. Neu chua sua xong viec nay thi khong duoc bat GameServer thu hai.

### #11 [P1] Ve du 8 don + phan trang phia client (khong can dung toi server)

**FILE:** Sources/S3Client/Ui/UiCase/UiTongJX2.cpp:1369-1374, :784, :1641-1648, :1919-1932; UiTongJX2.h

Server DA gui du 8 don trong mot goi (KTongJX2.cpp:2202). Them bien 'int m_nRecStart;' vao UiTongJX2.h (khoi tao 0 o constructor va o SwitchPage). Doi 1369 thanh 'for (i = 0; i + m_nRecStart < (int)p->m_btApplyCount && i < 4; i++)' va lay p->m_sApply[m_nRecStart + i]. Doi 1641-1647 gan m_nSel = m_nRecStart + i - 1. Doi 1928-1931: m_RecPrev thi m_nRecStart = max(0, m_nRecStart-4), m_RecNext thi neu m_nRecStart+4 < btApplyCount thi +=4; sau do goi RenderRecruit() thay vi RequestPage. KHONG dung toi tran 8 phia server trong dot nay (muon >8 phai doi ca KTongJX2.h:41-43 lan defTONG_JX2_APPLY_MAX o CA HAI ban KProtocol.h - de dot sau).

### #12 [P1] KIEN TRUC: dua danh sach don xin len Relay (dung mo hinh KTongZhaoMuRelay cua ban goc)

**FILE:** Moi: Sources/MultiServer/S3Relay/KTongZhaoMuRelay.cpp/.h; sua Headers/KTongProtocol.h:49-55 (enumC2S) va :96-104 (enumS2C); Sources/MultiServer/S3Relay/TongConnect.cpp:14-49 (bang g_nTongPSSize) va switch :236-330; Sources/Core/Src/KTongJX2.cpp:391-438, :2202-2209, :2434-2484

Them 3 ma C2S o CUOI enum truoc enumC2S_TONG_NUM: JX2_APPLY_ADD, JX2_APPLY_GET, JX2_APPLY_ANSWER; va 1 ma S2C o CUOI truoc _NUM: JX2_APPLY_SYNC. BAT BUOC them dung 3 phan tu tuong ung vao CUOI mang g_nTongPSSize[] o TongConnect.cpp:14-49 theo DUNG THU TU enum (sai thu tu = doc nham do dai goi). Relay giu std::map<DWORD dwTongID, std::map<std::string, KJX2Apply>> trong bo nho (giong ban goc: goc cung KHONG luu don xuong DB, chi luu KZhaoMuInfo 135 byte), moi lan doi thi quang ba JX2_APPLY_SYNC toi moi GS. GS bo han 4 truong cuc bo trong KTongJX2.h:40-43, chuyen sang doc ban sao nhan tu relay. Ghi chu: khong sao chep 0xFF=TAT cua ban goc, ta dung 0=TAT - phai ghi vao ban giao de dot sau khong nhap nham du lieu goc. Relay don luong: dat ma xu ly trong JX2_ProcTongOp cung luong sock, KHONG cham tu JX2_TimerTick.

### #13 [P1] Phan quyen: bam 'Phan quyen' dang THU SACH quyen cua truong lao

**FILE:** Sources/S3Client/Ui/UiCase/UiTongJX2.cpp:1268-1284 (LoadChecksFromSel), :1676-1680 (lat kep), :1290-1315 (ApplyRights)

Dong 1270 'if (m_nPage != defTONG_JX2_PAGE_RIGHT || !m_bHasMember) return;' - m_nPage KHONG BAO GIO bang 2 (bang tab s_nTabPage khong co PAGE_RIGHT) nen o kiem khong bao gio nap quyen dang co; bam Phan quyen se gui DELRIGHT cho 10 quyen. Sua dieu kien thanh (m_nPage != defTONG_JX2_PAGE_MEMBER && m_nPage != defTONG_JX2_PAGE_RIGHT) || !m_bHasMember. THEM: dong 1678 lat co check lan hai trong khi KWndButton::OnLBtnDown (Sources/S3Client/Ui/Elem/WndButton.cpp:312-328) DA tu lat - phai XOA dong 1678 (chi giu doc trang thai), neu khong o kiem van khong tich duoc bang tay va sua LoadChecksFromSel la vo nghia.

### #14 [P1] Trang Phuong tho va Nhat ky: panel thanh vien luon trong

**FILE:** Sources/S3Client/Ui/UiCase/UiTongJX2.cpp:1219-1265 (RenderWorkshop, ClearRows o :1251), :1426-1450 (RenderRecord, ClearRows o :1429)

Sau khi da tach ClearMemberRows (viec so 1): doi ClearRows() o 1251 va 1429 thanh ClearRows() giu nguyen (vi hai trang nay dung m_Row cho noi dung trai) NHUNG them 'if (m_bHasMember) RenderMembers();' o CUOI ca hai ham, giong mau RenderFunUse:1421-1422. Ket hop viec so 6 de trang Nhat ky that su xin duoc MEMBER_SYNC.

### #15 [P1] Khoi phuc duong gop tien vao quy bang (COP_DONATE) va COP_DEGRADE

**FILE:** Sources/S3Client/Ui/UiCase/UiTongJX2.cpp:1006-1011 (Hide hang Act), :1487-1495 (than OnAction), :162-178 (bang s_sFunBtn), :860-879

Hang nut m_BtnAct da bi Hide + SetLabel("") nen COP_DONATE (10 van / 100 van) va COP_DEGRADE mat sach duong bam; bang 14 nut chuc nang s_sFunBtn khong co muc nao map sang hai lenh do -> nguoi choi KHONG CO cach nao gop tien cho bang. Sua: them 2 muc vao s_sFunBtn tro toi section ini co san [Fun_BtnStoreTongMoney] (ini:3387) va [Fun_BtnStoreBuildFund] (ini:3365) voi nAct moi, roi trong OnAction xu ly nAct do bang SendOp(defTONG_JX2_COP_DONATE, 0, 10 hoac 100, 0, NULL). Dong thoi bat lai NPC (viec 16) lam duong du phong.

### #16 [P1] Bat lai NPC bang hoi JX2 va them muc 'Xin gia nhap bang' vao menu NPC

**FILE:** E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/script/startgame.lua:49 (NGOAI kho git - phai sua tay tren may chay); D:/GAMEDEVNEW/tools/tongjx2_menu.lua

Dong 49 dang la '--JX2Tong_AddNpc()  --NPC bang hoi JX2 (TAT theo yeu cau...)'. Bo 2 dau '--' de bat lai 18 NPC (11 o map 53 + 7 Te Tuu). Nhat ky ScripNpcDialog.log ngay 13/08 chung minh cum NPC nay DA chay dung truoc khi bi tat. Trong tongjx2_menu.lua them muc goi TONG_ApplyJoin(nTongID) lam duong du phong khi cua so client hong. LUU Y: file nam ngoai repo nen khong hien trong git diff - phai ghi vao ban giao va chep tay khi trien khai.

### #17 [P1] Bao tri ngay khong tinh chi phi duy tri (field 16) - bao tri chay ma khong tru gi, tro cap luon 0

**FILE:** Sources/MultiServer/S3Relay/KTongJX2Relay.cpp:1375-1418 (JX2_DailyMaintain), :921-977 (sJX2_LoadLevelTable)

Dong 1377 chi DOC field 16, khong noi nao GHI (MAINTAIN_R cua tong.lua khong bao gio chay). Sua theo dung thu tu ban goc: (1) TRU truoc bang field 16 hien co, (2) roi moi TINH LAI field 16 cho ngay hom sau = tong chi phi duy tri cac tac phuong dang mo theo cap + soTV*10. Muon tinh duoc phai them bo doc settings/tong/workshop/workshop_level_data.txt vao sJX2_LoadLevelTable (bang MAINTAIN_FUND la SO THUC 0.75..3 theo cap) - file da co san trong bin/server va bin/multiserver. Dong thoi ghi field 44 (PauseState) khi WarBuildFund < StandFund nhu tong.lua:365-378.

### #18 [P1] Bang so TONGTSK_ trong C++ LECH voi tong_header.lua - se ghi de nhau khi bat script

**FILE:** Headers/KTongProtocol.h:938-943

defTONGTSK_STUNT_ID=1101 nhung script dung 1011; STUNT_ENABLED=1102 vs 1014; LAST_WM_DAY=1104 vs 1050. Trong khi 1101/1102/1104 ben script lai la WEEK_BFCONSUME/WEEK_WFADD/WEEK_WSCONSUME. Sua: STUNT_ID -> 1011, STUNT_ENABLED -> 1014, LAST_WM_DAY -> 1050. Rieng defTONGTSK_LAST_M_DAY KHONG co doi ung ben script - chon khoa trong trong dai 1108-1199 (script nhay tu 1107 len 1205). defTONGTSK_WEEK_WFCONSUME=1103 trung khop san nhung phai chot AI chay bao tri (C++ HAY script) de khong cong doi. Sua luon chu thich sai o dong 938.

### #19 [P1] Cong hien tuan cua thanh vien (khoa 9 va 11) luon 0 - client hien 0, don tuan don so 0

**FILE:** Sources/Core/Src/KTongJX2.cpp:2717-2737 (Add/SetWeeklyOffer, AddWeekGoalOffer), :2333-2334 (COP_DONATE); Sources/MultiServer/S3Relay/KTongJX2Relay.cpp:1351-1366

Cong hien tuan dang duoc cong vao SaveVal 2802/2804 CUA NHAN VAT (co kep tran 22400 that), hoan toan tach roi member-field 9/11 ma relay dong bo va client hien thi. Sua: trong LuaTONGM_AddWeeklyOffer / AddWeekGoalOffer, sau khi sAddSaveVal thi bac cau them sSendMemberFieldCmd(khoa 11 / khoa 9, OP_ADD). VA cho JX2_WeeklyMaintain don luon SaveVal 2802/2804 ve 0 (hien chi don field bang + member-field), neu khong nguoi choi cham tran 22400 mot lan la chet vinh vien khong nhan duoc cong hien nua.

### #20 [P1] Truc xuat thanh vien khong phat tien va khong bi chan / khong gioi han tan suat

**FILE:** Sources/MultiServer/S3Relay/KTongJX2Relay.cpp:1036-1108 (JX2_KickByNameID)

Ban goc (tong_mix.lua:386-407): nguoi bi duoi mat ceil(nOffer/20) cong hien; bang mat floor(nOffer*0.6) khoi kien thiet du tru va floor(nOffer*0.4) khoi chien bi; va TU CHOI DUOI neu bang khong du quy (return 0). Ngoai ra MEMBER_KICK_G_1 co bo dem MAX_KICK_COUNT=20 lan/ngay. Hien C++ chi go ten khoi mang + xoa ban ghi DB. Bo sung day du 3 phan: cong chan quy, phat tien, bo dem 20 lan/ngay (dung 2 khoa task trong).

### #21 [P1] Duong bang hoi kieu cu khong phat JX2 sync - ban sao lech toi 750 giay

**FILE:** Sources/MultiServer/S3Relay/TongConnect.cpp:275-316 (CREATE), :317-359 (ADD_MEMBER), :388-397 (INSTATE), :398-407 (KICK), :408-417 (LEAVE), :418-475 (CHANGE_MASTER), :477-482 (ACCEPT_MASTER), :507-575 (CHANGE_TITLE/ACCEPT_TITLE)

Toan bo KTongSet.cpp khong co chu 'JX2' (grep 0). Sau moi nhanh tren, tim lai pTong (g_cTongSet.FindTong theo ten) roi goi sJX2_BroadcastTong(pTong) truoc break. Viec nay dong cua so lech tao ra lo 'pMe==NULL = bang chu' (viec 7) va tinh trang nguoi vua bi duoi van con quyen trong ban sao. Chu y ACCEPT_MASTER va ACCEPT_TITLE cung phai lam, khong chi 6 nhanh.

### #22 [P1] Menu Khuynh huong / Hoat dong trang Chieu mo dang la nut xoay so 0..9

**FILE:** Sources/S3Client/Ui/UiCase/UiTongJX2.cpp:382-384, :1358-1366, :1872-1892; clientui_jx2/UiTongJX2.ini:4342-4349 va :4361-4373

Blueprint co [Rec_QingXiangMenu] 6 muc chu (Ranh moi choi / Thich PK / Tranh ba / Da thu vo tranh / Tinh nang PVE / Ket giao) va [Rec_HuoDongMenu] 11 muc chu (Tong Kim, Vuot ai, Danh Boss, Cong Thanh, Phong Lang Do, Nhiem vu Da Tau, Viem De, Tin Su, Moc Nhan, Vo lam lien dau, Thien Tru Mat Canh) - hai section nay la DU LIEU CHET, khong dong C++ nao doc khoa MenuItemCount/MenuText. Sua toi thieu (khong can lop menu moi): dat 2 mang chuoi tinh trong .cpp, doi %10 thanh %6 va %11 tai 1358/1363, va SetLabel bang chuoi thay vi so. Server khong chan bien (KTongJX2.cpp:2425-2429 ghi thang (nParam1>>k)&15) nen phai kep bien phia client.

### #23 [P1] Trang Tin tuc: hai bo Fun_ va Fun2_ hien dong thoi, ve chong len nhau; o 'Bang chu' do vao nhan 'Lien minh'

**FILE:** Sources/S3Client/Ui/UiCase/UiTongJX2.cpp:845-856 (Show ca hai bo), :1396-1400; clientui_jx2/UiTongJX2.ini:2249-2290 vs :5419-5437, :2314-2355 vs :5457-5494

Fun2_ von la BO CUC THAY THE (trung toa do 20/92 va 20/112 voi Fun_) nhung dong 848-856 Show ca 15 m_FunTxt lan 13 m_Fun2 khong phan biet m_nFunMode. Sua: Show m_FunTxt khi m_nFunMode==0, Show m_Fun2 khi m_nFunMode==1 (hoac nguoc lai sau khi doi chieu anh ban goc). Rieng dong 1397 dat p->m_szMaster vao m_FunTxt[4] = section Fun_TxtTongUnion (nhan 'Lien minh') - doi Label cua [Fun_TitleTongUnion] trong ini thanh 'Bang chu', KHONG do du lieu vao Fun2_TxtLeague vi o do bi che.

### #24 [P1] relay_config.ini thieu han muc [tongjx2] - moi tham so kinh te chay bang mac dinh hard-code

**FILE:** E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/multiserver/relay_config.ini (NGOAI kho git); doi chieu Sources/MultiServer/S3Relay/KTongJX2Relay.cpp:1424-1435, :1652-1655

Them muc [tongjx2] voi day du 8 khoa: MoneyLimit=1000000, NormalMoney=5000, NormalExp=120, NormalMemberLimit=100, NormalMemberLimitMoney=50, LimitMoney=0, LimitExp=0, TimeLong=750. Cac gia tri nay doc mot lan (static) nen sua file luc chay khong co tac dung cho toi khi restart relay - ghi ro vao ban giao.

### #25 [P1] Relay KHONG don luong that - JX2_TimerTick va goi tin chay o hai luong, sua chung m_mapJX2Member

**FILE:** Sources/MultiServer/S3Relay/KTongJX2Relay.cpp:9 (chu thich sai), :1649 (chu thich sai), :1650-1704 (TimerTick tu WM_TIMER = luong GUI); SockThread.cpp:62-63 (luong sock rieng)

Luong sock goi JX2_ProcTongField/ProcMemberField/ProcTongOp va JX2_KickByNameID (erase node) trong khi luong GUI dang duyet m_mapJX2Member trong JX2_WeeklyMaintain -> use-after-free. Sua theo mau da co san trong chinh du an: FriendMgr.cpp dung AUTOLOCKWRITE(m_lockFriend) cho dung mo hinh nay. Them mot CLockMRSW vao CTongSet, boc AUTOLOCKWRITE o dau MOI ham JX2_Proc* va o dau JX2_TimerTick. Sua luon 2 chu thich sai o dong 9 va 1649 (dang khang dinh 'don luong - an toan').

### #26 [P1] Bao tri tuan / phat theo nhom ghi Berkeley DB tung nguoi - moi thanh vien = 1 lan sync + checkpoint

**FILE:** Sources/MultiServer/S3Relay/KTongJX2Relay.cpp:1351-1366 (WeeklyMaintain), :1516-1524 (Distribute), :1691-1700 (TimerTick dat bChanged vo dieu kien); DBTable.cpp:94-109, :133-140

(a) Them ZDBTable::addNoCommit() (copy add() bo dong goi commit()) va goi commit() DUY NHAT mot lan sau vong lap - KHONG duoc tach commit khoi add() toan cuc vi anh huong moi noi goi. (b) Go giao dich rong o DBTable.cpp:98-100 (txn_begin roi commit ma khong co thao tac nao). (c) Cho JX2_MoneyToExpTick tra BOOL va chi dat bChanged khi that su doi (hien 1691-1700 dat TRUE cho MOI bang moi 750 giay -> ghi DB + quang ba toan bo roster du khong doi gi). (d) JX2_Distribute chay ngay khi bang chu bam nut phat theo nhom - uu tien gom ghi o day truoc.

### #27 [P2] Chuc vu An si (figure 4) khong the ton tai - ca cum thoai an chet theo

**FILE:** Sources/MultiServer/S3Relay/KTongJX2Relay.cpp:1118 (SetFigureByNameID chan 1..3), :345-425 (CollectMembers chi phat 0..3), :1375-1392 (DailyMaintain khong tru an si); Sources/Core/Src/KTongJX2.cpp:2410 (COP_SET_FIGURE chi 1..3)

Client DA san sang hien thi (UiTongJX2.cpp:79-82 s_szFigure[5] co 'An si'). Sua: cho phep nNewFigure==4 o relay va o COP_SET_FIGURE; CollectMembers phat btFigure 4; them 2 ma COP moi o CUOI bang defTONG_JX2_COP_* (truoc so dem) tren CA HAI ban KProtocol.h: COP_RETIRE (tu thoai an) va COP_FORCE_RETIRE (lenh thoai an, kiem quyen 1902), kem tran TONGMEMBER_RETIRE_MAX_RATE 50%; noi voi khoa 16 RetireDate; va tru an si khoi m_nMemberNum khi tinh phi duy tri (theo controlhelp_task_def.txt:14).

### #28 [P2] Tuyet ky bang: tru 6000 hard-code HANG NGAY (ban goc tru hang TUAN) va khong cap SkillID

**FILE:** Sources/MultiServer/S3Relay/KTongJX2Relay.cpp:1397-1407, :1598-1601; serverscript_jx2/settings_tong/tongstunt_setting.txt

(a) Chuyen khoan tru tu JX2_DailyMaintain sang nhanh TUAN trong JX2_WeeklyMaintain (ban goc goi Maintain_Stunt trong khoi bao tri tuan, tong.lua:199 + :270-276) - hien dang lech gap 7 lan chi phi. (b) Doc Consume tu tongstunt_setting.txt thay vi hard-code 6000. (c) Chan StuntID chi trong {1,2,3,4,5,7} (bang chi co 6 dong, KHONG co StuntID 6) va chan cap bang >= 4 (tong.lua:181); hien client gui thang m_nSel va relay:1599 nhan bat ky gia tri nao. (d) Viec cap SkillID 739-743/745 chi lam duoc khi bat lai duong Lua - de sau, vi con thieu RemoveNpcSkillState va GetCityOwner chua dang ky trong ScriptFuns.cpp.

### #29 [P2] Don rac 3 the he giao dien + tai lieu/chu thich lac hau

**FILE:** Sources/S3Client/Ui/UiCase/UiTongJX2.h:3-4, :32-33, :67; Sources/S3Client/Ui/UiCase/UiTongJX2.cpp:49-52, :55-60, :438-442, :1025, :1605-1612, :986-987; clientui_jx2/UiTongJX2.ini:300-620 ([Chk0..11]), :671-791 ([Act0..5])

Go han m_Chk[12] (da tuyen bo bo, van khai bao), go 12 section [Chk*] va 6 section [Act*] khoi ini sau khi da chuyen COP_DONATE sang nut chuc nang (viec 15), go hai bang chet s_szTabName va s_szRightName, go m_BtnList neu khong dung. Sua chu thich lac: UiTongJX2.h:3-4 con ghi '5 trang: thong tin/thanh vien/quyen han/tac phuong/thong bao' (thuc te Tin tuc/Chieu mo/Thanh vien/Tac phuong/Nhat ky); :32-33 ghi 'chua vao bang -> mo cua so cu' (thuc te da mo trang Danh sach bang); :1025 ghi 'bo nut blueprint m_WsBtn' trong khi m_WsBtn van hoat dong. Rac chua don la ly do moi dot test chu game lai bao 'loi moi' cho manh the he cu.

### #30 [P2] Chot duong trien khai: 2 cay lib song song lech byte + 6 goc dich khong co script dong bo

**FILE:** serverscript_jx2/lib/ (basic.lua, say.lua, string.lua - ban HONG, con khoi chu thich /* */) vs serverscript_jx2/libfix/ (ban da sua); E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/CAPNHAT_SERVER_JX2.bat

Ban dang chay bin/server/scriptjx2/lib/ trung md5 voi libfix, nhung duong dan phan chieu (lib/) van giu ban hong - bat ky lan chep lai nao cung lam song lai loi bien dich Lua ('end expected'). Sua: chep 3 file libfix de len lib/ roi XOA thu muc libfix (mot nguon su that duy nhat). Bo sung vao CAPNHAT_SERVER_JX2.bat cac lenh chep: clientui_jx2/UiTongJX2.ini -> bin/client/Ui/Ui3 VA J:/CayChay/pakgame/volamngaothe/PATCHFULL_NGAOTHE_MK_123456/Ui/Ui3; serverscript_jx2/tong_vn -> bin/server/scriptjx2/tong_vn; serverscript_jx2/settings_tong -> bin/server/settings/tong (2 file dang lech: bingjia_level_data.txt, workshops.txt); serverscript_jx2/boot/tongjx2npc.lua -> bin/server/script/startgame/. Da tung mat tron mot vong test vi user chay client o PATCHFULL chu khong phai bin/client.
