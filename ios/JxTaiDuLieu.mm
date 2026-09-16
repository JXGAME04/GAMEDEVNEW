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
//   [IOS-DAKIEM 16/09] - thiet ke THIETKE_CAPNHAT_IOS_MAC_1509.md §3-§4, da qua 4 phan bien 16/09:
//   5. Giu lai manifest DA KIEM CHU KY o <Documents>/manifest_dakiem.txt (dong dau = chu ky base64, sau do la
//      manifest.txt nguyen byte; MOT tep de rename nguyen khoi). Day la "dau hoan tat": mat mang van biet
//      du lieu trong may co du theo manifest da biet hay khong.
//   6. Moi lan mo: tai manifest online (thoi han 8 s neu du lieu da du theo manifest da luu, 30 s neu chua).
//      Lay duoc + chu ky dung -> luu de, dong bo nhu thuong (B: tai chenh lech, khong nut bo qua; C: vao game).
//      Khong lay duoc / chu ky sai (F) -> dung manifest da luu: du -> D (vao game, bao nhe "khong kiem tra
//      duoc cap nhat"); thieu -> E (bao ro ly do mang, nut Thu lai). Khong co manifest da luu -> E.
//      Moi that bai SAU khi da co manifest hop le (404 tep, md5 sai, het cho) deu la E - khong loi vao game.
//   7. phienban.txt (co md5 trong manifest da ky) duoc tai va kiem TRUOC khi tai hang GB (A: chan han).
//   8. Dieu 4.2.3(ii): tu 50 MB thi noi dung luong va cho bam "Tai xuong" truoc khi tai (mot nut, khong bo qua).
//   Khong HMAC/Keychain: ECDSA tren manifest da luu da tra loi "manifest that" - KHONG phai "moi nhat" (chu ky
//   khong co dau thoi gian, mot cap cu hop le vinh vien); chuyen do chi may chu game chan duoc luc dang nhap.
//   KHONG "chi gieo lan dau" config.ini: game tren mobile khong ghi config.ini (moi thiet dat vao userdata/, may chu
//   da loai), tru hai ham Lua thoi PC Set3D/SetFullWindow; giu dong bo de chu con day duoc config.ini xuong may.
//
// [16/09 MRC] Tep nay dich KHONG ARC (xem JxIosNhatKy.mm). Luat: doi tuong nhan trong block cua NSURLSession phai
// retain (d la dispatch_data, chet ngay sau handler - phan bien 16/09 do duoc bang NSZombie); alloc/init thi release;
// tra ve autorelease trong cung pool. Luong lam viec boc @autoreleasepool.
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
#include <unistd.h>
#include <sys/stat.h>

#if __has_feature(objc_arc)
#define JX_GIU(x)     (x)
#define JX_THA(x)     ((void)0)
#define JX_TUTHA(x)   (x)
#define JX_THA_GCD(x) ((void)0)
#else
#define JX_GIU(x)     [(x) retain]
#define JX_THA(x)     [(x) release]
#define JX_TUTHA(x)   [(x) autorelease]
#define JX_THA_GCD(x) dispatch_release(x)
#endif

extern "C" int JxTaiDuLieu_KiemPhienBan(const char* pszThuMuc, int nPhienBanApp, char* pszLoi, int nLoi);

// ---------------------------------------------------------------- trang thai
// Luong tai chay o hang doi nen; luong chinh bom vong lap de ve giao dien.
// Chi trao doi qua may bien nguyen tu don gian nay.
@interface JxTaiTrangThai : NSObject
@property (atomic) long long coTong;      // tong so byte phai tai
@property (atomic) long long coDaTai;     // da tai duoc
// [IOS-MANTAI 15/09] Tien do cua RIENG tep dang tai, de ve thanh thu hai giong VNKU:
// thanh tren = tep hien tai, thanh duoi = tong. Chi co tong thi nguoi choi nhin tep pak
// vai tram MB se tuong may treo, vi con so tong gan nhu dung yen hang phut.
@property (atomic) long long coTepTong;   // co cua tep dang tai
@property (atomic) long long coTepDaTai;  // da tai duoc cua tep dang tai
@property (atomic) long long coDaBam;     // da bam md5 (buoc kiem lan dau)
@property (atomic) long long coCanBam;    // tong phai bam
@property (atomic, copy) NSString* mucHienTai;
@property (atomic, copy) NSString* loi;          // loi KY THUAT (ghi log, hien co nho duoi loi nguoi dung)
// [IOS-DAKIEM 16/09] loi cho NGUOI CHOI (khong URL, md5, lenh python - nguoi duyet chup dung man nay); rong -> cau chung
@property (atomic, copy) NSString* loiNguoiDung;
@property (atomic, copy) NSString* canhBao;      // D: xong nhung khong kiem tra duoc cap nhat (hien 1,5 s roi vao game)
@property (atomic) BOOL chan;                    // A: chan han (ban app qua cu), khong nut Thu lai
@property (atomic) long long canHoi;             // 4.2.3(ii): >0 = dang cho nguoi choi bam "Tai xuong" cho tung nay byte
@property (atomic) BOOL daDongY;
@property (atomic) BOOL xong;
@property (atomic) BOOL huy;
@end
@implementation JxTaiTrangThai
#if !__has_feature(objc_arc)
- (void)dealloc
{	// thuoc tinh copy khong tu giai phong khi khong ARC
	[_mucHienTai release]; [_loi release]; [_loiNguoiDung release]; [_canhBao release];
	[super dealloc];
}
#endif
@end

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

/** Ma hoa tung doan cua duong dan cho URL theo tung BYTE UTF-8 cua chuoi.
    [16/09] Dung UTF8String, KHONG dung fileSystemRepresentation: cai sau tra dang tach dau (NFD) - "thử" thanh
    "thu" + dau rieng - khac byte trong manifest (Python ghi NFC) -> may chu Windows/Linux tra 404 voi ten co dau
    Latin (do 16/09). Tren dia thi APFS khong phan biet NFC/NFD nen stat/mo tep khong sao. */
