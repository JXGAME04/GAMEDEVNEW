# -*- coding: utf-8 -*-
"""[REP3 03/09] Va Represent3 trong worktree D:\GAMEDEVNEW_wt_rep3 de ve PHANG nhu Represent2 tren GPU
+ port cai tien 2.0 (NPOT, cache, texture 8888). Doc/ghi latin-1, giu CRLF. Chay 1 lan; chay lai se bao 'da va'."""
import io, re, sys, os

ROOT = r"D:\GAMEDEVNEW_wt_rep3\Sources\Represent\Represent3"

def rd(name):
    with io.open(os.path.join(ROOT, name), "r", encoding="latin-1", newline="") as f:
        return f.read()

def wr(name, s):
    assert "\r\n" in s and s.count("\n") == s.count("\r\n"), "mat CRLF"
    with io.open(os.path.join(ROOT, name), "w", encoding="latin-1", newline="") as f:
        f.write(s)

def crlf(s):
    return s.replace("\r\n", "\n").replace("\n", "\r\n")

def sub1(text, pattern, repl, flags=re.S, count_expected=1, name=""):
    n = len(re.findall(pattern, text, flags))
    assert n == count_expected, "pattern '%s' found %d (expected %d)" % (name or pattern[:60], n, count_expected)
    return re.sub(pattern, repl, text, count=count_expected, flags=flags)

MARK = "[REP3 03/09]"

# ============================================================ BaseInclude.h
h = rd("BaseInclude.h")
if MARK not in h:
    add = crlf("""
// %s cong tac doc tu [Client] config.ini (KRepresentShell3.cpp)
extern int  g_nRep3Flat;        // 1 = ve phang 2D nhu Represent2 (mac dinh), 0 = ortho 3D cu
extern int  g_nRep3Composite;   // 1 = ghep nhan vat len texture roi ve (cach cu), 0 = ve tung phan nhu Represent2
extern int  g_nRep3Tex32;       // 1 = texture sprite A8R8G8B8 (dung mau palette), 0 = A4R4G4B4
extern int  g_nRep3Npot;        // 1 = dung texture khong luy thua 2 neu card ho tro (theo 2.0)
extern int  g_nRep3Vsync;       // 0 = Present ngay (client tu dieu nhip PaintFps), 1 = cho vsync
extern int  g_nRep3CacheMB;     // 0 = tu tinh theo RAM, >0 = ngan sach cache texture (MB)
extern int  g_nRep3Log;         // 1 = ghi jx_rep3.log
extern bool g_bNpotOK;          // card + thiet bi da qua thu NPOT
extern int  g_nMaxTexW, g_nMaxTexH;
void Rep3Log(const char* fmt, ...);
""" % MARK)
    h = sub1(h, r"(#define YESNO\(X\))", lambda m: add.lstrip("\r\n") + "\r\n" + m.group(1), name="BaseInclude YESNO")
    wr("BaseInclude.h", h)
    print("BaseInclude.h OK")
else:
    print("BaseInclude.h da va")

# ============================================================ KRepresentShell3.h
h = rd("KRepresentShell3.h")
if MARK not in h:
    h = sub1(h,
        r"void DrawSpritePartAlpha\(int32 nX, int32 nY, int32 nWidth, int32 nHeight, int32 nFrame, TextureResSpr\* pSprite, RECT& rect\);",
        crlf("""void DrawSpritePartAlpha(int32 nX, int32 nY, int32 nWidth, int32 nHeight, int32 nFrame, TextureResSpr* pSprite, RECT& rect,
        DWORD color = 0xffffffff, int32 nRenderStyle = 0);
    // %s ve tung sprite theo dung ngu nghia Represent2 (khong ghep texture, khong anh sang)
    void DrawImage2DFlat(int nPrimitiveCount, KRepresentUnit* pPrimitives, int bSinglePlaneCoord, int bClipRect);
    // %s dat/khoi phuc trang thai tron mau theo kieu ve (ALPHA/OPACITY/COLOR_ADJUST/spr moi = SCREEN)
    void SetSpriteBlend(int nRenderStyle, DWORD color, bool bNew, DWORD& vtxColor);
    void ResetSpriteBlend();""" % (MARK, MARK)), name="hdr DrawSpritePartAlpha")
    h = sub1(h, r"(\r\n\s*bool m_bDeviceLost;)",
        lambda m: crlf("""
    int   m_nBlendMode;                 // %s 0 = binh thuong, 1 = alpha test (OPACITY), 2 = screen (spr moi)
    DWORD m_dwLastPresent;              // %s moc Present truoc, tinh fps trung binh
    float m_fFpsAvg;                    // %s fps trung binh (EMA ~100 khung) - cong don cache cuoi khung""" % (MARK, MARK, MARK)) + m.group(1),
        name="hdr members")
    wr("KRepresentShell3.h", h)
    print("KRepresentShell3.h OK")
else:
    print("KRepresentShell3.h da va")

# ============================================================ TextureResMgr.h
h = rd("TextureResMgr.h")
if MARK not in h:
    h = sub1(h, r"(\r\n\s*//## \S*\r\n\s*void CheckBalance\(\);)",
        lambda m: m.group(1) + crlf("""
    // %s goi cuoi moi khung ve (khi fps trung binh >= 25): bo toi da 1 tai nguyen/khung texture nghi > 10 s
    void CheckBalanceFrame();""" % MARK), name="mgr hdr CheckBalance")
    wr("TextureResMgr.h", h)
    print("TextureResMgr.h OK")
else:
    print("TextureResMgr.h da va")

