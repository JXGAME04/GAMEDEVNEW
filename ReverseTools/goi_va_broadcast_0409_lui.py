# -*- coding: utf-8 -*-
"""[BC 04/09 LUI] LUI LAI F4 - thay doi da KHOI RA CHUOI LOI tu ban 486cb9aa.
 Su that theo dong thoi gian: truoc 486cb9aa (bdd9bb46, chay tu 21:49) KHONG co lan sap nao kieu nay.
 Ngay sau khi dat 486cb9aa: may chu in 'Socket write where not all data was written' lien tuc va Game.exe sap lien tiep
 (00:21, 00:24, 00:25, 00:26, 00:32, 00:39, 00:47, 00:59) o 5 ham khac nhau.
 Vi sao: F4 (chi tru ngan sach khi THAT gui) sua dung mot cho sai, NHUNG tran MAX_BROADCAST_COUNT 100 truoc do
 dang la BO HAN DONG TINH CO - bot an het suat nen client that chi nhan ~44 goi/giay. Bo cai van do ra thi client
 nhan 21.000-94.844 goi/giay (do duoc), socket nghen, WSASend ghi thieu, tang mang may chu VUT phan duoi
 (SocketServer.cpp WriteCompleted chi printf roi Release) -> luong TCP lech giua goi -> client doc rac.
 Chua sua duoc goc do (nam trong heaven.dll, phai doi cach swap) thi KHONG duoc de F4 chay.
 Nay: tra vong lap BroadCast ve ban truoc F4 (tru ngan sach cho MOI node, khong duyet vong tron).
 GIU: tran 100, loc tam hai chieu |dx|,|dy| <= BroadCastTam (chi giam bot goi, khong the gay hong),
      va toan bo bo dem [BC-DEM]/[BC-LOAI]/[BC-NGUOI]/[BC-TOP]/[BC-VUNG]/[BC-TUVUNG] (chi doc, khong doi hanh vi).
 Doc/ghi latin-1 newline=''. Neo kiem dem == 1."""
import io, os

ROOT = r"D:\GAMEDEVNEW_wt_bc\Sources\Core\Src"
MARK = "[BC 04/09 LUI]"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def rep1(s, old, new, name):
    n = s.count(old)
    assert n == 1, "%s: found %d (expected 1)" % (name, n)
    return s.replace(old, new)

p = os.path.join(ROOT, "KRegion.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK in s:
    print("da lui"); raise SystemExit

# 1. vong lap: bo duyet vong tron, quay ve 'while(pNode && nMaxCount > 0)'
old = ("\tint nF4Duyet = 0;\t// [F4] so node da xet - chan vong tron" + N +
       "\tint nF4Gui   = 0;\t// [F4] so nguoi that da gui trong lan phat nay" + N)
i = s.index(old)
j = s.index("\t\tpNode = pNext;" + N + "\t}" + N, i) + len("\t\tpNode = pNext;" + N + "\t}" + N)
than_cu = N.join([
    "\tint nF4Duyet = 0;\t// so node da xet (chi de dem)",
    "\tint nF4Gui   = 0;\t// so nguoi that da gui trong lan phat nay (chi de dem)",
    "\tint nBCNgoaiTam = 0;\t// nguoi that bi loc vi ngoai tam (chi de dem)",
    "\t// %s TRA VE VONG LAP TRUOC F4: tru ngan sach cho MOI node hop le (ke ca bot), khong duyet vong tron." % MARK,
    "\t// Day chinh la bo han dong tinh co ma ca he thong dang dua vao: bo no ra thi mot client nhan",
    "\t// 21.000-94.844 goi/giay -> nghen socket -> may chu vut byte khi ghi thieu -> luong lech -> client doc rac -> sap.",
    "\t// Chi duoc bo F4 tro lai SAU KHI sua loi vut byte trong SocketServer.cpp (heaven.dll).",
    "\twhile(pNode && nMaxCount > 0)",
    "\t{",
    "\t\tKIndexNode* pNext = (KIndexNode *)pNode->GetNext();",
    "\t\tnF4Duyet++;",
    "\t\tif (pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER)",
    "\t\t{",
    "\t\t\tint nPlayerIndex = pNode->m_nIndex;",
    "\t\t\tint nNpcIndex = Player[nPlayerIndex].m_nIndex;",
    "\t\t\tint nDX = Npc[nNpcIndex].m_MapX - nOX;",
    "\t\t\tint nDY = Npc[nNpcIndex].m_MapY - nOY;",
    "\t\t\tif (nDX < 0) nDX = -nDX;\t// [BC 03/09 b] tri tuyet doi hai chieu (chi loc bot, giu lai)",
    "\t\t\tif (nDY < 0) nDY = -nDY;",
    "\t\t\tif (Player[pNode->m_nIndex].m_nNetConnectIdx >= 0 && (nDX > s_nBCTam || nDY > s_nBCTam))",
    "\t\t\t\tnBCNgoaiTam++;",
    "\t\t\tif (Player[pNode->m_nIndex].m_nNetConnectIdx >= 0 ",
    "\t\t\t\t&& nDX <= s_nBCTam && nDY <= s_nBCTam",
    "\t\t\t\t&& Player[pNode->m_nIndex].m_bSleepMode == FALSE)",
    "\t\t\t{",
    "\t\t\t\tg_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, (BYTE*)pBuffer, dwSize);",
    "\t\t\t\tnF4Gui++;",
    "\t\t\t\tBC_DemNguoiNhan(pNode->m_nIndex, nBCLoaiGoi, (int)dwSize);",
    "\t\t\t\tif (pNode->m_nIndex == g_nBCTheoDoi && m_nIndex >= 0 && m_nIndex < BC_MAX_VUNG)",
    "\t\t\t\t\tg_abyBCVungGui[m_nIndex] = 1;",
    "\t\t\t}",
    "\t\t\tnMaxCount--;\t// %s TRU CHO MOI NODE nhu ban goc (day la cai van)" % MARK,
    "\t\t}",
    "\t\tpNode = pNext;",
    "\t}",
    "",
])
s = s[:i] + than_cu + s[j:]

# 2. bo khai bao trung nBCNgoaiTam cu (neu con)
dup = "\tint nBCNgoaiTam = 0;\t// [BC 03/09 b] nguoi that bi loc vi ngoai tam" + N
if dup in s:
    s = s.replace(dup, "", 1)
wr(p, s)
print("KRegion.cpp OK - da lui F4, giu tran 100 + loc tam + bo dem")
