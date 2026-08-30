# -*- coding: utf-8 -*-
"""Trich danh sach 128 khoa NEN tu workflow phan loai ra tep de dung sau."""
import io
import json
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
J = (r"C:\Users\nguye\.claude\projects\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
     r"\61d924bb-ce66-46e1-a22b-820305b4bde7\subagents\workflows"
     r"\wf_3d3434ab-5a0\journal.jsonl")
RA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "khoa_nen.json")
RA_TXT = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                      "khoa_nen.txt")

tat = []
for l in io.open(J, encoding="utf-8"):
    try:
        o = json.loads(l)
    except Exception:
        continue
    if o.get("type") != "result":
        continue
    r = o.get("result")
    if not isinstance(r, dict) or "ket_qua" not in r:
        continue
    tat.extend(r["ket_qua"])

nen = [x for x in tat
       if x.get("loai") == "NEN" and x.get("dang_chay") is not False]
chet = [x for x in tat if x.get("loai") == "CHET"]
khong = [x for x in tat if x.get("loai") == "KHONG"]

print("tong phan loai : %d" % len(tat))
print("  NEN (dang chay): %d" % len(nen))
print("  CHET           : %d" % len(chet))
print("  KHONG          : %d" % len(khong))

theo_tep = {}
for x in nen:
    theo_tep.setdefault(x["tep"], []).append(x)

with io.open(RA, "w", encoding="utf-8", newline="") as f:
    json.dump({"nen": nen, "chet": chet}, f, ensure_ascii=False, indent=1)

with io.open(RA_TXT, "w", encoding="utf-8", newline="") as f:
    f.write("KHOA NEN DUA RA CAU HINH (%d khoa o %d tep)\n"
            % (len(nen), len(theo_tep)))
    f.write("Sinh tu workflow phan loai - da xac minh tep va so dong\n")
    f.write("=" * 78 + "\n\n")
    for tep, hs in sorted(theo_tep.items(), key=lambda x: -len(x[1])):
        f.write("# %s  (%d)\n" % (tep, len(hs)))
        for x in sorted(hs, key=lambda y: y.get("dong", 0)):
            f.write('        (":%d", "%s", "%s",\n'
                    % (x.get("dong", 0), x.get("ten", ""),
                       x.get("ten_khoa", "")))
            f.write('         "%s"),\n' % (x.get("mo_ta", "")
                                           .replace('"', "'")[:150]))
        f.write("\n")
    if chet:
        f.write("\n\n## HANG SO CHET (khong noi nao doc) - %d\n" % len(chet))
        for x in chet:
            f.write("    %s:%s  %s\n" % (x.get("tep"), x.get("dong"),
                                         x.get("ten")))

print()
print("=> %s" % RA_TXT)
print()
print("Top tep co nhieu khoa NEN nhat:")
for tep, hs in sorted(theo_tep.items(), key=lambda x: -len(x[1]))[:12]:
    print("  %-52s %2d" % (tep[:52], len(hs)))