# ============================================================ KRepresentShell3.cpp
c = rd("KRepresentShell3.cpp")
if MARK not in c:
    # ---- 1. globals + helpers
    globals_add = crlf("""
// %s cong tac doc tu [Client] config.ini - xem BaseInclude.h
int  g_nRep3Flat      = 1;
int  g_nRep3Composite = 0;
int  g_nRep3Tex32     = 1;
int  g_nRep3Npot      = 1;
int  g_nRep3Vsync     = 0;
int  g_nRep3CacheMB   = 0;
int  g_nRep3Log       = 1;
bool g_bNpotOK        = false;
int  g_nMaxTexW = 1024, g_nMaxTexH = 1024;

static char s_szRep3Ini[MAX_PATH] = "";
static int Rep3Ini(const char* szKey, int nDef)
{
	if (!s_szRep3Ini[0])
	{
		GetCurrentDirectoryA(MAX_PATH - 16, s_szRep3Ini);
		strcat(s_szRep3Ini, "\\\\config.ini");
	}
	return (int)GetPrivateProfileIntA("Client", szKey, nDef, s_szRep3Ini);
}

void Rep3Log(const char* fmt, ...)
{
	if (!g_nRep3Log)
		return;
	FILE* pLog = fopen("jx_rep3.log", "a");
	if (!pLog)
		return;
	char szBuf[1024];
	va_list va;
	va_start(va, fmt);
	_vsnprintf(szBuf, sizeof(szBuf) - 1, fmt, va);
	va_end(va);
	szBuf[sizeof(szBuf) - 1] = 0;
	fprintf(pLog, "[%%u] %%s\\n", (unsigned int)GetTickCount(), szBuf);
	fclose(pLog);
}
""" % MARK)
    c = sub1(c, r"(bool\s+g_bNonPow2Conditional = false;\r\n)", lambda m: m.group(1) + globals_add, name="globals")

    # ---- 2. ctor init
    c = sub1(c, r"(\tm_bDoLighting = true;\r\n\tm_pJxReplay = NULL;)",
        lambda m: crlf("""	m_bDoLighting = true;
	m_nBlendMode = 0;		// %s
	m_dwLastPresent = 0;
	m_fFpsAvg = 0.0f;
	m_pJxReplay = NULL;""" % MARK), name="ctor")

    # ---- 3. SetOption(PERSPECTIVE): giu 2D khi Rep3Flat
    c = sub1(c, r"case PERSPECTIVE:\r\n\t\t\{\tif\(bOn\)\r\n\t\t\t\tg_renderModel = RenderModel3DOrtho;\r\n\t\t\telse\r\n\t\t\t\tg_renderModel = RenderModel3DOrtho;",
        crlf("""case PERSPECTIVE:
		{	if (g_nRep3Flat)			// %s ve phang: khong doi sang 3D
				g_renderModel = RenderModel2D;
			else if(bOn)
				g_renderModel = RenderModel3DOrtho;
			else
				g_renderModel = RenderModel3DOrtho;""" % MARK), name="SetOption")

    # ---- 4. Create(): doc config + che do 2D + cua so 1:1
    create_add = crlf("""
	// %s doc cong tac [Client] trong config.ini
	g_nRep3Flat      = Rep3Ini("Rep3Flat", 1);
	g_nRep3Composite = Rep3Ini("Rep3Composite", 0);
	g_nRep3Tex32     = Rep3Ini("Rep3Tex32", 1);
	g_nRep3Npot      = Rep3Ini("Rep3Npot", 1);
	g_nRep3Vsync     = Rep3Ini("Rep3Vsync", 0);
	g_nRep3CacheMB   = Rep3Ini("Rep3CacheMB", 0);
	g_nRep3Log       = Rep3Ini("Rep3Log", 1);
	g_bUse4444Texture = (g_nRep3Tex32 == 0);
	if (g_nRep3Flat)
		g_renderModel = RenderModel2D;
	Rep3Log("[REP3] Create %%dx%%d full=%%d flat=%%d composite=%%d tex32=%%d npot=%%d vsync=%%d cacheMB=%%d",
		nWidth, nHeight, (int)bFullScreen, g_nRep3Flat, g_nRep3Composite, g_nRep3Tex32, g_nRep3Npot, g_nRep3Vsync, g_nRep3CacheMB);
""" % MARK)
    c = sub1(c, r"(bool KRepresentShell3::Create\(int nWidth, int nHeight, bool bFullScreen\)\r\n\{\r\n)",
        lambda m: m.group(1) + create_add.lstrip("\r\n"), name="Create head")
    window_add = crlf("""
	if (g_bRunWindowed)
	{
		// %s cua so phai dung nWidth x nHeight de Present 1:1 (nhu KDirectDraw::SetWindowStyle).
		// Game.exe cu (truoc 03/09) tu keo cao them 40 px va co toa do chuot theo 40/808:
		// gap dung dau hieu do thi GIU NGUYEN de chuot khop, chap nhan hinh bi keo 5%%.
		RECT rcClient = {0, 0, 0, 0};
		GetClientRect(g_hWnd, &rcClient);
		int nCW = rcClient.right - rcClient.left;
		int nCH = rcClient.bottom - rcClient.top;
		if (nCW == nWidth && nCH == nHeight + 40)
		{
			Rep3Log("[REP3] cua so %%dx%%d = Game.exe cu (+40 px), giu nguyen", nCW, nCH);
		}
		else if (nCW != nWidth || nCH != nHeight)
		{
			RECT	rc = {0, 0, nWidth, nHeight};
			DWORD	dwStyle = WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX;
			SetWindowLong(g_hWnd, GWL_STYLE, dwStyle);
			AdjustWindowRectEx(&rc, dwStyle, GetMenu(g_hWnd) != NULL, GetWindowLong(g_hWnd, GWL_EXSTYLE));
			SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
				SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
			Rep3Log("[REP3] cua so %%dx%%d -> dat lai %%dx%%d", nCW, nCH, nWidth, nHeight);
		}
	}
""" % MARK)
    c = sub1(c, r"(\tD3DAdapterInfo\* pAdapterInfo\t = NULL;\r\n\tD3DDeviceInfo\*  pDeviceInfo\t\t = NULL;\r\n\tD3DModeInfo\*\tpModeInfo\t\t = NULL;\r\n\r\n\tpDeviceInfo = g_D3DShell\.PickDefaultDev)",
        lambda m: window_add + m.group(1), name="Create window")

    # ---- 5. RestoreDeviceObjects: ALPHAOP MODULATE (alpha dinh x alpha texture) + CLAMP cho NPOT
    c = c.replace("D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );", "D3DTSS_ALPHAOP,   D3DTOP_MODULATE );")
    c = sub1(c, r"([ \t]+PD3DDEVICE->SetSamplerState\( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT \);\r\n\r\n[ \t]+PD3DDEVICE->SetTextureStageState\( 0, D3DTSS_TEXCOORDINDEX, 0 \);)",
        lambda m: crlf("""	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	// %s NPOT co dieu kien bat buoc CLAMP; UV cua ta luon trong [0,1] nen khong doi hinh
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	PD3DDEVICE->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

    PD3DDEVICE->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );""" % MARK), name="Restore sampler")

    # ---- 6. DrawPrimitives: RU_T_IMAGE / RU_T_IMAGE_4 -> flat; RU_T_IMAGE_PART them kieu ve
    c = sub1(c, r"\t\t\tif\( g_renderModel == RenderModel2D \|\| bSinglePlaneCoord\)\r\n\t\t\t\tDrawImage2D\(nPrimitiveCount, pPrimitives, bSinglePlaneCoord\);\r\n\t\t\telse\r\n\t\t\t\tDrawImage3D\(uGenre, nPrimitiveCount, pPrimitives, bSinglePlaneCoord\);",
        crlf("""			if( g_renderModel == RenderModel2D || bSinglePlaneCoord)
			{
				if (g_nRep3Composite)
					DrawImage2D(nPrimitiveCount, pPrimitives, bSinglePlaneCoord);
				else	// %s ve tung sprite y nhu Represent2
					DrawImage2DFlat(nPrimitiveCount, pPrimitives, bSinglePlaneCoord, uGenre == RU_T_IMAGE_4);
			}
			else
				DrawImage3D(uGenre, nPrimitiveCount, pPrimitives, bSinglePlaneCoord);""" % MARK), name="DrawPrimitives image")
    c = sub1(c, r"(\t\t\t\t\t\tswitch\(pTemp->bRenderStyle\)\r\n\t\t\t\t\t\t\{\r\n)(\t\t\t\t\t\tcase IMAGE_RENDER_STYLE_ALPHA:\r\n\t\t\t\t\t\tcase IMAGE_RENDER_STYLE_3LEVEL:\r\n\t\t\t\t\t\tcase IMAGE_RENDER_STYLE_OPACITY:\r\n\t\t\t\t\t\tcase IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT:\r\n\t\t\t\t\t\t\{\r\n\t\t\t\t\t\t\tDrawSpritePartAlpha\(nX, nY, pSprite->m_pFrameInfo\[pTemp->nFrame\]\.nWidth, ?\r\n\t\t\t\t\t\t\t\tpSprite->m_pFrameInfo\[pTemp->nFrame\]\.nHeight, pTemp->nFrame, pSprite, rc\);)",
        lambda m: m.group(1) + crlf("""						case IMAGE_RENDER_STYLE_BORDER:				// %s
						case IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST:
""" % MARK) + m.group(2).replace("pSprite, rc);", "pSprite, rc, pTemp->Color.Color_dw, pTemp->bRenderStyle);"),
        name="DrawPrimitives part")

    # ---- 7. DrawImage2DFlat + SetSpriteBlend/ResetSpriteBlend (chen truoc DrawImage2DStretch)
    flat_fn = crlf("""
// %s ve tung sprite theo dung Represent2::DrawPrimitives (RU_T_IMAGE / RU_T_IMAGE_4):
//  - toa do: CoordinateTransform 2D, REF_SPOT, FRAME_DRAW y het;
//  - RU_T_IMAGE_4 (bClipRect): cat theo oImgLTPos/oImgRBPos nhu SetClipRect cua Represent2;
//  - kieu ve giao cho SetSpriteBlend (ALPHA/3LEVEL/NOT_BE_LIT = alpha; OPACITY = duc; BORDER = khong ve;
//    COLOR_ADJUST = nhan mau; spr moi (Reserved[1]) = SCREEN nhu DrawSpriteBlendColor(..., 1, TRUE)).
void KRepresentShell3::DrawImage2DFlat(int nPrimitiveCount, KRepresentUnit* pPrimitives, int bSinglePlaneCoord, int bClipRect)
{
	int i;
	KRUImage* pTemp = (KRUImage*)pPrimitives;
	int nStep = bClipRect ? sizeof(KRUImage4) : sizeof(KRUImage);

	for (i = 0; i < nPrimitiveCount; i++, pTemp = (KRUImage*)((char*)pTemp + nStep))
	{
		switch(pTemp->nType)
		{
		case ISI_T_SPR:
			{
				if (pTemp->bRenderStyle == IMAGE_RENDER_STYLE_BORDER)	// Represent2 khong ve gi
					break;
				TextureResSpr* pSprite = (TextureResSpr *)m_TextureResMgr.GetImage(
					pTemp->szImage,	pTemp->uImage,
					pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
				if (!pSprite || pTemp->nFrame >= pSprite->m_nFrameNum)
					break;

				int nX = pTemp->oPosition.nX;
				int nY = pTemp->oPosition.nY;
				if (!bSinglePlaneCoord)
					CoordinateTransform(nX, nY, pTemp->oPosition.nZ);

				if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
				{
					int nCenterX = pSprite->GetCenterX();
					int nCenterY = pSprite->GetCenterY();
					if (nCenterX || nCenterY)
					{
						nX -= nCenterX;
						nY -= nCenterY;
					}
					else if (pSprite->GetWidth() > 160)
					{
						nX -= 160;
						nY -= 192;
					}
				}
				if (!(pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW))
				{
					nX += pSprite->m_pFrameInfo[pTemp->nFrame].nOffX;
					nY += pSprite->m_pFrameInfo[pTemp->nFrame].nOffY;
				}
				int nW = pSprite->m_pFrameInfo[pTemp->nFrame].nWidth;
				int nH = pSprite->m_pFrameInfo[pTemp->nFrame].nHeight;
				if (bClipRect)
				{
					KRUImage4* p4 = (KRUImage4*)pTemp;
					RECT rc;
					rc.left  = nX;
					rc.top   = nY;
					nX -= p4->oImgLTPos.nX;
					nY -= p4->oImgLTPos.nY;
					rc.right = nX + p4->oImgRBPos.nX;
					rc.bottom= nY + p4->oImgRBPos.nY;
					if (rc.left < 0) rc.left = 0;
					if (rc.top < 0) rc.top = 0;
					if (rc.right > g_nScreenWidth) rc.right = g_nScreenWidth;
					if (rc.bottom > g_nScreenHeight) rc.bottom = g_nScreenHeight;
					DrawSpritePartAlpha(nX, nY, nW, nH, pTemp->nFrame, pSprite, rc, pTemp->Color.Color_dw, pTemp->bRenderStyle);
				}
				else
					DrawSpriteAlpha(nX, nY, nW, nH, pTemp->nFrame, pSprite, pTemp->Color.Color_dw, pTemp->bRenderStyle);
			}
			break;
		case ISI_T_BITMAP16:
			{
				TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
					pTemp->szImage,	pTemp->uImage,
					pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
				if (!pBitmap)
					break;

				int nX = pTemp->oPosition.nX;
				int nY = pTemp->oPosition.nY;
				if (!bSinglePlaneCoord)
					CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
				DrawBitmap16(nX, nY, pBitmap->GetWidth(), pBitmap->GetHeight(), pBitmap);
			}
			break;
		}
	}
}

// %s trang thai tron mau theo kieu ve - doi chieu tung nhanh cua Represent2::DrawPrimitives
//   Represent2 (32 bit):  ALPHA/3LEVEL/NOT_BE_LIT -> DrawSpriteAlpha(alpha = run*Color.a)
//                         OPACITY -> DrawSprite (duc, bo Color.a)
//                         BORDER  -> khong ve (DrawSpriteBorder da bi chu thich)
//                         COLOR_ADJUST -> Color.rgb==0: nhu ALPHA; khac 0: g_BlendColor32b mode 0 = nhan mau
//                         spr moi Reserved[1] -> g_DrawSpriteScreen32b: d' = d + a*s*(1-d) (mode 1 doi mau ~ nhan mau)
void KRepresentShell3::SetSpriteBlend(int nRenderStyle, DWORD color, bool bNew, DWORD& vtxColor)
{
	DWORD a   = color >> 24;
	DWORD rgb = color & 0x00ffffff;
	DWORD tint = 0x00ffffff;
	m_nBlendMode = 0;

	if (nRenderStyle == IMAGE_RENDER_STYLE_OPACITY)
	{
		vtxColor = 0xffffffff;
		PD3DDEVICE->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		PD3DDEVICE->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		PD3DDEVICE->SetRenderState( D3DRS_ALPHAREF, 0x01 );
		PD3DDEVICE->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		m_nBlendMode = 1;
		return;
	}
	if ((bNew || nRenderStyle == IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST) && rgb != 0)
		tint = rgb;
	vtxColor = (a << 24) | tint;
	if (bNew)
	{
		// screen: SRC = tex*diffuse*(texA*diffA) [2 tang], out = SRC*(1-dst) + dst
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT | D3DTA_ALPHAREPLICATE );
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
		PD3DDEVICE->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_INVDESTCOLOR );
		PD3DDEVICE->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		m_nBlendMode = 2;
	}
}

void KRepresentShell3::ResetSpriteBlend()
{
	switch (m_nBlendMode)
	{
	case 1:
		PD3DDEVICE->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		PD3DDEVICE->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		break;
	case 2:
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		PD3DDEVICE->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		PD3DDEVICE->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;
	}
	m_nBlendMode = 0;
}
""" % (MARK, MARK))
    c = sub1(c, r"(\r\nvoid KRepresentShell3::DrawImage2DStretch\()", lambda m: flat_fn + m.group(1), name="insert flat fn")

    # ---- 8. DrawSpriteAlpha: kieu ve + mau dinh + DISCARD
    m = re.search(r"void KRepresentShell3::DrawSpriteAlpha\(int32 nX, int32 nY, int32 nWidth, int32 nHeight, \r\n\t\t\t\t\t\t\t\t\t   int32 nFrame, TextureResSpr\* pSprite, DWORD color, int32 nRenderStyle\)\r\n\{.*?\r\n\}\r\n", c, re.S)
    assert m, "DrawSpriteAlpha body"
    new_dsa = crlf("""void KRepresentShell3::DrawSpriteAlpha(int32 nX, int32 nY, int32 nWidth, int32 nHeight,
									   int32 nFrame, TextureResSpr* pSprite, DWORD color, int32 nRenderStyle)
{
	int i;

	if(nFrame >= pSprite->m_nFrameNum)
		return;
	// %s BORDER: Represent2 khong ve gi (DrawSpriteBorder da bi chu thich) - khong lam sang nhan vat nua
	if (nRenderStyle == IMAGE_RENDER_STYLE_BORDER)
		return;

	if(FAILED(PD3DDEVICE->SetStreamSource( 0, m_pVB2D, 0, sizeof(VERTEX2D) )))
		return;
	PD3DDEVICE->SetFVF( D3DFVF_VERTEX2D );

	float fX1, fY1, fX2, fY2;
	fX1 = (float)nX;
	fY1 = (float)nY;
	fX2 = fX1 + (float)nWidth;
	fY2 = fY1 + (float)nHeight;

	if(fX2 < 0 || fX1 > g_nScreenWidth || fY2 < 0 || fY1 > g_nScreenHeight)
		return;

	DWORD vtxColor = 0xffffffff;
	SetSpriteBlend(nRenderStyle, color, pSprite->m_bNew, vtxColor);

	VERTEX2D* pvb = NULL;
	if(FAILED(m_pVB2D->Lock( 0, 16*sizeof(VERTEX2D), (void**)&pvb, D3DLOCK_DISCARD )))
	{
		ResetSpriteBlend();
		return;
	}

	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		float fU2, fV2;
		float x1, y1, x2, y2;
		fU2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth /
				(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nWidth;
		fV2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight /
				(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nHeight;

		x1 = ChaZhi(fX1, fX2, 0, (float)nWidth, (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX);
		y1 = ChaZhi(fY1, fY2, 0, (float)nHeight, (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY);
		x2 = ChaZhi(fX1, fX2, 0, (float)nWidth, (float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX +
										pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth));
		y2 = ChaZhi(fY1, fY2, 0, (float)nHeight, (float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY +
										pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight));

		x1 -= 0.5f,	y1 -= 0.5f,	x2 -= 0.5f,	y2 -= 0.5f;

		int nBase = i*4;
		pvb[nBase+0].position = D3DXVECTOR4( x1,y1, 100, 1 );
		pvb[nBase+0].color    = vtxColor;
		pvb[nBase+0].tu       = 0.0f;
		pvb[nBase+0].tv       = 0.0f;

		pvb[nBase+1].position = D3DXVECTOR4( x2,y1, 100, 1 );
		pvb[nBase+1].color    = vtxColor;
		pvb[nBase+1].tu       = fU2;
		pvb[nBase+1].tv       = 0.0f;

		pvb[nBase+2].position = D3DXVECTOR4( x1,y2, 100, 1 );
		pvb[nBase+2].color    = vtxColor;
		pvb[nBase+2].tu       = 0.0f;
		pvb[nBase+2].tv       = fV2;

		pvb[nBase+3].position = D3DXVECTOR4( x2,y2, 100, 1 );
		pvb[nBase+3].color    = vtxColor;
		pvb[nBase+3].tu       = fU2;
		pvb[nBase+3].tv       = fV2;
	}
	m_pVB2D->Unlock();

	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		LPDIRECT3DTEXTURE9 pTex = pSprite->GetTexture(nFrame, i);
		if(!pTex)
			continue;
		PD3DDEVICE->SetTexture( 0, pTex );
		PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*4, 2 );
	}
	ResetSpriteBlend();
}
""" % MARK)
    c = c[:m.start()] + new_dsa + c[m.end():]

    # ---- 9. DrawSpritePartAlpha: them color/style
    c = sub1(c, r"void KRepresentShell3::DrawSpritePartAlpha\(int32 nX, int32 nY, int32 nWidth, int32 nHeight, \r\n\t\t\t\t\t\t\t\t\t   int32 nFrame, TextureResSpr\* pSprite, RECT &rect\)\r\n\{\r\n\tint i;\r\n\r\n\tif\(nFrame >= pSprite->m_nFrameNum\)\r\n\t\treturn;",
        crlf("""void KRepresentShell3::DrawSpritePartAlpha(int32 nX, int32 nY, int32 nWidth, int32 nHeight,
									   int32 nFrame, TextureResSpr* pSprite, RECT &rect, DWORD color, int32 nRenderStyle)
{
	int i;

	if(nFrame >= pSprite->m_nFrameNum)
		return;
	if (nRenderStyle == IMAGE_RENDER_STYLE_BORDER)	// %s Represent2 khong ve
		return;""" % MARK), name="part head")
    # trong DrawSpritePartAlpha: Lock DISCARD + mau dinh + blend
    pm = re.search(r"void KRepresentShell3::DrawSpritePartAlpha\(.*?\r\n\}\r\n", c, re.S)
    body = pm.group(0)
    body2 = body.replace("if(FAILED(m_pVB2D->Lock( 0, 16*sizeof(VERTEX2D), (void**)&pvb, 0 )))\r\n\t\treturn;",
        "DWORD vtxColor = 0xffffffff;\r\n\tSetSpriteBlend(nRenderStyle, color, pSprite->m_bNew, vtxColor);	// " + MARK + "\r\n\tif(FAILED(m_pVB2D->Lock( 0, 16*sizeof(VERTEX2D), (void**)&pvb, D3DLOCK_DISCARD )))\r\n\t{\r\n\t\tResetSpriteBlend();\r\n\t\treturn;\r\n\t}")
    assert body2 != body, "part lock"
    n = body2.count("].color    = 0xffffffff;")
    assert n == 4, n
    body2 = body2.replace("].color    = 0xffffffff;", "].color    = vtxColor;")
    body2 = body2.replace("\t\tPD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*4, 2 );\t\r\n\t}\r\n}\r\n",
                          "\t\tPD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*4, 2 );\t\r\n\t}\r\n\tResetSpriteBlend();\r\n}\r\n")
    assert "ResetSpriteBlend();\r\n}" in body2, "part tail"
    c = c.replace(body, body2)

    # ---- 10. DISCARD cho cac Lock VB con lai
    c = re.sub(r"m_pVB2D->Lock\( 0, (\d+)\*sizeof\(VERTEX2D\), \(void\*\*\)&pvb, 0 \)", r"m_pVB2D->Lock( 0, \1*sizeof(VERTEX2D), (void**)&pvb, D3DLOCK_DISCARD )", c)

    # ---- 11. CoordinateTransform / X: nhanh 2D
    for fn in ("CoordinateTransform", "CoordinateTransformX"):
        c = sub1(c, r"(void KRepresentShell3::%s\( ?int& nX, int& nY, int nZ\)\r\n\{\r\n)" % fn,
            lambda m: m.group(1) + crlf("""	if (g_renderModel == RenderModel2D)		// %s cong thuc Represent2
	{
		nX = nX - m_nLeft;
		nY = nY / 2 - m_nTop - ((nZ * 887) >> 10);
		return;
	}
""" % MARK), name=fn)

    # ---- 12. RepresentBegin: xoa theo mau yeu cau; RepresentEnd: fps + don cache
    c = sub1(c, r"\tPD3DDEVICE->Clear\( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB\(0,0,0\), 1\.0f, 0L \);",
        crlf("""	PD3DDEVICE->Clear( 0, NULL, D3DCLEAR_TARGET, bClear ? (0xff000000 | (Color & 0x00ffffff)) : D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );	// %s""" % MARK), name="Begin clear")
    c = sub1(c, r"(\tPD3DDEVICE->Present\(NULL,NULL,NULL,NULL\);\r\n\})",
        lambda m: crlf("""	PD3DDEVICE->Present(NULL,NULL,NULL,NULL);

	// %s fps trung binh (EMA ~100 khung); chi don cache khi may khong dang chay cham (theo 2.0: >= 25 fps)
	DWORD dwNow = timeGetTime();
	if (m_dwLastPresent)
	{
		DWORD dwDt = dwNow - m_dwLastPresent;
		if (dwDt < 10) dwDt = 10;
		if (dwDt > 1000) dwDt = 1000;
		float fFps = 1000.0f / (float)dwDt;
		m_fFpsAvg = (m_fFpsAvg <= 0.0f) ? fFps : (m_fFpsAvg * 0.98f + fFps * 0.02f);
	}
	m_dwLastPresent = dwNow;
	if (m_fFpsAvg >= 25.0f)
		m_TextureResMgr.CheckBalanceFrame();
}""" % MARK), name="End present")

    wr("KRepresentShell3.cpp", c)
    print("KRepresentShell3.cpp OK")
