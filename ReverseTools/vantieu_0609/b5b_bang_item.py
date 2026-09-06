# -*- coding: utf-8 -*-
"""b5b_bang_item.py -- BO VA BANG VAT PHAM cho port Long Mon Tieu Cuc (van tieu).

Lam gi:
  1. Quet 40 tep Lua 5.4 cua van tieu, lay DAY DU ma genre 6 (4 dang:
     {6,d,N} trong bang / chuoi "6,d,N" / khoa ["6,d,N"] / so sanh tran nP == N).
  2. Voi tung ma, tra HAI BUOC:
       B1  Linux magicscript.txt  <- THEO GIA TRI (Genre, DetailType, ParticularType)
           => lay TEN CHUAN cot 1 (KHONG dung nhan szName viet trong script).
       B2  JX1 magicscript.txt    <- THEO TEN, roi lay ParticularType TU SO DONG
           (bat bien: ParticularType == so dong 1-based - 2, dung cho ca 4978 dong).
     Bat khop: khong phan biet hoa thuong (ke ca cap TCVN3 0xA1..0xA7 <-> 0xA8..0xAE),
     gop khoang trang. KHONG tra duoc / tra ra nhieu ket qua ma khong co quyet dinh
     ghi ro trong QUYET_DINH  =>  BAO LOI VA DUNG.
  3. Sinh ba loai thay doi len magicscript.txt cua JX1:
       (a) DUNG LAI  - khong dong toi tep.
       (b) DOI TEN TAI CHO - dong DA CO dung vat pham do nhung ten con la GBK chua dich.
           Chi ghi lai o TEN, KHONG chen/xoa dong => ParticularType khong xe dich.
       (c) THEM MOI  - NOI VAO CUOI tep, ParticularType lien tuc tu 4978.
  4. Ghi CUNG MOT noi dung cho ca server lan client.

Bo cuc cot (tu doc tu dong tieu de cua CA HAI tep, sai la dung):
  Linux 30 cot: 1 Name 2 ItemGenre 3 DetailType 4 ParticularType 5 ImageName 6 ObjIdx
                7 Width 8 Height 9 Intro 10 NguHanh 11 Price 12 Level 13 CoXepChong
                14 Script 15 SkillID 16..19 Param2-5 20 Target 21 MaxStack 22..30 YeuCau*
  JX1   30 cot: 1 Name 2 Genre 3 DetailType 4 ParticularType 5 ImageName 6 ObjIdx
                7 Width 8 Height 9 Intro 10 Script 11 Price 12 ShortKey 13 nMaxStack
                14 PickExecute  (KBasPropTbl.CPP:1045 nap 15 cot)
  => hai bang CHI trung nhau tu cot 1 den cot 9. Chep nguyen dong Linux se nhet
     duong dan script vao o PickExecute va co xep chong vao o nMaxStack: mon hien
     trong tui, bam khong chay, khong xep chong, KHONG CO LOI NAO BAO.

Chay:
  set PYTHONIOENCODING=utf-8
  python b5b_bang_item.py                 # chay thu, ghi ra scratchpad, khong dung cay that
  python b5b_bang_item.py --that          # ghi that vao bin\\server + bin\\client
  python b5b_bang_item.py --ra <thumuc>   # doi noi ghi thu
"""
import os, re, sys, json, shutil, importlib.util

# ----------------------------------------------------------------- duong dan
# Ma vat pham KHONG doi qua buoc chuyen Lua 4 -> 5.4, nen bo va doc cay GOC (chi doc).
# Cay lua54 dang bi buoc "nan ma" ghi de tai cho (co tep *.lua.truoc_nanma), doc no se
# lay nham ma JX1 da nan roi ma tuong la ma Linux.
ROOT_LUA = r"D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\goc_lua4"
ROOT_LUA54 = r"D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\lua54"
F_LINUX  = r"D:\ServerLinux\server1\settings\item\004\magicscript.txt"
F_SRV    = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt"
F_CLI    = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\settings\item\magicscript.txt"
VN2OCT   = r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts\vn_to_octal.py"
PT_BAT_DAU = 4978          # id moi bat dau (max hien tai 4977 = "Chien Lenh Hao Hoa")

# --------------------------------------------------- vn_to_octal (bat buoc)
_spec = importlib.util.spec_from_file_location("vn_to_octal", VN2OCT)
_vn = importlib.util.module_from_spec(_spec); _spec.loader.exec_module(_vn)
tcvn3 = _vn.unicode_to_tcvn3_bytes           # chu Viet -> byte TCVN3, KHONG go tay


def V(s):
    """Chu Viet unicode -> chuoi latin-1 mang byte TCVN3 (de ghep vao dong tep)."""
    return tcvn3(s).decode("latin-1")


