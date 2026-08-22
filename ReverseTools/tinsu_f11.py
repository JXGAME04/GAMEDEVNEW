# -*- coding: utf-8 -*-
r"""Chi Nam Nhiem Vu (F11): them muc 'Nhiem vu Tin Su' (TaskId 7) - UiTaskGuide.cpp/.h + UiTaskGuideStr.h
+ client UI\uitasklist.ini. Chuoi Viet = raw TCVN3 (vn_edit.vn), tep doc/ghi latin-1."""
import io, os, sys
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
D = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase"
CPP = os.path.join(D, "UiTaskGuide.cpp"); H = os.path.join(D, "UiTaskGuide.h"); STR = os.path.join(D, "UiTaskGuideStr.h")
INI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\UI\uitasklist.ini"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) > 127)
def nlof(s): return "\r\n" if "\r\n" in s else "\n"
def rep1(s, old, new, nl):
    old = old.replace("\r\n", "\n").replace("\n", nl); new = new.replace("\r\n", "\n").replace("\n", nl)
    assert s.count(old) == 1, (s.count(old), old[:80])
    return s.replace(old, new, 1)

# ---------- UiTaskGuideStr.h ----------
s = rd(STR); nl = nlof(s); h0 = hi(s)
assert "TS_NOTASK" not in s
strs = [
 ("TS_NOTASK",      vn("Bạn chưa tiếp nhận nhiệm vụ Tín Sứ. Hãy đến gặp Dịch Quan ở Thành Đô hoặc Đại Lý (cần cấp 90) để nhận nhiệm vụ.")),
 ("TS_ROUTE_FMT",   vn("Nhiệm vụ Tín Sứ đặc biệt: đưa tin từ <color=yellow>%s<color> đến <color=yellow>%s<color>.")),
 ("TS_ROUTE_1A",    vn("Thành Đô")),
 ("TS_ROUTE_1B",    vn("Đại Lý")),
 ("TS_ST10",        vn("<color=green>[Đang làm]<color> Hãy tìm Xa Phu chọn <color=yellow>Đi nơi đặc biệt làm Nhiệm vụ Tín Sứ<color> để vào ải Thiên Bảo Khố, rồi đối thoại với Dịch Quan trong ải.")),
 ("TS_ST20_FMT",    vn("<color=green>[Đang làm]<color> Trong Thiên Bảo Khố: mở <color=yellow>5<color> Bảo Rương đúng thứ tự <color=yellow>%s<color>. Đã mở: <color=yellow>%s<color> (%d/5). Xong thì tìm Tiêu Trấn để ra ải.")),
 ("TS_ST21",        vn("<color=green>[Đang làm]<color> Nhiệm vụ tạm ngưng. Vào lại ải Thiên Bảo Khố qua Xa Phu và đối thoại với Dịch Quan trong ải để tiếp tục.")),
 ("TS_ST25",        vn("<color=yellow>[Hoàn thành một phần]<color> Đã rời ải khi chưa mở đủ rương. Về gặp Dịch Quan ở thành đích để trả nhiệm vụ.")),
 ("TS_ST30",        vn("<color=yellow>[Hoàn thành - về gặp Dịch Quan]<color> Đã vượt ải Thiên Bảo Khố! Về <color=yellow>%s<color> gặp Dịch Quan nhận thưởng (cần 5 ô trống hành trang).")),
 ("TS_NONE",        vn("(chưa mở)")),
 ("TS_POINT_FMT",   vn("Điểm tích luỹ Tín Sứ: <color=yellow>%d<color>. Danh hiệu Tín Sứ: <color=yellow>%s<color>. Hôm nay đã hoàn thành: %d lần.")),
 ("TS_TITLE_0",     vn("chưa có")),
 ("TS_TITLE_1",     vn("Mộc")),
 ("TS_TITLE_2",     vn("Đồng")),
 ("TS_TITLE_3",     vn("Ngân")),
 ("TS_TITLE_4",     vn("Kim")),
 ("TS_TITLE_5",     vn("Ngự Tứ")),
 ("TS_BRIEF_NONE",  vn("Chưa nhận nhiệm vụ Tín Sứ.")),
]
add = nl + "// [TIN SU 21/08] Chi nam nhiem vu - muc Nhiem vu Tin Su (TaskId 7), them tay (gen_strings.py da mat)" + nl
for k, v in strs:
    assert '"' not in v
    add += '#define %s "%s"' % (k, v) + nl
s = s.rstrip("\r\n") + nl + add
assert hi(s) == h0 + sum(hi(v) for _, v in strs)
wr(STR, s); print("UiTaskGuideStr.h +%d chuoi" % len(strs))

# ---------- UiTaskGuide.h ----------
s = rd(H); nl = nlof(s); h0 = hi(s)
s = rep1(s, "\tvoid\tBuildDaTauText();\t\t\t// noi dung nhiem vu Da Tau (TaskId 6)\n",
            "\tvoid\tBuildDaTauText();\t\t\t// noi dung nhiem vu Da Tau (TaskId 6)\n\tvoid\tBuildTinSuText();\t\t\t// [TIN SU 21/08] noi dung nhiem vu Tin Su (TaskId 7)\n", nl)
assert hi(s) == h0; wr(H, s); print("UiTaskGuide.h ok")

# ---------- UiTaskGuide.cpp ----------
s = rd(CPP); nl = nlof(s); h0 = hi(s)
s = rep1(s, "#define TASKGUIDE_DATAU_TASKID\t6\n",
            "#define TASKGUIDE_DATAU_TASKID\t6\n#define TASKGUIDE_TINSU_TASKID\t7\t// [TIN SU 21/08] task 1201..1218 (nt_setTask -> SetSaveVal -> UI_TASKVALUE)\n", nl)
