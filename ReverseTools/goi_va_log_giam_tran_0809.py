# -*- coding: ascii -*-
"""log_patch_giam_tran.py - [LOG 08/09] Ha tan suat cac nhan lam TRAN log (client 64 MB / 27 phut, may chu / 40 phut)
-> AUTOLOG_EVERY(1000): moi giay toi da mot dong, khong doi hanh vi game.
Do 08/09 08:47 (jx_auto.log.1 client 27 phut): [S6-SYNC] 385.596 dong (238/giay, 68 %), [MOVE-RESET] 24.936, [S6-ME] 24.204,
[S6-CMD] 14.387; may chu (48 phut): SPICK-WORLD/RECV/FAR 37.234 moi nhan, SPICK-BELONG 25.356, SPICK-BAG 22.915.
Vi tran log nen [FX], [FX-SV], [NS-BO], [S7-*] cua tran TK 04:21-04:51 mat sach truoc khi doc duoc.
Byte-safe: latin-1, ASCII-only, CRLF, kiem so byte cao."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src"
F = {"ppc": ROOT + r"\KProtocolProcess.cpp", "shell": ROOT + r"\CoreShell.cpp", "plr": ROOT + r"\KPlayer.cpp"}
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
def rep(s, old, new, cnt=1, tag=""):
    n = s.count(old)
    if n != cnt: print("FAIL [%s]: found %d expect %d" % (tag, n, cnt)); sys.exit(1)
    return s.replace(old, new)
out = {}
s = rd(F["ppc"]); h0 = hb(s)
s = rep(s, 'AUTOLOG("[S6-SYNC] npc=%u', 'AUTOLOG_EVERY(1000, "[S6-SYNC] npc=%u', 1, "S6-SYNC")
s = rep(s, 'AUTOLOG("[S6-ME] nhanh=%s', 'AUTOLOG_EVERY(1000, "[S6-ME] nhanh=%s', 1, "S6-ME")
s = rep(s, 'AUTOLOG("[S6-CMD] lenh=run npc=%u', 'AUTOLOG_EVERY(1000, "[S6-CMD] lenh=run npc=%u', 1, "S6-CMD-run")
s = rep(s, 'AUTOLOG("[S6-CMD] lenh=walk npc=%u', 'AUTOLOG_EVERY(1000, "[S6-CMD] lenh=walk npc=%u', 1, "S6-CMD-walk")
if hb(s) != h0: print("FAIL hb ppc"); sys.exit(1)
out["ppc"] = s
s = rd(F["shell"]); h0 = hb(s)
s = rep(s, 'AUTOLOG("[MOVE-RESET] t2cu=%u', 'AUTOLOG_EVERY(1000, "[MOVE-RESET] t2cu=%u', 1, "MOVE-RESET")
if hb(s) != h0: print("FAIL hb shell"); sys.exit(1)
out["shell"] = s
s = rd(F["plr"]); h0 = hb(s)
for tag in ("SPICK-RECV", "SPICK-BELONG", "SPICK-WORLD", "SPICK-FAR", "SPICK-BAG"):
    s = rep(s, 'AUTOLOG("%s objid=' % tag, 'AUTOLOG_EVERY(1000, "%s objid=' % tag, 1, tag)
if hb(s) != h0: print("FAIL hb plr"); sys.exit(1)
out["plr"] = s
for k, v in out.items():
    if re.search(r"[^\r]\n", v): print("FAIL bare LF", k); sys.exit(1)
for k, v in out.items(): wr(F[k], v)
print("OK giam tran log: 10 nhan -> AUTOLOG_EVERY(1000)")
