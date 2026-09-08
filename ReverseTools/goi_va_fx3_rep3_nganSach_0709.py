# -*- coding: ascii -*-
"""fx_patch3_rep3_budget.py - [FX 08/09] Represent3: ngan sach cache texture mac dinh 512 -> 1024 MB + kep theo VRAM con.
Chu xac nhan 08/09 04:01: sau [Client] Rep3CacheMB=1500 (17:48 07/09) "chua thay mat hieu ung ky nang lai" qua 5 tran
Tong Kim, cache dinh 768 MB, RAM rieng dinh 789 MB. Tran 512 cu lam cache ket (508-511/512) va nhanh vuot ngan sach
bao het khung hieu ung nghi > 10 s roi giai ma lai khong kip. Nay sua trong ma de may khac khong phai sua ini:
 - SetBudget: kep 512 -> 1024 (RAM rieng ~ 240 + 0,66 x texture -> ~920 MB, duoi 2 GB vi Game.exe chua LAA).
 - CapBudgetByVram(): sau khi tao device, neu KHONG co Rep3CacheMB thi kep them <= 1/2 VRAM con (POOL_DEFAULT = VRAM).
Byte-safe: latin-1, ASCII-only, CRLF."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
F = {"h": ROOT + r"\TextureResMgr.h", "mgr": ROOT + r"\TextureResMgr.cpp", "sh": ROOT + r"\KRepresentShell3.cpp"}
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

h = rd(F["h"]); h0 = hb(h)
h = rep(h,
"    void SetBudget();\n",
"    void SetBudget();\n"
"    void CapBudgetByVram(unsigned __int64 uVramFreeMB);	// [FX 08/09] kep them theo VRAM con, goi sau khi tao device\n", 1, "h")
if hb(h) != h0: print("FAIL hb h"); sys.exit(1)

m = rd(F["mgr"]); m0 = hb(m)
m = rep(m,
"	if (uBudgetMB > 512) uBudgetMB = 512;	// [REP3 03/09 RAM2] DINH CHINH: chu thich cu noi POOL_DEFAULT\n",
"	if (uBudgetMB > 1024) uBudgetMB = 1024;	// [FX 08/09] 512 -> 1024: tran 512 lam cache KET (508-511/512, 07/09 17:xx) va nhanh vuot\n"
"											// ngan sach bao het khung hieu ung nghi > 10 s -> 'mat het hinh anh ky nang'. Chu xac nhan\n"
"											// Rep3CacheMB=1500 het loi qua 5 tran (dinh 768 MB, RAM rieng 789 MB). 1024 MB ~ 240 + 0,66 x 1024\n"
"											// = ~920 MB RAM rieng, duoi 2 GB (Game.exe chua LARGEADDRESSAWARE). Kep them theo VRAM: CapBudgetByVram.\n"
"											// [REP3 03/09 RAM2] DINH CHINH: chu thich cu noi POOL_DEFAULT\n", 1, "mgr-clamp")
m = rep(m,
"void TextureResMgr::GetStat(",
"// [FX 08/09] Kep them theo VRAM con trong luc tao device (texture o POOL_DEFAULT = VRAM): toi da 1/2 VRAM con.\n"
"// Chi ap khi KHONG co [Client] Rep3CacheMB (ini ghi de thi giu nguyen y chu). Goi tu KRepresentShell3::Create sau CreateDevice.\n"
"void TextureResMgr::CapBudgetByVram(unsigned __int64 uVramFreeMB)\n"
"{\n"
"	if (g_nRep3CacheMB > 0 || uVramFreeMB == 0)\n"
"		return;\n"
"	unsigned __int64 uBudgetMB = ((unsigned __int64)(uint32)m_nBalanceNum) >> 20;\n"
"	unsigned __int64 uCapMB = uVramFreeMB / 2;\n"
"	if (uCapMB < 60)\n"
"		uCapMB = 60;\n"
"	if (uBudgetMB > uCapMB)\n"
"	{\n"
"		m_nBalanceNum = (int32)(uCapMB * 1024 * 1024);\n"
"		Rep3Log(\"[REP3] cache texture: VRAM con %I64u MB -> kep ngan sach %I64u -> %I64u MB\", uVramFreeMB, uBudgetMB, uCapMB);\n"
"	}\n"
"}\n"
"\n"
"void TextureResMgr::GetStat(", 1, "mgr-cap")
if hb(m) != m0: print("FAIL hb mgr"); sys.exit(1)

s = rd(F["sh"]); s0 = hb(s)
s = rep(s,
"	g_DebugLog(\"[D3DRender]RestoreDeviceObjects ok!\");\n",
"	g_DebugLog(\"[D3DRender]RestoreDeviceObjects ok!\");\n"
"	m_TextureResMgr.CapBudgetByVram((unsigned __int64)(PD3DDEVICE->GetAvailableTextureMem() >> 20));	// [FX 08/09] kep ngan sach theo VRAM con\n", 1, "sh-cap")
if hb(s) != s0: print("FAIL hb sh"); sys.exit(1)

for k, v in (("h", h), ("mgr", m), ("sh", s)):
    if re.search(r"[^\r]\n", v): print("FAIL bare LF", k); sys.exit(1)
wr(F["h"], h); wr(F["mgr"], m); wr(F["sh"], s)
print("OK budget patch: h=%d mgr=%d sh=%d high bytes" % (h0, m0, s0))
print("ALL APPLIED (rep3 budget)")
