//---------------------------------------------------------------------------
// [BAOMAT 12/09 KHOMAT] Kho luu bi mat cua he dieu hanh (Keychain) cho iOS va macOS.
//
// VI SAO: tai khoan va mat khau dang duoc ghi vao <du lieu>/userdata/uicommon.ini muc [Login].
// Tep do nam trong thu muc du lieu, nguoi dung mo ra doc duoc, va no di theo moi ban sao luu.
// Noi dung tuy da qua EDOneTimePad nhung day chi la che mat, khong phai ma hoa co khoa:
// ai co tep la doc lai duoc. Keychain thi khoa nam trong phan cung may, app khac khong doc duoc,
// va co the chan khong cho di theo ban sao luu.
//
// kSecAttrAccessibleAfterFirstUnlockThisDeviceOnly:
//   - AfterFirstUnlock: doc duoc sau lan mo khoa dau tien, de game vao lai duoc khi may dang khoa.
//   - ThisDeviceOnly : KHONG di theo ban sao luu / khong sang may khac. Doi may thi nhap lai,
//     doi lai la mat khau khong ro ri qua ban sao luu iCloud.
//
// Tep nay la ObjC++ THUAN: khong include header nao cua game (va cham kieu BOOL - xem JxIosDuongDan.mm).
//---------------------------------------------------------------------------
#import <Foundation/Foundation.h>
#import <Security/Security.h>

#define JX_DICH_VU	"vn.jx1.taikhoan"

static NSDictionary* JxTruyVan(const char* pszKhoa)
{
	return @{ (id)kSecClass       : (id)kSecClassGenericPassword,
	          (id)kSecAttrService : @JX_DICH_VU,
	          (id)kSecAttrAccount : [NSString stringWithUTF8String:(pszKhoa ? pszKhoa : "")] };
}

// Luu nDai byte. Tra 1 neu duoc.
extern "C" int JxKhoMat_Luu(const char* pszKhoa, const void* pDuLieu, int nDai)
{
	if (!pszKhoa || !pDuLieu || nDai <= 0) return 0;
	@autoreleasepool {
		NSMutableDictionary* q = [JxTruyVan(pszKhoa) mutableCopy];
		SecItemDelete((__bridge CFDictionaryRef)q);	// ghi de = xoa roi them
		q[(id)kSecValueData]       = [NSData dataWithBytes:pDuLieu length:(NSUInteger)nDai];
		q[(id)kSecAttrAccessible]  = (id)kSecAttrAccessibleAfterFirstUnlockThisDeviceOnly;
		OSStatus st = SecItemAdd((__bridge CFDictionaryRef)q, NULL);
		if (st != errSecSuccess) NSLog(@"[KHOMAT] luu '%s' that bai: %d", pszKhoa, (int)st);
		return st == errSecSuccess ? 1 : 0;
	}
}

// Doc toi da nRa byte vao pRa. Tra so byte doc duoc, 0 neu khong co.
extern "C" int JxKhoMat_Doc(const char* pszKhoa, void* pRa, int nRa)
{
	if (!pszKhoa || !pRa || nRa <= 0) return 0;
	@autoreleasepool {
		NSMutableDictionary* q = [JxTruyVan(pszKhoa) mutableCopy];
		q[(id)kSecReturnData] = @YES;
		q[(id)kSecMatchLimit] = (id)kSecMatchLimitOne;
		CFTypeRef ra = NULL;
		if (SecItemCopyMatching((__bridge CFDictionaryRef)q, &ra) != errSecSuccess || !ra)
			return 0;
		NSData* d = (__bridge_transfer NSData*)ra;
		int n = (int)d.length; if (n > nRa) n = nRa;
		memcpy(pRa, d.bytes, (size_t)n);
		return n;
	}
}

extern "C" void JxKhoMat_Xoa(const char* pszKhoa)
{
	if (!pszKhoa) return;
	@autoreleasepool { SecItemDelete((__bridge CFDictionaryRef)JxTruyVan(pszKhoa)); }
}
