# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 ONGMAU] Chu: "tinh nang nay ban tham khao USVOLAM co san - van mat ong mau mau do".
#
# DUNG: USVOLAM khong ve ong mau bang toa do cung trong Core. UiNpcBar.cpp cua ho dung mot O CON that:
#     KWndImageTextButton m_Life;  ...  m_Life.Init(&Ini, "Life");  ...  m_Life.Set2IntValue(nPerLife, 100);
# O con thi tu di theo cua so khi doi cho / thu nho, va tu ve ANH thanh mau - khong bao gio lech.
# Ban nay CO SAN KWndImageTextButton::Set2IntValue (WndButton.cpp:486) va mau khai bao ini san trong
# ui\Ui3\uiheadercontrolbar.ini ([Life] + [Life_Image] + [Life_Text], Part=1).
#
# Cach cu (va 84/85) ve ong mau trong KNpc::PaintTargetInfo o toa do CUNG 420,65 roi tu nhan ti le -> luon co the lech,
# va tren Android thi mat han mau do. Nay:
#   - Them o con m_Life vao KUiTargetInfo (chi JX_ANDROID), Init tu muc [Life] moi trong kuitargetinfo.ini.
#   - UpdateData: co muc tieu -> Set2IntValue(% mau, 100) + Show; khong co -> Hide.
#   - PaintWindow tren Android KHONG goi GOI_DRAW_TARGET_INFO nua (bo han ong mau ve tay cua Core).
#   - Tt_ThuNhoO thu nho DE QUY ca cay con -> ong mau va anh ben trong no cung thu theo [Cham] ThongTinTiLe.
# Windows: khong doi (moi thu rao JX_ANDROID; KNpc::PaintTargetInfo giu nguyen cho ban PC).

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 ONGMAU]"
LIVE = "D:/jx1_android_data"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, ham, kiem_byte=True):
    s = doc(p)
    if DAU in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if kiem_byte and cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


# ---------------------------------------------------------------- UiTargetInfo.h
def va_h(s):
    return thay(s, ["\tKWndText32\t\tm_pLifePercent;", "\tKWndText32\t\tm_pTargetName;"],
                ["\tKWndText32\t\tm_pLifePercent;", "\tKWndText32\t\tm_pTargetName;",
                 "#ifdef JX_ANDROID",
                 "\tKWndImageTextButton\tm_Life;\t\t// %s ong mau la O CON that (nhu UiNpcBar cua USVOLAM) -> tu di theo cua so, khong lech" % DAU,
                 "#endif"], "UiTargetInfo.h: m_Life")


# ---------------------------------------------------------------- UiTargetInfo.cpp
def va_cpp(s):
    # 1. gan vao cua so
    s = thay(s, ["\tAddChild(&m_pLifePercent);", "\tAddChild(&m_pTargetName);"],
             ["\tAddChild(&m_pLifePercent);", "\tAddChild(&m_pTargetName);",
              "#ifdef JX_ANDROID",
              "\tAddChild(&m_Life);\t// %s" % DAU,
              "#endif"], "Initialize: AddChild")
    # 2. thu nho DE QUY ca cay con
    s = thay(s, ["\tpWnd->SetPosition(nL * nTL / 100, nT * nTL / 100);",
                 "\tnW = nW * nTL / 100;",
                 "\tnH = nH * nTL / 100;",
                 "\tif (nCaoToiThieu > 0 && nH < nCaoToiThieu)",
                 "\t\tnH = nCaoToiThieu;",
                 "\tpWnd->SetSize(nW, nH);",
                 "}"],
                ["\tpWnd->SetPosition(nL * nTL / 100, nT * nTL / 100);",
                 "\tnW = nW * nTL / 100;",
                 "\tnH = nH * nTL / 100;",
                 "\tif (nCaoToiThieu > 0 && nH < nCaoToiThieu)",
                 "\t\tnH = nCaoToiThieu;",
                 "\tpWnd->SetSize(nW, nH);",
                 "\t// %s thu ca CAY CON (o con cua m_Life la anh thanh mau) - khong thu thi anh tran ra ngoai" % DAU,
                 "\tfor (KWndWindow* pCon = pWnd->GetFirstChild(); pCon; pCon = pCon->GetNextWnd())",
                 "\t\tTt_ThuNhoO(pCon, 0);",
                 "}"], "Tt_ThuNhoO de quy")
    # 3. Init o con moi + thu nho no
    s = thay(s, ["\t\tm_pSelf->m_pTargetName.Init(&Ini, \"Name\");",
                 "#ifdef JX_ANDROID"],
                ["\t\tm_pSelf->m_pTargetName.Init(&Ini, \"Name\");",
                 "#ifdef JX_ANDROID",
                 "\t\tm_pSelf->m_Life.Init(&Ini, \"Life\");\t// %s muc [Life] moi trong kuitargetinfo.ini" % DAU,
                 "\t\tm_pSelf->m_Life.Hide();"], "LoadScheme: Init m_Life")
    s = thay(s, ["\t\tTt_ThuNhoO(&m_pSelf->m_pTargetName, 12);"],
             ["\t\tTt_ThuNhoO(&m_pSelf->m_pTargetName, 12);",
              "\t\tTt_ThuNhoO(&m_pSelf->m_Life, 0);\t// %s" % DAU], "LoadScheme: thu nho m_Life")
    # 4. UpdateData: an / hien + dat gia tri
    s = thay(s, ["\tm_pLifePercent.Hide();", "\tm_pTargetName.Hide();"],
             ["\tm_pLifePercent.Hide();", "\tm_pTargetName.Hide();",
              "#ifdef JX_ANDROID",
              "\tm_Life.Hide();\t// %s" % DAU,
              "#endif"], "UpdateData: Hide")
    s = thay(s, ["\t\t\tm_pTargetName.SetText(m_pPlayersList[0].Name); m_pTargetName.Show();"],
             ["\t\t\tm_pTargetName.SetText(m_pPlayersList[0].Name); m_pTargetName.Show();",
              "#ifdef JX_ANDROID",
              "\t\t\t// " + DAU + " ong mau: o con tu ve anh theo phan tram mau (nhu UiNpcBar cua USVOLAM)",
              "\t\t\tm_Life.Set2IntValue(m_Info.nLifePercent, 100);",
              "\t\t\tm_Life.Show();",
              "#endif"], "UpdateData: Show m_Life")
    # 5. PaintWindow: Android khong nho Core ve ong mau nua
    s = thay(s, ["\tif (g_pRepresentShell == NULL)", "\t\treturn;"],
             ["\tif (g_pRepresentShell == NULL)", "\t\treturn;",
              "#ifdef JX_ANDROID",
              "\treturn;\t// %s ong mau da la o con m_Life; khong nho Core ve o toa do cung nua (ban PC giu nguyen)" % DAU,
              "#endif"], "PaintWindow: Android khong goi Core")
    return s


