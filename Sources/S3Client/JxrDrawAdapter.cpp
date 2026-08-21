//---------------------------------------------------------------------------
// File     : JxrDrawAdapter.cpp
// Muc dich : Cai dat 9 o that su duoc jxreplay.dll goi nguoc khi PHAT LAI.
//            Moi o chi chuyen tiep sang g_pRepresentShell dang chay.
// Xem phan giai thich day du trong JxrDrawAdapter.h
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "JxrDrawAdapter.h"
#include "../Represent/iRepresent/iRepresentShell.h"
#include "../Represent/iRepresent/KRepresentUnit.h"

extern struct iRepresentShell* g_pRepresentShell;

static KJxrDrawAdapter	gs_JxrDrawAdapter;

KJxrDrawAdapter* JxrGetDrawAdapter()
{
	return &gs_JxrDrawAdapter;
}

//--- [4] +0x10  nType 14 -------------------------------------------------
void KJxrDrawAdapter::OutputText(int nFontId, const char* psText, int nCount,
		int nX, int nY, unsigned int Color, int nLineWidth, int nZ,
		unsigned int BorderColor)
{
	if (g_pRepresentShell)
		g_pRepresentShell->OutputText(nFontId, psText, nCount, nX, nY,
				Color, nLineWidth, nZ, BorderColor);
}

//--- [5] +0x14  nType 13 -------------------------------------------------
int KJxrDrawAdapter::OutputRichText(int nFontId, KOutputTextParam* pParam,
		const char* psText, int nCount, int nLineWidth)
{
	if (g_pRepresentShell)
		return g_pRepresentShell->OutputRichText(nFontId, pParam, psText,
				nCount, nLineWidth);
	return 0;
}

//--- [8] +0x20  nType 1 --------------------------------------------------
unsigned int KJxrDrawAdapter::CreateImage(const char* pszName, int nWidth,
		int nHeight, int nType)
{
	if (g_pRepresentShell)
		return g_pRepresentShell->CreateImage(pszName, nWidth, nHeight, nType);
	return 0;
}

//--- [18] +0x48  nType 11 ------------------------------------------------
void KJxrDrawAdapter::DrawPrimitives(int nPrimitiveCount, KRepresentUnit* pPrimitives,
		unsigned int uGenre, int bSinglePlaneCoord)
{
	if (g_pRepresentShell)
		g_pRepresentShell->DrawPrimitives(nPrimitiveCount, pPrimitives,
				uGenre, bSinglePlaneCoord);
}

//--- [19] +0x4C  nType 12 ------------------------------------------------
void KJxrDrawAdapter::DrawPrimitivesOnImage(int nPrimitiveCount, KRepresentUnit* pPrimitives,
		unsigned int uGenre, const char* pszImage, unsigned int uImage,
		short& nImagePosition)
{
	if (g_pRepresentShell)
		g_pRepresentShell->DrawPrimitivesOnImage(nPrimitiveCount, pPrimitives,
				uGenre, pszImage, uImage, nImagePosition);
}

//--- [20] +0x50  nType 4 -------------------------------------------------
// LUU Y: bo GHI cua jxreplay.dll vut bo loai 4, nen o nay chi chay khi doc
// mot tep .jxr do phien ban khac sinh ra. Van cai dat cho day du.
void KJxrDrawAdapter::ClearImageData(const char* pszImage, unsigned int uImage,
		short nImagePosition)
{
	if (g_pRepresentShell)
		g_pRepresentShell->ClearImageData(pszImage, uImage, nImagePosition);
}

//--- [21] +0x54  nType 17 ------------------------------------------------
void KJxrDrawAdapter::LookAt(int nX, int nY, int nZ)
{
	if (g_pRepresentShell)
		g_pRepresentShell->LookAt(nX, nY, nZ);
}

//--- [26] +0x68  nType 21 ------------------------------------------------
void KJxrDrawAdapter::SetLightInfo(int nX, int nY, unsigned int* pLighting)
{
	if (g_pRepresentShell)
		g_pRepresentShell->SetLightInfo(nX, nY, pLighting);
}

//--- [27] +0x6C  nType 20 ------------------------------------------------
void KJxrDrawAdapter::SetOption(int eOption, bool bOn)
{
	if (g_pRepresentShell)
		g_pRepresentShell->SetOption((RepresentOption)eOption, bOn);
}
