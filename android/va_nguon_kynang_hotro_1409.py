# -*- coding: latin-1 -*-
# [KNHOTRO 14/09] Chu: "fix lai cac ky nang nao thuoc dang ho tro (khong dung duoc) thi khong cho bo xuong o ky nang".
#
# Cach nhan ra "dung duoc": dung DUNG ba bang ma Core da cho san, y nhu cach [KYNANG 14/09 TRONG] don o chet:
#   GDI_LEFT_ENABLE_SKILLS  (s_KNBang)     - ky nang danh TRAI dang dung duoc
#   GDI_RIGHT_ENABLE_SKILLS (s_KNBangPhai) - ky nang danh PHAI (vong sang / tro)
#   GDI_FIGHT_SKILLS        (s_KNBangDanh) - toan bo ky nang DANH trong bang vo cong (ke ca cap 0)
# Ky nang KHONG nam trong bang nao trong ba bang do = ky nang ho tro / noi cong / khong dung duoc -> khong gan vao o.
# Dung lai ham co san KyNang_CoTrongBang, khong them duong hoi Core moi, khong doi cau truc dung chung.
#
# An toan: chi tu choi khi bang TRAI da doc duoc (s_nKNCo1 > 1) - giong dieu kien cua KyNang_DonOChet; chua doc duoc
# thi van cho gan nhu cu (tha lot con hon chan nham). Bao mot dong ly do (thong bao kieu "vi sao khong an" van giu
# theo [GONMAN 14/09 b]).
# Chi JX_MOBILE (ca tep chi bien dich cho mobile); kiem --pc phai DAT.
# Chu tieng Viet sinh bang vn_edit.vn (KHONG go byte bang tay).
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_kynang_hotro_1409.py [goc]
import io, os, sys

sys.path.insert(0, os.path.join(os.path.expanduser('~'), '.claude', 'skills', 'swordonline-dev', 'scripts'))
from vn_edit import vn

GOC = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith('--') else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KQ = []
R = '\r\n'
T = '\t'

def doc(p):
    with io.open(p, 'r', encoding='latin-1', newline='') as f:
        return f.read()

def ghi(p, s):
    with io.open(p, 'w', encoding='latin-1', newline='') as f:
        f.write(s)

def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)

def va(p, cu, moi, dau_hieu, so_lan=1):
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:58])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:110]))
    s2 = s.replace(cu, moi)
    mong = cao(s) - cao(cu) * so_lan + cao(moi) * so_lan
    if cao(s2) != mong:
        raise SystemExit('LOI: so byte cao %d, mong %d, o %s' % (cao(s2), mong, p))
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:58]))

CD = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxCanDieuKhien.cpp')

BAO = vn(u"K\u1ef9 n\u0103ng h\u1ed7 tr\u1ee3: kh\u00f4ng g\u1eafn v\u00e0o \u00f4 \u0111\u01b0\u1ee3c")   # "Ky nang ho tro: khong gan vao o duoc" (dung \u trong tep latin-1)

