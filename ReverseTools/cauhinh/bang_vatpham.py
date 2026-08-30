# -*- coding: utf-8 -*-
"""bang_vatpham.py - tra bang vat pham DUNG CACH MAY TRA.

BAY (bo phan bien 29/08 bat duoc, da tu doc ma xac nhan):
  KItemGenerator.CPP:1660-1661
        const int i = nParticularType;
        pMagicScript = m_BPTLib.GetMagicScript(i);
  -> KBasPropTbl.cpp:1058   return ((KBASICPROP_MAGICSCRIPT*)m_pBuf) + i;
  Tuc may lay PHAN TU THU i cua mang - tra theo CHI SO DONG, khong phai tra
  theo cot ParticularType.

  Trong magicscript.txt hien tai, hai thu nay LECH NHAU o 35 dong (tu chi so
  4881 tro di). Do bang `_kiem_magicscript_chiso.py`. Voi 18 chi so thi
  "tra theo cot" va "tra theo chi so" cho HAI MON KHAC NHAU.

  Cong cu nao tra theo cot se nan ma sai o vung do MA KHONG BAO LOI.

Module nay luon tra theo CHI SO, va co ham bao cao vung lech de ai dung cung
biet minh dang o dau.
"""
import io
import os
import re
import sys

sys.path.insert(0, os.path.join(r"D:\GAMEDEVNEW\ReverseTools", "viemde"))
from bangtxt import tcvn2uni  # noqa: E402

MS_MAC_DINH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
               r"\settings\item\magicscript.txt")


def _gon(s):
    """Chuan hoa ten DA LA Unicode de doi chieu."""
    return re.sub(r"[\s\(\)\-]+", " ", s).strip().lower()


_DAU = {
    "à": "a", "á": "a", "ả": "a", "ã": "a", "ạ": "a",
    "ă": "a", "ằ": "a", "ắ": "a", "ẳ": "a", "ẵ": "a", "ặ": "a",
    "â": "a", "ầ": "a", "ấ": "a", "ẩ": "a", "ẫ": "a", "ậ": "a",
    "è": "e", "é": "e", "ẻ": "e", "ẽ": "e", "ẹ": "e",
    "ê": "e", "ề": "e", "ế": "e", "ể": "e", "ễ": "e", "ệ": "e",
    "ì": "i", "í": "i", "ỉ": "i", "ĩ": "i", "ị": "i",
    "ò": "o", "ó": "o", "ỏ": "o", "õ": "o", "ọ": "o",
    "ô": "o", "ồ": "o", "ố": "o", "ổ": "o", "ỗ": "o", "ộ": "o",
    "ơ": "o", "ờ": "o", "ớ": "o", "ở": "o", "ỡ": "o", "ợ": "o",
    "ù": "u", "ú": "u", "ủ": "u", "ũ": "u", "ụ": "u",
    "ư": "u", "ừ": "u", "ứ": "u", "ử": "u", "ữ": "u", "ự": "u",
    "ỳ": "y", "ý": "y", "ỷ": "y", "ỹ": "y", "ỵ": "y",
    "đ": "d",
}


def _bo_dau(s):
    return "".join(_DAU.get(c, c) for c in s)


def _gon_manh(s):
    """Chuan hoa MANH: bo dau tieng Viet + bo tu chi cap do, de khop GAN DUNG.

    Hai ly do that, deu tu do:
     - Bo kiem tung bao "Dai Thanh Bi Kip 150" la KHONG CO trong du an, trong
       khi magicscript chi so 3208 co "Dai Thanh Bi Kip cap 150" - khac moi
       chu "cap". Suyt nua chu game them mot mon TRUNG TEN vao bang.
     - Script con go sai dau: "Dai THANH Bi Kip 150" (dau sac) trong khi bang
       ghi "Dai THANH Bi Kip cap 150" (dau huyen). Bo dau moi bat duoc.
    """
    t = _bo_dau(_gon(s))
    for tu in ("cap ", "lv ", "level ", "lv."):
        t = t.replace(tu, "")
    return re.sub(r"\s+", " ", t).strip()


def nap(duong_dan=None):
    """Doc magicscript.txt. Tra ve (theo_chiso, theo_ten, theo_ten_manh, lech).

    theo_chiso   : {chi_so -> (ten, genre, detail)}      <- CACH MAY TRA
    theo_ten     : {ten_chuan -> chi_so}
    theo_ten_manh: {ten_chuan_manh -> [chi_so, ...]}
    lech         : {chi_so} nhung chi so ma cot ParticularType khac chi so
    """
    p = duong_dan or MS_MAC_DINH
    d = io.open(p, "rb").read().decode("latin-1").replace("\r\n", "\n").split("\n")
    theo_chiso = {}
    theo_ten = {}
    theo_ten_manh = {}
    lech = set()
    for k, l in enumerate(d[1:]):        # LoadRecord bo dong tieu de
        c = l.split("\t")
        if len(c) < 4 or not c[3].strip().isdigit():
            continue
        ten = tcvn2uni(c[0]).strip()
        theo_chiso[k] = (ten, c[1].strip(), c[2].strip())
        theo_ten.setdefault(_gon(ten), k)
        theo_ten_manh.setdefault(_gon_manh(ten), []).append(k)
        if int(c[3]) != k:
            lech.add(k)
    return theo_chiso, theo_ten, theo_ten_manh, lech


def tim_theo_ten(ten, theo_ten, theo_ten_manh):
    """Tra chi so tu ten. Tra ve (chi_so, cach_khop) hoac (None, ung_vien).

    cach_khop: "chinh xac" | "gan dung"
    Khi khong khop, tra ve danh sach ung vien gan giong de NGUOI xem, thay vi
    ket luan "khong co" (luat: cong cu nan ma phai BAO LOI, khong duoc im lang).
    """
    k = theo_ten.get(_gon(ten))
    if k is not None:
        return k, "chinh xac"
    ds = theo_ten_manh.get(_gon_manh(ten))
    if ds and len(ds) == 1:
        return ds[0], "gan dung"
    return None, (ds or [])


def bao_cao_lech(lech, theo_chiso):
    """In canh bao ve vung lech (goi o dau moi cong cu nan ma)."""
    if not lech:
        print("  bang vat pham: cot ParticularType KHOP chi so tren toan bang.")
        return 0
    print("  !! CANH BAO: %d dong co cot ParticularType KHAC chi so dong."
          % len(lech))
    print("     May tra theo CHI SO (KItemGenerator.CPP:1660), nen o vung nay")
    print("     tra theo cot la SAI. Chi so nho nhat bi lech: %d" % min(lech))
    return len(lech)
