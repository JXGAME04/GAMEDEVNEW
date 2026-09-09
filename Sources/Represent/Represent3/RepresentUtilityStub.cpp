//---------------------------------------------------------------------------
// [ANDROID 08/09] Thay iRepresent/RepresentUtility.cpp (luu anh chup JPG bang GDI+, chi Windows) tren POSIX:
// cung 4 ham cua RepresentUtility.h; chup man hinh JPG chua co tren Android (tra false, ghi log). Chi bien dich khi JX_POSIX.
//---------------------------------------------------------------------------
#ifdef JX_POSIX
#include "../../Engine/Src/KWin32.h"
#include "../iRepresent/RepresentUtility.h"

bool InitGdiplus() { return false; }
void ShutdownGdiplus() {}
bool SaveBufferToJpgFile32(LPSTR lpFileName, PVOID pBitmap, int nPitch, int nWidth, int nHeight, unsigned int nQuality)
{
	(void)lpFileName; (void)pBitmap; (void)nPitch; (void)nWidth; (void)nHeight; (void)nQuality;
	return false;
}
bool SaveBufferToJpgFile24(LPSTR lpFileName, PVOID pBitmap, int nPitch, int nWidth, int nHeight, unsigned int nQuality)
{
	(void)lpFileName; (void)pBitmap; (void)nPitch; (void)nWidth; (void)nHeight; (void)nQuality;
	return false;
}
#endif // JX_POSIX
