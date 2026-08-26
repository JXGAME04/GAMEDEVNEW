# -*- coding: ascii -*-
# DOT 10 (26/08 toi) - chu duyet muc 1+2+3+5, uu tien KHONG anh huong trai nghiem.
# Moi mieng deu va DUNG CHO SINH LOI (khong vat nguong, khong che trieu chung).
import io, sys
ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
CRLF = "\r\n"
def L(*a): return CRLF.join(a)
ok = True

def ap(fname, old, new, note):
    global ok
    p = ROOT + "\\" + fname
    with io.open(p, "r", encoding="latin-1", newline="") as f: c = f.read()
    hb = sum(1 for ch in c if ord(ch) > 127)
    n = c.count(old)
    if n != 1:
        print("LOI [%s] %s: neo khop %d lan (can 1)" % (note, fname, n)); ok = False; return
    c = c.replace(old, new)
    if sum(1 for ch in c if ord(ch) > 127) != hb:
        print("LOI [%s] %s: high-byte doi" % (note, fname)); ok = False; return
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(c)
    print("OK [%s] %s" % (note, fname))

# ==================== A1: tach "DA TOI DICH" khoi "BI CHAN" ====================
ap("KNpcFindPath.cpp",
L("\tif ( !CheckDistance(nXpos >> 10, nYpos >> 10, nDestX, nDestY, nMoveSpeed))",
  "\t{",
  "\t\tm_nFindTimer = 0;",
  "\t\tm_nFindState = 0;",
  "\t\tm_nFindTimes = 0;",
  "\t\treturn 0;",
  "\t}"),
L("\tif ( !CheckDistance(nXpos >> 10, nYpos >> 10, nDestX, nDestY, nMoveSpeed))",
  "\t{",
  "\t\tm_nFindTimer = 0;",
  "\t\tm_nFindState = 0;",
  "\t\tm_nFindTimes = 0;",
  "\t\t// [FIX-1 26/08] Cho nay nghia la DA TOI DICH (khoang cach con nho hon MOT BUOC",
  "\t\t// chan), nhung ham nay con tra 0 o hai cho khac khi THAT SU BI CHAN (:157, :166).",
  "\t\t// Nguoi goi duy nhat (KNpc.cpp ServeMove) gop hai nghia lam mot => NPC toi dich bi",
  "\t\t// coi la dang bi chan: m_nNeedFixPos++ vo han, m_Doing giu do_run, ban sao client",
  "\t\t// dung im va bi goi dong bo KEO tung nac (do that: 158/160 dong ret=0 la da toi",
  "\t\t// dich; 68% NPC 'dang chay' khong nhuc nhich giua hai goi sync).",
  "\t\t// Tra ma RIENG de nguoi goi phan biet. Ban _SERVER gop moi ma != 1 vao DoStand()",
  "\t\t// nen hanh vi may chu KHONG DOI.",
  "\t\treturn 2;",
  "\t}"), "A1 GetDir tra ma rieng")

# ==================== A2: cho goi xu ly ma 2 = dung THAT ====================
ap("KNpc.cpp",
L("\telse if (nRet == 0)",
  "\t{",
  "\t\t// Bi chan. Ban goc JX2 (0x00598080) KHONG dung nhan vat lai o day: no gio co"),
L("\telse if (nRet == 2)",
  "\t{",
  "\t\t// [FIX-1 26/08] DA TOI DICH (khong phai bi chan): dung that - dat m_Doing =",
  "\t\t// do_stand va xoa bo dem 'toi dang sai'. Truoc day roi vao nhanh duoi va bi coi",
  "\t\t// la bi chan nen NPC ket trang thai chay + bo dem tang mai. Ban _SERVER (:4674)",
  "\t\t// von da lam dung dieu nay.",
  "\t\tDoStand();",
  "\t\treturn;",
  "\t}",
  "\telse if (nRet == 0)",
  "\t{",
  "\t\t// Bi chan. Ban goc JX2 (0x00598080) KHONG dung nhan vat lai o day: no gio co"), "A2 nhanh nRet==2")

# ==================== A2b: DoStand xoa bo dem ====================
ap("KNpc.cpp",
L("\t\tFixPos(); //add by Fong Ki\xd2u from KT",
  "\t\tm_Doing = do_stand;",
  "\t\tm_Frames.nCurrentFrame = 0;"),
L("\t\tFixPos(); //add by Fong Ki\xd2u from KT",
  "\t\tm_Doing = do_stand;",
  "\t\t// [FIX-1 26/08] Dung han thi khong con 'dang bi chan' nua - xoa bo dem, neu khong",
  "\t\t// no giu quyen ghi de toa do cua goi dong bo mai mai.",
  "\t\tm_nNeedFixPos = 0;",
  "\t\tm_Frames.nCurrentFrame = 0;"), "A2b DoStand xoa co")

