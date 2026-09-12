# -*- coding: utf-8 -*-
"""[IOS-GOP 12/09 b] Buoc 2a: bat bang mau kieu storage buffer (Rep3PalBuffer) cho ban iOS.

VI SAO: do tren may that luc Tong Kim, ban iOS ghi 4331 lenh ve cho 4282 quad - gan nhu moi quad
mot lenh, khong gop duoc gi. Ban Android gop duoc nho bang mau nam trong storage buffer thay vi
texture 256x8192: khi do quad khac bang mau VAN gop chung mot lenh ve.

CACH LAM AN TOAN NHAT: KHONG viet lai ma, chi MO RAO cac khoi san co tu JX_ANDROID sang JX_MOBILE.
Android dinh nghia JX_MOBILE nen bien dich DUNG Y HET ma cu - khong mot dong logic nao doi.
iOS cung dinh nghia JX_MOBILE nen duoc huong. macOS khong (chua can). Windows khong dinh nghia
ca hai nen khong lien quan.

Rieng cho CHON SHADER thi khoi Apple da co san, chi them nhanh chon ban MSL PalBuf.

CAC CONG TAC CHUA PORT (ps buffer, bind ring, texture mang, atlas khoi) bi EP TAT tren Apple o
KRepresentShell3.cpp, du ma cua chung nay da duoc bien dich. Bat tung cai mot, do roi moi bat tiep.

Tat khan cap: dat [Client] Rep3PalBuffer=0 trong config.ini la ve duong cu ngay.
"""
import os, sys

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TEP = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'D3D9onGPUDev.cpp')
SHELL = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.cpp')
MOC = b'IOS-GOP 12/09 b'

# Moi muc: (dong neo ngay SAU #ifdef JX_ANDROID) - dung de xac dinh dung khoi can mo rao.
NEO = [
 b'\tif (g_nJxPalBuffer && !PalInit()) return false;',
 b'\t\tm_uJxPsStageOff = 0xFFFFFFFFu;',
 b'\t\tif ((!m_texUploads.empty() || !m_jxPalUploads.empty() || m_uJxPsStageOff != 0xFFFFFFFFu) && !m_texStage.empty())',
 b'\t\t\t\tjxK.dChepTexLenh = JxVeMs(uT0, SDL_GetPerformanceCounter()) - jxK.dChepTexMap;',
 b'\t\t\tif (g_nJxBindRing && m_pRingGpu)',
 b'\t\t\tSDL_BindGPUFragmentSamplers(pass, 0, tb, g_nJxPalBuffer ? 2 : 3);',
 b'\tif (m_pJxPalBuf) return true;',
]

SHADER_CU = (b'#ifdef JX_APPLE\t// [IOS-METAL 11/09]\n'
             b'\tsi.code = (const Uint8*)g_Rep3GpuFSMsl; si.code_size = sizeof(g_Rep3GpuFSMsl) - 1; si.entrypoint = "main0"; si.format = SDL_GPU_SHADERFORMAT_MSL;\n'
             b'#endif\n')
SHADER_MOI = (b'#ifdef JX_APPLE\t// [IOS-METAL 11/09]\n'
              b'\tsi.code = (const Uint8*)g_Rep3GpuFSMsl; si.code_size = sizeof(g_Rep3GpuFSMsl) - 1; si.entrypoint = "main0"; si.format = SDL_GPU_SHADERFORMAT_MSL;\n'
              b'\tif (g_nJxPalBuffer)\n'
              b'\t{	// [IOS-GOP 12/09 b] bang mau = storage buffer -> quad khac bang mau van gop chung mot lenh ve\n'
              b'\t\tsi.code = (const Uint8*)g_Rep3GpuFSPalBufMsl; si.code_size = sizeof(g_Rep3GpuFSPalBufMsl) - 1;\n'
              b'\t\tsi.num_samplers = 2; si.num_storage_buffers = 1;\n'
              b'\t}\n'
              b'#endif\n')

EP_CU = b'\tg_nJxPalBuffer      = Rep3Ini("Rep3PalBuffer", 1) ? 1 : 0;'
EP_MOI = (EP_CU + b'\n'
  b'#ifdef JX_APPLE\t// [IOS-GOP 12/09 b] Metal moi port bang mau; cac toi uu con lai EP TAT cho toi khi do xong tung cai\n'
  b'\tg_nJxPsBuffer = 0; g_nJxBindRing = 0; g_nJxAtlasMang = 0; g_nJxAtlasKhoi = 0;\n'
  b'#endif')

def mo_rao(d):
    n = 0
    for neo in NEO:
        cu = b'#ifdef JX_ANDROID\n' + neo
        if d.count(cu) != 1:
            print('LOI: neo %r khop %d lan' % (neo[:48], d.count(cu))); return None, 0
        d = d.replace(cu, b'#ifdef JX_MOBILE\t// [IOS-GOP 12/09 b] mo cho iOS: Android dinh nghia JX_MOBILE nen bien dich y het truoc\n' + neo, 1)
        n += 1
    return d, n

def main():
    d = open(TEP, 'rb').read()
    if MOC in d:
        print('da va roi'); return 0
    d, n = mo_rao(d)
    if d is None: return 1
    if d.count(SHADER_CU) != 1:
        print('LOI: khong thay khoi chon shader Apple'); return 1
    d = d.replace(SHADER_CU, SHADER_MOI, 1)
    open(TEP, 'wb').write(d)
    print('D3D9onGPUDev.cpp: mo rao %d khoi + them nhanh chon shader MSL PalBuf' % n)

    s = open(SHELL, 'rb').read()
    if s.count(EP_CU) != 1:
        print('LOI: khong thay cho doc Rep3PalBuffer'); return 1
    open(SHELL, 'wb').write(s.replace(EP_CU, EP_MOI, 1))
    print('KRepresentShell3.cpp: ep tat cac cong tac chua port tren Apple')
    return 0

sys.exit(main())
