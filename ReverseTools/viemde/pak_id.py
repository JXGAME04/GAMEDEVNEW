# -*- coding: utf-8 -*-
"""KPakList::FileNameToId - dich DUNG tung dong C.

BAY DA VAP: `const char *ptr` tren MSVC la CHAR CO DAU. Byte GBK 0xB2
vao phep nhan la -78 chu khong phai 178. Dung so KHONG dau thi moi
duong dan chua chu Trung deu bam ra sai -> 0/714 tep GBK co that
tren dia "khong thay" trong pak.
"""
SEP = "\\"


def pack_path(rel):
    """g_GetPackPath: bo dau \ dau, g_StrLower (chi ha A-Z, char co dau
    nen byte >=0x80 giu nguyen), roi KPakList them lai mot dau \."""
    n = rel[1:] if rel[:1] in (SEP, "/") else rel
    b = (SEP + n).encode("latin-1")
    return bytes(x + 32 if 65 <= x <= 90 else x for x in b)


def file_name_to_id(rel):
    pb = pack_path(rel)
    uid = 0
    index = 0
    for x in pb:
        index += 1
        c = x - 256 if x >= 128 else x        # <-- char CO DAU
        if 65 <= x <= 90:                     # 'A'..'Z' -> +('a'-'A')
            c = x + 32
        # id + index*c : int am doi sang unsigned long = wrap 2^32
        uid = (((uid + index * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF & 0xFFFFFFFF
    return uid ^ 0x12345678