# ==================== A3: giao DICH THAT thay vi keo tung nac ====================
OLD_A3 = "\t\tif (Npc[nIdx].m_nNeedFixPos > 0 && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)"
NEW_A3 = L(
  "#ifndef _SERVER",
  "\t\t// [FIX-2 26/08] GOC cua 'truot toi - lui': ban sao client cua NPC KHONG BIET DICH.",
  "\t\t// NPC vua vao tam duoc giao dich = CHINH CHO no dang dung (:1979) nen 'toi dich'",
  "\t\t// ngay, dung im, roi moi goi dong bo KEO no mot nac (do that: 30 cu ghi de/giay,",
  "\t\t// 23% dich chuyen qua nua o; lop noi suy bien moi nac thanh mot cu truot muot).",
  "\t\t// Sua dung goc: may chu bao NPC nay DANG DI CHUYEN va dang o cho khac => giao cho",
  "\t\t// ban sao client DUNG DICH DO de no TU CHAY toi bang toc do cua no.",
  "\t\t// Chi kich hoat khi da lech >= 1 o (NPC chay dung thi lech p50 chi 12 mps) va < 12 o",
  "\t\t// (xa hon la dich chuyen that - de nguyen cho nhanh nan ben duoi lo).",
  "\t\tif (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex &&",
  "\t\t\t(NpcSync->Doing == do_run || NpcSync->Doing == do_walk) &&",
  "\t\t\tNpc[nIdx].m_RegionIndex >= 0 && Npc[nIdx].m_RegionIndex == nRegion)",
  "\t\t{",
  "\t\t\tint nS9X = 0, nS9Y = 0;",
  "\t\t\tNpc[nIdx].GetMpsPos(&nS9X, &nS9Y);",
  "\t\t\tint nS9D = g_GetDistance(nS9X, nS9Y, NpcSync->MapX, NpcSync->MapY);",
  "\t\t\tif (nS9D >= 32 && nS9D < 32 * 12)",
  "\t\t\t{",
  "\t\t\t\tAUTOLOG_EVERY(1000, \"[S9-DICH] npc=%u idx=%d doing=%d lech=%d -> giao dich (%d,%d) t=%u\", NpcSync->ID, nIdx, (int)NpcSync->Doing, nS9D, NpcSync->MapX, NpcSync->MapY, SubWorld[0].m_dwCurrentTime);",
  "\t\t\t\tNpc[nIdx].SendCommand((NPCCMD)NpcSync->Doing, NpcSync->MapX, NpcSync->MapY);",
  "\t\t\t}",
  "\t\t}",
  "#endif",
  OLD_A3)
ap("KProtocolProcess.cpp", OLD_A3, NEW_A3, "A3 giao dich that")

# ==================== B1: nho vi tri MAY CHU cua chinh minh ====================
ap("KProtocolProcess.cpp",
L("int S6_UsedSlots()",
  "{"),
L("// [FIX-3 26/08] Vi tri MAY CHU dang tin cua CHINH NHAN VAT. Client co y KHONG nan toa do",
  "// ban than (KProtocolProcess.cpp :2104 va :2135 deu loai tru chinh minh) nen hai ben lech",
  "// (do that luc bi tu choi don danh: p50 66 mps = 2 o). May danh cua auto lai do bang toa do",
  "// CLIENT roi ban, con may chu phan quyet bang toa do CUA NO => don bi tu choi IM LANG",
  "// (557 lan / 21,9 phut) = 'danh vao khong khi'. Luu lai de may danh do bang CA HAI goc nhin.",
  "int g_nS9SvMeX = 0;",
  "int g_nS9SvMeY = 0;",
  "",
  "int S6_UsedSlots()",
  "{"), "B1 bien vi tri server")

