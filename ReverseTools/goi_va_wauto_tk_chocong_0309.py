# -*- coding: utf-8 -*-
"""
goi_va_wauto_tk_chocong_0309.py - bo va idempotent (03/09/2026 toi, DOT 3) cho WAuto Tong Kim.
Chu game (20:2x, sau khi swap 4c69d7ad): "toi thay no di chuyen chay lang qua lang ve".

Do jx_auto.log pid 23876 (+112..+143 s): nhan vat chay ve Tay Nam ~6 giay roi bi may chu SetPos
ve DUNG MOT diem (1571,3196) [S6-ME nhanh=loadmap svmps=(50272,102272)], lap 4 lan, toi khi tran
mo (+143 s) moi di tiep. Nguon: lib_tktc.lua:659-661 dat 3 vet trap kim_chancong.lua (va 655-657
tong_chancong.lua) = "Chan cong 3 trap khi chua bat dau": con hen gio 1 (timerserver.lua:620
StartMissionTimer(MS_TONGKIM,1, TIME_BD_TK phut bao danh)) thi ai qua cong bi SetPos(1571,3196) /
SetPos(1332,3443) + Talk "Dai chien Tong Kim con <color=red>N<color> giay se bat dau". Het hen gio:
task01.lua:62 Talk "Tong Kim Dai chien chinh thuc bat dau!" + loa 10651. Auto rao map ngay khi con
dem nguoc nen bi nem ve lien tuc.

Sua (chi CoreShell.cpp, chi CoreClient.dll):
  1. TK_Process doc thoai Talk (g_sDTCap.szTalk / uTalkSeq): "... con N giay ..." -> dung cho N giay
     (khong rao, khong san, van an thuoc, may PK van danh tra); "chinh thuc bat dau" -> di ngay.
  2. Du phong: trong TKP_FIGHT nhay >= 1500 mps trong mot nhip (khong chet) va roi dung diem chan
     cong -> cho 20 giay.
  3. TK_RaoDung: loai o rao trong 480 mps quanh vet trap VAO TRAI (tongvaotrai/kimvaotrai - giam vao
     la bi keo vao hau doanh, phai ngoi ~80 giay moi ra lai).
Chuoi thoai + toa do trap/diem ve duoc TRICH THANG tu script may chu (byte-for-byte, TCVN3) luc chay
tool - khong go tay; KHONG sinh lai KTongKimTables.h vi TAB_TIME_TONG_KIM trong lua da doi dang
TK_CFG(...) va bang gio g_TKGio trong .h khac lua hien tai (sinh lai se doi gio mac dinh tab WAuto).
Chay:  python goi_va_wauto_tk_chocong_0309.py [--thu] [--root <Sources\\Core\\Src>] [--srv <bin\\server\\script>]
"""
import io, sys, os, re

ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
if "--root" in sys.argv:
    ROOT = sys.argv[sys.argv.index("--root") + 1]
if "--srv" in sys.argv:
    SRV = sys.argv[sys.argv.index("--srv") + 1]
THU = "--thu" in sys.argv

_TV = ("b5:à b6:ả b7:ã b8:á b9:ạ a8:ă bb:ằ bc:ẳ bd:ẵ be:ắ c6:ặ a9:â c7:ầ c8:ẩ c9:ẫ ca:ấ cb:ậ a7:Đ ae:đ "
       "cc:è ce:ẻ cf:ẽ d0:é d1:ẹ aa:ê d2:ề d3:ể d4:ễ d5:ế d6:ệ d7:ì d8:ỉ dc:ĩ dd:í de:ị df:ò e1:ỏ e2:õ e3:ó e4:ọ "
       "ab:ô e5:ồ e6:ổ e7:ỗ e8:ố e9:ộ ac:ơ ea:ờ eb:ở ec:ỡ ed:ớ ee:ợ ef:ù f1:ủ f2:ũ f3:ú f4:ụ ad:ư f5:ừ f6:ử f7:ữ "
       "f8:ứ f9:ự fa:ỳ fb:ỷ fc:ỹ fd:ý fe:ỵ a1:Ă a2:Â a3:Ê a4:Ô a5:Ơ a6:Ư")
