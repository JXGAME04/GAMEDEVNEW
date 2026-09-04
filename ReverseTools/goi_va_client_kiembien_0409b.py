# -*- coding: utf-8 -*-
"""[BIEN 04/09 b] Vá CA HO thay vi tung ham: 18 cho trong KProtocolProcess.cpp (client) chep du lieu goi bang cong thuc
   memcpy(dich, nguon, pX->m_wLength + 1 + sizeof(truong) - sizeof(struct))
tuc do dai LAY TU GOI. Mot goi hong la ra so khong lo -> ghi tran mang tren ngan xep -> sap.
Da thay 3 lan sap khac nhau roi vao 3 ham khac nhau cua ho nay (GetChat, ReceiveInvite, s2cChatBeRefusedAddFriend),
nen vá tung ham la duoi bat. Nay thay het bang mot ham chep CO KIEM BIEN:
   BIEN_ChepAnToan(dich, sizeof(dich), nguon, so_byte, "ten cho")
   - so_byte < 0 hoac > sizeof(dich)-1  -> BO, ghi [BIEN-XAU] mot lan/giay, tra 0.
   - hop le -> chep binh thuong, luon dong ky tu ket thuc.
Khong doi hanh vi khi goi dung. Doc/ghi latin-1 newline=''."""
import io, os, re

ROOT = r"D:\GAMEDEVNEW_wt_net\Sources\Core\Src"
MARK = "[BIEN 04/09 b]"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)

p = os.path.join(ROOT, "KProtocolProcess.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK in s:
    print("da va"); raise SystemExit

# 1. ham chep an toan - dat ngay sau khoi #include (truoc ham dau tien)
helper = N.join([
    "",
    "#ifndef _SERVER",
    "// %s CHEP AN TOAN tu goi mang: cac ham xu ly goi tinh so byte bang 'm_wLength + 1 + sizeof(truong) - sizeof(struct)'," % MARK,
    "// ma m_wLength lay THANG TU GOI. Goi hong (luong lech) cho ra so khong lo -> memcpy ghi tran mang tren ngan xep -> sap.",
    "// Nhat ky sap 04/09: 00:32:40 chep 20.608 byte vao 256; 00:21:13 chep 56.196 byte vao 32; 00:39:06 chep 45.675 byte vao 64.",
    "// Ham nay bo goi khi so byte vo ly, ghi [BIEN-XAU] mot lan/giay. Goi dung thi hanh vi khong doi.",
    "static int BIEN_ChepAnToan(void* pDich, int nCoDich, const void* pNguon, int nSoByte, const char* szCho)",
    "{",
    "\tif (!pDich || nCoDich <= 0)",
    "\t\treturn 0;",
    "\tif (!pNguon || nSoByte < 0 || nSoByte > nCoDich - 1)",
    "\t{",
    "\t\tAUTOLOG_EVERY(1000, \"[BIEN-XAU] %s: goi doi chep %d byte vao cho %d byte - BO GOI\", szCho ? szCho : \"?\", nSoByte, nCoDich);",
    "\t\t((char*)pDich)[0] = 0;",
    "\t\treturn 0;",
    "\t}",
    "\tmemcpy(pDich, pNguon, nSoByte);",
    "\t((char*)pDich)[nSoByte] = 0;",
    "\treturn nSoByte;",
    "}",
    "#endif",
    "",
])
m = re.search(r"(#include[^\r\n]*\r?\n)(?![^\r\n]*#include)", s)
i = m.end()
s = s[:i] + helper + s[i:]

# 2. thay 18 cho memcpy dung m_wLength
pat = re.compile(r"memcpy\(\s*([^,]+?),\s*(.+?),\s*([^;]*?m_wLength[^;]*?)\);", re.S)
n = 0
def thay(mm):
    global n
    dich, nguon, sl = mm.group(1).strip(), mm.group(2).strip(), mm.group(3).strip()
    n += 1
    return 'BIEN_ChepAnToan(%s, (int)sizeof(%s), %s, (int)(%s), "%s");' % (dich, dich, nguon, sl, dich.replace('"', ''))
s2 = pat.sub(thay, s)
assert n >= 15, "chi thay duoc %d cho (mong doi >= 15)" % n
wr(p, s2)
print("KProtocolProcess.cpp OK - thay %d cho, them ham BIEN_ChepAnToan" % n)
