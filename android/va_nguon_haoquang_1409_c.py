# -*- coding: latin-1 -*-
# [HAOQUANG 14/09 c] Vong hao quang DO MAC cua minh theo kieu game 3D (chu 14/09 09:0x: "vong hao quang do mac phai lam giong nhu 3d, khong dung
# vong sang co san"): bo vongtronvang/tim/xanh.spr, dung anh TRANG phang spr\haoquang\vongnguoi.spr (16 khung 144x72, android/anh_haoquang_vong.py)
# nhuom theo pham chat: hoang kim 0xFFD94E, do tim 0xE65AFF, bach kim 0xC8F0FF, xanh 0x5AE65A (khi [HaoQuang] TrangBiTu <= 1).
# Chay SAU va_nguon_haoquang_1409.py + _b.py. Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_haoquang_1409_c.py [goc]
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

KN = os.path.join(GOC, 'Sources', 'Core', 'Src', 'KNpc.cpp')

# 1) bang anh 6 -> 7: them vongnguoi.spr (chi so 6)
va(KN,
   'static KRUImage    s_HqAnh[6];' + R +
   'static const char* s_HqTen[6] = { "\\\\spr\\\\haoquang\\\\vongquai.spr", "\\\\spr\\\\haoquang\\\\vongboss.spr",' + R +
   '\t"\\\\spr\\\\haoquang\\\\vongtronvang.spr", "\\\\spr\\\\haoquang\\\\vongtrontim.spr", "\\\\spr\\\\haoquang\\\\vongtronxanh.spr",' + R +
   '\t"\\\\spr\\\\haoquang\\\\vongtrondo.spr" };' + R +
   'static const int   s_HqKhung[6] = { 12, 16, 30, 30, 30, 30 };' + R +
   'static const int   s_HqMs[6]    = { 70, 70, 50, 50, 80, 50 };' + R,
   'static KRUImage    s_HqAnh[7];' + R +
   'static const char* s_HqTen[7] = { "\\\\spr\\\\haoquang\\\\vongquai.spr", "\\\\spr\\\\haoquang\\\\vongboss.spr",' + R +
   '\t"\\\\spr\\\\haoquang\\\\vongtronvang.spr", "\\\\spr\\\\haoquang\\\\vongtrontim.spr", "\\\\spr\\\\haoquang\\\\vongtronxanh.spr",' + R +
   '\t"\\\\spr\\\\haoquang\\\\vongtrondo.spr", "\\\\spr\\\\haoquang\\\\vongnguoi.spr" };\t// [HAOQUANG 14/09 c] 6 = vong do mac kieu 3D (anh trang, nhuom)' + R +
   'static const int   s_HqKhung[7] = { 12, 16, 30, 30, 30, 30, 16 };' + R +
   'static const int   s_HqMs[7]    = { 70, 70, 50, 50, 80, 50, 70 };' + R,
   '[HAOQUANG 14/09 c] 6 = vong do mac kieu 3D')

# 2) nhan vat cua minh: anh 6 nhuom theo pham chat; cho phep xanh khi TrangBiTu <= 1
va(KN,
   '\t\tif (nMau < s_nHqTrangBiTu || nMau < gold_item)' + R +
   '\t\t\treturn;' + R +
   '\t\tnAnh = (nMau == gold_item) ? 2 : (nMau == purple_item) ? 3 : (nMau == platinum_item) ? 4 : -1;' + R,
   '\t\tif (nMau < s_nHqTrangBiTu || nMau < green_item)' + R +
   '\t\t\treturn;' + R +
   '\t\t// [HAOQUANG 14/09 c] chu: "vong hao quang do mac phai lam giong nhu 3d, khong dung vong sang co san" -> vongnguoi.spr (phang, trang)' + R +
   '\t\t// nhuom theo pham chat: hoang kim vang, do tim tim, bach kim trang xanh, xanh la cho do xanh (chi khi TrangBiTu <= 1)' + R +
   '\t\tnAnh = 6;' + R +
   '\t\tdwMau = (nMau == gold_item) ? 0xFFD94E : (nMau == purple_item) ? 0xE65AFF : (nMau == platinum_item) ? 0xC8F0FF : 0x5AE65A;' + R,
   '[HAOQUANG 14/09 c] chu: "vong hao quang do mac')

print('\n'.join(KQ))