else:
    print("KRepresentShell3.cpp da va")

# ============================================================ TextureResMgr.cpp
c = rd("TextureResMgr.cpp")
if MARK not in c:
    # ctor: ngan sach theo RAM
    c = sub1(c, r"\tMEMORYSTATUS stat;\r\n\tGlobalMemoryStatus \(&stat\);\r\n\tif\(stat\.dwTotalPhys <= 134217728\)\r\n    \{\r\n\t\tm_nBalanceNum = ISBP_BALANCE_NUM_DEF128;\r\n        m_nMaxReleaseCount = 16;\r\n    \}\r\n\telse if\(stat\.dwTotalPhys <= 134217728 \* 2\)\r\n    \{\r\n\t\tm_nBalanceNum = ISBP_BALANCE_NUM_DEF256;\r\n        m_nMaxReleaseCount = 32;\r\n    \}\r\n\telse\r\n    \{\r\n\t\tm_nBalanceNum = ISBP_BALANCE_NUM_DEF512;\r\n        m_nMaxReleaseCount = 64;\r\n    \}",
        crlf("""	// %s ngan sach cache texture theo RAM (2.0: 30/50/80/120 MB); may 4 GB+ cho rong hon vi texture 8888
	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof(stat);
	GlobalMemoryStatusEx(&stat);
	unsigned __int64 uPhysMB = stat.ullTotalPhys / (1024 * 1024);
	unsigned __int64 uBudgetMB = uPhysMB / 16;
	if (uBudgetMB < 30)  uBudgetMB = 30;
	if (uBudgetMB > 480) uBudgetMB = 480;
	if (g_nRep3CacheMB > 0)
		uBudgetMB = (unsigned __int64)g_nRep3CacheMB;
	m_nBalanceNum = (int32)(uBudgetMB * 1024 * 1024);
	m_nMaxReleaseCount = 64;
	m_uCheckPoint = 25;
	Rep3Log("[REP3] cache texture: RAM %%I64u MB -> ngan sach %%I64u MB", uPhysMB, uBudgetMB);""" % MARK), name="mgr ctor")
    # CheckBalance: thay toan bo than ham
    m = re.search(r"void TextureResMgr::CheckBalance\(\)\r\n\{.*?\r\n\}\r\n", c, re.S)
    assert m, "CheckBalance body"
    new_cb = crlf("""void TextureResMgr::CheckBalance()
{
	// %s theo represent3free.dll 2.0 (0x10024F80): duyet tu cuoi, bo qua tai nguyen vua ve khung truoc,
	// chi xet tai nguyen nghi > 10 s; moi luot BO DUNG MOT KHUNG texture (ReleaseAFrameData) - het khung
	// moi xoa ca tai nguyen. Khong con cu xoa hang loat 16/32/64 cai gay khung.
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	uint32 nTickCount = GetTickCount();
	for (int i = (int)m_TextureResList.size() - 1; i >= 0; i--)
	{
		ResNode& node = m_TextureResList[i];
		if (!node.m_bCacheable || !node.m_pTextureRes)
			continue;
		if (node.m_pTextureRes->m_bLastFrameUsed)
			continue;
		if ((nTickCount - node.m_nLastUsedTime) <= 10000)
			continue;
		m_nReleaseCount++;
		if (node.m_pTextureRes->ReleaseAFrameData())
			return;
		if (m_uTexCacheMemUsed >= node.m_pTextureRes->m_nTexMemUsed)
			m_uTexCacheMemUsed -= node.m_pTextureRes->m_nTexMemUsed;
		node.m_pTextureRes->Release();
		SAFE_DELETE(node.m_pTextureRes);
		m_TextureResList.erase(m_TextureResList.begin() + i);
		return;
	}
}

void TextureResMgr::CheckBalanceFrame()
{
	DWORD tmCur = timeGetTime();
	if ((tmCur - m_tmLastCheckBalance) <= m_uCheckPoint)
		return;
	m_tmLastCheckBalance = tmCur;
	CheckBalance();
}
""" % MARK)
    c = c[:m.start()] + new_cb + c[m.end():]
    # GetImage trigger: giu nguyen dieu kien (m_uTexCacheMemUsed > m_nBalanceNum, qua checkpoint)
    wr("TextureResMgr.cpp", c)
    print("TextureResMgr.cpp OK")
