//---------------------------------------------------------------------------
// [IOS-LOG 13/09] Gui nhat ky cua ban iOS ve MAY CHU TREN PC, dung chung cho voi ban Android.
//
// Chu 13/09: "co cach nao toi test ios ma log ve tren pc win de phien toi uu kia lay log phan tich".
// Ha tang da co san, khong phai lam moi: android/may_chu_tai_du_lieu.py nhan
//     POST /nhatky?may=..&phien=..&tep=..     (than = PHAN MOI cua tep)
// roi noi vao  <--nhat-ky, mac dinh D:\jx1_android_log>\<may>_<phien>\<tep>.
// Ban Android da gui kieu nay (JxDoNhip.java). Ban nay lam y het cho iOS nen log hai nen
// nam CANH NHAU trong cung mot thu muc, phien toi uu doc mot cho.
//
// Cach lam giong ban Android tung diem:
//   - Moi 10 giay gui phan MOI cua tung tep (nho vi tri da gui, khong gui lai tu dau).
//   - Vi tri ban dau = do dai tep luc mo app, nen log cua cac lan chay truoc KHONG bi gui lai.
//   - Dia chi may chu lay tu dong dau <Documents>/may_chu_nhatky.txt; khong co thi moi lay may_chu_tai.txt.
//     [IOS-PHATHANH 16/09] CHI o cay ban thu (JX_IOS_NOI_BO=1). Ban App Store (JX_IOS_KHOA_NOI_BO=OFF) khong doc hai
//     tep nay va khong gui nhat ky di dau: duong nay khong kiem chu ky, va nguoi choi that khong co may chu PC de nhan.
//     [IOS-LOG 14/09] Chu chon tep RIENG: tao may_chu_tai.txt la bat bo tai dong bo du lieu moi lan mo app,
//     ma bo tai se ghi de config.ini rieng cua iPhone bang ban cua PC.
//   - CHI tien vi tri khi may chu nhan THAT (2xx): mat mang giua chung thi lan sau gui lai,
//     khong thung log - day la cho de sai nhat neu lam au.
//   - Gui not mot lan khi app ra nen, vi iOS dong bang app ngay sau do.
//
// [IOS-LOG 14/09 SUA SAP] Du an Xcode do CMake sinh ra dich .mm KHONG CO ARC (khong co -fobjc-arc). Ban 13/09 viet
// nhu co ARC: gan doi tuong autorelease vao bien static roi thoat @autoreleasepool -> doi tuong bi giai phong,
// 10 s sau hen gio goi vao con tro treo: 4 lan sap tren iPhone 14/09 11:13-11:16 (SIGSEGV objc_msgSend /
// doesNotRecognizeSelector tai JxGuiMotTep). Nay:
//   - moi doi tuong song suot doi app deu duoc GIU (JX_GIU hoac alloc/init) - van dung neu sau nay bat ARC;
//   - s_daGui / s_dangGui CHI dung tren hang doi noi tiep s_hangDoi (truoc day ham xong cua NSURLSession ghi
//     s_daGui tu luong khac trong luc hen gio doc no - NSMutableDictionary khong an toan luong);
//   - khong gui chong: tep con yeu cau chua xong thi doi lan sau (tranh mot doan bi noi hai lan tren PC);
//   - ghi MOT dong vao jx_ios.log lan dau may chu nhan / lan dau gui hong -> kiem duoc tu xa ma khong can PC.
//
// Tep nay la ObjC++ THUAN: khong include header nao cua game (va cham kieu BOOL - xem JxIosDuongDan.mm).
//---------------------------------------------------------------------------
#import <Foundation/Foundation.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif
#include <stdio.h>
#include <sys/stat.h>
#include <sys/sysctl.h>

#define JX_NHATKY_GIAY	10.0
#define JX_NHATKY_TOIDA	(4LL * 1024 * 1024)	// toi da moi lan gui cua mot tep (may chu tu choi than > 16 MB)

#if __has_feature(objc_arc)
#define JX_GIU(x)	(x)
#else
#define JX_GIU(x)	[(x) retain]
#endif

static NSString*            s_thuMuc  = nil;
static NSString*            s_urlGoc  = nil;		// ".../nhatky?may=..&phien=..&tep="
static NSMutableDictionary* s_daGui   = nil;		// ten tep -> so byte da gui duoc   (CHI dung tren s_hangDoi)
static NSMutableSet*        s_dangGui = nil;		// tep dang co yeu cau chua xong   (CHI dung tren s_hangDoi)
static NSURLSession*        s_phien   = nil;
static dispatch_queue_t     s_hangDoi = nil;
static dispatch_source_t    s_hen     = nil;
static int                  s_daBaoNhan = 0, s_daBaoHong = 0;	// moi loai chi ghi jx_ios.log mot lan

