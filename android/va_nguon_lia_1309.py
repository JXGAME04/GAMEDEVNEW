# -*- coding: latin-1 -*-
# [LIA 13/09] Va nguon cho LIA CANH mobile (xem PHUONGAN_LIA_CANH_MOBILE_1309.md, Sources/S3Client/Platform/JxLiaCanh.h).
# Idempotent: chay lai khong doi gi. Doc/ghi latin-1 (giu nguyen byte TCVN3), neo CRLF cho nguon, LF cho cmake/ini.
# Dung: python android/va_nguon_lia_1309.py [goc worktree]  [--config <config.ini> ...]
import io, os, sys

GOC = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith('--') else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KQ = []

def doc(p):
    with io.open(p, 'r', encoding='latin-1', newline='') as f:
        return f.read()

def ghi(p, s):
    with io.open(p, 'w', encoding='latin-1', newline='') as f:
        f.write(s)

def va(p, cu, moi, dau_hieu):
    """Thay 'cu' bang 'moi' dung 1 lan; bo qua neu 'dau_hieu' da co (idempotent)."""
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:50])); return
    n = s.count(cu)
    if n != 1:
        raise SystemExit('LOI: neo gap %d lan trong %s: %r' % (n, p, cu[:80]))
    cao_truoc = sum(1 for c in s if ord(c) >= 0x80)
    s2 = s.replace(cu, moi)
    cao_sau = sum(1 for c in s2 if ord(c) >= 0x80)
    if cao_truoc != cao_sau:
        raise SystemExit('LOI: so byte cao doi %d -> %d o %s' % (cao_truoc, cao_sau, p))
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:50]))

R = '\r\n'
S = os.path.join(GOC, 'Sources', 'S3Client')

# --- A. KSdlApp.h: them CHAM_LIA ----------------------------------------------------------------
va(os.path.join(S, 'Platform', 'KSdlApp.h'),
   '\t\tCHAM_KYNANG, CHAM_CAM };' + R,
   '\t\tCHAM_KYNANG, CHAM_CAM, CHAM_LIA };\t// [LIA 13/09] CHAM_LIA = ngon keo tren ban do = lia canh (JxLiaCanh)' + R,
   'CHAM_LIA')

# --- B. KSdlApp.cpp -------------------------------------------------------------------------------
KS = os.path.join(S, 'Platform', 'KSdlApp.cpp')
va(KS,
   '#include "JxCanDieuKhien.h"' + R,
   '#include "JxCanDieuKhien.h"' + R + '#include "JxLiaCanh.h"\t// [LIA 13/09] lia canh: mot ngon keo tren ban do' + R,
   '#include "JxLiaCanh.h"')
va(KS,
   '\t\tJxCan_Nhip();\t// [ANDROID 09/09 CAN] dang cam can thi day nhan vat di theo huong' + R,
   '\t\tJxCan_Nhip();\t// [ANDROID 09/09 CAN] dang cam can thi day nhan vat di theo huong' + R +
   '\t\tJxLia_Nhip();\t// [LIA 13/09] lia canh: ap do lech moi khung / dem cho / troi ve nhan vat' + R,
   'JxLia_Nhip();')
va(KS,
   '\t\t\tif (JxCan_TrongVung(m_nChamX0, m_nChamY0) && !JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0))' + R +
   '\t\t\t{' + R +
   '\t\t\t\tm_nCham = CHAM_CAN;' + R +
   '\t\t\t\tJxCan_BatDau(m_nChamX0, m_nChamY0, m_nChamX, m_nChamY);' + R +
   '\t\t\t\treturn true;' + R +
   '\t\t\t}' + R,
   '\t\t\tif (JxCan_TrongVung(m_nChamX0, m_nChamY0) && !JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0))' + R +
   '\t\t\t{' + R +
   '\t\t\t\tm_nCham = CHAM_CAN;' + R +
   '\t\t\t\tJxCan_BatDau(m_nChamX0, m_nChamY0, m_nChamX, m_nChamY);' + R +
   '\t\t\t\treturn true;' + R +
   '\t\t\t}' + R +
   '\t\t\t// [LIA 13/09] Keo tren BAN DO (ngoai giao dien, ngoai vung can, ngoai nut ky nang) = LIA CANH - nhu game 3D mot ngon keo' + R +
   '\t\t\t// la quay camera; di bang can dieu khien hoac cham. Truoc: giu chuot trai roi re (di lien tuc) - [Cham] LiaCanh=0 thi ve nhu cu.' + R +
   '\t\t\tif (JxLia_DuocBatDau(m_nChamX0, m_nChamY0))' + R +
   '\t\t\t{' + R +
   '\t\t\t\tm_nCham = CHAM_LIA;' + R +
   '\t\t\t\tJxLia_BatDau(m_nChamX0, m_nChamY0);' + R +
   '\t\t\t\tJxLia_Keo(m_nChamX, m_nChamY);' + R +
   '\t\t\t\treturn true;' + R +
   '\t\t\t}' + R,
   'JxLia_DuocBatDau(m_nChamX0, m_nChamY0)')
