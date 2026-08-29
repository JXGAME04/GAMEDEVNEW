# -*- coding: utf-8 -*-
r"""[BDH 28/08] Icon DONG HANH tren UiPlayerBar, dat NGAY TREN icon bau cua.

- Anh = nut goc ban Linux "\spr\UI3\同伴界面\同伴快捷栏\同伴的基本属性.spr"
  (da rut loose vao client o b02; pak KHONG co icon main-UI rieng - da do 24 ten).
- Bam icon: mo/dong bo giao dien dong hanh (thanh nhanh + cua so thuoc tinh).
- Kem: chep settings\partner\level_exp.txt sang client (attr hien exp/nguong).
Cac moi: UiPlayerBar.ini (data) + UiPlayerBar.h/.cpp (4 cho). Idempotent,
sao luu .truoc_bdh_icon.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes as MA

CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
UI = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase"
BS = chr(92)
CR = chr(13)
LF = chr(10)
T = chr(9)
E = CR + LF


def g(s):
    return s.encode("gbk").decode("latin-1")


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s, bak=".truoc_bdh_icon"):
    if not os.path.exists(p + bak):
        shutil.copyfile(p, p + bak)
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def patch(p, neo, moi, marker):
    s = doc(p)
    if marker in s:
        print("  da co:", os.path.basename(p), marker[:36])
        return
    n = s.count(neo)
    assert n == 1, "anchor %d lan trong %s: %r" % (n, p, neo[:70])
    ghi(p, s.replace(neo, moi, 1))
    print("  VA:", os.path.basename(p), marker[:36])


# ---------- 0. chep level_exp.txt sang client ----------
src = os.path.join(SV, r"settings\partner\level_exp.txt")
dst = os.path.join(CL, r"settings\partner\level_exp.txt")
os.makedirs(os.path.dirname(dst), exist_ok=True)
if not os.path.isfile(dst) or os.path.getsize(dst) != os.path.getsize(src):
    shutil.copyfile(src, dst)
    print("  chep level_exp.txt sang client")
else:
    print("  level_exp.txt client da co")

# ---------- 1. UiPlayerBar.ini (data client) ----------
p = os.path.join(CL, r"Ui\Ui3\UiPlayerBar.ini")
s = doc(p)
if "[PartnerIcon]" not in s:
    anh = (BS + "spr" + BS + "UI3" + BS + g("同伴界面") + BS + g("同伴快捷栏") +
           BS + g("同伴的基本属性") + ".spr")
    tip = MA("Bạn đồng hành (Y)").decode("latin-1")
    sec = (E + "[PartnerIcon]" + E +
           "Left=765" + E +
           "Top=215" + E +
           "Width=22" + E +
           "Height=22" + E +
           "Image=" + anh + E +
           "Trans=0" + E +
           "Up=0" + E +
           "Down=1" + E +
           "Over=0" + E +
           "OverFrame=0" + E +
           "Tip=" + tip + E)
    neo = "[SpringGame]"
    assert s.count(neo) == 1
    i = s.find(neo)
    # chen TRUOC [SpringGame]
    ghi(p, s[:i] + sec.lstrip(CR + LF) + E + s[i:])
    print("  VA: UiPlayerBar.ini [PartnerIcon]")
else:
    print("  da co: UiPlayerBar.ini [PartnerIcon]")

# ---------- 2. UiPlayerBar.h ----------
patch(os.path.join(UI, "UiPlayerBar.h"),
      T + "KWndButton		m_SpringGame;",
      T + "KWndButton		m_SpringGame;" + E +
      T + "KWndButton		m_PartnerIcon;	// [BDH-G4] icon dong hanh (tren icon bau cua)",
      "m_PartnerIcon;")

# ---------- 3. UiPlayerBar.cpp: include + 4 moi ----------
p = os.path.join(UI, "UiPlayerBar.cpp")
patch(p,
      '#include "UiPlayerBar.h"' + E,
      '#include "UiPlayerBar.h"' + E +
      '#include "UiPartnerBar.h"' + T + "// [BDH-G4]" + E +
      '#include "UiPartnerAttr.h"' + E,
      'UiPartnerBar.h"' + T + "// [BDH-G4]")

patch(p,
      T + 'm_SpringGame.Init(pIni, "SpringGame");',
      T + 'm_SpringGame.Init(pIni, "SpringGame");' + E +
      T + 'm_PartnerIcon.Init(pIni, "PartnerIcon");	// [BDH-G4]',
      'm_PartnerIcon.Init(')

patch(p,
      T + T + "m_SpringGame.GetPosition(&nX, &nY);" + E +
      T + T + "m_SpringGame.SetPosition(1024 - 30, nY);",
      T + T + "m_SpringGame.GetPosition(&nX, &nY);" + E +
      T + T + "m_SpringGame.SetPosition(1024 - 30, nY);" + E +
      T + T + "m_PartnerIcon.GetPosition(&nX, &nY);" + T + "// [BDH-G4] neo cung cot phai" + E +
      T + T + "m_PartnerIcon.SetPosition(1024 - 30, nY);",
      "m_PartnerIcon.SetPosition(1024")

patch(p,
      T + "AddChild(&m_SpringGame);",
      T + "AddChild(&m_SpringGame);" + E +
      T + "AddChild(&m_PartnerIcon);	// [BDH-G4]",
      "AddChild(&m_PartnerIcon);")

wnd = (
    T + T + "else if ((KWndWindow*)uParam == (KWndWindow*)&m_PartnerIcon)" + E +
    T + T + "{" + E +
    T + T + T + "// [BDH-G4] mo/dong bo giao dien dong hanh (thanh nhanh + thuoc tinh)" + E +
    T + T + T + "if (KUiPartnerAttr::GetIfVisible() || KUiPartnerBar::GetIfVisible())" + E +
    T + T + T + "{" + E +
    T + T + T + T + "KUiPartnerAttr::CloseWindow();" + E +
    T + T + T + T + "KUiPartnerBar::CloseWindow();" + E +
    T + T + T + "}" + E +
    T + T + T + "else" + E +
    T + T + T + "{" + E +
    T + T + T + T + "KUiPartnerBar::OpenWindow();" + E +
    T + T + T + T + "KUiPartnerAttr::OpenWindow();" + E +
    T + T + T + "}" + E +
    T + T + "}" + E)
patch(p,
      T + T + "else if ((KWndWindow*)uParam == (KWndWindow*)&m_SpringGame)" + E +
      T + T + "{" + E +
      T + T + T + "KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SPRINGGAME);" + E +
      T + T + "}" + E,
      T + T + "else if ((KWndWindow*)uParam == (KWndWindow*)&m_SpringGame)" + E +
      T + T + "{" + E +
      T + T + T + "KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SPRINGGAME);" + E +
      T + T + "}" + E + wnd,
      "(KWndWindow*)&m_PartnerIcon)")

print("XONG p12")
