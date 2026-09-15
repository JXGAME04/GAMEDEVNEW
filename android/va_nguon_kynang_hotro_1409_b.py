# -*- coding: latin-1 -*-
# [KNHOTRO 14/09 b] Sua lai phep thu "ky nang nay dung duoc khong" - ban a chan hut.
#
# Thu tren may ao: cham "The Van Tung" (id 160, Property = "ho tro chien dau - bi dong", LRSkill = 3)
# van gan duoc vao o. Doc lai Core moi ro:
#   - KSkillList::GetSkillSortList (GDI_FIGHT_SKILLS, KSkillList.cpp:638) chi loai ky nang GOC (IsBase) va
#     cac the loai ngoai danh sach; ky nang BI DONG (SKILL_SS_PassivityNpcState) VAN nam trong bang.
#     -> lay bang nay lam phep thu la nhan het ky nang ho tro.
#   - GetLeftSkillSortList / GetRightSkillSortList (700 / 767) moi la phep thu dung: hai ham do doi
#     GetSkillLRInfo() phai la BothSkill / LeftOnlySkill / RightOnlySkill. Ky nang ho tro co
#     LRSkill = 3 = NoneSkill (SkillDef.h:55-61 "khong tay nao") nen bi loai khoi CA HAI bang.
#     Hai ham cung doi da hoc (SkillLevel > 0) va du cap nhan vat (GetSkillReqLevel).
#   MAX_LRSKILL_SORTLIST = 65 = KYNANG_DS_TOI_DA nen khong cat bot muc nao.
#
# Vay: dung duoc = co mat trong bang danh TRAI hoac bang danh PHAI. Bo han duong GDI_FIGHT_SKILLS o day.
# (KyNang_DonOChet van giu ca ba bang - o do la tha lot con hon xoa nham o cua nguoi choi, nguoc chieu voi cho nay.)
#
# Bang PHAI doc lai MOI LAN thay vi nho (s_nKNCoPhai <= 0): day la viec theo cu cham cua nguoi choi,
# khong phai moi khung, ma bang cu se sai ngay sau khi doi phai / hoc them chieu.
#
# Chi JX_MOBILE (ca tep chi bien dich cho mobile); kiem --pc phai DAT. Khong dung chu tieng Viet moi.
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_kynang_hotro_1409_b.py [goc]
import io, os, sys

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

CU = (
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
    '}' + R
)

MOI = (
    '//' + T + '[KNHOTRO 14/09] Chu: "ky nang nao thuoc dang ho tro (khong dung duoc) thi khong cho bo xuong o ky nang".' + R +
    '//' + T + '[KNHOTRO 14/09 b] Dung duoc = co mat trong bang danh TRAI (GDI_LEFT_ENABLE_SKILLS) HOAC bang danh PHAI /' + R +
    '//' + T + 'vong sang (GDI_RIGHT_ENABLE_SKILLS). Hai bang do la cho duy nhat Core doi GetSkillLRInfo() phai la' + R +
    '//' + T + 'BothSkill / LeftOnlySkill / RightOnlySkill (KSkillList.cpp:700, 767), nen ky nang ho tro - noi cong' + R +
    '//' + T + '(LRSkill = 3 = NoneSkill, "khong tay nao") bi loai khoi ca hai. Hai bang cung doi DA HOC (cap > 0) va' + R +
    '//' + T + 'DU CAP nhan vat, dung y "khong dung duoc" cua chu.' + R +
    '//' + T + 'KHONG dung GDI_FIGHT_SKILLS o day: GetSkillSortList (KSkillList.cpp:638) chi loai ky nang GOC, ky nang' + R +
    '//' + T + 'BI DONG van nam trong bang -> lay lam phep thu la nhan het (ban a chan hut: "The Van Tung" id 160,' + R +
    '//' + T + 'Property "ho tro chien dau - bi dong", van gan duoc). KyNang_DonOChet van dung ca ba bang vi o do' + R +
    '//' + T + 'tha lot con hon xoa nham o cua nguoi choi - nguoc chieu voi cho nay.' + R +
    'static bool KyNang_DungDuoc(unsigned int uId)' + R +
    '{' + R +
    T + 'if (g_pCoreShell == NULL || uId == 0)' + R +
    T + T + 'return true;' + T + '// khong biet thi cu cho gan (nhu truoc)' + R +
    T + 'KyNang_DocBang();' + R +
    T + 'if (s_nKNCo1 <= 1)' + R +
    T + T + 'return true;' + T + '// chua doc duoc bang trai (vua vao game / dang chuyen phai): khong chan' + R +
    T + 'if (KyNang_CoTrongBang(s_KNBang, s_nKNCo1, uId))' + R +
    T + T + 'return true;' + R +
    T + '// Doc lai bang PHAI MOI LAN: day la viec theo cu cham cua nguoi choi (khong phai moi khung), ma bang cu' + R +
    T + '// se sai ngay sau khi doi phai / hoc them chieu.' + R +
    T + 'memset(s_KNBangPhai, 0, sizeof(s_KNBangPhai));' + R +
    T + 's_nKNCoPhai = g_pCoreShell->GetGameData(GDI_RIGHT_ENABLE_SKILLS, (KUPARAM)&s_KNBangPhai, 0);' + R +
    T + 'if (s_nKNCoPhai < 0)' + T * 4 + 's_nKNCoPhai = 0;' + R +
    T + 'if (s_nKNCoPhai > KYNANG_DS_TOI_DA)' + T + 's_nKNCoPhai = KYNANG_DS_TOI_DA;' + R +
    T + 'return KyNang_CoTrongBang(s_KNBangPhai, s_nKNCoPhai, uId);' + R +
    '}' + R
)

va(CD, CU, MOI, '[KNHOTRO 14/09 b] Dung duoc = co mat trong bang danh TRAI')

# thong bao / dong log: noi ro ly do theo cach hieu moi
va(CD,
   T + '// [KNHOTRO 14/09] ky nang ho tro / noi cong (khong nam trong ba bang dung duoc): khong cho gan vao o.' + R,
   T + '// [KNHOTRO 14/09 b] ky nang ho tro / noi cong (khong danh duoc bang tay trai lan tay phai): khong cho gan vao o.' + R,
   '[KNHOTRO 14/09 b] ky nang ho tro / noi cong (khong danh duoc bang tay trai')

va(CD,
   T + T + 'g_DebugLog("[KYNANG] ky nang %u khong dung duoc (ho tro / noi cong) -> khong gan vao o", uId);' + R,
   T + T + 'g_DebugLog("[KYNANG] ky nang %u khong dung duoc (khong co trong bang danh trai lan phai) -> khong gan vao o", uId);' + R,
   '[KYNANG] ky nang %u khong dung duoc (khong co trong bang danh trai lan phai)')

for k in KQ:
    print(k)
