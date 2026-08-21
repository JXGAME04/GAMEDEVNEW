# -*- coding: utf-8 -*-
"""tcvn3.py -- doi chuoi TCVN3 (byte don) sang Unicode.

VI SAO CAN: moi chuoi tieng Viet trong game (ten vat pham, ten ban do, noi dung
nhat ky) deu la TCVN3 -- ma mot byte. Doc thang bang latin-1 se ra chu vo nghia
kieu "L.nh b.i ho.n th.nh D. T.u". Bang chuyen doi lay VERBATIM tu
    <skill>/scripts/vn_to_octal.py  (_TCVN3_CHARS / _UNICODE_CHARS)
la ban do chinh thong cua nguoi van hanh (usvolam.com item_index.py).

Dung de SINH THEM cot doc duoc trong MySQL; cot goc van giu VARBINARY nguyen byte
nen khong mat mat gi.
"""
import os
import sys

_SKILL = os.path.join(os.path.expanduser('~'), '.claude', 'skills',
                      'swordonline-dev', 'scripts')


def _nap_bang():
    """Nap bang tu vn_to_octal.py cua skill. Neu khong co thi tra bang rong."""
    if _SKILL not in sys.path:
        sys.path.insert(0, _SKILL)
    try:
        import vn_to_octal as v
    except Exception:
        return {}
    chars = getattr(v, '_TCVN3_CHARS', None)
    uni = getattr(v, '_UNICODE_CHARS', None)
    if not chars or not uni:
        return {}
    # _TCVN3_CHARS[i] la ky tu latin-1 (byte TCVN3), _UNICODE_CHARS[i] la chu Viet
    return dict((ord(c), u) for c, u in zip(chars, uni) if u != ' ')


_BANG = _nap_bang()


def sang_unicode(b):
    """bytes TCVN3 -> str Unicode. Byte ASCII giu nguyen; byte cao khong co trong
    bang thi giu nguyen dang latin-1 (khong nem loi, khong mat ky tu)."""
    if b is None:
        return None
    if isinstance(b, str):
        b = b.encode('latin-1', 'replace')
    ra = []
    for x in b:
        if x < 0x80:
            ra.append(chr(x))
        else:
            ra.append(_BANG.get(x, chr(x)))
    return ''.join(ra)


def co_bang():
    return len(_BANG) > 0


if __name__ == '__main__':
    print('so byte trong bang TCVN3: %d' % len(_BANG))
    thu = [
        b'L\xcanh b\xb5i ho\xb5n th\xb5nh D\xb7 T\xe8u',
        b'T\xb5i kho\xb6n',
        b'Ph\xf3c Duy\xaan L\xe9 (\xa7\xe3i)',
        b'Ba L\xb6ng huy\xd6n',
    ]
    for t in thu:
        out = sang_unicode(t)
        try:
            print('  %-42r -> %s' % (t, out))
        except UnicodeEncodeError:
            print('  %-42r -> %s' % (t, out.encode('utf-8')))
