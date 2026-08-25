# -*- coding: utf-8 -*-
"""B6 - VA CHAN 3: nap trigger o \\settings\\ vao cay script luc boot.

VAN DE: g_IniScriptEngine (KSortScript.cpp:51-66) CHI nap "\\script" va
"\\scriptjx2\\tong_vn". Tep \\settings\\trigger_challengeoftime.lua KHONG nam
trong cay => DynamicExecute -> g_GetScript tra NULL => lich Vuot Ai KHONG CHAY.

CACH VA (khong can build lai DLL): goi ReLoadScript mot lan trong HD3_DriverInit.
KSortScript.cpp:347 ReLoadScript -> UnLoadScript + LoadScriptToSortListA (:246)
= THEM MOI vao cay, khong doi tep phai co san.
Than trigger_challengeoftime.lua chi co 2 Include + 1 function, dong --OnTrigger()
da chu thich => nap vao cay khong co tac dung phu luc boot.
"""
import io, os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
rel = os.path.join("script", "tinhnang", "3hoatdong", "hd3_driver.lua")

p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in d else "\n"

if "ReLoadScript" in d:
    print("da va roi - bo qua")
    raise SystemExit

anchor = "\tprint(\"[3HD] HD3_DriverInit xong (boot 3 hoat dong Linux).\")"
assert d.count(anchor) == 1, "anchor=%d" % d.count(anchor)

BS = chr(92)
ins = NL.join([
    "\t-- [3HD 25/08] CHAN: g_IniScriptEngine (KSortScript.cpp:51-66) chi nap",
    "\t-- " + BS + "script va " + BS + "scriptjx2" + BS + "tong_vn. Hai tep trigger nam o " + BS + "settings nen",
    "\t-- KHONG co trong cay => DynamicExecute tra NULL => lich Vuot Ai khong chay.",
    "\t-- ReLoadScript = UnLoadScript + LoadScriptToSortListA (THEM MOI vao cay).",
    "\tReLoadScript(" + '"' + BS + BS + "settings" + BS + BS + "trigger_include.lua" + '"' + ")",
    "\tReLoadScript(" + '"' + BS + BS + "settings" + BS + BS + "trigger_challengeoftime.lua" + '"' + ")",
    "",
]) + NL

d = d.replace(anchor, ins + anchor)
io.open(p, "w", encoding="latin-1", newline="").write(d)
m = os.path.join(MIRROR, rel)
os.makedirs(os.path.dirname(m), exist_ok=True)
io.open(m, "w", encoding="latin-1", newline="").write(d)
print("CHAN 3: da them 2 ReLoadScript vao HD3_DriverInit")
for ln in d.split(NL):
    if "ReLoadScript" in ln:
        print("   ", ln.strip())
