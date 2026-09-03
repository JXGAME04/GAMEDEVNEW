# -*- coding: utf-8 -*-
# vhtd_engine_patch13.py [VHTD 02/09x]
# BONG MO HUYEN NHAN VAN YEN (1358) "CHAY MOT LUC ROI MAT HIEN LUON" - TIM RA GOC THAT.
#
# GOC: KPlayer.cpp dong 465-468, NHANH CLIENT cua KPlayer::Active() (#ifdef _SERVER mo o 390, #else o 415
# -> dong 465 la ma CLIENT). Moi nhip, neu nhan vat CUA CHINH MINH khong o TU THE CHIEN DAU thi engine
# ra lenh XOA bong mo:
#       if(!Npc[m_nIndex].m_FightMode)
#           Npc[m_nIndex].FkAutoSetBlur(FALSE);   // fix loi thien vuong bang phu ve thanh bi ao anh
# FkAutoSetBlur (KNpc.h:661) chi la m_DataRes.SetBlur(b).
#
# Day la ban va CU cua doi truoc (chong "ao anh" khi phu ve thanh), KHONG phai loi dot nay. Nhung no chay
# MOI NHIP va dam thang vao cong tac bong mo [VHTD 02/09r] toi them o KNpc::Activate. JX1 TU ROI KHOI tu the
# chien dau sau vai giay khong danh -> dung luc do lenh xoa nay thang, bong mo tat va khong tro lai cho toi
# khi nguoi choi danh nhau lai. Trieu chung quan sat duoc dung la "chay mot luc roi mat hien luon".
#
# VA: bien lenh xoa thanh lenh QUYET DINH thay vi noi long no.
#   - Dang DI/CHAY va dang co buff walkrunshadow  -> BAT bong mo
#   - MOI truong hop con lai                      -> giu nguyen lenh XOA cu (la chan chong ao anh)
# Nho vay tu the DUNG YEN / CHET / HOI SINH / PHU VE THANH deu roi vao nhanh "xoa" nhu truoc, KHONG tai
# phat loi ao anh, va KHONG de xac nha anh mo (may chu ngung gui goi dong bo cho xac nen client se dong bang
# co bong mo neu ta chi "noi long" - do la ly do PHUONG AN NOI LONG DA BI BAC BO va toi khong dung no).
#
# Dieu kien nay TRUNG KHIT voi cong tac o KNpc::Activate [VHTD 02/09r] nen hai cho khong con danh nhau.
#
# GIU NGUYEN TUNG BYTE dong 467 goc (co chu Viet TCVN3): chi CHEN them dong phia truoc va bien no thanh
# than cua "else". Khong dung Edit/Write tool len tep nguon; doc/ghi latin-1, ton trong kieu xuong dong.
#
# DUNG: python vhtd_engine_patch13.py --kiem   (thu)
#       python vhtd_engine_patch13.py          (ghi that)
import io
import os
import sys

KIEM = "--kiem" in sys.argv
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

MK = "[VHTD 02/09x]"
P = "D:/GAMEDEVNEW/Sources/Core/Src/KPlayer.cpp"
T = chr(9)


def main():
    b = io.open(P, "rb").read()
    bom = b[:3] == b"\xef\xbb\xbf"
    cao_truoc = sum(1 for c in b if c >= 0x80)
    s = b.decode("latin-1")
    nl = "\r\n" if s.count("\r\n") * 2 > s.count("\n") else "\n"
    lines = s.split(nl)

    if any(MK in x for x in lines):
        print("  [=] KPlayer.cpp da co " + MK)
        return

    # neo: dong goi FkAutoSetBlur(FALSE) - phai DUY NHAT
    hit = [i for i, x in enumerate(lines) if "FkAutoSetBlur(FALSE)" in x]
    if len(hit) != 1:
        raise SystemExit("  [X] neo FkAutoSetBlur(FALSE) khop %d dong (can dung 1)" % len(hit))
    i = hit[0]

    # kiem ngu canh: dong tren phai la '{' va tren nua phai la if(!...m_FightMode)
    if lines[i - 1].strip() != "{" or "m_FightMode" not in lines[i - 2]:
        raise SystemExit("  [X] ngu canh la: %r / %r" % (lines[i - 2], lines[i - 1]))

    them = [
        T * 2 + "// " + MK + " GOC cua \"bong mo chay mot luc roi mat hien luon\": lenh xoa ben duoi chay MOI NHIP khi",
        T * 2 + "// nhan vat khong o tu the chien dau, ma JX1 TU ROI khoi tu the do sau vai giay khong danh -> no dam",
        T * 2 + "// thang vao cong tac bong mo o KNpc::Activate " + "[VHTD 02/09r]" + " va luon thang.",
        T * 2 + "// Bien thanh lenh QUYET DINH thay vi noi long: chi rieng luc DANG DI/CHAY va DANG co buff walkrunshadow",
        T * 2 + "// (Huyen Nhan Van Yen 1358) thi bat bong mo; MOI truong hop con lai giu nguyen lenh xoa cu.",
        T * 2 + "// Nho vay DUNG YEN / CHET / HOI SINH / PHU VE THANH van duoc la chan chong \"ao anh\" bao ve nhu truoc,",
        T * 2 + "// va xac khong the nha anh mo (may chu ngung gui goi dong bo cho xac -> client se dong bang co bong mo).",
        T * 2 + "if ((Npc[m_nIndex].m_Doing == do_walk || Npc[m_nIndex].m_Doing == do_run) && Npc[m_nIndex].m_WalkRun.nTime > 0)",
        T * 3 + "Npc[m_nIndex].FkAutoSetBlur(TRUE);",
        T * 2 + "else",
    ]
    lines[i:i] = them

    out = nl.join(lines)
    nb = out.encode("latin-1")
    if bom and nb[:3] != b"\xef\xbb\xbf":
        raise SystemExit("  [X] mat BOM")
    cao = sum(1 for c in nb if c >= 0x80)
    if cao != cao_truoc:
        raise SystemExit("  [X] lech byte cao %d -> %d" % (cao_truoc, cao))
    if b"\xef\xbf\xbd" in nb:
        raise SystemExit("  [X] co EF BF BD")

    print("  [+] KPlayer::Active (nhanh client): lenh xoa bong mo -> co dieu kien, giu nguyen dong goc")
    if not KIEM:
        io.open(P, "wb").write(nb)
    print("  => %s %s" % ("KIEM" if KIEM else "GHI", P))


if __name__ == "__main__":
    print("vhtd_engine_patch13 %s%s" % (MK, " (KIEM)" if KIEM else ""))
    main()
    print("XONG. Chi anh huong CLIENT -> build CoreClient (Win32).")
