# -*- coding: utf-8 -*-
"""goi_va_nap3_khoa_pak_chung_0809.py - [NAP 08/09 c] NEN DEN NUA MAN HINH sau khi bat nap luong nen: XPackFile giu cache phan tu
TINH DUNG CHUNG cho moi pak (ms_ElemFileCache[MAX_XPACKFILE_CACHE]) nhung khoa lai la khoa RIENG tung pak (m_ReadCritical) -> luong nen
doc pak sprite va luong ve doc pak ban do (anh toan cuc get_jpg_image) cung luc: AddElemFileToCache/FreeElemCache cua pak nay giai phong
/ghi de o cache ma pak kia dang memcpy -> anh ban do doc rac -> nua den. Sua: mot khoa TINH dung chung cho moi XPackFile (ms_ReadCritical,
khoi tao mot lan bang InterlockedCompareExchange, khong xoa); giu nguyen thanh vien m_ReadCritical (khong doi bo cuc lop).
Tep: Engine/Src/XPackFile.h, XPackFile.cpp (latin-1 CRLF, chu thich GBK giu nguyen)."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Engine\Src"
def load(name):
    p = ROOT + "\\" + name
    return p, io.open(p, "r", encoding="latin-1", newline="").read()
def save(p, s, h0):
    if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    if re.search(r"[^\r]\n", s): print("FAIL LF", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:90], c, n)); sys.exit(1)
    return s.replace(old, new)
def hi(s): return sum(1 for ch in s if ord(ch) >= 0x80)

p, s = load("XPackFile.h"); h0 = hi(s)
if "ms_ReadCritical" not in s:
    s = rep(s, "\tstatic\tint\t\t\t\t\tms_nNumElemFileCache;\r\n",
        "\tstatic\tint\t\t\t\t\tms_nNumElemFileCache;\r\n"
        "\t// [NAP 08/09 c] khoa TINH dung chung cho moi pak: cache phan tu ms_ElemFileCache la tinh dung chung nen khoa rieng tung pak\r\n"
        "\t// (m_ReadCritical) KHONG bao ve duoc khi hai luong doc hai pak khac nhau (luong nen nap sprite + luong ve nap anh ban do).\r\n"
        "\tstatic\tCRITICAL_SECTION\tms_ReadCritical;\r\n"
        "\tstatic\tlong\t\t\t\tms_lReadCriticalInit;\r\n")
save(p, s, h0); print("OK XPackFile.h")

p, s = load("XPackFile.cpp"); h0 = hi(s)
if "ms_ReadCritical" not in s:
    s = rep(s, "int\t\t\t\t\t\t\t\tXPackFile::ms_nNumElemFileCache = 0;\r\n",
        "int\t\t\t\t\t\t\t\tXPackFile::ms_nNumElemFileCache = 0;\r\n"
        "CRITICAL_SECTION\t\t\t\tXPackFile::ms_ReadCritical;\t\t// [NAP 08/09 c] khoa chung moi pak\r\n"
        "long\t\t\t\t\t\t\tXPackFile::ms_lReadCriticalInit = 0;\r\n")
    s = rep(s, "\tInitializeCriticalSection(&m_ReadCritical);\r\n}\r\n",
        "\tInitializeCriticalSection(&m_ReadCritical);\r\n"
        "\tif (InterlockedCompareExchange(&ms_lReadCriticalInit, 1, 0) == 0)\t// [NAP 08/09 c] khoa chung khoi tao mot lan, khong xoa\r\n"
        "\t\tInitializeCriticalSection(&ms_ReadCritical);\r\n}\r\n")
    n = s.count("EnterCriticalSection(&m_ReadCritical)") + s.count("LeaveCriticalSection(&m_ReadCritical)")
    s = s.replace("EnterCriticalSection(&m_ReadCritical)", "EnterCriticalSection(&ms_ReadCritical)").replace("LeaveCriticalSection(&m_ReadCritical)", "LeaveCriticalSection(&ms_ReadCritical)")
    print("   doi %d cho Enter/Leave sang khoa chung" % n)
    if n < 8: print("FAIL: it cho qua"); sys.exit(1)
save(p, s, h0); print("OK XPackFile.cpp")
print("XONG NAP c")
