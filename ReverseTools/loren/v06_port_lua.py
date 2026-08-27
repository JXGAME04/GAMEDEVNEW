# -*- coding: utf-8 -*-
r"""v06 - PORT CAC KICH BAN LUA CON LAI TU BAN LINUX.

Bon tep loi (item_header, itemvalue_header, compound_header, equip_compound,
atlas) da viet tay de chu thich tieng Viet cho ky. Cac tep con lai phan lon la
CONG THUC TOAN - chep tay de sai tung dau cong, nen port bang cong cu:

  1. Doc tep Linux theo BYTE (latin-1). Ma la ASCII, chuoi hien cho nguoi choi
     la TCVN3, chu thich la GBK tieng Trung.
  2. Dong nao la CHU THICH co byte tieng Trung thi:
        - co ban dich trong DICH_CHU_THICH -> thay bang cau tieng Viet
        - khong co -> BO dong do (chu thich, bo di khong doi hanh vi)
     Chu thich ASCII thuan va moi dong MA deu giu nguyen tung byte.
  3. Ap bang doi ma vat pham + vai sua chua bat buoc cho JX1 (bang DOI_MA).
  4. Ghi ra lua_utf8\... duoi dang UTF-8 de v05 chuyen sang TCVN3 va kiem
     vong tron.

Chay:  python v06_port_lua.py           -> bao cao
       python v06_port_lua.py --ghi     -> ghi ra lua_utf8
"""
import io
import os
import re
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location(
    "bangtxt", os.path.join(HERE, "..", "viemde", "bangtxt.py"))
bangtxt = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(bangtxt)

LNX = r"D:\ServerLinux\server1\script\item"
RA = os.path.join(HERE, "lua_utf8", "item")

CR = chr(13)
NL = chr(10)

# Bon tep da viet tay - khong port de len
DA_VIET_TAY = {
    "item_header.lua",
    "itemvalue\\itemvalue_header.lua",
    "compound\\compound_header.lua",
    "compound\\equip_compound.lua",
    "compound\\atlas.lua",
}

