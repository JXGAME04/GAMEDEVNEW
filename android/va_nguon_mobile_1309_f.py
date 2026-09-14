# -*- coding: utf-8 -*-
# [XOANEN 13/09] + [NENTRUOC 13/09 b]: sau khi doc log Fold 7 19:33-20:06 (ban 109131908):
#   * [PGND] van 1156 lan x 17 ms nhung 've len anh' chi 0,6 ms va 1002/1156 lan nam o nhanh XA -> 17 ms KHONG o ghep ma o
#     ClearImageData: LockData + memset + UnLockData -> UpdateTexture -> UpdateSurface khoa DICH -> CTexGpu::LockRect thay
#     render target da ve (m_bGpuNewer) -> ReadbackTexture = SubmitFrame + doc GPU ve CPU DONG BO. Sua: anh nen vung
#     (_*PlaceGround*_) xoa TREN GPU (SetRenderTarget + Clear + tra lai), khong khoa CPU. Do [PGND-X] >= 4 ms cho ca hai duong.
#   * CPU loi 0 len 1,9-2,1 GHz, 3,3 W (truoc 1 GHz, 2,1 W): vong hoi cua JxNenTruoc goi 66 o x moi khung x moi vung dang cho.
#     Sua: hoi lan dau du, ghi chi so o con cho; lan sau chi hoi lai cac o do va toi da moi 32 ms.
#   * Tep o nen vao hang TRUOC cua luong nen (nguon 3), khong xep sau moi anh nap truoc khac.
# Chi JX_MOBILE, ban PC khong doi. Chay lai vo hai (chay sau va_nguon_mobile_1309_e.py).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[XOANEN 13/09]"
DAUB = "[NENTRUOC 13/09 b]"
NL = "\r\n"


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    assert NL in s, p
    return s


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def cao(s):
    return sum(1 for c in s if ord(c) >= 128)


def thay(s, cu, moi, ten, n=1):
    if s.count(cu) != n:
        raise SystemExit("%s: neo '%s...' thay %d lan, can %d" % (ten, cu.strip()[:60], s.count(cu), n))
    return s.replace(cu, moi)


def va(p, viec):
    s = doc(p)
    if DAU in s or DAUB in s:
        print("da va roi:", p)
        return
    c0 = cao(s)
    s = viec(s)
    assert cao(s) == c0, p + ": byte cao doi"
    ghi(p, s)
    print("da va:", p)