s = rep1(s, """	if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_DATAU_TASKID)
	{
		m_pSelf->BuildDaTauText();
		m_pSelf->UpdateButtons();	// course doi -> nut Bo nhiem vu doi trang thai
	}
}
""", """	if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_DATAU_TASKID)
	{
		m_pSelf->BuildDaTauText();
		m_pSelf->UpdateButtons();	// course doi -> nut Bo nhiem vu doi trang thai
	}
	else if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_TINSU_TASKID)
	{
		if (nTaskId >= 1201 && nTaskId <= 1218)
			m_pSelf->BuildTinSuText();
	}
}
""", nl)
s = rep1(s, """	if (pEntry->nTaskId == TASKGUIDE_DATAU_TASKID)
	{
		BuildDaTauText();
	}
	else
	{
		m_Content.Clear();
		AddLine(DTG_NO_SUPPORT);
	}
	UpdateButtons();
}
""", """	if (pEntry->nTaskId == TASKGUIDE_DATAU_TASKID)
	{
		BuildDaTauText();
	}
	else if (pEntry->nTaskId == TASKGUIDE_TINSU_TASKID)
	{
		BuildTinSuText();
	}
	else
	{
		m_Content.Clear();
		AddLine(DTG_NO_SUPPORT);
	}
	UpdateButtons();
}

// [TIN SU 21/08] Soan noi dung nhiem vu Tin Su (port Linux task\\tollgate\\messenger):
//   1204 = tuyen (1 Thanh Do->Dai Ly, 2 Dai Ly->Thanh Do; 0 = chua nhan)
//   1203 = buoc ai Thien Bao Kho: 10 da nhan / 20 dang trong ai / 21 tam ngung /
//          25 ra ai chua du ruong / 30 da vuot ai (posthouse.lua, messenger_turenpc.lua)
//   1201 = ma thu tu 5 ruong phai mo (5 chu so), 1202 = ruong da mo theo thu tu
//   1205 = diem tich luy, 1206 = cap danh hieu, 1218 = so lan hoan thanh hom nay
static void TS_Digits(int nCode, char* szOut, int nOutSize, int* pnCount)
{
	char szRev[16];
	int n = 0;
	while (nCode > 0 && n < 10)
	{
		szRev[n++] = (char)('0' + (nCode % 10));
		nCode /= 10;
	}
	int k = 0;
	for (int i = n - 1; i >= 0 && k < nOutSize - 2; i--)
	{
		if (k)
			szOut[k++] = ',';
		szOut[k++] = szRev[i];
	}
	szOut[k] = 0;
	if (pnCount)
		*pnCount = n;
}

void KUiTaskGuide::BuildTinSuText()
{
	m_Content.Clear();
	int nRoute = DTG_TaskVal(1204);
	int nState = DTG_TaskVal(1203);
	if (nRoute == 0 || nState == 0)
	{
		AddLine(TS_NOTASK);
	}
	else
	{
		char szLine[1024];
		const char* szFrom = (nRoute == 2) ? TS_ROUTE_1B : TS_ROUTE_1A;
		const char* szTo   = (nRoute == 2) ? TS_ROUTE_1A : TS_ROUTE_1B;
		sprintf(szLine, TS_ROUTE_FMT, szFrom, szTo);
		AddLine(szLine);
		switch (nState)
		{
		case 10:
			AddLine(TS_ST10);
			break;
		case 20:
			{
				char szNeed[32], szDone[32];
				int nDone = 0;
				TS_Digits(DTG_TaskVal(1201), szNeed, sizeof(szNeed), NULL);
				TS_Digits(DTG_TaskVal(1202), szDone, sizeof(szDone), &nDone);
				if (!szDone[0])
					strcpy(szDone, TS_NONE);
				sprintf(szLine, TS_ST20_FMT, szNeed, szDone, nDone);
				AddLine(szLine);
			}
			break;
		case 21:
			AddLine(TS_ST21);
			break;
		case 25:
			AddLine(TS_ST25);
			break;
		case 30:
			sprintf(szLine, TS_ST30, szTo);
			AddLine(szLine);
			break;
		default:
			AddLine(TS_ST10);
			break;
		}
	}
	static const char* s_szTitle[6] = { TS_TITLE_0, TS_TITLE_1, TS_TITLE_2, TS_TITLE_3, TS_TITLE_4, TS_TITLE_5 };
	int nTitle = DTG_TaskVal(1206);
	if (nTitle < 0 || nTitle > 5)
		nTitle = 0;
	char szPt[512];
	sprintf(szPt, TS_POINT_FMT, DTG_TaskVal(1205), s_szTitle[nTitle], DTG_TaskVal(1218));
	AddLine(szPt);
}
""", nl)
assert hi(s) == h0; wr(CPP, s); print("UiTaskGuide.cpp ok")

# ---------- uitasklist.ini (client dang chay) ----------
s = rd(INI); nl = nlof(s); h0 = hi(s)
assert "TaskId=7" not in s
sec = "[2]" + nl + "Name=+" + vn("Nhiệm vụ Tín Sứ") + nl + "TaskId=7" + nl + "Type=1" + nl + "Class=2" + nl + "Trace=0" + nl
s = s.rstrip("\r\n") + nl + nl + sec
wr(INI, s); print("uitasklist.ini + [2] Tin Su (byte cao %d -> %d)" % (h0, hi(s)))
