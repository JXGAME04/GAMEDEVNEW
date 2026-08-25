# -*- coding: utf-8 -*-
r"""[25/08] NOI MAT XICH LOGOUT cua Tin Su - port 21/08 thieu.

GOC (doi chieu Linux, khong doan):
  Linux: engine goi script\global\logout.lua khi nguoi choi THOAT GAME
         -> messenger_livegame() (messenger_lievegame.lua): neu dang o ai 387-395:
            SetFightState(0), DisabledUseTownP(0), SetPunish(1), SetDeathScript(""),
            va 1203: 20 -> 21 ("tam ngat nhiem vu").
  JX1  : duong logout la script\player\playerlogout.lua (KPlayerSet.cpp:365)
         - ban minh CHI ghi log, KHONG goi messenger_livegame => thoat game trong ai
         thi 1203 KET o 20; vao lai bam "Tiep tuc" roi vao nhanh 20 (chi Describe,
         khong SetFightState) => m_FightMode=0 vinh vien =>
           * khong danh duoc quai (KNpcSet.cpp:1553-1558: player fight_none ↔ quai
             thuong = relation_none)
           * quai danh nguoi khong chet - kep 1 mau (KNpc.cpp:1477-1481)
         Trang thai "1203=20 sau relog" la trang thai BAT KHA o Linux - chi ton tai
         o ban minh vi thieu hook. Noi hook xong thi nhanh 21 lo het nhu Linux.

VA: Include messenger_lievegame.lua vao playerlogout.lua + goi messenger_livegame()
trong main(). Ham do TU lay GetPos + TU kiem map 387-395 ben trong (dong 40-41)
nen goi vo dieu kien la dung hanh vi Linux (logout.lua Linux chi kiem map truoc
de do goi thua).
"""
import io, os, shutil

P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\player\playerlogout.lua"
ENC = "latin-1"
s = io.open(P, "r", encoding=ENC, newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"

if "messenger_livegame" in s:
    print("da va truoc do"); raise SystemExit

# 1) Include (HAI backslash trong nguon Lua!)
inc_cu = 'Include("\\\\script\\\\log_game\\\\save_log.lua")'
assert inc_cu in s, "khong thay dong Include save_log"
inc_moi = (inc_cu + NL +
    "-- [TIN SU 25/08] noi mat xich logout cua Linux (global\\logout.lua:9,28-31):" + NL +
    "-- thoat game trong ai 387-395 phai don trang thai + doi 1203: 20->21 de lan sau" + NL +
    "-- 'Tiep tuc nhiem vu' bat lai SetFightState. Ham tu kiem map ben trong." + NL +
    'Include("\\\\script\\\\task\\\\tollgate\\\\messenger\\\\messenger_lievegame.lua")')
s = s.replace(inc_cu, inc_moi, 1)

# 2) goi trong main()
main_cu = "function main()" + NL + "\tLogDangXuat()" + NL + "end"
assert main_cu in s, "khong thay main() dang mong doi"
main_moi = ("function main()" + NL +
            "\tLogDangXuat()" + NL +
            "\tmessenger_livegame()\t-- [TIN SU 25/08] tu kiem map 387-395 ben trong" + NL +
            "end")
s = s.replace(main_cu, main_moi, 1)

q = P + ".truoc_tinsu_logout_2508"
if not os.path.isfile(q): shutil.copyfile(P, q)
io.open(P, "w", encoding=ENC, newline="").write(s)
print("DA VA playerlogout.lua")
