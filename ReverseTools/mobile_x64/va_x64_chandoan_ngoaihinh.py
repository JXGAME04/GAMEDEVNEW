# -*- coding: utf-8 -*-
"""[X64 08/09] Chan doan 'nhan vat hien thi sai hinh anh (nhu dang mang mat na) luc vao game, thao/mac lai thi binh thuong'
tren Game.exe x64. Them AUTOLOG '[NGOAIHINH]' (jx_auto.log) - chi client (#ifndef _SERVER), chi nhan vat cua minh, chi luc doi:
 1. KProtocolProcess.cpp SyncPlayer + SyncPlayerMin: sau khi gan m_MaskType tu goi -> ghi armor/helm/weapon/mask(goi)/mantle/horse/fig/low.
 2. KItemList.cpp Equip: truoc khoi [MATDO 06/09] -> ghi item/detail/particular/place va ket qua cac *Type + m_nMaskLock.
 3. KItemList.cpp UnEquip: sau switch (sau case itempart_hoods/default) -> tuong tu.
 4. KNpc.cpp Activate (khoi #mat na): khi m_MaskType != m_MaskMark (dung luc sap ReSetRes) -> ghi mask/mark/fig.
 5. KProtocolProcess.cpp SyncMaskLock: ghi ID.
 6. KNpc.cpp SwitchMaskFeature: ghi co moi + mask.
Byte-safe (chi chen ASCII), moi neo phai DUY NHAT, dem byte >127 khong doi. Chay lai an toan (co dau [X64 08/09 NGOAIHINH])."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
S = ROOT + r"\Sources\Core\Src"
TAG = b"[X64 08/09 NGOAIHINH]"

def rd(p): return io.open(p, "rb").read()
def wr(p, b, nb):
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127), p
    io.open(p, "wb").write(nb)
def nl_of(b): return b"\r\n" if b"\r\n" in b else b"\n"
def block(nl, lines): return nl.join(lines) + nl

# ---------- 1. KProtocolProcess.cpp ----------
p = S + r"\KProtocolProcess.cpp"; b = rd(p); nl = nl_of(b)
if TAG in b:
    print("KProtocolProcess.cpp: da va")
else:
    assert b"AUTOLOG" in b
    pat = re.compile(rb"(\t\tNpc\[nIdx\]\.m_MaskType[ \t]*=[ \t]*pPlaySync->MaskType;[^\r\n]*" + re.escape(nl) + rb")")
    hits = pat.findall(b); assert len(hits) == 2, len(hits)
    names = [b"SyncPlayer", b"SyncPlayerMin"]
    def rep(m, _n=[0]):
        n = names[_n[0]]; _n[0] += 1
        ins = block(nl, [
            b"#ifndef _SERVER",
            b"\tif (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)\t// " + TAG + b" chan doan ngoai hinh cua chinh minh",
            b"\t\tAUTOLOG(\"[NGOAIHINH] " + n + b" self: armor=%d helm=%d weapon=%d mask=%d(goi %d) mantle=%d/%d horse=%d fig=%d low=%d t=%u\", (int)Npc[nIdx].m_ArmorType, (int)Npc[nIdx].m_HelmType, (int)Npc[nIdx].m_WeaponType, (int)Npc[nIdx].m_MaskType, (int)pPlaySync->MaskType, (int)pPlaySync->MantleType, (int)pPlaySync->MantleLevel, (int)(char)pPlaySync->HorseType, (int)Npc[nIdx].m_NpcSettingIdx, (int)Option.GetLow(LowPlayer), (unsigned)SubWorld[0].m_dwCurrentTime);",
            b"#endif"])
        return m.group(1) + ins
    nb = pat.sub(rep, b)
    # 5. SyncMaskLock
    a = b"\tPlayer[CLIENT_PLAYER_INDEX].m_ItemList.SetMaskLock(pInfo->ID);" + nl
    assert nb.count(a) == 1
    nb = nb.replace(a, a + block(nl, [b"\tAUTOLOG(\"[NGOAIHINH] SyncMaskLock id=%d\", (int)pInfo->ID);\t// " + TAG]))
    wr(p, b, nb); print("KProtocolProcess.cpp: 2 sync + SyncMaskLock OK")

# ---------- 2+3. KItemList.cpp ----------
p = S + r"\KItemList.cpp"; b = rd(p); nl = nl_of(b)
if TAG in b:
    print("KItemList.cpp: da va")
else:
    a = b"\t// [MATDO 06/09] O DICH DA CO MON KHAC -> phai go mon cu ra TRUOC."
    assert b.count(a) == 1, b.count(a)
    ins = block(nl, [
        b"#ifndef _SERVER",
        b"\tif (m_PlayerIdx == CLIENT_PLAYER_INDEX)\t// " + TAG + b" chan doan ngoai hinh",
        b"\t\tAUTOLOG(\"[NGOAIHINH] Equip item=%u detail=%d part=%d lv=%d place=%d -> armor=%d helm=%d weapon=%d mask=%d(lock %d) mantle=%d horse=%d\", (unsigned)Item[nIdx].m_dwID, (int)Item[nIdx].GetDetailType(), (int)Item[nIdx].GetParticular(), (int)Item[nIdx].GetLevel(), nEquipPlace, (int)Npc[nNpcIdx].m_ArmorType, (int)Npc[nNpcIdx].m_HelmType, (int)Npc[nNpcIdx].m_WeaponType, (int)Npc[nNpcIdx].m_MaskType, (int)m_nMaskLock, (int)Npc[nNpcIdx].m_MantleType, (int)Npc[nNpcIdx].m_HorseType);",
        b"#endif"])
    nb = b.replace(a, ins + a)
    a2 = b"\t\t\tNpc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(0, 0);" + nl + b"\t\tbreak;" + nl + b"\tdefault:" + nl + b"\t\tbreak;" + nl + b"\t}" + nl
    assert nb.count(a2) == 1, nb.count(a2)
    ins2 = block(nl, [
        b"#ifndef _SERVER",
        b"\tif (m_PlayerIdx == CLIENT_PLAYER_INDEX)\t// " + TAG + b" chan doan ngoai hinh",
        b"\t\tAUTOLOG(\"[NGOAIHINH] UnEquip item=%u detail=%d pos=%d -> armor=%d helm=%d weapon=%d mask=%d(lock %d) mantle=%d horse=%d\", (unsigned)Item[nIdx].m_dwID, (int)Item[nIdx].GetDetailType(), nPos, (int)Npc[nNpcIdx].m_ArmorType, (int)Npc[nNpcIdx].m_HelmType, (int)Npc[nNpcIdx].m_WeaponType, (int)Npc[nNpcIdx].m_MaskType, (int)m_nMaskLock, (int)Npc[nNpcIdx].m_MantleType, (int)Npc[nNpcIdx].m_HorseType);",
        b"#endif"])
    nb = nb.replace(a2, a2 + ins2)
    if b"AUTOLOG" not in b and not re.search(rb'#include\s+"KCore.h"', b):
        # them include de co AUTOLOG (KCore.h dinh nghia macro)
        first = re.search(rb'#include "KItemList.h"[^\r\n]*' + re.escape(nl), nb)
        assert first, "khong thay include KItemList.h"
        nb = nb[:first.end()] + b'#include "KCore.h"\t// ' + TAG + nl + nb[first.end():]
        print("KItemList.cpp: them #include KCore.h")
    wr(p, b, nb); print("KItemList.cpp: Equip + UnEquip OK")

# ---------- 4+6. KNpc.cpp ----------
p = S + r"\KNpc.cpp"; b = rd(p); nl = nl_of(b)
if TAG in b:
    print("KNpc.cpp: da va")
else:
    assert b"AUTOLOG" in b
    a = b"\tif (m_MaskType > 0 && m_MaskMark != 0 && m_MaskMark != m_MaskType)//#mat na"
    assert b.count(a) == 1, b.count(a)
    ins = block(nl, [
        b"#ifndef _SERVER",
        b"\tif (m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex && m_MaskType != m_MaskMark)\t// " + TAG + b" sap ReSetRes vi mat na",
        b"\t\tAUTOLOG(\"[NGOAIHINH] Activate self: mask=%d mark=%d fig=%d armor=%d helm=%d weapon=%d t=%u\", (int)m_MaskType, (int)m_MaskMark, (int)m_NpcSettingIdx, (int)m_ArmorType, (int)m_HelmType, (int)m_WeaponType, (unsigned)SubWorld[0].m_dwCurrentTime);",
        b"#endif"])
    nb = b.replace(a, ins + a)
    a2 = b"\tm_bMaskFeature = !m_bMaskFeature;" + nl
    assert nb.count(a2) == 1, nb.count(a2)
    nb = nb.replace(a2, a2 + block(nl, [
        b"#ifndef _SERVER",
        b"\tAUTOLOG(\"[NGOAIHINH] SwitchMaskFeature -> %d mask=%d\", (int)m_bMaskFeature, (int)m_MaskType);\t// " + TAG,
        b"#endif"]))
    wr(p, b, nb); print("KNpc.cpp: Activate + SwitchMaskFeature OK")
print("XONG")
