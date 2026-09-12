# -*- coding: utf-8 -*-
r"""[IOS-GOP 12/09 b] KIEM TUONG DUONG CHO ANDROID.

Khi port toi uu sang Metal, ta doi mot so "#ifdef JX_ANDROID" thanh "#ifdef JX_MOBILE".
Android dinh nghia CA HAI macro nen bien dich ra dung y het ma cu - nhung ios/kiem_rao.py
chi so TUNG DONG nen no chi thay dong rao doi chu, va bao KHAC. Bao khac ma thuc ra khong khac
thi lan sau khong ai them doc canh bao nua, nen phai co cong cu tra loi dung cau hoi:

    "Voi bo macro cua ANDROID, hai ban co bien dich ra CUNG MOT tap dong khong?"

Cach lam: chay trinh tien xu ly rut gon tren CA HAI ban voi bo macro cua Android
(JX_ANDROID, JX_MOBILE, JX_POSIX, JX_PLATFORM_SDL = co; JX_APPLE/JX_IOS/JX_MACOS = khong),
roi so tung dong. Giong het = Android khong doi mot dong nao.

Chay:  python3 ios/kiem_android_tuongduong.py [<moc git>]
"""
import io, os, re, subprocess, sys

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CHEDO_PC = "--pc" in sys.argv[1:]
_ds = [a for a in sys.argv[1:] if a != "--pc"]
MOC = _ds[0] if _ds else "HEAD"
NEN = "WINDOWS" if CHEDO_PC else "ANDROID"
if CHEDO_PC:
    CO = {"WIN32", "_WIN32"}
    KHONG = {"JX_ANDROID", "JX_MOBILE", "JX_POSIX", "JX_PLATFORM_SDL", "JX_APPLE", "JX_IOS", "JX_MACOS"}
else:
    CO = {"JX_ANDROID", "JX_MOBILE", "JX_POSIX", "JX_PLATFORM_SDL"}
    KHONG = {"JX_APPLE", "JX_IOS", "JX_MACOS", "WIN32", "_WIN32"}

def dinh_gia(dong):
    """Tra True/False/None cho mot dong #if...; None = khong danh gia duoc (giu ca hai nhanh)."""
    t = dong.strip()
    m = re.match(r"#ifdef\s+(\w+)", t)
    if m:
        return m.group(1) in CO if (m.group(1) in CO or m.group(1) in KHONG) else None
    m = re.match(r"#ifndef\s+(\w+)", t)
    if m:
        return m.group(1) not in CO if (m.group(1) in CO or m.group(1) in KHONG) else None
    m = re.match(r"#if\s+(.+)$", t)
    if m:
        bt = m.group(1)
        if not re.fullmatch(r"[\s!|&()]*(defined\s*\(?\s*\w+\s*\)?[\s!|&()]*)+", bt):
            return None
        ten = re.findall(r"\w+", bt.replace("defined", " "))
        if any(x not in CO and x not in KHONG for x in ten):
            return None
        bt2 = re.sub(r"defined\s*\(?\s*(\w+)\s*\)?", lambda g: "1" if g.group(1) in CO else "0", bt)
        try:
            return bool(eval(bt2.replace("||", " or ").replace("&&", " and ").replace("!", " not ")))
        except Exception:
            return None
    return None

def loc(dong):
    ra, i, n = [], 0, len(dong)
    ngan = []          # ngan xep: (dang_lay,)
    while i < n:
        t = dong[i].strip()
        if t.startswith("#if"):
            v = dinh_gia(dong[i])
            ngan.append(v)
            if v is None: ra.append(dong[i])
            i += 1; continue
        if t.startswith("#elif") or t == "#else":
            if ngan and ngan[-1] is not None:
                ngan[-1] = (not ngan[-1]) if t == "#else" else None
            else:
                ra.append(dong[i])
            i += 1; continue
        if t.startswith("#endif"):
            v = ngan.pop() if ngan else None
            if v is None: ra.append(dong[i])
            i += 1; continue
        if all(x is not False for x in ngan):
            ra.append(dong[i])
        i += 1
    return ra

def git(*a):
    r = subprocess.run(["git", "-C", GOC] + list(a), capture_output=True)
    return r.stdout.decode("latin-1")

tep = [t for t in git("diff", "--name-only", MOC, "--", "Sources").splitlines() if t.strip()]
loi = 0
if not tep:
    print("OK  khong co tep nao trong Sources/ bi sua so voi %s" % MOC)
for p in tep:
    cu = loc(git("show", "%s:%s" % (MOC, p)).splitlines())
    moi = loc(io.open(os.path.join(GOC, p), encoding="latin-1", newline="").read().splitlines())
    if cu == moi:
        print("OK  %s  (voi bo macro %s: giong het %s, %d dong)" % (p, NEN, MOC, len(cu)))
    else:
        loi += 1
        print("!! %s  KHAC khi bien dich cho %s:" % (p, NEN))
        import difflib
        for d in list(difflib.unified_diff(cu, moi, MOC, "sau-khi-sua", lineterm=""))[:20]:
            print("     " + d)
print("=" * 60)
print("KIEM TUONG DUONG %s: %s" % (NEN, ("DAT - %s bien dich y het" % NEN) if loi == 0 else "HONG - %d tep phai xem lai" % loi))
sys.exit(1 if loi else 0)
