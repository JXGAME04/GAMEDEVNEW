// [ANDROID 12/09 TAI] Man hinh TAI DU LIEU truoc khi vao game (chu: "phan dang nhap co ong load du lieu nhu VNKU",
// "nguoi choi tai ve nhanh nhat khong ton thoi gian").
//
// Chay TRUOC JxActivity (SDL). Viec:
//   1. Tim thu muc du lieu nhu ben C (JxAndroidMain.cpp): jx_data_dir.txt, thu muc app, /mnt/shared/Misc/jx1, /mnt/shared/Misc ...
//      Co config.ini o mot thu muc NGOAI app (may ao LDPlayer chia se) -> khong tai gi, vao game ngay.
//   2. Doc URL may chu tai o <thu muc app>/tai_du_lieu.txt (dong dau), khong co thi lay R.string.may_chu_tai.
//   3. Tai manifest.txt (moi dong: <co>\t<md5>\t<duong dan>), so voi da_tai.txt (tep da tai + md5 da kiem) va tep tren dia
//      -> chi tai tep MOI / DOI (delta). Tai 4 luong song song, HTTP Range de TAI TIEP tep .part dang do, ghi da_tai.txt sau moi tep.
//   4. Ong tien do (thanh vang) + MB / toc do / con lai; xong -> JxActivity. Khong noi duoc may chu ma da co config.ini -> vao game.
// Anh nen / nut lay tu kho VNKU (UiUpdateNow) qua res/drawable-nodpi. May chu: android/may_chu_tai_du_lieu.py.
package vn.jx1.mobile;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;

public class TaiDuLieuActivity extends Activity
{
    private static final int SO_LUONG = 4;              // so luong tai song song
    private static final int THOI_HAN_MS = 8000;         // noi may chu
    private final Handler mChinh = new Handler(Looper.getMainLooper());
    private TextView mTieuDe, mTrangThai, mChiTiet;
    private ProgressBar mThanh;
    private Button mNut;
    private File mThuMuc;                                // thu muc du lieu (thu muc app)
    private String mUrl;                                 // http://.../ (co / cuoi)
    private final AtomicBoolean mHuy = new AtomicBoolean(false);

