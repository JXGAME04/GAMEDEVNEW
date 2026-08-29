# -*- coding: utf-8 -*-
# [S12c 28/08 toi] Chu game: "phu ve con nhay toa do bay".
# Phap y (log 20:15-21:02, CaiBang + WAuto): 122/573 mau SYNCME-DRIFT lech >= 4 o,
# dao dong LAC QUA LAC LAI ~30 o moi 1-2 giay sau khi ra cua trai: d=(5,-3) ->
# (-12,28) -> (5,-3) -> (-12,-5)... = client bi GIANG CO giua HAI nguoi lai:
# [S12-THEO] ap lenh dat cua server, con WAuto cung luc lai theo muc tieu cua no.
# Goc: bypass V1b (dich la >64 mps = khong phai echo -> ap) song SUOT cua so 3s,
# de len ca luc auto dang chu dong lai - dung cai ma 2 gac nguyen ban ngan.
# Va: bypass chi con hieu luc khi auto CHUA TU GUI lenh nao KE TU luc mo cua so
# (ghi moc g_uS12TuGuiTick tai SendClientCmdRun/Walk; so voi g_uS12CuaSoSelf).
# -> cu dat ban giao luc ha canh van duoc nuot (auto chua kip noi gi); auto len
# tieng lai la auto THANG ngay, het keo co. Nguoi choi khong auto: gac cu van cho
# qua nhu truoc (HaveTarget=false, SendMoveFrames>=5) - hanh vi 9.44 giu nguyen.
# CLIENT-ONLY (ca hai cho deu trong #ifndef _SERVER). AP SAU goi_va_S12b_cuaso_vanh.py.
import io, sys

def sua(path, hunks):
    s = io.open(path, "r", encoding="latin-1", newline="").read()
    truoc = sum(1 for c in s if ord(c) > 127)
    crlf = "\r\n" in s
    n = 0
    for ten, cu, moi in hunks:
        if crlf:
            cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
        if moi in s:
            print("  [=] %s da ap tu truoc" % ten); continue
        if s.count(cu) != 1:
            print("LOI: %s neo khop %d cho (can 1)" % (ten, s.count(cu))); sys.exit(1)
        s = s.replace(cu, moi); n += 1
        print("  [+] %s" % ten)
    sau = sum(1 for c in s if ord(c) > 127)
    if truoc != sau:
        print("LOI: high-byte doi"); sys.exit(1)
    if n:
        io.open(path, "w", encoding="latin-1", newline="").write(s)
    return n

D = r"D:\GAMEDEVNEW\Sources\Core\Src"
tong = 0

tong += sua(D + r"\KProtocolProcess.cpp", [
("H1 bien moc tu-gui",
 'int g_nS12TuGuiX = 0, g_nS12TuGuiY = 0;\n',
 'int g_nS12TuGuiX = 0, g_nS12TuGuiY = 0;\n'
 'DWORD g_uS12TuGuiTick = 0;   // [S12c] timeGetTime luc client TU GUI lenh move gan nhat\n'),
("H2 bypass chi khi auto im tieng",
 '\tif (g_nS12TuGuiX != 0 || g_nS12TuGuiY != 0)\n'
 '\t{\n'
 '\t\tint nS12Lx = nDichX - g_nS12TuGuiX; if (nS12Lx < 0) nS12Lx = -nS12Lx;\n'
 '\t\tint nS12Ly = nDichY - g_nS12TuGuiY; if (nS12Ly < 0) nS12Ly = -nS12Ly;\n'
 '\t\tif (nS12Lx > 64 || nS12Ly > 64)\n'
 '\t\t\treturn TRUE;\n'
 '\t}\n',
 '\tif (g_nS12TuGuiX != 0 || g_nS12TuGuiY != 0)\n'
 '\t{\n'
 '\t\tint nS12Lx = nDichX - g_nS12TuGuiX; if (nS12Lx < 0) nS12Lx = -nS12Lx;\n'
 '\t\tint nS12Ly = nDichY - g_nS12TuGuiY; if (nS12Ly < 0) nS12Ly = -nS12Ly;\n'
 '\t\t// [S12c 28/08] chi nuot cu dat BAN GIAO luc ha canh: auto ma DA tu gui lenh\n'
 '\t\t// KE TU luc mo cua so thi auto THANG - khong ap de nua (do that 20:15-21:02:\n'
 '\t\t// bypass song suot 3s lam client giang co 2 nguoi lai, lac +-30 o moi 1-2s).\n'
 '\t\tif ((nS12Lx > 64 || nS12Ly > 64)\n'
 '\t\t && (int)(g_uS12CuaSoSelf - g_uS12TuGuiTick) >= 0)\n'
 '\t\t\treturn TRUE;\n'
 '\t}\n'),
])

tong += sua(D + r"\KProtocol.cpp", [
("H3 extern + moc run",
 '// [S12b 28/08] xem KProtocolProcess.cpp: phan biet echo voi lenh dat-di cua server.\n'
 'extern int g_nS12TuGuiX, g_nS12TuGuiY;\n'
 'void SendClientCmdRun(int nX, int nY)\n'
 '{\n'
 '\tNPC_RUN_COMMAND\tNetCommand;\n'
 '\tg_nS12TuGuiX = nX; g_nS12TuGuiY = nY;\n',
 '// [S12b 28/08] xem KProtocolProcess.cpp: phan biet echo voi lenh dat-di cua server.\n'
 'extern int g_nS12TuGuiX, g_nS12TuGuiY;\n'
 'extern DWORD g_uS12TuGuiTick;   // [S12c] moc lan tu-gui gan nhat\n'
 'void SendClientCmdRun(int nX, int nY)\n'
 '{\n'
 '\tNPC_RUN_COMMAND\tNetCommand;\n'
 '\tg_nS12TuGuiX = nX; g_nS12TuGuiY = nY; g_uS12TuGuiTick = timeGetTime();\n'),
("H4 moc walk",
 '\tNPC_WALK_COMMAND\tNetCommand;\n'
 '\tg_nS12TuGuiX = nX; g_nS12TuGuiY = nY;\n',
 '\tNPC_WALK_COMMAND\tNetCommand;\n'
 '\tg_nS12TuGuiX = nX; g_nS12TuGuiY = nY; g_uS12TuGuiTick = timeGetTime();\n'),
])

print("Tong hunk ap: %d" % tong)