_U2T = {v: chr(int(k, 16)) for k, v in (x.split(":") for x in _TV.split())}
_T2U = {v: k for k, v in _U2T.items()}

def tcvn3(s):
    out = []
    for ch in s:
        if ord(ch) < 0x80:
            out.append(ch)
        elif ch in _U2T:
            out.append(_U2T[ch])
        else:
            raise SystemExit("khong ma hoa duoc TCVN3: %r trong %r" % (ch, s))
    return "".join(out)

def doc_vn(s):
    return "".join(_T2U.get(c, c) for c in s)

def rd(path):
    return io.open(path, "r", encoding="latin-1", newline="").read()

def cstr(s):
    """chuoi C giu nguyen byte (latin-1)"""
    out = []
    for ch in s:
        if ch == "\\":
            out.append("\\\\")
        elif ch == '"':
            out.append('\\"')
        elif ord(ch) < 0x20:
            raise SystemExit("chuoi co ky tu dieu khien: %r" % s)
        else:
            out.append(ch)
    return '"%s"' % "".join(out)

# ---------------- trich tu script may chu ----------------
TRAP = os.path.join(SRV, "maps", "tongkim", "trap")
s_kim = rd(os.path.join(TRAP, "kim_chancong.lua"))
s_tong = rd(os.path.join(TRAP, "tong_chancong.lua"))
s_task01 = rd(os.path.join(SRV, "timertask", "task01.lua"))
s_lib = rd(os.path.join(SRV, "tinhnang", "tong_kim_tcap", "lib_tktc.lua"))

def pick1(text, pat, what):
    m = re.findall(pat, text)
    if len(m) != 1:
        raise SystemExit("KHONG trich duoc %s (%d ket qua)" % (what, len(m)))
    return m[0]

MK_CON = pick1(s_kim, r'Talk\(1,\s*"",\s*"([^"]*?)<color=red>', "thoai chan cong (kim_chancong.lua)")
MK_CON2 = pick1(s_tong, r'Talk\(1,\s*"",\s*"([^"]*?)<color=red>', "thoai chan cong (tong_chancong.lua)")
if MK_CON != MK_CON2:
    raise SystemExit("hai script chan cong noi khac nhau: %r vs %r" % (doc_vn(MK_CON), doc_vn(MK_CON2)))
MK_BATDAU = pick1(s_task01, r'Talk\(1,\s*"",\s*"([^"]+)"\)', "thoai bat dau (task01.lua)")
VE_KIM = pick1(s_kim, r'SetPos\((\d+),\s*(\d+)\)', "SetPos kim_chancong")
VE_TONG = pick1(s_tong, r'SetPos\((\d+),\s*(\d+)\)', "SetPos tong_chancong")
VAO_TONG = pick1(s_lib, r'AddTrapEx2\(MAP_TK_TC,\s*(\d+),\s*(\d+),\s*(\d+),\s*"[^"]*tongvaotrai\.lua"\)', "trap tongvaotrai")
VAO_KIM = pick1(s_lib, r'AddTrapEx2\(MAP_TK_TC,\s*(\d+),\s*(\d+),\s*(\d+),\s*"[^"]*kimvaotrai\.lua"\)', "trap kimvaotrai")
def _asc(s):
    return doc_vn(s).encode("ascii", "backslashreplace").decode("ascii")
print("marker  : %s" % _asc(MK_CON))
print("bat dau : %s" % _asc(MK_BATDAU))
print("ve cong : Tong %s / Kim %s ; trap vao trai: Tong %s / Kim %s" % (VE_TONG, VE_KIM, VAO_TONG, VAO_KIM))

