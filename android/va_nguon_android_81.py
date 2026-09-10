# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 WAUTO B0] Buoc B0 cua LOTRINH_WAUTO_MOBILE_1109.md: DUONG ONG WAuto TRONG TIEN TRINH (chua co giao dien).
#
# Bo nao WAuto (180 ham DT_/TK_/LD_/CT_/ST_/HD_ trong CoreShell.cpp + ExtAutoLoop trong S3Client.cpp) DA nam trong
# libCoreClient.so / libmain.so. Thu thieu la BEN GUI: WAuto.exe (PC) cu 54 ms nap goi IPCGameLoop vao vung nho chia se
# + SetEvent; tren Android vung nho la calloc trong tien trinh, event la pthread cond (KPosixWin32.cpp) va ProcIpcCommand
# van duoc goi moi khung. Tep moi Platform/JxWAutoNoiBo.cpp lam dung viec cua AppLoop trong WAuto.exe, ben nhan khong doi.
#
#  1. TAO  Sources/S3Client/Platform/JxWAutoNoiBo.h + .cpp (chi JX_ANDROID; chuoi TCVN3 sinh bang vn_edit.vn()).
#  2. VA   Sources/S3Client/S3Client.cpp: include + goi JxWAuto_NhipVongLap() ngay truoc ProcIpcCommand() (rao JX_ANDROID).
#  3. VA   Sources/Core/Src/CoreShell.cpp: them JxCore_WAutoNhanVat() (id + mau/noi luc toi da) trong khoi #ifdef JX_ANDROID
#          da co (canh JxCore_GotoHuong). Ban Windows KHONG bien dich (khoi rao).
#  4. VA   android/CMakeLists.txt: them JxWAutoNoiBo.cpp vao target main (KHONG dua vao vcxproj -> khong cau hinh Windows
#          nao biet tep nay; day la cach da dung cho JxPerfHudAndroid.cpp o va 77).
#  5. VA   android/du_lieu_ghi_de/config.ini: muc [WAuto] Bat=0 (cong tac tam cho B0).
#
# LUAT (LOTRINH_WAUTO_MOBILE_1109.md §0): ban PC khong doi mot hanh vi nao - moi dong them vao tep dung chung deu trong
# #ifdef JX_ANDROID; khong xoa gi cua WAuto; nguon TCVN3 doc/ghi latin-1, giu CRLF, so byte cao truoc/sau bang nhau.
# Chay lai vo hai.

import io
import os
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402  (tieng Viet co dau -> byte TCVN3 dang chuoi latin-1)

DAU = "[ANDROID 11/09 WAUTO B0]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


def tao(p, dong, dau):
    """Tao tep moi (CRLF, latin-1). Co san va da mang dau thi bo qua."""
    if os.path.isfile(p) and dau in doc(p):
        print("da co roi, bo qua:", p)
        return
    ghi(p, "\r\n".join(dong) + "\r\n")
    print("da tao:", p, "(%d dong)" % len(dong))


