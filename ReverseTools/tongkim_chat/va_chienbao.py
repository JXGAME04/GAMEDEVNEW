# -*- coding: ascii -*-
# [TKINFO 06/09] Bang chien bao KUiBattleReport: them nut "Dong" cua rieng no + tu dong khi het tran (kind 9)
# + chan kind 7/8 (cua cua so Thong Tin Tran) truoc strcpy khong kiem tra vao szString[128].
# Chu 06/09: "nhap xem chien bao thi mo ra tat di thi khong duoc".
import io, os, re

D = r"D:\GAMEDEVNEW_wt_mail\Sources\S3Client\Ui\UiCase"
T = "\t"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) >= 128)

def chen_sau(s, pattern, them):
    """Chen 'them' (khong ke thut dau dong) ngay sau dong khop pattern, giu dung kieu xuong dong cua tep."""
    m = re.search(pattern, s)
    assert m, ("khong tim thay neo", pattern)
    dong = m.group(0)
    nl = "\r\n" if dong.endswith("\r\n") else "\n"
    ind = re.match(r"[ \t]*", dong).group(0)
    return s[:m.end()] + "".join(ind + l + nl for l in them) + s[m.end():]

# ---------------------------------------------------------------- .h
p = os.path.join(D, "UiBattleReport.h"); s = rd(p); h0 = hi(s)
assert "m_CloseBtn" not in s, "da va roi"
s = chen_sau(s, r'#include "\.\./Elem/WndButton\.h"[^\r\n]*\r?\n',
             ['#include "../Elem/WndLabeledButton.h"' + T + '// [TKINFO 06/09] nut "Dong"'])
s = chen_sau(s, r'[ \t]*KWndButton[ \t]+m_SwitchBtn;[^\r\n]*\r?\n',
             ['KWndLabeledButton m_CloseBtn;' + T + '// [TKINFO 06/09] nut dong cua rieng bang chien bao (ini [CloseBtn])'])
assert hi(s) == h0
wr(p, s); print("UiBattleReport.h ok")

# ---------------------------------------------------------------- .cpp
p = os.path.join(D, "UiBattleReport.cpp"); s = rd(p); h0 = hi(s)
assert "m_CloseBtn" not in s, "da va roi"

s = chen_sau(s, r'[ \t]*AddChild\(&m_SwitchBtn\);[^\r\n]*\r?\n',
             ['AddChild(&m_CloseBtn);' + T + '// [TKINFO 06/09] phai AddChild TRUOC LoadScheme'])
s = chen_sau(s, r'[ \t]*m_SwitchBtn\.Init\(pIni, "SwitchBtn"\);[^\r\n]*\r?\n',
             ['m_CloseBtn.Init(pIni, "CloseBtn");' + T + '// [TKINFO 06/09] co trong CA HAI ini Big + Small'])

# WndProc: them nhanh dong
old = (T*2 + 'if (uParam == (unsigned int)(KWndWindow*)&m_SwitchBtn)\r\n'
       + T*3 + 'SetMode(s_eBattleMode == BATTLE_M_SMALL ? BATTLE_M_BIG : BATTLE_M_SMALL);\r\n')
if s.count(old) != 1:
    old = old.replace("\r\n", "\n")
assert s.count(old) == 1, ("neo WndProc", s.count(old))
nl = "\r\n" if "\r\n" in old else "\n"
new = old + nl.join([
    T*2 + '// [TKINFO 06/09] nut dong cua rieng bang chien bao. Truoc day cua so nay khong co duong dong nao:',
    T*2 + '// [SwitchBtn] chi doi che do To/Nho, con nhanh VK_ESCAPE ben duoi hau nhu khong bao gio chay vi',
    T*2 + '// KShortcutKeyCentre an phim ESC truoc (Wnds.cpp) - ESC thuc te dong SACH moi cua so qua UiShell.',
    T*2 + 'else if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)',
    T*3 + 'CloseWindow(false);' + T + '// false = chi an, giu du lieu (true se Destroy va bo m_pSelf)',
    ""])
s = s.replace(old, new)

# UpdateRankWorld: kind 9 dong bang, chan kind 7/8 truoc strcpy
old2 = None
for cand in [
    'void KUiBattleReport::UpdateRankWorld(const char* pszWorldRank, BYTE nType)\r\n{\r\n\tif (m_pSelf == NULL)\r\n\t{\r\n\t\treturn;\r\n\t}\r\n',
    'void KUiBattleReport::UpdateRankWorld(const char* pszWorldRank, BYTE nType)\n{\n\tif (m_pSelf == NULL)\n\t{\n\t\treturn;\n\t}\n',
]:
    if s.count(cand) == 1:
        old2 = cand; break
assert old2, "khong tim thay dau ham UpdateRankWorld"
nl2 = "\r\n" if "\r\n" in old2 else "\n"
new2 = old2 + nl2.join([
    T + '// [TKINFO 06/09] kind 9 = het tran / roi tran (script Tong Kim da gui san): dong luon bang chien bao,',
    T + '// truoc day no treo lai tren man hinh cho toi khi nguoi choi bam ESC (ma ESC dong SACH moi cua so).',
    T + 'if (nType == 9)',
    T + '{',
    T*2 + 'CloseWindow(false);',
    T*2 + 'return;',
    T + '}',
    T + '// kind 7/8 la cua cua so Thong Tin Tran (KUiTongKimInfo), khong phai cua bang nay -> ra som,',
    T + '// tranh strcpy KHONG kiem tra do dai o duoi chep chuoi 127 byte vao szString[128] ma khong dung den.',
    T + 'if (pszWorldRank == NULL || nType > 6)',
    T*2 + 'return;',
    ""])
s = s.replace(old2, new2)
assert hi(s) == h0, (hi(s), h0)
wr(p, s); print("UiBattleReport.cpp ok")
print("XONG")
