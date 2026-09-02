# fix3: (1) hs_engine_patch.py: bang FactionName trong KNpc.cpp bi hong ma hoa (UTF-8 lan TCVN3) -> thay CA BANG bang regex, 13 ten TCVN3
#       (2) hs_data1.py: neo spawn Con Lon trong 3 thon -> tim dong chua npcmonphai\conlon.lua bang regex
import io, os
d = os.path.dirname(os.path.abspath(__file__))

p = os.path.join(d, "hs_engine_patch.py")
s = io.open(p, "r", encoding="utf-8").read()
old = '''    old = "\\t\\t\\"C«n L«n\\"\\n\\t};\\n"
    new = ("\\t\\t\\"C«n L«n\\",\\n\\t\\t\\"%s\\",\\t\\t// 10 %s\\n\\t\\t\\"%s\\",\\t\\t// 11\\n\\t\\t\\"%s\\"\\t\\t// 12\\n\\t};\\n"
           % (V("Hoa Sơn"), MARK, V("Vũ Hồn"), V("Tiêu Dao")))
    t.thay(old, new)'''
new = '''    # bang cu bi hong ma hoa (mot so ten la TCVN3 bi UTF-8 hoa boi tool sua) -> viet lai ca bang bang TCVN3 chuan, 13 ten
    m = re.search(r"\\tstatic const char\\* const FactionName\\[\\] = \\{.*?\\r?\\n\\t\\};", t.d, re.S)
    assert m, "KNpc: khong thay bang FactionName"
    ten = ["Thiếu Lâm", "Thiên Vương Bang", "Đường Môn", "Ngũ Độc", "Nga My", "Thúy Yên", "Cái Bang", "Thiên Nhẫn", "Võ Đang", "Côn Lôn", "Hoa Sơn", "Vũ Hồn", "Tiêu Dao"]
    new = ("\\tstatic const char* const FactionName[MAX_FACTION_NUM] = {\\t// %s 13 phai, TCVN3 (bang cu 10 ten bi hong ma hoa)\\n" % MARK
           + "".join("\\t\\t\\"%s\\"%s\\t// %d\\n" % (V(x), "," if i < 12 else "", i) for i, x in enumerate(ten)) + "\\t};")
    t.d = t.d[:m.start()] + t.nx(new) + t.d[m.end():]; t.n += 1'''
assert old in s, "neo KNpc trong engine patch"
s = s.replace(old, new)
io.open(p, "w", encoding="utf-8").write(s)

p = os.path.join(d, "hs_data1.py")
s = io.open(p, "r", encoding="utf-8").read()
old = '''        old = "\\\\\\\\script\\\\\\\\npcthon\\\\\\\\npcmonphai\\\\\\\\conlon.lua\\",6,181) SetNpcValue(nNpcIdx, 9);"
        assert d.count(old) == 1, "%s: neo spawn Con Lon (%d)" % (fn, d.count(old))
        i = d.find(old); j = d.find(nl, i)'''
new = '''        ms = [m for m in re.finditer(r"^[^\\n]*npcmonphai\\\\\\\\conlon\\.lua[^\\n]*$", d, re.M)]
        assert len(ms) >= 1, "%s: khong thay dong spawn Con Lon" % fn
        j = ms[-1].end()
        if d[j-1:j] == "\\r": j -= 1'''
assert old in s, "neo spawn trong hs_data1"
s = s.replace(old, new)
io.open(p, "w", encoding="utf-8").write(s)
print("fix3 ok")