# ---------------------------------------------------------------- 1a. JxWAutoNoiBo.h
H = [
    "//---------------------------------------------------------------------------",
    "// %s BANG WAuto TRONG GAME - buoc B0: DUONG ONG TRONG TIEN TRINH (chua co giao dien)." % DAU,
    "//",
    "// WAuto.exe (ban PC) la mot tien trinh rieng: no dien struct autoData roi cu 54 ms nap mot goi IPCGameLoop vao",
    "// vung nho chia se cua game + bao tin hieu; game (S3Client.cpp ProcIpcCommand) doc goi va goi ExtAutoLoop -> bo",
    "// nao auto trong CoreShell.cpp (DT_/TK_/LD_/CT_/ST_/HD_). Tren Android khong co tien trinh ngoai, nhung vung nho",
    "// chia se DA la calloc trong tien trinh va tin hieu la pthread cond (KPosixWin32.cpp), ProcIpcCommand van duoc",
    "// goi moi khung hinh. Tep nay la BEN GUI nam ngay trong game: cung goi, cung duong, ben nhan khong doi mot dong.",
    "//",
    "// Cau hinh: APdata\\<ma nhan vat>.dat - CUNG dinh dang va ten voi WAuto.exe (ghi nguyen struct autoData), nen tep",
    "// APdata cua ban PC chep sang dien thoai la dung duoc. Chua co tep -> mac dinh LAN DAU giong WAuto.exe",
    "// (LoadRoleData: danh, tam 1000, nhat do, uong thuoc theo 2/3 va 1/3 mau...) roi ghi ra tep ngay.",
    "// Cong tac: config.ini [WAuto] Bat=1 (tam cho B0; tu B1 co nut BAT/TAT trong game).",
    "// Lo trinh: LOTRINH_WAUTO_MOBILE_1109.md. Chi bien dich khi JX_ANDROID; khong nam trong vcxproj nao.",
    "//---------------------------------------------------------------------------",
    "#ifndef JxWAutoNoiBo_H",
    "#define JxWAutoNoiBo_H",
    "#ifdef JX_ANDROID",
    "#include \"../../Core/Src/ipc_shared.h\"",
    "",
    "// Goi moi vong lap game (KMyApp::GameLoop), NGAY TRUOC ProcIpcCommand() de goi vua nap duoc tieu thu cung khung.",
    "void\t\t\tJxWAuto_NhipVongLap();",
    "// Bat / tat may auto. Tra ve trang thai moi. Doi trang thai -> gui PRT_TICKSTART (ATYPE_CLEAR) nhu WAuto.exe.",
    "int\t\t\t\tJxWAuto_Bat(int bBat);",
    "int\t\t\t\tJxWAuto_DangBat();",
    "// Cau hinh dang dung cua nhan vat hien tai (B1+ doc/ghi thang vao day roi goi JxWAuto_LuuCauHinh).",
    "autoData*\t\tJxWAuto_CauHinh();",
    "int\t\t\t\tJxWAuto_LuuCauHinh();\t\t// ghi APdata\\<id>.dat; 1 = xong",
    "int\t\t\t\tJxWAuto_NapCauHinh();\t\t// nap lai tu tep (tu goi khi doi nhan vat); 1 = co tep, 0 = mac dinh lan dau",
    "unsigned int\tJxWAuto_IdNhanVat();\t\t// ma nhan vat cua cau hinh dang nap (0 = chua vao game)",
    "",
    "#endif // JX_ANDROID",
    "#endif",
]

