# -*- coding: utf-8 -*-
r"""b5c_nan_ma_script.py  --  NAN MA VAT PHAM genre 6 cua cay script Long Mon Tieu Cuc.

Doi ma vat pham tu so cua ban LINUX sang so cua JX1 trong
    D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\lua54\

BANG ANH XA lap bang QUY TRINH HAI BUOC (tu lap, KHONG lay tu viec magicscript):
  B1  tra TEN chuan o bang Linux  D:\ServerLinux\server1\settings\item\004\magicscript.txt
      theo GIA TRI (ItemGenre=6, DetailType=1, ParticularType=N).  Bang Linux co cot PT
      that, PT KHONG theo dong (PT max 30594 tren 4995 dong) nen bat buoc tra theo gia tri.
  B2  tra chinh TEN do o bang JX1   ...\bin\server\settings\item\magicscript.txt
      theo DONG, luat  PT = dong - 2  (da kiem: 4978 dong du lieu, cot PT khop luat, lech 0).
  Khi mot ten trung nhieu dong JX1 thi loc them bang TEN TEP SPR; van con trung thi giai
  bang do lech dong cua ca khoi (Linux dong + 13 = JX1 dong) va doi chieu cach dung that
  trong cay script JX1 dang chay.  Chi tiet tung ma kho: xem GHICHU o duoi.

BON+HAI DANG DUOC XU LY
  (a) bang    tbProp = {6, 1, N, ...}
  (b) chuoi   "6,1,N"
  (c) khoa    ["6,1,N"]
  (e) doi so  CalcItemCount(-1, 6, 1, N, -1) / ConsumeItem(..., 6, 1, N, -1)
      -> bon dang tren deu khop MOT bieu thuc  6 , 1 , N  (RE_GDP)
  (d) so sanh / so hoc voi bien ma  nP == N, nP > N, nP - N     -> LIET KE TAY (DS_TAY)
  (f) khoa bang tra cuu bang ma     [4473] = {...}              -> LIET KE TAY (DS_TAY)

Dang (d)/(f) KHONG duoc thay mu: moi cho ghi ro tep:dong + doan van ban cho doi.
Neu mot cho trong DS_TAY khong con dung nhu mo ta -> BAO LOI, khong ghi gi ca.

AN TOAN
  * chi doc/ghi bang bytes (khong dung Edit/Write tool len tep TCVN3)
  * sao luu <tep>.truoc_nanma
  * idempotent: ghi trang thai (sha256 sau khi nan) vao nanma_trangthai.json;
    chay lai -> bo qua tep da nan.  BAT BUOC vi mot so ma DICH lai trung ma NGUON
    (vd 3570 vua la ma Linux vua la ma JX1) -> chay hai lan khong guard se hong.
  * assert: day byte >= 0x80 cua tep TRUOC va SAU phai GIONG HET (chi doi chu so ASCII)
  * ma khong tra duoc ten -> BAO LOI, thoat 2 (khong im lang)

CACH CHAY
  set PYTHONIOENCODING=utf-8
  python b5c_nan_ma_script.py --thu        # chi in, khong ghi
  python b5c_nan_ma_script.py --lam        # ghi that
  python b5c_nan_ma_script.py --lam --lai  # khoi phuc .truoc_nanma roi nan lai
"""
import os, re, sys, json, hashlib, shutil, argparse

ROOT = r"D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\lua54"
TRANGTHAI = os.path.join(os.path.dirname(os.path.abspath(__file__)), "nanma_trangthai.json")

