# -*- coding: utf-8 -*-
"""C36 - chu bao thong bao PLD hien "con10 phut thuyen roi ben" (dinh chu).
Doi chieu: dong do GIONG HET ban Linux tung byte - loi co san cua ban goc (dich
tu tieng Trung, tieng Trung khong can dau cach). Nhung tieng Viet doc rat xau
nen chen 1 DAU CACH tai cho noi chuoi.
CHI sua cac lenh HIEN THI cho nguoi choi (Msg2Player / Msg2MSAll / Say / broadcast
/ Talk / PutMessage) va CHI o cho noi voi SO hoac TEN; KHONG dung toi:
  - WriteLog (log ky thuat)
  - noi voi "\\n" hoac the <...> (khong phai chu)
  - DescLink_* (link anh + chu, ban goc co y viet lien)
Moi cho sua deu in ra de doi chieu.
"""
import io, os, re, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

# (tep, dong 1-based) da soi tay - chi cac cho chen SO vao giua chu
MUC = [
    (r"script\missions\fengling_ferry\fld_head.lua", 142),
    (r"script\missions\fengling_ferry\fld_smalltimer.lua", 22),
    (r"script\missions\fengling_ferry\fld_smalltimer.lua", 27),
    (r"script\missions\fengling_ferry\fld_smalltimer.lua", 32),
    (r"script\missions\challengeoftime\award.lua", 207),
    (r"script\missions\challengeoftime\npc.lua", 486),
    (r"script\missions\challengeoftime\npc\dragonboat_main.lua", 113),
    (r"script\missions\challengeoftime\npc\dragonboat_main.lua", 116),
    (r"script\missions\challengeoftime\npc\dragonboat_main.lua", 120),
]

LENH = ("Msg2Player", "Msg2MSAll", "Say", "broadcast", "Talk", "PutMessage", "SayEx")


def co_lenh(s):
    return any(re.search(r"\b" + x + r"\s*\(", s) for x in LENH)


def them_space(line):
    """chen dau cach o 2 khuon: 'chu"..' -> 'chu "..'  va  '.."chu' -> '.." chu'"""
    out = line
    # ve TRUOC dau noi: ky tu cuoi cua chuoi la CHU
    out = re.sub(r'([A-Za-z\xa1-\xfe])"(\s*\.\.)', lambda m: m.group(1) + ' "' + m.group(2), out)
    # ve SAU dau noi: ky tu dau cua chuoi la CHU (bo qua \n va the <)
    out = re.sub(r'(\.\.\s*)"([A-Za-z\xa1-\xfe])', lambda m: m.group(1) + '" ' + m.group(2), out)
    return out


theotep = {}
for (rel, dong) in MUC:
    theotep.setdefault(rel, []).append(dong)

tong = 0
for rel, dongs in theotep.items():
    p = os.path.join(JX1, rel)
    d = io.open(p, encoding="latin-1", newline="").read()
    lines = d.split("\n")
    sua = 0
    for dong in dongs:
        i = dong - 1
        if i >= len(lines):
            print("  !! %s khong co dong %d" % (rel, dong))
            continue
        cu = lines[i]
        if not co_lenh(cu):
            print("  (bo qua - khong phai lenh hien thi) %s:%d" % (rel, dong))
            continue
        moi = them_space(cu)
        if moi != cu:
            lines[i] = moi
            sua += 1
            print("  %s:%d" % (rel.rsplit("\\", 1)[-1], dong))
            print("     cu : %s" % cu.strip()[:78])
            print("     moi: %s" % moi.strip()[:78])
    if sua:
        io.open(p, "w", encoding="latin-1", newline="").write("\n".join(lines))
        dst = os.path.join(MIR, rel)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copyfile(p, dst)
        tong += sua
print("\nda chen dau cach o %d cho (dong bo guong)" % tong)