ap("KProtocolProcess.cpp",
L("\tSubWorld[0].Mps2Map(pSync->m_dwMapX, pSync->m_dwMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);",
  "\tAUTOLOG_EVERY(500, \"SYNCME-DRIFT me idx=%d"),
L("\tSubWorld[0].Mps2Map(pSync->m_dwMapX, pSync->m_dwMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);",
  "#ifndef _SERVER",
  "\t// [FIX-3 26/08] ghi lai vi tri may chu tin la cua minh (KHONG dong vao toa do client).",
  "\tg_nS9SvMeX = pSync->m_dwMapX;",
  "\tg_nS9SvMeY = pSync->m_dwMapY;",
  "#endif",
  "\tAUTOLOG_EVERY(500, \"SYNCME-DRIFT me idx=%d"), "B1b ghi vi tri server")

# ==================== B2: may danh do bang CA HAI goc nhin ====================
# nhanh danh thuong (tab Chien dau)
ap("CoreShell.cpp",
L("\t\t\t\t\tNpc[nTGNpcIdx].GetMpsPos(&x, &y);",
  "\t\t\t\t\tint nDist = g_GetDistance(nX, nY, x, y);",
  "\t\t\t\t\tAUTOLOG_EVERY(1000, \"[FIGHT-DIST]"),
L("\t\t\t\t\tNpc[nTGNpcIdx].GetMpsPos(&x, &y);",
  "\t\t\t\t\tint nDist = g_GetDistance(nX, nY, x, y);",
  "\t\t\t\t\t// [FIX-3 26/08] May chu phan quyet don danh bang toa do CUA NO (KSkills.cpp:359",
  "\t\t\t\t\t// tu choi khi dist > radius + 20) va tu choi IM LANG - auto khong he biet. Toa do",
  "\t\t\t\t\t// ban than hai ben lech p50 66 mps luc bi tu choi. Nen lay KHOANG CACH XAU HON",
  "\t\t\t\t\t// trong hai goc nhin: chi ban khi CA HAI deu thay trong tam => het don bi tu choi,",
  "\t\t\t\t\t// va khong phai vat nguong bang mot con so mo ho.",
  "\t\t\t\t\t{",
  "\t\t\t\t\t\textern int g_nS9SvMeX, g_nS9SvMeY;",
  "\t\t\t\t\t\tif (g_nS9SvMeX || g_nS9SvMeY)",
  "\t\t\t\t\t\t{",
  "\t\t\t\t\t\t\tint nS9DSv = g_GetDistance(g_nS9SvMeX, g_nS9SvMeY, x, y);",
  "\t\t\t\t\t\t\tAUTOLOG_EVERY(1000, \"[S9-TAM] tgID=%u dcli=%d dsv=%d lechme=%d\", Npc[nTGNpcIdx].m_dwID, nDist, nS9DSv, g_GetDistance(nX, nY, g_nS9SvMeX, g_nS9SvMeY));",
  "\t\t\t\t\t\t\tif (nS9DSv > nDist)",
  "\t\t\t\t\t\t\t\tnDist = nS9DSv;",
  "\t\t\t\t\t\t}",
  "\t\t\t\t\t}",
  "\t\t\t\t\tAUTOLOG_EVERY(1000, \"[FIGHT-DIST]"), "B2a may danh thuong")

# nhanh PK (TK/LD/BC)
ap("CoreShell.cpp",
L("\t\t\t\t\tNpc[nTGNpcIdx].GetMpsPos(&x, &y);",
  "\t\t\t\t\tint nDist = g_GetDistance(nX, nY, x, y);",
  "\t\t\t\t\tint nMainSkill = Player[nPlayerIdx].GetLeftSkill();"),
L("\t\t\t\t\tNpc[nTGNpcIdx].GetMpsPos(&x, &y);",
  "\t\t\t\t\tint nDist = g_GetDistance(nX, nY, x, y);",
  "\t\t\t\t\t// [FIX-3 26/08] xem chu thich cung ten o may danh thuong: lay khoang cach XAU",
  "\t\t\t\t\t// HON giua goc nhin client va goc nhin may chu de khong ban vao vung bi tu choi.",
  "\t\t\t\t\t{",
  "\t\t\t\t\t\textern int g_nS9SvMeX, g_nS9SvMeY;",
  "\t\t\t\t\t\tif (g_nS9SvMeX || g_nS9SvMeY)",
  "\t\t\t\t\t\t{",
  "\t\t\t\t\t\t\tint nS9DSv = g_GetDistance(g_nS9SvMeX, g_nS9SvMeY, x, y);",
  "\t\t\t\t\t\t\tAUTOLOG_EVERY(1000, \"[S9-TAM-PK] tgID=%u dcli=%d dsv=%d lechme=%d\", Npc[nTGNpcIdx].m_dwID, nDist, nS9DSv, g_GetDistance(nX, nY, g_nS9SvMeX, g_nS9SvMeY));",
  "\t\t\t\t\t\t\tif (nS9DSv > nDist)",
  "\t\t\t\t\t\t\t\tnDist = nS9DSv;",
  "\t\t\t\t\t\t}",
  "\t\t\t\t\t}",
  "\t\t\t\t\tint nMainSkill = Player[nPlayerIdx].GetLeftSkill();"), "B2b may danh PK")

