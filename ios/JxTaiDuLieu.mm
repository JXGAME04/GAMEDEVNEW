//---------------------------------------------------------------------------
// [IOS-TAI 11/09] Buoc F: bo tai du lieu NGAY TRONG APP cho iOS (va macOS).
//
// Dung DUNG giao thuc cua ban Android (android/may_chu_tai_du_lieu.py +
// TaiDuLieuActivity.java) de mot kho du lieu phuc vu duoc ca ba nen:
//
//   GET <goc>/manifest.txt   moi dong: "<co>\t<md5>\t<duong dan tuong doi>"
//   GET <goc>/<duong dan>    tep that, co ho tro HTTP Range de tai tiep
//
// Vi chi can tep tinh + Range nen dat o dau cung chay: may chu Python trong LAN,
// S3 / R2 / bat ky CDN nao. Khong can phan mem rieng phia may chu.
//
// Cach lam:
//   1. Tai manifest.txt.
//   2. So voi tep tren dia: dung co VA md5. md5 da kiem duoc ghi nho o da_tai.txt
//      ("<md5>\t<duong dan>") de lan sau khong phai bam lai ca bo du lieu.
//      Lan dau tien tren may da co san du lieu (chep tay) thi bam md5 mot luot -
//      co thanh tien trinh rieng, xong la khong bao gio bam lai.
//   3. Chi tai tep THIEU hoac KHAC. Tai vao "<ten>.part", xong moi doi ten -
//      dut mang giua chung thi lan sau gui Range: bytes=<da co>- de tai tiep.
//   4. Bao thu muc du lieu KHONG sao luu len iCloud (NSURLIsExcludedFromBackupKey).
//      Thieu buoc nay la Apple tu choi khi duyet: du lieu tai ve duoc khong duoc
//      chiem cho sao luu cua nguoi dung.
//
// Tep nay la ObjC++ THUAN: KHONG duoc include KWin32.h / KPosixCompat.h vi
// <Foundation/Foundation.h> cung dinh nghia BOOL -> dung dinh nghia (xem JxIosDuongDan.mm).
//---------------------------------------------------------------------------
#import <Foundation/Foundation.h>
#import <Security/Security.h>	// [BAOMAT 12/09 KY] kiem chu ky ban ke
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif
#include <CommonCrypto/CommonDigest.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

// ---------------------------------------------------------------- trang thai
// Luong tai chay o hang doi nen; luong chinh bom vong lap de ve giao dien.
// Chi trao doi qua may bien nguyen tu don gian nay.
@interface JxTaiTrangThai : NSObject
@property (atomic) long long coTong;      // tong so byte phai tai
@property (atomic) long long coDaTai;     // da tai duoc
@property (atomic) long long coDaBam;     // da bam md5 (buoc kiem lan dau)
@property (atomic) long long coCanBam;    // tong phai bam
@property (atomic, copy) NSString* mucHienTai;
@property (atomic, copy) NSString* loi;
@property (atomic) BOOL xong;
@property (atomic) BOOL huy;
@end
@implementation JxTaiTrangThai @end

// ---------------------------------------------------------------- chu ky ban ke
// [BAOMAT 12/09 KY] Bo tai kiem md5 tung tep THEO manifest.txt. Nhung ke dung giua sua duoc
// manifest thi sua luon md5 trong do -> kiem md5 thanh vo nghia. Cay du lieu co script Lua,
// ma Lua la MA CHAY THAT: doi mot tep Lua = dieu khien duoc may nguoi choi.
// Nen manifest phai co CHU KY, khoa cong khai nam san trong ung dung, khoa rieng giu o may chu.
// Ky bang: python3 android/ky_manifest.py --ky <thu muc> --khoa <khoa rieng>
static const unsigned char s_jxKhoaCongKhai[] = {
	0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02,
	0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03,
	0x42, 0x00, 0x04, 0xB6, 0xC0, 0xF7, 0x7F, 0xC9, 0x22, 0x5B, 0x8D, 0xBE,
	0x7C, 0xF9, 0x0A, 0xB7, 0x1E, 0xF1, 0x51, 0xF8, 0x36, 0x6F, 0x92, 0x76,
	0x06, 0x13, 0x18, 0x56, 0xD1, 0x56, 0xD9, 0xF9, 0xAD, 0xFD, 0x35, 0x89,
	0x51, 0xA3, 0x51, 0x70, 0x14, 0x1A, 0x45, 0xEB, 0xB6, 0x31, 0xCD, 0xB8,
	0x80, 0xA0, 0x28, 0xCD, 0xDE, 0x52, 0xE4, 0x04, 0x74, 0xAE, 0x0F, 0x43,
	0x8A, 0xEA, 0x94, 0xE3, 0x40, 0x61, 0x0B,
};

