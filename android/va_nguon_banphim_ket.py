# -*- coding: utf-8 -*-
"""[BANPHIM 12/09 KET] Sua "ban phim ao hien len ma khong an duoc" tren iOS.

LO HONG (co trong ma, khong phai doan):
  Wnds.cpp:285  khi mot cua so bi GO khoi he cua so:
        if (s_WndStation.pFocusWnd == pWnd)
            s_WndStation.pFocusWnd = NULL;
  Cho nay xoa con tro tieu diem NHUNG KHONG gui WND_M_KILL_FOCUS.
  Ma chi WND_M_KILL_FOCUS moi goi JxSdl_BanPhimAo(0) (WndEdit.cpp:250) de hen tat ban phim.
  => O nhap dang giu tieu diem ma cua so cua no bi dong (dong khung chat, dong bang co o nhap,
     doi man...) thi: tieu diem = NULL, ban phim VAN MO, va KHONG CO GI hen tat no nua.

Vi sao chi iOS moi ket:
  - Android: nguoi choi bam nut Back -> SDL StopTextInput -> JxSdl_BanPhimNhip (dong 798) thay
    SDL_TextInputActive = false roi dong bo lai. Co duong thoat.
  - iOS: KHONG co nut Back, ban phim cua SDL cung khong co phim an. Khong co duong thoat nao
    -> ban phim nam mai tren man hinh, dung nhu chu bao.

SUA: them luoi an toan trong JxSdl_BanPhimNhip(): ta da mo ban phim, ban phim con dang mo,
ma KHONG cua so nao giu tieu diem -> hen tat (dung dung co che hen 200 ms san co, khong tat
thang, de khong pha truong hop bam Enter nhay sang o ke tiep).
Trang thai "ban phim mo ma khong o nhap nao giu tieu diem" la trang thai HONG ro rang,
khong co ly do chinh dang nao de giu, nen luoi nay khong the cat nham.

Ban Windows KHONG lien quan: toan bo nam trong #ifdef JX_MOBILE.
"""
import os, sys

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MOC = b'BANPHIM 12/09 KET'

T1 = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'Elem', 'Wnds.cpp')
CU1 = (b'extern "C" void JxUi_BoTieuDiem(void)\n'
       b'{\n'
       b'\tWnd_SetFocusWnd(NULL);\n'
       b'}\n')
MOI1 = (CU1 +
        b'// [BANPHIM 12/09 KET] co cua so nao dang giu tieu diem nhap khong (1/0).\n'
        b'// KSdlApp dung de bat trang thai hong: ban phim ao con mo ma khong o nhap nao giu tieu diem\n'
        b'// (xay ra khi cua so chua o nhap bi GO - dong 285 xoa con tro tieu diem ma khong gui KILL_FOCUS).\n'
        b'extern "C" int JxUi_CoTieuDiem(void)\n'
        b'{\n'
        b'\treturn Wnd_GetFocusWnd() ? 1 : 0;\n'
        b'}\n')

T2 = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'KSdlApp.cpp')
CU2 = b'extern "C" void JxUi_BoTieuDiem(void);\t// [BANPHIM 14/09] Wnds.cpp\n'
MOI2 = CU2 + b'extern "C" int JxUi_CoTieuDiem(void);\t// [BANPHIM 12/09 KET] Wnds.cpp: 1 = co cua so dang giu tieu diem nhap\n'

CU3 = (b'\t\tif (s_bBanPhimDangMo && pWinKT && !SDL_TextInputActive(pWinKT))\n'
       b'\t\t{\n'
       b'\t\t\ts_bBanPhimDangMo = 0;\n'
       b'\t\t\tJxUi_BoTieuDiem();\n'
       b'\t\t\tg_DebugLog("[BANPHIM] IME da dong ngoai y game -> bo tieu diem o nhap");\n'
       b'\t\t}\n')
MOI3 = (CU3 +
        b'\t\t// [BANPHIM 12/09 KET] Chieu nguoc lai: ban phim con MO ma khong o nhap nao giu tieu diem.\n'
        b'\t\t// Xay ra khi cua so chua o nhap bi GO (Wnds.cpp:285 xoa con tro tieu diem ma KHONG gui\n'
        b'\t\t// WND_M_KILL_FOCUS, ma chi KILL_FOCUS moi hen tat ban phim). Android con nut Back de thoat,\n'
        b'\t\t// iOS thi khong co gi -> ban phim nam mai. Hen tat bang dung co che 200 ms san co.\n'
        b'\t\tif (s_bBanPhimDangMo && !s_uBanPhimTat && pWinKT && SDL_TextInputActive(pWinKT) && !JxUi_CoTieuDiem())\n'
        b'\t\t{\n'
        b'\t\t\ts_uBanPhimTat = SDL_GetTicks() + 200;\n'
        b'\t\t\tg_DebugLog("[BANPHIM] ban phim con mo ma khong o nhap nao giu tieu diem -> hen tat");\n'
        b'\t\t}\n')

def va(tep, cap):
    d = open(tep, 'rb').read()
    if MOC in d:
        print('  %s: da va roi' % os.path.basename(tep)); return 0
    for cu, moi in cap:
        n = d.count(cu)
        if n != 1:
            print('LOI: %s - tim thay %d cho khop, can 1' % (os.path.basename(tep), n)); return 1
        d = d.replace(cu, moi)
    open(tep, 'wb').write(d)
    print('  da va %s' % os.path.basename(tep)); return 0

sys.exit(va(T1, [(CU1, MOI1)]) or va(T2, [(CU2, MOI2), (CU3, MOI3)]))
