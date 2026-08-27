# -*- coding: utf-8 -*-
r"""v37 - SOAT TOAN BO VAT PHAM lien quan Viem De: chuc nang, hinh anh, thong tin.

Kiem 6 viec cho tung vat pham:
  1. Co ton tai trong bang magicscript.txt cua MAY CHU khong
  2. Bang cua CLIENT co dong y het khong (lech la client ve sai / doc rac)
  3. Ten hien thi co dung nghia khong (doi chieu ten mong doi)
  4. Tep anh (ImageName) co that khong - tim ca tren DIA lan trong PAK
     (dung ham bam DA KIEM CHUNG o pak_id.py: char CO DAU)
  5. Tep kich ban chuc nang (cot Script) co that khong - vat pham "dung duoc"
     ma tro vao tep khong ton tai la bam vao khong co gi xay ra
  6. Kich thuoc o tui (Width x Height) va so luong chong (nMaxStack)

Nguon danh sach vat pham: quet THUC TE cac kich ban Viem De dang chay, cong voi
bang doi ma cua v09 (Linux -> JX1).
"""
import io
import os
import re
import struct
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))


def _nap(ten, tep):
    spec = importlib.util.spec_from_file_location(ten, os.path.join(HERE, tep))
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


bangtxt = _nap("bangtxt", "bangtxt.py")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
KB = os.path.join(SRV, r"script\missions\yandibaozang")
SEP = "\\"

# ten mong doi, lay tu chu thich bang doi ma cua v09 (Linux -> JX1)
MONG_DOI = {
    "6,1,3": "Đại Lực hoàn", "6,1,6": "Phi Tốc hoàn",
    "6,1,146": "Huyền Tinh Khoáng Thạch", "6,1,214": "Càn Khôn Tạo Hóa Đan",
    "6,1,397": "Thần bí khoáng thạch", "6,1,1314": "Mảnh Bổ Thiên Thạch",
    "6,1,1613": "Anh Hùng Thiếp", "6,1,1614": "Hình nhân",
    "6,1,1615": "Viêm Đế Đồ Đằng", "6,1,1626": "Viêm Đế Lệnh",
    "6,1,1755": "Phượng Minh Chùy", "6,1,2360": "Huyền Viên Lệnh",
    "6,1,2541": "Viêm Đế Lệnh Kỳ", "6,1,3362": "Chìa Khóa Như ý",
    "6,1,3423": "Viêm Đế Bí Bảo", "6,1,3821": "Hộ Mạch Đơn",
    "6,1,4428": "Tinh Thiết Khoáng", "6,1,4429": "Tinh Tinh Khoáng",
    "6,1,3926": "Chân Nguyên Đan", "6,1,4862": "Đồ Phổ Tinh Sương Y",
    "6,1,4867": "Đồ Phổ Tinh Sương Yêu Đái", "6,1,4868": "Đồ Phổ Tinh Sương Ngọc Bội",
    "6,1,2953": "Chìa khóa vàng", "6,1,4846": "Chân Nguyên Đơn (trung)",
    "6,1,4847": "Chân Nguyên Đơn (đại)", "6,1,3051": "Huyết Long Đằng",
    "6,1,4857": "Hỗn nguyên chân đơn", "6,1,4865": "Hộp Mặt Nạ Chiến Trường",
    "6,1,4863": "Đồ Phổ Tinh Sương Khí Giới", "6,1,4860": "Tinh Sương Lệnh",
    "6,1,4861": "Huyền Thiết", "6,1,4858": "Đồ Phổ Đằng Long Y",
    "6,1,4869": "Đồ Phổ Đằng Long Bội", "6,1,4859": "Đồ Phổ Đằng Long Khí Giới",
    "6,1,4866": "Đằng Long Thạch - Hạ", "6,1,4813": "Túi Dược Phẩm",
}


# --------------------------------------------------------------- bang vat pham
def doc_bang(duong):
    """tra ve {(g,d,p): {cot: gia tri}} + danh sach ten cot"""
    d = io.open(duong, "rb").read().decode("latin-1")
    dong = d.replace("\r\n", "\n").split("\n")
    cot = dong[0].split("\t")
    ra = {}
    for ln in dong[1:]:
        c = ln.split("\t")
        if len(c) < 5:
            continue
        try:
            khoa = "%s,%s,%s" % (int(c[1]), int(c[2]), int(c[3]))
        except ValueError:
            continue
        ra[khoa] = dict(zip(cot, c))
    return ra, cot


