# -*- coding: utf-8 -*-
# [BOTNOI dot 4c 06/09] Chu game: "loi bot vao bang nguoi choi hien thi thanh bang chu".
# GOC: ban sao relay JX2 (KTongJX2.h:24) dung figure 0 bang chu / 1 truong lao / 2 doi truong /
# 3 bang chung / 4 an si; con KPlayerTong::m_nFigure (dong bo len moi client qua
# PlayerSync.TongFigure, KNpc.cpp:6705) dung enum JX1 GameDataDef.h:1616: 0 MEMBER / 1 MANAGER /
# 2 DIRECTOR / 3 MASTER. pb_BangDongBo (KPlayerBot.cpp ~8271) chep THO t.m_nFigure = btFigure ->
# bot bang chung (3) thanh MASTER (3) = "Bang chu" tren client; bang chu bot (0) thanh MEMBER.
# bot.log 06/09: 2952 dong "chuc vu 3" (TESTGAME) = tat ca deu hien Bang chu.
# SUA: pb_FigureJx2SangJx1 + dung o ca so sanh bMoi lan gan.
# AP SAU goi_va_botnoi_dot4b_0609.py. Idempotent. Chi ASCII.
import io, os, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_botnoi"
P = os.path.join(ROOT, "Sources", "Core", "Src", "KPlayerBot.cpp")

s = io.open(P, "r", encoding="latin-1", newline="").read()
truoc = sum(1 for c in s if ord(c) > 127)
crlf = "\r\n" in s
n = 0

def ap(ten, cu, moi):
    global s, n
    if crlf:
        cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
    if moi in s:
        print("  [=] %s da ap tu truoc" % ten); return
    if s.count(cu) != 1:
        print("LOI: neo %s khop %d cho (can 1)" % (ten, s.count(cu))); sys.exit(1)
    s = s.replace(cu, moi); n += 1
    print("  [+] %s" % ten)

ap("H23 doi figure JX2 -> JX1 khi dong bo bang cho bot",
 '\tb.uBangMatTu = 0;\n'
 '\tconst int nCamp = (int)pT->btCamp;\n'
 '\tconst int bMoi = (!t.m_nFlag || t.m_nCamp != nCamp || t.m_szName[0] == 0\n'
 '\t               || strcmp(t.m_szName, pT->szName) != 0\n'
 '\t               || t.m_nFigure != (int)pMe->btFigure);\n'
 '\tif (bMoi)\n'
 '\t{\n'
 '\t\tt.m_nFlag   = 1;\n'
 '\t\tt.m_nFigure = (int)pMe->btFigure;\n',
 '\tb.uBangMatTu = 0;\n'
 '\tconst int nCamp = (int)pT->btCamp;\n'
 '\t// [BANGFIG 06/09] chu game: "loi bot vao bang nguoi choi hien thi thanh bang chu". Ban sao\n'
 '\t// relay JX2 (KTongJX2.h:24): 0 bang chu / 1 truong lao / 2 doi truong / 3 bang chung / 4 an\n'
 '\t// si; KPlayerTong::m_nFigure (len client qua PlayerSync.TongFigure, KNpc.cpp:6705) la enum JX1\n'
 '\t// GameDataDef.h:1616: 0 MEMBER / 1 MANAGER / 2 DIRECTOR / 3 MASTER. Chep tho: bang chung 3\n'
 '\t// = MASTER -> moi bot vao bang nguoi choi deu hien "Bang chu"; bang chu bot 0 = MEMBER.\n'
 '\tint nFigJx1 = enumTONG_FIGURE_MEMBER;\n'
 '\tswitch ((int)pMe->btFigure)\n'
 '\t{\n'
 '\tcase 0:  nFigJx1 = enumTONG_FIGURE_MASTER;   break;   // bang chu\n'
 '\tcase 1:  nFigJx1 = enumTONG_FIGURE_DIRECTOR; break;   // truong lao\n'
 '\tcase 2:  nFigJx1 = enumTONG_FIGURE_MANAGER;  break;   // doi truong / duong chu\n'
 '\tdefault: nFigJx1 = enumTONG_FIGURE_MEMBER;   break;   // 3 bang chung, 4 an si\n'
 '\t}\n'
 '\tconst int bMoi = (!t.m_nFlag || t.m_nCamp != nCamp || t.m_szName[0] == 0\n'
 '\t               || strcmp(t.m_szName, pT->szName) != 0\n'
 '\t               || t.m_nFigure != nFigJx1);\n'
 '\tif (bMoi)\n'
 '\t{\n'
 '\t\tt.m_nFlag   = 1;\n'
 '\t\tt.m_nFigure = nFigJx1;\n')

ap("H23b log ghi ca figure JX2 goc",
 '\t\tpb_Log("[BotBang] %s dong bo bang \'%s\' (id %u, camp %d, chuc vu %d) tu ban sao relay\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, t.m_szName, (unsigned int)dwId, nCamp, t.m_nFigure);\n',
 '\t\tpb_Log("[BotBang] %s dong bo bang \'%s\' (id %u, camp %d, chuc vu JX2 %d -> JX1 %d) tu ban sao relay\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, t.m_szName, (unsigned int)dwId, nCamp,\n'
 '\t\t       (int)pMe->btFigure, t.m_nFigure);\n')

if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("da ap %d hunk; high-byte truoc=%d sau=%d %s" % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
