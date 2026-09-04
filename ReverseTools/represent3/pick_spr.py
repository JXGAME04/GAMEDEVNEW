# -*- coding: utf-8 -*-
import re, random, os
here = os.path.dirname(os.path.abspath(__file__))
L = [l.strip() for l in open(os.path.join(here, "spr_all.txt"), encoding="latin-1") if l.strip()]
cats = {
    "npc_enemy": r"npcres.enemy", "npc_animal": r"npcres.animal", "player": r"npcres.player|.player.",
    "horse": r"horse", "magic": r".magic|.effect|.skill", "ui": r".ui3?.", "item": r".item.", "other": r".",
}
picked = []; used = set()
random.seed(7)
for name, pat in cats.items():
    c = [l for l in L if re.search(pat, l, re.I) and l.lower() not in used]
    random.shuffle(c)
    for l in c[: (6 if name != "other" else 4)]:
        picked.append(l); used.add(l.lower())
open(os.path.join(here, "spr_samples.txt"), "w", encoding="latin-1").write("\n".join(picked) + "\n")
print(len(picked)); print("\n".join(picked))