# ==================== C: 3 nhan bao dong gia -> dat DUNG trong cua chan ====================
ap("KSkills.cpp",
L("#ifndef _SERVER\t\t\t\t",
  "\t\t\t\tif (distance > GetAttackRadius()*0.8)",
  "#endif",
  "\t\t\t\t{",
  "\t\t\t\tAUTOLOG_EVERY(1000, \"[SKILL-REFUSE-FAR] sk=%d lv=%d launcher=%d tgt=%d d=%d radius=%d client08=%d -> tra ve -1 (bot se chay lai gan)\", (int)m_nId, (int)m_ulLevel, nLauncher, nParam2, distance, GetAttackRadius(), (int)(GetAttackRadius() * 0.8));",
  "\t\t\t\t}",
  "\t\t\t\tif (distance > GetAttackRadius() + 20)",
  "\t\t\t\t\treturn -1;"),
L("\t\t\t\t// [FIX-4 26/08] Nhan nay TRUOC DAY DAT NGOAI cua chan: dieu kien 0.8R nam trong",
  "\t\t\t\t// #ifndef _SERVER nen ban may chu in VO DIEU KIEN, moi don danh binh thuong cung",
  "\t\t\t\t// in ra 'tra ve -1'. Da lam ca doi chan doan tin nham la don bi tu choi (thuc te",
  "\t\t\t\t// chi 4 dong la tu choi that). Dat vao DUNG TRONG cua chan.",
  "\t\t\t\tif (distance > GetAttackRadius() + 20)",
  "\t\t\t\t{",
  "\t\t\t\t\tAUTOLOG_EVERY(1000, \"[SKILL-REFUSE-FAR] sk=%d lv=%d launcher=%d tgt=%d d=%d radius=%d limit=%d -> TU CHOI THAT\", (int)m_nId, (int)m_ulLevel, nLauncher, nParam2, distance, GetAttackRadius(), GetAttackRadius() + 20);",
  "\t\t\t\t\treturn -1;",
  "\t\t\t\t}"), "C1 SKILL-REFUSE-FAR")

ap("KMissleSet.cpp",
L("\tint nFreeIndex = FindFree();",
  "\tAUTOLOG_EVERY(2000, \"[MSL-SET-FULL] khong con khe dan: MAX_MISSLE=%d dang dung=%d subworld=%d mps(%d,%d)\", MAX_MISSLE, GetCount(), nSubWorldId, nPX, nPY);",
  "\tif (nFreeIndex <= 0) ",
  "\t{"),
L("\tint nFreeIndex = FindFree();",
  "\t// [FIX-4 26/08] Nhan nay truoc day dat TRUOC cua chan nen in ra o MOI lan Add THANH CONG",
  "\t// (co dong 'khong con khe' voi dang dung=0/20000). Dat vao trong than if.",
  "\tif (nFreeIndex <= 0) ",
  "\t{",
  "\t\tAUTOLOG_EVERY(2000, \"[MSL-SET-FULL] HET KHE DAN THAT: MAX_MISSLE=%d dang dung=%d subworld=%d mps(%d,%d)\", MAX_MISSLE, GetCount(), nSubWorldId, nPX, nPY);"), "C2 MSL-SET-FULL")