# --------------------------------------------------------------- QUYET DINH
# Cac ma ma B2 tra ra NHIEU dong trung ten. Moi muc phai ghi ro dong JX1 duoc chon
# va BANG CHUNG. Bo va TU KIEM: PT duoc chon phai nam trong danh sach ung vien.
QUYET_DINH = {
    # ma Linux "genre,detail,particular" : (ParticularType JX1, ly do)
    "6,0,20":    (20,   "dong 22: spr flower.spr + obj 330 + intro + gia 10000 GIONG HET Linux; "
                        "ung vien PT 3939 co intro GBK, obj 41, gia 0 = vat pham khac"),
    "6,1,906":   (907,  "dong 909: spr huihuangzhiguo.spr + obj 377 + script huihuangzhiguo3.lua "
                        "+ MaxStack 0 GIONG HET Linux; PT 3440 khong co script, MaxStack 50"),
    # LUAT: khi nhieu dong trung ten, chon dong ma HE THONG JX1 DANG CHAY THAT tieu thu,
    # KHONG chon dong chi vi sprite trung voi Linux (dong "giong Linux" thuong la ban CN
    # cu da chet, da bi thay bang dong kinh mach tieng Viet).
    "6,1,3203":  (4844, "he kinh mach song CHI chap nhan 4844: script/player/setmeridian.lua:21 "
                        "KM_ITEM_HMD = 4844, KM_DemItem/DelItem deu dung hang so do. PT 3821 co "
                        "spr giong Linux nhung KHONG he thong nao tieu thu (cac bang rot con phat "
                        "3821 la loi CO SAN cua ban live)"),
    "6,1,3308":  (4752, "script/item/event/kinhmach/channguyendan.lua dang ky KM_CND_GIATRI[4752]=10, "
                        "[4846]=5, [4847]=10 -> 4752 la vien Chan Nguyen he kinh mach dang chay; "
                        "PT 3926 chi con 1 bang rot cu dung, khong co script"),
    "6,1,3455":  (4876, "dong 4878: spr VA script petsys/feed.lua deu GIONG Linux; "
                        "PT 4073 cung spr nhung Script=0 (khong an duoc)"),
    "6,1,3456":  (4877, "dong 4879: spr VA script petsys/feed.lua GIONG Linux; PT 4074 Script=0"),
    "6,1,3457":  (4878, "dong 4880: spr VA script petsys/feed.lua GIONG Linux; PT 4075 Script=0"),
    "6,1,3458":  (4879, "dong 4881: spr VA script petsys/feed.lua GIONG Linux; PT 4076 Script=0"),
    "6,1,30229": (4847, "dong 4849 'Chan Nguyen Don (Dai)' bo kinh mach, MaxStack 500, script "
                        "kinhmach/channguyendan.lua - cung bo voi 30228 -> 4846 (Trung) va "
                        "4845 (Tieu); PT 2991 la ban cu spr questkey/taskobj056.spr"),
    "6,1,30289": (4848, "he kinh mach song CHI chap nhan 4848: script/player/setmeridian.lua:25 "
                        "KM_ITEM_HLDANG = 4848. PT 3051/3946 co spr giong Linux nhung khong he "
                        "thong nao tieu thu"),
}

# Dong DA CO dung vat pham nhung ten con la GBK chua dich -> chi doi o TEN tai cho.
# Bang chung: spr + ObjIdx + Intro trung khop 100% voi dong Linux.
DOI_TEN = {
    # ma Linux : (ParticularType JX1 dang co, ten Viet moi, bang chung)
    "6,1,3156": (3774, "Huy\u1ec1n Kim Kho\u00e1ng Th\u1ea1ch (C\u1ef1c l\u1edbn)",
                 "dong 3776 spr vnxmas2007/wuse_bingjin.spr + obj 333 + intro Viet GIONG Linux, "
                 "ten con GBK; nam ngay sau tieu/trung/dai (3772/3773)"),
    "6,1,3189": (3807, "Huy Ho\u00e0ng Tr\u00e2n Di\u1ec7p",
                 "dong 3809 spr questkey/taskobj396.spr + obj 41 + INTRO GBK GIONG HET Linux"),
    "6,1,3190": (3808, "Huy Ho\u00e0ng Hoa",
                 "dong 3810 spr yulanpenjie/jinlianhua.spr + obj 443 + INTRO GBK GIONG HET Linux"),
}

