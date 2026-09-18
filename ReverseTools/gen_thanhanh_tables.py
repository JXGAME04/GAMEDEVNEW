# -*- coding: utf-8 -*-
r"""gen_thanhanh_tables.py - SINH BANG MENU THAN HANH PHU (6/1/1271) cho auto Ac chinh.

Chu game 18/09: "toi noi dung than hanh phu de di chuyen theo acc chinh nhung toi test thay
no di chuyen thang len map khong bam mo than hanh phu?" -> ac phu phai MO PHU va lan menu
nhu nguoi choi, khong di tat bang lenh movemapid (GotoMapId).

Menu bai luyen cong cua phu (bin\server\script\vatpham\ib\shenxingfu.lua):
    "Su dung thuat than hanh di den noi chi dinh"      (main)
    -> "Ban do luyen cong tu 20 den 90"                (chondiadiem1 -> gotoluyencong)
    -> "Ban do luyen cong 90 tro len."  |  "Ban do Luyen cong 20 den 80."
    -> (20-80) "Di chuyen den ban do luyen cong cap NN"  (luyencongtanthu -> gopos_step2lvNN)
    -> ten bai (tab_lvNNmap[i][4], chi N dong dau ma gopos_step2lvNN liet ke)   -> gopos_step3lvNN -> NewWorld

Sinh KThanHanhTables.h: bang (mapId, cap, ten bai) + 4 chuoi muc menu, GIU NGUYEN BYTE TCVN3
cua script (vi du "Kho\266 Lang" viet 'oa' + dau hoi tren a - go tay de sai byte).

Chay: python gen_thanhanh_tables.py [--ghi] [--root <worktree>]
"""
import io
import os
import re
import sys

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\vatpham\ib\shenxingfu.lua"
ROOT = r"D:\GAMEDEVNEW"
for i, a in enumerate(sys.argv):
    if a == "--root" and i + 1 < len(sys.argv):
        ROOT = sys.argv[i + 1]
HDR = os.path.join(ROOT, r"Sources\Core\Src\KThanHanhTables.h")
GHI = "--ghi" in sys.argv

sys.path.insert(0, os.path.join(os.environ.get("USERPROFILE", ""),
                                ".claude", "skills", "swordonline-dev", "scripts"))
from vn_to_octal import _UNICODE_TO_TCVN3

_T = {ord(v): k for k, v in _UNICODE_TO_TCVN3.items()}


def doc(s):
    """byte TCVN3 (da doc bang latin-1) -> tieng Viet, chi de IN ra man hinh"""
    return "".join(_T.get(ord(c), c) for c in s)


def c_str(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def main():
    p = SRV if os.path.exists(SRV) else os.path.join(ROOT, r"serverscript_live\script\vatpham\ib\shenxingfu.lua")
    s = io.open(p, encoding="latin-1", newline="").read().replace("\r", "")
    lines = s.split("\n")

    # 1. so dong menu liet ke cho tung cap: gopos_step2lvNN co "for i = 1, N do"
    so_dong = {}
    for m in re.finditer(r"function gopos_step2lv(\d+)\s*\(.*?\n(.*?)\nend", s, re.S):
        cap = int(m.group(1))
        f = re.search(r"for i = 1, (\d+) do", m.group(2))
        if f:
            so_dong[cap] = int(f.group(1))
    # 2. bang tab_lvNNmap: chi lay dong KHONG bi comment, va chi N dong dau
    bai = []
    for m in re.finditer(r"tab_lv(\d+)map\s*=\s*\{(.*?)\n\s*\}", s, re.S):
        cap = int(m.group(1))
        n = so_dong.get(cap, 0)
        dem = 0
        for l in m.group(2).split("\n"):
            l = l.strip()
            if not l or l.startswith("--"):
                continue
            r = re.match(r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*\"([^\"]+)\"", l)
            if not r:
                continue
            dem += 1
            if dem > n:
                break
            bai.append((int(r.group(1)), cap, r.group(4)))
    bai_sorted = sorted(bai, key=lambda t: (-t[1], t[0]))

    # 3. chuoi muc menu (text truoc dau '/')
    def muc(dau):
        for l in lines:
            if dau in l:
                q = re.search(r'"([^"]*)/' + re.escape(dau), l)
                if q:
                    return q.group(1).strip()
        raise SystemExit("khong thay muc menu: " + dau)
    m_luyen = muc("gotoluyencong")            # chondiadiem1
    m_lv90 = muc("#gopos_step2lv90()")        # gotoluyencong
    m_lv2080 = muc("#luyencongtanthu()")      # gotoluyencong
    m_cap20 = muc("gopos_step2lv20")          # luyencongtanthu: "... cap 20"
    m_cap = m_cap20[:m_cap20.rfind("20")]     # prefix "... cap "

    print("so dong menu theo cap:", so_dong)
    print("%d bai:" % len(bai_sorted))
    for mid, cap, ten in bai_sorted:
        print("  %4d  lv%d  %s" % (mid, cap, doc(ten)))
    for k, v in (("LUYEN", m_luyen), ("LV90", m_lv90), ("LV2080", m_lv2080), ("CAP", m_cap)):
        print("  %-7s %s" % (k, doc(v)))

    out = []
    out.append("// ===== MENU THAN HANH PHU (6/1/1271) - SINH TU DONG boi ReverseTools/gen_thanhanh_tables.py")
    out.append("// tu bin\\server\\script\\vatpham\\ib\\shenxingfu.lua. DUNG SUA TAY - script doi thi sinh lai.")
    out.append("// Duong menu toi bai luyen cong: \"Su dung thuat than hanh...\" (TKM_OPT_THP_DI) -> THPM_LUYEN")
    out.append("//   -> THPM_LV90 (cap 90) | THPM_LV2080 -> THPM_CAP + so cap (20..80) -> ten bai (bang duoi).")
    out.append("// Ten giu nguyen byte TCVN3 nhu script (strstr len thoai). Chi N dong dau ma gopos_step2lvNN liet ke.")
    out.append("#ifndef KTHANHANHTABLES_H")
    out.append("#define KTHANHANHTABLES_H")
    out.append("")
    out.append("struct THPBaiRow { int nMapId; int nCap; const char* szTen; };")
    out.append("#define THP_BAI_COUNT\t%d" % len(bai_sorted))
    out.append("static const THPBaiRow g_aTHPBai[THP_BAI_COUNT] =")
    out.append("{")
    for mid, cap, ten in bai_sorted:
        out.append("\t{ %-4d, %d, %s }," % (mid, cap, c_str(ten)))
    out.append("};")
    out.append("static const char THPM_LUYEN[]  = %s;\t// chondiadiem1 -> gotoluyencong" % c_str(m_luyen))
    out.append("static const char THPM_LV90[]   = %s;\t// gotoluyencong -> gopos_step2lv90" % c_str(m_lv90))
    out.append("static const char THPM_LV2080[] = %s;\t// gotoluyencong -> luyencongtanthu" % c_str(m_lv2080))
    out.append("static const char THPM_CAP[]    = %s;\t// luyencongtanthu: + \"20\"..\"80\" -> gopos_step2lvNN" % c_str(m_cap))
    out.append("")
    out.append("#endif // KTHANHANHTABLES_H")
    txt = "\r\n".join(out) + "\r\n"
    if GHI:
        io.open(HDR, "w", encoding="latin-1", newline="").write(txt)
        print("da ghi", HDR)
    else:
        print("(--ghi de ghi %s)" % HDR)


if __name__ == "__main__":
    main()
