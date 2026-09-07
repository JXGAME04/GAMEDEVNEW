#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""goi_va_tukich_chuot_0709.py

[TUKICH 07/09] Chu game bao: "tu kich chuot danh quai -> khi quai chet hoac rot
item thi player tu chay toi vi tri quai hoac item (khong mo WAuto)".

Goc (da doc ma, khong doan):
  * Ui\\autoexec.lua:15 gan LButton -> Mouse_Action() (ShortcutKey.cpp:1681).
    Mouse_Action lam 3 viec, viec thu 3 la VO DIEU KIEN:
        FindSelectNPC  -> LockSomeoneUseSkill / LockSomeoneAction(0)
        FindSelectObject -> LockObjectAction(idx) / (0)
        GotoWhere(x,y,0)   <-- luon chay, gui do_run toi diem con tro
    Con tro van nam nguyen cho con quai, nen cu kich KE TIEP (tu kich chuot)
    roi vao O TRONG hoac vao MON DO vua rot -> keo nhan vat chay toi day,
    toi noi FollowObject con nhat luon.
  * KNpcAI::FollowPeople (KNpcAI.cpp:856) khi muc tieu chet chi dat
    m_nPeopleIdx = 0 roi return, KHONG huy lenh do_run da gui -> quai chet
    giua luc dang duoi thi nhan vat chay not toi xac.

Chu chot (07/09): "Dung yen han, khong nhat" + "Luon bat" (khong them o tuy chon).

