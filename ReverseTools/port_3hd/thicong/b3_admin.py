# -*- coding: utf-8 -*-
"""B3 - sinh hd3_admin.lua (menu test 3 hoat dong tren Lenh Bai Admin).
Chuoi tieng Viet ma hoa TCVN3 bang unicode_to_tcvn3_bytes cua skill.
Ghi ca JX1 lan MIRROR. Sau do noi Include + 1 muc menu vao lenhbaiadmin.lua.
"""
import io, os, sys
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"


def V(s):
    """unicode -> chuoi latin-1 mang byte TCVN3 (de ghep vao noi dung file)."""
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


# Noi dung file: dung {V} cho tieng Viet, ASCII cho code.
L = []
a = L.append
a('-- ============================================================================')
a('-- HD3_ADMIN.LUA - Menu TEST 3 hoat dong ban Linux tren Lenh Bai Admin.')
a('-- Include tu lenhbaiadmin.lua (main() dofile lai) => SUA KHONG CAN RESTART.')
a('-- Cau hinh: script\\header\\cauhinh_hoatdong.lua (khoi [6]).')
a('-- ============================================================================')
a('Include("\\\\script\\\\tinhnang\\\\3hoatdong\\\\hd3_driver.lua")')
a('')
a('function HD3_AdminMenu()')
a('\tSayEx({"<color=yellow>' + V("Hoạt động ban Linux (test)") + '<color>: ' + V("chọn mục") + '",')
a('\t"1. ' + V("Săn Boss Sát Thủ") + '/HD3_ADM_ST",')
a('\t"2. ' + V("Phong Lăng Độ") + '/HD3_ADM_PLD",')
a('\t"3. ' + V("Vượt ải") + '/HD3_ADM_VA",')
a('\t"4. ' + V("Nạp lại CONFIG") + '/HD3_ADM_Reload",')
a('\t"' + V("Kết thúc đối thoại") + './no"})')
a('end')
a('')
# ---- (1) SAN BOSS SAT THU ----
a('function HD3_ADM_ST()')
a('\tSayEx({"<color=yellow>' + V("Săn Boss Sát Thủ") + '<color> - ' + V("cấp") + ' >=" .. HD_CFG("HD3_ST_CAP_TOITHIEU", 90) .. ", ' + V("trần") + ' " .. HD_CFG("HD3_ST_MAX_NGAY", 8) .. " ' + V("lần/ngày") + '",')
a('\t"' + V("Sinh lại NPC 769 + 160 boss (boot)") + '/HD3_ADM_ST_Boot",')
a('\t"' + V("Dịch chuyển: NPC Nhiếp Thí Trần (Ba Lăng Huyện)") + '/HD3_ADM_ST_Tele",')
a('\t"' + V("Nhận 5 Sát Thủ lệnh cấp 90 (test gộp)") + '/HD3_ADM_ST_Lenh",')
a('\t"' + V("Nhận 1 Sát Thủ Giản cấp 90 (vé Vượt ải)") + '/HD3_ADM_ST_Gian",')
a('\t"' + V("Xem biến nhiệm vụ (1082/1192/1193/1217)") + '/HD3_ADM_ST_Task",')
a('\t"' + V("Reset số lần giết trong ngày") + '/HD3_ADM_ST_Reset",')
a('\t"' + V("Quay lại") + '/HD3_AdminMenu"})')
a('end')
a('function HD3_ADM_ST_Boot() HD3_DriverInit() Msg2Player("' + V("Đã gọi HD3_DriverInit (sinh NPC + boss).") + '") end')
a('function HD3_ADM_ST_Tele() NewWorld(1, 1506, 3198) end')
a('function HD3_ADM_ST_Lenh()')
a('\tfor i = 1, 5 do AddItem(6, 1, 398, 90, 0, 0) end')
a('\tMsg2Player("' + V("Đã nhận 5 Sát Thủ lệnh cấp 90 (6,1,398). Gặp Nhiếp Thí Trần để gộp thành Sát Thủ Giản.") + '")')
a('end')
a('function HD3_ADM_ST_Gian() AddItem(6, 1, 399, 90, 0, 0) Msg2Player("' + V("Đã nhận 1 Sát Thủ Giản cấp 90 (6,1,399) - vé vào Vượt ải.") + '") end')
a('function HD3_ADM_ST_Task()')
a('\tMsg2Player(format("' + V("1082=%d (chỉ số) | 1192=%d (ngày) | 1193=%d (số lần) | 1217=%d (tích lũy)") + '", GetTask(1082), GetTask(1192), GetTask(1193), GetTask(1217)))')
a('\tHD3_ADM_ST()')
a('end')
a('function HD3_ADM_ST_Reset() SetTask(1193, 0) Msg2Player("' + V("Đã reset số lần giết boss trong ngày.") + '") end')
a('')
# ---- (2) PHONG LANG DO ----
a('function HD3_ADM_PLD()')
a('\tSayEx({"<color=yellow>' + V("Phong Lăng Độ") + '<color> - ' + V("mở mỗi giờ phút :00; giờ tốn phí 10/14/16/18/20h") + '",')
a('\t"' + V("Khai cuộc NGAY (mở đăng ký thuyền)") + '/HD3_ADM_PLD_Now",')
a('\t"' + V("Dịch chuyển: Thuyền phu bờ Nam (map 336)") + '/HD3_ADM_PLD_Tele336",')
a('\t"' + V("Dịch chuyển: vào map thuyền 337") + '/HD3_ADM_PLD_Tele337",')
a('\t"' + V("Nhận Lệnh bài Phong Lăng Độ (4,489)") + '/HD3_ADM_PLD_LB",')
a('\t"' + V("Nhận Lệnh Bài Thủy Tặc (6,1,2745)") + '/HD3_ADM_PLD_LBTT",')
a('\t"' + V("Quay lại") + '/HD3_AdminMenu"})')
a('end')
a('function HD3_ADM_PLD_Now() HD3_Adm_PLD_Now() Msg2Player("' + V("Đã ép khai cuộc Phong Lăng Độ (fenglingdu_main).") + '") end')
a('function HD3_ADM_PLD_Tele336() NewWorld(336, 1147, 3018) end')
a('function HD3_ADM_PLD_Tele337() NewWorld(337, 1646, 3233) end')
a('function HD3_ADM_PLD_LB() AddItem(4, 489, 0, 0, 0, 0) Msg2Player("' + V("Đã nhận Lệnh bài Phong Lăng Độ.") + '") end')
a('function HD3_ADM_PLD_LBTT() AddItem(6, 1, 2745, 0, 0, 0) Msg2Player("' + V("Đã nhận Lệnh Bài Thủy Tặc.") + '") end')
a('')
# ---- (3) VUOT AI ----
a('function HD3_ADM_VA()')
a('\tSayEx({"<color=yellow>' + V("Vượt ải") + '<color> - ' + V("báo danh mỗi giờ :00; cần 1 Sát Thủ Giản + đủ 4 người tổ đội") + '",')
a('\t"' + V("Báo danh NGAY (mở mission)") + '/HD3_ADM_VA_Now",')
a('\t"' + V("Trao bảng xếp hạng ngày") + '/HD3_ADM_VA_Rank",')
a('\t"' + V("Nhận 1 Sát Thủ Giản cấp 90 (vé vào)") + '/HD3_ADM_ST_Gian",')
a('\t"' + V("Dịch chuyển: thành Ba Lăng (gặp Dịch Quán)") + '/HD3_ADM_ST_Tele",')
a('\t"' + V("Quay lại") + '/HD3_AdminMenu"})')
a('end')
a('function HD3_ADM_VA_Now() HD3_Adm_VA_Now() Msg2Player("' + V("Đã ép báo danh Vượt ải (trigger OnTrigger).") + '") end')
a('function HD3_ADM_VA_Rank() HD3_Adm_VA_Rank() Msg2Player("' + V("Đã chạy bảng xếp hạng ngày (Ladder 10235).") + '") end')
a('')
a('function HD3_ADM_Reload()')
a('\tif (HD_NapLaiCauHinh ~= nil) then HD_NapLaiCauHinh() end')
a('\tMsg2Player("' + V("Đã nạp lại CONFIG (script\\header\\cauhinh_hoatdong.lua).") + '")')
a('\tHD3_AdminMenu()')
a('end')
a('')

data = ("\r\n".join(L) + "\r\n").encode("latin-1")
rel = os.path.join("script", "item", "hd3_admin.lua")
for base in (JX1, MIRROR):
    p = os.path.join(base, rel)
    os.makedirs(os.path.dirname(p), exist_ok=True)
    open(p, "wb").write(data)
    print("ghi", p, os.path.getsize(p), "byte")
