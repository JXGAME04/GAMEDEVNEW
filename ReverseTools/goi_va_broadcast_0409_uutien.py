# -*- coding: utf-8 -*-
"""[BC 04/09 UUTIEN] Sua TAN GOC hai viec cung luc:
 (1) "Khong thay dich va chieu khi dong nguoi": nguyen nhan la tran MAX_BROADCAST_COUNT 100 tinh cho MOI node
     (ke ca BOT khong co ket noi) nen o tran Tong Kim ~258 bot an sach suat truoc khi toi nguoi that -> client
     chi nhan ~44 goi/giay -> dich khong hien, chieu khong thay, chet khong biet vi sao.
 (2) Nhung bo han che do ra (F4) thi client nhan 21.000-94.844 goi/giay -> nghen socket -> may chu vut byte -> sap.
 Cach dung: BO HAN DONG THEO TUNG NGUOI CHOI, CO UU TIEN.
   - Bot khong con an suat: chi tru ngan sach khi THAT SU gui (F4).
   - Moi client co han muc goi VI TRI moi giay ([Server] BroadCastGoiToiDa, mac dinh 1500).
     Vuot han muc thi CHI bo goi vi tri (75 syncplayermin, 77 syncnpcmin, 85 npcwalk, 86 npcrun) - loai nay lap lai
     lien tuc nen bo mot nhip khong mat gi.
   - Goi CHIEN DAU khong bao gio bi bo: 76 syncnpc, 78 syncnpcminplayer, 84 npcremove, 87 npcattack, 88 npcmagic,
     91 npchurt, 92 npcdeath, 93 chgcurcamp, 95 skillcast, 148 castskilldirectly, 207 show_damage, va moi loai khac.
 Ket qua mong doi: dich hien ra (van co goi vi tri, toi 1500/giay thay vi 44), chieu luon thay, ma bang thong
 ~150 KB/giay/client thay vi 9,6 MB/giay.
 Doc/ghi latin-1 newline=''. Neo kiem dem == 1."""
import io, os

ROOT = r"D:\GAMEDEVNEW_wt_bc\Sources\Core\Src"
MARK = "[BC 04/09 UUTIEN]"

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
    print("da va"); raise SystemExit

# 1. bien han muc theo nguoi choi (dat canh cac bien dem san co)
old = "int g_nBCTheoDoi = 0;\t\t\t\t// chi so Player dang theo doi (client nhan nhieu nhat 10 giay truoc)" + N
new = (old +
       "// %s han muc goi VI TRI moi giay cho TUNG client; goi chien dau khong bao gio bi bo." % MARK + N +
       "static int   g_anBCGiay[MAX_PLAYER];\t// so goi vi tri da gui cho tung nguoi trong giay hien tai" + N +
       "static DWORD g_uBCMocGiay = 0;" + N +
       "static int   g_nBCBoTrongGiay = 0;\t// so goi vi tri bi bo (de in ra nhat ky)" + N +
       "static int   g_nBCHanMuc = -1;\t\t// doc mot lan tu config.ini" + N +
       "// loai goi VI TRI - lap lai lien tuc nen bo mot nhip khong mat gi" + N +
       "static bool BC_LaGoiViTri(int nLoai)" + N +
       "{" + N +
       "\treturn (nLoai == 75 || nLoai == 77 || nLoai == 85 || nLoai == 86);" + N +
       "}" + N)
s = rep1(s, old, new, "bien han muc")

# 2. trong BroadCast: doc han muc + moc giay
old = "\tconst int nBCLoaiGoi = (pBuffer && dwSize > 0) ? (int)((const BYTE*)pBuffer)[0] : 0;" + N
new = (old +
       "\tif (g_nBCHanMuc < 0)\t// %s han muc goi vi tri moi giay cho mot client" % MARK + N +
       "\t{" + N +
       "\t\tg_nBCHanMuc = (int)GetPrivateProfileIntA(\"Server\", \"BroadCastGoiToiDa\", 1500, \".\\\\config.ini\");" + N +
       "\t\tif (g_nBCHanMuc < 100) g_nBCHanMuc = 100;" + N +
       "\t\tif (g_nBCHanMuc > 20000) g_nBCHanMuc = 20000;" + N +
       "\t}" + N +
       "\t{" + N +
       "\t\tconst DWORD uBCNay = GetTickCount() / 1000;" + N +
       "\t\tif (uBCNay != g_uBCMocGiay)" + N +
       "\t\t{" + N +
       "\t\t\tg_uBCMocGiay = uBCNay;" + N +
       "\t\t\tmemset(g_anBCGiay, 0, sizeof(g_anBCGiay));" + N +
       "\t\t}" + N +
       "\t}" + N +
       "\tconst bool bBCBoDuoc = BC_LaGoiViTri(nBCLoaiGoi);" + N)
