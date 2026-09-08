#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""goi_va_xephang_0709.py

[XEPHANG 07/09] Cua so "Xep hang the gioi": cot trai day du, BANG PHAI TRONG.

GOC (hoi quy tu dot [HOASON 01/09] fa4235ef): TGAME_STAT_DATA co BA ban sao.
Dot Hoa Son noi mang phai [11] -> [MAX_FACTION_NUM+1]=[14] o HAI cho
(Sources/Core/Src/KProtocol.h:2308, Goddess/DBBackup.h:39) - chu thich con ghi
"PHAI cung co" - nhung SOT ban thu ba la Headers/KProtocol.h:1483, ma DUNG ban do
moi la ban Goddess bien dich (Goddess.vcxproj:75 chi co ..\\..\\..\\lib va
..\\..\\..\\Headers, KHONG co Core\\Src).

  => Goddess gui goi 6382 byte; CoreServer doi 7918 byte.
     KLadder::Init (KLadder.cpp:20) "if (uSize != sizeof(...)) return FALSE" - TU CHOI
     IM LANG, khong log. GameStatData giu nguyen ZeroMemory tu ham dung.
     May chu VAN tra 10 ban ghi nhung ten RONG (KProtocolProcess.cpp:5885 strlen("")=0).
     Client vut sach ban ghi ten rong (UiStrengthRank.cpp:672 if(pIncome->szMsg[0]))
     -> ConstructRankView nCount=0 -> khung phai chi con hoa van nen.
     Cot trai van day vi doc cuc bo tu Ui\\IndexAndRankMapping.ini.

DO TREN NHI PHAN DANG CHAY (dem hang so 4 byte little-endian):
     bin\\multiserver\\Goddess.exe  (01/09 22:03): 6382 x6, 7918 x0
     bin\\server\\CoreServer.dll    (07/09 17:04): 6382 x1, 7918 x5
     bin\\multiserver\\StatData.dat : 231.048 byte = dung co ban [14] -> DU LIEU VAN TOT.

BAN VA NAY (may chu; chu game duyet 07/09 "lam tron goi"):
 1. Headers/KProtocol.h : 5 mang [11] -> [MAX_FACTION_NUM + 1]  (het lech co)
 2. Goddess/DBBackup.h  : 5 mang [14] -> [MAX_FACTION_NUM + 1]  (bo hang so roi, het nguy co troi)
 3. Goddess/DBBackup.cpp: 9 vong duyet chieu phai <11 -> < MAX_FACTION_NUM + 1
    (khong co buoc nay thi Hoa Son / Vu Hon / Tieu Dao van rong sau khi sua co).
    CO Y KHONG DUNG dong 709/721 ("Sect <=10" + "[Sect+1]"): quy uoc chi so o do khac
    han cac cho con lai, chua hieu ro -> bao cao rieng, khong doan.
 4. GameServer/KSOServer.cpp: THEM chot kiem do dai cho s2c_gamestatistic (chep mau
    da co san o s2c_roleserver_getroleinfo_result, dot [RECV 04/09]). Bat buoc: goi
    phong 6382 -> 7918 byte = 13 -> 16 khung 512; mat mot khung se thanh DOC TRAN
    thay vi im lang. Kem log in ra co nhan / co mong doi.
 5. Core/KLadder.cpp: log khi tu choi vi lech co (truoc day im lang tuyet doi).

THU TU BAT BUOC: sua (1) TRUOC roi moi (3). Noi vong truoc khi noi mang = GHI TRAN.
Bo va nay lam dung thu tu do trong mot lan chay.

