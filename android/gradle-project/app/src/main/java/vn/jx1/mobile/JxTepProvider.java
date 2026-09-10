// [CAPNHAT 12/09] ContentProvider toi gian: dua tep APK vua tai (thu muc app/capnhat/) cho trinh cai dat qua content://vn.jx1.mobile.tep/<ten>.
// Android 7+ cam file:// trong Intent cai dat; khong dung androidx FileProvider de khong them thu vien.
package vn.jx1.mobile;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.database.Cursor;
import android.database.MatrixCursor;
import android.net.Uri;
import android.os.ParcelFileDescriptor;
import android.provider.OpenableColumns;

import java.io.File;
import java.io.FileNotFoundException;

public class JxTepProvider extends ContentProvider
{
    private File tep(Uri uri)
    {
        String ten = uri.getLastPathSegment();
        if (ten == null || ten.contains("/") || ten.contains("..")) return null;
        File thu = new File(getContext().getExternalFilesDir(null), "capnhat");
        return new File(thu, ten);
    }

    @Override public boolean onCreate() { return true; }

    @Override
    public ParcelFileDescriptor openFile(Uri uri, String mode) throws FileNotFoundException
    {
        File f = tep(uri);
        if (f == null || !f.isFile()) throw new FileNotFoundException(String.valueOf(uri));
        return ParcelFileDescriptor.open(f, ParcelFileDescriptor.MODE_READ_ONLY);
    }

    @Override
    public Cursor query(Uri uri, String[] cot, String sel, String[] selArgs, String sort)
    {
        File f = tep(uri);
        MatrixCursor c = new MatrixCursor(new String[] { OpenableColumns.DISPLAY_NAME, OpenableColumns.SIZE });
        if (f != null && f.isFile()) c.addRow(new Object[] { f.getName(), f.length() });
        return c;
    }

    @Override public String getType(Uri uri) { return "application/vnd.android.package-archive"; }
    @Override public Uri insert(Uri uri, ContentValues v) { return null; }
    @Override public int delete(Uri uri, String s, String[] a) { return 0; }
    @Override public int update(Uri uri, ContentValues v, String s, String[] a) { return 0; }
}
