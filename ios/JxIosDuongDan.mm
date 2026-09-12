//---------------------------------------------------------------------------
// [IOS 11/09] Hoi he dieu hanh cac thu muc chuan cua ung dung.
//
// BAY DA TRA GIA: KHONG duoc include "KWin32.h" (lop gia lap Win32) trong cung mot tep voi header
// Objective-C. Lop gia lap dinh nghia BOOL = int, con <objc/objc.h> dinh nghia BOOL = bool ->
// "typedef redefinition with different types", keo theo ca loat loi "unexpected '@' in program".
// Vi vay tep .mm nay CHI dung Foundation, khong biet gi ve Win32, va bay ra ba ham C thuan
// cho JxIosMain.cpp goi.
//---------------------------------------------------------------------------
#import <Foundation/Foundation.h>
#include <stdio.h>

static const char* JxIos_ThuMuc(NSSearchPathDirectory loai, const char* pszThem, char* pszRa, size_t nRa)
{
	NSArray* ds = NSSearchPathForDirectoriesInDomains(loai, NSUserDomainMask, YES);
	if ([ds count] == 0) { if (nRa) pszRa[0] = 0; return pszRa; }
	if (pszThem && *pszThem)
		snprintf(pszRa, nRa, "%s/%s", [[ds objectAtIndex:0] UTF8String], pszThem);
	else
		snprintf(pszRa, nRa, "%s", [[ds objectAtIndex:0] UTF8String]);
	return pszRa;
}

extern "C" const char* JxIos_ThuMucTaiLieu(char* pszRa, size_t nRa)
{
	return JxIos_ThuMuc(NSDocumentDirectory, NULL, pszRa, nRa);
}

extern "C" const char* JxIos_ThuMucHoTro(char* pszRa, size_t nRa)
{
	return JxIos_ThuMuc(NSApplicationSupportDirectory, "jx1", pszRa, nRa);
}

extern "C" const char* JxIos_ThuMucGoi(char* pszRa, size_t nRa)
{
	NSString* p = [[NSBundle mainBundle] resourcePath];
	snprintf(pszRa, nRa, "%s", p ? [p UTF8String] : "");
	return pszRa;
}