va(KS,
   '\t\tif (m_nCham == CHAM_CAN)' + R +
   '\t\t{' + R +
   '\t\t\tJxCan_Keo(m_nChamX, m_nChamY);' + R +
   '\t\t\treturn true;' + R +
   '\t\t}' + R,
   '\t\tif (m_nCham == CHAM_CAN)' + R +
   '\t\t{' + R +
   '\t\t\tJxCan_Keo(m_nChamX, m_nChamY);' + R +
   '\t\t\treturn true;' + R +
   '\t\t}' + R +
   '\t\tif (m_nCham == CHAM_LIA)' + R +
   '\t\t{' + R +
   '\t\t\tJxLia_Keo(m_nChamX, m_nChamY);\t// [LIA 13/09]' + R +
   '\t\t\treturn true;' + R +
   '\t\t}' + R,
   'JxLia_Keo(m_nChamX, m_nChamY);\t// [LIA 13/09]')
va(KS,
   '\t\telse if (nTruoc == CHAM_CAN)' + R +
   '\t\t{' + R +
   '\t\t\tJxCan_Nha();' + R +
   '\t\t}' + R +
   '\t\telse if (nTruoc == CHAM_KEO)' + R,
   '\t\telse if (nTruoc == CHAM_CAN)' + R +
   '\t\t{' + R +
   '\t\t\tJxCan_Nha();' + R +
   '\t\t}' + R +
   '\t\telse if (nTruoc == CHAM_LIA)' + R +
   '\t\t{' + R +
   '\t\t\tJxLia_Nha();\t// [LIA 13/09] nha ngon: cho LiaChoVeMs roi troi ve nhan vat' + R +
   '\t\t}' + R +
   '\t\telse if (nTruoc == CHAM_KEO)' + R,
   'JxLia_Nha();')

# --- C. GameSpaceChangedNotify.cpp: doi map that -> dat lai ----------------------------------------
GN = os.path.join(S, 'Ui', 'GameSpaceChangedNotify.cpp')
# [LIA 13/09 b] include phai nam TRONG rao JX_MOBILE (phien do nhip soi cheo 22:2x: dong include tran ra ban Windows)
va(GN,
   '#include "Elem/MouseHover.h"' + R,
   '#include "Elem/MouseHover.h"' + R + '#ifdef JX_MOBILE' + R + '#include "../Platform/JxLiaCanh.h"\t// [LIA 13/09] doi map that -> bo do lech lia canh' + R + '#endif' + R,
   'JxLiaCanh.h')
# cay da va truoc 13/09 22:30 (include chua rao) -> rao lai
va(GN,
   '#include "Elem/MouseHover.h"' + R + '#include "../Platform/JxLiaCanh.h"\t// [LIA 13/09] doi map that -> bo do lech lia canh' + R,
   '#include "Elem/MouseHover.h"' + R + '#ifdef JX_MOBILE' + R + '#include "../Platform/JxLiaCanh.h"\t// [LIA 13/09] doi map that -> bo do lech lia canh' + R + '#endif' + R,
   '#ifdef JX_MOBILE' + R + '#include "../Platform/JxLiaCanh.h"')
