# -*- coding: utf-8 -*-
"""vhtd_data_patch7.py [VHTD 02/09i] - dot 6b, theo 2 anh tooltip VLTK chu gui ~11:40:
 D1 StatePos.ini (client): VLTK slistcache \\ui\\StatePos.ini (vltk_raw/ui__StatePos.ini) CO icon trang thai THAT cho 1965 (chua co), 1970 (chua co),
    1973 (dang tro nham 智勇超伦), 1988 / 2128 / 2133 / 2139 (dot 5 tu sinh vhtd\\*.spr) -> dung dung duong dan VLTK (SPR co san trong
    updatejx15/16.pak ma client dang nap), Name/Desc theo VLTK; BuffCount +2.
 D2 huashan.lua (server+client): SKILLS.duoming_start.autoreplyskill v3 (1364 Doat Menh Lien Hoan Tam Tien Kiem): 15 s / 1..3 % (du lieu
    Level Up = Linux) -> 5 s / 1..10 % nhu tooltip anh chu ("Don danh co 10% ty le xuat Thai Nhac Thanh Phong ... hoi chieu 5 giay") - GIA DINH,
    cach dich: engine JX1 doc byte thap = ty le %, phan cao = khung hoi chieu (dot Hoa Son f).
Doc/ghi latin-1, giu NL, idempotent. DUNG: python vhtd_data_patch7.py [--kiem]
"""
import io, os, re, sys, shutil

KIEM = "--kiem" in sys.argv
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
VLTK_SP = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw\ui__StatePos.ini"
BAK = ".truoc_vhtd_patch7_0209"
IDS = [1965, 1970, 1973, 1988, 2128, 2133, 2139]

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s, tag):
    if "\xef\xbf\xbd" in s: raise SystemExit("EF BF BD " + p)
    if not KIEM:
        if not os.path.exists(p + BAK): shutil.copy2(p, p + BAK)
        io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  => %s %s (%s)" % ("KIEM" if KIEM else "ghi", p, tag))

def vltk_entries():
    t = rd(VLTK_SP); out = {}
    for m in re.finditer(r"Buff_(\d+)_ID=(\d+)", t):
        n, sid = int(m.group(1)), int(m.group(2))
        e = {}
        for k in ("Name", "Image", "Desc"):
            mm = re.search(r"Buff_%d_%s=(.*)" % (n, k), t)
            if mm: e[k] = mm.group(1).rstrip("\r")
        out[sid] = e
    return out

def d1_statepos():
    p = os.path.join(BIN, "client", "ui", "StatePos.ini")
    s = rd(p); nl = "\r\n" if s.count("\r\n") * 2 > s.count("\n") else "\n"
    V = vltk_entries()
    ours = dict((int(m.group(2)), int(m.group(1))) for m in re.finditer(r"Buff_(\d+)_ID=(\d+)", s))
    cnt = int(re.search(r"BuffCount=(\d+)", s).group(1)); n = 0
    for sid in IDS:
        v = V[sid]
        if sid in ours:
            idx = ours[sid]
            for k in ("Name", "Image", "Desc"):
                m = re.search(r"(?m)^(Buff_%d_%s=)(.*)$" % (idx, k), s)
                if m and m.group(2).rstrip("\r") != v[k]:
                    s = s[:m.start(2)] + v[k] + s[m.end(2) - (1 if m.group(2).endswith("\r") else 0):]
                    n += 1; print("  [+] StatePos %d Buff_%d_%s -> %s" % (sid, idx, k, v[k].encode("latin-1").decode("gbk", "replace")[:70]))
        else:
            idx = cnt; cnt += 1
            block = nl + "Buff_%d_ID=%d" % (idx, sid) + nl + "Buff_%d_Name=%s" % (idx, v["Name"]) + nl + "Buff_%d_Image=%s" % (idx, v["Image"]) + nl + "Buff_%d_Desc=%s" % (idx, v["Desc"]) + nl
            s = s.rstrip("\r\n") + nl + block
            n += 1; print("  [+] StatePos them %d (Buff_%d) %s" % (sid, idx, v["Image"].encode("latin-1").decode("gbk", "replace")))
    if n:
        s = re.sub(r"BuffCount=\d+", "BuffCount=%d" % cnt, s, count=1)
        wr(p, s, "StatePos %d thay doi, BuffCount=%d" % (n, cnt))
    else:
        print("  [=] StatePos da khop VLTK")

def d2_huashan():
    old = "{{1,15*18*256 + 1},{20,15*18*256 + 3},{31,15*18*256 + 3},{32,15*18*256 + 3}}"
    new = "{{1,5*18*256 + 1},{20,5*18*256 + 10},{31,5*18*256 + 10},{32,5*18*256 + 10}}"
    for side in ("server", "client"):
        p = os.path.join(BIN, side, "script", "skill", "huashan.lua")
        s = rd(p)
        m = re.search(r"(?m)^[ \t]*SKILLS\.duoming_start\s*=", s)
        if not m: raise SystemExit("khong thay SKILLS.duoming_start " + p)
        seg_end = s.find("\tSKILLS.", m.end())
        seg = s[m.start(): seg_end if seg_end > 0 else len(s)]
        if new in seg: print("  [=] %s duoming_start.autoreplyskill da 5 s / 1..10 %%" % side); continue
        if seg.count(old) != 1: raise SystemExit("neo autoreplyskill %d lan trong %s" % (seg.count(old), p))
        # KHONG chen chu thich "--" giua dong: 02/09 13:20 chu thich nuot dau "}," -> huashan.lua loi cu phap -> "skill Hoa Son mat het sach thuoc tinh"
        s = s[:m.start()] + seg.replace(old, new) + s[m.start() + len(seg):]
        # bat buoc: kiem cu phap ban MOI bang lua4 -s100 (stack client) TRUOC khi ghi
        import subprocess
        tmpf = p + ".p7tmp"; io.open(tmpf, "w", encoding="latin-1", newline="").write(s)
        chk = os.path.join(os.environ.get("TEMP", "."), "_t_p7.lua")
        io.open(chk, "w", encoding="latin-1", newline="\n").write('SKILLS={}\ndofile("%s")\nlocal n=0 for k,v in SKILLS do n=n+1 end print("bang",n)\n' % tmpf.replace("\\", "/"))
        r = subprocess.run([r"D:\GAMEDEVNEW\ReverseTools\lua4\lua4.exe", "-s100", chk], capture_output=True)
        os.remove(tmpf)
        if r.returncode != 0 or b"bang" not in r.stdout: raise SystemExit("lua4 loi sau khi sua: " + r.stderr.decode("latin-1")[:200])
        print("  lua4 -s100 OK:", r.stdout.decode("latin-1").strip())
        wr(p, s, "duoming_start.autoreplyskill 5 s / 1..10 %")

if __name__ == "__main__":
    print("vhtd_data_patch7 [VHTD 02/09i]%s" % (" (KIEM)" if KIEM else ""))
    d1_statepos(); d2_huashan()
    print("XONG.")
