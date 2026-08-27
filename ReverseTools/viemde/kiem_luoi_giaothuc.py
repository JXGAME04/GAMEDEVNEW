# -*- coding: utf-8 -*-
"""Doi chieu LUOI GIAO THUC: enum c2s/s2c  <->  khoi khoi tao g_nProtocolSize.

Vi sao phai co: g_nProtocolSize la khoi khoi tao PHANG, duoc tra cuu bang
    ban client: g_nProtocolSize[s2c_id - s2c_clientbegin - 1]
    ban server: g_nProtocolSize[c2s_id - c2s_gameserverbegin - 1]
Lech MOT o la hai dau hieu sai do dai goi -> rot nguoi choi, va loi nay
KHONG lo ra luc bien dich.

Cach kiem: moi phan tu trong khoi khoi tao deu co chu thich cuoi dong ghi TEN
giao thuc ma tac gia NGHI no la. Ta suy ten tu VI TRI roi so voi ten trong chu
thich. Hai ten lech nhau = luoi da hong.

Chay:  python kiem_luoi_giaothuc.py [duong_dan_cay]
"""
import io
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CAY = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW"
F_DEF = os.path.join(CAY, "Headers", "KProtocolDef.h")
F_SIZE = os.path.join(CAY, "Sources", "Core", "Src", "KProtocol.cpp")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


# --------------------------------------------------------------- enum
def lay_enum(txt, ten):
    """tra ve [(ten_thanh_vien, gia_tri, so_dong)] - BO thanh vien bi chu thich"""
    m = re.search(r"enum\s+" + ten + r"\s*\{", txt)
    if not m:
        return None
    i = m.end()
    sau = 1
    j = i
    while j < len(txt) and sau:
        if txt[j] == "{":
            sau += 1
        elif txt[j] == "}":
            sau -= 1
        j += 1
    than = txt[i:j - 1]
    dong0 = txt[:i].count("\n") + 1

    ra = []
    gt = 0
    for k, dong in enumerate(than.split("\n")):
        # bo chu thich -> thanh vien bi chu thich KHONG chiem gia tri
        ma = re.sub(r"/\*.*?\*/", "", dong)
        ma = re.split(r"//", ma)[0]
        for phan in ma.split(","):
            phan = phan.strip()
            if not phan:
                continue
            mm = re.match(r"^([A-Za-z_]\w*)\s*(?:=\s*(.+))?$", phan)
            if not mm:
                continue
            nm, val = mm.group(1), mm.group(2)
            if val is not None:
                val = val.strip()
                try:
                    gt = int(val, 0)
                except ValueError:
                    # gia tri la bieu thuc tham chieu thanh vien khac
                    truoc = {a: b for a, b, _ in ra}
                    try:
                        gt = eval(val, {"__builtins__": {}}, truoc)
                    except Exception:
                        gt = None
            ra.append((nm, gt, dong0 + k))
            if gt is not None:
                gt += 1
    return ra


# --------------------------------------- khoi khoi tao g_nProtocolSize
def lay_khoi(txt):
    """tra ve {'client': [(ma, chuthich, dong)], 'server': [...]}"""
    m = re.search(r"g_nProtocolSize\s*\[[^\]]*\]\s*=\s*\{", txt)
    if not m:
        return None
    i = m.end()
    sau = 1
    j = i
    while j < len(txt) and sau:
        if txt[j] == "{":
            sau += 1
        elif txt[j] == "}":
            sau -= 1
        j += 1
    than = txt[i:j - 1]
    dong0 = txt[:i].count("\n") + 1

    ra = {"client": [], "server": []}
    nhanh = None
    dem = []          # ngan xep #if long nhau
    for k, dong in enumerate(than.split("\n")):
        rut = dong.strip()
        if rut.startswith("#"):
            if re.match(r"#\s*ifndef\s+_SERVER", rut):
                dem.append("client")
                nhanh = "client"
            elif re.match(r"#\s*ifdef\s+_SERVER", rut):
                dem.append("server")
                nhanh = "server"
            elif re.match(r"#\s*if", rut):
                dem.append(nhanh)
            elif re.match(r"#\s*else", rut):
                if dem:
                    dem[-1] = "server" if dem[-1] == "client" else "client"
                    nhanh = dem[-1]
            elif re.match(r"#\s*endif", rut):
                if dem:
                    dem.pop()
                nhanh = dem[-1] if dem else None
            continue
        if nhanh is None:
            continue
        # tach ma / chu thich
        ma = re.sub(r"/\*.*?\*/", "", dong)
        cat = ma.split("//", 1)
        ma, chu = cat[0], (cat[1].strip() if len(cat) > 1 else "")
        for phan in ma.split(","):
            if phan.strip():
                ra[nhanh].append((phan.strip(), chu, dong0 + k))
    return ra


