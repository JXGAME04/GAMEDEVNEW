# -*- coding: utf-8 -*-
"""t06b_log_chi_core.py - SUA HUONG he log: HOAN TAC phan Engine, lam trong Core.

VI SAO PHAI DOI HUONG (do that, khong doan):
  t06 sua Engine\\Src\\KDebug.cpp de g_DebugLog ghi ra tep. Nhung:
   - Core.vcxproj (Server Release|x64) KHONG bien dich KDebug.cpp; no LIEN KET
     `D:\\GAMEDEVNEW\\lib\\release\\engine.lib` dung san tu 21/08 15:48.
   - Muon sua co hieu luc thi phai build lai Engine.vcxproj (cau hinh
     "Engine Server Release|x64"), MA ENGINE DANG KHONG BUILD DUOC:
       KWin32App.cpp(18): Cannot open include file: 'ipc_shared.h'
     Tep do nam o Core\\Src\\ipc_shared.h - tuc duong include cua Engine thieu.
     Loi nay CO TRUOC dot va nay (khong phai do toi gay ra) va se anh huong
     MOI du an dung Engine, ke ca Game.exe => KHONG tu y sua, BAO CHU.
   - Ket qua build that: Core link hong 3 symbol
       unresolved external symbol __imp_?g_nGhiLogTep@@3HA ...
     dung nhu du doan tren.

VA NAY LAM:
 1) HOAN TAC KDebug.cpp + KDebug.h ve ban truoc t06 (co san .truoc_helog).
 2) Bo 3 dong doc [Log] trong KCore.cpp lien quan bien cua Engine, thay bang
    bien cua CHINH Core.
 3) Them ham ghi log dung chung trong Core + ham Lua `GhiLog(szHe, szNoiDung)`
    de script chu dong ghi. Ghi vao logs\\hethong.log kem gio + ten he.

HAN CHE CON LAI (bao chu, khong giau):
  336 diem goi g_DebugLog trong ma nguon VAN cam cho toi khi Engine build lai
  duoc. Duong log Lua thi da day du: loi runtime Lua co ca stack traceback
  kem tep:dong trong ScriptError.log (da kiem chung tren log that 29/08).

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
HAU_TO = ".truoc_helog"
NHAN_CU = "[HELOG 29/08]"
NHAN = "[HELOG2 29/08]"
T = "\t"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


HAM_LOG = r'''
//---------------------------------------------------------------------------
// ''' + NHAN + r''' GHI LOG HE THONG (ban chi-Core)
// Khong dung g_DebugLog cua Engine vi Core lien ket engine.lib dung san va
// Engine hien khong build duoc (KWin32App.cpp thieu ipc_shared.h) - xem
// ReverseTools\cauhinh\t06b_log_chi_core.py.
// Cong tac o gamesetting.ini nhom [Log].
//---------------------------------------------------------------------------
int  g_nGhiLogHeThong = 1;
char g_szTepLogHeThong[260] = "logs\\hethong.log";
int  g_nLogHeThongTranMB = 64;

void g_GhiLogHeThong(const char* szHe, const char* szNoiDung)
{
	if (!g_nGhiLogHeThong || !szNoiDung || !szNoiDung[0] || !g_szTepLogHeThong[0])
		return;

	FILE* pFile = fopen(g_szTepLogHeThong, "a");
	if (!pFile)		// dia day / thu muc chua co / tep bi khoa: bo qua im lang.
		return;		// TUYET DOI khong duoc lam sap may chu chi vi ghi log.

	SYSTEMTIME systm;
	::GetLocalTime(&systm);
	fprintf(pFile, "%04d/%02d/%02d %02d:%02d:%02d.%03d\t[%s]\t%s\n",
		systm.wYear, systm.wMonth, systm.wDay,
		systm.wHour, systm.wMinute, systm.wSecond, systm.wMilliseconds,
		(szHe && szHe[0]) ? szHe : "?", szNoiDung);

	long nCo = ftell(pFile);
	fclose(pFile);
	if (g_nLogHeThongTranMB > 0 &&
		nCo > (long)g_nLogHeThongTranMB * 1024 * 1024)
	{
		char szCu[300];
		_snprintf(szCu, sizeof(szCu) - 1, "%s.1", g_szTepLogHeThong);
		szCu[sizeof(szCu) - 1] = 0;
		remove(szCu);
		rename(g_szTepLogHeThong, szCu);
	}
}
'''

HAM_LUA = r'''
// ''' + NHAN + r''' GhiLog("TEN_HE", "noi dung") - cho script chu dong ghi lai
// dien bien/loi cua chinh no vao logs\hethong.log. Khac WriteLog (ghi
// logs\script_jx2.log, khong co truong "he") o cho co ten he de loc.
int LuaGhiLog(Lua_State* L)
{
	const char* szHe = Lua_IsString(L, 1)
		? (const char*)Lua_ValueToString(L, 1) : "SCRIPT";
	const char* szND = Lua_IsString(L, 2)
		? (const char*)Lua_ValueToString(L, 2) : NULL;
	if (!szND)		// goi 1 tham so: coi ca chuoi la noi dung
	{
		szND = szHe;
		szHe = "SCRIPT";
	}
	g_GhiLogHeThong(szHe, szND);
	return 0;
}
'''


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t06b_log_chi_core - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    # ---------- 1) hoan tac Engine ----------
    for ten in ("KDebug.cpp", "KDebug.h"):
        p = os.path.join(ENG, ten)
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            if NHAN_CU in doc(p):
                print("!!! LOI TO: %s da va ma khong co ban sao luu" % ten)
                return 1
            print("  %-12s khong can hoan tac" % ten)
            continue
        if ghi:
            shutil.copy2(sao, p)
            os.remove(sao)
            print("  %-12s DA HOAN TAC ve ban goc" % ten)
        else:
            print("  %-12s se hoan tac ve ban goc" % ten)

    # ---------- 2) KCore.cpp: doi 3 dong doc [Log] sang bien cua Core ----------
    pk = os.path.join(CORE, "KCore.cpp")
    raw = doc(pk)
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    cu_ini = eol.join([
        T*2 + 'g_GameSetting.GetInteger("Log", "GhiTep", 0, &g_nGhiLogTep);',
        T*2 + 'g_GameSetting.GetInteger("Log", "TranMB", 64, &g_nLogTranMB);',
        T*2 + 'g_GameSetting.GetString("Log", "TepLog", "logs\\\\debug.log",'
              " g_szTepLog, sizeof(g_szTepLog));",
    ])
    moi_ini = eol.join([
        T*2 + 'g_GameSetting.GetInteger("Log", "GhiTep", 1, &g_nGhiLogHeThong);',
        T*2 + 'g_GameSetting.GetInteger("Log", "TranMB", 64,'
              " &g_nLogHeThongTranMB);",
        T*2 + 'g_GameSetting.GetString("Log", "TepLog", "logs\\\\hethong.log",'
              " g_szTepLogHeThong, sizeof(g_szTepLogHeThong));",
    ])
    if NHAN in raw:
        print("  KCore.cpp    DA VA - bo qua")
        nd = raw
    else:
        if raw.count(cu_ini) != 1:
            print("!!! LOI TO: KCore.cpp - khong thay dung 1 khoi doc [Log] cu")
            return 1
        nd = raw.replace(cu_ini, moi_ini)
        # chen than ham ngay truoc InitGameSetting
        moc = "int InitGameSetting("
        vt = nd.find(moc)
        if vt < 0:
            moc = "InitGameSetting()"
            vt = nd.find(moc)
        if vt < 0:
            print("!!! LOI TO: KCore.cpp - khong thay InitGameSetting")
            return 1
        dau_dong = nd.rfind(eol, 0, vt) + len(eol)
        # lui qua khoi chu thich ngay tren
        nd = nd[:dau_dong] + HAM_LOG.strip("\n").replace("\n", eol) + eol + eol \
            + nd[dau_dong:]
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi (KCore.cpp)")
            return 1
        print("  KCore.cpp    them g_GhiLogHeThong + doi 3 dong doc [Log]")

    # ---------- 3) KCore.h: extern ----------
    ph = os.path.join(CORE, "KCore.h")
    raw_h = doc(ph)
    cu_h = eol.join([
        "// " + NHAN_CU + " cong tac ghi g_DebugLog ra tep - KCore.cpp dat tu",
        "// gamesetting.ini nhom [Log]. Mac dinh 0 = tat (y het truoc day).",
        "ENGINE_API extern int  g_nGhiLogTep;",
        "ENGINE_API extern char g_szTepLog[260];",
        "ENGINE_API extern int  g_nLogTranMB;",
    ])
    nd_h = raw_h
    if NHAN in raw_h:
        print("  KCore.h      DA VA - bo qua")
    elif cu_h in raw_h:
        print("!!! LOI TO: KCore.h co khoi extern cua t06 - khong mong doi")
        return 1
    else:
        moc_h = "extern int\t\t \t\tg_ExpRate;"
        if raw_h.count(moc_h) != 1:
            print("!!! LOI TO: KCore.h - khong thay moc g_ExpRate")
            return 1
        nd_h = raw_h.replace(moc_h, eol.join([
            "// " + NHAN + " he log cua Core (KHONG dung g_DebugLog cua Engine -",
            "// xem ReverseTools/cauhinh/t06b_log_chi_core.py)",
            "extern int  g_nGhiLogHeThong;",
            "extern char g_szTepLogHeThong[260];",
            "extern int  g_nLogHeThongTranMB;",
            "void g_GhiLogHeThong(const char* szHe, const char* szNoiDung);",
            "",
            moc_h,
        ]))
        print("  KCore.h      them extern he log")

    # ---------- 4) ScriptFuns.cpp: ham Lua GhiLog ----------
    ps = os.path.join(CORE, "ScriptFuns.cpp")
    raw_s = doc(ps)
    nd_s = raw_s
    if NHAN in raw_s:
        print("  ScriptFuns   DA VA - bo qua")
    else:
        moc_ham = "int LuaGameAlert(Lua_State* L)"
        if raw_s.count(moc_ham) != 1:
            print("!!! LOI TO: ScriptFuns.cpp - khong thay LuaGameAlert")
            return 1
        nd_s = raw_s.replace(
            moc_ham,
            HAM_LUA.strip("\n").replace("\n", eol) + eol + eol + moc_ham)
        moc_dk = '{"_ALERT",LuaGameAlert},'
        n_dk = nd_s.count(moc_dk)
        if n_dk != 1:
            print("!!! LOI TO: ScriptFuns.cpp - moc dang ky _ALERT khop %d lan"
                  % n_dk)
            return 1
        vt = nd_s.find(moc_dk)
        het = nd_s.find(eol, vt)
        nd_s = nd_s[:het + len(eol)] + T*2 + '{"GhiLog", LuaGhiLog},\t// ' \
            + NHAN + " script ghi vao logs\\hethong.log" + eol + nd_s[het + len(eol):]
        if sum(1 for c in nd_s if ord(c) > 127) != \
           sum(1 for c in raw_s if ord(c) > 127):
            print("!!! LOI TO: byte cao doi (ScriptFuns.cpp)")
            return 1
        print("  ScriptFuns   them LuaGhiLog + dang ky \"GhiLog\"")

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, nd0, raw0 in ((pk, nd, raw), (ph, nd_h, raw_h), (ps, nd_s, raw_s)):
        if nd0 == raw0:
            continue
        sao = p + ".truoc_helog2"
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd0.encode("latin-1"))
        if doc(p) != nd0:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    print("\nBuild lai Core CA HAI cau hinh (khong can Engine).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
