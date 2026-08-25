# -*- coding: utf-8 -*-
"""C27 - va 2 loi that ma bo soat C26 tim ra:
 (1) nieshichen.lua aboutchallenge(): goi Describe = 92(DescLink)+891(ContentList[28])
     +11 = 994B, VO TRAN 511B cua JX1 => bi cat, nguoi choi doc thieu. Ban Linux
     hien duoc vi engine JX2 khong co tran nay.
     Va: CHIA TRANG theo dau <enter> co san trong chinh chuoi goc (khong doi mot
     chu nao cua noi dung), them 3 muc MOI vao CUOI ContentList (giu nguyen chi so
     cac muc cu - chen giua se lam lech moi loi goi ContentList[i] trong tep).
 (2) dragonboat_main.lua:120: the dong </color> dung ngay sau chu co dau ("gio<color>")
     => TEncodeText (Text.cpp:468) coi byte cao la chu Han 2 byte va NUOT dau '<'
     => man hinh hien "<co lor>". Va: chen MOT dau cach truoc the (luat chung cua
     engine JX1; lech 1 byte so Linux nhung la loi hien thi that cua JX1).
"""
import io, os, re, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
T = "\t"
BUDGET = 380          # 511 - DescLink 92 - nut Tiep ~25 - nut Dong 11 - du phong


def sync(rel):
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(os.path.join(JX1, rel), dst)


# ---------------- (1) nieshichen: chia trang ----------------
rel = r"script\task\tollgate\killer\nieshichen.lua"
p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in d else "\n"

if "aboutchallenge2" in d:
    print("(1) da chia trang roi")
else:
    # lay ContentList[28] nguyen van
    m = re.search(r"ContentList\s*=\s*\{(.*?)\n\}", d, re.S)
    dong = [l for l in m.group(1).split("\n") if l.strip().startswith('"')]
    goc = dong[27].strip()                       # muc 28 (1-based)
    j = goc.rfind('"')
    noi_dung = goc[1:j]
    assert noi_dung.startswith("<#>"), noi_dung[:20]
    than = noi_dung[3:]                          # bo "<#>"
    print("(1) ContentList[28] =", len(noi_dung.encode("latin-1")), "B;",
          "so muc hien co:", len(dong))
    # chia theo <enter> co san, gom lai sao cho moi trang <= BUDGET
    doan = than.split("<enter>")
    trang, cur = [], ""
    for x in doan:
        thu = (cur + "<enter>" + x) if cur else x
        if cur and len(thu.encode("latin-1")) > BUDGET:
            trang.append(cur)
            cur = x
        else:
            cur = thu
    if cur:
        trang.append(cur)
    print("    -> chia", len(trang), "trang:", [len(t.encode("latin-1")) for t in trang])
    assert all(len(t.encode("latin-1")) <= BUDGET for t in trang), "con trang qua dai"

    # them cac muc moi vao CUOI bang (giu nguyen chi so cu)
    n_cu = len(dong)
    them = []
    for i, t in enumerate(trang):
        them.append(T + '"<#>' + t + '",\t-- ' + str(n_cu + 1 + i) + ' [C27] trang ' + str(i + 1) + '/' + str(len(trang)))
    # chen truoc dau '}' ket thuc bang: bam vao dong cuoi cua bang
    dong_cuoi = dong[-1]
    i_cuoi = d.index(dong_cuoi) + len(dong_cuoi)
    d = d[:i_cuoi] + NL + NL.join(them) + d[i_cuoi:]

    # thay aboutchallenge() bang chuoi trang
    cu = (T + "Describe(DescLink_NieShiChen..ContentList[28],1, ContentList[15]);")
    assert d.count(cu) == 1, d.count(cu)
    ham = []
    for i in range(len(trang)):
        idx = n_cu + 1 + i
        if i == 0:
            ham.append(T + "-- [C27] goi 994B vo tran 511B cua JX1 => chia %d trang, chu giu NGUYEN VAN." % len(trang))
        if i < len(trang) - 1:
            ham.append(T + 'Describe(DescLink_NieShiChen..ContentList[%d], 2, "<#>%s/#aboutchallenge%d", ContentList[15]);'
                       % (idx, V("Tiếp"), i + 2))
        else:
            ham.append(T + "Describe(DescLink_NieShiChen..ContentList[%d], 1, ContentList[15]);" % idx)
        if i < len(trang) - 1:
            ham.append("end")
            ham.append("")
            ham.append("function aboutchallenge%d()" % (i + 2))
    d = d.replace(cu, NL.join(ham))
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    sync(rel)
    print("    da ghi + dong bo guong")


# ---------------- (2) dragonboat_main: the <color> ----------------
rel2 = r"script\missions\challengeoftime\npc\dragonboat_main.lua"
p2 = os.path.join(JX1, rel2)
d2 = io.open(p2, encoding="latin-1", newline="").read()
lines = d2.split("\n")
sua = 0
for i, line in enumerate(lines):
    if line.lstrip().startswith("--"):
        continue
    out = ""
    k = 0
    while k < len(line):
        if line.startswith("<color", k) and k > 0 and ord(line[k - 1]) > 0x80:
            out += " "
            sua += 1
        out += line[k]
        k += 1
    lines[i] = out
if sua:
    io.open(p2, "w", encoding="latin-1", newline="").write("\n".join(lines))
    sync(rel2)
    print("(2) da chen dau cach truoc %d the <color> dinh chu co dau + dong bo guong" % sua)
else:
    print("(2) khong con the <color> dinh chu")
