# -*- coding: utf-8 -*-
r"""[IOS-METAL 11/09] Noi shader Metal vao tang ve, chi cho iOS.

Truoc do: D3D9onGPUDev.cpp xin SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV) -> tren iOS (Metal)
that bai -> tang ve khong khoi dong -> man hinh den.

Sau khi va (chi khi JX_IOS): xin SDL_GPU_SHADERFORMAT_MSL va nap ban MSL sinh boi
ios/sinh_shader_msl.py. Diem vao MSL la "main0" (spirv-cross doi ten tu "main").
codeSize = do dai chuoi KHONG ke ky tu ket thuc (SDL_gpu_metal.m dung initWithBytes:length:).

Android va Windows di nhanh #else: khong doi mot dong. Ba bien the storage buffer / texture mang
van rao JX_ANDROID nen ban iOS khong dung toi.

Chay lai vo hai.  python3 ios/va_nguon_ios_metal.py   roi   python3 ios/kiem_rao.py
"""
import io, os, sys
GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
P = os.path.join(GOC, "Sources", "Represent", "Represent3", "D3D9onGPUDev.cpp")
s = io.open(P, encoding="latin-1", newline="").read()
bc = lambda t: sum(1 for c in t if ord(c) >= 0x80)
n0 = bc(s)

def thay(cu, moi, ten):
    global s
    if moi in s:
        print("   bo qua (da co): " + ten); return 0
    if s.count(cu) != 1:
        sys.exit("LOI: '%s' khop %d lan, phai 1" % (ten, s.count(cu)))
    s = s.replace(cu, moi); print("   da va: " + ten); return 1

n = 0
# 1. them header MSL
n += thay('#include "Rep3ShadersGPU_spv.h"\n',
          '#include "Rep3ShadersGPU_spv.h"\n'
          '#ifdef JX_IOS\n'
          '#include "Rep3ShadersGPU_msl.h"\t// [IOS-METAL 11/09] ban MSL cho Metal (sinh boi ios/sinh_shader_msl.py)\n'
          '#endif\n', "them include Rep3ShadersGPU_msl.h")

# 2. tao thiet bi GPU: iOS xin MSL thay vi SPIRV
CU2 = ('\tm_pGpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, (e && atoi(e) != 0), NULL);\n'
       '\tif (!m_pGpu) { RgLog("SDL_CreateGPUDevice(SPIRV) that bai: %s", SDL_GetError()); return false; }\n')
MOI2 = ('#ifdef JX_IOS\t// [IOS-METAL 11/09] Metal chi nhan MSL, khong nhan SPIR-V\n'
        '\tm_pGpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_MSL, (e && atoi(e) != 0), NULL);\n'
        '\tif (!m_pGpu) { RgLog("SDL_CreateGPUDevice(MSL) that bai: %s", SDL_GetError()); return false; }\n'
        '#else\n' + CU2 + '#endif\n')
n += thay(CU2, MOI2, "SDL_CreateGPUDevice xin MSL khi JX_IOS")

# 3. hai shader: doi sang ban MSL
CU3 = '\tm_pVS = SDL_CreateGPUShader(m_pGpu, &si);\n'
MOI3 = ('#ifdef JX_IOS\t// [IOS-METAL 11/09] code_size = do dai chuoi KHONG ke ky tu ket thuc\n'
        '\tsi.code = (const Uint8*)g_Rep3GpuVSMsl; si.code_size = sizeof(g_Rep3GpuVSMsl) - 1; si.entrypoint = "main0"; si.format = SDL_GPU_SHADERFORMAT_MSL;\n'
        '#endif\n' + CU3)
n += thay(CU3, MOI3, "shader dinh sang MSL")

CU4 = '\tm_pFS = SDL_CreateGPUShader(m_pGpu, &si);\n'
MOI4 = ('#ifdef JX_IOS\t// [IOS-METAL 11/09]\n'
        '\tsi.code = (const Uint8*)g_Rep3GpuFSMsl; si.code_size = sizeof(g_Rep3GpuFSMsl) - 1; si.entrypoint = "main0"; si.format = SDL_GPU_SHADERFORMAT_MSL;\n'
        '#endif\n' + CU4)
n += thay(CU4, MOI4, "shader diem anh sang MSL")

if n:
    if bc(s) != n0: sys.exit("LOI: byte >= 0x80 doi %d -> %d" % (n0, bc(s)))
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("xong va_nguon_ios_metal.py (%d cho, byte cao %d khong doi)" % (n, n0))
