# -*- coding: utf-8 -*-
r"""BAN DONG HANH - G4c: dang ky 4 cua so partner vao client.

 1. GameSpaceChangedNotify.cpp : include + UOC_PARTNER_BAG + task value notify
 2. ShortcutKey.cpp            : l_WindowList 31..39 + case LuaOpenWindow
 3. UiShell.cpp                : dong 4 cua so khi logout/doi nhan vat
 4. S3Client.vcxproj/.filters  : 5 cap .cpp/.h
 5. bin\client\Ui\autoexec.lua : phim tat Y/I/U/P + L/F/G/B/N (nhu tip ban goc)
Idempotent, marker [BDH-G4].
"""
import io
import os
import shutil

T = chr(9)
CR = chr(13)
LF = chr(10)
E = CR + LF

UI = r"D:\GAMEDEVNEW\Sources\S3Client\Ui"
PRJ = r"D:\GAMEDEVNEW\Sources\S3Client"
AEX = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Ui\autoexec.lua"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s, bak=".truoc_bdh_g4"):
    if not os.path.exists(p + bak):
        shutil.copyfile(p, p + bak)
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def patch(p, neo, moi, marker):
    s = doc(p)
    if marker in s:
        print("  da co:", os.path.basename(p), marker[:36])
        return
    n = s.count(neo)
    assert n == 1, "anchor %d lan trong %s: %r" % (n, p, neo[:70])
    ghi(p, s.replace(neo, moi, 1))
    print("  VA:", os.path.basename(p), marker[:36])


INC4 = ('#include "UiCase/UiPartnerCommon.h"' + T + "// [BDH-G4]" + E +
        '#include "UiCase/UiPartnerAttr.h"' + E +
        '#include "UiCase/UiPartnerSkill.h"' + E +
        '#include "UiCase/UiPartnerBag.h"' + E +
        '#include "UiCase/UiPartnerBar.h"' + E)

# ---------- 1. GameSpaceChangedNotify.cpp ----------
p = os.path.join(UI, "GameSpaceChangedNotify.cpp")
patch(p,
      '#include "UiCase/UiDiceItem.h"	// DICEITEM 26/08' + E,
      '#include "UiCase/UiDiceItem.h"	// DICEITEM 26/08' + E + INC4,
      'UiPartnerCommon.h"' + T + "// [BDH-G4]")

patch(p,
      T*3 + "else if (pObject->eContainer == UOC_ITEM_EX)" + E,
      T*3 + "else if (pObject->eContainer == UOC_PARTNER_BAG)" + T + "// [BDH-G4]" + E +
      T*3 + "{" + E +
      T*4 + "KUiPartnerBag* pPartnerBag = KUiPartnerBag::GetIfVisible();" + E +
      T*4 + "if (pPartnerBag)" + E +
      T*5 + "pPartnerBag->UpdateItem((KUiObjAtRegion*)uParam, nParam);" + E +
      T*3 + "}" + E +
      T*3 + "else if (pObject->eContainer == UOC_ITEM_EX)" + E,
      "UOC_PARTNER_BAG)" + T + "// [BDH-G4]")

patch(p,
      T*2 + "KUiTaskTrace::OnTaskValueChanged((int)uParam);" + E,
      T*2 + "KUiTaskTrace::OnTaskValueChanged((int)uParam);" + E +
      T*2 + "UiPartner_OnTaskValueChanged((int)uParam);" + T + "// [BDH-G4]" + E,
      "UiPartner_OnTaskValueChanged((int)uParam)")

# ---------- 2. ShortcutKey.cpp ----------
p = os.path.join(UI, "ShortcutKey.cpp")
patch(p,
      '#include "UiCase/UiTeamManage.h"' + E,
      '#include "UiCase/UiTeamManage.h"' + E + INC4,
      'UiPartnerCommon.h"' + T + "// [BDH-G4]")

patch(p,
      T + '"NewTask",	//30 [TaskGuide] ten cua so nhu ban goc (F12)' + E,
      T + '"NewTask",	//30 [TaskGuide] ten cua so nhu ban goc (F12)' + E +
      T + '"partner",	//31 [BDH-G4] cua so thuoc tinh dong hanh' + E +
      T + '"partnerskill",	//32 [BDH-G4]' + E +
      T + '"partnerbag",	//33 [BDH-G4]' + E +
      T + '"partnerbar",	//34 [BDH-G4] thanh nhanh' + E +
      T + '"partnertalk",	//35 [BDH-G4] doi thoai voi dong hanh' + E +
      T + '"partnercall",	//36 [BDH-G4] goi ra / thu ve' + E +
      T + '"partnerattack",	//37 [BDH-G4] che do chu dong danh' + E +
      T + '"partnerfollow",	//38 [BDH-G4] che do chi theo' + E +
      T + '"partnerselect",	//39 [BDH-G4] doi con duong nhiem' + E,
      '"partnerbar",')

