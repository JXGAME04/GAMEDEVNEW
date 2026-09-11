// [ANDROID 09/09 DPG] Activity cua JX1 Mobile = SDLActivity + AN HAN THANH HE THONG NGAY TU onCreate.
//
// Vi sao phai co lop nay:
//   SDLActivity.onCreate() goi setWindowStyle(false) -> them co FLAG_FORCE_NOT_FULLSCREEN, tuc la BAT thanh
//   trang thai, du theme cua ung dung la Theme.NoTitleBar.Fullscreen. Mai den khi ma C goi
//   SDL_SetWindowFullscreen(true) (trong KSdlApp::Init) thi Android moi an thanh trang thai, nhung luc do
//   be mat ve da tao roi -> cua so bi doi co giua chung (vi du 1040x604 -> 1040x568 -> 1040x604).
//   Chot do phan giai giua luc do thi khung ve lech, chu mo.
//
// O day xin toan man hinh NGAY o onCreate (truoc khi be mat duoc tao) nen co cua so on dinh tu dau,
// va xin lai moi lan lay lai tieu diem (Android tra thanh trang thai ve khi ung dung ra truoc).
//
// KHONG bao gio goi setWindowStyle(false) khi da chay: xem BANGIAO_ANDROID_PHA4_0809.md muc 3.1
// ("BAY DA TRA GIA") - Android tao lai Activity, game khoi dong vong lap.
package vn.jx1.mobile;

import android.graphics.Insets;
import android.os.Build;
import android.os.Bundle;
import android.view.DisplayCutout;
import android.view.View;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowManager;

import org.libsdl.app.SDLActivity;

public class JxActivity extends SDLActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        datVungAnToanTheoCamera();
        anThanhHeThong();
        JxDoNhip.batDau(this);      // [DONHIP 12/09] ban do nhip ve: chi chay khi config.ini [DoNhip] Bat=1
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        JxDoNhip.guiNgay();         // [DONHIP 12/09] ra nen -> gui not log ve may chu tai
    }

    @Override
    protected void onDestroy()
    {
        JxDoNhip.dung();            // [DONHIP 12/09]
        super.onDestroy();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus)
            anThanhHeThong();
    }

    /** Toan man hinh "immersive sticky": an ca thanh trang thai lan thanh dieu huong, vuot vao thi hien tam roi tu an lai. */
    // [ANTOAN 13/09 b] VUNG AN TOAN CHI THEO CHO KHOET CAMERA (displayCutout). SDLSurface.onApplyWindowInsets cua SDL3 gop
    // ca systemBars + systemGestures (~30 dp moi ben cho cu chi lui) + tappableElement + cutout, nen tren Fold 7 giao dien
    // bi thut vao ca bon phia (~75 diem trai/phai, ~46 diem tren) - chu 13/09: "khi choi tren dien thoai no chua tu cang
    // chinh". Thay listener cua SDLSurface (moi View chi co mot listener) bang listener nay: chi lay cutout, bao SDL nhu cu
    // (SDL_SetWindowSafeAreaInsets -> JxSdl_LayVungAnToan). May khong co cutout -> 0,0,0,0 = neo theo mep man.
    private void datVungAnToanTheoCamera()
    {
        if (Build.VERSION.SDK_INT < 28 /* Android 9: DisplayCutout */ || mSurface == null)
            return;
        mSurface.setOnApplyWindowInsetsListener(new View.OnApplyWindowInsetsListener() {
            @Override
            public WindowInsets onApplyWindowInsets(View v, WindowInsets insets)
            {
                int l = 0, r = 0, t = 0, b = 0;
                if (Build.VERSION.SDK_INT >= 30 /* Android 11 (R) */)
                {
                    Insets c = insets.getInsets(WindowInsets.Type.displayCutout());
                    l = c.left; r = c.right; t = c.top; b = c.bottom;
                }
                else
                {
                    DisplayCutout c = insets.getDisplayCutout();
                    if (c != null)
                    {
                        l = c.getSafeInsetLeft(); r = c.getSafeInsetRight();
                        t = c.getSafeInsetTop(); b = c.getSafeInsetBottom();
                    }
                }
                SDLActivity.onNativeInsetsChanged(l, r, t, b);
                return insets;
            }
        });
    }

    private void anThanhHeThong()
    {
        Window window = getWindow();
        if (window == null)
            return;
        window.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        window.clearFlags(WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN);
        window.getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_FULLSCREEN |
                View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
                View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
        // Man hinh co tai tho (notch): cho ve tran ca phan tai, dung de he thong chua mot dai den.
        if (Build.VERSION.SDK_INT >= 28 /* Android 9 (Pie) */)
            window.getAttributes().layoutInDisplayCutoutMode =
                    WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        // [ANTOAN 13/09] Xin Android phat lai inset (tai tho / vung vuot he thong) -> SDLSurface.onApplyWindowInsets ->
        // SDL_SetWindowSafeAreaInsets. Can vi SDLActivity.setWindowStyle dat inset = 0 tren Android 11..14 sau khi
        // vao toan man hinh; bo cuc HUD (UiToaDo) neo theo vung an toan nay.
        window.getDecorView().requestApplyInsets();
    }
}