# ---------------------------------------------------------------- Represent3: ClearImageData xoa tren GPU
def v_shell(s):
    cu = ("\tTextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(" + NL
          + "\t\tpszImage, uImage, nImagePosition, 0, ISI_T_BITMAP16);" + NL
          + "\tif (pBitmap)" + NL + "\t{" + NL + "\t\tbyte* pBuffer;" + NL)
    moi = ("\tTextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(" + NL
           + "\t\tpszImage, uImage, nImagePosition, 0, ISI_T_BITMAP16);" + NL
           + "\tif (pBitmap)" + NL + "\t{" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t\t// " + DAU + " anh nen vung (_*PlaceGround*_): xoa TREN GPU (Clear vao dich ve) thay vi LockData + memset + UpdateTexture." + NL
           + "\t\t// UpdateSurface khoa DICH -> CTexGpu::LockRect thay render target da ve (m_bGpuNewer) -> ReadbackTexture = SubmitFrame + doc GPU" + NL
           + "\t\t// ve CPU DONG BO ~17 ms (Fold 7 13/09: 1002/1156 lan [PGND] o nhanh XA la cai nay, 've len anh' chi 0,6 ms). Ban CPU cua anh" + NL
           + "\t\t// nen khong ai doc. ScenePlaceMapC / UiPlayVideo giu duong cu (ten khac)." + NL
           + "\t\tif (strncmp(pszImage, \"_*PlaceGround*_\", 15) == 0 && pBitmap->m_FrameInfo.texInfo[0].pTexture)" + NL
           + "\t\t{" + NL
           + "\t\t\tLARGE_INTEGER jxX0, jxX1, jxXF; QueryPerformanceFrequency(&jxXF); QueryPerformanceCounter(&jxX0);" + NL
           + "\t\t\tIDirect3DSurface9 *pXDes = NULL, *pXOld = NULL;" + NL
           + "\t\t\tbool bXong = false;" + NL
           + "\t\t\tif (SUCCEEDED(PD3DDEVICE->GetRenderTarget(0, &pXOld)) && SUCCEEDED(pBitmap->m_FrameInfo.texInfo[0].pTexture->GetSurfaceLevel(0, &pXDes)))" + NL
           + "\t\t\t{" + NL
           + "\t\t\t\tif (SUCCEEDED(PD3DDEVICE->SetRenderTarget(0, pXDes)))" + NL
           + "\t\t\t\t{" + NL
           + "\t\t\t\t\tPD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0L);" + NL
           + "\t\t\t\t\tPD3DDEVICE->SetRenderTarget(0, pXOld);" + NL
           + "\t\t\t\t\tbXong = true;" + NL
           + "\t\t\t\t}" + NL
           + "\t\t\t}" + NL
           + "\t\t\tif (pXDes) pXDes->Release();" + NL
           + "\t\t\tif (pXOld) pXOld->Release();" + NL
           + "\t\t\tQueryPerformanceCounter(&jxX1);" + NL
           + "\t\t\tconst double dXMs = jxXF.QuadPart ? (double)(jxX1.QuadPart - jxX0.QuadPart) * 1000.0 / (double)jxXF.QuadPart : 0.0;" + NL
           + "\t\t\tif (dXMs >= 4.0) Rep3Log(\"[PGND-X] xoa nen %s tren GPU: %.1f ms (%s)\", pszImage, dXMs, bXong ? \"xong\" : \"HONG -> memset\");" + NL
           + "\t\t\tif (bXong)" + NL
           + "\t\t\t\treturn;" + NL
           + "\t\t}" + NL
           + "\t\tLARGE_INTEGER jxC0, jxC1, jxCF; QueryPerformanceFrequency(&jxCF); QueryPerformanceCounter(&jxC0);\t// " + DAU + " do duong cu (LockData/UpdateTexture)" + NL
           + "#endif" + NL
           + "\t\tbyte* pBuffer;" + NL)
    s = thay(s, cu, moi, "ClearImageData dau")
    cu = "\t\t\tpBitmap->UnLockData();" + NL + "\t\t}" + NL + "\t}" + NL + "}" + NL + NL + NL + "void KRepresentShell3::FreeAllImage()" + NL
    moi = ("\t\t\tpBitmap->UnLockData();" + NL + "\t\t}" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t\tQueryPerformanceCounter(&jxC1);" + NL
           + "\t\t{ const double dCMs = jxCF.QuadPart ? (double)(jxC1.QuadPart - jxC0.QuadPart) * 1000.0 / (double)jxCF.QuadPart : 0.0; if (dCMs >= 4.0) Rep3Log(\"[PGND-X] xoa anh %s bang CPU (LockData/UpdateTexture): %.1f ms\", pszImage, dCMs); }\t// " + DAU + NL
           + "#endif" + NL
           + "\t}" + NL + "}" + NL + NL + NL + "void KRepresentShell3::FreeAllImage()" + NL)
    s = thay(s, cu, moi, "ClearImageData cuoi")
    return s


# ---------------------------------------------------------------- Represent3: tep o nen vao hang TRUOC
def v_mgr(s):
    cu = "\tif (!NapNenGiao(pszImage, uImage, nType, true))" + NL + "\t\treturn 0;" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "\tif (!NapNenGiao(pszImage, uImage, nType, nNguon != 3))\t// " + DAUB + " nguon 3 = o nen dat cua vung ke ben: hang TRUOC (can som), khong xep sau moi anh nap truoc" + NL
           + "\t\treturn 0;" + NL
           + "#else" + NL
           + cu
           + "#endif" + NL)
    return thay(s, cu, moi, "NapTruoc NapNenGiao")


