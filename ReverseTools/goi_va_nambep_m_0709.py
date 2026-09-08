# -*- coding: ascii -*-
"""delta_patch13.py - [NAMBEP 07/09 m] chi CLIENT, chi THEM LOG (khong doi hanh vi).
Va j dat nhan [S7-NAMBEP] voi dieu kien 'm_CurrentLife > 0 ma van tu the chet'. Sau 11 lan chet (nhat ky
07/09 17:40) nhan NAY KHONG GHI DONG NAO, trong khi chu van thay 'chet ve thanh roi nam duoi nen dat'
=> dieu kien sai: khi nhan vat ket o trang thai chet thi client cung dang giu m_CurrentLife = 0
(NetCommandDeath dat 0), nen cong 'con mau' chan mat bao dong.
Va m:
 (1) [S7-SAUHOISINH] - moc chac chan nhat: sau MOI lan nhan goi hoi sinh cua chinh minh, ghi trang thai
     day du o +1 s, +3 s, +6 s (3 dong/lan chet). Nam bep hay khong deu thay ro.
 (2) [S7-NAMBEP-LAU] - o tu the chet qua 10 giay lien tuc (khong can con mau): chet that luon duoc hoi
     sinh trong ~1 giay (tu dong 5 giay), nen qua 10 giay chac chan la ket.
Byte-safe: latin-1, ASCII-only, CRLF, kiem so byte cao."""
import io, sys
F = {
 "ppc": r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KProtocolProcess.cpp",
 "npc": r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpc.cpp",
}
edits = {k: [] for k in F}
def E(k, old, new, count=1): edits[k].append((old, new, count))

# (1a) dat moc luc nhan goi hoi sinh cho chinh minh
E("ppc",
  "\t\t\tNpc[nIdx].ProcNetCommand(do_revive);\n",
  "\t\t\t// [NAMBEP 07/09 m] moc de ghi trang thai o +1 s / +3 s / +6 s sau hoi sinh (KNpc::Activate)\n"
  "#ifndef _SERVER\n"
  "\t\t\tif (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)\n"
  "\t\t\t{\n"
  "\t\t\t\textern DWORD g_uS7LucHoiSinh;\n"
  "\t\t\t\textern int   g_nS7BuocHoiSinh;\n"
  "\t\t\t\tg_uS7LucHoiSinh = timeGetTime();\n"
  "\t\t\t\tg_nS7BuocHoiSinh = 0;\n"
  "\t\t\t}\n"
  "#endif\n"
  "\t\t\tNpc[nIdx].ProcNetCommand(do_revive);\n")

