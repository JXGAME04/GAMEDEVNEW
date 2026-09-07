# -*- coding: latin-1 -*-
# [SK120 07/09] Bo va engine: dua co che missle_missrate / ignorenegativestate_p ve dung Linux (jx_linux_y).
#   H1 KMissle.cpp  vong quet va cham : khe TRUOT m_nMissRate mot lan cho moi muc tieu (Linux 0x080753F0-0x08075600)
#   H2 KMissle.cpp  ProcessDamage     : bo khe ign% tung tai cho (chuyen vao KNpc::SetStateSkillEffect)
#   H3 KNpc.cpp     ReceiveDamage o15 : tay trang thai KHONG tung (100 - nMissRate) nua (Linux handler 201 0x080977D0)
#   H4 KNpc.cpp     ReceiveDamage     : BO nhanh 'else if (g_RandPercent(nMissRate))' tay debuff NAN NHAN (JX1-only)
#   H5 KNpc.cpp     ReceiveDamage o16 : bo 'g_RandPercent(nMissRate) -> return FALSE'
#   H6 KNpc.cpp     DoHurt            : them khe ign% (Linux 0x0807F821)
#   H7 KNpc.cpp     ReceiveDamage bang: them khe ign% huy bang (Linux 0x0808B21F)
#   H8 KNpc.cpp     SetStateSkillEffect: khe ign% cho MOI chieu nham ke dich (Linux 0x08086410-0x08086D13)
# Cach chay:  python goi_va_sk120_linux_0709.py [goc_cay] [--check]
#   --check : chi dem anchor, khong ghi.  Idempotent; sao luu *.truoc_sk120b_0709 (chi lan dau).
import io, os, sys

root = r"D:\GAMEDEVNEW"
check = False
for a in sys.argv[1:]:
    if a == "--check":
        check = True
    else:
        root = a

SRC = os.path.join(root, "Sources", "Core", "Src")


