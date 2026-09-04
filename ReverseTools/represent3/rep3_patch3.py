# -*- coding: utf-8 -*-
"""[REP3 03/09 LOAD] TextureResMgr::GetImage: khi LoadImage that bai, ma cu van CHEN mot muc voi m_pTextureRes = NULL
va khong bao gio thu nap lai (CheckBalance cung bo qua muc NULL) -> anh do 'trong suot' vinh vien trong phien
(Represent2 nap lai moi lan ve). Vá: khong chen muc NULL; muc NULL con sot thi thu nap lai; ghi jx_rep3.log
'[REP3] LoadImage FAIL' (toi da 200 dong) de biet vi sao (pak doi luc game dang chay, ten sai...)."""
import io, os

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def nl(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
def rep(text, old, new, name, count=1):
    n = text.count(old)
    assert n == count, "%s: found %d (expected %d)" % (name, n, count)
    return text.replace(old, new)

MARK = "[REP3 03/09 LOAD]"
ROOT = r"D:\GAMEDEVNEW_wt_rep3\Sources\Represent\Represent3"
p = os.path.join(ROOT, "TextureResMgr.cpp"); s = rd(p); N = nl(s)
if MARK not in s:
    # helper ghi log that bai (gioi han 200 dong)
    helper = N.join([
        "",
        "// %s ghi ten anh nap that bai vao jx_rep3.log (toi da 200 dong) - truoc day im lang va cache NULL vinh vien" % MARK,
        "static void Rep3LogLoadFail(const char* pszImage, int nType)",
        "{",
        "\tstatic int s_nCount = 0;",
        "\tif (s_nCount >= 200)",
        "\t\treturn;",
        "\ts_nCount++;",
        '\tRep3Log("[REP3] LoadImage FAIL (%d/200) type=%d: %s", s_nCount, nType, pszImage ? pszImage : "");',
        "}",
        "",
    ])
    s = rep(s, "TextureResMgr::TextureResMgr()" + N, helper + "TextureResMgr::TextureResMgr()" + N, "ctor anchor")

    # nhanh tim thay: muc NULL -> thu nap lai
    old_found = ("\t\tif (m_TextureResList[nImagePosition].m_nType == nType)" + N +
                 "\t\t{" + N +
                 "\t\t\tm_TextureResList[nImagePosition].m_nLastUsedTime = GetTickCount();" + N +
                 "\t\t\tpObject = m_TextureResList[nImagePosition].m_pTextureRes;" + N +
                 "\t\t}" + N)
    new_found = ("\t\tif (m_TextureResList[nImagePosition].m_nType == nType)" + N +
                 "\t\t{" + N +
                 "\t\t\tm_TextureResList[nImagePosition].m_nLastUsedTime = GetTickCount();" + N +
                 "\t\t\tpObject = m_TextureResList[nImagePosition].m_pTextureRes;" + N +
                 "\t\t\tif (!pObject)\t// %s muc NULL (nap that bai truoc do) -> thu nap lai thay vi bo ve vinh vien" % MARK + N +
                 "\t\t\t{" + N +
                 "\t\t\t\tpObject = LoadImage(pszImage, nType);" + N +
                 "\t\t\t\tif (pObject)" + N +
                 "\t\t\t\t{" + N +
                 "\t\t\t\t\tm_nLoadCount++;" + N +
                 "\t\t\t\t\tm_TextureResList[nImagePosition].m_pTextureRes = pObject;" + N +
                 '\t\t\t\t\tRep3Log("[REP3] LoadImage OK sau khi that bai: %s", pszImage);' + N +
                 "\t\t\t\t}" + N +
                 "\t\t\t\telse" + N +
                 "\t\t\t\t\tRep3LogLoadFail(pszImage, nType);" + N +
                 "\t\t\t}" + N +
                 "\t\t}" + N)
    s = rep(s, old_found, new_found, "found branch")

    # nhanh chen moi: LoadImage that bai -> KHONG chen muc NULL
    old_ins = ("\t\tpObject = LoadImage(pszImage, nType);" + N +
               "\t\tm_nLoadCount++;" + N +
               "\t\t" + N +
               "\t\tResNode node;" + N)
    if old_ins not in s:
        old_ins = ("\t\tpObject = LoadImage(pszImage, nType);" + N +
                   "\t\tm_nLoadCount++;" + N +
                   N +
                   "\t\tResNode node;" + N)
    new_ins = ("\t\tpObject = LoadImage(pszImage, nType);" + N +
               "\t\tm_nLoadCount++;" + N +
               "\t\tif (!pObject)\t// %s khong chen muc NULL (ma cu chen -> khong bao gio nap lai, CheckBalance cung bo qua)" % MARK + N +
               "\t\t{" + N +
               "\t\t\tRep3LogLoadFail(pszImage, nType);" + N +
               "\t\t\treturn NULL;" + N +
               "\t\t}" + N +
               N +
               "\t\tResNode node;" + N)
    s = rep(s, old_ins, new_ins, "insert branch")
    wr(p, s); print("TextureResMgr.cpp OK")
else:
    print("TextureResMgr.cpp da va")
print("DONE")
