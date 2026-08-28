# -*- coding: utf-8 -*-
"""w5_the_dopho_client.py - DUNG LOP CUA SO THE DO PHO (KUiAtlas) + noi Lua/ini.

Tiep sau w4 (da lam phong pos_atlas / UOC_ATLAS_ITEM phia loi).

BO CUC LAY NGUYEN VAN BAN GOC: `Dopho.ini` rut tu pak VLTK
(update03__(dau tao_hoang kim do pho).ini), CHEP 100% khong sua mot byte.
Cac muc o vat pham trong do:
    AtlasBox     o Do pho Hoang Kim
    CryoliteBox  o Huyen Tinh khoang thach
    Box1..Box6   6 o nguyen lieu
    ItemBox      o tu chon ("tang ti le")
=> 8 O CHINH + 1 o tu chon. Con so 8 KHOP `s_anSoO[6] = 8` cua may chu.
Nut: `AtlasBtn` (duc) va `PreviewBtn` (xem truoc). Hieu ung: `ConsumeEffect`.
Vi bo cuc nay KHAC cac the khac (khong co BigBox/SmallBox/Consume1..8,
khong co CancleBtn, chi MOT anh hieu ung) nen lop KUiAtlas duoc viet theo dung
bo cuc goc, KHONG sao chep mu KUiEnchase.

CAC MIENG:
  CoreShell.h    + GDI_ATLAS_ITEM (them CUOI enum - khong xe dich gia tri cu)
  CoreShell.cpp  + khoi `case GDI_ATLAS_ITEM` (nhan ban GDI_ENCHASE_ITEM)
  UiCompoundItem.h   + class KUiAtlas, + KUiAtlas m_AtlasPad;
  UiCompoundItem.cpp + SCHEME_INI_ATLAS, + CtrlAtlasItemMap, + than lop,
                     + bo chu thich m_AtlasPad (Initialize/AddPage/Show/Hide),
                     + thu hoi pos_atlas khi dong cua so
  compound_ui.lua    + LR_UI_Atlas()  -> LR_UI_Lam(10, 9, 8, 6)
                       (phong 10, 9 o, o thu 8 tro di la PHU, nType 6 = atlas)
  Khamnammain.ini    - bo `Disable=1` o [AtlasBtn]
  Dopho.ini          dat vao bin\\client\\Ui\\Ui3\\khamnam\\

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 27/08] THE DO PHO"
HAU_TO = ".truoc_dopho2"

SRC = r"D:\GAMEDEVNEW\Sources"
CORE = os.path.join(SRC, "Core", "Src")
UI = os.path.join(SRC, "S3Client", "Ui", "UiCase")
CLIENT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
SERVER = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
RA = r"D:\GAMEDEVNEW\ReverseTools\loren\ui_loren\ra_khamnam2"


def K(*d):
    return list(d)


# ============================== CoreShell.h ==============================
CSH = [
    ("GDI_ATLAS_ITEM",
     K(T + "GDI_ITEM_IN_PARTNER_BAG,	// [BDH-G4] tui ban dong hanh (them CUOI)"),
     K(T + "GDI_ITEM_IN_PARTNER_BAG,	// [BDH-G4] tui ban dong hanh (them CUOI)",
       T + "GDI_ATLAS_ITEM,		// " + NHAN + " - o cua the Do pho (them CUOI)")),
]

# ============================== CoreShell.cpp ==============================
_GDI = []
for ten, ham in (("GDI_ATLAS_ITEM", "GetAtlasItem"),):
    _GDI = K(
        T + "case %s:		// %s (khuon GDI_ENCHASE_ITEM)" % (ten, NHAN),
        T*2 + "nRet = 0;",
        T*2 + "if (uParam)",
        T*2 + "{",
        T*3 + "if (nParam == 1)",
        T*4 + "break;",
        "",
        T*3 + "int nCount = 0;",
        T*3 + "KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;",
        "",
        T*3 + "for (int i = 0; i < outinpart_num; i++)",
        T*3 + "{",
        T*4 + "pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.%s(i);" % ham,
        T*4 + "if (pInfo->Obj.uId)",
        T*4 + "{",
        T*5 + "pInfo->Obj.uGenre = CGOG_ITEM;",
        "",
        T*5 + "pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();",
        T*5 + "pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();",
        T*5 + "pInfo->Region.h = 0;",
        T*5 + "pInfo->Region.v = i;",
        T*4 + "}",
        T*4 + "else",
        T*4 + "{",
        T*5 + "pInfo->Obj.uGenre = CGOG_NOTHING;",
        T*4 + "}",
        T*4 + "nCount++;",
        T*4 + "pInfo++;",
        T*3 + "}",
        T*3 + "nRet = nCount;",
        T*2 + "}",
        T*2 + "break;",
    )

CSC = [
    ("case GDI_ATLAS_ITEM",
     K(T + "case GDI_CHAT_ITEM_IMAGE:"),
     _GDI + K(T + "case GDI_CHAT_ITEM_IMAGE:")),
]

# ============================== UiCompoundItem.h ==============================
LOP = K(
    "// " + NHAN + " - trang \"Do pho Hoang Kim\".",
    "// Bo cuc theo dung ban goc (Dopho.ini rut tu pak VLTK): 8 o CHINH",
    "// (AtlasBox + CryoliteBox + Box1..Box6) va 1 o tu chon (ItemBox).",
    "class KUiAtlas : public KWndPage",
    "{",
    "public:",
    T + "KUiAtlas();",
    T + "void	Initialize();",
    T + "void	LoadScheme(const char* pScheme);",
    "",
    T + "void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);",
    T + "void	UpdateAllItem();",
    T + "void	UpdateData();",
    T + "void	Breathe();",
    "private:",
    T + "int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);",
    T + "void	PaintWindow();",
    T + "void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);",
    T + "void	ProcessAtlas();",
    T + "int		PlayEffect();",
    "private:",
    T + "enum THIS_INTERFACE_STATUS",
    T + "{",
    T*2 + "STATUS_WAITING_MATERIALS,",
    T*2 + "STATUS_BEGIN_ATLAS,",
    T*2 + "STATUS_ATLASING,",
    T*2 + "STATUS_CHANGING_ITEM,",
    T + "};",
    "",
    T + "int				m_nStatus;",
    "",
    T + "KWndObjectBox	m_ItemBox[_ITEM_OUTIN_COUNT];",
    T + "KWndMessageListBox	m_Guide;",
    T + "KWndScrollBar	m_ListScroll;",
    T + "KWndButton		m_Atlas;",
    T + "KWndImage		m_AtlasEffect;",
    "};",
    "",
)

UIH = [
    # Neo bang dong `class KUiCompoundItem...` chu KHONG bang dong phan cach
    # dau sao: so luong dau `*` rat de go lech mot ky tu -> khop 0 lan.
    ("class KUiAtlas",
     K("class KUiCompoundItem : public KWndPageSet"),
     LOP + K("class KUiCompoundItem : public KWndPageSet")),
    ("m_AtlasPad",
     K(T + "KUiEnchase		m_EnchasePad;"),
     K(T + "KUiEnchase		m_EnchasePad;",
       T + "KUiAtlas		m_AtlasPad;		// " + NHAN)),
]

# ============================== UiCompoundItem.cpp ==============================
THAN = K(
    "//=========================================================================",
    "// " + NHAN + " - than lop KUiAtlas.",
    "// Bo cuc doc tu Dopho.ini (ban goc VLTK). Anh xa o:",
    "//   0 AtlasBox | 1 CryoliteBox | 2..7 Box1..Box6 | 8 ItemBox (o tu chon)",
    "//=========================================================================",
    "KUiAtlas::KUiAtlas()",
    "{",
    T + "m_nStatus = STATUS_WAITING_MATERIALS;",
    "}",
    "",
    "void KUiAtlas::Initialize()",
    "{",
    T + "for (int i = 0; i < _ITEM_OUTIN_COUNT; i ++)",
    T + "{",
    T*2 + "m_ItemBox[i].SetObjectGenre(CGOG_ITEM);",
    T*2 + "AddChild(&m_ItemBox[i]);",
    T*2 + "m_ItemBox[i].SetContainerId((int)UOC_ATLAS_ITEM);",
    T + "}",
    T + "AddChild(&m_Atlas);",
    T + "AddChild(&m_Guide);",
    T + "AddChild(&m_ListScroll);",
    T + "AddChild(&m_AtlasEffect);",
    T + "m_Guide.SetScrollbar(&m_ListScroll);",
    "",
    T + "char Scheme[256];",
    T + "g_UiBase.GetCurSchemePath(Scheme, 256);",
    T + "LoadScheme(Scheme);",
    "",
    T + "char		Buff[600];",
    T + "KIniFile	Ini;",
    T + "sprintf(Buff, \"%s\\\\%s\", Scheme, SCHEME_INI_SHEET);",
    T + "if (Ini.Load(Buff))",
    T + "{",
    T*2 + "int nLen = sizeof(Buff);",
    T*2 + "ZeroMemory(Buff, nLen);",
    T*2 + "Ini.GetString(\"RuleInfo\", \"Atlas\", \"\", Buff, nLen);",
    T*2 + "nLen = TEncodeText(Buff, strlen(Buff));",
    T*2 + "m_Guide.AddOneMessage(Buff, nLen);",
    T*2 + "ZeroMemory(Buff, nLen);",
    T + "}",
    T + "Wnd_AddWindow(this);",
    "}",
    "",
    "void KUiAtlas::LoadScheme( const char* pScheme )",
    "{",
    T + "char		Buff[128];",
    T + "KIniFile	Ini;",
    T + "sprintf(Buff, \"%s\\\\%s\", pScheme, SCHEME_INI_ATLAS);",
    T + "if (Ini.Load(Buff))",
    T + "{",
    T*2 + "KWndImage::Init(&Ini, \"Main\");",
    T*2 + "for (int i = 0; i < _ITEM_OUTIN_COUNT; i ++)",
    T*2 + "{",
    T*3 + "if (CtrlAtlasItemMap[i].pIniSection)",
    T*4 + "m_ItemBox[i].Init(&Ini, CtrlAtlasItemMap[i].pIniSection);",
    T*2 + "}",
    T*2 + "m_Atlas.Init(&Ini, \"AtlasBtn\");",
    T*2 + "m_Guide.Init(&Ini, \"GuideList\");",
    T*2 + "m_ListScroll.Init(&Ini, \"GuideList_Scroll\");",
    T*2 + "m_AtlasEffect.Init(&Ini, \"ConsumeEffect\");",
    T*2 + "m_AtlasEffect.Hide();",
    T + "}",
    "}",
    "",
    "void KUiAtlas::Breathe()",
    "{",
    T + "if (m_nStatus == STATUS_BEGIN_ATLAS)",
    T + "{",
    T*2 + "m_AtlasEffect.Show();",
    T*2 + "m_AtlasEffect.SetFrame(0);",
    T*2 + "m_nStatus = STATUS_ATLASING;",
    T + "}",
    T + "else if (m_nStatus == STATUS_ATLASING)",
    T + "{",
    T*2 + "if (!PlayEffect())",
    T*2 + "{",
    T*3 + "m_nStatus = STATUS_CHANGING_ITEM;",
    T*3 + "m_AtlasEffect.Hide();",
    T*2 + "}",
    T + "}",
    T + "else if (m_nStatus == STATUS_CHANGING_ITEM)",
    T + "{",
    T*2 + "ProcessAtlas();",
    T*2 + "m_nStatus = STATUS_WAITING_MATERIALS;",
    T + "}",
    "}",
    "",
    "int KUiAtlas::PlayEffect()",
    "{",
    T + "if (m_AtlasEffect.GetCurrentFrame() >= MAX_SPR_FRAME)",
    T + "{",
    T*2 + "m_AtlasEffect.SetFrame(0);",
    T*2 + "return 0;",
    T + "}",
    T + "m_AtlasEffect.NextFrame();",
    T + "return 1;",
    "}",
    "",
    "int KUiAtlas::WndProc( unsigned int uMsg, unsigned int uParam, int nParam )",
    "{",
    T + "switch(uMsg)",
    T + "{",
    T + "case WND_N_SCORLLBAR_POS_CHANGED:",
    T*2 + "if (uParam == (unsigned int)&m_ListScroll)",
    T*2 + "{",
    T*3 + "m_Guide.SetFirstShowLine(nParam);",
    T*2 + "}",
    T*2 + "break;",
    T + "case WND_N_BUTTON_CLICK:",
    T*2 + "if (uParam == (unsigned int)&m_Atlas)",
    T*2 + "{",
    T*3 + "// Nhip \"nap\": bam nut -> hien hieu ung -> het 25 khung moi GUI lenh,",
    T*3 + "// dung may trang thai goc (xem KUiCompoundOne::Breathe).",
    T*3 + "if (m_nStatus == STATUS_WAITING_MATERIALS)",
    T*3 + "{",
    T*4 + "m_nStatus = STATUS_BEGIN_ATLAS;",
    T*4 + "return 1;",
    T*3 + "}",
    T*2 + "}",
    T*2 + "break;",
    T + "case WND_N_ITEM_PICKDROP:",
    T*2 + "if (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM))",
    T*3 + "OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);",
    T*2 + "break;",
    T + "default:",
    T*2 + "return KWndImage::WndProc(uMsg, uParam, nParam);",
    T + "}",
    T + "return 1;",
    "}",
    "",
    "void KUiAtlas::ProcessAtlas()",
    "{",
    T + "// May chu kiem toan bo luat (KFoundryResDemand + atlas.lua), client chi gui.",
    T + "if (g_pCoreShell)",
    T*2 + "g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 7, (unsigned int)\"LR_UI_Atlas\");",
    "}",
    "",
    "void KUiAtlas::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)",
    "{",
    T + "KUiObjAtContRegion	Drop, Pick;",
    T + "KUiDraggedObject	Obj;",
    T + "KWndWindow*			pWnd = NULL;",
    "",
    T + "if (pPickPos)",
    T + "{",
    T*2 + "((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);",
    T*2 + "Pick.Obj.uGenre = Obj.uGenre;",
    T*2 + "Pick.Obj.uId = Obj.uId;",
    T*2 + "Pick.Region.Width = Obj.DataW;",
    T*2 + "Pick.Region.Height = Obj.DataH;",
    T*2 + "Pick.Region.h = 0;",
    T*2 + "Pick.eContainer = UOC_ATLAS_ITEM;",
    T*2 + "pWnd = pPickPos->pWnd;",
    T + "}",
    T + "else if (pDropPos)",
    T + "{",
    T*2 + "pWnd = pDropPos->pWnd;",
    T + "}",
    T + "else",
    T*2 + "return;",
    "",
    T + "if (pDropPos)",
    T + "{",
    T*2 + "Wnd_GetDragObj(&Obj);",
    T*2 + "Drop.Obj.uGenre = Obj.uGenre;",
    T*2 + "Drop.Obj.uId = Obj.uId;",
    T*2 + "Drop.Region.Width = Obj.DataW;",
    T*2 + "Drop.Region.Height = Obj.DataH;",
    T*2 + "Drop.Region.h = 0;",
    T*2 + "Drop.eContainer = UOC_ATLAS_ITEM;",
    T + "}",
    "",
    T + "for (int i = 0; i < _ITEM_OUTIN_COUNT; i++)",
    T + "{",
    T*2 + "if (pWnd == (KWndWindow*)&m_ItemBox[i])",
    T*2 + "{",
    T*3 + "Drop.Region.v = Pick.Region.v = CtrlAtlasItemMap[i].nPosition;",
    T*3 + "break;",
    T*2 + "}",
    T + "}",
    T + "g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,",
    T*2 + "pPickPos ? (unsigned int)&Pick : 0,",
    T*2 + "pDropPos ? (int)&Drop : 0);",
    "}",
    "",
    "void KUiAtlas::UpdateData()",
    "{",
    T + "UpdateAllItem();",
    "}",
    "",
    "void KUiAtlas::UpdateAllItem()",
    "{",
    T + "KUiObjAtRegion	Item[_ITEM_OUTIN_COUNT];",
    T + "int nCount = g_pCoreShell->GetGameData(GDI_ATLAS_ITEM, (unsigned int)&Item, 0);",
    T + "int	i;",
    T + "for (i = 0; i < _ITEM_OUTIN_COUNT; i++)",
    T*2 + "m_ItemBox[i].Celar();",
    T + "for (i = 0; i < nCount; i++)",
    T + "{",
    T*2 + "if (Item[i].Obj.uGenre != CGOG_NOTHING)",
    T*3 + "UpdateItem(&Item[i], true);",
    T + "}",
    "}",
    "",
    "void KUiAtlas::UpdateItem(KUiObjAtRegion* pItem, int bAdd)",
    "{",
    T + "if (pItem)",
    T + "{",
    T*2 + "for (int i = 0; i < _ITEM_OUTIN_COUNT; i++)",
    T*2 + "{",
    T*3 + "if (CtrlAtlasItemMap[i].nPosition == pItem->Region.v)",
    T*3 + "{",
    T*4 + "if (bAdd)",
    T*5 + "m_ItemBox[i].HoldObject(pItem->Obj.uGenre, pItem->Obj.uId,",
    T*6 + "pItem->Region.Width, pItem->Region.Height);",
    T*4 + "else",
    T*5 + "m_ItemBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);",
    T*4 + "break;",
    T*3 + "}",
    T*2 + "}",
    T + "}",
    "}",
    "",
    "void KUiAtlas::PaintWindow()",
    "{",
    T + "KWndPage::PaintWindow();",
    "}",
    "",
)

# CHU Y: cac bang o la MOT khai bao chung nhieu bien (ngan bang `,`, ket thuc
# bang `;` o bang cuoi la CtrlEnchaseItemMap). Nen KHONG chen bang moi vao giua
# nhu mot khai bao doc lap - lam vay la cat dut chuoi va mat kieu
# (error C4430 "missing type specifier"). Cach dung: doi `};` cuoi thanh `},`
# roi noi them bang moi va ket thuc bang `;`.
BANGO = K(
    "},",
    "",
    "// " + NHAN + " - anh xa o cua trang Do pho, doc tu Dopho.ini (ban goc VLTK).",
    "// 8 o CHINH (0..7) + 1 o TU CHON (8); hai o cuoi cua _ITEM_OUTIN_COUNT",
    "// khong dung nen de NULL - LoadScheme bo qua.",
    "CtrlAtlasItemMap[_ITEM_OUTIN_COUNT] =",
    "{",
    T + "{ 0,	\"AtlasBox\"		},",
    T + "{ 1,	\"CryoliteBox\"	},",
    T + "{ 2,	\"Box1\"			},",
    T + "{ 3,	\"Box2\"			},",
    T + "{ 4,	\"Box3\"			},",
    T + "{ 5,	\"Box4\"			},",
    T + "{ 6,	\"Box5\"			},",
    T + "{ 7,	\"Box6\"			},",
    T + "{ 8,	\"ItemBox\"		},",
    T + "{ 9,	NULL			},",
    T + "{ 10,	NULL			},",
    "};",
    "",
)

UIC = [
    ("SCHEME_INI_ATLAS",
     K("#define 	SCHEME_INI_OUTIN			\"khamnam/Lay.ini\""),
     K("#define 	SCHEME_INI_OUTIN			\"khamnam/Lay.ini\"",
       "#define 	SCHEME_INI_ATLAS			\"khamnam/Dopho.ini\"	// " + NHAN)),
    ("CtrlAtlasItemMap",
     K(T + "{ UIEP_CONS8,\t\t\"Consume8\"\t},\t", "};"),
     K(T + "{ UIEP_CONS8,\t\t\"Consume8\"\t},\t") + BANGO),
    ("than lop KUiAtlas",
     K("KUiEnchase::KUiEnchase()"),
     THAN + K("KUiEnchase::KUiEnchase()")),
    ("noi pad vao cua so",
     K(T + "//m_AtlasPad.Initialize();",
       T + "//AddPage(&m_AtlasPad,&m_AtlasPadBtn);"),
     K(T + "m_AtlasPad.Initialize();		// " + NHAN,
       T + "AddPage(&m_AtlasPad,&m_AtlasPadBtn);")),
    ("thu hoi pos_atlas",
     K(T*2 + "g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_enchase, 0);"),
     K(T*2 + "g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_enchase, 0);",
       T*2 + "g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_atlas, 0);	// " + NHAN)),
]


def apkhoi(dong, cu, moi, ten):
    vt = [i for i in range(len(dong) - len(cu) + 1) if dong[i:i + len(cu)] == cu]
    if len(vt) != 1:
        print("   !!! %-30s khop %d lan (can 1)" % (ten, len(vt)))
        return None
    i = vt[0]
    print("   ok  %-30s dong %d (+%d)" % (ten, i + 1, len(moi) - len(cu)))
    return dong[:i] + moi + dong[i + len(cu):]


def bochuthich(dong):
    """Bo `//` o moi dong `//\\t\\tm_pSelf->m_AtlasPad...`"""
    n = 0
    for i, l in enumerate(dong):
        if l.startswith("//") and "m_pSelf->m_AtlasPad" in l:
            dong[i] = l[2:]
            n += 1
    print("   ok  %-30s bo chu thich %d dong Show/Hide" % ("m_AtlasPad Show/Hide", n))
    return dong, n


BANG = [
    (os.path.join(CORE, "CoreShell.h"), CSH),
    (os.path.join(CORE, "CoreShell.cpp"), CSC),
    (os.path.join(UI, "UiCompoundItem.h"), UIH),
    (os.path.join(UI, "UiCompoundItem.cpp"), UIC),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== w5_the_dopho_client - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    ketqua = []

    for p, mieng in BANG:
        if not os.path.isfile(p):
            print("!!! LOI TO: khong thay %s" % p)
            return 1
        raw = io.open(p, "rb").read().decode("latin-1")
        n_crlf = raw.count("\r\n")
        n_lf = raw.count("\n") - n_crlf
        eol = "\r\n" if n_crlf >= n_lf else "\n"
        hi0 = sum(1 for c in raw if ord(c) > 127)
        print("\n--- %s (eol=%s) ---"
              % (os.path.basename(p), "CRLF" if eol == "\r\n" else "LF"))
        if NHAN in raw:
            print("   DA CO - bo qua")
            continue
        dong = raw.split(eol)
        for ten, cu, moi in mieng:
            dong = apkhoi(dong, cu, moi, ten)
            if dong is None:
                return 1
        if p.endswith("UiCompoundItem.cpp"):
            dong, n = bochuthich(dong)
            if n != 12:
                print("   !!! mong 12 dong Show/Hide, thay %d" % n)
                return 1
        nd = eol.join(dong)
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi")
            return 1
        if nd.count("{") - raw.count("{") != nd.count("}") - raw.count("}"):
            print("!!! LOI TO: ngoac lech")
            return 1
        try:
            nd.encode("latin-1")
        except UnicodeEncodeError as e:
            print("!!! LOI TO: ngoai latin-1: %s" % e)
            return 1
        print("   byte cao %d (khong doi) | ngoac can bang" % hi0)
        ketqua.append((p, nd))

    # ---------- ini + lua (cay van hanh) ----------
    ini = os.path.join(CLIENT, "Ui", "Ui3", "khamnam", "Khamnammain.ini")
    raw = io.open(ini, "rb").read().decode("latin-1")
    eol2 = "\r\n" if "\r\n" in raw else "\n"
    print("\n--- Khamnammain.ini ---")
    # kiem NHAN truoc: chuoi thay the VAN chua "Disable=1" nen neu chi do
    # "Disable=1" thi lan chay sau se thay chong lan nua.
    if NHAN in raw:
        print("   DA MO san - bo qua")
    elif "Disable=1" in raw:
        nd = raw.replace("Disable=1", "; Disable=1   ; " + NHAN + ": mo lai nut", 1)
        print("   ok  bo Disable=1 o [AtlasBtn]")
        ketqua.append((ini, nd))
    else:
        print("   DA MO san - bo qua")

    lua = os.path.join(SERVER, "script", "item", "compound", "compound_ui.lua")
    raw = io.open(lua, "rb").read().decode("latin-1")
    eol3 = "\r\n" if "\r\n" in raw else "\n"
    print("--- compound_ui.lua ---")
    if "LR_UI_Atlas" in raw:
        print("   DA CO - bo qua")
    else:
        neo = "function LR_UI_Enchase()"
        if raw.count(neo) != 1:
            print("   !!! moc neo LR_UI_Enchase khop %d lan" % raw.count(neo))
            return 1
        them = eol3.join([
            "-- " + NHAN + ": phong 10, 9 o (0..7 CHINH = do pho + huyen tinh + 6",
            "-- nguyen lieu; o 8 la TU CHON), nType 6 = atlas.lua",
            "function LR_UI_Atlas()",
            T + "LR_UI_Lam(10, 9, 8, 6);",
            "end",
            "",
        ]) + eol3
        ketqua.append((lua, raw.replace(neo, them + neo, 1)))
        print("   ok  them LR_UI_Atlas (phong 10, 9 o, phu tu o 8, nType 6)")

    # ---------- Dopho.ini ----------
    src = os.path.join(RA, "Dopho.ini")
    dst = os.path.join(CLIENT, "Ui", "Ui3", "khamnam", "Dopho.ini")
    print("--- Dopho.ini ---")
    if os.path.isfile(dst):
        print("   DA CO - bo qua")
    elif not os.path.isfile(src):
        print("   !!! khong thay ban goc %s" % src)
        return 1
    else:
        ketqua.append((dst, io.open(src, "rb").read().decode("latin-1")))
        print("   ok  se dat Dopho.ini (%d byte)" % os.path.getsize(src))

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    for p, nd in ketqua:
        if os.path.isfile(p):
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
