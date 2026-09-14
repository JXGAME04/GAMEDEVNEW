# -*- coding: latin-1 -*-
# [LAC 14/09 b] LAC LEN / XUONG khi keo doc (chu 14/09 00:3x: "lam xoay qua trai va phai - tren duoi can vay cung oke"): keo doc toi
# LiaXaDoc thi canh co/dan doc LiaLacDoc % quanh tam khung (nhu camera cui / ngang), tha tay troi ve cung do lech. Represent3 lenh 9 dat
# co dan doc (phan nghin), 10 hoi; M = R(goc) x diag(1, k): x' = c.x - 2s.k.y, y' = s.x/2 + c.k.y; M^-1: x = c.x + 2s.y, y = (-s.x/2 + c.y)/k.
# Chay SAU va_nguon_lac_1409.py (neo la chu do kich ban a chen). Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_lac_1409_b.py [goc] [--config <ini> ...]
import io, os, sys

GOC = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith('--') else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KQ = []
R = '\r\n'

def doc(p):
    with io.open(p, 'r', encoding='latin-1', newline='') as f:
        return f.read()

def ghi(p, s):
    with io.open(p, 'w', encoding='latin-1', newline='') as f:
        f.write(s)

def va(p, cu, moi, dau_hieu, so_lan=1):
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:50])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:90]))
    cao_truoc = sum(1 for c in s if ord(c) >= 0x80)
    s2 = s.replace(cu, moi)
    cao_sau = sum(1 for c in s2 if ord(c) >= 0x80)
    if cao_truoc != cao_sau:
        raise SystemExit('LOI: so byte cao doi %d -> %d o %s' % (cao_truoc, cao_sau, p))
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:50]))

RH = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.h')
RC = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.cpp')
LC = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxLiaCanh.cpp')

# ===================== Represent3 =====================
va(RH,
   '    int   m_nTgXoay, m_nTgLe, m_nTgZoomRt;' + R,
   '    int   m_nTgXoay, m_nTgLe, m_nTgZoomRt;' + R +
   '    int   m_nTgDoc;      // [LAC 14/09 b] co dan doc (phan nghin, 1000 = khong): lac len / xuong khi keo doc, M = R(goc) x diag(1, k)' + R,
   'int   m_nTgDoc;')
va(RC,
   '\tm_nTgXoay = 0; m_nTgLe = 1000; m_nTgZoomRt = 1000;\t// [LAC 14/09]' + R,
   '\tm_nTgXoay = 0; m_nTgLe = 1000; m_nTgZoomRt = 1000;\t// [LAC 14/09]' + R +
   '\tm_nTgDoc = 1000;\t// [LAC 14/09 b]' + R,
   '\tm_nTgDoc = 1000;\t// [LAC 14/09 b]')
va(RC,
   '\t\treturn m_nTgLe;' + R + '\t}' + R,
   '\t\treturn m_nTgLe;' + R + '\t}' + R +
   '\tif (nLenh == 9)' + R +
   '\t{\t// [LAC 14/09 b] co dan doc (phan nghin, kep 850..1150): keo doc -> camera "cui / ngang" nhe; 1000 = khong. Chi doi blit + cua toa do' + R +
   '\t\tint nD = nThamSo; if (nD < 850) nD = 850; if (nD > 1150) nD = 1150;' + R +
   '\t\tif (nD != m_nTgDoc) { m_nTgDoc = nD; g_nJxTheGioiEp = 1; }' + R +
   '\t\treturn m_nTgDoc;' + R +
   '\t}' + R +
   '\tif (nLenh == 10) return m_nTgDoc;' + R,
   '\tif (nLenh == 9)')
# fK o 4 cho (blit + 3 cua toa do)
va(RC,
   '\t\t\tconst float fGoc = (float)m_nTgXoay * 3.14159265f / 18000.0f, fC = cosf(fGoc), fS = sinf(fGoc);' + R,
   '\t\t\tconst float fGoc = (float)m_nTgXoay * 3.14159265f / 18000.0f, fC = cosf(fGoc), fS = sinf(fGoc), fK = (float)m_nTgDoc / 1000.0f;\t// [LAC 14/09 b] fK = co dan doc' + R,
   'fK = (float)m_nTgDoc / 1000.0f;', so_lan=4)
