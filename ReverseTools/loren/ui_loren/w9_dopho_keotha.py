# -*- coding: utf-8 -*-
"""w9_dopho_keotha.py - VA "o Do pho khong bo gi vao duoc het".

GOC: w4 da them 9 nhanh `pos_atlas` vao KItemList.cpp nhung BO SOT DUNG NHANH
QUAN TRONG NHAT - nhanh xu ly THAO TAC KEO-THA THAT (`KItemList.cpp:3754
case pos_enchase`). Cac nhanh da them chi lo phan ke toan (dat vao mang, dong
bo container, go ra, thu hoi...), con nhanh nay moi la cho THUC SU nhan mon do
tu tay nguoi choi (`m_Hand`) va goi `AddAtlasItem`.

Kiem bang danh sach nhanh trong tep:
    368 pos_distill | 377 pos_enchase | 386 pos_atlas | 395 pos_forge   OK
    589 / 594 / 599 / 604                                              OK
    782 / 785 / 788 / 791                                              OK
    922 / 927 / 932 / 937                                              OK
    3716 pos_distill | 3754 pos_enchase | 3792 pos_forge  <-- THIEU pos_atlas
Khong co nhanh nay thi moi cu tha do vao o Do pho deu roi xuong `default` va
KHONG LAM GI => o luon rong, dung nhu chu game ta.

MIENG VA: nhan ban NGUYEN KHUON nhanh pos_enchase, doi
`m_EnchaseItem`->`m_AtlasItem`, `AddEnchaseItem`->`AddAtlasItem`,
`UnEnchaseItem`->`UnAtlasItem`. Khong doi mot phep tinh nao.

Tep dich: Sources\\Core\\Src\\KItemList.cpp (dung chung client + server)
=> build CA HAI cau hinh, thay dong bo CoreClient.dll + CoreServer.dll.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_keotha lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] Do pho: nhanh keo-tha"
DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemList.cpp"
HAU_TO = ".truoc_keotha"


def than(pos, mang, add, un):
    return [
        T + "case %s:" % pos,
        T*2 + "if (Player[this->m_PlayerIdx].CheckTrading())",
        T*3 + "return;",
        T*2 + "if (SrcPos->nX < 0 || SrcPos->nX >= outinpart_num || DesPos->nX < 0 || DesPos->nX >= outinpart_num)",
        T*3 + "return;",
        T*2 + "nEquipIdx1 = %s[SrcPos->nX];" % mang,
        T*2 + "if (m_Hand)",
        T*2 + "{",
        T*3 + "if(%s(m_Hand, DesPos->nX) == TRUE)" % add,
        T*3 + "{",
        T*4 + "if (nEquipIdx1)",
        T*4 + "{",
        T*5 + "%s(nEquipIdx1, SrcPos->nX);" % un,
        T*4 + "}",
        T*4 + "m_Hand = nEquipIdx1;",
        T*4 + "m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;",
        "#ifdef _SERVER",
        T*4 + "g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));",
        "#endif",
        T*3 + "}",
        T*3 + "else if (nEquipIdx1)",
        T*3 + "{",
        T*4 + "%s(nEquipIdx1, SrcPos->nX);" % add,
        T*3 + "}",
        T*2 + "}",
        T*2 + "else",
        T*2 + "{",
        T*3 + "if (nEquipIdx1)",
        T*3 + "{",
        T*4 + "%s(nEquipIdx1, SrcPos->nX);" % un,
        T*3 + "}",
        T*3 + "m_Hand = nEquipIdx1;",
        T*3 + "m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;",
        "#ifdef _SERVER",
        T*3 + "g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));",
        "#endif",
        T*2 + "}",
        T*2 + "break;",
    ]


CU = than("pos_enchase", "m_EnchaseItem", "AddEnchaseItem", "UnEnchaseItem")
MOI = CU + ["", T + "// " + NHAN + " - BO SOT o w4: cac nhanh kia chi lo ke toan,",
            T + "// nhanh NAY moi thuc su nhan mon do tu tay nguoi choi (m_Hand).",
            T + "// Thieu no thi moi cu tha do vao o Do pho deu roi xuong `default`",
            T + "// va khong lam gi => o luon rong."] \
      + than("pos_atlas", "m_AtlasItem", "AddAtlasItem", "UnAtlasItem")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== w9_dopho_keotha - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)
    vt = [i for i in range(len(dong) - len(CU) + 1) if dong[i:i + len(CU)] == CU]
    if len(vt) != 1:
        print("!!! LOI TO: nhanh keo-tha pos_enchase khop %d lan (can 1)" % len(vt))
        return 1
    i = vt[0]
    print("  ok  nhan ban nhanh keo-tha: dong %d..%d (+%d dong)"
          % (i + 1, i + len(CU), len(MOI) - len(CU)))
    dong = dong[:i] + MOI + dong[i + len(CU):]

    nd = eol.join(dong)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if nd.count("{") - raw.count("{") != nd.count("}") - raw.count("}"):
        print("!!! LOI TO: ngoac lech")
        return 1
    # chot: dem so nhanh pos_atlas phai bang so nhanh pos_enchase
    n_a = nd.count("case pos_atlas:")
    n_e = nd.count("case pos_enchase:")
    print("  chot: case pos_atlas = %d | case pos_enchase = %d" % (n_a, n_e))
    if n_a != n_e:
        print("!!! LOI TO: van con nhanh bo sot")
        return 1
    for ten in ("AddAtlasItem", "UnAtlasItem", "m_AtlasItem"):
        print("  chot: %-16s xuat hien %d lan" % (ten, nd.count(ten)))
    print("  byte cao %d (khong doi)" % hi0)

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % os.path.basename(sao))
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    print("\n  => build Core CA HAI cau hinh, thay CoreClient.dll + CoreServer.dll")
    return 0


if __name__ == "__main__":
    sys.exit(main())
