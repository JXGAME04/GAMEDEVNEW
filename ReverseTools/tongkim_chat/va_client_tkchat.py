# -*- coding: utf-8 -*-
"""[TKCHAT 04/09] Va client (S3Client, Game.exe): thong bao giet dich TK can giua + khong cat + xep hang;
bang diem Tong VS Kim (UiTongKimScore) noi vao GameSpaceChangedNotify/UiShell/vcxproj.
Doc/ghi latin-1 (nguon TCVN3/GBK). Idempotent theo dau [TKCHAT 04/09] / [TKDIEM 04/09].
Chay: python va_client_tkchat.py [--check]
"""
import io, sys, re

ROOT = r"D:\GAMEDEVNEW_wt_mail\Sources\S3Client"
CHECK = "--check" in sys.argv
BS = chr(92)


def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def wr(p, s):
    if CHECK:
        print("  (check) ghi", p, len(s)); return
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  ghi:", p, len(s))


def eol(s):
    return "\r\n" if "\r\n" in s else "\n"


def hi(s):
    return sum(1 for c in s if ord(c) >= 128)


def rep1(s, old, new, label):
    n = s.count(old)
    if n != 1:
        raise AssertionError("%s: neo khop %d lan: %s" % (label, n, ascii(old[:100])))
    return s.replace(old, new)


