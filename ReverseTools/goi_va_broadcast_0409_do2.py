# -*- coding: utf-8 -*-
"""[BC 04/09 do2] Chot cau hoi cuoi: goi toi mot client den TU BAO NHIEU VUNG khac nhau?
 Lap luan: khoa 5 NPC/vung/tick + phat ra 9 vung => mot client chi co the nhan tu 9 VUNG (vung minh dung + 8 vung ke),
 toi da 9 x 5 = 45 goi/tick. Do that luc dong: 63.324 goi/giay. Neu so vung gui toi client > 9 thi nut cua client dang
 nam trong danh sach nguoi choi cua NHIEU VUNG (hong quan ly danh sach) - do moi la goc, khong phai "gui qua nhieu".
 Them:
  1. Dem so vung KHAC NHAU gui toi mot client (theo doi client nhan nhieu nhat cua 10 giay truoc), in kem vung cua client.
  2. Dem NHIP TICK THAT (moi lan KSubWorldSet::Activate = 1 tick cho ca the gioi) - ban truoc dem theo m_nIndex bi
     CONG DON GIUA CAC BAN DO (vung so 20 ton tai o moi ban do) nen ra 27-90/giay, khong dung.
 KHONG doi hanh vi. Doc/ghi latin-1 newline=''. Neo kiem dem == 1."""
import io, os

ROOT = r"D:\GAMEDEVNEW_wt_bc\Sources\Core\Src"
MARK = "[BC 04/09 do2]"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def rep1(s, old, new, name):
    n = s.count(old)
    assert n == 1, "%s: found %d (expected 1)" % (name, n)
    return s.replace(old, new)

# ---------------- KSubWorldSet.cpp: nhip tick that
p = os.path.join(ROOT, "KSubWorldSet.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    old = "\tnActiveRegionCount = 0;" + N + "\tnActiveNpcCount = 0;" + N
    new = ("\t{\t// %s nhip tick THAT cua ca the gioi (mot lan moi vong Activate)" % MARK + N +
           "\t\textern int g_nBCTick;" + N +
           "\t\tg_nBCTick++;" + N +
           "\t}" + N + old)
    s = rep1(s, old, new, "dem tick")
    wr(p, s); print("KSubWorldSet.cpp OK")
else:
    print("KSubWorldSet.cpp da va")

# ---------------- KRegion.cpp
p = os.path.join(ROOT, "KRegion.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    old = "int g_nBCNormalSync = 0;\t\t\t// so lan KNpc::NormalSync (dat trong KNpc.cpp)" + N
    new = (old +
           "int g_nBCTick = 0;\t\t\t\t\t// %s nhip tick that (KSubWorldSet::Activate)" % MARK + N +
           "int g_nBCTheoDoi = 0;\t\t\t\t// chi so Player dang theo doi (client nhan nhieu nhat 10 giay truoc)" + N +
           "static unsigned char g_abyBCVungGui[BC_MAX_VUNG];\t// vung nao da gui cho client do (1 byte/vung)" + N)
    s = rep1(s, old, new, "bien do2")
    # danh dau vung khi gui cho client dang theo doi
    old = "\t\t\t\tBC_DemNguoiNhan(pNode->m_nIndex, nBCLoaiGoi, (int)dwSize);\t// [BC 03/09 c] do theo TUNG nguoi nhan" + N
    new = (old +
           "\t\t\t\tif (pNode->m_nIndex == g_nBCTheoDoi && m_nIndex >= 0 && m_nIndex < BC_MAX_VUNG)\t// %s" % MARK + N +
           "\t\t\t\t\tg_abyBCVungGui[m_nIndex] = 1;" + N)
    s = rep1(s, old, new, "danh dau vung")
    # in bao cao
    old = "\t\tAUTOLOG(\"[BC-SYNC] 10s: NormalSync=%d (%d/giay) -> moi lan 9 luot (NPC thuong) hoac 18 luot (NPC nguoi choi: gui ca goi 77 va 75)\"," + N + "\t\t\tg_nBCNormalSync, g_nBCNormalSync / 10);" + N
    new = (old +
           "\t\t{\t// %s so VUNG KHAC NHAU da gui cho client dang theo doi + nhip tick that" % MARK + N +
           "\t\t\tint nSoVung = 0;" + N +
           "\t\t\tfor (int k = 0; k < BC_MAX_VUNG; k++)" + N +
           "\t\t\t\tif (g_abyBCVungGui[k])" + N +
           "\t\t\t\t\tnSoVung++;" + N +
           "\t\t\tint nVungCuaHo = (g_nBCTheoDoi > 0 && g_nBCTheoDoi < MAX_PLAYER) ? Npc[Player[g_nBCTheoDoi].m_nIndex].m_RegionIndex : -1;" + N +
           "\t\t\tAUTOLOG(\"[BC-TUVUNG] 10s: client theo doi=%d dung o vung=%d, NHAN GOI TU %d VUNG khac nhau (dung thiet ke phai <= 9) | tick that=%d (%d/giay)\"," + N +
           "\t\t\t\tg_nBCTheoDoi, nVungCuaHo, nSoVung, g_nBCTick, g_nBCTick / 10);" + N +
           "\t\t\tmemset(g_abyBCVungGui, 0, sizeof(g_abyBCVungGui));" + N +
           "\t\t\tg_nBCTheoDoi = nMaxIdx;\t// 10 giay toi theo doi client nhan nhieu nhat vua roi" + N +
           "\t\t\tg_nBCTick = 0;" + N +
           "\t\t}" + N)
    s = rep1(s, old, new, "in BC-TUVUNG")
    wr(p, s); print("KRegion.cpp OK")
else:
    print("KRegion.cpp da va")
print("DONE")
