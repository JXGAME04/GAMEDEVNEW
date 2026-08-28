# -*- coding: utf-8 -*-
"""x3_dopho_nhan_nguyenlieu.py - VE TEN NGUYEN LIEU TREN TRANG DO PHO (dung ban goc).

Lam LAI cho dung sau khi go mieng va tu che (x2). Lan nay DOC BO CUC GOC TRUOC:

`Dopho.ini` (chep nguyen van tu pak VLTK) co san:
    [AtlasPos]    Pos=66,109      <- nhan cho o Do pho
    [CryolitePos] Pos=156,110     <- nhan cho o Huyen Tinh
    [ConsumePos]  Pos=110,108     <- nhan cho cum o nguyen lieu
    [Material_0]  Pos=47,43       [Material_3] Pos=174,174
    [Material_1]  Pos=175,43      [Material_4] Pos=46,173
    [Material_2]  Pos=195,110     [Material_5] Pos=27,112
    [TextColor]   Font=110,60,10
=> 6 vi tri VE TEN NGUYEN LIEU ngay canh 6 o tren TRANG lo ren (khong phai tren
   tooltip vat pham nhu toi da bia ra o lan truoc).

Khuon dung y het `KUiEnchase::LoadScheme` (UiCompoundItem.cpp:3346-3370):
    Ini.GetInteger2("<ten>Pos","Pos",&nX,&nY);
    m_Pos.SetPosition(nX-..,nY-4); m_Pos.SetTextColor(nColor);
    m_Pos.BringToTop();           m_Pos.SetText("Trang bÞ tÝm");
va chuoi tieng Viet viet RAW TCVN3 y nhu cac dong san co - KHONG go escape.
(Chuoi trong mieng va nay duoc SINH bang `bangtxt.uni2tcvn` roi nhung duoi dang
 escape PYTHON `\\xNN`, nen tep .cpp nhan duoc BYTE TCVN3 that.)

CACH LAY TEN NGUYEN LIEU: khi o AtlasBox co vien do pho, lay ma (Particular) cua
no roi tra `settings\\item\\atlas_compound.txt` tim dong co ATLAS_PARTICULAR khop,
doc 6 cot `i_NAME` va `i_LEVEL`. Ten trong bang DA la TCVN3 nen chi viec ve.

Client chua co duong lay Particular cua mot vat pham => them
`GDI_ITEM_PARTICULAR` (them CUOI enum GAMEDATA_INDEX nen khong xe dich gia tri
cu), khuon y het `GDI_ITEM_NAME` (nParam = chi so vat pham).

CAC MIENG:
  CoreShell.h        + GDI_ITEM_PARTICULAR
  CoreShell.cpp      + case GDI_ITEM_PARTICULAR
  UiCompoundItem.h   + 3 nhan tinh + 6 nhan nguyen lieu trong KUiAtlas
                     + khai bao CapNhatNguyenLieu()
  UiCompoundItem.cpp + include KTabFile, dat vi tri/mau/chu cho 9 nhan,
                     + than CapNhatNguyenLieu(), goi trong UpdateAllItem

Tep dung chung client+server => build Core CA HAI cau hinh + S3Client.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_nhannl lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] Do pho: ve ten nguyen lieu tren trang"
HAU_TO = ".truoc_nhannl"
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
UI = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase"

# chuoi RAW TCVN3, sinh bang bangtxt.uni2tcvn (khong go tay)
S_DOPHO = "\xa7\xe5 ph\xe6"          # Do pho
S_HUYENTINH = "Huy\xd2n tinh"        # Huyen tinh
S_NGUYENLIEU = "Nguy\xaan li\xd6u"   # Nguyen lieu


def K(*d):
    return list(d)


CSH = [
    ("GDI_ITEM_PARTICULAR",
     K(T + "GDI_ATLAS_ITEM,		// [LOREN 27/08] THE DO PHO - o cua the Do pho (them CUOI)"),
     K(T + "GDI_ATLAS_ITEM,		// [LOREN 27/08] THE DO PHO - o cua the Do pho (them CUOI)",
       T + "GDI_ITEM_PARTICULAR,	// " + NHAN + " (them CUOI)")),
]

CSC = [
    ("case GDI_ITEM_PARTICULAR",
     K(T + "case GDI_ITEM_NAME:			//lay ten item"),
     K(T + "case GDI_ITEM_PARTICULAR:	// " + NHAN,
       T*2 + "// nParam = chi so vat pham (khuon y het GDI_ITEM_NAME ngay duoi).",
       T*2 + "nRet = 0;",
       T*2 + "if (nParam > 0 && nParam < MAX_ITEM)",
       T*3 + "nRet = Item[nParam].GetParticular();",
       T*2 + "break;",
       T + "case GDI_ITEM_NAME:			//lay ten item")),
]

UIH = [
    ("nhan trong KUiAtlas",
     K(T + "KWndButton		m_Atlas;",
       T + "KWndImage		m_AtlasEffect;"),
     K(T + "KWndButton		m_Atlas;",
       T + "KWndImage		m_AtlasEffect;",
       T + "// " + NHAN,
       T + "KWndText32		m_PosAtlas;",
       T + "KWndText32		m_PosCryolite;",
       T + "KWndText32		m_PosConsume;",
       T + "KWndText32		m_Material[6];")),
    ("khai bao CapNhatNguyenLieu",
     K(T + "void	ProcessAtlas();"),
     K(T + "void	ProcessAtlas();",
       T + "void	CapNhatNguyenLieu();		// " + NHAN)),
]

# ---- than ham, chen truoc PaintWindow cua KUiAtlas ----
THAN = K(
    "// " + NHAN + ".",
    "// Doc ma (Particular) cua vien do pho dang nam trong o AtlasBox, tra bang",
    "// atlas_compound.txt tim dong co ATLAS_PARTICULAR khop, roi ve ten 6 nguyen",
    "// lieu vao dung sau vi tri Material_0..5 cua bo cuc goc.",
    "// Ten trong bang DA la TCVN3 nen chi viec ve, khong phai chuyen ma.",
    "void KUiAtlas::CapNhatNguyenLieu()",
    "{",
    T + "for (int k = 0; k < 6; k++)",
    T*2 + "m_Material[k].SetText(\"\");",
    "",
    T + "KUiDraggedObject Obj;",
    T + "memset(&Obj, 0, sizeof(Obj));",
    T + "m_ItemBox[0].GetObject(Obj);",
    T + "if (Obj.uId <= 0 || !g_pCoreShell)",
    T*2 + "return;",
    T + "int nPtc = g_pCoreShell->GetGameData(GDI_ITEM_PARTICULAR, 0, (int)Obj.uId);",
    T + "if (nPtc <= 0)",
    T*2 + "return;",
    "",
    T + "KTabFile Tab;",
    T + "if (!Tab.Load(\"\\\\Settings\\\\Item\\\\atlas_compound.txt\"))",
    T*2 + "return;",
    T + "int nHang = 0;",
    T + "for (int r = 2; r <= Tab.GetHeight(); r++)",
    T + "{",
    T*2 + "int nCur = -1;",
    T*2 + "Tab.GetInteger(r, (LPSTR)\"ATLAS_PARTICULAR\", -1, &nCur);",
    T*2 + "if (nCur == nPtc)",
    T*2 + "{",
    T*3 + "nHang = r;",
    T*3 + "break;",
    T*2 + "}",
    T + "}",
    T + "if (nHang < 2)",
    T*2 + "return;",
    "",
    T + "for (int i = 1; i <= 6; i++)",
    T + "{",
    T*2 + "char szCot[32];",
    T*2 + "char szTen[64];",
    T*2 + "_snprintf(szCot, sizeof(szCot) - 1, \"%d_NAME\", i);",
    T*2 + "szCot[sizeof(szCot) - 1] = 0;",
    T*2 + "szTen[0] = 0;",
    T*2 + "Tab.GetString(nHang, (LPSTR)szCot, (LPSTR)\"\", szTen, sizeof(szTen));",
    T*2 + "if (szTen[0] == 0)",
    T*3 + "continue;",
    T*2 + "int nCap = -1;",
    T*2 + "_snprintf(szCot, sizeof(szCot) - 1, \"%d_LEVEL\", i);",
    T*2 + "szCot[sizeof(szCot) - 1] = 0;",
    T*2 + "Tab.GetInteger(nHang, (LPSTR)szCot, -1, &nCap);",
    T*2 + "char szDong[96];",
    T*2 + "if (nCap > 0)",
    T*3 + "_snprintf(szDong, sizeof(szDong) - 1, \"%s (%d)\", szTen, nCap);",
    T*2 + "else",
    T*3 + "_snprintf(szDong, sizeof(szDong) - 1, \"%s\", szTen);",
    T*2 + "szDong[sizeof(szDong) - 1] = 0;",
    T*2 + "m_Material[i - 1].SetText(szDong);",
    T + "}",
    "}",
    "",
)

UIC = [
    ("include KTabFile",
     K("#include \"../../../Engine/src/Text.h\""),
     K("#include \"../../../Engine/src/Text.h\"",
       "#include \"../../../Engine/src/KTabFile.h\"	// " + NHAN)),

    ("dat 9 nhan trong LoadScheme",
     K(T*2 + "m_AtlasEffect.Init(&Ini, \"ConsumeEffect\");",
       T*2 + "m_AtlasEffect.Hide();"),
     K(T*2 + "m_AtlasEffect.Init(&Ini, \"ConsumeEffect\");",
       T*2 + "m_AtlasEffect.Hide();",
       "",
       T*2 + "// " + NHAN + " - khuon y het KUiEnchase::LoadScheme.",
       T*2 + "int nX, nY, nColor = 0xFFFFFF;",
       T*2 + "if (Ini.GetString(\"TextColor\", \"Font\", \"\", Buff, sizeof(Buff)))",
       T*3 + "nColor = (::GetColor(Buff) & 0xFFFFFF);",
       "",
       T*2 + "Ini.GetInteger2(\"AtlasPos\", \"Pos\", &nX, &nY);",
       T*2 + "m_PosAtlas.SetPosition(nX - 28, nY - 4);",
       T*2 + "m_PosAtlas.SetTextColor(nColor);",
       T*2 + "m_PosAtlas.BringToTop();",
       T*2 + "m_PosAtlas.SetText(\"" + S_DOPHO + "\");",
       "",
       T*2 + "Ini.GetInteger2(\"CryolitePos\", \"Pos\", &nX, &nY);",
       T*2 + "m_PosCryolite.SetPosition(nX - 28, nY - 4);",
       T*2 + "m_PosCryolite.SetTextColor(nColor);",
       T*2 + "m_PosCryolite.BringToTop();",
       T*2 + "m_PosCryolite.SetText(\"" + S_HUYENTINH + "\");",
       "",
       T*2 + "Ini.GetInteger2(\"ConsumePos\", \"Pos\", &nX, &nY);",
       T*2 + "m_PosConsume.SetPosition(nX - 28, nY - 4);",
       T*2 + "m_PosConsume.SetTextColor(nColor);",
       T*2 + "m_PosConsume.BringToTop();",
       T*2 + "m_PosConsume.SetText(\"" + S_NGUYENLIEU + "\");",
       "",
       T*2 + "for (int m = 0; m < 6; m++)",
       T*2 + "{",
       T*3 + "char szMuc[32];",
       T*3 + "_snprintf(szMuc, sizeof(szMuc) - 1, \"Material_%d\", m);",
       T*3 + "szMuc[sizeof(szMuc) - 1] = 0;",
       T*3 + "nX = nY = 0;",
       T*3 + "Ini.GetInteger2(szMuc, \"Pos\", &nX, &nY);",
       T*3 + "m_Material[m].SetPosition(nX - 28, nY - 4);",
       T*3 + "m_Material[m].SetTextColor(nColor);",
       T*3 + "m_Material[m].BringToTop();",
       T*3 + "m_Material[m].SetText(\"\");",
       T*2 + "}")),

    ("them 9 nhan vao Initialize",
     K(T + "AddChild(&m_AtlasEffect);"),
     K(T + "AddChild(&m_AtlasEffect);",
       T + "// " + NHAN,
       T + "AddChild(&m_PosAtlas);",
       T + "AddChild(&m_PosCryolite);",
       T + "AddChild(&m_PosConsume);",
       T + "for (int m = 0; m < 6; m++)",
       T*2 + "AddChild(&m_Material[m]);")),

    ("than CapNhatNguyenLieu",
     K("void KUiAtlas::PaintWindow()"),
     THAN + K("void KUiAtlas::PaintWindow()")),

    ("goi khi lam moi o",
     K(T + "for (i = 0; i < nCount; i++)",
       T + "{",
       T*2 + "if (Item[i].Obj.uGenre != CGOG_NOTHING)",
       T*3 + "UpdateItem(&Item[i], true);",
       T + "}",
       "}",
       "",
       "void KUiAtlas::UpdateItem(KUiObjAtRegion* pItem, int bAdd)"),
     K(T + "for (i = 0; i < nCount; i++)",
       T + "{",
       T*2 + "if (Item[i].Obj.uGenre != CGOG_NOTHING)",
       T*3 + "UpdateItem(&Item[i], true);",
       T + "}",
       T + "CapNhatNguyenLieu();		// " + NHAN,
       "}",
       "",
       "void KUiAtlas::UpdateItem(KUiObjAtRegion* pItem, int bAdd)")),
]

BANG = [
    (os.path.join(CORE, "CoreShell.h"), CSH),
    (os.path.join(CORE, "CoreShell.cpp"), CSC),
    (os.path.join(UI, "UiCompoundItem.h"), UIH),
    (os.path.join(UI, "UiCompoundItem.cpp"), UIC),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== x3_dopho_nhan_nguyenlieu - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    ketqua = []
    for p, mieng in BANG:
        if not os.path.isfile(p):
            print("!!! LOI TO: khong thay %s" % p)
            return 1
        raw = io.open(p, "rb").read().decode("latin-1")
        n_crlf = raw.count("\r\n")
        eol = "\r\n" if n_crlf >= (raw.count("\n") - n_crlf) else "\n"
        hi0 = sum(1 for c in raw if ord(c) > 127)
        print("\n--- %s ---" % os.path.basename(p))
        if NHAN in raw:
            print("   DA CO - bo qua")
            continue
        dong = raw.split(eol)
        for ten, cu, moi in mieng:
            vt = [i for i in range(len(dong) - len(cu) + 1) if dong[i:i + len(cu)] == cu]
            if len(vt) != 1:
                print("   !!! %-30s khop %d lan (can 1)" % (ten, len(vt)))
                return 1
            print("   ok  %-30s dong %d (+%d)" % (ten, vt[0] + 1, len(moi) - len(cu)))
            dong = dong[:vt[0]] + moi + dong[vt[0] + len(cu):]
        nd = eol.join(dong)
        hi1 = sum(1 for c in nd if ord(c) > 127)
        them = hi1 - hi0
        # 3 chuoi TCVN3 moi: Do pho 3 + Huyen tinh 1 + Nguyen lieu 2 = 6 byte cao
        mong = 6 if p.endswith("UiCompoundItem.cpp") else 0
        if them != mong:
            print("   !!! byte cao %d -> %d (them %d, mong %d)" % (hi0, hi1, them, mong))
            return 1
        print("   byte cao %d -> %d (+%d chuoi TCVN3 dung nhu mong)" % (hi0, hi1, them))
        if nd.count("{") - raw.count("{") != nd.count("}") - raw.count("}"):
            print("   !!! ngoac lech")
            return 1
        try:
            nd.encode("latin-1")
        except UnicodeEncodeError as e:
            print("   !!! ngoai latin-1: %s" % e)
            return 1
        ketqua.append((p, nd))

    if not ketqua:
        print("\nKhong co gi de ghi.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0
    for p, nd in ketqua:
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if io.open(p, "rb").read().decode("latin-1") != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    print("\n  => build Core CA HAI cau hinh + S3Client, thay dong bo 3 nhi phan")
    return 0


if __name__ == "__main__":
    sys.exit(main())