// Doi xung voi TEP[] cua JxDoNhip.java, doi jx_android.log -> jx_ios.log va them hai tep rieng cua ban nay.
// [IOS-LOG 14/09] scripterror.log viet thuong: tren iPhone ten tep trong thu muc du lieu da ha chu thuong,
// ma he tep iOS phan biet hoa thuong (xem JxIosMain.cpp [IOS-DUONGDAN]) -> "ScriptError.log" khong bao gio thay.
static NSArray* JxDanhSachTep(void)
{
	return @[ @"jx_nhip.log", @"jx_ios.log", @"jx_paint.log", @"jx_rep3.log",
	          @"jx_mail.log", @"jx_tep_dung.log", @"scripterror.log" ];
}

static NSString* JxTenMay(void)
{
	char sz[64] = ""; size_t n = sizeof(sz);
	if (sysctlbyname("hw.machine", sz, &n, NULL, 0) != 0 || !sz[0])
		strcpy(sz, "iphone");
	return [NSString stringWithFormat:@"ios_%s", sz];	// vi du ios_iPhone17,2 - nhin thu muc la biet ngay khong phai may Android
}

// Ghi mot dong vao <thu muc du lieu>/jx_ios.log (JxIosLog cua JxIosMain.cpp la static nen tu mo tep)
static void JxGhiIosLog(NSString* dong)
{
	FILE* f = fopen([s_thuMuc stringByAppendingPathComponent:@"jx_ios.log"].fileSystemRepresentation, "a");
	if (!f) return;
	fprintf(f, "%s\n", dong.UTF8String);
	fclose(f);
}

// CHI chay tren s_hangDoi
static void JxGuiMotTep(NSString* ten)
{
	if ([s_dangGui containsObject:ten])
		return;		// yeu cau truoc cua tep nay chua xong -> lan sau gui tiep, khong gui chong mot doan hai lan
	NSString* p = [s_thuMuc stringByAppendingPathComponent:ten];
	struct stat st;
	if (stat(p.fileSystemRepresentation, &st) != 0)
		return;
	const long long co = (long long)st.st_size;
	const long long da = [s_daGui[ten] longLongValue];
	if (co <= da)
	{
		if (co < da) s_daGui[ten] = @(co);	// tep bi cat / ghi lai tu dau -> nhan lai tu do
		return;
	}
	NSFileHandle* f = [NSFileHandle fileHandleForReadingAtPath:p];
	if (!f) return;
	@try { [f seekToFileOffset:(unsigned long long)da]; }
	@catch (NSException* e) { [f closeFile]; return; }
	NSData* d = [f readDataOfLength:(NSUInteger)MIN(co - da, JX_NHATKY_TOIDA)];
	[f closeFile];
	if (!d.length) return;

	NSString* su = [s_urlGoc stringByAppendingString:
		[ten stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet alphanumericCharacterSet]]];
	NSMutableURLRequest* rq = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:su]
	                                                 cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
	                                             timeoutInterval:15.0];
	rq.HTTPMethod = @"POST";
	rq.HTTPBody   = d;
	[rq setValue:@"application/octet-stream" forHTTPHeaderField:@"Content-Type"];
	const long long nGui = (long long)d.length;
	const long long moi  = da + nGui;
	[s_dangGui addObject:ten];
	[[s_phien dataTaskWithRequest:rq completionHandler:^(NSData* du, NSURLResponse* r, NSError* e) {
		(void)du;
		const long ma = [r isKindOfClass:[NSHTTPURLResponse class]] ? (long)((NSHTTPURLResponse*)r).statusCode : 0;
		const BOOL ok = !e && ma >= 200 && ma < 300;
		NSString* loi = ok ? nil : (e ? e.localizedDescription : [NSString stringWithFormat:@"HTTP %ld", ma]);
		// ham xong chay tren luong cua NSURLSession -> quay ve s_hangDoi moi dung s_daGui / s_dangGui
		dispatch_async(s_hangDoi, ^{ @autoreleasepool {
			[s_dangGui removeObject:ten];
			if (ok)
			{
				s_daGui[ten] = @(moi);
				if (!s_daBaoNhan)
				{
					s_daBaoNhan = 1;
					JxGhiIosLog([NSString stringWithFormat:@"[IOS-LOG] may chu PC da nhan lan dau: %@ +%lld B (HTTP %ld)", ten, nGui, ma]);
				}
			}
			else if (!s_daBaoHong)
			{
				s_daBaoHong = 1;
				JxGhiIosLog([NSString stringWithFormat:@"[IOS-LOG] gui hong lan dau: %@ (%@) - lan sau gui lai tu vi tri cu", ten, loi]);
			}
		} });
	}] resume];
}

