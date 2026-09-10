# -*- coding: utf-8 -*-
r"""[ANDROID 12/09 TAI] May chu TAI DU LIEU cho dien thoai (di voi TaiDuLieuActivity.java trong APK).

  * Phuc vu thu muc du lieu da dong goi (mac dinh D:\jx1_android_data_dt) qua HTTP, ho tro Range (tai tiep) va nhieu luong.
  * /manifest.txt : moi dong "<co>\t<md5>\t<duong dan tuong doi>" - dien thoai so voi tep da co, CHI tai tep moi/doi.
    md5 duoc luu dem trong <thu muc>\manifest_cache.txt theo (co, mtime) -> chay lai khong phai bam lai 9 GB.
  * Bo qua: userdata\, apdata\ (game ghi), *.log, *.part, da_tai.txt, jx_data_dir.txt, tai_du_lieu.txt, manifest*.txt.

Dung:  python android\may_chu_tai_du_lieu.py [--thu-muc D:\jx1_android_data_dt] [--cong 8765] [--chi-manifest]
Dien thoai (cung mang LAN) : APK mac dinh tai tu http://<IP may nay>:8765/ (R.string.may_chu_tai); doi bang tep
  /storage/emulated/0/Android/data/vn.jx1.mobile/files/tai_du_lieu.txt (dong dau = URL).
Tuong lua Windows: cho phep python.exe hoac mo cong 8765 (netsh advfirewall firewall add rule name=jx1tai dir=in action=allow protocol=TCP localport=8765).
"""
import hashlib
import io
import os
import posixpath
import sys
import time
import urllib.parse
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer

sys.stdout.reconfigure(encoding="utf-8", errors="replace", line_buffering=True)   # nhat ky hien ngay khi chuyen huong ra tep
BO_THU_MUC = ("userdata", "apdata")
BO_TEP = ("da_tai.txt", "jx_data_dir.txt", "tai_du_lieu.txt", "manifest.txt", "manifest_cache.txt")
MB = 1048576.0


def doc_tham_so():
    a = sys.argv[1:]
    thu = r"D:\jx1_android_data_dt"; cong = 8765; chi_manifest = False
    i = 0
    while i < len(a):
        if a[i] == "--thu-muc": thu = a[i + 1]; i += 2
        elif a[i] == "--cong": cong = int(a[i + 1]); i += 2
        elif a[i] == "--chi-manifest": chi_manifest = True; i += 1
        else: raise SystemExit("tham so la: " + a[i])
    return thu, cong, chi_manifest


def md5_tep(p):
    h = hashlib.md5()
    with open(p, "rb") as f:
        for khoi in iter(lambda: f.read(8 * MB.__int__()), b""):
            h.update(khoi)
    return h.hexdigest()


def lam_manifest(thu):
    """Tao manifest.txt (co, md5, duong dan) cho moi tep trong thu muc; dem md5 theo (co, mtime) o manifest_cache.txt"""
    dem = {}
    p_dem = os.path.join(thu, "manifest_cache.txt")
    if os.path.isfile(p_dem):
        for ln in io.open(p_dem, encoding="utf-8"):
            phan = ln.rstrip("\n").split("\t")
            if len(phan) == 4:
                dem[phan[3]] = (int(phan[0]), int(phan[1]), phan[2])
    dong = []; dem_moi = []; tong = 0; bam = 0; t0 = time.time()
    for root, ds, fs in os.walk(thu):
        rel_thu = os.path.relpath(root, thu)
        if rel_thu == ".":
            ds[:] = [d for d in ds if d.lower() not in BO_THU_MUC]
        for f in sorted(fs):
            if f.lower() in BO_TEP or f.lower().endswith((".log", ".part")):
                continue
            p = os.path.join(root, f)
            rel = os.path.relpath(p, thu).replace(os.sep, "/")
            st = os.stat(p)
            co = st.st_size; mt = int(st.st_mtime)
            cu = dem.get(rel)
            if cu and cu[0] == co and cu[1] == mt:
                md5 = cu[2]
            else:
                print("  bam md5 %s (%.0f MB)" % (rel, co / MB))
                md5 = md5_tep(p); bam += co
            dem_moi.append("%d\t%d\t%s\t%s" % (co, mt, md5, rel))
            dong.append("%d\t%s\t%s" % (co, md5, rel))
            tong += co
    io.open(p_dem, "w", encoding="utf-8", newline="\n").write("\n".join(dem_moi) + "\n")
    io.open(os.path.join(thu, "manifest.txt"), "w", encoding="utf-8", newline="\n").write("\n".join(dong) + "\n")
    print("manifest.txt: %d tep, %.0f MB (bam lai %.0f MB, %.0f s)" % (len(dong), tong / MB, bam / MB, time.time() - t0))
    return len(dong), tong


class BoXuLy(SimpleHTTPRequestHandler):
    """SimpleHTTPRequestHandler + Range (206) de dien thoai tai tiep; khong liet ke thu muc"""
    protocol_version = "HTTP/1.1"

    def list_directory(self, path):
        self.send_error(403)
        return None

    def do_GET(self):
        f = self.send_head()
        if f:
            try:
                if self._range:
                    dau, cuoi = self._range
                    f.seek(dau)
                    con = cuoi - dau + 1
                    while con > 0:
                        khoi = f.read(min(1024 * 1024, con))
                        if not khoi:
                            break
                        self.wfile.write(khoi)
                        con -= len(khoi)
                else:
                    self.copyfile(f, self.wfile)
            finally:
                f.close()

    def send_head(self):
        self._range = None
        path = self.translate_path(self.path)
        if os.path.isdir(path):
            self.send_error(403); return None
        try:
            f = open(path, "rb")
        except OSError:
            self.send_error(404, "Khong co tep"); return None
        co = os.fstat(f.fileno()).st_size
        rng = self.headers.get("Range")
        if rng and rng.startswith("bytes="):
            try:
                a, b = rng[6:].split("-", 1)
                dau = int(a) if a else max(0, co - int(b))
                cuoi = int(b) if (b and a) else co - 1
            except ValueError:
                dau, cuoi = 0, co - 1
            if dau >= co or dau > cuoi:
                f.close()
                self.send_response(416); self.send_header("Content-Range", "bytes */%d" % co); self.send_header("Content-Length", "0"); self.end_headers()
                return None
            cuoi = min(cuoi, co - 1)
            self._range = (dau, cuoi)
            self.send_response(206)
            self.send_header("Content-Range", "bytes %d-%d/%d" % (dau, cuoi, co))
            self.send_header("Content-Length", str(cuoi - dau + 1))
        else:
            self.send_response(200)
            self.send_header("Content-Length", str(co))
        self.send_header("Content-Type", "application/octet-stream")
        self.send_header("Accept-Ranges", "bytes")
        self.send_header("Cache-Control", "no-cache")
        self.end_headers()
        return f

    def log_message(self, fmt, *args):
        sys.stdout.write("%s %s %s\n" % (time.strftime("%H:%M:%S"), self.client_address[0], fmt % args))


def main():
    thu, cong, chi_manifest = doc_tham_so()
    if not os.path.isfile(os.path.join(thu, "config.ini")):
        raise SystemExit("khong thay config.ini trong " + thu)
    lam_manifest(thu)
    if chi_manifest:
        return
    os.chdir(thu)
    sv = ThreadingHTTPServer(("0.0.0.0", cong), BoXuLy)
    sv.daemon_threads = True
    print("dang phuc vu %s tai cong %d (Ctrl+C de dung). Dien thoai: http://<IP may nay>:%d/" % (thu, cong, cong))
    try:
        sv.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
