# -*- coding: utf-8 -*-
"""goi_va_loctg_0909.py - [LOCTG 09/09] Bo loc thoi gian luc trinh khung (temporal filter) - cach chung
cho moi may, khong buoc vao tan so man hinh.

Chu: "fix sao de nhieu nguoi choi may yeu - manh van duoc; tim cong nghe moi de fix chu fix doi pho
the nay khong duoc". Buoc theo khung (NhipTheGioi) buoc vao Hz man hinh: 60 Hz -> K=1 vo tac dung,
240 Hz -> K=4 giat, may yeu 50 fps -> khac nua. Khong phai cach chung.

Cach chung: loc thoi gian voi HANG SO THOI GIAN VAT LY tau (ms), ap ngay truoc Present trong lop D3D11:
   back = lerp(back, lich_su, a),  a = exp(-dt / tau),  lich_su = back sau khi tron.
 - dt = thoi gian tu lan Present truoc => tu thich nghi: 60 Hz a~0,12 (gan tat), 143 Hz a~0,42, 240 Hz
   a~0,59; may yeu fps thap -> a -> 0. Khong doi hanh vi tren may khong co van de.
 - Lam tin hieu dua ra tam nen bien thien dan thay vi nhay bac => diem anh kip chuyen, het "am";
   chuyen dong van moi khung (khong giat). Vat dang chay co vet mo ngan = dung nhu mat nguoi / camera
   thay, khong lech mau.
 - Chi phi: 2 CopyResource + 1 tam giac toan man o 1024x768 ~ 0,1 ms.
 - [Client] Rep3LocMs = tau ms (mac dinh 8; 0 = tat). NhipTheGioi mac dinh ve 0 (giu lam cong tac thu).
Shader rieng (Rep3LocTG11.hlsl -> _vs.h/_ps.h bang fxc), pass rieng, xong lam mat hieu luc cache trang
thai cua lop de lan ve sau gan lai het.
"""
import io
import sys

NL = "\r\n"
T = "\t"
R = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
F_I = R + r"\D3D9on11i.h"
F_H = R + r"\D3D9on11.h"
F_D = R + r"\D3D9on11Dev.cpp"
F_S = R + r"\KRepresentShell3.cpp"
F_CS = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\CoreShell.cpp"


LF0 = {}     # so LF DON cua tung tep luc doc
CRLF0 = {}   # so CRLF luc doc: 0 => tep LF thuan (lop D3D9on11* la LF), giu dung kieu do khi ghi
EOL = {}     # kieu xuong dong cua tung tep


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    CRLF0[p] = s.count("\r\n")
    LF0[p] = s.count("\n") - CRLF0[p]
    EOL[p] = "\r\n" if CRLF0[p] > 0 else "\n"
    return s


def ghi(p, s, h0, ten):
    if CRLF0.get(p, 0) == 0:
        s = s.replace("\r\n", "\n")            # tep LF thuan: moi thu chen vao (soan bang CRLF) doi ve LF
        ok = (s.count("\r\n") == 0)
    else:
        ok = (s.count("\n") - s.count("\r\n") == LF0.get(p, 0))   # tep CRLF: khong sinh LF don moi
    if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or not ok:
        print("FAIL ma hoa %s (byte cao %d/%d, CRLF %d, LF don %d/%d)" % (ten, sum(1 for c in s if ord(c) >= 0x80), h0, s.count("\r\n"), s.count("\n") - s.count("\r\n"), LF0.get(p, 0))); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK %s (%s)" % (ten, "LF" if CRLF0.get(p, 0) == 0 else "CRLF"))


def rep(s, old, new, ten, cho=1):
    n = s.count(old)
    if n != cho:
        print("FAIL neo %s: thay %d, can %d" % (ten, n, cho)); sys.exit(1)
    return s.replace(old, new)


def chen_sau_dong(s, chua, them, ten, eol=None):
    """chen 'them' ngay sau DONG dau tien chua chuoi 'chua' (dung khi khoang trang trong dong khong chac).
    Tach theo EOL that cua tep (LF hay CRLF); 'them' soan bang CRLF, ghi() se doi ve LF neu tep la LF."""
    if eol is None:
        eol = "\r\n" if s.count("\r\n") > 0 else "\n"
    dong = s.split(eol)
    idx = [i for i, l in enumerate(dong) if chua in l]
    if len(idx) != 1:
        print("FAIL dong %s: thay %d, can 1" % (ten, len(idx))); sys.exit(1)
    dong.insert(idx[0] + 1, them)
    return eol.join(dong)


