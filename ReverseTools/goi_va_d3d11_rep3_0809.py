# -*- coding: ascii -*-
"""goi_va_d3d11_rep3_0809.py - [D3D11 08/09] noi lop D3D9-tren-D3D11 vao Represent3 (cong tac [Client] Rep3Api=11).
Sua: D3D_Shell.cpp (tao IDirect3D9 gia lap), D3D_Device.h (Rep3CreateTex thay D3DXCreateTexture), TextureRes.cpp,
KRepresentShell3.cpp (ini + log + thong ke), BaseInclude.h (extern), Represent3.vcxproj (tep moi + d3d11.lib/dxgi.lib).
Nguon TCVN3: doc/ghi latin-1, neo ASCII, kiem so byte cao khong doi."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"

def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
def load(name):
    p = ROOT + "\\" + name
    return p, io.open(p, "r", encoding="latin-1", newline="").read()
def save(p, s, h0):
    if hb(s) != h0: print("FAIL byte cao doi", p); sys.exit(1)
    if re.search(r"[^\r]\n", s): print("FAIL LF tran", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:60], c, n)); sys.exit(1)
    return s.replace(old, new)

# 1. D3D_Shell.cpp
p, s = load("D3D_Shell.cpp"); h0 = hb(s)
s = rep(s, '#include "d3d_shell.h"\r\n', '#include "d3d_shell.h"\r\n#include "D3D9on11.h"\t// [D3D11 08/09]\r\n')
s = rep(s, "\tg_nRep3ExOn = 0;\r\n\tif (g_nRep3Ex)\r\n",
    "\tg_nRep3ExOn = 0;\r\n"
    "\tg_nRep3ApiOn = 9;\r\n"
    "\tif (g_nRep3Api == 11)\t// [D3D11 08/09] lop D3D9 tren D3D11: driver khong giu ban sao texture trong RAM\r\n"
    "\t{\r\n"
    "\t\tm_pD3D = Rep3_CreateD3D9on11();\r\n"
    "\t\tif (m_pD3D)\r\n"
    "\t\t\tg_nRep3ApiOn = 11;\r\n"
    "\t\telse\r\n"
    "\t\t\tRep3Log(\"[REP3] Rep3Api=11 nhung khong tao duoc D3D11 -> lui ve D3D9\");\r\n"
    "\t}\r\n"
    "\tif (!m_pD3D && g_nRep3Ex)\r\n")
save(p, s, h0); print("OK D3D_Shell.cpp")

# 2. D3D_Device.h
p, s = load("D3D_Device.h"); h0 = hb(s)
s = rep(s, "#include <d3d9.h>\r\n", "#include <d3d9.h>\r\n#include <d3dx9.h>\r\n")
s = rep(s, "#define PD3DDEVICE (g_Device.m_pD3DDevice)",
    "extern int g_nRep3ApiOn;\r\n"
    "// [D3D11 08/09] D3DXCreateTexture khong biet thiet bi gia lap -> goi thang CreateTexture khi chay D3D11; D3D9 giu nguyen D3DX\r\n"
    "static inline HRESULT Rep3CreateTex(LPDIRECT3DDEVICE9 pDev, UINT w, UINT h, UINT mip, DWORD usage, D3DFORMAT fmt, D3DPOOL pool, LPDIRECT3DTEXTURE9* pp)\r\n"
    "{\r\n"
    "\tif (g_nRep3ApiOn == 11)\r\n"
    "\t\treturn pDev->CreateTexture(w, h, mip, usage, fmt, pool, pp, NULL);\r\n"
    "\treturn D3DXCreateTexture(pDev, w, h, mip, usage, fmt, pool, pp);\r\n"
    "}\r\n"
    "#define PD3DDEVICE (g_Device.m_pD3DDevice)")
save(p, s, h0); print("OK D3D_Device.h")

# 3. TextureRes.cpp: 9 cho (5 song + 4 trong khoi chu thich)
p, s = load("TextureRes.cpp"); h0 = hb(s)
s = rep(s, "D3DXCreateTexture(PD3DDEVICE,", "Rep3CreateTex(PD3DDEVICE,", 9)
save(p, s, h0); print("OK TextureRes.cpp")

# 4. BaseInclude.h
p, s = load("BaseInclude.h"); h0 = hb(s)
s = rep(s, "extern int  g_nMaxTexW, g_nMaxTexH;\r\n",
    "extern int  g_nMaxTexW, g_nMaxTexH;\r\n"
    "extern int  g_nRep3Api;         // [D3D11 08/09] [Client] Rep3Api: 9 = D3D9 (mac dinh), 11 = D3D9 tren D3D11\r\n"
    "extern int  g_nRep3ApiOn;       // API thuc chay sau D3D_Shell::Create\r\n"
    "extern unsigned g_uRep3GpuTexCount;          // [D3D11 08/09] so texture GPU dang song\r\n"
    "extern unsigned __int64 g_uRep3GpuTexBytes;  // [D3D11 08/09] byte texture GPU\r\n")
save(p, s, h0); print("OK BaseInclude.h")

# 5. KRepresentShell3.cpp
p, s = load("KRepresentShell3.cpp"); h0 = hb(s)
s = rep(s, "int  g_nRep3Ex        = 0;", "int  g_nRep3Api       = 9;\t// [D3D11 08/09]\r\nint  g_nRep3ApiOn     = 9;\t// [D3D11 08/09]\r\nint  g_nRep3Ex        = 0;")
s = rep(s, '\tg_nRep3Ex        = Rep3Ini("Rep3Ex", 0);', '\tg_nRep3Api       = Rep3Ini("Rep3Api", 9);\t// [D3D11 08/09]\r\n\tg_nRep3Ex        = Rep3Ini("Rep3Ex", 0);')
s = rep(s, '\tRep3Log("[REP3] D3D9Ex: %s", g_nRep3ExOn ? "BAT (Rep3Ex=1, texture MANAGED -> DYNAMIC+DEFAULT)" : "tat");',
    '\tRep3Log("[REP3] D3D9Ex: %s", g_nRep3ExOn ? "BAT (Rep3Ex=1, texture MANAGED -> DYNAMIC+DEFAULT)" : "tat");\r\n'
    '\tRep3Log("[REP3] API: %s", g_nRep3ApiOn == 11 ? "Direct3D 11 (lop D3D9 tren D3D11, Rep3Api=11)" : "Direct3D 9");\t// [D3D11 08/09]')
s = rep(s, 'giai_ma %u khung %.1f ms",', 'giai_ma %u khung %.1f ms | gpu tex %u (%u MB)",')
s = rep(s, "g_uRep3FxGiaiMa, g_dRep3FxGiaiMaMs);", "g_uRep3FxGiaiMa, g_dRep3FxGiaiMaMs, g_uRep3GpuTexCount, (unsigned)(g_uRep3GpuTexBytes >> 20));")
save(p, s, h0); print("OK KRepresentShell3.cpp")

# 6. vcxproj
p, s = load("Represent3.vcxproj"); h0 = hb(s)
s = rep(s, "d3d9.lib;winmm.lib;", "d3d9.lib;d3d11.lib;dxgi.lib;winmm.lib;", 2)
s = rep(s, '    <ClCompile Include="D3D_Utils.cpp" />\r\n',
    '    <ClCompile Include="D3D_Utils.cpp" />\r\n'
    '    <ClCompile Include="D3D9on11.cpp" />\r\n'
    '    <ClCompile Include="D3D9on11Dev.cpp" />\r\n'
    '    <ClCompile Include="D3D9on11D3D.cpp" />\r\n')
s = rep(s, '    <ClInclude Include="D3D_Utils.h" />\r\n',
    '    <ClInclude Include="D3D_Utils.h" />\r\n'
    '    <ClInclude Include="D3D9on11.h" />\r\n'
    '    <ClInclude Include="D3D9on11i.h" />\r\n'
    '    <ClInclude Include="Rep3Shaders11_vs.h" />\r\n'
    '    <ClInclude Include="Rep3Shaders11_ps.h" />\r\n')
save(p, s, h0); print("OK Represent3.vcxproj")
print("XONG")
