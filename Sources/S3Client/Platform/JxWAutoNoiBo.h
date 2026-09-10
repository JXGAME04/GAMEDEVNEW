//---------------------------------------------------------------------------
// [ANDROID 11/09 WAUTO B0] BANG WAuto TRONG GAME - buoc B0: DUONG ONG TRONG TIEN TRINH (chua co giao dien).
//
// WAuto.exe (ban PC) la mot tien trinh rieng: no dien struct autoData roi cu 54 ms nap mot goi IPCGameLoop vao
// vung nho chia se cua game + bao tin hieu; game (S3Client.cpp ProcIpcCommand) doc goi va goi ExtAutoLoop -> bo
// nao auto trong CoreShell.cpp (DT_/TK_/LD_/CT_/ST_/HD_). Tren Android khong co tien trinh ngoai, nhung vung nho
// chia se DA la calloc trong tien trinh va tin hieu la pthread cond (KPosixWin32.cpp), ProcIpcCommand van duoc
// goi moi khung hinh. Tep nay la BEN GUI nam ngay trong game: cung goi, cung duong, ben nhan khong doi mot dong.
//
// Cau hinh: APdata\<ma nhan vat>.dat - CUNG dinh dang va ten voi WAuto.exe (ghi nguyen struct autoData), nen tep
// APdata cua ban PC chep sang dien thoai la dung duoc. Chua co tep -> mac dinh LAN DAU giong WAuto.exe
// (LoadRoleData: danh, tam 1000, nhat do, uong thuoc theo 2/3 va 1/3 mau...) roi ghi ra tep ngay.
// Cong tac: config.ini [WAuto] Bat=1 (tam cho B0; tu B1 co nut BAT/TAT trong game).
// Lo trinh: LOTRINH_WAUTO_MOBILE_1109.md. Chi bien dich khi JX_ANDROID; khong nam trong vcxproj nao.
//---------------------------------------------------------------------------
#ifndef JxWAutoNoiBo_H
#define JxWAutoNoiBo_H
#ifdef JX_ANDROID
#include "../../Core/Src/ipc_shared.h"

// Goi moi vong lap game (KMyApp::GameLoop), NGAY TRUOC ProcIpcCommand() de goi vua nap duoc tieu thu cung khung.
void			JxWAuto_NhipVongLap();
// Bat / tat may auto. Tra ve trang thai moi. Doi trang thai -> gui PRT_TICKSTART (ATYPE_CLEAR) nhu WAuto.exe.
int				JxWAuto_Bat(int bBat);
int				JxWAuto_DangBat();
// Cau hinh dang dung cua nhan vat hien tai (B1+ doc/ghi thang vao day roi goi JxWAuto_LuuCauHinh).
autoData*		JxWAuto_CauHinh();
int				JxWAuto_LuuCauHinh();		// ghi APdata\<id>.dat; 1 = xong
int				JxWAuto_NapCauHinh();		// nap lai tu tep (tu goi khi doi nhan vat); 1 = co tep, 0 = mac dinh lan dau
unsigned int	JxWAuto_IdNhanVat();		// ma nhan vat cua cau hinh dang nap (0 = chua vao game)
// [ANDROID 11/09 WAUTO B2 i] nut ban tay: nhat ngay trong nMs mili giay (khong danh, chay toi nhat), ke ca khi auto dang tat
void			JxWAuto_NhatNgay(int nMs);
int				JxWAuto_DangNhat();

#endif // JX_ANDROID
#endif