# Sprite Linux ma KHONG dong JX1 nao tham chieu -> gan nhu chac chan thieu trong pak client.
# Phai thay bang cap (sprite, ObjIdx) lay tu MOT dong JX1 dang song.
THAY_SPRITE = {
    "6,1,4536": ("\\spr\\item\\script\\duankaidejian.spr", "41",
                 "goc questkey/taskobj125.spr khong co dong JX1 nao dung; "
                 "muon icon cua 'Tinh Thiet Toa' PT 3785"),
    "6,1,4538": ("\\spr\\item\\questkey\\taskobj025.spr", "333",
                 "goc questkey/nanmingzhijing.spr khong co o JX1; "
                 "muon icon cua 'Tinh Thiet Khoang' PT 4428"),
    "6,1,4539": ("\\spr\\item\\medecine\\obj-potion15.spr", "19",
                 "goc questkey/taskobj127.spr khong co o JX1; muon icon vien thuoc "
                 "cua 'Truong Menh hoan' PT 1 (ObjIdx 19 trung dung voi Linux)"),
}

# Dong JX1 DUNG LAI nhung o Script dang la 0, trong khi ban port van tieu CO cung cap
# tep script cho no. Khong dat lai o nay thi mon nam trong tui, bam KHONG CHAY, KHONG
# CO LOI NAO BAO. Chi dat cho script ma ban port that su mang sang.
DAT_SCRIPT = {
    # ma Linux : (PT JX1, duong dan script se dat, ly do)
    "6,1,4204": (4775, "\\script\\event\\longmenbiaoju\\item.lua",
                 "'Khoai Ma Gia Tien' - buff xe tieu; Linux 004 dong 4764 cot 14 tro vao "
                 "item.lua, JX1 dong 4777 dang de 0"),
    "6,1,4205": (4776, "\\script\\event\\longmenbiaoju\\item.lua",
                 "'Kien Bat Kha Toa' - buff xe tieu; JX1 dang de Script 0"),
    "6,1,4206": (4777, "\\script\\event\\longmenbiaoju\\item.lua",
                 "'Thiet Xa Mat Bo' - buff xe tieu; JX1 dang de Script 0"),
    "6,1,4207": (4778, "\\script\\event\\longmenbiaoju\\item.lua",
                 "'Tieu Xa Di Vi' - buff xe tieu; JX1 dang de Script 0"),
}

# Ten trung nhung SPRITE khac Linux. Mac dinh bo va coi la CON MO (bao cho chu).
# Muc nao co o day = da co bang chung khac chung minh cung mot vat pham.
SPRITE_KHAC_DA_XAC_NHAN = {
    "6,1,30228": "script/item/event/kinhmach/channguyendan.lua dang ky KM_CND_GIATRI[4846]=5 "
                 "dung bang gia tri vien 'trung' cua Linux -> cung vat pham, JX1 chi ve lai icon "
                 "(kinhmach/channguyendon.spr thay vng/.../chanlongdon.spr)",
}

# Script Linux tro vao cay KHONG duoc port -> phai de 0, neu khong bam se ScriptError.
BO_SCRIPT = {
    "6,1,4475": "Linux dung script/item/noscript.lua; JX1 KHONG co tep do, JX1 ghi 0",
    "6,1,4502": "Linux dung script/activitysys/config/50/item_love.lua; config 50 khong port",
    "6,1,4539": "Linux dung script/activitysys/config/31/item_dilingdan.lua; config 31 khong port",
    "6,1,4604": "Linux dung script/missions/tong_guta/...; man Co Thap khong port",
}

# Diem CON MO ma bo va khong tu quyet duoc -- chu phai chot.
GHI_CHU_MO = [
    "6,1,4539: script/event/longmenbiaoju/item_yaobao.lua co bang tbItem voi khoa "
    "\"6,1,4539\" = 'Ngu hoa ngoc lo hoan dac biet', TRONG KHI bang Linux 004 dong 4868 goi "
    "6,1,4539 la 'Dia Linh Don' va gan script config/31/item_dilingdan.lua. Bang 004 la bang "
    "DUY NHAT co that (000-003 chi 3-18 dong), nen item_yaobao.lua gan nhu chac chan la tep "
    "thua tu goi khac, khong duoc bang vat pham tro toi. Bo va van cap id cho 'Dia Linh Don' "
    "(theo bang Linux), nhung neu chu muon dung Duoc Bao thi phai chot lai.",
    "6,1,4539 / 6,1,4502 / 6,1,4604: script goc nam ngoai pham vi port (config 31, config 50, "
    "man Co Thap) nen o Script de 0 -- mon SE HIEN va NHAN DUOC nhung bam KHONG CHAY. Neu day "
    "la phan thuong that su can dung thi phai port them script tuong ung.",
]

