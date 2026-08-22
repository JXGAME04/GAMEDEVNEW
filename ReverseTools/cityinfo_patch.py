# -*- coding: utf-8 -*-
r"""[CITYINFO 21/08] Chiem linh ban do + thue moi thanh hien o CLIENT:
server KJx2CityWar -> PLAYER_SCRIPTACTION_SYNC UIId=UI_CITYINFO (kenh mo rong, khong doi wire-format)
-> Core client luu bang 7 thanh -> Game.exe: ban do the gioi 7 nhan + ban do lon [CityInfo1]/[CityInfo2]."""
import io, os, sys
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
SRC = r"D:\GAMEDEVNEW\Sources"
def P(*a): return os.path.join(SRC, *a)
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) > 127)
def nlof(s): return "\r\n" if "\r\n" in s else "\n"
def rep1(s, old, new, nl):
    o = old.replace("\r\n", "\n"); n = new.replace("\r\n", "\n")
    for cand in (nl, "\n" if nl == "\r\n" else "\r\n"):   # tep tron CRLF/LF: thu ca 2
        o2 = o.replace("\n", cand)
        if s.count(o2) == 1:
            return s.replace(o2, n.replace("\n", cand), 1)
    assert False, (s.count(o.replace("\n", nl)), old[:90])

# ======== 1. KPlayer.h: UI_CITYINFO (cuoi enum) ========
p = P("Core", "Src", "KPlayer.h"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, "\tUI_TASKVALUE,\t// [TaskGuide] (int id, int value); id = -1 -> xoa bang task (client cu bo qua)\n};\n",
            "\tUI_TASKVALUE,\t// [TaskGuide] (int id, int value); id = -1 -> xoa bang task (client cu bo qua)\n\tUI_CITYINFO,\t// [CITYINFO 21/08] chuoi \"id\\tstate\\ttax\\tprice\\tmapid\\tten\\tchu\\tthaithu\" 1 thanh (client cu bo qua)\n};\n", nl)
assert hi(s) == h0; wr(p, s); print("KPlayer.h ok")

# ======== 2. CoreShell.h: struct + 3 id cuoi enum ========
p = P("Core", "Src", "CoreShell.h"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, "\tGOI_TASKGUIDE_GOTO_XAFU,\t// [TaskGuide] bam nhiem vu loai 4 -> tu chay den Xa Phu (bam lai = huy)\n\n};\n",
"""	GOI_TASKGUIDE_GOTO_XAFU,	// [TaskGuide] bam nhiem vu loai 4 -> tu chay den Xa Phu (bam lai = huy)
	GDI_CITY_INFO,				// [CITYINFO 21/08] uParam = thanh 1..7, nParam = KCityInfoView* -> 1 co du lieu / 0
	GDI_CITY_INFO_CURMAP,		// [CITYINFO 21/08] nParam = KCityInfoView* -> id thanh cua map dang dung / 0
	GDCNI_CITY_INFO_UPDATE,		// [CITYINFO 21/08] uParam = id thanh vua nhan tu server

};

// [CITYINFO 21/08] ban sao 7 thanh (KJx2CityWar server -> UI_CITYINFO -> Core client)
struct KCityInfoView
{
	int		nCityId;		// 0 = chua co du lieu
	int		nState;			// JX2CW_STATE_* (0 roi .. 5 cong thanh)
	int		nTax;			// thue giao dich (%)
	int		nPrice;			// chi so vat gia (-1 = chua dat)
	int		nMapId;			// map chinh cua thanh (AreaIncludes dau tien)
	char	szName[32];		// ten thanh (TCVN3)
	char	szOwner[32];	// bang chiem thanh ("" = vo chu)
	char	szMaster[32];	// Thai Thu
};
""", nl)
assert hi(s) == h0; wr(p, s); print("CoreShell.h ok")

