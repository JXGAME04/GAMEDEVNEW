# -*- coding: utf-8 -*-
# [BOTNOI dot 4 06/09] Chu game (anh chup Kim Co Bong cap 10 co 7 dong): "vu khi add cho bot co
# 7 dong thuoc tinh trong khi do xanh mac dinh nhieu nhat 6 dong thuoc tinh".
# GOC: MAX_ITEM_MAGICATTRIB cua du an = 8 (GameDataDef.h:40), khong phai 6 nhu toi tuong;
# pb_MagicVuKhi dot 3 lap `i < MAX_ITEM_MAGICATTRIB` -> dien 8 o -> Gen_MagicAttrib (ban 7 tham
# so, lap toi MAX_ITEM_MAGICATTRIB) sinh toi 8 dong (anh chup: 7 dong, mot o khong tim duoc dong
# khac loai). "tong 45" trong log = GetTotalMagicLevel chi cong 6 o dau (KItem.cpp:3961 for i<6).
# Ngoai ra g_Random(2) tra 0,1,0,1... (bit thap cua LCG) -> moi vu khi deu 7,8,7,8,7,8 - khong
# ngau nhien; lay bit cao cua g_Random(65536) (cung meo KPlayerBot.cpp:9815).
# SUA (KPlayerBot.cpp, 2 hunk):
#   H15 pb_MagicVuKhi: DUNG 6 o (PB_VK_SO_DONG) = tran do xanh; o 7-8 = 0; ngau nhien bit cao.
#   H16 buoc 4: vu khi da lo sinh 7-8 dong (nGeneratorLevel[6] hoac [7] > 0, ban 18:53) sinh lai
#       MOT lan 6 dong (pb_VuKhiQuaDong).
# AP SAU goi_va_botnoi_dot3_0609.py. Idempotent. Chi ASCII.
import io, os, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_botnoi"
P = os.path.join(ROOT, "Sources", "Core", "Src", "KPlayerBot.cpp")

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

ap("H15 pb_MagicVuKhi dung 6 dong + ngau nhien bit cao",
 '// 06/09 truyen toan 0 = vu khi TRANG. Nay 6 o = 7 hoac 8 ngau nhien tung o; 6 o sau = 0.\n'
 '// ===========================================================================\n'
 'static void pb_MagicVuKhi(int* nMagic)\n'
 '{\n'
 '\tZeroMemory(nMagic, sizeof(int) * MAX_ITEM_MAGICLEVEL);\n'
 '\tfor (int i = 0; i < MAX_ITEM_MAGICATTRIB && i < MAX_ITEM_MAGICLEVEL; i++)\n'
 '\t\tnMagic[i] = 7 + (int)g_Random(2);\n'
 '}\n',
 '// 06/09 truyen toan 0 = vu khi TRANG. Nay 6 o = 7 hoac 8 ngau nhien tung o; o con lai = 0.\n'
 '// [VKMAGIC-6 06/09 dot 4] Chu game: "vu khi add cho bot co 7 dong thuoc tinh trong khi do\n'
 '// xanh mac dinh nhieu nhat 6 dong". MAX_ITEM_MAGICATTRIB cua du an = 8 (GameDataDef.h:40),\n'
 '// ban 18:53 lap toi do nen sinh 7-8 dong -> nay DUNG 6 o (PB_VK_SO_DONG), o 7-8 de 0 ->\n'
 '// Gen_MagicAttrib dung o o thu 7. Ngau nhien: g_Random(2) tra 0,1,0,1 (bit thap LCG - 143\n'
 '// vu khi deu "tong 45"), nen lay bit cao cua g_Random(65536) nhu meo bam o KPlayerBot.cpp.\n'
 '// ===========================================================================\n'
 '#define PB_VK_SO_DONG  6      // tran dong thuoc tinh do xanh cua du an\n'
 'static void pb_MagicVuKhi(int* nMagic)\n'
 '{\n'
 '\tZeroMemory(nMagic, sizeof(int) * MAX_ITEM_MAGICLEVEL);\n'
 '\tfor (int i = 0; i < PB_VK_SO_DONG && i < MAX_ITEM_MAGICATTRIB && i < MAX_ITEM_MAGICLEVEL; i++)\n'
 '\t\tnMagic[i] = 7 + (int)(((unsigned int)g_Random(65536) >> 8) & 1u);\n'
 '}\n'
 '\n'
 '// vu khi co dong thu 7 tro di (lo sinh boi ban 18:53 06/09) -> can sinh lai 6 dong\n'
 'static int pb_VuKhiQuaDong(int nItem)\n'
 '{\n'
 '\tif (nItem <= 0)\n'
 '\t\treturn 0;\n'
 '\tfor (int i = PB_VK_SO_DONG; i < MAX_ITEM_MAGICATTRIB && i < MAX_ITEM_MAGICLEVEL; i++)\n'
 '\t\tif (Item[nItem].m_GeneratorParam.nGeneratorLevel[i] > 0)\n'
 '\t\t\treturn 1;\n'
 '\treturn 0;\n'
 '}\n')

ap("H16 buoc 4: sinh lai vu khi qua 6 dong",
 '\t\tif (nW > 0 && Item[nW].GetGoldId() == 0\n'
 '\t\t && (Item[nW].GetLevel() < 10 || Item[nW].GetTotalMagicLevel() <= 0))\n'
 '\t\t{\n',
 '\t\t// [VKMAGIC-6 dot 4] vu khi lo sinh 7-8 dong (ban 18:53) cung sinh lai MOT lan 6 dong.\n'
 '\t\tif (nW > 0 && Item[nW].GetGoldId() == 0\n'
 '\t\t && (Item[nW].GetLevel() < 10 || Item[nW].GetTotalMagicLevel() <= 0\n'
 '\t\t  || pb_VuKhiQuaDong(nW)))\n'
 '\t\t{\n')

if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("da ap %d hunk; high-byte truoc=%d sau=%d %s" % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
