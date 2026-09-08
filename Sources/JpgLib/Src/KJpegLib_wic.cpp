//---------------------------------------------------------------------------
// [X64 08/09] JpgLib cho ban 64-bit: cung 4 ham API cua KJpegLib.h (jpeg_decode_init/info/data/dataEx)
// nhung giai ma bang Windows Imaging Component (WIC, co san trong Windows) thay cho bo giai ma
// hop ngu x86/MMX cua Cloud Wu (KJpegColor.c / KJpegDecode.c / KJpegIdct.c khong bien dich duoc tren x64).
// - Chi bien dich khi _WIN64 (JpgLib.vcxproj loai cac .c hop ngu khoi cau hinh x64).
// - Dau ra y het ban cu: anh 16-bit RGB565 (hoac RGB555 khi jpeg_decode_init(bRGB555=TRUE)), pitch tinh bang BYTE.
// - jpeg_decode_info khong nhan do dai bo dem (API cu cung vay) nen tu quet cau truc JPEG toi dau EOI (FF D9).
// Khi sang mobile, thay than ham nay bang stb_image / bo giai ma cua nen tang, giu nguyen 4 ham API.
//---------------------------------------------------------------------------
#ifdef _WIN64
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincodec.h>
#include <shlwapi.h>
#include <stdlib.h>
extern "C" {
#include "KJpegLib.h"      // ban trong JpgLib\Src khong tu boc extern "C" khi include tu C++
}

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shlwapi.lib")

static BOOL                 s_bRGB555   = FALSE;
static BOOL                 s_bComInit  = FALSE;
static IWICImagingFactory*  s_pFactory  = NULL;
static IWICBitmapSource*    s_pSource   = NULL;   // anh da doi sang 24bpp BGR, doi jpeg_decode_data
static UINT                 s_uWidth    = 0;
static UINT                 s_uHeight   = 0;

static void jpeg_wic_release_source()
{
	if (s_pSource) { s_pSource->Release(); s_pSource = NULL; }
	s_uWidth = s_uHeight = 0;
}