# ======== 3. CoreShell.cpp: bang client + GetGameData ========
p = P("Core", "Src", "CoreShell.cpp"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, "int\tKCoreShell::GetGameData(unsigned int uDataId, unsigned int uParam, int nParam)\n{\n\tint nRet = 0;\n\tswitch(uDataId)\n\t{\n",
"""// [CITYINFO 21/08] ban sao 7 thanh phia client (ghi o KPlayer.cpp case UI_CITYINFO)
KCityInfoView g_ClientCityInfo[8];

int	KCoreShell::GetGameData(unsigned int uDataId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uDataId)
	{
	case GDI_CITY_INFO:			// [CITYINFO 21/08]
		if (uParam >= 1 && uParam <= 7 && nParam)
		{
			memcpy((void*)nParam, &g_ClientCityInfo[uParam], sizeof(KCityInfoView));
			nRet = g_ClientCityInfo[uParam].nCityId ? 1 : 0;
		}
		break;
	case GDI_CITY_INFO_CURMAP:	// [CITYINFO 21/08] thanh cua map dang dung
		if (nParam && Player[CLIENT_PLAYER_INDEX].m_nIndex > 0)
		{
			int nSW = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex;
			if (nSW >= 0 && nSW < MAX_SUBWORLD)
			{
				int nMapId = SubWorld[nSW].m_SubWorldID;
				for (int c = 1; c <= 7; c++)
				{
					if (g_ClientCityInfo[c].nCityId && g_ClientCityInfo[c].nMapId == nMapId)
					{
						memcpy((void*)nParam, &g_ClientCityInfo[c], sizeof(KCityInfoView));
						nRet = c;
						break;
					}
				}
			}
		}
		break;
""", nl)
assert hi(s) == h0; wr(p, s); print("CoreShell.cpp ok")

# ======== 4. KPlayer.cpp (client): case UI_CITYINFO ========
p = P("Core", "Src", "KPlayer.cpp"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, """					CoreDataChanged(GDCNI_TASK_VALUE_UPDATE, (unsigned int)nTaskId, nTaskVal);
					break;
				}
""", """					CoreDataChanged(GDCNI_TASK_VALUE_UPDATE, (unsigned int)nTaskId, nTaskVal);
					break;
				}
			case UI_CITYINFO:	// [CITYINFO 21/08] "id\\tstate\\ttax\\tprice\\tmapid\\tten\\tchu\\tthaithu"
				{
					extern KCityInfoView g_ClientCityInfo[8];
					char szBuf[512];
					int nLen = pScriptAction->m_nBufferLen;
					if (nLen <= 0 || nLen >= (int)sizeof(szBuf))
						break;
					memcpy(szBuf, pScriptAction->m_pContent, nLen);
					szBuf[nLen] = 0;
					char* pField[8];
					int nField = 0;
					char* pCur = szBuf;
					pField[nField++] = pCur;
					while (*pCur && nField < 8)
					{
						if (*pCur == '\\t')
						{
							*pCur = 0;
							pField[nField++] = pCur + 1;
						}
						pCur++;
					}
					if (nField < 8)
						break;
					int nCity = atoi(pField[0]);
					if (nCity < 1 || nCity > 7)
						break;
					KCityInfoView* pV = &g_ClientCityInfo[nCity];
					memset(pV, 0, sizeof(KCityInfoView));
					pV->nCityId = nCity;
					pV->nState = atoi(pField[1]);
					pV->nTax = atoi(pField[2]);
					pV->nPrice = atoi(pField[3]);
					pV->nMapId = atoi(pField[4]);
					strncpy(pV->szName, pField[5], sizeof(pV->szName) - 1);
					strncpy(pV->szOwner, pField[6], sizeof(pV->szOwner) - 1);
					strncpy(pV->szMaster, pField[7], sizeof(pV->szMaster) - 1);
					CoreDataChanged(GDCNI_CITY_INFO_UPDATE, (unsigned int)nCity, 0);
					break;
				}
""", nl)
assert hi(s) == h0; wr(p, s); print("KPlayer.cpp ok")

