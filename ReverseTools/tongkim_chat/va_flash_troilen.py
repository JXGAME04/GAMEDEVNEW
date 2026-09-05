# -*- coding: ascii -*-
# [TKCHAT 04/09c] Va KUiFlashMessage thanh dong TROI LEN kieu 2.0 (chu: "dong thong bao luc danh chet chay giua man hinh len roi bien mat").
# Chi chen ASCII; doc/ghi latin-1; kiem so byte cao khong doi (nguon TCVN3).
import io, os, sys

D = r"D:\GAMEDEVNEW_wt_mail\Sources\S3Client\Ui\UiCase"
T = "\t"
NL = "\n"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) >= 128)
def L(*lines): return NL.join(lines) + NL
def crlf(x): return x.replace("\r\n", "\n").replace("\n", "\r\n")
def rep(s, old, new, n=1):
    # tep nguon co the CRLF: thu neo LF truoc, khong thay thi thu CRLF va chen theo CRLF
    if s.count(old) != n and s.count(crlf(old)) == n:
        old, new = crlf(old), crlf(new)
    assert s.count(old) == n, ("anchor", old[:70], s.count(old))
    assert all(ord(c) < 128 for c in new), "new khong ASCII"
    return s.replace(old, new)
def find_nl(s, anchor, start=0):
    # tra (vi tri, chuoi neo thuc) theo LF hoac CRLF
    i = s.find(anchor, start)
    if i >= 0: return i, anchor
    a2 = crlf(anchor); i = s.find(a2, start)
    assert i >= 0, ("anchor", anchor[:70])
    return i, a2

# ---------------- header
p = os.path.join(D, "UiFlashMessage.h"); s = rd(p); h0 = hi(s)
old = T + "unsigned int    m_uDisplayDuration;   // NEW: how long to show after head hits left (ms)" + NL
new = old + L(
    T + '// [TKCHAT 04/09c] dong TROI LEN nhu 2.0 (chu: "dong thong bao luc danh chet chay giua man hinh len roi bien mat")',
    T + "int             m_nRiseFrom;          // Y tuyet doi noi dong sinh ra (ini RiseFrom; -1 = day cua so)",
    T + "int             m_nRiseSpeed;         // toc do troi len, px/giay (ini RiseSpeed)",
    T + "int             m_nFadeMs;            // toi dan trong FadeMs cuoi (ini FadeMs; 0 = khong)",
    T + "unsigned int    m_uMaxQueueDelay;     // hang doi don qua muc nay (ms) thi bo tin moi (ini MaxQueueDelay)",
    T + "unsigned int    m_uNextStartTime;     // thoi diem som nhat dong ke tiep duoc sinh (cach dong truoc dung 1 hang)",
    T + "int     RiseFromY() { return (m_nRiseFrom >= 0) ? m_nRiseFrom : (m_nAbsoluteTop + m_Height - m_nIndentV - m_nLineHeight); }")
s = rep(s, old, new)
assert hi(s) == h0
wr(p, s); print("h ok")

# ---------------- cpp
p = os.path.join(D, "UiFlashMessage.cpp"); s = rd(p); h0 = hi(s)

# constructor
old = T + "srand(IR_GetCurrentTime());" + NL
new = old + L(
    T + "m_nRiseFrom = -1;" + T + T + "// [TKCHAT 04/09c]",
    T + "m_nRiseSpeed = 35;",
    T + "m_nFadeMs = 1500;",
    T + "m_uMaxQueueDelay = 1500;",
    T + "m_uNextStartTime = 0;")
s = rep(s, old, new)

# LoadScheme
old = T*3 + 'Ini.GetInteger("Main", "DisplayDuration", 5000, (int*)&m_pSelf->m_uDisplayDuration);' + NL
new = old + L(
    T*3 + "// [TKCHAT 04/09c] tham so troi len - doc o section dang dung de 800/1024 dat khac nhau",
    T*3 + "{",
    T*4 + 'const char* pszSec = (SCREEN_WIDTH == 1024) ? "Main1024" : "Main";',
    T*4 + 'Ini.GetInteger(pszSec, "RiseFrom", -1, &m_pSelf->m_nRiseFrom);',
    T*4 + 'Ini.GetInteger(pszSec, "RiseSpeed", 35, &m_pSelf->m_nRiseSpeed);',
    T*4 + 'Ini.GetInteger(pszSec, "FadeMs", 1500, &m_pSelf->m_nFadeMs);',
    T*4 + 'Ini.GetInteger(pszSec, "MaxQueueDelay", 1500, (int*)&m_pSelf->m_uMaxQueueDelay);',
    T*4 + "if (m_pSelf->m_nRiseSpeed < 1)",
    T*5 + "m_pSelf->m_nRiseSpeed = 1;",
    T*3 + "}")
s = rep(s, old, new)