static NSString* JxUrlHoaDuongDan(NSString* rel)
{
	NSMutableString* ra = [NSMutableString string];
	const char* p = rel.UTF8String;
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
	// [16/09] Tep hong UTF-8 -> nil -> coi nhu rong (bam lai roi JxGhiGonDaTai ghi lai sach), khong ket vinh vien.
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

// [16/09] Ghi lai da_tai.txt GON (moi tep mot dong) bang tep tam + rename. Tep chi noi them nen phinh dan qua moi ban
// cap nhat, va mot byte hong UTF-8 la ca tep bi bo -> bam lai 8,5 GB moi lan mo, vinh vien (phan bien 16/09 muc 13).
static void JxGhiGonDaTai(NSString* thuMuc, NSDictionary* daTai)
{
	if (!daTai.count) return;
	NSString* p = [thuMuc stringByAppendingPathComponent:@"da_tai.txt"];
	NSString* tam = [p stringByAppendingString:@".tmp"];
	FILE* f = fopen(tam.fileSystemRepresentation, "wb");
	if (!f) return;
	BOOL ok = YES;
	for (NSString* rel in [daTai.allKeys sortedArrayUsingSelector:@selector(compare:)])
		if (fprintf(f, "%s\t%s\n", [daTai[rel] UTF8String], rel.UTF8String) < 0) { ok = NO; break; }
	if (fflush(f) != 0) ok = NO;
	if (ok && fsync(fileno(f)) != 0) ok = NO;
	if (fclose(f) != 0) ok = NO;
	if (!ok || rename(tam.fileSystemRepresentation, p.fileSystemRepresentation) != 0)
		unlink(tam.fileSystemRepresentation);
}

// ---------------------------------------------------------------- manifest da kiem (dau hoan tat)
#define JX_TEP_DAKIEM @"manifest_dakiem.txt"

// Chuoi base64 (nhu noi dung manifest.sig) -> byte chu ky DER. nil neu khong hop le (vd cong Wi-Fi tra HTML).
static NSData* JxGiaiMaChuKy(NSString* s)
{
	s = [s stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
	if (!s.length) return nil;
	return JX_TUTHA([[NSData alloc] initWithBase64EncodedString:s options:0]);
}

// [IOS-DAKIEM 16/09] Luu manifest DA KIEM: dong dau = chu ky base64, sau do la manifest NGUYEN BYTE (chu ky ky tren
// dung cac byte do). MOT tep, ghi tam + fsync + rename: giet app giua chung thi hoac con ban cu, hoac co ban moi tron ven.
static BOOL JxLuuManifestDaKiem(NSString* thuMuc, NSData* dm, NSString* sigB64)
{
	NSString* p = [thuMuc stringByAppendingPathComponent:JX_TEP_DAKIEM];
	NSString* tam = [p stringByAppendingString:@".tmp"];
	FILE* f = fopen(tam.fileSystemRepresentation, "wb");
	if (!f) return NO;
	BOOL ok = fprintf(f, "%s\n", sigB64.UTF8String) > 0
	       && fwrite(dm.bytes, 1, dm.length, f) == dm.length
	       && fflush(f) == 0 && fsync(fileno(f)) == 0;
	if (fclose(f) != 0) ok = NO;
	if (ok && rename(tam.fileSystemRepresentation, p.fileSystemRepresentation) != 0) ok = NO;
	if (!ok) unlink(tam.fileSystemRepresentation);
	return ok;
}

// Doc lai manifest da luu VA KIEM CHU KY lai bang khoa cong khai trong app (~ms). Tra manifest (autorelease),
// nil khi chua co / cut / lech chu ky - khi do coi nhu chua tung hoan tat.
static NSData* JxDocManifestDaKiem(NSString* thuMuc)
{
	NSData* d = [NSData dataWithContentsOfFile:[thuMuc stringByAppendingPathComponent:JX_TEP_DAKIEM]];
	if (!d.length) return nil;
	const char* b = (const char*)d.bytes;
	const char* nl = (const char*)memchr(b, '\n', d.length);
	if (!nl || nl == b) return nil;
	NSUInteger nSig = (NSUInteger)(nl - b);
	NSString* s = JX_TUTHA([[NSString alloc] initWithBytes:b length:nSig encoding:NSASCIIStringEncoding]);
	NSData* dm = [d subdataWithRange:NSMakeRange(nSig + 1, d.length - nSig - 1)];
	return (dm.length && JxKiemChuKy(dm, JxGiaiMaChuKy(s))) ? dm : nil;
}

// ---------------------------------------------------------------- phan tich manifest
// Ba mang song song: duong dan tuong doi, co, md5. Tra NO + *pLoi khi manifest hong (khong doc mot dong nao).
static BOOL JxDocManifest(NSData* dm, NSMutableArray<NSString*>* duong, NSMutableArray<NSNumber*>* cos,
                          NSMutableArray<NSString*>* md5s, NSString** pLoi)
{
	[duong removeAllObjects]; [cos removeAllObjects]; [md5s removeAllObjects];
	// [16/09] May chu ghi UTF-8 (may_chu_tai_du_lieu.py, io.open utf-8). Nhanh Latin-1 truoc day la ma chet, ma neu roi
	// vao thi moi duong dan/URL deu sai byte -> bao thang cho nguoi ta thay.
	NSString* sm = JX_TUTHA([[NSString alloc] initWithData:dm encoding:NSUTF8StringEncoding]);
	if (!sm) { *pLoi = @"manifest.txt khong phai UTF-8"; return NO; }
	for (NSString* ln in [sm componentsSeparatedByString:@"\n"])
	{
		NSArray* c = [ln componentsSeparatedByString:@"\t"];
		if (c.count < 3) continue;
		NSString* rel = [[c[2] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]
		                 stringByReplacingOccurrencesOfString:@"\\" withString:@"/"];
		if (!rel.length) continue;
		// [IOS-TAI-CHAC 15/09] Loc duong dan. Ban Android da loc tu lau (TaiDuLieuActivity.java:340-341), ban iOS bo sot:
		// stringByAppendingPathComponent KHONG bo "..", ma JxTaiMotTep con tu tao cac thu muc trung gian - nen mot dong
		// manifest "../../Library/..." se ghi ra NGOAI thu muc du lieu. Chi ke giu khoa ky moi lam duoc, nhung khoa thi
		// co ngay lo; chan o day la chan them mot lop.
		while ([rel hasPrefix:@"/"]) rel = [rel substringFromIndex:1];
		if (!rel.length) continue;
		if ([rel isEqualToString:@".."] || [rel hasPrefix:@"../"] ||
		    [rel hasSuffix:@"/.."]     || [rel containsString:@"/../"])
		{	// Chat hon ban Android (ben do chi bo qua dong): manifest DA KY thi khong duoc phep co duong dan kieu nay -
			// co la dau hieu hong that, phai dung lai cho nguoi ta thay.
			*pLoi = [NSString stringWithFormat:@"manifest co duong dan khong hop le: %@", rel];
			return NO;
		}
		[duong addObject:rel];
		[cos   addObject:@([c[0] longLongValue])];
		[md5s  addObject:[c[1] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]];
	}
	if (duong.count == 0) { *pLoi = @"manifest.txt rong"; return NO; }
	return YES;
}

// ---------------------------------------------------------------- so voi dia
// [16/09] Nho ket qua bam TRONG TIEN TRINH theo (duong dan, co, mtime): tep bam ra KHONG khop khong duoc ghi vao
// da_tai.txt, nen truoc day moi lan bam "Thu lai" lai bam lai vai tram MB y nguyen (phan bien 16/09). Chi dung tren luong lam viec.
static NSMutableDictionary* s_bamNho = nil;

static NSString* JxKhoaBam(NSString* dich, long long co, long mtime)
{
	return [NSString stringWithFormat:@"%@\t%lld\t%ld", dich, co, mtime];
}

// Nhanh, KHONG bam: moi tep dung co va da_tai.txt ghi dung md5 -> YES. Dung de chon thoi han cho manifest online.
static BOOL JxDuTheoDaTai(NSString* thuMuc, NSArray<NSString*>* duong, NSArray<NSNumber*>* cos,
                          NSArray<NSString*>* md5s, NSDictionary* daTai)
{
	for (NSUInteger i = 0; i < duong.count; i++)
	{
		if (JxCoTep([thuMuc stringByAppendingPathComponent:duong[i]]) != cos[i].longLongValue) return NO;
		if (![daTai[duong[i]] isEqualToString:md5s[i]]) return NO;
	}
	return YES;
}

// So manifest voi dia: (1) tep dung co ma chua ghi trong da_tai.txt -> bam md5 (co tien do; nho ket qua trong tien trinh);
// (2) chot danh sach phai tai (*canTai = chi so vao mang). Tra tong byte con phai tai (da tru phan .part dang do).
static long long JxChotDanhSach(NSString* thuMuc, NSArray<NSString*>* duong, NSArray<NSNumber*>* cos, NSArray<NSString*>* md5s,
                                NSMutableDictionary* daTai, JxTaiTrangThai* tt, NSMutableArray<NSNumber*>* canTai)
{
	if (!s_bamNho) s_bamNho = [[NSMutableDictionary alloc] init];	// song suot doi app (co y)
	[canTai removeAllObjects];

	// vong 1: tim tep can bam md5 (co dung nhung chua co trong da_tai.txt)
	NSMutableArray<NSNumber*>* canBam = [NSMutableArray array];
	long long tongBam = 0;
	for (NSUInteger i = 0; i < duong.count; i++)
	{
		long long cd = JxCoTep([thuMuc stringByAppendingPathComponent:duong[i]]);
		if (cd != cos[i].longLongValue) continue;                 // thieu hoac khac co -> tai, khong can bam
		if ([daTai[duong[i]] isEqualToString:md5s[i]]) continue;   // da kiem roi
		[canBam addObject:@(i)]; tongBam += cd;
	}
	tt.coCanBam = tongBam;
	if (tongBam > 0)
	{
		tt.mucHienTai = @"Kiểm tra dữ liệu đã có (chỉ làm một lần)…";
		for (NSNumber* k in canBam)
		{
			if (tt.huy) break;
			NSUInteger i = k.unsignedIntegerValue;
			NSString* dich = [thuMuc stringByAppendingPathComponent:duong[i]];
			struct stat st;
			NSString* khoa = (stat(dich.fileSystemRepresentation, &st) == 0)
				? JxKhoaBam(dich, (long long)st.st_size, (long)st.st_mtime) : nil;
			NSString* m = khoa ? s_bamNho[khoa] : nil;
			if (m) tt.coDaBam = tt.coDaBam + cos[i].longLongValue;	// da bam trong lan thu truoc, khong bam lai
			else
			{
				m = JxMd5Tep(dich, tt);
				if (m && khoa) s_bamNho[khoa] = m;
			}
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
		NSString* dich = [thuMuc stringByAppendingPathComponent:duong[i]];
		if (JxCoTep(dich) == cos[i].longLongValue && [daTai[duong[i]] isEqualToString:md5s[i]]) continue;
		[canTai addObject:@(i)];
		long long co = cos[i].longLongValue;
		long long dap = JxCoTep([dich stringByAppendingString:@".part"]);
		tongTai += co - (dap > 0 && dap < co ? dap : 0);
	}
	return tongTai;
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
	// [16/09] Phan .part co san luc bat dau: JxChotDanhSach da TRU no khoi coTong, nen khi phai tai lai tu 0 thi
	// cong tra vao coTong va chi rut khoi coDaTai phan da dem trong lan nay (truoc day rut ca daCo -> thanh tien do am).
	long long daCoBanDau = daCo;

	tt.coTepTong = co;		// [IOS-MANTAI 15/09] cho thanh tien do cua rieng tep nay
	tt.coTepDaTai = daCo;	// tai tiep thi bat dau tu phan da co, khong ve lai tu 0

	// [16/09] Tep 0 byte: vong duoi khong chay (daCo < co sai ngay), ma phep kiem co o cuoi doi thay .part ->
	// truoc day "co -1, manifest ghi 0" -> unlink -> NO -> ket "Thu lai" vinh vien voi may cai moi (phan bien 16/09).
	if (co == 0)
	{
		FILE* f0 = fopen(tam.fileSystemRepresentation, "wb");
		if (!f0) { tt.loi = [NSString stringWithFormat:@"%@: khong tao duoc tep", rel]; return NO; }
		fclose(f0);
	}

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

		// [16/09 MRC] Khong giu doi tuong (hp/le) qua khoi block: chi lay so va chuoi da retain. Truoc day __block
		// NSHTTPURLResponse* song nho pool cua GCD, se chet neu boc them @autoreleasepool (phan bien 16/09 muc 7).
		__block long st = -1;            // -1 = khong co phan hoi HTTP
		__block NSString* moTaLoi = nil; // retain trong block, release cuoi vong
		__block BOOL bDayDia = NO, bRangeSai = NO, bTuDau = NO;
		dispatch_semaphore_t sem = dispatch_semaphore_create(0);
		// Dung downloadTask: no ghi thang ra tep tam cua he thong nen tep pak vai tram MB
		// khong phai nam het trong bo nho.
		NSURLSessionDownloadTask* t = [ss downloadTaskWithRequest:rq
			completionHandler:^(NSURL* vt, NSURLResponse* rp, NSError* er) {
				NSHTTPURLResponse* hp = [rp isKindOfClass:[NSHTTPURLResponse class]] ? (NSHTTPURLResponse*)rp : nil;
				if (er) moTaLoi = JX_GIU([er.localizedDescription copy] ?: @"");
				if (hp) st = hp.statusCode;
				// [IOS-TAI-CHAC 15/09] KIEM MA TRA VE TRUOC KHI GHI. downloadTask giao tep tam voi
				// BAT KY ma nao - 404, 500, trang cong dang nhap - vi loi HTTP khong phai NSError.
				// Truoc day khoi ghi nam trong "if (vt)" chay vo dieu kien, nen than trang loi bi noi
				// thang vao .part roi con nam lai do sang lan mo app sau.
				BOOL bGhi = NO;
				if (!er && hp && vt)
				{
					if (st == 206)
					{	// Phai dung khuc ta xin. May chu noi doi -> noi vao la hong am tham.
						long long a = -1, b = -1, tong = -1;
						NSString* cr = [hp valueForHTTPHeaderField:@"Content-Range"];
						int k = cr ? sscanf(cr.UTF8String, "bytes %lld-%lld/%lld", &a, &b, &tong) : 0;
						if (k >= 2 && a == daCoLucGui && (k < 3 || tong == co)) bGhi = YES;
						else bRangeSai = YES;
					}
					else if (st == 200)
					{	// 200 khi dang xin khuc = may chu bo qua Range, dang gui CA TEP tu byte 0.
						// [16/09] Ghi luon tu dau (nhu ban Android, TaiDuLieuActivity.java:477) thay vi vut di
						// roi tai lai lan nua - truoc day tep pak vai tram MB bi tai gap doi.
						bGhi = YES;
						bTuDau = (daCoLucGui > 0);
					}
					// 4xx/5xx: khong ghi gi ca, .part giu nguyen
				}
				if (bGhi)
				{	// noi vao duoi .part (hoac ghi lai tu dau)
					FILE* fo = fopen(tam.fileSystemRepresentation, bTuDau ? "wb" : "ab");
					FILE* fi = fopen(vt.path.fileSystemRepresentation, "rb");
					if (bTuDau)
					{	// phan da dem cua tep nay khong con gia tri
						tt.coDaTai = tt.coDaTai - (daCoLucGui - daCoBanDau);
						tt.coTong  = tt.coTong + daCoBanDau;
						tt.coTepDaTai = 0;
					}
					if (fo && fi)
					{
						static const size_t nD = 1 << 20;
						unsigned char* pD = (unsigned char*)malloc(nD);
						size_t n;
						while (pD && (n = fread(pD, 1, nD, fi)) > 0)
						{
							size_t w = fwrite(pD, 1, n, fo);
							tt.coDaTai = tt.coDaTai + (long long)w;	// chi dem byte GHI DUOC that
							tt.coTepDaTai = tt.coTepDaTai + (long long)w;
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
		JX_THA_GCD(sem);
		if (bTuDau) { daCo = 0; daCoBanDau = 0; }
		NSString* sLoi = JX_TUTHA(moTaLoi); moTaLoi = nil;

		// [IOS-TAI-CHAC 15/09] Phep tac xu ly loi: byte tren dia SAI thi xoa, chi THIEU thi giu.
		// Dut mang chi lam cut tep chu khong lam hong, nen xoa .part luc do la tu bat nguoi choi
		// tai lai tu dau ca tram MB - dung cai hai ma .part sinh ra de tranh.
		if (bDayDia)
		{	// giu .part: byte da ghi van dung, chi la chua du
			tt.loi = [NSString stringWithFormat:@"%@: may het cho trong", rel];
			tt.loiNguoiDung = @"Máy hết chỗ trống. Hãy xoá bớt dữ liệu rồi bấm Thử lại.\n(Not enough free space on this device.)";
			return NO;
		}
		if (bRangeSai)
		{	// byte cu khong noi tiep duoc voi byte moi -> cat ve 0 va tai lai TEP NAY tu dau
			if (++soLamLai > 2)
			{
				unlink(tam.fileSystemRepresentation);
				tt.loi = [NSString stringWithFormat:@"%@: may chu khong theo dung Range", rel];
				return NO;
			}
			FILE* fx = fopen(tam.fileSystemRepresentation, "wb"); if (fx) fclose(fx);
			tt.coDaTai = tt.coDaTai - (daCo - daCoBanDau);	// tra lai thanh tien do cho khoi dem trung
			tt.coTong  = tt.coTong + daCoBanDau;
			daCo = 0; daCoBanDau = 0;
			tt.coTepDaTai = 0;	// tep nay tai lai tu dau -> thanh tien do cua no ve 0
			continue;
		}
		if (sLoi || st < 0)
		{	// loi truyen tai, hoac phan hoi khong phai HTTP. Giu .part, cho roi thu lai.
			if (++soLoiMang > 4)
			{
				tt.loi = sLoi ? [NSString stringWithFormat:@"%@: %@", rel, sLoi]
				              : [NSString stringWithFormat:@"%@: phan hoi khong hop le", rel];
				tt.loiNguoiDung = @"Mất kết nối khi đang tải. Hãy kiểm tra mạng rồi bấm Thử lại, phần đã tải được giữ nguyên.\n(Connection lost while downloading. Check your network and tap Retry.)";
				return NO;
			}
			for (int i = 0; i < nNghi * 10 && !tt.huy; i++) usleep(100000);
			if (nNghi < 16) nNghi *= 2;
			continue;
		}
		if (st != 200 && st != 206)
		{	// giu .part: 404 luc dang rai ban moi, 403 het han chu ky... khong noi gi ve byte da tai
			tt.loi = [NSString stringWithFormat:@"%@: may chu tra ve %ld", rel, st];
			tt.loiNguoiDung = [NSString stringWithFormat:@"Kho dữ liệu đang được cập nhật (máy chủ trả về %ld). Hãy thử lại sau ít phút.\n(The data server returned %ld. Please try again in a few minutes.)", st, st];
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
		tt.coTepDaTai = mm;	// dong bo lai theo co that tren dia
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
	// [16/09] rename thay the nguyen tu, khong can unlink truoc: unlink truoc roi rename hong la mat luon ban cu.
	if (rename(tam.fileSystemRepresentation, dich.fileSystemRepresentation) != 0)
	{
		tt.loi = [NSString stringWithFormat:@"%@: doi ten that bai", rel];
		return NO;
	}
	JxGhiThemDaTai(thuMuc, m ?: @"", rel);
	return YES;
}

// ---------------------------------------------------------------- viec chinh
static int  s_nPhienBanApp = 0;     // [IOS-PHIENBAN 16/09] JxTaiDuLieu_DatPhienBanApp; 0 = khong kiem o day (main() van kiem sau)
static BOOL s_coGiaoDien   = NO;    // [16/09] khong giao dien (macOS / bo thu) thi khong hoi y truoc khi tai
#define JX_HOI_TU_BYTE  (50LL * 1024 * 1024)   // dieu 4.2.3(ii): tu 50 MB thi noi dung luong va cho bam "Tai xuong"

typedef enum { JxNguonKhong = 0, JxNguonOnline, JxNguonDaLuu } JxNguon;

// Tai mot dia chi ve bo nho (toi da nToiDa byte), cho xong moi tra ve (chay o hang doi nen nen chan duoc).
// Tra nil khi hong. *pMa = ma HTTP (0 = khong co phan hoi HTTP), *pLoiMang = ma NSError (0 = khong co),
// *pHtml = YES khi than la trang web (cong dang nhap Wi-Fi cong cong tra 200 + HTML cho moi duong).
static NSData* JxTaiDongBo(NSURLSession* ss, NSString* su, NSTimeInterval giay, NSUInteger nToiDa,
                           long* pMa, NSInteger* pLoiMang, BOOL* pHtml)
{
	if (pMa) *pMa = 0; if (pLoiMang) *pLoiMang = 0; if (pHtml) *pHtml = NO;
	NSURL* u = [NSURL URLWithString:su];
	if (!u) return nil;	// [16/09] requestWithURL:nil NEM ngoai le -> sap; goc doc tu tep/plist nguoi ta sua duoc
	// [16/09] Thoi han theo YEU CAU thang thoi han cua phien (do duoc: 3 s yeu cau vs 30 s phien -> -1001 sau 3 s).
	NSMutableURLRequest* rq = [NSMutableURLRequest requestWithURL:u cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
	                                              timeoutInterval:giay];
	__block NSData* ra = nil; __block long ma = 0; __block NSInteger loiMang = 0; __block BOOL html = NO;
	dispatch_semaphore_t sem = dispatch_semaphore_create(0);
	[[ss dataTaskWithRequest:rq completionHandler:^(NSData* d, NSURLResponse* r, NSError* e) {
		NSHTTPURLResponse* hp = [r isKindOfClass:[NSHTTPURLResponse class]] ? (NSHTTPURLResponse*)r : nil;
		ma = hp ? hp.statusCode : 0;
		loiMang = e ? e.code : 0;
		NSString* kieu = [hp.MIMEType lowercaseString];
		html = [kieu containsString:@"text/html"] || (d.length > 0 && ((const char*)d.bytes)[0] == '<');
		// [16/09 SUA SAP] PHAI retain: d la dispatch_data, bi giai phong ngay sau khi handler tra ve (NSZombie bat duoc);
		// truoc day chi gan con tro, song nho heap chua bi tai dung - dung lop loi sap 14/09 cua JxIosNhatKy.mm.
		if (!e && hp && ma == 200 && d.length > 0 && d.length <= nToiDa) ra = JX_GIU(d);
		dispatch_semaphore_signal(sem); }] resume];
	dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
	JX_THA_GCD(sem);
	if (pMa) *pMa = ma; if (pLoiMang) *pLoiMang = loiMang; if (pHtml) *pHtml = html;
	return JX_TUTHA(ra);
}

// Loi cho NGUOI CHOI khi khong lay duoc manifest hop le tu may chu (phan bien 16/09: "hay kiem tra mang" khi mang
// tot va loi o may chu la chi dan sai, nguoi duyet se ghi "app bao mat ket noi du mang hoat dong").
static NSString* JxLyDoMang(long ma, NSInteger loiMang, BOOL html, BOOL chuKySai)
{
	if (html)
		return @"Mạng Wi-Fi này cần đăng nhập trước (máy chủ trả về một trang web thay vì dữ liệu). Hãy mở Safari để đăng nhập Wi-Fi, hoặc đổi mạng.";
	if (loiMang == NSURLErrorNotConnectedToInternet || loiMang == NSURLErrorNetworkConnectionLost ||
	    loiMang == NSURLErrorInternationalRoamingOff || loiMang == NSURLErrorDataNotAllowed)
		return @"Không có kết nối mạng.";
	if (loiMang == NSURLErrorAppTransportSecurityRequiresSecureConnection)
		return @"Địa chỉ kho dữ liệu không dùng https (lỗi cấu hình bản dựng, không phải lỗi mạng).";
	if (chuKySai)
		return @"Kho dữ liệu đang được cập nhật (chữ ký chưa khớp). Hãy thử lại sau ít phút.";
	if (ma >= 400)
		return [NSString stringWithFormat:@"Kho dữ liệu đang bảo trì (máy chủ trả về %ld). Hãy thử lại sau.", ma];
	return @"Không nối được máy chủ dữ liệu.";
}

static NSString* JxDungLuong(long long n)
{
	if (n >= (1LL << 30)) return [NSString stringWithFormat:@"%.1f GB", (double)n / (1024.0*1024.0*1024.0)];
	if (n >= (1LL << 20)) return [NSString stringWithFormat:@"%.1f MB", (double)n / (1024.0*1024.0)];
	return [NSString stringWithFormat:@"%lld KB", (n + 1023) / 1024];
}

// Than viec: tra ve sau khi dat tt.loi / tt.canhBao / xong xuoi. Nguoi goi dat tt.xong.
static void JxLamViecThat(NSURLSession* ss, NSArray<NSString*>* cacGoc, NSString* thuMuc, JxTaiTrangThai* tt)
{
	NSMutableArray<NSString*>* duong = [NSMutableArray array];
	NSMutableArray<NSNumber*>* cos   = [NSMutableArray array];
	NSMutableArray<NSString*>* md5s  = [NSMutableArray array];
	NSMutableDictionary* daTai = JxDocDaTai(thuMuc);

	// --- 0. manifest DA KIEM lan truoc (dau hoan tat) ---
	NSMutableArray<NSString*>* duongLuu = [NSMutableArray array];
	NSMutableArray<NSNumber*>* cosLuu   = [NSMutableArray array];
	NSMutableArray<NSString*>* md5sLuu  = [NSMutableArray array];
	BOOL coLuu = NO, duTheoLuu = NO;
	{
		NSData* dmLuu = JxDocManifestDaKiem(thuMuc);
		NSString* l = nil;
		if (dmLuu && JxDocManifest(dmLuu, duongLuu, cosLuu, md5sLuu, &l))
		{
			coLuu = YES;
			duTheoLuu = JxDuTheoDaTai(thuMuc, duongLuu, cosLuu, md5sLuu, daTai);
		}
		NSLog(@"[IOS-TAI] manifest da kiem: %@ (%lu tep)%@", coLuu ? @"co" : @"khong", (unsigned long)duongLuu.count,
		      coLuu ? (duTheoLuu ? @", du lieu DU theo no" : @", du lieu chua du theo no") : @"");
	}

	// --- 1. manifest online, thu tung guong ---
	// [16/09] Da du de choi thi khong bat cho lau: 8 s (= THOI_HAN_MS cua Android khi chua co du lieu; 3000 ms cua
	// Android la so chinh cho LAN - tren mang di dong DNS + TLS + danh thuc song vuot 3 s la thuong -> D gia, tuc
	// chay vong qua ep cap nhat). Chua du thi 30 s: thu lai ton mot cu bam cua nguoi choi.
	tt.mucHienTai = @"Đang kiểm tra cập nhật…";
	const NSTimeInterval giay = duTheoLuu ? 8.0 : 30.0;
	JxNguon nguon = JxNguonKhong; NSString* goc = nil;
	long maCuoi = 0; NSInteger loiMangCuoi = 0; BOOL htmlCuoi = NO, chuKySaiCuoi = NO;
	for (NSString* g in cacGoc)
	{
		long ma = 0; NSInteger lm = 0; BOOL html = NO;
		NSData* dm = JxTaiDongBo(ss, [g stringByAppendingString:@"manifest.txt"], giay, 16u << 20, &ma, &lm, &html);
		if (!dm)
		{
			NSLog(@"[IOS-TAI] %@manifest.txt: khong lay duoc (HTTP %ld, loi %ld%@)", g, ma, (long)lm, html ? @", HTML" : @"");
			maCuoi = ma; loiMangCuoi = lm; htmlCuoi = html;
			continue;
		}
		// [BAOMAT 12/09 KY] Bat buoc co chu ky hop le. Khong co chu ky, hoac chu ky sai, la KHONG doc lay mot dong
		// nao cua manifest. Manifest sai = tai ve tep bi doi ruot, ma trong do co script Lua chay that tren may.
		tt.mucHienTai = @"Đang kiểm tra chữ ký danh sách tệp…";
		NSData* dSigB64 = JxTaiDongBo(ss, [g stringByAppendingString:@"manifest.sig"], giay, 64u << 10, &ma, &lm, &html);
		NSString* sSig = dSigB64 ? JX_TUTHA([[NSString alloc] initWithData:dSigB64 encoding:NSASCIIStringEncoding]) : nil;
		sSig = [sSig stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
		if (!JxKiemChuKy(dm, JxGiaiMaChuKy(sSig)))
		{
			NSLog(@"[IOS-TAI] %@manifest.sig: chu ky KHONG hop le (HTTP %ld, loi %ld%@)", g, ma, (long)lm, html ? @", HTML" : @"");
			chuKySaiCuoi = YES; htmlCuoi = html; maCuoi = ma; loiMangCuoi = lm;
			continue;
		}
		NSString* l = nil;
		if (!JxDocManifest(dm, duong, cos, md5s, &l))
		{	// chu ky dung ma noi dung hong: loi cua kho, khong dung manifest da luu de che
			tt.loi = [NSString stringWithFormat:@"%@: %@", g, l];
			tt.loiNguoiDung = @"Danh sách tệp trên máy chủ không hợp lệ. Hãy thử lại sau.\n(The server's file list is invalid. Please try again later.)";
			return;
		}
		NSLog(@"[IOS-TAI] %@: chu ky danh sach tep HOP LE, %lu tep", g, (unsigned long)duong.count);
		// [IOS-DAKIEM 16/09] Luu NGAY khi biet manifest that (sau khi phan tich sach), TRUOC khi tai: mot khi may da
		// biet co ban moi thi mat mang cung khong duoc choi voi du lieu cu (ep cap nhat). Mat trai (phan bien 16/09):
		// rai ban do (manifest moi len truoc, tep 404) la moi may mo app trong cua so do bi khoa toi khi kho du.
		if (!JxLuuManifestDaKiem(thuMuc, dm, sSig))
			NSLog(@"[IOS-TAI] khong ghi duoc %@", JX_TEP_DAKIEM);
		nguon = JxNguonOnline; goc = g;
		break;
	}

	if (nguon == JxNguonKhong)
	{
		if (!coLuu)
		{	// E1: chua tung co manifest hop le tren may -> khong biet du lieu co du khong -> chi con Thu lai
			tt.loi = [NSString stringWithFormat:@"khong lay duoc manifest hop le tu %@ (HTTP %ld, loi %ld%@%@)",
			          [cacGoc componentsJoinedByString:@" "], maCuoi, (long)loiMangCuoi,
			          htmlCuoi ? @", HTML" : @"", chuKySaiCuoi ? @", chu ky sai" : @""];
			tt.loiNguoiDung = [NSString stringWithFormat:
				@"%@\nDữ liệu trong máy chưa đủ để chơi. Hãy kiểm tra mạng rồi bấm Thử lại.\n"
				 "(Cannot reach the data server and game data is incomplete. Check your network and tap Retry.)",
				JxLyDoMang(maCuoi, loiMangCuoi, htmlCuoi, chuKySaiCuoi)];
			return;
		}
		nguon = JxNguonDaLuu;
		[duong setArray:duongLuu]; [cos setArray:cosLuu]; [md5s setArray:md5sLuu];
		NSLog(@"[IOS-TAI] dung manifest DA KIEM lan truoc (%lu tep)", (unsigned long)duong.count);
	}

	// --- 2. so voi tep tren dia; bam md5 nhung tep dung co ma chua ghi trong da_tai.txt ---
	NSMutableArray<NSNumber*>* canTai = [NSMutableArray array];
	long long tongTai = JxChotDanhSach(thuMuc, duong, cos, md5s, daTai, tt, canTai);
	if (tt.huy) return;

	// --- 2b. [IOS-PHIENBAN 16/09] phienban.txt: tai va kiem TRUOC khi tai hang GB ---
	// Tep nay co md5 trong manifest da ky nen tin duoc. Truoc day chi kiem o main() SAU khi dong bo xong: app cu tai
	// tron 8,5 GB roi moi bi chan "Ban game da cu". main() van kiem lai lam lop cuoi.
	if (s_nPhienBanApp > 0)
	{
		NSUInteger iPb = [duong indexOfObject:@"phienban.txt"];
		if (iPb != NSNotFound)
		{
			NSUInteger kPb = [canTai indexOfObject:@(iPb)];
			if (kPb != NSNotFound)
			{
				if (nguon != JxNguonOnline)
				{	// offline ma phienban.txt chua co/khac -> du lieu chua du, xu nhu E2 ben duoi
					goto e2;
				}
				tt.mucHienTai = @"phienban.txt";
				if (!JxTaiMotTep(ss, goc, thuMuc, @"phienban.txt", cos[iPb].longLongValue, md5s[iPb], tt))
					return;	// tt.loi da duoc dat
				daTai[@"phienban.txt"] = md5s[iPb];
				[canTai removeObjectAtIndex:kPb];
				tongTai -= cos[iPb].longLongValue;
			}
			char szL[768] = "";
			if (JxTaiDuLieu_KiemPhienBan(thuMuc.fileSystemRepresentation, s_nPhienBanApp, szL, (int)sizeof(szL)))
			{
				tt.chan = YES;
				tt.loi = [NSString stringWithFormat:@"phienban.txt: app %d qua cu", s_nPhienBanApp];
				tt.loiNguoiDung = [NSString stringWithUTF8String:szL];
				return;
			}
		}
	}

	tt.coTong = tongTai;
	if (canTai.count == 0)
	{
		if (nguon == JxNguonDaLuu)
		{	// D: khong kiem tra duoc cap nhat, nhung du lieu DU theo manifest da kiem -> vao game, bao nhe
			tt.canhBao = [NSString stringWithFormat:@"%@ Không kiểm tra được cập nhật, dùng dữ liệu đã có.\n(Could not check for updates; using existing data.)",
			              JxLyDoMang(maCuoi, loiMangCuoi, htmlCuoi, chuKySaiCuoi)];
		}
		tt.mucHienTai = @"Dữ liệu đã đầy đủ";
		JxGhiGonDaTai(thuMuc, daTai);
		return;
	}
	if (nguon == JxNguonDaLuu)
	{
e2:		// E2: co ban moi (theo manifest da kiem) chua tai xong, ma khong nol duoc may chu -> khong co loi vao game
		tt.loi = [NSString stringWithFormat:@"offline, con %lu tep (%lld B) theo manifest da kiem (HTTP %ld, loi %ld%@%@)",
		          (unsigned long)canTai.count, tongTai, maCuoi, (long)loiMangCuoi,
		          htmlCuoi ? @", HTML" : @"", chuKySaiCuoi ? @", chu ky sai" : @""];
		tt.loiNguoiDung = [NSString stringWithFormat:
			@"Có bản cập nhật chưa tải xong (%lu tệp, %@).\n%@\nDữ liệu trong máy chưa đủ để chơi. Hãy kiểm tra mạng rồi bấm Thử lại.\n"
			 "(An update has not finished downloading and the data server is unreachable. Check your network and tap Retry.)",
			(unsigned long)canTai.count, JxDungLuong(tongTai), JxLyDoMang(maCuoi, loiMangCuoi, htmlCuoi, chuKySaiCuoi)];
		return;
	}

	// --- 2c. [16/09] dieu 4.2.3(ii): noi dung luong va cho nguoi choi bam "Tai xuong" truoc khi tai lon ---
	// Mot nut duy nhat, khong co "bo qua" (chu da chot ep cap nhat). Duoi nguong thi tai luon nhu ban va nho.
	if (s_coGiaoDien && tongTai >= JX_HOI_TU_BYTE)
	{
		tt.daDongY = NO;
		tt.canHoi = tongTai;
		while (!tt.daDongY && !tt.huy) usleep(50000);
		tt.canHoi = 0;
		if (tt.huy) return;
	}

	// --- 3. tai ---
	for (NSNumber* k in canTai)
	{
		if (tt.huy) break;
		NSUInteger i = k.unsignedIntegerValue;
		tt.mucHienTai = duong[i];
		BOOL ok;
		@autoreleasepool { ok = JxTaiMotTep(ss, goc, thuMuc, duong[i], cos[i].longLongValue, md5s[i], tt); }
		if (!ok)
			return;	// tt.loi da duoc dat
		daTai[duong[i]] = md5s[i];
	}
	if (!tt.huy)
	{
		tt.mucHienTai = @"Dữ liệu đã đầy đủ";
		JxGhiGonDaTai(thuMuc, daTai);
	}
}

static void JxLamViec(NSArray<NSString*>* cacGoc, NSString* thuMuc, JxTaiTrangThai* tt)
{
	@autoreleasepool {	// [16/09] luong nen khong co pool san: khong boc thi moi doi tuong autorelease cua ca dot tai don toi cuoi
		NSURLSessionConfiguration* cf = [NSURLSessionConfiguration ephemeralSessionConfiguration];
		cf.timeoutIntervalForRequest  = 30.0;
		cf.timeoutIntervalForResource = 3600.0;
		NSURLSession* ss = [NSURLSession sessionWithConfiguration:cf];
		JxLamViecThat(ss, cacGoc, thuMuc, tt);
		[ss finishTasksAndInvalidate];	// [16/09] khong invalidate = moi lan Thu lai ro ri mot phien + luong cua no
		tt.xong = YES;	// dat SAU CUNG: luong chinh chi doc ket qua khi thay co nay
	}
}

// ---------------------------------------------------------------- giao dien
// [IOS-MANTAI 15/09] Khai truoc: veTienDo: ben duoi goi JxMb, ma than ham JxMb nam SAU khoi lop nay.
static NSString* JxMb(long long n);

#if TARGET_OS_IPHONE
@interface JxTaiMan : UIViewController
@property (nonatomic, strong) UILabel* nhan;
@property (nonatomic, strong) UILabel* chiTiet;
@property (nonatomic, strong) UILabel* kyThuat;   // [16/09] dong ky thuat co nho duoi loi nguoi dung (md5, URL, ma loi)
@property (nonatomic, strong) UIProgressView* thanh;
// [IOS-KHONGCHET 15/09] nut duoi man tai: luc tai thi an, luc hong thi hien "Thử lại".
// daBam la atomic vi vong bom o luong chinh doc no, con UIKit dat no trong ham bam.
@property (nonatomic, strong) UIButton* nut;
@property (atomic) BOOL daBam;
// [IOS-MANTAI 15/09] thanh thu hai (tong) + nhan tong + dong ho, theo dung bo cuc cua VNKU
@property (nonatomic, strong) UIProgressView* thanh2;
@property (nonatomic, strong) UILabel* nhanTong;
@property (nonatomic, strong) UILabel* dongHo;
@end
@implementation JxTaiMan

// [IOS-MANTAI 15/09] Nen man tai. Uu tien anh "nen_tai" nhet san trong goi (ios/nen_tai.jpg,
// ios/CMakeLists.txt tu dong goi neu co tep); chua co anh thi ve nen chuyen sac toi.
// KHONG duoc lay anh tu cay du lieu game: man nay chay khi may VUA CAI, luc do chua co du lieu.
- (void)datNen
{
	UIImage* anh = [UIImage imageNamed:@"nen_tai"];
	if (anh)
	{
		UIImageView* nen = JX_TUTHA([[UIImageView alloc] initWithImage:anh]);
		nen.translatesAutoresizingMaskIntoConstraints = NO;
		nen.contentMode = UIViewContentModeScaleAspectFill;
		nen.clipsToBounds = YES;
		[self.view addSubview:nen];
		[NSLayoutConstraint activateConstraints:@[
			[nen.leadingAnchor  constraintEqualToAnchor:self.view.leadingAnchor],
			[nen.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor],
			[nen.topAnchor      constraintEqualToAnchor:self.view.topAnchor],
			[nen.bottomAnchor   constraintEqualToAnchor:self.view.bottomAnchor],
		]];
		// Phu mot lop toi o nua duoi de chu va thanh tien do luon doc duoc, du anh sang mau gi.
		UIView* phu = JX_TUTHA([[UIView alloc] init]);
		phu.translatesAutoresizingMaskIntoConstraints = NO;
		phu.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.45];
		[self.view addSubview:phu];
		[NSLayoutConstraint activateConstraints:@[
			[phu.leadingAnchor  constraintEqualToAnchor:self.view.leadingAnchor],
			[phu.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor],
			[phu.bottomAnchor   constraintEqualToAnchor:self.view.bottomAnchor],
			[phu.heightAnchor   constraintEqualToAnchor:self.view.heightAnchor multiplier:0.42],
		]];
		return;
	}
	self.view.backgroundColor = [UIColor colorWithRed:0.05 green:0.04 blue:0.03 alpha:1.0];
}

- (UILabel*)taoNhan:(CGFloat)co dam:(BOOL)dam mau:(UIColor*)mau
{
	UILabel* l = JX_TUTHA([[UILabel alloc] init]);	// [16/09 MRC] thuoc tinh strong giu them mot lan; alloc/init khong autorelease la ro ri
	l.translatesAutoresizingMaskIntoConstraints = NO;
	l.textColor = mau;
	l.font = [UIFont systemFontOfSize:co weight:(dam ? UIFontWeightSemibold : UIFontWeightRegular)];
	l.numberOfLines = 1;
	[self.view addSubview:l];
	return l;
}

- (void)viewDidLoad
{
	[super viewDidLoad];
	[self datNen];

	// [IOS-MANTAI 15/09] TAT CA dat bang rang buoc, KHONG dung toa do cung: cua so duoc tao theo
	// [UIScreen mainScreen].bounds (luc khoi dong la chieu DOC) trong khi Info.plist khoa game nam
	// NGANG, nen toa do tinh mot lan trong viewDidLoad se lech - nut bam se khong trung.
	UILayoutGuide* an = self.view.safeAreaLayoutGuide;

	self.nhan    = [self taoNhan:17 dam:YES mau:[UIColor whiteColor]];
	self.nhan.textAlignment = NSTextAlignmentCenter;
	self.nhan.text = @"Chuẩn bị dữ liệu";

	// Hang tep: "Tên tệp — 37.5% (366.8 / 978.5 MB)"
	self.chiTiet = [self taoNhan:13 dam:NO mau:[UIColor colorWithWhite:0.86 alpha:1.0]];
	self.chiTiet.numberOfLines = 2;

	self.kyThuat = [self taoNhan:10 dam:NO mau:[UIColor colorWithWhite:0.55 alpha:1.0]];
	self.kyThuat.numberOfLines = 2;
	self.kyThuat.text = @"";

	self.thanh = JX_TUTHA([[UIProgressView alloc] initWithProgressViewStyle:UIProgressViewStyleDefault]);
	self.thanh.translatesAutoresizingMaskIntoConstraints = NO;
	self.thanh.progressTintColor = [UIColor colorWithRed:0.25 green:0.55 blue:0.95 alpha:1.0];	// xanh duong: tep
	self.thanh.trackTintColor = [UIColor colorWithWhite:1.0 alpha:0.22];
	[self.view addSubview:self.thanh];

	self.nhanTong = [self taoNhan:13 dam:NO mau:[UIColor colorWithWhite:0.86 alpha:1.0]];

	self.thanh2 = JX_TUTHA([[UIProgressView alloc] initWithProgressViewStyle:UIProgressViewStyleDefault]);
	self.thanh2.translatesAutoresizingMaskIntoConstraints = NO;
	self.thanh2.progressTintColor = [UIColor colorWithRed:0.30 green:0.80 blue:0.35 alpha:1.0];	// xanh la: tong
	self.thanh2.trackTintColor = [UIColor colorWithWhite:1.0 alpha:0.22];
	[self.view addSubview:self.thanh2];

	self.dongHo = [self taoNhan:12 dam:NO mau:[UIColor colorWithWhite:0.70 alpha:1.0]];
	self.dongHo.text = @"00:00:00";

	self.nut = [UIButton buttonWithType:UIButtonTypeSystem];
	self.nut.translatesAutoresizingMaskIntoConstraints = NO;
	self.nut.titleLabel.font = [UIFont systemFontOfSize:16 weight:UIFontWeightSemibold];
	self.nut.backgroundColor = [UIColor colorWithWhite:0.22 alpha:0.95];
	[self.nut setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
	self.nut.layer.cornerRadius = 8;
	self.nut.hidden = YES;
	[self.nut addTarget:self action:@selector(bamNut) forControlEvents:UIControlEventTouchUpInside];
	[self.view addSubview:self.nut];

	const CGFloat le = 26;	// le trai/phai
	[NSLayoutConstraint activateConstraints:@[
		// tieu de nam giua man
		[self.nhan.centerXAnchor  constraintEqualToAnchor:self.view.centerXAnchor],
		[self.nhan.bottomAnchor   constraintEqualToAnchor:self.chiTiet.topAnchor constant:-18],
		[self.nhan.leadingAnchor  constraintEqualToAnchor:an.leadingAnchor  constant:le],
		[self.nhan.trailingAnchor constraintEqualToAnchor:an.trailingAnchor constant:-le],

		// khoi tien do bam DAY man, xep tu duoi len (giong VNKU)
		[self.chiTiet.leadingAnchor  constraintEqualToAnchor:an.leadingAnchor  constant:le],
		[self.chiTiet.trailingAnchor constraintEqualToAnchor:an.trailingAnchor constant:-le],
		[self.chiTiet.bottomAnchor   constraintEqualToAnchor:self.kyThuat.topAnchor constant:-4],

		[self.kyThuat.leadingAnchor  constraintEqualToAnchor:an.leadingAnchor  constant:le],
		[self.kyThuat.trailingAnchor constraintEqualToAnchor:an.trailingAnchor constant:-le],
		[self.kyThuat.bottomAnchor   constraintEqualToAnchor:self.thanh.topAnchor constant:-6],

		[self.thanh.leadingAnchor  constraintEqualToAnchor:an.leadingAnchor  constant:le],
		[self.thanh.trailingAnchor constraintEqualToAnchor:an.trailingAnchor constant:-le],
		[self.thanh.bottomAnchor   constraintEqualToAnchor:self.nhanTong.topAnchor constant:-16],
		[self.thanh.heightAnchor   constraintEqualToConstant:5],

		[self.nhanTong.leadingAnchor constraintEqualToAnchor:an.leadingAnchor constant:le],
		[self.nhanTong.bottomAnchor  constraintEqualToAnchor:self.thanh2.topAnchor constant:-6],

		[self.thanh2.leadingAnchor  constraintEqualToAnchor:an.leadingAnchor  constant:le],
		[self.thanh2.trailingAnchor constraintEqualToAnchor:an.trailingAnchor constant:-le],
		[self.thanh2.bottomAnchor   constraintEqualToAnchor:self.dongHo.topAnchor constant:-8],
		[self.thanh2.heightAnchor   constraintEqualToConstant:5],

		[self.dongHo.leadingAnchor constraintEqualToAnchor:an.leadingAnchor constant:le],
		[self.dongHo.bottomAnchor  constraintEqualToAnchor:an.bottomAnchor  constant:-16],

		// nut Thu lai / Tai xuong: chi hien khi can, nam TREN tieu de. [16/09] Truoc day neo vao chiTiet.top nhu tieu de
		// -> nut de len chu tieu de (thay tren may ao).
		[self.nut.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor],
		[self.nut.bottomAnchor  constraintEqualToAnchor:self.nhan.topAnchor constant:-16],
		[self.nut.widthAnchor   constraintGreaterThanOrEqualToConstant:200],
		[self.nut.heightAnchor  constraintEqualToConstant:46],
	]];
}

- (void)bamNut
{
	self.daBam = YES;
}

// Cap nhat mot lan ve: tien do tep + tien do tong + dong ho.
- (void)veTienDo:(NSString*)ten tepDa:(long long)tepDa tepTong:(long long)tepTong
            tong:(long long)tong  da:(long long)da     giay:(double)giay
{
	if (tepTong > 0)
	{
		double r = (double)tepDa / (double)tepTong;
		self.thanh.hidden = NO;
		self.thanh.progress = (float)(r > 1.0 ? 1.0 : r);
		self.chiTiet.text = [NSString stringWithFormat:@"%@ — %.1f%% (%@ / %@ MB)",
		                     ten ?: @"", r * 100.0, JxMb(tepDa), JxMb(tepTong)];
	}
	else
	{
		self.thanh.hidden = YES;
		self.chiTiet.text = ten ?: @"";
	}

	if (tong > 0)
	{
		double r = (double)da / (double)tong;
		self.thanh2.hidden = NO;
		self.thanh2.progress = (float)(r > 1.0 ? 1.0 : r);
		self.nhanTong.text = [NSString stringWithFormat:@"Tổng: %.1f%%  (%@ / %@ MB)",
		                      r * 100.0, JxMb(da), JxMb(tong)];
	}
	else
	{
		self.thanh2.hidden = YES;
		self.nhanTong.text = @"";
	}

	int g = (int)(giay < 0 ? 0 : giay);
	double td = giay > 0.5 ? (double)da / giay : 0;
	self.dongHo.text = [NSString stringWithFormat:@"%02d:%02d:%02d   %.1f MB/s",
	                    g / 3600, (g / 60) % 60, g % 60, td / (1024.0 * 1024.0)];
}

// Chuyen man tai sang trang thai THONG BAO: giau thanh tien do, hien loi cho nguoi choi (kem dong ky thuat co nho)
// va nut (ten rong = khong nut).
- (void)hienLoi:(NSString*)loi kyThuat:(NSString*)kyThuat nut:(NSString*)tenNut
{
	self.nhan.text = @"Không tải được dữ liệu";
	self.thanh.hidden = YES;
	self.thanh2.hidden = YES;
	self.nhanTong.text = @"";
	self.chiTiet.numberOfLines = 0;
	self.chiTiet.text = loi ?: @"";
	self.kyThuat.text = kyThuat ?: @"";
	[self.nut setTitle:tenNut forState:UIControlStateNormal];
	self.daBam = NO;
	self.nut.hidden = !(tenNut.length > 0);
}

// Ve lai trang thai dang tai (goi truoc moi lan thu lai / sau khi bam Tai xuong).
- (void)veLaiDangTai
{
	self.nut.hidden = YES;
	self.thanh.hidden = NO;  self.thanh.progress = 0;
	self.thanh2.hidden = NO; self.thanh2.progress = 0;
	self.chiTiet.numberOfLines = 2;
	self.nhan.text = @"Chuẩn bị dữ liệu";
	self.chiTiet.text = @"";
	self.kyThuat.text = @"";
	self.nhanTong.text = @"";
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
// khong sua noi dung duoc - muon ep cap nhat chi can sua tep roi ky lai.
// [16/09] Dinh chinh: chu ky KHONG co dau thoi gian nen mot cap (manifest, chu ky) CU van hop le -> ke dung giua
// HA nguong duoc bang cach phat lai cap cu. Cong that van phai o may chu game luc dang nhap.
//
// LUU Y THAT: day la cong cho nguoi choi NGAY TINH. Client bi sua ruot thi bo qua duoc buoc nay.
// Cong THAT phai nam o may chu (tu choi client qua cu luc dang nhap) - viec do con lai.
// [16/09] Bo tai goi ham nay TRUOC khi tai hang GB (JxLamViecThat, buoc 2b); main() goi lai sau khi dong bo lam lop cuoi.
// Luat van hanh (phan bien 16/09): KHONG nang phienban.txt tren kho vuot ban dang "Waiting for Review / In Review";
// chi nang sau khi ban moi "Ready for Sale" - khong thi nguoi duyet cai xong gap man chan nay, khong loi thoat.
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
				"Bản game này đã cũ.\n\nBạn đang dùng bản %d, máy chủ yêu cầu từ bản %d trở lên.\n"
				"Hãy cập nhật ứng dụng trên App Store rồi mở lại.\n"
				"(This version of the app is outdated. Please update it from the App Store.)", nPhienBanApp, nCan);
		NSLog(@"[PHIENBAN] app %d < yeu cau %d -> chan", nPhienBanApp, nCan);
		return 1;
	}
}

// [IOS-PHIENBAN 16/09] main() goi TRUOC JxTaiDuLieu_Chay de bo tai kiem phienban.txt som (buoc 2b). Ham rieng de giu
// nguyen chu ky 4 tham so cua JxTaiDuLieu_Chay (macos/JxMacMain.cpp:92 van goi duoc; ben do khong dat = khong kiem som).
extern "C" void JxTaiDuLieu_DatPhienBanApp(int nPhienBanApp)
{
	s_nPhienBanApp = nPhienBanApp;
}

// ---------------------------------------------------------------- API cho C++
// pszGoc: mot hay nhieu dia chi kho (cach nhau bang dau cach = cac guong, thu lan luot).
// Tra ve 0 neu du lieu da san sang (pszLoi co the mang canh bao "khong kiem tra duoc cap nhat" de ghi log),
// khac 0 neu that bai (pszLoi mo ta). Tren iOS co giao dien thi KHONG tra ve loi: cho bam Thu lai, hoac chan han (A).
extern "C" int JxTaiDuLieu_Chay(const char* pszThuMuc, const char* pszGoc, char* pszLoi, int nLoi)
{
	@autoreleasepool {
		NSString* thuMuc = [NSString stringWithUTF8String:pszThuMuc ? pszThuMuc : ""];
		NSString* goc    = [NSString stringWithUTF8String:pszGoc ? pszGoc : ""];
		NSMutableArray<NSString*>* cacGoc = [NSMutableArray array];
		for (NSString* g in [goc componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]])
			if (g.length) [cacGoc addObject:([g hasSuffix:@"/"] ? g : [g stringByAppendingString:@"/"])];
		if (!thuMuc.length || !cacGoc.count) { if (pszLoi && nLoi) snprintf(pszLoi, nLoi, "thieu tham so"); return 2; }
		if (pszLoi && nLoi) pszLoi[0] = 0;

		[[NSFileManager defaultManager] createDirectoryAtPath:thuMuc withIntermediateDirectories:YES attributes:nil error:nil];
		JxKhongSaoLuu(thuMuc);

#if TARGET_OS_IPHONE
		// [IOS-KHONGCHET 15/09] Cua so tao MOT lan, dung cho ca cac lan thu lai. Truoc day no bi
		// an di ngay truoc nhanh loi, ma no la tham chieu manh duy nhat -> giai phong luon man hinh.
		UIWindow* cua = nil; JxTaiMan* man = nil;
		if ([UIApplication sharedApplication])
		{
			cua = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];	// +1, giu toi cuoi ham
			man = [[JxTaiMan alloc] init];
			cua.rootViewController = man;	// cua so giu man
			JX_THA(man);	// [16/09 MRC] bo +1 cua alloc; man song theo cua so
			cua.windowLevel = UIWindowLevelAlert + 1;
			[cua makeKeyAndVisible];
			// Tai 8,5 GB mat hang gio; khong chan thi may tu khoa man hinh giua chung roi dut tai.
			// Ban Android da lam viec nay tu lau (FLAG_KEEP_SCREEN_ON trong TaiDuLieuActivity).
			[UIApplication sharedApplication].idleTimerDisabled = YES;
			s_coGiaoDien = YES;
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
			JxTaiTrangThai* tt = [[JxTaiTrangThai alloc] init];	// +1, release o cuoi moi vong
			tt.mucHienTai = @"Đang kết nối…";
			dispatch_async(dispatch_get_global_queue(QOS_CLASS_UTILITY, 0), ^{ JxLamViec(cacGoc, thuMuc, tt); });

			NSDate* batDau = [NSDate date];
#if TARGET_OS_IPHONE
			BOOL daHienHoi = NO;
#endif
			while (!tt.xong)
			{
				@autoreleasepool {
					[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
#if TARGET_OS_IPHONE
					if (man)
					{
						long long canBam = tt.coCanBam, daBam = tt.coDaBam, canHoi = tt.canHoi;
						double gy = -[batDau timeIntervalSinceNow];
						if (canHoi > 0 && !tt.daDongY)
						{	// [16/09] dieu 4.2.3(ii): noi dung luong, cho bam "Tai xuong" (mot nut, khong bo qua)
							if (!daHienHoi)
							{
								[man hienLoi:[NSString stringWithFormat:
									@"Cần tải %@ dữ liệu game để chơi. Nên dùng Wi-Fi.\n(%@ of game data must be downloaded before you can play. Wi-Fi recommended.)",
									JxDungLuong(canHoi), JxDungLuong(canHoi)]
								     kyThuat:@"" nut:@"Tải xuống"];
								man.nhan.text = @"Cần tải dữ liệu";
								daHienHoi = YES;
							}
							else if (man.daBam)
							{
								[man veLaiDangTai];
								batDau = [NSDate date];	// dong ho tinh tu luc bat dau tai that
								tt.daDongY = YES;
							}
						}
						else if (canBam > 0 && daBam < canBam)
						{	// buoc kiem md5 bo du lieu da co (chi lam mot lan)
							man.nhan.text = @"Kiểm tra dữ liệu đã có";
							[man veTienDo:@"Đang kiểm tra dữ liệu…" tepDa:daBam tepTong:canBam
							          tong:0 da:0 giay:gy];
						}
						else
						{
							man.nhan.text = @"Đang tải dữ liệu";
							[man veTienDo:(tt.mucHienTai ?: @"")
							        tepDa:tt.coTepDaTai tepTong:tt.coTepTong
							         tong:tt.coTong     da:tt.coDaTai  giay:gy];
						}
					}
#endif
				}
			}

			if (!tt.loi.length)
			{	// xong xuoi (C, B) hoac D
				if (tt.canhBao.length)
				{
					NSLog(@"[IOS-TAI] D: %@", tt.canhBao);
					if (pszLoi && nLoi) snprintf(pszLoi, nLoi, "%s", tt.canhBao.UTF8String);
#if TARGET_OS_IPHONE
					if (man)
					{	// bao nhe ~1,5 s roi vao game; khong nut
						[man hienLoi:tt.canhBao kyThuat:@"" nut:@""];
						man.nhan.text = @"Không kiểm tra được cập nhật";
						NSDate* het = [NSDate dateWithTimeIntervalSinceNow:1.5];
						while ([het timeIntervalSinceNow] > 0)
						{
							@autoreleasepool {
								[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
							}
						}
					}
#endif
				}
				JX_THA(tt);
				break;
			}

			NSLog(@"[IOS-TAI] loi: %@%@%@", tt.loi, tt.loiNguoiDung.length ? @" | " : @"", tt.loiNguoiDung ?: @"");
			if (pszLoi && nLoi) snprintf(pszLoi, nLoi, "%s", (tt.loiNguoiDung.length ? tt.loiNguoiDung : tt.loi).UTF8String);
#if TARGET_OS_IPHONE
			if (man)
			{	// Co man hinh -> KHONG bao gio tra ve loi: cho nguoi dung bam thu lai, hoac chan han.
				NSString* loiND = tt.loiNguoiDung.length ? tt.loiNguoiDung
					: @"Tải dữ liệu bị lỗi. Hãy bấm Thử lại.\n(Download failed. Tap Retry.)";
				if (tt.chan)
				{	// A: ban app qua cu - khong co gi de thu lai, chan mai (nguoi choi phai cap nhat app)
					[man hienLoi:loiND kyThuat:tt.loi nut:@""];
					man.nhan.text = @"Bản game đã cũ";
					for (;;)
					{
						@autoreleasepool {
							[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
						}
					}
				}
				[man hienLoi:loiND kyThuat:tt.loi nut:@"Thử lại"];
				while (!man.daBam)
				{
					@autoreleasepool {
						[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
					}
				}
				[man veLaiDangTai];
				JX_THA(tt);
				continue;
			}
#endif
			nKetQua = 1;	// ban macOS / khong co giao dien: giu nguyen hanh vi cu, ben goi tu xu ly
			JX_THA(tt);
			break;
		}

#if TARGET_OS_IPHONE
		if (cua)
		{
			[UIApplication sharedApplication].idleTimerDisabled = NO;
			cua.hidden = YES;
			JX_THA(cua); cua = nil; man = nil;
			s_coGiaoDien = NO;
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
		JX_THA(man);
		cua.windowLevel = UIWindowLevelAlert + 1;
		[cua makeKeyAndVisible];

		[man hienLoi:[NSString stringWithUTF8String:pszNoiDung ? pszNoiDung : ""]
		     kyThuat:@""
		         nut:[NSString stringWithUTF8String:(pszNut && *pszNut) ? pszNut : ""]];
		if (pszTieuDe && *pszTieuDe)
			man.nhan.text = [NSString stringWithUTF8String:pszTieuDe];

		while (!man.daBam)	// khong nut -> chan mai, nhung van hien thong bao
		{
			@autoreleasepool {
				[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
			}
		}
		cua.hidden = YES; JX_THA(cua); cua = nil;
		return 0;
	}
#else
	(void)pszTieuDe; (void)pszNoiDung; (void)pszNut;
	return -1;
#endif
}
