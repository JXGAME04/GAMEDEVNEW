# -*- coding: utf-8 -*-
"""t78_va_vs_decref_dorevive.py - dot VA VE SINH sau va goc "o chet" (t76, commit aaf5bb24).

BOI CANH (doc BANGIAO_ODCHET_NPCREF_3108.md muc 7):
    Xac cho hoi sinh (m_Doing == do_revive) DA tra bo dem o trong KNpc::DoRevive
    (KNpc.cpp:2307 DecRef + :2308 NpcChangeRegion sang VOID_REGION), nhung
    m_RegionIndex KHONG bi reset (KSubWorld.cpp:2368 co y bo qua VOID_REGION)
    => moi noi lay "m_RegionIndex >= 0" lam bang chung con chiem o roi DecRef
    la TRU LAN HAI => an mat phan dem cua NPC KHAC dung chung o => "o chet"
    (KRegion::FindNpc thoat ngay o KRegion.h:191, quai song tang hinh truoc
    va cham). Va goc da go DecRef thua trong OnRevive; dot nay chan not cac
    duong XOA NPC khac co the cham xac do_revive:

    1. KJx2WarInfra.cpp  LuaClearMapNpc        (ClearMapNpc - cuoi tran Viem De;
       hom nay tu triet tieu vi nguoi choi da bi kick, va cho sach)
    2. KJx2WarInfra.cpp  LuaHD3_DelNpcByName
    3. KJx2WarInfra.cpp  LuaHD3_DelNpcByNameEx (HD3_DonNpcCu - hd3_driver.lua
       chay MOI PHUT toan may chu, quet ca boss co xac nhieptran.lua)
    4. KJx2WarInfra.cpp  LuaHD3_DelNpcByScript (cung duong moi phut)
    5. ScriptFuns.cpp    LuaDelNpc             (moi script goi DelNpc)
    6. KMission.cpp      KMission::RemoveNpc   (don NPC cua hoat dong)
    7. ScriptFuns.cpp    LuaNpcEnterNewWorld   (NpcNewWorld - tieudau.lua:191;
       ChangeWorld/SetPos tren xac do_revive vua tru doi vua NOI m_Node vao
       region list khi con nam trong m_NoneRegionNpcList = hong list ->
       chan som: xac thi khong chuyen map, tra 0)

    Khuon chot DA CO SAN trong cay: KRegion.cpp:659 (DelAllNpc bo qua do_revive).
    Nguoi choi o do_revive KHONG tra ref (KNpc.cpp:2300-2312 chi !IsPlayer())
    - ca 6 diem deu da loai nguoi choi truoc khi DecRef nen guard nay an toan.

VA: boc DUNG MOT DONG DecRef moi diem trong "if (Npc[x].m_Doing != do_revive)".
    GIU NGUYEN RemoveNpc (go m_Node khoi list dang chua - can cho xac mo coi)
    va NpcSet.Remove (tra khe + goi s2c_npcremove).
    RIENG ScriptFuns.cpp (diem 5+7) boc them #ifdef _SERVER: tep nay bien dich
    ca CLIENT, ma phia client xac do_revive VAN giu ref (KNpc.cpp:2313-2328
    khong DecRef) nen guard chi duoc song phia server (tien le: KRegion.cpp:647).
    (Phan bien 4 tac tu 31/08: bat bien "do_revive <=> da tra ref" vung tren moi
    duong server; script OnDeath chay luc do_death TRUOC DoRevive nen DecRef cua
    duong chet van chay dung.)

KHONG dung toi: thu tu duyet, dieu kien gom, goi tin, can bang game, .lua.

Mac dinh DIEN TAP; --ghi moi ghi that. Sao luu .truoc_va_vs_3108 canh tep goc.
"""
import io
import os
import re
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
MOC = "[REFOAN-VS 31/08]"

GHICHU = (
    "// %s xac cho hoi sinh (do_revive) DA tra bo dem o trong DoRevive\n"
    "// (KNpc.cpp:2307); m_RegionIndex chi la gia tri cu, khong phai bang chung con\n"
    "// chiem o. DecRef nua la tru LAN HAI -> \"o chet\" (quai song tang hinh truoc\n"
    "// va cham - xem KNpc.cpp:2344). Khuon chot co san: KRegion.cpp:659.\n" % MOC
)

