# -*- coding: utf-8 -*-
r"""[TENTEP 16/09] Viec phien iOS ban giao (BANGIAO_ANDROID_TU_IOS_1609.md muc 1, 3, 4, 5, 6, 7) - phan ma:

  1. TaiDuLieuActivity: ten tren dia = ten CHUAN (ha A-Z + Latin-1 nhu JxPathPosix: U+00C0-00DE tru x +0x20, S/OE/Z hoa +1,
     Y hoa -> y, roi NFC); URL van ten goc; khoa da_tai.txt cung chuan hoa; bien the hoa da co tren may -> DOI TEN (khong tai lai).
  3. may_chu_tai_du_lieu.py: khoa dem md5 = (co, st_mtime_ns) thay vi giay -> ghi cung tep hai lan trong mot giay khong con mang md5 cu.
  4. Tep 0 byte: tao .part rong truoc khi doi ten.
  5. Bam md5 tep .part sau khi tai, sai thi xoa + bao loi (nhu iOS).
  6. Bo cuc mac dinh doi: xoa ca userdata/uitoado_<id>.ini theo nhan vat.
  7. Don mo coi: sau khi dong bo tron ven, xoa tep trong cac thu muc cap 1 cua manifest ma khong co trong manifest
     (giu userdata/, apdata/, tep o goc, .part).
  + chuan_bi_du_lieu.ps1: ha chu thuong theo dung anh xa cua game (khong chi A-Z) - dao nguoc quyet dinh [TENGBK 11/09] vi
     JxPathPosix (518fbacd 08/09) van ha Latin-1; sua_ten_gbk_android.py danh dau KHONG DUNG NUA.

Chay:  python android\va_tai_du_lieu_1609_tentep.py
"""
import io
import os

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TAG = "[TENTEP 16/09]"


def va(duong, cap):
    p = os.path.join(GOC, duong)
    s = io.open(p, "r", encoding="utf-8", newline="").read()
    if TAG in s:
        print("  bo qua (da co %s): %s" % (TAG, duong))
        return
    for cu, moi in cap:
        if s.count(cu) != 1:
            raise SystemExit("HONG: neo xuat hien %d lan trong %s:\n%s" % (s.count(cu), duong, cu[:200]))
        s = s.replace(cu, moi, 1)
    io.open(p, "w", encoding="utf-8", newline="").write(s)
    print("  da va: %s" % duong)


