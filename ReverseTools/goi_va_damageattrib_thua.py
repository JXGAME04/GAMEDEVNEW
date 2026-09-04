# -*- coding: utf-8 -*-
# [SPARSE 31/08] GOC THAT cua "bot Duong Mon di toi quai roi dung yen":
# m_DamageAttribs la MANG THUA 17 o, danh chi so theo LOAI don (KSkills.cpp:2590-2700):
#   [0]attackrating [1]ignoredefense [2]MAGIC [3]seriesdamage [4]deadlystrike
#   [5]fatallystrike [6]steallife [7]stealmana [8]stealstamina [9]PHYSICS
#   [10]COLD [11]FIRE [12]LIGHTING [13]POISON [14]stun [16]randmove
# con GetDamageAttribsNum() chi la BO DEM so o da dien - KHONG phai do dai mang goi.
# Ba vong quet trong KPlayerBot.cpp deu viet "for (a = 0; a < nDaNum; a++)" nen chi doc
# vai o DAU (thuong rong) => moi phep thu "chieu co don phep" LUON = 0:
#   - bNoi luon 0  -> co che "nua dan thien noi cong" (23/08) nam im tu dau;
#   - khoi DOCTHUAN (30/08) gate tren bNoi -> KHONG BAO GIO chay -> chieu 303 doc-thuan
#     van duoc chon (do that 31/08 sau restart 11:05: 833/1088 cu "khong sut mau" la 303,
#     "dung chieu 45" = 0, "DON DANH THUONG" = 0);
#   - pb_CoChieuNoiTayKhong luon tra 0 -> duong noi/ngoai (28/08) cung nam im.
# Mang KHONG duoc memset => phai so DUNG gia tri enum (khong the chi kiem != 0).
# Va: 3 phep thu doc theo O + so enum chinh xac. Chi KPlayerBot.cpp (server-only).
import io, sys
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.cpp"

s = io.open(P, "r", encoding="latin-1", newline="").read()
truoc = sum(1 for c in s if ord(c) > 127)
crlf = "\r\n" in s
n = 0

def ap(ten, cu, moi):
    global s, n
    if crlf:
        cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
    if moi in s:
        print("  [=] %s da ap tu truoc" % ten); return
    if s.count(cu) != 1:
        print("LOI: neo %s khop %d cho (can 1)" % (ten, s.count(cu))); sys.exit(1)
    s = s.replace(cu, moi); n += 1
    print("  [+] %s" % ten)

# ---- H1: 3 phep thu dung, dat truoc pb_CoChieuNoiTayKhong ----
ap("H1 helper doc mang thua",
 'static int pb_CoChieuNoiTayKhong(int nNpcIdx)\n',
 '// [SPARSE 31/08] m_DamageAttribs la MANG THUA 17 o danh chi so theo LOAI don\n'
 '// (KSkills.cpp: [2]magic [9]physics [10]cold [11]fire [12]lighting [13]poison),\n'
 '// GetDamageAttribsNum() chi la BO DEM. Quet "a < nDaNum" (ban cu) chi doc vai o DAU\n'
 '// thuong rong => moi phep thu don-phep luon 0. Mang KHONG duoc memset nen phai so\n'
 '// DUNG enum chu khong the chi kiem khac 0.\n'
 '#define PB_DA_LA(p, slot, val)  ((p)->GetDamageAttribs()[(slot)].nAttribType == (val))\n'
 '\n'
 '// don sat thuong PHEP THAT (KHONG tinh doc: do that 199k cu cho thay doc khong bao\n'
 '// mon quai tren build nay - xem 303-DOC)\n'
 'static int pb_DonPhepThat(KSkill* p)\n'
 '{\n'
 '\treturn PB_DA_LA(p, 2,  magic_magicdamage_v)\n'
 '\t    || PB_DA_LA(p, 10, magic_colddamage_v)\n'
 '\t    || PB_DA_LA(p, 11, magic_firedamage_v)\n'
 '\t    || PB_DA_LA(p, 12, magic_lightingdamage_v);\n'
 '}\n'
 '\n'
 'static int pb_DonDoc(KSkill* p)\n'
 '{\n'
 '\treturn PB_DA_LA(p, 13, magic_poisondamage_v);\n'
 '}\n'
 '\n'
 '// o [9] dung chung cho physicsdamage_v va physicsenhance_p (KSkills.cpp cung mot case)\n'
 'static int pb_DonVatLy(KSkill* p)\n'
 '{\n'
 '\treturn PB_DA_LA(p, 9, magic_physicsdamage_v) || PB_DA_LA(p, 9, magic_physicsenhance_p);\n'
 '}\n'
 '\n'
 'static int pb_CoChieuNoiTayKhong(int nNpcIdx)\n')