# ---------------------------------------------------------------- UiFlashMessage.cpp
def va_flash():
    p = ROOT + r"\Ui\UiCase\UiFlashMessage.cpp"
    s = rd(p)
    if "[TKCHAT 04/09]" in s:
        print("  da co, bo qua:", p); return
    E = eol(s); h0 = hi(s)
    # 1) chieu cao dong: font + 4
    s = rep1(s, "m_nLineHeight = m_nFontSize + 2; // font size + small padding (adjust if desired)",
             "m_nLineHeight = m_nFontSize + 4; // [TKCHAT 04/09] font + 4: ba dong TK khong dinh nhau", "line height")
    # 2) thay toan bo PaintWindow
    a = s.find("void KUiFlashMessage::PaintWindow()")
    assert a > 0, "khong thay PaintWindow"
    b = s.find("bool KUiFlashMessage::PickAMessage()", a)
    assert b > a
    # lui ve dau khoi chu thich /**** truoc PickAMessage
    c = s.rfind("/****", a, b)
    assert c > a
    new_paint = E.join([
        "// [TKCHAT 04/09] Do rong (px) cua chuoi DA MA HOA, dem DUNG cach KTextProcess::GetSimplexText/DrawTextLine",
        "// dem khi ve: byte > 0x80 = 2 nua-o va nuot 2 byte (chuan GBK), KTC_COLOR/BORDER 4 byte = 0 nua-o,",
        "// RESTORE 1 byte = 0, INLINE_PIC 3 byte ~ 2 nua-o, con lai 1 nua-o; mot nua-o = nFontSize/2 px",
        "// (DrawTextLine cong nCount/2*nFontSize). KHONG dung TGetEncodedTextOutputLenPos: ham do dem RESTORE",
        "// thanh 1 o (loi && thay vi ||) nen lech.",
        "static int sTkChatDoRong(const char* p, int nLen, int nFontSize)",
        "{",
        "\tint nUnits = 0, i = 0;",
        "\twhile (i < nLen)",
        "\t{",
        "\t\tunsigned char c = (unsigned char)p[i];",
        "\t\tif (c > 0x80)",
        "\t\t{",
        "\t\t\tnUnits += 2;",
        "\t\t\ti += 2;",
        "\t\t}",
        "\t\telse if (c == KTC_COLOR || c == KTC_BORDER_COLOR)",
        "\t\t\ti += 4;",
        "\t\telse if (c == KTC_COLOR_RESTORE || c == KTC_BORDER_RESTORE)",
        "\t\t\ti += 1;",
        "\t\telse if (c == KTC_INLINE_PIC)",
        "\t\t{",
        "\t\t\tnUnits += 2;",
        "\t\t\ti += 3;",
        "\t\t}",
        "\t\telse if (c == KTC_ENTER)",
        "\t\t\ti += 1;",
        "\t\telse",
        "\t\t{",
        "\t\t\tnUnits += 1;",
        "\t\t\ti += 1;",
        "\t\t}",
        "\t}",
        "\treturn nUnits * nFontSize / 2;",
        "}",
        "",
        "// [TKCHAT 04/09] Chu 04/09: \"thong bao giet dich o tong kim giua man hinh dang co nhung bi loi\".",
        "// Truoc: moi o ve o x = trai + IndentH (can TRAI) voi OutputRichText(..., m_nVisionWidth) mot dong ->",
        "// dong dai hon 578 px bi CAT o mep phai (anh chu: \"...nhan duoc 330 tich l\"), o nao trong thi nhan tin",
        "// moi -> dong moi co the nhay len TREN dong cu. Nay: xep cac o dang hien theo thoi diem bat dau (cu tren,",
        "// moi duoi), ve lien tiep tu tren xuong, CAN GIUA theo be rong that, khong gioi han be rong (0).",
        "// Cua so nay rong = ca man hinh (UiFlashMessage.ini Left=0 Width=800/1024).",
        "void KUiFlashMessage::PaintWindow()",
        "{",
        "\tif (!g_pRepresentShell)",
        "\t\treturn;",
        "",
        "\tKOutputTextParam Param;",
        "\tParam.Color = m_uTextColor;",
        "\tParam.BorderColor = m_uTextBorderColor;",
        "\tParam.nSkipLine = 0;",
        "\tParam.nNumLine = 1;",
        "\tParam.nZ = TEXT_IN_SINGLE_PLANE_COORD;",
        "\tParam.nVertAlign = 0;",
        "\tParam.bPicPackInSingleLine = true;",
        "",
        "\tint aIdx[64];",
        "\tint nSo = 0;",
        "\tfor (int i = 0; i < m_nNumSlots && nSo < 64; ++i)",
        "\t{",
        "\t\tif (m_DisplaySlots[i].bActive)",
        "\t\t\taIdx[nSo++] = i;",
        "\t}",
        "\tfor (int a = 1; a < nSo; ++a)",
        "\t{",
        "\t\tint k = aIdx[a];",
        "\t\tint b = a - 1;",
        "\t\twhile (b >= 0 && m_DisplaySlots[aIdx[b]].uDisplayStartTime > m_DisplaySlots[k].uDisplayStartTime)",
        "\t\t{",
        "\t\t\taIdx[b + 1] = aIdx[b];",
        "\t\t\t--b;",
        "\t\t}",
        "\t\taIdx[b + 1] = k;",
        "\t}",
        "",
        "\tfor (int r = 0; r < nSo; ++r)",
        "\t{",
        "\t\tDisplaySlot& slot = m_DisplaySlots[aIdx[r]];",
        "\t\tif (slot.CurrentMsg.nMsgLen <= 0)",
        "\t\t\tcontinue;",
        "\t\tchar szTemp[512];",
        "\t\tint nCopy = slot.CurrentMsg.nMsgLen;",
        "\t\tif (nCopy > (int)sizeof(szTemp) - 1)",
        "\t\t\tnCopy = (int)sizeof(szTemp) - 1;",
        "\t\tmemcpy(szTemp, slot.CurrentMsg.sMsg, nCopy);",
        "\t\tszTemp[nCopy] = 0;",
        "\t\tint nLen = TEncodeText(szTemp, nCopy);",
        "\t\tif (nLen <= 0)",
        "\t\t\tcontinue;",
        "\t\tint nRong = sTkChatDoRong(szTemp, nLen, m_nFontSize);",
        "\t\tint nX = m_nAbsoluteLeft + (m_Width - nRong) / 2;",
        "\t\tif (nX < m_nAbsoluteLeft + m_nIndentH)",
        "\t\t\tnX = m_nAbsoluteLeft + m_nIndentH;",
        "\t\tParam.nX = nX;",
        "\t\tParam.nY = m_nAbsoluteTop + m_nIndentV + r * m_nLineHeight;",
        "\t\tg_pRepresentShell->OutputRichText(m_nFontSize, &Param, szTemp, nLen, 0);",
        "\t}",
        "}",
        "",
        "",
    ])
    s = s[:a] + new_paint + s[c:]
    assert hi(s) == h0
    wr(p, s)


