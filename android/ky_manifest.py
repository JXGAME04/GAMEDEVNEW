# -*- coding: utf-8 -*-
"""[BAOMAT 12/09 KY] Ky ban ke tep tai ve (manifest.txt) bang ECDSA P-256.

VI SAO PHAI CO: bo tai kiem md5 tung tep THEO manifest.txt. Nhung neu ke dung giua sua duoc
chinh manifest.txt thi ho sua luon ca md5 -> kiem md5 thanh vo nghia. Trong cay du lieu co
script Lua, ma Lua la MA CHAY THAT, khong phai du lieu chet. Doi mot tep Lua = dieu khien
duoc may nguoi choi. Nen manifest PHAI co chu ky, va khoa cong khai nam SAN trong ung dung.

Dung:
  python3 android/ky_manifest.py --tao-khoa <duong dan khoa rieng>     # tao cap khoa mot lan
  python3 android/ky_manifest.py --ky <thu muc du lieu> --khoa <khoa rieng>
  python3 android/ky_manifest.py --in-khoa-cong-khai --khoa <khoa rieng>

KHOA RIENG KHONG DUOC nam trong kho ma nguon. Giu o may cua chu, hoac may chu dong goi.
Mat khoa rieng = phai doi khoa cong khai trong ung dung roi phat hanh lai.

Ket qua: <thu muc>/manifest.sig = chu ky DER cua ECDSA-SHA256 tren TOAN BO byte manifest.txt,
ma hoa base64. Bo tai tai ca hai tep, kiem chu ky truoc khi doc mot dong nao cua manifest.
"""
import base64, os, subprocess, sys

def chay(*a):
    r = subprocess.run(list(a), capture_output=True)
    if r.returncode != 0:
        sys.exit("LOI: %s\n%s" % (" ".join(a), r.stderr.decode("utf-8", "replace")))
    return r.stdout

def tao_khoa(p):
    if os.path.exists(p):
        sys.exit("da co %s - khong ghi de. Xoa tay neu that su muon tao khoa moi." % p)
    chay("openssl", "ecparam", "-name", "prime256v1", "-genkey", "-noout", "-out", p)
    os.chmod(p, 0o600)
    print("da tao khoa rieng: %s  (quyen 600)" % p)
    print("GIU KY: mat no thi phai doi khoa cong khai trong ung dung roi phat hanh lai.")
    in_khoa(p)

def in_khoa(p):
    der = chay("openssl", "ec", "-in", p, "-pubout", "-outform", "DER")
    print("\n// [BAOMAT 12/09 KY] khoa cong khai ECDSA P-256 (SubjectPublicKeyInfo, DER) - %d byte" % len(der))
    print("static const unsigned char s_jxKhoaCongKhai[] = {")
    for i in range(0, len(der), 12):
        print("\t" + " ".join("0x%02X," % b for b in der[i:i+12]))
    print("};")
    print("\n(chep khoi tren vao ios/JxTaiDuLieu.mm)")

def ky(thu, khoa):
    mf = os.path.join(thu, "manifest.txt")
    if not os.path.exists(mf):
        sys.exit("khong thay %s - chay may_chu_tai_du_lieu.py --chi-manifest truoc" % mf)
    sig = chay("openssl", "dgst", "-sha256", "-sign", khoa, mf)
    p = os.path.join(thu, "manifest.sig")
    open(p, "wb").write(base64.b64encode(sig) + b"\n")
    print("da ky %s (%d byte) -> %s (%d byte base64)" % (mf, os.path.getsize(mf), p, len(base64.b64encode(sig))))

def main():
    a = sys.argv[1:]
    if "--tao-khoa" in a:
        tao_khoa(a[a.index("--tao-khoa") + 1]); return
    if "--in-khoa-cong-khai" in a:
        in_khoa(a[a.index("--khoa") + 1]); return
    if "--ky" in a:
        ky(a[a.index("--ky") + 1], a[a.index("--khoa") + 1]); return
    sys.exit(__doc__)

main()
