# -*- coding: ascii -*-
"""fx_patch4_rep3_d3d9ex.py - [RAM 08/09] Represent3: cong tac [Client] Rep3Ex=1 -> tao D3D9Ex (Direct3DCreate9Ex +
CreateDeviceEx). Muc dich: do xem driver co con giu BAN SAO texture trong RAM tien trinh khong.
Do 08/09 04:2x tren Game.exe: moi MB texture o VRAM keo theo ~0,7 MB RAM rieng, nam trong cac vung 15,8 MB gom ~110 manh
0,1-0,6 MB (dung co mot texture), khong doc duoc tu ngoai = bo nho driver anh xa. Voi D3D9 cu (khong Ex) driver phai giu
ban sao he thong cua tai nguyen POOL_DEFAULT; D3D9Ex de WDDM tu quan ly nen ky vong khong con ban sao.
 - D3D_Shell: neu Rep3Ex=1 thi Direct3DCreate9Ex (tra dong qua GetProcAddress, khong doi lib), luu m_pD3DEx, g_nRep3ExOn=1.
 - D3D_Device: co Ex -> CreateDeviceEx (windowed: NULL; fullscreen: D3DDISPLAYMODEEX tu pMode); hong thi lui ve CreateDevice.
 - D3D9Ex KHONG co POOL_MANAGED: 8 cho MANAGED -> (REP3_USAGE_MANAGED, REP3_POOL_MANAGED) = DYNAMIC+DEFAULT khi Ex, giu cu khi khong;
   Rep3Pool bat buoc = 1 (dem SYSTEMMEM + DEFAULT) khi Ex.
 - Mac dinh Rep3Ex=0 (hanh vi cu y het). Log [REP3] D3D9Ex: BAT/tat.
Byte-safe: latin-1, ASCII-only, CRLF."""
import io, re, sys
R3 = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
F = {"shh": R3 + r"\D3D_Shell.h", "sh": R3 + r"\D3D_Shell.cpp", "devh": R3 + r"\D3D_Device.h", "dev": R3 + r"\D3D_Device.cpp",
     "rs": R3 + r"\KRepresentShell3.cpp", "tr": R3 + r"\TextureRes.cpp",
     "font": r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\iRepresent\Font\KFontRes.cpp"}
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
def crlf(s): return s.replace("\r\n", "\n").replace("\n", "\r\n")
def rep(s, old, new, cnt=1, tag=""):
    old = crlf(old); new = crlf(new)
    for t in (old, new):
        if any(ord(c) >= 0x80 for c in t): print("FAIL non-ASCII"); sys.exit(1)
    n = s.count(old)
    if n != cnt: print("FAIL [%s]: found %d expect %d: %r" % (tag, n, cnt, old[:80])); sys.exit(1)
    return s.replace(old, new)
def rep_re(s, pat, new, cnt=1, tag=""):
    ms = list(re.finditer(pat, s))
    if len(ms) != cnt: print("FAIL re[%s]: found %d expect %d" % (tag, len(ms), cnt)); sys.exit(1)
    return re.sub(pat, new, s)
out = {}

# ---------------- D3D_Shell.h: them m_pD3DEx
s = rd(F["shh"]); h0 = hb(s)
s = rep_re(s, r"(LPDIRECT3D9\s+m_pD3D;[^\r\n]*\r\n)",
 lambda m: m.group(1) + "    LPDIRECT3D9EX			m_pD3DEx;		// [RAM 08/09] khac NULL khi tao duoc D3D9Ex (Rep3Ex=1); cung doi tuong voi m_pD3D\r\n", 1, "shh")
if hb(s) != h0: print("FAIL hb shh"); sys.exit(1)
out["shh"] = s

# ---------------- D3D_Shell.cpp: tao Ex
s = rd(F["sh"]); h0 = hb(s)
s = rep(s,
"CD3D_Shell g_D3DShell;",
"// [RAM 08/09] Rep3Ex doc tu config.ini trong KRepresentShell3::Create (chay truoc g_D3DShell.Create); g_nRep3ExOn = da tao duoc Ex\n"
"extern int g_nRep3Ex;\n"
"int g_nRep3ExOn = 0;\n"
"\n"
"CD3D_Shell g_D3DShell;", 1, "sh-glob")
s = rep(s,
"	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);\n	if (!m_pD3D) return false;\n",
"	// [RAM 08/09] Rep3Ex=1: thu D3D9Ex truoc (tra ham dong de khong phu thuoc lib); hong thi ve D3D9 cu\n"
"	m_pD3D = NULL;\n"
"	m_pD3DEx = NULL;\n"
"	g_nRep3ExOn = 0;\n"
"	if (g_nRep3Ex)\n"
"	{\n"
"		typedef HRESULT (WINAPI *PFN_REP3_D3DC9EX)(UINT, IDirect3D9Ex**);\n"
"		HMODULE hD3D9 = GetModuleHandleA(\"d3d9.dll\");\n"
"		if (!hD3D9)\n"
"			hD3D9 = LoadLibraryA(\"d3d9.dll\");\n"
"		PFN_REP3_D3DC9EX pfnEx = hD3D9 ? (PFN_REP3_D3DC9EX)GetProcAddress(hD3D9, \"Direct3DCreate9Ex\") : NULL;\n"
"		IDirect3D9Ex* pEx = NULL;\n"
"		if (pfnEx && SUCCEEDED(pfnEx(D3D_SDK_VERSION, &pEx)) && pEx)\n"
"		{\n"
"			m_pD3DEx = pEx;\n"
"			m_pD3D = pEx;\n"
"			g_nRep3ExOn = 1;\n"
"		}\n"
"	}\n"
"	if (!m_pD3D)\n"
"		m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);\n"
"	if (!m_pD3D) return false;\n", 1, "sh-create")
s = rep(s,
"void CD3D_Shell::Reset()\n{\n	m_pD3D = NULL;\n",
"void CD3D_Shell::Reset()\n{\n	m_pD3D = NULL;\n	m_pD3DEx = NULL;	// [RAM 08/09] cung doi tuong, da Release qua m_pD3D\n", 1, "sh-reset")
if hb(s) != h0: print("FAIL hb sh"); sys.exit(1)
out["sh"] = s

# ---------------- D3D_Device.h: extern + macro pool
s = rd(F["devh"]); h0 = hb(s)
s = rep(s,
"#define PD3DDEVICE (g_Device.m_pD3DDevice)",
"// [RAM 08/09] D3D9Ex khong co POOL_MANAGED: cac texture 'MANAGED' cu dung DYNAMIC + DEFAULT khi Ex bat (van LockRect duoc)\n"
"extern int g_nRep3ExOn;\n"
"#define REP3_POOL_MANAGED  (g_nRep3ExOn ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED)\n"
"#define REP3_USAGE_MANAGED (g_nRep3ExOn ? D3DUSAGE_DYNAMIC : 0)\n"
"#define PD3DDEVICE (g_Device.m_pD3DDevice)", 1, "devh")
if hb(s) != h0: print("FAIL hb devh"); sys.exit(1)
out["devh"] = s

# ---------------- D3D_Device.cpp: CreateDeviceEx + probe
s = rd(F["dev"]); h0 = hb(s)
s = rep(s,
"	HRESULT hResult = PDIRECT3D->CreateDevice(pAdapter->iAdapterNum,pDevice->DeviceType,g_hWnd,BehaviorFlags,&m_PresentationParam,&m_pD3DDevice);\n",
"	// [RAM 08/09] co D3D9Ex thi CreateDeviceEx (windowed: khong can display mode; fullscreen: D3DDISPLAYMODEEX theo pMode); hong thi lui ve CreateDevice\n"
"	HRESULT hResult = E_FAIL;\n"
"	if (g_nRep3ExOn && g_D3DShell.m_pD3DEx)\n"
"	{\n"
"		D3DDISPLAYMODEEX modeEx;\n"
"		ZeroMemory(&modeEx, sizeof(modeEx));\n"
"		modeEx.Size = sizeof(modeEx);\n"
"		modeEx.Width = pMode->Width;\n"
"		modeEx.Height = pMode->Height;\n"
"		modeEx.RefreshRate = 0;\n"
"		modeEx.Format = pMode->Format;\n"
"		modeEx.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;\n"
"		IDirect3DDevice9Ex* pDevEx = NULL;\n"
"		hResult = g_D3DShell.m_pD3DEx->CreateDeviceEx(pAdapter->iAdapterNum, pDevice->DeviceType, g_hWnd, BehaviorFlags, &m_PresentationParam,\n"
"			m_PresentationParam.Windowed ? NULL : &modeEx, &pDevEx);\n"
"		if (SUCCEEDED(hResult) && pDevEx)\n"
"			m_pD3DDevice = pDevEx;\n"
"		else\n"
"		{\n"
"			g_DebugLog(\"[D3DRender]CreateDeviceEx that bai 0x%08X -> lui ve CreateDevice\", (unsigned)hResult);\n"
"			m_pD3DDevice = NULL;\n"
"		}\n"
"	}\n"
"	if (!m_pD3DDevice)\n"
"		hResult = PDIRECT3D->CreateDevice(pAdapter->iAdapterNum,pDevice->DeviceType,g_hWnd,BehaviorFlags,&m_PresentationParam,&m_pD3DDevice);\n", 1, "dev-create")
s = rep(s,
"HRESULT hrProbe = m_pD3DDevice->CreateTexture(33, 17, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pProbe, NULL);",
"HRESULT hrProbe = m_pD3DDevice->CreateTexture(33, 17, 1, 0, D3DFMT_A8R8G8B8, REP3_POOL_MANAGED, &pProbe, NULL);	// [RAM 08/09] Ex: DEFAULT", 1, "dev-probe")
if hb(s) != h0: print("FAIL hb dev"); sys.exit(1)
out["dev"] = s

# ---------------- KRepresentShell3.cpp: ini, log, 3 texture pre-render
s = rd(F["rs"]); h0 = hb(s)
s = rep(s,
"int  g_nRep3CacheMB   = 0;\n",
"int  g_nRep3CacheMB   = 0;\nint  g_nRep3Ex        = 0;	// [RAM 08/09] 1 = tao D3D9Ex (ky vong driver khong giu ban sao texture trong RAM)\n", 1, "rs-glob")
s = rep(s,
"	g_nRep3Pool      = Rep3Ini(\"Rep3Pool\", 1);		// [REP3 03/09 RAM]\n",
"	g_nRep3Pool      = Rep3Ini(\"Rep3Pool\", 1);		// [REP3 03/09 RAM]\n"
"	g_nRep3Ex        = Rep3Ini(\"Rep3Ex\", 0);		// [RAM 08/09]\n"
"	if (g_nRep3Ex)\n"
"		g_nRep3Pool = 1;	// D3D9Ex khong co POOL_MANAGED: bat buoc dem SYSTEMMEM + DEFAULT\n", 1, "rs-ini")
s = rep(s, "cacheMB=%d pool=%s statSec=%d\",\n", "cacheMB=%d pool=%s statSec=%d ex=%d\",\n", 1, "rs-fmt")
s = rep(s, "g_nRep3Pool ? \"DEFAULT(VRAM)\" : \"MANAGED(RAM+VRAM)\", g_nRep3StatSec);	// [REP3 03/09 RAM]\n",
          "g_nRep3Pool ? \"DEFAULT(VRAM)\" : \"MANAGED(RAM+VRAM)\", g_nRep3StatSec, g_nRep3Ex);	// [REP3 03/09 RAM]\n", 1, "rs-args")
s = rep(s,
"	g_DebugLog(\"[D3DRender]RestoreDeviceObjects ok!\");\n",
"	g_DebugLog(\"[D3DRender]RestoreDeviceObjects ok!\");\n"
"	Rep3Log(\"[REP3] D3D9Ex: %s\", g_nRep3ExOn ? \"BAT (Rep3Ex=1, texture MANAGED -> DYNAMIC+DEFAULT)\" : \"tat\");	// [RAM 08/09]\n", 1, "rs-log")
s = rep_re(s, r"0, (D3DFMT_A4R4G4B4), D3DPOOL_MANAGED, &m_pPreRenderTexture", r"REP3_USAGE_MANAGED, \1, REP3_POOL_MANAGED, &m_pPreRenderTexture", 3, "rs-prerender")
if hb(s) != h0: print("FAIL hb rs"); sys.exit(1)
out["rs"] = s

# ---------------- TextureRes.cpp: 4 cho MANAGED
s = rd(F["tr"]); h0 = hb(s)
s = rep_re(s, r"0, (D3DFMT_A4R4G4B4|D3DFMT_A8R8G8B8|D3DFMT_DXT5), D3DPOOL_MANAGED, &m_pTextureInfo", r"REP3_USAGE_MANAGED, \1, REP3_POOL_MANAGED, &m_pTextureInfo", 4, "tr-managed")
if hb(s) != h0: print("FAIL hb tr"); sys.exit(1)
out["tr"] = s

# ---------------- KFontRes.cpp: texture chu
s = rd(F["font"]); h0 = hb(s)
s = rep(s,
"#include \"KFontRes.h\"\n",
"#include \"KFontRes.h\"\n"
"// [RAM 08/09] D3D9Ex khong co POOL_MANAGED (xem Represent3/D3D_Device.h)\n"
"extern int g_nRep3ExOn;\n", 1, "font-ext")
s = rep(s,
"                                      0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &m_pCharTexture, NULL);\n",
"                                      (g_nRep3ExOn ? D3DUSAGE_DYNAMIC : 0), D3DFMT_A4R4G4B4, (g_nRep3ExOn ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED), &m_pCharTexture, NULL);	// [RAM 08/09]\n", 1, "font-tex")
if hb(s) != h0: print("FAIL hb font"); sys.exit(1)
out["font"] = s

for k, v in out.items():
    if re.search(r"[^\r]\n", v): print("FAIL bare LF", k); sys.exit(1)
for k, v in out.items():
    wr(F[k], v)
print("OK d3d9ex patch:", ", ".join(out.keys()))
print("ALL APPLIED (rep3 d3d9ex)")
