# -*- coding: utf-8 -*-
# sua loi cu phap do hs_hocvocong.py: khoi tbAllSkill2.huashan thieu dau dong "\t},"
import io, sys
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\global\hocvocong.lua"
d = io.open(p, "r", encoding="latin-1", newline="").read()
nl = "\r\n" if "\r\n" in d else "\n"
old = "\t\t[9] = {1364,1382,1365}," + nl + "}" + nl
assert d.count(old) == 1, d.count(old)
d = d.replace(old, "\t\t[9] = {1364,1382,1365}," + nl + "\t}," + nl + "}" + nl)
io.open(p, "w", encoding="latin-1", newline="").write(d)
print("da dong khoi huashan")