# ---------------------------------------------------------------- 1. bo tai Android
J = "android/gradle-project/app/src/main/java/vn/jx1/mobile/TaiDuLieuActivity.java"
va(J, [
    ("import java.nio.charset.StandardCharsets;\n",
     "import java.nio.charset.StandardCharsets;\n"
     "import java.security.MessageDigest;           // [TENTEP 16/09] kiem md5 tep tai ve\n"
     "import java.text.Normalizer;                   // [TENTEP 16/09] ten chuan NFC\n"),
    ("    private static class Muc { long co; String md5; String duong; }\n",
     "    private static class Muc { long co; String md5; String duong; String duongGoc; }   // [TENTEP 16/09] duong = ten CHUAN tren dia, duongGoc = ten trong manifest (URL)\n"),
    # docManifest: chuan hoa ten
    ("                if (m.duong.startsWith(\"/\")) m.duong = m.duong.substring(1);\n                if (m.duong.contains(\"..\")) continue;\n",
     "                if (m.duong.startsWith(\"/\")) m.duong = m.duong.substring(1);\n                if (m.duong.contains(\"..\")) continue;\n"
     "                m.duongGoc = m.duong; m.duong = chuanHoa(m.duong);   // [TENTEP 16/09] tren dia luon la ten chuan (JxPathPosix ha Latin-1); URL giu ten goc\n"),
    # docDaTai: khoa chuan hoa
    ("                if (i > 0) m.put(ln.substring(i + 1), ln.substring(0, i));\n",
     "                if (i > 0) m.put(chuanHoa(ln.substring(i + 1)), ln.substring(0, i));   // [TENTEP 16/09] khoa = ten chuan (ban cu ghi ten hoa van khop)\n"),
    # kiemVaTai: tep tren dia (doi ten bien the hoa neu co)
    ("            File f = new File(mThuMuc, m.duong);\n            String ghi = daTai.get(m.duong);\n",
     "            File f = tepTrenDia(m.duong);          // [TENTEP 16/09] bien the hoa da tai truoc day -> doi ten ve chuan, khong tai lai\n            String ghi = daTai.get(m.duong);\n"),
    # khong can tai: don mo coi roi vao game
    ("        if (can.isEmpty())\n        {\n            donPakCu();\n            vaoGame(0);\n            return;\n        }\n",
     "        if (can.isEmpty())\n        {\n            donPakCu();\n            donMoCoi();     // [TENTEP 16/09] dong bo tron ven -> don tep khong con trong manifest\n            vaoGame(0);\n            return;\n        }\n"),
    # tai xong khong loi: don mo coi + xoa bo cuc theo nhan vat
    ("        mChinh.post(() -> mThanh.setProgress(1000));\n        donPakCu();\n",
     "        mChinh.post(() -> mThanh.setProgress(1000));\n        donPakCu();\n        donMoCoi();     // [TENTEP 16/09] tai xong khong loi = dong bo tron ven\n"),
    ("                File cu = new File(mThuMuc, \"userdata/UiToaDo.ini\");\n                if (cu.isFile() && cu.delete()) android.util.Log.i(\"JxTai\", \"bo cuc mac dinh doi -> xoa userdata/UiToaDo.ini\");\n                break;\n",
     "                File cu = new File(mThuMuc, \"userdata/UiToaDo.ini\");\n                if (cu.isFile() && cu.delete()) android.util.Log.i(\"JxTai\", \"bo cuc mac dinh doi -> xoa userdata/UiToaDo.ini\");\n"
     "                // [TENTEP 16/09] UiToaDoMobile.inc uu tien UserData\\UiToaDo_<id>.ini theo nhan vat (game ghi ten chu thuong qua JxPathPosix) -> xoa ca ho nay\n"
     "                File[] ud = new File(mThuMuc, \"userdata\").listFiles();\n"
     "                if (ud != null)\n"
     "                    for (File t : ud)\n"
     "                    {\n"
     "                        String tn = t.getName().toLowerCase(Locale.US);\n"
     "                        if (t.isFile() && tn.startsWith(\"uitoado\") && tn.endsWith(\".ini\") && t.delete()) android.util.Log.i(\"JxTai\", \"bo cuc mac dinh doi -> xoa userdata/\" + t.getName());\n"
     "                    }\n"
     "                break;\n"),
    # taiMot: dich theo ten chuan (doi ten thu muc bien the), URL ten goc, 0 byte, md5
    ("        File dich = new File(mThuMuc, m.duong);\n        File cha = dich.getParentFile();\n",
     "        File dich = tepTrenDia(m.duong);       // [TENTEP 16/09] thu muc cha bien the hoa -> doi ten truoc, khong tao thu muc thu hai\n        File cha = dich.getParentFile();\n"),
    ("            String url = mUrl + duongUrl(m.duong);\n",
     "            String url = mUrl + duongUrl(m.duongGoc != null ? m.duongGoc : m.duong);   // [TENTEP 16/09] URL = ten trong manifest\n"),
    ("        if (dich.isFile() && !dich.delete())\n            throw new IOException(\"không xoá được tệp cũ\");\n",
     "        if (m.co == 0 && !part.isFile()) new FileOutputStream(part).close();   // [TENTEP 16/09] tep 0 byte: khong co .part de doi ten -> tao rong\n"
     "        {   // [TENTEP 16/09] kiem md5 nhu iOS: manifest da ky ma tep tai ve khong kiem thi chu ky vo nghia voi tep\n"
     "            String bam = md5Tep(part);\n"
     "            if (!m.md5.equalsIgnoreCase(bam)) { part.delete(); throw new IOException(\"md5 sai (\" + bam + \" != \" + m.md5 + \")\"); }\n"
     "        }\n"
     "        if (dich.isFile() && !dich.delete())\n            throw new IOException(\"không xoá được tệp cũ\");\n"),
    # ham phu
    ("    private static String duongUrl(String d)\n",
     "    /** [TENTEP 16/09] ten chuan = cach JxPathPosix (KPosixWin32.cpp) ha chu thuong: A-Z, Latin-1 U+00C0-00DE (tru U+00D7), S/OE/Z hoa +1, Y hoa -> y; roi NFC.\n"
     "     *  Ten GBK tren dia PC doc kieu cp1252 la chu Latin-1 hoa ('I' co dau...); /storage/emulated Android <= 10 (sdcardfs) chi gap hoa/thuong ASCII\n"
     "     *  -> 115 tep ten Latin-1 hoa trong manifest khong tim thay (iOS: APFS phan biet ca hai). Manifest PC da doi sang ten chuan; day la phong thu. */\n"
     "    static String chuanHoa(String s)\n"
     "    {\n"
     "        String nfc = Normalizer.normalize(s, Normalizer.Form.NFC);\n"
     "        char[] p = nfc.toCharArray();\n"
     "        for (int i = 0; i < p.length; i++)\n"
     "        {\n"
     "            char c = p[i];\n"
     "            if (c >= 'A' && c <= 'Z') c = (char) (c + 32);\n"
     "            else if (c >= 0xC0 && c <= 0xDE && c != 0xD7) c = (char) (c + 0x20);\n"
     "            else if (c == 0x160 || c == 0x152 || c == 0x17D) c = (char) (c + 1);\n"
     "            else if (c == 0x178) c = 0xFF;\n"
     "            p[i] = c;\n"
     "        }\n"
     "        return Normalizer.normalize(new String(p), Normalizer.Form.NFC);\n"
     "    }\n"
     "\n"
     "    /** [TENTEP 16/09] tep/thu muc theo ten chuan; thanh phan nao chua co ma co bien the (khac hoa/thuong) tren dia thi DOI TEN ve chuan\n"
     "     *  (he tep phan biet hoa/thuong: tranh tai lai 292 MB va tranh hai thu muc song song). He tep khong phan biet: exists() dung -> khong dong gi. */\n"
     "    private File tepTrenDia(String duongChuan)\n"
     "    {\n"
     "        File cur = mThuMuc;\n"
     "        for (String tp : duongChuan.split(\"/\"))\n"
     "        {\n"
     "            if (tp.isEmpty()) continue;\n"
     "            File f = new File(cur, tp);\n"
     "            if (!f.exists())\n"
     "            {\n"
     "                File[] ds = cur.listFiles();\n"
     "                if (ds != null)\n"
     "                    for (File s : ds)\n"
     "                        if (!s.getName().equals(tp) && chuanHoa(s.getName()).equals(tp))\n"
     "                        {\n"
     "                            if (s.renameTo(f)) android.util.Log.i(\"JxTai\", \"doi ten ve chuan: \" + s.getName() + \" -> \" + tp);\n"
     "                            break;\n"
     "                        }\n"
     "            }\n"
     "            cur = f;\n"
     "        }\n"
     "        return cur;\n"
     "    }\n"
     "\n"
     "    private static String md5Tep(File f) throws IOException\n"
     "    {\n"
     "        try (InputStream in = new java.io.FileInputStream(f))\n"
     "        {\n"
     "            MessageDigest md = MessageDigest.getInstance(\"MD5\");\n"
     "            byte[] dem = new byte[256 * 1024];\n"
     "            int n;\n"
     "            while ((n = in.read(dem)) > 0) md.update(dem, 0, n);\n"
     "            StringBuilder sb = new StringBuilder();\n"
     "            for (byte b : md.digest()) sb.append(String.format(Locale.US, \"%02x\", b & 0xFF));\n"
     "            return sb.toString();\n"
     "        }\n"
     "        catch (java.security.NoSuchAlgorithmException e) { throw new IOException(\"MD5\"); }\n"
     "    }\n"
     "\n"
     "    /** [TENTEP 16/09] don mo coi nhu iOS: sau khi dong bo TRON VEN, xoa tep trong cac thu muc cap 1 cua manifest ma khong co trong manifest\n"
     "     *  (ten so theo dang chuan). Giu userdata/, apdata/, tep o goc, .part; thu muc khong co trong manifest (capnhat/...) khong dung. */\n"
     "    private void donMoCoi()\n"
     "    {\n"
     "        if (mManifest == null || mManifest.isEmpty()) return;\n"
     "        java.util.HashSet<String> con = new java.util.HashSet<>();\n"
     "        java.util.HashSet<String> goc = new java.util.HashSet<>();\n"
     "        for (Muc m : mManifest)\n"
     "        {\n"
     "            con.add(m.duong);\n"
     "            int i = m.duong.indexOf('/');\n"
     "            if (i > 0) goc.add(m.duong.substring(0, i));\n"
     "        }\n"
     "        goc.remove(\"userdata\"); goc.remove(\"apdata\");\n"
     "        int xoa = 0;\n"
     "        for (String g : goc)\n"
     "            xoa += donMoCoiThu(new File(mThuMuc, g), g, con);\n"
     "        if (xoa > 0) android.util.Log.i(\"JxTai\", \"don \" + xoa + \" tep mo coi (khong co trong manifest)\");\n"
     "    }\n"
     "\n"
     "    private int donMoCoiThu(File thu, String rel, java.util.HashSet<String> con)\n"
     "    {\n"
     "        File[] ds = thu.listFiles();\n"
     "        if (ds == null) return 0;\n"
     "        int xoa = 0;\n"
     "        for (File f : ds)\n"
     "        {\n"
     "            String r = rel + \"/\" + f.getName();\n"
     "            if (f.isDirectory()) { xoa += donMoCoiThu(f, r, con); continue; }\n"
     "            if (!f.isFile() || f.getName().endsWith(\".part\")) continue;\n"
     "            if (con.contains(chuanHoa(r))) continue;\n"
     "            if (f.delete()) { xoa++; android.util.Log.i(\"JxTai\", \"xoa mo coi \" + r); }\n"
     "        }\n"
     "        return xoa;\n"
     "    }\n"
     "\n"
     "    private static String duongUrl(String d)\n"),
])