va(RC,
   '\t\t\t\tv[i].position = D3DXVECTOR4(fKW * 0.5f + fC * qx - 2.0f * fS * qy, fKH * 0.5f + 0.5f * fS * qx + fC * qy, 100, 1);' + R,
   '\t\t\t\tv[i].position = D3DXVECTOR4(fKW * 0.5f + fC * qx - 2.0f * fS * fK * qy, fKH * 0.5f + 0.5f * fS * qx + fC * fK * qy, 100, 1);' + R,
   'fC * qx - 2.0f * fS * fK * qy, fKH')
va(RC,
   '\t\t\tnX = (int)floorf((float)g_nScreenWidth * 0.5f + fC * qx - 2.0f * fS * qy + 0.5f);' + R +
   '\t\t\tnY = (int)floorf((float)g_nScreenHeight * 0.5f + 0.5f * fS * qx + fC * qy + 0.5f);' + R,
   '\t\t\tnX = (int)floorf((float)g_nScreenWidth * 0.5f + fC * qx - 2.0f * fS * fK * qy + 0.5f);' + R +
   '\t\t\tnY = (int)floorf((float)g_nScreenHeight * 0.5f + 0.5f * fS * qx + fC * fK * qy + 0.5f);' + R,
   'fC * qx - 2.0f * fS * fK * qy + 0.5f', so_lan=2)
va(RC,
   '\t\t\tconst float py = (float)m_nTgH * 0.5f + (-0.5f * fS * qx + fC * qy) * fZ;' + R,
   '\t\t\tconst float py = (float)m_nTgH * 0.5f + (-0.5f * fS * qx + fC * qy) / fK * fZ;\t// [LAC 14/09 b] chia co dan doc' + R,
   '(-0.5f * fS * qx + fC * qy) / fK * fZ')
va(RC,
   '\t\t{ const DWORD dwLoc = (m_nTgZoomRt > 1000 || m_nTgXoay) ? D3DTEXF_LINEAR : D3DTEXF_POINT;',
   '\t\t{ const DWORD dwLoc = (m_nTgZoomRt > 1000 || m_nTgXoay || m_nTgDoc != 1000) ? D3DTEXF_LINEAR : D3DTEXF_POINT;',
   '(m_nTgZoomRt > 1000 || m_nTgXoay || m_nTgDoc != 1000) ? D3DTEXF_LINEAR')
va(RC,
   '\t\tif (m_nTgZoomRt > 1000 || m_nTgXoay)' + R,
   '\t\tif (m_nTgZoomRt > 1000 || m_nTgXoay || m_nTgDoc != 1000)' + R,
   '\t\tif (m_nTgZoomRt > 1000 || m_nTgXoay || m_nTgDoc != 1000)' + R)
va(RC,
   '\t\tif ((m_nTgZoomRt > 1000 || m_nTgXoay) && m_nTgTrangThai == 0)' + R,
   '\t\tif ((m_nTgZoomRt > 1000 || m_nTgXoay || m_nTgDoc != 1000) && m_nTgTrangThai == 0)' + R,
   '\t\tif ((m_nTgZoomRt > 1000 || m_nTgXoay || m_nTgDoc != 1000) && m_nTgTrangThai == 0)' + R, so_lan=2)

# ===================== JxLiaCanh.cpp =====================
va(LC,
   'static int\t\t\ts_nGocDaGui = 0, s_nLeDaGui = 1000;\t// da gui cho Represent3' + R,
   'static int\t\t\ts_nGocDaGui = 0, s_nLeDaGui = 1000;\t// da gui cho Represent3' + R +
   'static int\t\t\ts_nLacDoc = 6;\t\t// [LAC 14/09 b] LiaLacDoc: % co/dan doc toi da khi keo doc toi LiaXaDoc (0 = tat)' + R +
   'static int\t\t\ts_nDocDaGui = 1000;' + R,
   's_nLacDoc = 6;')
