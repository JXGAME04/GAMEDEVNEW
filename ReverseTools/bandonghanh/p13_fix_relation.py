# -*- coding: utf-8 -*-
r"""[BDH 28/08] FIX "dong hanh bi nguoi choi khac danh".

GOC: bang quan he 6 chieu m_RelationTable sinh boi KNpcSet::GenOneRelation
(KNpcSet.cpp) - chua he co nhanh kind_partner, nen pet roi vao cac luat
player/npc thuong => trong PK / che do chien dau co the tro thanh muc tieu.

FIX dung hanh vi ban VLTK/Linux: dong hanh KHONG THE danh/bi danh boi NGUOI
CHOI hay dong hanh khac (ke ca PK); voi NPC thuong theo phe binh thuong
(quai van danh pet, pet van danh quai -> pet chet thi hon me nhu goc).
Chan o TANG BANG de moi duong dung chung mot luat: chon muc tieu client,
dan/AOE (KNpc.cpp:4130, KMissle FindNpc), AI quai, AI pet (sPartnerPickTarget).

KNpcSet.cpp compile CA server lan client => build + swap CoreServer + CoreClient.
"""
import io
import os
import shutil

CR = chr(13)
LF = chr(10)
T = chr(9)
E = CR + LF

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpcSet.cpp"

s = io.open(P, "r", encoding="latin-1", newline="").read()
if "KindKia" in s:
    print("da va roi")
    raise SystemExit

neo = (T + "if (Camp1 == camp_begin || Camp2 == camp_begin)" + E +
       T + T + "return relation_ally;" + E)
assert s.count(neo) == 1, s.count(neo)

them = (neo + T + E +
    T + "// [BDH 28/08] Ban dong hanh - hanh vi ban VLTK/Linux: pet KHONG THE" + E +
    T + "// danh/bi danh boi NGUOI CHOI hay pet khac (ke ca PK - doi thu khong" + E +
    T + "// giet duoc pet); voi NPC thuong theo phe binh thuong (quai va pet van" + E +
    T + "// danh nhau - pet chet thi hon me). Chan o TANG BANG de moi duong" + E +
    T + "// (chon muc tieu, dan/AOE, AI quai, AI pet) cung mot luat." + E +
    T + "if (Kind1 == kind_partner || Kind2 == kind_partner)" + E +
    T + "{" + E +
    T + T + "NPCKIND KindKia = (Kind1 == kind_partner) ? Kind2 : Kind1;" + E +
    T + T + "if (KindKia == kind_partner)" + E +
    T + T + T + "return relation_none;" + E +
    T + T + "if (KindKia == kind_player)" + E +
    T + T + T + "return relation_ally;" + E +
    T + T + "if (Camp1 == Camp2)" + E +
    T + T + T + "return relation_ally;" + E +
    T + T + "return relation_enemy;" + E +
    T + "}" + E)

if not os.path.exists(P + ".truoc_bdh_relation"):
    shutil.copyfile(P, P + ".truoc_bdh_relation")
io.open(P, "w", encoding="latin-1", newline="").write(s.replace(neo, them, 1))
print("DA VA GenOneRelation (nhanh kind_partner)")