# ---- 1. ham kiem tra: ky nang co dung duoc khong ----
va(CD,
   '//' + T + '[ANDROID 09/09 GAN] Bang ky nang goi vao day khi nguoi choi cham mot ky nang.' + R,
   '//' + T + '[KNHOTRO 14/09] Chu: "ky nang nao thuoc dang ho tro (khong dung duoc) thi khong cho bo xuong o ky nang".' + R +
   '//' + T + 'Dung duoc = co mat trong MOT trong ba bang cua Core: danh TRAI (GDI_LEFT_ENABLE_SKILLS), danh PHAI /' + R +
   '//' + T + 'vong sang (GDI_RIGHT_ENABLE_SKILLS), hay bang ky nang DANH (GDI_FIGHT_SKILLS, co ca ky nang cap 0).' + R +
   '//' + T + 'Vang mat ca ba = ky nang ho tro / noi cong / khong dung duoc. Dung dung ba bang ma [KYNANG 14/09 TRONG]' + R +
   '//' + T + 'da dung de don o chet, nen khong them duong hoi Core nao moi.' + R +
   'static bool KyNang_DungDuoc(unsigned int uId)' + R +
   '{' + R +
   T + 'if (g_pCoreShell == NULL || uId == 0)' + R +
   T + T + 'return true;' + T + '// khong biet thi cu cho gan (nhu truoc)' + R +
   T + 'KyNang_DocBang();' + R +
   T + 'if (s_nKNCo1 <= 1)' + R +
   T + T + 'return true;' + T + '// chua doc duoc bang trai (vua vao game / dang chuyen phai): khong chan' + R +
   T + 'if (KyNang_CoTrongBang(s_KNBang, s_nKNCo1, uId))' + R +
   T + T + 'return true;' + R +
   T + 'if (s_nKNCoPhai <= 0)' + R +
   T + '{' + R +
   T + T + 'memset(s_KNBangPhai, 0, sizeof(s_KNBangPhai));' + R +
   T + T + 's_nKNCoPhai = g_pCoreShell->GetGameData(GDI_RIGHT_ENABLE_SKILLS, (KUPARAM)&s_KNBangPhai, 0);' + R +
   T + T + 'if (s_nKNCoPhai < 0)' + T * 4 + 's_nKNCoPhai = 0;' + R +
   T + T + 'if (s_nKNCoPhai > KYNANG_DS_TOI_DA)' + T + 's_nKNCoPhai = KYNANG_DS_TOI_DA;' + R +
   T + '}' + R +
   T + 'if (KyNang_CoTrongBang(s_KNBangPhai, s_nKNCoPhai, uId))' + R +
   T + T + 'return true;' + R +
   T + 'if (s_nKNCoDanh <= 0)' + R +
   T + '{' + R +
   T + T + 'memset(s_KNBangDanh, 0, sizeof(s_KNBangDanh));' + R +
   T + T + 'g_pCoreShell->GetGameData(GDI_FIGHT_SKILLS, (KUPARAM)&s_KNBangDanh, 0);' + R +
   T + T + 'for (s_nKNCoDanh = 0; s_nKNCoDanh < KYNANG_DSDANH_TOI_DA && s_KNBangDanh[s_nKNCoDanh].uId; s_nKNCoDanh++)' + R +
   T + T + T + ';' + R +
   T + '}' + R +
   T + 'return KyNang_CoTrongBang(s_KNBangDanh, s_nKNCoDanh, uId);' + R +
   '}' + R +
   R +
   '//' + T + '[ANDROID 09/09 GAN] Bang ky nang goi vao day khi nguoi choi cham mot ky nang.' + R,
   'static bool KyNang_DungDuoc(unsigned int uId)')

# ---- 2. chan o dau JxKyNang_GanKyNang ----
va(CD,
   'bool JxKyNang_GanKyNang(unsigned int uGenre, unsigned int uId)' + R +
   '{' + R +
   T + 'if (!s_nKNCheDoGan || uId == 0)' + R +
   T + T + 'return false;' + R,
   'bool JxKyNang_GanKyNang(unsigned int uGenre, unsigned int uId)' + R +
   '{' + R +
   T + 'if (!s_nKNCheDoGan || uId == 0)' + R +
   T + T + 'return false;' + R +
   T + '// [KNHOTRO 14/09] ky nang ho tro / noi cong (khong nam trong ba bang dung duoc): khong cho gan vao o.' + R +
   T + 'if (!KyNang_DungDuoc(uId))' + R +
   T + '{' + R +
   T + T + 'KyNang_Bao("' + BAO + '");' + R +
   T + T + 'g_DebugLog("[KYNANG] ky nang %u khong dung duoc (ho tro / noi cong) -> khong gan vao o", uId);' + R +
   T + T + 'return false;' + R +
   T + '}' + R,
   '[KNHOTRO 14/09] ky nang ho tro / noi cong (khong nam trong ba bang dung duoc)')

for k in KQ:
    print(k)
