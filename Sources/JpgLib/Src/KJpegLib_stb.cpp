//---------------------------------------------------------------------------
// [ANDROID 08/09] JpgLib cho POSIX (Android/Linux): cung 4 ham API cua KJpegLib.h (jpeg_decode_init/info/data/dataEx)
// nhu ban WIC x64 (KJpegLib_wic.cpp) nhung giai ma bang stb_image (ThirdParty/stb/stb_image.h, public domain / MIT).
// - Dau ra y het ban cu: anh 16-bit RGB565 (hoac RGB555 khi jpeg_decode_init(bRGB555=TRUE)), pitch tinh bang BYTE.
// - jpeg_decode_info khong nhan do dai bo dem (API cu cung vay) nen tu quet cau truc JPEG toi dau EOI (FF D9) - chep tu ban WIC.
// Chi bien dich khi JX_POSIX (CMake Android); ban Windows khong dung tep nay.
//---------------------------------------------------------------------------
#ifdef JX_POSIX
#include "KPosixCompat.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_NO_STDIO
#define STBI_NO_FAILURE_STRINGS
#include "stb_image.h"
extern "C" {
#include "KJpegLib.h"
}

static BOOL   s_bRGB555 = FALSE;
static BYTE*  s_pRGB    = NULL;   // anh da giai ma (RGB 24 bit), doi jpeg_decode_data
static int    s_nWidth  = 0;
static int    s_nHeight = 0;

static void jpeg_stb_release()
{
	if (s_pRGB) { stbi_image_free(s_pRGB); s_pRGB = NULL; }
	s_nWidth = s_nHeight = 0;
}

// Quet cau truc JPEG de tim do dai (toi het EOI FF D9). Tra ve 0 neu khong phai JPEG / khong thay EOI trong gioi han.
static size_t jpeg_stb_scan_length(const BYTE* p)
{
	const size_t MAX_SCAN = 64u * 1024u * 1024u;
	if (!p || p[0] != 0xFF || p[1] != 0xD8)
		return 0;
	size_t i = 2;
	while (i + 4 <= MAX_SCAN)
	{
		if (p[i] != 0xFF) { i++; continue; }
		BYTE m = p[i + 1];
		if (m == 0xFF) { i++; continue; }
		if (m == 0xD8) { i += 2; continue; }
		if (m == 0xD9) return i + 2;
		if (m >= 0xD0 && m <= 0xD7) { i += 2; continue; }
		size_t len = ((size_t)p[i + 2] << 8) | p[i + 3];
		if (len < 2) return 0;
		if (m == 0xDA)
		{
			i += 2 + len;
			while (i + 1 < MAX_SCAN)
			{
				if (p[i] == 0xFF)
				{
					BYTE n = p[i + 1];
					if (n == 0xD9) return i + 2;
					if (n == 0x00 || (n >= 0xD0 && n <= 0xD7) || n == 0xFF) { i += (n == 0xFF) ? 1 : 2; continue; }
					if (i + 4 <= MAX_SCAN)
					{
						size_t l2 = ((size_t)p[i + 2] << 8) | p[i + 3];
						if (l2 < 2) return 0;
						i += 2 + l2;
						continue;
					}
					return 0;
				}
				i++;
			}
			return 0;
		}
		i += 2 + len;
	}
	return 0;
}

extern "C" BOOL jpeg_decode_init(BOOL bRGB555, BOOL bMMXCPU)
{
	(void)bMMXCPU;
	s_bRGB555 = bRGB555;
	return TRUE;
}

extern "C" BOOL jpeg_decode_info(PBYTE pJpgBuf, JPEG_INFO* pInfo)
{
	if (!pInfo || !pJpgBuf)
		return FALSE;
	jpeg_stb_release();
	size_t nLen = jpeg_stb_scan_length(pJpgBuf);
	if (nLen == 0 || nLen > 0x7FFFFFFF)
		return FALSE;
	int w = 0, h = 0, comp = 0;
	if (!stbi_info_from_memory(pJpgBuf, (int)nLen, &w, &h, &comp))
		return FALSE;
	s_pRGB = stbi_load_from_memory(pJpgBuf, (int)nLen, &w, &h, &comp, 3);
	if (!s_pRGB || w <= 0 || h <= 0)
	{
		jpeg_stb_release();
		return FALSE;
	}
	s_nWidth = w; s_nHeight = h;
	pInfo->mode = (comp == 1) ? 0 : 1;   // 1 = mau, 0 = xam (nhu ban cu)
	pInfo->width = w;
	pInfo->height = h;
	return TRUE;
}

extern "C" BOOL jpeg_decode_dataEx(PWORD pBmpBuf, int nPitch, JPEG_INFO* pInfo)
{
	if (!pBmpBuf || !pInfo || !s_pRGB || s_nWidth <= 0 || s_nHeight <= 0)
		return FALSE;
	if (nPitch < s_nWidth * 2)
		return FALSE;
	for (int y = 0; y < s_nHeight; y++)
	{
		const BYTE* s = s_pRGB + (size_t)y * s_nWidth * 3;   // R G B
		WORD* d = (WORD*)((BYTE*)pBmpBuf + (size_t)y * nPitch);
		if (s_bRGB555)
		{
			for (int x = 0; x < s_nWidth; x++, s += 3)
				d[x] = (WORD)(((s[0] >> 3) << 10) | ((s[1] >> 3) << 5) | (s[2] >> 3));
		}
		else
		{
			for (int x = 0; x < s_nWidth; x++, s += 3)
				d[x] = (WORD)(((s[0] >> 3) << 11) | ((s[1] >> 2) << 5) | (s[2] >> 3));
		}
	}
	jpeg_stb_release();
	return TRUE;
}

extern "C" BOOL jpeg_decode_data(PWORD pBmpBuf, JPEG_INFO* pInfo)
{
	if (!pInfo)
		return FALSE;
	return jpeg_decode_dataEx(pBmpBuf, s_nWidth * 2, pInfo);
}
#endif // JX_POSIX
