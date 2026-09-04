# -*- coding: utf-8 -*-
"""[BC 03/09 b] Sau khi chay ban 486cb9aa: sap client + may chu in 'Socket write where not all data was written'.
So do [BC-DEM]/[BC-LOAI]: gui toi nguoi that len 594.563 goi/10 s (~59.000 goi/giay) vi F4 khien MOI goi cua 9 vung
(258 bot ria) toi tay client, trong khi kiem tam cu 'nDX <= 32 && nDY <= 32' KHONG lay tri tuyet doi (phia am khong loc)
va 40 o la nguong client tu bo (S6_XaQuaTam). Vá:
 1. Loc theo KHOANG CACH THAT hai chieu: |dx| <= BC_TAM_O va |dy| <= BC_TAM_O (mac dinh 32 o, doc [Server] BroadCastTam
    trong config.ini may chu de chinh khong can build) - nguoi ngoai tam KHONG duoc gui (client van bo >= 40 o).
 2. Dem them 'ngoai_tam' vao [BC-DEM] de thay bao nhieu goi bi loc.
Doc/ghi latin-1 newline='' (chu thich GBK). Neo kiem dem == 1."""
import io, os

ROOT = r"D:\GAMEDEVNEW_wt_bc\Sources\Core\Src"
MARK = "[BC 03/09 b]"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def rep1(s, old, new, name):
    n = s.count(old)
    assert n == 1, "%s: found %d (expected 1)" % (name, n)
    return s.replace(old, new)

p = os.path.join(ROOT, "KRegion.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    # (a) nguong tam doc mot lan tu config.ini [Server] BroadCastTam (mac dinh 32)
    old = "\t#define\tMAX_SYNC_RANGE\t32//25" + N
    new = (old +
           "\t// %s tam phat THAT hai chieu (|dx|,|dy| <= BC_TAM): kiem cu chi 'nDX <= 32' (khong tri tuyet doi) nen phia am" % MARK + N +
           "\t// khong loc -> sau F4 moi goi cua 9 vung (~258 bot ria) doi vao 1 client (~59.000 goi/giay, do 03/09 dem) -> client sap." + N +
           "\t// Client tu bo goi >= 40 o (S6_XaQuaTam) nen gui ngoai 32 o la vo ich. [Server] BroadCastTam trong config.ini de chinh." + N +
           "\tstatic int s_nBCTam = -1;" + N +
           "\tif (s_nBCTam < 0)" + N +
           "\t{" + N +
           "\t\ts_nBCTam = (int)GetPrivateProfileIntA(\"Server\", \"BroadCastTam\", MAX_SYNC_RANGE, \".\\\\config.ini\");" + N +
           "\t\tif (s_nBCTam < 8) s_nBCTam = 8;" + N +
           "\t\tif (s_nBCTam > 64) s_nBCTam = 64;" + N +
           "\t}" + N)
    s = rep1(s, old, new, "MAX_SYNC_RANGE define")
    # (b) kiem tam hai chieu + dem ngoai tam
    old = ("\t\t\tint nDX = Npc[nNpcIndex].m_MapX - nOX;" + N +
           "\t\t\tint nDY = Npc[nNpcIndex].m_MapY - nOY;" + N +
           "\t\t\tif (Player[pNode->m_nIndex].m_nNetConnectIdx >= 0 " + N +
           "\t\t\t\t&& nDX <= MAX_SYNC_RANGE && nDY <= MAX_SYNC_RANGE" + N +
           "\t\t\t\t&& Player[pNode->m_nIndex].m_bSleepMode == FALSE)" + N)
    new = ("\t\t\tint nDX = Npc[nNpcIndex].m_MapX - nOX;" + N +
           "\t\t\tint nDY = Npc[nNpcIndex].m_MapY - nOY;" + N +
           "\t\t\tif (nDX < 0) nDX = -nDX;\t// %s tri tuyet doi hai chieu" % MARK + N +
           "\t\t\tif (nDY < 0) nDY = -nDY;" + N +
           "\t\t\tif (Player[pNode->m_nIndex].m_nNetConnectIdx >= 0 && (nDX > s_nBCTam || nDY > s_nBCTam))" + N +
           "\t\t\t\tnBCNgoaiTam++;\t// %s nguoi that nhung ngoai tam -> khong gui" % MARK + N +
           "\t\t\tif (Player[pNode->m_nIndex].m_nNetConnectIdx >= 0 " + N +
           "\t\t\t\t&& nDX <= s_nBCTam && nDY <= s_nBCTam" + N +
           "\t\t\t\t&& Player[pNode->m_nIndex].m_bSleepMode == FALSE)" + N)
    s = rep1(s, old, new, "kiem tam")
    # (c) khai bao bien dem truoc vong lap
    old = "\tint nF4Gui   = 0;\t// [F4] so nguoi that da gui trong lan phat nay" + N
    new = old + "\tint nBCNgoaiTam = 0;\t// %s nguoi that bi loc vi ngoai tam" % MARK + N
    s = rep1(s, old, new, "khai bao ngoai tam")
    # (d) cong don + in
    old = "\t\ts_nF4Goi++;" + N + "\t\ts_nF4Gui   += nF4Gui;" + N
    new = ("\t\tstatic int s_nBCNgoaiTam = 0;\t// %s" % MARK + N +
           "\t\ts_nBCNgoaiTam += nBCNgoaiTam;" + N + old)
    s = rep1(s, old, new, "cong don ngoai tam")
    old = "\t\t\tAUTOLOG(\"[BC-DEM] 10s: goi=%d gui=%d cat_vi_het_ngan_sach=%d node_duyet=%d\", s_nF4Goi, s_nF4Gui, s_nF4Bo, s_nF4Duyet);" + N
    new = "\t\t\tAUTOLOG(\"[BC-DEM] 10s: goi=%d gui=%d cat_vi_het_ngan_sach=%d node_duyet=%d ngoai_tam=%d tam=%d\", s_nF4Goi, s_nF4Gui, s_nF4Bo, s_nF4Duyet, s_nBCNgoaiTam, s_nBCTam);" + N
    s = rep1(s, old, new, "in BC-DEM")
    old = "\t\t\ts_nF4Goi = 0; s_nF4Gui = 0; s_nF4Bo = 0; s_nF4Duyet = 0;" + N
    new = "\t\t\ts_nF4Goi = 0; s_nF4Gui = 0; s_nF4Bo = 0; s_nF4Duyet = 0; s_nBCNgoaiTam = 0;" + N
    s = rep1(s, old, new, "reset")
    wr(p, s); print("KRegion.cpp OK (loc tam hai chieu)")
else:
    print("KRegion.cpp da va")
print("DONE")