# ---------------------------------------------------------------- bang anh xa
# ma Linux -> ma JX1  (ParticularType, genre 6 detail 1)
MAP = {
    127: 126, 906: 907, 907: 908, 2126: 2135, 2269: 2278, 2271: 2280,
    2390: 2399, 2391: 2400, 2527: 2536, 2952: 3570, 2953: 3571,
    3150: 3768, 3154: 3772, 3155: 3773, 3156: 3774, 3189: 3807, 3190: 3808,
    3203: 4844, 3308: 4752,
    3455: 4876, 3456: 4877, 3457: 4878, 3458: 4879,
    3554: 4172, 3559: 4177, 3560: 4178, 3570: 4188, 3580: 4198, 3590: 4208,
    3600: 4218, 3610: 4228, 3620: 4238, 3630: 4248, 3640: 4258, 3650: 4268,
    3660: 4278, 3670: 4288, 3680: 4298, 3690: 4308, 3700: 4318, 3710: 4328,
    3720: 4338, 3730: 4348, 3740: 4358, 3750: 4368, 3760: 4378, 3770: 4388,
    3780: 4398, 3790: 4408, 3800: 4418, 3810: 4428,
    4200: 4771, 4201: 4772, 4202: 4773, 4203: 4774,
    4204: 4775, 4205: 4776, 4206: 4777, 4207: 4778,
    4550: 4885,
    30228: 4846, 30229: 4847, 30289: 4848, 30301: 4857, 30438: 4864,
    30538: 4866, 30557: 4813,
    # [KIEMCHUNG 06/09] 10 mon MOI, bang vat pham JX1 da cap 4978..4987
    # (b5b_bang_item.py noi vao cuoi magicscript.txt, PT == so dong - 2)
    4473: 4978, 4474: 4979, 4475: 4980, 4502: 4981, 4535: 4982,
    4536: 4983, 4537: 4984, 4538: 4985, 4539: 4986, 4604: 4987,
}

# ma CHUA co trong bang JX1 -> KHONG doi; phai them dong vao magicscript.txt truoc.
# [KIEMCHUNG 06/09] TRONG: ca 10 ma da duoc cap id 4978..4987 va chuyen len MAP.
MO = {
}

# vi sao chon so nay khi co nhieu ung vien / khi ten khong khop thang
GHICHU = {
    906: "Linux 'Qua Huy Hoang (cao) ' trung 2 dong JX1 (PT 907 va 3440). Chon 907: cung spr "
         "huihuangzhiguo.spr, lien khoi voi 907->908, va script JX1 dang chay dung "
         "AddItem(6,1,907,...) --qua cao (bin/server/script/global/vatpham.lua:125)",
    3156: "Linux 'Huyen Kim Khoang Thach (Cuc lon)' -> JX1 PT 3774 con de TEN HAN (chua dich, "
          "nghia 'Huyen Kim khoang thach (dac dai)'), cung spr wuse_bingjin.spr, lien khoi "
          "3154->3772 / 3155->3773",
    3189: "JX1 PT 3807 con de TEN HAN (nghia 'Huy Hoang tran diep'), cung spr taskobj396.spr, "
          "do lech dong khoi +13 khop",
    3190: "JX1 PT 3808 con de TEN HAN (nghia 'Huy Hoang chi hoa'), cung spr jinlianhua.spr",
    3203: "[KIEMCHUNG 06/09] 'Ho Mach Don' co HAI dong JX1 (3821, 4844). Chon 4844: he kinh "
          "mach SONG chi chap nhan 4844 -- script/player/setmeridian.lua:21 KM_ITEM_HMD = 4844, "
          "KM_DemItem (dong 191) va DelItem (dong 216) deu dung hang so do. 3821 trung sprite "
          "voi ban Linux nhung Script = 0 va KHONG he thong nao tieu thu -> phat ra vien vo dung.",
    3455: "[KIEMCHUNG 06/09] 'Trai tao' trung 2 dong (4073, 4876). Chon 4876: "
          "script/petsys/feed.lua:3-6 dat ITEM_APPLE=4876, ITEM_SUGARCANE=4877, ITEM_MAIZE=4878, "
          "ITEM_SWEET_POTATO=4879 va tbFood tra theo dung 4 ma do. 4073..4076 cung sprite nhung "
          "Script = 0 -> ban dong hanh KHONG an duoc, khong bao loi.",
    3456: "nhu 3455",
    3457: "nhu 3455",
    3458: "nhu 3455",
    30228: "spr khac (Linux chanlongdon.spr vs JX1 kinhmach\\channguyendon.spr) nhung ten khop "
           "duy nhat, va script JX1 dang chay dung 6,1,4846 'Chan Nguyen Don (trung)'",
    3308: "[KIEMCHUNG 06/09] 'Chan Nguyen Dan' trung 2 dong (3926, 4752). Chon 4752: "
          "script/item/event/kinhmach/channguyendan.lua:10-16 dang ky KM_CND_GIATRI[4752]=10, "
          "[4846]=5, [4847]=10 -- dong bo voi 30228->4846 va 30229->4847. 3926 Script = 0.",
    30229: "'Chan Nguyen Don (dai)' trung 2 dong (2991, 4847). Chon 4847 vi cung KHOI KINH MACH "
           "voi 30228->4846, va script JX1 dang chay dung 6,1,4847",
    30289: "'Huyet Long Dang' trung 3 dong (3051, 3946, 4848). Chon 4848 cho DONG BO voi khoi "
           "kinh mach 4845..4849 (30228->4846, 30229->4847). CA 3051 LAN 4848 deu dang duoc "
           "dung that trong script JX1 (3051 o chuangguanbaoxiang.lua, 4848 o xinshibaoxiang.lua "
           "voi dung cac cap 9/11/12 nhu ban Linux) -> hai lua chon deu chay duoc; neu chu muon "
           "3051 thi doi mot dong trong MAP.",
    30557: "'Tui Duoc Pham' -> JX1 4813 'Tui duoc pham'. Spr khac (Linux qiutiandalibao.spr vs "
           "JX1 obj_item_jixiang.spr) nhung ten khop duy nhat va 4813 dang duoc dung that "
           "(bin/server/script/global/vatpham.lua:188)",
}