# Ten Viet cho cac mon them moi (sinh bang vn_to_octal, KHONG go tay byte).
TEN_MOI = {
    "6,1,4473": "Th\u01b0\u1edfng T\u1eadn Trung",
    "6,1,4474": "Th\u01b0\u1edfng Trung Th\u00e0nh",
    "6,1,4475": "R\u01b0\u01a1ng Ti\u00eau V\u1eadt Bang",
    "6,1,4502": "Th\u1ec1 Non H\u1eb9n Bi\u1ec3n",
    "6,1,4535": "M\u1ea3nh Tinh Tinh Kho\u00e1ng",
    "6,1,4536": "M\u1ea3nh Tinh Thi\u1ebft Kho\u00e1ng",
    "6,1,4537": "Tinh Tinh Kho\u00e1ng kh\u00f4ng nguy\u00ean ch\u1ea5t",
    "6,1,4538": "Tinh Thi\u1ebft Kho\u00e1ng kh\u00f4ng nguy\u00ean ch\u1ea5t",
    "6,1,4539": "\u0110\u1ecba Linh \u0110\u01a1n",
    "6,1,4604": "L\u1ec7nh b\u00e0i C\u1ed5 Th\u00e1p - B\u1ea1c",
}


# ------------------------------------------------------------------ tien ich
class Loi(Exception):
    pass


def doc_bang(p):
    """Doc magicscript.txt: latin-1, giu nguyen byte. Tra danh sach dong da tach tab."""
    d = open(p, "rb").read().decode("latin-1")
    if "\r\n" not in d:
        raise Loi("%s: khong phai CRLF, bo va nay chi lam viec voi CRLF" % p)
    ls = d.split("\r\n")
    while ls and ls[-1] == "":
        ls.pop()
    return [l.split("\t") for l in ls]


# TCVN3: 0xA1..0xA7 = A-breve A-circ E-circ O-circ O-horn U-horn D-stroke (HOA)
#        0xA8..0xAE = cac chu thuong tuong ung.  ASCII ha bang .lower().
_FOLD = {chr(0xA1 + i): chr(0xA8 + i) for i in range(7)}


def chuan(s):
    s = re.sub(r"\s+", " ", s.replace("\xa0", " ")).strip().lower()
    return "".join(_FOLD.get(c, c) for c in s)


def kiem_bo_cuc(lx, jx):
    """Tu assert bo cuc cot bang chinh dong tieu de + du lieu cua ca hai tep."""
    mong_jx = ["Name", "Genre", "DetailType", "ParticularType", "ImageName", "ObjIdx",
               "Width", "Height", "Intro", "Script", "Price", "ShortKey", "nMaxStack",
               "PickExecute"]
    if jx[0][:14] != mong_jx:
        raise Loi("JX1 doi bo cuc cot! tieu de = %r" % (jx[0][:14],))
    if not (lx[0][1] == "ItemGenre" and lx[0][2] == "DetailType" and lx[0][3] == "ParticularType"):
        raise Loi("Linux doi bo cuc cot! tieu de = %r" % (lx[0][:5],))
    if len(lx[0]) != 30 or len(jx[0]) != 30:
        raise Loi("so cot tieu de doi: Linux %d, JX1 %d" % (len(lx[0]), len(jx[0])))
    # tieu de Linux la GBK khong doc duoc -> kiem cot 14 = Script, cot 21 = MaxStack
    # bang chinh DU LIEU.
    co_script = sum(1 for r in lx[1:] if len(r) > 13 and r[13].lower().endswith(".lua"))
    if co_script < 500:
        raise Loi("Linux cot 14 khong phai Script (chi %d dong co duoi .lua)" % co_script)
    co_ms = sum(1 for r in lx[1:] if len(r) > 20 and r[20].strip().isdigit())
    if co_ms < len(lx) - 50:
        raise Loi("Linux cot 21 khong phai MaxStack (chi %d dong la so)" % co_ms)
    co_sc_jx = sum(1 for r in jx[1:] if r[9].lower().endswith(".lua"))
    if co_sc_jx < 500:
        raise Loi("JX1 cot 10 khong phai Script (chi %d dong co duoi .lua)" % co_sc_jx)
    co_ms_jx = sum(1 for r in jx[1:] if r[12].strip().isdigit())
    if co_ms_jx < len(jx) - 50:
        raise Loi("JX1 cot 13 khong phai nMaxStack (chi %d dong la so)" % co_ms_jx)


def kiem_bat_bien_dong(jx):
    """JX1 tra bang THEO DONG: ParticularType phai == so dong 1-based - 2."""
    xau = [i + 1 for i in range(1, len(jx)) if jx[i][3].strip() != str(i - 1)]
    if xau:
        raise Loi("JX1 lech bat bien 'PT == so dong - 2' o %d dong, vd %r" % (len(xau), xau[:5]))
    return len(jx) - 1                      # so dong du lieu


