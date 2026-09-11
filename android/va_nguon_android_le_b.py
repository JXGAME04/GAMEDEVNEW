# -*- coding: utf-8 -*-
#   [UITOADO 12/09 LE b] Chu van thay icon ben trai sat / lot ra ngoai mep:
#     * nang le mac dinh 10 -> 20 (may cong vien nhu Fold nuot nhieu hon)
#     * keo ca o dang nam AM (x < 0) vao trong, truoc day chi xu ly 0..LE
#     * thong bao "sua giao dien" hien them "le=N" de biet may dang chay ban nao
import io

P = r"D:\GAMEDEVNEW\.claude\worktrees\full-wauto-mobile-analysis-f781df\Sources\S3Client\Ui\Elem\UiToaDo.cpp"
B = chr(92)


def cao(x):
    return sum(1 for c in x if ord(c) >= 0x80)


s = io.open(P, encoding="latin-1", newline="").read()
if "UITOADO 12/09 LE b" in s:
    raise SystemExit("da co")
nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
s0 = s

#   1) le mac dinh 20 + keo ca o am vao
cu = nl.join([
    "\t\tif (s_nLe < 0)",
    '\t\t\ts_nLe = GetPrivateProfileInt("Ui", "LeAnToan", 10, ".' + B + B + 'config.ini");',
    "\t\tpWnd->GetAbsolutePos(&nAbsX, &nAbsY);",
    "\t\tif (s_nLe > 0 && nAbsX >= 0 && nAbsX < s_nLe)",
    "\t\t\tpWnd->SetPosition(s_Bang[nMuc].nLeft + (s_nLe - nAbsX), s_Bang[nMuc].nTop);",
])
moi = nl.join([
    "\t\tif (s_nLe < 0)",
    '\t\t\ts_nLe = GetPrivateProfileInt("Ui", "LeAnToan", 20, ".' + B + B + 'config.ini");',
    "\t\tpWnd->GetAbsolutePos(&nAbsX, &nAbsY);",
    "\t\t//\t[UITOADO 12/09 LE b] keo ca o dang nam AM vao trong (truoc chi xu ly 0..le nen o nao lot han ra",
    "\t\t//\tngoai van nam nguyen); -300 tro ra la co y giau han (Item_4..8, ChatRoom) thi de yen.",
    "\t\tif (s_nLe > 0 && nAbsX > -100 && nAbsX < s_nLe)",
    "\t\t\tpWnd->SetPosition(s_Bang[nMuc].nLeft + (s_nLe - nAbsX), s_Bang[nMuc].nTop);",
])
if s.count(cu) != 1:
    raise SystemExit("khong thay khoi le (%d)" % s.count(cu))
s = s.replace(cu, moi)

#   2) thong bao kem le
cu2 = '\t\t\t_snprintf(szTB, sizeof(szTB), "%s  [%dx%d]", "'
i = s.find(cu2)
if i < 0:
    raise SystemExit("khong thay dong thong bao")
j = s.index(nl, i)
dong = s[i:j]
chuoi = dong.split('", "', 1)[1].rsplit('",', 1)[0]
moi2 = ('\t\t\t_snprintf(szTB, sizeof(szTB), "%s  [%dx%d le=%d]", "' + chuoi + '", SCREEN_WIDTH, SCREEN_HEIGHT,'
        + nl + '\t\t\t\tGetPrivateProfileInt("Ui", "LeAnToan", 20, ".' + B + B + 'config.ini"));')
s = s[:i] + moi2 + s[j:]

if cao(s) != cao(s0):
    raise SystemExit("byte cao doi: %d -> %d" % (cao(s0), cao(s)))
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va le b")
