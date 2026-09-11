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

import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import org.libsdl.app.SDLActivity;

public class JxActivity extends SDLActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
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