# ======== 5. KJx2CityWar.h/.cpp (server): sync ========
p = P("Core", "Src", "KJx2CityWar.h"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, "void KJx2CityWar_Breathe();\n",
            "void KJx2CityWar_Breathe();\n// [CITYINFO 21/08] day 7 thanh xuong client (UI_CITYINFO) - luc nap nhan vat, khi mo ban do, khi doi\nvoid KJx2CityWar_SyncToPlayer(int nPlayerIndex);\nvoid KJx2CityWar_SyncToAll();\n", nl)
assert hi(s) == h0; wr(p, s); print("KJx2CityWar.h ok")

p = P("Core", "Src", "KJx2CityWar.cpp"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, """	if (bOk)
	{
		MoveFileEx(szTmp, szPath, MOVEFILE_REPLACE_EXISTING);
#ifdef _SERVER
		// (20/08) Ghi xong tep thi day luon len MySQL. BAT DONG BO nen khong
		// cham vao vong lap game. Tu day tep chi con la ban dem cuc bo.
		KGameKV::PutFile(JX2CW_KV_NS, "file", szPath, true);
#endif
	}
""", """	if (bOk)
	{
		MoveFileEx(szTmp, szPath, MOVEFILE_REPLACE_EXISTING);
#ifdef _SERVER
		// (20/08) Ghi xong tep thi day luon len MySQL. BAT DONG BO nen khong
		// cham vao vong lap game. Tu day tep chi con la ban dem cuc bo.
		KGameKV::PutFile(JX2CW_KV_NS, "file", szPath, true);
#endif
		KJx2CityWar_SyncToAll();	// [CITYINFO 21/08] moi thay doi -> client cap nhat ban do/thue
	}
""", nl)
s = rep1(s, """void KJx2CityWar_Breathe()
{""", """// [CITYINFO 21/08] 7 goi PLAYER_SCRIPTACTION_SYNC UIId = UI_CITYINFO (kenh mo rong nhu
// UI_TASKVALUE - khong doi wire-format; client cu bo qua). Noi dung = chuoi tab.
void KJx2CityWar_SyncToPlayer(int nPlayerIndex)
{
	sEnsureStore();
	if (nPlayerIndex <= 0 || nPlayerIndex > PlayerSet.GetPlayerMaxNumber() || Player[nPlayerIndex].m_nIndex <= 0)
		return;
	if (Player[nPlayerIndex].m_nNetConnectIdx < 0)
		return;		// bot / chua co ket noi
	for (int c = 1; c <= 7; c++)
	{
		KJx2City* p = &s_Cities[c];
		PLAYER_SCRIPTACTION_SYNC sSync;
		sSync.m_nOperateType = SCRIPTACTION_UISHOW;
		sSync.m_bUIId = UI_CITYINFO;
		sSync.m_bOptionNum = 0;
		sSync.m_bParam1 = 0;
		sSync.m_bParam2 = 1;
		sSync.m_Select = 0;
		sSync.m_nParam = 0;
		int n = _snprintf(sSync.m_pContent, 250, "%d\\t%d\\t%d\\t%d\\t%d\\t%s\\t%s\\t%s",
			c, p->nState, p->nTax, p->nPriceParam, p->nMapIdCount ? p->nMapIds[0] : 0,
			p->szAreaName, p->szOwnerTong, p->szMaster);
		if (n <= 0)
			continue;
		sSync.m_pContent[n] = 0;
		sSync.m_nBufferLen = n;
		Player[nPlayerIndex].DoScriptAction(&sSync);
	}
}

void KJx2CityWar_SyncToAll()
{
	for (int i = 1; i <= PlayerSet.GetPlayerMaxNumber(); i++)
	{
		if (Player[i].m_nIndex > 0 && Player[i].m_nNetConnectIdx >= 0)
			KJx2CityWar_SyncToPlayer(i);
	}
}

void KJx2CityWar_Breathe()
{""", nl)
assert hi(s) == h0; wr(p, s); print("KJx2CityWar.cpp ok")