class Tep(object):
    def __init__(self, path):
        self.path = path
        self.s = rd(path)
        self.goc = self.s
        self.nl = "\r\n" if self.s.count("\r\n") > self.s.count("\n") // 2 else "\n"
        self.log = []
    def N(self, text):
        return text.replace("\r\n", "\n").replace("\n", self.nl)
    def them_sau(self, neo, moi, dau):
        neo = self.N(neo); moi = self.N(moi); dau = self.N(dau)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60]); return
        if self.s.count(neo) != 1:
            raise SystemExit("neo khong duy nhat (%d): %r" % (self.s.count(neo), neo[:70]))
        self.s = self.s.replace(neo, neo + moi, 1)
        self.log.append("  them sau: %s" % dau[:60])
    def thay(self, cu, moi, dau):
        cu = self.N(cu); moi = self.N(moi); dau = self.N(dau)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60]); return
        n = self.s.count(cu)
        if n != 1:
            raise SystemExit("doan can thay khong duy nhat (%d): %r" % (n, cu[:90]))
        self.s = self.s.replace(cu, moi, 1)
        self.log.append("  thay: %s" % dau[:60])
    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path)); return
        hb_goc = sum(1 for c in self.goc if ord(c) >= 0x80)
        hb_moi = sum(1 for c in self.s if ord(c) >= 0x80)
        print("%s: %d dong log, byte cao %d -> %d" % (os.path.basename(self.path), len(self.log), hb_goc, hb_moi))
        for l in self.log: print(l)
        if not THU:
            io.open(self.path, "w", encoding="latin-1", newline="").write(self.s)

f = Tep(os.path.join(ROOT, "CoreShell.cpp"))

# =====================================================================
# 1. bien + hang so (sau s_uTKRaoTickT cua dot 2)
# =====================================================================
f.them_sau(
    "static UINT  s_uTKRaoTickT = 0;		// nhip TK_RaoDi lan cuoi (vua danh / san xong thi do lai tu dau)\n",
    "// ===== (03/09 toi, dot 3) TRAN CHUA BAT DAU - trap chan cong =====\n"
    "// lib_tktc.lua addtraptongkimtrungcap: 3 vet trap kim_chancong.lua + 3 vet tong_chancong.lua\n"
    "// = \"Chan cong 3 trap khi chua bat dau\": con hen gio 1 (bao danh, timerserver.lua:620) thi ai\n"
    "// qua cong bi SetPos ve diem duoi + Talk \"... con <color=red>N<color> giay se bat dau\". Het hen\n"
    "// gio: task01.lua ontime_tongkim Talk \"... chinh thuc bat dau!\" cho tung nguoi. Log 03/09 20:17:\n"
    "// auto rao ngay khi con dem nguoc -> cu 6 giay bi nem ve (1571,3196) mot lan, chu game thay\n"
    "// \"chay lang qua lang ve\". Chuoi/toa do duoi TRICH THANG tu script (goi_va_wauto_tk_chocong_0309.py).\n"
    "static const char TKM_TALK_CHUABATDAU[] = " + cstr(MK_CON) + ";\n"
    "static const char TKM_TALK_BATDAU[] = " + cstr(MK_BATDAU) + ";\n"
    "static const TKPoint g_TKChanCongVe[2] = { { %s, %s }, { %s, %s } };	// tong_chancong / kim_chancong SetPos\n" % (VE_TONG[0], VE_TONG[1], VE_KIM[0], VE_KIM[1]) +
    "static const TKPoint g_TKTrapVaoA = { %s, %s };	// tongvaotrai.lua (AddTrapEx2 goc + %s o cheo)\n" % (VAO_TONG[0], VAO_TONG[1], VAO_TONG[2]) +
    "static const TKPoint g_TKTrapVaoB = { %s, %s };	// kimvaotrai.lua\n" % (VAO_KIM[0], VAO_KIM[1]) +
    "#define TK_TRAPVAO_LEN	%s\n" % VAO_TONG[2] +
    "static UINT s_uTKTalkSeen = 0;		// uTalkSeq da xem\n"
    "static UINT s_uTKChoBatDau = 0;		// != 0: dung cho toi moc nay (uCurTime) vi tran chua bat dau\n"
    "static int  s_nTKPosX = 0, s_nTKPosY = 0;	// vi tri nhip FIGHT truoc (nhan dien bi nem ve)\n"
    "static UINT s_uTKPosT = 0;\n",
    "static UINT s_uTKChoBatDau = 0;")

