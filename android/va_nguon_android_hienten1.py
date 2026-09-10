# -*- coding: utf-8 -*-
#
# [HIENTEN 12/09] Chu 08:40: "vao game phai bam F7 - F8 - Ctrl+Space moi hien ten - thanh mau - do duoi dat, game mobile khong bam
# duoc -> hien ngay khi vao game". KUiPlayerBar::LoadPrivateSetting chi ap [Player] ShowLife/ShowName/ShowObjName khi tep tuy chon
# rieng cua nhan vat CO khoa (nguoi choi da bam phim it nhat mot lan). Android: thieu khoa -> coi nhu 1 (hien) va ap luon.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[HIENTEN 12/09]"
TEP = "Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, nl.join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


s = doc(TEP)
if DAU in s:
    raise SystemExit("da va roi")
s0 = s
s = thay(s, ['\t\tif (pFile->GetInteger("Player", "ShowLife", 0, (int*)(&nLife)))',
             "\t\t\tg_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_LIFE, 0, nLife);",
             '\t\tif (pFile->GetInteger("Player", "ShowName", 0, (int*)(&nName)))',
             "\t\t\tg_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_NAME, 0, nName);",
             '\t\tif (pFile->GetInteger("Player", "ShowObjName", 0, (int*)(&nNameObj)))',
             "\t\t\tg_pCoreShell->OperationRequest(GOI_SHOW_OBJ_NAME, 0, nNameObj);"],
            ["#ifdef JX_ANDROID",
             "\t\t// %s dien thoai khong co F7 / F8 / Ctrl+Space: chua luu tuy chon thi mac dinh HIEN ten, thanh mau, ten do duoi dat" % DAU,
             '\t\tif (!pFile->GetInteger("Player", "ShowLife", 0, (int*)(&nLife)))',
             "\t\t\tnLife = 1;",
             "\t\tg_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_LIFE, 0, nLife);",
             '\t\tif (!pFile->GetInteger("Player", "ShowName", 0, (int*)(&nName)))',
             "\t\t\tnName = 1;",
             "\t\tg_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_NAME, 0, nName);",
             '\t\tif (!pFile->GetInteger("Player", "ShowObjName", 0, (int*)(&nNameObj)))',
             "\t\t\tnNameObj = 1;",
             "\t\tg_pCoreShell->OperationRequest(GOI_SHOW_OBJ_NAME, 0, nNameObj);",
             "#else",
             '\t\tif (pFile->GetInteger("Player", "ShowLife", 0, (int*)(&nLife)))',
             "\t\t\tg_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_LIFE, 0, nLife);",
             '\t\tif (pFile->GetInteger("Player", "ShowName", 0, (int*)(&nName)))',
             "\t\t\tg_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_NAME, 0, nName);",
             '\t\tif (pFile->GetInteger("Player", "ShowObjName", 0, (int*)(&nNameObj)))',
             "\t\t\tg_pCoreShell->OperationRequest(GOI_SHOW_OBJ_NAME, 0, nNameObj);",
             "#endif"], "LoadPrivateSetting")
if cao(s) != cao(s0):
    raise SystemExit("so byte cao doi")
ghi(TEP, s)
print("da va:", TEP)