# ---------------------------------------------------------------------------
# Doi ma vat pham + sua chua bat buoc cho JX1.
# Moi muc: (ten tep hoac None = moi tep, chuoi tim, chuoi thay, giai thich)
# ---------------------------------------------------------------------------
DOI_MA = [
    # --- khoang thuoc tinh: Linux 200..205 -> JX1 199..204 (lech deu -1) ---
    ("compound\\equip_enchase.lua",
     "nParticular >= 200 and nParticular <= 205",
     "nParticular >= 199 and nParticular <= 204",
     "dai khoang thuoc tinh cua JX1 la 199..204"),
    ("compound\\equip_enchase.lua",
     "nOreMagLvlPos = nParticular - 199;",
     "nOreMagLvlPos = nParticular - 198;",
     "khoang 199 la o thu 1 (ban Linux: 200 la o thu 1)"),
    # --- magic_distill: dai NGUYEN KHOANG Linux 149..154 -> JX1 148..153 ---
    #     Neu khong nan: 6/1/154 cua JX1 la "Tong Kim Chieu thu" (ve vao map
    #     324) se lot vao dai va bi lo ren AN, con san pham ra 6/1/205 =
    #     "Than bi do chi". Dung cai bay "trung so khac nghia".
    ("compound\\magic_distill.lua",
     "149 <= nParticular and nParticular <= 154",
     "148 <= nParticular and nParticular <= 153",
     "nguyen khoang cua JX1 la 148..153; 154 la Tong Kim Chieu thu"),
    ("compound\\magic_distill.lua",
     "g_nDistillMagicPos = nParticular - 148;",
     "g_nDistillMagicPos = nParticular - 147;",
     "nguyen khoang dau tien cua JX1 la 148"),
    ("compound\\magic_distill.lua",
     "6, 1, 199 + g_nDistillMagicPos,",
     "6, 1, 198 + g_nDistillMagicPos,",
     "khoang thuoc tinh dau tien cua JX1 la 199"),
    # --- ore_upgrade: phep thu chan/le bi DAO sau khi doi ma -1 ---
    #     Ban goc thu tren dai 200..205: le (201/203/205) = o 2/4/6 = o CHAN.
    #     Doi chung doc lap: equip_enchase.lua thu bang VI TRI - mod(pos,2)==0.
    #     Dai cua JX1 la 199..204 nen phai doi sang chan (200/202/204).
    ("compound\\ore_upgrade.lua",
     "if( mod( nParticular, 2 ) ~= 0 and g_nOreSeries ~= nSeries ) then",
     "if( mod( nParticular, 2 ) == 0 and g_nOreSeries ~= nSeries ) then",
     "dai ma doi -1 nen phep thu chan/le phai dao lai"),
    # --- xoa nguyen lieu phai xoa CA CHONG ---
    #     ITEM_CalcItemValue nhan gia tri theo so luong chong, nen chi tru 1
    #     don vi la nguoi choi tra gia bang 1 vien ma huong gia tri ca chong.
    ("compound\\ore_upgrade.lua",
     "RemoveItemByIndex( arynNecessaryItemIdx[1] );",
     "RemoveItemByIndex( arynNecessaryItemIdx[1], GetItemStackCount( arynNecessaryItemIdx[1] ) );",
     "xoa ca chong cho khop cach tinh gia tri"),
    ("compound\\fantasygold_upgrade.lua",
     "RemoveItemByIndex(arynNecessaryItemIdx[i]);",
     "RemoveItemByIndex(arynNecessaryItemIdx[i], GetItemStackCount(arynNecessaryItemIdx[i]));",
     "xoa ca chong cho khop cach tinh gia tri"),
    ("compound\\fantasygoldessence_upgrade.lua",
     "RemoveItemByIndex(arynNecessaryItemIdx[i]);",
     "RemoveItemByIndex(arynNecessaryItemIdx[i], GetItemStackCount(arynNecessaryItemIdx[i]));",
     "xoa ca chong cho khop cach tinh gia tri"),
    # --- CHOT LOC LOAI VAT PHAM cho hai kich ban nang cap Huyen Kim ---
    #     Ban goc chi kiem "3 mon giong nhau, cap < 10" va KHONG kiem do co
    #     phai Huyen Kim khong - vi ben Linux CUA SO LO REN da loc san: o so 7
    #     chi nhan Huyen Kim. Ban port dua nguyen lieu qua HOP GIAO VAT PHAM,
    #     ma hop giao nhan bat cu thu gi, con may chu thi chua bao gio co bo
    #     loc. Bo cua so client ma khong them loc may chu la mo toang cua:
    #     ba trang bi giong het nhau cung cho ra mot mon cap cao hon, va
    #     Compound() cua hai kich ban nay KHONG quay so - thanh cong 100%.
    ("compound\\fantasygold_upgrade.lua",
     "\tif( nLevel >= 10 ) then\n",
     "\t-- [JX1] chot loc: chi Huyen Kim 6/1/3769 moi vao duoc duong nay.\n"
     "\t-- Ban goc khong can vi cua so lo ren cua client da loc san o so 7.\n"
     "\tif( nGenre ~= 6 or nDetailType ~= 1 or nParticular ~= 3769 ) then\n"
     "\t\treturn RESULT_RULE_ERROR;\n"
     "\tend\n"
     "\tif( nLevel >= 10 ) then\n",
     "chi Huyen Kim moi nang cap duoc"),
    ("compound\\fantasygoldessence_upgrade.lua",
     "\tif( nLevel >= 4 ) then\n",
     "\t-- [JX1] chot loc: chi Huyen Kim Chi Tinh 6/1/3770 moi vao duoc.\n"
     "\t-- Ban goc khong can vi cua so lo ren cua client da loc san o so 8.\n"
     "\tif( nGenre ~= 6 or nDetailType ~= 1 or nParticular ~= 3770 ) then\n"
     "\t\treturn RESULT_RULE_ERROR;\n"
     "\tend\n"
     "\tif( nLevel >= 4 ) then\n",
     "chi Huyen Kim Chi Tinh moi nang cap duoc"),
    # --- Huyen Tinh Khoang Thach: Linux 6/1/147 -> JX1 6/1/146 ---
    (None,
     "nParticular == 147 ",
     "nParticular == 146 ",
     "Huyen Tinh Khoang Thach cua JX1 la 6/1/146 (147 la Le vat Hoa Hong)"),
    (None,
     "XUANJING = { 6, 1, 147 }",
     "XUANJING = { 6, 1, 146 }",
     "Huyen Tinh Khoang Thach"),
    (None,
     "{ 6, 1, 147 }",
     "{ 6, 1, 146 }",
     "Huyen Tinh Khoang Thach"),
    # xuanjing_compound.lua dung mo ta mon dich dang thang:
    #   { ver, 0, 0, 6, 1, 147, i, 0, 0, nil, nil, ... }
    ("compound\\xuanjing_compound.lua",
     "0, 6, 1, 147, i,",
     "0, 6, 1, 146, i,",
     "Huyen Tinh Khoang Thach o mo ta mon dich"),
    # --- Huyen Kim: Linux 6/1/3151, 3152 -> JX1 6/1/3769, 3770 ---
    (None, "6, 1, 3151", "6, 1, 3769", "Huyen Kim"),
    (None, "6, 1, 3152", "6, 1, 3770", "Huyen Kim Chi Tinh"),
    (None, "6,1,3151", "6,1,3769", "Huyen Kim"),
    (None, "6,1,3152", "6,1,3770", "Huyen Kim Chi Tinh"),
]

