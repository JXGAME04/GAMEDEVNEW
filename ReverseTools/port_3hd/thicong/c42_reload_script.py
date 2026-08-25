# -*- coding: utf-8 -*-
"""C42 - chu game can MOT LENH nap lai TOAN BO script cho tien khi test.

DA CO SAN trong engine (KGMCommand.cpp):
    ?gm RLAS            -> ReLoadAllScript() = g_ScriptBinTree.ClearList() + g_IniScriptEngine()
    ?gm ReLoadAllSct    -> nhu tren
    ?gm RLS <duong dan> -> nap lai MOT tep
(tien to bat buoc la "?gm " - TextGMFilter khop 4 byte dau '?gm ' hoac '?GM ')

VAN DE: g_IniScriptEngine CHI quet \\script va \\scriptjx2\\tong_vn (KSortScript.cpp),
KHONG quet \\settings. Ma 3 hoat dong co 2 tep trigger nam o \\settings:
    \\settings\\trigger_include.lua
    \\settings\\trigger_challengeoftime.lua
=> sau khi RLAS, hai tep nay BIEN MAT khoi cay script => lich Vuot Ai chet cho toi
   lan restart. (Luc boot chung duoc HD3_DriverInit nap bang ReLoadScript.)

VA (script-only, khong dung engine): them nut "Nap lai TOAN BO script" vao lenh bai
admin - goi ReLoadScript cho 2 tep trigger de bu lai sau khi chu game go ?gm RLAS,
kem huong dan ngay tren menu.
"""
import io, os, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
rel = r"script\item\hd3_admin.lua"
p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
if "HD3_ADM_ReloadAll" in d:
    print("da co"); raise SystemExit
nl = "\r\n" if "\r\n" in d else "\n"

# 1) them muc menu
a = '\t"5. ' + V("Xem các khoá cấu hình HD3 đang hiệu lực") + '/HD3_ADM_ShowCfg",'
assert d.count(a) == 1, d.count(a)
d = d.replace(a, a + nl + '\t"6. ' + V("Nạp lại toàn bộ script (sau khi gõ ?gm RLAS)") + '/HD3_ADM_ReloadAll",')

# 2) them ham
b = "function HD3_ADM_Reload()"
assert d.count(b) == 1
ham = nl.join([
    "-- [3HD 25/08 C42] Nap lai TOAN BO script cho nhanh khi test.",
    "-- Lenh GM co san:  ?gm RLAS   (= ReLoadAllScript: xoa sach cay script roi nap lai)",
    "-- NHUNG g_IniScriptEngine CHI quet \\script va \\scriptjx2\\tong_vn, KHONG quet",
    "-- \\settings => 2 tep trigger cua Vuot Ai bi mat sau RLAS. Nut nay nap bu chung.",
    "function HD3_ADM_ReloadAll()",
    '\tReLoadScript("\\\\settings\\\\trigger_include.lua")',
    '\tReLoadScript("\\\\settings\\\\trigger_challengeoftime.lua")',
    "\tif (HD_NapLaiCauHinh ~= nil) then HD_NapLaiCauHinh() end",
    '\tMsg2Player("' + V("Đã nạp bù 2 trigger Vượt ải + config. Muốn nạp lại toàn bộ script thì gõ trong khung chat:") + ' <color=yellow>?gm RLAS<color>")',
    '\tMsg2Player("' + V("Thứ tự đúng: gõ ?gm RLAS trước, rồi bấm nút này để nạp bù trigger.") + '")',
    "\tHD3_AdminMenu()",
    "end",
    "",
])
d = d.replace(b, ham + b)
io.open(p, "w", encoding="latin-1", newline="").write(d)
dst = os.path.join(MIR, rel)
os.makedirs(os.path.dirname(dst), exist_ok=True)
shutil.copyfile(p, dst)
print("da them nut 'Nap lai TOAN BO script' + huong dan ?gm RLAS")
