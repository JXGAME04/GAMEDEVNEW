//---------------------------------------------------------------------------
// [IOS-AN 11/09] "An game" tren iOS: day ung dung xuong nen, man hinh tro ve Home.
//
// iOS KHONG co ham chinh thuc nao cho ung dung tu day minh xuong nen (khac Android co
// moveTaskToBack(), macOS co [NSApp hide:]). Co hai duong di, thu lan luot:
//
//   1. -[UIApplication suspend] : phuong thuc NOI BO cua he dieu hanh, tac dung y het bam
//      nut Home. Day la cach cac app "an duoc" dang lam. Goi gian tiep qua ten chuoi nen
//      trinh bien dich khong can khai bao. Neu Apple bo phuong thuc nay o ban iOS moi thi
//      respondsToSelector tra ve khong va ta rot xuong cach 2, KHONG chet ung dung.
//      LUU Y PHAT HANH: day la ham noi bo, dua len App Store se bi tu choi khi duyet.
//      Voi ban tu cai (chung chi rieng / doanh nghiep / TestFlight noi bo) thi khong sao.
//
//   2. Mo mot dia chi ngoai : he se chuyen sang Safari, game xuong nen. Hoan toan hop le
//      nhung nguoi khac thay Safari bat len, khong "sach" bang cach 1.
//
// KHONG lam duoc, khong co duong nao: giau BIEU TUONG app khoi man hinh chinh. iOS khong
// mo ham do cho ung dung. Chi nguoi dung tu giau vao Thu vien ung dung (iOS 18, mo bang
// Face ID), hoac dat ho so cau hinh quan tri thiet bi.
//
// Tep nay la ObjC++ THUAN: khong include header nao cua game (xem ghi chu o JxIosDuongDan.mm
// ve va cham kieu BOOL).
//---------------------------------------------------------------------------
#import <Foundation/Foundation.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

// Kiem truoc xem may nay co day duoc xuong nen khong (ghi vao log luc khoi dong).
// 1 = co -[UIApplication suspend]; 0 = khong, se phai mo dia chi ngoai.
extern "C" int JxIosAnGame_Co(void)
{
#if TARGET_OS_IPHONE
	@autoreleasepool {
		UIApplication* app = [UIApplication sharedApplication];
		return (app && [app respondsToSelector:NSSelectorFromString(@"suspend")]) ? 1 : 0;
	}
#else
	return NSApp ? 1 : 0;
#endif
}

// Tra ve: 1 = da day xuong nen bang cach 1, 2 = bang cach 2, 0 = khong lam duoc.
extern "C" int JxIosAnGame(const char* pszDiaChiDuPhong)
{
#if TARGET_OS_IPHONE
	@autoreleasepool {
		UIApplication* app = [UIApplication sharedApplication];
		if (!app)
			return 0;

		SEL sel = NSSelectorFromString(@"suspend");
		if ([app respondsToSelector:sel])
		{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
			[app performSelector:sel];
#pragma clang diagnostic pop
			NSLog(@"[IOS-AN] da day game xuong nen (suspend)");
			return 1;
		}

		// Cach 2: mo dia chi ngoai -> he chuyen sang Safari, game xuong nen.
		NSString* s = (pszDiaChiDuPhong && *pszDiaChiDuPhong)
			? [NSString stringWithUTF8String:pszDiaChiDuPhong] : @"https://www.google.com/";
		NSURL* u = [NSURL URLWithString:s];
		if (u && [app canOpenURL:u])
		{
			[app openURL:u options:@{} completionHandler:nil];
			NSLog(@"[IOS-AN] khong co suspend, mo %@ de day game xuong nen", s);
			return 2;
		}
		NSLog(@"[IOS-AN] khong day duoc game xuong nen");
		return 0;
	}
#else
	// macOS: co ham CHINH THUC, an cua so ung dung y nhu bam Cmd+H.
	(void)pszDiaChiDuPhong;
	@autoreleasepool {
		if (NSApp) { [NSApp hide:nil]; NSLog(@"[MAC-AN] da an cua so game"); return 1; }
	}
	return 0;
#endif
}
