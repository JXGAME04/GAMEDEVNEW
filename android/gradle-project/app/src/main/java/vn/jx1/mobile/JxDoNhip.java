// [DONHIP 12/09] Ban DO NHIP VE tren dien thoai that - phan Java (phan C++: JxPerfHudAndroid.cpp, D3D9onGPUDev.cpp, S3Client.cpp).
// Chu: "Lay log tu Fold 7: toi dung chung mang nen ban cu up apk toi tai ve test roi ban ghi log ve may tinh toi".
//
// Chi chay khi config.ini (thu muc du lieu cua game) co [DoNhip] Bat=1. Viec:
//   1. jx_thietbi.log: may / SoC / Android, moi che do man hinh (co + tan so); roi moi 5 s: tan so man dang chay, che do, xoay,
//      nhiet (trang thai + headroom), pin (%, nhiet do, dong, ap -> cong suat). Man doi che do thi ghi ngay ([DOI]).
//   2. Moi 10 s gui PHAN MOI cua jx_nhip.log, jx_thietbi.log, jx_paint.log, jx_rep3.log, jx_android.log ve may chu tai:
//      POST <URL may chu tai>/nhatky?may=..&phien=..&tep=..  (android/may_chu_tai_du_lieu.py ghi vao D:\jx1_android_log\<may>_<phien>\).
// Loi gi cung nuot, khong bao gio lam sap game. Bat=0 (mac dinh) -> khong tao luong, khong mo mang.
package vn.jx1.mobile;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.display.DisplayManager;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.PowerManager;
import android.os.SystemClock;
import android.util.Log;
import android.view.Display;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

public final class JxDoNhip implements DisplayManager.DisplayListener
{
    private static final String TAG = "JxDoNhip";
    private static final String[] TEP = { "jx_nhip.log", "jx_thietbi.log", "jx_paint.log", "jx_rep3.log", "jx_android.log", "jx_mail.log" };   // [DAN 11/09] + jx_mail.log (SP_ClientLog / dau gia, phien giao dien nho)
    private static final long MAU_MS = 5000;                 // lay mau thiet bi
    private static final long GUI_MS = 10000;                // gui log
    private static final int KHOI_TOI_DA = 2 * 1024 * 1024;  // moi lan POST toi da 2 MB / tep

    private static JxDoNhip sMot;

    private final Context mUd;
    private final File mThu;                 // thu muc du lieu (noi game ghi log)
    private final String mUrlGui;            // http://.../nhatky?may=..&phien=..&tep=
    private final String mPhien;
    private final HandlerThread mLuong;
    private final Handler mH;
    private final Map<String, Long> mDaGui = new HashMap<>();
    private final SimpleDateFormat mGio = new SimpleDateFormat("HH:mm:ss.SSS", Locale.US);
    private DisplayManager mDm;
    private boolean mDung = false;

    /** goi trong JxActivity.onCreate: chi bat khi config.ini [DoNhip] Bat=1 */
    public static synchronized void batDau(Activity a)
    {
        if (sMot != null)
            return;
        try
        {
            File thu = thuMucDuLieu(a);
            if (thu == null || !batTrongIni(new File(thu, "config.ini")))
                return;
            sMot = new JxDoNhip(a.getApplicationContext(), thu, docUrl(a));
        }
        catch (Throwable t) { Log.w(TAG, "khong bat duoc ban do nhip", t); }
    }

    /** goi trong JxActivity.onPause: gui not log truoc khi ra nen */
    public static synchronized void guiNgay()
    {
        if (sMot != null)
            sMot.mH.post(sMot::gui);
    }

    /** goi trong JxActivity.onDestroy */
    public static synchronized void dung()
    {
        final JxDoNhip m = sMot;
        sMot = null;
        if (m == null)
            return;
        m.mH.post(() -> {
            m.mDung = true;
            try { if (m.mDm != null) m.mDm.unregisterDisplayListener(m); } catch (Throwable ignored) {}
            m.ghi("[DUNG] " + m.gio() + " app dong (onDestroy)");
            m.gui();
            m.mLuong.quitSafely();
        });
    }

