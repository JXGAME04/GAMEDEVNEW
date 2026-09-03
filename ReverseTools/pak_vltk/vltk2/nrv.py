# -*- coding: utf-8 -*-
"""Bo giai nen UCL NRV2B / NRV2D / NRV2E (bien the 8-bit va LE32) — Python thuan.
Chep theo ucl n2b_d.c / n2d_d.c / n2e_d.c + getbit.h.
`limit`: dung som khi output >= limit (de 'nhin dau tep')."""
import struct


class _Bits8:
    __slots__ = ("src", "ilen", "bb")

    def __init__(self, src):
        self.src = src
        self.ilen = 0
        self.bb = 0

    def getbit(self):
        bb = self.bb
        if bb & 0x7F:
            bb = bb * 2
        else:
            bb = self.src[self.ilen] * 2 + 1
            self.ilen += 1
        self.bb = bb
        return (bb >> 8) & 1

    def getbyte(self):
        b = self.src[self.ilen]
        self.ilen += 1
        return b


class _BitsLE32:
    __slots__ = ("src", "ilen", "bb", "bc")

    def __init__(self, src):
        self.src = src
        self.ilen = 0
        self.bb = 0
        self.bc = 0

    def getbit(self):
        if self.bc > 0:
            self.bc -= 1
            return (self.bb >> self.bc) & 1
        self.bc = 31
        self.bb = struct.unpack_from("<I", self.src, self.ilen)[0]
        self.ilen += 4
        return (self.bb >> 31) & 1

    def getbyte(self):
        b = self.src[self.ilen]
        self.ilen += 1
        return b


def _copy(dst, m_off, n):
    # chep n byte tu dst[-m_off:] (co the chong lan)
    pos = len(dst) - m_off
    if m_off >= n:
        dst += dst[pos:pos + n]
    else:
        for _ in range(n):
            dst.append(dst[pos])
            pos += 1


def nrv2b(src, bits, limit=None):
    getbit = bits.getbit
    dst = bytearray()
    last_m_off = 1
    while True:
        while getbit():
            dst.append(bits.getbyte())
            if limit and len(dst) >= limit:
                return bytes(dst)
        m_off = 1
        while True:
            m_off = m_off * 2 + getbit()
            if getbit():
                break
            if m_off > 0xFFFFFF + 3:
                raise ValueError("m_off overrun")
        if m_off == 2:
            m_off = last_m_off
        else:
            m_off = (m_off - 3) * 256 + bits.getbyte()
            if m_off == 0xFFFFFFFF:
                break
            m_off += 1
            last_m_off = m_off
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
            raise ValueError("lookbehind overrun")
        _copy(dst, m_off, m_len + 1)
        if limit and len(dst) >= limit:
            return bytes(dst)
    return bytes(dst)


def nrv2d(src, bits, limit=None):
    getbit = bits.getbit
    dst = bytearray()
    last_m_off = 1
    while True:
        while getbit():
            dst.append(bits.getbyte())
            if limit and len(dst) >= limit:
                return bytes(dst)
        m_off = 1
        while True:
            m_off = m_off * 2 + getbit()
            if getbit():
                break
            m_off = (m_off - 1) * 2 + getbit()
            if m_off > 0xFFFFFF + 3:
                raise ValueError("m_off overrun")
        if m_off == 2:
            m_off = last_m_off
            m_len = getbit()
        else:
            m_off = (m_off - 3) * 256 + bits.getbyte()
            if m_off == 0xFFFFFFFF:
                break
            m_len = (m_off ^ 0xFFFFFFFF) & 1
            m_off >>= 1
            m_off += 1
            last_m_off = m_off
        m_len = m_len * 2 + getbit()
        if m_len == 0:
            m_len = 1
            while True:
                m_len = m_len * 2 + getbit()
                if getbit():
                    break
            m_len += 2
        m_len += 1 if m_off > 0x500 else 0
        if m_off > len(dst):
            raise ValueError("lookbehind overrun")
        _copy(dst, m_off, m_len + 1)
        if limit and len(dst) >= limit:
            return bytes(dst)
    return bytes(dst)


def nrv2e(src, bits, limit=None):
    getbit = bits.getbit
    dst = bytearray()
    last_m_off = 1
    while True:
        while getbit():
            dst.append(bits.getbyte())
            if limit and len(dst) >= limit:
                return bytes(dst)
        m_off = 1
        while True:
            m_off = m_off * 2 + getbit()
            if getbit():
                break
            m_off = (m_off - 1) * 2 + getbit()
            if m_off > 0xFFFFFF + 3:
                raise ValueError("m_off overrun")
        if m_off == 2:
            m_off = last_m_off
            m_len = getbit()
        else:
            m_off = (m_off - 3) * 256 + bits.getbyte()
            if m_off == 0xFFFFFFFF:
                break
            m_len = (m_off ^ 0xFFFFFFFF) & 1
            m_off >>= 1
            m_off += 1
            last_m_off = m_off
        if m_len:
            m_len = 1 + getbit()
        elif getbit():
            m_len = 3 + getbit()
        else:
            m_len = 1
            while True:
                m_len = m_len * 2 + getbit()
                if getbit():
                    break
            m_len += 3
        m_len += 1 if m_off > 0x500 else 0
        if m_off > len(dst):
            raise ValueError("lookbehind overrun")
        _copy(dst, m_off, m_len + 1)
        if limit and len(dst) >= limit:
            return bytes(dst)
    return bytes(dst)


def nrv2b_8(src, limit=None):
    return nrv2b(src, _Bits8(src), limit)


def nrv2b_le32(src, limit=None):
    return nrv2b(src, _BitsLE32(src), limit)


def nrv2d_le32(src, limit=None):
    return nrv2d(src, _BitsLE32(src), limit)


def nrv2e_le32(src, limit=None):
    return nrv2e(src, _BitsLE32(src), limit)


# UPX method id -> ham
UPX_METHOD = {2: nrv2b_le32, 5: nrv2d_le32, 8: nrv2e_le32}
