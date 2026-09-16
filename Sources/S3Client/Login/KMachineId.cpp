/*
 * KMachineId.cpp - [MAYID 14/09] Ma may on dinh cho client PC. Xem KMachineId.h de biet vi sao.
 *
 * NGUYEN TAC VIET O DAY:
 *   - Khong bao gio nem ngoai le, khong bao gio lam treo dang nhap. Moi loi -> bo qua nguon do, di tiep.
 *   - Moi loi goi API deu co duong lui. Neu khong lay duoc gi thi van tra ve mot chuoi hop le hang 'C'.
 *   - Tinh MOT LAN roi nho lai (dang nhap lai khong tinh lai).
 */

/*
 * Tep nay KHONG dung tien bien dich (PCH) cua S3Client: PCH do dat _WIN32_WINNT = 0x0400 (NT 4.0),
 * ma ta can khai bao cua Vista tro len. Da dat NotUsing trong S3Client.vcxproj cho rieng tep nay.
 */
#ifdef JX_MOBILE
/*
 * [MAYID 16/09] Android / iOS (nhanh mobile-0809 rao bang JX_MOBILE): KHONG co SMBIOS, PhysicalDrive, GetAdaptersInfo.
 * Giu nguyen cach cu cua ban mobile: shim GetCurrentHwProfile trong Engine/Src/Platform/KPosixWin32.cpp bam
 * hostname + thu muc du lieu thanh chuoi dang "{%08lX-4E44-4A58-B1B1-%012lX}" (38 ky tu, bat dau bang '{' nen may chu
 * dem/da binh thuong, khong roi vao hang C). Login.cpp / NetConnectAgent.cpp goi JX_GetMachineId() o ca hai nen.
 * CHUA DUNG THU tren mobile trong phien 16/09 (phien PC) - khi gop sang mobile-0809 phai them tep nay vao danh sach
 * nguon S3Client cua android/CMakeLists.txt va dung thu.
 */
#include "KWin32.h"
#include <string.h>
#include "KMachineId.h"

static char s_szMachineId[40] = { 0 };

const char* JX_GetMachineId(void)
{
	if (s_szMachineId[0] == 0)
	{
		HW_PROFILE_INFO hw;

		memset(&hw, 0, sizeof(hw));

		if (GetCurrentHwProfile(&hw) && hw.szHwProfileGuid[0])
		{
			strncpy(s_szMachineId, hw.szHwProfileGuid, sizeof(s_szMachineId) - 1);
			s_szMachineId[sizeof(s_szMachineId) - 1] = 0;
		}
		else
		{
			strcpy(s_szMachineId, "C00000000000000000000000000000000");
		}
	}

	return s_szMachineId;
}

char JX_GetMachineIdTier(void)
{
	return JX_GetMachineId()[0];
}

#else /* ---- PC (Windows that) ---- */


#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>
#include <winioctl.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "iphlpapi.lib")
/* [MAYID 15/09 PHAN BIEN] _LayMachineGuid dung RegOpenKeyExA/RegQueryValueExA/RegCloseKey. MSBuild mac dinh
 * co advapi32 nen van lien ket duoc, nhung khai ro de khong phu thuoc ngam dinh. */
#pragma comment(lib, "advapi32.lib")

#include "KMachineId.h"

/*---------------------------------------------------------------- bam FNV-1a 64 bit */

static unsigned __int64 _Fnv1a64(const unsigned char* p, size_t n, unsigned __int64 seed)
{
	unsigned __int64 h = seed;

	for (size_t i = 0; i < n; i++)
	{
		h ^= (unsigned __int64)p[i];
		h *= 1099511628211ULL;
	}

	return h;
}

/*---------------------------------------------------------------- tien ich chuoi */

/* Cat khoang trang hai dau, tra ve TRUE neu con lai it nhat mot ky tu co nghia. */
static BOOL _TrimUseful(char* s)
{
	if (!s)
		return FALSE;

	size_t n = strlen(s);
	size_t b = 0;

	while (b < n && (unsigned char)s[b] <= ' ')
		b++;

	while (n > b && (unsigned char)s[n - 1] <= ' ')
		n--;

	if (n <= b)
	{
		s[0] = 0;
		return FALSE;
	}

	memmove(s, s + b, n - b);
	s[n - b] = 0;

	/* Se-ri toan '0' hoac toan mot ky tu lap lai thi coi nhu vo nghia. */
	{
		size_t i;
		BOOL bKhac = FALSE;

		for (i = 1; s[i]; i++)
		{
			if (s[i] != s[0])
			{
				bKhac = TRUE;
				break;
			}
		}

		if (!bKhac)
		{
			s[0] = 0;
			return FALSE;
		}
	}

	return TRUE;
}

