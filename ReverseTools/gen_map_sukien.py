# -*- coding: utf-8 -*-
"""SINH TU DONG Sources/Core/Src/KMapSuKien.h tu bang map_type.txt DANG CHAY cua may chu.

Vi sao lay tu day: map_type.txt la bang CUA CHINH MAY CHU liet ke moi ban do dac biet
(su kien / chien truong / pho ban) kem loai va mo ta - khong phai bang do nguoi go tay.
Chay lai khi may chu them ban do su kien moi.

Dung de: auto WAuto biet "dang o map su kien" thi DUNG cac auto tu do (Da Tau, di
chuyen theo toa do, ve thanh theo dieu kien) - dung y chu game 25/08/2026:
  "vi du da tau ma dang di tong kim - phong lang do - tin su - cac hoat dong bang hoi -
   vuot ai thi se khong chay auto da tau".

DA DOI CHIEU (gen chay ra 0 trung): khong mot map su kien nao trung diem cay Da Tau
(KDaTauSpots.h) hay map boss Sat Thu (KSatThuBossPos.h).
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

SRC = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\map_type.txt"
OUT = r"D:\GAMEDEVNEW\Sources\Core\Src\KMapSuKien.h"
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"

# ten tieng Viet + co CHAN AUTO TU DO:
#   1 = su kien / pho ban / chien truong -> DUNG Da Tau, dung tu di chuyen, dung ve thanh
#   0 = ban do dac biet nhung VAN la cho cay binh thuong -> khong chan
TEN = {
    "SONGJIN":       (u"T\u1ed1ng Kim (chi\u1ebfn tr\u01b0\u1eddng + \u0111i\u1ec3m b\xe1o danh)", 1),
    "SONGJIN_NONE":  (u"T\u1ed1ng Kim (c\u1ea5m thu\u1ed1c PK)", 1),
    "GUOZHAN":       (u"Qu\u1ed1c Chi\u1ebfn T\u1ed1ng Kim", 1),
    "FENGLING":      (u"Phong L\u0103ng \u0110\u1ed9 (b\xe3i \u0111\xe1nh b\u1ea3o)", 1),
    "FERRY":         (u"Phong L\u0103ng \u0110\u1ed9 (\u0111\xf2)", 1),
    "XINSHI":        (u"T\xedn S\u1ee9", 1),
    "SHILIAN":       (u"S\xe1t Th\u1ee7 Th\xed Luy\u1ec7n (V\u01b0\u1ee3t \u1ea3i)", 1),
    "LEAGUEMATCH":   (u"Li\xean \u0110\u1ea5u", 1),
    "DATUSHA":       (u"Li\xean \u0110\u1ea5u (\u0111\u1ea1i \u0111\u1ed3 s\xe1t)", 1),
    "YANWUCHANG":    (u"Di\u1ec5n V\xf5 Tr\u01b0\u1eddng (l\xf4i \u0111\xe0i nhi\u1ec1u ng\u01b0\u1eddi)", 1),
    "TONGLEITAI":    (u"L\xf4i \u0110\xe0i Bang H\u1ed9i", 1),
    "TONGGONGCHENG": (u"Bang H\u1ed9i C\xf4ng Th\xe0nh Chi\u1ebfn", 1),
    "DIYIBANG":      (u"V\xf5 L\xe2m \u0110\u1ec7 Nh\u1ea5t Bang (Bang Chi\u1ebfn)", 1),
    "TOPMANMATCH":   (u"L\xf4i \u0110\xe0i Tinh Anh Bang H\u1ed9i", 1),
    "BAIRENLEITAI":  (u"B\xe1ch Nh\xe2n L\xf4i \u0110\xe0i", 1),
    "SEVENCITY":     (u"Th\u1ea5t Th\xe0nh \u0110\u1ea1i Chi\u1ebfn", 1),
    "WULINDAHUI":    (u"V\xf5 L\xe2m \u0110\u1ea1i H\u1ed9i", 1),
    "QUFUYONGDI":    (u"V\xf5 L\xe2m \u0110\u1ea1i H\u1ed9i (khu v\u1ef1c)", 1),
    "GUANZHANCHANG": (u"V\xf5 L\xe2m \u0110\u1ea1i H\u1ed9i (kh\xe1n \u0111\xe0i)", 1),
    "DRAGONBOAT":    (u"\u0110ua Thuy\u1ec1n R\u1ed3ng", 1),
    "FENGHUO":       (u"Phong H\u1ecfa Li\xean Th\xe0nh", 1),
    "CHRISTMAS":     (u"S\u1ef1 ki\u1ec7n Gi\xe1ng Sinh", 1),
    "SPRING":        (u"S\u1ef1 ki\u1ec7n N\u0103m M\u1edbi", 1),
    "DUANWUJIE":     (u"S\u1ef1 ki\u1ec7n \u0110oan Ng\u1ecd", 1),
    "YANDIBAOZANG":  (u"Vi\xeam \u0110\u1ebf B\u1ea3o T\xe0ng", 1),
    "JINDI":         (u"C\u1ea5m \u0110\u1ecba Tr\u01b0\u1eddng Ca M\xf4n", 1),
    "TONGBAN":       (u"Nhi\u1ec7m v\u1ee5 \u0110\u1ed3ng B\u1ea1n", 1),
    "FUBEN":         (u"Ph\xf3 b\u1ea3n", 1),
    "JIANLAO":       (u"\u0110\u1ea1i Lao (nh\xe0 lao)", 1),
    "RACEGAME":      (u"\u0110ua ch\u1ea1y", 1),
    # --- duoi day KHONG chan: van la cho cay / lam nghe binh thuong ---
    "TAOHUADAO":     (u"\u0110\xe0o Hoa \u0110\u1ea3o", 0),
    "XISUIDAO":      (u"T\u1ea9y T\u1ee7y \u0110\u1ea3o", 0),
    "MINGYUE":       (u"Minh Nguy\u1ec7t Tr\u1ea5n", 0),
    "CAIYAO":        (u"H\xe1i thu\u1ed1c (ngh\u1ec1)", 0),
}


def doc_bang():
    out = []
    for l in io.open(SRC, encoding="latin-1", newline="").read().split("\n")[2:]:
        c = l.rstrip("\r").split("\t")
        if len(c) < 2 or not c[0].strip():
            continue
        loai = c[0].strip()
        khoang = []
        for it in c[1].strip().split(","):
            it = it.strip()
            if not it:
                continue
            if "|" in it:
                a, b = it.split("|")
                khoang.append((int(a), int(b)))
            else:
                khoang.append((int(it), int(it)))
        out.append((loai, khoang))
    return out


bang = doc_bang()
la = [l for l, _ in bang if l not in TEN]
assert not la, "map_type.txt co loai MOI chua dat ten: %s" % la

# --- doi chieu an toan: khong duoc trung diem cay Da Tau / map boss Sat Thu ---
ids = set()
for loai, kh in bang:
    if TEN[loai][1]:
        for a, b in kh:
            ids.update(range(a, b + 1))
s = io.open(os.path.join(CORE, "KDaTauSpots.h"), encoding="latin-1", newline="").read()
dt = set(int(m) for m in re.findall(r"\{\s*(\d+)\s*,\s*\d+\s*,\s*\d+\s*,\s*\d+\s*\}", s))
s2 = io.open(os.path.join(CORE, "KSatThuBossPos.h"), encoding="latin-1", newline="").read()
st = set(int(x) for x in re.findall(r"\d+", s2.split("s_nST3BossMap[ST3_POS_MAX + 1] = {")[1].split("};")[0]) if int(x) > 0)
assert not (ids & dt), "map su kien trung diem cay Da Tau: %s" % sorted(ids & dt)
assert not (ids & st), "map su kien trung map boss Sat Thu: %s" % sorted(ids & st)
print("doi chieu OK: %d map su kien, 0 trung Da Tau, 0 trung boss Sat Thu" % len(ids))

L = []
L.append("// ============================================================================")
L.append("// KMapSuKien.h - SINH TU DONG tu settings/map_type.txt cua may chu DANG CHAY")
L.append("// boi ReverseTools/gen_map_sukien.py - DUNG SUA TAY.")
L.append("//")
L.append("// Bang nay tra loi mot cau: \"ban do dang dung co phai MAP SU KIEN khong?\".")
L.append("// Auto WAuto dung no de DUNG cac auto TU DO khi nguoi choi dang di su kien")
L.append("// (y chu game 25/08: 'da tau ma dang di tong kim - phong lang do - tin su -")
L.append("// cac hoat dong bang hoi - vuot ai thi se khong chay auto da tau').")
L.append("//")
L.append("// nChan = 1: su kien / chien truong / pho ban -> DUNG Da Tau, dung tu di")
L.append("//             chuyen, dung tu ve thanh (van danh tra + nhat do binh thuong).")
L.append("// nChan = 0: ban do dac biet nhung VAN la cho cay -> khong chan gi.")
L.append("//")
L.append("// Da doi chieu luc sinh: 0 map su kien nao trung diem cay Da Tau")
L.append("// (KDaTauSpots.h) hay map boss Sat Thu (KSatThuBossPos.h).")
L.append("// ============================================================================")
L.append("#ifndef KMAPSUKIEN_H")
L.append("#define KMAPSUKIEN_H")
L.append("")
L.append("struct KMapSKKhoang")
L.append("{")
L.append("\tshort\t\tnTu;\t\t// map dau (bao gom)")
L.append("\tshort\t\tnDen;\t\t// map cuoi (bao gom)")
L.append("\tunsigned char\tnLoai;\t// chi so trong s_aTenSuKien")
L.append("\tunsigned char\tnChan;\t// 1 = dung auto tu do khi dang o day")
L.append("};")
L.append("")
L.append("static const char* const s_aTenSuKien[] = {")
for i, (loai, _) in enumerate(bang):
    L.append("\t\"%s\",\t// %d %s" % (V(TEN[loai][0]), i, loai))
L.append("};")
L.append("")
L.append("static const KMapSKKhoang s_aMapSuKien[] = {")
for i, (loai, kh) in enumerate(bang):
    ten, chan = TEN[loai]
    dong = ", ".join("{ %d, %d, %d, %d }" % (a, b, i, chan) for a, b in kh)
    L.append("\t%s," % dong)
    L.append("\t\t// %s = %s" % (loai, V(ten)))
L.append("};")
L.append("#define KMAPSK_SO\t(int)(sizeof(s_aMapSuKien) / sizeof(s_aMapSuKien[0]))")
L.append("")
L.append("// Tra chi so LOAI su kien cua ban do (dung cho s_aTenSuKien), -1 neu khong phai.")
L.append("// pbChan (co the NULL) nhan 1 khi ban do nay phai DUNG cac auto tu do.")
L.append("inline int KMapSK_Loai(int nMap, int* pbChan)")
L.append("{")
L.append("\tfor (int i = 0; i < KMAPSK_SO; ++i)")
L.append("\t{")
L.append("\t\tif (nMap >= s_aMapSuKien[i].nTu && nMap <= s_aMapSuKien[i].nDen)")
L.append("\t\t{")
L.append("\t\t\tif (pbChan)")
L.append("\t\t\t\t*pbChan = s_aMapSuKien[i].nChan;")
L.append("\t\t\treturn s_aMapSuKien[i].nLoai;")
L.append("\t\t}")
L.append("\t}")
L.append("\tif (pbChan)")
L.append("\t\t*pbChan = 0;")
L.append("\treturn -1;")
L.append("}")
L.append("")
L.append("#endif")
L.append("")
io.open(OUT, "w", encoding="latin-1", newline="\r\n").write("\n".join(L))
print("da sinh %s (%d khoang, %d loai)" % (OUT, sum(len(k) for _, k in bang), len(bang)))
