# -*- coding: utf-8 -*-
"""goi_va_wauto_congthanh.py - WAuto tu tham gia CONG THANH CHIEN (03/09/2026).

Vá idempotent 9 tệp (chạy lại không đổi gì); --thu = chỉ kiểm neo, không ghi.
  Core/Src/CoreShell.h     : ATYPE_CONGTHANH (cuối enum)
  Core/Src/CoreShell.cpp   : #include KCongThanhTables.h + khối CT_Process (từ wauto_ct/ct_block.cpp.txt,
                             UTF-8 -> TCVN3) + gác nCTHold trong TK_Process + case ATYPE_CONGTHANH
  Core/Src/KPlayer.h       : trường nCT*/uCT* trong ExtAuto + khởi tạo
  S3Client/S3Client.cpp    : gọi máy CT trước Tống Kim, cổng nBS
  ipc_shared.h (3 bản)     : 12 trường cấu hình ở CUỐI struct autoData + ctor
  WAuto: Resource.h, WAuto.rc (UTF-16), WAuto.cpp (UTF-16) ở E:\\Src_Auto_Ngoai + mirror D:\\GAMEDEVNEW\\WAutoUI

Luật: tệp TCVN3 đọc/ghi latin-1 newline='' (không phá byte cao); UTF-16 đọc/ghi utf-16 newline='';
mọi hunk C++ là ASCII trừ khối CT (đếm byte cao trước/sau phải khớp).
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes  # noqa: E402

ROOT = r"D:\GAMEDEVNEW"
CORE = os.path.join(ROOT, "Sources", "Core", "Src")
S3C = os.path.join(ROOT, "Sources", "S3Client", "S3Client.cpp")
WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = os.path.join(ROOT, "WAutoUI")
HERE = os.path.dirname(os.path.abspath(__file__))
BLOCK = os.path.join(HERE, "wauto_ct", "ct_block.cpp.txt")
THU = "--thu" in sys.argv
LOI = []


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def rd(p, enc="latin-1"):
    with io.open(p, "r", encoding=enc, newline="") as f:
        return f.read()


def wr(p, s, enc="latin-1"):
    if THU:
        return
    with io.open(p, "w", encoding=enc, newline="") as f:
        f.write(s)


def crlf(s):
    return s.replace("\r\n", "\n").replace("\n", "\r\n")


class Tep:
    def __init__(self, path, enc="latin-1"):
        self.path = path
        self.enc = enc
        self.s = rd(path, enc)
        self.goc = self.s
        self.nl = "\r\n" if self.s.count("\r\n") > self.s.count("\n") // 2 else "\n"
        self.doi = 0

    def fmt(self, s):
        return crlf(s) if self.nl == "\r\n" else s.replace("\r\n", "\n")

    def them(self, neo, moi, sau=True, dau=None):
        """chèn `moi` sau (hay trước) dòng chứa `neo` (neo phải khớp đúng 1 lần). `dau` = chuỗi
        chứng tỏ đã áp (mặc định = 1 dòng đầu không rỗng của `moi`)."""
        neo = self.fmt(neo)
        moi = self.fmt(moi)
        dau = self.fmt(dau) if dau else moi.strip().splitlines()[0].strip()
        if dau in self.s:
            return
        n = self.s.count(neo)
        if n != 1:
            LOI.append("%s: neo khop %d lan (phai 1): %r" % (os.path.basename(self.path), n, neo[:70]))
            return
        i = self.s.index(neo)
        if sau:
            j = self.s.find(self.nl, i)
            j = len(self.s) if j < 0 else j + len(self.nl)
            self.s = self.s[:j] + moi + self.s[j:]
        else:
            j = self.s.rfind(self.nl, 0, i)
            j = 0 if j < 0 else j + len(self.nl)
            self.s = self.s[:j] + moi + self.s[j:]
        self.doi += 1

    def thay(self, cu, moi):
        """thay đúng 1 lần `cu` -> `moi`; đã áp nếu `moi` có sẵn và `cu` không còn."""
        cu = self.fmt(cu)
        moi = self.fmt(moi)
        if moi in self.s:
            return		# da ap (ke ca khi `moi` chua `cu` - vd them mot phan tu vao cuoi danh sach)
        n = self.s.count(cu)
        if n != 1:
            LOI.append("%s: chuoi thay khop %d lan (phai 1): %r" % (os.path.basename(self.path), n, cu[:70]))
            return
        self.s = self.s.replace(cu, moi)
        self.doi += 1

    def ghi(self):
        if self.s == self.goc:
            print("  %-16s khong doi" % os.path.basename(self.path))
            return
        print("  %-16s %d hunk, high byte %d -> %d, dong %d -> %d" % (
            os.path.basename(self.path), self.doi, hi(self.goc), hi(self.s),
            self.goc.count("\n"), self.s.count("\n")))
        wr(self.path, self.s, self.enc)


# =========================================================================
# 1. CoreShell.h - enum
# =========================================================================
def va_coreshell_h():
    t = Tep(os.path.join(CORE, "CoreShell.h"))
    t.them("\tATYPE_MAPSUKIEN,",
           "\tATYPE_CONGTHANH,\t// (03/09) may Cong Thanh Chien cho nguoi choi that (CT_Process)\n")
    t.ghi()


# =========================================================================
# 2. KPlayer.h - ExtAuto
# =========================================================================
KP_FIELDS = """\t// == Cong Thanh Chien (03/09/2026) - trang thai may ATYPE_CONGTHANH, POD (ATYPE_CLEAR memset) ==
\tint  nCTPhase;       // pha CTP_* (CoreShell.cpp)
\tint  nCTStep;        // buoc con trong pha (dap trap: 0 di toi / 1 lui ra)
\tint  nCTHold;        // 0 tha may / 1 cam lai / 2 trong tran (may PK danh uNpcID)
\tint  nCTTry;         // dem thu lai trong pha
\tint  nCTKey;         // yymmdd (gio may chu) ngay da chay xong / bo
\tint  nCTPhe;         // phe da vao: 1 = thu, 2 = cong
\tint  nCTBackMap;     // map truoc khi di (de bao cao)
\tint  nCTCongChet;    // bitmask cong da vo (bit i = cong i, ben cong)
\tint  nCTTruXong;     // bitmask tru da chiem / da xem (ben cong)
\tint  nCTMucTieu;     // tru dang canh / diem tuan tra (1-based), 0 = chua
\tint  nCTChet;        // so mang da chet trong tran
\tint  nCTDaVao;       // 1 = da tung dung tren map 221 trong luot nay
\tint  nCTHetTran;     // 1 = da nghe loa "Chung cuoc" (tran ket thuc)
\tUINT uCTNext;        // moc hanh dong ke tiep
\tUINT uCTPhaseT;      // moc vao pha (bat ket pha)
\tUINT uCTDlgSeen;     // uDlgSeq da xu ly
\tUINT uCTNewsSeen;    // uNewsSeq (tin toan may chu) da xu ly
\tUINT uCTMsgT;        // throttle bao trang thai
\tUINT uCTMoT;         // han cua so vao theo LOA khai chien (0 = khong co)
\tUINT uCTTrapT;       // moc dung tren o trap ma khong kich (de lui ra dap lai)
\tUINT uCTMucT;        // moc doi tru dang canh (ben thu)
\tchar szCTCong[32];   // ten bang ben cong (doc tu thoai Xa Phu)
\tchar szCTThu[32];    // ten bang ben thu
"""
KP_INIT = """\t\tnCTPhase = 0;
\t\tnCTStep = 0;
\t\tnCTHold = 0;
\t\tnCTTry = 0;
\t\tnCTKey = 0;
\t\tnCTPhe = 0;
\t\tnCTBackMap = 0;
\t\tnCTCongChet = 0;
\t\tnCTTruXong = 0;
\t\tnCTMucTieu = 0;
\t\tnCTChet = 0;
\t\tnCTDaVao = 0;
\t\tnCTHetTran = 0;
\t\tuCTNext = 0;
\t\tuCTPhaseT = 0;
\t\tuCTDlgSeen = 0;
\t\tuCTNewsSeen = 0;
\t\tuCTMsgT = 0;
\t\tuCTMoT = 0;
\t\tuCTTrapT = 0;
\t\tuCTMucT = 0;
\t\tszCTCong[0] = 0;
\t\tszCTThu[0] = 0;
"""


def va_kplayer_h():
    t = Tep(os.path.join(CORE, "KPlayer.h"))
    t.them("\tUINT uSTVongT;", KP_FIELDS)
    t.them("\t\tuSTVongT = 0;", KP_INIT)
    t.ghi()


# =========================================================================
# 3. CoreShell.cpp - include + khoi CT + gac TK + case
# =========================================================================
def va_coreshell_cpp():
    t = Tep(os.path.join(CORE, "CoreShell.cpp"))
    t.them('#include "KHoatDongTables.h"', '#include "KCongThanhTables.h"\n')
    with io.open(BLOCK, "r", encoding="utf-8", newline="") as f:
        blk = f.read()
    blk = unicode_to_tcvn3_bytes(blk).decode("latin-1").replace("\r\n", "\n")
    if not blk.endswith("\n"):
        blk += "\n"
    hi_blk = hi(blk)
    hi_truoc = hi(t.s)
    t.them("// ==================== HET AUTO TONG KIM ====================", blk,
           dau="// ==================== AUTO CONG THANH CHIEN (03/09/2026) ====================")
    da_co_khoi = (hi(t.s) == hi_truoc)
    t.them("\t// LOA may chu la duong vao CHINH: admin doi gio trong TAB_TIME_TONG_KIM",
           "\t// (03/09) may Cong Thanh Chien dang cam lai -> Tong Kim nhuong (hai may loai tru nhau;\n"
           "\t// S3Client goi CT truoc TK nen binh thuong khong toi day, giu lam luoi do)\n"
           "\tif (ea.nCTHold)\n"
           "\t\treturn 0;\n", sau=False)
    t.them("\t\t\t\tcase ATYPE_MAPSUKIEN:",
           "\t\t\t\tcase ATYPE_CONGTHANH:\n"
           "\t\t\t\t{\n"
           "\t\t\t\t\treturn CT_Process(nPlayerIdx, (const autoData*)nParam, uCurTime);\n"
           "\t\t\t\t}\n", sau=False)
    if not da_co_khoi and hi(t.s) != hi_truoc + hi_blk:
        LOI.append("CoreShell.cpp: high byte lech: truoc %d + khoi %d != sau %d" % (hi_truoc, hi_blk, hi(t.s)))
    t.ghi()


# =========================================================================
# 4. S3Client.cpp - goi may CT truoc Tong Kim
# =========================================================================
def va_s3client():
    t = Tep(S3C)
    t.thay("\tint nTK = 0;\n\tif(pApData->bTongKim == 1)\n\t\tnTK = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TONGKIM, (int)pApData);",
           "\t// [CongThanh] (03/09) may Cong Thanh Chien - goi TRUOC Tong Kim: tran cong thanh\n"
           "\t// moi tuan mot lan cho moi thanh, Tong Kim ngay 4 tran. Hai may loai tru nhau:\n"
           "\t// CT_Process tu nhuong khi Tong Kim dang cam lai (nTKHold), TK_Process nhuong khi\n"
           "\t// Cong Thanh dang cam lai (nCTHold). 0 = tha may; 1 = cam lai; 2 = trong tran.\n"
           "\tint nCT = 0;\n"
           "\tif(pApData->bCongThanh == 1)\n"
           "\t\tnCT = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_CONGTHANH, (int)pApData);\n"
           "\tint nTK = 0;\n"
           "\tif(pApData->bTongKim == 1 && nCT == 0)\n"
           "\t\tnTK = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TONGKIM, (int)pApData);")
    t.thay("\tif(pApData->bLienDau == 1 && nTK == 0)\n",
           "\tif(pApData->bLienDau == 1 && nTK == 0 && nCT == 0)\n")
    t.thay("\t\t&& nTK == 0 && nLD == 0)\n\t\tnHD = ",
           "\t\t&& nTK == 0 && nLD == 0 && nCT == 0)\n\t\tnHD = ")
    t.thay("\tif(pApData->bSatThu == 1 && nTK == 0 && nLD == 0 && nHD == 0)\n",
           "\tif(pApData->bSatThu == 1 && nTK == 0 && nLD == 0 && nHD == 0 && nCT == 0)\n")
    t.thay("\tconst int nBS = nTK ? nTK : (nLD ? nLD : (nHD ? nHD : nST));",
           "\tconst int nBS = nCT ? nCT : (nTK ? nTK : (nLD ? nLD : (nHD ? nHD : nST)));")
    t.thay('\tAUTOLOG_EVERY(5000, "[HD-GATE] nTK=%d nLD=%d nHD=%d nST=%d nBS=%d nSK=%d | bat: TK=%d LD=%d BN=%d BC=%d TS=%d ST=%d DT=%d",\n'
           "\t\tnTK, nLD, nHD, nST, nBS, nSK, pApData->bTongKim, pApData->bLienDau,",
           '\tAUTOLOG_EVERY(5000, "[HD-GATE] nCT=%d nTK=%d nLD=%d nHD=%d nST=%d nBS=%d nSK=%d | bat: CT=%d TK=%d LD=%d BN=%d BC=%d TS=%d ST=%d DT=%d",\n'
           "\t\tnCT, nTK, nLD, nHD, nST, nBS, nSK, pApData->bCongThanh, pApData->bTongKim, pApData->bLienDau,")
    t.ghi()


# =========================================================================
# 5. ipc_shared.h (Core) -> chep sang 2 ban kia
# =========================================================================
IPC_FIELDS = """\t// == Cong Thanh Chien (03/09/2026) - PHAI o CUOI struct, truoc constructor ==
\t// (file APdata\\<ID nhan vat>.dat ghi NGUYEN struct - them truong o GIUA la nat cau
\t//  hinh cua moi nguoi choi cu; LoadRoleData di tru theo offsetof(autoData, bCongThanh))
\tint\t\tbCongThanh;\t\t// bat/tat auto Cong Thanh Chien
\tint\t\tnCTGio;\t\t\t// gio khai chien (gio MAY CHU + nCTLech) - lich may chu 20h00
\tint\t\tnCTPhut;
\tint\t\tnCTLech;\t\t// gio may chu lech gio may nay bao nhieu PHUT (co the am)
\tint\t\tnCTSom;\t\t\t// toi Xa Phu som may phut truoc gio khai chien
\tint\t\tnCTCua;\t\t\t// con vao tran trong bao nhieu phut sau moc gio (tran 90 phut)
\tint\t\tnCTThanh;\t\t// thanh toi gap Xa Phu: 0..6 theo g_LDVeMap, 7 = thanh dang dung
\tint\t\tnCTPhe;\t\t\t// 0 = tu theo bang hoi, 1 = ben cong, 2 = ben thu (khi co lenh bai)
\tint\t\tnCTCong;\t\t// ben cong: 0 = pha cong -> ha Long tru, 1 = chi danh nguoi
\tint\t\tnCTThu;\t\t\t// ben thu: 0 = canh Long tru / chiem lai, 1 = chi danh nguoi
\tint\t\tnCTVe;\t\t\t// het tran ve: 0..6 thanh theo g_LDVeMap, 7 = diem luu (Roi khoi dau truong)
\tint\t\tbCTLoa;\t\t\t// nghe loa "cong thanh chien chinh thuc bat dau" cua may chu la di ngay
"""
IPC_INIT = """\t\tbCongThanh = 0;
\t\tnCTGio = 20;
\t\tnCTPhut = 0;
\t\tnCTLech = 0;
\t\tnCTSom = 3;
\t\tnCTCua = 95;
\t\tnCTThanh = 7;
\t\tnCTPhe = 0;
\t\tnCTCong = 0;
\t\tnCTThu = 0;
\t\tnCTVe = 7;
\t\tbCTLoa = 1;
"""


def va_ipc():
    p = os.path.join(CORE, "ipc_shared.h")
    t = Tep(p)
    t.them("bComboNoUT;", IPC_FIELDS)
    t.them("bComboNoUT = 1;", IPC_INIT)
    t.ghi()
    if not THU:
        for q in (os.path.join(WA_E, "ipc_shared.h"), os.path.join(WA_D, "ipc_shared.h")):
            shutil.copyfile(p, q)
            print("  chep ipc_shared.h -> %s" % q)


# =========================================================================
# 6. Resource.h
# =========================================================================
RES_MOVE = [("IDD_SETSERIES_DIALOG", 600, 700), ("IDD_NOPICK_DIALOG", 601, 701), ("IDC_COMBO_SELNOPICK", 602, 702),
            ("IDC_LIST_NOPICK", 603, 703), ("IDC_BTN_NOPREMOVE", 604, 704), ("IDC_LIST_SERIES", 605, 705),
            ("IDC_BTN_SERUP", 606, 706), ("IDC_BTN_SERDOWN", 607, 707), ("IDC_STRING_HOMEPAGE", 608, 708),
            ("IDC_TABBTN_9", 609, 709), ("IDC_TABBTN_10", 610, 710), ("IDC_TABBTN_11", 611, 711),
            ("IDC_STRING_HD_L", 612, 712), ("IDC_STRING_HD_V", 613, 713), ("IDC_TABBTN_12", 614, 714),
            ("IDC_TABBTN_13", 615, 715)]
RES_NEW = """// == tab 14 (Cong Thanh Chien, 03/09/2026) ==
// PHAI lien mach 598..622 va < IDC_INDEX_END: ShowTab hien tab nay bang mot dai duy nhat.
// Khoi ID ngoai dai (hop thoai roi, nut tab, dong trang thai) doi 600-615 -> 700-715 de
// nuong cho; IDC_INDEX_END 598 -> 640.
#define IDC_CHECKBOX_15_ON\t598
#define IDC_STRING_15_GIO\t599
#define IDC_EDITOR_15_GIO\t600
#define IDC_STRING_15_H\t\t601
#define IDC_EDITOR_15_PHUT\t602
#define IDC_STRING_15_LECH\t603
#define IDC_EDITOR_15_LECH\t604
#define IDC_STRING_15_SOM\t605
#define IDC_EDITOR_15_SOM\t606
#define IDC_STRING_15_CUA\t607
#define IDC_EDITOR_15_CUA\t608
#define IDC_STRING_15_BD\t609
#define IDC_COMBO_15_BD\t\t610
#define IDC_STRING_15_PHE\t611
#define IDC_COMBO_15_PHE\t612
#define IDC_STRING_15_CONG\t613
#define IDC_COMBO_15_CONG\t614
#define IDC_STRING_15_THU\t615
#define IDC_COMBO_15_THU\t616
#define IDC_STRING_15_VE\t617
#define IDC_COMBO_15_VE\t\t618
#define IDC_CHECKBOX_15_LOA\t619
#define IDC_GRP_TAB14\t\t620
#define IDC_SEP_14A\t\t\t621
#define IDC_SEP_14B\t\t\t622
"""


def va_resource():
    p = os.path.join(WA_E, "Resource.h")
    t = Tep(p)
    if "IDC_CHECKBOX_15_ON" not in t.s:
        for name, cu, moi in RES_MOVE:
            pat = re.compile(r"(#define\s+%s\s+)%d(\s*\r?\n)" % (re.escape(name), cu))
            if len(pat.findall(t.s)) != 1:
                LOI.append("Resource.h: %s %d khop %d lan" % (name, cu, len(pat.findall(t.s))))
                continue
            t.s = pat.sub(lambda m: m.group(1) + str(moi) + m.group(2), t.s)
        pat = re.compile(r"(#define\s+IDC_INDEX_END\s+)598(\s*\r?\n)")
        if len(pat.findall(t.s)) != 1:
            LOI.append("Resource.h: IDC_INDEX_END 598 khong thay")
        t.s = pat.sub(lambda m: m.group(1) + "640" + m.group(2), t.s)
        t.doi += 1
        t.them("#define IDC_INDEX_END", RES_NEW, sau=False)
    t.ghi()
    if not THU:
        shutil.copyfile(p, os.path.join(WA_D, "Resource.h"))


# =========================================================================
# 7. WAuto.rc (UTF-16)
# =========================================================================
RC_NEW = """\t//tab 14 - Cong Thanh Chien (03/09/2026)
\tGROUPBOX "Công Thành · tự tham chiến đúng giờ", IDC_GRP_TAB14, 2, 112, 156, 135
\tCONTROL "", IDC_SEP_14A, "Static", SS_ETCHEDHORZ, 6, 161, 148, 1
\tCONTROL "", IDC_SEP_14B, "Static", SS_ETCHEDHORZ, 6, 217, 148, 1
\t\tCONTROL "Bật auto Công Thành Chiến", IDC_CHECKBOX_15_ON, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 124, 152, 10
\t\tLTEXT "Khai chiến:", IDC_STRING_15_GIO, 4, 136, 42, 10, SS_CENTERIMAGE
\t\tEDITTEXT IDC_EDITOR_15_GIO, 46, 136, 18, 10, ES_NUMBER | WS_TABSTOP
\t\tLTEXT ":", IDC_STRING_15_H, 65, 136, 5, 10, SS_CENTERIMAGE
\t\tEDITTEXT IDC_EDITOR_15_PHUT, 70, 136, 18, 10, ES_NUMBER | WS_TABSTOP
\t\tLTEXT "Lệch giờ:", IDC_STRING_15_LECH, 94, 136, 34, 10, SS_CENTERIMAGE
\t\tEDITTEXT IDC_EDITOR_15_LECH, 128, 136, 28, 10, WS_TABSTOP
\t\tLTEXT "Đi sớm (phút):", IDC_STRING_15_SOM, 4, 148, 54, 10, SS_CENTERIMAGE
\t\tEDITTEXT IDC_EDITOR_15_SOM, 60, 148, 20, 10, ES_NUMBER | WS_TABSTOP
\t\tLTEXT "Cửa sổ (phút):", IDC_STRING_15_CUA, 94, 148, 40, 10, SS_CENTERIMAGE
\t\tEDITTEXT IDC_EDITOR_15_CUA, 134, 148, 22, 10, ES_NUMBER | WS_TABSTOP
\t\tLTEXT "Thành báo danh:", IDC_STRING_15_BD, 4, 166, 60, 10, SS_CENTERIMAGE
\t\tCOMBOBOX IDC_COMBO_15_BD, 66, 165, 90, 140, WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS
\t\tLTEXT "Phe:", IDC_STRING_15_PHE, 4, 179, 30, 10, SS_CENTERIMAGE
\t\tCOMBOBOX IDC_COMBO_15_PHE, 36, 178, 120, 60, WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS
\t\tLTEXT "Bên công:", IDC_STRING_15_CONG, 4, 192, 40, 10, SS_CENTERIMAGE
\t\tCOMBOBOX IDC_COMBO_15_CONG, 46, 191, 110, 60, WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS
\t\tLTEXT "Bên thủ:", IDC_STRING_15_THU, 4, 205, 40, 10, SS_CENTERIMAGE
\t\tCOMBOBOX IDC_COMBO_15_THU, 46, 204, 110, 60, WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS
\t\tLTEXT "Hết trận về:", IDC_STRING_15_VE, 4, 222, 48, 10, SS_CENTERIMAGE
\t\tCOMBOBOX IDC_COMBO_15_VE, 54, 221, 102, 140, WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS
\t\tCONTROL "Nghe loa khai chiến của máy chủ là đi ngay", IDC_CHECKBOX_15_LOA, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 234, 152, 10
"""


def va_rc():
    p = os.path.join(WA_E, "WAuto.rc")
    t = Tep(p, "utf-16")
    t.them('    CTEXT "Võ Lâm Ngạo Thế",IDC_STRING_HOMEPAGE', RC_NEW, sau=False)
    t.ghi()
    if not THU:
        shutil.copyfile(p, os.path.join(WA_D, "WAuto.rc"))


# =========================================================================
# 8. WAuto.cpp (UTF-16)
# =========================================================================
CPP_SAVE = """\t// tab 14 Cong Thanh Chien (03/09)
\tgnode.apdata.bCongThanh = (IsDlgButtonChecked(hDlg, IDC_CHECKBOX_15_ON) == BST_CHECKED);
\tGetDlgItemTextA(hDlg, IDC_EDITOR_15_GIO, szRootPath, MAX_PATH);
\tgnode.apdata.nCTGio = atoi(szRootPath);
\tGetDlgItemTextA(hDlg, IDC_EDITOR_15_PHUT, szRootPath, MAX_PATH);
\tgnode.apdata.nCTPhut = atoi(szRootPath);
\tGetDlgItemTextA(hDlg, IDC_EDITOR_15_LECH, szRootPath, MAX_PATH);
\tgnode.apdata.nCTLech = atoi(szRootPath);
\tGetDlgItemTextA(hDlg, IDC_EDITOR_15_SOM, szRootPath, MAX_PATH);
\tgnode.apdata.nCTSom = atoi(szRootPath);
\tGetDlgItemTextA(hDlg, IDC_EDITOR_15_CUA, szRootPath, MAX_PATH);
\tgnode.apdata.nCTCua = atoi(szRootPath);
\tgnode.apdata.nCTThanh = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO_15_BD), CB_GETCURSEL, 0, 0);
\tif(gnode.apdata.nCTThanh < 0) gnode.apdata.nCTThanh = 7;
\tgnode.apdata.nCTPhe = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO_15_PHE), CB_GETCURSEL, 0, 0);
\tif(gnode.apdata.nCTPhe < 0) gnode.apdata.nCTPhe = 0;
\tgnode.apdata.nCTCong = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO_15_CONG), CB_GETCURSEL, 0, 0);
\tif(gnode.apdata.nCTCong < 0) gnode.apdata.nCTCong = 0;
\tgnode.apdata.nCTThu = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO_15_THU), CB_GETCURSEL, 0, 0);
\tif(gnode.apdata.nCTThu < 0) gnode.apdata.nCTThu = 0;
\tgnode.apdata.nCTVe = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO_15_VE), CB_GETCURSEL, 0, 0);
\tif(gnode.apdata.nCTVe < 0) gnode.apdata.nCTVe = 7;
\tgnode.apdata.bCTLoa = (IsDlgButtonChecked(hDlg, IDC_CHECKBOX_15_LOA) == BST_CHECKED);
"""
CPP_UI = """\t// tab 14 Cong Thanh Chien (03/09)
\tCheckDlgButton(hDlg, IDC_CHECKBOX_15_ON, gnode.apdata.bCongThanh?BST_CHECKED:BST_UNCHECKED);
\t{ wchar_t wct[16]; _itow_s(gnode.apdata.nCTGio, wct, 10); SetDlgItemText(hDlg, IDC_EDITOR_15_GIO, wct); }
\t{ wchar_t wct[16]; _itow_s(gnode.apdata.nCTPhut, wct, 10); SetDlgItemText(hDlg, IDC_EDITOR_15_PHUT, wct); }
\t{ wchar_t wct[16]; _itow_s(gnode.apdata.nCTLech, wct, 10); SetDlgItemText(hDlg, IDC_EDITOR_15_LECH, wct); }
\t{ wchar_t wct[16]; _itow_s(gnode.apdata.nCTSom, wct, 10); SetDlgItemText(hDlg, IDC_EDITOR_15_SOM, wct); }
\t{ wchar_t wct[16]; _itow_s(gnode.apdata.nCTCua, wct, 10); SetDlgItemText(hDlg, IDC_EDITOR_15_CUA, wct); }
\tComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COMBO_15_BD), gnode.apdata.nCTThanh);
\tComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COMBO_15_PHE), gnode.apdata.nCTPhe);
\tComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COMBO_15_CONG), gnode.apdata.nCTCong);
\tComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COMBO_15_THU), gnode.apdata.nCTThu);
\tComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COMBO_15_VE), gnode.apdata.nCTVe);
\tCheckDlgButton(hDlg, IDC_CHECKBOX_15_LOA, gnode.apdata.bCTLoa?BST_CHECKED:BST_UNCHECKED);
"""
CPP_DEF_BODY = """\t\tgnode.apdata.bCongThanh = 0;
\t\tgnode.apdata.nCTGio = 20;
\t\tgnode.apdata.nCTPhut = 0;
\t\tgnode.apdata.nCTLech = 0;
\t\tgnode.apdata.nCTSom = 3;
\t\tgnode.apdata.nCTCua = 95;
\t\tgnode.apdata.nCTThanh = 7;
\t\tgnode.apdata.nCTPhe = 0;
\t\tgnode.apdata.nCTCong = 0;
\t\tgnode.apdata.nCTThu = 0;
\t\tgnode.apdata.nCTVe = 7;
\t\tgnode.apdata.bCTLoa = 1;
"""
CPP_DEF = ("\t\t// tab 14 Cong Thanh Chien (03/09): mac dinh TAT; khai chien 20h00 (timerserver_ctc.lua),\n"
           "\t\t// cua so 95 phut (tran 90 phut + toi da 5 phut timer mo mission), phe theo bang, ve diem luu.\n"
           + CPP_DEF_BODY)
CPP_MIGR = ("\t\t\tgnode.apdata.bComboNoUT = 1;\n\t\t}\n"
            "\t\tif(uOldSize <= offsetof(autoData, bCongThanh))\n"
            "\t\t{\t// .dat truoc dot Cong Thanh Chien 03/09: dat mac dinh khoi CT (tinh nang van TAT)\n"
            + CPP_DEF_BODY.replace("\t\tgnode", "\t\t\tgnode")
            + "\t\t}\n\t}\n\tif(bUpUI)")
CPP_TIPS = """\t{ IDC_CHECKBOX_15_ON, L"BẬT auto Công Thành Chiến: tới giờ khai chiến (hoặc nghe loa máy chủ) tự dừng việc đang làm, ĐI BỘ tới Xa Phu của thành, chọn \\"Đi Chiến trường công thành\\", chọn phe theo bang hội của mình, sang hậu phương đạp cửa tập kết, vào doanh rồi ra trận đánh theo cấu hình tab PK. Không dùng phù, không nhảy map." },
\t{ IDC_EDITOR_15_GIO, L"Giờ khai chiến theo giờ MÁY CHỦ (lịch máy chủ: 20h00, mỗi thành một thứ trong tuần; auto vẫn đi mỗi ngày và tự thôi nếu Xa Phu báo không có trận)." },
\t{ IDC_EDITOR_15_PHUT, L"Phút khai chiến (0-59)." },
\t{ IDC_EDITOR_15_LECH, L"Giờ máy chủ trừ giờ máy này, tính bằng PHÚT (âm được)." },
\t{ IDC_EDITOR_15_SOM, L"Tới Xa Phu trước giờ khai chiến bao nhiêu phút (0-30). Chưa có trận thì cứ 60 giây hỏi lại Xa Phu." },
\t{ IDC_EDITOR_15_CUA, L"Còn vào trận trong bao nhiêu phút sau mốc giờ (trận dài 90 phút, mission có thể mở trễ tới 5 phút)." },
\t{ IDC_COMBO_15_BD, L"Thành sẽ tới để gặp Xa Phu (Xa Phu nào cũng có mục \\"Đi Chiến trường công thành\\"). Thành đang đứng = dùng Xa Phu của map hiện tại; map không có Xa Phu thì dùng phù về thành rồi đi tiếp." },
\t{ IDC_COMBO_15_PHE, L"Tự theo bang hội = đọc tên 2 bang trong lời thoại Xa Phu, bang mình là bên nào thì vào bên đó (không thuộc bang nào thì bỏ lượt). Chọn Bên công / Bên thủ khi có Thành chiến lệnh bài của phe đó." },
\t{ IDC_COMBO_15_CONG, L"Khi không có địch trong tầm nhìn tab PK: phá cổng gần nhất, cổng vỡ thì tiến vào đánh Long trụ bên thủ đang giữ (đủ 3 trụ = thắng). Chỉ đánh người = tuần tra qua các cổng và trụ." },
\t{ IDC_COMBO_15_THU, L"Khi không có địch trong tầm nhìn tab PK: đứng canh một Long trụ (đổi trụ mỗi 2 phút), thấy trụ bị chiếm thì đánh chiếm lại. Chỉ đánh người = tuần tra." },
\t{ IDC_COMBO_15_VE, L"Hết trận (máy chủ kéo về hậu phương) nhờ Xa Phu hậu phương: về thành đã chọn qua \\"Những thành thị đã đi qua\\", hoặc \\"Rời khỏi đấu trường\\" về điểm lưu (điểm lưu = thành đã lưu rương)." },
\t{ IDC_CHECKBOX_15_LOA, L"Nghe tin toàn máy chủ \\"... công thành chiến chính thức bắt đầu\\" là đi ngay, không cần đúng khung giờ. Tắt = chỉ đi theo giờ đã đặt." },
"""
CPP_SHOWTAB = """\telse if(nTabBtn == 14)
\t{\t// Cong Thanh Chien (03/09) - dai ID 598..622 lien mach
\t\tfor(i=IDC_CHECKBOX_15_ON;i<=IDC_SEP_14B;++i)
\t\t{
\t\t\tShowWindow(::GetDlgItem( hDlg, i ), SW_SHOW);
\t\t}
\t}
"""
CPP_GRP_NEW = """\tstatic const int nGrpId[15] = { IDC_GRP_TAB0, IDC_GRP_TAB1, IDC_GRP_TAB2,
\t\tIDC_GRP_TAB3, IDC_GRP_TAB4, IDC_GRP_TAB5, IDC_GRP_TAB6, IDC_GRP_TAB7, IDC_GRP_TAB8,
\t\tIDC_GRP_TAB9, IDC_GRP_TAB10, IDC_GRP_TAB11, IDC_GRP_TAB12, IDC_GRP_TAB13, IDC_GRP_TAB14 };
\tstatic const int nSepFrom[15] = { IDC_SEP_0A, IDC_SEP_1A, IDC_SEP_2A, IDC_SEP_3A,
\t\tIDC_SEP_4A, IDC_SEP_5A, IDC_SEP_6A, IDC_SEP_7A, IDC_SEP_8A, IDC_SEP_9A, IDC_SEP_10A, IDC_SEP_11A,
\t\tIDC_SEP_12A, IDC_SEP_13A, IDC_SEP_14A };
\tstatic const int nSepTo[15] = { IDC_SEP_0B, IDC_SEP_1B, IDC_SEP_2A, IDC_SEP_3A,
\t\tIDC_SEP_4B, IDC_SEP_5B, IDC_SEP_6A, IDC_SEP_7A, IDC_SEP_8B, IDC_SEP_9B, IDC_SEP_10B, IDC_SEP_11B,
\t\tIDC_SEP_12A, IDC_SEP_13A, IDC_SEP_14B };
\tif (nTabBtn >= 0 && nTabBtn <= 14)"""
CPP_COMBOS = """\t\thCtrl = ::GetDlgItem(hDlg, IDC_COMBO_15_BD);
\t\t// 7 dong dau PHAI dung thu tu bang g_LDVeMap trong KLienDauTables.h
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Phượng Tường Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Thành Đô Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Đại Lý");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Biện Kinh Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Tương Dương Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Dương Châu Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Lâm An Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Thành đang đứng (Xa Phu gần nhất)");
\t\tSendMessage(hCtrl, CB_SETCURSEL, 7, 0);
\t\thCtrl = ::GetDlgItem(hDlg, IDC_COMBO_15_PHE);
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Tự theo bang hội");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Bên công (có lệnh bài)");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Bên thủ (có lệnh bài)");
\t\tSendMessage(hCtrl, CB_SETCURSEL, 0, 0);
\t\thCtrl = ::GetDlgItem(hDlg, IDC_COMBO_15_CONG);
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Phá cổng rồi hạ Long trụ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Chỉ đánh người (tuần tra)");
\t\tSendMessage(hCtrl, CB_SETCURSEL, 0, 0);
\t\thCtrl = ::GetDlgItem(hDlg, IDC_COMBO_15_THU);
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Canh Long trụ, chiếm lại nếu mất");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Chỉ đánh người (tuần tra)");
\t\tSendMessage(hCtrl, CB_SETCURSEL, 0, 0);
\t\thCtrl = ::GetDlgItem(hDlg, IDC_COMBO_15_VE);
\t\t// 7 dong dau PHAI dung thu tu bang g_LDVeMap trong KLienDauTables.h
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Phượng Tường Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Thành Đô Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Đại Lý");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Biện Kinh Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Tương Dương Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Dương Châu Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Lâm An Phủ");
\t\tSendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)L"Điểm lưu (Rời khỏi đấu trường)");
\t\tSendMessage(hCtrl, CB_SETCURSEL, 7, 0);
"""


def va_wauto_cpp():
    p = os.path.join(WA_E, "WAuto.cpp")
    t = Tep(p, "utf-16")
    t.thay("#define WA_SO_TAB\t\t14", "#define WA_SO_TAB\t\t15")
    t.thay('{ L"Sự kiện",   { 9, 10 },             2 },', '{ L"Sự kiện",   { 9, 10, 14 },         3 },')
    t.thay('L"H.động", L"Sát thủ", L"Chiêu KH",', 'L"H.động", L"Sát thủ", L"Chiêu KH", L"Công Thành",')
    t.thay("330, 279, 279, 304, 282, 260, 266, 303, 294, 268, 326, 328, 225, 335",
           "330, 279, 279, 304, 282, 260, 266, 303, 294, 268, 326, 328, 225, 335, 246")
    t.them("\t// tab 10 Lien dau\n\tgnode.apdata.bLienDau =", CPP_SAVE, sau=False,
           dau="gnode.apdata.bCongThanh = (IsDlgButtonChecked")
    t.them("\tCheckDlgButton(hDlg, IDC_CHECKBOX_10_ON, gnode.apdata.bLienDau?BST_CHECKED:BST_UNCHECKED);", CPP_UI, sau=False,
           dau="CheckDlgButton(hDlg, IDC_CHECKBOX_15_ON,")
    t.them("\t\t// tab 10 Lien dau: tinh nang mac dinh TAT;", CPP_DEF, sau=False,
           dau="// tab 14 Cong Thanh Chien (03/09): mac dinh TAT")
    t.thay("\t\t\tgnode.apdata.bComboNoUT = 1;\n\t\t}\n\t}\n\tif(bUpUI)", CPP_MIGR)
    t.them('IDC_CHECKBOX_14_TCP, L"', CPP_TIPS, dau='IDC_CHECKBOX_15_ON, L"')
    t.them("\telse if(nTabBtn == 10)\n\t{\n\t\tfor(i=IDC_CHECKBOX_10_ON;i<=IDC_COMBO_10_BD;++i)", CPP_SHOWTAB, sau=False,
           dau="else if(nTabBtn == 14)")
    # 3 mang nGrpId/nSepFrom/nSepTo [14] -> [15] (thay ca cum vi 2 mang ket thuc giong nhau)
    if "nGrpId[15]" not in t.s:
        a = t.s.find(t.fmt("\tstatic const int nGrpId[14] = {"))
        b = t.s.find(t.fmt("\tif (nTabBtn >= 0 && nTabBtn <= 13)"))
        if a < 0 or b < 0 or b < a:
            LOI.append("WAuto.cpp: khong thay cum nGrpId[14] .. nTabBtn <= 13")
        else:
            b2 = b + len(t.fmt("\tif (nTabBtn >= 0 && nTabBtn <= 13)"))
            t.s = t.s[:a] + t.fmt(CPP_GRP_NEW) + t.s[b2:]
            t.doi += 1
    t.thay("IDC_CHECKBOX_13_GHEP, IDC_CHECKBOX_13_CHO,\n\t\t\t};",
           "IDC_CHECKBOX_13_GHEP, IDC_CHECKBOX_13_CHO,\n\t\t\t\tIDC_CHECKBOX_15_ON,\n\t\t\t};")
    t.thay("IDC_GRP_TAB12,\n\t\t\t};", "IDC_GRP_TAB12, IDC_GRP_TAB14,\n\t\t\t};")
    t.them("\t\thCtrl = ::GetDlgItem(hDlg, IDC_COMBO_9_VE);", CPP_COMBOS, sau=False,
           dau="IDC_COMBO_15_BD);")
    t.them("\t\t\tcase IDC_CHECKBOX_13_GHEP:", "\t\t\tcase IDC_CHECKBOX_15_ON:\n\t\t\tcase IDC_CHECKBOX_15_LOA:\n", sau=False)
    t.them("\t\t\tcase IDC_EDITOR_13_NGHI:",
           "\t\t\tcase IDC_EDITOR_15_GIO:\n\t\t\tcase IDC_EDITOR_15_PHUT:\n\t\t\tcase IDC_EDITOR_15_LECH:\n"
           "\t\t\tcase IDC_EDITOR_15_SOM:\n\t\t\tcase IDC_EDITOR_15_CUA:\n", sau=False)
    t.them("\t\t\tcase IDC_COMBO_13_CHON:",
           "\t\t\tcase IDC_COMBO_15_BD:\n\t\t\tcase IDC_COMBO_15_PHE:\n\t\t\tcase IDC_COMBO_15_CONG:\n"
           "\t\t\tcase IDC_COMBO_15_THU:\n\t\t\tcase IDC_COMBO_15_VE:\n", sau=False)
    t.ghi()
    if not THU:
        shutil.copyfile(p, os.path.join(WA_D, "WAuto.cpp"))


def main():
    print("== goi_va_wauto_congthanh %s ==" % ("(THU - khong ghi)" if THU else ""))
    va_coreshell_h()
    va_kplayer_h()
    va_coreshell_cpp()
    va_s3client()
    va_ipc()
    va_resource()
    va_rc()
    va_wauto_cpp()
    if LOI:
        print("LOI:")
        for l in LOI:
            print("  - " + l)
        return 1
    print("OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
