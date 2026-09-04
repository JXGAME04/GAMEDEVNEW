# -*- coding: utf-8 -*-
"""[BC 03/09 dem] Huong 1 + 5 (chu chon) tren nen F4 cua wauto-75 (da ap bang git apply truoc khi chay script nay):
 1. KRegion.h: MAX_BROADCAST_COUNT 100 -> 500 (bang NPC_SYNC_BROADCAST_LIMIT) cho goi MOT LAN
    (ProcessState phat chieu s2c_skillcast, DoHurt, DoRun, SetCamp/SetCurrentCamp/RestoreCurrentCamp).
 5. KRegion::BroadCast: bo dem theo LOAI GOI (byte dau = ProtocolType): goi phat / nguoi that da gui / lan bi cat vi het
    ngan sach, in cung nhip 10 s voi [BC-DEM] cua F4 -> dong [BC-LOAI]. Chi phi: vai phep cong moi lan phat.
 Doc/ghi latin-1 newline='' (KRegion.cpp co chu thich GBK - KHONG dung Edit tool). Moi neo kiem dem == 1."""
import io, os, re

ROOT = r"D:\GAMEDEVNEW_wt_bc\Sources\Core\Src"
MARK = "[BC 03/09]"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def rep1(s, old, new, name):
    n = s.count(old)
    assert n == 1, "%s: found %d (expected 1)" % (name, n)
    return s.replace(old, new)

