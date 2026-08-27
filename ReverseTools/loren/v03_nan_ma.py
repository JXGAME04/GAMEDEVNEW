# -*- coding: utf-8 -*-
r"""v03 - NAN MA VAT PHAM cho hai bang cau hinh cua he lo ren.

VI SAO PHAI NAN:
  Co che, cong thuc, thuat toan cua he lo ren giu 100% ban Linux. Nhung MA
  DINH DANH vat pham (genre, detailtype, particular) la cua TUNG KHO VAT PHAM.
  Kho cua JX1 danh so lien mach (particular == dong - 2), kho cua Linux danh so
  thua. Trung so nhung KHAC NGHIA - dung cai bay da vap voi Hinh nhan
  {6,1,1605} (o JX1 la "Thiep chuc su de") va voi 6/1/154 (o JX1 la "Tong Kim
  Chieu thu" - ve vao map 324).

CACH LAM DUY NHAT DUNG: bang goc co san COT TEN ben canh moi ma. Tra ten do
sang kho vat pham cua JX1 de lay ma dung. Khong suy so hoc, khong tru hang so.

Hai bang:
  atlas_compound.txt   8 cho co ma: ATLAS + 6 o nguyen lieu + DES
  itemvaluescript.txt  1 cho co ma: chinh dong do

Chay khong tham so = BAO CAO (khong ghi gi). Them --ghi de xuat tep.
"""
import io
import os
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location(
    "bangtxt", os.path.join(HERE, "..", "viemde", "bangtxt.py"))
bangtxt = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(bangtxt)

LNX = r"D:\ServerLinux\server1\settings\item"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item"
RA = os.path.join(HERE, "ra")

TAB = chr(9)
CR = chr(13)
NL = chr(10)


def doc_bang(p):
    d = io.open(p, "rb").read().decode("latin-1")
    dong = d.replace(CR + NL, NL).split(NL)
    return [ln.split(TAB) for ln in dong]


# Vai ten cua ban Linux khong trung TEN voi vat pham tuong duong cua JX1.
# Chi liet ke nhung cai da DOI CHIEU TAY va chac chan la mot mon:
#   Linux "Tran bang chi thach cap 1..10" (6,1,1019 + ITEM_LEVEL 1..10)
#   JX1   "Tran Bang Thach"               (6,1,1020) - mot muc, phan cap van
#         do cot ITEM_LEVEL cua chinh bang gia tri gánh.
#   >> CAI BAY: 6,1,1019 cua JX1 la "Hop hoa". Chep nguyen so la gan gia tri
#      hang tram trieu cho mot hop hoa.
DOI_TEN = {}
for _i in range(1, 11):
    DOI_TEN["tran bang chi thach cap %d" % _i] = "tran bang thach"

# itemvaluescript.txt: sau dong khoang phai doi THEO SO (Linux P -> JX1 P-1),
# khong theo ten. Xem giai thich o cho dung.
DOI_SO_ITEMVALUE = {}
for _p in range(200, 206):
    DOI_SO_ITEMVALUE[("6", "1", str(_p))] = ("6", "1", str(_p - 1))


def _bo_dau(s):
    return s


def chuan(s):
    """chuan hoa ten de doi chieu: bo khoang trang thua, ha chu thuong"""
    k = " ".join(bangtxt.tcvn2uni(s).split()).lower()
    return k


def tra_kho(kho, ten):
    """tra ten -> muc trong kho JX1, co di qua bang doi ten tay"""
    k = chuan(ten)
    if k in kho:
        return kho[k]
    # bo dau tieng Viet de so voi bang DOI_TEN (viet khong dau)
    import unicodedata
    kk = "".join(c for c in unicodedata.normalize("NFD", k)
                 if unicodedata.category(c) != "Mn").replace("đ", "d")
    if kk in DOI_TEN:
        for k2, v in kho.items():
            k2k = "".join(c for c in unicodedata.normalize("NFD", k2)
                          if unicodedata.category(c) != "Mn").replace("đ", "d")
            if k2k == DOI_TEN[kk]:
                return v
    return None