Chay:  python ReverseTools/goi_va_xephang_0709.py [--lui]
"""
import io
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HAUTO = ".truoc_xephang_0709"

FAC = "MAX_FACTION_NUM + 1"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def eol(s, crlf):
    s = s.replace("\r\n", "\n")
    return s.replace("\n", "\r\n") if crlf else s


# ---------------------------------------------------------------- 1. Headers
KP = [
    ("\tTRoleList MoneyStatBySect[11][10];",
     "\t// [XEPHANG 07/09] [11] -> [MAX_FACTION_NUM+1]: ban Core\\Src\\KProtocol.h da doi tu dot\n"
     "\t// [HOASON 01/09] nhung ban NAY thi sot, ma Goddess lai bien dich dung ban nay\n"
     "\t// (Goddess.vcxproj chi co include ..\\Headers) -> Goddess gui 6382, CoreServer doi 7918,\n"
     "\t// KLadder::Init tu choi im lang -> bang xep hang rong. PHAI bang Core\\Src\\KProtocol.h.\n"
     "\tTRoleList MoneyStatBySect[" + FAC + "][10];"),
    ("\tTRoleList LevelStatBySect[11][10];",
     "\tTRoleList LevelStatBySect[" + FAC + "][10];"),
    ("\tint SectPlayerNum[11];",
     "\tint SectPlayerNum[" + FAC + "];"),
    ("\tint SectMoneyMost[11];",
     "\tint SectMoneyMost[" + FAC + "];"),
    ("\tint SectLevelMost[11];",
     "\tint SectLevelMost[" + FAC + "];"),
]

# ------------------------------------------------------------- 2. DBBackup.h
DBH = [
    ("\t\tTRoleList MoneyStatBySect[14][SECTMAXSTATNUM];",
     "\t\t// [XEPHANG 07/09] bo hang so roi 14 -> dung thang MAX_FACTION_NUM+1 nhu hai ban\n"
     "\t\t// KProtocol.h, de lan sau doi so phai khong con cho nao troi lai.\n"
     "\t\tTRoleList MoneyStatBySect[" + FAC + "][SECTMAXSTATNUM];"),
    ("\t\tTRoleList LevelStatBySect[14][SECTMAXSTATNUM];",
     "\t\tTRoleList LevelStatBySect[" + FAC + "][SECTMAXSTATNUM];"),
    ("\t\tint SectPlayerNum[14];",
     "\t\tint SectPlayerNum[" + FAC + "];"),
    ("\t\tint SectMoneyMost[14];",
     "\t\tint SectMoneyMost[" + FAC + "];"),
    ("\t\tint SectLevelMost[14];",
     "\t\tint SectLevelMost[" + FAC + "];"),
]

# ----------------------------------------------------------- 3. DBBackup.cpp
# (so dong 1-based, noi dung mong doi sau khi strip, bien lap)
# Co y BO QUA dong 709/721 (Sect <=10 / [Sect+1]) - quy uoc chi so khac, chua hieu ro.
VONG = [
    (735,  "for(i=0;i<11;++i)", "i", "sap xep MoneyStatBySect"),
    (739,  "for(i=0;i<11;++i)", "i", "sap xep LevelStatBySect"),
    (809,  "for(j=0;j<11;++j)", "j", "tinh mui ten len/xuong (Sort)"),
    (878,  "for(i=0;i<11;++i)", "i", "ghi log theo phai"),
    (896,  "for(i=0;i<11;++i)", "i", "ghi log SectPlayerNum"),
    (901,  "for(i=0;i<11;++i)", "i", "ghi log SectMoneyMost"),
    (906,  "for(i=0;i<11;++i)", "i", "ghi log SectLevelMost"),
    (934,  "for(j=0;j<11;++j)", "j", "chep sang SendStatData (goi gui di)"),
    (1006, "for(i=0;i<11;++i)", "i", "ghi nSectStat nguoc vao ban ghi nhan vat"),
]

# --------------------------------------------------------- 4. KSOServer.cpp
KSO_CU = '''				_ASSERT( pRD->nDataLen == sizeof(TGAME_STAT_DATA) );

				if (m_pCoreServerShell)
				{
					m_pCoreServerShell->SetLadder((void *)pRD->pDataBuffer, pRD->nDataLen);
				}
'''
KSO_MOI = '''				_ASSERT( pRD->nDataLen == sizeof(TGAME_STAT_DATA) );	// vo hieu o ban NDEBUG

#ifndef _STANDALONE
				// [XEPHANG 07/09] Goi thong ke phong tu 6382 -> 7918 byte (13 -> 16 khung 512) sau khi
				// TGAME_STAT_DATA len [MAX_FACTION_NUM+1]. Truoc day khong ai kiem do dai o day: mat mot
				// khung giua duong (lop su co Rainbow 04/09) thi KLadder::Init van memcpy theo nDataLen
				// KHAI BAO -> DOC TRAN. Chep dung mau chot da co o s2c_roleserver_getroleinfo_result.
				{
					const size_t nDauTP = (size_t)((const char*)&pRD->pDataBuffer[0] - (const char*)pRD);
					const size_t nDung = pBuffer->GetUsed();
					if (nDung < nDauTP || nDung - nDauTP != (size_t)pRD->nDataLen)
					{
						printf("--XepHang: goi thong ke ghep duoc %u byte nhung khai bao %u -> BO (mat khung Goddess->GameServer)--\\n",
							(unsigned)(nDung > nDauTP ? nDung - nDauTP : 0), (unsigned)pRD->nDataLen);
						break;
					}
				}
#endif
				// [XEPHANG 07/09] in mot dong de doi chieu co hai ben; truoc day lech co bi nuot hoan toan.
				if ((size_t)pRD->nDataLen != sizeof(TGAME_STAT_DATA))
				{
					printf("--XepHang: LECH CO! Goddess gui %u byte, GameServer doi %u byte -> bang xep hang se RONG. Build lai Goddess sau khi sua Headers/KProtocol.h--\\n",
						(unsigned)pRD->nDataLen, (unsigned)sizeof(TGAME_STAT_DATA));
				}

				if (m_pCoreServerShell)
				{
					m_pCoreServerShell->SetLadder((void *)pRD->pDataBuffer, pRD->nDataLen);
				}
'''

# ------------------------------------------------------------ 5. KLadder.cpp
KL_CU = '''BOOL KLadder::Init(void* pData, size_t uSize)
{
	if (uSize != sizeof(TGAME_STAT_DATA))
		return FALSE;
'''
KL_MOI = '''BOOL KLadder::Init(void* pData, size_t uSize)
{
	if (uSize != sizeof(TGAME_STAT_DATA))
	{
		// [XEPHANG 07/09] truoc day tu choi IM LANG: bang xep hang rong ma khong co lay mot dau vet
		// nao trong log, mat nhieu gio moi lan ra. Gio in mot dong - khong the im lang nua.
		printf("--KLadder::Init: LECH CO, nhan %u byte, doi %u byte -> BO bang xep hang (Goddess va CoreServer bien dich hai ban TGAME_STAT_DATA khac nhau)--\\n",
			(unsigned)uSize, (unsigned)sizeof(TGAME_STAT_DATA));
		return FALSE;
	}
'''

VIEC = [
    ("Headers/KProtocol.h", KP),
    ("Sources/MultiServer/Goddess/DBBackup.h", DBH),
    ("Sources/MultiServer/GameServer/KSOServer.cpp", [(KSO_CU, KSO_MOI)]),
    ("Sources/Core/Src/KLadder.cpp", [(KL_CU, KL_MOI)]),
]

DBCPP = "Sources/MultiServer/Goddess/DBBackup.cpp"


def lui():
    for duong, _ in VIEC:
        p = os.path.join(ROOT, duong)
        h = p + HAUTO
        if os.path.exists(h):
            ghi(p, doc(h)); print("LUI  %s" % duong)
    p = os.path.join(ROOT, DBCPP); h = p + HAUTO
    if os.path.exists(h):
        ghi(p, doc(h)); print("LUI  %s" % DBCPP)
    return 0


def va_theo_neo():
    loi = 0
    for duong, buoc in VIEC:
        p = os.path.join(ROOT, duong)
        if not os.path.exists(p):
            print("KHONG CO TEP %s" % p); return 1
        goc = doc(p); src = goc
        crlf = ("\r\n" in goc)
        print("%s (%s)" % (duong, "CRLF" if crlf else "LF"))
        for cu, moi in buoc:
            c = eol(cu, crlf); m = eol(moi, crlf)
            if m in src:
                print("  BO QUA (da co): %s" % cu.strip().split("\n")[0][:52]); continue
            n = src.count(c)
            if n != 1:
                print("  LOI khop %d lan: %s" % (n, cu.strip().split("\n")[0][:52])); loi += 1; continue
            src = src.replace(c, m, 1)
            print("  OK    %s" % cu.strip().split("\n")[0][:60])
        if loi:
            print("  CO LOI -> KHONG GHI"); continue
        if src != goc:
            h = p + HAUTO
            if not os.path.exists(h): ghi(h, goc)
            if hi(src) != hi(goc):
                print("  LOI: byte >127 doi %d -> %d, KHONG GHI" % (hi(goc), hi(src))); loi += 1; continue
            ghi(p, src); print("  DA GHI (byte >127 giu nguyen %d)" % hi(src))
        else:
            print("  (khong doi gi)")
    return loi


def va_vong_lap():
    """Sua theo SO DONG + kiem noi dung mong doi (an toan hon neo van ban vi
    cac dong 'for(i=0;i<11;++i)' giong het nhau, va chu thich xung quanh la GBK)."""
    p = os.path.join(ROOT, DBCPP)
    if not os.path.exists(p):
        print("KHONG CO TEP %s" % p); return 1
    goc = doc(p)
    crlf = ("\r\n" in goc)
    nl = "\r\n" if crlf else "\n"
    dong = goc.split(nl)
    print("%s (%s, %d dong)" % (DBCPP, "CRLF" if crlf else "LF", len(dong)))
    loi = 0
    doi = 0
    for so, mong, bien, ghichu in VONG:
        if so - 1 >= len(dong):
            print("  LOI dong %d vuot tep" % so); loi += 1; continue
        cu = dong[so - 1]
        moiText = "for(%s=0;%s<%s;++%s)" % (bien, bien, FAC, bien)
        if cu.strip() == moiText:
            print("  BO QUA dong %-4d (da co)  %s" % (so, ghichu)); continue
        if cu.strip() != mong:
            print("  LOI dong %-4d: mong '%s' nhung thay '%s'" % (so, mong, cu.strip())); loi += 1; continue
        thut = cu[:len(cu) - len(cu.lstrip())]
        dong[so - 1] = thut + moiText
        doi += 1
        print("  OK    dong %-4d -> %s   (%s)" % (so, moiText, ghichu))
    if loi:
        print("  CO LOI -> KHONG GHI %s" % DBCPP); return loi
    if not doi:
        print("  (khong doi gi)"); return 0
    src = nl.join(dong)
    h = p + HAUTO
    if not os.path.exists(h): ghi(h, goc)
    if hi(src) != hi(goc):
        print("  LOI: byte >127 doi %d -> %d, KHONG GHI" % (hi(goc), hi(src))); return 1
    ghi(p, src)
    print("  DA GHI %d vong (byte >127 giu nguyen %d)" % (doi, hi(src)))
    return 0


def main():
    if "--lui" in sys.argv:
        return lui()
    # THU TU BAT BUOC: noi MANG truoc (va_theo_neo), roi moi noi VONG LAP.
    loi = va_theo_neo()
    if loi:
        print("\nDUNG LAI: chua noi duoc mang -> KHONG dam noi vong lap (se ghi tran).")
        return 1
    loi += va_vong_lap()
    return 1 if loi else 0


if __name__ == "__main__":
    sys.exit(main())