# ---------------------------------------------------------------- D3D9on11i.h
s = doc(F_I); h0 = sum(1 for c in s if ord(c) >= 0x80)
if "m_pLocHist" not in s:
    s = chen_sau_dong(s, "m_pLastFrame;", NL.join([
        T + "ID3D11Texture2D*        m_pLocCur;" + T + "// [LOCTG 09/09] ban sao khung hien tai (co SRV)",
        T + "ID3D11ShaderResourceView* m_pLocCurSrv;",
        T + "ID3D11Texture2D*        m_pLocHist;" + T + "// [LOCTG] lich su da tron (co SRV)",
        T + "ID3D11ShaderResourceView* m_pLocHistSrv;",
        T + "ID3D11VertexShader*     m_pLocVS;",
        T + "ID3D11PixelShader*      m_pLocPS;",
        T + "ID3D11Buffer*           m_pLocCb;",
        T + "double                  m_dLocLast;" + T + "// thoi diem Present truoc (ms) de tinh dt",
    ]), "I thanh vien")
    s = chen_sau_dong(s, "UpdateLastFrame();", NL.join([
        T + "void    LocThoiGian();" + T + "// [LOCTG 09/09] loc thoi gian, goi ngay truoc Present",
        T + "void    LocRelease();",
    ]), "I ham")
    ghi(F_I, s, h0, "D3D9on11i.h")
else:
    print("D3D9on11i.h da co")

# ---------------------------------------------------------------- D3D9on11.h extern
s = doc(F_H); h0 = sum(1 for c in s if ord(c) >= 0x80)
if "g_nRep3LocMs" not in s:
    s = chen_sau_dong(s, "extern int      g_nRep3Pal;",
        "extern int      g_nRep3LocMs;        // [LOCTG 09/09] [Client] Rep3LocMs: hang so thoi gian bo loc trinh khung (ms), 0 = tat",
        "H extern")
    ghi(F_H, s, h0, "D3D9on11.h")
else:
    print("D3D9on11.h da co")