/*---------------------------------------------------------------- nguon 1: UUID he thong trong SMBIOS */

/* Chi de ghi log chan doan: UUID doc duoc (dang wmic hien thi) va ket luan ve no. */
static char s_szUuidHienThi[40] = { 0 };
static char s_szUuidGhiChu[64] = { 0 };

static void _DatGhiChuUuid(const char* sz)
{
	::strncpy(s_szUuidGhiChu, sz ? sz : "", sizeof(s_szUuidGhiChu) - 1);
	s_szUuidGhiChu[sizeof(s_szUuidGhiChu) - 1] = 0;
}

/*
 * [MAYID 16/09] Nhan dien UUID MAU (placeholder) cua firmware. Phai loai, khong thi ca dan may cung lo bo mach
 * ra CUNG MOT ma, ma lai la hang A (khong co duong mien) - dung trieu chung ban dau.
 *
 * BAY THU TU BYTE (loi cua ban 14/09): 'wmic csproduct get uuid' hien thi UUID sau khi DAO little-endian ba
 * truong dau (4-2-2 byte). Mau hay gap nhat hien thi la 03000200-0400-0500-0006-000700080009, nhung 16 byte
 * THO trong bang SMBIOS la 00 02 00 03 | 00 04 | 00 05 | 00 06 | 00 07 00 08 00 09 (day tang dan). Ban 14/09
 * chep hang so theo thu tu HIEN THI nen memcmp khong bao gio khop. Do that tren may dev 16/09:
 * tho 86 7C B8 34 C0 CA 16 04 ... <-> wmic 34B87C86-CAC0-0416-.... Nay so CA HAI thu tu cho moi mau.
 */
static BOOL _UuidLaMau(const unsigned char u[16])
{
	/* Moi mau ghi theo thu tu byte THO; thu tu hien thi duoc suy ra luc so. */
	static const unsigned char s_aMau[][16] =
	{
		/* wmic: 03000200-0400-0500-0006-000700080009 (AMI/Intel mac dinh, gap nhieu nhat) */
		{ 0x00,0x02,0x00,0x03, 0x00,0x04, 0x00,0x05, 0x00,0x06, 0x00,0x07,0x00,0x08,0x00,0x09 },
		/* wmic: 12345678-1234-5678-90AB-CDDEEFAABBCC */
		{ 0x78,0x56,0x34,0x12, 0x34,0x12, 0x78,0x56, 0x90,0xAB, 0xCD,0xDE,0xEF,0xAA,0xBB,0xCC },
		/* wmic: 00010203-0405-0607-0809-0A0B0C0D0E0F (day 00..0F) */
		{ 0x03,0x02,0x01,0x00, 0x05,0x04, 0x07,0x06, 0x08,0x09, 0x0A,0x0B,0x0C,0x0D,0x0E,0x0F },
		/* wmic: 4C4C4544-0000-2010-8020-80C04F202020 (Dell chua ghi service tag) */
		{ 0x44,0x45,0x4C,0x4C, 0x00,0x00, 0x10,0x20, 0x80,0x20, 0x80,0xC0,0x4F,0x20,0x20,0x20 },
	};
	char szLyDo[64];
	int i, k;
	int nSoByte0 = 0;
	BOOL bDeuGiong = TRUE;

	for (i = 0; i < 16; i++)
	{
		if (u[i] == 0)
			nSoByte0++;

		if (u[i] != u[0])
			bDeuGiong = FALSE;
	}

	/* Toan mot gia tri (00 00 ..., FF FF ..., AA AA ...). */
	if (bDeuGiong)
	{
		::_snprintf(szLyDo, sizeof(szLyDo) - 1, "LOAI:16-byte-deu-%02X", u[0]);
		szLyDo[sizeof(szLyDo) - 1] = 0;
		_DatGhiChuUuid(szLyDo);
		return TRUE;
	}

	for (k = 0; k < (int)(sizeof(s_aMau) / sizeof(s_aMau[0])); k++)
	{
		const unsigned char* m = s_aMau[k];
		unsigned char d[16];

		/* d = cung mau nhung theo thu tu HIEN THI (dao 4-2-2 byte dau) */
		d[0] = m[3]; d[1] = m[2]; d[2] = m[1]; d[3] = m[0];
		d[4] = m[5]; d[5] = m[4];
		d[6] = m[7]; d[7] = m[6];
		memcpy(d + 8, m + 8, 8);

		if (memcmp(u, m, 16) == 0 || memcmp(u, d, 16) == 0)
		{
			::_snprintf(szLyDo, sizeof(szLyDo) - 1, "LOAI:mau-so-%d", k + 1);
			szLyDo[sizeof(szLyDo) - 1] = 0;
			_DatGhiChuUuid(szLyDo);
			return TRUE;
		}
	}

	/*
	 * Luat entropy: UUID that (ngau nhien, hoac sinh tu MAC/serial) gan nhu khong bao gio co >= 8 byte 0.
	 * UUID "dien do" cua Gigabyte/ASRock (dau lay tu MAC, duoi giu 0006-000700080009) chi co 2-3 byte 0
	 * nen van duoc coi la that - chung khac nhau theo tung bo mach.
	 */
	if (nSoByte0 >= 8)
	{
		::_snprintf(szLyDo, sizeof(szLyDo) - 1, "LOAI:%d-byte-0", nSoByte0);
		szLyDo[sizeof(szLyDo) - 1] = 0;
		_DatGhiChuUuid(szLyDo);
		return TRUE;
	}

	_DatGhiChuUuid("ok");
	return FALSE;
}

