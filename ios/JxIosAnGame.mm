//---------------------------------------------------------------------------
// [IOS-AN 16/09] KHONG CON DUOC DICH CHO iOS. Chu chot 16/09: bo han nut "an game" tren Apple (UiPlayerBar.cpp khong
// AddChild trong rao JX_APPLE, ios/CMakeLists.txt khong liet ke tep nay). Giu tep vi macos/CMakeLists.txt:162 con liet ke
// (ban macOS chu da bo, khong sua). Muon khoi phuc thi phai xem lai dieu 2.5.1 / 2.3.1(a) truoc - xem BANGIAO_PHIENSAU_IOS_1609.md §6.
//
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
//      [IOS-PHATHANH 16/09] Cach 1 CHI duoc dich khi JX_IOS_NOI_BO=1 (cay ban thu, xem ios/CMakeLists.txt). Ban App
//      Store (JX_IOS_KHOA_NOI_BO=OFF) khong con ma lan chuoi "suspend" trong nhi phan - ios/dung_ban_phat_hanh.sh kiem
//      bang strings. Khi do nut "an game" chi con cach 2. Phan bien 16/09 de nghi bo han nut nay tren Apple (dieu
//      2.3.1(a): nut bi giau, khong nhan, bam ra Safari) - CHU QUYET; bo cuc mobile mac dinh da giau nut
//      (android/du_lieu_ghi_de/ui/ui3/uitoado_macdinh.ini) nhung nguoi choi bat lai duoc qua "Sua giao dien".
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
#if JX_IOS_NOI_BO
	@autoreleasepool {
		UIApplication* app = [UIApplication sharedApplication];
		return (app && [app respondsToSelector:NSSelectorFromString(@"suspend")]) ? 1 : 0;
	}
#else
	return 0;	// [IOS-PHATHANH 16/09] ban phat hanh: khong dung API noi bo, chi con cach 2
#endif
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

#if JX_IOS_NOI_BO	// [IOS-PHATHANH 16/09] cach 1 chi o cay ban thu (xem dau tep)
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
#endif

		// Cach 2: mo dia chi ngoai -> he chuyen sang Safari, game xuong nen.
		NSString* s = (pszDiaChiDuPhong && *pszDiaChiDuPhong)
			? [NSString stringWithUTF8String:pszDiaChiDuPhong] : @"https://www.google.com/";
		NSURL* u = [NSURL URLWithString:s];
		if (u && [app canOpenURL:u])
		{
			[app openURL:u options:@{} completionHandler:nil];
			NSLog(@"[IOS-AN] khong day truc tiep xuong nen duoc, mo %@ de game xuong nen", s);
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