va(GN,
   '\tcase GDCNI_SWITCHING_MAPMODE:' + R,
   '\tcase GDCNI_SWITCHING_MAPMODE:' + R +
   '#ifdef JX_MOBILE' + R +
   '\t\tJxLia_DatLai();\t// [LIA 13/09] doi map that (KScenePlaceC::OpenPlace) -> bo do lech lia canh, tat co FollowWithMap' + R +
   '#endif' + R,
   'JxLia_DatLai();')

# --- D. android/lists/s3client.cmake (LF) ----------------------------------------------------------
va(os.path.join(GOC, 'android', 'lists', 's3client.cmake'),
   '    ${JX_ROOT}/Sources/S3Client/Platform/JxCanDieuKhien.cpp\n',
   '    ${JX_ROOT}/Sources/S3Client/Platform/JxCanDieuKhien.cpp\n    ${JX_ROOT}/Sources/S3Client/Platform/JxLiaCanh.cpp\n',
   'JxLiaCanh.cpp')

# --- E. config.ini: khoa [Cham] Lia* (lop ghi de + cac ban sao du lieu neu duoc chi) --------------
KHOI = [
    '; [LIA 13/09] LIA CANH: mot ngon keo tren ban do (ngoai giao dien, ngoai vung can, ngoai nut ky nang) = lia khung nhin theo tay,',
    ';   nha tay cho LiaChoVeMs roi tu troi ve nhan vat (chep cam giac camera Kiem Vong Giang Ho). 0 = tat han -> keo = di lien tuc nhu cu.',
    'LiaCanh=1',
    '; lech toi da: % be rong / chieu cao khung ve (60 % ~ 800 x 370 px tren Fold 7 gap)',
    'LiaXaNgang=60',
    'LiaXaDoc=60',
    '; tay di it hon bao nhieu px thi chua tinh (rung tay)',
    'LiaNguong=3',
    '; nha tay bao lau (ms) thi bat dau ve',
    'LiaChoVeMs=1000',
    '; toc do ve toi thieu (px/giay) va he so em cuoi (moi giay ve bao nhieu lan phan con lai; 0 = di deu)',
    'LiaVeTocDo=700',
    'LiaVeEm=3',
    '; 1 = nhan vat buoc di / cam can dieu khien -> ve ngay, khong cho',
    'LiaVeKhiDi=1',
    '; 1 = ghi [LIA] ra nhat ky',
    'LiaNhatKy=0',
    '; GO LOI (may ao khong co ngon tay): dx,dy px -> 3 s sau khi vao the gioi tu lia roi tu ve; 0,0 = tat',
    'LiaThu=0,0',
]

def va_config(p):
    s = doc(p)
    if 'LiaCanh=' in s:
        KQ.append('bo qua (da co): %s : LiaCanh' % p); return
    nl = '\r\n' if s.count('\r\n') > s.count('\n') // 2 else '\n'
    neo = nl + '[DoNhip]' + nl
    khoi = nl.join(KHOI) + nl
    if s.count(neo) == 1:
        s2 = s.replace(neo, nl + khoi + nl + '[DoNhip]' + nl)
    else:
        # khong co [DoNhip] (config may ao): chen cuoi muc [Cham]; khong co [Cham] thi them muc moi cuoi tep
        dong = s.split(nl)
        i_cham = -1
        for i, d in enumerate(dong):
            if d.strip().lower() == '[cham]':
                i_cham = i; break
        if i_cham < 0:
            if not s.endswith(nl): s += nl
            s2 = s + nl + '[Cham]' + nl + khoi
        else:
            j = i_cham + 1
            while j < len(dong) and not dong[j].startswith('['):
                j += 1
            dong[j:j] = KHOI + ['']
            s2 = nl.join(dong)
    ghi(p, s2)
    KQ.append('da va: %s : khoi [Cham] Lia*' % p)

va_config(os.path.join(GOC, 'android', 'du_lieu_ghi_de', 'config.ini'))
if '--config' in sys.argv:
    for p in sys.argv[sys.argv.index('--config') + 1:]:
        if os.path.isfile(p):
            va_config(p)
        else:
            KQ.append('khong co tep: %s' % p)

print('\n'.join(KQ))
