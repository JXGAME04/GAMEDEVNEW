# -*- coding: utf-8 -*-
r"""[25/08] Va CLIENT: parse tag chan dung NPC trong thoai SelectUI (KPlayer.cpp).

BAI HOC vua tra gia: Edit tool pha byte TCVN3 cua KPlayer.cpp (chuoi phinh utf-8
-> C2117 szMagicScriptName tran [32]) => PHAI va file C++ co tieng Viet bang
python latin-1 nhu file .lua. File da git checkout ve sach truoc khi chay.

Noi dung va (3 diem, nhu da thiet ke):
 1. khai bao bTagNpcImage canh pImage
 2. sau g_StrCpyLen(Question): parse "<#>" + "<link=image[a,b]:path>Ten<link>"
    -> pImage->ImageFile/MaxFrame, bTagNpcImage=TRUE, dung lai chuoi = "Ten..."
 3. case 0: bTagNpcImage -> CoreDataChanged(..., (int)pImage)  (mo KUiMsgSel2 co anh)
 4. truoc free(pQuest): free pImage neu con (fix ro ri co san case 0/2)
"""
import io, os

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayer.cpp"
ENC = "latin-1"
s = io.open(P, "r", encoding=ENC, newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"
assert "bTagNpcImage" not in s, "da va roi"

# ---- 1) khai bao co ----
cu1 = ("\t\t\t\t\tKUiNpcSpr                *pImage = NULL;" + NL +
       "\t\t\t\t\tif (pScriptAction->m_nBufferLen <= 0) break;")
assert s.count(cu1) == 1, "anchor1 = %d" % s.count(cu1)
moi1 = ("\t\t\t\t\tKUiNpcSpr                *pImage = NULL;" + NL +
        "\t\t\t\t\tBOOL\t\t\t\t\tbTagNpcImage = FALSE;\t// [TIN SU 25/08] thoai co tag <link=image...> cua JX2" + NL +
        "\t\t\t\t\tif (pScriptAction->m_nBufferLen <= 0) break;")
s = s.replace(cu1, moi1, 1)

# ---- 2) parse tag sau khi copy Question ----
cu2 = ("\t\t\t\t\t\tg_StrCpyLen(pQuest->Question, strContent, sizeof(pQuest->Question));" + NL +
       "\t\t\t\t\t\tif (m_nLastNpcIndex)")
assert s.count(cu2) == 1, "anchor2 = %d" % s.count(cu2)
parse = NL.join([
"\t\t\t\t\t\tg_StrCpyLen(pQuest->Question, strContent, sizeof(pQuest->Question));",
"\t\t\t\t\t\t// [TIN SU 25/08] Thoai JX2 nhung CHAN DUNG NPC ngay trong van ban:",
"\t\t\t\t\t\t//   \"<#><link=image[a,b]:\\spr\\npcres\\...\\xxx_pst.spr>Ten NPC<link>: noi dung\"",
"\t\t\t\t\t\t// Client JX1 khong parse => tag hien THO va khong co anh (chu game bao 25/08).",
"\t\t\t\t\t\t// Tach tag: path + so khung nap vao pImage roi mo hop CO ANH KUiMsgSel2",
"\t\t\t\t\t\t// (co san, truoc gio chi di duong m_Select==1); \"Ten NPC\" giu lai lam dau cau.",
"\t\t\t\t\t\t// An het moi thoai dung DescLink_* (posthouse/messenger/killer). \"<#>\" don le bi bo.",
"\t\t\t\t\t\t{",
"\t\t\t\t\t\t\tchar* pHash = strstr(pQuest->Question, \"<#>\");",
"\t\t\t\t\t\t\tif (pHash)",
"\t\t\t\t\t\t\t\tmemmove(pHash, pHash + 3, strlen(pHash + 3) + 1);",
"\t\t\t\t\t\t\tchar* pTag = strstr(pQuest->Question, \"<link=image[\");",
"\t\t\t\t\t\t\tif (pTag)",
"\t\t\t\t\t\t\t{",
"\t\t\t\t\t\t\t\tchar* pNum = pTag + 12;\t\t\t// sau \"<link=image[\"",
"\t\t\t\t\t\t\t\tchar* pComma = strchr(pNum, ',');",
"\t\t\t\t\t\t\t\tchar* pColon = strchr(pNum, ':');",
"\t\t\t\t\t\t\t\tchar* pGt = pColon ? strchr(pColon, '>') : NULL;",
"\t\t\t\t\t\t\t\tchar* pEnd = pGt ? strstr(pGt + 1, \"<link>\") : NULL;",
"\t\t\t\t\t\t\t\tif (pComma && pColon && pGt && pEnd &&",
"\t\t\t\t\t\t\t\t\tpGt > pColon + 1 && pGt - pColon - 1 < (int)sizeof(pImage->ImageFile))",
"\t\t\t\t\t\t\t\t{",
"\t\t\t\t\t\t\t\t\tint nFrame = atoi(pComma + 1);",
"\t\t\t\t\t\t\t\t\tint nPathLen = (int)(pGt - pColon - 1);",
"\t\t\t\t\t\t\t\t\tmemcpy(pImage->ImageFile, pColon + 1, nPathLen);",
"\t\t\t\t\t\t\t\t\tpImage->ImageFile[nPathLen] = 0;",
"\t\t\t\t\t\t\t\t\tpImage->MaxFrame = (unsigned short)(nFrame > 0 ? nFrame : 1);",
"\t\t\t\t\t\t\t\t\tbTagNpcImage = TRUE;",
"\t\t\t\t\t\t\t\t\t// xoa \"<link>\" dong TRUOC (pEnd tinh theo chuoi hien tai)...",
"\t\t\t\t\t\t\t\t\tmemmove(pEnd, pEnd + 6, strlen(pEnd + 6) + 1);",
"\t\t\t\t\t\t\t\t\t// ...roi xoa tag mo (phan bi xoa nam SAU pGt nen pGt van hop le)",
"\t\t\t\t\t\t\t\t\tmemmove(pTag, pGt + 1, strlen(pGt + 1) + 1);",
"\t\t\t\t\t\t\t\t}",
"\t\t\t\t\t\t\t}",
"\t\t\t\t\t\t}",
"\t\t\t\t\t\tif (m_nLastNpcIndex)"])
s = s.replace(cu2, parse, 1)

# ---- 3) case 0 mo hop co anh ----
cu3 = ("\t\t\t\t\tcase 0:" + NL +
       "\t\t\t\t\t\t{" + NL +
       "\t\t\t\t\t\t\tCoreDataChanged(GDCNI_QUESTION_CHOOSE,(unsigned int) pQuest, 0);" + NL +
       "\t\t\t\t\t\t}" + NL +
       "\t\t\t\t\t\tbreak;")
assert s.count(cu3) == 1, "anchor3 = %d" % s.count(cu3)
moi3 = ("\t\t\t\t\tcase 0:" + NL +
        "\t\t\t\t\t\t{" + NL +
        "\t\t\t\t\t\t\t// [TIN SU 25/08] thoai co tag <link=image...> -> mo hop CO ANH nhu Linux" + NL +
        "\t\t\t\t\t\t\tif (bTagNpcImage)" + NL +
        "\t\t\t\t\t\t\t\tCoreDataChanged(GDCNI_QUESTION_CHOOSE,(unsigned int) pQuest, (int) pImage);" + NL +
        "\t\t\t\t\t\t\telse" + NL +
        "\t\t\t\t\t\t\t\tCoreDataChanged(GDCNI_QUESTION_CHOOSE,(unsigned int) pQuest, 0);" + NL +
        "\t\t\t\t\t\t}" + NL +
        "\t\t\t\t\t\tbreak;")
s = s.replace(cu3, moi3, 1)

# ---- 4) free pImage truoc free(pQuest) ----
cu4 = ("\t\t\t\t\t/******************************************the end********************************************************/ " + NL +
       "\t\t\t\t\tfree(pQuest); ")
assert s.count(cu4) == 1, "anchor4 = %d" % s.count(cu4)
moi4 = ("\t\t\t\t\t/******************************************the end********************************************************/ " + NL +
        "\t\t\t\t\t// [TIN SU 25/08] fix ro ri co san: case 0/2 malloc pImage khong free" + NL +
        "\t\t\t\t\t// (case 1 da free + NULL nen guard nay khong double-free)." + NL +
        "\t\t\t\t\tif (pImage)" + NL +
        "\t\t\t\t\t{" + NL +
        "\t\t\t\t\t\tfree(pImage);" + NL +
        "\t\t\t\t\t\tpImage = NULL;" + NL +
        "\t\t\t\t\t}" + NL +
        "\t\t\t\t\tfree(pQuest); ")
s = s.replace(cu4, moi4, 1)

io.open(P, "w", encoding=ENC, newline="").write(s)
print("DA VA KPlayer.cpp (4 diem) bang latin-1 - khong dung byte TCVN3")