# ---------------------------------------------------------------------------
# Kho vat pham cua JX1
# ---------------------------------------------------------------------------
def nap_kho_jx1():
    kho = {}          # ten chuan -> (nguon, genre, detail, particular)
    trung = {}        # ten chuan -> so lan xuat hien
    theo_ma = {}      # 'g,d,p' -> ten goc (byte TCVN3)

    def them(nguon, ten, g, d, p):
        k = chuan(ten)
        if not k:
            return
        trung[k] = trung.get(k, 0) + 1
        if k not in kho:
            kho[k] = (nguon, g, d, p)
        theo_ma.setdefault("%s,%s,%s" % (g, d, p), ten)

    b = doc_bang(os.path.join(SRV, "magicscript.txt"))
    for c in b[1:]:
        if len(c) > 3 and c[0].strip():
            them("magicscript", c[0], c[1].strip(), c[2].strip(), c[3].strip())

    p = os.path.join(SRV, "questkey.txt")
    if os.path.isfile(p):
        b = doc_bang(p)
        for c in b[1:]:
            if len(c) > 2 and c[0].strip():
                # questkey cua JX1 KHONG co cot ParticularType
                them("questkey", c[0], c[1].strip(), c[2].strip(), "")

    # goldequip: san pham, tra theo SO DONG (khong phai particular)
    gold = {}
    p = os.path.join(SRV, "goldequip.txt")
    if os.path.isfile(p):
        b = doc_bang(p)
        for i, c in enumerate(b[1:], start=1):   # dong du lieu 1..n
            if c and c[0].strip():
                k = chuan(c[0])
                if k not in gold:
                    gold[k] = i
    return kho, trung, gold, theo_ma


# vi tri cac cho co ma trong atlas_compound.txt
CHO_ATLAS = [("ATLAS", 0, 1, 2, 3)]
for _k in range(6):
    _o = 4 + _k * 7
    CHO_ATLAS.append(("NL%d" % (_k + 1), _o, _o + 1, _o + 2, _o + 3))