# Moi diem: (tep, ham bao quanh (de neo duy nhat), dong DecRef nguyen van, bien chi so,
#           can #ifdef _SERVER hay khong)
DIEM = [
    ("KJx2WarInfra.cpp", "int LuaClearMapNpc(",
     "SubWorld[w].m_Region[Npc[i].m_RegionIndex].DecRef(Npc[i].m_MapX, Npc[i].m_MapY, obj_npc);",
     "i", False),
    ("KJx2WarInfra.cpp", "int LuaHD3_DelNpcByName(",
     "SubWorld[Npc[n].m_SubWorldIndex].m_Region[Npc[n].m_RegionIndex].DecRef(Npc[n].m_MapX, Npc[n].m_MapY, obj_npc);",
     "n", False),
    ("KJx2WarInfra.cpp", "int LuaHD3_DelNpcByNameEx(",
     "SubWorld[Npc[n].m_SubWorldIndex].m_Region[Npc[n].m_RegionIndex].DecRef(Npc[n].m_MapX, Npc[n].m_MapY, obj_npc);",
     "n", False),
    ("KJx2WarInfra.cpp", "int LuaHD3_DelNpcByScript(",
     "SubWorld[Npc[n].m_SubWorldIndex].m_Region[Npc[n].m_RegionIndex].DecRef(Npc[n].m_MapX, Npc[n].m_MapY, obj_npc);",
     "n", False),
    ("ScriptFuns.cpp", "int LuaDelNpc(",
     "SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].DecRef(Npc[nNpcIndex].m_MapX, Npc[nNpcIndex].m_MapY, obj_npc);",
     "nNpcIndex", True),
    # KMission.cpp: than tep DA nam tron trong #ifdef _SERVER (dong 11-368)
    # -> khong can boc them
    ("KMission.cpp", "BOOL\tKMission::RemoveNpc(",
     "SubWorld[Npc[ulNpcIndex].m_SubWorldIndex].m_Region[Npc[ulNpcIndex].m_RegionIndex].DecRef(Npc[ulNpcIndex].m_MapX, Npc[ulNpcIndex].m_MapY, obj_npc);",
     "ulNpcIndex", False),
]

