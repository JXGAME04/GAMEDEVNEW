# -*- coding: utf-8 -*-
r"""[24/08] VA loi the mau <color> hien ra thanh CHU trong bang chi nam nhiem vu.

GOC: TEncodeText (Engine\Src\Text.cpp:468) coi MOI byte > 0x80 la chu Han 2 byte va
chep luon byte ke tiep. Tieng Viet TCVN3 la 1 BYTE, nen khi the dung ngay sau mot chu
co dau (vi du "...Tin Su<color>"), dau '<' bi nuot vao cap 2 byte => parser khong thay
the nua => phan "color>" con lai in ra man hinh.

VA: tien xu ly TRUOC khi goi TEncodeText - chen 1 dau cach truoc moi '<' neu byte lien
truoc > 0x80. Dau cach do se bi nuot thay cho '<' (nen KHONG hien ra), con '<' tro thanh
byte dau => parser xu ly the binh thuong.
Sua MOT cho (AddLine) nen ap cho toan bo noi dung bang nhiem vu.
"""
import io, shutil

P = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiTaskGuide.cpp"
s = io.open(P, "r", encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"

if "DTG_FixTagAfterVn" in s:
    print("da va truoc do"); raise SystemExit(0)

old = ("void KUiTaskGuide::AddLine(const char* pText)" + NL +
       "{" + NL +
       "\tchar szBuf[2048];" + NL +
       "\tstrncpy(szBuf, pText, sizeof(szBuf) - 1);" + NL +
       "\tszBuf[sizeof(szBuf) - 1] = 0;" + NL +
       "\tint nLen = TEncodeText(szBuf, strlen(szBuf));" + NL)
assert s.count(old) == 1, "anchor AddLine = %d" % s.count(old)

new = (
"// [FIX 24/08] TEncodeText (Engine\\Src\\Text.cpp:468) coi MOI byte > 0x80 la chu Han 2 byte" + NL +
"// va chep luon byte ke tiep. Tieng Viet TCVN3 la 1 BYTE => the dung ngay sau chu co dau" + NL +
"// (vd \"...Tin Su<color>\") bi nuot mat dau '<', parser khong nhan ra the nua nen \"color>\"" + NL +
"// in thang ra man hinh. Chen 1 dau cach truoc '<' khi byte lien truoc > 0x80: dau cach do" + NL +
"// se bi nuot THAY cho '<' (nen khong hien ra), con '<' tro thanh byte dau va duoc xu ly." + NL +
"static void DTG_FixTagAfterVn(char* pBuf, size_t nCap)" + NL +
"{" + NL +
"\tif (!pBuf) return;" + NL +
"\tchar szTmp[2048];" + NL +
"\tsize_t i = 0, o = 0;" + NL +
"\tsize_t nLen = strlen(pBuf);" + NL +
"\tfor (; i < nLen && o + 2 < sizeof(szTmp); i++)" + NL +
"\t{" + NL +
"\t\tif (pBuf[i] == '<' && o > 0 && (unsigned char)szTmp[o - 1] > 0x80)" + NL +
"\t\t\tszTmp[o++] = ' ';" + NL +
"\t\tszTmp[o++] = pBuf[i];" + NL +
"\t}" + NL +
"\tszTmp[o] = 0;" + NL +
"\tstrncpy(pBuf, szTmp, nCap - 1);" + NL +
"\tpBuf[nCap - 1] = 0;" + NL +
"}" + NL +
"" + NL +
"void KUiTaskGuide::AddLine(const char* pText)" + NL +
"{" + NL +
"\tchar szBuf[2048];" + NL +
"\tstrncpy(szBuf, pText, sizeof(szBuf) - 1);" + NL +
"\tszBuf[sizeof(szBuf) - 1] = 0;" + NL +
"\tDTG_FixTagAfterVn(szBuf, sizeof(szBuf));\t// [FIX 24/08]" + NL +
"\tint nLen = TEncodeText(szBuf, strlen(szBuf));" + NL)

s = s.replace(old, new, 1)
shutil.copyfile(P, P + ".truoc_fix_color_2408")
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("DA VA UiTaskGuide.cpp: them DTG_FixTagAfterVn + goi trong AddLine")
