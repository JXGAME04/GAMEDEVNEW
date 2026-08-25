# -*- coding: utf-8 -*-
r"""[25/08] Them muc 10 'TEST TIN SU' vao lenh bai admin (hoatdong_admin.lua).

Moi thao tac CHEP tu ham goc, khong tu che:
 - Huy nhiem vu   = losemessengertask()            (messenger_losetask.lua:9 - ham NPC dung)
 - Den ai 3       = SetRevPos(11,10)+NewWorld(395,1417,3207)  (wagoner.lua:23-27)
 - Ve Ba Lang     = dung chuoi don trang thai cua ture_movecity (messenger_turerukou.lua:171-192)
 - Go ket 20->21  = dung viec messenger_livegame lam voi 1203 khi logout (lievegame:50-52);
                    phan don trang thai KHONG can vi se duoc ture_real_starttask dat lai
                    khi bam 'Tiep tuc nhiem vu'.
 - Bat chien dau  = SetFightState(1) (ScriptFuns.cpp:9839 -> m_FightMode) - de test nhanh:
                    day chinh la co ma thieu no thi KHONG danh duoc quai (KNpcSet.cpp:1553)
                    va KHONG THE CHET (KNpc.cpp:1477 kep 1 mau).
Chuoi ASCII khong dau -> khong dung toi byte TCVN3. Nhan menu KHONG chua '/' (bay strstr).
lenhbaiadmin dofile lai moi lan dung -> AN NGAY, khong can restart GS.
"""
import io, os, shutil

P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\hoatdong_admin.lua"
ENC = "latin-1"
s = io.open(P, "r", encoding=ENC, newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"

if "HD_TS_Menu" in s:
    print("da co muc 10"); raise SystemExit

# 1) dong menu chinh: chen truoc dong 'Ket thuc doi thoai' cuoi bang menu goc
neo = '"9. Danh hi'
i = s.find(neo)
assert i > 0, "khong thay dong muc 9"
# tim dong ket thuc ngay sau muc 9
j = s.find(NL, i)
assert j > 0
dong10 = '\t"10. Test Tin Su - chan doan va go ket/HD_TS_Menu",' + NL
s = s[:j + len(NL)] + dong10 + s[j + len(NL):]

# 2) Include messenger_losetask o dau file (sau dong Include dau tien co san)
inc = 'Include("\\\\script\\\\task\\\\tollgate\\\\messenger\\\\messenger_losetask.lua")\t-- [25/08] losemessengertask + nt_* cho muc 10'
first_inc_end = s.find(NL, s.find("Include("))
assert first_inc_end > 0
s = s[:first_inc_end + len(NL)] + inc + NL + s[first_inc_end + len(NL):]

# 3) section 10 cuoi file
sec = NL.join([
"",
"-- ================= 10) TEST TIN SU =================",
"-- Chan doan + go ket tai cho. Nguon tung thao tac ghi trong add_menu_tinsu.py.",
"-- Y nghia 1203: 0=chua nhan/da xong het, 10=da nhan chua bat dau, 20=dang lam,",
"-- 21=tam ngat (duoc phep 'Tiep tuc'), 25/30=hoan thanh cho tra.",
"function HD_TS_Menu()",
"\tSayEx({\"<color=yellow>Test Tin Su<color> - 1203=\"..nt_getTask(1203)..\" | RutVuKhi=\"..GetFightState()..\" (0 = khong danh duoc quai + khong the chet)\",",
"\t\"1. Xem trang thai chi tiet/HD_TS_Xem\",",
"\t\"2. Go ket: 20 sang 21 roi toi Dich Quan bam Tiep tuc/HD_TS_GoKet\",",
"\t\"3. Bat trang thai chien dau NGAY - test nhanh trong ai/HD_TS_BatFight\",",
"\t\"4. Huy sach nhiem vu Tin Su - nhu NPC/HD_TS_Huy\",",
"\t\"5. Den ai 3 Thien Bao Kho - map 395/HD_TS_DenAi\",",
"\t\"6. Ve Ba Lang Huyen - don trang thai nhu NPC/HD_TS_VeThanh\",",
"\t\"Ket thuc doi thoai/no\"})",
"end",
"",
"function HD_TS_Xem()",
"\tMsg2Player(format(\"[TinSu] 1203=%d 1201=%d 1202=%d 1204=%d 1205=%d 1206=%d 1211=%d\",",
"\t\tnt_getTask(1203), nt_getTask(1201), nt_getTask(1202), nt_getTask(1204),",
"\t\tnt_getTask(1205), nt_getTask(1206), nt_getTask(1211)))",
"\tlocal nMap, nX, nY = GetWorldPos()",
"\tMsg2Player(format(\"[TinSu] map=%d toado=(%d,%d) RutVuKhi=%d\", nMap, nX, nY, GetFightState()))",
"end",
"",
"function HD_TS_GoKet()",
"\tif (nt_getTask(1203) == 20) then",
"\t\tnt_setTask(1203, 21)",
"\t\tMsg2Player(\"[TinSu] 1203: 20 -> 21. Toi Dich Quan bam 'Tiep tuc nhiem vu' de bat lai day du trang thai (dong ho, chien dau, hoi sinh...).\")",
"\telse",
"\t\tMsg2Player(\"[TinSu] 1203 = \"..nt_getTask(1203)..\" - chi go ket khi dang 20.\")",
"\tend",
"end",
"",
"function HD_TS_BatFight()",
"\tSetFightState(1)",
"\tMsg2Player(\"[TinSu] Da RUT VU KHI (bat chien dau). Thu danh quai ngay. Luu y: gio co the CHET that.\")",
"end",
"",
"function HD_TS_Huy()",
"\tlosemessengertask()",
"\tMsg2Player(\"[TinSu] Da goi huy nhiem vu (chi xoa khi dang 20/21). Nhan lai o Dich Quan thanh Ba Lang/Dai Ly.\")",
"end",
"",
"function HD_TS_DenAi()",
"\tSetRevPos(11,10)",
"\tNewWorld(395,1417,3207)",
"end",
"",
"function HD_TS_VeThanh()",
"\tDisabledUseTownP(0)",
"\tSetFightState(0)",
"\tSetPunish(1)",
"\tSetCreateTeam(1)",
"\tSetPKFlag(0)",
"\tForbidChangePK(0)",
"\tSetDeathScript(\"\")",
"\tSetLogoutRV(0)",
"\tNewWorld(11,3021,5090)",
"end",
""])
s = s + sec

q = P + ".truoc_menu10_2508"
if not os.path.isfile(q): shutil.copyfile(P, q)
io.open(P, "w", encoding=ENC, newline="").write(s)
print("DA THEM muc 10 TEST TIN SU")