# (1b) + (2) trong KNpc::Activate, ngay truoc khoi [NAMBEP 07/09 j]
E("npc",
  "\t\t// [NAMBEP 07/09 j] Bao dong tu the CHET khi CON MAU: chu bao 'chet ve thanh roi van nam bep duoi dat'\n",
  "\t\t// [NAMBEP 07/09 m] Ghi trang thai SAU HOI SINH o +1 s / +3 s / +6 s - moc chac chan nhat de doi chieu\n"
  "\t\t// voi cai chu nhin thay (va j dat nhan theo 'con mau' nhung 11 lan chet khong ghi dong nao vi khi ket\n"
  "\t\t// o trang thai chet thi client cung dang giu mau = 0).\n"
  "\t\t{\n"
  "\t\t\tif (g_uS7LucHoiSinh)\n"
  "\t\t\t{\n"
  "\t\t\t\tstatic const DWORD s_auMoc[3] = { 1000, 3000, 6000 };\n"
  "\t\t\t\tconst DWORD uNay = timeGetTime();\n"
  "\t\t\t\twhile (g_nS7BuocHoiSinh < 3 && (DWORD)(uNay - g_uS7LucHoiSinh) >= s_auMoc[g_nS7BuocHoiSinh])\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\tAUTOLOG(\"[S7-SAUHOISINH] +%u ms: doing=%d cdoing=%d resdoing=%d resaction=%d frame=%d/%d life=%d/%d reg=%d cell=(%d,%d) t=%u\",\n"
  "\t\t\t\t\t\t(unsigned int)s_auMoc[g_nS7BuocHoiSinh], (int)m_Doing, (int)m_ClientDoing, m_DataRes.GetResDoing(), m_DataRes.GetAction(),\n"
  "\t\t\t\t\t\tm_Frames.nCurrentFrame, m_Frames.nTotalFrame, m_CurrentLife, m_CurrentLifeMax, m_RegionIndex, m_MapX, m_MapY, SubWorld[0].m_dwCurrentTime);\n"
  "\t\t\t\t\tg_nS7BuocHoiSinh++;\n"
  "\t\t\t\t}\n"
  "\t\t\t\tif (g_nS7BuocHoiSinh >= 3)\n"
  "\t\t\t\t\tg_uS7LucHoiSinh = 0;\n"
  "\t\t\t}\n"
  "\t\t}\n"
  "\t\t// [NAMBEP 07/09 m] O tu the chet qua 10 giay lien tuc - KHONG can con mau (khi ket thi mau cung ket 0).\n"
  "\t\t// Chet that luon duoc hoi sinh trong ~1 giay (tu dong 5 giay) nen qua 10 giay chac chan la ket.\n"
  "\t\t{\n"
  "\t\t\tstatic DWORD s_uLauBatDau = 0, s_uLauGhi = 0;\n"
  "\t\t\tconst DWORD uLauNay = timeGetTime();\n"
  "\t\t\tconst BOOL bChet = (m_Doing == do_death || m_ClientDoing == cdo_death || m_DataRes.GetResDoing() == (int)cdo_death);\n"
  "\t\t\tif (bChet)\n"
  "\t\t\t{\n"
  "\t\t\t\tif (s_uLauBatDau == 0)\n"
  "\t\t\t\t\ts_uLauBatDau = uLauNay;\n"
  "\t\t\t\telse if ((DWORD)(uLauNay - s_uLauBatDau) >= 10000 && (DWORD)(uLauNay - s_uLauGhi) >= 5000)\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\ts_uLauGhi = uLauNay;\n"
  "\t\t\t\t\tAUTOLOG(\"[S7-NAMBEP-LAU] o tu the chet %u ms lien tuc: doing=%d cdoing=%d resdoing=%d resaction=%d frame=%d/%d life=%d/%d reg=%d cell=(%d,%d) t=%u\",\n"
  "\t\t\t\t\t\t(unsigned int)(uLauNay - s_uLauBatDau), (int)m_Doing, (int)m_ClientDoing, m_DataRes.GetResDoing(), m_DataRes.GetAction(),\n"
  "\t\t\t\t\t\tm_Frames.nCurrentFrame, m_Frames.nTotalFrame, m_CurrentLife, m_CurrentLifeMax, m_RegionIndex, m_MapX, m_MapY, SubWorld[0].m_dwCurrentTime);\n"
  "\t\t\t\t}\n"
  "\t\t\t}\n"
  "\t\t\telse if (s_uLauBatDau)\n"
  "\t\t\t{\n"
  "\t\t\t\tif ((DWORD)(uLauNay - s_uLauBatDau) >= 10000)\n"
  "\t\t\t\t\tAUTOLOG(\"[S7-NAMBEP-LAU-HET] dung day sau %u ms t=%u\", (unsigned int)(uLauNay - s_uLauBatDau), SubWorld[0].m_dwCurrentTime);\n"
  "\t\t\t\ts_uLauBatDau = 0;\n"
  "\t\t\t\ts_uLauGhi = 0;\n"
  "\t\t\t}\n"
  "\t\t}\n"
  "\t\t// [NAMBEP 07/09 j] Bao dong tu the CHET khi CON MAU: chu bao 'chet ve thanh roi van nam bep duoi dat'\n")

# bien toan cuc cho moc hoi sinh
E("npc",
  "// [DELTA 07/09 l] bam nhom TRANG THAI KY NANG + CHI SO TOI DA lan phat truoc (goi 223 thay goi day du 98 byte)\n",
  "// [NAMBEP 07/09 m] moc luc chinh minh nhan goi hoi sinh (KProtocolProcess::PlayerRevive dat) + buoc da ghi\n"
  "DWORD g_uS7LucHoiSinh = 0;\n"
  "int   g_nS7BuocHoiSinh = 0;\n"
  "// [DELTA 07/09 l] bam nhom TRANG THAI KY NANG + CHI SO TOI DA lan phat truoc (goi 223 thay goi day du 98 byte)\n")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
out = {}
for k, p in F.items():
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