/*
 * Doc bang SMBIOS tho, tim cau truc Type 1 (System Information), lay 16 byte UUID o offset 0x08.
 * Day la dinh danh do NHA SAN XUAT BO MACH ghi - nhan ban dia KHONG lam no doi.
 */
static BOOL _LaySmbiosUuid(unsigned char out[16])
{
	BOOL bOk = FALSE;
	UINT uSize;
	BYTE* pBuf = NULL;

	s_szUuidHienThi[0] = 0;
	_DatGhiChuUuid("khong-thay-Type1");

	/*
	 * Goi DONG: GetSystemFirmwareTable chi co tu Windows Vista. Neu lien ket TINH thi Game.exe se
	 * KHONG MO DUOC tren Windows XP (thieu ham nhap). Lay qua GetProcAddress de may cu chi mat
	 * nguon nay chu khong hong ca game.
	 */
	{
		typedef UINT (WINAPI *PFN_GSFT)(DWORD, DWORD, PVOID, DWORD);
		static PFN_GSFT s_pfn = NULL;
		static BOOL s_bDaTim = FALSE;

		if (!s_bDaTim)
		{
			HMODULE hK32 = ::GetModuleHandleA("kernel32.dll");

			if (hK32)
				s_pfn = (PFN_GSFT)::GetProcAddress(hK32, "GetSystemFirmwareTable");

			s_bDaTim = TRUE;
		}

		if (!s_pfn)
		{
			_DatGhiChuUuid("khong-co-GetSystemFirmwareTable");
			return FALSE;
		}

		uSize = s_pfn('RSMB', 0, NULL, 0);

		if (uSize == 0 || uSize > (1u << 20))
		{
			_DatGhiChuUuid("bang-SMBIOS-rong-hoac-qua-lon");
			return FALSE;
		}

		pBuf = (BYTE*)::malloc(uSize);

		if (!pBuf)
		{
			_DatGhiChuUuid("het-bo-nho");
			return FALSE;
		}

		if (s_pfn('RSMB', 0, pBuf, uSize) != uSize)
		{
			::free(pBuf);
			_DatGhiChuUuid("doc-bang-SMBIOS-loi");
			return FALSE;
		}
	}

	/* RawSMBIOSData: 4 byte dau la thong tin phien ban, roi DWORD Length, roi du lieu bang. */
	if (uSize > 8)
	{
		DWORD dwLen = *(DWORD*)(pBuf + 4);
		BYTE* p = pBuf + 8;
		BYTE* pEnd;

		if (dwLen > uSize - 8)
			dwLen = uSize - 8;

		pEnd = p + dwLen;

		while (p + 4 <= pEnd)
		{
			BYTE byType = p[0];
			BYTE byLen = p[1];
			BYTE* pNext;

			if (byLen < 4)
				break;

			if (p + byLen > pEnd)
				break;

			if (byType == 1 && byLen >= 0x18)
			{
				unsigned char* u = p + 0x08;

				/* Ghi theo dang wmic hien thi (dao 4-2-2 byte dau) de doi chieu 'wmic csproduct get uuid'. */
				::_snprintf(s_szUuidHienThi, sizeof(s_szUuidHienThi) - 1,
					"%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
					u[3], u[2], u[1], u[0], u[5], u[4], u[7], u[6],
					u[8], u[9], u[10], u[11], u[12], u[13], u[14], u[15]);
				s_szUuidHienThi[sizeof(s_szUuidHienThi) - 1] = 0;

				if (!_UuidLaMau(u))
				{
					memcpy(out, u, 16);
					bOk = TRUE;
				}

				break;
			}
			else if (byType == 1)
			{
				_DatGhiChuUuid("Type1-qua-ngan-khong-co-UUID");
				break;
			}

			/* Nhay qua vung dinh dang roi qua vung chuoi (ket thuc bang hai NUL lien tiep). */
			pNext = p + byLen;

			while (pNext + 1 < pEnd && !(pNext[0] == 0 && pNext[1] == 0))
				pNext++;

			pNext += 2;

			if (pNext <= p)
				break;

			p = pNext;
		}
	}

	::free(pBuf);

	return bOk;
}

