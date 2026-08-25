# -*- coding: ascii -*-
"""C10 - ten boss mau VANG nhu ban Linux (chu game chot 25/08 sau restart 10:01).
Chuoi that: client dat m_Type = NpcSync->NpcEnchant (KProtocolProcess.cpp:1873 va
SyncNpcMin :~155) -> switch mau KNpc.cpp:6563; NpcEnchant do server do tu
m_cGold.GetGoldType() tai 2 cho (KNpc.cpp SendSyncData + SendNormalSyncData).
m_byType co trong NPC_SYNC nhung client BO QUA, NPC_NORMAL_SYNC khong co.
=> Va SERVER-ONLY: (1) sHD3_AddNpcCommon doc tham so flag cuoi (cot [8] bang
killbosshead {id,lv,map,x,y,noRevive,"ten",FLAG,"script",..}) -> m_Type=boss_gold;
(2) 2 cho do NpcEnchant: neu GetGoldType()==0 va m_Type==boss_gold thi day
boss_gold qua NpcEnchant. He quai vang (golding) giu uu tien; NPC tinh
bSpecialNpc chi la 0/1 (boss_blue) khong bao gio trung boss_gold => 0 dung cham.
Client CU khong can build lai van thay ten vang.
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

C = r"D:\GAMEDEVNEW\Sources\Core\Src"


def rw(p, fn):
    d = io.open(p, encoding="latin-1", newline="").read()
    d2 = fn(d)
    if d2 != d:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        print("  OK", p.rsplit("\\", 1)[-1])
    else:
        print("  (khong doi)", p)


T = "\t"
NL = "\r\n"


# ---------- 1) KNpc.cpp: 2 cho do NpcEnchant ----------
def f_knpc(d):
    if "m_Type == boss_gold" in d:
        return d
    a1 = T + "NpcSync.NpcEnchant" + T * 3 + "= (WORD)this->m_cGold.GetGoldType();"
    assert d.count(a1) == 1, ("a1", d.count(a1))
    ins1 = NL.join([
        "",
        T + "// [3HD 25/08] boss san boss sat thu / tin su (HD3_AddNpc flag 1): server",
        T + "// danh dau m_Type = boss_gold; client to mau ten theo NpcEnchant (client",
        T + "// dat m_Type = NpcEnchant o ca 2 duong sync) nen day qua truong nay.",
        T + "// He quai vang (GetGoldType != 0) giu uu tien - khong doi hanh vi cu.",
        T + "if (NpcSync.NpcEnchant == 0 && m_Type == boss_gold)",
        T + T + "NpcSync.NpcEnchant" + T * 2 + "= (WORD)boss_gold;",
    ])
    d = d.replace(a1, a1 + ins1)
    a2 = T + "NpcSync.NpcEnchant" + T * 3 + "= m_cGold.GetGoldType();"
    assert d.count(a2) == 1, ("a2", d.count(a2))
    ins2 = NL.join([
        "",
        T + "// [3HD 25/08] nhu SendSyncData: giu mau VANG qua duong sync lien tuc.",
        T + "if (NpcSync.NpcEnchant == 0 && m_Type == boss_gold)",
        T + T + "NpcSync.NpcEnchant" + T * 2 + "= boss_gold;",
    ])
    d = d.replace(a2, a2 + ins2)
    return d
rw(C + r"\KNpc.cpp", f_knpc)


# ---------- 2) KJx2WarInfra.cpp: doc flag cuoi trong sHD3_AddNpcCommon ----------
def f_infra(d):
    if "boss_gold" in d:
        return d
    old = T + "// tham so cuoi (flag/isboss cua ban Linux): AddNpcSet2 da tu nap thuoc tinh, bo qua"
    assert d.count(old) == 1, ("old", d.count(old))
    new = NL.join([
        T + "// tham so cuoi (flag/isboss ban Linux - cot [8] bang killbosshead): dich",
        T + "// nguoc ban Linux ghi +0x181C=3 khi flag==1 => ten boss mau VANG tren",
        T + "// client (chu game chot 25/08). m_Type duoc day sang client qua",
        T + "// NpcEnchant (KNpc.cpp SendSyncData/SendNormalSyncData - va kem 25/08).",
        T + "nArg++;",
        T + "if (nTop >= nArg && Lua_IsNumber(L, nArg) && (int)Lua_ValueToNumber(L, nArg) != 0)",
        T + T + "Npc[nNpcIdx].m_Type = boss_gold;",
    ])
    d = d.replace(old, new)
    return d
rw(C + r"\KJx2WarInfra.cpp", f_infra)
print("xong C10 (nguon) - can build lai CoreServer x64")
