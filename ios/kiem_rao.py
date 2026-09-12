# -*- coding: utf-8 -*-
r"""[IOS 11/09] KIEM RAO - chay sau MOI lan va nguon cho iOS (quy tac cua chu 11/09).
Chung minh bang may rang ban PC va ban Android KHONG doi hanh vi:

  1. Voi moi tep nguon dung chung ma ban iOS co sua: loc bo cac khoi dieu kien co JX_IOS / JX_MOBILE
     (lay dung nhanh #else neu co) roi so TUNG DONG voi ban trong git. Phai giong het.
  2. Kiem android/CMakeLists.txt KHONG dinh nghia JX_IOS va KHONG dinh nghia JX_MOBILE
     -> mo hinh o muc 1 moi dung. Neu mot ngay nao do Android bat JX_MOBILE thi kich ban nay
     bao loi de nguoi sua biet phai xem lai, chu khong im lang cho qua.

Chay:  python3 ios/kiem_rao.py            (so voi HEAD)
       python3 ios/kiem_rao.py <ref>      (so voi mot moc khac, dung sau khi da commit phan iOS)
"""
import io, os, re, subprocess, sys

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MOC = sys.argv[1] if len(sys.argv) > 1 else "HEAD"
MACRO = re.compile(r"\bJX_IOS\b")   # [JXMOBILE 11/09] Android NAY dinh nghia JX_MOBILE -> KHONG loc no nua

def loc_bo_rao_ios(dong):
    """Tra ve danh sach dong nhu trinh tien xu ly thay khi JX_IOS / JX_MOBILE KHONG duoc dinh nghia."""
    ra, i, n = [], 0, len(dong)
    while i < n:
        t = dong[i].strip()
        la_dk = t.startswith("#if") and MACRO.search(t)
        if not la_dk:
            ra.append(dong[i]); i += 1; continue
        phu_dinh = t.startswith("#ifndef") or re.match(r"#if\s*!\s*defined", t)
        muc, j, vt_else = 1, i + 1, None
        while j < n:
            u = dong[j].strip()
            if u.startswith("#if"):
                muc += 1
            elif u.startswith("#endif"):
                muc -= 1
                if muc == 0: break
            elif u == "#else" and muc == 1:
                vt_else = j
            j += 1
        if muc != 0:
            sys.exit("LOI: khoi #if bat dau dong %d khong co #endif" % (i + 1))
        if phu_dinh:          # #ifndef JX_IOS: khi khong dinh nghia thi nhanh DAU duoc lay
            ra.extend(loc_bo_rao_ios(dong[i + 1: vt_else if vt_else is not None else j]))
        elif vt_else is not None:   # #ifdef JX_IOS ... #else <phan cua Android/PC> #endif
            ra.extend(loc_bo_rao_ios(dong[vt_else + 1: j]))
        i = j + 1
    return ra

# ---- muc 2: macro cua iOS khong duoc co trong cau hinh Android ----
loi = 0
cm_android = io.open(os.path.join(GOC, "android", "CMakeLists.txt"), encoding="latin-1").read()
if re.search(r"\bJX_IOS\b", cm_android):
    print("!! android/CMakeLists.txt CO dinh nghia JX_IOS - khong duoc"); loi += 1
elif not re.search(r"\bJX_MOBILE\b", cm_android):
    print("!! android/CMakeLists.txt CHUA dinh nghia JX_MOBILE - mo hinh kiem khong con dung"); loi += 1
else:
    print("OK  android/CMakeLists.txt dinh nghia JX_MOBILE, khong dinh nghia JX_IOS")

# ---- muc 1: tung tep nguon dung chung ma ban iOS co sua ----
def git(*a):
    """Chay git va giai ma latin-1: nguon co byte cao (TCVN3/GBK), khong phai UTF-8."""
    r = subprocess.run(["git", "-C", GOC] + list(a), capture_output=True)
    return r.stdout.decode("latin-1")

tep = [t for t in git("diff", "--name-only", MOC, "--", "Sources").splitlines() if t.strip()]
if not tep:
    print("OK  khong co tep nao trong Sources/ bi sua so voi %s" % MOC)
for p in tep:
    # Loc ca HAI phia: sau khi da commit phan iOS thi ban trong git CUNG co nhanh JX_IOS.
    # Bat bien can kiem la "phan KHONG phai iOS khong doi", nen phai bo rao o ca hai ban roi moi so.
    cu = loc_bo_rao_ios(git("show", "%s:%s" % (MOC, p)).splitlines())
    moi = io.open(os.path.join(GOC, p), encoding="latin-1", newline="").read().splitlines()
    sau = loc_bo_rao_ios(moi)
    if sau == cu:
        print("OK  %s  (bo nhanh iOS o ca hai ban thi giong het %s, %d dong)" % (p, MOC, len(cu)))
    else:
        loi += 1
        print("!! %s  KHAC %s sau khi bo nhanh iOS:" % (p, MOC))
        import difflib
        for d in list(difflib.unified_diff(cu, sau, MOC, "sau-khi-loc", lineterm=""))[:24]:
            print("     " + d)
    # byte cao (TCVN3/GBK) phai giu nguyen so luong
    bc_cu = sum(1 for d in cu for c in d if ord(c) >= 0x80)
    bc_moi = sum(1 for d in moi for c in d if ord(c) >= 0x80)
    if bc_cu != bc_moi:
        loi += 1
        print("!! %s  so byte >= 0x80 doi %d -> %d" % (p, bc_cu, bc_moi))

print("=" * 60)
print("KIEM RAO: %s" % ("DAT - ban PC va Android khong doi" if loi == 0 else "HONG - %d cho phai xem lai" % loi))
sys.exit(1 if loi else 0)