# =====================================================================
# 2. TK_RaoDung: tranh vet trap VAO TRAI
# =====================================================================
f.thay(
    "		if (g_GetDistance(mx, my, nCamAX, nCamAY) < TK_SAN_CAMTRAI\n"
    "		 || g_GetDistance(mx, my, nCamBX, nCamBY) < TK_SAN_CAMTRAI)\n"
    "			continue;\n"
    "		s_aTKRaoX[nSo] = (short)bx;\n",
    "		if (g_GetDistance(mx, my, nCamAX, nCamAY) < TK_SAN_CAMTRAI\n"
    "		 || g_GetDistance(mx, my, nCamBX, nCamBY) < TK_SAN_CAMTRAI)\n"
    "			continue;\n"
    "		// (03/09 toi, dot 3) tranh vet trap VAO TRAI cua ca hai phe (tongvaotrai/kimvaotrai): phe minh\n"
    "		// giam vao la bi keo vao hau doanh, dong ho TIME_IN_TRAI bat lai -> ngoi ~80 giay moi ra duoc.\n"
    "		if (g_GetDistance(mx, my, TK_O((int)g_TKTrapVaoA.x + TK_TRAPVAO_LEN / 2), TK_O((int)g_TKTrapVaoA.y + TK_TRAPVAO_LEN / 2)) < 480\n"
    "		 || g_GetDistance(mx, my, TK_O((int)g_TKTrapVaoB.x + TK_TRAPVAO_LEN / 2), TK_O((int)g_TKTrapVaoB.y + TK_TRAPVAO_LEN / 2)) < 480)\n"
    "			continue;\n"
    "		s_aTKRaoX[nSo] = (short)bx;\n",
    "tranh vet trap VAO TRAI cua ca hai phe")

# =====================================================================
# 3. TK_Pha: dat lai moc do nhay
# =====================================================================
f.thay(
    "	s_uTKSanQuyen = 0;\n"
    "	TK_SanBo();\n"
    "	ea.uTKPhaseT = uCurTime;\n",
    "	s_uTKSanQuyen = 0;\n"
    "	TK_SanBo();\n"
    "	s_uTKPosT = 0;		// (03/09 toi, dot 3) doi pha (hoi sinh / ra trai) la nhay hop le - khong tinh la bi nem ve\n"
    "	ea.uTKPhaseT = uCurTime;\n",
    "s_uTKPosT = 0;		// (03/09 toi, dot 3)")

# =====================================================================
# 4. TK_Process: doc thoai Talk (chua bat dau / bat dau)
# =====================================================================
MSG_CON = tcvn3("<color=Yellow>Trận chưa bắt đầu - còn %d giây, đứng chờ trước cổng.")
MSG_BATDAU = tcvn3("<color=Green>Trận đã bắt đầu - xuất quân.")
MSG_NEM = tcvn3("<color=Yellow>Bị đưa về cổng - trận chưa bắt đầu, đứng chờ.")
f.thay(
    "	ea.uTKMsgSeen = cap.uMsgSeq;\n"
    "	const int nLoaMo = (nTinMo || uCurTime < ea.uTKMoT);\n",
    "	ea.uTKMsgSeen = cap.uMsgSeq;\n"
    "	// (03/09 toi, dot 3) thoai Talk cua trap chan cong / cua ontime_tongkim - xem chu thich TKM_TALK_CHUABATDAU\n"
    "	if (cap.uTalkSeq != s_uTKTalkSeen)\n"
    "	{\n"
    "		s_uTKTalkSeen = cap.uTalkSeq;\n"
    "		if (DT_Has(cap.szTalk, TKM_TALK_CHUABATDAU))\n"
    "		{\n"
    "			const char* p = strstr(cap.szTalk, TKM_TALK_CHUABATDAU) + (sizeof(TKM_TALK_CHUABATDAU) - 1);\n"
    "			while (*p && (*p < '0' || *p > '9'))\n"
    "				++p;		// bo qua <color=red>\n"
    "			int nGiay = atoi(p);\n"
    "			if (nGiay < 1)\n"
    "				nGiay = 20;\n"
    "			else if (nGiay > 1800)\n"
    "				nGiay = 1800;\n"
    "			s_uTKChoBatDau = uCurTime + (UINT)nGiay * 1000u + 1500u;\n"
    "			char szCho[160];\n"
    "			sprintf(szCho, " + cstr(MSG_CON) + ", nGiay);\n"
    "			TK_Msg(nPlayerIdx, szCho);\n"
    "			AUTOLOG(\"[TK-CHO] thoai chan cong: tran chua bat dau, con %d giay - dung cho. me=(%d,%d)\", nGiay, nX, nY);\n"
    "		}\n"
    "		else if (DT_Has(cap.szTalk, TKM_TALK_BATDAU))\n"
    "		{\n"
    "			if (s_uTKChoBatDau)\n"
    "				TK_Msg(nPlayerIdx, " + cstr(MSG_BATDAU) + ");\n"
    "			s_uTKChoBatDau = 0;\n"
    "			AUTOLOG(\"[TK-CHO] thoai: tran da bat dau - xuat quan. me=(%d,%d)\", nX, nY);\n"
    "		}\n"
    "	}\n"
    "	const int nLoaMo = (nTinMo || uCurTime < ea.uTKMoT);\n",
    "[TK-CHO] thoai chan cong")

