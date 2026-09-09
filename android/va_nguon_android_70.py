# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 LUAN c] Vong xoay CHI o o vong sang DANG BAT - dung nhu ban tham khao
# (KgameWorldVN.cpp:8642: auraEffectRing setVisible(m_ActiveAuraID == auxiliaryskill[s].m_skillidx)).
# Truoc (va 67) ve vong xoay o MOI o co vong sang. Ky nang phai (IMmediaSkill[1]) = vong sang dang bat
# (KPlayer::SetRightSkill -> SetAuraSkill).

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
DAU = "[ANDROID 10/09 LUAN c]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten, so=1):
    for nl in ("\r\n", "\n"):
        c = nl.join(cu)
        if s.count(c) == so:
            return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho: %s" % (so, ten))


P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

s = thay(s, ["static KUiGameObject s_KNChinh;\t\t// ky nang danh TRAI dang hien tren thanh trang thai"], [
    "static KUiGameObject s_KNChinh;\t\t// ky nang danh TRAI dang hien tren thanh trang thai",
    "static KUiGameObject s_KNPhai;\t\t// %s ky nang danh PHAI = vong sang dang bat (neu la aura)" % DAU,
], "khai bao s_KNChinh")
s = thay(s, ["\ts_KNChinh = oTay.IMmediaSkill[0];"], [
    "\ts_KNChinh = oTay.IMmediaSkill[0];",
    "\ts_KNPhai  = oTay.IMmediaSkill[1];\t// %s" % DAU,
], "DocBang s_KNChinh")
s = thay(s, [
    "\t\tif (bCo && i > 0 && KyNang_OLaAura(i, &o) && CoAnh(s_szKNAnhXoay))",
], [
    "\t\t// %s chi o vong sang DANG BAT (= ky nang phai) moi co vong xoay - KgameWorldVN.cpp:8642" % DAU,
    "\t\tif (bCo && i > 0 && KyNang_OLaAura(i, &o) && o.uId == s_KNPhai.uId && CoAnh(s_szKNAnhXoay))",
], "Ve vong xoay 70")
ghi(P, s)
print("da va:", P)