    private JxDoNhip(Context ud, File thu, String url)
    {
        mUd = ud;
        mThu = thu;
        mPhien = new SimpleDateFormat("yyyyMMdd_HHmmss", Locale.US).format(new Date());
        mUrlGui = url + "nhatky?may=" + ma(sach(Build.MODEL)) + "&phien=" + ma(mPhien) + "&tep=";
        for (String t : TEP)
            mDaGui.put(t, new File(thu, t).length());     // chi gui phan MOI tu luc mo app (log cu cua cac lan truoc thi bo)
        mLuong = new HandlerThread("jx-donhip");
        mLuong.start();
        mH = new Handler(mLuong.getLooper());
        mH.post(() -> {
            try
            {
                mDm = (DisplayManager) mUd.getSystemService(Context.DISPLAY_SERVICE);
                if (mDm != null) mDm.registerDisplayListener(this, mH);
            }
            catch (Throwable ignored) {}
            ghiThietBi();
        });
        mH.postDelayed(this::lapMau, MAU_MS);
        mH.postDelayed(this::lapGui, GUI_MS);
        Log.i(TAG, "bat: thu muc " + thu + ", gui ve " + url + "nhatky, phien " + mPhien);
    }

    // ------------------------------------------------------------ thiet bi
    private void ghiThietBi()
    {
        try
        {
            StringBuilder sb = new StringBuilder();
            sb.append("[THIETBI] ").append(gio()).append(" phien=").append(mPhien)
              .append(" may=").append(Build.MANUFACTURER).append(' ').append(Build.MODEL).append(" (").append(Build.DEVICE).append(')');
            if (Build.VERSION.SDK_INT >= 31)
                sb.append(" soc=").append(Build.SOC_MANUFACTURER).append(' ').append(Build.SOC_MODEL);
            sb.append(" android=").append(Build.VERSION.RELEASE).append(" sdk=").append(Build.VERSION.SDK_INT)
              .append(" thu_muc=").append(mThu.getAbsolutePath()).append(" gui=").append(mUrlGui);
            ghi(sb.toString());
            Display d = manHinh();
            if (d != null)
            {
                StringBuilder m = new StringBuilder("[MANHINH] ").append(gio()).append(" che do ho tro:");
                for (Display.Mode md : d.getSupportedModes())
                    m.append(String.format(Locale.US, " id=%d %dx%d %.2f Hz;", md.getModeId(), md.getPhysicalWidth(), md.getPhysicalHeight(), md.getRefreshRate()));
                Display.Mode cur = d.getMode();
                m.append(String.format(Locale.US, " | dang dung id=%d %dx%d %.2f Hz, xoay=%d", cur.getModeId(), cur.getPhysicalWidth(), cur.getPhysicalHeight(), d.getRefreshRate(), d.getRotation()));
                ghi(m.toString());
            }
            ghiMau();
        }
        catch (Throwable t) { ghi("[LOI] ghiThietBi " + t); }
    }

    private void lapMau()
    {
        if (mDung) return;
        ghiMau();
        mH.postDelayed(this::lapMau, MAU_MS);
    }