# ---------------- KRegion.h : huong 1
p = os.path.join(ROOT, "KRegion.h"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    s = rep1(s, "#define\tMAX_BROADCAST_COUNT\t\t100" + N,
             "// %s huong 1 (chu chon 03/09 dem): tran nguoi nhan cho goi MOT LAN (phat chieu, trung don, chay, doi phe)" % MARK + N +
             "// 100 -> 500 = bang NPC_SYNC_BROADCAST_LIMIT. Sau F4 (chi tru khi that gui) tran nay chi con cham khi > 500" + N +
             "// NGUOI THAT trong 9 vung; truoc do 258 bot ria da an sach 100 -> nguoi that khong thay chieu (do 04/09: 366 chieu/95 NPC" + N +
             "// trong khi 4.991 lenh chay/405 NPC). Chi phi: goi chieu ~30 B x so nguoi that, khong dang ke." + N +
             "#define\tMAX_BROADCAST_COUNT\t\t500" + N, "MAX_BROADCAST_COUNT")
    wr(p, s); print("KRegion.h OK (100 -> 500)")
else:
    print("KRegion.h da va")

# ---------------- KRegion.cpp : huong 5 theo loai goi (mo rong khoi dem F4)
p = os.path.join(ROOT, "KRegion.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    assert "[F4" in s, "chua ap diff F4 cua wauto-75 (git apply) truoc khi chay script nay"
    old = ("\t{" + N +
           "\t\tstatic int   s_nF4Goi = 0, s_nF4Gui = 0, s_nF4Bo = 0, s_nF4Duyet = 0;" + N +
           "\t\tstatic DWORD s_uF4Moc = 0;" + N +
           "\t\ts_nF4Goi++;" + N +
           "\t\ts_nF4Gui   += nF4Gui;" + N +
           "\t\ts_nF4Duyet += nF4Duyet;" + N +
           "\t\tif (nMaxCount <= 0 && nF4Duyet < nF4Tong)" + N +
           "\t\t\ts_nF4Bo++;" + N +
           "\t\tconst DWORD uF4Now = GetTickCount();" + N +
           "\t\tif (s_uF4Moc == 0)" + N +
           "\t\t\ts_uF4Moc = uF4Now;" + N +
           "\t\telse if (uF4Now - s_uF4Moc >= 10000)" + N +
           "\t\t{" + N +
           "\t\t\ts_uF4Moc = uF4Now;" + N +
           "\t\t\tAUTOLOG(\"[BC-DEM] 10s: goi=%d gui=%d cat_vi_het_ngan_sach=%d node_duyet=%d\", s_nF4Goi, s_nF4Gui, s_nF4Bo, s_nF4Duyet);" + N +
           "\t\t\ts_nF4Goi = 0; s_nF4Gui = 0; s_nF4Bo = 0; s_nF4Duyet = 0;" + N +
           "\t\t}" + N +
           "\t}" + N)
    new = ("\t{" + N +
           "\t\tstatic int   s_nF4Goi = 0, s_nF4Gui = 0, s_nF4Bo = 0, s_nF4Duyet = 0;" + N +
           "\t\tstatic DWORD s_uF4Moc = 0;" + N +
           "\t\t// %s huong 5: dem THEO LOAI GOI (byte dau cua goi = ProtocolType, xem Headers/KProtocolDef.h:" % MARK + N +
           "\t\t// 76 syncnpcmin, 85 npcrun, 84 npcwalk, 90 npchurt, 91 npcdeath, 92 chgcurcamp, 94 skillcast)." + N +
           "\t\tstatic int   s_anBCGoi[256], s_anBCGui[256], s_anBCBo[256];" + N +
           "\t\tconst int nBCLoai = (pBuffer && dwSize > 0) ? (int)((const BYTE*)pBuffer)[0] : 0;" + N +
           "\t\tconst int nBCCat  = (nMaxCount <= 0 && nF4Duyet < nF4Tong) ? 1 : 0;" + N +
           "\t\ts_anBCGoi[nBCLoai]++; s_anBCGui[nBCLoai] += nF4Gui; s_anBCBo[nBCLoai] += nBCCat;" + N +
           "\t\ts_nF4Goi++;" + N +
           "\t\ts_nF4Gui   += nF4Gui;" + N +
           "\t\ts_nF4Duyet += nF4Duyet;" + N +
           "\t\tif (nBCCat)" + N +
           "\t\t\ts_nF4Bo++;" + N +
           "\t\tconst DWORD uF4Now = GetTickCount();" + N +
           "\t\tif (s_uF4Moc == 0)" + N +
           "\t\t\ts_uF4Moc = uF4Now;" + N +
           "\t\telse if (uF4Now - s_uF4Moc >= 10000)" + N +
           "\t\t{" + N +
           "\t\t\ts_uF4Moc = uF4Now;" + N +
           "\t\t\tAUTOLOG(\"[BC-DEM] 10s: goi=%d gui=%d cat_vi_het_ngan_sach=%d node_duyet=%d\", s_nF4Goi, s_nF4Gui, s_nF4Bo, s_nF4Duyet);" + N +
           "\t\t\t// %s in toi da 8 loai goi nhieu nhat + moi loai co bi cat: 'loai:goi/gui/cat'" % MARK + N +
           "\t\t\tchar szBC[512]; int nBCLen = 0; int nBCIn = 0;" + N +
           "\t\t\tfor (int nBCK = 0; nBCK < 256 && nBCLen < 440; nBCK++)" + N +
           "\t\t\t{" + N +
           "\t\t\t\tif (s_anBCGoi[nBCK] <= 0)" + N +
           "\t\t\t\t\tcontinue;" + N +
           "\t\t\t\tif (nBCIn >= 8 && s_anBCBo[nBCK] <= 0)" + N +
           "\t\t\t\t\tcontinue;" + N +
           "\t\t\t\tnBCLen += _snprintf(szBC + nBCLen, sizeof(szBC) - 1 - nBCLen, \" %d:%d/%d/%d\", nBCK, s_anBCGoi[nBCK], s_anBCGui[nBCK], s_anBCBo[nBCK]);" + N +
           "\t\t\t\tnBCIn++;" + N +
           "\t\t\t}" + N +
           "\t\t\tszBC[sizeof(szBC) - 1] = 0;" + N +
           "\t\t\tif (nBCLen > 0)" + N +
           "\t\t\t\tAUTOLOG(\"[BC-LOAI] 10s loai:goi/gui/cat:%s\", szBC);" + N +
           "\t\t\tmemset(s_anBCGoi, 0, sizeof(s_anBCGoi)); memset(s_anBCGui, 0, sizeof(s_anBCGui)); memset(s_anBCBo, 0, sizeof(s_anBCBo));" + N +
           "\t\t\ts_nF4Goi = 0; s_nF4Gui = 0; s_nF4Bo = 0; s_nF4Duyet = 0;" + N +
           "\t\t}" + N +
           "\t}" + N)
    s = rep1(s, old, new, "khoi dem F4")
    wr(p, s); print("KRegion.cpp OK (dem theo loai goi)")
else:
    print("KRegion.cpp da va")
print("DONE")
