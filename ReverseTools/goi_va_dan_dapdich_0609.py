#!/usr/bin/env python3
# -*- coding: ascii -*-
"""
goi_va_dan_dapdich_0609.py

[DANDAP 06/09] Bo sung BUOC DAP DICH cho dan truy duoi (MISSLE_MMK_Follow) -
phan ban Linux co ma du an JX1 con thieu.

Do duoc tu nhi phan D:/ServerLinux/server1/jx_linux_y:
  KMissle::Fly 0x080758E0, nhanh MoveKind==5 tai 0x08075D58
    - moi 8 nhip nan huong (JX1 DA CO), NGOAI RA con:
        GetMpsPos(muc tieu) -> ghi vao m_nDesX/m_nDesY (+0x88/+0x8C)
        m_nParam2 = nDistance / m_nSpeed + 1        (so nhip toi dich)
    - moi nhip (0x08075DA0): dem lui; khi ve 0 thi di MOT buoc
        nDOffset = (m_nDes - vi_tri_hien_tai) << 10
      dat dan DUNG len toa do dich da ghi. Dan vot qua -> buoc nay di NGUOC
      => hieu ung "bay toi roi bay lui" ma chu game thay o ban Linux
      (2 chieu Cai Bang: 357 Phi Long Tai Thien / 359 Thien Ha Vo Cau).

JX1 da co san 3 truong m_nDesMapX/m_nDesMapY/m_nDesRegion (KMissle.h:110-112,
chu thich goc "toa do muc tieu luc sinh dan") nhung CHI GAN 0, khong doc o dau.
Va nay dung lai dung hai truong do + them mot bo dem m_nFollowArrive.

KHONG dung toi: cua chan huy bam muc tieu bat tu / an than cua du an
(KMissle.cpp:506-511) - giu nguyen theo yeu cau chu game.

Chay lai duoc (idempotent). Doc/ghi latin-1, giu nguyen EOL va byte TCVN3.
"""
import io
import os
import shutil

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
NHAN = "[DANDAP 06/09]"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, d):
    io.open(p, "w", encoding="latin-1", newline="").write(d)


def eol(d):
    return "\r\n" if "\r\n" in d else "\n"


def cao(p):
    b = p + ".truoc_dandap_0609"
    if not os.path.exists(b):
        shutil.copyfile(p, b)
        print("  sao luu -> " + os.path.basename(b))


def kiem_cao(truoc, sau, ten):
    hb_t = sum(1 for c in truoc if ord(c) >= 0x80)
    hb_s = sum(1 for c in sau if ord(c) >= 0x80)
    assert hb_t == hb_s, "%s: so byte cao doi %d -> %d" % (ten, hb_t, hb_s)
    assert "\xef\xbf\xbd" not in sau, ten + ": xuat hien FFFD"


def chen_sau_dong(d, moc, them, nl):
    """Chen 'them' thanh mot dong moi ngay SAU dong chua 'moc'."""
    assert d.count(moc) == 1, "moc khong duy nhat (%d lan): %r" % (d.count(moc), moc)
    i = d.find(moc)
    het = d.find("\n", i)
    assert het >= 0
    return d[:het + 1] + them + nl + d[het + 1:]


def thay(d, cu, moi):
    assert d.count(cu) == 1, "neo khong duy nhat (%d lan): %r" % (d.count(cu), cu[:70])
    return d.replace(cu, moi)


# ---------------------------------------------------------------- KMissle.h
def va_h():
    p = os.path.join(SRC, "KMissle.h")
    d0 = doc(p)
    if NHAN in d0:
        print("KMissle.h : da va roi, bo qua")
        return False
    cao(p)
    nl = eol(d0)
    them = ("\tint\t\t\t\t\tm_nFollowArrive;\t\t// " + NHAN +
            " so nhip con lai toi vi tri dich da ghi (0 = chua co)")
    d = chen_sau_dong(d0, "m_nDesRegion;", them, nl)
    kiem_cao(d0, d, "KMissle.h")
    ghi(p, d)
    print("KMissle.h : them m_nFollowArrive")
    return True