# ---------------------------------------------------------- quet ma tu script
RX = [(re.compile(r'\[\s*"\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*"\s*\]'), "khoa"),
      (re.compile(r'"\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*"'),           "chuoi"),
      (re.compile(r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)"),              "bang")]
RX_NP = re.compile(r"\bnP\b\s*[=~]=\s*(\d+)|(\d+)\s*[=~]=\s*\bnP\b")


def quet_ma():
    """Tra {"6,d,p": {"dang": [...], "tep": [...]}} cho moi ma genre 6 trong cay lua54."""
    h, so_tep = {}, 0
    for dp, dn, fn in os.walk(ROOT_LUA):
        for f in sorted(fn):
            if not f.lower().endswith(".lua"):
                continue
            so_tep += 1
            path = os.path.join(dp, f)
            rel = os.path.relpath(path, ROOT_LUA).replace("\\", "/")
            for line in open(path, "rb").read().decode("latin-1").split("\n"):
                ma = line.split("--")[0]            # bo phan ghi chu
                for rx, tag in RX:
                    for m in rx.finditer(ma):
                        if m.group(1) != "6":
                            continue
                        k = "6,%s,%s" % (m.group(2), m.group(3))
                        e = h.setdefault(k, {"dang": [], "tep": []})
                        if tag not in e["dang"]:
                            e["dang"].append(tag)
                        if rel not in e["tep"]:
                            e["tep"].append(rel)
                for m in RX_NP.finditer(ma):        # so sanh tran nP == N (mac dinh detail 1)
                    k = "6,1,%s" % (m.group(1) or m.group(2))
                    e = h.setdefault(k, {"dang": [], "tep": []})
                    if "nP_sosanh" not in e["dang"]:
                        e["dang"].append("nP_sosanh")
                    if rel not in e["tep"]:
                        e["tep"].append(rel)
    if so_tep == 0:
        raise Loi("khong thay tep .lua nao duoi %s" % ROOT_LUA)
    return h, so_tep


# --------------------------------------------------------------------- chinh
def chay(ghi_that, thu_muc_ra):
    lx = doc_bang(F_LINUX)
    jx = doc_bang(F_SRV)
    if open(F_SRV, "rb").read() != open(F_CLI, "rb").read():
        raise Loi("bin server va bin client magicscript.txt KHAC NHAU -- phai hop nhat truoc")
    kiem_bo_cuc(lx, jx)
    so_dong = kiem_bat_bien_dong(jx)
    pt_max = int(jx[-1][3])
    if pt_max + 1 != PT_BAT_DAU:
        raise Loi("PT max hien tai = %d, bo va cho PT_BAT_DAU = %d" % (pt_max, PT_BAT_DAU))
    print("bo cuc cot OK | JX1 %d dong du lieu, PT max %d | Linux %d dong"
          % (so_dong, pt_max, len(lx) - 1))

    lx_idx, jx_ten, jx_spr = {}, {}, set()
    for i in range(1, len(lx)):
        lx_idx["%s,%s,%s" % (lx[i][1].strip(), lx[i][2].strip(), lx[i][3].strip())] = i
    for i in range(1, len(jx)):
        jx_ten.setdefault(chuan(jx[i][0]), []).append(i)
        jx_spr.add(jx[i][4].strip().lower())

    ma, so_tep = quet_ma()
    print("quet %d tep .lua -> %d ma genre 6 duy nhat" % (so_tep, len(ma)))

    # dong JX1 da bi DOI_TEN chiem: khong duoc coi la ung vien trung ten nua
    doiten_pt = set(v[0] for v in DOI_TEN.values())

    dung_lai, doi_ten, cap_moi, mo, script_khac = [], [], [], [], []
    for k in sorted(ma, key=lambda x: (int(x.split(",")[1]), int(x.split(",")[2]))):
        e = ma[k]
        rec = {"linux_ma": k, "dang": e["dang"], "tep": e["tep"]}
        # ---- B1: Linux, THEO GIA TRI
        if k not in lx_idx:
            raise Loi("B1 that bai: ma %s khong co trong %s" % (k, F_LINUX))
        r = lx[lx_idx[k]]
        rec.update(linux_dong=lx_idx[k] + 1, ten_chuan=r[0], linux_spr=r[4], linux_obj=r[5],
                   linux_w=r[6], linux_h=r[7], linux_intro=r[8], linux_price=r[10],
                   linux_script=(r[13] if len(r) > 13 else "0"),
                   linux_maxstack=(r[20] if len(r) > 20 else "0"))
        # ---- doi ten tai cho ?
        if k in DOI_TEN:
            pt, ten, bc = DOI_TEN[k]
            d = pt + 2
            if jx[d - 1][3] != str(pt):
                raise Loi("DOI_TEN %s: dong %d khong phai PT %d" % (k, d, pt))
            if jx[d - 1][4].strip().lower() != r[4].strip().lower():
                raise Loi("DOI_TEN %s: sprite JX1 (%s) khac Linux (%s)" % (k, jx[d - 1][4], r[4]))
            rec.update(trang_thai="DOI_TEN_TAI_CHO", jx1_pt=pt, jx1_dong=d,
                       jx1_ma="6,1,%d" % pt, ten_moi=ten, ten_cu=jx[d - 1][0], bang_chung=bc)
            doi_ten.append(rec)
            continue
        # ---- B2: JX1, THEO TEN -> lay PT tu SO DONG
        ung = [i for i in jx_ten.get(chuan(r[0]), []) if int(jx[i][3]) not in doiten_pt]
        if len(ung) == 1:
            i = ung[0]
            if jx[i][3].strip() != str(i - 1):
                raise Loi("B2 %s: dong %d co PT %s, lech bat bien" % (k, i + 1, jx[i][3]))
            rec.update(trang_thai="DUNG_LAI", jx1_dong=i + 1, jx1_pt=int(jx[i][3]),
                       jx1_ma="6,%s,%s" % (jx[i][2], jx[i][3]), jx1_spr=jx[i][4],
                       jx1_script=jx[i][9], jx1_maxstack=jx[i][12],
                       spr_khop=(jx[i][4].strip().lower() == r[4].strip().lower()))
            if not rec["spr_khop"]:
                if k in SPRITE_KHAC_DA_XAC_NHAN:
                    rec["spr_khac_ly_do"] = SPRITE_KHAC_DA_XAC_NHAN[k]
                else:
                    rec["canh_bao"] = ("TEN trung nhung SPRITE khac Linux -- can chu xac nhan la "
                                       "cung mot vat pham (doi hoa) hay la vat pham khac")
                    mo.append(rec)
            dung_lai.append(rec)
            continue
        if len(ung) > 1:
            if k not in QUYET_DINH:
                raise Loi("B2 %s ('%s') tra ra %d dong JX1 trung ten (%s) ma khong co muc trong "
                          "QUYET_DINH -- DUNG. Phai ghi ro chon dong nao va vi sao."
                          % (k, r[0], len(ung), [i + 1 for i in ung]))
            pt, ly_do = QUYET_DINH[k]
            chon = [i for i in ung if int(jx[i][3]) == pt]
            if len(chon) != 1:
                raise Loi("QUYET_DINH %s chon PT %d nhung PT do khong nam trong danh sach ung "
                          "vien %s" % (k, pt, [int(jx[i][3]) for i in ung]))
            i = chon[0]
            rec.update(trang_thai="DUNG_LAI", jx1_dong=i + 1, jx1_pt=pt, jx1_ma="6,1,%d" % pt,
                       jx1_spr=jx[i][4], jx1_script=jx[i][9], jx1_maxstack=jx[i][12],
                       ung_vien=[int(jx[x][3]) for x in ung], ly_do=ly_do,
                       spr_khop=(jx[i][4].strip().lower() == r[4].strip().lower()))
            dung_lai.append(rec)
            continue
        # ---- khong co: them moi
        if k not in TEN_MOI:
            raise Loi("B2 %s ('%s') khong tra duoc ten nao o JX1 va cung khong co ten Viet "
                      "trong TEN_MOI -- DUNG." % (k, r[0]))
        rec["trang_thai"] = "THEM_MOI"
        cap_moi.append(rec)

    # -------------------------------- o Script cua dong DUNG LAI khac voi Linux
    # (chi bao cao; chi SUA nhung muc ghi ro trong DAT_SCRIPT)
    for rec in dung_lai:
        a = rec.get("linux_script", "0").strip().lower()
        b = rec.get("jx1_script", "0").strip().lower()
        if a in ("", "0") or a.endswith("noscript.lua"):
            a = "0"
        if b == "":
            b = "0"
        if a != b:
            script_khac.append({"linux_ma": rec["linux_ma"], "jx1_ma": rec["jx1_ma"],
                                "linux_script": rec.get("linux_script"),
                                "jx1_script": rec.get("jx1_script")})
    dat_script = []
    for k, (pt, sc, ly_do) in DAT_SCRIPT.items():
        r = [x for x in dung_lai if x["linux_ma"] == k]
        if len(r) != 1:
            raise Loi("DAT_SCRIPT %s: ma nay khong nam trong nhom DUNG_LAI" % k)
        if r[0]["jx1_pt"] != pt:
            raise Loi("DAT_SCRIPT %s: bo va chon PT %d chu khong phai %d"
                      % (k, r[0]["jx1_pt"], pt))
        if r[0].get("jx1_script", "0").strip() not in ("", "0"):
            raise Loi("DAT_SCRIPT %s: dong JX1 DA CO script %r -- khong duoc de len"
                      % (k, r[0]["jx1_script"]))
        r[0]["dat_script"] = sc
        r[0]["dat_script_ly_do"] = ly_do
        dat_script.append((pt, sc))

    # ------------------------------------------------ dung cac dong them moi
    cap_moi.sort(key=lambda x: int(x["linux_ma"].split(",")[2]))
    pt = PT_BAT_DAU
    dong_moi = []
    for rec in cap_moi:
        k = rec["linux_ma"]
        spr, obj = rec["linux_spr"], rec["linux_obj"]
        if k in THAY_SPRITE:
            spr, obj, bc = THAY_SPRITE[k]
            rec["thay_sprite"] = bc
        if spr.strip().lower() not in jx_spr:
            raise Loi("them moi %s: sprite %s KHONG co dong JX1 nao dung -> nhieu kha nang thieu "
                      "trong pak client. Them muc vao THAY_SPRITE roi chay lai." % (k, spr))
        script = rec["linux_script"]
        if k in BO_SCRIPT or script.strip() in ("", "0") or script.lower().endswith("noscript.lua"):
            rec["bo_script"] = BO_SCRIPT.get(k, "Linux de trong / noscript")
            script = "0"
        ten = V(TEN_MOI[k])                      # sinh tu vn_to_octal, khong go tay byte
        # ANH XA COT DUNG -- KHONG chep nguyen dong Linux
        o = [""] * 30
        o[0] = ten                     # Name
        o[1] = "6"                     # Genre
        o[2] = "1"                     # DetailType  (JX1 luon 1)
        o[3] = str(pt)                 # ParticularType == so dong - 2
        o[4] = spr                     # ImageName
        o[5] = obj                     # ObjIdx
        o[6] = rec["linux_w"]          # Width
        o[7] = rec["linux_h"]          # Height
        o[8] = rec["linux_intro"]      # Intro
        o[9] = script                  # Script      (Linux cot 14 -> JX1 cot 10)
        o[10] = rec["linux_price"] or "0"      # Price
        o[11] = "1"                    # ShortKey
        o[12] = rec["linux_maxstack"] or "0"   # nMaxStack (Linux cot 21 -> JX1 cot 13)
        o[13] = "0"                    # PickExecute
        for c in o:
            if "\t" in c or "\r" in c or "\n" in c:
                raise Loi("them moi %s: o chua tab/xuong dong" % k)
        rec.update(jx1_pt=pt, jx1_ma="6,1,%d" % pt, jx1_dong=pt + 2,
                   ten_moi=TEN_MOI[k], script_dat=script, spr_dat=spr, obj_dat=obj)
        dong_moi.append("\t".join(o))
        pt += 1

    # ------------------------------------------------------- dung tep dau ra
    goc = open(F_SRV, "rb").read().decode("latin-1")
    ls = goc.split("\r\n")
    duoi_trong = 0
    while ls and ls[-1] == "":
        ls.pop()
        duoi_trong += 1
    n_truoc = len(ls)
    # (b) doi ten tai cho -- CHI o cot 1, khong dong den so dong
    for rec in doi_ten:
        i = rec["jx1_dong"] - 1
        c = ls[i].split("\t")
        if c[3] != str(rec["jx1_pt"]):
            raise Loi("doi ten: dong %d khong phai PT %d" % (i + 1, rec["jx1_pt"]))
        c[0] = V(rec["ten_moi"])
        ls[i] = "\t".join(c)
    # (b2) dat o Script cho dong DUNG LAI -- cung chi sua MOT o, khong dong so dong
    for pt, sc in dat_script:
        i = pt + 1                                  # 0-based cua dong (pt + 2) - 1
        c = ls[i].split("\t")
        if c[3] != str(pt):
            raise Loi("dat script: dong %d khong phai PT %d" % (i + 1, pt))
        if c[9].strip() not in ("", "0"):
            raise Loi("dat script: dong %d da co script %r" % (i + 1, c[9]))
        c[9] = sc
        ls[i] = "\t".join(c)
    # (c) noi vao CUOI
    ls.extend(dong_moi)
    ra = "\r\n".join(ls) + "\r\n" * max(duoi_trong, 1)

    # ------------------------------------------------------------- tu kiem
    ls2 = [l for l in ra.split("\r\n") if l != ""]
    if len(ls2) != n_truoc + len(dong_moi):
        raise Loi("so dong sai: truoc %d, sau %d, them %d" % (n_truoc, len(ls2), len(dong_moi)))
    for i in range(1, len(ls2)):
        c = ls2[i].split("\t")
        if c[3].strip() != str(i - 1):
            raise Loi("SAU KHI VA: dong %d co PT %s (phai la %d) -- bang da xe dich!"
                      % (i + 1, c[3], i - 1))
        if c[1].strip() != "6":
            raise Loi("SAU KHI VA: dong %d Genre = %s" % (i + 1, c[1]))
    # moi dong cu (tru cac dong doi ten) phai y nguyen tung byte
    goc_ls = [l for l in goc.split("\r\n") if l != ""]
    doi = set(r["jx1_dong"] for r in doi_ten)
    dsc = set(pt + 2 for pt, _ in dat_script)
    for i in range(len(goc_ls)):
        if (i + 1) in doi:
            a, b = goc_ls[i].split("\t"), ls2[i].split("\t")
            if a[1:] != b[1:]:
                raise Loi("dong doi ten %d bi sua ngoai o TEN" % (i + 1))
        elif (i + 1) in dsc:
            a, b = goc_ls[i].split("\t"), ls2[i].split("\t")
            if a[:9] != b[:9] or a[10:] != b[10:]:
                raise Loi("dong dat script %d bi sua ngoai o Script" % (i + 1))
        elif goc_ls[i] != ls2[i]:
            raise Loi("dong %d bi doi ma khong duoc phep" % (i + 1))
    print("tu kiem OK: %d dong cu giu nguyen tung byte, %d dong doi TEN, %d dong dat o SCRIPT, "
          "%d dong noi them, PT lien tuc 0..%d"
          % (n_truoc - 1 - len(doi_ten) - len(dsc), len(doi_ten), len(dsc), len(dong_moi),
             len(ls2) - 2))

    # ------------------------------------------------------------------ ghi
    if ghi_that:
        dich = [F_SRV, F_CLI]
    else:
        os.makedirs(thu_muc_ra, exist_ok=True)
        dich = [os.path.join(thu_muc_ra, "magicscript.server.txt"),
                os.path.join(thu_muc_ra, "magicscript.client.txt")]
    for p in dich:
        if ghi_that and not os.path.exists(p + ".truoc_vantieu"):
            shutil.copy2(p, p + ".truoc_vantieu")
        with open(p, "wb") as f:                 # latin-1: giu nguyen byte TCVN3/GBK
            f.write(ra.encode("latin-1"))
        print("  ghi %s (%d byte)" % (p, len(ra.encode("latin-1"))))
    if open(dich[0], "rb").read() != open(dich[1], "rb").read():
        raise Loi("server va client ra khac nhau")

    # ------------------------------------------------------------- bang anh xa
    anh_xa = {}
    for rec in dung_lai + doi_ten + cap_moi:
        anh_xa[rec["linux_ma"]] = rec["jx1_ma"]
    bao = {
        "nguon_script": ROOT_LUA, "so_tep_lua": so_tep,
        "bang_linux": F_LINUX, "bang_jx1_server": F_SRV, "bang_jx1_client": F_CLI,
        "quy_tac": "B1 tra Linux THEO GIA TRI (Genre,DetailType,ParticularType) lay ten cot 1; "
                   "B2 tra JX1 THEO TEN roi lay ParticularType TU SO DONG (PT == so dong - 2).",
        "tong_ma": len(ma), "dung_lai": len(dung_lai), "doi_ten_tai_cho": len(doi_ten),
        "dat_o_script": [{"jx1_pt": pt, "script": sc} for pt, sc in dat_script],
        "script_khac_chi_bao_cao": script_khac,
        "them_moi": len(cap_moi),
        "pt_moi": [PT_BAT_DAU, PT_BAT_DAU + len(cap_moi) - 1] if cap_moi else [],
        "con_mo": [r["linux_ma"] for r in mo], "ghi_chu_mo": GHI_CHU_MO,
        "anh_xa": anh_xa, "chi_tiet": dung_lai + doi_ten + cap_moi,
    }
    os.makedirs(thu_muc_ra, exist_ok=True)
    pj = os.path.join(thu_muc_ra, "anh_xa_vatpham.json")
    json.dump(bao, open(pj, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
    pj2 = os.path.join(os.path.dirname(os.path.abspath(__file__)), "anh_xa_vatpham.json")
    json.dump(bao, open(pj2, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
    print("bang anh xa: %s\n             %s" % (pj, pj2))
    print("\nTONG KET: %d ma | dung lai %d | doi ten tai cho %d | cap id moi %d (%d..%d) | con mo %d"
          % (len(ma), len(dung_lai), len(doi_ten), len(cap_moi), PT_BAT_DAU,
             PT_BAT_DAU + len(cap_moi) - 1, len(mo)))
    for r in mo:
        print("  CON MO %s -> %s : %s" % (r["linux_ma"], r["jx1_ma"], r["canh_bao"]))
    for g in GHI_CHU_MO:
        print("  CON MO %s" % g)
    return bao


if __name__ == "__main__":
    that = "--that" in sys.argv
    ra = (r"C:\Users\nguye\AppData\Local\Temp\claude"
          r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
          r"\4ffa7363-6c62-41ba-a49a-e06ebd239dea\scratchpad\vantieu\thu")
    if "--ra" in sys.argv:
        ra = sys.argv[sys.argv.index("--ra") + 1]
    try:
        chay(that, ra)
    except Loi as e:
        print("\n*** DUNG: %s" % e)
        sys.exit(2)
