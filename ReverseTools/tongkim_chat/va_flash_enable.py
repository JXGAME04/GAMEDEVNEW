# -*- coding: ascii -*-
# [TKCHAT 05/09b] KUiFlashMessage: cong tac ini Enable (0 = tat hoan toan: khong nhan tin, khong xep hang, cua so an).
import io, os

D = r"D:\GAMEDEVNEW_wt_mail\Sources\S3Client\Ui\UiCase"
T = "\t"
NL = "\n"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) >= 128)
def L(*lines): return NL.join(lines) + NL
def crlf(x): return x.replace("\r\n", "\n").replace("\n", "\r\n")
def rep(s, old, new, n=1):
    if s.count(old) != n and s.count(crlf(old)) == n:
        old, new = crlf(old), crlf(new)
    assert s.count(old) == n, ("anchor", old[:70], s.count(old))
    assert all(ord(c) < 128 for c in new), "new khong ASCII"
    return s.replace(old, new)

# ---------------- header
p = os.path.join(D, "UiFlashMessage.h"); s = rd(p); h0 = hi(s)
old = T + "int             m_nMaxLines;          // [TKCHAT 05/09] toi da so dong cung hien (ini MaxLines, mac dinh 3)" + NL
new = old + L(T + "int             m_bEnable;            // [TKCHAT 05/09b] ini Enable (0 = tat hoan toan; chu thu xem con lag khong)")
s = rep(s, old, new)
assert hi(s) == h0
wr(p, s); print("h ok")

# ---------------- cpp
p = os.path.join(D, "UiFlashMessage.cpp"); s = rd(p); h0 = hi(s)
old = T + "m_nMaxLines = 3;" + T + "// [TKCHAT 05/09]" + NL
new = old + L(T + "m_bEnable = 1;" + T + "// [TKCHAT 05/09b]")
s = rep(s, old, new)

old = T*4 + 'Ini.GetInteger(pszSec, "MaxLines", 3, &m_pSelf->m_nMaxLines);' + T + "// [TKCHAT 05/09]" + NL
new = old + L(T*4 + 'Ini.GetInteger(pszSec, "Enable", 1, &m_pSelf->m_bEnable);' + T + "// [TKCHAT 05/09b] 0 = tat hoan toan")
s = rep(s, old, new)

# OpenWindow: khong Show khi tat
old = L(
    T + "if (m_pSelf)",
    T + "{",
    T*2 + "m_pSelf->m_uLastShowTime = IR_GetCurrentTime();",
    T*2 + "m_pSelf->Show();",
    T*2 + "m_pSelf->BringToTop();",
    T + "}")
new = L(
    T + "if (m_pSelf)",
    T + "{",
    T*2 + "m_pSelf->m_uLastShowTime = IR_GetCurrentTime();",
    T*2 + "if (m_pSelf->m_bEnable)" + T + "// [TKCHAT 05/09b] Enable=0: cua so an, khong Paint/Breathe gi",
    T*2 + "{",
    T*3 + "m_pSelf->Show();",
    T*3 + "m_pSelf->BringToTop();",
    T*2 + "}",
    T*2 + "else",
    T*3 + "m_pSelf->Hide();",
    T + "}")
s = rep(s, old, new)

# MessageArrival: bo qua khi tat
old = L(
    T + "unsigned int uTime;",
    T + "if (m_pSelf && pMsg &&")
new = L(
    T + "unsigned int uTime;",
    T + "if (m_pSelf && !m_pSelf->m_bEnable)" + T + "// [TKCHAT 05/09b] tat hoan toan: khong nhan, khong xep hang",
    T*2 + "return;",
    T + "if (m_pSelf && pMsg &&")
s = rep(s, old, new)
assert hi(s) == h0, (hi(s), h0)
wr(p, s); print("cpp ok; high bytes", h0)

# ---------------- ini: them Enable=0 vao ca hai section (live + guong)
for q in [r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Ui\Ui3\UiFlashMessage.ini",
          r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\tongkim_chat\client\Ui\Ui3\UiFlashMessage.ini"]:
    s = rd(q)
    assert s.count("[Main]\r\n") == 1 and s.count("[Main1024]\r\n") == 1
    s = s.replace(";Moi dong CHI hien 1 lan.", ";Enable=0 = TAT HOAN TOAN (05/09 chu thu lag), Enable=1 = bat lai (khong can build). Moi dong CHI hien 1 lan.")
    s = s.replace("[Main]\r\n", "[Main]\r\nEnable=0\r\n").replace("[Main1024]\r\n", "[Main1024]\r\nEnable=0\r\n")
    wr(q, s); print("ini ok", q)
