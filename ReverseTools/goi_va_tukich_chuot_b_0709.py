#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""goi_va_tukich_chuot_b_0709.py

[TUKICH 07/09 b] Chu bao ban va a (e2a03fc0) "fix chua dung cho - kich danh xong
van chay toi vi tri quai".

Do ban dang chay (`CoreClient.dll` 673a5275, 12:29) - CO du ma TUKICH nhung
`jx_auto.log` KHONG co lay mot dong `[TUKICH]` nao, trong khi AUTOLOG client van
ghi binh thuong (CHECKOBJ-ENTRY 19, E4_SKILL_ABORT 11, NET-DEATH 57).

Goc that (ban va a bo lot):
  Lenh di chuyen cua NGUOI CHOI THAT khong nam o m_Command ma o KHE RIENG
  `s_S13Move[m_Index]` (KNpc.cpp:133, chu thich [S13]); KNpc::SendCommand day
  moi do_walk/do_run cua nguoi choi that vao khe do va THOAT (KNpc.cpp:5980).
  Khe nay KHONG bi xoa khi m_Command duoc tieu thu (KNpc.cpp:1238), nen no
  SONG SOT qua ca hoat anh danh: danh xong, ProcCommand (KNpc.cpp:1109) thi hanh
  no va nhan vat chay toi cho con quai da chet.
  Ban va a chi gui lenh dung khi `m_Doing == do_walk || do_run`. Luc giet duoc
  quai thi m_Doing dang la hoat anh danh -> khong gui gi -> khe s_S13Move con
  nguyen -> van chay toi xac. Va do nhanh do khong chay nen cung khong co dong log.

Sua:
  1. KNpc.cpp: them `S13_HuyLenhDiToiXac()` (canh `S13_ClearCmd` san co, cho duy
     nhat nhin thay s_S13Move): vut CA khe di chuyen lan m_Command khi dich cua
     lenh treo - hoac dich dang chay m_DesX/m_DesY - nam quanh xac quai.
     Chay di cho khac (ne, kite) thi GIU, khong dung cua nguoi choi.
  2. CoreShell.cpp `g_OnLockedTargetDead`: goi ham tren (bo dieu kien m_Doing),
     vut xong moi gui "di toi cho minh dang dung" de may chu dung theo.
     Them log BAT CHOT chay MOI LAN de lan sau doc log la biet ngay.
  3. KNpcAI.cpp `FollowPeople`: nhanh `CheckNpc` (nhanh THAT SU bat muc tieu chet,
     vi CheckNpc goi IsAlive) cung bat chot - ban va a dat nham o nhanh
     `m_Doing == do_death` phia sau, khong bao gio toi.

