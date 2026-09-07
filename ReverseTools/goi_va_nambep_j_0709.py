# -*- coding: ascii -*-
"""delta_patch10.py - [NAMBEP 07/09 j] chi CLIENT.
 (1) CHAN goi dong bo day du (s2c_syncnpc / SyncNpc) dat CHINH NHAN VAT ve tu the CHET khi minh dang song.
     Dieu kien cu 'if (m_Doing != do_death || m_Doing != do_revive)' LUON DUNG (khong the vua bang vua khac)
     nen goi dong bo LUON goi SendCommand(Doing) - ke ca Doing = do_death cho chinh minh. Trang thai chet/hoi
     sinh cua chinh minh CHI duoc phep den tu goi rieng s2c_npcdeath / s2c_playerrevive.
 (2) Nhan bao dong [S7-NAMBEP]: chinh minh CON MAU (m_CurrentLife > 0) ma tu the van la CHET (logic hoac lop ve)
     lien tuc >= 0,5 giay -> ghi day du trang thai moi 2 giay, va ghi mot dong khi het.
Byte-safe: latin-1, ASCII-only, CRLF, kiem so byte cao."""
import io, sys
FILES = {
    "ppc": r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KProtocolProcess.cpp",
    "npc": r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpc.cpp",
}
edits = {k: [] for k in FILES}
def E(k, old, new, count=1): edits[k].append((old, new, count))

# (1) chan goi dong bo day du dat chinh minh ve tu the chet
E("ppc",
  "\tif (Npc[nIdx].m_Doing != do_death || Npc[nIdx].m_Doing != do_revive) // need check later -- spe 03/05/27\n"
  "\t\tNpc[nIdx].SendCommand((NPCCMD)NpcSync->m_Doing, NpcSync->MapX, NpcSync->MapY);\n",
  "\t// [NAMBEP 07/09 j] Dieu kien duoi LUON DUNG (mot gia tri khong the vua bang do_death vua bang do_revive)\n"
  "\t// nen goi dong bo day du LUON ap Doing cua may chu, ke ca cho CHINH NHAN VAT. Neu mot goi mang do_death\n"
  "\t// (vd goi dung khi con dang chet, toi tay sau khi da hoi sinh) thi chinh minh nam xuong lai trong khi may\n"
  "\t// chu van coi la dang dung -> 'chet ve thanh roi van nam bep duoi dat', chi het khi nguoi choi tu di.\n"
  "\t// Trang thai chet/hoi sinh cua chinh minh CHI duoc den tu goi rieng s2c_npcdeath (NetCommandDeath) va\n"
  "\t// s2c_playerrevive (PlayerRevive), nen o day bo qua - khong dung toi NPC khac.\n"
  "\tBOOL bBoLenhChet = FALSE;\n"
  "#ifndef _SERVER\n"
  "\tif (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex &&\n"
  "\t\t(NpcSync->m_Doing == do_death || NpcSync->m_Doing == do_revive) &&\n"
  "\t\tNpc[nIdx].m_Doing != do_death && Npc[nIdx].m_Doing != do_revive)\n"
  "\t{\n"
  "\t\tbBoLenhChet = TRUE;\n"
  "\t\tAUTOLOG(\"[S7-NAMBEP-CHAN] goi dong bo day du dinh dat chinh minh ve tu the chet: sync_doing=%d doing=%d cdoing=%d life=%d sync_life=%d t=%u\", (int)NpcSync->m_Doing, (int)Npc[nIdx].m_Doing, (int)Npc[nIdx].m_ClientDoing, Npc[nIdx].m_CurrentLife, NpcSync->m_CurrentLife, SubWorld[0].m_dwCurrentTime);\n"
  "\t}\n"
  "#endif\n"
  "\tif (!bBoLenhChet && (Npc[nIdx].m_Doing != do_death || Npc[nIdx].m_Doing != do_revive)) // need check later -- spe 03/05/27\n"
  "\t\tNpc[nIdx].SendCommand((NPCCMD)NpcSync->m_Doing, NpcSync->MapX, NpcSync->MapY);\n")