va(LC,
   '\ts_nLacThu = GetPrivateProfileInt("Cham", "LiaLacThu", 0, szCfg);' + R,
   '\ts_nLacThu = GetPrivateProfileInt("Cham", "LiaLacThu", 0, szCfg);' + R +
   '\ts_nLacDoc = GetPrivateProfileInt("Cham", "LiaLacDoc", 6, szCfg);\t// [LAC 14/09 b]' + R +
   '\tif (s_nLacDoc < -15) s_nLacDoc = -15; if (s_nLacDoc > 15) s_nLacDoc = 15;' + R,
   '"LiaLacDoc"')
va(LC,
   '\tg_DebugLog("[LAC] camera lac nhe khi lia: %d do, le RT %d%%, thu %d do", s_nLacDo, s_nLacLe, s_nLacThu);' + R,
   '\tg_DebugLog("[LAC] camera lac nhe khi lia: ngang %d do, doc %d%%, le RT %d%%, thu %d do", s_nLacDo, s_nLacDoc, s_nLacLe, s_nLacThu);' + R,
   'ngang %d do, doc %d%%')
va(LC,
   '\tint nGoc = 0, nLe = 1000;' + R + R + '\tif (s_nLacThu && s_nLacThuXong)' + R,
   '\tint nGoc = 0, nLe = 1000, nDoc = 1000;' + R + R + '\tif (s_nLacThu && s_nLacThuXong)' + R,
   '\tint nGoc = 0, nLe = 1000, nDoc = 1000;')
va(LC,
   '\tif (s_nLacDo != 0 && s_nTrangThai != LIA_KHONG)' + R +
   '\t{' + R +
   '\t\tconst float fMaxX = (float)(SCREEN_WIDTH * s_nXaNgang / 100) * (float)s_nZoom / 100.f;' + R +
   '\t\tfloat fTi = (fMaxX > 1.f) ? s_fLechX / fMaxX : 0.f;' + R +
   '\t\tif (fTi > 1.f) fTi = 1.f; if (fTi < -1.f) fTi = -1.f;' + R +
   '\t\tnGoc = (int)(fTi * (float)s_nLacDo * 100.f);' + R +
   '\t\tnLe = s_nLacLe * 10;' + R +
   '\t}' + R,
   '\tif ((s_nLacDo != 0 || s_nLacDoc != 0) && s_nTrangThai != LIA_KHONG)' + R +
   '\t{' + R +
   '\t\tconst float fMaxX = (float)(SCREEN_WIDTH * s_nXaNgang / 100) * (float)s_nZoom / 100.f;' + R +
   '\t\tconst float fMaxY = (float)(SCREEN_HEIGHT * s_nXaDoc / 100 * 2) * (float)s_nZoom / 100.f;\t// [LAC 14/09 b] Y the gioi = 2 x px' + R +
   '\t\tfloat fTi = (fMaxX > 1.f) ? s_fLechX / fMaxX : 0.f;' + R +
   '\t\tfloat fTiY = (fMaxY > 1.f) ? s_fLechY / fMaxY : 0.f;' + R +
   '\t\tif (fTi > 1.f) fTi = 1.f; if (fTi < -1.f) fTi = -1.f;' + R +
   '\t\tif (fTiY > 1.f) fTiY = 1.f; if (fTiY < -1.f) fTiY = -1.f;' + R +
   '\t\tnGoc = (int)(fTi * (float)s_nLacDo * 100.f);' + R +
   '\t\tnDoc = 1000 + (int)(fTiY * (float)s_nLacDoc * 10.f);\t// keo xuong (lech am) -> k < 1: canh det nhu camera ngang hon; keo len -> k > 1' + R +
   '\t\tnLe = s_nLacLe * 10;' + R +
   '\t}' + R,
   'nDoc = 1000 + (int)(fTiY * (float)s_nLacDoc * 10.f);')
va(LC,
   '\tif (nGoc != s_nGocDaGui) { Rep3TheGioi(6, nGoc); s_nGocDaGui = nGoc; }' + R + '}' + R,
   '\tif (nGoc != s_nGocDaGui) { Rep3TheGioi(6, nGoc); s_nGocDaGui = nGoc; }' + R +
   '\tif (nDoc != s_nDocDaGui) { Rep3TheGioi(9, nDoc); s_nDocDaGui = nDoc; }\t// [LAC 14/09 b]' + R + '}' + R,
   'Rep3TheGioi(9, nDoc); s_nDocDaGui = nDoc;')

