# -*- coding: utf-8 -*-
r"""Kiem chung logic DTG_FixTagAfterVn + TEncodeText tren CHUOI THAT (sau khi ghep %s).
Mo phong y het ma C vua viet, roi mo phong TEncodeText de xem the co con bi nuot khong."""

def fix_tag(b):
    """mo phong DTG_FixTagAfterVn"""
    out, i, n = bytearray(), 0, len(b)
    while i < n:
        c = b[i]
        if c > 0x80 and i + 1 < n:
            if b[i+1] == 0x3C:      # '<'
                out.append(c); out.append(0x20); i += 1
            else:
                out.append(c); out.append(b[i+1]); i += 2
        else:
            out.append(c); i += 1
    return bytes(out)

def broken_tags(b):
    """mo phong TEncodeText: tra ve so the '<' bi nuot"""
    bad, i, n = 0, 0, len(b)
    while i < n:
        c = b[i]
        if c > 0x80:
            if i + 1 < n:
                if b[i+1] == 0x3C: bad += 1
                i += 2
            else: break
        else:
            i += 1
    return bad

# chuoi THAT sau khi ghep (TCVN3), lay dung tu UiTaskGuideStr.h + gia tri thuc te
CASES = [
    ("TS_ROUTE_FMT + Thanh Do/Dai Ly",
     b"Nhi\xd6m v\xf4 T\xddn S\xf8 \xae\xd6c bi\xd6t: \xae\xdca tin t\xf5 <color=yellow>Th\xb5nh \xae\xabng<color> \xae\xd5n <color=yellow>\xae\xb9i L\xfd<color>."),
    ("TS_POINT_FMT + chua co",
     b"\xaei\xd3m t\xdbch lu\xfd T\xddn S\xf8: <color=yellow>0<color>. Danh hi\xd6u T\xddn S\xf8: <color=yellow>ch\xdaa c\xe3<color>. H\xf4m nay \xae\xb7 ho\xb5n th\xb5nh: 0 l\xa7n."),
    ("TS_ST10 (da sua)",
     b"<color=green>[\xaeang l\xb5m]<color> H\xb7y t\xd7m Xa Phu ch\xe4n <color=yellow>\xaei n\xa1i \xae\xd6c bi\xd6t l\xb5m Nhi\xd6m v\xf4 T\xddn S\xf8 <color>\xae\xd3 v\xb5o \xb7i."),
]

print("%-34s %-16s %s" % ("chuoi", "TRUOC khi va", "SAU khi va"))
ok = True
for name, b in CASES:
    a = broken_tags(b)
    c = broken_tags(fix_tag(b))
    print("%-34s %-16s %s" % (name, "%d the hong" % a, "%d the hong" % c))
    if c != 0: ok = False

print("\nKET LUAN:", "DAT - khong con the nao bi nuot" if ok else "*** VAN CON THE HONG ***")

# in thu ket qua de kiem mat thuong
print("\nVi du ket qua (byte sau khi va, doan quanh the):")
b = fix_tag(CASES[0][1])
i = b.find(b"<color>")
print("  ", b[max(0,i-14):i+9])
