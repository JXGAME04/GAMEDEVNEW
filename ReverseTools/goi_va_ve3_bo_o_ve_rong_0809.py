# -*- coding: utf-8 -*-
"""goi_va_ve3_bo_o_ve_rong_0809.py - [VE 08/09 c] Do duoc: [REP3] anh_null 0,2-4,7 TRIEU moi 30 s, gan het la TEN ANH RONG '(k0)'
= don vi ve duoc gui xuong Represent3 roi bo vi khong co anh (dong '[REP3-NAP] ve/khung ... khac 323-923' chinh la chung).
Nguon: KNpcRes::Draw - (a) o BONG do gui vo dieu kien du NPC khong co sprite bong; (b) vong ve THAN NGUOI gui moi o trong bang
thu tu, ke ca phan trang bi nguoi choi KHONG mac (ten rong). Vong ve HIEU UNG trang bi ngay tren da co cua chan nay tu truoc,
vong than nguoi bi bo sot. Sua: bo qua o co ten rong (ten rong khong bao gio nap duoc anh) + khong goi DrawPrimitives khi nPos = 0.
Chi sua Core -> CoreClient.dll (bo cuc lop khong doi)."""
import io, re, sys
p = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpcRes.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for ch in s if ord(ch) >= 0x80)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)
if "[VE 08/09 c]" in s:
    print("da va roi"); sys.exit(0)

# (a) o bong: chi gui khi co sprite
old = ("\tm_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;\r\n"
       "\tm_cDrawFile[nPos].Color.Color_b.a = Npc[nNpcIdx].m_HideState.nTime ? START_BLUR_ALPHA : 255;\r\n"
       "\tstrcpy(m_cDrawFile[nPos].szImage, this->m_cNpcShadow.m_szName);\r\n"
       "\tm_cDrawFile[nPos].uImage = m_cNpcShadow.m_dwNameID;\r\n"
       "\tm_cDrawFile[nPos].nFrame = this->m_cNpcShadow.m_nCurFrame;\r\n"
       "\tm_cDrawFile[nPos].oPosition.nX = nScreenX;\r\n"
       "\tm_cDrawFile[nPos].oPosition.nY = nScreenY;\r\n"
       "\tm_cDrawFile[nPos].oPosition.nZ = 0;//nScreenZ;\r\n"
       "\tnPos++;\r\n")
new = ("\t// [VE 08/09 c] NPC khong co sprite bong thi dung gui o rong (Represent3 nap khong ra anh roi bo).\r\n"
       "\tif (m_cNpcShadow.m_szName[0])\r\n\t{\r\n"
       "\t\tm_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;\r\n"
       "\t\tm_cDrawFile[nPos].Color.Color_b.a = Npc[nNpcIdx].m_HideState.nTime ? START_BLUR_ALPHA : 255;\r\n"
       "\t\tstrcpy(m_cDrawFile[nPos].szImage, this->m_cNpcShadow.m_szName);\r\n"
       "\t\tm_cDrawFile[nPos].uImage = m_cNpcShadow.m_dwNameID;\r\n"
       "\t\tm_cDrawFile[nPos].nFrame = this->m_cNpcShadow.m_nCurFrame;\r\n"
       "\t\tm_cDrawFile[nPos].oPosition.nX = nScreenX;\r\n"
       "\t\tm_cDrawFile[nPos].oPosition.nY = nScreenY;\r\n"
       "\t\tm_cDrawFile[nPos].oPosition.nZ = 0;//nScreenZ;\r\n"
       "\t\tnPos++;\r\n\t}\r\n")
s = rep(s, old, new)

# (b) vong ve than nguoi: bo qua o ten rong (phan trang bi nguoi choi khong mac)
old = ("\t\t\t\t// [MAUBUFF 04/09] dung nPos, KHONG dung i: anh gan vao m_cDrawFile[nPos] nen kieu ve/mau\r\n")
new = ("\t\t\t\t// [VE 08/09 c] Bo qua o KHONG co anh (phan trang bi nguoi choi khong mac, NPC thieu bo phan):\r\n"
       "\t\t\t\t// truoc day van gui di anh ten RONG - do duoc 300-900 o rong moi khung luc dong nguoi.\r\n"
       "\t\t\t\t// Cua chan nay vong ve HIEU UNG trang bi ngay tren da co tu truoc, vong than nguoi bi bo sot.\r\n"
       "\t\t\t\tif (!m_cNpcImage[m_nSortTable[i]].m_szName[0])\r\n\t\t\t\t\tcontinue;\r\n"
       "\t\t\t\t// [MAUBUFF 04/09] dung nPos, KHONG dung i: anh gan vao m_cDrawFile[nPos] nen kieu ve/mau\r\n")
s = rep(s, old, new)

# (c) khong goi khi rong (3 cho goi vo dieu kien trong Draw)
n = 0
for old2, ghi in [
    ("\tg_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu); //v", "\tif (nPos)\tg_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu); //v"),
    ("\tif(!Option.GetLow(LowMissle))//add by phong ki\xea\xf5 x\xf6 l\xfd kh\xebng v\xef skill hi\xdau \xf8ng gi\xf7a ng\xf5\xeai body g\xefm 2 ph\xefn \xae\xe2y l\xef ph\xefn 2\r\n\t\tg_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);\r\n", None),
]:
    if ghi and s.count(old2) == 1:
        s = s.replace(old2, ghi); n += 1
old3 = "\t}\r\n\r\n\tg_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);\r\n\tnPos = 0;\r\n\tbool gb_skill_150_draw = true;\r\n"
new3 = "\t}\r\n\r\n\tif (nPos)\tg_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);\t// [VE 08/09 c]\r\n\tnPos = 0;\r\n\tbool gb_skill_150_draw = true;\r\n"
s = rep(s, old3, new3); n += 1
print("   bo goi rong: %d cho" % n)

if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao"); sys.exit(1)
if re.search(r"[^\r]\n", s): print("FAIL LF"); sys.exit(1)
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("OK KNpcRes.cpp"); print("XONG VE c")
