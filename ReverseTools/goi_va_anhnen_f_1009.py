# -*- coding: utf-8 -*-
"""goi_va_anhnen_f_1009.py - [ANHNEN 10/09 f] SUA GOC "khong thay anh nen" + "may nhu dap duoi chan".

Do duoc (jx_rep3.log, dau do [ANHNEN 10/09 e]):
  bitmap16 background.jpg: CO | tai -15,0 | co 1648x2176 | tex 1648x2176 | ptex 12A52168
  | cull 1 tron 1 (5/6) at 0 | op 4 a1 2 a2 0        <- anh CO texture, trang thai ve DUNG, ve kin man hinh
  ngay sau do: bitmap16 _*PlaceGround*_#~4~#_ ... tai 81920,110592   <- anh NEN DAT dung san ve DE LEN

=> Anh nen bi chinh anh nen dat dung san (512x512, DUC) phu kin. Cho vuc sau khong co o dat thi anh dung
   san van la mau DEN duc. Ban goc da luong truoc viec nay: KScenePlaceRegionC::PaintGround(BOOL) co
   duong PaintGroundDirect() ve TUNG o dat (co cho trong) khi dang co anh nen - dung khi PaintBackGround()
   tra FALSE. Ta chua noi day: VeLopCanh(1) khong bao ve Paint() nen van di duong anh dung san.

Sua:
  1. VeLopCanh tra BOOL: co ve anh nen kieu 1 hay khong. Co thi Paint() dat bPrerenderGroundImg = FALSE
     -> nen dat ve truc tiep, cho trong de lo anh nen ra (dung y ban goc va ban 2.0).
  2. Lop kieu 3 (背景覆盖层 - lop phu NEN) ve NGAY SAU anh nen va TRUOC nen dat, khong phai sau nen dat:
     may xa chi hien o cho nen dat trong (vuc sau), con cho co dat thi dat che di. Truoc day ve sau nen
     dat nen may nam tren mat dat, nhin nhu nguoi choi dap len may.
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[ANHNEN 10/09 f]"
F_H = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/Scene/KScenePlaceC.h"
F_C = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/Scene/KScenePlaceC.cpp"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n"), s.count("\r\n")


def ghi(p, s, h0, lf0, crlf0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong)); sys.exit(1)
    return s.replace(old, new)


# ---------------- header: doi kieu tra ve
P = F_H
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, T + "void" + T + "VeLopCanh(int nKieu);", T + "BOOL" + T + "VeLopCanh(int nKieu);" + T + "// " + TAG + " tra TRUE neu co ve anh nen kieu 1", "H VeLopCanh")
    ghi(P, s, h0, lf0, crlf0, "KScenePlaceC.h")
else:
    print("KScenePlaceC.h da co")

# ---------------- cpp
P = F_C
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    # 1. than ham tra BOOL
    s = rep(s, "void KScenePlaceC::VeLopCanh(int nKieu)" + NL + "{" + NL +
            T + "if (m_nLopCanh <= 0 || g_pRepresent == NULL)" + NL + T*2 + "return;" + NL,
            "BOOL KScenePlaceC::VeLopCanh(int nKieu)" + NL + "{" + NL +
            T + "BOOL bCoVe = FALSE;" + T + "// " + TAG + NL +
            T + "if (m_nLopCanh <= 0 || g_pRepresent == NULL)" + NL + T*2 + "return bCoVe;" + NL, "C VeLopCanh dau")
    s = rep(s, T*2 + "if (nKieu == 1)" + NL + T*3 + "VeLopNen(p);" + NL + T*2 + "else" + NL + T*3 + "VeLopMay(p);" + NL + T + "}" + NL + "}" + NL,
            T*2 + "if (nKieu == 1)" + NL + T*3 + "{ VeLopNen(p); bCoVe = TRUE; }" + NL + T*2 + "else" + NL + T*3 + "VeLopMay(p);" + NL + T + "}" + NL + T + "return bCoVe;" + NL + "}" + NL, "C VeLopCanh cuoi")
    # 2. Paint: anh nen tat anh dung san, lop phu nen ve truoc nen dat
    s = rep(s, T + "VeLopCanh(1);" + T + "// [ANHNEN 10/09] anh nen ve TRUOC nen dat" + NL,
            T + "if (VeLopCanh(1))" + T + "// " + TAG + " co anh nen -> ve nen dat TRUC TIEP (co cho trong) de lo anh nen ra" + NL +
            T*2 + "bPrerenderGroundImg = FALSE;" + NL +
            T + "VeLopCanh(3);" + T + "// " + TAG + " lop phu nen (may xa) ve tren anh nen, DUOI nen dat" + NL, "C Paint kieu 1")
    s = rep(s, T + "VeLopCanh(3);" + T + "// [ANHNEN 10/09] lop phu nen (may xa) ve sau nen dat, truoc vat the" + NL, "", "C bo kieu 3 cu")
    ghi(P, s, h0, lf0, crlf0, "KScenePlaceC.cpp")
else:
    print("KScenePlaceC.cpp da co")
print("XONG " + TAG)