# (2) nhan bao dong [S7-NAMBEP] canh [S9-VE] trong KNpc::Activate
E("npc",
  "\t\tstatic DWORD s_uS9VeT = 0;\n"
  "\t\tstatic int s_nS9VeCu = -999;\n",
  "\t\t// [NAMBEP 07/09 j] Bao dong tu the CHET khi CON MAU: chu bao 'chet ve thanh roi van nam bep duoi dat'\n"
  "\t\t// ma 15/15 lan chet trong nhat ky 07/09 deu dung day trong 0,2 s => phai bat duoc dung luc no xay ra.\n"
  "\t\t// Dieu kien: chinh minh, m_CurrentLife > 0 (da hoi sinh: may chu tra mau ve) nhung logic hoac LOP VE\n"
  "\t\t// van o tu the chet, lien tuc >= 500 ms. Ghi moi 2 s trong luc ket va mot dong khi het ket.\n"
  "\t\t{\n"
  "\t\t\tstatic DWORD s_uNamBepBatDau = 0, s_uNamBepGhi = 0;\n"
  "\t\t\tconst DWORD uNamBepNay = timeGetTime();\n"
  "\t\t\tconst BOOL bTuTheChet = (m_Doing == do_death || m_ClientDoing == cdo_death || m_DataRes.GetResDoing() == (int)cdo_death);\n"
  "\t\t\tif (m_CurrentLife > 0 && bTuTheChet)\n"
  "\t\t\t{\n"
  "\t\t\t\tif (s_uNamBepBatDau == 0)\n"
  "\t\t\t\t\ts_uNamBepBatDau = uNamBepNay;\n"
  "\t\t\t\telse if ((DWORD)(uNamBepNay - s_uNamBepBatDau) >= 500 && (DWORD)(uNamBepNay - s_uNamBepGhi) >= 2000)\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\ts_uNamBepGhi = uNamBepNay;\n"
  "\t\t\t\t\tAUTOLOG(\"[S7-NAMBEP] CON MAU MA VAN O TU THE CHET %u ms: doing=%d cdoing=%d resdoing=%d resaction=%d frame=%d/%d life=%d/%d reg=%d cell=(%d,%d) t=%u\", (unsigned int)(uNamBepNay - s_uNamBepBatDau), (int)m_Doing, (int)m_ClientDoing, m_DataRes.GetResDoing(), m_DataRes.GetAction(), m_Frames.nCurrentFrame, m_Frames.nTotalFrame, m_CurrentLife, m_CurrentLifeMax, m_RegionIndex, m_MapX, m_MapY, SubWorld[0].m_dwCurrentTime);\n"
  "\t\t\t\t}\n"
  "\t\t\t}\n"
  "\t\t\telse if (s_uNamBepBatDau)\n"
  "\t\t\t{\n"
  "\t\t\t\tif ((DWORD)(uNamBepNay - s_uNamBepBatDau) >= 500)\n"
  "\t\t\t\t\tAUTOLOG(\"[S7-NAMBEP-HET] da dung day sau %u ms (doing=%d cdoing=%d resdoing=%d) t=%u\", (unsigned int)(uNamBepNay - s_uNamBepBatDau), (int)m_Doing, (int)m_ClientDoing, m_DataRes.GetResDoing(), SubWorld[0].m_dwCurrentTime);\n"
  "\t\t\t\ts_uNamBepBatDau = 0;\n"
  "\t\t\t\ts_uNamBepGhi = 0;\n"
  "\t\t\t}\n"
  "\t\t}\n"
  "\t\tstatic DWORD s_uS9VeT = 0;\n"
  "\t\tstatic int s_nS9VeCu = -999;\n")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
out = {}
for k, p in FILES.items():
    s = rd(p); before = hb(s)
    for old, new, cnt in edits[k]:
        old = old.replace("\n", "\r\n"); new = new.replace("\n", "\r\n")
        if any(ord(c) >= 0x80 for c in old + new): print("FAIL non-ASCII", k); sys.exit(1)
        n = s.count(old)
        if n != cnt:
            print("FAIL %s: found %d (expect %d): %s" % (k, n, cnt, old[:100].encode("ascii", "replace").decode())); sys.exit(1)
        s = s.replace(old, new)
    if hb(s) != before: print("FAIL high-byte changed", k); sys.exit(1)
    out[k] = (p, s, before)
for k, (p, s, before) in out.items():
    wr(p, s); print("OK %-4s high-bytes %5d  %s" % (k, before, p))
print("ALL APPLIED")
