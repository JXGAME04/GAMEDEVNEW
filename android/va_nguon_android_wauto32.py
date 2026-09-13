# -*- coding: utf-8 -*-
#
# [WAUTO 13/09] Tieu de bang chon dai: "Chon mot muc" -> ten that cua o.
#
# Do 11:25 tren LDPlayer: the Hau can, bam hop "Di ban do" thi bang phu de tieu de "Chon mot muc".
# Goc: hang do la "[v] Di ban do   [hop chon]" - chu "Di ban do" la NHAN CUA O TICK, ma bo sinh chi cho
# hop chon muon nhan RIENG cung hang (nhan_gan bo qua nhan cua tick) -> nKheNhan = 255 -> roi ve cau chung.
#
#  * sinh_bocuc_wauto.py: them nhan_tick, hop chon khong co nhan rieng thi dung chung nhan cua tick cung hang.
#    Sau khi sinh lai: 58 hop chon, chi con 4 cai khong co ten (hang khong co chu nao).
#  * Mot nhan bay gio co the ung voi CA tick lan hop chon -> vong tim trong WndProc phai UU TIEN O TICK
#    (truoc day an theo thu tu bang: tick dung truoc chon nen dung, nhung la dung nho may man).
#
# Chi mobile (JX_MOBILE). Chay lai vo hai.

import io
import os
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")

DAU = "[WAUTO 13/09]"
C = "Sources/S3Client/Ui/UiCase/UiWAutoTrang.cpp"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


CU = """\t\t\t\tif (!m_pTab)
\t\t\t\t\treturn 1;
\t\t\t\tfor (int k = 0; k < m_pTab->nMuc; k++)
\t\t\t\t{
\t\t\t\t\tconst WAUiMuc& m = m_pTab->pMuc[k];
\t\t\t\t\tif (m.nKheNhan != i)
\t\t\t\t\t\tcontinue;
\t\t\t\t\tif (m.nLoai == WA_MUC_TICK && m.nKhe < WA_TR_TICK && !m_Tick[m.nKhe].IsDisable())
\t\t\t\t\t{
\t\t\t\t\t\tWndProc(WND_N_BUTTON_CLICK, (KUPARAM)(KWndWindow*)&m_Tick[m.nKhe], 0);
\t\t\t\t\t\tbreak;
\t\t\t\t\t}
\t\t\t\t\tif (m.nLoai == WA_MUC_CHON && m.nKhe < WA_TR_CHON && !m_Chon[m.nKhe].IsDisable())
\t\t\t\t\t{\t// [WAUTO 12/09] cham vao nhan cua hop chon ("Vong sang #1", "Ruong cua"...) = mo hop do
\t\t\t\t\t\tMoMenuChon(m.nKhe);
\t\t\t\t\t\tbreak;
\t\t\t\t\t}
\t\t\t\t}
\t\t\t\treturn 1;
"""

MOI = """\t\t\t\tif (!m_pTab)
\t\t\t\t\treturn 1;
\t\t\t\t// %s MOT nhan co the ung voi ca o tick lan hop chon cung hang (hang "[v] Di ban do  [hop chon]":
\t\t\t\t// hop chon muon nhan cua tick de lam tieu de bang phu) -> quet HAI LUOT, o tick duoc uu tien.
\t\t\t\tfor (int k = 0; k < m_pTab->nMuc; k++)
\t\t\t\t{
\t\t\t\t\tconst WAUiMuc& m = m_pTab->pMuc[k];
\t\t\t\t\tif (m.nKheNhan == i && m.nLoai == WA_MUC_TICK && m.nKhe < WA_TR_TICK && !m_Tick[m.nKhe].IsDisable())
\t\t\t\t\t{
\t\t\t\t\t\tWndProc(WND_N_BUTTON_CLICK, (KUPARAM)(KWndWindow*)&m_Tick[m.nKhe], 0);
\t\t\t\t\t\treturn 1;
\t\t\t\t\t}
\t\t\t\t}
\t\t\t\tfor (int k = 0; k < m_pTab->nMuc; k++)
\t\t\t\t{\t// [WAUTO 12/09] cham vao nhan cua hop chon ("Vong sang #1", "Ruong cua"...) = mo hop do
\t\t\t\t\tconst WAUiMuc& m = m_pTab->pMuc[k];
\t\t\t\t\tif (m.nKheNhan == i && m.nLoai == WA_MUC_CHON && m.nKhe < WA_TR_CHON && !m_Chon[m.nKhe].IsDisable())
\t\t\t\t\t{
\t\t\t\t\t\tMoMenuChon(m.nKhe);
\t\t\t\t\t\treturn 1;
\t\t\t\t\t}
\t\t\t\t}
\t\t\t\treturn 1;
""" % DAU


def main():
    global CU, MOI
    CU = CU.replace("\n", "\r\n")       # nguon C dung CRLF
    MOI = MOI.replace("\n", "\r\n")
    s = doc(C)
    if "quet HAI LUOT" in s:
        print("   bo qua (da va):", C)
        return
    if s.count(CU) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d)" % s.count(CU))
    ghi(C, s.replace(CU, MOI))
    print("   da va: UiWAutoTrang.cpp: nhan dung chung -> uu tien o tick")
    print("xong wauto32")


if __name__ == "__main__":
    main()