# ---------------------------------------------------------------- D3D9on11Dev.cpp
s = doc(F_D); h0 = sum(1 for c in s if ord(c) >= 0x80)
if "LocThoiGian" not in s:
    s = rep(s, '#include "Rep3Shaders11_ps.h"',
            '#include "Rep3Shaders11_ps.h"' + NL + '#include "Rep3LocTG11_vs.h"' + T + '// [LOCTG 09/09]' + NL + '#include "Rep3LocTG11_ps.h"' + NL + '#include <math.h>',
            "D include")
    s = rep(s, "m_pLastFrame = NULL; m_pStaging = NULL;",
            "m_pLastFrame = NULL; m_pStaging = NULL;" + NL +
            T + "m_pLocCur = NULL; m_pLocCurSrv = NULL; m_pLocHist = NULL; m_pLocHistSrv = NULL; m_pLocVS = NULL; m_pLocPS = NULL; m_pLocCb = NULL; m_dLocLast = 0.0;" + T + "// [LOCTG 09/09]",
            "D ctor")
    s = rep(s, "R11_SAFE_RELEASE(m_pLastFrame); R11_SAFE_RELEASE(m_pStaging);",
            "R11_SAFE_RELEASE(m_pLastFrame); R11_SAFE_RELEASE(m_pStaging);" + NL +
            T + "LocRelease();" + T + "// [LOCTG 09/09] texture theo kich thuoc back buffer -> tao lai khi doi",
            "D release")
    E = EOL[F_D]   # tep nay la LF thuan: neo hai dong phai dung EOL that
    s = rep(s, T + "UpdateLastFrame();" + E + T + "UINT interval =",
            T + "if (g_nRep3LocMs > 0)" + E + T + T + "LocThoiGian();" + T + "// [LOCTG 09/09] tron voi lich su TRUOC khi chup / trinh" + E +
            T + "UpdateLastFrame();" + E + T + "UINT interval =",
            "D Present")
    body = NL.join([
        "// [LOCTG 09/09] Bo loc thoi gian luc trinh khung: back = lerp(back, lich_su, a), a = exp(-dt/tau).",
        "// Xem dau tep Rep3LocTG11.hlsl. Pass rieng (shader rieng, khong vertex buffer); xong lam mat hieu luc",
        "// cache trang thai cua lop de lan ve sau gan lai het (ApplyComputed).",
        "unsigned g_uRep3LocKhung = 0;",
        "void CDev11::LocRelease()",
        "{",
        T + "R11_SAFE_RELEASE(m_pLocCurSrv); R11_SAFE_RELEASE(m_pLocCur);",
        T + "R11_SAFE_RELEASE(m_pLocHistSrv); R11_SAFE_RELEASE(m_pLocHist);",
        T + "m_dLocLast = 0.0;",
        "}",
        "",
        "void CDev11::LocThoiGian()",
        "{",
        T + "if (!m_pBackTex || !m_pBackRtv || !m_pDev || !m_pCtx)",
        T + T + "return;",
        T + "LARGE_INTEGER q, f; QueryPerformanceCounter(&q); QueryPerformanceFrequency(&f);",
        T + "const double dNow = (double)q.QuadPart * 1000.0 / (double)f.QuadPart;",
        T + "const double dt = (m_dLocLast > 0.0) ? (dNow - m_dLocLast) : 1000.0;",
        T + "m_dLocLast = dNow;",
        T + "if (!m_pLocVS)",
        T + "{",
        T + T + "if (FAILED(m_pDev->CreateVertexShader(g_Rep3LocVS11, sizeof(g_Rep3LocVS11), NULL, &m_pLocVS))) { m_pLocVS = NULL; g_nRep3LocMs = 0; R11Log(\"[LOCTG] VS that bai -> tat\"); return; }",
        T + T + "if (FAILED(m_pDev->CreatePixelShader(g_Rep3LocPS11, sizeof(g_Rep3LocPS11), NULL, &m_pLocPS))) { m_pLocPS = NULL; g_nRep3LocMs = 0; R11Log(\"[LOCTG] PS that bai -> tat\"); return; }",
        T + T + "D3D11_BUFFER_DESC bd; memset(&bd, 0, sizeof(bd));",
        T + T + "bd.Usage = D3D11_USAGE_DYNAMIC; bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; bd.ByteWidth = 16;",
        T + T + "if (FAILED(m_pDev->CreateBuffer(&bd, NULL, &m_pLocCb))) { m_pLocCb = NULL; g_nRep3LocMs = 0; R11Log(\"[LOCTG] cbuffer that bai -> tat\"); return; }",
        T + "}",
        T + "if (!m_pLocCur || !m_pLocHist)",
        T + "{",
        T + T + "D3D11_TEXTURE2D_DESC td; m_pBackTex->GetDesc(&td);",
        T + T + "td.BindFlags = D3D11_BIND_SHADER_RESOURCE; td.Usage = D3D11_USAGE_DEFAULT; td.CPUAccessFlags = 0; td.MiscFlags = 0; td.SampleDesc.Count = 1; td.SampleDesc.Quality = 0;",
        T + T + "if (!m_pLocCur && (FAILED(m_pDev->CreateTexture2D(&td, NULL, &m_pLocCur)) || FAILED(m_pDev->CreateShaderResourceView(m_pLocCur, NULL, &m_pLocCurSrv))))",
        T + T + "{ LocRelease(); g_nRep3LocMs = 0; R11Log(\"[LOCTG] texture that bai -> tat\"); return; }",
        T + T + "if (!m_pLocHist && (FAILED(m_pDev->CreateTexture2D(&td, NULL, &m_pLocHist)) || FAILED(m_pDev->CreateShaderResourceView(m_pLocHist, NULL, &m_pLocHistSrv))))",
        T + T + "{ LocRelease(); g_nRep3LocMs = 0; R11Log(\"[LOCTG] texture lich su that bai -> tat\"); return; }",
        T + T + "m_pCtx->CopyResource(m_pLocHist, m_pBackTex);" + T + "// lich su ban dau = chinh khung nay",
        T + T + "R11Log(\"[LOCTG] bat: tau = %d ms, %ux%u\", g_nRep3LocMs, (unsigned)td.Width, (unsigned)td.Height);",
        T + "}",
        T + "// trong so lich su; dt lon (khung dau / treo / doi map) -> khong tron, chi cap nhat lich su",
        T + "float a = 0.0f;",
        T + "if (dt > 0.0 && dt < 100.0)",
        T + T + "a = (float)exp(-dt / (double)g_nRep3LocMs);",
        T + "if (a > 0.9f) a = 0.9f;",
        T + "if (a <= 0.001f) { m_pCtx->CopyResource(m_pLocHist, m_pBackTex); return; }",
        T + "m_pCtx->CopyResource(m_pLocCur, m_pBackTex);",
        T + "D3D11_MAPPED_SUBRESOURCE ms;",
        T + "if (SUCCEEDED(m_pCtx->Map(m_pLocCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) { float v[4] = { a, 0.0f, 0.0f, 0.0f }; memcpy(ms.pData, v, 16); m_pCtx->Unmap(m_pLocCb, 0); }",
        T + "ID3D11ShaderResourceView* srv[2] = { m_pLocCurSrv, m_pLocHistSrv };",
        T + "ID3D11RenderTargetView* rtv = m_pBackRtv;",
        T + "m_pCtx->OMSetRenderTargets(1, &rtv, NULL);",
        T + "D3D11_VIEWPORT vp; vp.TopLeftX = 0.0f; vp.TopLeftY = 0.0f; vp.Width = (float)m_bbW; vp.Height = (float)m_bbH; vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;",
        T + "m_pCtx->RSSetViewports(1, &vp);",
        T + "m_pCtx->RSSetState(NULL);",
        T + "float bf[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; m_pCtx->OMSetBlendState(NULL, bf, 0xFFFFFFFF);",
        T + "m_pCtx->OMSetDepthStencilState(NULL, 0);",
        T + "m_pCtx->IASetInputLayout(NULL);",
        T + "m_pCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);",
        T + "m_pCtx->VSSetShader(m_pLocVS, NULL, 0); m_pCtx->PSSetShader(m_pLocPS, NULL, 0);",
        T + "m_pCtx->PSSetConstantBuffers(0, 1, &m_pLocCb);",
        T + "m_pCtx->PSSetShaderResources(0, 2, srv);",
        T + "m_pCtx->Draw(3, 0);",
        T + "ID3D11ShaderResourceView* nul[2] = { NULL, NULL }; m_pCtx->PSSetShaderResources(0, 2, nul);",
        T + "m_pCtx->CopyResource(m_pLocHist, m_pBackTex);" + T + "// lich su = ket qua da tron",
        T + "// cache trang thai cua lop khong con dung -> lan ve sau gan lai het",
        T + "m_bAppliedValid = false; m_bPipeBound = false; m_bRtBound = false;",
        T + "m_lastSrv[0] = m_lastSrv[1] = (ID3D11ShaderResourceView*)1; m_lastIL = NULL; m_lastTopo = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;",
        T + "memset(&m_lastVp, 0xFF, sizeof(m_lastVp));",
        T + "g_uRep3LocKhung++;",
        "}",
        "",
        "HRESULT CDev11::Present(CONST RECT* pSourceRect",
    ])
    s = rep(s, "HRESULT CDev11::Present(CONST RECT* pSourceRect", body, "D ham")
    ghi(F_D, s, h0, "D3D9on11Dev.cpp")
