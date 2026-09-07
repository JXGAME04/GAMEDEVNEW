# -*- coding: latin-1 -*-
# [SKEXP 07/09] Bo va engine dot 'luyen ky nang 90/120/150 + cong thuc vat ly/hoa sat' theo Linux (jx_linux_y).
#   H1 KSkills.h        : them m_AddSkillExp[2]/m_nAddSkillExpNum + getter (khong them ten vao enum KMagicAttrib)
#   H2 KSkills.cpp      : reset bo dem (constructor + LoadSkillLevelData) + doc LvlSetting 'addskillexp1/2' truoc ParseString2MagicAttrib
#   H3 KNpc.h / KNpc.cpp: CongExpKyNangKhiTrung(skill, level, victim) - luat Linux ReceiveDamage 0x0808AA3C-0x0808AA97
#   H4 KPlayer.h / .cpp : AddSkillExpKhiTrung(skill, exp) - luat KSkillList::AddSkillExp Linux 0x080E5D90 (chan bang MaxLevel that)
#   H5 KMissle.cpp      : goi H3 sau khi ReceiveDamage tra TRUE (moi lan trung moi muc tieu)
#   H6 KNpc.cpp         : BO hai khoi AddSkillExp120/90 theo TUNG HE trong CalcDamage (goc 'phai nhanh phai cham')
#   H7 KNpc.cpp + KPlayer.cpp: physicsenhance_p = (goc+add)(100+p)/100(100+tang)/100; hoa sat chi nhan MAX, hoa vu khi khong nhan
# Cach chay: python goi_va_sk150_luyen_0709.py [goc_cay] [--check]   (idempotent; sao luu *.truoc_skexp_0709 lan dau)
import io, os, re, sys

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
    bak = p + ".truoc_skexp_0709"
    if not os.path.exists(bak):
        io.open(bak, "w", encoding="latin-1", newline="").write(rd(p))
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def eol(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def hunk(s, name, old, new, count=1):
    e = eol(s)
    old = old.replace("\n", e)
    new = new.replace("\n", e)
    if new in s and old not in s:
        print("  [da co] %s" % name)
        return s
    n = s.count(old)
    if n != count:
        raise SystemExit("KHONG KHOP %s: anchor xuat hien %d lan (mong %d)" % (name, n, count))
    print("  [%s] %s" % ("ok" if check else "va", name))
    return s if check else s.replace(old, new)


files = {}


def get(name):
    if name not in files:
        files[name] = rd(os.path.join(SRC, name))
    return files[name]


def put(name, s):
    files[name] = s


# ------------------------------------------------------------------------------------------ H1 KSkills.h
s = get("KSkills.h")
s = hunk(s, "H1a KSkills.h thanh vien m_AddSkillExp",
"""	KMagicAttrib        m_AddSkillDamage[MAX_ADDSKILLDAMAGE];
	int					m_nAddSkillDamageNum;
""",
"""	KMagicAttrib        m_AddSkillDamage[MAX_ADDSKILLDAMAGE];
	int					m_nAddSkillDamageNum;
	// [SKEXP 07/09] addskillexp1/2 theo Linux (idx 73/74): {id ky nang nhan exp (0 = chinh chieu nay), exp moi lan trung, co}.
	// KHONG dua vao enum KMagicAttrib (tranh doi so thu tu thuoc tinh dang dong bo client) - doc thang ten LvlSetting.
	KMagicAttrib		m_AddSkillExp[2];
	int					m_nAddSkillExpNum;
""")
# H1b: dong GetAddSkillDamage co khoang trang duoi -> chen theo regex sau dong do
if "GetAddSkillExpNum()" in s:
    print("  [da co] H1b KSkills.h getter")
else:
    mg = re.search(r"^[ \t]*KMagicAttrib\*[ \t]+GetAddSkillDamage\(\)[^\r\n]*\r?\n", s, re.M)
    if not mg:
        raise SystemExit("KHONG KHOP H1b: khong thay dong GetAddSkillDamage")
    ins = ("""	KMagicAttrib*       GetAddSkillExp()  { return m_AddSkillExp;};			// [SKEXP 07/09]
	int					GetAddSkillExpNum() { return m_nAddSkillExpNum;};	// [SKEXP 07/09]
""").replace("\n", eol(s))
    print("  [%s] H1b KSkills.h getter" % ("ok" if check else "va"))
    if not check:
        s = s[:mg.end()] + ins + s[mg.end():]
put("KSkills.h", s)

# ------------------------------------------------------------------------------------------ H2 KSkills.cpp
s = get("KSkills.cpp")
s = hunk(s, "H2a KSkills.cpp reset bo dem (constructor + LoadSkillLevelData)",
"""	m_nAddSkillDamageNum = 0;
""",
"""	m_nAddSkillDamageNum = 0;
	m_nAddSkillExpNum = 0; memset(m_AddSkillExp, 0, sizeof(m_AddSkillExp));	// [SKEXP 07/09]
""", count=2)
s = hunk(s, "H2b KSkills.cpp doc addskillexp1/2 trong LoadSkillLevelData",
"""		ParseString2MagicAttrib(nLevel, szSettingNameValue, szResult);
	}
""",
"""		// [SKEXP 07/09] addskillexp1/2 (Linux idx 73/74, o sat thuong 16/17): JX1 khong co ten nay trong KMagicDesc
		// nen ParseString2MagicAttrib bo qua -> doc rieng o day. Gia tri Lua: {id, exp moi lan trung, co} (id 0 = chinh chieu).
		if (!strcmp(szSettingNameValue, "addskillexp1") || !strcmp(szSettingNameValue, "addskillexp2"))
		{
			if (m_nAddSkillExpNum < 2)
			{
				const char *pcszExp = szResult;
				int nExpV0 = KSG_StringGetInt(&pcszExp, 0);
				KSG_StringSkipSymbol(&pcszExp, ',');
				int nExpV1 = KSG_StringGetInt(&pcszExp, 0);
				KSG_StringSkipSymbol(&pcszExp, ',');
				int nExpV2 = KSG_StringGetInt(&pcszExp, 0);
				m_AddSkillExp[m_nAddSkillExpNum].nAttribType = 0;
				m_AddSkillExp[m_nAddSkillExpNum].nValue[0] = nExpV0;
				m_AddSkillExp[m_nAddSkillExpNum].nValue[1] = nExpV1;
				m_AddSkillExp[m_nAddSkillExpNum].nValue[2] = nExpV2;
				m_nAddSkillExpNum++;
			}
			continue;
		}
		ParseString2MagicAttrib(nLevel, szSettingNameValue, szResult);
	}
""")
put("KSkills.cpp", s)

# ------------------------------------------------------------------------------------------ H3 KNpc.h / KNpc.cpp
s = get("KNpc.h")
# H3a: khai bao PUBLIC (KMissle goi) - dat ngay sau khai bao ReceiveDamage (cung vung public)
if "CongExpKyNangKhiTrung" in s:
    print("  [da co] H3a KNpc.h khai bao CongExpKyNangKhiTrung")
else:
    mr = re.search(r"^[ \t]*BOOL[ \t]+ReceiveDamage\(int nLauncher[^\r\n]*\r?\n", s, re.M)
    if not mr:
        raise SystemExit("KHONG KHOP H3a: khong thay khai bao ReceiveDamage")
    ins = ("""	void				CongExpKyNangKhiTrung(int nSkillId, int nLevel, int nVictim);	// [SKEXP 07/09] exp ky nang khi trung, luat Linux (public: KMissle goi)
""").replace("\n", eol(s))
    print("  [%s] H3a KNpc.h khai bao CongExpKyNangKhiTrung" % ("ok" if check else "va"))
    if not check:
        s = s[:mr.end()] + ins + s[mr.end():]
put("KNpc.h", s)

s = get("KNpc.cpp")
# H6: bo hai khoi exp theo tung he trong CalcDamage
e = eol(s)
pat = re.compile(r"\t\tif\(g_Skill120ExpRate && !m_btSimCityBot\) // luyen skill 120\r?\n.*?AddSkillExp90\(calExpSkill\);\r?\n\t\t\t\}\r?\n\t\t\}\r?\n", re.S)
m = list(pat.finditer(s))
newblk = ("""		// [SKEXP 07/09] DA BO hai khoi AddSkillExp120/AddSkillExp90 cua JX1 (cong g_Skill*Rate x he so cho MOI lan CalcDamage
		// = moi HE sat thuong, moi tia doc, moi muc tieu, doc nham m_CurrentExpSkillsEnchance cua Player[CLIENT_PLAYER_INDEX]).
		// Linux khong co: exp ky nang chi phat o ReceiveDamage theo o addskillexp1/2 cua chieu (KMissle::ProcessDamage ->
		// KNpc::CongExpKyNangKhiTrung), moi lan trung 1 muc tieu, 60 %, khong nhan theo he. Ky nang 120 nhan exp qua Add120SkillExp.
""").replace("\n", e)
if len(m) == 1:
    print("  [%s] H6 bo 2 khoi AddSkillExp120/90 theo he (%d byte)" % ("ok" if check else "va", m[0].end() - m[0].start()))
    if not check:
        s = s[:m[0].start()] + newblk + s[m[0].end():]
elif len(m) == 0 and "[SKEXP 07/09] DA BO hai khoi" in s:
    print("  [da co] H6")
else:
    raise SystemExit("KHONG KHOP H6: %d lan" % len(m))

s = hunk(s, "H7a KNpc.cpp physicsenhance_p (goc+add)(100+p)(100+tang)",
"""		pDes->nValue[0] = nMinDamage * (MAX_PERCENT + (pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT))) / MAX_PERCENT;
		pDes->nValue[2] = nMaxDamage * (MAX_PERCENT + (pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT))) / MAX_PERCENT;
""",
"""		// [SK150 07/09] Linux 0x0807C7A8-0x0807C844: v = (goc + add) x (100 + p) / 100, roi v += v x tang / 100 -
		// he so tang sat thuong ky nang nhan CA phan goc, khong chi phan p nhu ban cu (lech 4-9 % khi co skill_enhance).
		pDes->nValue[0] = (int)((__int64)nMinDamage * (MAX_PERCENT + pTemp->nValue[0]) / MAX_PERCENT * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT);
		pDes->nValue[2] = (int)((__int64)nMaxDamage * (MAX_PERCENT + pTemp->nValue[0]) / MAX_PERCENT * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT);
""")
s = hunk(s, "H7b KNpc.cpp hoa sat chi nhan MAX",
"""		pDes->nValue[0] = pTemp->nValue[0] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT + pTemp->nValue[0] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT * m_CurrentFireEnhance / MAX_PERCENT;
		pDes->nValue[2] = pTemp->nValue[2] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT + pTemp->nValue[2] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT * m_CurrentFireEnhance / MAX_PERCENT;
""",
"""		// [SK150 07/09] Linux 0x0807CD98-0x0807CE1C: hoa sat (m_CurrentFireEnhance) chi nhan vao MAX cua chieu; sau do
		// ca min/max x (100 + tang) / 100. Ban cu nhan hoa sat vao ca min lan max.
		pDes->nValue[0] = pTemp->nValue[0] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT;
		pDes->nValue[2] = (pTemp->nValue[2] + pTemp->nValue[2] * m_CurrentFireEnhance / MAX_PERCENT) * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT;
""")
s = hunk(s, "H7c KNpc.cpp hoa vu khi khong nhan hoa sat",
"""		pDes->nValue[0] += m_CurrentFireDamage.nValue[0] + m_CurrentFireDamage.nValue[0] * m_CurrentFireEnhance / MAX_PERCENT;
		pDes->nValue[2] += m_CurrentFireDamage.nValue[2] + m_CurrentFireDamage.nValue[2] * m_CurrentFireEnhance / MAX_PERCENT;
""",
"""		// [SK150 07/09] Linux 0x0807CD78: hoa cua vu khi cong thang, KHONG nhan hoa sat.
		pDes->nValue[0] += m_CurrentFireDamage.nValue[0];
		pDes->nValue[2] += m_CurrentFireDamage.nValue[2];
""")
# H3b: than ham CongExpKyNangKhiTrung - dat ngay truoc SetImmediatelySkillEffect (ham server co san)
s = hunk(s, "H3b KNpc.cpp than CongExpKyNangKhiTrung",
"""void KNpc::SetImmediatelySkillEffect(int nLauncher, void *pData, int nDataNum)
""",
"""#ifdef _SERVER
// [SKEXP 07/09] Luat Linux ReceiveDamage 0x0808AA3C-0x0808AA97 (sau khe choang): duyet o addskillexp1/2 cua chieu vua trung:
//   v0 (id ky nang nhan exp; 0 = chinh chieu) > 0; tung rand(100) > 59 thi bo (60 % moi lan trung);
//   co v2 & 2 -> exp cho NAN NHAN (neu la nguoi choi); khong thi cho NGUOI PHAT (phai la nguoi choi);
//   KSkillList::AddSkillExp (0x080E5D90): chi ky nang kinh nghiem, cap < cap toi da, exp += v1 (co 0).
// JX1: exp = v1 x Skill90Rate (gamesetting.ini [ServerConfig], 1 = dung Linux) x he so x2 (m_CurrentExpSkillsEnchance CUA NGUOI NHAN).
void KNpc::CongExpKyNangKhiTrung(int nSkillId, int nLevel, int nVictim)
{
	if (nVictim <= 0 || nVictim >= MAX_NPC || Npc[nVictim].m_btSimCityBot)
		return;
	KSkill *pSkill = (KSkill *)g_SkillManager.GetSkill(nSkillId, nLevel);
	if (!pSkill || pSkill->GetAddSkillExpNum() <= 0)
		return;
	KMagicAttrib *pExp = pSkill->GetAddSkillExp();
	for (int i = 0; i < pSkill->GetAddSkillExpNum(); i++)
	{
		int nIdNhan = pExp[i].nValue[0] > 0 ? pExp[i].nValue[0] : nSkillId;
		int nExp = pExp[i].nValue[1];
		if (nIdNhan <= 0 || nIdNhan >= MAX_SKILL || nExp <= 0)
			continue;
		if (!g_RandPercent(60))
			continue;
		int nNhan = m_Index;
		if (pExp[i].nValue[2] & 2)
		{
			if (!Npc[nVictim].IsPlayer())
				continue;
			nNhan = nVictim;
		}
		if (!Npc[nNhan].IsPlayer() || Npc[nNhan].m_nPlayerIdx <= 0 || Npc[nNhan].m_nPlayerIdx >= MAX_PLAYER)
			continue;
		int nHeSo = g_Skill90ExpRate > 0 ? g_Skill90ExpRate : 1;
		int nX2 = Npc[nNhan].m_CurrentExpSkillsEnchance > 1 ? Npc[nNhan].m_CurrentExpSkillsEnchance : 1;
		Player[Npc[nNhan].m_nPlayerIdx].AddSkillExpKhiTrung(nIdNhan, nExp * nHeSo * nX2);
	}
}
#endif

void KNpc::SetImmediatelySkillEffect(int nLauncher, void *pData, int nDataNum)
""")
put("KNpc.cpp", s)

# ------------------------------------------------------------------------------------------ H4 KPlayer.h / KPlayer.cpp
s = get("KPlayer.h")
s = hunk(s, "H4a KPlayer.h khai bao AddSkillExpKhiTrung",
"""	void			IncSkillExp(int nSkillId, int nAddExp);
""",
"""	void			IncSkillExp(int nSkillId, int nAddExp);
	void			AddSkillExpKhiTrung(int nSkillId, int nExp);		// [SKEXP 07/09] luat Linux KSkillList::AddSkillExp
""")
put("KPlayer.h", s)

s = get("KPlayer.cpp")
s = hunk(s, "H4b KPlayer.cpp than AddSkillExpKhiTrung",
"""void	KPlayer::IncSkillExp(int nSkillId, int nAddExp)
""",
"""// [SKEXP 07/09] Cong exp ky nang khi trung - theo KSkillList::AddSkillExp Linux (0x080E5D90): chi ky nang kinh nghiem (IsExp),
// dung khi cap >= cap toi da THAT cua ky nang (skills.txt MaxLevel) - AddSkillExp90 cu chan o MAX_TRAIN_SKILLEXPLEVEL 20 nen
// ky nang 150 khong bao gio len duoc 21-26. Nguong cap = magic_level_exp.txt (trung skill_skillexp_v Linux 62/62).
void	KPlayer::AddSkillExpKhiTrung(int nSkillId, int nExp)
{
	if (nSkillId <= 0 || nSkillId >= MAX_SKILL || nExp <= 0)
		return;
	int nSkillIndex = Npc[m_nIndex].m_SkillList.FindSame(nSkillId);
	if (nSkillIndex <= 0)
		return;
	int nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(nSkillId);
	if (nSkillLevel <= 0 || nSkillLevel >= (int)g_SkillManager.GetSkillMaxLevel(nSkillId))
		return;
	KSkill *pSkill = (KSkill *)g_SkillManager.GetSkill(nSkillId, nSkillLevel);
	if (!pSkill || !pSkill->IsExp())
		return;
	if (Npc[m_nIndex].m_SkillList.IncreaseExp(nSkillIndex, nExp))
		UpdataCurData();
	PLAYER_SKILL_LEVEL_SYNC	sSkill;
	sSkill.ProtocolType = s2c_playerskilllevel;
	sSkill.m_nSkillID = nSkillId;
	sSkill.m_nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(nSkillId);
	sSkill.m_nAddLevel = Npc[m_nIndex].m_SkillList.GetAddLevel(nSkillId);
	sSkill.m_nSkillExp = Npc[m_nIndex].m_SkillList.GetExp(nSkillId);
	sSkill.m_bTempSkill = Npc[m_nIndex].m_SkillList.IsTempSkill(nSkillId);
	sSkill.m_nLeavePoint = m_nSkillPoint;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
}

void	KPlayer::IncSkillExp(int nSkillId, int nAddExp)
""")
s = hunk(s, "H7d KPlayer.cpp tooltip physicsenhance_p",
"""		*nMin += nMinNpcDamage * (100 + pMagicData->nValue[0] * (100 + nAddDamageP) / 100) / 100;
		*nMax += nMaxNpcDamage * (100 + pMagicData->nValue[0] * (100 + nAddDamageP) / 100) / 100;
""",
"""		// [SK150 07/09] cung cong thuc voi KNpc::AppendSkillEffect (Linux 0x0807C7A8)
		*nMin += (int)((__int64)nMinNpcDamage * (100 + pMagicData->nValue[0]) / 100 * (100 + nAddDamageP) / 100);
		*nMax += (int)((__int64)nMaxNpcDamage * (100 + pMagicData->nValue[0]) / 100 * (100 + nAddDamageP) / 100);
""")
s = hunk(s, "H7e KPlayer.cpp tooltip hoa sat chi MAX",
"""		*nMin += pMagicData->nValue[0] * (100 + nAddDamageP) / 100 + pMagicData->nValue[0] * (100 + nAddDamageP) / 100 * Npc[m_nIndex].m_CurrentFireEnhance / 100;
		*nMax += pMagicData->nValue[2] * (100 + nAddDamageP) / 100 + pMagicData->nValue[2] * (100 + nAddDamageP) / 100 * Npc[m_nIndex].m_CurrentFireEnhance / 100;
""",
"""		// [SK150 07/09] cung cong thuc voi KNpc::AppendSkillEffect (Linux 0x0807CD98): hoa sat chi nhan MAX
		*nMin += pMagicData->nValue[0] * (100 + nAddDamageP) / 100;
		*nMax += (pMagicData->nValue[2] + pMagicData->nValue[2] * Npc[m_nIndex].m_CurrentFireEnhance / 100) * (100 + nAddDamageP) / 100;
""")
s = hunk(s, "H7f KPlayer.cpp tooltip hoa vu khi",
"""		*nMin += Npc[m_nIndex].m_CurrentFireDamage.nValue[0] + Npc[m_nIndex].m_CurrentFireDamage.nValue[0] * Npc[m_nIndex].m_CurrentFireEnhance / 100;
		*nMax += Npc[m_nIndex].m_CurrentFireDamage.nValue[2] + Npc[m_nIndex].m_CurrentFireDamage.nValue[2] * Npc[m_nIndex].m_CurrentFireEnhance / 100;
""",
"""		*nMin += Npc[m_nIndex].m_CurrentFireDamage.nValue[0];	// [SK150 07/09] Linux 0x0807CD78: hoa vu khi khong nhan hoa sat
		*nMax += Npc[m_nIndex].m_CurrentFireDamage.nValue[2];
""")
put("KPlayer.cpp", s)

# ------------------------------------------------------------------------------------------ H5 KMissle.cpp
s = get("KMissle.cpp")
s = hunk(s, "H5 KMissle.cpp goi CongExpKyNangKhiTrung sau khi trung",
"""			if (m_pMagicAttribsData->m_nImmediateMagicAttribsNum > 0)
			{
				Npc[nNpcId].SetImmediatelySkillEffect(m_nLauncher, m_pMagicAttribsData->m_pImmediateAttribs, m_pMagicAttribsData->m_nImmediateMagicAttribsNum);
			}
		}
		return TRUE;
""",
"""			if (m_pMagicAttribsData->m_nImmediateMagicAttribsNum > 0)
			{
				Npc[nNpcId].SetImmediatelySkillEffect(m_nLauncher, m_pMagicAttribsData->m_pImmediateAttribs, m_pMagicAttribsData->m_nImmediateMagicAttribsNum);
			}
			// [SKEXP 07/09] exp ky nang khi TRUNG (Linux ReceiveDamage 0x0808AA3C): moi lan trung moi muc tieu, theo o addskillexp1/2 cua chieu.
			if (m_nLauncher > 0 && m_nLauncher < MAX_NPC)
				Npc[m_nLauncher].CongExpKyNangKhiTrung(m_nSkillId, m_nLevel, nNpcId);
		}
		return TRUE;
""")
put("KMissle.cpp", s)

if not check:
    for name, s in files.items():
        wr(os.path.join(SRC, name), s)
print("XONG" if not check else "CHECK XONG (chua ghi)")
