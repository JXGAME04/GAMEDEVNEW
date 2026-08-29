# -*- coding: utf-8 -*-
"""BAN DONG HANH - G2: dau day module KPlayerPartner vao engine (9 moi).

Moi moi vá:
  - assert neo DUY NHAT truoc khi thay
  - idempotent: co nhan [BDH 27/08] thi bo qua
  - doc/ghi latin-1 newline='' de giu nguyen byte TCVN3/CRLF
"""
import io
import os
import shutil

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
NHAN = "[BDH 27/08]"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def va(ten_tep, neo, moi, dem=1):
    p = os.path.join(SRC, ten_tep)
    s = doc(p)
    if moi in s:
        print("  da co, bo qua:", ten_tep)
        return
    assert s.count(neo) == dem, (ten_tep, s.count(neo), neo[:60])
    if not os.path.exists(p + ".truoc_bdh_2708"):
        shutil.copyfile(p, p + ".truoc_bdh_2708")
    ghi(p, s.replace(neo, moi, 1))
    print("  OK:", ten_tep)


# ---- 1. KNpc.h: 2 truong chu so huu (canh m_bNoRevive, trong #ifdef _SERVER) ----
va("KNpc.h",
   "\tBYTE\t\t\tm_bNoRevive;\t\t\t// Linux +0x1824 (AddNpcEx tham so 7): chet la bien mat\r\n#endif",
   "\tBYTE\t\t\tm_bNoRevive;\t\t\t// Linux +0x1824 (AddNpcEx tham so 7): chet la bien mat\r\n"
   "\t// " + NHAN + " he ban dong hanh (KPlayerPartner.cpp): chu so huu cua NPC kind_partner\r\n"
   "\tint\t\t\t\tm_nPartnerOwner;\t\t// player idx cua chu (0 = khong phai partner)\r\n"
   "\tint\t\t\t\tm_nPartnerNo;\t\t\t// con so may cua chu (1..3)\r\n"
   "#endif")

# ---- 2. KPlayerTask.h: MAX_TASK 4600 -> 5200 ----
va("KPlayerTask.h",
   "#define\t\tMAX_TASK\t\t\t\t4600",
   "// " + NHAN + " 4600 -> 5200: cap dai 4600..5099 cho he BAN DONG HANH\r\n"
   "// (so do o KPlayerPartner.h) + sua loi task 5100 (npc_chuyensinh.lua) dang bi\r\n"
   "// SetSaveVal nuot im lang vi vuot tran cu. Dinh dang blob (id,value) THUA nen\r\n"
   "// khong doi dinh dang DB. MAX_TEMP_TASK giu 4600 (tiet kiem RAM).\r\n"
   "#define\t\tMAX_TASK\t\t\t\t5200")

# ---- 3a. KPlayer.h: include ----
va("KPlayer.h",
   '#include "../KMeridian.h"',
   '#include "../KMeridian.h"\r\n#include "KPlayerPartner.h"\t// ' + NHAN + ' he ban dong hanh')

# ---- 3b. KPlayer.h: member (sau m_cMeridian) ----
va("KPlayer.h",
   "\tKMeridian\t\tm_cMeridian;\t\t\t\t\t//Kinh mach",
   "\tKMeridian\t\tm_cMeridian;\t\t\t\t\t//Kinh mach\r\n"
   "#ifdef _SERVER\r\n"
   "\tKPartnerSys\t\tm_cPartner;\t\t\t\t\t// " + NHAN + " he ban dong hanh\r\n"
   "#endif")

# ---- 4. KPlayerSet.cpp: init ----
va("KPlayerSet.cpp",
   "\t\tPlayer[i].m_cMeridian.Init(i);",
   "\t\tPlayer[i].m_cMeridian.Init(i);\r\n"
   "\t\tPlayer[i].m_cPartner.Init(i);\t// " + NHAN)

