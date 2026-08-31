# -*- coding: utf-8 -*-
# [303-DOC 30/08] Chu game: "bot van chua danh duoc, chi thay bot phai Duong Mon o
# cap 20 - chay log dieu tra fix cho chinh xac".
# Dieu tra (bot.log 29-30/08 + skills.txt + tangmen.lua):
#   - Bot DM cap 20 KHONG he roi vao fallback don-thuong (0 dong DON DANH THUONG sau
#     restart 16:27) vi ho VAN chon duoc mot chieu: 303 "Doc Thach Cot" - chieu DM
#     duy nhat rq=20 (eqt=-2, phys=0; lan truoc toi loc eqt>=100 nen sot no).
#   - 303 doc du lieu tu \script\skill\tangmen.lua bang duci_gu: CHI co poisondamage_v
#     (8->40) + seriesdamage_p, KHONG co don sat thuong tuc thoi; syncheck cu phap OK.
#   - Do that: 199k cu (29-30/08) + 308 cu (sau 16:27) "10 giay khong sut mau" cua 303
#     vao quai HP 600 CON NGUYEN 600 -> tren build nay DOC KHONG BAO MON QUAI.
#   => Bot DM cap 20 danh 303 ca ngay = 0 sat thuong = "khong danh duoc npc", ket cap
#      20 vinh vien. Nội-DM cap cao cung bi tuoc vu khi (vi 303 duoc dem la "chieu noi")
#      roi danh 303 vo dung not.
# Va (khong dung engine/gameplay, khong dung skills.txt):
#   H1 pb_PickSkill: chieu PHEP (IsPhysical=0) ma MOI don sat thuong deu la poison
#      -> loai han ung vien (PB_DIAG DOCTHUAN) -> DM20 roi ve fallback don-thuong 9.48;
#      chieu VAT LY mang doc phu van giu (con sat thuong vu khi).
#   H2 pb_CoChieuNoiTayKhong: bo poisondamage_v khoi danh sach don-phep-that -> DM
#      khong con bi coi la "phai co noi cong" -> bot DM le GIU VU KHI (con nao da bi
#      tuoc thi [BotVuKhi] tu phat lai nhu co che TV/DM 9.34).
# Chi KPlayerBot.cpp. AP SAU goi_va_dm20_donthuong.py.
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

# ---- H1: loai chieu phep doc-thuan khoi ung vien ----
ap("H1 loai chieu phep doc thuan",
 '\t\tconst int nNoi = bThienNoi ? bNoi : 0;\n',
 '\t\t// [303-DOC 30/08] chieu PHEP ma MOI don sat thuong deu la POISON (303 Doc\n'
 '\t\t// Thach Cot cua DM: duci_gu chi co poisondamage_v): do that 29-30/08 co\n'
 '\t\t// 199k cu "10 giay khong sut mau" vao quai HP600 con nguyen mau - doc khong\n'
 '\t\t// bao mon quai tren build nay. LOAI HAN ung vien loai nay: DM cap 20 se roi\n'
 '\t\t// ve don danh thuong (fallback DM20) va len 30 co chieu that. Chieu VAT LY\n'
 '\t\t// mang doc phu van giu (con sat thuong vu khi).\n'
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
 '\t\t}\n'
 '\t\tconst int nNoi = bThienNoi ? bNoi : 0;\n')

# ---- H2: bo poison khoi phep thu "co chieu noi tay khong" ----
ap("H2 bo poison khoi CoChieuNoi",
 '\t\tKMagicAttrib* pDa = p->GetDamageAttribs();\n'
 '\t\tconst int nDaNum = p->GetDamageAttribsNum();\n'
 '\t\tfor (int a = 0; a < nDaNum && a < MAX_MISSLE_DAMAGEATTRIB; a++)\n'
 '\t\t{\n'
 '\t\t\tconst int nT = pDa[a].nAttribType;\n'
 '\t\t\tif (nT == magic_magicdamage_v  || nT == magic_colddamage_v\n'
 '\t\t\t || nT == magic_firedamage_v   || nT == magic_lightingdamage_v\n'
 '\t\t\t || nT == magic_poisondamage_v)\n'
 '\t\t\t\treturn 1;\n'
 '\t\t}\n'
 '\t}\n'
 '\treturn 0;\n'
 '}\n'
 '\n'
 '// duong NOI THAT SU = dwID le VA phai co chieu noi tay khong dung duoc ngay bay gio\n',
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
 '\t\t}\n'
 '\t}\n'
 '\treturn 0;\n'
 '}\n'
 '\n'
 '// duong NOI THAT SU = dwID le VA phai co chieu noi tay khong dung duoc ngay bay gio\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
