# -*- coding: utf-8 -*-
r"""[JXMOBILE 11/09] Chung minh dot doi ten JX_ANDROID -> JX_MOBILE la TRUNG TINH.

Lap luan: sau dot nay android/CMakeLists.txt dinh nghia CA HAI macro, nen moi rao doi ten van
duoc thoa y het -> Android khong doi hanh vi. Windows va may chu khong dinh nghia macro nao
trong hai -> cung khong doi.

Kiem bang MAY: voi moi tep bi sua, doi NGUOC JX_MOBILE -> JX_ANDROID roi so TUNG BYTE voi ban
trong git. Giong het => thay doi CHI LA doi ten macro, khong them bot mot dong logic nao.
Cong them: android/CMakeLists.txt phai dinh nghia JX_MOBILE va KHONG dinh nghia JX_IOS.

Chay:  python3 ios/kiem_doi_ten.py [moc]      (mac dinh so voi HEAD)
"""
import io, os, re, subprocess, sys
GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MOC = sys.argv[1] if len(sys.argv) > 1 else "HEAD"

def git(*a):
    return subprocess.run(["git", "-C", GOC] + list(a), capture_output=True).stdout.decode("latin-1")

loi = 0
cm = io.open(os.path.join(GOC, "android", "CMakeLists.txt"), encoding="latin-1").read()
if not re.search(r"\bJX_MOBILE\b", cm):
    print("!! android/CMakeLists.txt CHUA dinh nghia JX_MOBILE"); loi += 1
elif re.search(r"\bJX_IOS\b", cm):
    print("!! android/CMakeLists.txt dinh nghia JX_IOS - khong duoc"); loi += 1
else:
    print("OK  android/CMakeLists.txt dinh nghia JX_MOBILE, khong dinh nghia JX_IOS")

tep = [t for t in git("diff", "--name-only", MOC, "--", "Sources").splitlines() if t.strip()]
n_doiten = 0
for p in tep:
    cu = git("show", "%s:%s" % (MOC, p))
    moi = io.open(os.path.join(GOC, p), encoding="latin-1", newline="").read()
    nguoc = re.sub(r"\bJX_MOBILE\b", "JX_ANDROID", moi)
    if nguoc == cu:
        n_doiten += 1
        print("OK  %s  (chi doi ten macro, %d cho)" % (p, len(re.findall(r"\bJX_MOBILE\b", moi))))
    else:
        print("--  %s  (co thay doi khac ngoai doi ten - phai kiem bang ios/kiem_rao.py)" % p)

print("=" * 62)
print("KIEM DOI TEN: %d tep thuan doi ten." % n_doiten, end=" ")
print("DAT" if loi == 0 else "HONG (%d cho)" % loi)
sys.exit(1 if loi else 0)