# ---------------------------------------------------------------- Core: KScenePlaceRegionC.h
def v_region_h(s):
    cu = ("#ifdef JX_MOBILE" + NL
          + "\tbool JxNenChuaSan();\t// [NENTRUOC 13/09] vung ke ben/xa: xin luong nen chuan bi khung cac o; true = con dang chuan bi (hoan ghep sang khung sau), qua 1,5 s -> false" + NL
          + "\tint  JxNenTruoc();\t\t// xin/hoi tung o (grunode + object); tra so o con 'dang chuan bi'" + NL
          + "\tunsigned m_uJxNenXinLuc;\t// timeGetTime luc xin lan dau (0 = chua)" + NL
          + "#endif" + NL)
    moi = ("#ifdef JX_MOBILE" + NL
           + "\tbool JxNenChuaSan();\t// [NENTRUOC 13/09] vung ke ben/xa: xin luong nen chuan bi khung cac o; true = con dang chuan bi (hoan ghep sang khung sau), qua 1,5 s -> false" + NL
           + "\tint  JxNenTruoc();\t\t// " + DAUB + " lan dau hoi du, ghi chi so o con cho; lan sau chi hoi lai cac o do, toi da moi 32 ms; tra so o con 'dang chuan bi'" + NL
           + "\tvoid JxNenDatLai() { m_uJxNenXinLuc = 0; m_uJxNenHoiLuc = 0; m_nJxNenCho = 0; m_bJxNenDaHoi = false; }" + NL
           + "\tunsigned m_uJxNenXinLuc;\t// timeGetTime luc xin lan dau (0 = chua)" + NL
           + "\tunsigned m_uJxNenHoiLuc;\t// " + DAUB + " lan hoi gan nhat" + NL
           + "\tint      m_nJxNenCho;\t\t// so o con cho trong m_jxNenCho (-1 = qua nhieu, hoi lai tat ca)" + NL
           + "\tbool     m_bJxNenDaHoi;\t\t// da hoi du mot lan" + NL
           + "\tunsigned short m_jxNenCho[640];\t// chi so o (grunode roi object) con 'dang chuan bi', tang dan" + NL
           + "#endif" + NL)
    return thay(s, cu, moi, "KScenePlaceRegionC.h khoi NENTRUOC")


# ---------------------------------------------------------------- Core: KScenePlaceRegionC.cpp
def v_region_cpp(s):
    s = thay(s, "\tm_uJxNenXinLuc = 0;\t// [NENTRUOC 13/09]" + NL, "\tJxNenDatLai();\t// [NENTRUOC 13/09] " + DAUB + NL, "dat lai 2 cho", 2)
    s = thay(s, "\t\tm_uJxNenXinLuc = 0;\t// [NENTRUOC 13/09] bo anh nen -> xin lai" + NL, "\t\tJxNenDatLai();\t// [NENTRUOC 13/09] bo anh nen -> xin lai" + NL, "dat lai bo anh")
    s = thay(s, "\tm_uJxNenXinLuc = 0;\t// [NENTRUOC 13/09] ve lai -> xin lai" + NL, "\tJxNenDatLai();\t// [NENTRUOC 13/09] ve lai -> xin lai" + NL, "dat lai ve lai")
    # thay ca ham JxNenTruoc
    a = s.find("int KScenePlaceRegionC::JxNenTruoc()")
    b = s.find("// [NENTRUOC 13/09] Vung ke ben / xa: lan dau xin luong nen", a)
    assert a > 0 and b > a, "khong thay ham JxNenTruoc"
    dau_cmt = s.rfind("// [NENTRUOC 13/09] Xin / hoi tung o cua vung", 0, a)
    assert dau_cmt > 0 and a - dau_cmt < 400
    moi = NL.join([
        "// [NENTRUOC 13/09] Xin / hoi tung o cua vung (cung danh sach ma PrerenderGround ghep). Tra so o con 'dang chuan bi' (2); 0 (khong duoc) coi nhu san.",
        "// " + DAUB + " Lan dau hoi DU va ghi chi so o con cho vao m_jxNenCho; lan sau chi hoi lai cac o do, va toi da moi 32 ms - truoc do hoi",
        "// 66 o x moi khung x moi vung dang cho lam CPU loi 0 tren Fold 7 len 1,9-2,1 GHz (3,3 W) trong Tong Kim.",
        "int KScenePlaceRegionC::JxNenTruoc()",
        "{",
        "\tconst unsigned uNow = (unsigned)timeGetTime();",
        "\tif (m_bJxNenDaHoi && m_nJxNenCho == 0)",
        "\t\treturn 0;",
        "\tif (m_bJxNenDaHoi && uNow - m_uJxNenHoiLuc < 32)",
        "\t\treturn m_nJxNenCho < 0 ? 1 : m_nJxNenCho;",
        "\tm_uJxNenHoiLuc = uNow;",
        "\tconst bool bTatCa = !m_bJxNenDaHoi || m_nJxNenCho < 0;",
        "\tm_bJxNenDaHoi = true;",
        "\tint nCho = 0, nGhi = 0, k = 0;",
        "\tunsigned short nIdx = 0;",
        "\tbool bTran = false;",
        "\tchar szTen[MAX_PATH];",
        "\tKSPRCrunode* pGrunode = m_GroundLayerData.pGrunodes;",
        "\tfor (unsigned int n = 0; pGrunode && n < m_GroundLayerData.uNumGrunode; n++, nIdx++)",
        "\t{",
        "\t\tconst bool bHoi = bTatCa || (k < m_nJxNenCho && m_jxNenCho[k] == nIdx);",
        "\t\tif (!bTatCa && bHoi) k++;",
        "\t\tif (bHoi)",
        "\t\t{",
        "\t\t\tint nLen = (int)pGrunode->Param.nFileNameLen;",
        "\t\t\tif (nLen >= MAX_PATH) nLen = MAX_PATH - 1;",
        "\t\t\tif (nLen > 0)",
        "\t\t\t{",
        "\t\t\t\tmemcpy(szTen, pGrunode->szImgName, nLen); szTen[nLen] = 0;",
        "\t\t\t\tif (Rep3NenTruocKhung(szTen, pGrunode->Param.nFrame) == 2) { nCho++; if (nGhi < 640) m_jxNenCho[nGhi++] = nIdx; else bTran = true; }",
        "\t\t\t}",
        "\t\t}",
        "\t\tpGrunode = (KSPRCrunode*)(((char*)pGrunode) + sizeof(KSPRCrunode::KSPRCrunodeParam) + pGrunode->Param.nFileNameLen);",
        "\t}",
        "\tKSPRCoverGroundObj* pObj = m_GroundLayerData.pObjects;",
        "\tfor (unsigned int n = 0; pObj && n < m_GroundLayerData.uNumObject; n++, pObj++, nIdx++)",
        "\t{",
        "\t\tconst bool bHoi = bTatCa || (k < m_nJxNenCho && m_jxNenCho[k] == nIdx);",
        "\t\tif (!bTatCa && bHoi) k++;",
        "\t\tif (bHoi && Rep3NenTruocKhung(pObj->szImage, pObj->nFrame) == 2) { nCho++; if (nGhi < 640) m_jxNenCho[nGhi++] = nIdx; else bTran = true; }",
        "\t}",
        "\tm_nJxNenCho = bTran ? -1 : nGhi;",
        "\treturn nCho;",
        "}",
        "",
        "",
    ])
    s = s[:dau_cmt] + moi + s[b:]
    return s


