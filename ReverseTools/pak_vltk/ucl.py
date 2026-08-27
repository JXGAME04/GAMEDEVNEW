# -*- coding: utf-8 -*-
"""ucl_nrv2b_decompress_8 - ban Python.

Chep NGUYEN VAN thuat toan tu chinh nguon cua du an:
  D:\\GAMEDEVNEW\\Sources\\Engine\\Src\\ucl\\n2b_d.c   (than vong lap)
  D:\\GAMEDEVNEW\\Sources\\Engine\\Src\\ucl\\getbit.h  (getbit_8)
Noi goi: Sources\\Engine\\src\\XPackFile.cpp:258 (co ca co 0x20000000 cua VNG).
"""


def nrv2b_decompress_8(src, expected=None):
    src = bytearray(src)
    slen = len(src)
    dst = bytearray()
    st = {"bb": 0, "ilen": 0}

    def getbit():
        # getbit_8(bb, src, ilen)
        bb = st["bb"]
        if bb & 0x7F:
            bb = bb * 2
        else:
            bb = src[st["ilen"]] * 2 + 1
            st["ilen"] += 1
        st["bb"] = bb
        return (bb >> 8) & 1

    last_m_off = 1
    while True:
        # while (getbit(bb)) dst[olen++] = src[ilen++];
        while getbit():
            if st["ilen"] >= slen:
                raise ValueError("input overrun (literal)")
            dst.append(src[st["ilen"]])
            st["ilen"] += 1

        # do { m_off = m_off*2 + getbit(bb); } while (!getbit(bb));
        m_off = 1
        while True:
            m_off = m_off * 2 + getbit()
            if m_off > 0xFFFFFF + 3:
                raise ValueError("lookbehind overrun (m_off)")
            if getbit():
                break

        if m_off == 2:
            m_off = last_m_off
        else:
            if st["ilen"] >= slen:
                raise ValueError("input overrun (m_off byte)")
            m_off = (m_off - 3) * 256 + src[st["ilen"]]
            st["ilen"] += 1
            if m_off == 0xFFFFFFFF:
                break
            m_off += 1
            last_m_off = m_off

        # m_len = getbit(); m_len = m_len*2 + getbit();
        m_len = getbit()
        m_len = m_len * 2 + getbit()
        if m_len == 0:
            m_len = 1
            while True:
                m_len = m_len * 2 + getbit()
                if getbit():
                    break
            m_len += 2

        m_len += 1 if m_off > 0xD00 else 0

        if m_off > len(dst):
            raise ValueError("lookbehind overrun (copy)")
        pos = len(dst) - m_off
        for _ in range(m_len + 1):
            dst.append(dst[pos])
            pos += 1

    if expected is not None and len(dst) != expected:
        raise ValueError("size mismatch: %d != %d" % (len(dst), expected))
    return bytes(dst)
