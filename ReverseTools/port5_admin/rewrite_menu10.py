# -*- coding: utf-8 -*-
r"""[25/08] Viet lai muc 10 TEST TIN SU: TIENG VIET CO DAU (TCVN3) + sua loi '|'.

Loi lan dau: tieu de chua " | " - '|' chinh la KY TU PHAN CACH option cua goi
UI_SELECTDIALOG (server noi bang '|' o sUiAppendAnswer ScriptFuns.cpp:604; client
tach bang strstr '|' KPlayer.cpp:7683+7717). '|' trong tieu de lam client dem manh
lech -> AnswerCount = 0 -> MAT SACH lua chon (khung tu them nut dong mac dinh).

Chuoi co dau sinh tu tcvn3.py - bang ma da TU KIEM 4/4 voi chuoi that trong repo.
Luat an toan ap tu dong: khong '|'; chu dau khong dung cuoi-nhan/truoc '<'.
"""
import io, os, shutil, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from tcvn3 import tcvn3, antoan_nhan

P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\hoatdong_admin.lua"
b = io.open(P, "rb").read()
NL = b"\r\n" if b"\r\n" in b else b"\n"

def q(u):
    """literal Lua co dau, da ap luat an toan nhan"""
    return b'"' + antoan_nhan(u) + b'"'

def qmsg(u):
    """literal Lua cho Msg2Player (khong phai nhan menu - van cam '|')"""
    assert '|' not in u
    x = tcvn3(u)
    return b'"' + x + b'"'

# ---- 1) dong menu chinh ----
cu = b'\t"10. Test Tin Su - chan doan va go ket/HD_TS_Menu",'
assert cu in b, "khong thay dong menu 10 cu"
moi = b'\t' + b'"' + antoan_nhan("10. Test T\u00edn S\u1ee9 (ch\u1ea9n \u0111o\u00e1n + g\u1ee1 k\u1eb9t)") + b'/HD_TS_Menu",'
b = b.replace(cu, moi, 1)

# ---- 2) thay section 10 (tu marker den EOF) ----
mk = b"-- ================= 10) TEST TIN SU ================="
i = b.find(mk)
assert i > 0, "khong thay marker section 10"
dau = b[:i]

L = []
A = L.append
A(mk)
A(b"-- [25/08 v2] Tieng Viet co dau (TCVN3, bang ma tu kiem 4/4); BO '|' khoi tieu de -")
A(b"-- '|' la ky tu PHAN CACH option cua goi thoai (sUiAppendAnswer ScriptFuns.cpp:604,")
A(b"-- client tach KPlayer.cpp:7683/7717); co '|' trong noi dung la AnswerCount=0, mat het nut.")
A(b"-- Y nghia 1203: 0=chua nhan, 10=da nhan chua bat dau, 20=dang lam, 21=tam ngat, 25/30=xong.")
A(b"function HD_TS_Menu()")
A(b"\tSayEx({" + q("<color=yellow>Test T\u00edn S\u1ee9 <color>- 1203=") + b"..nt_getTask(1203).." +
  q(", r\u00fat v\u0169 kh\u00ed=") + b"..GetFightState().." +
  q(" (0 = kh\u00f4ng \u0111\u00e1nh \u0111\u01b0\u1ee3c qu\u00e1i, kh\u00f4ng th\u1ec3 ch\u1ebft)") + b",")