// Kiem chu ky DER cua ECDSA-SHA256 tren toan bo byte cua manifest.
static BOOL JxKiemChuKy(NSData* dManifest, NSData* dChuKy)
{
	if (!dManifest || !dChuKy || dChuKy.length == 0)
		return NO;
	NSDictionary* thuoc = @{ (id)kSecAttrKeyType      : (id)kSecAttrKeyTypeECSECPrimeRandom,
	                         (id)kSecAttrKeyClass     : (id)kSecAttrKeyClassPublic,
	                         (id)kSecAttrKeySizeInBits: @256 };
	// SecKeyCreateWithData muon diem tho (0x04||X||Y), khong phai SubjectPublicKeyInfo:
	// 91 byte DER o tren = 26 byte dau goi + 65 byte diem.
	if (sizeof(s_jxKhoaCongKhai) < 65)
		return NO;
	NSData* diem = [NSData dataWithBytes:(s_jxKhoaCongKhai + sizeof(s_jxKhoaCongKhai) - 65) length:65];
	CFErrorRef e = NULL;
	SecKeyRef khoa = SecKeyCreateWithData((__bridge CFDataRef)diem, (__bridge CFDictionaryRef)thuoc, &e);
	if (!khoa) { if (e) CFRelease(e); return NO; }
	BOOL ok = SecKeyVerifySignature(khoa, kSecKeyAlgorithmECDSASignatureMessageX962SHA256,
	                                (__bridge CFDataRef)dManifest, (__bridge CFDataRef)dChuKy, &e) ? YES : NO;
	if (e) CFRelease(e);
	CFRelease(khoa);
	return ok;
}

// ---------------------------------------------------------------- tien ich
static NSString* JxMd5Tep(NSString* p, JxTaiTrangThai* tt)
{
	FILE* f = fopen(p.fileSystemRepresentation, "rb");
	if (!f) return nil;
	CC_MD5_CTX c; CC_MD5_Init(&c);
	static const size_t nDem = 1 << 20;
	unsigned char* pDem = (unsigned char*)malloc(nDem);
	if (!pDem) { fclose(f); return nil; }
	size_t n;
	while ((n = fread(pDem, 1, nDem, f)) > 0)
	{
		CC_MD5_Update(&c, pDem, (CC_LONG)n);
		if (tt) tt.coDaBam = tt.coDaBam + (long long)n;
		if (tt && tt.huy) break;
	}
	free(pDem); fclose(f);
	unsigned char d[CC_MD5_DIGEST_LENGTH]; CC_MD5_Final(d, &c);
	NSMutableString* s = [NSMutableString stringWithCapacity:32];
	for (int i = 0; i < CC_MD5_DIGEST_LENGTH; i++) [s appendFormat:@"%02x", d[i]];
	return s;
}

static long long JxCoTep(NSString* p)
{
	struct stat st;
	if (stat(p.fileSystemRepresentation, &st) != 0) return -1;
	return (long long)st.st_size;
}

/** Ma hoa tung doan cua duong dan cho URL. Ten tep du lieu la GBK (byte cao) nen
    PHAI ma hoa theo BYTE tho, khong duoc giai ma ra chuoi roi ma hoa lai. */