# ======== 6. server: login (KPlayerDBFuns.cpp) + c2sGetCityOwnTong ========
p = P("Core", "Src", "KPlayerDBFuns.cpp"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, "\t\tSyncTaskValueToClient(-1, 0);\t// [TaskGuide] bao client xoa gia tri nhan vat truoc\n",
            "\t\tSyncTaskValueToClient(-1, 0);\t// [TaskGuide] bao client xoa gia tri nhan vat truoc\n\t\t{\n\t\t\textern void KJx2CityWar_SyncToPlayer(int nPlayerIndex);\n\t\t\tKJx2CityWar_SyncToPlayer(m_nPlayerIndex);\t// [CITYINFO 21/08] 7 thanh cho ban do/thue\n\t\t}\n", nl)
assert hi(s) == h0; wr(p, s); print("KPlayerDBFuns.cpp ok")

p = P("Core", "Src", "KPlayer.cpp"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, """	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sView, sizeof(sView));
}

void KPlayer::RecoveryBox(DWORD dwID, int nX, int nY)
""", """	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sView, sizeof(sView));
	{
		extern void KJx2CityWar_SyncToPlayer(int nPlayerIndex);
		KJx2CityWar_SyncToPlayer(m_nPlayerIndex);	// [CITYINFO 21/08] mo ban do -> lam tuoi 7 thanh
	}
}

void KPlayer::RecoveryBox(DWORD dwID, int nX, int nY)
""", nl)
assert hi(s) == h0; wr(p, s); print("KPlayer.cpp (GetCityOwnTong) ok")

# ======== 7. Game.exe: GameSpaceChangedNotify ========
p = P("S3Client", "Ui", "GameSpaceChangedNotify.cpp"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, """	case GDCNI_RETURN_CITY_OWN_TONG:
	{
		KUiWorldmap* worldmap = KUiWorldmap::GetIfVisible();
		if (worldmap)
		{
			worldmap->SetCityOwnTong((char*)nParam);
		}
	}
	break;
""", """	case GDCNI_RETURN_CITY_OWN_TONG:
	{
		KUiWorldmap* worldmap = KUiWorldmap::GetIfVisible();
		if (worldmap)
		{
			worldmap->SetCityOwnTong((char*)nParam);
		}
	}
	break;
	case GDCNI_CITY_INFO_UPDATE:	// [CITYINFO 21/08] server day 1 thanh -> ve lai ban do + khung thue
	{
		KUiWorldmap* worldmap = KUiWorldmap::GetIfVisible();
		if (worldmap)
			worldmap->RefreshCityLabels();
		KUiMiniMap::UpdateCityInfo();
	}
	break;
""", nl)
assert hi(s) == h0; wr(p, s); print("GameSpaceChangedNotify.cpp ok")

# ======== 8. Game.exe: UiWorldMap ========
p = P("S3Client", "Ui", "UiCase", "UiWorldMap.h"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, "\tvoid SetCityOwnTong(const char* szTongName);\n",
            "\tvoid SetCityOwnTong(const char* szTongName);\n\tvoid RefreshCityLabels();\t// [CITYINFO 21/08] 7 nhan tu GDI_CITY_INFO\n", nl)
assert hi(s) == h0; wr(p, s); print("UiWorldMap.h ok")

