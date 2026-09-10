# -*- coding: utf-8 -*-
#
# [ANDROID 12/09 KYNANG AN] Chu: "Cac nut ky nang bi ra ngoai phan vao game" - bang nut ky nang / can dieu khien / vong chon /
# mui ten huong di / icon NPC ve ca o menu chinh, dang nhap, chon nhan vat (UiShell goi JxKyNang_Ve + JxCan_Ve moi khung,
# khong xet trang thai). Sua trong JxCanDieuKhien.cpp (tep chi Android, ngoai vcxproj -> ban Windows khong doi):
#   KyNang_TrongGame() = KUiToolsControlBar::GetSelf() != NULL (thanh cong cu mo trong UiStartGame, huy khi roi the gioi)
#   -> khong ve va khong nhan cham khi chua vao the gioi.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 12/09 KYNANG AN]"
TEP = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"


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
    raise SystemExit("da va roi: " + TEP)
s0 = s

# 1. include + ham KyNang_TrongGame
s = thay(s, ['#include <stdint.h>\t// [UITOADO 10/09 F] intptr_t'],
            ['#include <stdint.h>\t// [UITOADO 10/09 F] intptr_t',
             '#include "../Ui/UiCase/UiToolsControlBar.h"\t// %s thanh cong cu = dau hieu da vao the gioi' % DAU,
             '',
             '// %s chu: "cac nut ky nang bi ra ngoai phan vao game" - o menu / dang nhap / chon nhan vat chua co thanh cong cu' % DAU,
             '// (KUiToolsControlBar mo trong UiStartGame, huy khi roi the gioi) -> khong ve, khong nhan cham.',
             'static bool KyNang_TrongGame()',
             '{',
             '\treturn KUiToolsControlBar::GetSelf() != NULL;',
             '}'], "include + KyNang_TrongGame")

# 2. cham can dieu khien
s = thay(s, ['bool JxCan_TrongVung(int x, int y)', '{', '\tDocCaiDat();', '\tif (!s_nBat)', '\t\treturn false;'],
            ['bool JxCan_TrongVung(int x, int y)', '{', '\tDocCaiDat();', '\tif (!s_nBat || !KyNang_TrongGame())\t// %s' % DAU, '\t\treturn false;'],
        "JxCan_TrongVung")
# 3. vong chon
s = thay(s, ['void JxVongChon_Ve()', '{', '\tDocCaiDat();', '\tif (!s_nVongBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)'],
            ['void JxVongChon_Ve()', '{', '\tDocCaiDat();', '\tif (!s_nVongBat || g_pCoreShell == NULL || g_pRepresentShell == NULL || !KyNang_TrongGame())\t// %s' % DAU],
        "JxVongChon_Ve")
# 4. cham nut ky nang
s = thay(s, ['int JxKyNang_TrungNut(int x, int y)', '{', '\tint i, nX, nY, nR;', '', '\tDocCaiDat();', '\tif (!s_nKNBat)', '\t\treturn 0;'],
            ['int JxKyNang_TrungNut(int x, int y)', '{', '\tint i, nX, nY, nR;', '', '\tDocCaiDat();', '\tif (!s_nKNBat || !KyNang_TrongGame())\t// %s' % DAU, '\t\treturn 0;'],
        "JxKyNang_TrungNut")
# 5. mui ten huong di
s = thay(s, ['\tif (!s_nHuongDiBat || !s_bCam || g_pRepresentShell == NULL)'],
            ['\tif (!s_nHuongDiBat || !s_bCam || g_pRepresentShell == NULL || !KyNang_TrongGame())\t// %s' % DAU],
        "JxHuongDi_Ve")
# 6. bang chon ky nang
s = thay(s, ['int JxKyNang_ChamBangChon(int x, int y)', '{', '\tint nCao = BC_CAO_TEN + BC_CAO_NUT * BC_SO_NUT;', '\tint nNut;', '\tKUiGameObject o;', '', '\tif (!s_nBCBat)'],
            ['int JxKyNang_ChamBangChon(int x, int y)', '{', '\tint nCao = BC_CAO_TEN + BC_CAO_NUT * BC_SO_NUT;', '\tint nNut;', '\tKUiGameObject o;', '', '\tif (!s_nBCBat || !KyNang_TrongGame())\t// %s' % DAU],
        "JxKyNang_ChamBangChon")
# 7. ve bang nut ky nang
s = thay(s, ['void JxKyNang_Ve()', '{', '\tint i, nX, nY, nR, nIcon;', '\tKUiGameObject o;', '', '\tDocCaiDat();', '\tif (!s_nKNBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)'],
            ['void JxKyNang_Ve()', '{', '\tint i, nX, nY, nR, nIcon;', '\tKUiGameObject o;', '', '\tDocCaiDat();', '\tif (!s_nKNBat || g_pCoreShell == NULL || g_pRepresentShell == NULL || !KyNang_TrongGame())\t// %s' % DAU],
        "JxKyNang_Ve")
# 8. ve can
s = thay(s, ['void JxCan_Ve()', '{', '\tif (!s_bCam || g_pRepresentShell == NULL)'],
            ['void JxCan_Ve()', '{', '\tif (!s_bCam || g_pRepresentShell == NULL || !KyNang_TrongGame())\t// %s' % DAU],
        "JxCan_Ve")
# 9. icon NPC
s = thay(s, ['void JxIconNpc_Ve()', '{'],
            ['void JxIconNpc_Ve()', '{', '\tif (!KyNang_TrongGame())\t// %s' % DAU, '\t\treturn;'],
        "JxIconNpc_Ve")

if cao(s) != cao(s0):
    raise SystemExit("so byte cao doi")
ghi(TEP, s)
print("da va:", TEP)