# Diem 7 - chan som trong LuaNpcEnterNewWorld (khong phai boc DecRef ma la
# tu choi ca thao tac: ChangeWorld:10272 / SetPos:10174 tru doi + hong list).
D7_NEO = (
    "\tint nNpcIndex = (int)Lua_ValueToNumber(L, 1);\n"
    "\tif (nNpcIndex <= 0) return 0;\n"
)
D7_MOI = (
    "\tint nNpcIndex = (int)Lua_ValueToNumber(L, 1);\n"
    "\tif (nNpcIndex <= 0) return 0;\n"
    "#ifdef _SERVER\n"
    "\t// [REFOAN-VS 31/08] xac cho hoi sinh (do_revive) DA tra bo dem o (DoRevive\n"
    "\t// KNpc.cpp:2307) nhung m_RegionIndex con cu -> ChangeWorld (KNpc.cpp:10272)\n"
    "\t// se DecRef LAN HAI va AddNpc noi m_Node vao region list trong khi node con\n"
    "\t// nam trong m_NoneRegionNpcList (hong lien ket). Xac thi khong chuyen map.\n"
    "\t// Chi phia server: client khong DecRef trong DoRevive (KNpc.cpp:2313-2328).\n"
    "\tif (nNpcIndex >= MAX_NPC || (!Npc[nNpcIndex].IsPlayer() && Npc[nNpcIndex].m_Doing == do_revive))\n"
    "\t\treturn 0;\n"
    "#endif\n"
)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def va_mot_diem(tam, ham, dong_decref, bien, can_ifdef):
    """Tim dong DecRef DAU TIEN sau moc ham; tra ve (tam_moi, so_dong) hoac (None, ly do)."""
    vitri_ham = tam.find(ham)
    if vitri_ham < 0:
        return None, "khong thay ham %r" % ham
    # ket thuc ham = dau "\n}" ke tiep (moi ham o day dong ngoac o cot 0)
    vitri_het = tam.find("\n}", vitri_ham)
    if vitri_het < 0:
        return None, "khong thay dau dong ham %r" % ham
    than = tam[vitri_ham:vitri_het]
    # dong DecRef trong than ham, lay ca khoang trang dau dong
    mau = re.compile(r"\n([ \t]+)" + re.escape(dong_decref))
    khop = list(mau.finditer(than))
    if len(khop) != 1:
        return None, "trong %r khop %d dong DecRef (can dung 1)" % (ham, len(khop))
    m = khop[0]
    ws = m.group(1)
    ghichu = "".join(ws + l + "\n" for l in GHICHU.rstrip("\n").split("\n"))
    guard = ws + "if (Npc[%s].m_Doing != do_revive)\n" % bien
    if can_ifdef:
        # ScriptFuns.cpp bien dich ca client; guard chi song phia server
        # (client: xac do_revive VAN giu ref - KNpc.cpp:2313-2328 khong DecRef)
        moi = ("\n#ifdef _SERVER\n" + ghichu + guard + "#endif\n"
               + ws + "\t" + dong_decref)
    else:
        moi = "\n" + ghichu + guard + ws + "\t" + dong_decref
    than_moi = than[:m.start()] + moi + than[m.end():]
    so_dong = tam[:vitri_ham + m.start()].count("\n") + 2
    return tam[:vitri_ham] + than_moi + tam[vitri_het:], so_dong


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t78 va ve sinh: chan DecRef len xac do_revive (6 diem) - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    theo_tep = {}
    for tep, ham, dong, bien, can_ifdef in DIEM:
        theo_tep.setdefault(tep, []).append((ham, dong, bien, can_ifdef))

    ket = {}
    for tep, ds in theo_tep.items():
        p = os.path.join(SRC, tep)
        if not os.path.isfile(p):
            print("!!! LOI TO: khong thay %s" % p)
            return 1
        raw = doc(p)
        if MOC in raw:
            print("  %s: da va roi - bo qua" % tep)
            continue
        crlf = "\r\n" in raw
        tam = raw.replace("\r\n", "\n")
        hi0 = hi(tam)
        ngoac0 = (tam.count("{"), tam.count("}"))
        decref0 = tam.count("DecRef(")

        for ham, dong, bien, can_ifdef in ds:
            tam2, kq = va_mot_diem(tam, ham, dong, bien, can_ifdef)
            if tam2 is None:
                print("!!! LOI TO: %s: %s" % (tep, kq))
                print("    Ma nguon co the da doi. DUNG LAI, khong ghi.")
                return 1
            tam = tam2
            print("  %s dong ~%d: %s -> boc if (Npc[%s].m_Doing != do_revive)%s"
                  % (tep, kq, ham.rstrip("("), bien,
                     " trong #ifdef _SERVER" if can_ifdef else ""))

        # diem 7: chan som trong LuaNpcEnterNewWorld (chi ScriptFuns.cpp).
        # LuaSetNpcPos (ma chet, 0 loi goi) co cung khuon mo dau -> phai neo
        # theo THAN HAM nhu va_mot_diem.
        if tep == "ScriptFuns.cpp":
            vitri_ham = tam.find("int LuaNpcEnterNewWorld(")
            if vitri_ham < 0:
                print("!!! LOI TO: %s: khong thay LuaNpcEnterNewWorld" % tep)
                return 1
            vitri_het = tam.find("\n}", vitri_ham)
            than = tam[vitri_ham:vitri_het]
            n7 = than.count(D7_NEO)
            if n7 != 1:
                print("!!! LOI TO: %s: neo diem 7 trong than LuaNpcEnterNewWorld"
                      " khop %d lan (can 1)" % (tep, n7))
                return 1
            than = than.replace(D7_NEO, D7_MOI)
            tam = tam[:vitri_ham] + than + tam[vitri_het:]
            print("  %s: LuaNpcEnterNewWorld -> chan som xac do_revive (#ifdef _SERVER)"
                  % tep)

        # kiem toan tung tep
        if hi(tam) != hi0:
            print("!!! LOI TO: %s byte cao doi (%d -> %d)" % (tep, hi0, hi(tam)))
            return 1
        if (tam.count("{"), tam.count("}")) != ngoac0:
            print("!!! LOI TO: %s so ngoac nhon doi" % tep)
            return 1
        if tam.count("DecRef(") != decref0:
            print("!!! LOI TO: %s so lenh DecRef( doi (%d -> %d, phai giu nguyen)"
                  % (tep, decref0, tam.count("DecRef(")))
            return 1
        so_guard = tam.count("m_Doing != do_revive)")
        print("  %s: byte cao giu nguyen %d, DecRef( giu nguyen %d, guard moi trong tep: %d"
              % (tep, hi0, decref0, so_guard))
        ket[p] = (tam, crlf)

    if not ket:
        print()
        print("Khong co gi de lam.")
        return 0

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, (tam, crlf) in ket.items():
        sao = p + ".truoc_va_vs_3108"
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
            print("  sao luu: %s" % sao)
        nd = tam.replace("\n", "\r\n") if crlf else tam
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(p) != nd:
            print("!!! LOI TO: %s doc lai KHONG khop" % p)
            return 1
        print("  DA GHI %s" % p)
    return 0


if __name__ == "__main__":
    sys.exit(main())