# AddMessage: FIFO + bo khi don
old = L(
    T*2 + "pNew = (KNewsMessageNode*)malloc(sizeof(KNewsMessageNode));",
    T*2 + "if (pNew)",
    T*2 + "{",
    T*3 + "*(KNewsMessage*)pNew = *pMsg;",
    T*3 + "pNew->uTime = uTime;",
    T*3 + "pNew->uStartTime = uTimeParam;",
    T*3 + "pNew->pNext = m_pHead;",
    T*3 + "m_pHead = pNew;",
    T*2 + "}")
new = L(
    T*2 + "// [TKCHAT 04/09c] hang doi da don qua MaxQueueDelay -> bo tin moi (dong troi len theo nhip co dinh,",
    T*2 + "// khong de tin giet dich hien muon hang chuc giay khi Tong Kim dong nguoi)",
    T*2 + "if (m_uMaxQueueDelay > 0)",
    T*2 + "{",
    T*3 + "unsigned int uNow = IR_GetCurrentTime();",
    T*3 + "if ((int)(m_uNextStartTime - uNow) > (int)m_uMaxQueueDelay)",
    T*4 + "return false;",
    T*2 + "}",
    T*2 + "pNew = (KNewsMessageNode*)malloc(sizeof(KNewsMessageNode));",
    T*2 + "if (pNew)",
    T*2 + "{",
    T*3 + "*(KNewsMessage*)pNew = *pMsg;",
    T*3 + "pNew->uTime = uTime;",
    T*3 + "pNew->uStartTime = uTimeParam;",
    T*3 + "pNew->pNext = NULL;",
    T*3 + "AddToTail(pNew);" + T + "// [TKCHAT 04/09c] FIFO (truoc: chen dau danh sach -> khi don, tin moi hien truoc tin cu)",
    T*2 + "}")
s = rep(s, old, new)

# ResetSlot: lich sinh dong
old = L(
    T + "slot.uDisplayStartTime = IR_GetCurrentTime();",
    "",
    T + "slot.nCharIndex = 0;")
new = L(
    T + "// [TKCHAT 04/09c] lich sinh dong: cach dong truoc dung 1 hang (m_nLineHeight / RiseSpeed) -> khong bao gio de len nhau",
    T + "{",
    T*2 + "unsigned int uNow = IR_GetCurrentTime();",
    T*2 + "unsigned int uStart = uNow;",
    T*2 + "if ((int)(m_uNextStartTime - uNow) > 0)",
    T*3 + "uStart = m_uNextStartTime;",
    T*2 + "slot.uDisplayStartTime = uStart;",
    T*2 + "int nSpacingMs = (m_nRiseSpeed > 0) ? (m_nLineHeight * 1000 / m_nRiseSpeed) : 500;",
    T*2 + "if (nSpacingMs < 50)",
    T*3 + "nSpacingMs = 50;",
    T*2 + "m_uNextStartTime = uStart + nSpacingMs;",
    T + "}",
    "",
    T + "slot.nCharIndex = 0;")
s = rep(s, old, new)

# Breathe: het han hoac qua mep tren; NORMAL chi hien 1 lan
old = L(
    T*3 + "// check stationary timeout",
    T*3 + "if (now >= slot.uDisplayStartTime + m_uDisplayDuration)",
    T*3 + "{",
    T*4 + "if (slot.pSourceNode) AddToTail(slot.pSourceNode);",
    T*4 + "slot.bActive = false;",
    T*4 + "slot.pSourceNode = NULL;",
    T*4 + "continue;",
    T*3 + "}")
new = L(
    T*3 + "// [TKCHAT 04/09c] het han (DisplayDuration) HOAC da troi qua mep tren cua so -> bien mat;",
    T*3 + "// tin NORMAL chi hien MOT lan (truoc: AddToTail -> cung dong hien lai toi MAX_NORMAL_SHOW_TIMES = 3 lan)",
    T*3 + "{",
    T*4 + "int nElapsed = (int)(now - slot.uDisplayStartTime);",
    T*4 + "int nY = RiseFromY() - (nElapsed > 0 ? nElapsed * m_nRiseSpeed / 1000 : 0);",
    T*4 + "if (nElapsed >= (int)m_uDisplayDuration || nY < m_nAbsoluteTop)",
    T*4 + "{",
    T*5 + "if (slot.pSourceNode)",
    T*5 + "{",
    T*6 + "if (slot.pSourceNode->nType == NEWSMESSAGE_NORMAL)",
    T*7 + "free(slot.pSourceNode);",
    T*6 + "else",
    T*7 + "AddToTail(slot.pSourceNode);",
    T*5 + "}",
    T*5 + "slot.bActive = false;",
    T*5 + "slot.pSourceNode = NULL;",
    T*5 + "continue;",
    T*4 + "}",
    T*3 + "}")