# ------------------------------------------------- dang (d) va (f): liet ke TAY
# (duong dan tuong doi, so dong, doan CU, doan MOI hoac None neu CO Y giu, ly do)
DS_TAY = [
    # --- extend.lua : so sanh tran voi ma Ruong Tieu Vat Bang (4475) --------- MO
    (r"script\activitysys\config\129\extend.lua", 1412, b"nP ~= 4475", b"nP ~= 4980",
     "dang (d) so sanh tran; 4475 Ruong Tieu Vat Bang -> JX1 4980 (id moi)"),
    (r"script\activitysys\config\129\extend.lua", 1428, b"nP == 4475", b"nP == 4980",
     "dang (d) so sanh tran; 4475 Ruong Tieu Vat Bang -> JX1 4980 (id moi)"),
    # --- item_addproperty.lua : DAI LIEN TUC 4204..4207 --------------------------
    (r"script\activitysys\config\129\item_addproperty.lua", 15,
     b"(nP>4207 or nP<4204)", b"(nP>4778 or nP<4775)",
     "dang (d) KIEM DAI: 4204..4207 -> 4775..4778 VAN LIEN TUC va van dung 4 mon "
     "(JX1 4775 Khoai Ma Gia Tien, 4776 Kien Bat Kha Toa, 4777 Thiet Xa Mat Bo, "
     "4778 Tieu Xa Di Vi) -> phep kiem dai giu nguyen y nghia"),
    (r"script\activitysys\config\129\item_addproperty.lua", 32,
     b"nP - 4204 + TSK_ItemKMJBTime", b"nP - 4775 + TSK_ItemKMJBTime",
     "dang (d) SO HOC: lay chi so 0..3 trong dai; moc dai doi 4204 -> 4775, do lech giu nguyen"),
    (r"script\activitysys\config\129\item_addproperty.lua", 51,
     b"(nP == 4204)", b"(nP == 4775)", "dang (d) so sanh tran"),
    (r"script\activitysys\config\129\item_addproperty.lua", 57,
     b"(nP == 4205)", b"(nP == 4776)", "dang (d) so sanh tran"),
    (r"script\activitysys\config\129\item_addproperty.lua", 65,
     b"(nP == 4206)", b"(nP == 4777)",
     "dang (d) so sanh tran - dong nay DANG BI CHU THICH (--), van doi cho dong bo"),
    (r"script\activitysys\config\129\item_addproperty.lua", 71,
     b"(nP == 4207)", b"(nP == 4778)", "dang (d) so sanh tran"),
    # --- item.lua : so sanh tran ---------------------------------------------
    (r"script\event\longmenbiaoju\item.lua", 126, b"(nP == 4204)", b"(nP == 4775)",
     "dang (d) so sanh tran"),
    (r"script\event\longmenbiaoju\item.lua", 129, b"(nP == 4205)", b"(nP == 4776)",
     "dang (d) so sanh tran"),
    (r"script\event\longmenbiaoju\item.lua", 134, b"(nP == 4207)", b"(nP == 4778)",
     "dang (d) so sanh tran"),
    # --- award.lua : DANG (g) genre 6 DETAIL 0 -- RE_GDP (6,1,N) KHONG BAT DUOC ---
    # Linux (6,0,2389) = "Truong Hieu Ngoc Lo Hoan" (\spr\item\medecine\obj-potion14.spr,
    # bang Linux dong 2401).  JX1 PT 2389 = "Thiep that sac le hoa" -> MON KHAC.
    # Ten + spr khop DUY NHAT o JX1 PT 2398 "Truong Hieu Ngoc Lo Hoan" (cung
    # obj-potion14.spr); ca khoi khop: Linux 2387/2388/2389 = JX1 2396/2397/2398
    # (Cuu Menh / Cuu Cong / Ngoc Lo).
    # GIU nguyen DetailType 0: KItemGenerator::Gen_MagicScript (KItemGenerator.cpp:1734)
    # tra bang bang GetMagicScript(nParticularType) - KHONG dung nDetailType - roi
    # *pItem = *pMagicScript nen mon sinh ra mang detail 1 cua bang.
    (r"script\event\longmenbiaoju\award.lua", 32,
     b"{6,0,2389,1,0,0}", b"{6,0,2398,1,0,0}",
     "dang (g) detail 0: 2389 -> 2398 Truong Hieu Ngoc Lo Hoan"),
    (r"script\event\longmenbiaoju\award.lua", 50,
     b"{6,0,2389,1,0,0}", b"{6,0,2398,1,0,0}",
     "dang (g) detail 0: 2389 -> 2398 Truong Hieu Ngoc Lo Hoan"),
    # --- item_followaward.lua : KHOA BANG bang ma vat pham ----------------- MO
    # tbAwardList[nP] va tbMinCells[nP] tra theo CHINH ma vat pham (main() dong 51-52,
    # 61): khong doi khoa thi ruong 4978/4979 tra nil -> bam KHONG RA GI, khong bao loi.
    (r"script\activitysys\config\129\item_followaward.lua", 10, b"[4473] = {", b"[4978] = {",
     "dang (f) khoa bang tra thuong; Thuong Tan Trung 4473 -> JX1 4978"),
    (r"script\activitysys\config\129\item_followaward.lua", 29, b"[4474] = {", b"[4979] = {",
     "dang (f) khoa bang tra thuong; Thuong Trung Thanh 4474 -> JX1 4979"),
    (r"script\activitysys\config\129\item_followaward.lua", 57, b"[4473] = 5,", b"[4978] = 5,",
     "dang (f) khoa bang so o tui; 4473 -> 4978"),
    (r"script\activitysys\config\129\item_followaward.lua", 58, b"[4474] = 5,", b"[4979] = 5,",
     "dang (f) khoa bang so o tui; 4474 -> 4979"),
]