ap("KSkills.cpp",
L("\t\t\t\t\tAUTOLOG_EVERY(1000, \"[MISSLE-POOL-FULL] sk=%d launcher=%d subworld=%d des(%d,%d) i=%d j=%d Add tra ve %d -> BO QUA vien dan\", (int)m_nId, nLauncher, nSubWorldId, nDesSubX, nDesSubY, i, j, nMissleIndex);",
  "\t\t\t\t\t",
  "\t\t\t\t\tAUTOLOG_EVERY(1000, \"[E3_MISSLE_ADDFAIL] skill=%d launcher=%d subworld=%d pos=(%d,%d) i=%d j=%d childnum=%d\", (int)m_nId, nLauncher, nSubWorldId, nDesSubX, nDesSubY, i, j, m_nChildSkillNum);",
  "\t\t\t\t\tif (nMissleIndex < 0)\tcontinue;"),
L("\t\t\t\t\t// [FIX-4 26/08] Hai nhan nay truoc day dat TRUOC cua chan nen in ra ca khi",
  "\t\t\t\t\t// Add THANH CONG (co dong 'Add tra ve 1 -> BO QUA'). Dat vao trong than if.",
  "\t\t\t\t\tif (nMissleIndex < 0)",
  "\t\t\t\t\t{",
  "\t\t\t\t\t\tAUTOLOG_EVERY(1000, \"[MISSLE-POOL-FULL] sk=%d launcher=%d subworld=%d des(%d,%d) i=%d j=%d Add tra ve %d -> BO QUA vien dan THAT\", (int)m_nId, nLauncher, nSubWorldId, nDesSubX, nDesSubY, i, j, nMissleIndex);",
  "\t\t\t\t\t\tcontinue;",
  "\t\t\t\t\t}"), "C3 MISSLE-POOL-FULL")

# ==================== D: nhan LOP VE + va 1 dong KNpcRes::Init ====================
ap("KNpc.cpp",
L("\tm_DataRes.SetAction(m_ClientDoing);",
  "\tm_DataRes.SetRideHorse(m_bRideHorse);",
  "\tm_DataRes.SetArmor(m_ArmorType, m_MantleType);"),
L("#ifndef _SERVER",
  "\t// [S9-VE 26/08] Nhan DUY NHAT nhin thay LOP VE. Toan bo he log truoc day chi chup trang",
  "\t// thai logic (m_Doing/m_ClientDoing) nen khi nguoi choi bao 'nam bep duoi dat' thi khong",
  "\t// the phan biet loi o KNpc, o KNpcRes hay o Represent. Chi ghi cho CHINH NHAN VAT.",
  "\t// Doc: resdoing=8 => loi o lop ve; cdoing=8 => loi o KNpc; ca ba deu 1 ma van nam => Represent.",
  "\tif (m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)",
  "\t{",
  "\t\tstatic DWORD s_uS9VeT = 0;",
  "\t\tstatic int s_nS9VeCu = -999;",
  "\t\tDWORD uS9Now = timeGetTime();",
  "\t\tif (s_nS9VeCu != (int)m_ClientDoing || (DWORD)(uS9Now - s_uS9VeT) >= 2000)",
  "\t\t{",
  "\t\t\ts_uS9VeT = uS9Now;",
  "\t\t\ts_nS9VeCu = (int)m_ClientDoing;",
  "\t\t\tAUTOLOG(\"[S9-VE] doing=%d cdoing=%d resdoing=%d resaction=%d frame=%d/%d reg=%d t=%u\", (int)m_Doing, (int)m_ClientDoing, m_DataRes.m_nDoing, m_DataRes.m_nAction, m_Frames.nCurrentFrame, m_Frames.nTotalFrame, m_RegionIndex, SubWorld[0].m_dwCurrentTime);",
  "\t\t}",
  "\t}",
  "#endif",
  "\tm_DataRes.SetAction(m_ClientDoing);",
  "\tm_DataRes.SetRideHorse(m_bRideHorse);",
  "\tm_DataRes.SetArmor(m_ArmorType, m_MantleType);"), "D1 nhan lop ve")

ap("KNpcRes.cpp",
L("\tm_nNpcKind = m_pcResNode->GetNpcKind();",
  "\tm_nAction = 0;"),
L("\tm_nNpcKind = m_pcResNode->GetNpcKind();",
  "\t// [FIX-5 26/08] Init dat lai m_nAction nhung QUEN m_nDoing, trong khi SetAction mo dau",
  "\t// bang 'if (m_nDoing == nDoing) return TRUE;'. Khe NPC duoc tai su dung rat thuong xuyen",
  "\t// (do that 7,5 lan/giay) va chinh nhan vat cung bi go/nap lai o moi lan doi vung ban do;",
  "\t// neu trang thai cu trung voi trang thai moi thi bo anh MOI KHONG BAO GIO duoc nap =>",
  "\t// ve bang hoat anh cua chu cu (ung vien cho trieu chung 'nam bep duoi dat').",
  "\t// -1 khong phai tu the hop le nao nen lan SetAction ke tiep chac chan chay day du.",
  "\tm_nDoing = -1;",
  "\tm_nAction = 0;"), "D2 KNpcRes::Init dat lai m_nDoing")

sys.exit(0 if ok else 1)