# ---------------------------------------------------------------- 1b. JxWAutoNoiBo.cpp
# Mac dinh LAN DAU chep nguyen tu WAuto.exe LoadRoleData (nhanh "chua co du lieu gi -> tao du lieu mac dinh"),
# WAutoUI/WAuto.cpp (UTF-16) dong ~2573-2721. Hai o mau/noi luc phu thuoc lifemax/manamax cua nhan vat.
MAC_DINH = [
    ("nIlifeCell1", "nLifeMax / 3 * 2"), ("nIlifeCell2", "nLifeMax / 3"), ("nIlifeCell3", "1000"),
    ("nImanaCell1", "nManaMax / 3 * 2"), ("nImanaCell2", "nManaMax / 3"), ("nImanaCell3", "1000"),
    ("bCheckiLife", "1"), ("bCheckiMana", "1"), ("bFight", "1"), ("nVision", "1000"), ("nNearDist", "75"),
    ("nFBVision", "600"), ("nSLSPerc", "50"), ("nSMSPerc", "50"), ("nSkillCSec", "1"), ("uFKey", "0x20 /* VK_SPACE */"),
    ("nPKVision", "800"), ("nPKNearDist", "75"), ("bPKFollowTG", "1"), ("bPKPlayer", "1"), ("bPKNpc", "1"),
    ("bPickUp", "1"), ("bFollowPick", "1"), ("nPickVision", "400"),
    ("nLeavePtMem", "5"), ("nLeavePtMin", "10"), ("nRemovePtMin", "10"),
    ("bRepair", "1"), ("bFRepair", "1"), ("nWDMoney", "20"), ("nBLNum", "15"), ("nBMNum", "10"), ("nBPNum", "5"),
    ("nBTPNum", "1"), ("nBuyLifeSel", "9"), ("nBuyManaSel", "4"), ("nBuyPoisSel", "4"), ("bSaveItem", "1"),
    ("nFollowDist", "100"),
    ("bDTType[0]", "1"), ("bDTType[1]", "1"), ("bDTType[2]", "1"), ("bDTType[3]", "1"), ("bDTType[4]", "1"), ("bDTType[5]", "1"),
    ("bDTUseBox", "1"), ("nDTReward1", "0"), ("nDTReward2", "2"), ("nDTWDMoney", "50"), ("bDTLenhBai", "1"),
    ("bDTMuaSap", "1"), ("nDTMaxMua", "200"),
    ("bTongKim", "0"), ("bTKGio[0]", "1"), ("bTKGio[1]", "1"), ("bTKGio[2]", "1"), ("bTKGio[3]", "1"),
    ("nTKSom", "2"), ("nTKLech", "0"), ("nTKPhe", "2"), ("nTKMuaMau", "0"), ("nTKSoBinh", "20"), ("bTKThuoc", "1"),
    ("nTKThuocSel", "0"), ("nTKUuTien", "1"), ("bTKVeCho", "1"), ("nTKVeThanh", "0"),
    ("bTKRuong", "0"), ("nTKRuongHuong", "5"), ("bTKRuongCat", "1"), ("bPKBoQuaKhien", "1"),
    ("bTimAcChinh", "0"), ("nAcChinhKC", "200"), ("bAcChinhThanh", "0"), ("bCungMucTieu", "0"), ("bAcChinhVaoMap", "1"),
    ("szAcChinhTen[0]", "0"),
    ("bCongThanh", "0"), ("nCTGio", "20"), ("nCTPhut", "0"), ("nCTLech", "0"), ("nCTSom", "3"), ("nCTCua", "95"),
    ("nCTThanh", "7"), ("nCTPhe", "0"), ("nCTCong", "0"), ("nCTThu", "0"), ("nCTVe", "7"), ("bCTLoa", "1"),
    ("bCTMua", "1"), ("nCTSoBinh", "10"), ("nCTCapBinh", "5"),
    ("bWANpcTheSame", "0"), ("bWAMissle", "0"), ("nWAMissleIndex", "1"),
    ("bLienDau", "0"), ("bLDKhung[0]", "1"), ("bLDKhung[1]", "1"), ("nLDGio[0]", "18"), ("nLDPhut[0]", "0"),
    ("nLDGio[1]", "20"), ("nLDPhut[1]", "0"), ("nLDLuot", "4"), ("nLDPhutLuot", "15"), ("nLDSom", "5"), ("nLDLech", "0"),
    ("bLDTuLap", "1"), ("szLDTen[0]", "0"), ("bLDCatDoCam", "1"), ("bLDNhanThuong", "1"), ("nLDVeThanh", "7"),
    ("bLDPhuVe", "0"), ("bLDParty", "0"), ("nLDThanhBD", "7"), ("nLDPtCount", "0"),
    ("bHDBachNhan", "0"), ("nHDBNGio", "12"), ("nHDBNPhut", "5"), ("nHDBNCay", "0"), ("nHDBNMode", "0"), ("nHDBNDai", "0"),
    ("bHDBNBuff", "1"), ("bHDBangChien", "0"), ("nHDBCGio", "20"), ("nHDBCPhut", "25"), ("nHDLech", "0"),
    ("bHDTinSu", "0"), ("nHDTSGio", "9"), ("nHDTSPhut", "30"), ("nHDTSLuot", "2"),
    ("bCombo", "1"), ("bComboNoUT", "1"), ("bTienChieu", "0"), ("nTCKieu", "0"), ("nTCMs", "8000"), ("nTCDist", "120"),
    ("nTCHoi", "6"),
]

