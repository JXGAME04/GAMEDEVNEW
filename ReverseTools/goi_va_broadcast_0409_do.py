# -*- coding: utf-8 -*-
"""[BC 04/09 do] Do CHINH XAC vi sao MOT client nhan 63.000 goi/giay (9,6 MB/s) trong khi khoa 5 NPC/vung/tick
chi cho phep toi da 9 vung x 5 = 45 goi/tick (810/giay). Bon so lieu chot moi 10 giay:
  [BC-VUNG] vung cua client nhan bao nhieu luot BroadCast, vung do duoc Activate bao nhieu lan (= so tick/giay),
            va 3 vung bi phat nhieu nhat toan may chu.
  [BC-SYNC] so lan KNpc::NormalSync duoc goi (moi lan = 9 luot phat cho NPC thuong, 18 luot cho NPC nguoi choi
            vi gui CA hai goi 77 va 75) va so lan Activate cua toan bo vung.
So sanh: neu vung client nhan >> 9 x 5 luot/tick thi khoa chunk khong con hieu luc; neu Activate/giay >> 18 thi
nhip tick may chu chay nhanh hon thiet ke; neu ca hai binh thuong thi con duong phat khong phai NormalSync.
KHONG doi hanh vi - chi dem. Doc/ghi latin-1 newline='' (chu thich GBK). Neo kiem dem == 1."""
import io, os

ROOT = r"D:\GAMEDEVNEW_wt_bc\Sources\Core\Src"
MARK = "[BC 04/09 do]"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def rep1(s, old, new, name):
    n = s.count(old)
    assert n == 1, "%s: found %d (expected 1)" % (name, n)
    return s.replace(old, new)

# ---------------- KRegion.cpp
p = os.path.join(ROOT, "KRegion.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    # (a) bien dem + dem trong BroadCast
    old = "static int   g_anBCNguoiGoi[MAX_PLAYER];\t// so goi da gui cho tung nguoi choi (chi so Player[])" + N
    new = ("#define BC_MAX_VUNG 8192" + N +
           "int g_anBCVungGoi[BC_MAX_VUNG];\t\t// %s so luot BroadCast phat VAO tung vung" % MARK + N +
           "int g_anBCVungActive[BC_MAX_VUNG];\t// so lan vung do duoc Activate (= so tick trong 10 giay)" + N +
           "int g_nBCNormalSync = 0;\t\t\t// so lan KNpc::NormalSync (dat trong KNpc.cpp)" + N +
           old)
    s = rep1(s, old, new, "bien dem vung")
    old = "\tconst int nBCLoaiGoi = (pBuffer && dwSize > 0) ? (int)((const BYTE*)pBuffer)[0] : 0;"
    new = (old + N +
           "\tif (m_nIndex >= 0 && m_nIndex < BC_MAX_VUNG)\t// %s dem luot phat vao vung nay" % MARK + N +
           "\t\tg_anBCVungGoi[m_nIndex]++;")
    s = rep1(s, old, new, "dem luot phat vao vung")
    # (b) dem Activate
    old = "void KRegion::Activate()" + N + "{" + N
    new = ("void KRegion::Activate()" + N + "{" + N +
           "\tif (m_nIndex >= 0 && m_nIndex < BC_MAX_VUNG)\t// %s dem so lan vung nay chay mot tick" % MARK + N +
           "\t\tg_anBCVungActive[m_nIndex]++;" + N)
    s = rep1(s, old, new, "dem activate")
    # (c) in bao cao
    old = "\tif (nLen > 0)" + N + "\t\tAUTOLOG(\"[BC-TOP] 10s top loai theo luot GUI:%s\", szTop);" + N
    new = (old +
           "\t// %s vung cua client + 3 vung bi phat nhieu nhat + nhip tick" % MARK + N +
           "\t{" + N +
           "\t\tint nVungClient = (nMaxIdx > 0) ? Npc[Player[nMaxIdx].m_nIndex].m_RegionIndex : -1;" + N +
           "\t\tint nGoiVung = (nVungClient >= 0 && nVungClient < BC_MAX_VUNG) ? g_anBCVungGoi[nVungClient] : 0;" + N +
           "\t\tint nTick = (nVungClient >= 0 && nVungClient < BC_MAX_VUNG) ? g_anBCVungActive[nVungClient] : 0;" + N +
           "\t\tint anTop[3] = { -1, -1, -1 };" + N +
           "\t\tfor (int nL = 0; nL < 3; nL++)" + N +
           "\t\t{" + N +
           "\t\t\tint nBest = -1, nVal = 0;" + N +
           "\t\t\tfor (int k = 0; k < BC_MAX_VUNG; k++)" + N +
           "\t\t\t{" + N +
           "\t\t\t\tif (k == anTop[0] || k == anTop[1] || k == anTop[2])" + N +
           "\t\t\t\t\tcontinue;" + N +
           "\t\t\t\tif (g_anBCVungGoi[k] > nVal)" + N +
           "\t\t\t\t{" + N +
           "\t\t\t\t\tnVal = g_anBCVungGoi[k];" + N +
           "\t\t\t\t\tnBest = k;" + N +
           "\t\t\t\t}" + N +
           "\t\t\t}" + N +
           "\t\t\tanTop[nL] = nBest;" + N +
           "\t\t}" + N +
           "\t\tAUTOLOG(\"[BC-VUNG] 10s: vung_client=%d nhan=%d luot (%d/tick, tick=%d ~%d/giay) | top vung: %d:%d %d:%d %d:%d\"," + N +
           "\t\t\tnVungClient, nGoiVung, nTick > 0 ? nGoiVung / nTick : 0, nTick, nTick / 10," + N +
           "\t\t\tanTop[0], anTop[0] >= 0 ? g_anBCVungGoi[anTop[0]] : 0," + N +
           "\t\t\tanTop[1], anTop[1] >= 0 ? g_anBCVungGoi[anTop[1]] : 0," + N +
           "\t\t\tanTop[2], anTop[2] >= 0 ? g_anBCVungGoi[anTop[2]] : 0);" + N +
           "\t\tAUTOLOG(\"[BC-SYNC] 10s: NormalSync=%d (%d/giay) -> moi lan 9 luot (NPC thuong) hoac 18 luot (NPC nguoi choi: gui ca goi 77 va 75)\"," + N +
           "\t\t\tg_nBCNormalSync, g_nBCNormalSync / 10);" + N +
           "\t\tg_nBCNormalSync = 0;" + N +
           "\t\tmemset(g_anBCVungGoi, 0, sizeof(g_anBCVungGoi));" + N +
           "\t\tmemset(g_anBCVungActive, 0, sizeof(g_anBCVungActive));" + N +
           "\t}" + N)
    s = rep1(s, old, new, "in BC-VUNG")
    wr(p, s); print("KRegion.cpp OK")
else:
    print("KRegion.cpp da va")

# ---------------- KNpc.cpp: dem NormalSync
p = os.path.join(ROOT, "KNpc.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    old = "void KNpc::NormalSync() //Sync npc min li"
    i = s.index(old); j = s.index(N, i) + len(N)
    # dong sau la '{'
    j2 = s.index("{", j) + 1
    add = (N + "\textern int g_nBCNormalSync;\t// %s dem so lan phat dong bo (KRegion.cpp)" % MARK + N +
           "\tg_nBCNormalSync++;" + N)
    s = s[:j2] + add + s[j2:]
    wr(p, s); print("KNpc.cpp OK")
else:
    print("KNpc.cpp da va")
print("DONE")