# ---------------------------------------------------------------- UiMsgCentrePad.cpp
def va_pad():
    p = ROOT + r"\Ui\UiCase\UiMsgCentrePad.cpp"
    s = rd(p)
    if "[TKCHAT 04/09]" in s:
        print("  da co, bo qua:", p); return
    E = eol(s); h0 = hi(s)
    old = ("\t\tint tmpSize = strlen(pMsgBuff) > sizeof(newsMsg.sMsg)? sizeof(newsMsg.sMsg)-1: strlen(pMsgBuff)-1;" + E
           + "\t\tstrncpy(newsMsg.sMsg, pMsgBuff, tmpSize);" + E
           + "\t\tnewsMsg.sMsg[tmpSize] = '\\0';" + E
           + "\t\tnewsMsg.nMsgLen = static_cast<int>(strlen(newsMsg.sMsg));" + E)
    new = ("\t\t// [TKCHAT 04/09] chep DU chuoi (truoc day strlen-1 lam roi ky tu cuoi) va gop dau cach lien tiep:" + E
           + "\t\t// mau cau may chu \"%s %s <color> %s\" + chuc vu co dau cach duoi sinh 2-3 dau cach quanh chuc vu." + E
           + "\t\tint nSrc = (int)strlen(pMsgBuff);" + E
           + "\t\tint nDst = 0;" + E
           + "\t\tfor (int s = 0; s < nSrc && nDst < (int)sizeof(newsMsg.sMsg) - 1; ++s)" + E
           + "\t\t{" + E
           + "\t\t\tchar ch = pMsgBuff[s];" + E
           + "\t\t\tif (ch == ' ' && nDst > 0 && newsMsg.sMsg[nDst - 1] == ' ')" + E
           + "\t\t\t\tcontinue;" + E
           + "\t\t\tnewsMsg.sMsg[nDst++] = ch;" + E
           + "\t\t}" + E
           + "\t\tnewsMsg.sMsg[nDst] = 0;" + E
           + "\t\tnewsMsg.nMsgLen = nDst;" + E)
    s = rep1(s, old, new, "MsgCentrePad copy")
    assert hi(s) == h0
    wr(p, s)


# ---------------------------------------------------------------- GameSpaceChangedNotify.cpp
def va_notify():
    p = ROOT + r"\Ui\GameSpaceChangedNotify.cpp"
    s = rd(p)
    if "[TKDIEM 04/09]" in s:
        print("  da co, bo qua:", p); return
    E = eol(s); h0 = hi(s)
    # include: sau include UiBattleReport.h (hoac include UiCase cuoi cung)
    m = None
    for mm in re.finditer(r'#include "UiCase[/\\]UiBattleReport\.h"[^\r\n]*', s):
        m = mm
    if m is None:
        for mm in re.finditer(r'#include "UiCase[/\\][^"]+\.h"[^\r\n]*', s):
            m = mm
    assert m, "khong thay include UiCase"
    s = s[:m.end()] + E + '#include "UiCase/UiTongKimScore.h"\t// [TKDIEM 04/09] bang diem Tong VS Kim' + s[m.end():]
    old = ("\tcase GDCNI_UPDATE_BATTLE_BOX:" + E + "\t\tif (nParam)" + E
           + "\t\t\tKUiBattleReport::UpdateRankWorld((char*)uParam, nParam);" + E)
    new = ("\tcase GDCNI_UPDATE_BATTLE_BOX:" + E
           + "\t\t// [TKDIEM 04/09] bang diem Tong VS Kim: nKind 6 = cap nhat \"tong|kim|diem\", 9 = an" + E
           + "\t\tKUiTongKimScore::OnBattleBox((char*)uParam, nParam);" + E
           + "\t\tif (nParam)" + E
           + "\t\t\tKUiBattleReport::UpdateRankWorld((char*)uParam, nParam);" + E)
    s = rep1(s, old, new, "battle box")
    old2 = "\tcase GDCNI_SWITCHING_SCENEPLACE:" + E + "\t\tbreak;" + E
    new2 = ("\tcase GDCNI_SWITCHING_SCENEPLACE:" + E
            + "\t\tKUiTongKimScore::OnSwitchMap(nParam);\t// [TKDIEM 04/09] bat dau nap map moi -> an bang diem" + E
            + "\t\tbreak;" + E)
    s = rep1(s, old2, new2, "switch map")
    assert hi(s) == h0
    wr(p, s)