    private void ghiMau()
    {
        try
        {
            StringBuilder sb = new StringBuilder("[MAU] ").append(gio()).append(" t=").append(SystemClock.elapsedRealtime());
            Display d = manHinh();
            if (d != null)
                sb.append(String.format(Locale.US, " man=%.2f Hz che_do=%d xoay=%d", d.getRefreshRate(), d.getMode().getModeId(), d.getRotation()));
            PowerManager pm = (PowerManager) mUd.getSystemService(Context.POWER_SERVICE);
            if (pm != null && Build.VERSION.SDK_INT >= 29)
                sb.append(" | nhiet=").append(pm.getCurrentThermalStatus());
            if (pm != null && Build.VERSION.SDK_INT >= 30)
                sb.append(String.format(Locale.US, " headroom=%.3f", pm.getThermalHeadroom(0)));
            Intent b = mUd.registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
            if (b != null)
            {
                int muc = b.getIntExtra(BatteryManager.EXTRA_LEVEL, -1), thang = b.getIntExtra(BatteryManager.EXTRA_SCALE, 100);
                int nhietDo = b.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, 0);      // phan muoi do C
                int mV = b.getIntExtra(BatteryManager.EXTRA_VOLTAGE, 0);
                int st = b.getIntExtra(BatteryManager.EXTRA_STATUS, 0);
                boolean sac = st == BatteryManager.BATTERY_STATUS_CHARGING || st == BatteryManager.BATTERY_STATUS_FULL;
                int uA = 0;
                BatteryManager bm = (BatteryManager) mUd.getSystemService(Context.BATTERY_SERVICE);
                if (bm != null) uA = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_CURRENT_NOW);
                double w = Math.abs((double) uA) / 1e6 * (mV / 1000.0);
                sb.append(String.format(Locale.US, " | pin=%d%% %.1fC %s dong=%d uA ap=%d mV p=%.2f W",
                        thang > 0 ? muc * 100 / thang : muc, nhietDo / 10.0, sac ? "SAC" : "khong_sac", uA, mV, w));
            }
            ghi(sb.toString());
        }
        catch (Throwable t) { ghi("[LOI] ghiMau " + t); }
    }

    private Display manHinh()
    {
        try { return mDm != null ? mDm.getDisplay(Display.DEFAULT_DISPLAY) : null; }
        catch (Throwable t) { return null; }
    }

    @Override public void onDisplayAdded(int id) {}
    @Override public void onDisplayRemoved(int id) {}

    @Override
    public void onDisplayChanged(int id)
    {
        if (id != Display.DEFAULT_DISPLAY)
            return;
        try
        {
            Display d = manHinh();
            if (d != null)
                ghi(String.format(Locale.US, "[DOI] %s t=%d man doi: che do id=%d %.2f Hz, xoay=%d", gio(), SystemClock.elapsedRealtime(),
                        d.getMode().getModeId(), d.getRefreshRate(), d.getRotation()));
        }
        catch (Throwable ignored) {}
    }

    // ------------------------------------------------------------ gui log
    private void lapGui()
    {
        if (mDung) return;
        gui();
        mH.postDelayed(this::lapGui, GUI_MS);
    }

    private void gui()
    {
        for (String t : TEP)
        {
            try
            {
                File f = new File(mThu, t);
                if (!f.isFile()) continue;
                long co = f.length();
                long da = mDaGui.containsKey(t) ? mDaGui.get(t) : 0L;
                if (co < da) da = 0;                          // tep bi tao lai / cat ngan -> gui tu dau
                while (co > da)
                {
                    int n = (int) Math.min(KHOI_TOI_DA, co - da);
                    byte[] b = new byte[n];
                    try (RandomAccessFile r = new RandomAccessFile(f, "r")) { r.seek(da); r.readFully(b); }
                    if (!post(t, b)) break;                   // may chu chua chay / mat mang: giu vi tri, lan sau gui tiep
                    da += n;
                    mDaGui.put(t, da);
                }
            }
            catch (Throwable e) { Log.w(TAG, "gui " + t + ": " + e); }
        }
    }

    private boolean post(String tep, byte[] b)
    {
        HttpURLConnection c = null;
        try
        {
            c = (HttpURLConnection) new URL(mUrlGui + ma(tep)).openConnection();
            c.setConnectTimeout(3000);
            c.setReadTimeout(10000);
            c.setDoOutput(true);
            c.setRequestMethod("POST");
            c.setFixedLengthStreamingMode(b.length);
            c.setRequestProperty("Content-Type", "application/octet-stream");
            try (OutputStream o = c.getOutputStream()) { o.write(b); }
            return c.getResponseCode() == 200;
        }
        catch (Throwable e) { return false; }
        finally { if (c != null) c.disconnect(); }
    }

    // ------------------------------------------------------------ tien ich
    private synchronized void ghi(String dong)
    {
        try (FileOutputStream o = new FileOutputStream(new File(mThu, "jx_thietbi.log"), true))
        {
            o.write((dong + "\n").getBytes(StandardCharsets.UTF_8));
        }
        catch (Throwable ignored) {}
    }

    private String gio() { synchronized (mGio) { return mGio.format(new Date()); } }

    private static String sach(String s)
    {
        if (s == null || s.isEmpty()) return "may";
        return s.replaceAll("[^A-Za-z0-9_.-]", "_");
    }

    private static String ma(String s)
    {
        try { return URLEncoder.encode(s, "UTF-8"); }
        catch (Exception e) { return s; }
    }

    /** thu muc du lieu: CUNG thu tu voi JxAndroidMain.cpp (JX_DATA_DIR, jx_data_dir.txt, thu muc app, thu muc chia se may ao ...) */
    private static File thuMucDuLieu(Context c)
    {
        List<String> cand = new ArrayList<>();
        String env = System.getenv("JX_DATA_DIR");
        if (env != null && !env.trim().isEmpty()) cand.add(env.trim());
        File app = c.getExternalFilesDir(null);
        if (app != null)
        {
            String d = dongDau(new File(app, "jx_data_dir.txt"));
            if (d != null && !d.isEmpty()) cand.add(d);
            cand.add(app.getAbsolutePath());
        }
        cand.add("/mnt/shared/Misc/jx1"); cand.add("/mnt/shared/Misc"); cand.add("/mnt/shared/Pictures/jx1");
        cand.add("/mnt/shared/Applications/jx1"); cand.add("/sdcard/jx1"); cand.add("/sdcard/Download/jx1"); cand.add("/storage/emulated/0/jx1");
        for (String s : cand)
            if (new File(s, "config.ini").isFile())
                return new File(s);
        return null;
    }

    /** URL may chu tai: dong dau cua <thu muc app>/tai_du_lieu.txt, khong co thi R.string.may_chu_tai (nhu TaiDuLieuActivity) */
    private static String docUrl(Context c)
    {
        File app = c.getExternalFilesDir(null);
        String u = app != null ? dongDau(new File(app, "tai_du_lieu.txt")) : null;
        if (u == null || !u.startsWith("http"))
            u = c.getString(R.string.may_chu_tai);
        return u.endsWith("/") ? u : u + "/";
    }

    private static String dongDau(File f)
    {
        if (!f.isFile()) return null;
        try (BufferedReader r = new BufferedReader(new InputStreamReader(new FileInputStream(f), StandardCharsets.UTF_8)))
        {
            String s = r.readLine();
            return s != null ? s.trim() : null;
        }
        catch (Exception e) { return null; }
    }

    /** config.ini: muc [DoNhip] co Bat=1 */
    private static boolean batTrongIni(File ini)
    {
        if (!ini.isFile()) return false;
        try (BufferedReader r = new BufferedReader(new InputStreamReader(new FileInputStream(ini), StandardCharsets.ISO_8859_1)))
        {
            String ln, muc = "";
            while ((ln = r.readLine()) != null)
            {
                ln = ln.trim();
                if (ln.startsWith("[") && ln.endsWith("]")) { muc = ln.substring(1, ln.length() - 1).trim(); continue; }
                if (!muc.equalsIgnoreCase("DoNhip") || ln.startsWith(";")) continue;
                int i = ln.indexOf('=');
                if (i > 0 && ln.substring(0, i).trim().equalsIgnoreCase("Bat"))
                    return ln.substring(i + 1).trim().startsWith("1");
            }
        }
        catch (Exception ignored) {}
        return false;
    }
}
