# -*- coding: utf-8 -*-
"""[PS 04/09] Cach 1: KHONG GUI LAI GOI NGOAI HINH KHI KHONG CO GI DOI (khong gioi han gi ca).
 Su that do duoc: goi s2c_syncplayermin (loai 75) nang 234 byte, chiem 62% TOAN BO bang thong toi client,
 va no KHONG CHUA TOA DO - toa do di bang goi NPC (loai 77, 99 byte). Noi dung 234 byte do la:
   GameTitle[64] + TongName[32] + TongTitle[32] + MateName[32] + bMeridianLevel[12] + trang bi/ngua/cap bac/
   danh vong/phuc duyen/trung sinh... = thu KHONG DOI khi nguoi ta chay.
 Nghia la may chu dang gui di gui lai ten bang, ten ban doi, danh hieu ~1,5 lan/giay cho MOI nguoi trong tam nhin.
 Vá (chi phia may chu, KHONG doi cau truc goi, KHONG can thay client):
   - Bam vet (FNV-1a) noi dung goi. Giong het lan truoc VA chua toi ky lam moi thi KHONG phat.
   - Ky lam moi: [Server] BroadCastLamMoi giay (mac dinh 5, kep 1..60) - luoi an toan cho nguoi vua vao tam nhin.
   - Khi mot client hoi rieng NPC nay (SendSyncData - luc nguoi do vua thay minh) thi XOA dau vet de lan phat
     ke tiep gui lai ngay: nguoi moi thay khong bi thieu trang bi.
   - Dem so lan bo qua, ghi [PS-BO] moi 10 giay.
 Doc/ghi latin-1 newline=''. Neo kiem dem == 1."""
import io, os

ROOT = r"D:\GAMEDEVNEW_wt_bc\Sources\Core\Src"
MARK = "[PS 04/09]"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def rep1(s, old, new, name):
    n = s.count(old)
    assert n == 1, "%s: found %d (expected 1)" % (name, n)
    return s.replace(old, new)