Ban va (chi CoreClient.dll, khong dung goi tin / cau truc / vtable):
  1. CoreShell.cpp: chot "o quai vua chet" + ham g_OnLockedTargetDead().
     GotoWhere va LockObjectAction bo qua lenh nham vao vung do.
  2. KNpc.cpp  DoDeath (nhanh #ifndef _SERVER): muc tieu dang khoa chet ->
     goi g_OnLockedTargetDead (huy lenh chay treo + chot o).
  3. KNpcAI.cpp FollowPeople: du phong khi DoDeath khong chay.

Chay:  python ReverseTools/goi_va_tukich_chuot_0709.py [--lui]
"""
import io
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HAUTO = ".truoc_tukich_0709"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def eol(s, crlf):
    """Doi kieu xuong dong cua chuoi neo/chen cho khop tep.

    BAY (.gitattributes text=auto tung sinh \\r\\r\\n): phai chuan hoa ve \\n
    TRUOC roi moi doi sang \\r\\n, khong duoc replace thang."""
    s = s.replace("\r\n", "\n")
    return s.replace("\n", "\r\n") if crlf else s


# ---------------------------------------------------------------- 1. CoreShell
CS_KHOI = '''// [TUKICH 07/09] "tu kich chuot danh quai": moi cu kich chuot trai deu chay
// Mouse_Action (Ui\\autoexec.lua) -> LockObjectAction + GotoWhere VO DIEU KIEN.
// Khi con quai dang danh chet, con tro van nam nguyen cho no, nen cu kich ke
// tiep roi vao O TRONG (hoac vao mon do vua rot) va keo nhan vat chay toi day.
// Chot: quai bi khoa chet -> ghi lai o vua chet; ke tu do, moi lenh DI CHUYEN
// hoac KHOA VAT PHAM nham vao vung quanh o do deu bi bo qua, va moi lan bi bo
// qua thi han cho lai duoc gia han - tu kich chuot bam mai mot cho se khong bao
// gio keo duoc nhan vat toi. Nham RA NGOAI vung (nguoi choi chu dong doi cho),
// hoac dang khoa mot muc tieu con song (lenh ap sat), thi go chot ngay.
// Ngung bam DEAD_TGT_GUARD_MS thi chot tu het -> bam lai la nhat do binh thuong.
#define DEAD_TGT_GUARD_MS		2000
#define DEAD_TGT_GUARD_RANGE	96		// ~3 o: du bao xac quai lan do rot quanh no

static int		s_nDeadTgtX = 0;
static int		s_nDeadTgtY = 0;
static DWORD	s_dwDeadTgtTime = 0;

// TRUE = diem (nX,nY) dang nam trong vung chot -> phai bo qua lenh nay.
static BOOL DeadTargetGuardBlock(int nX, int nY)
{
	if (!s_dwDeadTgtTime)
		return FALSE;

	if (GetTickCount() - s_dwDeadTgtTime > DEAD_TGT_GUARD_MS)
	{
		s_dwDeadTgtTime = 0;
		return FALSE;
	}

	int nMeIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (nMeIdx > 0 && nMeIdx < MAX_NPC && Npc[nMeIdx].m_nPeopleIdx > 0)
	{
		s_dwDeadTgtTime = 0;	// dang khoa muc tieu con song -> day la lenh ap sat
		return FALSE;
	}

	if (g_GetDistance(nX, nY, s_nDeadTgtX, s_nDeadTgtY) > DEAD_TGT_GUARD_RANGE)
	{
		s_dwDeadTgtTime = 0;	// nguoi choi nham cho khac -> tra lai dieu khien
		return FALSE;
	}

	s_dwDeadTgtTime = GetTickCount();	// van nham vao xac quai -> giu chot
	AUTOLOG_EVERY(500, "[TUKICH] bo qua lenh nham o quai vua chet: diem=(%d,%d) oquai=(%d,%d)",
		nX, nY, s_nDeadTgtX, s_nDeadTgtY);
	return TRUE;
}

// Muc tieu dang khoa vua chet: huy lenh di chuyen dang treo (khong co no thi
// nhan vat chay not toi xac quai) va bat chot o vua chet.
// Goi tu KNpc::DoDeath va KNpcAI::FollowPeople (deu trong #ifndef _SERVER).
void g_OnLockedTargetDead(int nIdxDead)
{
	if (nIdxDead <= 0 || nIdxDead >= MAX_NPC)
		return;

	Npc[nIdxDead].GetMpsPos(&s_nDeadTgtX, &s_nDeadTgtY);
	s_dwDeadTgtTime = GetTickCount();

	int nMeIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (nMeIdx <= 0 || nMeIdx >= MAX_NPC)
		return;

	// "di toi cho minh dang dung" = dung lai; dung dung khuon san co o
	// KNpcAI::FollowPeople (nhanh gap NPC doi thoai).
	if (Npc[nMeIdx].m_Doing == do_walk || Npc[nMeIdx].m_Doing == do_run)
	{
		int nMeX = 0, nMeY = 0;
		Npc[nMeIdx].GetMpsPos(&nMeX, &nMeY);
		Npc[nMeIdx].SendCommand(do_walk, nMeX, nMeY);
		SendClientCmdWalk(nMeX, nMeY);
		AUTOLOG("[TUKICH] muc tieu %d chet luc dang duoi -> dung tai (%d,%d) oquai=(%d,%d)",
			nIdxDead, nMeX, nMeY, s_nDeadTgtX, s_nDeadTgtY);
	}
}

'''

CS_1_CU = "int KCoreShell::LockObjectAction(int nTargetIndex)\n"
CS_1_MOI = CS_KHOI + CS_1_CU

CS_2_CU = (
    "\tif (nTargetIndex <= 0)\t\n"
    "\t\tNpc[nIndex].m_nObjectIdx = 0;\n"
)
CS_2_MOI = (
    "\t// [TUKICH 07/09] mon do vua rot ngay cho quai chet: khong khoa, de cu kich\n"
    "\t// chuot ke tiep khong keo nhan vat chay toi nhat.\n"
    "\tif (nTargetIndex > 0 && nTargetIndex < MAX_OBJECT)\n"
    "\t{\n"
    "\t\tint nObjX = 0, nObjY = 0;\n"
    "\t\tObject[nTargetIndex].GetMpsPos(&nObjX, &nObjY);\n"
    "\t\tif (DeadTargetGuardBlock(nObjX, nObjY))\n"
    "\t\t{\n"
    "\t\t\tNpc[nIndex].m_nObjectIdx = 0;\n"
    "\t\t\treturn 1;\n"
    "\t\t}\n"
    "\t}\n"
    "\n"
) + CS_2_CU

CS_3_CU = (
    "void KCoreShell::GotoWhere(int x, int y, int mode)\n"
    "{\n"
    "\tif (mode < 0 || mode > 2)\n"
    "\t\treturn;\n"
)
CS_3_MOI = CS_3_CU + (
    "\n"
    "\t// [TUKICH 07/09] bo qua cu kich roi vao o con quai vua chet\n"
    "\t// (xem chu thich o DEAD_TGT_GUARD_MS phia tren LockObjectAction).\n"
    "\t{\n"
    "\t\tint nGX = x, nGY = y, nGZ = 0;\n"
    "\t\tg_ScenePlace.ViewPortCoordToSpaceCoord(nGX, nGY, nGZ);\n"
    "\t\tif (DeadTargetGuardBlock(nGX, nGY))\n"
    "\t\t\treturn;\n"
    "\t}\n"
)

# ------------------------------------------------------------------- 2. KNpc
KN_1_CU = (
    "#ifndef _SERVER\n"
    "\tm_ClientDoing = cdo_death;\n"
    "\tif (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx == m_Index)\n"
    "\t{\n"
    "\t\tNpc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;\n"
    "\t}\n"
    "#endif\n"
)
KN_1_MOI = (
    "#ifndef _SERVER\n"
    "\tm_ClientDoing = cdo_death;\n"
    "\tif (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx == m_Index)\n"
    "\t{\n"
    "\t\t// [TUKICH 07/09] muc tieu dang khoa vua chet: truoc day chi xoa khoa,\n"
    "\t\t// lenh do_run da gui van con hieu luc -> nhan vat chay not toi xac quai,\n"
    "\t\t// va cu kich chuot ke tiep (tu kich chuot) lai keo toi xac / do roi.\n"
    "\t\t// g_OnLockedTargetDead (CoreShell.cpp) dung tai cho + chot o vua chet.\n"
    "\t\tg_OnLockedTargetDead(m_Index);\n"
    "\t\tNpc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;\n"
    "\t}\n"
    "#endif\n"
)

KN_0_CU = "void KNpc::DoDeath(int nMode/* = 0*/, int nAttacker)\n"
KN_0_MOI = (
    "#ifndef _SERVER\n"
    "// [TUKICH 07/09] dinh nghia o CoreShell.cpp (tep chi bien dich cho client).\n"
    "extern void g_OnLockedTargetDead(int nIdxDead);\n"
    "#endif\n"
    "\n"
) + KN_0_CU

# ------------------------------------------------------------------ 3. KNpcAI
KA_0_CU = (
    "#ifndef _SERVER\n"
    "void KNpcAI::FollowPeople(int nIdx)\n"
)
KA_0_MOI = (
    "#ifndef _SERVER\n"
    "// [TUKICH 07/09] dinh nghia o CoreShell.cpp (tep chi bien dich cho client).\n"
    "extern void g_OnLockedTargetDead(int nIdxDead);\n"
    "\n"
    "void KNpcAI::FollowPeople(int nIdx)\n"
)

KA_1_CU = (
    "\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)\n"
    "\t{\n"
    "\t\tNpc[m_nIndex].m_nPeopleIdx = 0;\n"
    "\t\treturn;\n"
    "\t}\n"
    "\n"
    "\tif (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive)\n"
)
KA_1_MOI = (
    "\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)\n"
    "\t{\n"
    "\t\t// [TUKICH 07/09] du phong khi KNpc::DoDeath khong chay (muc tieu vao\n"
    "\t\t// trang thai chet qua duong dong bo): dung tai cho + chot o vua chet.\n"
    "\t\tg_OnLockedTargetDead(nIdx);\n"
    "\t\tNpc[m_nIndex].m_nPeopleIdx = 0;\n"
    "\t\treturn;\n"
    "\t}\n"
    "\n"
    "\tif (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive)\n"
)

VIEC = [
    ("Sources/Core/Src/CoreShell.cpp", [
        ("CoreShell:khoi chot", CS_1_CU, CS_1_MOI),
        ("CoreShell:LockObject", CS_2_CU, CS_2_MOI),
        ("CoreShell:GotoWhere", CS_3_CU, CS_3_MOI),
    ]),
    ("Sources/Core/Src/KNpc.cpp", [
        ("KNpc:khai bao", KN_0_CU, KN_0_MOI),
        ("KNpc:DoDeath", KN_1_CU, KN_1_MOI),
    ]),
    ("Sources/Core/Src/KNpcAI.cpp", [
        ("KNpcAI:khai bao", KA_0_CU, KA_0_MOI),
        ("KNpcAI:FollowPeople", KA_1_CU, KA_1_MOI),
    ]),
]


def lui():
    for duong, _ in VIEC:
        p = os.path.join(ROOT, duong)
        h = p + HAUTO
        if os.path.exists(h):
            ghi(p, doc(h))
            print("LUI  %s" % duong)
        else:
            print("KHONG CO BAN LUU %s" % h)
    return 0


def main():
    if "--lui" in sys.argv:
        return lui()

    loi = 0
    for duong, buoc in VIEC:
        p = os.path.join(ROOT, duong)
        if not os.path.exists(p):
            print("KHONG CO TEP %s" % p)
            return 1
        goc = doc(p)
        src = goc
        crlf = ("\r\n" in goc)
        print("%s (%s)" % (duong, "CRLF" if crlf else "LF"))
        for nhan, cu, moi in buoc:
            cu = eol(cu, crlf)
            moi = eol(moi, crlf)
            if moi in src:
                print("  BO QUA %-20s (da co)" % nhan)
                continue
            n = src.count(cu)
            if n != 1:
                print("  LOI %-22s khop %d lan (can dung 1)" % (nhan, n))
                loi += 1
                continue
            src = src.replace(cu, moi, 1)
            print("  OK    %-20s" % nhan)
        if src == goc:
            print("  (khong doi gi)")
            continue
        if loi:
            print("  CO LOI -> KHONG GHI %s" % duong)
            continue
        h = p + HAUTO
        if not os.path.exists(h):
            ghi(h, goc)
        if hi(src) != hi(goc):
            print("  LOI: so byte >127 doi %d -> %d, KHONG GHI" % (hi(goc), hi(src)))
            loi += 1
            continue
        ghi(p, src)
        print("  DA GHI (byte >127 giu nguyen %d)" % hi(src))

    return 1 if loi else 0


if __name__ == "__main__":
    sys.exit(main())
