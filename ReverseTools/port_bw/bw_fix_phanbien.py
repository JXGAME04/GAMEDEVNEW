# -*- coding: utf-8 -*-
r"""[BW 23/08 - hau phan bien] F1: guard de quy OnLeave <-> LeaveGame <-> DelMSPlayer.
(F2 - vi tri cong cap 90 - da sua TAI GOC trong bw_port.py: chen ngay sau khoi MemberCount,
truoc 'local OldSubWorld'/OpenMission. Tep nay CHI con F1, idempotent.)
Co che F1: KMission::RemovePlayer goi script "OnLeave" TRUOC m_MissionPlayer.Remove
(KMission.cpp:185/188 'Fix by Fong Kieu') -> OnLeave bw goi LeaveGame() -> bwhead:72
DelMSPlayer -> RemovePlayer long nhau van thay entry -> OnLeave lan nua -> de quy vo han
(tran C stack ngay tran dau: chet/roi dai/GameOver/logout). LeaveGame ha TaskTemp(200)=0
(bwhead:61) TRUOC DelMSPlayer nen guard theo 200 cat vong lap o depth 2.
"""
import io, os, shutil

E   = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\bw"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)

p = os.path.join(E, r"script\missions\bw\bwmission.lua")
s = rd(p)
if "CHAN DE QUY" in s:
    print("F1 da co san - bo qua")
else:
    NL = "\r\n" if "\r\n" in s else "\n"
    old = "function OnLeave(RoleIndex)" + NL + "\tPlayerIndex = RoleIndex;" + NL
    assert s.count(old) == 1, "anchor OnLeave: %d" % s.count(old)
    new = ("function OnLeave(RoleIndex)" + NL +
           "\tPlayerIndex = RoleIndex;" + NL +
           "\t-- [BW 23/08] CHAN DE QUY: engine JX1 goi OnLeave TRUOC khi xoa entry (KMission.cpp:185/188" + NL +
           "\t-- 'Fix by Fong Kieu') -> LeaveGame():72 DelMSPlayer se vao lai OnLeave vo han (tran C stack)." + NL +
           "\t-- LeaveGame ha TaskTemp(200)=0 (bwhead:61) TRUOC DelMSPlayer nen nhanh long nhau return o day;" + NL +
           "\t-- nguoi da roi tran (200==0) cung khong bi NewWorld keo lai." + NL +
           "\tif (GetTaskTemp(200) ~= 1) then" + NL +
           "\t\treturn" + NL +
           "\tend" + NL)
    s = s.replace(old, new, 1)
    wr(p, s)
    print("F1 ok: guard TaskTemp(200) dau OnLeave")
shutil.copyfile(p, os.path.join(MIR, r"script\missions\bw\bwmission.lua"))
print("XONG")
