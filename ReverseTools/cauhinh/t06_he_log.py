# -*- coding: utf-8 -*-
"""t06_he_log.py - THI CONG DOT 4: HE LOG - lam SONG 336 diem chan doan dang cam.

HIEN TRANG (da doc tan ma, khong doan):
  Engine\\Src\\KDebug.cpp:44-71  g_DebugLog CHI gui WM_COPYDATA toi cua so
  DebugWin.exe:
        if (m_hWndDebug) { ... SendMessage(WM_COPYDATA) ... }
  Chay dich vu that thi m_hWndDebug = NULL => TOAN BO 336 diem goi g_DebugLog
  trong ma nguon KHONG DE LAI DAU VET NAO. Do la ly do "loi xay ra ma khong
  biet o dau".

  Nhung duong log KHAC thi da chay tot, KHONG dung toi:
   - loi runtime Lua  -> ScriptError.log (_ALERT dang ky o ScriptFuns.cpp:14672,
     co ca stack traceback kem tep:dong - da kiem chung tren log that 29/08)
   - WriteLog(...) cua Lua -> logs\\script_jx2.log (KTongJX2.cpp:3999)
   - KLuaScript::ScriptError -> ScriptError.log

MIENG VA:
 1) g_DebugLog ghi them ra TEP khi bat cong tac. MAC DINH TAT (0) => khong
    doi hanh vi, khong ton dia neu chu khong bat.
 2) Kem theo: doi `char buffer[256]` + `vsprintf` (KHONG gioi han) thanh
    `_vsnprintf` co chan. Day la TRAN BO DEM that: ban ghi log dai hon 256 byte
    la dam ngan xep. Vi du diem goi ScriptFuns.cpp:2483 in ca cau lenh Lua
    (szCall toi 2 KB).
 3) Xoay vong tep khi vuot nguong (mac dinh 64 MB) - doi ten thanh .1 roi mo
    lai, de khong bao gio an het dia.

CONG TAC (gamesetting.ini, nhom moi [Log]):
    GhiTep   = 0            ; 1 = bat ghi g_DebugLog ra tep
    TepLog   = logs\\debug.log
    TranMB   = 64           ; vuot nguong nay thi xoay vong

CAN BUILD LAI CA HAI cau hinh (Engine + Core).
Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

ENG = r"D:\GAMEDEVNEW\Sources\Engine\Src"
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
INI = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
       r"\settings\gamesetting.ini")
HAU_TO = ".truoc_helog"
NHAN = "[HELOG 29/08]"
T = "\t"

THAN_MOI = r'''
//---------------------------------------------------------------------------
// ''' + NHAN + r''' GHI LOG RA TEP
// g_DebugLog von chi gui WM_COPYDATA cho DebugWin.exe, nen khi chay dich vu
// that thi 336 diem goi trong ma nguon khong de lai dau vet nao. Ba bien duoi
// day cho phep ghi ra tep; MAC DINH TAT nen khong doi hanh vi cu.
// KCore.cpp dat gia tri tu gamesetting.ini nhom [Log].
//---------------------------------------------------------------------------
int  g_nGhiLogTep = 0;
char g_szTepLog[260] = "logs\\debug.log";
int  g_nLogTranMB = 64;

static void sGhiMotDongLog(const char* pszNoiDung)
{
	if (!g_nGhiLogTep || !pszNoiDung || !pszNoiDung[0] || !g_szTepLog[0])
		return;

	FILE* pFile = fopen(g_szTepLog, "a");
	if (!pFile)		// o dia day / thu muc chua co / tep bi khoa: bo qua im lang,
		return;		// TUYET DOI khong duoc lam sap may chu chi vi ghi log

	SYSTEMTIME systm;
	::GetLocalTime(&systm);
	fprintf(pFile, "%04d/%02d/%02d %02d:%02d:%02d.%03d\t%s\n",
		systm.wYear, systm.wMonth, systm.wDay,
		systm.wHour, systm.wMinute, systm.wSecond, systm.wMilliseconds,
		pszNoiDung);

	// xoay vong: vuot nguong thi doi ten thanh <tep>.1 roi lan sau mo tep moi
	long nCo = ftell(pFile);
	fclose(pFile);
	if (g_nLogTranMB > 0 && nCo > (long)g_nLogTranMB * 1024 * 1024)
	{
		char szCu[300];
		_snprintf(szCu, sizeof(szCu) - 1, "%s.1", g_szTepLog);
		szCu[sizeof(szCu) - 1] = 0;
		remove(szCu);
		rename(g_szTepLog, szCu);
	}
}
'''


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


VA = []

# ---- KDebug.cpp: them than ham + goi trong g_DebugLog + chan tran ----
VA.append(dict(
    ten="KDebug.cpp - them bo ghi tep + chan tran bo dem",
    tep=os.path.join(ENG, "KDebug.cpp"),
    cu=[
        "void g_DebugLog(LPSTR Fmt, ...)",
        "{",
        "#ifndef __linux",
        T + "if (m_hWndDebug)",
        T + "{",
        T*2 + "char buffer[256];",
        T*2 + "va_list va;",
        "",
        T*2 + "COPYDATASTRUCT data;",
        T*2 + "va_start(va, Fmt);",
        T*2 + "int n = vsprintf(buffer, Fmt, va);",
        T*2 + "va_end(va);",
        T*2 + "data.dwData = 1;",
        T*2 + "data.cbData = n + 1;",
        T*2 + "data.lpData = buffer;",
        T*2 + "SendMessage(m_hWndDebug, WM_COPYDATA,",
        T*3 + "(WPARAM)m_hWndDebug, (LPARAM)&data);",
        T + "}",
        "#else",
    ],
    moi=[
        "void g_DebugLog(LPSTR Fmt, ...)",
        "{",
        "#ifndef __linux",
        T + "// " + NHAN + " dung 1 bo dem chung cho ca hai duong ra (cua so",
        T + "// DebugWin va tep log). Dem 2048 byte + _vsnprintf CO CHAN thay cho",
        T + "// buffer[256] + vsprintf khong chan truoc day: co diem goi in ca cau",
        T + "// lenh Lua dai toi 2 KB (ScriptFuns.cpp:2483) - do la tran ngan xep.",
        T + "if (!m_hWndDebug && !g_nGhiLogTep)",
        T*2 + "return;",
        "",
        T + "char buffer[2048];",
        T + "va_list va;",
        T + "va_start(va, Fmt);",
        T + "int n = _vsnprintf(buffer, sizeof(buffer) - 1, Fmt, va);",
        T + "va_end(va);",
        T + "if (n < 0)\t\t\t\t// _vsnprintf cat bot: tu dong ket chuoi",
        T*2 + "n = (int)sizeof(buffer) - 1;",
        T + "buffer[n] = 0;",
        "",
        T + "if (m_hWndDebug)",
        T + "{",
        T*2 + "COPYDATASTRUCT data;",
        T*2 + "data.dwData = 1;",
        T*2 + "data.cbData = n + 1;",
        T*2 + "data.lpData = buffer;",
        T*2 + "SendMessage(m_hWndDebug, WM_COPYDATA,",
        T*3 + "(WPARAM)m_hWndDebug, (LPARAM)&data);",
        T + "}",
        T + "sGhiMotDongLog(buffer);",
        "#else",
    ],
))

# ---- KDebug.cpp: chen than ham TRUOC g_DebugLog ----
VA.append(dict(
    ten="KDebug.cpp - chen bo ghi tep",
    tep=os.path.join(ENG, "KDebug.cpp"),
    cu=[
        "HWND g_FindDebugWindow(char* lpClassName, char* lpWindowName)",
        "{",
    ],
    moi=["#include <stdio.h>\t// " + NHAN + " fopen/fprintf/rename cho bo ghi tep"]
        + THAN_MOI.strip("\n").split("\n") + [
        "",
        "HWND g_FindDebugWindow(char* lpClassName, char* lpWindowName)",
        "{",
    ],
))

# ---- KDebug.h: extern ----
VA.append(dict(
    ten="KDebug.h - extern 3 bien",
    tep=os.path.join(ENG, "KDebug.h"),
    cu=["ENGINE_API void g_DebugLog(LPSTR Fmt, ...);"],
    moi=[
        "ENGINE_API void g_DebugLog(LPSTR Fmt, ...);",
        "// " + NHAN + " cong tac ghi g_DebugLog ra tep - KCore.cpp dat tu",
        "// gamesetting.ini nhom [Log]. Mac dinh 0 = tat (y het truoc day).",
        "ENGINE_API extern int  g_nGhiLogTep;",
        "ENGINE_API extern char g_szTepLog[260];",
        "ENGINE_API extern int  g_nLogTranMB;",
    ],
))

# ---- KCore.cpp: doc ini nhom [Log] ----
VA.append(dict(
    ten="KCore.cpp - doc nhom [Log]",
    tep=os.path.join(CORE, "KCore.cpp"),
    cu=[
        T*2 + 'g_GameSetting.GetInteger("ServerConfig", "Skill120Rate", 0,'
              ' &g_Skill120ExpRate);\t\t\t//#trong file config khong duoc duoi 100',
    ],
    moi=[
        T*2 + 'g_GameSetting.GetInteger("ServerConfig", "Skill120Rate", 0,'
              ' &g_Skill120ExpRate);\t\t\t//#trong file config khong duoc duoi 100',
        "",
        T*2 + "// " + NHAN + " nhom [Log]: bat ghi g_DebugLog ra tep.",
        T*2 + "// Mac dinh 0 = tat, y het hanh vi truoc day.",
        T*2 + 'g_GameSetting.GetInteger("Log", "GhiTep", 0, &g_nGhiLogTep);',
        T*2 + 'g_GameSetting.GetInteger("Log", "TranMB", 64, &g_nLogTranMB);',
        T*2 + 'g_GameSetting.GetString("Log", "TepLog", "logs\\\\debug.log",'
              " g_szTepLog, sizeof(g_szTepLog));",
    ],
))


def ap_ini(ghi):
    if not os.path.isfile(INI):
        print("  (bo qua ini: khong thay tep)")
        return
    raw = doc(INI)
    if "[Log]" in raw:
        print("  gamesetting.ini: DA CO nhom [Log] - bo qua")
        return
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    them = [
        "",
        "; " + NHAN + " Ghi log chan doan ra tep. g_DebugLog co 336 diem goi trong",
        "; ma nguon nhung truoc day chi gui cho DebugWin.exe nen chay dich vu la",
        "; mat sach. Bat GhiTep=1 khi can truy loi, tat lai khi xong.",
        "[Log]",
        "GhiTep        = 0     ; 1 = ghi ra tep",
        "TepLog        = logs\\debug.log",
        "TranMB        = 64    ; vuot nguong thi doi ten thanh .1 va mo tep moi",
    ]
    nd = raw.rstrip() + eol + eol.join(them) + eol
    if not ghi:
        print("  gamesetting.ini: se them nhom [Log]")
        return
    sao = INI + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(INI, sao)
    with io.open(INI, "wb") as f:
        f.write(nd.encode("latin-1"))
    print("  DA GHI gamesetting.ini (+nhom [Log])")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t06_he_log - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    theo_tep = {}
    for v in VA:
        theo_tep.setdefault(v["tep"], []).append(v)

    ketqua = []
    for tep, ds in theo_tep.items():
        if not os.path.isfile(tep):
            print("!!! LOI TO: thieu %s" % tep)
            return 1
        raw = doc(tep)
        ten = os.path.basename(tep)
        if NHAN in raw:
            print("  %-12s DA VA - bo qua" % ten)
            continue
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
        nd = raw
        hi0 = sum(1 for c in raw if ord(c) > 127)
        for v in ds:
            kcu = eol.join(v["cu"])
            n = nd.count(kcu)
            if n != 1:
                print("!!! LOI TO: %s - moc khop %d lan (can 1)" % (v["ten"], n))
                print("    moc:\n%s" % kcu.replace("\t", "->"))
                return 1
            nd = nd.replace(kcu, eol.join(v["moi"]))
            print("  %-12s %s" % (ten, v["ten"]))
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi (%s)" % ten)
            return 1
        them = "".join(eol.join(v["moi"]) for v in ds)
        bot = "".join(eol.join(v["cu"]) for v in ds)
        for cap in ("{}", "()"):
            if (them.count(cap[0]) - them.count(cap[1])) != \
               (bot.count(cap[0]) - bot.count(cap[1])):
                print("!!! LOI TO: ngoac %s lech (%s)" % (cap, ten))
                return 1
        ketqua.append((tep, nd, ten))

    ap_ini(False)
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for tep, nd, ten in ketqua:
        sao = tep + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(tep, sao)
        with io.open(tep, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(tep) != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % tep)
            return 1
        print("  DA GHI %s" % ten)
    ap_ini(True)
    print("\nBUILD LAI: Engine + Core, CA HAI cau hinh.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
