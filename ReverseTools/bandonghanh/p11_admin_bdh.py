# -*- coding: utf-8 -*-
r"""[BDH 28/08] Them BO TEST DONG HANH vao lenh bai admin.

1. Sinh script\item\bdh_admin.lua (nhan TCVN3, ham BDH_Root + 11 muc).
2. Va lenhbaiadmin.lua: Include + 1 muc menu chinh "Bo test Dong hanh/BDH_Root".
3. DO lai bo dem 512 byte cua SayEx menu chinh (bai hoc 27/08 tran 551/512);
   neu tran -> tu rut nhan dai nhat da biet.
Chay lai duoc; sao luu .truoc_bdh_admin. lenhbaiadmin tu dofile moi lan bam
(main:49) nen sua xong AN NGAY khong can reload.
"""
import io
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes as MA

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
P_ADMIN = os.path.join(SV, r"script\item\lenhbaiadmin.lua")
P_BDH = os.path.join(SV, r"script\item\bdh_admin.lua")
BS = chr(92)
LF = chr(10)
CAP = 512
DU_TRU_TIEUDE = 175


def vn(s):
    return MA(s).decode("latin-1")


# ============ 1. sinh bdh_admin.lua ============
L = []
L.append("-- [BDH 28/08] BO TEST DONG HANH tren lenh bai admin (GO khi het can)")
L.append("-- Cac ham PARTNER_* la API C da dang ky; chay trong ngu canh NGUOI BAM.")
L.append('Include("' + BS*2 + "script" + BS*2 + "partner" + BS*2 + 'partner_test_bdh.lua")')
L.append("")
L.append("function BDH_Root()")
L.append("\tlocal nCount = PARTNER_Count()")
L.append("\tlocal nCur, nSt = PARTNER_GetCurPartner()")
L.append('\tSayEx({format("' + vn("Đồng hành: %d con - đương nhiệm %d - %s") + '",')
L.append('\t\tnCount, nCur, (nSt == 1) and "' + vn("đang gọi ra") + '" or "' + vn("đang nghỉ") + '"),')
L.append('\t"' + vn("Cấp thú hệ Kim tư chất 5") + '/BDH_A_CapKim",')
L.append('\t"' + vn("Cấp thú ngẫu nhiên") + '/BDH_A_CapNgau",')
L.append('\t"' + vn("Gọi ra - thu về") + '/BDH_A_Goi",')
L.append('\t"' + vn("Cộng 10000 exp") + '/BDH_A_Exp",')
L.append('\t"' + vn("Thăng 1 cấp") + '/BDH_A_LenCap",')
L.append('\t"' + vn("Thân mật = 100") + '/BDH_A_Emo",')
L.append('\t"' + vn("Túi cấp 10") + '/BDH_A_Tui",')
L.append('\t"' + vn("Dạy kỹ năng thử") + '/BDH_A_Skill",')
L.append('\t"' + vn("Xem thông tin thú") + '/BDH_A_Info",')
L.append('\t"' + vn("Xóa con đang chọn") + '/BDH_A_Xoa",')
L.append('\t"' + vn("Chạy FULL bộ test tự động") + '/BDH_A_FullTest",')
L.append('\t"' + vn("Kết thúc đối thoại.") + '/no"})')
L.append("end")
L.append("")
L.append("function BDH_A_CapKim()")
L.append("\tif (PARTNER_Count() >= 3) then")
L.append('\t\tMsg2Player("' + vn("Đã đủ 3 con - xóa bớt rồi cấp.") + '")')
L.append("\t\treturn")
L.append("\tend")
L.append("\tlocal r = PARTNER_AddFightPartner(5, 0, 1, 5, 5, 5, 5, 5, 5)")
L.append('\tMsg2Player("' + vn("Cấp thú hệ Kim: ") + '" .. r)')
L.append("\tBDH_Root()")
L.append("end")
L.append("")
L.append("function BDH_A_CapNgau()")
L.append("\tif (PARTNER_Count() >= 3) then")
L.append('\t\tMsg2Player("' + vn("Đã đủ 3 con - xóa bớt rồi cấp.") + '")')
L.append("\t\treturn")
L.append("\tend")
L.append("\tlocal r = PARTNER_AddFightPartner(random(1, 5), random(0, 4), random(1, 4),")
L.append("\t\trandom(1, 10), random(1, 10), random(1, 10), random(1, 10), random(1, 10), random(1, 10))")
L.append('\tMsg2Player("' + vn("Cấp thú ngẫu nhiên: ") + '" .. r)')
L.append("\tBDH_Root()")
L.append("end")
L.append("")
L.append("function BDH_A_Goi()")
L.append("\tlocal nCur, nSt = PARTNER_GetCurPartner()")
L.append("\tif (nSt == 1) then")
L.append("\t\tPARTNER_CallOutCurPartner(0)")
L.append('\t\tMsg2Player("' + vn("Đã thu về.") + '")')
L.append("\telse")
L.append("\t\tlocal r = PARTNER_CallOutCurPartner(1)")
L.append('\t\tMsg2Player("' + vn("Gọi ra: ") + '" .. r .. "' +
         vn(" (0 = hôn mê/cooldown/map cấm)") + '")')