else:
    print("TextureResMgr.cpp da va")

# ============================================================ TextureRes.cpp
c = rd("TextureRes.cpp")
if MARK not in c:
    # ham doi RLE -> A8R8G8B8 (chen truoc 'inline void RenderToA4R4G4B4')
    conv = crlf("""// %s giai nen RLE spr -> A8R8G8B8: [n][alpha] roi n chi so palette neu alpha != 0 (alpha 0..255)
static void RenderToA8R8G8B8(DWORD* pDest, BYTE* pSrc, int nSrcLen, int nTotal, KPAL24* pPal, int nColors)
{
	BYTE*  p    = pSrc;
	BYTE*  pEnd = pSrc + nSrcLen;
	DWORD* d    = pDest;
	DWORD* dEnd = pDest + nTotal;
	while (p + 2 <= pEnd && d < dEnd)
	{
		int n = *p++;
		int a = *p++;
		if (a == 0)
		{
			for (int k = 0; k < n && d < dEnd; k++)
				*d++ = 0;
		}
		else
		{
			for (int k = 0; k < n && d < dEnd; k++)
			{
				if (p >= pEnd)
				{
					*d++ = 0;
					continue;
				}
				int idx = *p++;
				if (idx >= nColors)
					idx = 0;
				*d++ = ((DWORD)a << 24) | ((DWORD)pPal[idx].Red << 16) | ((DWORD)pPal[idx].Green << 8) | (DWORD)pPal[idx].Blue;
			}
		}
	}
	while (d < dEnd)
		*d++ = 0;
}

""" % MARK)
    c = sub1(c, r"(inline void RenderToA4R4G4B4\()", lambda m: conv + m.group(1), name="conv insert")

    # CreateTexture16Bit: thay than ham (tu chu ky toi 'error:' cua chinh no)
    m = re.search(r"void TextureResSpr::CreateTexture16Bit\(const char\* szImage, int32 nFrame\)\r\n\{.*?\r\nerror:\r\n\tSAFE_DELETE_ARRAY\(pTempData\);\r\n\treturn;\r\n\}\r\n", c, re.S)
    assert m, "CreateTexture16Bit body"
    new_ct = crlf("""void TextureResSpr::CreateTexture16Bit(const char* szImage, int32 nFrame)
{
	int i, j;

	if(nFrame < 0 || nFrame >= m_nFrameNum)
		return;

	if(m_pFrameInfo[nFrame].texInfo[0].pTexture)
		return;

	SplitTexture(nFrame);

	// %s texture 8888 (dung mau palette 24 bit nhu Represent2) hoac 4444 nhu cu
	int nBpp = g_nRep3Tex32 ? 4 : 2;
	D3DFORMAT eFmt = g_nRep3Tex32 ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;
	int nW = m_pFrameInfo[nFrame].nWidth;
	int nH = m_pFrameInfo[nFrame].nHeight;
	if (nW <= 0 || nH <= 0)
		return;

	BYTE *pTempData = NULL;
	try {
		pTempData = new BYTE[nW * nH * nBpp];
	}
	catch (const std::bad_alloc&) {
		return;
	}
	if(!pTempData)
		return;

	if (g_nRep3Tex32)
		RenderToA8R8G8B8((DWORD*)pTempData, m_pFrameInfo[nFrame].pRawData, m_pFrameInfo[nFrame].nRawDataLen,
						nW * nH, m_pPal24, (int)m_nColors);
	else
		RenderToA4R4G4B4((WORD*)pTempData, m_pFrameInfo[nFrame].pRawData, nW, nH, (BYTE*)m_pPal16);

	for(i=0; i<m_pFrameInfo[nFrame].nTexNum; i++)
	{
		m_nTexMemUsed += m_pFrameInfo[nFrame].texInfo[i].nWidth * m_pFrameInfo[nFrame].texInfo[i].nHeight * nBpp;

		SAFE_RELEASE(m_pFrameInfo[nFrame].texInfo[i].pTexture);
		if (FAILED(PD3DDEVICE->CreateTexture(m_pFrameInfo[nFrame].texInfo[i].nWidth, m_pFrameInfo[nFrame].texInfo[i].nHeight, 1,
								0, eFmt, D3DPOOL_MANAGED, &m_pFrameInfo[nFrame].texInfo[i].pTexture, NULL)))
			goto error;

		D3DLOCKED_RECT LockedRect;
		if (FAILED(m_pFrameInfo[nFrame].texInfo[i].pTexture->LockRect(0, &LockedRect, NULL, 0)))
			goto error;

		BYTE *pTexData = (BYTE*)LockedRect.pBits;
		BYTE *pTp = pTempData + (m_pFrameInfo[nFrame].texInfo[i].nFrameY * nW +
					m_pFrameInfo[nFrame].texInfo[i].nFrameX) * nBpp;
		for(j=0; j<m_pFrameInfo[nFrame].texInfo[i].nFrameHeight; j++)
		{
			memcpy(pTexData, pTp, m_pFrameInfo[nFrame].texInfo[i].nFrameWidth * nBpp);
			pTexData += LockedRect.Pitch;
			pTp += nW * nBpp;
		}

		m_pFrameInfo[nFrame].texInfo[i].pTexture->UnlockRect(0);
	}

	SAFE_DELETE_ARRAY(pTempData);
	if(m_pHeader)
	{
		SprReleaseFrame((SPRFRAME*)m_pFrameInfo[nFrame].pFrame);
		m_pFrameInfo[nFrame].pFrame = NULL;
		m_pFrameInfo[nFrame].pRawData = NULL;
	}
	return;

error:
	SAFE_DELETE_ARRAY(pTempData);
	return;
}
""" % MARK)
    c = c[:m.start()] + new_ct + c[m.end():]

    # ReleaseAFrameData: bpp
    c = sub1(c, r"m_nTexMemUsed -= m_pFrameInfo\[i\]\.texInfo\[j\]\.nWidth \* m_pFrameInfo\[i\]\.texInfo\[j\]\.nHeight \* 2;",
        "m_nTexMemUsed -= m_pFrameInfo[i].texInfo[j].nWidth * m_pFrameInfo[i].texInfo[j].nHeight * (g_nRep3Tex32 ? 4 : 2);	// " + MARK,
        name="ReleaseAFrameData bpp")

    # SplitTexture: nhanh NPOT
    c = sub1(c, r"(int TextureResSpr::SplitTexture\(uint32 nFrame\)\r\n\{\r\n\tint nWidth = m_pFrameInfo\[nFrame\]\.nWidth;\r\n\tint nHeight = m_pFrameInfo\[nFrame\]\.nHeight;\r\n)",
        lambda m: m.group(1) + crlf("""	// %s card ho tro NPOT (da thu tao 33x17 luc tao thiet bi): 1 texture dung co khung, khong cat 1/2/4 (theo 2.0)
	if (g_bNpotOK && g_nRep3Npot && nWidth > 0 && nHeight > 0 && nWidth <= g_nMaxTexW && nHeight <= g_nMaxTexH)
	{
		m_pFrameInfo[nFrame].nTexNum = 1;
		m_pFrameInfo[nFrame].texInfo[0].nWidth = nWidth;
		m_pFrameInfo[nFrame].texInfo[0].nHeight = nHeight;
		m_pFrameInfo[nFrame].texInfo[0].nFrameX = 0;
		m_pFrameInfo[nFrame].texInfo[0].nFrameY = 0;
		m_pFrameInfo[nFrame].texInfo[0].nFrameWidth = nWidth;
		m_pFrameInfo[nFrame].texInfo[0].nFrameHeight = nHeight;
		return nWidth * nHeight * (g_nRep3Tex32 ? 4 : 2);
	}
""" % MARK), name="SplitTexture npot")
    wr("TextureRes.cpp", c)
    print("TextureRes.cpp OK")