# Cac khoi khoang thuoc tinh viet dang bang { {6,1,200}, ... }
DOI_MA_KHOANG = [("6, 1, %d" % (200 + i), "6, 1, %d" % (199 + i)) for i in range(6)]
DOI_MA_KHOANG += [("6,1,%d" % (200 + i), "6,1,%d" % (199 + i)) for i in range(6)]

# ---------------------------------------------------------------------------
# Ban dich cac dong chu thich tieng Trung hay gap.
# Khoa = chuoi tieng Trung doc theo GBK (da bo dau "--" va khoang trang).
# ---------------------------------------------------------------------------
DICH_CHU_THICH = {
    "功能：紫色装备系统 - 矿石的价值量计算":
        "Chức năng: hệ trang bị tím - tính giá trị của khoáng thạch.",
    "功能：紫色装备系统 - 蓝/白装备的价值量计算":
        "Chức năng: hệ trang bị tím - tính giá trị của trang bị trắng và xanh.",
    "功能：紫色装备系统 - 黄金装备的价值量计算":
        "Chức năng: hệ trang bị tím - tính giá trị của trang bị Hoàng Kim.",
    "功能：紫色装备系统 - 紫色装备的价值量计算":
        "Chức năng: hệ trang bị tím - tính giá trị của trang bị tím.",
    "功能：紫色装备系统 - 魔法属性的价值量计算":
        "Chức năng: hệ trang bị tím - tính giá trị của thuộc tính phép.",
    "功能：紫色装备系统 - 使用魔法属性矿石镶嵌到紫色装备":
        "Chức năng: hệ trang bị tím - khảm khoáng thuộc tính vào trang bị tím.",
    "功能：紫色装备系统 - 使用蓝/白装备和矿石打造空孔的紫色装备":
        "Chức năng: hệ trang bị tím - đúc trang bị tím có ô trống.",
    "功能：紫色装备系统 - 使用黄金图谱和原料合成黄金装备":
        "Chức năng: hệ trang bị tím - ghép Đồ phổ Hoàng Kim thành trang bị.",
    "功能：被程序访问的接口函数，计算指定物品信息的价值量":
        "Chức năng: cửa vào mà chương trình gọi, tính giá trị của một mô tả vật phẩm.",
    "参数：nItemVer 物品版本号":
        "Tham số: nItemVer      phiên bản vật phẩm",
    "nQuality 物品品质":
        "         nQuality      phẩm chất (0 thường, 1 Hoàng Kim, 2 khảm nạm được)",
    "nGenre, nDetailType, nParticular 物品类别":
        "         nGenre/nDetailType/nParticular   mã loại vật phẩm",
    "nLevel 物品级别":
        "         nLevel        cấp của vật phẩm",
    "nSeries 物品五行":
        "         nSeries       ngũ hành",
    "nLuck 物品生成参数的幸运值":
        "         nLuck         chỉ số may mắn trong tham số sinh",
    "arynMagLvl 物品MagicLevel数组":
        "         arynMagLvl    mảng MagicLevel của vật phẩm",
    "aryMagic 物品MagicID数组":
        "         aryMagic      mảng MagicID của vật phẩm",
    "strParam [合成]操作参数":
        "         strParam      tham số của thao tác lò rèn",
    "返回：价值量结果（错误返回0）":
        "Trả về : giá trị tính được (lỗi thì trả 0).",
    "计算空孔的价值量":
        "Tính giá trị của các ô khảm nạm còn trống.",
    "计算装备级别的价值量加权%":
        "Tính hệ số giá trị theo cấp trang bị.",
    "计算装备类型的价值量加权%":
        "Tính hệ số giá trị theo loại trang bị.",
    "内功系伤害":
        "sát thương hệ nội công",
    "初始化数值":
        "đặt lại dữ liệu",
    "初始化数据":
        "đặt lại dữ liệu",
    "获取[合成]操作参数（用于传递给ITEM_CalcItemValue计算物品价值量）":
        "Tham số truyền cho ITEM_CalcItemValue.",
    "校验原料是否符合[合成]规则（同时存储一些原料数据）":
        "Kiểm nguyên liệu có đúng luật, đồng thời cất lại vài số liệu.",
    "生成目标物品信息":
        "Dựng danh sách món đích.",
    "完成选择目标物品、删除原料等收尾操作":
        "Chọn món đích, xoá nguyên liệu, kết thúc.",
    "预览[合成]的成功率":
        "Xem trước tỉ lệ thành công.",
    "[合成]费用":
        "Phí một lần lò rèn",
}


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def tach_chu_thich(dong):
    """Tach mot dong Lua thanh (phan ma, phan chu thich).

    Chi coi '--' la mo chu thich khi no nam NGOAI dau nhay - neu khong thi
    chuoi "Tho ren -- ..." se bi cat mat.
    """
    trong = None
    i = 0
    n = len(dong)
    while i < n:
        c = dong[i]
        if trong is not None:
            if c == "\\":
                i += 2
                continue
            if c == trong:
                trong = None
        elif c == '"' or c == "'":
            trong = c
        elif c == "-" and i + 1 < n and dong[i + 1] == "-":
            return dong[:i], dong[i:]
        i += 1
    return dong, ""


