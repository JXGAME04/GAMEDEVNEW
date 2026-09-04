# -*- coding: utf-8 -*-
"""[DECHONG 04/09] Nguoi choi + bot khong dung DE CHONG len nhau nua.
 Nen: 20/08 chu game da cho TAT cong tac 'NPC la tuong' (g_nPbNpcChan = 0, KRegion.cpp) de nguoi/bot/quai
 KHONG chan duong nhau - vi bat len thi giua dam dong dac bot di bo khong nhuc nhich, phai SetPos nhay cho
 (129 cu nhay/11 phut = 'bot toc bien'). Nen KHONG bat lai cong tac do: se hong lai duong di.
 Cach lam o day: GIU nguyen viec khong chan duong, chi DAY BOT DUNG YEN dang chong o sang o trong ke ben,
 bang lenh DI BO that (do_walk) chu khong nhay:
   - chi xet bot dang m_Doing == do_stand (dang danh / ngoi sap / chet thi bo qua);
   - o hien tai co tu 2 NPC tro len (doc m_pNpcRef - bo dem AddRef/DecRef van chay du du cong tac tat);
   - chon o ke ben con TRONG (m_pNpcRef == 0) va di duoc (pb_ODuoc);
   - KHONG BAO GIO dong den nguoi choi that: chi bot buoc ra. May chu keo nguoi that la tai sinh
     canh giat toi/giat lui da sua ca tuan (S13).
   - han muc [Server] BotDeChongMoiNhip con moi nhip (mac dinh 4 = 72 con/giay) de khong dot ngot
     sinh mot loat goi di chuyen; moi con nghi 2 giay giua hai lan day.
   - tat bang [Server] BotDeChong = 0.
 Doc/ghi latin-1 newline=''. Neo kiem dem == 1."""
import io, os

ROOT = r"D:\GAMEDEVNEW_wt_bc\Sources\Core\Src"
MARK = "[DECHONG 04/09]"
INI  = '".\\\\config.ini"'          # trong nguon C++ se la ".\\config.ini"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()

def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)

def rep1(s, old, new, ten):
    n = s.count(old)
    assert n == 1, "%s: tim thay %d cho (can 1)" % (ten, n)
    return s.replace(old, new, 1)

# ---------- 1. KRegion.h: cho doc so NPC dang dung tren mot o ----------
p = os.path.join(ROOT, "KRegion.h")
s = rd(p)
N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    old = "\tlong\t\tGetObstacleCell(int nMapX, int nMapY)"
    new = N.join([
        "\t// %s so NPC dang dung tren mot o. Bo dem m_pNpcRef van duoc AddRef/DecRef" % MARK,
        "\t// day du ngay ca khi g_nPbNpcChan = 0 (nguoi/bot khong chan duong nhau).",
        "\tint\t\tGetNpcCell(int nMapX, int nMapY)",
        "\t{",
        "\t\tif (!m_pNpcRef || nMapX < 0 || nMapY < 0 || nMapX >= m_nWidth || nMapY >= m_nHeight)",
        "\t\t\treturn 0;",
        "\t\treturn (int)m_pNpcRef[nMapY * m_nWidth + nMapX];",
        "\t}",
        old,
    ])
    s = rep1(s, old, new, "KRegion.h GetNpcCell")
    wr(p, s)
    print("KRegion.h     : them GetNpcCell")

# ---------- 2. KPlayerBot.cpp ----------
p = os.path.join(ROOT, "KPlayerBot.cpp")
s = rd(p)
N = "\r\n" if "\r\n" in s else "\n"
if MARK in s:
    print("KPlayerBot.cpp: da va")
    raise SystemExit

# 2a. them truong vao PB_Bot (ngay canh nLachToi cho de doc)
old = "\tunsigned int nLachToi;                    // (20/08) dang DI BO lach ngang toi nhip nay:" + N
new = ("\tunsigned int nDeChongToi;                 // %s vua duoc day ra khoi o chong, nghi toi nhip nay" % MARK) + N + old
s = rep1(s, old, new, "truong nDeChongToi")

# 2b. dat lai khi cap khe bot
old = "\t\tb.nPhamViTick = 0;  b.nBienLogTick = 0;  b.nLachDem = 0;  b.nLachToi = 0;" + N
s = rep1(s, old, old + ("\t\tb.nDeChongToi = 0;\t// %s" % MARK) + N, "dat lai nDeChongToi")

