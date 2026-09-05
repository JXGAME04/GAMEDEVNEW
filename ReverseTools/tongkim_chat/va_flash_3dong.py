# -*- coding: ascii -*-
# [TKCHAT 05/09] KUiFlashMessage: toi da MaxLines dong cung luc + ma hoa chu MOT lan luc sinh (do CPU) + ini nhanh hon.
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
old = T*2 + "int             nLineLen;          // total char length for this slot" + NL
new = old + L(
    T*2 + "// [TKCHAT 05/09] chu da ma hoa + do rong px, tinh MOT lan luc sinh (khong TEncodeText moi khung)",
    T*2 + "char            szEnc[512];",
    T*2 + "int             nEncLen;",
    T*2 + "int             nRongPx;")
s = rep(s, old, new)
old = T + "unsigned int    m_uNextStartTime;     // thoi diem som nhat dong ke tiep duoc sinh (cach dong truoc dung 1 hang)" + NL
new = old + L(T + "int             m_nMaxLines;          // [TKCHAT 05/09] toi da so dong cung hien (ini MaxLines, mac dinh 3)")
s = rep(s, old, new)
assert hi(s) == h0
wr(p, s); print("h ok")

# ---------------- cpp
p = os.path.join(D, "UiFlashMessage.cpp"); s = rd(p); h0 = hi(s)
old = T + "m_uNextStartTime = 0;" + NL
new = old + L(T + "m_nMaxLines = 3;" + T + "// [TKCHAT 05/09]")
s = rep(s, old, new)

old = T*4 + "if (m_pSelf->m_nRiseSpeed < 1)" + NL
new = L(
    T*4 + 'Ini.GetInteger(pszSec, "MaxLines", 3, &m_pSelf->m_nMaxLines);' + T + "// [TKCHAT 05/09]",
    T*4 + "if (m_pSelf->m_nMaxLines < 1)",
    T*5 + "m_pSelf->m_nMaxLines = 1;") + old
s = rep(s, old, new)

# ResetSlot: khoang cach sinh >= DisplayDuration/MaxLines + cache ma hoa
old = L(
    T*2 + "int nSpacingMs = (m_nRiseSpeed > 0) ? (m_nLineHeight * 1000 / m_nRiseSpeed) : 500;",
    T*2 + "if (nSpacingMs < 50)",
    T*3 + "nSpacingMs = 50;",
    T*2 + "m_uNextStartTime = uStart + nSpacingMs;",
    T + "}")
new = L(
    T*2 + "int nSpacingMs = (m_nRiseSpeed > 0) ? (m_nLineHeight * 1000 / m_nRiseSpeed) : 500;",
    T*2 + "// [TKCHAT 05/09] chu: \"hien 3 dong thoi\" -> toi da MaxLines dong cung luc: cach sinh >= DisplayDuration / MaxLines",
    T*2 + "if (m_nMaxLines > 0 && nSpacingMs < (int)m_uDisplayDuration / m_nMaxLines)",
    T*3 + "nSpacingMs = (int)m_uDisplayDuration / m_nMaxLines;",
    T*2 + "if (nSpacingMs < 50)",
    T*3 + "nSpacingMs = 50;",
    T*2 + "m_uNextStartTime = uStart + nSpacingMs;",
    T + "}",
    T + "// [TKCHAT 05/09] ma hoa chu MOT lan luc sinh (truoc: TEncodeText + do rong moi khung cho moi dong -> ton CPU, chu 'lag hon')",
    T + "slot.nEncLen = 0;",
    T + "slot.nRongPx = 0;",
    T + "if (slot.nInsertPlace == NOT_NEED_INSERT && slot.CurrentMsg.nMsgLen > 0)",
    T + "{",
    T*2 + "int nCopy = slot.CurrentMsg.nMsgLen;",
    T*2 + "if (nCopy > (int)sizeof(slot.szEnc) - 1)",
    T*3 + "nCopy = (int)sizeof(slot.szEnc) - 1;",
    T*2 + "memcpy(slot.szEnc, slot.CurrentMsg.sMsg, nCopy);",
    T*2 + "slot.szEnc[nCopy] = 0;",
    T*2 + "slot.nEncLen = TEncodeText(slot.szEnc, nCopy);",
    T*2 + "if (slot.nEncLen > 0)",
    T*3 + "slot.nRongPx = sTkChatDoRong(slot.szEnc, slot.nEncLen, m_nFontSize);",
    T + "}")
s = rep(s, old, new)