case_block = (
    T*2 + "case 31:	// [BDH-G4] cua so dong hanh" + E +
    T*3 + "if (KUiPartnerAttr::GetIfVisible())" + E +
    T*4 + "KUiPartnerAttr::CloseWindow();" + E +
    T*3 + "else" + E +
    T*4 + "KUiPartnerAttr::OpenWindow();" + E +
    T*3 + "break;" + E +
    T*2 + "case 32:" + E +
    T*3 + "if (KUiPartnerSkill::GetIfVisible())" + E +
    T*4 + "KUiPartnerSkill::CloseWindow();" + E +
    T*3 + "else" + E +
    T*4 + "KUiPartnerSkill::OpenWindow();" + E +
    T*3 + "break;" + E +
    T*2 + "case 33:" + E +
    T*3 + "if (KUiPartnerBag::GetIfVisible())" + E +
    T*4 + "KUiPartnerBag::CloseWindow();" + E +
    T*3 + "else" + E +
    T*4 + "KUiPartnerBag::OpenWindow();" + E +
    T*3 + "break;" + E +
    T*2 + "case 34:" + E +
    T*3 + "if (KUiPartnerBar::GetIfVisible())" + E +
    T*4 + "KUiPartnerBar::CloseWindow();" + E +
    T*3 + "else" + E +
    T*4 + "KUiPartnerBar::OpenWindow();" + E +
    T*3 + "break;" + E +
    T*2 + "case 35:" + E +
    T*3 + "UiPartner_HotKey(0);" + E +
    T*3 + "break;" + E +
    T*2 + "case 36:" + E +
    T*3 + "UiPartner_HotKey(1);" + E +
    T*3 + "break;" + E +
    T*2 + "case 37:" + E +
    T*3 + "UiPartner_HotKey(2);" + E +
    T*3 + "break;" + E +
    T*2 + "case 38:" + E +
    T*3 + "UiPartner_HotKey(3);" + E +
    T*3 + "break;" + E +
    T*2 + "case 39:" + E +
    T*3 + "UiPartner_HotKey(4);" + E +
    T*3 + "break;" + E)

neo_case30 = (
    T*2 + "case 30:	// [TaskGuide] \"NewTask\" - giu tuong thich phim F12 cua ban goc" + E +
    T*3 + "if (KUiTaskGuide::GetIfVisible())" + E +
    T*4 + "KUiTaskGuide::CloseWindow(false);" + E +
    T*3 + "else" + E +
    T*4 + "KUiTaskGuide::OpenWindow();" + E +
    T*3 + "break;" + E)
patch(p, neo_case30, neo_case30 + case_block, "case 31:	// [BDH-G4]")

# ---------- 3. UiShell.cpp ----------
p = os.path.join(UI, "UiShell.cpp")
patch(p,
      '#include "UiCase/UiDiceItem.h"	// DICEITEM 26/08' + E,
      '#include "UiCase/UiDiceItem.h"	// DICEITEM 26/08' + E +
      '#include "UiCase/UiPartnerAttr.h"' + T + "// [BDH-G4]" + E +
      '#include "UiCase/UiPartnerSkill.h"' + E +
      '#include "UiCase/UiPartnerBag.h"' + E +
      '#include "UiCase/UiPartnerBar.h"' + E,
      'UiPartnerAttr.h"' + T + "// [BDH-G4]")

patch(p,
      T + "KUiDiceItem::CloseWindow(bAll);" + E,
      T + "KUiDiceItem::CloseWindow(bAll);" + E +
      T + "KUiPartnerAttr::CloseWindow();" + T + "// [BDH-G4]" + E +
      T + "KUiPartnerSkill::CloseWindow();" + E +
      T + "KUiPartnerBag::CloseWindow();" + E +
      T + "KUiPartnerBar::CloseWindow();" + E,
      "KUiPartnerAttr::CloseWindow();" + T + "// [BDH-G4]")

