# -*- coding: utf-8 -*-
#
# [DPG 12/09 RONG] Man 4:3 (may tinh bang, iPad): he so theo chieu cao 640 cho khung ve HEP hon 1040 (1440x1080 -> 1,75 -> 823x617)
# trong khi bo cuc PC cua chu thiet ke tren 1040x604 -> icon chong len nhau. Ha he so cho toi khi khung ve rong >= [Resolution]
# RongToiThieu (mac dinh 1040 = ManHinh cua Ui\UiToaDo_MacDinh.ini); dien thoai 2400x1080 -> 1371 khong doi.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[DPG 12/09 RONG]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, nl.join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


P = "Sources/S3Client/Platform/KSdlApp.cpp"
s = doc(P)
if DAU not in s:
    s0 = s
    s = thay(s, ["\t// Giao dien JX1 can it nhat 800x480 moi bay du (thanh ky nang, tui do, cua so chat) -> ha he so cho du cho.",
                 "\twhile (nHeSo > 100 && (nW * 100 / nHeSo < 800 || nH * 100 / nHeSo < 480))",
                 "\t\tnHeSo -= 25;"],
                ["\t// Giao dien JX1 can it nhat 800x480 moi bay du (thanh ky nang, tui do, cua so chat) -> ha he so cho du cho.",
                 "\t// %s bo cuc PC cua chu thiet ke tren 1040x604 (ManHinh cua Ui\\UiToaDo_MacDinh.ini): man 4:3 (may tinh bang)" % DAU,
                 "\t// ma khung ve hep hon 1040 thi icon chong nhau -> ha he so toi khi rong >= RongToiThieu (1440x1080: 1,75 -> 1,25 = 1152x864).",
                 "\tint nRongToiThieu = GetPrivateProfileInt(\"Resolution\", \"RongToiThieu\", 1040, szCfg);",
                 "\tif (nRongToiThieu < 800) nRongToiThieu = 800;",
                 "\twhile (nHeSo > 100 && (nW * 100 / nHeSo < nRongToiThieu || nH * 100 / nHeSo < 480))",
                 "\t\tnHeSo -= 25;"], "rong toi thieu")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi")
    ghi(P, s); print("da va:", P)
print("xong")
