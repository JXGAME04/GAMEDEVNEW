# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 PAK] BO DEM KHOI cho doc pak tren Android (XPackFile::DirectRead) - giam lag Tong Kim (buoc 1: I/O).
# Do tren may ao (12.14): "[REP3-NAP] rut khung 4770 lan / 30 s = 2,4 s tren luong ve" ~ 0,5 ms MOI khung sprite.
# Rut mot khung = XPackFile::GetSprFrame -> DirectRead = 1 seek + 1 read xuong he thong tep. Tren LDPlayer thu muc
# du lieu la thu muc CHIA SE (/mnt/shared/Misc -> D:\jx1_android_data): moi lan doc la mot chuyen khu hoi may ao <->
# may that; tren dien thoai that cung la mot syscall. PC doc qua bo dem trang cua Windows (vai chuc us).
# Cac khung cua cung mot sprite nam KE NHAU trong pak -> doc theo KHOI 64 KB, giu 512 khoi (32 MB) theo LRU: khung
# ke tiep lay tu bo nho. Yeu cau > 1 khoi (tep nguyen, chi muc pak) van doc thang.
# Chi bien dich khi JX_POSIX (Android). Windows (ca GameSDL.exe) khong doi mot dong.
# Tat de A/B: tao tep <thu muc du lieu>\jx_pak_khoi.off. Nhat ky [PAK] moi 30 s trong logcat (SDL/APP).

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 PAK]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten, so=1):
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho: %s" % (so, ten))


P = "Sources/Engine/Src/XPackFile.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

