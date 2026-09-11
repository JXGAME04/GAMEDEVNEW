# -*- coding: utf-8 -*-
r"""goi_va_trangtri_1109.py - [TRANGTRI 11/09] bat lai DONG VAT + AM THANH NEN quanh map (client).

DO DUOC TRUOC KHI VA (doc thang pak client cua du an, khong doan):
  - Region_C.dat co 6 phan; phan 2 = NPC client, phan 3 = OBJ. CA HAI PHAN DEU CO DU LIEU
    nhung client KHONG NAP: than ham KRegion::LoadObject(nSubWorld,nX,nY,lpszPath) bi chu thich
    tu dong 364 den 458, va HAI cho goi trong KSubWorld.cpp (2390, 2416) cung bi chu thich.
  - Map 53 (Ba Lang huyen) co dung 25 NPC trang tri va 41 OBJ; hai ben (du an va 2.0) TRUNG NHAU
    tung ban ghi: buom 2 x8, chuon chuon x5, buom 1 x4, se x3, ga mai x2, ech/cho vang/ga trong x1.
    41 OBJ deu la nguon AM THANH (ObjData.txt: 313 tieng da thu, 314 tieng hoang oanh, 319 tieng
    da man, 320 tieng chim non, 321 tieng da, 322 tieng con trung, 323 tieng roc rach) - khong ve hinh.
  - Toan bo pak: 55 map co du lieu nay, 2.159 NPC + 1.628 OBJ. Cua so 3x3 (so vung client giu cung luc)
    NANG NHAT chi 29 NPC (tran MAX_NPC client = 800) va 10 OBJ (tran MAX_OBJECT = 256) -> khong lo tran khe.
  - Bang mau: NpcS.txt co cot ClientOnly, 30 dong = 1, dung la nhom thu nuoi (dong 419 "Ga trong",
    420 "Ga mai") - khop voi ma mau trong du lieu map (418 = ga trong, 419 = ga mai) theo do lech +1 da biet.

VI SAO PHAI VA THEM 5 CHO NUA (neu chi bo chu thich thi thu se bien mat sau vai giay):
  1. KNpc::Activate (dong ~1059) go NPC khoi vung khi cach nguoi choi > MAX_SYNC_RANGE+2 o.
  2. KNpc::Activate (dong ~1076) go NPC khoi vung khi > 120 tick (~6,7 s) khong co goi dong bo.
  3. KNpcSet::CheckBalance (dong ~830) XOA HAN NPC khi > 1000 tick (~55 s) khong co goi dong bo.
  4. KNpcSet::CheckBalance (dong ~797) gan lai vung cho NPC mo coi - duong nay AddRef vat can,
     sai voi NPC trang tri (chung khong giu o).
  NPC trang tri KHONG BAO GIO duoc may chu dong bo nen ca 4 duong tren deu ban trung chung.
  5. KRegion::Close() chi go NPC khoi vung (mo coi) chu khong tra khe. Voi NPC trang tri thi
     SearchClientID van thay ban mo coi -> lan nap sau BO QUA -> thu bien mat vinh vien, va khe
     NPC ro dan. Sua: vung dong thi tra khe NGAY cho NPC trang tri.

Dau nhan dien chinh xac: Npc[].m_sClientNpcID.m_dwRegionID > 0 - CHI dat trong KNpcSet::AddClientNpc,
xoa ve 0 luc cap khe (KNpcSet.cpp:485) va luc khoi tao (KNpc.cpp:610). Khong dung m_bClientOnly vi
co cua bang mau cung dat co do cho NPC may chu sinh.

Khoa tat: [Client] VeTrangTri=0 trong config.ini (mac dinh 1 = bat).
Do: [TRANGTRI] moi 10 s trong jx_paint.log - so NPC them / hong / xoa, so OBJ them, 3 ten dau.
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[TRANGTRI 11/09]"
D = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n"), s.count("\r\n")


def ghi(p, s, h0, lf0, crlf0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten)
        sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong))
        sys.exit(1)
    return s.replace(old, new)


# ------------------------------------------------------------------ KRegion.cpp
P = D + "KRegion.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    # (a) bien dem + khoa tat, dat ngay truoc ham nap
    s = rep(s,
            "BOOL KRegion::LoadObject(int nSubWorld, int nX, int nY, char *lpszPath)" + NL + "{" + NL,
            NL.join([
                "// " + TAG + " dem NPC/OBJ trang tri nap tu du lieu vung (xem jx_paint.log dong [TRANGTRI])",
                "unsigned g_uTTNpcThem = 0, g_uTTNpcHong = 0, g_uTTNpcXoa = 0, g_uTTObjThem = 0;",
                "char     g_szTTTen[3][40] = { \"\", \"\", \"\" };",
                "int      g_nTTSo = 0;",
                "",
                "BOOL KRegion::LoadObject(int nSubWorld, int nX, int nY, char *lpszPath)",
                "{",
                ""]), "R dem")
    # (b) bo chu thich mo khoi + them khoa tat
    s = rep(s, "/*" + T + "char" + T + "szPath[FILE_NAME_LENGTH], szFile[FILE_NAME_LENGTH];" + NL,
            NL.join([
                T + "{" + T + "// " + TAG + " khoa tat: [Client] VeTrangTri=0 trong config.ini",
                T*2 + "static int s_nTT = -1;",
                T*2 + "if (s_nTT < 0)",
                T*3 + "s_nTT = GetPrivateProfileIntA(\"Client\", \"VeTrangTri\", 1, \".\\\\config.ini\") ? 1 : 0;",
                T*2 + "if (!s_nTT)",
                T*3 + "return TRUE;",
                T + "}",
                T + "char" + T + "szPath[FILE_NAME_LENGTH], szFile[FILE_NAME_LENGTH];",
                ""]), "R mo khoi")
    # (c) bo chu thich dong khoi (neo dai vi '\t}*/' co 2 cho trong tep)
    s = rep(s,
            T*2 + "ObjSet.ClientLoadRegionObj(szPath, nX, nY, nSubWorld, this->m_nIndex);" + NL + T + "}*/" + NL,
            T*2 + "ObjSet.ClientLoadRegionObj(szPath, nX, nY, nSubWorld, this->m_nIndex);" + NL + T + "}" + T + "// " + TAG + NL,
            "R dong khoi")
    # (d) dem trong LoadClientNpc
    s = rep(s, NL.join([
        T*3 + "int nIdx = NpcSet.AddClientNpc(sNpcCell.nTemplateID, LOWORD(m_RegionID), HIWORD(m_RegionID), sNpcCell.nPositionX, sNpcCell.nPositionY, i);",
        T*3 + "if (nIdx > 0)",
        T*3 + "{",
        T*4 + "Npc[nIdx].m_Kind = sNpcCell.shKind;",
        T*4 + "Npc[nIdx].SendCommand(do_stand);",
        T*4 + "Npc[nIdx].m_Dir = Npc[nIdx].GetNormalNpcStandDir(sNpcCell.nCurFrame);",
        T*3 + "}", ""]), NL.join([
        T*3 + "int nIdx = NpcSet.AddClientNpc(sNpcCell.nTemplateID, LOWORD(m_RegionID), HIWORD(m_RegionID), sNpcCell.nPositionX, sNpcCell.nPositionY, i);",
        T*3 + "if (nIdx > 0)",
        T*3 + "{",
        T*4 + "Npc[nIdx].m_Kind = sNpcCell.shKind;",
        T*4 + "Npc[nIdx].SendCommand(do_stand);",
        T*4 + "Npc[nIdx].m_Dir = Npc[nIdx].GetNormalNpcStandDir(sNpcCell.nCurFrame);",
        T*4 + "g_uTTNpcThem++;" + T + "// " + TAG,
        T*4 + "if (g_nTTSo < 3)",
        T*4 + "{",
        T*5 + "strncpy(g_szTTTen[g_nTTSo], Npc[nIdx].Name, sizeof(g_szTTTen[0]) - 1);",
        T*5 + "g_szTTTen[g_nTTSo][sizeof(g_szTTTen[0]) - 1] = 0;",
        T*5 + "g_nTTSo++;",
        T*4 + "}",
        T*3 + "}",
        T*3 + "else",
        T*4 + "g_uTTNpcHong++;" + T + "// " + TAG + " het khe NPC hoac toa do ngoai vung", ""]), "R dem npc")
    # (e) vung dong: tra khe cho NPC trang tri thay vi de mo coi
    s = rep(s, NL.join([
        T*2 + "Npc[pTempNode->m_nIndex].m_RegionIndex = -1;",
        T*2 + "RemoveNpc(pTempNode->m_nIndex);", ""]), NL.join([
        T*2 + "int nTT11 = pTempNode->m_nIndex;" + T + "// " + TAG,
        T*2 + "Npc[nTT11].m_RegionIndex = -1;",
        T*2 + "RemoveNpc(nTT11);",
        "#ifndef _SERVER",
        T*2 + "// " + TAG + " NPC trang tri (nap tu phan 2 cua Region_C.dat) phai TRA KHE ngay khi",
        T*2 + "// vung dong. Neu de mo coi thi SearchClientID van thay -> lan nap sau bo qua (thu",
        T*2 + "// bien mat vinh vien) va khe NPC ro dan (2.159 ban ghi tren 55 map, tran client 800).",
        T*2 + "if (Npc[nTT11].m_sClientNpcID.m_dwRegionID > 0)",
        T*2 + "{",
        T*3 + "extern unsigned g_uTTNpcXoa;",
        T*3 + "g_uTTNpcXoa++;",
        T*3 + "NpcSet.Remove(nTT11);",
        T*2 + "}",
        "#endif", ""]), "R dong vung")
    ghi(P, s, h0, lf0, crlf0, "KRegion.cpp")
else:
    print("KRegion.cpp da co")

# ------------------------------------------------------------------ KSubWorld.cpp: bat lai hai cho goi
P = D + "KSubWorld.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, T*3 + "//m_Region[nIdx].LoadObject(0, nX, nY, m_szMapPath);" + NL,
            T*3 + "m_Region[nIdx].LoadObject(0, nX, nY, m_szMapPath);" + T + "// " + TAG + " nap NPC+OBJ trang tri cua vung" + NL, "S goi 1")
    s = rep(s, T*4 + "//m_Region[nConIdx].LoadObject(0, nX + nXOff[i], nY + nYOff[i], m_szMapPath);" + NL,
            T*4 + "m_Region[nConIdx].LoadObject(0, nX + nXOff[i], nY + nYOff[i], m_szMapPath);" + T + "// " + TAG + NL, "S goi 2")
    ghi(P, s, h0, lf0, crlf0, "KSubWorld.cpp")
else:
    print("KSubWorld.cpp da co")

# ------------------------------------------------------------------ KNpc.cpp: 2 duong go vung
P = D + "KNpc.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, T + "if (!IsPlayer() && nS6VDisX != 0x7FFFFFFF && nS6VDisY != 0x7FFFFFFF" + NL,
            T + "// " + TAG + " NPC trang tri khong do may chu dong bo -> mien hai duong go vung ben duoi" + NL +
            T + "if (!IsPlayer() && m_sClientNpcID.m_dwRegionID == 0 && nS6VDisX != 0x7FFFFFFF && nS6VDisY != 0x7FFFFFFF" + NL, "N vanh")
    s = rep(s, T + "if (!IsPlayer() && SubWorld[0].m_dwCurrentTime - m_SyncSignal > 120)" + NL,
            T + "if (!IsPlayer() && m_sClientNpcID.m_dwRegionID == 0 && SubWorld[0].m_dwCurrentTime - m_SyncSignal > 120)" + T + "// " + TAG + NL, "N cam")
    ghi(P, s, h0, lf0, crlf0, "KNpc.cpp")
else:
    print("KNpc.cpp da co")

# ------------------------------------------------------------------ KNpcSet.cpp: 2 duong trong CheckBalance
P = D + "KNpcSet.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, T*2 + "if (Npc[nIdx].m_RegionIndex < 0" + NL + T*2 + " && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex" + NL,
            T*2 + "if (Npc[nIdx].m_RegionIndex < 0" + NL +
            T*2 + " && Npc[nIdx].m_sClientNpcID.m_dwRegionID == 0" + T + "// " + TAG + " NPC trang tri khong AddRef vat can" + NL +
            T*2 + " && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex" + NL, "NS gannhanh")
    s = rep(s, T*2 + "if (SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > 1000)" + NL,
            T*2 + "// " + TAG + " NPC trang tri khong bao gio co goi dong bo -> mien duong don theo han 1000 tick" + NL +
            T*2 + "if (Npc[nIdx].m_sClientNpcID.m_dwRegionID == 0 && SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > 1000)" + NL, "NS han")
    ghi(P, s, h0, lf0, crlf0, "KNpcSet.cpp")
else:
    print("KNpcSet.cpp da co")

# ------------------------------------------------------------------ KObjSet.cpp: dem OBJ them
P = D + "KObjSet.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, T*2 + "ClientAdd(0, sData.nTemplateID, sData.nState, sData.nDir, 0, sData.Pos.x, sData.Pos.y, sInfo);" + NL,
            T*2 + "{" + T + "// " + TAG + " dem vat the nen (nguon am thanh) nap tu du lieu vung" + NL +
            T*3 + "extern unsigned g_uTTObjThem;" + NL +
            T*3 + "if (ClientAdd(0, sData.nTemplateID, sData.nState, sData.nDir, 0, sData.Pos.x, sData.Pos.y, sInfo) > 0)" + NL +
            T*4 + "g_uTTObjThem++;" + NL +
            T*2 + "}" + NL, "O dem", 2)
    ghi(P, s, h0, lf0, crlf0, "KObjSet.cpp")
else:
    print("KObjSet.cpp da co")

# ------------------------------------------------------------------ KSubWorldSet.cpp: in [TRANGTRI] moi 10 s
P = D + "KSubWorldSet.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    neo = T*2 + "g_uVatDongVe = 0; g_nVatDongSo = 0; g_szVatDong[0][0] = g_szVatDong[1][0] = g_szVatDong[2][0] = 0;" + NL + T + "}" + NL
    # neo that trong tep dung 3 tab cho dong gan, lay lai cho dung
    neo = T*3 + "g_uVatDongVe = 0; g_nVatDongSo = 0; g_szVatDong[0][0] = g_szVatDong[1][0] = g_szVatDong[2][0] = 0;" + NL + T*2 + "}" + NL
    s = rep(s, neo, neo + NL.join([
        T*2 + "{" + T + "// " + TAG + " NPC + vat the nen nap tu du lieu vung",
        T*3 + "extern unsigned g_uTTNpcThem, g_uTTNpcHong, g_uTTNpcXoa, g_uTTObjThem;",
        T*3 + "extern char g_szTTTen[3][40]; extern int g_nTTSo;",
        T*3 + "if (g_uTTNpcThem || g_uTTNpcXoa || g_uTTObjThem || g_uTTNpcHong)",
        T*3 + "{",
        T*4 + "FILE* pT = fopen(\"jx_paint.log\", \"a\");",
        T*4 + "if (pT)",
        T*4 + "{",
        T*5 + "fprintf(pT, \"[TRANGTRI] 10s: npc them %u hong %u xoa %u | obj them %u | ten: %s %s %s\\n\",",
        T*6 + "g_uTTNpcThem, g_uTTNpcHong, g_uTTNpcXoa, g_uTTObjThem, g_szTTTen[0], g_szTTTen[1], g_szTTTen[2]);",
        T*5 + "fclose(pT);",
        T*4 + "}",
        T*3 + "}",
        T*3 + "g_uTTNpcThem = g_uTTNpcHong = g_uTTNpcXoa = g_uTTObjThem = 0;",
        T*3 + "g_nTTSo = 0; g_szTTTen[0][0] = g_szTTTen[1][0] = g_szTTTen[2][0] = 0;",
        T*2 + "}", ""]), "SW in")
    ghi(P, s, h0, lf0, crlf0, "KSubWorldSet.cpp")
else:
    print("KSubWorldSet.cpp da co")

print("XONG " + TAG)