def ten_trong_chuthich(chu):
    """rut ten giao thuc dau tien xuat hien trong chu thich"""
    m = re.search(r"\b((?:s2c|c2s)_\w+)", chu)
    return m.group(1) if m else None


def kiem(nhan, thanhvien, moc_ten, phantu):
    """thanhvien: enum day du; moc_ten: ten thanh vien lam goc (id - goc - 1)"""
    print("=" * 78)
    print("  %s" % nhan)
    print("=" * 78)
    goc = None
    for nm, gt, _ in thanhvien:
        if nm == moc_ten:
            goc = gt
            break
    if goc is None:
        print("  !! khong tim thay moc '%s'" % moc_ten)
        return 1
    print("  moc %s = %d  ->  chi so 0 ung voi gia tri %d" % (moc_ten, goc, goc + 1))

    # ban do gia tri -> ten (chi lay thanh vien co gia tri > goc)
    theo_gt = {}
    for nm, gt, dong in thanhvien:
        if gt is not None and gt > goc:
            theo_gt.setdefault(gt, (nm, dong))

    print("  so phan tu trong khoi khoi tao: %d" % len(phantu))
    loi = 0
    khop = 0
    khong_chu = 0
    for idx, (ma, chu, dong) in enumerate(phantu):
        gt = goc + 1 + idx
        suy = theo_gt.get(gt)
        ten_suy = suy[0] if suy else None
        ten_chu = ten_trong_chuthich(chu)
        if ten_chu is None:
            khong_chu += 1
            continue
        if ten_suy is None:
            print("  [%3d] gia tri %3d: KHONG CO thanh vien enum, nhung chu thich ghi '%s'  (%s:%d)"
                  % (idx, gt, ten_chu, os.path.basename(F_SIZE), dong))
            loi += 1
        elif ten_chu != ten_suy:
            print("  [%3d] gia tri %3d: vi tri suy ra '%s'  NHUNG chu thich ghi '%s'  (%s:%d)"
                  % (idx, gt, ten_suy, ten_chu, os.path.basename(F_SIZE), dong))
            loi += 1
        else:
            khop += 1
    print("  -> khop %d, LECH %d, khong co chu thich de doi chieu %d"
          % (khop, loi, khong_chu))
    # phan tu cuoi cung ung voi gia tri nao
    if phantu:
        print("  -> phan tu cuoi ung voi gia tri %d" % (goc + len(phantu)))
    return loi


def main():
    td = doc(F_DEF)
    ts = doc(F_SIZE)
    c2s = lay_enum(td, "c2s_PROTOCOL")
    s2c = lay_enum(td, "s2c_PROTOCOL")
    khoi = lay_khoi(ts)
    if not (c2s and s2c and khoi):
        print("!! khong doc duoc enum hoac khoi khoi tao")
        return 2
    print("cay: %s" % CAY)
    print("enum c2s: %d thanh vien | enum s2c: %d thanh vien" % (len(c2s), len(s2c)))
    print("khoi khoi tao: nua client %d phan tu, nua server %d phan tu\n"
          % (len(khoi["client"]), len(khoi["server"])))
    loi = 0
    loi += kiem("NUA CLIENT  (s2c_id - s2c_clientbegin - 1)",
                s2c, "s2c_clientbegin", khoi["client"])
    print()
    loi += kiem("NUA SERVER  (c2s_id - c2s_gameserverbegin - 1)",
                c2s, "c2s_gameserverbegin", khoi["server"])
    print()
    print("=" * 78)
    print("  TONG: %s" % ("LUOI DA LECH - %d cho" % loi if loi else "khop hoan toan"))
    print("=" * 78)
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