# =====================================================================
# 5. TKP_FIGHT: nhan dien bi nem ve + dung cho
# =====================================================================
f.thay(
    "		if (TK_AnThuoc(nPlayerIdx, pAp, uCurTime))\n"
    "			return 2;\n"
    "		// (03/09 chieu) THU TU MOI - chu game: \"tim doi thu KHAC MAU, khong chay toa do\":\n",
    "		if (TK_AnThuoc(nPlayerIdx, pAp, uCurTime))\n"
    "			return 2;\n"
    "		// (03/09 toi, dot 3) DU PHONG khi khong bat duoc thoai: nhay >= 1500 mps trong mot nhip (khong\n"
    "		// chet - chet da re sang TKP_CAMP o tren) va roi dung diem SetPos cua trap chan cong => tran chua\n"
    "		// bat dau, cho 20 giay (thoai \"con N giay\" neu bat duoc se dat moc chinh xac hon).\n"
    "		if (s_uTKPosT && (int)(uCurTime - s_uTKPosT) <= 1200\n"
    "		 && g_GetDistance(nX, nY, s_nTKPosX, s_nTKPosY) >= 1500)\n"
    "		{\n"
    "			for (int k = 0; k < 2; ++k)\n"
    "			{\n"
    "				if (g_GetDistance(nX, nY, TK_O((int)g_TKChanCongVe[k].x), TK_O((int)g_TKChanCongVe[k].y)) >= 300)\n"
    "					continue;\n"
    "				if (!s_uTKChoBatDau || (int)(s_uTKChoBatDau - uCurTime) < 20000)\n"
    "					s_uTKChoBatDau = uCurTime + 20000u;\n"
    "				TK_Msg(nPlayerIdx, " + cstr(MSG_NEM) + ");\n"
    "				AUTOLOG(\"[TK-CHO] bi nem ve diem chan cong (%d,%d) tu (%d,%d) - cho 20 giay\", nX, nY, s_nTKPosX, s_nTKPosY);\n"
    "				break;\n"
    "			}\n"
    "		}\n"
    "		s_nTKPosX = nX;\n"
    "		s_nTKPosY = nY;\n"
    "		s_uTKPosT = uCurTime;\n"
    "		if (s_uTKChoBatDau)\n"
    "		{\n"
    "			if ((int)(uCurTime - s_uTKChoBatDau) >= 0)\n"
    "			{\n"
    "				s_uTKChoBatDau = 0;\n"
    "				AUTOLOG(\"[TK-CHO] het gio cho - rao / san tiep. me=(%d,%d)\", nX, nY);\n"
    "			}\n"
    "			else\n"
    "			{	// dung cho truoc cong: khong rao, khong san; may PK van chay (danh tra neu bi danh)\n"
    "				ea.uNpcID = 0;\n"
    "				s_uTKSanQuyen = 0;\n"
    "				TK_SanBo();\n"
    "				if (SubWorld[0].HaveTarget(nAimTmpX, nAimTmpY))\n"
    "				{\n"
    "					g_ScenePlace.RemoveFlag();\n"
    "					SubWorld[0].StopPath();	// duong cu (xuyen cong) phai bo, khong thi buoc them la lai bi nem\n"
    "				}\n"
    "				AUTOLOG_EVERY(5000, \"[TK-CHO] dung cho tran bat dau - con %d giay. me=(%d,%d)\", (int)(s_uTKChoBatDau - uCurTime) / 1000, nX, nY);\n"
    "				return 2;\n"
    "			}\n"
    "		}\n"
    "		// (03/09 chieu) THU TU MOI - chu game: \"tim doi thu KHAC MAU, khong chay toa do\":\n",
    "[TK-CHO] bi nem ve diem chan cong")