p = os.path.join(ROOT, "KNpc.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK in s:
    print("da va"); raise SystemExit

# 1. bien + ham bam, dat truoc KNpc::SendSyncData
neo = "BOOL KNpc::SendSyncData(int nClient)"
i = s.index(neo)
j = s.rfind(N, 0, i) + len(N)
khoi = N.join([
    "#ifdef _SERVER",
    "// %s Goi ngoai hinh (s2c_syncplayermin, 234 byte) chiem 62%% bang thong ma KHONG chua toa do:" % MARK,
    "// GameTitle[64] + TongName[32] + TongTitle[32] + MateName[32] + kinh mach[12] + trang bi/ngua/cap bac...",
    "// Toan thu khong doi khi chay. Nay chi phat khi NOI DUNG DOI, hoac toi ky lam moi.",
    "static DWORD s_adwPSBam[MAX_NPC];\t// dau vet noi dung goi lan phat truoc",
    "static DWORD s_adwPSLuc[MAX_NPC];\t// tick luc phat lan truoc",
    "static int   s_nPSLamMoi = -1;\t\t// [Server] BroadCastLamMoi (giay)",
    "static int   s_nPSBo = 0, s_nPSGui = 0;",
    "static DWORD PS_Bam(const void* pData, int nCo)",
    "{",
    "\tDWORD dw = 2166136261u;",
    "\tconst BYTE* p = (const BYTE*)pData;",
    "\tfor (int k = 0; k < nCo; k++)",
    "\t{",
    "\t\tdw ^= p[k];",
    "\t\tdw *= 16777619u;",
    "\t}",
    "\treturn dw ? dw : 1;\t// 0 danh rieng cho 'phai gui lai'",
    "}",
    "// mot client vua hoi rieng NPC nay (ho vua nhin thay minh) -> phat lai ngoai hinh o lan sync ke tiep",
    "void PS_XoaDauVet(int nNpcIdx)",
    "{",
    "\tif (nNpcIdx > 0 && nNpcIdx < MAX_NPC)",
    "\t\ts_adwPSBam[nNpcIdx] = 0;",
    "}",
    "#endif",
    "",
])
s = s[:j] + khoi + s[j:]

# 2. SendSyncData: xoa dau vet cho nguoi choi (client vua hoi rieng NPC nay)
old = "BOOL KNpc::SendSyncData(int nClient)\t//Sync npc v"
i2 = s.index(old)
i3 = s.index("{", i2) + 1
s = s[:i3] + (N + "\tif (IsPlayer())\t// %s co nguoi moi thay minh -> lan sync ke tiep phat lai ngoai hinh" % MARK + N +
              "\t\tPS_XoaDauVet(m_Index);" + N) + s[i3:]

# 3. NormalSync: chi phat khi doi hoac toi ky lam moi
old = ("\t\tint nMaxCount = NPC_SYNC_BROADCAST_LIMIT;" + N +
       "\t\tCURREGION.BroadCast(&PlayerSync, sizeof(PLAYER_NORMAL_SYNC), nMaxCount, m_MapX, m_MapY);" + N +
       "\t\tfor (j = 0; j < 8; j++)" + N +
       "\t\t{" + N +
       "\t\t\tint nConRegion = CURREGION.m_nConnectRegion[j];" + N +
       "\t\t\tif (nConRegion == -1)" + N +
       "\t\t\t\tcontinue;" + N)
i4 = s.index(old)
i5 = s.index("\t\t}" + N, s.index("BroadCast((BYTE*)&PlayerSync", i4)) + len("\t\t}" + N)
than_moi = N.join([
    "\t\t// %s chi phat goi ngoai hinh khi NOI DUNG DOI hoac toi ky lam moi (goi nay khong chua toa do)" % MARK,
    "\t\tif (s_nPSLamMoi < 0)",
    "\t\t{",
    "\t\t\ts_nPSLamMoi = (int)GetPrivateProfileIntA(\"Server\", \"BroadCastLamMoi\", 5, \".\\\\config.ini\");",
    "\t\t\tif (s_nPSLamMoi < 1)  s_nPSLamMoi = 1;",
    "\t\t\tif (s_nPSLamMoi > 60) s_nPSLamMoi = 60;",
    "\t\t}",
    "\t\tbool bPSGui = true;",
    "\t\tif (m_Index > 0 && m_Index < MAX_NPC)",
    "\t\t{",
    "\t\t\tconst DWORD dwBam = PS_Bam(&PlayerSync, (int)sizeof(PLAYER_NORMAL_SYNC));",
    "\t\t\tconst DWORD dwLuc = GetTickCount();",
    "\t\t\tif (s_adwPSBam[m_Index] == dwBam &&",
    "\t\t\t\t(dwLuc - s_adwPSLuc[m_Index]) < (DWORD)(s_nPSLamMoi * 18))\t// 18 tick = 1 giay",
    "\t\t\t{",
    "\t\t\t\tbPSGui = false;",
    "\t\t\t\ts_nPSBo++;",
    "\t\t\t}",
    "\t\t\telse",
    "\t\t\t{",
    "\t\t\t\ts_adwPSBam[m_Index] = dwBam;",
    "\t\t\t\ts_adwPSLuc[m_Index] = dwLuc;",
    "\t\t\t\ts_nPSGui++;",
    "\t\t\t}",
    "\t\t}",
    "\t\tAUTOLOG_EVERY(10000, \"[PS-BO] goi ngoai hinh: gui=%d bo=%d (%d%% bo) lam moi moi %d giay\",",
    "\t\t\ts_nPSGui, s_nPSBo, (s_nPSGui + s_nPSBo) > 0 ? (s_nPSBo * 100 / (s_nPSGui + s_nPSBo)) : 0, s_nPSLamMoi);",
    "\t\tif (bPSGui)",
    "\t\t{",
    "\t\t\tint nMaxCount = NPC_SYNC_BROADCAST_LIMIT;",
    "\t\t\tCURREGION.BroadCast(&PlayerSync, sizeof(PLAYER_NORMAL_SYNC), nMaxCount, m_MapX, m_MapY);",
    "\t\t\tfor (j = 0; j < 8; j++)",
    "\t\t\t{",
    "\t\t\t\tint nConRegion = CURREGION.m_nConnectRegion[j];",
    "\t\t\t\tif (nConRegion == -1)",
    "\t\t\t\t\tcontinue;",
    "\t\t\t\tSubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&PlayerSync, sizeof(PLAYER_NORMAL_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);",
    "\t\t\t}",
    "\t\t}",
    "",
])
s = s[:i4] + than_moi + s[i5:]
wr(p, s)
print("KNpc.cpp OK - goi ngoai hinh chi phat khi doi")