Chay:  python ReverseTools/goi_va_tukich_chuot_b_0709.py [--lui]
"""
import io
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HAUTO = ".truoc_tukich_b_0709"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def eol(s, crlf):
    s = s.replace("\r\n", "\n")
    return s.replace("\n", "\r\n") if crlf else s


# ------------------------------------------------------------------- 1. KNpc
KN_CU = (
    "\t\tNpc[nIdx].SendCommand(do_none, 0, 0, 0);\t// m_Command private: do_none qua het cua chan va xoa khe\n"
    "}\n"
)
KN_MOI = KN_CU + '''
// [TUKICH 07/09 b] Muc tieu dang khoa vua CHET: vut lenh DI CHUYEN dang treo cua chinh
// minh neu no nham vao ngay cho con quai chet. Phai lam o day vi s_S13Move la static cua
// tep nay. Vi sao can: lenh chay/di cua nguoi choi that nam o khe rieng s_S13Move (xem
// chu thich [S13] dau tep) nen no SONG SOT qua ca hoat anh danh - danh xong, ProcCommand
// thi hanh no va nhan vat chay toi xac quai. Ban va 07/09 a chi kiem
// m_Doing == do_run/do_walk nen bo lot dung truong hop nay (luc do dang la hoat anh danh).
// CHI vut khi DICH nam quanh xac: nguoi choi dang chay di cho khac (ne, kite) thi giu
// nguyen y dinh cua ho. TRUE = da vut -> ben goi nen bao may chu dung theo.
BOOL S13_HuyLenhDiToiXac(int nIdx, int nXacX, int nXacY, int nBanKinh)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return FALSE;

	BOOL bNham = FALSE;
	NPC_COMMAND& sMove = s_S13Move[nIdx];
	if (sMove.CmdKind == do_walk || sMove.CmdKind == do_run)
		bNham = (g_GetDistance(sMove.Param_X, sMove.Param_Y, nXacX, nXacY) <= nBanKinh);

	// dang chay do lenh cu da thi hanh: dich hien tai nam ngay cho xac
	if (!bNham && (Npc[nIdx].m_Doing == do_walk || Npc[nIdx].m_Doing == do_run))
		bNham = (g_GetDistance(Npc[nIdx].m_DesX, Npc[nIdx].m_DesY, nXacX, nXacY) <= nBanKinh);

	if (!bNham)
		return FALSE;

	S13_ClearCmd(nIdx);		// xoa CA khe di chuyen lan m_Command
	return TRUE;
}
'''

# -------------------------------------------------------------- 2. CoreShell
CS_CU = '''	int nMeIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
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
CS_MOI = '''	int nMeIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (nMeIdx <= 0 || nMeIdx >= MAX_NPC)
		return;

	// [TUKICH 07/09 b] Ban va a kiem m_Doing == do_walk/do_run o day: SAI CHO. Luc giet
	// duoc quai thi m_Doing dang la hoat anh danh, nen khong gui gi - trong khi lenh chay
	// toi con quai VAN NAM trong khe rieng s_S13Move va se duoc ProcCommand thi hanh ngay
	// sau khi danh xong => "kich danh xong van chay toi vi tri quai".
	// Gio vut thang khe do (S13_HuyLenhDiToiXac o KNpc.cpp), roi moi bao may chu dung.
	BOOL bHuyLenh = S13_HuyLenhDiToiXac(nMeIdx, s_nDeadTgtX, s_nDeadTgtY, DEAD_TGT_GUARD_RANGE);
	if (bHuyLenh)
	{
		// "di toi cho minh dang dung" = dung lai (khuon san co o KNpcAI::FollowPeople
		// nhanh gap NPC doi thoai); can ca hai ve vi may chu giu lenh chay rieng.
		int nMeX = 0, nMeY = 0;
		Npc[nMeIdx].GetMpsPos(&nMeX, &nMeY);
		Npc[nMeIdx].SendCommand(do_walk, nMeX, nMeY);
		SendClientCmdWalk(nMeX, nMeY);
	}
	AUTOLOG("[TUKICH] chot o quai chet: quai=%d o=(%d,%d) toi=%d doing=%d huylenh=%d",
		nIdxDead, s_nDeadTgtX, s_nDeadTgtY, nMeIdx, (int)Npc[nMeIdx].m_Doing, (int)bHuyLenh);
}
'''

CS_KHAI_CU = '''// Muc tieu dang khoa vua chet: huy lenh di chuyen dang treo (khong co no thi
// nhan vat chay not toi xac quai) va bat chot o vua chet.
// Goi tu KNpc::DoDeath va KNpcAI::FollowPeople (deu trong #ifndef _SERVER).
void g_OnLockedTargetDead(int nIdxDead)
{
	if (nIdxDead <= 0 || nIdxDead >= MAX_NPC)
		return;

'''
CS_KHAI_MOI = '''// [TUKICH 07/09 b] dinh nghia o KNpc.cpp (cho duy nhat nhin thay khe lenh s_S13Move).
extern BOOL S13_HuyLenhDiToiXac(int nIdx, int nXacX, int nXacY, int nBanKinh);

// [TUKICH 07/09 b] Nho con quai MINH VUA BAM VAO de danh. Can vi m_nPeopleIdx co the bi
// xoa TRUOC khi con quai chet (KNpc.cpp:3015 DoSkill bo chieu, :3090 CastMeleeSkill that
// bai) - luc do moc o DoDeath se khong nhan ra day la muc tieu cua minh nua. Nho theo
// m_dwID (chi so NPC bi dung lai), va chi trong TUKICH_TGT_NHO_MS.
#define TUKICH_TGT_NHO_MS	15000

static DWORD	s_dwTuKichTgtID = 0;
static DWORD	s_dwTuKichTgtTime = 0;

void g_TuKichGhiMucTieu(DWORD dwID)
{
	s_dwTuKichTgtID = dwID;
	s_dwTuKichTgtTime = GetTickCount();
}

// Con vua chet co phai muc tieu cua CHINH MINH khong.
static BOOL TuKichLaMucTieuCuaToi(int nIdxDead)
{
	int nMeIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (nMeIdx <= 0 || nMeIdx >= MAX_NPC)
		return FALSE;
	if (Npc[nMeIdx].m_nPeopleIdx == nIdxDead)
		return TRUE;
	if (s_dwTuKichTgtID && Npc[nIdxDead].m_dwID == s_dwTuKichTgtID &&
		GetTickCount() - s_dwTuKichTgtTime <= TUKICH_TGT_NHO_MS)
		return TRUE;
	return FALSE;
}

// Muc tieu cua minh vua chet: huy lenh di chuyen dang treo (khong co no thi nhan vat
// chay not toi xac quai) va bat chot o vua chet.
// Goi tu KNpc::DoDeath va KNpcAI::FollowPeople (deu trong #ifndef _SERVER); goi cho
// MOI con chet cung duoc - ham tu loc con nao la muc tieu cua minh.
void g_OnLockedTargetDead(int nIdxDead)
{
	if (nIdxDead <= 0 || nIdxDead >= MAX_NPC)
		return;

	if (!TuKichLaMucTieuCuaToi(nIdxDead))
		return;		// con khac chet ngang duong (Tong Kim...) - khong dung chan nguoi choi

'''

# --------------------------------- 2a. khai bao truoc LockSomeoneUseSkill
# (dinh nghia g_TuKichGhiMucTieu nam duoi, canh khoi chot o quai chet)
CS_FWD_CU = '''int KCoreShell::LockSomeoneUseSkill(int nTargetIndex, int nSkillID)
'''
CS_FWD_MOI = '''// [TUKICH 07/09 b] dinh nghia phia duoi, canh khoi chot o quai vua chet.
void g_TuKichGhiMucTieu(DWORD dwID);

''' + CS_FWD_CU

# ------------------------------------------- 2b. ghi nho muc tieu vua bam
CS_GHI_CU = '''	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	if (nRelation == relation_enemy)
	{
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		return 1;
	}
'''
CS_GHI_MOI = '''	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	if (nRelation == relation_enemy)
	{
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		// [TUKICH 07/09 b] nho con vua bam vao: m_nPeopleIdx co the bi xoa truoc khi no chet
		g_TuKichGhiMucTieu(Npc[nTargetIndex].m_dwID);
		return 1;
	}
'''

# ----------------------------------------------------------------- 3. KNpcAI
KA_CU = '''	if (CheckNpc(nIdx))
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}
'''
KA_MOI = '''	if (CheckNpc(nIdx))
	{
		// [TUKICH 07/09 b] DAY moi la nhanh that su bat muc tieu chet: CheckNpc goi
		// IsAlive() (chi xet m_Doing == do_death/do_revive) nen no an truoc nhanh
		// "m_Doing == do_death" phia duoi - ban va a dat chot o nhanh do nen khong
		// bao gio toi. Chi bat chot khi CHET; ra khoi tam nhin / an minh giu nhu cu.
		if (nIdx > 0 && nIdx < MAX_NPC && Npc[nIdx].m_RegionIndex >= 0 && !Npc[nIdx].IsAlive())
			g_OnLockedTargetDead(nIdx);
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}
'''

# ------------------------------------------- 1b. KNpc::DoDeath goi vo dieu kien
KN2_CU = '''	m_ClientDoing = cdo_death;
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx == m_Index)
	{
		// [TUKICH 07/09] muc tieu dang khoa vua chet: truoc day chi xoa khoa,
		// lenh do_run da gui van con hieu luc -> nhan vat chay not toi xac quai,
		// va cu kich chuot ke tiep (tu kich chuot) lai keo toi xac / do roi.
		// g_OnLockedTargetDead (CoreShell.cpp) dung tai cho + chot o vua chet.
		g_OnLockedTargetDead(m_Index);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
	}
'''
KN2_MOI = '''	m_ClientDoing = cdo_death;
	// [TUKICH 07/09 b] goi VO DIEU KIEN: m_nPeopleIdx co the da bi xoa truoc khi con quai
	// chet (KNpc.cpp DoSkill bo chieu / CastMeleeSkill that bai), luc do dieu kien
	// "m_nPeopleIdx == m_Index" cua ban va a khong con dung. g_OnLockedTargetDead tu loc
	// con nao la muc tieu cua chinh minh (theo m_nPeopleIdx HOAC con vua bam vao).
	g_OnLockedTargetDead(m_Index);
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx == m_Index)
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
	}
'''

VIEC = [
    ("Sources/Core/Src/KNpc.cpp", [
        ("KNpc:S13_HuyLenh", KN_CU, KN_MOI),
        ("KNpc:DoDeath vodk", KN2_CU, KN2_MOI),
    ]),
    ("Sources/Core/Src/CoreShell.cpp", [
        ("CoreShell:khai bao", CS_KHAI_CU, CS_KHAI_MOI),
        ("CoreShell:fwd ghi", CS_FWD_CU, CS_FWD_MOI),
        ("CoreShell:ghi muctieu", CS_GHI_CU, CS_GHI_MOI),
        ("CoreShell:vut lenh", CS_CU, CS_MOI),
    ]),
    ("Sources/Core/Src/KNpcAI.cpp", [
        ("KNpcAI:CheckNpc", KA_CU, KA_MOI),
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