// Quet cau truc JPEG de tim do dai (toi het EOI FF D9). Tra ve 0 neu khong phai JPEG / khong thay EOI trong gioi han.
static size_t jpeg_wic_scan_length(const BYTE* p)
{
	const size_t MAX_SCAN = 64u * 1024u * 1024u;   // 64 MB: gioi han an toan
	if (!p || p[0] != 0xFF || p[1] != 0xD8)
		return 0;
	size_t i = 2;
	// cac doan co do dai truoc SOS
	while (i + 4 <= MAX_SCAN)
	{
		if (p[i] != 0xFF) { i++; continue; }
		BYTE m = p[i + 1];
		if (m == 0xFF) { i++; continue; }            // padding FF
		if (m == 0xD8) { i += 2; continue; }
		if (m == 0xD9) return i + 2;                 // EOI som (anh rong)
		if (m >= 0xD0 && m <= 0xD7) { i += 2; continue; }
		size_t len = ((size_t)p[i + 2] << 8) | p[i + 3];
		if (len < 2) return 0;
		if (m == 0xDA)                               // SOS: tu day la du lieu entropy
		{
			i += 2 + len;
			while (i + 1 < MAX_SCAN)
			{
				if (p[i] == 0xFF)
				{
					BYTE n = p[i + 1];
					if (n == 0xD9) return i + 2;     // EOI
					if (n == 0x00 || (n >= 0xD0 && n <= 0xD7) || n == 0xFF) { i += (n == 0xFF) ? 1 : 2; continue; }
					// marker khac trong vung entropy (JPEG tien trien nhieu SOS): bo qua doan co do dai
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
	if (!s_pFactory)
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (SUCCEEDED(hr))
			s_bComInit = TRUE;          // RPC_E_CHANGED_MODE: da co COM o che do khac -> van dung duoc
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&s_pFactory));
		if (FAILED(hr) || !s_pFactory)
		{
			s_pFactory = NULL;
			return FALSE;
		}
	}
	return TRUE;
}

extern "C" BOOL jpeg_decode_info(PBYTE pJpgBuf, JPEG_INFO* pInfo)
{
	if (!pInfo || !pJpgBuf)
		return FALSE;
	if (!s_pFactory && !jpeg_decode_init(s_bRGB555, TRUE))
		return FALSE;
	jpeg_wic_release_source();

	size_t nLen = jpeg_wic_scan_length(pJpgBuf);
	if (nLen == 0 || nLen > 0x7FFFFFFF)
		return FALSE;

	IStream* pStream = SHCreateMemStream(pJpgBuf, (UINT)nLen);
	if (!pStream)
		return FALSE;

	BOOL bOK = FALSE;
	IWICBitmapDecoder*     pDecoder = NULL;
	IWICBitmapFrameDecode* pFrame   = NULL;
	IWICBitmapSource*      pConv    = NULL;
	do
	{
		if (FAILED(s_pFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &pDecoder)) || !pDecoder)
			break;
		if (FAILED(pDecoder->GetFrame(0, &pFrame)) || !pFrame)
			break;
		WICPixelFormatGUID fmt;
		int nMode = 1;                                   // 1 = mau (nhu YCbCr411 cua ban cu), 0 = xam
		if (SUCCEEDED(pFrame->GetPixelFormat(&fmt)) && IsEqualGUID(fmt, GUID_WICPixelFormat8bppGray))
			nMode = 0;
		if (FAILED(WICConvertBitmapSource(GUID_WICPixelFormat24bppBGR, pFrame, &pConv)) || !pConv)
			break;
		UINT w = 0, h = 0;
		if (FAILED(pConv->GetSize(&w, &h)) || w == 0 || h == 0)
			break;
		s_pSource = pConv; pConv = NULL;
		s_uWidth = w; s_uHeight = h;
		pInfo->mode = nMode;
		pInfo->width = (int)w;
		pInfo->height = (int)h;
		bOK = TRUE;
	} while (0);

	if (pConv) pConv->Release();
	if (pFrame) pFrame->Release();
	if (pDecoder) pDecoder->Release();
	pStream->Release();
	return bOK;
}

extern "C" BOOL jpeg_decode_dataEx(PWORD pBmpBuf, int nPitch, JPEG_INFO* pInfo)
{
	if (!pBmpBuf || !pInfo || !s_pSource || s_uWidth == 0 || s_uHeight == 0)
		return FALSE;
	if (nPitch < (int)(s_uWidth * 2))
		return FALSE;

	UINT   uStride = s_uWidth * 3;
	size_t nSize   = (size_t)uStride * s_uHeight;
	BYTE*  pBGR    = (BYTE*)malloc(nSize);
	if (!pBGR)
		return FALSE;
	HRESULT hr = s_pSource->CopyPixels(NULL, uStride, (UINT)nSize, pBGR);
	if (FAILED(hr))
	{
		free(pBGR);
		jpeg_wic_release_source();
		return FALSE;
	}
	for (UINT y = 0; y < s_uHeight; y++)
	{
		const BYTE* s = pBGR + (size_t)y * uStride;
		WORD* d = (WORD*)((BYTE*)pBmpBuf + (size_t)y * nPitch);
		if (s_bRGB555)
		{
			for (UINT x = 0; x < s_uWidth; x++, s += 3)
				d[x] = (WORD)(((s[2] >> 3) << 10) | ((s[1] >> 3) << 5) | (s[0] >> 3));
		}
		else
		{
			for (UINT x = 0; x < s_uWidth; x++, s += 3)
				d[x] = (WORD)(((s[2] >> 3) << 11) | ((s[1] >> 2) << 5) | (s[0] >> 3));
		}
	}
	free(pBGR);
	jpeg_wic_release_source();
	return TRUE;
}

extern "C" BOOL jpeg_decode_data(PWORD pBmpBuf, JPEG_INFO* pInfo)
{
	if (!pInfo)
		return FALSE;
	return jpeg_decode_dataEx(pBmpBuf, (int)(s_uWidth * 2), pInfo);
}
#endif // _WIN64