def co_byte_trung(s):
    """co byte >= 0x80 ma KHONG phai chu Viet TCVN3 doc duoc"""
    for c in s:
        if ord(c) >= 0x80:
            return True
    return False


def gbk_cua(s_latin1):
    try:
        return s_latin1.encode("latin-1").decode("gbk")
    except Exception:
        return None


def dich_dong(dong):
    """tra ve dong tieng Viet, hoac None neu nen bo dong"""
    gbk = gbk_cua(dong)
    if gbk is None:
        return None
    noi_dung = gbk.lstrip()
    if noi_dung.startswith("--"):
        noi_dung = noi_dung[2:].strip()
    # thu khop nguyen van, roi khop tung manh
    for khoa, viet in DICH_CHU_THICH.items():
        if khoa == noi_dung:
            dau = dong[:len(dong) - len(dong.lstrip())]
            return dau + "-- " + viet
    for khoa, viet in DICH_CHU_THICH.items():
        if khoa in noi_dung:
            dau = dong[:len(dong) - len(dong.lstrip())]
            return dau + "-- " + viet
    return None


def port(rel):
    p = os.path.join(LNX, rel)
    t = doc(p).replace(CR + NL, NL)
    # BAY DA VAP: khong duoc goi tcvn2uni len CA VAN BAN sau khi da chen chu
    # thich tieng Viet (Unicode) - moi ky tu Viet co ma >= 0x80 se bi bang
    # TCVN3 anh xa lai lan nua, "Phi mot lan lo ren" thanh "Pho mot lan lu ron".
    # Vi vay: doi TCVN3 -> Unicode NGAY TAI DONG GOC, con dong tieng Viet ta
    # tu chen thi giu nguyen.
    ra = []
    bo = 0
    dich = 0
    for dong in t.split(NL):
        ma, ct = tach_chu_thich(dong)
        if ct and co_byte_trung(ct):
            v = dich_dong(ct)
            if v is not None:
                dich += 1
                v = v.lstrip()      # da la Unicode, KHONG qua tcvn2uni
                if ma.strip() == "":
                    ra.append(bangtxt.tcvn2uni(ma) + v)
                else:
                    ra.append(bangtxt.tcvn2uni(ma.rstrip()) + "\t" + v)
                continue
            bo += 1
            if ma.strip() == "":
                continue            # ca dong la chu thich tieng Trung -> bo han
            ra.append(bangtxt.tcvn2uni(ma.rstrip()))
            continue
        ra.append(bangtxt.tcvn2uni(dong))
    t2 = NL.join(ra)

    # ap bang doi ma
    doi = []
    for ten, tim, thay, vi_sao in DOI_MA:
        if ten is not None and ten != rel:
            continue
        n = t2.count(tim)
        if n:
            t2 = t2.replace(tim, thay)
            doi.append((tim, thay, n, vi_sao))
    for tim, thay in DOI_MA_KHOANG:
        n = t2.count(tim)
        if n:
            t2 = t2.replace(tim, thay)
            doi.append((tim, thay, n, "khoang thuoc tinh -1"))

    return t2, bo, dich, doi