p = P("S3Client", "Ui", "UiCase", "UiWorldMap.cpp"); s = rd(p); nl = nlof(s); h0 = hi(s)
lbl_owner = vn("Bang hội chiếm lĩnh: %s - Thuế %d%%")
lbl_none = vn("Vô chủ")
s = rep1(s, """void KUiWorldmap::CloseWindow()
{""", """// [CITYINFO 21/08] 7 nhan thanh theo thu tu CityID cua citywar.ini (1 Phuong Tuong, 2 Thanh Do,
// 3 Dai Ly, 4 Bien Kinh, 5 Tuong Duong, 6 Duong Chau, 7 Lam An) - ban cu chi set Tuong Duong.
void KUiWorldmap::RefreshCityLabels()
{
	KWndPureTextBtn* pBtn[8] = { NULL, &m_fengxiang, &m_chengdu, &m_dali, &m_bianjing, &m_xiangyang, &m_yangzhou, &m_linan };
	if (!g_pCoreShell)
		return;
	for (int c = 1; c <= 7; c++)
	{
		KCityInfoView sV;
		memset(&sV, 0, sizeof(sV));
		if (!g_pCoreShell->GetGameData(GDI_CITY_INFO, c, (int)&sV))
			continue;
		char szBuffer[160];
		_snprintf(szBuffer, sizeof(szBuffer) - 1, \"""" + lbl_owner + """\", sV.szOwner[0] ? sV.szOwner : \"""" + lbl_none + """\", sV.nTax);
		szBuffer[sizeof(szBuffer) - 1] = 0;
		pBtn[c]->SetText(szBuffer);
	}
}

void KUiWorldmap::CloseWindow()
{""", nl)
# UpdateData: cuoi ham (truoc "Ini.Clear();\n\t}\n}") -> xin server + ve nhan
s = rep1(s, """				sprintf(szBuffer, szTongInfo, "%s");
				m_xiangyang.SetText(szBuffer); //#tuong duong
""" % vn("Vô hủ "), """				sprintf(szBuffer, szTongInfo, "%s");
				m_xiangyang.SetText(szBuffer); //#tuong duong
				// [CITYINFO 21/08] 7 nhan tu ban sao client + xin server lam tuoi (tra ve UI_CITYINFO)
				RefreshCityLabels();
				g_pCoreShell->OperationRequest(GOI_GET_CITY_OWN_TONG, 0, 0);
""" % vn("Vô hủ "), nl) if False else s
# (chuoi "Vo hu " co the khac - tim theo m_xiangyang.SetText(szBuffer); //#tuong duong  o UpdateData)
idx = s.find("m_xiangyang.SetText(szBuffer); //#tuong duong", s.find("void KUiWorldmap::UpdateData()"))
assert idx > 0
eol = s.find(nl, idx) + len(nl)
ins = ("\t\t\t\t// [CITYINFO 21/08] 7 nhan tu ban sao client + xin server lam tuoi (tra ve UI_CITYINFO)" + nl
       + "\t\t\t\tRefreshCityLabels();" + nl
       + "\t\t\t\tg_pCoreShell->OperationRequest(GOI_GET_CITY_OWN_TONG, 0, 0);" + nl)
s = s[:eol] + ins + s[eol:]
assert hi(s) == h0 + hi(lbl_owner) + hi(lbl_none)
wr(p, s); print("UiWorldMap.cpp ok")

# ======== 9. Game.exe: UiMiniMap ========
p = P("S3Client", "Ui", "UiCase", "UiMiniMap.h"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, "\tstatic void\t\t\tUpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo);\n",
            "\tstatic void\t\t\tUpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo);\n\tstatic void\t\t\tUpdateCityInfo();\t// [CITYINFO 21/08] [CityInfo1]/[CityInfo2] ban do lon\n", nl)
s = rep1(s, "\tKWndText32\t\tm_SceneName;\n",
            "\tKWndText32\t\tm_SceneName;\n\tKWndText32\t\tm_CityInfo1;\t// [CITYINFO 21/08] Thai Thu + bang\n\tKWndText32\t\tm_CityInfo2;\t// [CITYINFO 21/08] thue + vat gia\n\tint\t\t\t\tm_bCityInfoOk;\t// ini co section CityInfo (chi ban do lon)\n", nl)
assert hi(s) == h0; wr(p, s); print("UiMiniMap.h ok")