# ---- H2: vong 1 trong pb_CoChieuNoiTayKhong ----
ap("H2 sua vong pb_CoChieuNoiTayKhong",
 '\t\tKMagicAttrib* pDa = p->GetDamageAttribs();\n'
 '\t\tconst int nDaNum = p->GetDamageAttribsNum();\n'
 '\t\tfor (int a = 0; a < nDaNum && a < MAX_MISSLE_DAMAGEATTRIB; a++)\n'
 '\t\t{\n'
 '\t\t\tconst int nT = pDa[a].nAttribType;\n'
 '\t\t\t// [303-DOC 30/08] BO poisondamage: doc khong bao mon quai tren build nay\n'
 '\t\t\t// (199k cu do that) - chieu doc-thuan khong du tu cach "chieu noi danh duoc",\n'
 '\t\t\t// tranh DM bi tuoc vu khi oan roi om 303 vo dung.\n'
 '\t\t\tif (nT == magic_magicdamage_v  || nT == magic_colddamage_v\n'
 '\t\t\t || nT == magic_firedamage_v   || nT == magic_lightingdamage_v)\n'
 '\t\t\t\treturn 1;\n'
 '\t\t}\n',
 '\t\t// [SPARSE 31/08] doc theo O (ban cu quet "a < nDaNum" nen luon tra 0 -> duong\n'
 '\t\t// noi/ngoai nam im tu 28/08). Doc bi loai theo 303-DOC.\n'
 '\t\tif (pb_DonPhepThat(p))\n'
 '\t\t\treturn 1;\n')

# ---- H3: vong 2 (bNoi) trong pb_PickSkill ----
ap("H3 sua bNoi",
 '\t\tint bNoi = 0;\n'
 '\t\t{\n'
 '\t\t\tKMagicAttrib* pDa = p->GetDamageAttribs();\n'
 '\t\t\tconst int nDaNum = p->GetDamageAttribsNum();\n'
 '\t\t\tfor (int a = 0; a < nDaNum && a < MAX_MISSLE_DAMAGEATTRIB; a++)\n'
 '\t\t\t{\n'
 '\t\t\t\tconst int nT = pDa[a].nAttribType;\n'
 '\t\t\t\tif (nT == magic_magicdamage_v  || nT == magic_colddamage_v\n'
 '\t\t\t\t || nT == magic_firedamage_v   || nT == magic_lightingdamage_v\n'
 '\t\t\t\t || nT == magic_poisondamage_v)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tbNoi = 1;\n'
 '\t\t\t\t\tbreak;\n'
 '\t\t\t\t}\n'
 '\t\t\t}\n'
 '\t\t}\n',
 '\t\t// [SPARSE 31/08] doc theo O; BO doc khoi "don phep" (doc = 0 sat thuong voi quai)\n'
 '\t\t// nen chieu doc khong con duoc uu tien lam "chieu noi cong".\n'
 '\t\tconst int bNoi = pb_DonPhepThat(p);\n')

# ---- H4: khoi DOCTHUAN doc lap khoi bNoi ----
ap("H4 DOCTHUAN doc lap",
 '\t\tif (bNoi && !p->IsPhysical())\n'
 '\t\t{\n'
 '\t\t\tint bDamKhacDoc = 0;\n'
 '\t\t\tKMagicAttrib* pDa3 = p->GetDamageAttribs();\n'
 '\t\t\tconst int nDaNum3 = p->GetDamageAttribsNum();\n'
 '\t\t\tfor (int a3 = 0; a3 < nDaNum3 && a3 < MAX_MISSLE_DAMAGEATTRIB; a3++)\n'
 '\t\t\t\tif (pDa3[a3].nAttribType != magic_poisondamage_v)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tbDamKhacDoc = 1;\n'
 '\t\t\t\t\tbreak;\n'
 '\t\t\t\t}\n'
 '\t\t\tif (!bDamKhacDoc)\n'
 '\t\t\t{ PB_DIAG(" %d:DOCTHUAN", id); continue; }\n'
 '\t\t}\n',
 '\t\t// [SPARSE 31/08] doc lap khoi bNoi: chieu KHONG an theo vu khi (IsPhysical=0)\n'
 '\t\t// ma don sat thuong DUY NHAT la doc -> vo dung voi quai (303 Doc Thach Cot).\n'
 '\t\t// 303 con co seriesdamage_p (o [3]) nen phai so theo O DON THAT, khong the\n'
 '\t\t// "co attrib khac doc" nhu ban cu.\n'
 '\t\tif (!p->IsPhysical() && pb_DonDoc(p)\n'
 '\t\t && !pb_DonPhepThat(p) && !pb_DonVatLy(p))\n'
 '\t\t{ PB_DIAG(" %d:DOCTHUAN", id); continue; }\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