else:
    print("D3D9on11Dev.cpp da co")

# ---------------------------------------------------------------- KRepresentShell3.cpp
s = doc(F_S); h0 = sum(1 for c in s if ord(c) >= 0x80)
if "g_nRep3LocMs" not in s:
    s = rep(s, "int  g_nRep3Pal       = 1;",
            "int  g_nRep3LocMs     = 8;" + T + "// [LOCTG 09/09] hang so thoi gian bo loc trinh khung (ms); 0 = tat" + NL +
            "extern unsigned g_uRep3LocKhung;" + NL +
            "int  g_nRep3Pal       = 1;",
            "S bien")
    s = rep(s, T + "g_nRep3Pal       = Rep3Ini(\"Rep3Pal\", 1);" + T + "// [D3D11 08/09 r]",
            T + "g_nRep3Pal       = Rep3Ini(\"Rep3Pal\", 1);" + T + "// [D3D11 08/09 r]" + NL +
            T + "g_nRep3LocMs     = Rep3Ini(\"Rep3LocMs\", 8);" + T + "// [LOCTG 09/09]" + NL +
            T + "if (g_nRep3LocMs < 0) g_nRep3LocMs = 0;" + NL +
            T + "if (g_nRep3LocMs > 100) g_nRep3LocMs = 100;",
            "S config")
    s = rep(s, T*3 + "g_dRep3PresentMs = 0.0; g_uRep3Presents = 0; g_uRep3PresentSkip = 0;",
            T*3 + "Rep3Log(\"[LOCTG] tau=%d ms | %u khung da tron\", g_nRep3LocMs, g_uRep3LocKhung);" + NL +
            T*3 + "g_uRep3LocKhung = 0;" + NL +
            T*3 + "g_dRep3PresentMs = 0.0; g_uRep3Presents = 0; g_uRep3PresentSkip = 0;",
            "S log")
    ghi(F_S, s, h0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")

# ---------------------------------------------------------------- CoreShell.cpp: NhipTheGioi mac dinh 60 -> 0
s = doc(F_CS); h0 = sum(1 for c in s if ord(c) >= 0x80)
if '"NhipTheGioi", 60, ' in s:
    s = rep(s, '"NhipTheGioi", 60, ', '"NhipTheGioi", 0, ', "CS mac dinh")
    s = rep(s, "// o nhip 60 Hz\". Nhoe bam mat = toc do truot x thoi gian giu khung; PaintInterp=0 (18 buoc/giay) het han vi",
               "// o nhip 60 Hz\". [LOCTG 09/09] MAC DINH 0 (tat) - cach chung la bo loc thoi gian Rep3LocMs; giu lam cong tac thu.",
               "CS chu thich")
    ghi(F_CS, s, h0, "CoreShell.cpp")
else:
    print("CoreShell.cpp: NhipTheGioi da khong con mac dinh 60")

print("XONG LOCTG")