# bien tam cho HaveTarget (xuat tham so) - khai bao o dau case TKP_FIGHT
f.thay(
    "	case TKP_FIGHT:\n"
    "	{\n"
    "		ea.nTKHold = 2;\n",
    "	case TKP_FIGHT:\n"
    "	{\n"
    "		ea.nTKHold = 2;\n"
    "		int nAimTmpX = 0, nAimTmpY = 0;	// (03/09 toi, dot 3) HaveTarget XUAT target ra tham so\n",
    "int nAimTmpX = 0, nAimTmpY = 0;")

# =====================================================================
# 6. TK_SanNguoi: dung yen trong tam PK 3 giay -> bo ngay (khong tinh lai duong 2 lan)
# =====================================================================
f.thay(
    "	else if (uCurTime - s_uTKSanMoveT > 3000u)\n"
    "	{\n"
    "		if (s_nTKSanRepath < 2)\n"
    "		{	// PHAI co RemoveFlag: chi ha uDTPath la vo tac dung vi DT_WalkTo con cua\n"
    "			// thu hai so dich cu (\"tx != nX || ty != nY\") ma dich thi khong doi.\n"
    "			g_ScenePlace.RemoveFlag();\n"
    "			ea.uDTPath = 0;\n"
    "			++s_nTKSanRepath;\n"
    "			s_uTKSanMoveT = uCurTime;\n"
    "		}\n"
    "		else\n"
    "		{\n"
    "			Player[nPlayerIdx].m_mAutoExcludeNpcID[uID] = uCurTime + 60000;\n"
    "			TK_SanBo();\n"
    "			AUTOLOG(\"[TK-SAN-BO] khong toi duoc id=%u - cam 60 giay\", uID);\n"
    "			return 0;\n"
    "		}\n"
    "	}\n",
    "	else if (uCurTime - s_uTKSanMoveT > 3000u)\n"
    "	{\n"
    "		// (03/09 toi, dot 3) DANG TRONG TAM PK ma dung yen 3 giay = tang danh khong nhan (khong duong\n"
    "		// nhin - tuong chan), tinh lai duong vo ich -> bo ngay, cam 20 giay. Log 03/09 20:5x: dich cach\n"
    "		// 164 mps, nhan vat dung sat tuong 7 giay (2 lan tinh lai) - chu game: \"chay vao goc\".\n"
    "		int nTamPK = pAp->nPKVision;\n"
    "		if (nTamPK < 100)\n"
    "			nTamPK = 100;\n"
    "		else if (nTamPK > 1200)\n"
    "			nTamPK = 1200;\n"
    "		if (s_nTKSanRepath < 2 && nChonD > nTamPK)\n"
    "		{	// PHAI co RemoveFlag: chi ha uDTPath la vo tac dung vi DT_WalkTo con cua\n"
    "			// thu hai so dich cu (\"tx != nX || ty != nY\") ma dich thi khong doi.\n"
    "			g_ScenePlace.RemoveFlag();\n"
    "			ea.uDTPath = 0;\n"
    "			++s_nTKSanRepath;\n"
    "			s_uTKSanMoveT = uCurTime;\n"
    "		}\n"
    "		else\n"
    "		{\n"
    "			const int nCam = (nChonD <= nTamPK) ? 20 : 60;\n"
    "			Player[nPlayerIdx].m_mAutoExcludeNpcID[uID] = uCurTime + (UINT)nCam * 1000u;\n"
    "			TK_SanBo();\n"
    "			AUTOLOG(\"[TK-SAN-BO] khong toi duoc id=%u (xa=%d tam=%d) - cam %d giay\", uID, nChonD, nTamPK, nCam);\n"
    "			return 0;\n"
    "		}\n"
    "	}\n",
    "DANG TRONG TAM PK ma dung yen 3 giay")