def nan_atlas(kho, gold, ghi):
    ten_tep = "atlas_compound.txt"
    nguon = os.path.join(LNX, "000", ten_tep)
    b = doc_bang(nguon)
    if not b or len(b[0]) != 56:
        print("  !! atlas_compound.txt khong dung 56 cot")
        return

    doi = 0          # so o da doi ma
    giu = 0          # so o ma da dung
    thieu = {}       # ten khong co trong kho JX1
    des_ok = 0
    des_lech = []
    des_khongtra = []

    # Phan loai dong theo DES_PIECE - day la KHOA TRA CUU thu 4 cua atlas.lua
    #   makeItemKey( genre, detail, particular, nNoSign )
    # nNoSign lay tu manh thien thach nguoi choi bo vao o TU CHON (1..9).
    #   PIECE = 1..9  -> ghep ra MANH. Duong DUY NHAT ban goc dang bat.
    #   PIECE = 0     -> ghep THANG ra trang bi Hoang Kim. Ban goc TAT
    #                    (atlas.lua: if bNoSign ~= 1 then return LACK_RESOURCE).
    #   PIECE rong    -> khoa thanh (g,d,p,-1), nguoi choi khong bao gio tao
    #                    duoc -> dong CHET SAN trong bang goc.
    loai = {"manh": 0, "thang": 0, "chet": 0}
    thieu_song = {}   # chi tinh cho dong SONG (PIECE 1..9)

    # GOC CHUNG CUA HAI LOI DA BI PHAN BIEN BAT:
    # truoc day cho nao tra ten khong ra thi GIU NGUYEN MA LINUX. Ma Linux va
    # ma JX1 trung so khac nghia, nen giu nguyen la:
    #   - 210 dong Do pho khong co o JX1 van mang ma Linux, VA ma do lai la ma
    #     cua mot cuon Do pho KHAC dang co that ==> 27 khoa tra cuu bi hai
    #     cong thuc khac nhau cung dung.
    #   - Cam cuon A, nop nguyen lieu cua cong thuc B, van ra san pham cua B.
    # Nay: dong nao co BAT KY o nao khong tra duoc ten thi VO HIEU HOA ca dong
    # (xoa trang ba o ma cua Do pho) de khoa tra cuu khong bao gio khop.
    vo_hieu = []

    tong_dong = 0
    for i in range(1, len(b)):
        c = b[i]
        if len(c) < 56:
            continue
        tong_dong += 1
        _hong = []      # cac o cua chinh dong nay khong tra duoc ten
        _pc = c[53].strip()
        if _pc == "":
            _loai = "chet"
        elif _pc == "0":
            _loai = "thang"
        else:
            _loai = "manh"
        loai[_loai] += 1
        # --- 7 cho nguyen lieu / do pho ---
        for nhan, ci, cg, cd, cp in CHO_ATLAS:
            ten = c[ci].strip()
            if not ten or not c[cg].strip():
                continue
            _m = tra_kho(kho, ten)
            if _m is None:
                _kh = (bangtxt.tcvn2uni(ten), "%s,%s,%s" % (c[cg], c[cd], c[cp]))
                thieu[_kh] = thieu.get(_kh, 0) + 1
                if _loai == "manh":
                    thieu_song[_kh] = thieu_song.get(_kh, 0) + 1
                _hong.append(nhan)
                continue
            _, jg, jd, jp = _m
            cu = (c[cg].strip(), c[cd].strip(), c[cp].strip())
            if _m[0] == "questkey":
                # questkey JX1 khong co particular -> ep ve rong
                moi = (jg, jd, "0")
            else:
                moi = (jg, jd, jp)
            if cu == moi:
                giu += 1
            else:
                c[cg], c[cd], c[cp] = moi
                doi += 1
        # --- san pham ---
        ten_des = c[46].strip()
        q = c[47].strip()
        if ten_des:
            k = chuan(ten_des)
            if q == "1":
                # trang bi Hoang Kim: DES_DETAILTYPE = SO DONG goldequip
                if k in gold:
                    moi = str(gold[k])
                    if c[49].strip() == moi:
                        des_ok += 1
                    else:
                        des_lech.append((bangtxt.tcvn2uni(ten_des),
                                         c[49].strip(), moi))
                        c[49] = moi
                else:
                    des_khongtra.append((bangtxt.tcvn2uni(ten_des), "goldequip", _loai))
                    _hong.append("DES")
            else:
                # manh: tra theo ten trong kho JX1
                _md = tra_kho(kho, ten_des)
                if _md is not None:
                    _, jg, jd, jp = _md
                    cu = (c[48].strip(), c[49].strip(), c[50].strip())
                    moi = (jg, jd, "0" if _md[0] == "questkey" else jp)
                    if cu == moi:
                        des_ok += 1
                    else:
                        des_lech.append((bangtxt.tcvn2uni(ten_des),
                                         ",".join(cu), ",".join(moi)))
                        c[48], c[49], c[50] = moi
                else:
                    des_khongtra.append((bangtxt.tcvn2uni(ten_des), "manh", _loai))
                    _hong.append("DES")

        # VO HIEU HOA dong khong tra duoc: xoa trang ba o ma cua Do pho.
        # atlas.lua doc bang TabFile_GetCell(..., -1) nen o trong thanh -1,
        # khoa tra cuu la "-1,-1,-1,<piece>" - nguoi choi khong bao gio cam
        # duoc mot vat pham co ma (-1,-1,-1) nen dong nay chet han.
        if _hong:
            vo_hieu.append((i, bangtxt.tcvn2uni(c[0])[:46],
                            "%s,%s,%s" % (c[1], c[2], c[3]), ",".join(_hong[:4])))
            c[1] = ""
            c[2] = ""
            c[3] = ""

    print()
    print("=" * 92)
    print("  atlas_compound.txt : %d dong du lieu" % tong_dong)
    print("=" * 92)
    print("  PHAN LOAI DONG (theo cot DES_PIECE - khoa tra cuu thu 4):")
    print("     %4d  DES_PIECE = 1..9  ghep ra MANH   << DUONG DUY NHAT BAN GOC BAT"
          % loai["manh"])
    print("     %4d  DES_PIECE = 0     ghep THANG ra trang bi Hoang Kim (ban goc TAT)"
          % loai["thang"])
    print("     %4d  DES_PIECE rong    dong CHET SAN trong bang goc (khoa -1)"
          % loai["chet"])
    print()
    print("  o nguyen lieu / do pho : GIU %d  |  DOI MA %d  |  KHONG TRA DUOC %d loai"
          % (giu, doi, len(thieu)))
    print("     trong do o DONG SONG (PIECE 1..9): %d loai" % len(thieu_song))
    print("  san pham               : DUNG %d  |  DOI %d  |  KHONG TRA DUOC %d"
          % (des_ok, len(des_lech), len(des_khongtra)))
    _sp_song = sum(1 for x in des_khongtra if x[2] == "manh")
    print("     trong do o DONG SONG (PIECE 1..9): %d" % _sp_song)

    print()
    print("  >> VO HIEU HOA %d dong (co o khong tra duoc ten trong kho JX1)."
          % len(vo_hieu))
    print("     Truoc day cac dong nay GIU NGUYEN ma Linux, ma ma do o JX1 lai")
    print("     la mot vat pham KHAC -> sinh khoa tra cuu trung. Nay khoa cua")
    print("     chung la (-1,-1,-1) nen khong bao gio khop.")
    if vo_hieu:
        print("     5 vi du dau:")
        for d, ten, ma, cho in vo_hieu[:5]:
            print("       dong %-5d %-46s ma cu %-12s hong: %s" % (d, ten, ma, cho))

    if thieu:
        print()
        print("--- TEN KHONG CO TRONG KHO VAT PHAM CUA JX1 ---")
        for (ten, ma), n in sorted(thieu.items(), key=lambda x: -x[1])[:60]:
            danh = "DONG SONG" if (ten, ma) in thieu_song else "dong tat/chet"
            print("   %-52s %-12s (%2d o, %s)" % (ten[:52], ma, n, danh))
        if len(thieu) > 60:
            print("   ... con %d loai nua" % (len(thieu) - 60))

    if des_khongtra:
        print()
        print("--- SAN PHAM KHONG TRA DUOC (gop theo loai dong) ---")
        seen = {}
        for ten, kind, lo in des_khongtra:
            seen[lo] = seen.get(lo, 0) + 1
        for lo, n in sorted(seen.items(), key=lambda x: -x[1]):
            print("   loai dong %-8s : %d dong" % (lo, n))

    if des_lech:
        print()
        print("--- SAN PHAM DOI MA (5 vi du dau) ---")
        for ten, cu, moi in des_lech[:5]:
            print("   %-52s %-14s -> %s" % (ten[:52], cu, moi))
        print("   (tong %d)" % len(des_lech))

    if ghi:
        if not os.path.isdir(RA):
            os.makedirs(RA)
        p = os.path.join(RA, ten_tep)
        # giu nguyen so cot va kieu xuong dong cua ban goc (LF)
        d = io.open(os.path.join(LNX, "000", ten_tep), "rb").read().decode("latin-1")
        kieu = CR + NL if d.count(CR + NL) > (d.count(NL) - d.count(CR + NL)) else NL
        out = kieu.join(TAB.join(r) for r in b)
        io.open(p, "wb").write(out.encode("latin-1"))
        print()
        print("  >> da ghi %s (%d byte, xuong dong %s)"
              % (p, len(out), "CRLF" if kieu != NL else "LF"))