# ---------------------------------------------------------------- UiShell.cpp
def va_shell():
    p = ROOT + r"\Ui\UiShell.cpp"
    s = rd(p)
    if "[TKDIEM 04/09]" in s:
        print("  da co, bo qua:", p); return
    E = eol(s); h0 = hi(s)
    m = None
    for mm in re.finditer(r'#include "UiCase[/\\]UiFlashMessage\.h"[^\r\n]*', s):
        m = mm
    if m is None:
        for mm in re.finditer(r'#include "UiCase[/\\][^"]+\.h"[^\r\n]*', s):
            m = mm
    assert m, "khong thay include UiCase"
    s = s[:m.end()] + E + '#include "UiCase/UiTongKimScore.h"\t// [TKDIEM 04/09]' + s[m.end():]
    s = rep1(s, "\tKUiFlashMessage::OpenWindow();" + E,
             "\tKUiFlashMessage::OpenWindow();" + E + "\tKUiTongKimScore::OpenWindow();\t// [TKDIEM 04/09] tao san (an), hien khi may chu gui diem" + E, "open")
    s = rep1(s, "\t\tKUiFlashMessage::CloseWindow(TRUE);" + E,
             "\t\tKUiFlashMessage::CloseWindow(TRUE);" + E + "\t\tKUiTongKimScore::CloseWindow(TRUE);\t// [TKDIEM 04/09]" + E, "close")
    assert hi(s) == h0
    wr(p, s)


# ---------------------------------------------------------------- vcxproj + filters
def va_vcxproj():
    p = ROOT + r"\S3Client.vcxproj"
    s = rd(p)
    if "UiTongKimScore" in s:
        print("  da co, bo qua:", p)
    else:
        E = eol(s)
        s = rep1(s, '    <ClCompile Include="Ui\\UiCase\\UiFlashMessage.cpp" />' + E,
                 '    <ClCompile Include="Ui\\UiCase\\UiFlashMessage.cpp" />' + E + '    <ClCompile Include="Ui\\UiCase\\UiTongKimScore.cpp" />' + E, "vcxproj cpp")
        s = rep1(s, '    <ClInclude Include="Ui\\UiCase\\UiFlashMessage.h" />' + E,
                 '    <ClInclude Include="Ui\\UiCase\\UiFlashMessage.h" />' + E + '    <ClInclude Include="Ui\\UiCase\\UiTongKimScore.h" />' + E, "vcxproj h")
        wr(p, s)
    p2 = ROOT + r"\S3Client.vcxproj.filters"
    s2 = rd(p2)
    if "UiTongKimScore" in s2:
        print("  da co, bo qua:", p2); return
    E = eol(s2)
    old = ('    <ClCompile Include="Ui\\UiCase\\UiFlashMessage.cpp">' + E
           + '      <Filter>BASE_ELEMENT_CASE\\UICase\\UI_NEW_MESS\\FLASH_MESS</Filter>' + E
           + '    </ClCompile>' + E)
    new = old + ('    <ClCompile Include="Ui\\UiCase\\UiTongKimScore.cpp">' + E
                 + '      <Filter>BASE_ELEMENT_CASE\\UICase\\UI_NEW_MESS\\FLASH_MESS</Filter>' + E
                 + '    </ClCompile>' + E)
    s2 = rep1(s2, old, new, "filters cpp")
    m = re.search(r'    <ClInclude Include="Ui\\UiCase\\UiFlashMessage\.h">\r?\n      <Filter>([^<]+)</Filter>\r?\n    </ClInclude>\r?\n', s2)
    assert m, "filters h"
    add = ('    <ClInclude Include="Ui\\UiCase\\UiTongKimScore.h">' + E
           + '      <Filter>' + m.group(1) + '</Filter>' + E
           + '    </ClInclude>' + E)
    s2 = s2[:m.end()] + add + s2[m.end():]
    wr(p2, s2)


def main():
    ok = True
    for name, fn in (("UiFlashMessage.cpp", va_flash), ("UiMsgCentrePad.cpp", va_pad),
                     ("GameSpaceChangedNotify.cpp", va_notify), ("UiShell.cpp", va_shell), ("vcxproj", va_vcxproj)):
        print("==", name)
        try:
            fn()
        except AssertionError as ex:
            ok = False
            print("  LOI:", ex)
    print("XONG" if ok else "CO LOI")


if __name__ == "__main__":
    main()
