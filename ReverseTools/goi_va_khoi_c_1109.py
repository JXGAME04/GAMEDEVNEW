# -*- coding: utf-8 -*-
r"""goi_va_khoi_c_1109.py - [KHOI 11/09 c] hai loi chu bao sau khi swap ban b:
  (1) "chua co khoi" o map 53
  (2) "dong vat bi mat hinh" (chi con the ten lo lung)

DO DUOC (khong doan):

(1) KHOI map 53 = `\游戏资源\地图动画\熔岩\10160.spr` o o (203,202), vung (101,100).
    Doc thang ban ghi KBuildinObj: Props=0x00000006 (sap xep POINT), nAniSpeed=1,
    nImgNumFrames=6, oPos1=(52051,103439,249). Dau tep vung ghi tree 0 / line 10 /
    point 5 / above 1, va vat nay la ban ghi thu 15 tren 16 ==> no la vat "TREN DAU"
    (above-head), KHONG di qua cay ve nhu vat thuong.
    Nhanh ve vat tren dau CO HOAT ANH (KScenePlaceRegionC::PaintAboveHeadObj, nhanh else)
    dung du anh roi goi BuildinObjNextFrame nhung **KHONG HE GOI DrawPrimitives**.
    Vi vay dong [VATDONG] van dem 1.438 lan/10 s (khung hinh van chay) ma man hinh
    khong co gi. Doi chieu duong DANG CHAY cho vat thuong:
    KIpotBuildinObj::PaintABuildinObject:199-210 co du ba buoc
    (kiem oPos1.y >= pRepresentArea->top, dat REF_SPOT, roi DrawPrimitives RU_T_IMAGE).
    Sua: bo sung dung ba buoc do. (Khoi o Phuong Tuong (203,191) hien duoc vi no la vat
    kieu LINE di qua KIpotBranch::PaintABranchObject, da sua o ban [VATDONG] hom truoc.)

(2) DONG VAT MAT HINH: chuoi nap anh NPC = NpcS.txt cot NpcResType (critterNNN)
    -> \settings\npcres\npc_res_kind_file_name.txt -> ResFilePath -> .spr.
    Do tren pak client cua du an:
      mau 434 chuon chuon critter019, 436 buom 1 critter021, 437 buom 2 critter022,
      439 chim se critter024, 442 ech critter027: CO hang, 7/7 anh co  -> ve duoc
      mau 418 ga trong critter003, 419 ga mai critter004, 430 cho vang critter015:
      **THIEU HANG** trong npc_res_kind_file_name.txt (bang chi co critter008 va 016-045)
      -> KNpcResNode::Init tra FALSE -> KNpcRes::Init tra FALSE (m_pcResNode = NULL)
      -> khong co bo anh nao, chi con the ten lo lung dung nhu chu chup.
    Anh goc: critter003 2/3 tep co, critter004 2/3 co, critter015 0/3 co.
    Vi day la DU LIEU GAME (phai sua trong pak) nen KHONG tu sua. Phan ma chi bo han
    NPC khong co bo anh de khong con the ten lo lung.

Do: them vao dong [TRANGTRI] so NPC bi bo vi thieu anh, va so lan VE THAT cua vat
tren dau co hoat anh (dong [KHOI]).
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[KHOI 11/09 c]"
D = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n"), s.count("\r\n")


def ghi(p, s, h0, lf0, crlf0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten)
        sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong))
        sys.exit(1)
    return s.replace(old, new)


# ---------------------------------------------------- (1) KScenePlaceRegionC.cpp: them lenh ve con thieu
P = D + "Scene/KScenePlaceRegionC.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, "//##ModelId=3DE29F360221" + NL +
            "void KScenePlaceRegionC::PaintAboveHeadObj(KBuildinObj* pObj, RECT* pRepresentArea)" + NL,
            "// " + TAG + " dem so lan VE THAT vat tren dau co hoat anh (khoi ong khoi, hoi nuoc...)" + NL +
            "unsigned g_uKhoiVe = 0, g_uKhoiBoQua = 0;" + NL +
            "//##ModelId=3DE29F360221" + NL +
            "void KScenePlaceRegionC::PaintAboveHeadObj(KBuildinObj* pObj, RECT* pRepresentArea)" + NL, "K dem")
    s = rep(s, NL.join([
        T + "else",
        T + "{",
        T*2 + "Img.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;",
        T*2 + "Img.oPosition.nX = pObj->oPos1.x;",
        T*2 + "Img.oPosition.nY = pObj->oPos1.y;",
        T*2 + "Img.oPosition.nZ = pObj->oPos1.z;",
        "",
        T*2 + "BuildinObjNextFrame(pObj);",
        T + "}", ""]), NL.join([
        T + "else",
        T + "{",
        T*2 + "// " + TAG + " nhanh nay dat du anh roi goi BuildinObjNextFrame nhung THIEU HAN",
        T*2 + "// lenh ve, nen vat tren dau co hoat anh (khoi ong khoi map 53 o (203,202)) chay",
        T*2 + "// khung hinh ma khong bao gio hien. Lam dung nhu duong vat thuong dang chay:",
        T*2 + "// KIpotBuildinObj::PaintABuildinObject:199-210.",
        T*2 + "if (pObj->oPos1.y >= pRepresentArea->top)",
        T*2 + "{",
        T*3 + "Img.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;",
        T*3 + "Img.oPosition.nX = pObj->oPos1.x;",
        T*3 + "Img.oPosition.nY = pObj->oPos1.y;",
        T*3 + "Img.oPosition.nZ = pObj->oPos1.z;",
        T*3 + "g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE, false);",
        T*3 + "g_uKhoiVe++;",
        T*2 + "}",
        T*2 + "else",
        T*3 + "g_uKhoiBoQua++;",
        "",
        T*2 + "BuildinObjNextFrame(pObj);",
        T + "}", ""]), "K ve")
    ghi(P, s, h0, lf0, crlf0, "KScenePlaceRegionC.cpp")
else:
    print("KScenePlaceRegionC.cpp da co")

# ---------------------------------------------------- (2a) KNpcRes.h: cho biet co bo anh hay khong
P = D + "KNpcRes.h"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, T + "int" + T*4 + "GetResDoing(){return m_nDoing;};" + NL,
            T + "int" + T*4 + "GetResDoing(){return m_nDoing;};" + NL +
            T + "// " + TAG + " FALSE = mau NPC khong co hang trong npc_res_kind_file_name.txt" + NL +
            T + "//" + T + "nen khong co bo anh nao de ve (chi con the ten lo lung)." + NL +
            T + "BOOL" + T*3 + "CoAnh(){return m_pcResNode != NULL;};" + NL, "NR coanh")
    ghi(P, s, h0, lf0, crlf0, "KNpcRes.h")
else:
    print("KNpcRes.h da co")

# ---------------------------------------------------- (2b) KRegion.cpp: bo NPC trang tri khong co anh
P = D + "KRegion.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, "unsigned g_uTTNpcThem = 0, g_uTTNpcHong = 0, g_uTTNpcXoa = 0, g_uTTObjThem = 0;" + NL,
            "unsigned g_uTTNpcThem = 0, g_uTTNpcHong = 0, g_uTTNpcXoa = 0, g_uTTObjThem = 0;" + NL +
            "unsigned g_uTTNpcThieuAnh = 0;" + T + "// " + TAG + " mau khong co bo anh -> bo han, khong de the ten lo lung" + NL, "R dem thieu anh")
    s = rep(s, NL.join([
        T*3 + "if (nIdx > 0)",
        T*3 + "{",
        T*4 + "Npc[nIdx].m_Kind = sNpcCell.shKind;", ""]), NL.join([
        T*3 + "if (nIdx > 0 && !Npc[nIdx].m_DataRes.CoAnh())",
        T*3 + "{",
        T*4 + "// " + TAG + " do duoc: critter003 (ga trong), critter004 (ga mai), critter015",
        T*4 + "// (cho vang) KHONG co hang trong \\settings\\npcres\\npc_res_kind_file_name.txt",
        T*4 + "// (bang chi co critter008 va 016-045) nen KNpcResNode::Init tra FALSE va NPC",
        T*4 + "// khong co bo anh nao. De lai thi chi thay THE TEN lo lung giua khong khi.",
        T*4 + "g_uTTNpcThieuAnh++;",
        T*4 + "if (Npc[nIdx].m_RegionIndex >= 0)",
        T*5 + "SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);",
        T*4 + "Npc[nIdx].m_RegionIndex = -1;",
        T*4 + "NpcSet.Remove(nIdx);",
        T*4 + "nIdx = 0;",
        T*3 + "}",
        T*3 + "if (nIdx > 0)",
        T*3 + "{",
        T*4 + "Npc[nIdx].m_Kind = sNpcCell.shKind;", ""]), "R bo khong anh")
    ghi(P, s, h0, lf0, crlf0, "KRegion.cpp")
else:
    print("KRegion.cpp da co")

# ---------------------------------------------------- (3) KSubWorldSet.cpp: in them hai so dem
P = D + "KSubWorldSet.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, T*3 + "extern unsigned g_uTTNpcThem, g_uTTNpcHong, g_uTTNpcXoa, g_uTTObjThem;" + NL,
            T*3 + "extern unsigned g_uTTNpcThem, g_uTTNpcHong, g_uTTNpcXoa, g_uTTObjThem;" + NL +
            T*3 + "extern unsigned g_uTTNpcThieuAnh;" + T + "// " + TAG + NL +
            T*3 + "extern unsigned g_uKhoiVe, g_uKhoiBoQua;" + T + "// " + TAG + NL, "SW extern")
    s = rep(s, T*5 + "fprintf(pT, \"[TRANGTRI] 10s: npc them %u hong %u xoa %u | obj them %u | map %s | ten: %s %s %s\\n\"," + NL +
            T*6 + "g_uTTNpcThem, g_uTTNpcHong, g_uTTNpcXoa, g_uTTObjThem, g_szTTMap, g_szTTTen[0], g_szTTTen[1], g_szTTTen[2]);" + NL,
            T*5 + "fprintf(pT, \"[TRANGTRI] 10s: npc them %u hong %u thieu_anh %u xoa %u | obj them %u | khoi ve %u bo qua %u | map %s | ten: %s %s %s\\n\"," + NL +
            T*6 + "g_uTTNpcThem, g_uTTNpcHong, g_uTTNpcThieuAnh, g_uTTNpcXoa, g_uTTObjThem, g_uKhoiVe, g_uKhoiBoQua," + NL +
            T*6 + "g_szTTMap, g_szTTTen[0], g_szTTTen[1], g_szTTTen[2]);" + NL, "SW dong in")
    s = rep(s, T*3 + "g_uTTNpcThem = g_uTTNpcHong = g_uTTNpcXoa = g_uTTObjThem = 0;" + NL,
            T*3 + "g_uTTNpcThem = g_uTTNpcHong = g_uTTNpcXoa = g_uTTObjThem = 0;" + NL +
            T*3 + "g_uTTNpcThieuAnh = 0; g_uKhoiVe = 0; g_uKhoiBoQua = 0;" + T + "// " + TAG + NL, "SW dat lai")
    ghi(P, s, h0, lf0, crlf0, "KSubWorldSet.cpp")
else:
    print("KSubWorldSet.cpp da co")

print("XONG " + TAG)