# [LAC 14/09 c] (soi cheo phien do nhip): het lia thi GIU le RT them 2 s (keo lien tiep khong cap lai RT 3 MB moi lan), JxLia_Nhip goi
# ApXoay trong luc cho; goc luon = 0 truoc khi le ve 1000 (cung mot ApXoay: gui le roi goc, ca hai truoc khung ke).
va(LC,
   'static int\t\t\ts_nDocDaGui = 1000;' + R,
   'static int\t\t\ts_nDocDaGui = 1000;' + R +
   'static unsigned int\ts_uLeChoTat = 0;\t// [LAC 14/09 c] luc bat dau cho tat le RT sau khi het lia (giu 2 s)' + R,
   's_uLeChoTat = 0;\t// [LAC 14/09 c]')
va(LC,
   '\tif (nLe != s_nLeDaGui)' + R + '\t{' + R + '\t\tRep3TheGioi(8, nLe); s_nLeDaGui = nLe;' + R,
   '\tif (nLe == 1000 && s_nLeDaGui != 1000)' + R +
   '\t{\t// [LAC 14/09 c] het lia: giu le RT them 2 s (keo lien tiep khong phai cap lai RT); JxLia_Nhip goi lai ApXoay trong luc cho' + R +
   '\t\tconst unsigned int uLuc = (unsigned int)GetTickCount();' + R +
   '\t\tif (!s_uLeChoTat) s_uLeChoTat = uLuc;' + R +
   '\t\tif (uLuc - s_uLeChoTat < 2000) nLe = s_nLeDaGui;' + R +
   '\t\telse s_uLeChoTat = 0;' + R +
   '\t}' + R +
   '\telse s_uLeChoTat = 0;' + R +
   '\tif (nLe != s_nLeDaGui)' + R + '\t{' + R + '\t\tRep3TheGioi(8, nLe); s_nLeDaGui = nLe;' + R,
   '[LAC 14/09 c] het lia: giu le RT them 2 s')
va(LC,
   '\tif (s_nTrangThai == LIA_KHONG)' + R + '\t\treturn;' + R + '\t// nhan vat co dang di khong: tieu diem goc (nOrigFocus) doi giua hai nhip' + R,
   '\tif (s_nTrangThai == LIA_KHONG && s_nLeDaGui != 1000)' + R +
   '\t\tApXoay();\t// [LAC 14/09 c] dem 2 s roi tat le RT' + R +
   '\tif (s_nTrangThai == LIA_KHONG)' + R + '\t\treturn;' + R + '\t// nhan vat co dang di khong: tieu diem goc (nOrigFocus) doi giua hai nhip' + R,
   '[LAC 14/09 c] dem 2 s roi tat le RT')

# ===================== config.ini =====================
KHOI = [
    '; [LAC 14/09 b] keo DOC toi LiaXaDoc thi canh co/dan doc LiaLacDoc % (keo xuong = det nhu camera ngang hon, keo len = cao hon), 0 = tat',
    'LiaLacDoc=6',
]
def va_config(p):
    s = doc(p)
    if 'LiaLacDoc=' in s:
        KQ.append('bo qua (da co): %s : LiaLacDoc' % p); return
    nl = '\r\n' if s.count('\r\n') > s.count('\n') // 2 else '\n'
    neo = 'LiaLacThu=0' + nl
    if s.count(neo) != 1:
        raise SystemExit('LOI: khong thay LiaLacThu=0 duy nhat trong %s (may ao dang de LiaLacThu khac 0?)' % p)
    ghi(p, s.replace(neo, neo + nl.join(KHOI) + nl))
    KQ.append('da va: %s : khoi [Cham] LiaLacDoc' % p)
va_config(os.path.join(GOC, 'android', 'du_lieu_ghi_de', 'config.ini'))
if '--config' in sys.argv:
    for p in sys.argv[sys.argv.index('--config') + 1:]:
        if os.path.isfile(p): va_config(p)
        else: KQ.append('khong co tep: %s' % p)
print('\n'.join(KQ))