# 6 , 1 , N   (bat dang a, b, c, e).  Chan truoc bang chu/so/dau cham de khong dinh
# vao toa do kieu  {37, 1690, 3150}  hay  {176,1448,3335}
RE_GDP = re.compile(rb"(?<![\w.])6(\s*,\s*)1(\s*,\s*)(\d+)")

# Canh gac dang (g): cum genre 6 co DETAIL KHAC 1 -- RE_GDP o tren khong bat duoc.
# Moi cho phai nam trong BACH_DANH_G6 (da doi chieu tay), neu khong thi BAO LOI.
RE_G6ANY = re.compile(rb"(?<![\w.])6(\s*,\s*)(\d+)(\s*,\s*)(\d+)")
BACH_DANH_G6 = {
    # (detail, particular)
    # Linux (6,0,20) "Hoa hong" == JX1 PT 20 "Hoa hong" -> KHONG phai doi
    (0, 20),
    # da nan o DS_TAY dang (g)
    (0, 2398),
    # yunbiao_system.lua:45 la DONG CHU THICH liet ke cap xe tieu ( ... 5, 6, 6, 6, 7 ...),
    # khong phai ma vat pham
    (6, 6),
}


def cao(b):
    """day byte >= 0x80 (phan chu Viet TCVN3 / chu Han GBK)"""
    return bytes(x for x in b if x >= 0x80)