KHOI = [
    "static inline void XP_Close(HANDLE h) { SDL_CloseIO((SDL_IOStream*)h); }",
    "#endif",
    "",
    "#ifdef JX_POSIX",
    "// %s Bo dem KHOI cho DirectRead (chi Android). Moi khung sprite truoc day = 1 seek + 1 read xuong he thong tep" % DAU,
    "// (thu muc chia se cua may ao: ~0,3-0,5 ms/lan; dien thoai: mot syscall). Cac khung cua cung sprite nam ke nhau",
    "// trong pak -> doc theo khoi 64 KB, giu 512 khoi (32 MB) LRU. Chi goi trong pham vi khoa ms_ReadCritical",
    "// (DirectRead chi duoc goi tu GetSprFrame/GetSprHeader/ReadElemFile - deu da giu khoa) nen khong can khoa rieng.",
    "// Tat de A/B: tao tep <thu muc du lieu>\\jx_pak_khoi.off. Thong ke [PAK] moi 30 s ra logcat (SDL/APP).",
    "#define XP_KHOI_CO	(64u * 1024u)",
    "#define XP_KHOI_SO	512",
    "struct XpKhoi { HANDLE h; unsigned int uKhoi; unsigned int uCo; unsigned int uDung; unsigned char* p; };",
    "static XpKhoi		s_xpKhoi[XP_KHOI_SO];",
    "static int			s_nXpKhoiSo = 0;		// so o da cap",
    "static unsigned int	s_uXpTick = 0;			// dong ho LRU",
    "static int			s_nXpBat = -1;			// -1 chua xet, 0 tat, 1 bat",
    "static unsigned		s_uXpDoc = 0, s_uXpTrung = 0, s_uXpTruot = 0, s_uXpThang = 0, s_uXpDiaLan = 0;",
    "static unsigned long long s_uXpDiaByte = 0;",
    "static double		s_dXpDiaMs = 0.0;",
    "static Uint64		s_uXpLogLuc = 0;",
    "",
    "static void XP_KhoiBo(HANDLE h)	// pak dong: bo moi khoi cua handle (handle co the duoc cap lai cho pak khac)",
    "{",
    "	for (int i = 0; i < s_nXpKhoiSo; i++)",
    "		if (s_xpKhoi[i].h == h) { s_xpKhoi[i].h = INVALID_HANDLE_VALUE; s_xpKhoi[i].uCo = 0; }",
    "}",
    "",
    "static bool XP_DocDia(HANDLE h, unsigned int uOff, void* pBuf, unsigned int uLen)	// doc thang + do thoi gian",
    "{",
    "	Uint64 t0 = SDL_GetPerformanceCounter();",
    "	bool bOk = XP_ReadAt(h, uOff, pBuf, uLen);",
    "	Uint64 t1 = SDL_GetPerformanceCounter();",
    "	s_uXpDiaLan++; s_uXpDiaByte += uLen;",
    "	s_dXpDiaMs += (double)(t1 - t0) * 1000.0 / (double)SDL_GetPerformanceFrequency();",
    "	return bOk;",
    "}",
    "",
    "static void XP_ThongKe()",
    "{",
    "	Uint64 uNay = SDL_GetTicks();",
    "	if (s_uXpLogLuc == 0) { s_uXpLogLuc = uNay; return; }",
    "	if (uNay - s_uXpLogLuc < 30000) return;",
    "	unsigned uGiay = (unsigned)((uNay - s_uXpLogLuc) / 1000);",
    "	SDL_Log(\"[PAK] %us: doc %u lan (trung khoi %u, truot %u, doc thang %u) | xuong dia %u lan %.1f MB %.1f ms | khoi dang giu %d x %u KB\",",
    "		uGiay, s_uXpDoc, s_uXpTrung, s_uXpTruot, s_uXpThang, s_uXpDiaLan, (double)s_uXpDiaByte / 1048576.0, s_dXpDiaMs, s_nXpKhoiSo, XP_KHOI_CO / 1024u);",
    "	s_uXpLogLuc = uNay;",
    "	s_uXpDoc = s_uXpTrung = s_uXpTruot = s_uXpThang = s_uXpDiaLan = 0; s_uXpDiaByte = 0; s_dXpDiaMs = 0.0;",
    "}",
    "",
    "// Lay khoi uKhoi cua handle h (doc tu dia neu chua co). Tra NULL neu doc hong.",
    "static XpKhoi* XP_LayKhoi(HANDLE h, unsigned int uKhoi, unsigned int uFileSize)",
    "{",
    "	int i, nChon = -1;",
    "	s_uXpTick++;",
    "	for (i = 0; i < s_nXpKhoiSo; i++)",
    "	{",
    "		if (s_xpKhoi[i].h == h && s_xpKhoi[i].uKhoi == uKhoi && s_xpKhoi[i].uCo)",
    "		{",
    "			s_xpKhoi[i].uDung = s_uXpTick;",
    "			s_uXpTrung++;",
    "			return &s_xpKhoi[i];",
    "		}",
    "	}",
    "	s_uXpTruot++;",
    "	if (s_nXpKhoiSo < XP_KHOI_SO)",
    "	{",
    "		nChon = s_nXpKhoiSo;",
    "		s_xpKhoi[nChon].p = (unsigned char*)malloc(XP_KHOI_CO);",
    "		if (!s_xpKhoi[nChon].p)",
    "			return NULL;",
    "		s_nXpKhoiSo++;",
    "	}",
    "	else",
    "	{	// LRU: o co uDung nho nhat; o trong (uCo = 0) uu tien",
    "		for (i = 0; i < s_nXpKhoiSo; i++)",
    "		{",
    "			if (!s_xpKhoi[i].uCo) { nChon = i; break; }",
    "			if (nChon < 0 || s_xpKhoi[i].uDung < s_xpKhoi[nChon].uDung) nChon = i;",
    "		}",
    "	}",
    "	XpKhoi& k = s_xpKhoi[nChon];",
    "	unsigned int uDau = uKhoi * XP_KHOI_CO;",
    "	unsigned int uCo = (uFileSize > uDau) ? (uFileSize - uDau) : 0;",
    "	if (uCo > XP_KHOI_CO) uCo = XP_KHOI_CO;",
    "	k.h = INVALID_HANDLE_VALUE; k.uCo = 0;",
    "	if (uCo == 0 || !XP_DocDia(h, uDau, k.p, uCo))",
    "		return NULL;",
    "	k.h = h; k.uKhoi = uKhoi; k.uCo = uCo; k.uDung = s_uXpTick;",
    "	return &k;",
    "}",
    "",
    "static bool XP_DocKhoi(HANDLE h, unsigned int uOff, void* pBuf, unsigned int uLen, unsigned int uFileSize)",
    "{",
    "	if (s_nXpBat < 0)",
    "	{",
    "		char szTat[1024];",
    "		s_nXpBat = (access(JxPathPosix(\"\\\\jx_pak_khoi.off\", szTat, sizeof(szTat)), F_OK) == 0) ? 0 : 1;",
    "		SDL_Log(\"[PAK] bo dem khoi doc pak: %s (%d x %u KB)\", s_nXpBat ? \"BAT\" : \"TAT (co jx_pak_khoi.off)\", XP_KHOI_SO, XP_KHOI_CO / 1024u);",
    "	}",
    "	s_uXpDoc++;",
    "	XP_ThongKe();",
    "	if (!s_nXpBat || uLen > XP_KHOI_CO || uLen == 0)",
    "	{",
    "		s_uXpThang++;",
    "		return XP_DocDia(h, uOff, pBuf, uLen);",
    "	}",
    "	unsigned char* pRa = (unsigned char*)pBuf;",
    "	unsigned int uCon = uLen, uTai = uOff;",
    "	while (uCon)",
    "	{",
    "		XpKhoi* k = XP_LayKhoi(h, uTai / XP_KHOI_CO, uFileSize);",
    "		if (!k)",
    "			return false;",
    "		unsigned int uTrong = uTai - (uTai / XP_KHOI_CO) * XP_KHOI_CO;",
    "		if (uTrong >= k->uCo)",
    "			return false;		// ngoai cuoi tep",
    "		unsigned int uLay = k->uCo - uTrong;",
    "		if (uLay > uCon) uLay = uCon;",
    "		memcpy(pRa, k->p + uTrong, uLay);",
    "		pRa += uLay; uTai += uLay; uCon -= uLay;",
    "	}",
    "	return true;",
    "}",
    "#endif // JX_POSIX",
]
s = thay(s, [
    "static inline void XP_Close(HANDLE h) { SDL_CloseIO((SDL_IOStream*)h); }",
    "#endif",
], KHOI, "khai bao bo dem khoi sau XP_Close")

