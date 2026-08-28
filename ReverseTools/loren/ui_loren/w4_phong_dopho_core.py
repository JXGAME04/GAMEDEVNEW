# -*- coding: utf-8 -*-
"""w4_phong_dopho_core.py - DUNG PHONG DO PHO (Atlas) phia CORE/SERVER.

CHU GAME: "ben do pho van kich khong duoc".

The Do pho la tinh nang CHUA THI CONG phia client, khong phai loi. Ban goc
(bo cuc rut nguyen van tu pak VLTK: update03__(dau tao_hoang kim do pho).ini)
dung MOT PHONG RIENG voi 8 O CHINH:
    AtlasBox     - Do pho Hoang Kim
    CryoliteBox  - Huyen Tinh khoang thach
    Box1..Box6   - 6 o nguyen lieu
    (+ ItemBox   - o tu chon "tang ti le")
Con so 8 KHOP CHINH XAC `s_anSoO[6] = 8` ma may chu JX1 da co san, va
`s_anKhoaTuChon[6] = FRK_CompoundGold_EnhanceItem` cho o tu chon. Tuc phia may
chu DA DU LUAT, chi thieu duong di cua vat pham.

MIENG VA NAY chi lam phan LOI (phong + duong dong bo). Lop giao dien
`KUiAtlas` lam o mieng rieng.

TAT CA deu NHAN BAN theo khuon `pos_enchase` / `m_EnchaseItem` da chay tot -
khong phat minh co che moi:

  GameDataDef.h
    + `pos_atlas`      them CUOI enum ITEM_POSITION  (sau pos_partnerbag = 25)
    + `UOC_ATLAS_ITEM` them CUOI enum container      (sau UOC_PARTNER_BAG)
    => THEM CUOI nen KHONG xe dich gia tri cu = khong pha du lieu/giao thuc cu.

  KItemList.h   + m_AtlasItem[outinpart_num], GetAtlasItem,
                  Check/Add/UnAtlasItem
  KItemList.cpp + 9 khoi nhan ban (dat vao o, dong bo container, go ra,
                  doi cho, thu hoi khi dong cua so) + 3 ham
  CoreShell.cpp + 4 khoi (2 cho kiem "con vat pham trong phong", 2 cho
                  anh xa UOC_ -> pos_)
  ScriptFuns.cpp+ noi gioi han phong `nPos > 9` -> `> 10` va them case 10
                  (GetIdItem(10, i) cho kich ban Lua doc o Do pho)

Vi sao AN TOAN voi ban dang chay:
  - Chi THEM nhanh moi; khong sua mot dong nao cua duong cu.
  - Hai enum deu them o CUOI.
  - Phong moi chi duoc dung khi nguoi choi mo the Do pho.

KItemList/CoreShell/GameDataDef dung CHUNG client + server
=> PHAI build CA HAI cau hinh va thay DONG BO ca CoreClient.dll lan
   CoreServer.dll (lech mot ben la vat pham di sai phong).

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_dopho lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 27/08] PHONG DO PHO"

D = r"D:\GAMEDEVNEW\Sources\Core\Src"
HAU_TO = ".truoc_dopho"


def K(*dong):
    return list(dong)


# =========================================================================
# 1. GameDataDef.h
# =========================================================================
GDD = [
    # (mo ta, khoi cu, khoi moi)
    ("pos_atlas",
     K(T + "pos_partnerbag,	// =25 [BDH-G3] tui ban dong hanh (them CUOI - giu gia tri cu)"),
     K(T + "pos_partnerbag,	// =25 [BDH-G3] tui ban dong hanh (them CUOI - giu gia tri cu)",
       T + "pos_atlas,		// " + NHAN + " - phong the Do pho Hoang Kim",
       T + "			// (them CUOI nen KHONG xe dich gia tri cu)")),
    ("UOC_ATLAS_ITEM",
     K(T + "UOC_PARTNER_BAG,	// [BDH-G3] tui ban dong hanh		//On the mutual gambling panel, others use it for gambling"),
     K(T + "UOC_PARTNER_BAG,	// [BDH-G3] tui ban dong hanh		//On the mutual gambling panel, others use it for gambling",
       T + "UOC_ATLAS_ITEM,		// " + NHAN + " - o cua the Do pho Hoang Kim")),
]

# =========================================================================
# 2. KItemList.h
# =========================================================================
KIL_H = [
    ("m_AtlasItem",
     K(T + "int" + T*3 + "m_EnchaseItem[outinpart_num];"),
     K(T + "int" + T*3 + "m_EnchaseItem[outinpart_num];",
       T + "int" + T*3 + "m_AtlasItem[outinpart_num];" + T*2 + "// " + NHAN)),
    ("GetAtlasItem",
     K(T + "int" + T*3 + "GetEnchaseItem(int nIdx) { return m_EnchaseItem[nIdx]; }"),
     K(T + "int" + T*3 + "GetEnchaseItem(int nIdx) { return m_EnchaseItem[nIdx]; }",
       T + "int" + T*3 + "GetAtlasItem(int nIdx) { return m_AtlasItem[nIdx]; }" + T + "// " + NHAN)),
    ("khai bao 3 ham",
     K(T + "void" + T*2 + "UnEnchaseItem(int nIdx, int nPlace = -1);"),
     K(T + "void" + T*2 + "UnEnchaseItem(int nIdx, int nPlace = -1);",
       T + "// " + NHAN,
       T + "BOOL" + T*2 + "CheckAtlasItem(int nIdx, int nPlace = -1);",
       T + "BOOL" + T*2 + "AddAtlasItem(int nIdx, int nPlace = -1);",
       T + "void" + T*2 + "UnAtlasItem(int nIdx, int nPlace = -1);")),
]

# =========================================================================
# 3. KItemList.cpp
# =========================================================================
KIL_C = [
    ("dat vao o (AddKIL)",
     K(T + "case pos_enchase:	// [LOREN]",
       T*2 + "if (nX < 0 || nX >= outinpart_num)",
       T*3 + "return 0;",
       T*2 + "if (m_EnchaseItem[nX])",
       T*3 + "return 0;",
       T*2 + "m_Items[i].nPlace = pos_enchase;",
       T*2 + "m_Items[i].nX = nX;",
       T*2 + "m_Items[i].nY = 0;",
       T*2 + "break;"),
     K(T + "case pos_enchase:	// [LOREN]",
       T*2 + "if (nX < 0 || nX >= outinpart_num)",
       T*3 + "return 0;",
       T*2 + "if (m_EnchaseItem[nX])",
       T*3 + "return 0;",
       T*2 + "m_Items[i].nPlace = pos_enchase;",
       T*2 + "m_Items[i].nX = nX;",
       T*2 + "m_Items[i].nY = 0;",
       T*2 + "break;",
       T + "case pos_atlas:	// " + NHAN,
       T*2 + "if (nX < 0 || nX >= outinpart_num)",
       T*3 + "return 0;",
       T*2 + "if (m_AtlasItem[nX])",
       T*3 + "return 0;",
       T*2 + "m_Items[i].nPlace = pos_atlas;",
       T*2 + "m_Items[i].nX = nX;",
       T*2 + "m_Items[i].nY = 0;",
       T*2 + "break;")),

    ("goi AddAtlasItem",
     K(T + "if (m_Items[i].nPlace == pos_enchase)	// [LOREN]",
       T + "{",
       T*2 + "AddEnchaseItem(m_Items[i].nIdx, nX);",
       T + "}"),
     K(T + "if (m_Items[i].nPlace == pos_enchase)	// [LOREN]",
       T + "{",
       T*2 + "AddEnchaseItem(m_Items[i].nIdx, nX);",
       T + "}",
       T + "if (m_Items[i].nPlace == pos_atlas)	// " + NHAN,
       T + "{",
       T*2 + "AddAtlasItem(m_Items[i].nIdx, nX);",
       T + "}")),

    ("dong bo container (them moi)",
     K(T + "case pos_enchase:	// [LOREN] khong can bang doi, v = nX truc tiep",
       T*2 + "pInfo.Region.h = 0;",
       T*2 + "pInfo.Region.v = nX;",
       T*2 + "pInfo.eContainer = UOC_ENCHASE_ITEM;",
       T*2 + "break;"),
     K(T + "case pos_enchase:	// [LOREN] khong can bang doi, v = nX truc tiep",
       T*2 + "pInfo.Region.h = 0;",
       T*2 + "pInfo.Region.v = nX;",
       T*2 + "pInfo.eContainer = UOC_ENCHASE_ITEM;",
       T*2 + "break;",
       T + "case pos_atlas:	// " + NHAN,
       T*2 + "pInfo.Region.h = 0;",
       T*2 + "pInfo.Region.v = nX;",
       T*2 + "pInfo.eContainer = UOC_ATLAS_ITEM;",
       T*2 + "break;")),

    ("go ra khoi o",
     K(T + "case pos_enchase:	// [LOREN]",
       T*2 + "UnEnchaseItem(m_Items[nIdx].nIdx);",
       T*2 + "break;"),
     K(T + "case pos_enchase:	// [LOREN]",
       T*2 + "UnEnchaseItem(m_Items[nIdx].nIdx);",
       T*2 + "break;",
       T + "case pos_atlas:	// " + NHAN,
       T*2 + "UnAtlasItem(m_Items[nIdx].nIdx);",
       T*2 + "break;")),

    ("dong bo container (mon co san)",
     K(T + "case pos_enchase:	// [LOREN]",
       T*2 + "pInfo.Region.h = 0;",
       T*2 + "pInfo.Region.v = m_Items[nIdx].nX;",
       T*2 + "pInfo.eContainer = UOC_ENCHASE_ITEM;",
       T*2 + "break;"),
     K(T + "case pos_enchase:	// [LOREN]",
       T*2 + "pInfo.Region.h = 0;",
       T*2 + "pInfo.Region.v = m_Items[nIdx].nX;",
       T*2 + "pInfo.eContainer = UOC_ENCHASE_ITEM;",
       T*2 + "break;",
       T + "case pos_atlas:	// " + NHAN,
       T*2 + "pInfo.Region.h = 0;",
       T*2 + "pInfo.Region.v = m_Items[nIdx].nX;",
       T*2 + "pInfo.eContainer = UOC_ATLAS_ITEM;",
       T*2 + "break;")),

    ("doi cho hai o (dong bo)",
     K(T*2 + "case pos_enchase:	// [LOREN]",
       T*3 + "pInfo1.Region.h = 0;",
       T*3 + "pInfo1.Region.v = SrcPos->nX;",
       T*3 + "pInfo2.Region.h = 0;",
       T*3 + "pInfo2.Region.v = DesPos->nX;",
       T*3 + "pInfo1.eContainer = UOC_ENCHASE_ITEM;",
       T*3 + "pInfo2.eContainer = UOC_ENCHASE_ITEM;",
       T*3 + "break;"),
     K(T*2 + "case pos_enchase:	// [LOREN]",
       T*3 + "pInfo1.Region.h = 0;",
       T*3 + "pInfo1.Region.v = SrcPos->nX;",
       T*3 + "pInfo2.Region.h = 0;",
       T*3 + "pInfo2.Region.v = DesPos->nX;",
       T*3 + "pInfo1.eContainer = UOC_ENCHASE_ITEM;",
       T*3 + "pInfo2.eContainer = UOC_ENCHASE_ITEM;",
       T*3 + "break;",
       T*2 + "case pos_atlas:	// " + NHAN,
       T*3 + "pInfo1.Region.h = 0;",
       T*3 + "pInfo1.Region.v = SrcPos->nX;",
       T*3 + "pInfo2.Region.h = 0;",
       T*3 + "pInfo2.Region.v = DesPos->nX;",
       T*3 + "pInfo1.eContainer = UOC_ATLAS_ITEM;",
       T*3 + "pInfo2.eContainer = UOC_ATLAS_ITEM;",
       T*3 + "break;")),

    ("thu hoi khi dong cua so",
     K(T*2 + "case pos_enchase:",
       "#ifdef _SERVER" + T,		# CHU Y: dong nay co TAB THUA o cuoi trong tep goc
       T*3 + "for(i = 0; i < outinpart_num; i++)",
       T*3 + "{",
       T*4 + "if(m_EnchaseItem[i] > 0)",
       T*4 + "{",
       T*5 + "int nIndex = ItemSet.AddI(&Item[m_EnchaseItem[i]]);",
       T*5 + "int nX, nY;",
       T*5 + "if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))",
       T*5 + "{",
       T*6 + "AddKIL(nIndex, pos_equiproom, nX, nY);",
       T*6 + "Remove(m_EnchaseItem[i]);",
       T*6 + "ItemSet.Remove(m_EnchaseItem[i]);",
       T*5 + "}",
       T*4 + "}",
       T*3 + "}",
       "#else",
       T*3 + "memset(&m_EnchaseItem, 0, sizeof(m_EnchaseItem));",
       "#endif",
       T*3 + "break;"),
     K(T*2 + "case pos_enchase:",
       "#ifdef _SERVER" + T,		# CHU Y: dong nay co TAB THUA o cuoi trong tep goc
       T*3 + "for(i = 0; i < outinpart_num; i++)",
       T*3 + "{",
       T*4 + "if(m_EnchaseItem[i] > 0)",
       T*4 + "{",
       T*5 + "int nIndex = ItemSet.AddI(&Item[m_EnchaseItem[i]]);",
       T*5 + "int nX, nY;",
       T*5 + "if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))",
       T*5 + "{",
       T*6 + "AddKIL(nIndex, pos_equiproom, nX, nY);",
       T*6 + "Remove(m_EnchaseItem[i]);",
       T*6 + "ItemSet.Remove(m_EnchaseItem[i]);",
       T*5 + "}",
       T*4 + "}",
       T*3 + "}",
       "#else",
       T*3 + "memset(&m_EnchaseItem, 0, sizeof(m_EnchaseItem));",
       "#endif",
       T*3 + "break;",
       T*2 + "case pos_atlas:		// " + NHAN,
       "#ifdef _SERVER" + T,		# CHU Y: dong nay co TAB THUA o cuoi trong tep goc
       T*3 + "for(i = 0; i < outinpart_num; i++)",
       T*3 + "{",
       T*4 + "if(m_AtlasItem[i] > 0)",
       T*4 + "{",
       T*5 + "int nIndex = ItemSet.AddI(&Item[m_AtlasItem[i]]);",
       T*5 + "int nX, nY;",
       T*5 + "if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))",
       T*5 + "{",
       T*6 + "AddKIL(nIndex, pos_equiproom, nX, nY);",
       T*6 + "Remove(m_AtlasItem[i]);",
       T*6 + "ItemSet.Remove(m_AtlasItem[i]);",
       T*5 + "}",
       T*4 + "}",
       T*3 + "}",
       "#else",
       T*3 + "memset(&m_AtlasItem, 0, sizeof(m_AtlasItem));",
       "#endif",
       T*3 + "break;")),

    ("xoa sach khi khoi tao",
     K(T + "ZeroMemory(m_EnchaseItem, sizeof(m_EnchaseItem));"),
     K(T + "ZeroMemory(m_EnchaseItem, sizeof(m_EnchaseItem));",
       T + "ZeroMemory(m_AtlasItem, sizeof(m_AtlasItem));	// " + NHAN)),

    ("ba ham Check/Add/Un",
     K("BOOL KItemList::CheckForgeItem(int nIdx, int nPlace /* = -1 */)"),
     K("// " + NHAN + " - ba ham nhan ban nguyen khuon tu *EnchaseItem.",
       "BOOL KItemList::CheckAtlasItem(int nIdx, int nPlace /* = -1 */)",
       "{",
       T + "if (m_PlayerIdx <= 0 || nIdx <= 0)",
       T*2 + "return FALSE;",
       "",
       T + "int nItemListIdx = FindSame(nIdx);",
       T + "if (!nItemListIdx)",
       T*2 + "return FALSE;",
       "",
       T + "if (nPlace < 0 || nPlace >= outinpart_num)",
       T*2 + "return FALSE;",
       "",
       T + "if (m_AtlasItem[nPlace])",
       T*2 + "return FALSE;",
       "",
       T + "return TRUE;",
       "}",
       "",
       "BOOL KItemList::AddAtlasItem(int nIdx, int nPlace /* = -1 */)",
       "{",
       T + "if (CheckAtlasItem(nIdx, nPlace) == FALSE)",
       T*2 + "return FALSE;",
       "",
       T + "int nItemListIdx = FindSame(nIdx);",
       T + "m_AtlasItem[nPlace] = nIdx;",
       T + "m_Items[nItemListIdx].nPlace = pos_atlas;",
       T + "m_Items[nItemListIdx].nX = nPlace;",
       T + "m_Items[nItemListIdx].nY = 0;",
       T + "return TRUE;",
       "}",
       "",
       "void KItemList::UnAtlasItem(int nIdx, int nPos /* = -1 */)",
       "{",
       T + "int i = 0;",
       T + "if (m_PlayerIdx <= 0)",
       T*2 + "return;",
       "",
       T + "if (nIdx <= 0)",
       T*2 + "return;",
       "",
       T + "if (nPos < 0)",
       T + "{",
       T*2 + "for (i = 0; i < outinpart_num; i++)",
       T*2 + "{",
       T*3 + "if (m_AtlasItem[i] == nIdx)",
       T*3 + "{",
       T*4 + "break;",
       T*3 + "}",
       T*2 + "}",
       T*2 + "if (i == outinpart_num)",
       T*3 + "return;",
       T + "}",
       T + "else",
       T + "{",
       T*2 + "if (nPos >= outinpart_num)",
       T*3 + "return;",
       T*2 + "if (m_AtlasItem[nPos] != nIdx)",
       T*3 + "return;",
       T*2 + "i = nPos;",
       T + "}",
       T + "m_AtlasItem[i] = 0;",
       "}",
       "",
       "BOOL KItemList::CheckForgeItem(int nIdx, int nPlace /* = -1 */)")),
]

# =========================================================================
# 4. CoreShell.cpp
# =========================================================================
def _khoiCon(ten_ham, nhan_case):
    """Khoi 'kiem con vat pham trong phong' - thut DUNG theo tep goc:
       case=4 tab, {=5, for=6, {=6, if=7, {=7, than=8."""
    return K(T*4 + nhan_case,
             T*5 + "{",
             T*6 + "for(i = 0; i < outinpart_num; i++)",
             T*6 + "{",
             T*7 + "if(Player[CLIENT_PLAYER_INDEX].m_ItemList." + ten_ham + "(i))",
             T*7 + "{",
             T*8 + "bExistId = TRUE;",
             T*7 + "}",
             T*6 + "}")


def _khoiUoc(so, ten_case, pos):
    return K(T*4 + ten_case,
             T*5 + "{",
             T*6 + "if (pObject%d->Region.h == 1)" % so,
             T*7 + "break;",
             T*6 + "P%d.nPlace = %s;" % (so, pos),
             T*6 + "P%d.nX = pObject%d->Region.v;" % (so, so),
             T*5 + "}",
             T*5 + "break;")


CS = [
    ("kiem con vat pham (1)",
     _khoiCon("GetEnchaseItem", "case pos_enchase:	// [UILOREN] khuon case pos_tremble song o tren"),
     _khoiCon("GetAtlasItem", "case pos_atlas:	// " + NHAN)
     + K(T*5 + "}", T*5 + "break;")
     + _khoiCon("GetEnchaseItem", "case pos_enchase:	// [UILOREN] khuon case pos_tremble song o tren")),

    ("kiem con vat pham (2)",
     _khoiCon("GetEnchaseItem", "case pos_enchase:"),
     _khoiCon("GetAtlasItem", "case pos_atlas:	// " + NHAN)
     + K(T*5 + "}", T*5 + "break;")
     + _khoiCon("GetEnchaseItem", "case pos_enchase:")),

    ("anh xa UOC -> pos (1)",
     _khoiUoc(1, "case UOC_ENCHASE_ITEM:	// [UILOREN] khuon UOC_TREMBLE_ITEM", "pos_enchase"),
     _khoiUoc(1, "case UOC_ENCHASE_ITEM:	// [UILOREN] khuon UOC_TREMBLE_ITEM", "pos_enchase")
     + _khoiUoc(1, "case UOC_ATLAS_ITEM:	// " + NHAN, "pos_atlas")),

    ("anh xa UOC -> pos (2)",
     _khoiUoc(2, "case UOC_ENCHASE_ITEM:	// [UILOREN] khuon UOC_TREMBLE_ITEM", "pos_enchase"),
     _khoiUoc(2, "case UOC_ENCHASE_ITEM:	// [UILOREN] khuon UOC_TREMBLE_ITEM", "pos_enchase")
     + _khoiUoc(2, "case UOC_ATLAS_ITEM:	// " + NHAN, "pos_atlas")),
]

# =========================================================================
# 5. ScriptFuns.cpp
# =========================================================================
SF = [
    ("noi gioi han phong",
     K(T + "if (nPos > 9)",
       T*2 + "return 0;"),
     K(T + "// " + NHAN + ": mo them phong 10 (the Do pho Hoang Kim).",
       T + "if (nPos > 10)",
       T*2 + "return 0;")),
    ("case 10",
     K(T + "default:",
       T*2 + "break;",
       T + "}",
       "",
       T + "return 1;",
       "}",
       "",
       "int LuaSetLevel(Lua_State* L)"),
     K(T + "case 10:	// " + NHAN + " - o cua the Do pho",
       T*2 + "if (nSocket < 0)",
       T*2 + "{",
       T*3 + "nSocket = 0;",
       T*2 + "}",
       T*2 + "if (nSocket > 10)",
       T*2 + "{",
       T*3 + "nSocket = 10;",
       T*2 + "}",
       T*2 + "Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetAtlasItem(nSocket));",
       T*2 + "break;",
       T + "default:",
       T*2 + "break;",
       T + "}",
       "",
       T + "return 1;",
       "}",
       "",
       "int LuaSetLevel(Lua_State* L)")),
]

BANG = [
    ("GameDataDef.h", GDD),
    ("KItemList.h", KIL_H),
    ("KItemList.cpp", KIL_C),
    ("CoreShell.cpp", CS),
    ("ScriptFuns.cpp", SF),
]


def apkhoi(dong, cu, moi, ten):
    vt = [i for i in range(len(dong) - len(cu) + 1) if dong[i:i + len(cu)] == cu]
    if len(vt) != 1:
        print("   !!! %-34s khop %d lan (can 1)" % (ten, len(vt)))
        return None
    i = vt[0]
    print("   ok  %-34s dong %d..%d  (+%d dong)"
          % (ten, i + 1, i + len(cu), len(moi) - len(cu)))
    return dong[:i] + moi + dong[i + len(cu):]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== w4_phong_dopho_core - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    ketqua = []
    for tep, mieng in BANG:
        p = os.path.join(D, tep)
        if not os.path.isfile(p):
            print("!!! LOI TO: khong thay %s" % p)
            return 1
        raw = io.open(p, "rb").read().decode("latin-1")
        # [BAY] EOL HON HOP: ScriptFuns.cpp chu yeu LF nhung lan vai CRLF.
        # Neu chon eol kieu `"\r\n" if "\r\n" in raw else "\n"` thi tep bi tach
        # thanh DUNG 5 DONG -> moi moc neo deu truot. Phai chon theo DA SO.
        # Tach bang LF van an toan cho tep CRLF that: cac dong con "\r" o cuoi
        # va duoc noi lai nguyen ven.
        n_crlf = raw.count("\r\n")
        n_lf = raw.count("\n") - n_crlf
        eol = "\r\n" if n_crlf >= n_lf else "\n"
        hi0 = sum(1 for c in raw if ord(c) > 127)
        print("\n--- %s (eol=%s) ---" % (tep, "CRLF" if eol == "\r\n" else "LF"))
        if NHAN in raw:
            print("   DA CO - bo qua tep nay")
            continue
        dong = raw.split(eol)
        for ten, cu, moi in mieng:
            dong = apkhoi(dong, cu, moi, ten)
            if dong is None:
                return 1
        nd = eol.join(dong)
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi o %s" % tep)
            return 1
        if nd.count("{") - raw.count("{") != nd.count("}") - raw.count("}"):
            print("!!! LOI TO: ngoac lech o %s" % tep)
            return 1
        try:
            nd.encode("latin-1")
        except UnicodeEncodeError as e:
            print("!!! LOI TO: ky tu ngoai latin-1 o %s: %s" % (tep, e))
            return 1
        print("   byte cao %d (khong doi) | ngoac can bang" % hi0)
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
    print("\n  => build Core CA HAI cau hinh, thay DONG BO CoreClient.dll + CoreServer.dll")
    return 0


if __name__ == "__main__":
    sys.exit(main())