/*---------------------------------------------------------------- nguon 2: se-ri o dia VAT LY */

/* BusType cua STORAGE_DEVICE_DESCRIPTOR (ntddstor.h) - viet so de khong phu thuoc phien ban SDK. */
#define JX_BUS_USB				7
#define JX_BUS_ISCSI			9
#define JX_BUS_SD				12
#define JX_BUS_MMC				13
#define JX_BUS_VIRTUAL			14
#define JX_BUS_FILEBACKED		15

/* Chi de ghi log chan doan: o dia nao duoc dung / vi sao bi loai. */
static char s_szODiaGhiChu[160] = { 0 };

static void _NoiGhiChu(char* szDst, size_t nDst, const char* szThem)
{
	size_t nCo = strlen(szDst);

	if (!szThem || !szThem[0] || nCo + 2 >= nDst)
		return;

	if (nCo > 0)
		::strncat(szDst, ";", nDst - nCo - 1);

	::strncat(szDst, szThem, nDst - strlen(szDst) - 1);
}

/*
 * Doc se-ri cua \\.\PhysicalDrive<nDrive> (mo quyen 0, KHONG can quyen quan tri). Day la se-ri ghi trong o dia,
 * khac han VolumeSerialNumber (thu bi doi khi format va bi sao khi ghost). Tra ve FALSE kem ly do neu khong doc
 * duoc hoac o dia thuoc loai KHONG gan voi may.
 */
static BOOL _DocSeriDrive(int nDrive, char* szOut, size_t nOut, char* szLyDo, size_t nLyDo)
{
	char szPath[64];
	HANDLE hDev;
	BOOL bOk = FALSE;

	szLyDo[0] = 0;
	::_snprintf(szPath, sizeof(szPath) - 1, "\\\\.\\PhysicalDrive%d", nDrive);
	szPath[sizeof(szPath) - 1] = 0;

	hDev = ::CreateFileA(szPath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hDev == INVALID_HANDLE_VALUE)
	{
		::_snprintf(szLyDo, nLyDo - 1, "drive%d:khong-mo-duoc", nDrive);
		szLyDo[nLyDo - 1] = 0;
		return FALSE;
	}

	{
		STORAGE_PROPERTY_QUERY query;
		BYTE byBuf[1024];
		DWORD dwRet = 0;

		memset(&query, 0, sizeof(query));
		query.PropertyId = StorageDeviceProperty;
		query.QueryType = PropertyStandardQuery;
		memset(byBuf, 0, sizeof(byBuf));

		/* sizeof(byBuf) - 1: byte cuoi luon = 0 nen strncpy ben duoi khong bao gio doc qua duoi dem. */
		if (::DeviceIoControl(hDev, IOCTL_STORAGE_QUERY_PROPERTY,
				&query, sizeof(query), byBuf, sizeof(byBuf) - 1, &dwRet, NULL)
			&& dwRet >= sizeof(STORAGE_DEVICE_DESCRIPTOR))
		{
			STORAGE_DEVICE_DESCRIPTOR* pDesc = (STORAGE_DEVICE_DESCRIPTOR*)byBuf;
			int nBus = (int)pDesc->BusType;

			/*
			 * [MAYID 16/09] Loai o dia KHONG gan voi may: thao roi / USB / the nho (cam rut la ma doi), iSCSI (phong
			 * may khong o cung: ca phong boot tu mot dia ao, se-ri giong nhau hoac rong), dia ao.
			 */
			if (pDesc->RemovableMedia || nBus == JX_BUS_USB || nBus == JX_BUS_SD || nBus == JX_BUS_MMC)
				::_snprintf(szLyDo, nLyDo - 1, "drive%d:thao-roi-usb(bus%d)", nDrive, nBus);
			else if (nBus == JX_BUS_ISCSI || nBus == JX_BUS_VIRTUAL || nBus == JX_BUS_FILEBACKED)
				::_snprintf(szLyDo, nLyDo - 1, "drive%d:iscsi-ao(bus%d)", nDrive, nBus);
			else if (pDesc->SerialNumberOffset > 0 && pDesc->SerialNumberOffset < dwRet)
			{
				::strncpy(szOut, (const char*)byBuf + pDesc->SerialNumberOffset, nOut - 1);
				szOut[nOut - 1] = 0;

				if (_TrimUseful(szOut))
					bOk = TRUE;
				else
				{
					szOut[0] = 0;
					::_snprintf(szLyDo, nLyDo - 1, "drive%d:seri-vo-nghia", nDrive);
				}
			}
			else
				::_snprintf(szLyDo, nLyDo - 1, "drive%d:khong-co-seri", nDrive);
		}
		else
			::_snprintf(szLyDo, nLyDo - 1, "drive%d:ioctl-loi", nDrive);

		szLyDo[nLyDo - 1] = 0;
	}

	::CloseHandle(hDev);

	return bOk;
}

