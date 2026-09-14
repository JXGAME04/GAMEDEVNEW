# -*- coding: latin-1 -*-
# [TGNAC 14/09 d] Chu 14/09 16:0x: "npc voi player bi mo" ngay sau khi 109141557 len dt_v4 -> TAT MAC DINH.
#   Ly do (phien do nhip + do lai): duong CU ve the gioi 1:1 vao RT to (khung x zoom x le) roi thu nho ca anh luc blit = SIEU LAY MAU
#   (sprite ve nguyen chi tiet roi moi loc mot lan) ; TGNAC ve THANG o co nho, moi sprite bi thu nho ngay luc quet (loc bang mau 4 diem)
#   -> mat chi tiet than NPC / nguoi choi. Chu chi keu NPC + nhan vat mo, KHONG keu chu, dung voi CHUNET (chu ve sau blit).
#   Luat chu: khong giam trai nghiem nguoi choi -> net hon uu tien hon GPU. Nay:
#     - TheGioiRTNac mac dinh 0 (TAT) trong ma va config; ai muon doi GPU lay do net thi dat 1100.
#     - Them TheGioiRTNacLoc (mac dinh 1): 1 = loc bang mau tuyen tinh (mo nhung muot), 0 = khong ep loc (net hon, co rang cua) - de so.
# Idempotent, latin-1, neo CRLF. Chay SAU va_nguon_tgnac_1409.py. Dung: python android/va_nguon_tgnac_1409_b.py [goc]
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
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:60])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:110]))
    s2 = s.replace(cu, moi)
    if cao(s2) != cao(s):
        raise SystemExit('LOI: so byte cao doi o %s' % p)
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:60]))

RC = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.cpp')

va(RC,
   'static int s_nTgNac = 1100;' + T + '// [TGNAC 14/09] zoom (phan nghin) tu muc nay ve the gioi THU NHO vao vung khung x le (diem anh ~1x) thay vi RT to; 0 = tat',
   'static int s_nTgNac = 0, s_nTgNacLoc = 1;' + T + '// [TGNAC 14/09 d] MAC DINH TAT (chu 16:0x "npc voi player bi mo": ve thang o co nho = moi sprite thu nho luc quet, mat chi tiet;'
   ' duong cu ve 1:1 vao RT to roi thu ca anh = sieu lay mau, net hon). 1100 = bat tu 110 %; NacLoc 1 = loc bang mau tuyen tinh (muot), 0 = khong ep loc (net hon, co rang cua)',
   '[TGNAC 14/09 d] MAC DINH TAT')

va(RC,
   T + 's_nTgNac = Rep3Ini("TheGioiRTNac", 1100); if (s_nTgNac < 0) s_nTgNac = 0; if (s_nTgNac > 0 && s_nTgNac < 1001) s_nTgNac = 1001;' + T + '// [TGNAC 14/09]' + R +
   T + 'Rep3Log("[TGNAC] nhin rong tu %d phan nghin: ve the gioi thu nho vao vung khung x le (TheGioiRTNac; 0 = tat)", s_nTgNac);',
   T + 's_nTgNac = Rep3Ini("TheGioiRTNac", 0); if (s_nTgNac < 0) s_nTgNac = 0; if (s_nTgNac > 0 && s_nTgNac < 1001) s_nTgNac = 1001;' + T + '// [TGNAC 14/09 d] mac dinh 0 = TAT (net hon)' + R +
   T + 's_nTgNacLoc = Rep3Ini("TheGioiRTNacLoc", 1) ? 1 : 0;' + R +
   T + 'Rep3Log("[TGNAC] nhin rong tu %d phan nghin: ve the gioi thu nho vao vung khung x le, loc bang mau %d (TheGioiRTNac 0 = tat / TheGioiRTNacLoc)", s_nTgNac, s_nTgNacLoc);',
   'Rep3Ini("TheGioiRTNac", 0)')

va(RC,
   'Rep3Gpu_VpLogic(PD3DDEVICE, m_nTgW, m_nTgH); Rep3Gpu_VpEp(PD3DDEVICE, m_pTgSurf, m_nTgPxW, m_nTgPxH); Rep3Gpu_PalLin(PD3DDEVICE, 1);',
   'Rep3Gpu_VpLogic(PD3DDEVICE, m_nTgW, m_nTgH); Rep3Gpu_VpEp(PD3DDEVICE, m_pTgSurf, m_nTgPxW, m_nTgPxH); Rep3Gpu_PalLin(PD3DDEVICE, s_nTgNacLoc);' + T + '// [TGNAC 14/09 d] NacLoc = 0 thi khong ep loc (net hon)',
   'Rep3Gpu_PalLin(PD3DDEVICE, s_nTgNacLoc);')

# ---------------- config: lop ghi de + may ao (dt_v4 lam rieng sau khi bao phien do nhip) ----------------
def doi_khoa(p, cu, moi, them_moi=None, dau=None):
    if not os.path.isfile(p):
        KQ.append('khong thay: ' + p); return
    s = doc(p)
    nl = '\r\n' if s.count('\r\n') > s.count('\n') // 2 else '\n'
    if s.count(cu + nl) == 1:
        s = s.replace(cu + nl, moi + nl, 1)
        KQ.append('da doi khoa: %s (%s -> %s)' % (p, cu, moi))
    elif moi + nl not in s:
        KQ.append('LOI khong thay %r o %s' % (cu, p)); return
    if them_moi and dau and dau not in s:
        s = s.replace(moi + nl, moi + nl + them_moi.replace('\n', nl), 1)
        KQ.append('da them: ' + dau)
    ghi(p, s)

for g in [os.path.join(GOC, 'android', 'du_lieu_ghi_de'), r'D:\jx1_android_data']:
    doi_khoa(os.path.join(g, 'config.ini'), 'TheGioiRTNac=1100', 'TheGioiRTNac=0',
             '; [TGNAC 14/09 d] TAT mac dinh (chu 16:0x "npc voi player bi mo"): ve thu nho luc quet lam mat chi tiet than sprite; duong cu (RT to roi thu ca anh)\n'
             ';   la sieu lay mau nen net hon - dung net doi lay GPU. Dat 1100 de bat lai khi can cuu GPU luc dong. TheGioiRTNacLoc: 1 = loc bang mau tuyen tinh (muot, mo),\n'
             ';   0 = khong ep loc (net hon, co rang cua) - chi co nghia khi TheGioiRTNac > 0.\n'
             'TheGioiRTNacLoc=1\n', 'TheGioiRTNacLoc=')

for k in KQ:
    print(k)
