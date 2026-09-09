# -*- coding: utf-8 -*-
"""[GPU 08/09] Dich shader GLSL cua lop D3D9 tren SDL_GPU sang SPIR-V (glslc trong Android NDK) va sinh header mang byte
Sources/Represent/Represent3/Rep3ShadersGPU_spv.h (g_Rep3GpuVS / g_Rep3GpuFS). Chay lai moi khi sua .vert/.frag.
Cach dung: python dich_shader_gpu.py [worktree_root]"""
import io, os, sys, glob, subprocess
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
D = os.path.join(ROOT, "Sources", "Represent", "Represent3")
ndks = sorted(glob.glob(r"C:\Users\nguye\AppData\Local\Android\Sdk\ndk\*"))
assert ndks, "khong thay Android NDK"
GLSLC = os.path.join(ndks[-1], "shader-tools", "windows-x86_64", "glslc.exe")
VAL = os.path.join(ndks[-1], "shader-tools", "windows-x86_64", "spirv-val.exe")
out = ["// [GPU 08/09] SINH TU DONG boi ReverseTools/mobile_x64/dich_shader_gpu.py tu Rep3ShadersGPU.vert/.frag (glslc %s). DUNG SUA TAY." % os.path.basename(ndks[-1]),
       "#pragma once", ""]
for stage, name in (("vert", "g_Rep3GpuVS"), ("frag", "g_Rep3GpuFS")):
    src = os.path.join(D, "Rep3ShadersGPU." + stage); spv = src + ".spv"
    r = subprocess.run([GLSLC, "-fshader-stage=" + stage, "-O", "--target-env=vulkan1.0", src, "-o", spv], capture_output=True, text=True)
    if r.returncode != 0:
        print("glslc LOI", stage, ":\n", r.stdout, r.stderr); sys.exit(1)
    v = subprocess.run([VAL, spv], capture_output=True, text=True)
    if v.returncode != 0:
        print("spirv-val LOI", stage, ":\n", v.stdout, v.stderr); sys.exit(1)
    b = open(spv, "rb").read()
    assert len(b) % 4 == 0
    out.append("// %s: %d byte" % (os.path.basename(src), len(b)))
    out.append("static const unsigned char %s[%d] = {" % (name, len(b)))
    for i in range(0, len(b), 24):
        out.append("\t" + ", ".join("0x%02X" % c for c in b[i:i + 24]) + ",")
    out.append("};"); out.append("")
    print("%s -> %d byte SPIR-V (hop le)" % (os.path.basename(src), len(b)))
io.open(os.path.join(D, "Rep3ShadersGPU_spv.h"), "w", newline="\r\n").write("\n".join(out) + "\n")
print("da sinh Rep3ShadersGPU_spv.h")