def main():
    ghi = "--ghi" in sys.argv
    # CHI hai thu muc cua he lo ren. Thu muc script\item cua ban Linux con
    # hang tram kich ban vat pham khong lien quan - quet ca cay la se ap bang
    # doi ma khoang len nhung tep chang dinh dang gi toi lo ren.
    rels = []
    for con in ("compound", "itemvalue"):
        d = os.path.join(LNX, con)
        if not os.path.isdir(d):
            continue
        for t in sorted(os.listdir(d)):
            if not t.lower().endswith(".lua"):
                continue
            rel = os.path.join(con, t)
            if rel in DA_VIET_TAY:
                continue
            rels.append(rel)

    print("PORT KICH BAN LUA CON LAI TU BAN LINUX")
    print("=" * 90)
    print("bo qua (da viet tay): %s" % ", ".join(sorted(DA_VIET_TAY)))
    print()

    for rel in rels:
        t2, bo, dich, doi = port(rel)
        print("  %-42s  dich %2d chu thich, bo %3d, doi ma %d cho"
              % (rel, dich, bo, len(doi)))
        for tim, thay, n, vi_sao in doi:
            print("        %-28s -> %-28s x%-3d %s" % (tim, thay, n, vi_sao))
        if ghi:
            p = os.path.join(RA, rel)
            d = os.path.dirname(p)
            if not os.path.isdir(d):
                os.makedirs(d)
            # Chuoi tieng Viet trong tep goc la byte TCVN3; doc bang latin-1
            # nen phai giai ma sang Unicode truoc khi ghi UTF-8.
            io.open(p, "wb").write(t2.encode("utf-8"))

    print()
    print("=" * 90)
    if ghi:
        print("  >> da ghi %d tep vao %s" % (len(rels), RA))
    else:
        print("  [bao cao - them --ghi de xuat tep]")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