s = rep1(s, old, new, "doc han muc")

# 3. than vong lap: F4 (chi tru khi that gui) + han muc theo nguoi
old_body = None
i = s.index("\twhile(pNode && nMaxCount > 0)" + N)
j = s.index("\t\tpNode = pNext;" + N + "\t}" + N, i) + len("\t\tpNode = pNext;" + N + "\t}" + N)
than_moi = N.join([
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
    "\t\t\tif (nDX < 0) nDX = -nDX;\t// [BC 03/09 b] tri tuyet doi hai chieu",
    "\t\t\tif (nDY < 0) nDY = -nDY;",
    "\t\t\tif (Player[nPlayerIndex].m_nNetConnectIdx >= 0 && (nDX > s_nBCTam || nDY > s_nBCTam))",
    "\t\t\t\tnBCNgoaiTam++;",
    "\t\t\tif (Player[nPlayerIndex].m_nNetConnectIdx >= 0 ",
    "\t\t\t\t&& nDX <= s_nBCTam && nDY <= s_nBCTam",
    "\t\t\t\t&& Player[nPlayerIndex].m_bSleepMode == FALSE)",
    "\t\t\t{",
    "\t\t\t\t// %s han muc CHI ap cho goi vi tri; goi chien dau (chieu, trung don, chet, hien sat thuong) luon gui." % MARK,
    "\t\t\t\tif (bBCBoDuoc && g_anBCGiay[nPlayerIndex] >= g_nBCHanMuc)",
    "\t\t\t\t{",
    "\t\t\t\t\tg_nBCBoTrongGiay++;",
    "\t\t\t\t}",
    "\t\t\t\telse",
    "\t\t\t\t{",
    "\t\t\t\t\tg_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pBuffer, dwSize);",
    "\t\t\t\t\tif (bBCBoDuoc)",
    "\t\t\t\t\t\tg_anBCGiay[nPlayerIndex]++;",
    "\t\t\t\t\tnMaxCount--;\t// %s CHI tru khi THAT SU gui: bot (khong ket noi) khong con an suat cua nguoi that" % MARK,
    "\t\t\t\t\tnF4Gui++;",
    "\t\t\t\t\tBC_DemNguoiNhan(nPlayerIndex, nBCLoaiGoi, (int)dwSize);",
    "\t\t\t\t\tif (nPlayerIndex == g_nBCTheoDoi && m_nIndex >= 0 && m_nIndex < BC_MAX_VUNG)",
    "\t\t\t\t\t\tg_abyBCVungGui[m_nIndex] = 1;",
    "\t\t\t\t}",
    "\t\t\t}",
    "\t\t}",
    "\t\tpNode = pNext;",
    "\t}",
    "",
])
s = s[:i] + than_moi + s[j:]

# 4. in so goi bi bo vao dong [BC-DEM]
old = "\t\t\tAUTOLOG(\"[BC-DEM] 10s: goi=%d gui=%d cat_vi_het_ngan_sach=%d node_duyet=%d ngoai_tam=%d tam=%d\", s_nF4Goi, s_nF4Gui, s_nF4Bo, s_nF4Duyet, s_nBCNgoaiTam, s_nBCTam);" + N
new = ("\t\t\tAUTOLOG(\"[BC-DEM] 10s: goi=%d gui=%d cat_vi_het_ngan_sach=%d node_duyet=%d ngoai_tam=%d tam=%d bo_vi_tri=%d han_muc=%d/giay\"," + N +
       "\t\t\t\ts_nF4Goi, s_nF4Gui, s_nF4Bo, s_nF4Duyet, s_nBCNgoaiTam, s_nBCTam, g_nBCBoTrongGiay, g_nBCHanMuc);" + N +
       "\t\t\tg_nBCBoTrongGiay = 0;" + N)
s = rep1(s, old, new, "in bo_vi_tri")
wr(p, s)
print("KRegion.cpp OK - F4 tro lai + han muc goi vi tri theo tung client")
