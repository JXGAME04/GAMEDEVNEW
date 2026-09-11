# -*- coding: utf-8 -*-
#   [UITOADO 12/09 LE] Le an toan mep trai luc CHAY (an ca khi nguoi choi da tu keo o do).
import io

P = r"D:\GAMEDEVNEW\.claude\worktrees\full-wauto-mobile-analysis-f781df\Sources\S3Client\Ui\Elem\UiToaDo.cpp"
B = chr(92)


def cao(x):
    return sum(1 for c in x if ord(c) >= 0x80)


s = io.open(P, encoding="latin-1", newline="").read()
if "UITOADO 12/09 LE" in s:
    raise SystemExit("da co")
nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
s0 = s
cu = "\tpWnd->SetPosition(s_Bang[nMuc].nLeft, s_Bang[nMuc].nTop);"
moi = nl.join([
    "\tpWnd->SetPosition(s_Bang[nMuc].nLeft, s_Bang[nMuc].nTop);",
    "#ifdef JX_ANDROID",
    "\t{",
    "\t\t//\t[UITOADO 12/09 LE] Chua LE AN TOAN o mep TRAI. Man dien thoai bo goc tron / vien cong nen cai gi nam",
    "\t\t//\tdung x = 0 la bi nuot mat mot dai - chu bao o man ngoai Fold 7 (khung ve 1440x616), do duoc: mui ten",
    '\t\t//\tcuon bang tin he thong (KSysMsgCentrePad|SysRoom) va tab "<<" gap khung chat (KUiPlayerBar|HideChat)',
    "\t\t//\tdeu nam dung x = 0. Day theo TOA DO THAT nen an ca khi nguoi choi da tu keo o do (tep",
    "\t\t//\tUserData" + B + B + "UiToaDo.ini de len bo cuc mac dinh). Cha duoc ap TRUOC con nen day cha xong la con di theo,",
    "\t\t//\tkhong day hai lan. Tat bang [Ui] LeAnToan=0.",
    "\t\tstatic int s_nLe = -1;",
    "\t\tint nAbsX = 0, nAbsY = 0;",
    "",
    "\t\tif (s_nLe < 0)",
    '\t\t\ts_nLe = GetPrivateProfileInt("Ui", "LeAnToan", 10, ".' + B + B + 'config.ini");',
    "\t\tpWnd->GetAbsolutePos(&nAbsX, &nAbsY);",
    "\t\tif (s_nLe > 0 && nAbsX >= 0 && nAbsX < s_nLe)",
    "\t\t\tpWnd->SetPosition(s_Bang[nMuc].nLeft + (s_nLe - nAbsX), s_Bang[nMuc].nTop);",
    "\t}",
    "#endif",
])
if s.count(cu) != 1:
    raise SystemExit("khong thay SetPosition (%d)" % s.count(cu))
s = s.replace(cu, moi)
if cao(s) != cao(s0):
    raise SystemExit("byte cao doi")
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va le an toan luc chay")