    private static class Muc { long co; String md5; String duong; }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        Window w = getWindow();
        w.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN | WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        w.getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);
        // 1. du lieu do ngoai (may ao) -> mo JxActivity NGAY, khong hien gi ca (chu: "giu lai cac buoc dang nhap nhu truoc";
        //    man hinh nay chi xuat hien khi phai tai / cap nhat). Tru khi tai_du_lieu.txt dong 2 = "ep" (ep tai de thu).
        File ngoai = epTai() ? null : thuMucNgoai();
        if (ngoai != null)
        {
            startActivity(new Intent(this, JxActivity.class));
            finish();
            return;
        }
        setContentView(R.layout.tai_du_lieu);
        mTieuDe = findViewById(R.id.tieu_de);
        mTrangThai = findViewById(R.id.trang_thai);
        mChiTiet = findViewById(R.id.chi_tiet);
        mThanh = findViewById(R.id.thanh_tai);
        mNut = findViewById(R.id.nut_cap_nhat);
        mNut.setVisibility(View.INVISIBLE);
        mThanh.setMax(1000);
        mThanh.setProgress(0);
        mThuMuc = getExternalFilesDir(null);
        if (mThuMuc == null)
            mThuMuc = getFilesDir();
        mUrl = docUrl();
        mTrangThai.setText("Đang kiểm tra dữ liệu…");
        new Thread(this::kiemVaTai, "jx-tai").start();
    }

    /** thu muc co config.ini ngoai thu muc app (thu tu nhu JxAndroidMain.cpp) */
    private File thuMucNgoai()
    {
        List<String> cand = new ArrayList<>();
        File app = getExternalFilesDir(null);
        if (app != null)
        {
            File txt = new File(app, "jx_data_dir.txt");
            if (txt.isFile())
            {
                try (BufferedReader r = new BufferedReader(new InputStreamReader(new java.io.FileInputStream(txt), StandardCharsets.UTF_8)))
                {
                    String d = r.readLine();
                    if (d != null && !d.trim().isEmpty())
                        cand.add(d.trim());
                }
                catch (IOException ignored) {}
            }
        }
        cand.add("/mnt/shared/Misc/jx1"); cand.add("/mnt/shared/Misc"); cand.add("/mnt/shared/Pictures/jx1");
        cand.add("/mnt/shared/Applications/jx1"); cand.add("/sdcard/jx1"); cand.add("/sdcard/Download/jx1"); cand.add("/storage/emulated/0/jx1");
        for (String c : cand)
            if (new File(c, "config.ini").isFile())
                return new File(c);
        return null;
    }

    /** tai_du_lieu.txt (thu muc app) dong 2 = "ep" -> bo qua du lieu ngoai, luon kiem/tai vao thu muc app (de thu tren may ao) */
    private boolean epTai()
    {
        File app = getExternalFilesDir(null);
        if (app == null) return false;
        File f = new File(app, "tai_du_lieu.txt");
        if (!f.isFile()) return false;
        try (BufferedReader r = new BufferedReader(new InputStreamReader(new java.io.FileInputStream(f), StandardCharsets.UTF_8)))
        {
            r.readLine();
            String d2 = r.readLine();
            return d2 != null && d2.trim().equalsIgnoreCase("ep");
        }
        catch (IOException e) { return false; }
    }

    private String docUrl()
    {
        File f = new File(mThuMuc, "tai_du_lieu.txt");
        if (f.isFile())
        {
            try (BufferedReader r = new BufferedReader(new InputStreamReader(new java.io.FileInputStream(f), StandardCharsets.UTF_8)))
            {
                String u = r.readLine();
                if (u != null && u.trim().startsWith("http"))
                    return u.trim().endsWith("/") ? u.trim() : u.trim() + "/";
            }
            catch (IOException ignored) {}
        }
        String u = getString(R.string.may_chu_tai);
        return u.endsWith("/") ? u : u + "/";
    }

    // ------------------------------------------------------------ kiem + tai (luong nen)
    private void kiemVaTai()
    {
        List<Muc> manifest;
        try
        {
            manifest = docManifest();
        }
        catch (Exception e)
        {
            if (new File(mThuMuc, "config.ini").isFile())
            {
                hien("Không nối được máy chủ tải (" + e.getMessage() + "). Vào game với dữ liệu đã có.", "");
                vaoGame(1500);
            }
            else
            {
                hien("Không nối được máy chủ tải dữ liệu:\n" + mUrl + "\n" + e.getMessage(), "Kiểm tra mạng rồi bấm Thử lại");
                nut("Thử lại", v -> { mNut.setVisibility(View.INVISIBLE); new Thread(this::kiemVaTai, "jx-tai").start(); });
            }
            return;
        }
        mManifest = manifest;
        Map<String, String> daTai = docDaTai();
        List<Muc> can = new ArrayList<>();
        long tong = 0, coSan = 0;
        for (Muc m : manifest)
        {
            File f = new File(mThuMuc, m.duong);
            String ghi = daTai.get(m.duong);
            if (f.isFile() && f.length() == m.co && m.md5.equals(ghi))
            {
                coSan += m.co;
                continue;
            }
            can.add(m);
            tong += m.co;
        }
        if (can.isEmpty())
        {
            donPakCu();
            vaoGame(0);
            return;
        }
        hien("Cần tải " + can.size() + " tệp, " + mb(tong) + " MB", "Đã có " + mb(coSan) + " MB");
        if (mangTinhPhi())
        {
            // 4G / mang tinh phi: khong tu tai (chu: "ton it data") - bao ro va de nguoi choi tu quyet
            final long tongTai = tong;
            final List<Muc> danhSach = can;
            hien("Đang dùng mạng di động (4G). Cần tải " + mb(tong) + " MB.", "Nên nối Wi‑Fi rồi mở lại game. Bấm nút nếu vẫn muốn tải bằng 4G.");
            nut("Tải bằng 4G", v -> { mNut.setVisibility(View.INVISIBLE); new Thread(() -> taiTatCa(danhSach, tongTai), "jx-tai2").start(); });
            return;
        }
        taiTatCa(can, tong);        // tu cap nhat ngay, khong cho bam (chu: "co ban cap nhat moi se tu cap nhat")
    }

    /** mang dang dung co tinh phi (4G/5G, diem phat Wi-Fi tinh phi) */
    private boolean mangTinhPhi()
    {
        try
        {
            android.net.ConnectivityManager cm = (android.net.ConnectivityManager) getSystemService(android.content.Context.CONNECTIVITY_SERVICE);
            return cm != null && cm.isActiveNetworkMetered();
        }
        catch (Exception e) { return false; }
    }

    private List<Muc> docManifest() throws IOException
    {
        HttpURLConnection c = (HttpURLConnection) new URL(mUrl + "manifest.txt").openConnection();
        int han = new File(mThuMuc, "config.ini").isFile() ? 3000 : THOI_HAN_MS;   // da co du lieu: khong bat cho lau
        c.setConnectTimeout(han); c.setReadTimeout(han);
        if (c.getResponseCode() != 200)
            throw new IOException("HTTP " + c.getResponseCode());
        List<Muc> ds = new ArrayList<>();
        try (BufferedReader r = new BufferedReader(new InputStreamReader(c.getInputStream(), StandardCharsets.UTF_8)))
        {
            String ln;
            while ((ln = r.readLine()) != null)
            {
                String[] p = ln.split("\t", 3);
                if (p.length < 3) continue;
                Muc m = new Muc();
                m.co = Long.parseLong(p[0].trim()); m.md5 = p[1].trim(); m.duong = p[2].trim().replace('\\', '/');
                if (m.duong.startsWith("/")) m.duong = m.duong.substring(1);
                if (m.duong.contains("..")) continue;
                ds.add(m);
            }
        }
        return ds;
    }

    private File tepDaTai() { return new File(mThuMuc, "da_tai.txt"); }

    private Map<String, String> docDaTai()
    {
        Map<String, String> m = new HashMap<>();
        File f = tepDaTai();
        if (!f.isFile()) return m;
        try (BufferedReader r = new BufferedReader(new InputStreamReader(new java.io.FileInputStream(f), StandardCharsets.UTF_8)))
        {
            String ln;
            while ((ln = r.readLine()) != null)
            {
                int i = ln.indexOf('\t');
                if (i > 0) m.put(ln.substring(i + 1), ln.substring(0, i));
            }
        }
        catch (IOException ignored) {}
        return m;
    }

    private synchronized void ghiDaTai(Muc m)
    {
        try (FileOutputStream o = new FileOutputStream(tepDaTai(), true))
        {
            o.write((m.md5 + "\t" + m.duong + "\n").getBytes(StandardCharsets.UTF_8));
        }
        catch (IOException ignored) {}
    }

    private void taiTatCa(List<Muc> ds, long tong)
    {
        donPakCu();     // xoa TRUOC pak khong con trong manifest (bo goi doi ten / rut gon) de dien thoai du cho cho goi moi
        final AtomicLong xong = new AtomicLong(0);
        final AtomicLong loi = new AtomicLong(0);
        final long t0 = System.currentTimeMillis();
        final long[] mocByte = { 0 }; final long[] mocT = { t0 };
        ExecutorService ex = Executors.newFixedThreadPool(SO_LUONG);
        // tep to truoc de cac luong deu tay
        List<Muc> sap = new ArrayList<>(ds);
        sap.sort((a, b) -> Long.compare(b.co, a.co));
        for (Muc m : sap)
            ex.submit(() -> {
                if (mHuy.get()) return;
                try { taiMot(m, xong); ghiDaTai(m); }
                catch (Exception e) { loi.incrementAndGet(); hien("Lỗi tải " + m.duong + ": " + e.getMessage(), ""); }
            });
        ex.shutdown();
        while (!ex.isTerminated())
        {
            try { Thread.sleep(250); } catch (InterruptedException ignored) {}
            long da = xong.get();
            long now = System.currentTimeMillis();
            double tocDo = 0;
            if (now - mocT[0] >= 1000)
            {
                tocDo = (da - mocByte[0]) * 1000.0 / (now - mocT[0]) / 1048576.0;
                mocByte[0] = da; mocT[0] = now;
                final double td = tocDo;
                final long con = (td > 0.05) ? (long) ((tong - da) / (td * 1048576.0)) : -1;
                final int pm = (int) (tong > 0 ? da * 1000 / tong : 1000);
                mChinh.post(() -> {
                    mThanh.setProgress(pm);
                    mTrangThai.setText(String.format(Locale.US, "Đang tải dữ liệu  %s / %s MB  (%d%%)", mb(da), mb(tong), pm / 10));
                    mChiTiet.setText(String.format(Locale.US, "%.1f MB/s · còn khoảng %s", td, con < 0 ? "…" : giay(con)));
                });
            }
        }
        if (loi.get() > 0)
        {
            hien("Tải xong nhưng " + loi.get() + " tệp lỗi. Bấm Thử lại để tải tiếp phần còn thiếu.", "");
            nut("Thử lại", v -> { mNut.setVisibility(View.INVISIBLE); new Thread(this::kiemVaTai, "jx-tai").start(); });
            return;
        }
        mChinh.post(() -> mThanh.setProgress(1000));
        donPakCu();
        hien("Tải xong " + mb(tong) + " MB trong " + giay((System.currentTimeMillis() - t0) / 1000) + ". Vào game…", "");
        vaoGame(800);
    }

    private List<Muc> mManifest;                 // manifest vua doc (de don pak cu)

    /** xoa data/*.pak (va .part) khong con trong manifest: pak doi ten / bo bot sau khi rut gon -> khong ton cho */
    private void donPakCu()
    {
        if (mManifest == null) return;
        java.util.HashSet<String> con = new java.util.HashSet<>();
        for (Muc m : mManifest) con.add(m.duong.toLowerCase(Locale.US));
        File[] ds = new File(mThuMuc, "data").listFiles();
        if (ds == null) return;
        for (File f : ds)
        {
            String t = "data/" + f.getName().toLowerCase(Locale.US);
            if (t.endsWith(".part")) t = t.substring(0, t.length() - 5);
            if (f.isFile() && (t.endsWith(".pak")) && !con.contains(t) && f.delete())
                android.util.Log.i("JxTai", "xoa pak cu " + f.getName());
        }
    }

    /** tai mot tep: ghi <duong>.part, HTTP Range de tai tiep, xong doi ten */
    private void taiMot(Muc m, AtomicLong xong) throws IOException
    {
        File dich = new File(mThuMuc, m.duong);
        File cha = dich.getParentFile();
        if (cha != null && !cha.isDirectory() && !cha.mkdirs())
            throw new IOException("không tạo được thư mục " + cha);
        File part = new File(mThuMuc, m.duong + ".part");
        long daCo = part.isFile() ? part.length() : 0;
        if (daCo > m.co) { part.delete(); daCo = 0; }
        if (daCo == m.co)
        {
            xong.addAndGet(m.co);
        }
        else
        {
            xong.addAndGet(daCo);
            String url = mUrl + duongUrl(m.duong);
            HttpURLConnection c = (HttpURLConnection) new URL(url).openConnection();
            c.setConnectTimeout(THOI_HAN_MS); c.setReadTimeout(30000);
            if (daCo > 0) c.setRequestProperty("Range", "bytes=" + daCo + "-");
            int ma = c.getResponseCode();
            if (ma == 200 && daCo > 0) { daCo = 0; xong.addAndGet(-part.length()); }   // may chu khong ho tro Range: tai lai tu dau
            else if (ma != 200 && ma != 206) throw new IOException("HTTP " + ma + " " + url);
            try (InputStream in = c.getInputStream(); RandomAccessFile out = new RandomAccessFile(part, "rw"))
            {
                out.seek(daCo);
                byte[] dem = new byte[256 * 1024];
                int n;
                while ((n = in.read(dem)) > 0)
                {
                    if (mHuy.get()) throw new IOException("huỷ");
                    out.write(dem, 0, n);
                    xong.addAndGet(n);
                }
            }
            if (part.length() != m.co)
                throw new IOException("cỡ sai " + part.length() + " != " + m.co);
        }
        if (dich.isFile() && !dich.delete())
            throw new IOException("không xoá được tệp cũ");
        if (!part.renameTo(dich))
            throw new IOException("không đổi tên được .part");
    }

    private static String duongUrl(String d)
    {
        StringBuilder sb = new StringBuilder();
        for (String p : d.split("/"))
        {
            if (sb.length() > 0) sb.append('/');
            try { sb.append(java.net.URLEncoder.encode(p, "UTF-8").replace("+", "%20")); }
            catch (Exception e) { sb.append(p); }
        }
        return sb.toString();
    }

    // ------------------------------------------------------------ giao dien
    private void hien(final String trangThai, final String chiTiet)
    {
        mChinh.post(() -> { mTrangThai.setText(trangThai); mChiTiet.setText(chiTiet); });
    }

    private void nut(final String nhan, final View.OnClickListener l)
    {
        mChinh.post(() -> {
            // nut VNKU co san chu "Cap nhat ngay" trong anh; nhan khac -> nen trong + chu Android
            boolean coSan = "Cập nhật ngay".equals(nhan);
            mNut.setBackgroundResource(coSan ? R.drawable.nut_cap_nhat : R.drawable.nut_trong);
            mNut.setText(coSan ? "" : nhan);
            mNut.setOnClickListener(l); mNut.setVisibility(View.VISIBLE);
        });
    }

    private void vaoGame(long treMs)
    {
        mChinh.postDelayed(() -> {
            startActivity(new Intent(this, JxActivity.class));
            finish();
        }, treMs);
    }

    private static String mb(long b) { return String.format(Locale.US, "%.0f", b / 1048576.0); }

    private static String giay(long s)
    {
        if (s < 60) return s + " giây";
        if (s < 3600) return (s / 60) + " phút " + (s % 60) + " giây";
        return (s / 3600) + " giờ " + ((s % 3600) / 60) + " phút";
    }

    @Override
    public void onBackPressed()
    {
        mHuy.set(true);
        super.onBackPressed();
    }
}