# ------------------------------------------------------------------ anh trong pak
def nap_pak(goc):
    pak_id = _nap("pak_id", "pak_id.py")
    ids = set()
    thumuc = os.path.join(goc, "data")
    if not os.path.isdir(thumuc):
        return ids, pak_id
    for f in sorted(os.listdir(thumuc)):
        if not f.lower().endswith((".pak", ".mps")):
            continue
        try:
            fh = open(os.path.join(thumuc, f), "rb")
            h = fh.read(32)
            if h[:4] != b"PACK":
                fh.close()
                continue
            cnt, itoff, _ = struct.unpack_from("<III", h, 4)
            fh.seek(itoff)
            raw = fh.read(16 * cnt)
            fh.close()
        except Exception:
            continue
        for k in range(len(raw) // 16):
            ids.add(struct.unpack_from("<I", raw, 16 * k)[0])
    return ids, pak_id


def co_tep(duongdan, ids, pak_id):
    """tra ve 'dia' / 'pak' / '' - duongdan dang \\spr\\...\\x.spr"""
    if not duongdan:
        return ""
    p = duongdan if duongdan.startswith(SEP) else SEP + duongdan
    if os.path.isfile(CLI + p):
        return "đĩa"
    if pak_id.file_name_to_id(p) in ids:
        return "pak"
    return ""


# --------------------------------------------------------------------- quet
def quet_kichban():
    """gom moi bo ba {g, d, p} xuat hien trong kich ban Viem De dang chay"""
    ra = {}
    for dp, dn, fn in os.walk(KB):
        for f in fn:
            if not f.lower().endswith(".lua"):
                continue
            d = io.open(os.path.join(dp, f), "rb").read().decode("latin-1")
            for m in re.finditer(r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*[,}]", d):
                g, dt, p = m.group(1), m.group(2), m.group(3)
                if g != "6":
                    continue
                ra.setdefault("%s,%s,%s" % (g, dt, p), []).append(f)
    return ra


def main():
    print("=" * 96)
    print("  SOAT VAT PHAM VIEM DE - chuc nang / hinh anh / thong tin")
    print("=" * 96)

    bs, _ = doc_bang(os.path.join(SRV, r"settings\item\magicscript.txt"))
    bc, _ = doc_bang(os.path.join(CLI, r"settings\item\magicscript.txt"))
    print("bang may chu: %d muc | bang client: %d muc" % (len(bs), len(bc)))

    ids, pak_id = nap_pak(CLI)
    print("chi muc pak client: %d muc\n" % len(ids))

    dung = quet_kichban()
    khoa = sorted(set(list(MONG_DOI.keys()) + list(dung.keys())),
                  key=lambda k: int(k.split(",")[2]))

    loi = []
    print("%-11s %-30s %-6s %-5s %-6s %-5s %s"
          % ("ma", "ten trong bang may chu", "anh", "cỡ", "chồng", "k.bản", "ghi chú"))
    print("-" * 96)
    for k in khoa:
        if k not in bs:
            if k in dung:
                print("%-11s %s" % (k, "*** KHONG CO TRONG BANG MAY CHU ***"))
                loi.append((k, "thiếu trong bảng máy chủ"))
            continue
        r = bs[k]
        ten = bangtxt.tcvn2uni(r.get("Name", ""))
        anh = r.get("ImageName", "")
        kq_anh = co_tep(anh, ids, pak_id)
        w, h = r.get("Width", "?"), r.get("Height", "?")
        stack = r.get("nMaxStack", "?")
        sc = r.get("Script", "")
        kq_sc = "-"
        if sc and sc not in ("0", ""):
            kq_sc = "có" if os.path.isfile(SRV + (sc if sc.startswith(SEP) else SEP + sc)) else "THIẾU"

        ghi = []
        if not kq_anh:
            ghi.append("KHÔNG THẤY ẢNH")
            loi.append((k, "không thấy ảnh " + anh))
        if kq_sc == "THIẾU":
            ghi.append("THIẾU KỊCH BẢN " + sc)
            loi.append((k, "thiếu kịch bản " + sc))
        if k in bc:
            rc = bc[k]
            if rc.get("Name", "") != r.get("Name", ""):
                ghi.append("TÊN LỆCH CLIENT: " + bangtxt.tcvn2uni(rc.get("Name", "")))
                loi.append((k, "tên lệch giữa máy chủ và client"))
            elif rc.get("ImageName", "") != anh:
                ghi.append("ẢNH LỆCH CLIENT")
                loi.append((k, "ảnh lệch giữa máy chủ và client"))
        else:
            ghi.append("CLIENT KHÔNG CÓ MỤC NÀY")
            loi.append((k, "client thiếu mục"))
        if k in MONG_DOI and MONG_DOI[k].lower() not in ten.lower() \
                and ten.lower() not in MONG_DOI[k].lower():
            ghi.append("mong đợi: " + MONG_DOI[k])
            loi.append((k, "tên khác mong đợi (%s)" % MONG_DOI[k]))
        if k in dung:
            ghi.append("dùng ở: " + ", ".join(sorted(set(dung[k]))[:3]))

        print("%-11s %-30s %-6s %-5s %-6s %-5s %s"
              % (k, ten[:30], kq_anh or "!!", w + "x" + h, stack, kq_sc,
                 " | ".join(ghi)))

    print()
    print("=" * 96)
    if loi:
        print("  CO %d VAN DE:" % len(loi))
        for k, v in loi:
            print("   %-11s %s" % (k, v))
    else:
        print("  KHONG CO VAN DE - moi vat pham deu du ten, anh, kich ban va khop hai ban bang.")
    print("=" * 96)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
