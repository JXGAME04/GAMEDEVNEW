# -*- coding: utf-8 -*-
"""v33 - NOI LO REN VAO MENU LENH BAI ADMIN.

Them hai thu vao script\item\lenhbaiadmin.lua:
  1. Include tep test_loren_admin.lua
  2. Mot dong menu goi LR_Root

BAY DA GAP (26/08): tung noc menu TRUOC khi dat tep dich len may chu, ket qua la
ScriptError.log bao "LR_Root attempt to call a nil value" va menu Lenh bai hong.
Nen cong cu nay KIEM tep dich co that tren may chu roi moi ghi.

BAY MA HOA: nhan menu la TCVN3. Chu HOA co dau khong ma hoa duoc (chi 7 nguyen am
hoa an toan) nen viet chu thuong. Va CAM dau '/' trong nhan vi '/' la dau ngan
giua nhan va ten ham.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, "D:/GAMEDEVNEW/ReverseTools/viemde")
import bangtxt

CR = chr(13)
NL = chr(10)
TAB = chr(9)

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
LB = os.path.join(SRV, "script", "item", "lenhbaiadmin.lua")
DICH = os.path.join(SRV, "script", "item", "test_loren_admin.lua")

MOC_INC = 'Include("\\\\script\\\\item\\\\test_hoatdong_admin.lua")'
DONG_INC = ('Include("\\\\script\\\\item\\\\test_loren_admin.lua")' + TAB +
            '-- [27/08] BO TEST LO REN (ep do tim / do pho)')

NHAN = "Bo test lo ren - ep do tim va do pho"      # khong dau, khong co dau '/'
MOC_MENU = "no"


def main():
    ghi = "--ghi" in sys.argv
    print("NOI LO REN VAO MENU LENH BAI  %s" % ("[GHI THAT]" if ghi else "[DIEN TAP]"))
    print("=" * 78)

    if not os.path.isfile(DICH):
        print("  *** TEP DICH CHUA CO TREN MAY CHU: %s" % DICH)
        print("      Dat tep truoc roi hay noc menu (bay 26/08).")
        return 1
    print("  tep dich CO tren may chu: %s (%d byte)" % (DICH, os.path.getsize(DICH)))

    b = io.open(LB, "rb").read()
    t = b.decode("latin-1")
    crlf = t.count(CR + NL)
    eol = (CR + NL) if crlf > (t.count(NL) - crlf) else NL

    if "test_loren_admin" in t:
        print("  BO QUA - da noi tu truoc")
        return 0

    # 1) Include
    if MOC_INC not in t:
        print("  *** khong tim thay moc Include: %s" % MOC_INC)
        return 1
    t = t.replace(MOC_INC, MOC_INC + eol + DONG_INC, 1)

    # 2) dong menu - chen NGAY TRUOC dong "Ket thuc doi thoai/no"
    dong = t.replace(CR + NL, NL).split(NL)
    vt = -1
    for i, l in enumerate(dong):
        if l.strip().startswith('"') and l.strip().rstrip(",").endswith('/%s"})' % MOC_MENU):
            vt = i
            break
        if l.strip().endswith('/%s"})' % MOC_MENU):
            vt = i
            break
    if vt < 0:
        print("  *** khong tim thay dong menu ket thuc (.../no\"})")
        return 1

    nhan_tcvn = bangtxt.uni2tcvn(NHAN)
    if "/" in NHAN:
        print("  *** nhan menu co dau '/' - cam")
        return 1
    dong_menu = TAB + TAB + '"%s/LR_Root",' % nhan_tcvn
    dong.insert(vt, dong_menu)
    t = eol.join(dong)

    # chot: vong tron ma hoa
    if bangtxt.tcvn2uni(nhan_tcvn) != NHAN:
        print("  *** vong tron TCVN3 khong khop - dung lai")
        return 1

    print("  them Include  : %s" % DONG_INC.split(TAB)[0])
    print("  them dong menu: %s  (truoc dong %d)" % (dong_menu.strip(), vt + 1))

    if ghi:
        sao = LB + ".truoc_loren_menu"
        if not os.path.isfile(sao):
            io.open(sao, "wb").write(b)
        io.open(LB, "wb").write(t.encode("latin-1"))
        print("  DA GHI %s (sao luu: %s)" % (LB, os.path.basename(sao)))
    else:
        print("  (chay lai voi --ghi de ghi that)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
