# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Hoan thien tooltip o trang bi:
1. Them sPetItemName() - tra ten mon tu bang magicscript (cot 1) theo id.
2. Bat WM_MOUSEMOVE trong KUiPet::WndProc: tinh o trang bi dang tro (theo
   toa do 10 o: x = 328 + k*30, y = 82, 24x24 - trong toa do CUA SO), goi
   g_MouseOver.SetMouseHoverInfo + SetMouseHoverTitle (khuon UiPlayerBar).
3. Khai bao Breathe/WndProc da co; them #include MouseHover.h neu thieu.
"""
import io

CR = chr(13)
p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
lf = io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")

# ---- include ----
if "MouseHover.h" not in lf:
    i = lf.find("#include")
    j = lf.find("\n", i)
    lf = lf[:j + 1] + '#include "../Elem/MouseHover.h"\t// [30/08] tooltip o trang bi\n' + lf[j + 1:]
    print("1. them include MouseHover.h")

# ---- sPetItemName ----
if "static void sPetItemName" not in lf:
    neo = "// [30/08] Tooltip o trang bi"
    assert lf.count(neo) == 1
    them = """// [30/08] ten mon theo id (cot 1 bang magicscript)
static void sPetItemName(int nParticular, char* szOut, int nOutLen)
{
	szOut[0] = 0;
	static KTabFile s_TenTab;
	static int s_bLoaded = 0;
	if (!s_bLoaded)
	{
		s_bLoaded = 1;
		s_TenTab.Load((LPSTR)"\\\\settings\\\\item\\\\magicscript.txt");
	}
	// bang JX1: record index = particular -> hang = particular + 2
	s_TenTab.GetString(nParticular + 2, 1, (LPSTR)"", szOut, nOutLen);
}

"""
    lf = lf.replace(neo, them + neo, 1)
    print("2. them sPetItemName")

# ---- bat chuot trong WndProc ----
if "WM_MOUSEMOVE" not in lf:
    neo = "    default:\n        return KWndImage::WndProc(uMsg, uParam, nParam);"
    assert lf.count(neo) == 1, lf.count(neo)
    them = """    case WM_MOUSEMOVE:
    {
        // [30/08] tooltip o trang bi: toa do 10 o trong INI (x 328 + k*30,
        // y 82, 24x24) - uParam/nParam la toa do chuot tuyet doi
        int nX = (int)uParam - m_nAbsoluteLeft;
        int nY = nParam - m_nAbsoluteTop;
        int nO = -1;
        if (nY >= 82 && nY <= 106)
        {
            for (int k = 0; k < 10; k++)
            {
                int nL = 328 + k * 30;
                if (nX >= nL && nX <= nL + 24)
                {
                    nO = k;
                    break;
                }
            }
        }
        if (nO >= 0 && sPetTV(5143 + nO) >= 4907)
        {
            char szTip[256];
            sPetEquipTip(nO, szTip, sizeof(szTip));
            if (szTip[0])
            {
                g_MouseOver.SetMouseHoverInfo((void*)(KWndWindow*)this, nO,
                    (int)uParam, nParam, false, true);
                g_MouseOver.SetMouseHoverTitle(szTip, strlen(szTip), 0xffffffff);
            }
        }
        else if (g_MouseOver.IsMoseHoverWndObj((void*)(KWndWindow*)this, -1) == 0)
        {
            g_MouseOver.CancelMouseHoverInfo();
        }
        break;
    }
    default:
        return KWndImage::WndProc(uMsg, uParam, nParam);"""
    lf = lf.replace(neo, them, 1)
    print("3. bat WM_MOUSEMOVE -> tooltip")

io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
print("XONG p81")