CPP = [
    "//---------------------------------------------------------------------------",
    "// %s Ben gui WAuto trong tien trinh - xem JxWAutoNoiBo.h." % DAU,
    "// Chu Viet trong chuoi: TCVN3 (sinh bang vn_edit.py trong android/va_nguon_android_81.py), khong dung Edit/Write thuong.",
    "//---------------------------------------------------------------------------",
    "#include \"KWin32.h\"",
    "#ifdef JX_ANDROID",
    "#include \"JxWAutoNoiBo.h\"",
    "#include \"KFile.h\"",
    "#include \"KFilePath.h\"\t\t// g_CreatePath",
    "#include \"KDebug.h\"\t\t\t// g_DebugLog",
    "#include \"../Ui/UiCase/UiMsgCentrePad.h\"",
    "#include <stdio.h>",
    "#include <string.h>",
    "",
    "class iCoreShell;",
    "extern iCoreShell*\t\tg_pCoreShell;",
    "extern SharedState*\t\tg_pState;\t\t// S3Client.cpp: hop thu lenh cua WAuto (Android: calloc trong tien trinh)",
    "extern HANDLE\t\t\tg_hEventRecv;\t// S3Client.cpp: tin hieu \"co lenh moi\" (Android: pthread cond, auto-reset)",
    "extern \"C\" int JxCore_WAutoNhanVat(unsigned int* puId, int* pnLifeMax, int* pnManaMax);\t// CoreShell.cpp (chi Android)",
    "",
    "#define WA_NHIP_MS\t\t54\t\t\t\t// = GAMELOOPINTV cua WAuto.exe",
    "#define WA_THU_MUC\t\t\"\\\\APdata\"\t\t// cung thu muc voi WAuto.exe ben PC",
    "",
    "static autoData\t\ts_CauHinh;\t\t\t// constructor = mac dinh cua struct (nhu WAuto.exe)",
    "static int\t\t\ts_nBat = 0;",
    "static int\t\t\ts_nDaDocCfg = 0;",
    "static UINT\t\t\ts_uNhipKe = 0;",
    "static UINT\t\t\ts_uBaoKe = 0;",
    "static unsigned int\ts_uId = 0;\t\t\t// ma nhan vat cua cau hinh dang nap (0 = chua vao game)",
    "static int\t\t\ts_nTickDaBao = -1;\t// trang thai da bao cho bo nao (PRT_TICKSTART): -1 chua, 0 tat, 1 bat",
    "",
    "static void WA_DocConfig()",
    "{",
    "\tchar szCfg[MAX_PATH] = { 0 };",
    "\tGetCurrentDirectory(MAX_PATH, szCfg);",
    "\tstrcat(szCfg, \"\\\\Config.ini\");",
    "\ts_nBat = GetPrivateProfileInt(\"WAuto\", \"Bat\", 0, szCfg) ? 1 : 0;",
    "\tg_DebugLog(\"[WAUTO] config.ini [WAuto] Bat=%d; sizeof(autoData)=%u\", s_nBat, (unsigned)sizeof(autoData));",
    "}",
    "",
    "static void WA_TenTep(char* sz, int nMax, unsigned int uId)",
    "{",
    "\tsnprintf(sz, nMax, WA_THU_MUC \"\\\\%u.dat\", uId);",
    "}",
    "",
    "// Mac dinh LAN DAU y het WAuto.exe (LoadRoleData khi chua co tep .dat): danh + nhat + uong thuoc theo 2/3 va 1/3 mau.",
    "// Cung dung lam nen cho tep CU ngan hon struct: phan duoi tep khong phu toi giu gia tri nay (WAuto.exe lam y vay o",
    "// cac nhanh nang cap theo kich thuoc tep).",
    "static void WA_MacDinhLanDau(autoData& ap, int nLifeMax, int nManaMax)",
    "{",
] + ["\tap.%s = %s;" % (t, g) for t, g in MAC_DINH] + [
    "}",
    "",
    "int JxWAuto_NapCauHinh()",
    "{",
    "\tunsigned int uId = 0;",
    "\tint nLifeMax = 0, nManaMax = 0;",
    "\tautoData mac;",
    "\ts_CauHinh = mac;\t\t\t\t\t\t// ve mac dinh cua struct truoc",
    "\ts_nTickDaBao = -1;",
    "\tif (!JxCore_WAutoNhanVat(&uId, &nLifeMax, &nManaMax) || !uId)",
    "\t{",
    "\t\ts_uId = 0;",
    "\t\treturn 0;",
    "\t}",
    "\ts_uId = uId;",
    "\tWA_MacDinhLanDau(s_CauHinh, nLifeMax, nManaMax);",
    "\tchar sz[128];",
    "\tWA_TenTep(sz, sizeof(sz), uId);",
    "\tKFile f;",
    "\tif (!f.Open(sz))",
    "\t{",
    "\t\tg_DebugLog(\"[WAUTO] chua co %s -> mac dinh lan dau (danh + nhat + uong thuoc), mau %d / noi luc %d; ghi ra tep\", sz, nLifeMax, nManaMax);",
    "\t\tJxWAuto_LuuCauHinh();",
    "\t\treturn 0;",
    "\t}",
    "\tDWORD dwCo = f.Size();",
    "\tif (dwCo > sizeof(autoData))",
    "\t\tdwCo = (DWORD)sizeof(autoData);",
    "\tDWORD dwDoc = f.Read(&s_CauHinh, dwCo);\t\t// tep cu ngan hon struct: phan duoi giu mac dinh lan dau",
    "\tf.Close();",
    "\tg_DebugLog(\"[WAUTO] nap %s: %u/%u byte; fight=%d vis=%d pick=%d life=%d TK=%d DT=%d\", sz, (unsigned)dwDoc, (unsigned)sizeof(autoData),",
    "\t\ts_CauHinh.bFight, s_CauHinh.nVision, s_CauHinh.bPickUp, s_CauHinh.bCheckiLife, s_CauHinh.bTongKim, s_CauHinh.bDaTau);",
    "\treturn 1;",
    "}",
    "",
    "int JxWAuto_LuuCauHinh()",
    "{",
    "\tif (!s_uId)",
    "\t\treturn 0;",
    "\tchar szThuMuc[32];",
    "\tstrcpy(szThuMuc, WA_THU_MUC);",
    "\tg_CreatePath(szThuMuc);",
    "\tchar sz[128];",
    "\tWA_TenTep(sz, sizeof(sz), s_uId);",
    "\tKFile f;",
    "\tif (!f.Create(sz))",
    "\t{",
    "\t\tg_DebugLog(\"[WAUTO] KHONG ghi duoc %s\", sz);",
    "\t\treturn 0;",
    "\t}",
    "\tDWORD dwGhi = f.Write(&s_CauHinh, sizeof(autoData));",
    "\tf.Close();",
    "\tg_DebugLog(\"[WAUTO] ghi %s: %u byte\", sz, (unsigned)dwGhi);",
    "\treturn dwGhi == sizeof(autoData) ? 1 : 0;",
    "}",
    "",
    "autoData* JxWAuto_CauHinh()\t\t\t{ return &s_CauHinh; }",
    "unsigned int JxWAuto_IdNhanVat()\t{ return s_uId; }",
    "int JxWAuto_DangBat()\t\t\t\t{ return s_nBat; }",
    "int JxWAuto_Bat(int bBat)\t\t\t{ s_nBat = bBat ? 1 : 0; return s_nBat; }",
    "",
    "// Nap goi vao hop thu cua ProcIpcCommand y nhu AppLoop cua WAuto.exe: [so goi][goi 1][goi 2]... roi SetEvent.",
    "static void WA_GuiGoi(const void* p, unsigned int n, unsigned int uSo)",
    "{",
    "\tif (!g_pState || !g_hEventRecv || sizeof(UINT) + n > SHARED_SIZE)",
    "\t\treturn;",
    "\t*(UINT*)g_pState = uSo;",
    "\tmemcpy((BYTE*)g_pState + sizeof(UINT), p, n);",
    "\tSetEvent(g_hEventRecv);",
    "}",
    "",
    "// Bao bo nao bat/tat (PRT_TICKSTART -> ATYPE_CLEAR: xoa trang thai auto; tat thi dung nhan vat lai) + mot dong o khung chat.",
    "static void WA_BaoTick(int bBat)",
    "{",
    "\tIPCHideGame s;",
    "\ts.CmdID = PRT_TICKSTART;",
    "\ts.Size = sizeof(IPCHideGame);",
    "\ts.bHide = bBat;",
    "\tWA_GuiGoi(&s, sizeof(s), 1);",
    "\tconst char* sz = bBat ? \"" + vn("Bật auto trong game (WAuto)") + "\" : \"" + vn("Tắt auto trong game (WAuto)") + "\";",
    "\tKUiMsgCentrePad::SystemMessageArrival(sz, (unsigned short)strlen(sz));",
    "\tg_DebugLog(\"[WAUTO] %s (PRT_TICKSTART -> ATYPE_CLEAR)\", bBat ? \"BAT\" : \"TAT\");",
    "}",
    "",
    "void JxWAuto_NhipVongLap()",
    "{",
    "\tif (!s_nDaDocCfg)",
    "\t{",
    "\t\ts_nDaDocCfg = 1;",
    "\t\tWA_DocConfig();",
    "\t}",
    "\tif (!g_pCoreShell || !g_pState)",
    "\t\treturn;",
    "\tunsigned int uId = 0;",
    "\tint nL = 0, nM = 0;",
    "\tif (!JxCore_WAutoNhanVat(&uId, &nL, &nM) || !uId)",
    "\t{",
    "\t\tif (s_uId)",
    "\t\t{",
    "\t\t\ts_uId = 0;\t\t\t// vua roi map / dang xuat: lan vao lai se nap lai cau hinh",
    "\t\t\ts_nTickDaBao = -1;",
    "\t\t}",
    "\t\treturn;",
    "\t}",
    "\tif (uId != s_uId)",
    "\t\tJxWAuto_NapCauHinh();",
    "\tif (s_nTickDaBao != s_nBat)",
    "\t{",
    "\t\ts_nTickDaBao = s_nBat;",
    "\t\tWA_BaoTick(s_nBat);",
    "\t\treturn;\t\t\t\t\t// khung nay hop thu da co goi; goi vong lap gui tu khung sau",
    "\t}",
    "\tif (!s_nBat)",
    "\t\treturn;",
    "\tUINT uNow = timeGetTime();",
    "\tif (uNow < s_uNhipKe)",
    "\t\treturn;",
    "\ts_uNhipKe = uNow + WA_NHIP_MS;",
    "\tstatic BYTE s_Goi[sizeof(IPCGameLoop) + sizeof(IPCHienThi)];",
    "\tIPCGameLoop* pGL = (IPCGameLoop*)s_Goi;",
    "\tpGL->CmdID = PRT_GAMELOOP;",
    "\tpGL->Size = sizeof(IPCGameLoop);",
    "\tmemcpy(&pGL->setting, &s_CauHinh, sizeof(autoData));",
    "\t// Mot may = mot nhan vat: khong co ac chinh / ac phu (the Ac chinh khong dung tren mobile - ma va truong van giu nguyen).",
    "\tpGL->setting.nACLaChinh = 0;",
    "\tpGL->setting.szAcChinhTen[0] = 0;",
    "\tpGL->setting.bTimAcChinh = 0;",
    "\tpGL->setting.bCungMucTieu = 0;",
    "\tpGL->setting.bAcChinhVaoMap = 0;",
    "\tpGL->setting.uACTuoi = 0;",
    "\tIPCHienThi* pHT = (IPCHienThi*)(s_Goi + sizeof(IPCGameLoop));",
    "\tpHT->CmdID = PRT_HIENTHI;",
    "\tpHT->Size = sizeof(IPCHienThi);",
    "\tpHT->bNpcTheSame = s_CauHinh.bWANpcTheSame;",
    "\tpHT->nMissleIndex = s_CauHinh.bWAMissle ? s_CauHinh.nWAMissleIndex : 0;",
    "\tWA_GuiGoi(s_Goi, sizeof(s_Goi), 2);",
    "\tif (uNow >= s_uBaoKe)",
    "\t{",
    "\t\ts_uBaoKe = uNow + 5000;",
    "\t\tg_DebugLog(\"[WAUTO] nhip: id=%u fight=%d vis=%d pick=%d life=%d/%d TK=%d DT=%d\", s_uId, s_CauHinh.bFight, s_CauHinh.nVision,",
    "\t\t\ts_CauHinh.bPickUp, s_CauHinh.bCheckiLife, s_CauHinh.nIlifeCell1, s_CauHinh.bTongKim, s_CauHinh.bDaTau);",
    "\t}",
    "}",
    "#endif // JX_ANDROID",
]