# 2c. ham day bot ra khoi o chong, dat NGAY TRUOC pb_DriveBot
neo = "static void pb_DriveBot(PB_Bot& b)" + N
ham = N.join([
    "// %s Day bot DUNG YEN dang chong o sang o trong ke ben - de dam dong nhin nhu that." % MARK,
    "// Khong bat lai cong tac 'NPC la tuong' (g_nPbNpcChan) vi bat len thi bot ket giua dam dong,",
    "// phai nhay SetPos - dung canh 'bot toc bien' chu game da bat bo 21/08.",
    "// Khong bao gio dong den nguoi choi that: chi bot tu buoc ra. May chu keo nguoi that la de",
    "// tai sinh canh giat toi / giat lui vua sua xong.",
    "static int s_nDeChongBat   = -1;   // [Server] BotDeChong (1 = bat, mac dinh 1)",
    "static int s_nDeChongNhip  = -1;   // [Server] BotDeChongMoiNhip (mac dinh 4 con/nhip)",
    "static unsigned int s_uDeChongNhipNay = 0;",
    "static int s_nDeChongDaDay = 0;    // so con da day trong nhip nay",
    "static int s_nDeChongTong  = 0;    // tong so lan day (nhat ky)",
    "static bool pb_DeChong(PB_Bot& b, int nNpcIdx, int nSub, unsigned int nowAll)",
    "{",
    "\tif (s_nDeChongBat < 0)",
    "\t{",
    "\t\ts_nDeChongBat  = (int)GetPrivateProfileIntA(\"Server\", \"BotDeChong\", 1, " + INI + ");",
    "\t\ts_nDeChongNhip = (int)GetPrivateProfileIntA(\"Server\", \"BotDeChongMoiNhip\", 4, " + INI + ");",
    "\t\tif (s_nDeChongNhip < 1)   s_nDeChongNhip = 1;",
    "\t\tif (s_nDeChongNhip > 100) s_nDeChongNhip = 100;",
    "\t}",
    "\tif (!s_nDeChongBat)",
    "\t\treturn false;",
    "\tif (b.nDeChongToi && nowAll < b.nDeChongToi)\t// vua day xong, de yen",
    "\t\treturn false;",
    "\tif (b.nLachToi && nowAll < b.nLachToi)\t// dang lach ngang, khong xen vao",
    "\t\treturn false;",
    "\tKNpc& npc = Npc[nNpcIdx];",
    "\tif (npc.m_Doing != do_stand)\t// dang danh / ngoi sap / chet / dang di: khong dung toi",
    "\t\treturn false;",
    "\tconst int nR = npc.m_RegionIndex;",
    "\tif (nR < 0 || nR >= MAX_REGION)",
    "\t\treturn false;",
    "\tif (SubWorld[nSub].m_Region[nR].GetNpcCell(npc.m_MapX, npc.m_MapY) < 2)",
    "\t\treturn false;\t// dung mot minh, khong chong ai",
    "\tif (s_uDeChongNhipNay != nowAll)",
    "\t{",
    "\t\ts_uDeChongNhipNay = nowAll;",
    "\t\ts_nDeChongDaDay = 0;",
    "\t}",
    "\tif (s_nDeChongDaDay >= s_nDeChongNhip)",
    "\t\treturn false;\t// han muc moi nhip - trai deu ra, khong dot ngot sinh mot loat goi",
    "\tint nMx = 0, nMy = 0;",
    "\tnpc.GetMpsPos(&nMx, &nMy);",
    "\tstatic const int adx[8] = { 32, -32,   0,   0,  32,  32, -32, -32 };",
    "\tstatic const int ady[8] = {  0,   0,  32, -32,  32, -32,  32, -32 };",
    "\t// moi con bat dau tu mot huong khac -> khong don ca dan ve cung mot phia",
    "\tconst int nBatDau = (int)(npc.m_dwID % 8);",
    "\tfor (int k = 0; k < 8; k++)",
    "\t{",
    "\t\tconst int t = (nBatDau + k) % 8;",
    "\t\tconst int x = nMx + adx[t];",
    "\t\tconst int y = nMy + ady[t];",
    "\t\tif (!pb_ODuoc(nSub, x, y))",
    "\t\t\tcontinue;",
    "\t\tint r2 = -1, mx2 = 0, my2 = 0, ox2 = 0, oy2 = 0;",
    "\t\tSubWorld[nSub].Mps2Map(x, y, &r2, &mx2, &my2, &ox2, &oy2);",
    "\t\tif (r2 < 0 || r2 >= MAX_REGION)",
    "\t\t\tcontinue;",
    "\t\tif (SubWorld[nSub].m_Region[r2].GetNpcCell(mx2, my2) > 0)",
    "\t\t\tcontinue;\t// o ke ben cung da co nguoi",
    "\t\tnpc.SendCommand(do_walk, x, y);\t// DI BO mot buoc, khong nhay",
    "\t\tb.nDeChongToi = nowAll + (unsigned int)(GAME_FPS * 2);",
    "\t\tb.nLachToi    = nowAll + (unsigned int)(GAME_FPS / 2);\t// muon khoa san co de nhanh sau khong de len",
    "\t\ts_nDeChongDaDay++;",
    "\t\ts_nDeChongTong++;",
    "\t\tAUTOLOG_EVERY(10000, \"[DECHONG] da day %d luot bot ra khoi o chong (han %d con/nhip)\",",
    "\t\t\ts_nDeChongTong, s_nDeChongNhip);",
    "\t\treturn true;",
    "\t}",
    "\treturn false;\t// quanh minh kin dac, chiu",
    "}",
    "",
])
s = rep1(s, neo, ham + neo, "ham pb_DeChong")

# 2d. goi trong pb_DriveBot, ngay sau khoi xu ly doi subworld (da co nowAll)
old = ("\t\tb.nJamTick = 0;  b.nLachToi = 0;" + N +
       "\t\tb.nTkDichX = 0;  b.nTkDichY = 0;  b.nTkDichTick = 0;" + N +
       "\t}" + N)
new = old + N + N.join([
    "\t// %s dung yen ma de chong len con khac thi buoc sang o trong ke ben." % MARK,
    "\t// Tra ve true = vua phat lenh di bo: dung nhip tai day de nhanh AI phia duoi",
    "\t// khong phat lenh khac de len (m_Command cua KNpc chi co MOT khe).",
    "\tif (pb_DeChong(b, nNpcIdx, nSub, nowAll))",
    "\t\treturn;",
    "",
])
s = rep1(s, old, new, "goi pb_DeChong")
wr(p, s)
print("KPlayerBot.cpp: them pb_DeChong + goi trong pb_DriveBot")
