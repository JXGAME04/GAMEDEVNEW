# -*- coding: utf-8 -*-
#
# [NHOMTREN 12/09] Khung 800x600 cua KUiPlayerBar chua CA HAI phan: bang trang thai goc tren-trai (gio, song mang, hang buff,
# nut an hang icon) va khung chat o DAY. Tep bo cuc neo khung theo GIUA-DUOI (khung chat phai sat day) nen tren man CAO
# (may tinh bang 4:3: 1152x864) ca nhom tren bi day xuong giua man. Sua (chi Android): sau khi khung da duoc neo, keo rieng
# nhom tren len dung do cao nhu ban PC = tru dung khoang dich doc ma bo neo da cong cho khung (UiToaDo_LayDich).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[NHOMTREN 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, nl.join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


# ---- 1. UiToaDo: nho khoang dich cua tung muc + ham tra ve -----------------
T = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
s = doc(T)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tint\t\tnNeoY;\t\t\t// 0 tren, 1 giua, 2 duoi; -1"],
                ["\tint\t\tnNeoY;\t\t\t// 0 tren, 1 giua, 2 duoi; -1",
                 "\tint\t\tnDichX;\t\t\t// %s khoang bo neo da cong cho muc nay (khung ve khac tep thiet ke)" % DAU,
                 "\tint\t\tnDichY;"], "truong dich")
    s = thay(s, ["\t\ts_Bang[i].nLeft += nDX * (s_Bang[i].nNeoX - nNeoXCha) / 2;",
                 "\t\ts_Bang[i].nTop  += nDY * (s_Bang[i].nNeoY - nNeoYCha) / 2;"],
                ["\t\ts_Bang[i].nDichX = nDX * (s_Bang[i].nNeoX - nNeoXCha) / 2;\t// %s" % DAU,
                 "\t\ts_Bang[i].nDichY = nDY * (s_Bang[i].nNeoY - nNeoYCha) / 2;",
                 "\t\ts_Bang[i].nLeft += s_Bang[i].nDichX;",
                 "\t\ts_Bang[i].nTop  += s_Bang[i].nDichY;"], "ghi dich")
    s = thay(s, ["void UiToaDo_DumpThuc()"],
                ["//\t%s Khoang ma bo neo da dich muc nay so voi tep thiet ke (0 neu khong co muc / khong dich)." % DAU,
                 "void UiToaDo_LayDich(const char* pszKhoa, int* pnDX, int* pnDY)",
                 "{",
                 "\tint nMuc = TimKhoa(pszKhoa);",
                 "\tif (pnDX)",
                 "\t\t*pnDX = (nMuc >= 0) ? s_Bang[nMuc].nDichX : 0;",
                 "\tif (pnDY)",
                 "\t\t*pnDY = (nMuc >= 0) ? s_Bang[nMuc].nDichY : 0;",
                 "}",
                 "void UiToaDo_DumpThuc()"], "ham LayDich")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi")
    ghi(T, s); print("da va:", T)

H = "Sources/S3Client/Ui/Elem/UiToaDo.h"
s = doc(H)
if DAU not in s:
    s = thay(s, ["void\tUiToaDo_DumpThuc();\t// [UITOADO 12/09 NEO d] nhat ky vi tri thuc moi cua so (mot lan)"],
                ["void\tUiToaDo_DumpThuc();\t// [UITOADO 12/09 NEO d] nhat ky vi tri thuc moi cua so (mot lan)",
                 "void\tUiToaDo_LayDich(const char* pszKhoa, int* pnDX, int* pnDY);\t// %s khoang bo neo da dich mot muc" % DAU], "khai bao LayDich")
    ghi(H, s); print("da va:", H)

# ---- 2. KUiPlayerBar: keo nhom tren len -----------------------------------
HP = "Sources/S3Client/Ui/UiCase/UiPlayerBar.h"
s = doc(HP)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tvoid\t\t\t\t  ChangeWifiStatus(int nStatus);"],
                ["\tvoid\t\t\t\t  ChangeWifiStatus(int nStatus);",
                 "#ifdef JX_ANDROID",
                 "\tvoid\t\t\t\t  NeoNhomTren();\t// %s keo bang trang thai goc tren-trai len dinh man (man cao)" % DAU,
                 "#endif"], "khai bao NeoNhomTren")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (h)")
    ghi(HP, s); print("da va:", HP)

CP = "Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp"
s = doc(CP)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tWnd_AddWindow(this);",
                 "#ifdef JX_ANDROID",
                 "\tKUiTaskTrace::NeoLaiKhiCoThanh();\t// [TASKTRACE 12/09] khung theo doi nhiem vu mo truoc thanh nay -> neo lai canh nut",
                 "#endif",
                 "}"],
                ["\tWnd_AddWindow(this);",
                 "#ifdef JX_ANDROID",
                 "\tm_pSelf->NeoNhomTren();\t// %s bang trang thai goc tren-trai: giu do cao nhu ban PC tren man cao" % DAU,
                 "\tKUiTaskTrace::NeoLaiKhiCoThanh();\t// [TASKTRACE 12/09] khung theo doi nhiem vu mo truoc thanh nay -> neo lai canh nut",
                 "#endif",
                 "}",
                 "#ifdef JX_ANDROID",
                 "//\t%s Khung 800x600 nay chua ca bang trang thai (goc tren-trai: gio, song mang, hang buff, nut an hang icon)" % DAU,
                 "//\tlan khung chat (o day). Bo cuc neo khung theo GIUA-DUOI cho khung chat sat day man, nen tren man CAO",
                 "//\t(may tinh bang 4:3) bang trang thai bi keo xuong giua man. Tra rieng nhom tren ve dung do cao ban PC:",
                 "//\ttru dung khoang doc ma bo neo da cong cho khung (UiToaDo_LayDich) - khong doan theo kich co man hinh.",
                 "void KUiPlayerBar::NeoNhomTren()",
                 "{",
                 "\tint nDichY = 0, nX = 0, nY = 0, i;",
                 "\tUiToaDo_LayDich(\"KUiPlayerBar|Main\", NULL, &nDichY);",
                 "\tif (nDichY == 0)",
                 "\t\treturn;",
                 "\tKWndWindow* apO[] = { &m_DateTime, &m_WifiStatus, &m_AnIcon };",
                 "\tfor (i = 0; i < (int)(sizeof(apO) / sizeof(apO[0])); i++)",
                 "\t{",
                 "\t\tapO[i]->GetPosition(&nX, &nY);",
                 "\t\tapO[i]->SetPosition(nX, nY - nDichY);",
                 "\t}",
                 "\tfor (i = 0; i < MAX_BUTTON_STATE; i++)",
                 "\t{",
                 "\t\tm_StateImg[i].GetPosition(&nX, &nY);",
                 "\t\tm_StateImg[i].SetPosition(nX, nY - nDichY);",
                 "\t\tm_StateLife[i].GetPosition(&nX, &nY);",
                 "\t\tm_StateLife[i].SetPosition(nX, nY - nDichY);",
                 "\t}",
                 "}",
                 "#endif"], "goi + ham")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (cpp)")
    ghi(CP, s); print("da va:", CP)
print("xong")