L.append("\tend")
L.append("end")
L.append("")
L.append("function BDH_A_Exp()")
L.append("\tlocal nCur = PARTNER_GetCurPartner()")
L.append("\tif (nCur == 0) then return end")
L.append("\tPARTNER_AddExp(nCur, 10000, 0)")
L.append('\tMsg2Player("' + vn("Exp = ") + '" .. PARTNER_GetExp(nCur) .. "' +
         vn(" - cấp ") + '" .. PARTNER_GetLevel(nCur))')
L.append("end")
L.append("")
L.append("function BDH_A_LenCap()")
L.append("\tlocal nCur = PARTNER_GetCurPartner()")
L.append("\tif (nCur == 0) then return end")
L.append("\tPARTNER_LevelUp(nCur)")
L.append('\tMsg2Player("' + vn("Cấp mới = ") + '" .. PARTNER_GetLevel(nCur))')
L.append("end")
L.append("")
L.append("function BDH_A_Emo()")
L.append("\tlocal nCur = PARTNER_GetCurPartner()")
L.append("\tif (nCur == 0) then return end")
L.append("\tPARTNER_SetEmotionDegree(nCur, 100)")
L.append('\tMsg2Player("' + vn("Thân mật = ") + '" .. PARTNER_GetEmotionDegree(nCur))')
L.append("end")
L.append("")
L.append("function BDH_A_Tui()")
L.append("\tSetPartnerBagLevel(10)")
L.append('\tMsg2Player("' + vn("Túi đồng hành cấp 10 (6x10 ô mở hết).") + '")')
L.append("end")
L.append("")
L.append("function BDH_A_Skill()")
L.append("\tlocal nCur = PARTNER_GetCurPartner()")
L.append("\tif (nCur == 0) then return end")
L.append("\tPARTNER_AddSkill(nCur, 2, 594, 3, 0)")
L.append("\tlocal lv = PARTNER_GetSkillInfo(nCur, 594)")
L.append('\tMsg2Player("' + vn("Đã dạy kỹ năng 594 cấp ") + '" .. lv)')
L.append("end")
L.append("")
L.append("function BDH_A_Info()")
L.append("\tlocal nCur = PARTNER_GetCurPartner()")
L.append("\tif (nCur == 0) then")
L.append('\t\tMsg2Player("' + vn("Chưa có con nào được chọn.") + '")')
L.append("\t\treturn")
L.append("\tend")
L.append('\tMsg2Player("' + vn("Tên: ") + '" .. PARTNER_GetName(nCur) .. "' +
         vn(" - cấp ") + '" .. PARTNER_GetLevel(nCur) .. "' +
         vn(" - exp ") + '" .. PARTNER_GetExp(nCur))')
L.append('\tMsg2Player("' + vn("Hệ: ") + '" .. PARTNER_GetSeries(nCur) .. "' +
         vn(" - tính cách ") + '" .. PARTNER_GetCharacter(nCur) .. "' +
         vn(" - thân mật ") + '" .. PARTNER_GetEmotionDegree(nCur) .. "/100")')
L.append("\tlocal a1, a2, a3, a4, a5, a6 = PARTNER_GetAttribs(nCur)")
L.append('\tMsg2Player("' + vn("Sinh lực ") + '" .. floor(a1) .. "' +
         vn(" - sức đánh ") + '" .. floor(a2) .. "' +
         vn(" - chính xác ") + '" .. floor(a3))')