# ---------------------------------------------------------------- lop gia lap: Clear roi doi dich ve ma khong ve gi thi van phai xoa
def v_dev(s):
    cu = ("\t\tif (c.type == RGCMD_TARGET)" + NL + "\t\t{" + NL
          + "\t\t\tif (pass) { SDL_EndGPURenderPass(pass); pass = NULL; }" + NL
          + "\t\t\tif (c.pTarget)")
    moi = ("\t\tif (c.type == RGCMD_TARGET)" + NL + "\t\t{" + NL
           + "\t\t\tif (pass) { SDL_EndGPURenderPass(pass); pass = NULL; }" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t\t\tif (bPendingClear && pCur)" + NL
           + "\t\t\t{\t// " + DAU + " Clear roi doi dich ve ma khong co lenh ve theo sau (xoa anh nen vung tren GPU): truoc day bi bo (bPendingClear = false) -> mo mot pass rong load_op CLEAR" + NL
           + "\t\t\t\tSDL_GPUColorTargetInfo ciX; memset(&ciX, 0, sizeof(ciX));" + NL
           + "\t\t\t\tciX.texture = pCur; ciX.load_op = SDL_GPU_LOADOP_CLEAR; ciX.store_op = SDL_GPU_STOREOP_STORE;" + NL
           + "\t\t\t\tciX.clear_color.a = ((clearColor >> 24) & 0xFF) / 255.0f; ciX.clear_color.r = ((clearColor >> 16) & 0xFF) / 255.0f; ciX.clear_color.g = ((clearColor >> 8) & 0xFF) / 255.0f; ciX.clear_color.b = (clearColor & 0xFF) / 255.0f;" + NL
           + "\t\t\t\tSDL_GPURenderPass* pX = SDL_BeginGPURenderPass(cb, &ciX, 1, NULL);" + NL
           + "\t\t\t\tif (pX) SDL_EndGPURenderPass(pX);" + NL
           + "\t\t\t\tjxK.uPass++;" + NL
           + "\t\t\t}" + NL
           + "#endif" + NL
           + "\t\t\tif (c.pTarget)")
    return thay(s, cu, moi, "RGCMD_TARGET pending clear")


va("Sources/Represent/Represent3/D3D9onGPUDev.cpp", v_dev)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", v_shell)
va("Sources/Represent/Represent3/TextureResMgr.cpp", v_mgr)
va("Sources/Core/Src/Scene/KScenePlaceRegionC.h", v_region_h)
va("Sources/Core/Src/Scene/KScenePlaceRegionC.cpp", v_region_cpp)