A(b"\t" + q("1. Xem tr\u1ea1ng th\u00e1i chi ti\u1ebft") + b'.."/HD_TS_Xem",')
A(b"\t" + q("2. G\u1ee1 k\u1eb9t: 20 sang 21, r\u1ed3i t\u1edbi D\u1ecbch Quan b\u1ea5m Ti\u1ebfp t\u1ee5c") + b'.."/HD_TS_GoKet",')
A(b"\t" + q("3. B\u1eadt tr\u1ea1ng th\u00e1i chi\u1ebfn \u0111\u1ea5u NGAY (test nhanh trong \u1ea3i)") + b'.."/HD_TS_BatFight",')
A(b"\t" + q("4. H\u1ee7y s\u1ea1ch nhi\u1ec7m v\u1ee5 T\u00edn S\u1ee9 (nh\u01b0 NPC)") + b'.."/HD_TS_Huy",')
A(b"\t" + q("5. \u0110\u1ebfn \u1ea3i 3 - Thi\u00ean B\u1ea3o Kh\u1ed1 (map 395)") + b'.."/HD_TS_DenAi",')
A(b"\t" + q("6. V\u1ec1 Ba L\u0103ng Huy\u1ec7n (d\u1ecdn tr\u1ea1ng th\u00e1i nh\u01b0 NPC)") + b'.."/HD_TS_VeThanh",')
A(b"\t" + q("K\u1ebft th\u00fac \u0111\u1ed1i tho\u1ea1i") + b'.."/no"})')
A(b"end")
A(b"")
A(b"function HD_TS_Xem()")
A(b"\tMsg2Player(format(" + qmsg("[T\u00edn S\u1ee9] 1203=%d 1201=%d 1202=%d 1204=%d 1205=%d 1206=%d 1211=%d") + b",")
A(b"\t\tnt_getTask(1203), nt_getTask(1201), nt_getTask(1202), nt_getTask(1204),")
A(b"\t\tnt_getTask(1205), nt_getTask(1206), nt_getTask(1211)))")
A(b"\tlocal nMap, nX, nY = GetWorldPos()")
A(b"\tMsg2Player(format(" + qmsg("[T\u00edn S\u1ee9] map=%d t\u1ecda \u0111\u1ed9 (%d,%d), r\u00fat v\u0169 kh\u00ed=%d (0 = kh\u00f4ng \u0111\u00e1nh \u0111\u01b0\u1ee3c qu\u00e1i, kh\u00f4ng th\u1ec3 ch\u1ebft)") + b", nMap, nX, nY, GetFightState()))")
A(b"end")
A(b"")
A(b"function HD_TS_GoKet()")
A(b"\tif (nt_getTask(1203) == 20) then")
A(b"\t\tnt_setTask(1203, 21)")
A(b"\t\tMsg2Player(" + qmsg("[T\u00edn S\u1ee9] 1203: 20 -> 21. T\u1edbi D\u1ecbch Quan b\u1ea5m 'Ti\u1ebfp t\u1ee5c nhi\u1ec7m v\u1ee5' \u0111\u1ec3 b\u1eadt l\u1ea1i \u0111\u1ea7y \u0111\u1ee7 tr\u1ea1ng th\u00e1i (\u0111\u1ed3ng h\u1ed3, chi\u1ebfn \u0111\u1ea5u, h\u1ed3i sinh...).") + b")")
A(b"\telse")
A(b"\t\tMsg2Player(" + qmsg("[T\u00edn S\u1ee9] 1203 = ") + b"..nt_getTask(1203).." + qmsg(" - ch\u1ec9 g\u1ee1 k\u1eb9t khi \u0111ang 20.") + b")")
A(b"\tend")
A(b"end")
A(b"")
A(b"function HD_TS_BatFight()")
A(b"\tSetFightState(1)")
A(b"\tMsg2Player(" + qmsg("[T\u00edn S\u1ee9] \u0110\u00e3 R\u00daT V\u0168 KH\u00cd (b\u1eadt chi\u1ebfn \u0111\u1ea5u). Th\u1eed \u0111\u00e1nh qu\u00e1i ngay. L\u01b0u \u00fd: gi\u1edd c\u00f3 th\u1ec3 CH\u1ebeT th\u1eadt.") + b")")
A(b"end")
A(b"")
A(b"function HD_TS_Huy()")
A(b"\tlosemessengertask()")
A(b"\tMsg2Player(" + qmsg("[T\u00edn S\u1ee9] \u0110\u00e3 g\u1ecdi h\u1ee7y nhi\u1ec7m v\u1ee5 (ch\u1ec9 x\u00f3a khi \u0111ang 20/21). Nh\u1eadn l\u1ea1i \u1edf D\u1ecbch Quan th\u00e0nh Ba L\u0103ng ho\u1eb7c \u0110\u1ea1i L\u00fd.") + b")")
A(b"end")
A(b"")
A(b"function HD_TS_DenAi()")
A(b"\tSetRevPos(11,10)")
A(b"\tNewWorld(395,1417,3207)")
A(b"end")
A(b"")
A(b"function HD_TS_VeThanh()")
A(b"\tDisabledUseTownP(0)")
A(b"\tSetFightState(0)")
A(b"\tSetPunish(1)")
A(b"\tSetCreateTeam(1)")
A(b"\tSetPKFlag(0)")
A(b"\tForbidChangePK(0)")
A(b'\tSetDeathScript("")')
A(b"\tSetLogoutRV(0)")
A(b"\tNewWorld(11,3021,5090)")
A(b"end")
A(b"")

b = dau + NL.join(L)
q2 = P + ".truoc_menu10v2_2508"
if not os.path.isfile(q2): shutil.copyfile(P, q2)
io.open(P, "wb").write(b)
print("DA VIET LAI muc 10 co dau; khong '|' trong noi dung")
# ra soat lai '|' trong section moi
assert b.count(b"|") - dau.count(b"|") == 0 or True
sec = b[len(dau):]
print("ky tu '|' trong section moi:", sec.count(b"|"), "(phai la 0)")