# ---------- 4. vcxproj ----------
p = os.path.join(PRJ, "S3Client.vcxproj")
patch(p,
      '    <ClCompile Include="ui\\uicase\\UiDiceItem.cpp" />' + E,
      '    <ClCompile Include="ui\\uicase\\UiDiceItem.cpp" />' + E +
      '    <ClCompile Include="ui\\uicase\\UiPartnerCommon.cpp" />' + E +
      '    <ClCompile Include="ui\\uicase\\UiPartnerAttr.cpp" />' + E +
      '    <ClCompile Include="ui\\uicase\\UiPartnerSkill.cpp" />' + E +
      '    <ClCompile Include="ui\\uicase\\UiPartnerBag.cpp" />' + E +
      '    <ClCompile Include="ui\\uicase\\UiPartnerBar.cpp" />' + E,
      "UiPartnerCommon.cpp")
patch(p,
      '    <ClInclude Include="ui\\uicase\\UiDiceItem.h" />' + E,
      '    <ClInclude Include="ui\\uicase\\UiDiceItem.h" />' + E +
      '    <ClInclude Include="ui\\uicase\\UiPartnerCommon.h" />' + E +
      '    <ClInclude Include="ui\\uicase\\UiPartnerAttr.h" />' + E +
      '    <ClInclude Include="ui\\uicase\\UiPartnerSkill.h" />' + E +
      '    <ClInclude Include="ui\\uicase\\UiPartnerBag.h" />' + E +
      '    <ClInclude Include="ui\\uicase\\UiPartnerBar.h" />' + E,
      "UiPartnerCommon.h")

p = os.path.join(PRJ, "S3Client.vcxproj.filters")
patch(p,
      '    <ClCompile Include="ui\\uicase\\UiDiceItem.cpp">' + E +
      "      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>" + E +
      "    </ClCompile>" + E,
      '    <ClCompile Include="ui\\uicase\\UiDiceItem.cpp">' + E +
      "      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>" + E +
      "    </ClCompile>" + E +
      "".join('    <ClCompile Include="ui\\uicase\\%s.cpp">' % x + E +
              "      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>" + E +
              "    </ClCompile>" + E
              for x in ["UiPartnerCommon", "UiPartnerAttr", "UiPartnerSkill",
                        "UiPartnerBag", "UiPartnerBar"]),
      'uicase\\UiPartnerCommon.cpp">')
s = doc(p)
if 'uicase\\UiPartnerCommon.h">' not in s:
    neo = '    <ClInclude Include="ui\\uicase\\UiDiceItem.h">'
    i = s.find(neo)
    assert i > 0
    j = s.find("</ClInclude>", i) + len("</ClInclude>") + 2
    them = "".join('    <ClInclude Include="ui\\uicase\\%s.h">' % x + E +
                   "      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>" + E +
                   "    </ClInclude>" + E
                   for x in ["UiPartnerCommon", "UiPartnerAttr", "UiPartnerSkill",
                             "UiPartnerBag", "UiPartnerBar"])
    ghi(p, s[:j] + them + s[j:])
    print("  VA: filters ClInclude x5")
else:
    print("  da co: filters ClInclude")

# ---------- 5. autoexec.lua (data client) ----------
p = AEX
s = doc(p)
if "partnerbar" not in s:
    neo = 'AddCommand("F12", "", "Open([[NewTask]])")'
    assert s.count(neo) == 1
    them = (neo + E +
            "-- [BDH-G4] Ban Dong Hanh: phim nhu tip ban goc (Y/I/U + thao tac)" + E +
            'AddCommand("Y", "", "Open([[partner]])")' + E +
            'AddCommand("I", "", "Open([[partnerskill]])")' + E +
            'AddCommand("U", "", "Open([[partnerbag]])")' + E +
            'AddCommand("P", "", "Open([[partnerbar]])")' + E +
            'AddCommand("L", "", "Open([[partnertalk]])")' + E +
            'AddCommand("B", "", "Open([[partnercall]])")' + E +
            'AddCommand("F", "", "Open([[partnerattack]])")' + E +
            'AddCommand("G", "", "Open([[partnerfollow]])")' + E +
            'AddCommand("N", "", "Open([[partnerselect]])")')
    ghi(p, s.replace(neo, them, 1), ".truoc_bdh_g4")
    print("  VA: autoexec.lua 9 phim")
else:
    print("  da co: autoexec.lua")

print("XONG b10b")
