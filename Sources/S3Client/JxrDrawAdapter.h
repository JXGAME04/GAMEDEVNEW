//---------------------------------------------------------------------------
// File     : JxrDrawAdapter.h
// Muc dich : Lop chuyen tiep cho DUONG PHAT LAI cua he .jxr
//
// TAI SAO PHAI CO LOP NAY
// -----------------------
// Khi phat lai, jxreplay.dll goi NGUOC vao "giao dien ve" bang cac OFFSET VTABLE
// CHAN CUNG trong DLL (khong the doi):
//        +0x10 nType 14 OutputText            +0x4C nType 12 DrawPrimitivesOnImage
//        +0x14 nType 13 OutputRichText        +0x50 nType  4 ClearImageData
//        +0x20 nType  1 CreateImage           +0x54 nType 17 LookAt
//        +0x48 nType 11 DrawPrimitives        +0x68 nType 21 SetLightInfo
//                                             +0x6C nType 20 SetOption
// Ban tham chieu truyen THANG con tro shell cho SetDrawInterface duoc vi vtable
// iRepresentShell cua ho khop dung 9 offset do. Vtable CUA TA thi LECH: ta co
// them OutputVNText (iRepresentShell.h:127) nen moi o tu chi so 5 tro di dich -1,
// va o duoi con lech them. Doi chieu 9 offset:
//        offset | ban tham chieu        | CUA TA (neu truyen thang)
//        +0x10  | OutputText            | OutputText              <- trung
//        +0x14  | OutputRichText        | OutputVNText            <- LECH
//        +0x20  | CreateImage           | ReleaseAFont            <- LECH
//        +0x48  | DrawPrimitives        | SaveImage               <- LECH
//        +0x4C  | DrawPrimitivesOnImage | DrawPrimitives          <- LECH
//        +0x50  | ClearImageData        | DrawPrimitivesOnImage   <- LECH
//        +0x54  | LookAt                | ClearImageData          <- LECH
//        +0x68  | SetLightInfo          | ViewPortCoordToSpaceCoord <- LECH
//        +0x6C  | SetOption             | SetLightInfo            <- LECH
// 8/9 lech => truyen thang shell se goi nham ham va sap ngay khung dau tien.
//
// Vay nen ta dung mot lop RIENG co vtable dung 38 o THEO THU TU CUA BAN THAM CHIEU,
// va chi 9 o that su duoc cai dat (chuyen tiep sang g_pRepresentShell); 29 o con lai
// chi giu cho. jxreplay.dll KHONG BAO GIO goi chung (da quet toan bo bo giai ma).
//
// QUY UOC GOI: jxreplay.dll nap con tro giao dien vao ECX roi `call [vtable+off]`
// => dung __thiscall, tuc dung mot lop C++ voi ham ao thong thuong la khop.
//---------------------------------------------------------------------------
#ifndef _JXRDRAWADAPTER_H_
#define _JXRDRAWADAPTER_H_

struct KRepresentUnit;
struct KOutputTextParam;

class KJxrDrawAdapter
{
public:
	KJxrDrawAdapter() {}
	// CO Y KHONG khai bao ham huy ao: trong MSVC ham huy ao chiem MOT o vtable
	// tai dung vi tri no duoc khai bao, se day toan bo 38 o xuong 1 nac va lam
	// hong moi offset ma jxreplay.dll chan cung. Doi tuong nay la singleton tinh,
	// khong bao gio bi delete qua con tro lop co so nen khong can ham huy ao.
	~KJxrDrawAdapter() {}

	// -- 38 o vtable, DUNG THU TU cua represent2.dll ban tham chieu --
	// Cac o "Slot__NN" chi giu cho; doi ten cho de doi chieu voi ban goc.
	virtual void Slot00_Release()                {}
	virtual void Slot01_Create()                 {}
	virtual void Slot02_Reset()                  {}
	virtual void Slot03_CreateAFont()            {}

	// [4] +0x10  <- nType 14
	virtual void OutputText(int nFontId, const char* psText, int nCount,
			int nX, int nY, unsigned int Color, int nLineWidth, int nZ,
			unsigned int BorderColor);

	// [5] +0x14  <- nType 13
	virtual int  OutputRichText(int nFontId, KOutputTextParam* pParam,
			const char* psText, int nCount, int nLineWidth);

	virtual void Slot06_LocateRichText()         {}
	virtual void Slot07_ReleaseAFont()           {}

	// [8] +0x20  <- nType 1
	virtual unsigned int CreateImage(const char* pszName, int nWidth,
			int nHeight, int nType);

	virtual void Slot09_FreeImage()              {}
	virtual void Slot10_FreeAllImage()           {}
	virtual void Slot11_GetBitmapDataBuffer()    {}
	virtual void Slot12_ReleaseBitmapDataBuffer(){}
	virtual void Slot13_GetImageParam()          {}
	virtual void Slot14_GetImageFrameParam()     {}
	virtual void Slot15_GetImagePixelAlpha()     {}
	virtual void Slot16_SetImageStoreBalance()   {}
	virtual void Slot17_SaveImage()              {}

	// [18] +0x48  <- nType 11  (dong chinh cua ban dien)
	virtual void DrawPrimitives(int nPrimitiveCount, KRepresentUnit* pPrimitives,
			unsigned int uGenre, int bSinglePlaneCoord);

	// [19] +0x4C  <- nType 12
	virtual void DrawPrimitivesOnImage(int nPrimitiveCount, KRepresentUnit* pPrimitives,
			unsigned int uGenre, const char* pszImage, unsigned int uImage,
			short& nImagePosition);

	// [20] +0x50  <- nType 4
	virtual void ClearImageData(const char* pszImage, unsigned int uImage,
			short nImagePosition);

	// [21] +0x54  <- nType 17
	virtual void LookAt(int nX, int nY, int nZ);

	virtual void Slot22_CopyDeviceImageToImage() {}
	virtual void Slot23_RepresentBegin()         {}
	virtual void Slot24_RepresentEnd()           {}
	virtual void Slot25_ViewPortCoordToSpace()   {}

	// [26] +0x68  <- nType 21
	virtual void SetLightInfo(int nX, int nY, unsigned int* pLighting);

	// [27] +0x6C  <- nType 20
	virtual void SetOption(int eOption, bool bOn);

	virtual void Slot28_GetOption()              {}
	virtual void Slot29_SaveScreenToFile()       {}
	virtual void Slot30_SetGamma()               {}
	virtual void Slot31_SetAdjustColorList()     {}
	virtual void Slot32_SetReplayTimeAndStatus() {}
	virtual void Slot33_SetReplayTime()          {}
	virtual void Slot34_SetJxReplay()            {}
	virtual void Slot35_GetJpgImage()            {}
	virtual void Slot36_GetClipRect()            {}
	virtual void Slot37_SetClipRect()            {}
};

// The hien duy nhat, truyen cho IJXReplay::SetDrawInterface().
KJxrDrawAdapter* JxrGetDrawAdapter();

#endif // _JXRDRAWADAPTER_H_
