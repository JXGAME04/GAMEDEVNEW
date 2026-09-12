# -*- coding: utf-8 -*-
r"""[IOS-DEMDINH 11/09] Chi khai dung so DEM DINH thuc su duoc dung (chi iOS).

Benh (bat duoc bang "devicectl ... --console", KHONG phai doan):
  -[MTLVertexDescriptorInternal newSerializedDescriptor]:743: failed assertion
  'None of the attributes set bufferIndex to 15, but MTLVertexDescriptor set buffer layout[15].stride(16).'
  App terminated due to signal 6.

Goc: D3D9onGPUDev.cpp luon dat num_vertex_buffers = 2, nhung dem thu hai (slot 1) CHI duoc dung khi
dinh thieu mau (colOff == 0xFFFFFFFF) hoac thieu toa do texture (uvOff == 0xFFFFFFFF). Khi dinh co du
ca hai thi khong thuoc tinh nao tro toi slot 1.
SDL anh xa slot N -> chi so Metal 14 + N, nen slot 1 = layout[15]. Vulkan bo qua layout thua;
Metal thi BAT LOI CHET (assertion -> SIGABRT).

Chua (rao JX_IOS): dem so dem dinh thuc su duoc dung. Android va Windows di nhanh #else, giu nguyen
van dong cu -> ios/kiem_rao.py so tung dong van bang nhau.

Chay lai vo hai.  python3 ios/va_nguon_ios_dem_dinh.py
"""
import io, os, sys
GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
P = os.path.join(GOC, "Sources", "Represent", "Represent3", "D3D9onGPUDev.cpp")
s = io.open(P, encoding="latin-1", newline="").read()
bc = lambda t: sum(1 for c in t if ord(c) >= 0x80)
n0 = bc(s)

CU = "\tpi.vertex_input_state.vertex_buffer_descriptions = vb; pi.vertex_input_state.num_vertex_buffers = 2;\n"
MOI = ("#ifdef JX_IOS\t// [IOS-DEMDINH 11/09] Metal bat loi chet neu khai dem dinh ma khong thuoc tinh nao dung\n"
       "\tpi.vertex_input_state.vertex_buffer_descriptions = vb;\n"
       "\tpi.vertex_input_state.num_vertex_buffers = (va[1].buffer_slot == 1 || va[2].buffer_slot == 1) ? 2 : 1;\n"
       "#else\n" + CU + "#endif\n")

if "IOS-DEMDINH" in s:
    print("da va tu truoc")
else:
    if s.count(CU) != 1:
        sys.exit("LOI: khop %d lan, phai 1" % s.count(CU))
    s = s.replace(CU, MOI)
    if bc(s) != n0:
        sys.exit("LOI: byte >= 0x80 doi %d -> %d" % (n0, bc(s)))
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va: chi khai so dem dinh thuc su dung (byte cao %d khong doi)" % n0)