else:
    print("TextureRes.cpp da va")

# ============================================================ D3D_Device.cpp
c = rd("D3D_Device.cpp")
if MARK not in c:
    c = sub1(c, r"(\tif \(FAILED\(m_pD3DDevice->GetDeviceCaps\(&m_DeviceCaps\)\)\)\r\n\t\{\r\n\t\tFreeAll\(\); \r\n\t\treturn false; \r\n\t\}\r\n)",
        lambda m: m.group(1) + crlf("""
	// %s do NPOT nhu represent3free.dll 2.0 (0x10001700): caps + tao thu texture 33x17 A8R8G8B8
	g_nMaxTexW = (int)m_DeviceCaps.MaxTextureWidth;
	g_nMaxTexH = (int)m_DeviceCaps.MaxTextureHeight;
	if (g_nMaxTexW <= 0) g_nMaxTexW = 1024;
	if (g_nMaxTexH <= 0) g_nMaxTexH = 1024;
	g_bNpotOK = false;
	{
		bool bFull = (m_DeviceCaps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0;
		bool bCond = (m_DeviceCaps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0;
		if (g_nRep3Npot && (bFull || bCond))
		{
			LPDIRECT3DTEXTURE9 pProbe = NULL;
			HRESULT hrProbe = m_pD3DDevice->CreateTexture(33, 17, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pProbe, NULL);
			if (SUCCEEDED(hrProbe) && pProbe)
			{
				pProbe->Release();
				g_bNpotOK = true;
			}
			Rep3Log("[REP3] NPOT: full=%%d conditional=%%d probe=0x%%08x -> %%s", (int)bFull, (int)bCond, (unsigned int)hrProbe, g_bNpotOK ? "DUNG NPOT" : "cat POT");
		}
		else
			Rep3Log("[REP3] NPOT: card khong ho tro (TextureCaps=0x%%08x) hoac Rep3Npot=0 -> cat POT", (unsigned int)m_DeviceCaps.TextureCaps);
	}
	Rep3Log("[REP3] card: %%s | MaxTex %%dx%%d | VertexProc %%s | vsync=%%d",
		pDevice && pDevice->strDesc ? pDevice->strDesc : "?", g_nMaxTexW, g_nMaxTexH,
		(BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) ? "mixed" : "software", g_nRep3Vsync);
"""  % MARK), name="npot probe")
    c = sub1(c, r"(\tPresentationParam\.FullScreen_RefreshRateInHz\t\t= D3DPRESENT_RATE_DEFAULT;[^\r\n]*\r\n)",
        lambda m: m.group(1) + crlf("""	// %s Present ngay, khong cho vsync (client tu dieu nhip PaintFps); Rep3Vsync=1 de bat lai
	PresentationParam.PresentationInterval				= g_nRep3Vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
""" % MARK), name="vsync")
    wr("D3D_Device.cpp", c)
    print("D3D_Device.cpp OK")
else:
    print("D3D_Device.cpp da va")

print("DONE")