static NSString* JxUrlHoaDuongDan(NSString* rel)
{
	NSMutableString* ra = [NSMutableString string];
	const char* p = rel.fileSystemRepresentation;
	for (const unsigned char* q = (const unsigned char*)p; *q; q++)
	{
		unsigned char c = *q;
		if (c == '/' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
			(c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~')
			[ra appendFormat:@"%c", c];
		else
			[ra appendFormat:@"%%%02X", c];
	}
	return ra;
}

static void JxKhongSaoLuu(NSString* p)
{
	NSURL* u = [NSURL fileURLWithPath:p];
	NSError* e = nil;
	[u setResourceValue:@YES forKey:NSURLIsExcludedFromBackupKey error:&e];
}

// ---------------------------------------------------------------- da_tai.txt
// "<md5>\t<duong dan>" moi dong: tep da tai VA da kiem md5 dung.
static NSMutableDictionary* JxDocDaTai(NSString* thuMuc)
{
	NSMutableDictionary* d = [NSMutableDictionary dictionary];
	NSString* p = [thuMuc stringByAppendingPathComponent:@"da_tai.txt"];
	NSString* s = [NSString stringWithContentsOfFile:p encoding:NSUTF8StringEncoding error:nil];
	for (NSString* ln in [(s ?: @"") componentsSeparatedByString:@"\n"])
	{
		NSArray* c = [ln componentsSeparatedByString:@"\t"];
		if (c.count >= 2) d[c[1]] = c[0];
	}
	return d;
}

static void JxGhiThemDaTai(NSString* thuMuc, NSString* md5, NSString* rel)
{
	NSString* p = [thuMuc stringByAppendingPathComponent:@"da_tai.txt"];
	FILE* f = fopen(p.fileSystemRepresentation, "ab");
	if (!f) return;
	fprintf(f, "%s\t%s\n", md5.UTF8String, rel.UTF8String);
	fclose(f);
}

// ---------------------------------------------------------------- tai mot tep
/** Tai <goc>/<rel> ve <thuMuc>/<rel>, tai tiep neu da co <rel>.part.
    Tra ve YES neu tep cuoi cung dung co va dung md5. */
static BOOL JxTaiMotTep(NSURLSession* ss, NSString* goc, NSString* thuMuc, NSString* rel,
                        long long co, NSString* md5, JxTaiTrangThai* tt)
{
	NSString* dich = [thuMuc stringByAppendingPathComponent:rel];
	NSString* tam  = [dich stringByAppendingString:@".part"];
	[[NSFileManager defaultManager] createDirectoryAtPath:[dich stringByDeletingLastPathComponent]
	                         withIntermediateDirectories:YES attributes:nil error:nil];

	// [IOS-TAI-CHAC 15/09] Dia chi phai hop le TRUOC vong lap: goc doc tu tep nguoi dung sua duoc,
	// ma requestWithURL:nil NEM NGOAI LE -> sap app chu khong ra man bao loi.
	NSString* su = [NSString stringWithFormat:@"%@%@", goc, JxUrlHoaDuongDan(rel)];
	NSURL* dc = [NSURL URLWithString:su];
	if (!dc) { tt.loi = [NSString stringWithFormat:@"%@: dia chi khong hop le (%@)", rel, su]; return NO; }

	long long daCo = JxCoTep(tam);
	if (daCo < 0) daCo = 0;
	if (daCo > co) { unlink(tam.fileSystemRepresentation); daCo = 0; }	// .part hong

	// [IOS-TAI-CHAC 15/09] Dem so lan hong de KHONG quay vo han. Truoc day vong nay chi thoat khi
	// daCo >= co, ma moi vong "co them byte" la tinh la tien bo - nen mot cong dang nhap Wi-Fi cong
	// cong (tra 200 kem trang HTML cho MOI yeu cau) se lam no quay hang tram nghin lan, ghi day rac
	// vao may, thanh tien do van chay muot nhu that.
	int soLoiMang = 0, soLamLai = 0, soKhongTien = 0, nNghi = 1;

	while (daCo < co && !tt.huy)
	{
		NSMutableURLRequest* rq = [NSMutableURLRequest requestWithURL:dc
		                                                 cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
		                                             timeoutInterval:30.0];
		const long long daCoLucGui = daCo;	// chup lai de doi chieu voi Content-Range
		if (daCo > 0)
			[rq setValue:[NSString stringWithFormat:@"bytes=%lld-", daCo] forHTTPHeaderField:@"Range"];

		__block NSHTTPURLResponse* hp = nil; __block NSError* le = nil;
		__block BOOL bDayDia = NO, bLamLai = NO, bRangeSai = NO;
		dispatch_semaphore_t sem = dispatch_semaphore_create(0);
		// Dung downloadTask: no ghi thang ra tep tam cua he thong nen tep pak vai tram MB
		// khong phai nam het trong bo nho.
		NSURLSessionDownloadTask* t = [ss downloadTaskWithRequest:rq
			completionHandler:^(NSURL* vt, NSURLResponse* rp, NSError* er) {
				hp = [rp isKindOfClass:[NSHTTPURLResponse class]] ? (NSHTTPURLResponse*)rp : nil;
				le = er;
				// [IOS-TAI-CHAC 15/09] KIEM MA TRA VE TRUOC KHI GHI. downloadTask giao tep tam voi
				// BAT KY ma nao - 404, 500, trang cong dang nhap - vi loi HTTP khong phai NSError.
				// Truoc day khoi ghi nam trong "if (vt)" chay vo dieu kien, nen than trang loi bi noi
				// thang vao .part roi con nam lai do sang lan mo app sau.
				BOOL bGhi = NO;
				if (!er && hp && vt)
				{
					long st = hp.statusCode;
					if (st == 206)
					{	// Phai dung khuc ta xin. May chu noi doi -> noi vao la hong am tham.
						long long a = -1, b = -1, tong = -1;
						NSString* cr = [hp valueForHTTPHeaderField:@"Content-Range"];
						int k = cr ? sscanf(cr.UTF8String, "bytes %lld-%lld/%lld", &a, &b, &tong) : 0;
						if (k >= 2 && a == daCoLucGui && (k < 3 || tong == co)) bGhi = YES;
						else bRangeSai = YES;
					}
					else if (st == 200)
					{	// 200 khi dang xin khuc = may chu bo qua Range, dang gui tu byte 0.
						if (daCoLucGui == 0) bGhi = YES;
						else                 bLamLai = YES;
					}
					// 4xx/5xx: khong ghi gi ca, .part giu nguyen
				}
				if (bGhi)
				{	// noi vao duoi .part
					FILE* fo = fopen(tam.fileSystemRepresentation, "ab");
					FILE* fi = fopen(vt.path.fileSystemRepresentation, "rb");
					if (fo && fi)
					{
						static const size_t nD = 1 << 20;
						unsigned char* pD = (unsigned char*)malloc(nD);
						size_t n;
						while (pD && (n = fread(pD, 1, nD, fi)) > 0)
						{
							size_t w = fwrite(pD, 1, n, fo);
							tt.coDaTai = tt.coDaTai + (long long)w;	// chi dem byte GHI DUOC that
							if (w != n) { bDayDia = YES; break; }
						}
						free(pD);
					}
					else bDayDia = (fo == NULL);
					if (fi) fclose(fi);
					// het cho trong thuong lo ra o fflush/fclose chu khong o fwrite
					if (fo) { if (fflush(fo) != 0) bDayDia = YES; if (fclose(fo) != 0) bDayDia = YES; }
				}
				dispatch_semaphore_signal(sem);
			}];
		[t resume];
		dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);

		// [IOS-TAI-CHAC 15/09] Phep tac xu ly loi: byte tren dia SAI thi xoa, chi THIEU thi giu.
		// Dut mang chi lam cut tep chu khong lam hong, nen xoa .part luc do la tu bat nguoi choi
		// tai lai tu dau ca tram MB - dung cai hai ma .part sinh ra de tranh.
		if (bDayDia)
		{	// giu .part: byte da ghi van dung, chi la chua du
			tt.loi = [NSString stringWithFormat:@"%@: may het cho trong", rel];
			return NO;
		}
		if (bLamLai || bRangeSai)
		{	// byte cu khong noi tiep duoc voi byte moi -> cat ve 0 va tai lai TEP NAY tu dau
			if (++soLamLai > 2)
			{
				unlink(tam.fileSystemRepresentation);
				tt.loi = [NSString stringWithFormat:@"%@: may chu khong theo dung Range", rel];
				return NO;
			}
			FILE* fx = fopen(tam.fileSystemRepresentation, "wb"); if (fx) fclose(fx);
			if (daCo > 0) tt.coDaTai = tt.coDaTai - daCo;	// tra lai thanh tien do cho khoi dem trung
			daCo = 0;
			continue;
		}
		if (le || !hp)
		{	// loi truyen tai, hoac phan hoi khong phai HTTP. Giu .part, cho roi thu lai.
			// hp rong TRUOC day loc qua duoc "if (hp && ...)" -> nhan bua phan hoi la
			if (++soLoiMang > 4)
			{
				tt.loi = le ? [NSString stringWithFormat:@"%@: %@", rel, le.localizedDescription]
				            : [NSString stringWithFormat:@"%@: phan hoi khong hop le", rel];
				return NO;
			}
			for (int i = 0; i < nNghi * 10 && !tt.huy; i++) usleep(100000);
			if (nNghi < 16) nNghi *= 2;
			continue;
		}
		if (hp.statusCode != 200 && hp.statusCode != 206)
		{	// giu .part: 404 luc dang rai ban moi, 403 het han chu ky... khong noi gi ve byte da tai
			tt.loi = [NSString stringWithFormat:@"%@: may chu tra ve %ld", rel, (long)hp.statusCode];
			return NO;
		}

		long long mm = JxCoTep(tam);
		if (mm <= daCo)
		{	// than rong, hoac ghi khong duoc. Giu .part nhung KHONG duoc quay mai.
			if (++soKhongTien >= 3)
			{
				tt.loi = [NSString stringWithFormat:@"%@: khong tai them duoc byte nao", rel];
				return NO;
			}
			for (int i = 0; i < nNghi * 10 && !tt.huy; i++) usleep(100000);
			if (nNghi < 16) nNghi *= 2;
			continue;
		}
		soLoiMang = 0; soKhongTien = 0; nNghi = 1;	// co tien bo that -> dat lai bo dem
		daCo = mm;
	}
	if (tt.huy) return NO;

	if (JxCoTep(tam) != co)
	{
		tt.loi = [NSString stringWithFormat:@"%@: co %lld, manifest ghi %lld", rel, JxCoTep(tam), co];
		unlink(tam.fileSystemRepresentation);
		return NO;
	}
	// [IOS-TAI-CHAC 15/09] md5 rong = KHONG kiem. Truoc day dieu kien la "md5.length && ..." nen mot
	// dong manifest thieu cot md5 se cho tep di thang vao may ma khong ai bam, roi dong 'da tai' ghi
	// chuoi rong lam da kiem -> khong bao gio bam lai nua. Phai chan han, khong duoc im lang bo qua.
	if (!md5.length)
	{
		tt.loi = [NSString stringWithFormat:@"%@: manifest thieu md5 cho tep nay", rel];
		unlink(tam.fileSystemRepresentation);
		return NO;
	}
	NSString* m = JxMd5Tep(tam, nil);
	if (![m isEqualToString:md5])
	{
		tt.loi = [NSString stringWithFormat:@"%@: md5 sai (%@ != %@)", rel, m, md5];
		unlink(tam.fileSystemRepresentation);
		return NO;
	}
	unlink(dich.fileSystemRepresentation);
	if (rename(tam.fileSystemRepresentation, dich.fileSystemRepresentation) != 0)
	{
		tt.loi = [NSString stringWithFormat:@"%@: doi ten that bai", rel];
		return NO;
	}
	JxGhiThemDaTai(thuMuc, m ?: @"", rel);
	return YES;
}

// ---------------------------------------------------------------- viec chinh
typedef struct { long long co; char md5[36]; } JxMuc;

// Tai mot dia chi ve bo nho, cho xong moi tra ve (chay o hang doi nen nen chan duoc).
static NSData* JxTaiDongBo(NSURLSession* ss, NSString* su)
{
	__block NSData* ra = nil;
	dispatch_semaphore_t sem = dispatch_semaphore_create(0);
	[[ss dataTaskWithURL:[NSURL URLWithString:su]
	   completionHandler:^(NSData* d, NSURLResponse* r, NSError* e) {
			NSHTTPURLResponse* hp = [r isKindOfClass:[NSHTTPURLResponse class]] ? (NSHTTPURLResponse*)r : nil;
			if (!e && d && (!hp || hp.statusCode == 200)) ra = d;
			dispatch_semaphore_signal(sem); }] resume];
	dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
	return ra;
}

static void JxLamViec(NSString* goc, NSString* thuMuc, JxTaiTrangThai* tt)
{
	NSURLSessionConfiguration* cf = [NSURLSessionConfiguration ephemeralSessionConfiguration];
	cf.timeoutIntervalForRequest  = 30.0;
	cf.timeoutIntervalForResource = 3600.0;
	NSURLSession* ss = [NSURLSession sessionWithConfiguration:cf];

	// --- 1. manifest ---
	tt.mucHienTai = @"Dang lay danh sach tep…";
	NSData* dm = JxTaiDongBo(ss, [goc stringByAppendingString:@"manifest.txt"]);
	if (!dm)
	{
		tt.loi = [NSString stringWithFormat:@"Khong lay duoc manifest.txt tu %@", goc];
		tt.xong = YES; return;
	}
	// [BAOMAT 12/09 KY] Bat buoc co chu ky hop le. Khong co chu ky, hoac chu ky sai, la DUNG NGAY:
	// khong doc lay mot dong nao cua manifest. Manifest sai = tai ve tep bi doi ruot, ma trong do
	// co script Lua chay that tren may nguoi choi.
	{
		tt.mucHienTai = @"Dang kiem chu ky danh sach tep…";
		NSData* dSigB64 = JxTaiDongBo(ss, [goc stringByAppendingString:@"manifest.sig"]);
		NSData* dSig = nil;
		if (dSigB64)
		{
			NSString* s = [[NSString alloc] initWithData:dSigB64 encoding:NSASCIIStringEncoding];
			s = [s stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
			dSig = s.length ? [[NSData alloc] initWithBase64EncodedString:s options:0] : nil;
		}
		if (!JxKiemChuKy(dm, dSig))
		{
			tt.loi = [NSString stringWithFormat:
				@"Danh sach tep KHONG co chu ky hop le.\n\nDa dung tai de an toan.\n"
				 "Kho du lieu phai co manifest.sig ky bang khoa cua may chu:\n"
				 "  python3 android/ky_manifest.py --ky <thu muc> --khoa <khoa rieng>"];
			tt.xong = YES; return;
		}
		NSLog(@"[IOS-TAI] chu ky danh sach tep: HOP LE");
	}

	NSString* sm = [[NSString alloc] initWithData:dm encoding:NSUTF8StringEncoding];
	if (!sm) sm = [[NSString alloc] initWithData:dm encoding:NSISOLatin1StringEncoding];
	NSMutableArray<NSString*>* duong = [NSMutableArray array];
	NSMutableArray<NSNumber*>*  cos  = [NSMutableArray array];
	NSMutableArray<NSString*>*  md5s = [NSMutableArray array];
	for (NSString* ln in [sm componentsSeparatedByString:@"\n"])
	{
		NSArray* c = [ln componentsSeparatedByString:@"\t"];
		if (c.count < 3) continue;
		NSString* rel = [[c[2] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]
		                 stringByReplacingOccurrencesOfString:@"\\" withString:@"/"];
		if (!rel.length) continue;
		// [IOS-TAI-CHAC 15/09] Loc duong dan. Ban Android da loc tu lau (TaiDuLieuActivity.java:340-341),
		// ban iOS bo sot: stringByAppendingPathComponent KHONG bo ".." , ma JxTaiMotTep con tu tao cac
		// thu muc trung gian - nen mot dong manifest "../../Library/..." se ghi ra NGOAI thu muc du lieu.
		// Chi ke giu khoa ky moi lam duoc, nhung khoa thi co ngay lo; chan o day la chan them mot lop.
		while ([rel hasPrefix:@"/"]) rel = [rel substringFromIndex:1];
		if (!rel.length) continue;
		if ([rel isEqualToString:@".."] || [rel hasPrefix:@"../"] ||
		    [rel hasSuffix:@"/.."]     || [rel containsString:@"/../"])
		{	// Chat hon ban Android (ben do chi bo qua dong): manifest DA KY thi khong duoc phep co
			// duong dan kieu nay - co la dau hieu hong that, phai dung lai cho nguoi ta thay.
			tt.loi = [NSString stringWithFormat:@"manifest co duong dan khong hop le: %@", rel];
			tt.xong = YES; return;
		}
		[duong addObject:rel];
		[cos   addObject:@([c[0] longLongValue])];
		[md5s  addObject:[c[1] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]];
	}
	if (duong.count == 0) { tt.loi = @"manifest.txt rong"; tt.xong = YES; return; }

	// --- 2. so voi tep tren dia; bam md5 nhung tep dung co ma chua ghi trong da_tai.txt ---
	NSMutableDictionary* daTai = JxDocDaTai(thuMuc);
	NSMutableArray<NSNumber*>* canTai = [NSMutableArray array];

	// vong 1: tim tep can bam md5 (co dung nhung chua co trong da_tai.txt)
	NSMutableArray<NSNumber*>* canBam = [NSMutableArray array];
	long long tongBam = 0;
	for (NSUInteger i = 0; i < duong.count; i++)
	{
		NSString* rel = duong[i];
		long long co  = cos[i].longLongValue;
		NSString* dich = [thuMuc stringByAppendingPathComponent:rel];
		long long cd = JxCoTep(dich);
		if (cd != co) continue;                       // thieu hoac khac co -> tai, khong can bam
		if ([daTai[rel] isEqualToString:md5s[i]]) continue;   // da kiem roi
		[canBam addObject:@(i)]; tongBam += co;
	}
	tt.coCanBam = tongBam;
	if (tongBam > 0)
	{
		tt.mucHienTai = @"Kiem tra du lieu da co (chi lam mot lan)…";
		for (NSNumber* k in canBam)
		{
			if (tt.huy) { tt.xong = YES; return; }
			NSUInteger i = k.unsignedIntegerValue;
			NSString* dich = [thuMuc stringByAppendingPathComponent:duong[i]];
			NSString* m = JxMd5Tep(dich, tt);
			if (m && [m isEqualToString:md5s[i]])
			{
				JxGhiThemDaTai(thuMuc, m, duong[i]);
				daTai[duong[i]] = m;
			}
		}
	}

	// vong 2: chot danh sach phai tai
	long long tongTai = 0;
	for (NSUInteger i = 0; i < duong.count; i++)
	{
		NSString* rel = duong[i];
		long long co  = cos[i].longLongValue;
		NSString* dich = [thuMuc stringByAppendingPathComponent:rel];
		if (JxCoTep(dich) == co && [daTai[rel] isEqualToString:md5s[i]]) continue;
		[canTai addObject:@(i)];
		long long dap = JxCoTep([dich stringByAppendingString:@".part"]);
		tongTai += co - (dap > 0 && dap < co ? dap : 0);
	}
	tt.coTong = tongTai;
	if (canTai.count == 0) { tt.mucHienTai = @"Du lieu da day du"; tt.xong = YES; return; }

	// --- 3. tai ---
	for (NSNumber* k in canTai)
	{
		if (tt.huy) break;
		NSUInteger i = k.unsignedIntegerValue;
		tt.mucHienTai = duong[i];
		if (!JxTaiMotTep(ss, goc, thuMuc, duong[i], cos[i].longLongValue, md5s[i], tt))
			break;	// tt.loi da duoc dat
	}
	tt.xong = YES;
}

// ---------------------------------------------------------------- giao dien
#if TARGET_OS_IPHONE
@interface JxTaiMan : UIViewController
@property (nonatomic, strong) UILabel* nhan;
@property (nonatomic, strong) UILabel* chiTiet;
@property (nonatomic, strong) UIProgressView* thanh;
// [IOS-KHONGCHET 15/09] nut duoi man tai: luc tai thi an, luc hong thi hien "Thử lại".
// daBam la atomic vi vong bom o luong chinh doc no, con UIKit dat no trong ham bam.
@property (nonatomic, strong) UIButton* nut;
@property (atomic) BOOL daBam;
@end
@implementation JxTaiMan
- (void)viewDidLoad
{
	[super viewDidLoad];
	self.view.backgroundColor = [UIColor blackColor];
	CGFloat w = self.view.bounds.size.width, h = self.view.bounds.size.height;
	self.nhan = [[UILabel alloc] initWithFrame:CGRectMake(20, h/2 - 70, w - 40, 26)];
	self.nhan.textColor = [UIColor whiteColor];
	self.nhan.textAlignment = NSTextAlignmentCenter;
	self.nhan.font = [UIFont systemFontOfSize:17 weight:UIFontWeightSemibold];
	self.nhan.text = @"Chuẩn bị dữ liệu";
	[self.view addSubview:self.nhan];

	self.thanh = [[UIProgressView alloc] initWithFrame:CGRectMake(30, h/2 - 20, w - 60, 4)];
	[self.view addSubview:self.thanh];

	self.chiTiet = [[UILabel alloc] initWithFrame:CGRectMake(20, h/2 + 2, w - 40, 60)];
	self.chiTiet.textColor = [UIColor colorWithWhite:0.75 alpha:1.0];
	self.chiTiet.textAlignment = NSTextAlignmentCenter;
	self.chiTiet.numberOfLines = 3;
	self.chiTiet.font = [UIFont systemFontOfSize:13];
	[self.view addSubview:self.chiTiet];

	// [IOS-KHONGCHET 15/09] Nut nay PHAI bam trung, nen dat bang rang buoc chu KHONG bang toa do cung
	// nhu ba muc tren: cua so duoc tao theo [UIScreen mainScreen].bounds (luc khoi dong la chieu DOC)
	// trong khi Info.plist khoa game nam NGANG -> toa do cung tinh mot lan trong viewDidLoad se lech.
	self.nut = [UIButton buttonWithType:UIButtonTypeSystem];
	self.nut.translatesAutoresizingMaskIntoConstraints = NO;
	self.nut.titleLabel.font = [UIFont systemFontOfSize:16 weight:UIFontWeightSemibold];
	self.nut.backgroundColor = [UIColor colorWithWhite:0.22 alpha:1.0];
	[self.nut setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
	self.nut.layer.cornerRadius = 8;
	self.nut.hidden = YES;
	[self.nut addTarget:self action:@selector(bamNut) forControlEvents:UIControlEventTouchUpInside];
	[self.view addSubview:self.nut];
	[NSLayoutConstraint activateConstraints:@[
		[self.nut.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor],
		[self.nut.bottomAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.bottomAnchor constant:-28],
		[self.nut.widthAnchor  constraintGreaterThanOrEqualToConstant:200],
		[self.nut.heightAnchor constraintEqualToConstant:46],
	]];
}

- (void)bamNut
{
	self.daBam = YES;
}

// Chuyen man tai sang trang thai BAO LOI: giau thanh tien do, hien loi va nut.
- (void)hienLoi:(NSString*)loi nut:(NSString*)tenNut
{
	self.nhan.text = @"Không tải được dữ liệu";
	self.thanh.hidden = YES;
	self.chiTiet.numberOfLines = 4;
	self.chiTiet.text = loi ?: @"";
	[self.nut setTitle:tenNut forState:UIControlStateNormal];
	self.daBam = NO;
	self.nut.hidden = NO;
}

// Ve lai trang thai dang tai (goi truoc moi lan thu lai).
- (void)veLaiDangTai
{
	self.nut.hidden = YES;
	self.thanh.hidden = NO;
	self.thanh.progress = 0;
	self.chiTiet.numberOfLines = 3;
	self.nhan.text = @"Chuẩn bị dữ liệu";
	self.chiTiet.text = @"";
}
@end
#endif

static NSString* JxMb(long long n) { return [NSString stringWithFormat:@"%.1f", (double)n / (1024.0*1024.0)]; }

// ---------------------------------------------------------------- cong phien ban
// [BAOMAT 12/09 PHIENBAN] Kho du lieu mang mot tep "phienban.txt", dong dau la SO PHIEN BAN
// CLIENT TOI THIEU (so nguyen, dang ngay 20260912). App cu hon so do thi khong cho choi tiep.
//
// Vi sao dat o day ma khong dat trong goi dang nhap: bat truong ProtocolVersion trong goi
// c2s_login la doi GIAO THUC BAT TAY, phai sua ca may chu cung luc, khong lam mot ben duoc.
// Con tep nay thi da nam trong manifest.txt (co md5) ma manifest thi DA KY, nen ke dung giua
// khong ha nguong xuong duoc - muon ep cap nhat chi can sua tep roi ky lai.
//
// LUU Y THAT: day la cong cho nguoi choi NGAY TINH. Client bi sua ruot thi bo qua duoc buoc nay.
// Cong THAT phai nam o may chu (tu choi client qua cu luc dang nhap) - viec do con lai.
extern "C" int JxTaiDuLieu_KiemPhienBan(const char* pszThuMuc, int nPhienBanApp, char* pszLoi, int nLoi)
{
	@autoreleasepool {
		if (!pszThuMuc || !*pszThuMuc) return 0;
		NSString* p = [[NSString stringWithUTF8String:pszThuMuc] stringByAppendingPathComponent:@"phienban.txt"];
		NSString* s = [NSString stringWithContentsOfFile:p encoding:NSUTF8StringEncoding error:nil];
		if (!s.length)
			return 0;	// kho chua khai bao gi -> khong chan (de kho cu van chay duoc)
		int nCan = [[[s componentsSeparatedByString:@"\n"] firstObject] intValue];
		if (nCan <= 0 || nPhienBanApp >= nCan)
			return 0;
		if (pszLoi && nLoi)
			snprintf(pszLoi, nLoi,
				"Ban game nay da cu.\n\nBan dang dung %d, may chu yeu cau tu %d tro len.\n"
				"Hay cai ban moi roi vao lai.", nPhienBanApp, nCan);
		NSLog(@"[PHIENBAN] app %d < yeu cau %d -> chan", nPhienBanApp, nCan);
		return 1;
	}
}

// ---------------------------------------------------------------- API cho C++
// Tra ve 0 neu du lieu da san sang, khac 0 neu that bai (tt.loi mo ta).
extern "C" int JxTaiDuLieu_Chay(const char* pszThuMuc, const char* pszGoc, char* pszLoi, int nLoi)
{
	@autoreleasepool {
		NSString* thuMuc = [NSString stringWithUTF8String:pszThuMuc ? pszThuMuc : ""];
		NSString* goc    = [NSString stringWithUTF8String:pszGoc ? pszGoc : ""];
		if (![goc hasSuffix:@"/"]) goc = [goc stringByAppendingString:@"/"];
		if (!thuMuc.length || !goc.length) { if (pszLoi && nLoi) snprintf(pszLoi, nLoi, "thieu tham so"); return 2; }

		[[NSFileManager defaultManager] createDirectoryAtPath:thuMuc withIntermediateDirectories:YES attributes:nil error:nil];
		JxKhongSaoLuu(thuMuc);

#if TARGET_OS_IPHONE
		// [IOS-KHONGCHET 15/09] Cua so tao MOT lan, dung cho ca cac lan thu lai. Truoc day no bi
		// an di ngay truoc nhanh loi, ma no la tham chieu manh duy nhat -> giai phong luon man hinh.
		UIWindow* cua = nil; JxTaiMan* man = nil;
		if ([UIApplication sharedApplication])
		{
			cua = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
			man = [[JxTaiMan alloc] init];
			cua.rootViewController = man;
			cua.windowLevel = UIWindowLevelAlert + 1;
			[cua makeKeyAndVisible];
			// Tai 8,5 GB mat hang gio; khong chan thi may tu khoa man hinh giua chung roi dut tai.
			// Ban Android da lam viec nay tu lau (FLAG_KEEP_SCREEN_ON trong TaiDuLieuActivity).
			[UIApplication sharedApplication].idleTimerDisabled = YES;
		}
#endif

		// [IOS-KHONGCHET 15/09] Vong THU LAI. Truoc day tai hong mot lan la ham tra ve khac 0,
		// ben goi dung SDL_ShowSimpleMessageBox roi "return 1" - ma SDL3 da vo hieu exit() nen
		// tien trinh con song khong cua so = man hinh den vinh vien (Apple coi la treo, dieu 2.1).
		// Nay hong thi bao loi ngay tren man tai va cho bam "Thử lại"; bo tai von da noi tiep duoc
		// nho tep .part nen thu lai khong mat phan da tai.
		int nKetQua = 0;
		for (;;)
		{
			// Moi lan thu dung mot trang thai MOI: cac bien dem (coTong/coDaTai/coDaBam) phai ve 0,
			// va khoi tao lai thi khong the dinh dang voi luong nen cua lan truoc.
			JxTaiTrangThai* tt = [[JxTaiTrangThai alloc] init];
			tt.mucHienTai = @"Dang ket noi…";
			dispatch_async(dispatch_get_global_queue(QOS_CLASS_UTILITY, 0), ^{ JxLamViec(goc, thuMuc, tt); });

			NSDate* batDau = [NSDate date];
			while (!tt.xong)
			{
				@autoreleasepool {
					[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
#if TARGET_OS_IPHONE
					if (man)
					{
						long long canBam = tt.coCanBam, daBam = tt.coDaBam;
						long long tong = tt.coTong, da = tt.coDaTai;
						if (canBam > 0 && daBam < canBam)
						{
							man.nhan.text = @"Kiểm tra dữ liệu đã có";
							man.thanh.progress = (float)((double)daBam / (double)canBam);
							man.chiTiet.text = [NSString stringWithFormat:@"%@ / %@ MB", JxMb(daBam), JxMb(canBam)];
						}
						else if (tong > 0)
						{
							double gy = -[batDau timeIntervalSinceNow];
							double td = gy > 0.5 ? (double)da / gy : 0;
							man.nhan.text = @"Đang tải dữ liệu";
							man.thanh.progress = (float)((double)da / (double)tong);
							man.chiTiet.text = [NSString stringWithFormat:@"%@ / %@ MB\n%.1f MB/s\n%@",
							                    JxMb(da), JxMb(tong), td / (1024.0*1024.0), tt.mucHienTai ?: @""];
						}
						else
							man.chiTiet.text = tt.mucHienTai ?: @"";
					}
#endif
				}
			}

			if (!tt.loi.length)			// xong xuoi
				break;

			if (pszLoi && nLoi) snprintf(pszLoi, nLoi, "%s", tt.loi.UTF8String);
#if TARGET_OS_IPHONE
			if (man)
			{	// Co man hinh -> KHONG bao gio tra ve loi; cho nguoi dung bam thu lai.
				[man hienLoi:tt.loi nut:@"Thử lại"];
				while (!man.daBam)
				{
					@autoreleasepool {
						[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
					}
				}
				[man veLaiDangTai];
				continue;
			}
#endif
			nKetQua = 1;	// ban macOS / khong co giao dien: giu nguyen hanh vi cu, ben goi tu xu ly
			break;
		}

#if TARGET_OS_IPHONE
		if (cua)
		{
			[UIApplication sharedApplication].idleTimerDisabled = NO;
			cua.hidden = YES; cua = nil;
		}
#endif
		return nKetQua;
	}
}

// ---------------------------------------------------------------- man bao loi CHAN
// [IOS-KHONGCHET 15/09] Man bao loi cho cac hong hoc chi mang tinh KHOI DONG (khong co du lieu,
// tao tang ve that bai...). KHONG ket thuc tien trinh: iOS khong cho app tu dong, va SDL3 da vo hieu
// exit() (SDL_uikitappdelegate.m, dong "// exit(exit_status);") nen "return" tu main() chi de lai
// tien trinh song khong cua so = man hinh den.
//   pszNut = NULL/rong -> chan mai mai (khong co gi de thu lai), chi hien thong bao.
//   Tra ve 0 khi nguoi dung bam nut; -1 khi khong dung duoc giao dien (macOS, hoac chua co UIApplication).
extern "C" int JxIosManLoi(const char* pszTieuDe, const char* pszNoiDung, const char* pszNut)
{
#if TARGET_OS_IPHONE
	@autoreleasepool {
		if (![UIApplication sharedApplication])
			return -1;
		UIWindow* cua = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
		JxTaiMan* man = [[JxTaiMan alloc] init];
		cua.rootViewController = man;
		cua.windowLevel = UIWindowLevelAlert + 1;
		[cua makeKeyAndVisible];

		[man hienLoi:[NSString stringWithUTF8String:pszNoiDung ? pszNoiDung : ""]
		         nut:[NSString stringWithUTF8String:(pszNut && *pszNut) ? pszNut : ""]];
		if (pszTieuDe && *pszTieuDe)
			man.nhan.text = [NSString stringWithUTF8String:pszTieuDe];
		if (!(pszNut && *pszNut))
			man.nut.hidden = YES;	// khong co gi de thu lai -> chan, nhung van hien thong bao

		while (!man.daBam)
		{
			@autoreleasepool {
				[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
			}
		}
		cua.hidden = YES; cua = nil;
		return 0;
	}
#else
	(void)pszTieuDe; (void)pszNoiDung; (void)pszNut;
	return -1;
#endif
}