def nan_itemvalue(kho, ghi, ten_jx1_theo_ma=None):
    ten_tep = "itemvaluescript.txt"
    nguon = os.path.join(LNX, ten_tep)
    b = doc_bang(nguon)
    if not b:
        return
    hdr = [x.strip() for x in b[0]]
    try:
        cN = hdr.index("ITEM_NAME")
        cQ = hdr.index("ITEM_QUALITY")
        cG = hdr.index("ITEM_GENRE")
        cD = hdr.index("ITEM_DETAILTYPE")
        cP = hdr.index("ITEM_PARTICULAR")
    except ValueError:
        print("  !! itemvaluescript.txt thieu cot")
        return

    ncot = len(hdr)
    giu = doi = 0
    thieu = []
    boqua = 0
    bo_dong = []
    ra_dong = [b[0]]
    for i in range(1, len(b)):
        c = b[i]
        if len(c) == 1 and c[0].strip() == "":
            continue                       # dong rong cuoi tep
        if len(c) <= cP or not c[cN].strip():
            continue
        # dong tra theo QUALITY+GENRE (khong co particular) = luat chung,
        # khong co ma vat pham cu the -> khong nan
        if c[cQ].strip() != "" and c[cP].strip() == "":
            boqua += 1
            ra_dong.append(c)
            continue
        # DOI THEO SO, KHONG THEO TEN, cho 6 dong khoang.
        # Cot ten cua CHINH BAN LINUX gan sai: Mat Ngan / Chu Sa / Khong Tuoc /
        # Phu Dung deo nham ma. O ban goc vo hai (ca 6 dong deu tro ve cung mot
        # kich ban ore.lua, cot ITEMVALUE de trong) nhung nan theo ten thi ra
        # tap ma dung ma cap ten-ma lai lech - de lai la min cho nguoi sua sau.
        _ma_cu = (c[cG].strip(), c[cD].strip(), c[cP].strip())
        if _ma_cu in DOI_SO_ITEMVALUE:
            _moi = DOI_SO_ITEMVALUE[_ma_cu]
            if _ma_cu != _moi:
                c[cG], c[cD], c[cP] = _moi
                doi += 1
            else:
                giu += 1
            if ten_jx1_theo_ma is not None:
                _tk = "%s,%s,%s" % _moi
                if _tk in ten_jx1_theo_ma:
                    c[cN] = ten_jx1_theo_ma[_tk]
            ra_dong.append(c)
            continue

        m = tra_kho(kho, c[cN])
        if m is None:
            thieu.append((bangtxt.tcvn2uni(c[cN]),
                          "%s,%s,%s" % (c[cG].strip(), c[cD].strip(), c[cP].strip())))
            bo_dong.append(i)
            continue                        # BO dong - de lai la gan gia tri
                                            # cho mot vat pham KHAC cua JX1
        ng, jg, jd, jp = m
        moi = (jg, jd, "0" if ng == "questkey" else jp)
        cu = (c[cG].strip(), c[cD].strip(), c[cP].strip())
        if cu == moi:
            giu += 1
        else:
            c[cG], c[cD], c[cP] = moi
            doi += 1
        # Bang goc cua Linux co 4 dong khoang GAN SAI TEN vao ma (Mat Ngan /
        # Chu Sa / Khong Tuoc / Phu Dung lech nhau) - vo hai o ban goc vi ca 6
        # dong deu tro ve cung mot kich ban ore.lua, nhung de lai la lua nguoi
        # doc sau. Ghi lai TEN theo dung kho vat pham cua JX1.
        if ten_jx1_theo_ma is not None:
            _tk = "%s,%s,%s" % moi
            if _tk in ten_jx1_theo_ma:
                c[cN] = ten_jx1_theo_ma[_tk]
        ra_dong.append(c)

    # SAN LUOI COT: ban goc co 4 dong thieu mot TAB cuoi (dong 38/39/61/62).
    # KTabFile lech luoi la Load() tra FALSE -> vong nap bo ngang -> sap
    # GameServer (dung su co 26/08). Bat buoc bu du cot.
    bu = 0
    for c in ra_dong:
        while len(c) < ncot:
            c.append("")
            bu += 1
        if len(c) > ncot:
            del c[ncot:]

    print()
    print("=" * 92)
    print("  itemvaluescript.txt : GIU %d  |  DOI MA %d  |  BO %d dong  |  luat chung %d"
          % (giu, doi, len(bo_dong), boqua))
    print("     san luoi cot: bu %d o trong cho du %d cot" % (bu, ncot))
    print("=" * 92)
    if thieu:
        print("--- BO DONG (JX1 khong co vat pham nay; giu lai la gan gia tri nham mon khac) ---")
        for ten, ma in thieu:
            print("   %-58s bang ghi %s" % (ten[:58], ma))

    if ghi:
        if not os.path.isdir(RA):
            os.makedirs(RA)
        p = os.path.join(RA, ten_tep)
        d = io.open(nguon, "rb").read().decode("latin-1")
        kieu = CR + NL if d.count(CR + NL) > (d.count(NL) - d.count(CR + NL)) else NL
        out = kieu.join(TAB.join(r) for r in ra_dong) + kieu
        io.open(p, "wb").write(out.encode("latin-1"))
        print("  >> da ghi %s (%d byte, %d dong)" % (p, len(out), len(ra_dong)))


def main():
    ghi = "--ghi" in sys.argv
    kho, trung, gold, theo_ma = nap_kho_jx1()
    print("KHO VAT PHAM JX1: %d ten rieng | goldequip: %d ten" % (len(kho), len(gold)))
    nhieu = sum(1 for v in trung.values() if v > 1)
    print("   (%d ten bi TRUNG trong kho - lay muc dau tien)" % nhieu)
    nan_atlas(kho, gold, ghi)
    nan_itemvalue(kho, ghi, theo_ma)
    if not ghi:
        print()
        print("  [BAO CAO - chua ghi tep nao. Them --ghi de xuat ra %s]" % RA)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