def sha(b):
    return hashlib.sha256(b).hexdigest()


def liet_ke():
    out = []
    for dp, dn, fn in os.walk(ROOT):
        for f in sorted(fn):
            if f.lower().endswith(".lua"):
                out.append(os.path.join(dp, f))
    return sorted(out)


def nan_mot_tep(path, bo_sot, mo_thay):
    """tra ve (noi_dung_moi_bytes, [ (dong, cu, moi, dang) ... ])"""
    with open(path, "rb") as fh:
        raw = fh.read()
    rel = os.path.relpath(path, ROOT)
    dong = raw.split(b"\n")
    ghi = []

    # ---- dang a/b/c/e : bieu thuc  6,1,N
    for i, ln in enumerate(dong):
        def thay(m, i=i):
            v = int(m.group(3))
            if v in MAP:
                ghi.append((i + 1, "6,1,%d" % v, "6,1,%d" % MAP[v], "a/b/c/e"))
                return b"6" + m.group(1) + b"1" + m.group(2) + str(MAP[v]).encode("ascii")
            if v in MO:
                mo_thay.append((rel, i + 1, v, "a/b/c/e", MO[v]))
                return m.group(0)
            bo_sot.append((rel, i + 1, v))
            return m.group(0)
        dong[i] = RE_GDP.sub(thay, ln)

    # ---- dang d/f : liet ke tay
    for (frel, lno, cu, moi, ly) in DS_TAY:
        if frel != rel:
            continue
        if lno - 1 >= len(dong):
            raise SystemExit("LOI: %s khong co dong %d" % (rel, lno))
        ln = dong[lno - 1]
        if cu not in ln:
            raise SystemExit("LOI: %s:%d khong con chua doan mong doi %r\n     dong that: %r"
                             % (rel, lno, cu, ln[:160]))
        if moi is None:
            v = int(re.search(rb"\d+", cu).group(0))
            mo_thay.append((rel, lno, v, "d/f", MO.get(v, ly)))
            continue
        if ln.count(cu) != 1:
            raise SystemExit("LOI: %s:%d doan %r xuat hien %d lan" % (rel, lno, cu, ln.count(cu)))
        dong[lno - 1] = ln.replace(cu, moi)
        ghi.append((lno, cu.decode("latin-1"), moi.decode("latin-1"), "d/f"))

    # ---- canh gac dang (g): genre 6 detail != 1
    for i, ln in enumerate(dong):
        for m in RE_G6ANY.finditer(ln):
            d, v = int(m.group(2)), int(m.group(4))
            if d == 1 or (d, v) in BACH_DANH_G6:
                continue
            raise SystemExit("LOI: %s:%d co cum 6,%d,%d (detail KHAC 1) chua duoc doi "
                             "chieu tay.\n     dong that: %r" % (rel, i + 1, d, v, ln[:160]))

    return b"\n".join(dong), ghi


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--thu", action="store_true", help="chi in, khong ghi")
    ap.add_argument("--lam", action="store_true", help="ghi that")
    ap.add_argument("--lai", action="store_true", help="khoi phuc .truoc_nanma roi nan lai")
    a = ap.parse_args()
    if not (a.thu or a.lam):
        ap.error("phai chon --thu hoac --lam")
    # [KIEMCHUNG 06/09] --lai KHOI PHUC tep that ngay ca khi --thu (dong duoi lam
    # shutil.copyfile vo dieu kien) -> "--lai --thu" se bo cay script ve trang thai
    # CHUA NAN roi khong nan lai. Cam ket hop do.
    if a.lai and not a.lam:
        ap.error("--lai phai di kem --lam (--lai khoi phuc tep that, khong phai chay thu)")

    tt = {}
    if os.path.exists(TRANGTHAI):
        tt = json.load(open(TRANGTHAI, encoding="utf-8"))

    # [KIEMCHUNG 06/09] Cong tac hash o duoi bo qua tep "da nan roi". Khi MAP / MO /
    # DS_TAY DOI (vd them 10 ma moi 4978..4987) thi cac tep do bi bo qua LANG LE va
    # ma moi KHONG duoc nan. Ghi dau van tay cua bang anh xa vao tep trang thai:
    # bang doi -> bat buoc chay lai voi --lai (khoi phuc .truoc_nanma roi nan lai),
    # vi nan de len tep DA NAN la sai (907 va 3570 vua la dich vua la nguon).
    van_tay = sha(repr((sorted(MAP.items()), sorted(MO), DS_TAY)).encode("utf-8"))
    van_tay_cu = tt.get("_bang_anh_xa")
    # tt rong ma khong co dau van tay = tep trang thai kieu CU (truoc 06/09) -> cung phai chay lai
    if tt and van_tay_cu != van_tay and not a.lai:
        raise SystemExit(
            "DUNG: bang anh xa DA DOI so voi lan nan truoc (%s -> %s).\n"
            "      Cong tac hash se bo qua cac tep da nan -> ma moi khong duoc ap.\n"
            "      Chay lai voi --lai (khoi phuc .truoc_nanma roi nan lai tu dau):\n"
            "        python b5c_nan_ma_script.py --lai --lam"
            % ((van_tay_cu or "kieu-cu-khong-co")[:16], van_tay[:16]))
    tt["_bang_anh_xa"] = van_tay

    doi = 0
    bo_sot, mo_thay, bao = [], [], []
    tep_da, tep_bo = 0, 0

    for path in liet_ke():
        rel = os.path.relpath(path, ROOT)
        bak = path + ".truoc_nanma"

        if a.lai and os.path.exists(bak):
            shutil.copyfile(bak, path)
            tt.pop(rel, None)

        cur = open(path, "rb").read()
        if not a.lai and rel in tt and tt[rel].get("sau") == sha(cur):
            tep_bo += 1
            continue

        moi, ghi = nan_mot_tep(path, bo_sot, mo_thay)
        if not ghi:
            continue

        # ---- assert: phan byte cao KHONG DOI
        if cao(cur) != cao(moi):
            raise SystemExit("LOI NANG: %s doi phan byte >= 0x80 (chu Viet/Han) -> HUY" % rel)

        for lno, cu, mm, dang in ghi:
            bao.append("  %s:%d  [%s]  %s -> %s" % (rel, lno, dang, cu, mm))
        doi += len(ghi)
        tep_da += 1

        if a.lam:
            if not os.path.exists(bak):
                shutil.copyfile(path, bak)
            with open(path, "wb") as fh:
                fh.write(moi)
            tt[rel] = {"truoc": sha(cur), "sau": sha(moi), "so_cho": len(ghi)}

    print("=" * 78)
    print("DA DOI %d cho trong %d tep  (bo qua %d tep da nan tu truoc)" % (doi, tep_da, tep_bo))
    print("=" * 78)
    for d in bao:
        print(d)

    print("")
    print("=" * 78)
    print("CON MO (ma CHUA co ben JX1, CO Y GIU NGUYEN): %d cho" % len(mo_thay))
    print("=" * 78)
    for rel, lno, v, dang, ly in mo_thay:
        print("  %s:%d  [%s]  %d  -- %s" % (rel, lno, dang, v, ly))

    if bo_sot:
        print("")
        print("!" * 78)
        print("BO SOT %d cho: co dang 6,1,N nhung ma KHONG tra duoc ten" % len(bo_sot))
        print("!" * 78)
        for rel, lno, v in bo_sot:
            print("  %s:%d  ma %d  -- KHONG TRA DUOC TEN, PHAI XU LY TAY" % (rel, lno, v))

    if a.lam:
        json.dump(tt, open(TRANGTHAI, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
        print("")
        print("Da ghi trang thai: " + TRANGTHAI)
    else:
        print("")
        print("(--thu: KHONG ghi tep nao)")

    if bo_sot:
        sys.exit(2)


main()
