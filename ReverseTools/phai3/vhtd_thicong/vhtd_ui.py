# -*- coding: utf-8 -*-
"""vhtd_ui.py - UI client cho bang ky nang phai 11 (Vu Hon) / 12 (Tieu Dao) - khuon Hoa Son [HOASON 01/09] -> [VHTD 02/09]:
  ui\\Ui3\\UiSkillNew.ini     [Main11]/[Main111024] (khung_wh.spr), [Main12]/[Main121024] (khung_xy.spr) - chep khoi [Main10]/[Main101024]
  ui\\Ui3\\UiSkillFly.ini     [RemainPoint_11]/[RemainPoint_12] - chep [RemainPoint_10]
  ui\\Ui3\\UiSkillFlySub.ini  [Skill_11_i]/[SkillBtn_11_i], [Skill_12_i]/[SkillBtn_12_i] i = 0..20 - chep [Skill_10_i]/[SkillBtn_10_i] (cung toa do)
  settings\\gamesetting.ini   [WeaponLimit] 7 = Dao Thuan, 8 = Thuan Dao, 103 = Cam (TCVN3)
  Spr\\Ui3\\UiSkills\\khung_wh.spr, khung_xy.spr (khung_vhtd_make.py, PIL)
Doc/ghi latin-1, giu newline goc, ban luu .truoc_vhtd_0209, idempotent. DUNG: python vhtd_ui.py [--kiem]
"""
import io, os, sys, re, shutil, subprocess
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
HERE = os.path.dirname(os.path.abspath(__file__))
KIEM = "--kiem" in sys.argv
BAK = ".truoc_vhtd_0209"
M = "[VHTD 02/09]"
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes, _UNICODE_TO_TCVN3; _TCVN3_TO_UNICODE = {ord(v): k for k, v in _UNICODE_TO_TCVN3.items()}   # bang TCVN3 da kiem chung tren byte that cua JX1 (Hoc vo cong = 48 E4 63 ...)
def V(s): return unicode_to_tcvn3_bytes(s).decode("latin-1")
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    if KIEM: return
    if not os.path.exists(p + BAK): shutil.copy2(p, p + BAK)
    io.open(p, "w", encoding="latin-1", newline="").write(s)

def block(s, name, nl):
    """[name] ... den het dong trong dau tien (bao gom dong trong)"""
    i = s.find("[" + name + "]" + nl)
    if i < 0: raise SystemExit("khong thay khoi [%s]" % name)
    j = s.find(nl + nl, i)
    if j < 0: j = len(s) - len(nl)
    return i, j + 2 * len(nl), s[i:j + 2 * len(nl)]

def ui_skillnew():
    p = os.path.join(CLI, r"ui\Ui3\UiSkillNew.ini"); s = rd(p); nl = "\r\n" if "\r\n" in s else "\n"
    if "[Main11]" in s: print("  [=] UiSkillNew da co [Main11]"); return
    add = ""
    for fac, spr in ((11, "khung_wh.spr"), (12, "khung_xy.spr")):
        for suf in ("", "1024"):
            _, _, b = block(s, "Main10" + suf, nl)
            b2 = b.replace("[Main10" + suf + "]", "[Main%d%s]" % (fac, suf), 1).replace("khung_hs.spr", spr)
            if "khung_hs" in b2 or b2 == b: raise SystemExit("khoi Main10 khong doi duoc")
            add += b2
    _, e, _ = block(s, "Main101024", nl)
    s = s[:e] + add + s[e:]
    wr(p, s); print("  [+] UiSkillNew.ini +[Main11]/[Main111024]/[Main12]/[Main121024]")

def ui_fly():
    p = os.path.join(CLI, r"ui\Ui3\UiSkillFly.ini"); s = rd(p); nl = "\r\n" if "\r\n" in s else "\n"
    if "[RemainPoint_11]" in s: print("  [=] UiSkillFly da co"); return
    _, e, b = block(s, "RemainPoint_10", nl)
    add = b.replace("[RemainPoint_10]", "[RemainPoint_11]", 1) + b.replace("[RemainPoint_10]", "[RemainPoint_12]", 1)
    s = s[:e] + add + s[e:]
    wr(p, s); print("  [+] UiSkillFly.ini +[RemainPoint_11]/[RemainPoint_12]")

def ui_flysub():
    p = os.path.join(CLI, r"ui\Ui3\UiSkillFlySub.ini"); s = rd(p); nl = "\r\n" if "\r\n" in s else "\n"
    if "[Skill_11_0]" in s: print("  [=] UiSkillFlySub da co"); return
    add = ""
    for fac in (11, 12):
        for i in range(0, 21):
            for pre in ("Skill_10_", "SkillBtn_10_"):
                _, _, b = block(s, "%s%d" % (pre, i), nl)
                add += b.replace("[%s%d]" % (pre, i), "[%s%d_%d]" % (pre.replace("_10_", "_"), fac, i), 1)
    _, e, _ = block(s, "SkillBtn_10_30", nl)
    s = s[:e] + add + s[e:]
    wr(p, s); print("  [+] UiSkillFlySub.ini +%d khoi (Skill/SkillBtn 11_0..20, 12_0..20)" % (2 * 21 * 2))

def gamesetting():
    p = os.path.join(CLI, r"settings\gamesetting.ini"); s = rd(p); nl = "\r\n" if "\r\n" in s else "\n"
    i = s.find("[WeaponLimit]")
    if i < 0: raise SystemExit("gamesetting.ini khong co [WeaponLimit]")
    j = s.find(nl + "[", i + 1); sect = s[i:j if j > 0 else len(s)]
    if re.search(r"(?m)^103=", sect): print("  [=] gamesetting [WeaponLimit] da co 103"); return
    m = re.search(r"(?m)^102=.*$", sect)
    if not m: raise SystemExit("[WeaponLimit] khong co 102=")
    pre = V(u"H\u1ea1n ch\u1ebf v\u0169 kh\u00ed: ")
    add = (nl + "7=" + pre + "<color=Green>" + V(u"\u0110ao Thu\u1eabn") + "<color>"
           + nl + "8=" + pre + "<color=Metal>" + V(u"Thu\u1eabn \u0110ao") + "<color>"
           + nl + "103=" + pre + "<color=Purple>" + V(u"C\u1ea7m") + "<color>")
    k = i + m.end()
    s = s[:k] + add + s[k:]
    wr(p, s); print("  [+] gamesetting.ini [WeaponLimit] +7/8/103")

def khung():
    for key, name in (("wh", "khung_wh.spr"), ("xy", "khung_xy.spr")):
        out = os.path.join(CLI, r"Spr\Ui3\UiSkills", name)
        if os.path.exists(out): print("  [=] %s da co (%d byte)" % (name, os.path.getsize(out))); continue
        if KIEM: print("  [kiem] se ve %s" % out); continue
        r = subprocess.run([sys.executable, os.path.join(HERE, "khung_vhtd_make.py"), key, out, os.path.join(HERE, "out", name + ".png")], capture_output=True, text=True)
        print(r.stdout.strip());
        if r.returncode != 0: raise SystemExit(r.stderr)

if __name__ == "__main__":
    os.makedirs(os.path.join(HERE, "out"), exist_ok=True)
    ui_skillnew(); ui_fly(); ui_flysub(); gamesetting(); khung()
    print("XONG%s." % (" (KIEM)" if KIEM else ""))