/* So thu tu PhysicalDrive chua o dia Windows (thuong la C:); -1 neu khong xac dinh duoc. */
static int _SoDriveHeThong(void)
{
	char szWin[MAX_PATH];
	char szVol[16];
	HANDLE hVol;
	int nDrive = -1;

	if (::GetWindowsDirectoryA(szWin, sizeof(szWin)) < 2 || szWin[1] != ':')
		return -1;

	::_snprintf(szVol, sizeof(szVol) - 1, "\\\\.\\%c:", szWin[0]);
	szVol[sizeof(szVol) - 1] = 0;

	hVol = ::CreateFileA(szVol, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hVol == INVALID_HANDLE_VALUE)
		return -1;

	{
		BYTE byBuf[512];
		DWORD dwRet = 0;
		VOLUME_DISK_EXTENTS* pExt = (VOLUME_DISK_EXTENTS*)byBuf;

		memset(byBuf, 0, sizeof(byBuf));

		/* Dia dong (dynamic disk) tra ERROR_MORE_DATA nhung extent dau van hop le trong byBuf. */
		if (::DeviceIoControl(hVol, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, byBuf, sizeof(byBuf), &dwRet, NULL)
			|| ::GetLastError() == ERROR_MORE_DATA)
		{
			if (pExt->NumberOfDiskExtents >= 1)
				nDrive = (int)pExt->Extents[0].DiskNumber;
		}
	}

	::CloseHandle(hVol);

	return nDrive;
}

/*
 * [MAYID 16/09] Uu tien o dia HE THONG (chua Windows): PhysicalDrive0 khong chac la o he thong - cam USB truoc khi
 * khoi dong co the doi so thu tu, tuc la ma doi theo viec cam rut. Khong co thi moi thu PhysicalDrive0..3.
 */
static BOOL _LaySeriODia(char* szOut, size_t nOut)
{
	int nHeThong;
	int nDrive;
	char szLyDo[64];
	char szTam[32];

	if (!szOut || nOut == 0)
		return FALSE;

	szOut[0] = 0;
	s_szODiaGhiChu[0] = 0;

	nHeThong = _SoDriveHeThong();

	if (nHeThong >= 0)
	{
		if (_DocSeriDrive(nHeThong, szOut, nOut, szLyDo, sizeof(szLyDo)))
		{
			::_snprintf(szTam, sizeof(szTam) - 1, "drive%d(he-thong)", nHeThong);
			szTam[sizeof(szTam) - 1] = 0;
			_NoiGhiChu(s_szODiaGhiChu, sizeof(s_szODiaGhiChu), szTam);
			return TRUE;
		}

		_NoiGhiChu(s_szODiaGhiChu, sizeof(s_szODiaGhiChu), szLyDo);
	}
	else
		_NoiGhiChu(s_szODiaGhiChu, sizeof(s_szODiaGhiChu), "khong-biet-o-he-thong");

	for (nDrive = 0; nDrive < 4; nDrive++)
	{
		if (nDrive == nHeThong)
			continue;

		if (_DocSeriDrive(nDrive, szOut, nOut, szLyDo, sizeof(szLyDo)))
		{
			::_snprintf(szTam, sizeof(szTam) - 1, "drive%d", nDrive);
			szTam[sizeof(szTam) - 1] = 0;
			_NoiGhiChu(s_szODiaGhiChu, sizeof(s_szODiaGhiChu), szTam);
			return TRUE;
		}

		_NoiGhiChu(s_szODiaGhiChu, sizeof(s_szODiaGhiChu), szLyDo);
	}

	return FALSE;
}


