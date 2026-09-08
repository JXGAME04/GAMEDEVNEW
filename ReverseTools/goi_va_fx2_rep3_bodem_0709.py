# -*- coding: ascii -*-
"""fx_patch2_rep3.py - [FX 07/09] Represent3.dll: bo dem lop ve + 2 sua chac chan.
 (1) Bo dem in them vao dong thong ke 30 s cua jx_rep3.log:
     tex_null   = DrawSpriteAlpha/Draw... gap texture NULL -> bo qua (im lang truoc day)
     anh_null   = GetImage tra NULL / khung ngoai tam trong DrawImage2D* -> break
     tao_hong   = CreateTexture16Bit di vao nhanh error (alloc / CreateTexture / LockRect / UpdateTexture hong)
     khung_khong_tex = PrepareFrameData xong ma khung van khong co texture
     giai_ma N khung X ms = so khung giai ma RLE->texture DONG BO tren luong ve va tong ms
 (2) Sua: m_nTexMemUsed chi cong SAU khi tao texture thanh cong (truoc: cong truoc, hong thi cache ao phong).
 (3) Sua: nhanh vuot ngan sach 'tmCur <= m_tmLastCheckBalance' -> '<' (bang nhau = cung mili giay -> goi lien tuc);
     CheckBalance tru m_uTexCacheMemUsed ngay khi tha mot khung (truoc: chi tinh lai o EndProfile).
Byte-safe: latin-1, ASCII-only, CRLF, kiem so byte cao va so lan khop."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
F = {"res": ROOT + r"\TextureRes.cpp", "mgr": ROOT + r"\TextureResMgr.cpp", "sh": ROOT + r"\KRepresentShell3.cpp"}
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
def crlf(s): return s.replace("\r\n", "\n").replace("\n", "\r\n")
def chk(*parts):
    for t in parts:
        if any(ord(c) >= 0x80 for c in t): print("FAIL non-ASCII"); sys.exit(1)
def rep(s, old, new, cnt=1, tag=""):
    old = crlf(old); new = crlf(new); chk(old, new)
    n = s.count(old)
    if n != cnt: print("FAIL [%s]: found %d expect %d: %r" % (tag, n, cnt, old[:80])); sys.exit(1)
    return s.replace(old, new)
def rep_re(s, pat, new, cnt=1, tag=""):
    ms = list(re.finditer(pat, s))
    if len(ms) != cnt: print("FAIL re[%s]: found %d expect %d" % (tag, len(ms), cnt)); sys.exit(1)
    return re.sub(pat, new, s)

# ================= KRepresentShell3.cpp: dinh nghia bo dem + dong thong ke + tex_null/anh_null =================
sh = rd(F["sh"]); sh0 = hb(sh)
sh = rep(sh,
"void Rep3Log(const char* fmt, ...)\n{\n",
"// [FX 07/09] bo dem lop ve (in them vao dong thong ke 30 s, roi dat lai 0). Chi luong ve cham.\n"
"unsigned g_uRep3FxTexNull = 0;		// DrawSprite*: texture NULL -> bo qua quad\n"
"unsigned g_uRep3FxAnhNull = 0;		// DrawImage2D*: GetImage NULL / khung ngoai tam -> break\n"
"unsigned g_uRep3FxTaoHong = 0;		// CreateTexture16Bit vao nhanh error\n"
"unsigned g_uRep3FxKhungKhongTex = 0;	// PrepareFrameData xong ma khung khong co texture\n"
"unsigned g_uRep3FxGiaiMa = 0;		// so khung giai ma dong bo tren luong ve\n"
"double   g_dRep3FxGiaiMaMs = 0.0;	// tong ms giai ma + tao texture\n"
"\n"
"void Rep3Log(const char* fmt, ...)\n{\n", 1, "sh-globals")

sh = rep(sh,
"raw spr %u MB | nap %u, bo %u | fps TB %.0f\",\n",
"raw spr %u MB | nap %u, bo %u | fps TB %.0f | fx: tex_null %u anh_null %u tao_hong %u khung_khong_tex %u giai_ma %u khung %.1f ms\",\n", 1, "sh-fmt")
sh = rep(sh,
"				uNodes, uTexMB, uDrawMB, uBudgetMB, uRawMB, (unsigned)m_TextureResMgr.m_nLoadCount, (unsigned)m_TextureResMgr.m_nReleaseCount, m_fFpsAvg);\n",
"				uNodes, uTexMB, uDrawMB, uBudgetMB, uRawMB, (unsigned)m_TextureResMgr.m_nLoadCount, (unsigned)m_TextureResMgr.m_nReleaseCount, m_fFpsAvg,\n"
"				g_uRep3FxTexNull, g_uRep3FxAnhNull, g_uRep3FxTaoHong, g_uRep3FxKhungKhongTex, g_uRep3FxGiaiMa, g_dRep3FxGiaiMaMs);\n"
"			g_uRep3FxTexNull = 0; g_uRep3FxAnhNull = 0; g_uRep3FxTaoHong = 0; g_uRep3FxKhungKhongTex = 0; g_uRep3FxGiaiMa = 0; g_dRep3FxGiaiMaMs = 0.0;\n", 1, "sh-args")

# tex NULL -> continue (DrawSpriteAlpha) va -> return (ham ve thu hai)
sh = rep(sh,
"		LPDIRECT3DTEXTURE9 pTex = pSprite->GetTexture(nFrame, i);\n		if(!pTex)\n			continue;\n",
"		LPDIRECT3DTEXTURE9 pTex = pSprite->GetTexture(nFrame, i);\n		if(!pTex)\n		{\n			g_uRep3FxTexNull++;	// [FX 07/09] truoc day bo qua im lang\n			continue;\n		}\n", 1, "sh-texnull-continue")
sh = rep(sh,
"		LPDIRECT3DTEXTURE9 pTex = pSprite->GetTexture(nFrame, i);\n		if(!pTex)\n			return;\n",
"		LPDIRECT3DTEXTURE9 pTex = pSprite->GetTexture(nFrame, i);\n		if(!pTex)\n		{\n			g_uRep3FxTexNull++;	// [FX 07/09]\n			return;\n		}\n", 1, "sh-texnull-return")
# GetImage NULL / khung ngoai tam -> break (8 cho, giu nguyen thut dong)
sh = rep_re(sh,
 r"if \(!pSprite \|\| pTemp->nFrame >= pSprite->m_nFrameNum\)(\s*)(break|continue);",
 lambda m: "if (!pSprite || pTemp->nFrame >= pSprite->m_nFrameNum)" + m.group(1) + "{ g_uRep3FxAnhNull++; " + m.group(2) + "; }", 8, "sh-anhnull")
if hb(sh) != sh0: print("FAIL hb sh"); sys.exit(1)

# ================= TextureRes.cpp: giai ma dong bo + tao hong + cong bo nho sau khi tao =================
res = rd(F["res"]); res0 = hb(res)
res = rep(res,
"#include <cstdint>\n#include <new>\n",
"#include <cstdint>\n#include <new>\n"
"\n"
"// [FX 07/09] bo dem lop ve, dinh nghia o KRepresentShell3.cpp\n"
"extern unsigned g_uRep3FxTaoHong;\n"
"extern unsigned g_uRep3FxKhungKhongTex;\n"
"extern unsigned g_uRep3FxGiaiMa;\n"
"extern double   g_dRep3FxGiaiMaMs;\n"
"static double Rep3FxMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b)\n"
"{\n"
"	static LARGE_INTEGER s_liTanSo = {0};\n"
"	if (!s_liTanSo.QuadPart) QueryPerformanceFrequency(&s_liTanSo);\n"
"	return s_liTanSo.QuadPart ? (1000.0 * (double)(b.QuadPart - a.QuadPart) / (double)s_liTanSo.QuadPart) : 0.0;\n"
"}\n", 1, "res-extern")

# PrepareFrameData: dem khung khong texture sau khi tao (giu return true de khong doi luong ve)
res = rep(res,
"	if(bPrepareTex)\n		CreateTexture16Bit(szImage, nFrame);\n\n	return true;\n}\n",
"	if(bPrepareTex)\n"
"	{\n"
"		CreateTexture16Bit(szImage, nFrame);\n"
"		if (!m_pFrameInfo[nFrame].texInfo[0].pTexture)\n"
"			g_uRep3FxKhungKhongTex++;	// [FX 07/09] truoc day: van tra true, ve bo qua im lang\n"
"	}\n\n	return true;\n}\n", 1, "res-prepare")

# CreateTexture16Bit: dong ho + dem + cong bo nho sau khi tao
res = rep(res,
"	SplitTexture(nFrame);\n\n	// [REP3 03/09] texture 8888",
"	SplitTexture(nFrame);\n\n"
"	LARGE_INTEGER liFx0, liFx1;	// [FX 07/09] do giai ma dong bo tren luong ve\n"
"	QueryPerformanceCounter(&liFx0);\n\n"
"	// [REP3 03/09] texture 8888", 1, "res-qpc0")
res = rep(res,
"		TextureInfo& ti = m_pFrameInfo[nFrame].texInfo[i];\n		m_nTexMemUsed += ti.nWidth * ti.nHeight * nBpp;\n",
"		TextureInfo& ti = m_pFrameInfo[nFrame].texInfo[i];\n"
"		// [FX 07/09] m_nTexMemUsed chi cong SAU khi tao texture thanh cong (xem cuoi vong lap)\n", 1, "res-memmove")
res = rep(res,
"			pFill->Release();\n			ti.pTexture = pVram;\n		}\n		else\n			ti.pTexture = pFill;\n	}\n",
"			pFill->Release();\n			ti.pTexture = pVram;\n		}\n		else\n			ti.pTexture = pFill;\n"
"		m_nTexMemUsed += ti.nWidth * ti.nHeight * nBpp;	// [FX 07/09] cong sau khi tao thanh cong\n	}\n", 1, "res-memadd")
res = rep(res,
"		m_pFrameInfo[nFrame].pRawData = NULL;\n	}\n	return;\n\nerror:\n	SAFE_DELETE_ARRAY(pTempData);\n	return;\n}\n",
"		m_pFrameInfo[nFrame].pRawData = NULL;\n	}\n"
"	QueryPerformanceCounter(&liFx1);	// [FX 07/09]\n"
"	g_uRep3FxGiaiMa++;\n"
"	g_dRep3FxGiaiMaMs += Rep3FxMs(liFx0, liFx1);\n"
"	return;\n\nerror:\n"
"	QueryPerformanceCounter(&liFx1);	// [FX 07/09] truoc day hong im lang\n"
"	g_uRep3FxTaoHong++;\n"
"	g_dRep3FxGiaiMaMs += Rep3FxMs(liFx0, liFx1);\n"
"	SAFE_DELETE_ARRAY(pTempData);\n	return;\n}\n", 1, "res-return")
if hb(res) != res0: print("FAIL hb res"); sys.exit(1)

# ================= TextureResMgr.cpp: nhanh vuot ngan sach + tru bo nho khi tha khung =================
mgr = rd(F["mgr"]); mgr0 = hb(mgr)
mgr = rep(mgr,
"            (tmCur <= m_tmLastCheckBalance) || \n",
"            (tmCur < m_tmLastCheckBalance) || 	// [FX 07/09] truoc la <= : cung mili giay goi bao nhieu lan bo bay nhieu khung\n", 1, "mgr-le")
mgr = rep(mgr,
"		if (node.m_pTextureRes->ReleaseAFrameData())\n			continue;\n",
"		{\n"
"			// [FX 07/09] tru ngay phan vua tha (truoc: m_uTexCacheMemUsed chi tinh lai o EndProfile nen 'vuot ngan sach' keo dai ca khung)\n"
"			unsigned long uTruoc = node.m_pTextureRes->m_nTexMemUsed;\n"
"			if (node.m_pTextureRes->ReleaseAFrameData())\n"
"			{\n"
"				unsigned long uSau = node.m_pTextureRes->m_nTexMemUsed;\n"
"				if (uTruoc > uSau && m_uTexCacheMemUsed >= (uTruoc - uSau))\n"
"					m_uTexCacheMemUsed -= (uint32)(uTruoc - uSau);\n"
"				continue;\n"
"			}\n"
"		}\n", 1, "mgr-release")
if hb(mgr) != mgr0: print("FAIL hb mgr"); sys.exit(1)

for k, s in (("sh", sh), ("res", res), ("mgr", mgr)):
    if re.search(r"[^\r]\n", s): print("FAIL bare LF", k); sys.exit(1)
wr(F["sh"], sh); wr(F["res"], res); wr(F["mgr"], mgr)
print("OK rep3: sh hb=%d res hb=%d mgr hb=%d" % (sh0, res0, mgr0))
print("ALL APPLIED (rep3)")
