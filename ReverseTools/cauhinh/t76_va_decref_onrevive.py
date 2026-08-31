# -*- coding: utf-8 -*-
"""t76_va_decref_onrevive.py - go DecRef THUA trong KNpc::OnRevive (nhanh m_bNoRevive).

TRIEU CHUNG (chu game bao 31/08):
    Trong Viem De, danh mot con quai thuong ma khong bao gio trung ("Danh khong
    trung"). Quai van song (100.000 mau) va van danh lai duoc.

DO THAT tren jx_auto_server.log (nguoi choi CaiBang, phien 31/08):
    - 178 goi danh gui di; may chu CHAP NHAN muc tieu (accept=1 dist=5, ban kinh 90)
    - dan bay toi sat quai: 95 khung cach <= 3 mps
    - nhung phep quet va cham KHONG BAO GIO thay no: map9="........." (9 o deu trong)
    - dan tat voi lasthit=0
    - LOI KHONG PHAI O CON QUAI MA O CAI O:
        con 91481 dung o o (vung 37, 7,2): 440 lan quet, 0 lan cham  = 0,0%
        cung con 91481 o o (vung 91, 8,31):   8 lan quet, 2 lan cham = 25%
        cung con 91481 o o (vung 89, 6,10):   4 lan quet, 2 lan cham = 50%
        cung con 91481 o o (vung 87, 9,3) :   4 lan quet, 1 lan cham = 25%
      Tren 73 o co du mau: 72 o cham 24,9%, dung 1 o chet.

GOC (KRegion.h:191, KRegion::FindNpc):
        if (m_pNpcRef[nMapY * m_nWidth + nMapX] == 0)
            return 0;              <-- thoat NGAY, khong duyet m_NpcList
    => o nao co bo dem ve 0 thi MOI THU dung tren do tang hinh truoc va cham.

CHUOI LAM LECH BO DEM (da doc tan mat tung dong):
    KNpc.cpp:2306   DoRevive: DecRef(m_MapX, m_MapY, obj_npc)          <- TRU LAN 1 (dung)
    KNpc.cpp:2307   DoRevive: NpcChangeRegion(m_RegionIndex, VOID_REGION, m_Index)
    KSubWorld.cpp:2368  if (nDesRnIdx != VOID_REGION) m_RegionIndex = -1;
                        dich CHINH LA VOID_REGION (-2, KSubWorld.h:9) nen KHONG chay
                        => m_RegionIndex GIU NGUYEN gia tri cu (>= 0), du NPC da tra ref
    KNpc.cpp:1718   m_bNoRevive => ep m_Frames.nTotalFrame = 18 (~1 giay)
    KSubWorld.cpp:1148  duyet m_NoneRegionNpcList -> Npc[].Activate()
    KNpc.cpp:920    case do_revive: OnRevive()
    KNpc.cpp:2341   if (m_SubWorldIndex >= 0 && m_RegionIndex >= 0)    <- LOT (gia tri cu)
    KNpc.cpp:2344       DecRef(m_MapX, m_MapY, obj_npc)                <- TRU LAN 2  *** LOI ***

SO CAI: 1 lan AddRef luc sinh (KNpcSet.cpp:542) doi lay 2 lan DecRef.
    KRegion::RemoveNpc (KRegion.cpp:845-866) chi thao tac m_Node, KHONG DecRef.
    KNpcSet::Remove (KNpcSet.cpp:551-599) cung KHONG DecRef.
    => khong co lan thu ba, cung khong co gi bu lai.
Neu o do dang co con KHAC dung (g_nPbNpcChan=0 cho NPC chong o thoai mai; quai Viem De
sinh theo cum trong o vuong 7x7 - npc.lua:500), lan tru thu hai AN MAT phan dem cua
con dang song => con do tang hinh vinh vien tai o ay, chi lanh neu no DI CHUYEN.

VI SAO RIENG VIEM DE: 100% quai Viem De co m_bNoRevive = 1.
    npc.lua:19-22  YDBZ_AddNpcEx = HD3_AddNpcEx
    npc.lua:324-333/386/427/504-513  deu truyen tham so thu 7 = 1
    KJx2WarInfra.cpp:1740-1741  if (nNoRevive != 0) Npc[nNpcIdx].m_bNoRevive = 1;
Quai thuong (khong bNoRevive) di nhanh Revive() -> AddRef lai -> CAN BANG.

Khoi nay mang chu thich [PORT5 23/08] = ma MOI THEM 23/08, trung luc trieu chung xuat hien.

VA: xoa DUNG MOT DONG (2344). Giu RemoveNpc (no go m_Node khoi m_NoneRegionNpcList -
bo di se de lai nut mo coi) va giu NpcSet.Remove (giu dung hanh vi bNoRevive).

KHONG dung toi: KSubWorld.cpp:2368 (ep m_RegionIndex=-1 se lam NpcSet.Remove khong bao
gio chay => ro ri khe NPC, va hong DelAllNpcInWro o KSubWorld.cpp:3003/3037),
KRegion.h/KRegion.cpp, FindNpc, can bang game, cau truc du lieu, goi tin, va khong sua .lua.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp"
MOC = "[VA 31/08]"

# Neo lay ca 3 dong de chac chan duy nhat (dong DecRef don le co the trung cho khac).
CU = (
    "\t\t\t\tSubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);\n"
    "\t\t\t\tSubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);\n"
    "\t\t\t\tNpcSet.Remove(m_Index);\n"
)

MOI = (
    "\t\t\t\tSubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);\n"
    "\t\t\t\t// [VA 31/08] BO DecRef o day: DoRevive (KNpc.cpp:2306) DA tra phan dem cua o\n"
    "\t\t\t\t// nay 18 khung truoc. m_RegionIndex con >= 0 chi vi NpcChangeRegion\n"
    "\t\t\t\t// (KSubWorld.cpp:2368) CO Y khong reset khi dich la VOID_REGION - do la gia\n"
    "\t\t\t\t// tri CU, khong phai bang chung NPC con chiem o. Tru lan hai se AN MAT phan\n"
    "\t\t\t\t// dem cua NPC KHAC dung chung o (g_nPbNpcChan=0 cho chong o) => bo dem ve 0\n"
    "\t\t\t\t// trong khi van con quai song => KRegion::FindNpc (KRegion.h:191) thoat ngay\n"
    "\t\t\t\t// => con do TANG HINH truoc moi phep va cham du van song va van danh tra.\n"
    "\t\t\t\t// Do that: o (vung 37, 7,2) 440 lan quet 0 cham; cung con quai do o ba o khac\n"
    "\t\t\t\t// cham 25-50%. Khuon chot nay da co san o KRegion.cpp:659.\n"
    "\t\t\t\tNpcSet.Remove(m_Index);\n"
)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t76 go DecRef thua trong KNpc::OnRevive - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    if not os.path.isfile(P):
        print("!!! LOI TO: khong thay %s" % P)
        return 1

    raw = doc(P)
    if MOC in raw:
        print("  da va roi - bo qua")
        return 0

    # chuan hoa xuong dong de so khop, nho ghi lai dung kieu cu
    crlf = "\r\n" in raw
    tam = raw.replace("\r\n", "\n")

    n = tam.count(CU)
    if n != 1:
        print("!!! LOI TO: neo khop %d lan (can dung 1)." % n)
        print("    Ma nguon co the da doi. DUNG LAI, khong ghi.")
        return 1

    # kiem dong DecRef bi go co dung la dong 2344 khong (bao cao cho nguoi doc)
    dong = tam.split("\n")
    for i, l in enumerate(dong, 1):
        if "DecRef(m_MapX, m_MapY, obj_npc)" in l and l.startswith("\t\t\t\tSubWorld"):
            print("  dong se go: %d" % i)
            print("    %s" % l.strip())
            break

    hi0 = hi(tam)
    nd = tam.replace(CU, MOI)

    # kiem toan: khong duoc doi byte tieng Viet / GBK
    if hi(nd) != hi0:
        print("!!! LOI TO: byte cao doi (%d -> %d) - chu thich moi phai la ASCII thuan"
              % (hi0, hi(nd)))
        return 1

    # kiem toan: can bang ngoac nhon khong doi
    if nd.count("{") != tam.count("{") or nd.count("}") != tam.count("}"):
        print("!!! LOI TO: so ngoac nhon doi")
        return 1

    # kiem toan: chi bot dung MOT lenh DecRef trong ca tep
    if nd.count("DecRef(") != tam.count("DecRef(") - 1:
        print("!!! LOI TO: so lenh DecRef( bot %d (can bot dung 1)"
              % (tam.count("DecRef(") - nd.count("DecRef(")))
        return 1

    print("  byte cao giu nguyen: %d" % hi0)
    print("  DecRef( trong tep  : %d -> %d (bot dung 1)"
          % (tam.count("DecRef("), nd.count("DecRef(")))
    print("  RemoveNpc/NpcSet.Remove: GIU NGUYEN")

    if crlf:
        nd = nd.replace("\n", "\r\n")

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = P + ".truoc_va_decref_3108"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
        print("  sao luu: %s" % sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI KNpc.cpp")
    return 0


if __name__ == "__main__":
    sys.exit(main())