/*---------------------------------------------------------------- nguon 3: MAC card mang that */

/* Chi de ghi log chan doan: bao nhieu card that, card duoc chon co gateway khong. */
static char s_szMacGhiChu[64] = { 0 };

/* Cac tu khoa nhan dang card mang AO / KHONG gan voi may - phai loai vi chung doi hoac trung nhau. */
static BOOL _LaCardAo(const char* szMoTa)
{
	static const char* s_szAo[] =
	{
		"vmware", "virtualbox", "vbox", "hyper-v", "virtual", "tap-", "tap ",
		"loopback", "bluetooth", "npcap", "pcap", "vpn", "tunnel", "teredo",
		"wan miniport", "microsoft wi-fi direct", "hamachi", "radmin", "nordvpn",
		"openvpn", "wireguard", "zerotier", "docker", "wsl", "ppp",
		/* [MAYID 16/09] chia se mang tu dien thoai (Remote NDIS), card mang USB cam rut, tether */
		"ndis", "usb", "tether", "internet sharing"
	};

	char szThuong[512];
	size_t i;
	int k;

	if (!szMoTa || !szMoTa[0])
		return TRUE;

	::strncpy(szThuong, szMoTa, sizeof(szThuong) - 1);
	szThuong[sizeof(szThuong) - 1] = 0;

	for (i = 0; szThuong[i]; i++)
	{
		if (szThuong[i] >= 'A' && szThuong[i] <= 'Z')
			szThuong[i] = (char)(szThuong[i] - 'A' + 'a');
	}

	for (k = 0; k < (int)(sizeof(s_szAo) / sizeof(s_szAo[0])); k++)
	{
		if (::strstr(szThuong, s_szAo[k]) != NULL)
			return TRUE;
	}

	return FALSE;
}

/*
 * [MAYID 16/09] Chon card mang: UU TIEN card CO gateway mac dinh (card dang ra Internet), trong so do lay MAC nho
 * nhat de on dinh khi thu tu liet ke doi. Truoc day lay MAC nho nhat trong MOI card that, nen cam USB-NIC, bat
 * Wi-Fi phu hay chia se mang tu dien thoai la co the ra MAC nho hon => ma doi. Khong card nao co gateway thi
 * moi lay MAC nho nhat nhu cu.
 */
static BOOL _LayMac(unsigned char out[6])
{
	BOOL bOk = FALSE;
	BOOL bCoGw = FALSE;
	int nCardThat = 0;
	ULONG ulLen = 0;
	IP_ADAPTER_INFO* pInfo = NULL;

	s_szMacGhiChu[0] = 0;

	if (::GetAdaptersInfo(NULL, &ulLen) != ERROR_BUFFER_OVERFLOW || ulLen == 0 || ulLen > (1u << 20))
	{
		::strcpy(s_szMacGhiChu, "GetAdaptersInfo-loi");
		return FALSE;
	}

	pInfo = (IP_ADAPTER_INFO*)::malloc(ulLen);

	if (!pInfo)
		return FALSE;

	if (::GetAdaptersInfo(pInfo, &ulLen) == NO_ERROR)
	{
		IP_ADAPTER_INFO* p = pInfo;

		while (p)
		{
			if (p->AddressLength == 6 && p->Type != MIB_IF_TYPE_LOOPBACK && !_LaCardAo(p->Description))
			{
				int i;
				BOOL bAllZero = TRUE;

				for (i = 0; i < 6; i++)
				{
					if (p->Address[i] != 0)
						bAllZero = FALSE;
				}

				/* Bit thu hai cua byte dau = 1 nghia la MAC dat tay / ngau nhien hoa -> khong tin. */
				if (!bAllZero && (p->Address[0] & 0x02) == 0)
				{
					const char* szGw = p->GatewayList.IpAddress.String;
					BOOL bGw = (szGw[0] != 0 && strcmp(szGw, "0.0.0.0") != 0);

					nCardThat++;

					if (!bOk || (bGw && !bCoGw) || (bGw == bCoGw && memcmp(p->Address, out, 6) < 0))
					{
						memcpy(out, p->Address, 6);
						bOk = TRUE;
						bCoGw = bGw;
					}
				}
			}

			p = p->Next;
		}
	}

	::free(pInfo);

	::_snprintf(s_szMacGhiChu, sizeof(s_szMacGhiChu) - 1, "%d-card-that,%s",
		nCardThat, bOk ? (bCoGw ? "co-gateway" : "khong-gateway") : "khong-co");
	s_szMacGhiChu[sizeof(s_szMacGhiChu) - 1] = 0;

	return bOk;
}


