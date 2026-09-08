# -*- coding: ascii -*-
"""fx_patch6_ownerlost_once.py - [FX 08/09 c] ownerlost dem MOI NHIP (Activate goi lai khi dan dang o MS_DoVanish) -> so bi phong to
(9.720/10 s). Nay chi dem LAN DAU (khi status chua phai MS_DoVanish) = so VIEN DAN bi mat nguoi phong."""
import io, re, sys
p = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KMissle.cpp"
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
s = io.open(p, "r", encoding="latin-1", newline="").read(); h0 = hb(s)
old = ("\t\t{\t// [FX 07/09] + [FX 08/09] tach ly do: o NPC trong (da bi go) / o bi NPC khac dung / con NPC nhung mo coi\r\n"
       "\t\t\textern int g_nFX_msl_ownerlost, g_nFX_msl_ol_trong, g_nFX_msl_ol_khacid, g_nFX_msl_ol_mocoi;\r\n"
       "\t\t\tg_nFX_msl_ownerlost++;\r\n")
new = ("\t\tif (m_eMissleStatus != MS_DoVanish)\t// [FX 08/09 c] chi dem LAN DAU: Activate goi lai moi nhip khi dan dang tan -> truoc day dem theo nhip\r\n"
       "\t\t{\t// [FX 07/09] + [FX 08/09] tach ly do: o NPC trong (da bi go) / o bi NPC khac dung / con NPC nhung mo coi\r\n"
       "\t\t\textern int g_nFX_msl_ownerlost, g_nFX_msl_ol_trong, g_nFX_msl_ol_khacid, g_nFX_msl_ol_mocoi;\r\n"
       "\t\t\tg_nFX_msl_ownerlost++;\r\n")
n = s.count(old)
if n != 1: print("FAIL found", n); sys.exit(1)
s = s.replace(old, new)
if hb(s) != h0 or re.search(r"[^\r]\n", s): print("FAIL hb/LF"); sys.exit(1)
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("OK ownerlost dem mot lan")
