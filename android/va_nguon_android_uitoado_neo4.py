# -*- coding: utf-8 -*-
#
# [UITOADO 12/09 NEO b] Chu 08:30 (anh dien thoai): icon chien lenh mat, nut thu / dau gia lech. Goc: o CON (toa do tuong doi cha)
# khong co neo trong tep thi DoiCaTepTheoNeo tu suy theo toa do nho -> "trai/tren" -> dich = DX*(0 - neo cha)/2 = -331 so voi cha
# (cha neo phai) -> nut bay ra ngoai khung. Sua: o con chua co neo -> THUA neo cua cha ('<lop>|Main' trong bang); chi o khong co cha
# moi tu suy theo toa do. Chi JX_ANDROID.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[UITOADO 12/09 NEO b]"
TEP = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"


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


s = doc(TEP)
if DAU in s:
    raise SystemExit("da va roi")
s = thay(s, ["\t// buoc 1: suy neo cho o chua co (theo toa do thiet ke)",
             "\tfor (i = 0; i < s_nSo; i++)",
             "\t{",
             "\t\tif (s_aTepNap[i] != s_nTepNap)",
             "\t\t\tcontinue;",
             "\t\tif (s_Bang[i].nNeoX < 0 || s_Bang[i].nNeoX > 2)",
             "\t\t\ts_Bang[i].nNeoX = NeoTuDong(s_Bang[i].nLeft, nW0);",
             "\t\tif (s_Bang[i].nNeoY < 0 || s_Bang[i].nNeoY > 2)",
             "\t\t\ts_Bang[i].nNeoY = NeoTuDong(s_Bang[i].nTop, nH0);",
             "\t}"],
            ["\t// buoc 1: suy neo cho o chua co: o CON (toa do tuong doi cha, cha '<lop>|Main' co trong bang) THUA neo cua cha",
             "\t// (%s - tu suy theo toa do nho thanh 'trai' -> dich -DX so voi cha, nut bay ra ngoai khung); o goc suy theo toa do." % DAU,
             "\tfor (i = 0; i < s_nSo; i++)",
             "\t{",
             "\t\tint nCha = -1;",
             "\t\tconst char* pGach;",
             "\t\tif (s_aTepNap[i] != s_nTepNap)",
             "\t\t\tcontinue;",
             "\t\tif (s_Bang[i].nNeoX >= 0 && s_Bang[i].nNeoX <= 2 && s_Bang[i].nNeoY >= 0 && s_Bang[i].nNeoY <= 2)",
             "\t\t\tcontinue;",
             "\t\tpGach = strchr(s_Bang[i].szKhoa, '|');",
             "\t\tif (pGach && strcmp(pGach + 1, \"Main\") != 0)",
             "\t\t{",
             "\t\t\tchar szCha[UITOADO_CO_KHOA];",
             "\t\t\t_snprintf(szCha, sizeof(szCha), \"%.*s|Main\", (int)(pGach - s_Bang[i].szKhoa), s_Bang[i].szKhoa);",
             "\t\t\tszCha[sizeof(szCha) - 1] = 0;",
             "\t\t\tnCha = TimKhoa(szCha);",
             "\t\t\tif (nCha == i)",
             "\t\t\t\tnCha = -1;",
             "\t\t}",
             "\t\tif (s_Bang[i].nNeoX < 0 || s_Bang[i].nNeoX > 2)",
             "\t\t\ts_Bang[i].nNeoX = (nCha >= 0) ? ((s_Bang[nCha].nNeoX >= 0) ? s_Bang[nCha].nNeoX : NeoTuDong(s_Bang[nCha].nLeft, nW0))",
             "\t\t\t\t: NeoTuDong(s_Bang[i].nLeft, nW0);",
             "\t\tif (s_Bang[i].nNeoY < 0 || s_Bang[i].nNeoY > 2)",
             "\t\t\ts_Bang[i].nNeoY = (nCha >= 0) ? ((s_Bang[nCha].nNeoY >= 0) ? s_Bang[nCha].nNeoY : NeoTuDong(s_Bang[nCha].nTop, nH0))",
             "\t\t\t\t: NeoTuDong(s_Bang[i].nTop, nH0);",
             "\t}"], "buoc 1")
# 2. Ap NGAY khi o doc xong ini (KWndWindow::Init): con cua thanh duoi (KUiPlayerBar) duoc Init trong LoadScheme SAU khi cua so goc
#    dang ky (Wnd_AddWindow trong Initialize) -> ApChoCuaSo luc dang ky chua thay ten muc -> muc Item_0..3, ChannelBtn... khong ap.
s = thay(s, ["void UiToaDo_ApChoCuaSo(KWndWindow* pCuaSoGoc)"],
            ["#ifdef JX_ANDROID",
             "// %s ap cho MOT o ngay khi no doc xong ini (goi tu KWndWindow::Init): o con duoc Init sau khi goc da dang ky" % DAU,
             "void UiToaDo_ApChoO(KWndWindow* pWnd)",
             "{",
             "\tchar szKhoa[UITOADO_CO_KHOA];",
             "\tif (pWnd == NULL || s_nSo == 0)",
             "\t\treturn;",
             "\tif (TaoKhoaTuOCon(pWnd, szKhoa, sizeof(szKhoa)))",
             "\t\tApMotO(pWnd, TimKhoa(szKhoa));",
             "}",
             "#endif",
             "void UiToaDo_ApChoCuaSo(KWndWindow* pCuaSoGoc)"], "ApChoO")
ghi(TEP, s)
print("da va:", TEP)

H = "Sources/S3Client/Ui/Elem/UiToaDo.h"
sh = doc(H)
if DAU not in sh:
    sh = thay(sh, ["void\tUiToaDo_ApChoCuaSo(KWndWindow* pCuaSoGoc);"],
                  ["void\tUiToaDo_ApChoCuaSo(KWndWindow* pCuaSoGoc);",
                   "#ifdef JX_ANDROID",
                   "void\tUiToaDo_ApChoO(KWndWindow* pWnd);\t// %s ap cho mot o ngay khi doc xong ini" % DAU,
                   "#endif"], "UiToaDo.h")
    ghi(H, sh)
    print("da va:", H)

W = "Sources/S3Client/Ui/Elem/WndWindow.cpp"
sw = doc(W)
if DAU not in sw:
    sw = thay(sw, ['#include "Wnds.h"'],
                  ['#include "Wnds.h"',
                   "#ifdef JX_ANDROID",
                   "void UiToaDo_ApChoO(KWndWindow* pWnd);\t// UiToaDo.cpp, %s" % DAU,
                   "#endif"], "WndWindow include")
    sw = thay(sw, ["\t\tSetPosition(nValue1, nValue2);\t\t"],
                  ["\t\tSetPosition(nValue1, nValue2);\t\t",
                   "#ifdef JX_ANDROID",
                   "\t\tUiToaDo_ApChoO(this);\t// %s o con Init sau khi goc dang ky -> ap toa do nguoi choi / bo cuc mac dinh ngay" % DAU,
                   "#endif"], "WndWindow Init")
    ghi(W, sw)
    print("da va:", W)