# ---------------------------------------------------------------- 2. S3Client.cpp
def va_s3client(s):
    s = thay(s, ["#include \"Ui/UiCase/UiInformation.h\"",
                 "#include \"Ui/UiCase/UiSelServer.h\""],
                ["#include \"Ui/UiCase/UiInformation.h\"",
                 "#ifdef JX_ANDROID",
                 "#include \"Platform/JxWAutoNoiBo.h\"\t// %s ben gui WAuto trong tien trinh (thay WAuto.exe)" % DAU,
                 "#endif",
                 "#include \"Ui/UiCase/UiSelServer.h\""], "S3Client.cpp: include")
    s = thay(s, ["\tif(g_DrawVisionTime < timeGetTime())",
                 "\t\tg_DrawVision = 0;",
                 "\tProcIpcCommand();"],
                ["\tif(g_DrawVisionTime < timeGetTime())",
                 "\t\tg_DrawVision = 0;",
                 "#ifdef JX_ANDROID",
                 "\tJxWAuto_NhipVongLap();\t// %s bang WAuto trong game: nap goi PRT_GAMELOOP vao g_pState nhu WAuto.exe; ProcIpcCommand ngay duoi tieu thu cung khung" % DAU,
                 "#endif",
                 "\tProcIpcCommand();"], "S3Client.cpp: moc vong lap")
    return s