s = rep(s, old, new)

# helper mo mau
old = T + "return nUnits * nFontSize / 2;" + NL + "}" + NL
new = old + L(
    "",
    "// [TKCHAT 04/09c] giam do sang mau (0xAARRGGBB cua GetColor) con nPct % - giu byte alpha; chu khong co alpha nen 'mo dan' = toi dan",
    "static unsigned int sTkChatMoMau(unsigned int uColor, int nPct)",
    "{",
    T + "unsigned int r = ((uColor >> 16) & 0xFF) * nPct / 100;",
    T + "unsigned int g = ((uColor >> 8) & 0xFF) * nPct / 100;",
    T + "unsigned int b = (uColor & 0xFF) * nPct / 100;",
    T + "return (uColor & 0xFF000000) | (r << 16) | (g << 8) | b;",
    "}")
s = rep(s, old, new)

# PaintWindow: thay tu "int aIdx[64];" den het ham
a, a_anchor = find_nl(s, T + "int aIdx[64];" + NL)
b_anchor = L(
    T*2 + "g_pRepresentShell->OutputRichText(m_nFontSize, &Param, szTemp, nLen, 0);",
    T + "}",
    "}")
b, b_anchor = find_nl(s, b_anchor, a)
b += len(b_anchor)
USE_CRLF = ("\r" in a_anchor)
new_body = L(
    T + "// [TKCHAT 04/09c] kieu 2.0: moi dong sinh o RiseFromY() (giua man hinh), troi len RiseSpeed px/giay, toi dan FadeMs cuoi,",
    T + "// het DisplayDuration hoac cham mep tren thi bien mat. Thu tu tren-duoi tu nhien theo thoi diem sinh (cu tren, moi duoi).",
    T + "unsigned int uNow = IR_GetCurrentTime();",
    T + "int nFrom = RiseFromY();",
    T + "for (int i = 0; i < m_nNumSlots; ++i)",
    T + "{",
    T*2 + "DisplaySlot& slot = m_DisplaySlots[i];",
    T*2 + "if (!slot.bActive || slot.CurrentMsg.nMsgLen <= 0)",
    T*3 + "continue;",
    T*2 + "int nElapsed = (int)(uNow - slot.uDisplayStartTime);",
    T*2 + "if (nElapsed < 0)",
    T*3 + "continue;" + T + "// chua toi luot sinh (xep hang cach dong truoc dung 1 hang)",
    T*2 + "if (nElapsed >= (int)m_uDisplayDuration)",
    T*3 + "continue;",
    T*2 + "int nY = nFrom - nElapsed * m_nRiseSpeed / 1000;",
    T*2 + "if (nY < m_nAbsoluteTop)",
    T*3 + "continue;",
    T*2 + "char szTemp[512];",
    T*2 + "int nCopy = slot.CurrentMsg.nMsgLen;",
    T*2 + "if (nCopy > (int)sizeof(szTemp) - 1)",
    T*3 + "nCopy = (int)sizeof(szTemp) - 1;",
    T*2 + "memcpy(szTemp, slot.CurrentMsg.sMsg, nCopy);",
    T*2 + "szTemp[nCopy] = 0;",
    T*2 + "int nLen = TEncodeText(szTemp, nCopy);",
    T*2 + "if (nLen <= 0)",
    T*3 + "continue;",
    T*2 + "int nRong = sTkChatDoRong(szTemp, nLen, m_nFontSize);",
    T*2 + "int nX = m_nAbsoluteLeft + (m_Width - nRong) / 2;",
    T*2 + "if (nX < m_nAbsoluteLeft + m_nIndentH)",
    T*3 + "nX = m_nAbsoluteLeft + m_nIndentH;",
    T*2 + "unsigned int uColor = m_uTextColor;",
    T*2 + "int nRemain = (int)m_uDisplayDuration - nElapsed;",
    T*2 + "if (m_nFadeMs > 0 && nRemain < m_nFadeMs)",
    T*2 + "{",
    T*3 + "int nPct = nRemain * 100 / m_nFadeMs;",
    T*3 + "if (nPct < 12)",
    T*4 + "nPct = 12;",
    T*3 + "uColor = sTkChatMoMau(m_uTextColor, nPct);",
    T*2 + "}",
    T*2 + "Param.Color = uColor;",
    T*2 + "Param.nX = nX;",
    T*2 + "Param.nY = nY;",
    T*2 + "g_pRepresentShell->OutputRichText(m_nFontSize, &Param, szTemp, nLen, 0);",
    T + "}",
    "}")
assert all(ord(c) < 128 for c in new_body)
if USE_CRLF:
    new_body = crlf(new_body)
s = s[:a] + new_body + s[b:]
assert hi(s) == h0, (hi(s), h0)
wr(p, s); print("cpp ok; high bytes", h0)