L.append('\tMsg2Player("' + vn("Né ") + '" .. floor(a4) .. "' +
         vn(" - tốc độ ") + '" .. floor(a5) .. "' +
         vn(" - may mắn ") + '" .. floor(a6) .. "' +
         vn(" - máu NPC %") + '" .. PARTNER_GetEndure())')
L.append("end")
L.append("")
L.append("function BDH_A_Xoa()")
L.append("\tlocal nCur = PARTNER_GetCurPartner()")
L.append("\tif (nCur == 0) then return end")
L.append("\tlocal r = PARTNER_RemovePartner(nCur)")
L.append('\tMsg2Player("' + vn("Đã xóa con ") + '" .. nCur .. "' +
         vn(" - còn ") + '" .. PARTNER_Count() .. "' + vn(" con") + '")')
L.append("end")
L.append("")
L.append("function BDH_A_FullTest()")
L.append("\tBDH_TestAPI()")
L.append('\tMsg2Player("' + vn("Đã chạy bộ test - PASS/FAIL ghi ở bin/server/bdh_test.log") + '")')
L.append("end")
L.append("")

body = LF.join(L)
io.open(P_BDH, "w", encoding="latin-1", newline="").write(body)
hi = sum(1 for c in body if ord(c) > 127)
print("sinh bdh_admin.lua:", len(body), "byte,", hi, "byte cao (TCVN3)")

# ============ 2. va lenhbaiadmin.lua ============
s = io.open(P_ADMIN, "r", encoding="latin-1", newline="").read()
if not os.path.exists(P_ADMIN + ".truoc_bdh_admin"):
    io.open(P_ADMIN + ".truoc_bdh_admin", "w", encoding="latin-1", newline="").write(s)

if "bdh_admin.lua" not in s:
    neo = 'Include("' + BS*2 + "script" + BS*2 + "item" + BS*2 + 'test_loren_admin.lua")'
    assert s.count(neo) == 1, s.count(neo)
    s = s.replace(neo, neo + LF + 'Include("' + BS*2 + "script" + BS*2 + "item" + BS*2 +
                  'bdh_admin.lua")' + chr(9) + "-- [BDH 28/08] bo test Dong hanh", 1)
    print("da them Include bdh_admin")
else:
    print("Include da co")

muc = '"' + vn("Bộ test Đồng hành") + '/BDH_Root",'
if "BDH_Root" not in s:
    neo2 = '"' + vn("Bộ test Kinh Mạch") + '/KM_TestRoot",'
    assert s.count(neo2) == 1, "khong thay muc Kinh Mach"
    s = s.replace(neo2, neo2 + LF + chr(9) + chr(9) + muc, 1)
    print("da them muc menu chinh")
else:
    print("muc menu da co")

io.open(P_ADMIN, "w", encoding="latin-1", newline="").write(s)

# ============ 3. do lai bo dem 512 ============
t = io.open(P_ADMIN, "r", encoding="latin-1", newline="").read()
i = t.find("function chucnangadmin(itemIdx)")
j = t.find(LF + "end", i)
blk = t[i:j]
tong = 0
for ln in blk.split(LF):
    sln = ln.strip()
    if sln.startswith("--"):
        continue
    for m in re.findall(r'"([^"]*)"', ln):
        if "/" in m:
            tong += len(m.split("/")[0]) + 1
print("nhan menu chinh: %d + tieu de ~%d = %d / %d => %s" %
      (tong, DU_TRU_TIEUDE, tong + DU_TRU_TIEUDE, CAP,
       "VUA" if tong + DU_TRU_TIEUDE <= CAP else "!!! TRAN - PHAI RUT NHAN"))

# do submenu BDH_Root
t2 = body
i2 = t2.find("function BDH_Root()")
j2 = t2.find(LF + "end", i2)
blk2 = t2[i2:j2]
tong2 = 0
for ln in blk2.split(LF):
    for m in re.findall(r'"([^"]*)"', ln):
        if "/" in m:
            tong2 += len(m.split("/")[0]) + 1
print("nhan submenu BDH: %d + tieu de ~60 = %d / %d => %s" %
      (tong2, tong2 + 60, CAP, "VUA" if tong2 + 60 <= CAP else "!!! TRAN"))
print("XONG p11")