# ---------------------------------------------------------------- 3. CoreShell.cpp
def va_coreshell(s):
    return thay(s, ["\tPlayer[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;",
                    "\treturn 1;",
                    "}",
                    "#endif\t// JX_ANDROID"],
                   ["\tPlayer[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;",
                    "\treturn 1;",
                    "}",
                    "",
                    "// %s Nhan vat dang choi cho bang WAuto trong game (S3Client/Platform/JxWAutoNoiBo.cpp): ma so 32-bit" % DAU,
                    "// (Player.m_dwID - dung truong ma IPCMainSync.dwPID gui cho WAuto.exe, nen ten tep APdata\\<id>.dat trung voi ban PC)",
                    "// va mau / noi luc toi da (WAuto.exe lay lifemax/manamax dat nguong uong thuoc lan dau). Tra 1 khi da vao game. Chi Android.",
                    "extern \"C\" int JxCore_WAutoNhanVat(unsigned int* puId, int* pnLifeMax, int* pnManaMax)",
                    "{",
                    "\tint nIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;",
                    "\tif (nIdx <= 0)",
                    "\t\treturn 0;",
                    "\tif (puId)",
                    "\t\t*puId = (unsigned int)Player[CLIENT_PLAYER_INDEX].m_dwID;",
                    "\tif (pnLifeMax)",
                    "\t\t*pnLifeMax = Npc[nIdx].m_CurrentLifeMax;",
                    "\tif (pnManaMax)",
                    "\t\t*pnManaMax = Npc[nIdx].m_CurrentManaMax;",
                    "\treturn 1;",
                    "}",
                    "#endif\t// JX_ANDROID"], "CoreShell.cpp: JxCore_WAutoNhanVat")


