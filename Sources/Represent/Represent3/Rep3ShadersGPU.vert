#version 450
// [GPU 08/09] Shader dinh cho lop D3D9 tren SDL_GPU (Vulkan/SPIR-V) - chuyen tu Rep3Shaders11.hlsl (VS).
// Dich: glslc -fshader-stage=vert -O Rep3ShadersGPU.vert -o Rep3ShadersGPU.vert.spv  (ReverseTools/mobile_x64/dich_shader_gpu.py)
// Dinh: XYZRHW (toa do man hinh, VERTEX2D + chu) hoac XYZ (nhan ma tran W*V*P, duong 3D cu).
// SDL_GPU: uniform cua vertex o set 1; buffer 0 = dinh (pos, [color], [uv], palrow), buffer 1 = dinh gia (mau trang, uv 0) theo instance
// cho FVF thieu thanh phan.

layout(location = 0) in vec4 inPos;      // XYZRHW: x,y,z,rhw ; XYZ: x,y,z (w bo qua)
layout(location = 1) in vec4 inCol;      // D3DCOLOR 0xAARRGGBB trong bo nho = B,G,R,A -> UBYTE4_NORM -> doi .bgra
layout(location = 2) in vec2 inUv;
layout(location = 3) in uint inPal;      // hang bang mau (0xFFFF = khong)

layout(set = 1, binding = 0) uniform VSCB
{
    vec4 g_vp;     // x = rong viewport, y = cao viewport, z = x viewport, w = y viewport
    mat4 g_wvp;    // world * view * proj theo D3D9 (hang-chinh, v * M) -> truyen nguyen, nhan v * M
    vec4 g_flags;  // x = 1 -> dinh XYZRHW, 0 -> XYZ ; y = kich thuoc diem (POINTLIST)
};

layout(location = 0) out vec4 vCol;
layout(location = 1) out vec2 vUv;
layout(location = 2) flat out uint vPal;

void main()
{
    if (g_flags.x > 0.5)
    {
        // D3D9 dat tam diem anh o toa do nguyen, Vulkan o +0.5 -> cong 0.5 de anh xa texel y het D3D9
        vec2 p = inPos.xy + 0.5;
        // SDL_GPU chuan hoa NDC nhu D3D (y len tren, goc viewport tren-trai) tren moi backend
        gl_Position = vec4((p.x - g_vp.z) / g_vp.x * 2.0 - 1.0, 1.0 - (p.y - g_vp.w) / g_vp.y * 2.0, 0.5, 1.0);
    }
    else
    {
        // uniform giu 16 float cua D3DMATRIX (hang-chinh) -> GLSL doc thanh cot = hang D3D -> M * v == v * M_d3d
        gl_Position = g_wvp * vec4(inPos.xyz, 1.0);
    }
    gl_PointSize = (g_flags.y > 0.0) ? g_flags.y : 1.0;
    vCol = inCol.bgra;
    vUv = inUv;
    vPal = inPal;
}
