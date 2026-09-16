# -*- coding: utf-8 -*-
"""[IOS-THU 16/09] TRAM KY TRUNG GIAN - chay tren Mac de thu bo tai iOS voi kho du lieu tren PC khi PC CHUA co khoa rieng
(khoa ~/.jx1_khoa/jx1_manifest_ec.key nam tren Mac; may_chu_tai_du_lieu.py tren PC khong thay khoa thi xoa manifest.sig,
ban iOS se tu choi kho do). Chi de THU trong LAN; phat hanh that thi may chu phai tu ky (chep khoa sang PC hoac ky bang
android/ky_manifest.py sau moi lan sinh manifest).

  GET /manifest.txt, /manifest.sig : lay manifest tu PC (moi yeu cau), doi dong cua cac tep GIU (config.ini rieng cua iPhone)
                                     thanh co+md5 cua ban tren Mac, ky bang khoa tren Mac, tra ve. PC doi manifest -> tu ky lai.
  GET /<tep GIU>                    : phuc vu ban tren Mac (de iPhone giu config.ini rieng, khong bi ban PC ghi de).
  GET moi duong khac                : chuyen tiep sang PC, giu Range / Content-Range / ma tra ve (tai tiep van chay).

Dung:  python3 ios/tram_ky.py http://10.0.0.140:8765/ 8770 --giu=config.ini=$HOME/jx1_thu/iphone_config_1609.ini
Tren iPhone (cay ban thu): Documents/may_chu_tai.txt = "http://10.0.0.34:8770/" (day bang devicectl device copy to).
Tat dong bo: day mot tep may_chu_tai.txt RONG len (JxIosMain.cpp coi rong = khong co dia chi).
Da thu 16/09: iPhone An Nguyen bam 7,5 GB + tai 117 tep qua tram trong 66 s, config.ini giu nguyen.
"""
import sys, os, io, hashlib, base64, subprocess, urllib.request, urllib.error, threading, time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
PC = sys.argv[1].rstrip('/') + '/'; CONG = int(sys.argv[2])
KHOA = os.path.expanduser('~/.jx1_khoa/jx1_manifest_ec.key')
GIU = {}
for a in sys.argv[3:]:
    if a.startswith('--giu='): k, v = a[6:].split('=', 1); GIU[k] = v
_khoa = threading.Lock(); _man = {'goc': None, 'txt': b'', 'sig': b''}

def md5_tep(p):
    h = hashlib.md5()
    with open(p, 'rb') as f:
        for k in iter(lambda: f.read(1 << 20), b''): h.update(k)
    return h.hexdigest()

def lam_manifest():
    goc = urllib.request.urlopen(PC + 'manifest.txt', timeout=20).read()
    with _khoa:
        if goc == _man['goc']: return _man['txt'], _man['sig']
        dong = []
        for ln in goc.decode('utf-8').split('\n'):
            p = ln.split('\t')
            if len(p) >= 3 and p[2] in GIU:
                ln = '%d\t%s\t%s' % (os.path.getsize(GIU[p[2]]), md5_tep(GIU[p[2]]), p[2])
            dong.append(ln)
        txt = '\n'.join(dong).encode('utf-8')
        sig = subprocess.run(['openssl', 'dgst', '-sha256', '-sign', KHOA], input=txt, capture_output=True, check=True).stdout
        _man.update(goc=goc, txt=txt, sig=base64.b64encode(sig) + b'\n')
        sys.stdout.write('%s manifest PC doi -> da ky lai (%d byte, giu %s)\n' % (time.strftime('%H:%M:%S'), len(txt), ','.join(GIU) or '-')); sys.stdout.flush()
        return _man['txt'], _man['sig']

class H(BaseHTTPRequestHandler):
    protocol_version = 'HTTP/1.1'
    def _tra(self, ma, than, kieu='application/octet-stream', them=()):
        self.send_response(ma); self.send_header('Content-Type', kieu); self.send_header('Content-Length', str(len(than)))
        for k, v in them: self.send_header(k, v)
        self.end_headers(); self.wfile.write(than)
    def do_GET(self):
        duong = urllib.request.unquote(self.path.split('?')[0]).lstrip('/')
        try:
            if duong in ('manifest.txt', 'manifest.sig'):
                txt, sig = lam_manifest(); return self._tra(200, txt if duong == 'manifest.txt' else sig, 'text/plain', [('Cache-Control', 'no-cache')])
            if duong in GIU:
                return self._tra(200, open(GIU[duong], 'rb').read())
            rq = urllib.request.Request(PC + urllib.request.quote(duong))
            if self.headers.get('Range'): rq.add_header('Range', self.headers['Range'])
            try:
                r = urllib.request.urlopen(rq, timeout=60)
            except urllib.error.HTTPError as e:
                return self._tra(e.code, b'')
            self.send_response(r.status)
            for k in ('Content-Type', 'Content-Length', 'Content-Range', 'Accept-Ranges'):
                if r.headers.get(k): self.send_header(k, r.headers[k])
            self.end_headers()
            while True:
                k = r.read(1 << 20)
                if not k: break
                self.wfile.write(k)
        except Exception as e:
            try: self._tra(502, ('loi tram: %s' % e).encode())
            except Exception: pass
    def log_message(self, fmt, *a):
        sys.stdout.write('%s %s %s\n' % (time.strftime('%H:%M:%S'), self.client_address[0], fmt % a)); sys.stdout.flush()

lam_manifest()
print('tram ky: %s -> cong %d, giu: %s' % (PC, CONG, GIU)); sys.stdout.flush()
ThreadingHTTPServer(('0.0.0.0', CONG), H).serve_forever()
