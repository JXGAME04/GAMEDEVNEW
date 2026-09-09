# -*- coding: utf-8 -*-
"""goi_va_nap4_ghep_dong_bo_0809.py - [NAP 08/09 d] "moi vao game map dang dung thi den, qua map khac binh thuong": nen vung duoc
GHEP MOT LAN luc vao vung: KScenePlaceRegionC ve cac sprite o nen len anh tao san (_*PlaceGround*_) qua DrawPrimitivesOnImage; goi
GetImage trong do gap sprite chua nap -> nap nen tra NULL -> o nen KHONG duoc ve va khong bao gio ve lai -> vung den. Qua map khac:
luc do goi ngoai RepresentBegin/End nen nap ngay. Sua: cac ham GHEP/GHI anh mot lan (DrawPrimitivesOnImage, ClearImageData,
GetBitmapDataBuffer, ReleaseBitmapDataBuffer) luon nap dong bo (tat m_bVeDangDien trong pham vi ham, RAII). Chi cac duong ve MOI KHUNG
(DrawPrimitives) moi duoc nap nen."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
p = ROOT + "\\KRepresentShell3.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read(); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "Rep3NapDongBo" not in s:
    # lop RAII sau khai bao bien NAP
    old = "void Rep3NapCong(Rep3NapDo& d, double ms) { d.n++; d.ms += ms; if (ms > d.max) d.max = ms; g_dRep3NapKhung += ms; }\r\n"
    if s.count(old) != 1: print("FAIL neo Rep3NapCong"); sys.exit(1)
    s = s.replace(old, old +
        "// [NAP 08/09 d] trong pham vi ham GHEP/GHI anh mot lan: bat buoc nap dong bo (ket qua chi dung mot lan, nap nen tra NULL = mat vinh vien)\r\n"
        "struct Rep3NapDongBo { TextureResMgr& m; bool b; Rep3NapDongBo(TextureResMgr& mm) : m(mm), b(mm.m_bVeDangDien) { m.m_bVeDangDien = false; } ~Rep3NapDongBo() { m.m_bVeDangDien = b; } };\r\n")
    n = 0
    for sig in ["void KRepresentShell3::DrawPrimitivesOnImage(int nPrimitiveCount, KRepresentUnit* pPrimitives, ",
                "void KRepresentShell3::ClearImageData(const char* pszImage, unsigned int uImage, short nImagePosition)",
                "void* KRepresentShell3::GetBitmapDataBuffer(const char* pszImage, KBitmapDataBuffInfo* pInfo, int nType)",
                "void KRepresentShell3::ReleaseBitmapDataBuffer(const char* pszImage, void* pBuffer)"]:
        i = s.find(sig)
        if i < 0: print("FAIL khong thay", sig[:60]); sys.exit(1)
        j = s.find("{\r\n", i)
        if j < 0 or j - i > 400: print("FAIL than ham", sig[:40]); sys.exit(1)
        s = s[:j + 3] + "\tRep3NapDongBo napDongBo(m_TextureResMgr);\t// [NAP 08/09 d] ghep/ghi anh mot lan -> nap dong bo\r\n" + s[j + 3:]
        n += 1
    print("   them guard o %d ham" % n)
if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao"); sys.exit(1)
io.open(p, "w", encoding="latin-1", newline="").write(s); print("OK KRepresentShell3.cpp"); print("XONG NAP d")