# ---------------------------------------------------------------- ini: them muc [Life]
def them_muc_life(p_ini, p_mau):
    s = doc(p_ini)
    if "[Life]" in s:
        print("ini da co [Life], bo qua:", p_ini)
        return
    nl = nl_cua(s)
    # lay DUNG byte dong Image= cua thanh mau nguoi choi trong uiheadercontrolbar.ini (ten thu muc/tep la GBK)
    mau = doc(p_mau)
    nlm = nl_cua(mau)
    dong = mau.split(nlm)
    try:
        i = dong.index("[Life_Image]")
    except ValueError:
        raise SystemExit("khong thay [Life_Image] trong " + p_mau)
    anh = ""
    for d in dong[i + 1:i + 12]:
        if d.startswith("Image="):
            anh = d
            break
    if not anh:
        raise SystemExit("khong thay dong Image= cua [Life_Image] trong " + p_mau)
    them = [
        "",
        "; %s ONG MAU cua muc tieu - o CON that (nhu KWndImageTextButton m_Life cua UiNpcBar/USVOLAM)," % DAU,
        "; nen tu di theo cua so khi doi cho va thu nho, khong ve bang toa do cung trong Core nua.",
        "; Toa do goc (chua thu nho): dung dung cho ong mau cu cua Core (x 420-370=50, y 55, rong 120, cao 10).",
        "; Co thuc te = so nay x [Cham] ThongTinTiLe %.",
        "[Life]",
        "Left=50",
        "Top=53",
        "Width=120",
        "Height=12",
        "Part=1",
        "",
        "[Life_Image]",
        "Left=0",
        "Top=0",
        "Width=120",
        "Height=10",
        "Trans=0",
        anh,
        "PartType=0",
        "",
        "[Life_Text]",
        "Left=0",
        "Top=0",
        "Width=120",
        "Height=1",
        "Font=12",
        "",
    ]
    ghi(p_ini, s.rstrip("\r\n") + nl + nl.join(them) + nl)
    print("da them [Life] vao:", p_ini)


va("Sources/S3Client/Ui/UiCase/UiTargetInfo.h", va_h)
va("Sources/S3Client/Ui/UiCase/UiTargetInfo.cpp", va_cpp)

MAU = LIVE + "/ui/Ui3/uiheadercontrolbar.ini"
INI_SONG = LIVE + "/ui/Ui3/kuitargetinfo.ini"
INI_GHIDE = "android/du_lieu_ghi_de/ui/ui3/kuitargetinfo.ini"
them_muc_life(INI_SONG, MAU)
# lop ghi de: chep nguyen ban vua sua (chuan_bi_du_lieu.ps1 chay lai se khong mat)
if not os.path.isdir(os.path.dirname(INI_GHIDE)):
    os.makedirs(os.path.dirname(INI_GHIDE))
ghi(INI_GHIDE, doc(INI_SONG))
print("da chep sang lop ghi de:", INI_GHIDE)
print("xong")