p = P("S3Client", "Ui", "UiCase", "UiMiniMap.cpp"); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, "\tAddChild(&m_Shadow);\n\tAddChild(&m_SceneName);\n",
            "\tAddChild(&m_Shadow);\n\tAddChild(&m_SceneName);\n\tAddChild(&m_CityInfo1);\t// [CITYINFO 21/08]\n\tAddChild(&m_CityInfo2);\n\tm_bCityInfoOk = 0;\n", nl)
s = rep1(s, "\tm_SceneName.Init(pIni, \"SceneName\");\n\tm_ScenePos.Init(pIni, \"ScenePos\");\n",
"""	m_SceneName.Init(pIni, "SceneName");
	// [CITYINFO 21/08] chi UiMiniMapBig.ini co [CityInfo1]/[CityInfo2] (ban goc JX2)
	{
		int nW = 0;
		pIni->GetInteger("CityInfo1", "Width", 0, &nW);
		m_bCityInfoOk = (nW > 0) ? 1 : 0;
		if (m_bCityInfoOk)
		{
			m_CityInfo1.Init(pIni, "CityInfo1");
			m_CityInfo2.Init(pIni, "CityInfo2");
		}
		m_CityInfo1.Hide();
		m_CityInfo2.Hide();
	}
	m_ScenePos.Init(pIni, "ScenePos");
""", nl)
s = rep1(s, """		ms_pSelf->m_SceneName.SetText(pInfo->szSceneName);
		strcpy(ms_pSelf->m_szMapName,pInfo->szSceneName);
""", """		ms_pSelf->m_SceneName.SetText(pInfo->szSceneName);
		strcpy(ms_pSelf->m_szMapName,pInfo->szSceneName);
		UpdateCityInfo();	// [CITYINFO 21/08] doi map -> khung chu thanh/thue
""", nl)
ci1_fmt = vn("Thái Thú: %s - Bang hội: %s")
ci1_none = vn("Thành này chưa có Thái Thú")
ci2_fmt = vn("Thuế giao dịch: %d%% - Chỉ số vật giá: %d")
s = rep1(s, """void KUiMiniMap::Show()
{""", """// [CITYINFO 21/08] ban do lon: 2 dong [CityInfo1]/[CityInfo2] cho map thanh (GDI_CITY_INFO_CURMAP)
void KUiMiniMap::UpdateCityInfo()
{
	if (!ms_pSelf || !ms_pSelf->m_bCityInfoOk || !g_pCoreShell)
		return;
	KCityInfoView sV;
	memset(&sV, 0, sizeof(sV));
	int nCity = g_pCoreShell->GetGameData(GDI_CITY_INFO_CURMAP, 0, (int)&sV);
	if (nCity <= 0)
	{
		ms_pSelf->m_CityInfo1.Hide();
		ms_pSelf->m_CityInfo2.Hide();
		return;
	}
	char szLine[160];
	if (sV.szOwner[0])
		_snprintf(szLine, sizeof(szLine) - 1, \"""" + ci1_fmt + """\", sV.szMaster, sV.szOwner);
	else
		strcpy(szLine, \"""" + ci1_none + """\");
	szLine[sizeof(szLine) - 1] = 0;
	ms_pSelf->m_CityInfo1.SetText(szLine);
	_snprintf(szLine, sizeof(szLine) - 1, \"""" + ci2_fmt + """\", sV.nTax, (sV.nPrice < 0) ? 10 : sV.nPrice);
	szLine[sizeof(szLine) - 1] = 0;
	ms_pSelf->m_CityInfo2.SetText(szLine);
	ms_pSelf->m_CityInfo1.Show();
	ms_pSelf->m_CityInfo2.Show();
}

void KUiMiniMap::Show()
{""", nl)
s = rep1(s, """void KUiMiniMap::Show()
{
	KWndImage::Show();
""", """void KUiMiniMap::Show()
{
	KWndImage::Show();
	UpdateCityInfo();	// [CITYINFO 21/08]
""", nl)
assert hi(s) == h0 + hi(ci1_fmt) + hi(ci1_none) + hi(ci2_fmt)
wr(p, s); print("UiMiniMap.cpp ok")
print("OK")
