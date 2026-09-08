# -*- coding: utf-8 -*-
"""[X64 08/09 NGOAIHINH b] KNpcRes::Draw(nNpcIdx,...): moi 3 s ghi ten sprite + so khung/huong + CheckExist/m_bChange tung bo phan
cua CHINH MINH (Npc kind_player co m_nPlayerIdx > 0 tren client) -> biet Core dua ten gi cho bo ve luc dang nhap vs sau khi mac lai.
Chi client (#ifndef _SERVER). Byte-safe, neo duy nhat, chay lai an toan."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
p = ROOT + r"\Sources\Core\Src\KNpcRes.cpp"
TAG = b"[X64 08/09 NGOAIHINH b]"
b = io.open(p, "rb").read()
if TAG in b:
    print("KNpcRes.cpp: da va"); sys.exit(0)
nl = b"\r\n" if b"\r\n" in b else b"\n"
m = re.search(rb"void\tKNpcRes::Draw\(int nNpcIdx, int nDir, int nAllFrame, int nCurFrame, BOOL bInMenu, BOOL bPaintBody\)" + re.escape(nl) + rb"\{", b)
assert m, "khong thay dau ham Draw"
anchor = b"\tif (!m_pcResNode)" + nl + b"\t\treturn;" + nl
i = b.find(anchor, m.end())
assert i > 0 and i - m.end() < 600, "khong thay neo m_pcResNode gan dau ham"
ins = nl.join([
    b"#ifndef _SERVER",
    b"\t// " + TAG + b" moi 3 s ghi ten sprite + khung/huong tung bo phan cua chinh minh (chan doan 'nhu dang mang mat na' tren x64)",
    b"\tif (Npc[nNpcIdx].m_Kind == kind_player && Npc[nNpcIdx].m_nPlayerIdx > 0)",
    b"\t{",
    b"\t\tstatic DWORD s_uNgoaiHinhLuc = 0;",
    b"\t\tDWORD uNgoaiHinhNow = GetTickCount();",
    b"\t\tif ((DWORD)(uNgoaiHinhNow - s_uNgoaiHinhLuc) >= 3000)",
    b"\t\t{",
    b"\t\t\ts_uNgoaiHinhLuc = uNgoaiHinhNow;",
    b"\t\t\tfor (int k = 0; k < MAX_PART; k++)",
    b"\t\t\t\tAUTOLOG(\"[NGOAIHINH] Draw self part%d: name='%s' frames=%d dirs=%d exist=%d change=%d | fx='%s' fxframes=%d | act=%d\", k, m_cNpcImage[k].m_szName, m_cNpcImage[k].m_nTotalFrame, m_cNpcImage[k].m_nTotalDir, (int)m_cNpcImage[k].CheckExist(), (int)m_cNpcImage[k].m_bChange, m_cNpcEffectImage[k].m_szName, m_cNpcEffectImage[k].m_nTotalFrame, (int)m_nAction);",
    b"\t\t}",
    b"\t}",
    b"#endif"]) + nl
nb = b[:i + len(anchor)] + ins + b[i + len(anchor):]
assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127)
io.open(p, "wb").write(nb)
print("KNpcRes.cpp: Draw diag OK (chen sau dong", b.count(b"\n", 0, i + len(anchor)) + 1, ")")
