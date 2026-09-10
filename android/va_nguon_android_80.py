# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 ICON c] Thu tren may ao va 79: cham icon "Giao tiep" -> nhan vat DI TOI NPC nhung KHONG mo thoai; cham thang
# than NPC thi mo. Ly do: diem bam tinh la "chan - 28" roi trung mep chan (ngoai vung SearchNpcAt: duong thang dung z 0..120
# tu diem bam, sai so 40 mps); lan dau NPC nam duoi ban do nho -> cu bam roi vao cua so ban do (= di toi diem do).
# Sua: (1) JxIconNpc_Cham do lan luot chan-40/55/25/70/12/85 bang iCoreShell::FindSelectNPC (bSelect=false) cho toi khi
# trung dung NPC (so ten) va diem do khong bi giao dien che; (2) khong ve icon khi than NPC bi cua so che.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 ICON c]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


def va_can(s):
    s = thay(s, ["static unsigned int\ts_uIconVeLuc = 0;\t// [ANDROID 11/09 ICON b] luc ve icon gan nhat (0 = chua ve)"],
             ["static unsigned int\ts_uIconVeLuc = 0;\t// [ANDROID 11/09 ICON b] luc ve icon gan nhat (0 = chua ve)",
              "static char\ts_szIconTen[32] = \"\";\t// %s ten NPC cua icon vua ve (de do diem bam dung NPC)" % DAU,
              "extern \"C\" int JxUi_CoGiaoDienTaiDiem(int x, int y);\t// %s Wnds.cpp" % DAU], "khai bao icon")
    s = thay(s, ["\tint x = gan.nViTriVeX, y = gan.nViTriVeY;",
                 "\tg_pRepresentShell->CoordinateTransform(x, y, 0);\t// the gioi -> man hinh",
                 "\ty -= s_nIconCao;",
                 "\tif (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)",
                 "\t\treturn;\t\t// NPC ra ngoai khung ve"],
                ["\tint x = gan.nViTriVeX, y = gan.nViTriVeY;",
                 "\tg_pRepresentShell->CoordinateTransform(x, y, 0);\t// the gioi -> man hinh",
                 "\ty -= s_nIconCao;",
                 "\tif (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)",
                 "\t\treturn;\t\t// NPC ra ngoai khung ve",
                 "\tif (JxUi_CoGiaoDienTaiDiem(x, y + s_nIconCao - 40))",
                 "\t\treturn;\t\t// %s than NPC bi cua so (ban do nho...) che -> khong ve, keo cham icon lai bam vao cua so" % DAU,
                 "\tstrncpy(s_szIconTen, gan.sTargetName, sizeof(s_szIconTen) - 1);",
                 "\ts_szIconTen[sizeof(s_szIconTen) - 1] = 0;"], "than JxIconNpc_Ve: che + ten")
    s = thay(s, ["\tif (x < s_nIconVeX0 || x > s_nIconVeX1 || y < s_nIconVeY0 || y > s_nIconVeY1)",
                 "\t\treturn 0;",
                 "\t*pnX = s_nIconNpcX;",
                 "\t*pnY = s_nIconNpcY - 28;",
                 "\treturn 1;"],
                ["\tif (x < s_nIconVeX0 || x > s_nIconVeX1 || y < s_nIconVeY0 || y > s_nIconVeY1)",
                 "\t\treturn 0;",
                 "\t// %s do diem bam: chan-40, 55, 25, 70, 12, 85 - lay diem dau tien ma Core tra dung NPC nay (SearchNpcAt quet" % DAU,
                 "\t// duong thang dung z 0..120 tu diem bam, sai so 40 mps) va khong bi giao dien che. Diem \"chan - 28\" cu roi mep chan.",
                 "\tstatic const int aLui[] = { 40, 55, 25, 70, 12, 85 };",
                 "\tfor (int i = 0; i < (int)(sizeof(aLui) / sizeof(aLui[0])); i++)",
                 "\t{",
                 "\t\tKUiPlayerItem oAi;",
                 "\t\tint nKind = 0;",
                 "\t\tint nX = s_nIconNpcX, nY = s_nIconNpcY - aLui[i];",
                 "\t\tmemset(&oAi, 0, sizeof(oAi));",
                 "\t\tif (g_pCoreShell && g_pCoreShell->FindSelectNPC(nX, nY, relation_all, false, &oAi, nKind)",
                 "\t\t\t&& strncmp(oAi.Name, s_szIconTen, sizeof(s_szIconTen) - 1) == 0 && !JxUi_CoGiaoDienTaiDiem(nX, nY))",
                 "\t\t{",
                 "\t\t\t*pnX = nX;",
                 "\t\t\t*pnY = nY;",
                 "\t\t\treturn 1;",
                 "\t\t}",
                 "\t}",
                 "\tg_DebugLog(\"[ICON] cham icon nhung khong do duoc diem bam trung NPC %s\", s_szIconTen);",
                 "\treturn 0;"], "JxIconNpc_Cham do diem")
    return s


va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", va_can)
print("xong")
