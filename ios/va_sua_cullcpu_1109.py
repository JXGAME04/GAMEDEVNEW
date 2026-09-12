# -*- coding: utf-8 -*-
r"""[CULLCPU-SUA 11/09] Sua loi bien dich do commit [CULLCPU 11/09] cua phien Android gay ra.

Benh: D3D9onGPUDev.cpp khai bao s_ullJxPipeKeyCur o dong 55 BEN TRONG "#ifdef JX_ANDROID" (dong 49),
nhung DUNG no o dong 458 va 1119 ma KHONG co rao. Tren Android thi qua; tren iOS va tren WINDOWS
(Represent3.vcxproj CO dich D3D9onGPUDev.cpp) thi loi "use of undeclared identifier".

Chua: rao hai cho dung bang dung "#ifdef JX_ANDROID". Android khong doi mot byte hanh vi;
Windows va iOS bien dich duoc. Truong c.ullPipeKey van duoc memset ve 0 ngay trung o tren.

Chay lai vo hai.  python3 ios/va_sua_cullcpu_1109.py
"""
import io, os, sys
GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
P = os.path.join(GOC, "Sources", "Represent", "Represent3", "D3D9onGPUDev.cpp")
s = io.open(P, encoding="latin-1", newline="").read()
bc = lambda t: sum(1 for c in t if ord(c) >= 0x80)
n0 = bc(s)
cap = [
    ("\ts_ullJxPipeKeyCur = key;\t// [CULLCPU 11/09] chi de DO\n",
     "#ifdef JX_ANDROID\n\ts_ullJxPipeKeyCur = key;\t// [CULLCPU 11/09] chi de DO\n#endif\n"),
    ("\tc.ullPipeKey = s_ullJxPipeKeyCur;\t// [CULLCPU 11/09] chi de DO\n",
     "#ifdef JX_ANDROID\n\tc.ullPipeKey = s_ullJxPipeKeyCur;\t// [CULLCPU 11/09] chi de DO\n#endif\n"),
]
doi = 0
for cu, moi in cap:
    if moi in s:
        continue
    if s.count(cu) != 1:
        sys.exit("LOI: '%s' khop %d lan, phai 1" % (cu.strip()[:40], s.count(cu)))
    s = s.replace(cu, moi); doi += 1
if doi:
    if bc(s) != n0: sys.exit("LOI: byte >= 0x80 doi")
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da rao %d cho dung s_ullJxPipeKeyCur (byte cao %d khong doi)" % (doi, n0))
else:
    print("da rao tu truoc")
