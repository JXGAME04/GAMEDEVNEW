# sua hs_data1.py: (1) neo SKILL150 hocvocong ket thuc bang '}' khong ';'; (2) npcs(): anh xa cot theo TEN (Linux 103 -> JX1 87)
#                   (3) kiem mode: chay tat ca ham, in loi tung ham
import io, os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "hs_data1.py")
s = io.open(p, "r", encoding="utf-8").read()

old = '''    old = "\\t\\t{\\"" + V("Thiên Lôi Chấn Nhạc") + "\\",1081,1}," + nl + "\\t}" + nl + "};"
    assert d.count(old) == 1, "hocvocong: neo SKILL150 cuoi (%d)" % d.count(old)
    new = ("\\t\\t{\\"" + V("Thiên Lôi Chấn Nhạc") + "\\",1081,1}," + nl + "\\t}," + nl + "\\t[11]={" + nl +
           "\\t\\t{\\"" + V("Cửu Kiếm Hợp Nhất") + "\\",1369,1}," + nl + "\\t\\t{\\"" + V("Thần Quang Toàn Nhiễu") + "\\",1384,1}" + nl + "\\t}" + nl + "};")'''
new = '''    old = "\\t\\t{\\"" + V("Thiên Lôi Chấn Nhạc") + "\\",1081,1}," + nl + "\\t}" + nl + "}"
    assert d.count(old) == 1, "hocvocong: neo SKILL150 cuoi (%d)" % d.count(old)
    new = ("\\t\\t{\\"" + V("Thiên Lôi Chấn Nhạc") + "\\",1081,1}," + nl + "\\t}," + nl + "\\t[11]={" + nl +
           "\\t\\t{\\"" + V("Cửu Kiếm Hợp Nhất") + "\\",1369,1}," + nl + "\\t\\t{\\"" + V("Thần Quang Toàn Nhiễu") + "\\",1384,1}" + nl + "\\t}" + nl + "}")'''
assert old in s, "neo hocvocong"
s = s.replace(old, new)

old = '''def npcs():
    lin = rd(os.path.join(LIN, "settings", "npcs.txt")).replace("\\r\\n", "\\n").split("\\n")
    for p in (os.path.join(SRV, "settings", "npcs.txt"), os.path.join(CLI, "settings", "npcs.txt")):
        d = rd(p); nl = nl_of(d); lines = d.split(nl)
        n = 0
        for i in HS_NPC_IDS:
            li = i + 1
            cur = lines[li].split("\\t")[0].strip()
            src = lin[li].rstrip("\\r")
            assert src.split("\\t")[0].strip() not in ("", "0"), "Linux npcs dong %d rong" % li
            if cur in ("", "0"):
                lines[li] = src; n += 1
            elif lines[li] != src:
                print("   CANH BAO: dong %d JX1 da co '%s' - giu nguyen" % (li, cur[:30]))
        print("  npcs.txt %s: dien %d dong" % (p, n))
        wr(p, nl.join(lines))'''
new = '''def npcs():
    lin = rd(os.path.join(LIN, "settings", "npcs.txt")).replace("\\r\\n", "\\n").split("\\n")
    hl = lin[0].split("\\t")
    for p in (os.path.join(SRV, "settings", "npcs.txt"), os.path.join(CLI, "settings", "npcs.txt")):
        d = rd(p); nl = nl_of(d); lines = d.split(nl)
        hj = lines[0].split("\\t")
        assert all(h in hl for h in hj), "npcs: cot JX1 khong co o Linux: %r" % [h for h in hj if h not in hl]
        idx = [hl.index(h) for h in hj]   # anh xa theo TEN cot (Linux 103 cot -> JX1 87 cot)
        n = 0
        for i in HS_NPC_IDS:
            li = i + 1
            cur = lines[li].split("\\t")[0].strip()
            lc = lin[li].split("\\t")
            assert lc[0].strip() not in ("", "0"), "Linux npcs dong %d rong" % li
            src = "\\t".join(lc[k] if k < len(lc) else "" for k in idx)
            if cur in ("", "0"):
                lines[li] = src; n += 1
            elif lines[li] != src:
                print("   CANH BAO: dong %d JX1 da co '%s' - giu nguyen" % (li, cur[:30]))
        print("  npcs.txt %s: dien %d dong (%d cot)" % (p, n, len(hj)))
        wr(p, nl.join(lines))'''
assert old in s, "neo npcs"
s = s.replace(old, new)

old = '''    for f in (faction_ini, skills_txt, rank, npcs, worldset, huashan_lua, skills_table, factionhead, hocvocong, lib_faction, skillbook, lenhbai, npc_monphai, ui_ini):
        print("==", f.__name__)
        f()'''
new = '''    loi = []
    for f in (faction_ini, skills_txt, rank, npcs, worldset, huashan_lua, skills_table, factionhead, hocvocong, lib_faction, skillbook, lenhbai, npc_monphai, ui_ini):
        print("==", f.__name__)
        if KIEM:
            try: f()
            except Exception as e:
                print("   LOI:", e); loi.append(f.__name__)
        else:
            f()
    if loi: print("HAM LOI:", loi)'''
assert old in s, "neo main"
s = s.replace(old, new)
io.open(p, "w", encoding="utf-8").write(s)
print("fix2 ok")