# ---- 5a. KNpcAI.cpp: extern ----
va("KNpcAI.cpp",
   '#include "KNpcAI.h"',
   '#include "KNpcAI.h"\r\n'
   '#ifdef _SERVER\r\n'
   'extern void Partner_ProcessAI(int nNpcIdx);\t// ' + NHAN + ' KPlayerPartner.cpp\r\n'
   '#endif')

# ---- 5b. KNpcAI.cpp: nhanh partner TRUOC nhanh van tieu ----
va("KNpcAI.cpp",
   "\tif (Npc[m_nIndex].Owner[0] && Npc[m_nIndex].m_bNpcFollowFindPath)",
   "\t// " + NHAN + " NPC ban dong hanh: AI rieng (theo chu + 4 tinh cach 21..24),\r\n"
   "\t// van huong dieu tiet m_NextAITime/m_AIMAXTime nhu AI thuong.\r\n"
   "\tif (Npc[m_nIndex].m_Kind == kind_partner)\r\n"
   "\t{\r\n"
   "\t\tint nCurTimeP = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime;\r\n"
   "\t\tif (Npc[m_nIndex].m_NextAITime <= nCurTimeP)\r\n"
   "\t\t{\r\n"
   "\t\t\tNpc[m_nIndex].m_NextAITime = nCurTimeP + Npc[m_nIndex].m_AIMAXTime;\r\n"
   "\t\t\tPartner_ProcessAI(m_nIndex);\r\n"
   "\t\t}\r\n"
   "\t\treturn;\r\n"
   "\t}\r\n"
   "\tif (Npc[m_nIndex].Owner[0] && Npc[m_nIndex].m_bNpcFollowFindPath)")

# ---- 6. KNpc.cpp: OnDeath nhanh kind_partner (hon me) ----
va("KNpc.cpp",
   "\t\telse\t\r\n\t\t{\r\n\t\t\t//Not Finish\r\n\t\t}",
   "\t\telse\t\r\n\t\t{\r\n"
   "\t\t\t// " + NHAN + " dong hanh HON ME: khong hoi sinh, khong go theo duong thuong.\r\n"
   "\t\t\t// Partner_OnNpcDeath dat phat PUNISH_TIME + goi partner_action.lua OnDeath.\r\n"
   "#ifdef _SERVER\r\n"
   "\t\t\tPartner_OnNpcDeath(m_Index);\r\n"
   "#endif\r\n"
   "\t\t}")

# ---- 6b. KNpc.cpp: extern cho Partner_OnNpcDeath (dat sau include dau tien) ----
va("KNpc.cpp",
   '#include "KCore.h"',
   '#include "KCore.h"\r\n'
   '#ifdef _SERVER\r\n'
   'extern void Partner_OnNpcDeath(int nNpcIdx);\t// ' + NHAN + ' KPlayerPartner.cpp\r\n'
   '#endif')

# ---- 7. KNpc.cpp: va bug ChangeWorld ghi de Player[0] ----
va("KNpc.cpp",
   "\tPlayer[m_nPlayerIdx].m_nPrePayMoney = 0;",
   "\tif (IsPlayer())\t// " + NHAN + " NPC thuong co m_nPlayerIdx=0 -> truoc day ghi de Player[0]\r\n"
   "\t\tPlayer[m_nPlayerIdx].m_nPrePayMoney = 0;")

# ---- 8. CoreServerShell.cpp: nhip he partner canh PB_Breathe ----
va("CoreServerShell.cpp",
   "\tPB_Breathe();",
   "\tPB_Breathe();\r\n"
   "\tKPartner_Breathe();\t// " + NHAN + " he ban dong hanh (KPlayerPartner.cpp)")

# (KPartner_Breathe da khai bao trong KPlayerPartner.h, KPlayer.h keo vao
#  -> CoreServerShell.cpp include KPlayer.h la thay, khong can extern rieng)

print("XONG b04")
