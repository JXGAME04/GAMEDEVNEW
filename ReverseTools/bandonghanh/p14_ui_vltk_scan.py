# -*- coding: utf-8 -*-
r"""[BDH 28/08] DO bo giao dien DONG HANH ban VIET trong pak CLIENT VLTK.

Client VLTK (ban VNG cai that, Viet hoa chuan - chu game chot lam goc):
    C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data
Quet MOI pak, hash MOI to hop ten (ini + spr, cac biến thể path/hoa-thuong),
in ra bang: ten -> nhung pak nao co (kem size). Pak nap SAU de len truoc
(update__ sau update04__ - memory lo ren 27/08).
Chi DO - chua chep (buoc 2 chon ban thang roi chep).
"""
import os
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P  # noqa: E402

spec = importlib.util.spec_from_file_location(
    "pak_id", r"D:\GAMEDEVNEW\ReverseTools\viemde\pak_id.py")
pak_id = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pak_id)
fid = pak_id.file_name_to_id

SEP = chr(92)
ROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"


def g(s):
    return s.encode("gbk").decode("latin-1")


# ---- danh muc ten can do ----
INIS = ["同伴属性", "同伴技能", "同伴技能树", "同伴背包", "同伴快捷栏"]
SPR_GD = ["标签1", "标签2", "标签3", "标签4", "标签5", "关闭", "解散", "切换",
          "同伴-技能", "同伴-人物", "同伴技能title", "同伴属性title", "选定", "遗忘"]
SPR_QB = ["仅跟随", "同伴的基本属性", "同伴的武功技能", "同伴提示", "同伴选择",
          "与同伴对话", "召唤同伴", "主动攻击", "主界面按钮条改副本", "同伴的物品栏"]
SPR_BB = ["同伴背包界面", "同伴背包界面-关闭"]

CANDS = {}


def them(duong, nhan):
    for case in ("UI3", "Ui3", "ui3"):
        p = duong.replace("{U}", case)
        CANDS[fid(p)] = (nhan, p)


for n in INIS:
    for base in ("ui" + SEP + "{U}" + SEP, "ui" + SEP + "ui3_1024" + SEP, "ui" + SEP,
                 "{U}" + SEP, "ini" + SEP):
        them(SEP + base + g(n) + ".ini", "ini:" + n)
for n in SPR_GD:
    them(SEP + "spr" + SEP + "{U}" + SEP + g("同伴界面") + SEP + g(n) + ".spr", "gd:" + n)
for n in SPR_QB:
    them(SEP + "spr" + SEP + "{U}" + SEP + g("同伴界面") + SEP + g("同伴快捷栏") +
         SEP + g(n) + ".spr", "qb:" + n)
for n in SPR_BB:
    them(SEP + "spr" + SEP + "{U}" + SEP + g("同伴背包") + SEP + g(n) + ".spr", "bb:" + n)

print("tong to hop hash:", len(CANDS))

# ---- quet moi pak ----
paks = sorted(f for f in os.listdir(ROOT) if f.lower().endswith(".pak"))
print("pak trong data:", len(paks), paks[:20])
ket = {}
for fn in paks:
    pk = os.path.join(ROOT, fn)
    try:
        f, es = P.entries(pk)
    except Exception as ex:
        print("  ! khong doc duoc", fn, ex)
        continue
    idx = {e[0]: e for e in es}
    for uid, (nhan, duong) in CANDS.items():
        if uid in idx:
            e = idx[uid]
            ket.setdefault(nhan, []).append((fn, e[2], duong))
    f.close()

print()
print("%-28s | tim thay o" % "ten")
print("-" * 78)
for nhan in sorted(ket):
    ds = ket[nhan]
    print("%-28s | %s" % (nhan.encode("utf-8", "replace").decode("utf-8"),
          ", ".join("%s(%d)" % (fn, sz) for fn, sz, _ in ds)))
thieu = set(n for n, _ in [(v[0], 0) for v in CANDS.values()]) - set(ket)
print()
print("KHONG thay (%d):" % len(thieu), ", ".join(sorted(thieu)) if thieu else "-")
