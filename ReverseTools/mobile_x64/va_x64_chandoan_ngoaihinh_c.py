# -*- coding: utf-8 -*-
"""[X64 08/09 NGOAIHINH c] KNpcRes::Draw: khoi 'hoi GetImageParam MOT lan moi khi doi ten' (m_bChange) -> ghi OK (mot lan/moi doi ten)
va FAIL (moi 2 s) cho chinh minh. FAIL keo dai = m_bChange ket = bo phan khong duoc SetCurFrame theo huong -> 'nhu dang mang mat na'.
Chi client (#ifndef _SERVER). Byte-safe, neo duy nhat, chay lai an toan."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
p = ROOT + r"\Sources\Core\Src\KNpcRes.cpp"
TAG = b"[X64 08/09 NGOAIHINH c]"
b = io.open(p, "rb").read()
if TAG in b:
    print("KNpcRes.cpp: da va (c)"); sys.exit(0)
nl = b"\r\n" if b"\r\n" in b else b"\n"
pat = re.compile(
    rb"(if \(g_pRepresent->GetImageParam\(m_cNpcImage\[i\]\.m_szName, &sImage, ISI_T_SPR\)\)[ \t]*\r?\n"
    rb"[ \t]*\{[ \t]*\r?\n"
    rb"[ \t]*m_cNpcImage\[i\]\.m_nTotalDir = sImage\.nNumFramesGroup;[ \t]*\r?\n"
    rb"[ \t]*m_cNpcImage\[i\]\.m_nTotalFrame = sImage\.nNumFrames;[ \t]*\r?\n"
    rb"([ \t]*)m_cNpcImage\[i\]\.m_bChange = FALSE;[ \t]*\r?\n"
    rb"([ \t]*)\}[ \t]*\r?\n)")
ms = list(pat.finditer(b))
assert len(ms) == 1, len(ms)
m = ms[0]
ind_in = m.group(2)   # thut le trong khoi (7 tab)
ind_out = m.group(3)  # thut le cua dau '}' (6 tab)
ok_log = (b"#ifndef _SERVER" + nl +
          ind_in + b"if (Npc[nNpcIdx].m_Kind == kind_player && Npc[nNpcIdx].GetPlayerIdx() > 0)\t// " + TAG + nl +
          ind_in + b"\tAUTOLOG(\"[NGOAIHINH] GetImageParam OK part%d name='%s' frames=%d dirs=%d\", i, m_cNpcImage[i].m_szName, m_cNpcImage[i].m_nTotalFrame, m_cNpcImage[i].m_nTotalDir);" + nl +
          b"#endif" + nl)
fail_log = (ind_out + b"else" + nl +
            ind_out + b"{" + nl +
            b"#ifndef _SERVER" + nl +
            ind_in + b"if (Npc[nNpcIdx].m_Kind == kind_player && Npc[nNpcIdx].GetPlayerIdx() > 0)\t// " + TAG + b" that bai -> m_bChange ket, khong SetCurFrame" + nl +
            ind_in + b"\tAUTOLOG_EVERY(2000, \"[NGOAIHINH] GetImageParam FAIL part%d name='%s' (frames=%d dirs=%d)\", i, m_cNpcImage[i].m_szName, m_cNpcImage[i].m_nTotalFrame, m_cNpcImage[i].m_nTotalDir);" + nl +
            b"#endif" + nl +
            ind_out + b"}" + nl)
blk = m.group(1)
# chen OK sau dong m_bChange = FALSE; va else sau dau '}'
pos_false = blk.find(b"m_bChange = FALSE;")
pos_after_false = blk.find(b"\n", pos_false) + 1
new_blk = blk[:pos_after_false] + ok_log + blk[pos_after_false:] + fail_log
nb = b[:m.start(1)] + new_blk + b[m.end(1):]
assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127)
io.open(p, "wb").write(nb)
print("KNpcRes.cpp: GetImageParam OK/FAIL diag OK (dong", b.count(b"\n", 0, m.start()) + 1, ")")