def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def wr(p, s):
    bak = p + ".truoc_sk120b_0709"
    if not os.path.exists(bak):
        io.open(bak, "w", encoding="latin-1", newline="").write(rd(p))
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def eol(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def hunk(s, name, old, new):
    e = eol(s)
    old = old.replace("\n", e)
    new = new.replace("\n", e)
    if new in s and old not in s:
        print("  [da co] %s" % name)
        return s
    n = s.count(old)
    if n != 1:
        raise SystemExit("KHONG KHOP %s: anchor xuat hien %d lan" % (name, n))
    print("  [%s] %s" % ("ok" if check else "va", name))
    return s if check else s.replace(old, new)


# ----------------------------------------------------------------------------------------------- KMissle.cpp
pm = os.path.join(SRC, "KMissle.cpp")
s = rd(pm)

s = hunk(s, "H1 khe truot m_nMissRate o vong quet va cham",
"""				nRet++;
				VHLOG("[VH-SCAN-NPC]""",
"""#ifdef _SERVER
				// [SK120 07/09] Linux KMissle collide 0x080753F0-0x08075600: neu m_nMissRate > 0 thi tung rand(100) MOT lan
				// cho MOI muc tieu TRUOC khi xu ly; missrate > rand => log 'Missle nMissRate = %d%%, Miss!' va tra 0:
				// bo ca don (khong sat thuong, khong trang thai, khong tru m_nHitCount). JX1 truoc day khong co khe truot
				// nay ma dung nMissRate sai cho trong KNpc::ReceiveDamage (o 15 tay trang thai 100-miss, o 16 return FALSE).
				if (m_nMissRate > 0 && g_RandPercent(m_nMissRate))
				{
					AUTOLOG_EVERY(2000, "[SK120-MISS] msl=%d sk=%d/%d launcher=%d npc=%d missrate=%d -> truot ca don", m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, nNpcIdx, m_nMissRate);
					continue;
				}
#endif
				nRet++;
				VHLOG("[VH-SCAN-NPC]""")

s = hunk(s, "H2 bo khe ign% tung tai cho trong ProcessDamage",
"""						if (g_RandPercent(Npc[nNpcId].m_CurrentIgnoreNegativeStateP))
							return TRUE;

						if(pSkill->IsSkillReduceResist())""",
"""						// [SK120 07/09] khe ign% doi voi trang thai da chuyen vao KNpc::SetStateSkillEffect (Linux 0x08086410):
						// ap cho MOI chieu nham ke dich, khong bo luon SetImmediatelySkillEffect nhu ban cu, khong tung hai lan.

						if(pSkill->IsSkillReduceResist())""")

if not check:
    wr(pm, s)

# ----------------------------------------------------------------------------------------------- KNpc.cpp
pn = os.path.join(SRC, "KNpc.cpp")
s = rd(pn)

s = hunk(s, "H3 o15 ignorenegativestate_p tay khong tung",
"""	if (pTemp->nAttribType == magic_ignorenegativestate_p)
	{
		if (g_RandPercent(100 - nMissRate))
		{
""",
"""	if (pTemp->nAttribType == magic_ignorenegativestate_p)
	{
		// [SK120 07/09] Linux handler 201 (0x080977D0) tay trang thai KHONG tung xuc xac; khe truot m_nMissRate da tung
		// MOT lan o KMissle (vong quet va cham). Ban cu tung (100 - nMissRate) o day ma khong co khe truot -> tay va
		// mien dich (+ign% qua m_StateAttribs) lech xac suat nhau (738: tay 100-miss nhung mien dich luon 100%).
		{
""")

# H4: dong cuoi cua nhanh else-if co chu thich TCVN3 -> anchor toi truoc '//'
old4 = """	else if(g_RandPercent(nMissRate))
	{
		this->ClearNormalState();
		this->IgnoreState(TRUE);
		m_nTime_Ignorenegativestate = pTemp->nValue[1]; //"""
e = eol(s)
old4e = old4.replace("\n", e)
n4 = s.count(old4e)
if n4 == 1:
    i = s.index(old4e)
    j = s.index(e + "\t}" + e, i) + len(e + "\t}" + e)
    new4 = ("""	// [SK120 07/09] BO nhanh 'else if (g_RandPercent(nMissRate))' cua JX1: no TAY SACH trang thai xau cua NAN NHAN moi khi
	// chieu co missle_missrate ma o 15 khong phai ignorenegativestate (723/876/1406/1493 Ma Am Phe Phach, 1131 Ma Am Kich,
	// 1190...). Linux ReceiveDamage 0x0808A4A0 khong doc nMissRate. Xem PHANTICH_CAIBANG_DAN_VA_QUAI_DOT2_0609.md Phan J.4.
""").replace("\n", e)
    print("  [%s] H4 bo nhanh else-if tay debuff nan nhan (%d byte)" % ("ok" if check else "va", j - i))
    if not check:
        s = s[:i] + new4 + s[j:]
elif n4 == 0 and "H4" in s.replace("[SK120 07/09] BO nhanh", "H4"):
    print("  [da co] H4")
else:
    raise SystemExit("KHONG KHOP H4: %d lan" % n4)

s = hunk(s, "H5 o16 bo return FALSE theo nMissRate",
"""	if (pTemp->nValue[0] && pTemp->nValue[1] && g_RandPercent(nMissRate))
		return FALSE;
""",
"""	// [SK120 07/09] bo khe 'g_RandPercent(nMissRate) -> return FALSE' o o randmove[16]: khe truot da tung o KMissle.
""")

s = hunk(s, "H6 DoHurt khe ign%",
"""	if (!g_RandPercent(nHurtI * 3 / 4 + 60 - giam_tho_thuong / 4))
	//if (!g_RandPercent(nHurtI / 2 + 50 - giam_tho_thuong / 2))
	{
		return;
	}
#endif
""",
"""	if (!g_RandPercent(nHurtI * 3 / 4 + 60 - giam_tho_thuong / 4))
	//if (!g_RandPercent(nHurtI / 2 + 50 - giam_tho_thuong / 2))
	{
		return;
	}
	// [SK120 07/09] Linux DoHurt 0x0807F821-0x0807F85C: sau khe co ban, nan nhan co ignorenegativestate_p (+0x1474 > 0)
	// tung rand(100) < ign% => 'IgnoreNegState(Hurt): %d%%, Ignore!' -> KHONG tho thuong.
	if (m_CurrentIgnoreNegativeStateP > 0 && g_RandPercent(m_CurrentIgnoreNegativeStateP))
		return;
#endif
""")

s = hunk(s, "H7 bang: khe ign% huy bang",
"""			m_FreezeState.nTime = pTemp->nValue[1] * (MAX_PERCENT - nGiam) / MAX_PERCENT;
""",
"""			m_FreezeState.nTime = pTemp->nValue[1] * (MAX_PERCENT - nGiam) / MAX_PERCENT;
			// [SK120 07/09] Linux ReceiveDamage 0x0808B21F-0x0808B273: ngay sau khi dat thoi gian bang, nan nhan co
			// ignorenegativestate_p -> tung rand(100) < ign% => 'IgnoreNegState(Freeze): %d%%, Ignore!' -> huy bang.
			if (m_CurrentIgnoreNegativeStateP > 0 && g_RandPercent(m_CurrentIgnoreNegativeStateP))
				m_FreezeState.nTime = 0;
""")

s = hunk(s, "H8 SetStateSkillEffect khe ign% moi chieu nham ke dich",
"""	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nLevel);
	//
	_ASSERT(nDataNum < MAX_SKILL_STATE);
""",
"""	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nLevel);
#ifdef _SERVER
	// [SK120 07/09] Linux SetStateSkillEffect 0x08086410-0x08086D13: nan nhan co ignorenegativestate_p (+0x1474 > 0) va
	// chieu nham KE DICH (vfunc IsTargetEnemy) -> tung rand(100) < ign% => log 'bo qua trang thai xau (phep): %d%%, bo!'
	// -> KHONG ap bat ky trang thai nao cua chieu (ke ca dong bo len client). Linux khong xet nValue[1]/nValue[2]/
	// SkillStyle nhu khoi cu trong KMissle::ProcessDamage (da bo). nTime == 0 la lenh GO trang thai -> khong tung.
	if (m_CurrentIgnoreNegativeStateP > 0 && nTime != 0 && pOrdinSkill && pOrdinSkill->IsTargetEnemy()
		&& g_RandPercent(m_CurrentIgnoreNegativeStateP))
	{
		AUTOLOG_EVERY(2000, "[SK120-IGNSTATE] npc=%d skill=%d/%d launcher=%d ign=%d -> bo trang thai", m_Index, nSkillID, nLevel, nLauncher, m_CurrentIgnoreNegativeStateP);
		return;
	}
#endif
	//
	_ASSERT(nDataNum < MAX_SKILL_STATE);
""")

if not check:
    wr(pn, s)
print("XONG" if not check else "CHECK XONG (chua ghi)")
