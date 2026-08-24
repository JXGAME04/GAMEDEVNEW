# -*- coding: utf-8 -*-
r"""[24/08] VA use-after-free + double-free trong CFriendMgr::DB_LoadSomeone (FriendMgr.cpp).

GOC LOI:
    pValue = cursor->data;
    m_dbFriendRO.closeCursor(cursor);   // closeCursor() GOI free(cursor->data)!
    ... parse pValue ...                // <-- DOC VUNG DA GIAI PHONG (0xDDDDDDDD)
    delete []pValue;                    // <-- DOUBLE FREE + sai bo cap phat (malloc/free vs delete[])

Hau qua that (2 dump 24/08): AV doc 0xDDDDDDD9 trong std::basic_string::~basic_string
goi tu std::list<MEM_FRIENDRECORD>::~list; va hong heap -> _CrtIsValidHeapPointer.
Cung loi nay DA duoc va o TONGDB.CPP:256/337 hoi chuyen MySQL 20/08 - FriendMgr bi bo sot.

CACH VA (khuon TONGDB.CPP): giu cursor song trong suot qua trinh parse, closeCursor MOT LAN
o cuoi (no tu free data), bo delete[]; them chan bien theo cursor->size de ban ghi hong
khong lam chay qua duoi bo dem.
"""
import io, shutil

P = r"D:\GAMEDEVNEW\Sources\MultiServer\S3Relay\FriendMgr.cpp"
s = io.open(P, "r", encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"

if "FIX-UAF 24/08" in s:
    print("da va truoc do")
    raise SystemExit(0)

# gioi han moi thao tac TRONG than ham DB_LoadSomeone (anchor trung o DB_StoreSomeone)
_f0 = s.index("BOOL CFriendMgr::DB_LoadSomeone(")
_f1 = s.index("BOOL CFriendMgr::DB_StoreSomeone(")
assert _f0 < _f1
head, body, tail = s[:_f0], s[_f0:_f1], s[_f1:]
s = body

# ---- 1) khong dong cursor truoc khi parse ----
old1 = ("\tZCursor * cursor = m_dbFriendRO.search(dbkey.data(), dbkey.size());" + NL +
        "\tif (cursor) {" + NL +
        "\t\tpValue = cursor->data;" + NL +
        "\t\tm_dbFriendRO.closeCursor(cursor);" + NL +
        "\t}" + NL +
        "\telse" + NL +
        "\t\treturn TRUE;" + NL)
assert s.count(old1) == 1, "anchor 1 = %d" % s.count(old1)

new1 = ("\tZCursor * cursor = m_dbFriendRO.search(dbkey.data(), dbkey.size());" + NL +
        "\tif (!cursor)" + NL +
        "\t\treturn TRUE;" + NL +
        "\t// [FIX-UAF 24/08] KHONG duoc closeCursor o day: closeCursor() goi free(cursor->data)" + NL +
        "\t// (DBTable.cpp:107 va DBTable_MySQL.cpp:929) nen pValue se thanh con tro treo, doan" + NL +
        "\t// parse ben duoi doc vung DA GIAI PHONG. Cung loi da va o TONGDB.CPP:256/337." + NL +
        "\tpValue = cursor->data;" + NL +
        "\tvalsize = cursor->size;" + NL +
        "\tif (!pValue || valsize < (int)sizeof(DB_FRIENDRECORDLIST))" + NL +
        "\t{" + NL +
        "\t\tm_dbFriendRO.closeCursor(cursor);" + NL +
        "\t\treturn TRUE;" + NL +
        "\t}" + NL)
s = s.replace(old1, new1, 1)

# ---- 2) chan bien trong vong lap parse ----
old2 = ("\t\t_BASIC_STR* pName = (_BASIC_STR*)(pCursor + 1);" + NL)
assert s.count(old2) == 1, "anchor 2 = %d" % s.count(old2)
new2 = ("\t\t// [FIX-UAF 24/08] chan bien: ban ghi hong/cut khong duoc chay qua duoi bo dem" + NL +
        "\t\tif ((char*)(pCursor + 1) > pValue + valsize)" + NL +
        "\t\t\tbreak;" + NL +
        old2)
s = s.replace(old2, new2, 1)

old3 = ("\t\tchar* strName = (char*)(pName + 1);" + NL)
assert s.count(old3) == 1, "anchor 3 = %d" % s.count(old3)
new3 = ("\t\tchar* strName = (char*)(pName + 1);" + NL +
        "\t\tif (strName + pName->strlen > pValue + valsize)" + NL +
        "\t\t\tbreak;\t// [FIX-UAF 24/08]" + NL)
s = s.replace(old3, new3, 1)

old4 = ("\t\tchar* strGroup = (char*)(pGroup + 1);" + NL)
assert s.count(old4) == 1, "anchor 4 = %d" % s.count(old4)
new4 = ("\t\tchar* strGroup = (char*)(pGroup + 1);" + NL +
        "\t\tif (strGroup + pGroup->strlen > pValue + valsize)" + NL +
        "\t\t\tbreak;\t// [FIX-UAF 24/08]" + NL)
s = s.replace(old4, new4, 1)

# ---- 3) dong cursor o cuoi, BO delete[] ----
old5 = ("\tdelete []pValue;" + NL + "\treturn TRUE;" + NL)
assert s.count(old5) == 1, "anchor 5 = %d" % s.count(old5)
new5 = ("\t// [FIX-UAF 24/08] closeCursor() tu free(cursor->data); `delete []pValue` cu la" + NL +
        "\t// DOUBLE FREE + sai bo cap phat (vung nay do malloc cap, phai free) -> hong heap." + NL +
        "\tm_dbFriendRO.closeCursor(cursor);" + NL +
        "\treturn TRUE;" + NL)
s = s.replace(old5, new5, 1)

s = head + s + tail   # ghep lai ca tep

shutil.copyfile(P, P + ".truoc_fix_uaf_2408")
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("DA VA FriendMgr.cpp (backup .truoc_fix_uaf_2408)")
