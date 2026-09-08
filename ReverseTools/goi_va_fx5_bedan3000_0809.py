# -*- coding: ascii -*-
"""fx_patch5_bedan.py - [FX 08/09 b] Theo so [FX] tran Tong Kim 09:22 08/09 (ban do dau tien con nguyen log):
   dan: add_full=1.400-2.850 / 10 s (be dan client MAX_MISSLE=500 DAY lien tuc -> dan/hieu ung khong tao duoc),
   chet_som: ownerlost=750-3.100 / 10 s, tgtlost=1.260-1.500 / 10 s (dan bien mat giua chung),
   start - fire con ~100/10 s chua giai thich (nghi: goi dong bo do_stand/walk cua may chu de len thi trien).
 1. Client MAX_MISSLE 500 -> 3000 (may chu van 20.000). ~6 KB/o -> +15 MB tinh. Chi la be cuc bo, khong len goi tin.
 2. Dem muc day cao nhat cua be (lay mau 1/32 lan Add) -> [FX] msl_max=%d/%d.
 3. Tach ownerlost: o NPC trong (da bi go XOAXA/DEL) / o bi NPC khac dung (khac id) / con NPC nhung mo coi (RegionIndex<0, khac subworld).
 4. huy_sync: ProcNetCommand(do_stand/walk/run/jump) de len do_magic chua toi 60% (SyncNpcMin, NetCommandWalk/Run).
Byte-safe: latin-1, ASCII-only, CRLF."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src"
F = {"mh": ROOT + r"\KMissle.h", "mset": ROOT + r"\KMissleSet.cpp", "msl": ROOT + r"\KMissle.cpp", "npc": ROOT + r"\KNpc.cpp"}
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
out = {}

s = rd(F["mh"]); h0 = hb(s)
s = rep(s, "#else\n#define MAX_MISSLE 500\n#endif\n",
"#else\n#define MAX_MISSLE 3000	// [FX 08/09] 500 -> 3000: [FX] tran TK 09:22 08/09 add_full 1.400-2.850 lan/10 s (be day lien tuc) => dan/hieu ung khong tao duoc. ~6 KB/o = +15 MB tinh. Be cuc bo, khong len goi tin.\n#endif\n", 1, "mh")
if hb(s) != h0: print("FAIL hb mh"); sys.exit(1)
out["mh"] = s

s = rd(F["mset"]); h0 = hb(s)
s = rep(s,
"	int nFreeIndex = FindFree();\n	// [FIX-4 26/08] Nhan nay truoc day dat TRUOC cua chan nen in ra o MOI lan Add THANH CONG\n",
"	int nFreeIndex = FindFree();\n"
"	{	// [FX 08/09] muc day cao nhat cua be (lay mau 1/32 lan Add; GetCount duyet ca be)\n"
"		extern int g_nFX_msl_max;\n"
"		static int s_nFXMau = 0;\n"
"		if ((++s_nFXMau & 31) == 0)\n"
"		{\n"
"			const int nFXDung = GetCount();\n"
"			if (nFXDung > g_nFX_msl_max)\n"
"				g_nFX_msl_max = nFXDung;\n"
"		}\n"
"	}\n"
"	// [FIX-4 26/08] Nhan nay truoc day dat TRUOC cua chan nen in ra o MOI lan Add THANH CONG\n", 1, "mset-max")
if hb(s) != h0: print("FAIL hb mset"); sys.exit(1)
out["mset"] = s

s = rd(F["msl"]); h0 = hb(s)
s = rep(s,
"		{ extern int g_nFX_msl_ownerlost; g_nFX_msl_ownerlost++; }	// [FX 07/09]\n		DoVanish();\n",
"		{	// [FX 07/09] + [FX 08/09] tach ly do: o NPC trong (da bi go) / o bi NPC khac dung / con NPC nhung mo coi\n"
"			extern int g_nFX_msl_ownerlost, g_nFX_msl_ol_trong, g_nFX_msl_ol_khacid, g_nFX_msl_ol_mocoi;\n"
"			g_nFX_msl_ownerlost++;\n"
"			if (Npc[m_nLauncher].m_Index <= 0)\n"
"				g_nFX_msl_ol_trong++;\n"
"			else if (!Npc[m_nLauncher].IsMatch(m_dwLauncherId))\n"
"				g_nFX_msl_ol_khacid++;\n"
"			else\n"
"				g_nFX_msl_ol_mocoi++;\n"
"		}\n"
"		DoVanish();\n", 1, "msl-split")
if hb(s) != h0: print("FAIL hb msl"); sys.exit(1)
out["msl"] = s

s = rd(F["npc"]); h0 = hb(s)
s = rep(s, "int g_nFX_buff_heto = 0;\n",
"int g_nFX_buff_heto = 0;\n"
"int g_nFX_huy_sync = 0;		// [FX 08/09] lenh mang (do_stand/walk/run/jump qua ProcNetCommand) de len thi trien chua toi 60%\n"
"int g_nFX_msl_max = 0;		// [FX 08/09] muc day cao nhat cua be dan client trong 10 s\n"
"int g_nFX_msl_ol_trong = 0, g_nFX_msl_ol_khacid = 0, g_nFX_msl_ol_mocoi = 0;	// [FX 08/09] tach ownerlost\n", 1, "npc-glob")
s = rep(s,
"void KNpc::ProcNetCommand(NPCCMD cmd, int x /* = 0 */, int y /* = 0 */, int z /* = 0 */)\n{\n	switch (cmd)\n	{\n	case do_death:\n",
"void KNpc::ProcNetCommand(NPCCMD cmd, int x /* = 0 */, int y /* = 0 */, int z /* = 0 */)\n{\n"
"#ifndef _SERVER\n"
"	// [FX 08/09] goi dong bo / lenh mang de len thi trien chua toi khung 60% (SyncNpcMin do_stand, NetCommandWalk/Run) -> hieu ung khong ra\n"
"	if (m_Doing == do_magic && m_Frames.nCurrentFrame < m_Frames.nTotalFrame * ATTACKACTION_EFFECT_PERCENT / 100\n"
"		&& (cmd == do_stand || cmd == do_walk || cmd == do_run || cmd == do_jump))\n"
"		g_nFX_huy_sync++;\n"
"#endif\n"
"	switch (cmd)\n	{\n	case do_death:\n", 1, "npc-procnet")
s = rep(s, "chet_som(nolauncher=%d ownerlost=%d tgtlost=%d) | buff_het_o=%d\",\n",
          "chet_som(nolauncher=%d ownerlost=%d[trong=%d khacid=%d mocoi=%d] tgtlost=%d) | buff_het_o=%d | huy_sync=%d msl_max=%d/%d\",\n", 1, "npc-fmt")
s = rep(s, "					g_nFX_msl_nolauncher, g_nFX_msl_ownerlost, g_nFX_msl_tgtlost, g_nFX_buff_heto);\n",
          "					g_nFX_msl_nolauncher, g_nFX_msl_ownerlost, g_nFX_msl_ol_trong, g_nFX_msl_ol_khacid, g_nFX_msl_ol_mocoi, g_nFX_msl_tgtlost, g_nFX_buff_heto, g_nFX_huy_sync, g_nFX_msl_max, (int)MAX_MISSLE);\n", 1, "npc-args")
s = rep(s, "				g_nFX_msl_nolauncher = 0; g_nFX_msl_ownerlost = 0; g_nFX_msl_tgtlost = 0; g_nFX_buff_heto = 0;\n",
          "				g_nFX_msl_nolauncher = 0; g_nFX_msl_ownerlost = 0; g_nFX_msl_tgtlost = 0; g_nFX_buff_heto = 0;\n"
          "				g_nFX_msl_ol_trong = 0; g_nFX_msl_ol_khacid = 0; g_nFX_msl_ol_mocoi = 0; g_nFX_huy_sync = 0; g_nFX_msl_max = 0;\n", 1, "npc-reset")
if hb(s) != h0: print("FAIL hb npc"); sys.exit(1)
out["npc"] = s

for k, v in out.items():
    if re.search(r"[^\r]\n", v): print("FAIL bare LF", k); sys.exit(1)
for k, v in out.items(): wr(F[k], v)
print("OK be dan 3000 + bo dem: mh, mset, msl, npc")