/*---------------------------------------------------------------- nguon lui (KHONG song sot qua ghost dia) */

static BOOL _LayMachineGuid(char* szOut, size_t nOut)
{
	HKEY hKey = NULL;
	BOOL bOk = FALSE;

	if (!szOut || nOut == 0)
		return FALSE;

	szOut[0] = 0;

	/*
	 * KEY_WOW64_64KEY: tien trinh 32 bit tren Windows 64 bit phai doc dung nhanh 64 bit,
	 * khong thi lay nham ban sao trong Wow6432Node.
	 */
	if (::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0,
			KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType = 0;
		DWORD dwSize = (DWORD)(nOut - 1);

		if (::RegQueryValueExA(hKey, "MachineGuid", NULL, &dwType,
				(LPBYTE)szOut, &dwSize) == ERROR_SUCCESS && dwType == REG_SZ)
		{
			szOut[dwSize < nOut ? dwSize : nOut - 1] = 0;
			bOk = _TrimUseful(szOut);
		}

		::RegCloseKey(hKey);
	}

	return bOk;
}

/*---------------------------------------------------------------- ghi mot dong chan doan */

static void _GhiChanDoan(const char* szId, const char* szChiTiet)
{
	FILE* f = ::fopen("jx_machineid.log", "a");

	if (!f)
		return;

	::fprintf(f, "[MAYID] id=%s  %s\n", szId ? szId : "?", szChiTiet ? szChiTiet : "");
	::fclose(f);
}

/*---------------------------------------------------------------- dau vao chinh */

static char s_szMachineId[40] = { 0 };

