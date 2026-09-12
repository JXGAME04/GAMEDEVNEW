# -*- coding: utf-8 -*-
r"""[IOS-METAL 11/09] Sinh Sources/Represent/Represent3/Rep3ShadersGPU_msl.h (shader Metal cho ban iOS).

Khong can glslc: lay THANG ma SPIR-V dang dung trong Rep3ShadersGPU_spv.h (dung byte voi ban Android),
dich cheo sang MSL bang spirv-cross, roi nhung lai duoi dang chuoi C.

QUY UOC GAN KET cua SDL_GPU tren Metal (doc tu SDL_gpu_metal.m 3.2.30):
  - fragment: sampler/texture o chi so 0..n-1 ; dem hang so o buffer(0..) ; dem luu tru o buffer(so dem hang so..)
  - vertex  : dem hang so o buffer(0..) ; dem DINH o buffer(14..)  (METAL_FIRST_VERTEX_BUFFER_SLOT = 14)
  -> khong dung nhau, va "--msl-decoration-binding" cho spirv-cross dung thang so binding cua SPIR-V
     lam chi so MSL, dung bang cai SDL cho.
Quy uoc set cua SDL tren Vulkan (shader nguon dang theo): set1 = uniform cua vertex, set2 = sampler cua
fragment, set3 = uniform cua fragment.

CHI sinh hai ban CO BAN (g_Rep3GpuVS / g_Rep3GpuFS). Ba bien the dung storage buffer va texture mang
deu dang rao "#ifdef JX_ANDROID" - ban iOS dau tien khong bat, dung dung duong da chay on tu 08/09.

Chay:  python3 ios/sinh_shader_msl.py
"""
import io, os, re, subprocess, sys, tempfile

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
D = os.path.join(GOC, "Sources", "Represent", "Represent3")
NGUON = os.path.join(D, "Rep3ShadersGPU_spv.h")
RA = os.path.join(D, "Rep3ShadersGPU_msl.h")
XC = "spirv-cross"

s = io.open(NGUON, encoding="latin-1").read()

def lay_mang(ten):
    m = re.search(r"static const unsigned char %s\[(\d+)\] = \{(.*?)\};" % ten, s, re.S)
    if not m:
        sys.exit("LOI: khong thay mang %s trong %s" % (ten, NGUON))
    n = int(m.group(1))
    b = bytes(int(x, 16) for x in re.findall(r"0x([0-9A-Fa-f]{2})", m.group(2)))
    if len(b) != n:
        sys.exit("LOI: %s khai %d byte nhung doc duoc %d" % (ten, n, len(b)))
    return b

ra = ['// [IOS-METAL 11/09] SINH TU DONG boi ios/sinh_shader_msl.py tu Rep3ShadersGPU_spv.h. DUNG SUA TAY.',
      '// Shader Metal (MSL) cho ban iOS. Diem vao la "main0" (spirv-cross doi ten tu "main").',
      '#pragma once', '']
# [IOS-GOP 12/09] them hai bien the gop lenh ve cua ban Android:
#   PalBuf = bang mau nam trong dem luu tru -> quad khac bang mau van gop chung mot lenh
#   PalPs  = them ca to hop trang thai tang texture -> gop duoc nhieu hon nua
# Hai bien the dung texture mang / atlas khoi thi CHUA sinh: chung doi nhieu khe sampler hon
# va gan chet khe ca khung, phai do tren may that truoc.
# [IOS-GOP 12/09 c] DOI_SO_BUFFER - vi sao phai co:
#   spirv-cross voi --msl-decoration-binding lay THANG so binding cua SPIR-V lam chi so buffer MSL.
#   Binding do theo quy uoc Vulkan cua SDL: sampler o 0..n-1 roi storage buffer tiep ngay sau.
#   Nhung tren METAL, SDL xep khac han (SDL_gpu_metal.m:2728):
#       dem hang so  -> buffer(0 .. num_uniform-1)
#       dem luu tru  -> buffer(num_uniform .. num_uniform+num_storage-1)
#   Nen ban PalBuf khai bang mau o buffer(2) trong khi SDL gan no vao buffer(1) -> shader doc
#   nham cho -> MOI SPRITE RA MAU DEN. Do dung loi "vao game den thui, chi con ten va thanh mau"
#   (ten va thanh mau ve bang duong khac, khong qua bang mau).
#   Sua: sau khi dich, danh so lai cac tham so "const device" (storage buffer) theo dung quy uoc Metal.
SO_DEM = {          # ten bien the -> (so dem hang so, so dem luu tru) PHAI khop voi CreateShaders
    "g_Rep3GpuVS":        (1, 0),
    "g_Rep3GpuFS":        (1, 0),
    "g_Rep3GpuFSPalBuf":  (1, 1),
    "g_Rep3GpuFSPalPs":   (0, 2),
}

def doi_so_buffer(msl, ten):
    nU, nS = SO_DEM[ten]
    if nS == 0:
        return msl
    cu_so = sorted(set(int(x) for x in re.findall(r"const device [^\n]*?\[\[buffer\((\d+)\)\]\]", msl)))
    if len(cu_so) != nS:
        sys.exit("LOI: %s tim thay %d dem luu tru, khai bao %d" % (ten, len(cu_so), nS))
    for i, so in enumerate(cu_so):
        moi_so = nU + i
        if so == moi_so:
            continue
        msl = re.sub(r"(const device [^\n]*?\[\[buffer\()%d(\)\]\])" % so, r"\g<1>%d\g<2>" % moi_so, msl)
        print("    %s: dem luu tru buffer(%d) -> buffer(%d)  (Metal: sau %d dem hang so)" % (ten, so, moi_so, nU))
    return msl

for ten, giai in (("g_Rep3GpuVS", "vertex"), ("g_Rep3GpuFS", "fragment"),
                  ("g_Rep3GpuFSPalBuf", "fragment"), ("g_Rep3GpuFSPalPs", "fragment")):
    b = lay_mang(ten)
    with tempfile.NamedTemporaryFile(suffix=".spv", delete=False) as f:
        f.write(b); spv = f.name
    r = subprocess.run([XC, "--msl", "--msl-version", "20000", "--msl-decoration-binding", spv],
                       capture_output=True, text=True)
    os.unlink(spv)
    if r.returncode != 0:
        sys.exit("LOI spirv-cross (%s): %s" % (ten, r.stderr[:400]))
    msl = r.stdout
    if "main0" not in msl:
        sys.exit("LOI: khong thay diem vao main0 trong MSL cua %s" % ten)
    msl = doi_so_buffer(msl, ten)
    ra.append("// %s: %d byte SPIR-V -> %d ky tu MSL (%s)" % (ten, len(b), len(msl), giai))
    ra.append("static const char %sMsl[] =" % ten)
    for d in msl.splitlines():
        ra.append('\t"%s\\n"' % d.replace("\\", "\\\\").replace('"', '\\"'))
    ra.append("\t;")
    ra.append("")
    print("  %-14s SPIR-V %6d byte -> MSL %6d ky tu" % (ten, len(b), len(msl)))

io.open(RA, "w", encoding="latin-1", newline="\r\n").write("\n".join(ra) + "\n")
print("da sinh", os.path.relpath(RA, GOC))