# ---------------------------------------------------------------- 3. may chu manifest: khoa dem md5 theo st_mtime_ns
va("android/may_chu_tai_du_lieu.py", [
    ("            co = st.st_size; mt = int(st.st_mtime)\n",
     "            co = st.st_size; mt = st.st_mtime_ns   # [TENTEP 16/09] theo nano giay: ghi cung tep hai lan trong mot giay (cung co) khong con mang md5 cu (iOS bao 'md5 sai' voi phienban.txt 16/09); doi khoa = bam lai mot lan\n"),
    ("    \"\"\"Tao manifest.txt (co, md5, duong dan) cho moi tep trong thu muc; dem md5 theo (co, mtime) o manifest_cache.txt\"\"\"\n",
     "    \"\"\"Tao manifest.txt (co, md5, duong dan) cho moi tep trong thu muc; dem md5 theo (co, mtime_ns) o manifest_cache.txt\"\"\"\n"),
])

# ---------------------------------------------------------------- + chuan_bi_du_lieu.ps1: ha chu thuong theo anh xa cua game
va("android/chuan_bi_du_lieu.ps1", [
    ("function HaAscii([string]$s) { return [regex]::Replace($s, '[A-Z]', { param($m) $m.Value.ToLowerInvariant() }) }\n",
     "# [TENTEP 16/09] DAO NGUOC [TENGBK 11/09]: JxPathPosix (KPosixWin32.cpp, 518fbacd 08/09) van ha chu thuong ca Latin-1 khi mo tep, nen ten tren dia\n"
     "# PHAI la ten chuan y het anh xa cua game: A-Z, U+00C0-00DE (tru x U+00D7) +0x20, S/OE/Z hoa (U+0160/0152/017D) +1, Y hoa U+0178 -> y, roi NFC.\n"
     "# /storage/emulated Android <= 10 (sdcardfs) chi gap hoa/thuong ASCII, iOS (APFS) phan biet ca hai -> 115 tep Latin-1 hoa khong tim thay\n"
     "# (BANGIAO_ANDROID_TU_IOS_1609.md muc 1). Ket luan 11/09 'game xin bang byte goc' la sai; sua_ten_gbk_android.py KHONG DUNG NUA.\n"
     "function HaAscii([string]$s) {\n"
     "  $sb = New-Object System.Text.StringBuilder\n"
     "  foreach ($ch in $s.ToCharArray()) {\n"
     "    $c = [int]$ch\n"
     "    if ($c -ge 65 -and $c -le 90) { $c += 32 }\n"
     "    elseif ($c -ge 0xC0 -and $c -le 0xDE -and $c -ne 0xD7) { $c += 0x20 }\n"
     "    elseif ($c -eq 0x160 -or $c -eq 0x152 -or $c -eq 0x17D) { $c += 1 }\n"
     "    elseif ($c -eq 0x178) { $c = 0xFF }\n"
     "    [void]$sb.Append([char]$c)\n"
     "  }\n"
     "  return $sb.ToString().Normalize([System.Text.NormalizationForm]::FormC)\n"
     "}\n"),
])

# ---------------------------------------------------------------- sua_ten_gbk_android.py: danh dau khong dung
va("android/sua_ten_gbk_android.py", [
    ("# -*- coding: utf-8 -*-\n",
     "# -*- coding: utf-8 -*-\n"
     "# [TENTEP 16/09] KHONG DUNG NUA: ket luan 11/09 sai - JxPathPosix VAN ha chu thuong Latin-1 khi mo tep (518fbacd 08/09), nen ten tren dia\n"
     "# phai la ten chuan chu thuong (android/ha_ten_theo_game.py + chuan_bi_du_lieu.ps1 HaAscii moi). Chay tep nay la dua ten ve hoa = hong lai.\n"
     "raise SystemExit(\"[TENTEP 16/09] sua_ten_gbk_android.py da bo: dung android/ha_ten_theo_game.py (xem BANGIAO_ANDROID_TU_IOS_1609.md muc 1)\")\n"),
])
print("xong %s" % TAG)
