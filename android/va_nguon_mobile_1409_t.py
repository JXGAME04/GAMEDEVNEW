# -*- coding: latin-1 -*-
r"""[BANDONHO 14/09] Vao map: bo doc du lieu chan duong cua BAN DO NHO - khong ai doc no.

Do duoc tren ban Release, phien 21:45 (iPhone) va 21:24 (Fold 7), dong [VAOMAP-MO]:
    map 379: tong 85-111 ms = wor 1-2 + DU LIEU BAN DO NHO 84-109
Vong nay nam trong KScenePlaceMapC::Load, quet TOAN BO luoi vung cua map va goi KRegion::LoadLittleMapData
cho tung vung (map Chien truong xung phong: hang tram vung). No chi chay voi map KHONG co MapLTRegionIndex
trong .wor, nen map nay ton con map kia khong.

TAI SAO BO DUOC (da tra tung buoc, khong doan):
  - Du lieu do vao m_cLittleMap (ScenePlaceMapC.h:36). Doc ra chi qua KScenePlaceMapC::GetbtBarrier
    (ScenePlaceMapC.cpp:1352, 1357).
  - GetbtBarrier chi duoc goi tu KJXPathFinder::LoadMap (KJXPathFinder.cpp:185).
  - KJXPathFinder::LoadMap khong co noi goi song: cho duy nhat nhac den la KProtocolProcess.cpp:3284,
    nam TRONG khoi chu thich /* */; ban than g_JXPathFinder bi chu thich o ca bon cho khai bao
    (KCore.cpp:95, KJXPathFinder.cpp:20, KJXPathFinder.h:324, KProtocolProcess.cpp:163); loi goi trong
    KCoreShell::AutoMove (CoreShell.cpp:27302) cung nam trong khoi chu thich.
  - KLittleMap::Draw CO doc du lieu nay, nhung khong ai goi Draw (grep ca cay: 0 noi goi).
  => Du lieu duoc GHI vao roi khong bao gio duoc DOC. Bo phan ghi = khong doi hanh vi gi.

Giu nguyen m_cLittleMap.Init (cap phat mang con tro, re) de Release/Free va moi thu khac khong doi.
Rao #ifdef JX_MOBILE ... #else <nguyen van PC> #endif: ban PC giu y nguyen tung dong.

Chay:  PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1409_t.py
"""
import io
import os

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TAG = "[BANDONHO 14/09]"

CU = ("#ifndef _SERVER\r\n"
      "\t\t\t\tm_cLittleMap.Init(rc.left, rc.top, rc.right, rc.bottom);\r\n"
      "\t\t\t\tint\t\ti, j;\r\n"
      "\t\t\t\tint\t\tx, y;\r\n"
      "\t\t\t\tx = rc.left;\r\n"
      "\t\t\t\ty = rc.top;\r\n"
      "\t\t\t\tfor (i = x; i < rc.right; i++)\r\n"
      "\t\t\t\t{\r\n"
      "\t\t\t\t\tfor (j = y; j < rc.bottom; j++)\r\n"
      "\t\t\t\t\t{\r\n"
      "\t\t\t\t\t\tKRegion::LoadLittleMapData(i, j, SubWorld[0].m_szMapPath, m_cLittleMap.GetBarrierBuf(i, j));\r\n"
      "\t\t\t\t\t\tm_cLittleMap.SetHaveLoad(i, j);\r\n"
      "\t\t\t\t\t}\r\n"
      "\t\t\t\t}\r\n"
      "#endif\r\n")

MOI = ("#ifndef _SERVER\r\n"
       "#ifdef JX_MOBILE\r\n"
       "\t\t\t\t// [BANDONHO 14/09] KHONG doc du lieu chan duong cua ban do nho nua: do tren ban Release,\r\n"
       "\t\t\t\t// vong nay ton 84-109 ms moi lan vao map (dong [VAOMAP-MO] pha 'du lieu map'), ma du lieu\r\n"
       "\t\t\t\t// do KHONG AI DOC - GetbtBarrier chi duoc KJXPathFinder::LoadMap goi, ma ca g_JXPathFinder\r\n"
       "\t\t\t\t// lan moi loi goi den no deu nam trong khoi chu thich (KCore.cpp:95, KJXPathFinder.cpp:20,\r\n"
       "\t\t\t\t// KProtocolProcess.cpp:163/3284, CoreShell.cpp:27302); KLittleMap::Draw co doc nhung khong\r\n"
       "\t\t\t\t// ai goi Draw. Giu Init de cau truc va Release khong doi. Ban PC giu nguyen duong cu.\r\n"
       "\t\t\t\t// CANH BAO CHO NGUOI SAU: KJXPathFinder::Init va LoadMap la ma SONG, chi moi LOI VAO chung\r\n"
       "\t\t\t\t// dang bi chu thich. Ai bat lai g_JXPathFinder (lam tu tim duong) thi PHAI bat lai vong nay\r\n"
       "\t\t\t\t// cho mobile, khong thi tren dien thoai no nhan mang chan duong RONG va tim duong sai im lang,\r\n"
       "\t\t\t\t// trong khi ban PC van dung. Vong nay cung dat co SetHaveLoad; co do chi GetBarrierBuf va\r\n"
       "\t\t\t\t// KLittleMap::Draw doc, ca hai deu khong co noi goi song.\r\n"
       "\t\t\t\tm_cLittleMap.Init(rc.left, rc.top, rc.right, rc.bottom);\r\n"
       "#else\r\n"
       "\t\t\t\tm_cLittleMap.Init(rc.left, rc.top, rc.right, rc.bottom);\r\n"
       "\t\t\t\tint\t\ti, j;\r\n"
       "\t\t\t\tint\t\tx, y;\r\n"
       "\t\t\t\tx = rc.left;\r\n"
       "\t\t\t\ty = rc.top;\r\n"
       "\t\t\t\tfor (i = x; i < rc.right; i++)\r\n"
       "\t\t\t\t{\r\n"
       "\t\t\t\t\tfor (j = y; j < rc.bottom; j++)\r\n"
       "\t\t\t\t\t{\r\n"
       "\t\t\t\t\t\tKRegion::LoadLittleMapData(i, j, SubWorld[0].m_szMapPath, m_cLittleMap.GetBarrierBuf(i, j));\r\n"
       "\t\t\t\t\t\tm_cLittleMap.SetHaveLoad(i, j);\r\n"
       "\t\t\t\t\t}\r\n"
       "\t\t\t\t}\r\n"
       "#endif\r\n"
       "#endif\r\n")

p = os.path.join(GOC, r"Sources\Core\Src\Scene\ScenePlaceMapC.cpp")
s = io.open(p, "r", encoding="latin-1", newline="").read()
cao0 = sum(1 for c in s if ord(c) >= 128)
lf0 = s.count("\n") - s.count("\r\n")
if TAG in s:
    print("bo qua (da co %s)" % TAG)
else:
    if s.count(CU) != 1:
        raise SystemExit("HONG: neo xuat hien %d lan" % s.count(CU))
    s = s.replace(CU, MOI, 1)
    cao1 = sum(1 for c in s if ord(c) >= 128)
    lf1 = s.count("\n") - s.count("\r\n")
    if cao1 != cao0:
        raise SystemExit("HONG: byte cao %d -> %d" % (cao0, cao1))
    if lf1 != lf0:
        raise SystemExit("HONG: %d dong LF don (truoc %d)" % (lf1, lf0))
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("da va: Sources\\Core\\Src\\Scene\\ScenePlaceMapC.cpp")
print("xong.")
