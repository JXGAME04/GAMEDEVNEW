//---------------------------------------------------------------------------
// [ANDROID 09/09 CAN] CAN DIEU KHIEN AO (virtual joystick) cho ban Android.
//
// Ban PC di chuyen bang cach bam chuot xuong dat. Tren dien thoai cach do rat kho danh nhau:
// ngon tay che mat cho muon di, va huong lai tinh theo cho nhan vat dang dung tren man hinh.
// Ban JX1 Mobile cua chu (D:\USVOLAM, lop HRocker) dung CAN DIEU KHIEN: moi khung goi
// iCoreShell::Goto(nDir, mode) voi nDir 0..63. Ban nay lam dung nhu vay, chi khac phan ve.
//
// Cach dung tren may: KEO ngon tay o NUA TRAI man hinh (khong phai tren giao dien) = can dieu khien;
// CHAM thi van la bam chuot trai nhu thuong (di mot buoc / danh / nhat do). Nho vay khong an mat
// thao tac nao cua ban PC ma van co can dieu khien.
//
// Chi bien dich khi JX_ANDROID.
//---------------------------------------------------------------------------
#ifndef JxCanDieuKhien_H
#define JxCanDieuKhien_H
#ifdef JX_ANDROID

// Diem (x, y) - toa do KHUNG VE - co nam trong vung dat can khong.
bool JxCan_TrongVung(int x, int y);

// Bat dau keo: (x0, y0) la cho dat ngon (tam can), (x, y) la cho ngon dang o.
void JxCan_BatDau(int x0, int y0, int x, int y);
// Ngon tay di chuyen.
void JxCan_Keo(int x, int y);
// Nha ngon (hoac bi cat ngang).
void JxCan_Nha();
// Dang cam can khong.
bool JxCan_DangCam();

// Goi moi vong lap game (KSdlApp::Run): day nhan vat di theo huong dang chi.
void JxCan_Nhip();
// Goi cuoi moi khung ve (UiShell::UiPaint): ve can len man hinh.
void JxCan_Ve();
// Goi cuoi moi khung ve: ve vong chon duoi chan muc tieu dang chon.
void JxVongChon_Ve();

#endif // JX_ANDROID
#endif