# -------------------------------------------------------------- KMissle.cpp
def va_cpp():
    p = os.path.join(SRC, "KMissle.cpp")
    d0 = doc(p)
    if NHAN in d0:
        print("KMissle.cpp: da va roi, bo qua")
        return False
    cao(p)
    nl = eol(d0)
    d = d0

    # 1) khoi tao
    d = chen_sau_dong(d, "\tm_nDesRegion = 0;", "\tm_nFollowArrive = 0;", nl)

    # 2) operator= : dat lai khi tai su dung o dan
    d = chen_sau_dong(d, "Missle.m_nDesRegion\t\t=\t0;",
                      "\tMissle.m_nFollowArrive\t=\t0;", nl)

    # 3) trong khoi nan huong: ghi lai toa do dich + so nhip toi noi
    cu = ("\t\t\t\t\t\t\tm_nXFactor = nXFactor;" + nl +
          "\t\t\t\t\t\t\tm_nYFactor = nYFactor;" + nl)
    moi = (cu +
           "\t\t\t\t\t\t\t// " + NHAN + " ghi lai vi tri dich + so nhip bay toi noi" + nl +
           "\t\t\t\t\t\t\t// (ban Linux 0x08075F35: m_nDesX/Y va m_nParam2 = dist/speed + 1)" + nl +
           "\t\t\t\t\t\t\tm_nDesMapX = nDesMpsX;" + nl +
           "\t\t\t\t\t\t\tm_nDesMapY = nDesMpsY;" + nl +
           "\t\t\t\t\t\t\tm_nFollowArrive = (m_nSpeed > 0) ? (nDistance / m_nSpeed + 1) : 0;" + nl)
    d = thay(d, cu, moi)

    # 4) buoc dap dich thay cho buoc deu
    cu = ("\t\t\tnDOffsetX\t = m_nXFactor * m_nSpeed;" + nl +
          "\t\t\tnDOffsetY\t = m_nYFactor * m_nSpeed;" + nl +
          "\t\t}break;" + nl)
    moi = ("\t\t\t// " + NHAN + " BUOC DAP DICH - ban Linux co (0x08075DA0), JX1 truoc day thieu." + nl +
           "\t\t\t// Dem lui so nhip toi dich; dung nhip toi noi thi di MOT buoc dat dan DUNG" + nl +
           "\t\t\t// len toa do dich da ghi. Dan da vot qua thi buoc nay di NGUOC lai - dung la" + nl +
           "\t\t\t// hieu ung 'bay toi roi bay lui' cua hai chieu Cai Bang 357/359 ben Linux." + nl +
           "\t\t\tif (m_nFollowArrive > 0 && --m_nFollowArrive == 0)" + nl +
           "\t\t\t{" + nl +
           "\t\t\t\tint nCurMpsX = 0;" + nl +
           "\t\t\t\tint nCurMpsY = 0;" + nl +
           "\t\t\t\tSubWorld[m_nSubWorldId].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, &nCurMpsX, &nCurMpsY);" + nl +
           "\t\t\t\tnDOffsetX\t = (m_nDesMapX - nCurMpsX) << 10;" + nl +
           "\t\t\t\tnDOffsetY\t = (m_nDesMapY - nCurMpsY) << 10;" + nl +
           "\t\t\t\tAUTOLOG_EVERY(1000, \"[MIS-FLY-ARRIVE] id=%d skill=%d follow=%d cur=%d,%d des=%d,%d dOff=%d,%d\", m_nMissleId, m_nSkillId, m_nFollowNpcIdx, nCurMpsX, nCurMpsY, m_nDesMapX, m_nDesMapY, nDOffsetX, nDOffsetY);" + nl +
           "\t\t\t}" + nl +
           "\t\t\telse" + nl +
           "\t\t\t{" + nl +
           "\t\t\t\tnDOffsetX\t = m_nXFactor * m_nSpeed;" + nl +
           "\t\t\t\tnDOffsetY\t = m_nYFactor * m_nSpeed;" + nl +
           "\t\t\t}" + nl +
           "\t\t}break;" + nl)
    d = thay(d, cu, moi)

    kiem_cao(d0, d, "KMissle.cpp")
    ghi(p, d)
    print("KMissle.cpp: khoi tao + operator= + ghi dich + buoc dap dich")
    return True


if __name__ == "__main__":
    n = 0
    n += 1 if va_h() else 0
    n += 1 if va_cpp() else 0
    print("xong, %d tep da sua" % n)