// CHI chay tren s_hangDoi
static void JxGuiHet(void)
{
	if (!s_urlGoc) return;
	for (NSString* t in JxDanhSachTep())
		JxGuiMotTep(t);
}

// Tra 1 neu da bat. Goi MOT lan, sau khi da chot thu muc du lieu.
extern "C" int JxNhatKy_Bat(const char* pszThuMuc)
{
#if !JX_IOS_NOI_BO
	(void)pszThuMuc;
	return 0;	// [IOS-PHATHANH 16/09] ban phat hanh: xem ghi chu dau tep
#else
	@autoreleasepool {
		if (s_hen || !pszThuMuc || !*pszThuMuc)
			return 0;
		NSString* thuMuc = [NSString stringWithUTF8String:pszThuMuc];

		// [IOS-LOG 14/09] tep rieng cua nhat ky truoc (khong keo theo bo tai du lieu), roi moi toi tep cua bo tai
		NSString* goc = nil;
		for (NSString* ten in @[ @"may_chu_nhatky.txt", @"may_chu_tai.txt" ])
		{
			NSString* s = [NSString stringWithContentsOfFile:[thuMuc stringByAppendingPathComponent:ten]
			                                        encoding:NSUTF8StringEncoding error:nil];
			s = [[[s componentsSeparatedByString:@"\n"] firstObject]
			     stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
			if (s.length) { goc = s; break; }
		}
		if (!goc.length)
		{
			NSLog(@"[IOS-LOG] khong co may_chu_nhatky.txt / may_chu_tai.txt -> khong gui nhat ky");
			return 0;
		}
		if (![goc hasSuffix:@"/"]) goc = [goc stringByAppendingString:@"/"];

		NSDateFormatter* df = [[NSDateFormatter alloc] init];
		df.dateFormat = @"MMdd_HHmmss";
		NSString* phien = [df stringFromDate:[NSDate date]];
#if !__has_feature(objc_arc)
		[df release];
#endif
		NSString* url = [NSString stringWithFormat:@"%@nhatky?may=%@&phien=%@&tep=", goc, JxTenMay(), phien];

		// [IOS-LOG 14/09 SUA SAP] bien static song suot doi app -> PHAI giu (xem dau tep)
		s_thuMuc  = JX_GIU(thuMuc);
		s_urlGoc  = JX_GIU(url);
		s_daGui   = [[NSMutableDictionary alloc] init];
		s_dangGui = [[NSMutableSet alloc] init];

		// Vi tri ban dau = do dai hien tai: chi gui phan sinh ra TU LUC NAY.
		for (NSString* t in JxDanhSachTep())
		{
			struct stat st;
			NSString* p = [s_thuMuc stringByAppendingPathComponent:t];
			s_daGui[t] = @(stat(p.fileSystemRepresentation, &st) == 0 ? (long long)st.st_size : 0LL);
		}

		NSURLSessionConfiguration* cf = [NSURLSessionConfiguration ephemeralSessionConfiguration];
		cf.timeoutIntervalForRequest = 15.0;
		NSURLSession* ss = [NSURLSession sessionWithConfiguration:cf];
		s_phien   = JX_GIU(ss);
		s_hangDoi = dispatch_queue_create("jx-nhatky", DISPATCH_QUEUE_SERIAL);	// ham create -> da so huu, khong giu them

		s_hen = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, s_hangDoi);
		dispatch_source_set_timer(s_hen,
			dispatch_time(DISPATCH_TIME_NOW, (int64_t)(JX_NHATKY_GIAY * NSEC_PER_SEC)),
			(uint64_t)(JX_NHATKY_GIAY * NSEC_PER_SEC), NSEC_PER_SEC);
		dispatch_source_set_event_handler(s_hen, ^{ @autoreleasepool { JxGuiHet(); } });
		dispatch_resume(s_hen);

#if TARGET_OS_IPHONE
		// iOS dong bang app ngay khi ra nen -> gui not truoc khi bi dong bang (dua sang s_hangDoi, khong dung s_daGui tu luong chinh)
		[[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillResignActiveNotification
			object:nil queue:nil usingBlock:^(NSNotification* n) { (void)n; dispatch_async(s_hangDoi, ^{ @autoreleasepool { JxGuiHet(); } }); }];
#endif
		NSLog(@"[IOS-LOG] gui nhat ky ve %@nhatky moi %.0f s (may=%@, phien=%@)",
		      goc, JX_NHATKY_GIAY, JxTenMay(), phien);
		return 1;
	}
#endif
}

extern "C" void JxNhatKy_GuiNgay(void)
{
	if (s_hangDoi) dispatch_async(s_hangDoi, ^{ @autoreleasepool { JxGuiHet(); } });
}