# ---------------------------------------------------------------- 4. android/CMakeLists.txt
def va_cmake(s):
    return thay(s, ["    ${JX_SRC}/S3Client/Platform/JxPerfHudAndroid.cpp)  # [ANDROID 11/09 HUD] bang do FPS / CPU / RAM / pin / GPU"],
                   ["    ${JX_SRC}/S3Client/Platform/JxPerfHudAndroid.cpp   # [ANDROID 11/09 HUD] bang do FPS / CPU / RAM / pin / GPU",
                    "    ${JX_SRC}/S3Client/Platform/JxWAutoNoiBo.cpp)      # %s ben gui WAuto trong tien trinh (LOTRINH_WAUTO_MOBILE_1109.md)" % DAU],
                   "CMakeLists.txt: target main")


# ---------------------------------------------------------------- 5. config.ini lop ghi de
def va_config(s):
    nl = nl_cua(s)
    if not s.endswith(nl):
        s += nl
    return s + nl.join([
        "",
        "[WAuto]",
        "; %s Bang WAuto TRONG GAME (bo nao WAuto da nam san trong libCoreClient.so; xem LOTRINH_WAUTO_MOBILE_1109.md)." % DAU,
        "; 1 = bat may auto ngay khi vao game. Cau hinh doc tu APdata\\<ma nhan vat>.dat - CUNG dinh dang va ten tep voi WAuto.exe",
        ";     ben PC, chep tep APdata cua PC sang la dung duoc; chua co tep thi dung mac dinh lan dau cua WAuto (danh + nhat +",
        ";     uong thuoc) va tu ghi ra tep. Cong tac tam cho B0; tu B1 co nut BAT / TAT trong game. 0 = tat.",
        "Bat=0",
        "",
    ])


tao("Sources/S3Client/Platform/JxWAutoNoiBo.h", H, DAU)
tao("Sources/S3Client/Platform/JxWAutoNoiBo.cpp", CPP, DAU)
va("Sources/S3Client/S3Client.cpp", DAU, va_s3client)
va("Sources/Core/Src/CoreShell.cpp", DAU, va_coreshell)
va("android/CMakeLists.txt", DAU, va_cmake)
va("android/du_lieu_ghi_de/config.ini", DAU, va_config)
print("xong")