static void _TinhMachineId(void)
{
	unsigned char byUuid[16];
	unsigned char byMac[6];
	char szSeri[256];
	char szGuid[128];
	char szChiTiet[512];

	BOOL bCoUuid = FALSE;
	BOOL bCoSeri = FALSE;
	BOOL bCoMac = FALSE;
	int nManh = 0;
	char chHang;

	unsigned __int64 h1 = 14695981039346656037ULL;
	unsigned __int64 h2 = 1099511628211ULL;

	memset(byUuid, 0, sizeof(byUuid));
	memset(byMac, 0, sizeof(byMac));
	szSeri[0] = 0;
	szGuid[0] = 0;

	/* Moi nguon deu duoc bao boc: mot nguon hong khong lam hong ca ham. */
	__try
	{
		bCoUuid = _LaySmbiosUuid(byUuid);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		bCoUuid = FALSE;
	}

	__try
	{
		bCoSeri = _LaySeriODia(szSeri, sizeof(szSeri));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		bCoSeri = FALSE;
	}

	__try
	{
		bCoMac = _LayMac(byMac);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		bCoMac = FALSE;
	}

	/*
	 * [MAYID 15/09 PHAN BIEN] DOI HAN CACH CHON NGUON: truoc day TRON MOI nguon tim duoc.
	 *
	 * Chu thich cu o day viet 'them mot card mang USB hay cam them o dia khong lam doi ma may' - DO LA SAI,
	 * va tac tu phan bien da chung minh bang do that. Tron moi nguon nghia la ma may phu thuoc vao TAP nguon:
	 * chi can mot nguon XUAT HIEN hay BIEN MAT la ma doi. Cam mot SSD USB, rut no ra, cam day mang USB, chia
	 * se mang tu dien thoai - deu co the doi ma. Ma doi = may bi coi la may MOI = lach duoc gioi han.
	 *
	 * NAY: chon theo THU TU UU TIEN va CHI dung MOT nguon dau tien lay duoc. On dinh hon han, vi ma chi doi
	 * khi chinh nguon da chon bien mat - ma UUID bo mach thi gan nhu khong bao gio bien mat.
 * Uu tien: UUID bo mach (gan voi bo mach chu, ben nhat) > MAC > se-ri o dia.
 * [MAYID 16/09] MAC len truoc o dia: phong may khong o cung (diskless) ca phong boot tu mot dia ao cung se-ri,
 * con MAC chinh la thu may chu boot dung de phan biet tung ghe. Hat giong bam giu nguyen nen may truoc day da
 * dung MAC van ra dung ma cu; may truoc day dung o dia (khong UUID) se doi ma MOT LAN khi len ban nay.
	 * Tinh duy nhat khong giam: rieng UUID bo mach da la duy nhat theo tung may.
	 */
	if (bCoUuid)
	{
		nManh = 3;		/* nguon ben nhat */
		h1 = _Fnv1a64((const unsigned char*)"U", 1, h1);
		h1 = _Fnv1a64(byUuid, sizeof(byUuid), h1);
		h2 = _Fnv1a64(byUuid, sizeof(byUuid), h2 ^ 0x9E3779B97F4A7C15ULL);
	}
	else if (bCoMac)
	{
		nManh = 2;
		h1 = _Fnv1a64((const unsigned char*)"M", 1, h1);
		h1 = _Fnv1a64(byMac, sizeof(byMac), h1);
		h2 = _Fnv1a64(byMac, sizeof(byMac), h2 ^ 0x165667B19E3779F9ULL);
	}
	else if (bCoSeri)
	{
		nManh = 1;
		h1 = _Fnv1a64((const unsigned char*)"S", 1, h1);
		h1 = _Fnv1a64((const unsigned char*)szSeri, strlen(szSeri), h1);
		h2 = _Fnv1a64((const unsigned char*)szSeri, strlen(szSeri), h2 ^ 0xC2B2AE3D27D4EB4FULL);
	}

	if (nManh == 0)
	{
		/*
		 * Khong lay duoc nguon phan cung nao. Dung nguon lui CHI de co mot gia tri on dinh,
		 * va danh dau hang 'C' de may chu KHONG tu choi dang nhap dua tren ma may nay.
		 */
		BOOL bCoGuid = FALSE;

		__try
		{
			bCoGuid = _LayMachineGuid(szGuid, sizeof(szGuid));
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			bCoGuid = FALSE;
		}

		if (bCoGuid)
		{
			h1 = _Fnv1a64((const unsigned char*)szGuid, strlen(szGuid), h1);
			h2 = _Fnv1a64((const unsigned char*)szGuid, strlen(szGuid), h2 ^ 0x27D4EB2F165667C5ULL);
		}
		else
		{
			DWORD dwVol = 0;

			::GetVolumeInformationA("C:\\", NULL, 0, &dwVol, NULL, NULL, NULL, 0);
			h1 = _Fnv1a64((const unsigned char*)&dwVol, sizeof(dwVol), h1);
			h2 = _Fnv1a64((const unsigned char*)&dwVol, sizeof(dwVol), h2 ^ 0x85EBCA77C2B2AE63ULL);
		}
	}

	/* Hang theo DO BEN cua nguon da chon: 3 = UUID bo mach (A), 2 = MAC, 1 = o dia (deu B), 0 = du phong (C). */
	chHang = (nManh >= 3) ? 'A' : ((nManh >= 1) ? 'B' : 'C');

	::_snprintf(s_szMachineId, sizeof(s_szMachineId) - 1, "%c%016I64X%016I64X", chHang, h1, h2);
	s_szMachineId[sizeof(s_szMachineId) - 1] = 0;

	::_snprintf(szChiTiet, sizeof(szChiTiet) - 1,
		"hang=%c nguon_dung=%s (co: uuid=%d mac=%d seri=%d) uuid_smbios=%s [%s] mac=%02X%02X%02X%02X%02X%02X [%s] odia=[%s]",
		chHang,
		(nManh == 3) ? "uuid-bo-mach" : ((nManh == 2) ? "mac" : ((nManh == 1) ? "seri-o-dia" : "du-phong")),
		bCoUuid ? 1 : 0, bCoMac ? 1 : 0, bCoSeri ? 1 : 0,
		s_szUuidHienThi[0] ? s_szUuidHienThi : "-", s_szUuidGhiChu[0] ? s_szUuidGhiChu : "-",
		byMac[0], byMac[1], byMac[2], byMac[3], byMac[4], byMac[5],
		s_szMacGhiChu[0] ? s_szMacGhiChu : "-",
		s_szODiaGhiChu[0] ? s_szODiaGhiChu : "-");
	szChiTiet[sizeof(szChiTiet) - 1] = 0;

	_GhiChanDoan(s_szMachineId, szChiTiet);
}

const char* JX_GetMachineId(void)
{
	if (s_szMachineId[0] == 0)
	{
		__try
		{
			_TinhMachineId();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			s_szMachineId[0] = 0;
		}

		/* Bao hiem cuoi cung: khong bao gio tra ve chuoi rong. */
		if (s_szMachineId[0] == 0)
		{
			::strcpy(s_szMachineId, "C00000000000000000000000000000000");
		}
	}

	return s_szMachineId;
}

char JX_GetMachineIdTier(void)
{
	return JX_GetMachineId()[0];
}

#endif /* JX_MOBILE */
