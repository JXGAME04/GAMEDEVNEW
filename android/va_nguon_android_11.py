# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 11: cho phep KHUNG VE co co bat ky (khong phai mot che do man hinh).
#
# Trieu chung: dat man hinh may ao 2080x1080, he so giao dien 1,75 -> khung ve 1188x616 -> game SAP ngay khi
# tao thiet bi ve (SIGSEGV, fault addr 0x0, trong KRepresentShell3::Create).
#
# Goc: CD3D_Shell::PickDefaultMode() chi tra ve mot che do co Width/Height DUNG BANG g_nScreenWidth/Height.
# Khi ve trong CUA SO thi co khung ve (backbuffer) khong he bat buoc phai la mot che do man hinh - nhung ham
# nay van doi khop, nen voi 1188x616 no tra NULL. KRepresentShell3::Create gap NULL thi goi D3DTerm(), ma
# D3DTerm() lai lam PD3DDEVICE->SetGammaRamp(...) trong khi THIET BI CHUA HE DUOC TAO -> con tro NULL -> sap.
# (Truoc day khong lo vi Width/Height trong config.ini luon la do phan giai chuan, co san trong danh sach.)
#
# Hai va:
#   1. PickDefaultMode: ve trong cua so ma khong co che do khop -> lay che do man hinh nen (chi can DINH DANG
#      diem anh la dung; co backbuffer do SetPresentationParams lay tu g_nScreenWidth/Height). Rao JX_PLATFORM_SDL
#      nen Game.exe x64 thuong khong doi mot byte.
#   2. D3DTerm: khong goi SetGammaRamp khi chua co thiet bi. Day la sua SAP thuan tuy (duong nay hien gio LUON
#      sap), khong rao theo nen tang - bao cao ro trong ban giao de chu soat lai.
import io, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DUONG_SHELL = "Sources/Represent/Represent3/D3D_Shell.cpp"
DUONG_REP3 = "Sources/Represent/Represent3/KRepresentShell3.cpp"


def doc(p):
    return io.open(os.path.join(ROOT, p), encoding="latin-1", newline="").read()

def ghi(p, s):
    io.open(os.path.join(ROOT, p), "w", encoding="latin-1", newline="").write(s)

def nl(s, crlf):
    return s.replace("\n", "\r\n") if crlf else s

def va(duong, dau_da_co, cu, moi, ten):
    s = doc(duong)
    crlf = ("\r\n" in s)
    if nl(dau_da_co, crlf) in s:
        print("  bo qua (da co): %s" % ten)
        return
    cu2, moi2 = nl(cu, crlf), nl(moi, crlf)
    if s.count(cu2) != 1:
        print("  !! KHONG VA DUOC %s: tim thay %d cho (can dung 1)" % (ten, s.count(cu2)))
        sys.exit(1)
    ghi(duong, s.replace(cu2, moi2))
    print("  va xong: %s" % ten)


va(DUONG_SHELL, "che do man hinh nen",
"""		}

		return &(*itMode);
	}
	return NULL;
}""",
"""		}

		return &(*itMode);
	}
#ifdef JX_PLATFORM_SDL
	// [ANDROID 09/09 DPG] Ve trong CUA SO thi co khung ve (backbuffer) khong bat buoc phai la mot che do man
	// hinh: tren dien thoai khung ve duoc tinh theo he so giao dien (vi du 1188x616) nen chac chan khong co
	// trong danh sach che do. Vong tren tra NULL thi KRepresentShell3::Create goi D3DTerm() luc THIET BI CHUA
	// TAO -> sap. Khi ve trong cua so, che do chi dung de lay DINH DANG diem anh -> lay che do man hinh nen.
	if (g_bRunWindowed)
	{
		// KHONG lay bua mot che do trong danh sach: CD3D_Device::SetPresentationParams dat
		// BackBufferWidth/Height = pMode->Width/Height, lay bua se ra backbuffer sai (640x480).
		// Tu dat mot ban ghi che do dung bang co khung ve, dinh dang theo man hinh nen.
		static D3DModeInfo s_TuDat;
		s_TuDat.Width  = (uint32)g_nScreenWidth;
		s_TuDat.Height = (uint32)g_nScreenHeight;
		s_TuDat.Format = m_DesktopFormat.Format;
		s_TuDat.bHWTnL = true;
		g_DebugLog("[D3DRender] khung ve %dx%d khong co trong danh sach che do -> tu dat che do cua so %dx%d",
			g_nScreenWidth, g_nScreenHeight, (int)s_TuDat.Width, (int)s_TuDat.Height);
		return &s_TuDat;
	}
#endif
	return NULL;
}""", "PickDefaultMode lui ve che do man hinh nen")

va(DUONG_REP3, "chua tao duoc thiet bi",
"""void KRepresentShell3::D3DTerm()
{
	g_Device.ReleaseDevObjects();

	PD3DDEVICE->SetGammaRamp(0, D3DSGR_CALIBRATE, &m_ramp);""",
"""void KRepresentShell3::D3DTerm()
{
	g_Device.ReleaseDevObjects();

	// [09/09] D3DTerm con duoc goi tren duong LOI cua Create() (khong tim duoc card / che do man hinh),
	// luc do chua tao duoc thiet bi nen PD3DDEVICE la NULL -> dong duoi day sap thay vi bao loi tu te.
	if (PD3DDEVICE)
	PD3DDEVICE->SetGammaRamp(0, D3DSGR_CALIBRATE, &m_ramp);""", "D3DTerm khong go gamma khi chua co thiet bi")

print("XONG dot va 11.")