# =====================================================================
# 7. TK_SanNguoi: bi chan thi nham dung o dich (A* di vong); diem nham phai FindPath == 1
# =====================================================================
f.thay(
    "	int ax = x, ay = y;\n"
    "	if (nChonD > nLui)\n"
    "	{\n"
    "		ax = x + (nX - x) * nLui / nChonD;\n"
    "		ay = y + (nY - y) * nLui / nChonD;\n"
    "	}\n",
    "	int ax = x, ay = y;\n"
    "	if (nChonD > nLui)\n"
    "	{\n"
    "		ax = x + (nX - x) * nLui / nChonD;\n"
    "		ay = y + (nY - y) * nLui / nChonD;\n"
    "	}\n"
    "	// (03/09 toi, dot 3) duong thang toi dich bi CHAN (tuong) thi diem lui 3 o hay nam ngay chan\n"
    "	// tuong: toi do la dung yen ma tang DANH khong nhan (TK_ChonDich doi duong nhin). Bi chan thi\n"
    "	// nham DUNG o dich de A* client di vong; toi gan co duong nhin la tang danh nhan ngay.\n"
    "	if (!TK_ThayDuoc(nX, nY, x, y))\n"
    "	{\n"
    "		ax = x;\n"
    "		ay = y;\n"
    "	}\n",
    "nham DUNG o dich de A* client di vong")

f.thay(
    "	if (!s_uTKAimT\n"
    "	 || (g_GetDistance(ax, ay, s_nTKAimX, s_nTKAimY) > TK_O(8)\n"
    "	  && uCurTime - s_uTKAimT >= 3000u))\n"
    "	{\n"
    "		s_nTKAimX = ax;\n"
    "		s_nTKAimY = ay;\n"
    "		s_uTKAimT = uCurTime ? uCurTime : 1;\n"
    "	}\n",
    "	if (!s_uTKAimT\n"
    "	 || (g_GetDistance(ax, ay, s_nTKAimX, s_nTKAimY) > TK_O(8)\n"
    "	  && uCurTime - s_uTKAimT >= 3000u))\n"
    "	{\n"
    "		// (03/09 toi, dot 3) diem nham phai TOI DUOC: KSubWorld::FindPath tra 1 = toi dung dich;\n"
    "		// 2 = khong toi duoc, chi toi o gan nhat roi dung (cung bay voi o rao, dot 2). Diem lui\n"
    "		// khong toi duoc thi thu dung o dich; van khong -> dich nam trong vung kin, loai 20 giay.\n"
    "		// FindPath o day dat san duong; DT_WalkTo sau do thay HaveTarget trung nen khong tinh lai.\n"
    "		int nDuong = SubWorld[0].FindPath(ax, ay);\n"
    "		if (nDuong != 1 && (ax != x || ay != y))\n"
    "		{\n"
    "			ax = x;\n"
    "			ay = y;\n"
    "			nDuong = SubWorld[0].FindPath(ax, ay);\n"
    "		}\n"
    "		if (nDuong != 1)\n"
    "		{\n"
    "			Player[nPlayerIdx].m_mAutoExcludeNpcID[uID] = uCurTime + 20000u;\n"
    "			TK_SanBo();\n"
    "			AUTOLOG(\"[TK-SAN-BO] khong co duong toi id=%u o (%d,%d) (FindPath=%d) - cam 20 giay\", uID, x, y, nDuong);\n"
    "			return 0;\n"
    "		}\n"
    "		s_nTKAimX = ax;\n"
    "		s_nTKAimY = ay;\n"
    "		s_uTKAimT = uCurTime ? uCurTime : 1;\n"
    "	}\n",
    "khong co duong toi id=%u o (%d,%d)")

f.ghi()