# XP_Close: bo khoi cua handle truoc khi dong (Close() giu ms_ReadCritical)
s = thay(s, [
    "#ifdef JX_PLATFORM_SDL",
    "		XP_Close(m_hFile);",
    "#else",
    "		::CloseHandle(m_hFile);",
    "#endif",
], [
    "#ifdef JX_PLATFORM_SDL",
    "#ifdef JX_POSIX",
    "		XP_KhoiBo(m_hFile);	// %s handle sap dong co the duoc cap lai cho pak khac" % DAU,
    "#endif",
    "		XP_Close(m_hFile);",
    "#else",
    "		::CloseHandle(m_hFile);",
    "#endif",
], "Close")

# DirectRead: qua bo dem khoi
s = thay(s, [
    "#ifdef JX_PLATFORM_SDL",
    "	dwReaded = 0;",
    "	if (uOffset + uSize <= m_uFileSize && XP_ReadAt(m_hFile, uOffset, pBuffer, uSize))",
    "		bResult = true;",
    "#else",
], [
    "#ifdef JX_PLATFORM_SDL",
    "	dwReaded = 0;",
    "#ifdef JX_POSIX",
    "	if (uOffset + uSize <= m_uFileSize && XP_DocKhoi(m_hFile, uOffset, pBuffer, uSize, m_uFileSize))	// %s" % DAU,
    "		bResult = true;",
    "#else",
    "	if (uOffset + uSize <= m_uFileSize && XP_ReadAt(m_hFile, uOffset, pBuffer, uSize))",
    "		bResult = true;",
    "#endif",
    "#else",
], "DirectRead")

ghi(P, s)
print("da va:", P)