# PaintWindow: dung cache
old = L(
    T*2 + "char szTemp[512];",
    T*2 + "int nCopy = slot.CurrentMsg.nMsgLen;",
    T*2 + "if (nCopy > (int)sizeof(szTemp) - 1)",
    T*3 + "nCopy = (int)sizeof(szTemp) - 1;",
    T*2 + "memcpy(szTemp, slot.CurrentMsg.sMsg, nCopy);",
    T*2 + "szTemp[nCopy] = 0;",
    T*2 + "int nLen = TEncodeText(szTemp, nCopy);",
    T*2 + "if (nLen <= 0)",
    T*3 + "continue;",
    T*2 + "int nRong = sTkChatDoRong(szTemp, nLen, m_nFontSize);")
new = L(
    T*2 + "char szTemp[512];",
    T*2 + "const char* pszVe = szTemp;",
    T*2 + "int nLen = 0, nRong = 0;",
    T*2 + "if (slot.nEncLen > 0)",
    T*2 + "{" ,
    T*3 + "pszVe = slot.szEnc;" + T + "// [TKCHAT 05/09] da ma hoa san luc sinh",
    T*3 + "nLen = slot.nEncLen;",
    T*3 + "nRong = slot.nRongPx;",
    T*2 + "}",
    T*2 + "else",
    T*2 + "{",
    T*3 + "int nCopy = slot.CurrentMsg.nMsgLen;",
    T*3 + "if (nCopy > (int)sizeof(szTemp) - 1)",
    T*4 + "nCopy = (int)sizeof(szTemp) - 1;",
    T*3 + "memcpy(szTemp, slot.CurrentMsg.sMsg, nCopy);",
    T*3 + "szTemp[nCopy] = 0;",
    T*3 + "nLen = TEncodeText(szTemp, nCopy);",
    T*3 + "if (nLen <= 0)",
    T*4 + "continue;",
    T*3 + "nRong = sTkChatDoRong(szTemp, nLen, m_nFontSize);",
    T*2 + "}")
s = rep(s, old, new)
old = T*2 + "g_pRepresentShell->OutputRichText(m_nFontSize, &Param, szTemp, nLen, 0);" + NL + T + "}" + NL + "}" + NL
new = T*2 + "g_pRepresentShell->OutputRichText(m_nFontSize, &Param, (char*)pszVe, nLen, 0);" + NL + T + "}" + NL + "}" + NL
s = rep(s, old, new)
assert hi(s) == h0, (hi(s), h0)
wr(p, s); print("cpp ok; high bytes", h0)

# ---------------- ini (live + guong): 3 dong, nhanh hon
ini = """;[TKCHAT 05/09] Dong thong bao giet dich Tong Kim (KUiFlashMessage) - KIEU 2.0: moi dong sinh o giua man hinh (RiseFrom = Y tuyet doi),
;TROI LEN RiseSpeed px/giay, toi dan trong FadeMs cuoi, het DisplayDuration ms hoac cham mep tren (Top) thi bien mat.
;Toi da MaxLines dong cung luc (cach sinh >= DisplayDuration/MaxLines); tin moi den khi hang doi da don qua MaxQueueDelay ms thi bo.
;Moi dong CHI hien 1 lan. Cua so rong = ca man hinh, dong CAN GIUA theo be rong that. Top/Height = vung troi (khong ve nen).
[Main]
Left=0
Top=140
Width=800
Height=200
Moveable=0
IndentH=20
IndentV=2
Font=14
ShadowColor=0,0,0
ShadowColorAlpha=256
TextColor=0,255,0
MaxIdleTime=1000
ShowInterval=500
ScrollInterval=0
DisplayDuration=2500
RiseFrom=320
RiseSpeed=60
FadeMs=700
MaxLines=3
MaxQueueDelay=1200

[Main1024]
Left=0
Top=230
Width=1024
Height=200
Moveable=0
IndentH=20
IndentV=2
Font=14
ShadowColor=0,0,0
ShadowColorAlpha=256
TextColor=0,255,0
MaxIdleTime=1000
ShowInterval=500
ScrollInterval=0
DisplayDuration=2500
RiseFrom=410
RiseSpeed=60
FadeMs=700
MaxLines=3
MaxQueueDelay=1200
""".replace("\n", "\r\n")
assert all(ord(c) < 128 for c in ini)
for q in [r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Ui\Ui3\UiFlashMessage.ini",
          r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\tongkim_chat\client\Ui\Ui3\UiFlashMessage.ini"]:
    wr(q, ini); print("ini ok", q)
