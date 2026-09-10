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
// Goi cuoi moi khung ve: ve icon "noi chuyen" tren dau NPC doi thoai gan nhat.
void JxIconNpc_Ve();
int  JxIconNpc_Cham(int x, int y, int* pnX, int* pnY);	// [ANDROID 11/09 ICON b] cham trung icon Giao tiep -> toa do than NPC de bam

//---------------------------------------------------------------------------
// [ANDROID 09/09 KYNANG] Nut chon ky nang danh.
//
// Cham mot nut = chon ky nang do lam ky nang danh trai roi danh con dich gan nhat.
// Giu roi keo = ngam: huong keo quyet dinh danh con nao; tha ngon la danh.
// Cach lam theo lop KSkillRocker cua ban JX1 Mobile (D:\USVOLAM\Jx1mClientMobile).
//---------------------------------------------------------------------------

// (x, y) co trung mot nut ky nang khong. Tra ve so thu tu nut + 1, hoac 0.
int  JxKyNang_TrungNut(int x, int y);
// Dat ngon len nut thu (nNut - 1).
void JxKyNang_BatDau(int nNut, int x, int y);
// Ngon di chuyen - ngam huong danh.
void JxKyNang_Keo(int x, int y);
// [ANDROID 09/09 KYNANG I] Goi moi vong lap game: con de nut thi cu danh tiep,
// tai cho o xanh dang dung.
// [ANDROID 09/09 GAN] Bang ky nang goi vao day khi nguoi choi cham mot ky nang.
// Tra ve true = da gan vao o dang cho, ben goi khoi lam viec cua no nua.
bool JxKyNang_GanKyNang(unsigned int uGenre, unsigned int uId);
// [ANDROID 10/09 BANGCHON] bang 3 nut (gan chinh / gan phu / go) khi cham mot ky nang trong bang ky nang.
void JxKyNang_MoBangChon(unsigned int uGenre, unsigned int uId, int x, int y);
int  JxKyNang_ChamBangChon(int x, int y);	// 1 = bang dang mo da nhan cu cham nay
void JxKyNang_Nhip();
// Nha ngon = thoi danh. Tra ve true neu vua nha mot nut ky nang.
bool JxKyNang_Nha();
// Goi cuoi moi khung ve: ve bang nut + vach ngam + vong tron duoi chan con dang ngam.
void JxKyNang_Ve();
// [ANDROID 09/09 HUONGDI] Goi cuoi moi khung ve: mui ten nho duoi chan nhan vat
// theo huong dang di chuyen.
void JxHuongDi_Ve();

#endif // JX_ANDROID
#endif
