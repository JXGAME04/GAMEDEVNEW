# -*- coding: utf-8 -*-
r"""[DONTRUNG 11/09] DON KY HIEU TOAN CUC TRUNG TEN - cho CA BA nen (PC, Android, iOS).

Benh: 7 ky hieu duoc dinh nghia o HAI thu vien khac nhau. Tren Windows moi DLL giu ban rieng nen
khong ai va cham. Tren Android moi thu la .so, bo nap dong chi giu MOT dinh nghia cho ca tien trinh
va KHONG bao gi - sai lang le luc chay (dung benh da giet Thu / Dau gia / Chien Lenh, xem
BANGIAO_ANDROID_PHA4_0809.md muc 9.1). Tren iOS link tinh thi trinh lien ket bao to ngay luc dung.

Cach chua: doi ten ban PHIA GIAO DIEN (S3Client) va bo hai hang so thua cua Rainbow.
KHONG dong vao Core / Engine vi may chu cung dich nhung tep do.
Theo dung TIEN LE da co san trong du an: Represent gap y het va da doi ten ban cua no thanh
g_pIInlinePicSinkRP (Represent2/KRepresentShell2.cpp:31, Represent3/KRepresentShell3.cpp:577).

VI SAO TRUNG TINH VE HANH VI:
  - Windows: moi mo dun von da co bien rieng; doi ten khong doi cho luu, khong doi ngu nghia.
  - Android: hai ban dang dung chung mot o nho nhung deu tro toi CUNG mot doi tuong
    (g_pMusic: ca hai la &m_Music - Core nhan qua CoreShell.cpp:25325, S3Client gan o S3Client.cpp:664;
     g_pIInlinePicSink: ca hai la m_pInlinePicSink;
     l_Time: ca hai duoc cap nhat moi khung - Core o KScenePlaceC.cpp:1096, S3Client o UiShell.cpp:233/405)
    -> tach ra cho cung gia tri.
  - Hai GUID chi la hang so, hai ban trung nhau tung byte.

Chay lai vo hai (doi ten theo NGUYEN TU nen lan hai khong con gi de doi).
Doc/ghi latin-1, giu ky tu xuong dong, kiem so byte > 0x7F truoc va sau.
Chay:  python3 ios/va_dontrung_kyhieu.py
"""
import io, os, re, sys

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
def doc(p):  return io.open(p, encoding="latin-1", newline="").read()
def ghi(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def bc(s):   return sum(1 for c in s if ord(c) >= 0x80)

tong = 0
def doi_ten(duong, cap):
    """cap = [(ten_cu, ten_moi), ...] - thay theo NGUYEN TU (\\b...\\b)."""
    global tong
    p = os.path.join(GOC, duong)
    s = doc(p); n0 = bc(s); n = 0
    for cu, moi in cap:
        s, k = re.subn(r"\b%s\b" % re.escape(cu), moi, s)
        n += k
    if n:
        if bc(s) != n0:
            sys.exit("LOI: %s: byte >= 0x80 doi %d -> %d" % (duong, n0, bc(s)))
        ghi(p, s); tong += n
        print("   %-50s %3d cho (byte cao %d khong doi)" % (duong, n, n0))
    else:
        print("   %-50s  -  da doi tu truoc" % duong)

def doi_ten_ca_cay(thu_muc, cap):
    """Doi ten trong MOI tep .cpp/.h duoi thu_muc. Chi dung cho cay S3Client."""
    goc = os.path.join(GOC, thu_muc)
    ds = []
    for r, _, tep in os.walk(goc):
        for t in tep:
            if t.lower().endswith((".cpp", ".h", ".inc", ".c")):
                ds.append(os.path.join(r, t))
    n_tep = 0
    for p in sorted(ds):
        s = doc(p); n0 = bc(s); n = 0
        for cu, moi in cap:
            s, k = re.subn(r"\b%s\b" % re.escape(cu), moi, s)
            n += k
        if n:
            if bc(s) != n0:
                sys.exit("LOI: %s: byte >= 0x80 doi %d -> %d" % (p, n0, bc(s)))
            ghi(p, s); n_tep += 1
            global tong
            tong += n
            print("   %-50s %3d cho" % (os.path.relpath(p, GOC), n))
    if n_tep == 0:
        print("   %-50s  -  da doi tu truoc" % thu_muc)

print("NHOM 1: l_Time / IR_GetCurrentTime / IR_UpdateTime   UiImage.cpp <-> Core/Src/ImgRef.cpp")
# Khai bao nam o UiImage.h nen ~25 tep giao dien khac tu dong thay ten moi, khong phai sua tay tep nao.
N1 = [("l_Time", "l_UiTime"), ("IR_GetCurrentTime", "UiIR_GetCurrentTime"), ("IR_UpdateTime", "UiIR_UpdateTime")]
# Doi ten o TOAN BO cay S3Client (ke ca cac tep CHI-Windows ma ban iOS khong dich: CrashLog, AntiHack,
# TrayMode, PerfHud, JxReplay, UiPlayVideo...) - neu chi doi o UiImage.h thi noi goi van giu ten cu
# va se dinh vao ban cua Core. KHONG bao gio dong vao Core / Engine / Represent.
doi_ten_ca_cay("Sources/S3Client", N1)

print("NHOM 2: g_pMusic                                     S3Client.cpp <-> Core/Src/KCore.cpp")
N2 = [("g_pMusic", "g_pMusicUI")]
doi_ten_ca_cay("Sources/S3Client", N2)

print("NHOM 3: g_pIInlinePicSink                            S3Client.cpp <-> Engine/Src/Text.cpp")
N3 = [("g_pIInlinePicSink", "g_pIInlinePicSinkUI")]
doi_ten_ca_cay("Sources/S3Client", N3)

print("NHOM 4: IID_IESClient / IID_IClientFactory           Rainbow/IClient.cpp <-> NetConnectAgent.cpp")
# Headers/IClient.h da co DEFINE_GUID; NetConnectAgent.cpp include <initguid.h> nen sinh DINH NGHIA.
# Hai ban trong IClient.cpp la thua va trung tung byte -> bo, dung khai bao cua header.
p = os.path.join(GOC, "Sources/MultiServer/Rainbow/IClient.cpp")
s = doc(p); n0 = bc(s)
if "static const GUID IID_" in s:
    dong, giu, i = s.splitlines(True), [], 0
    while i < len(dong):
        if dong[i].lstrip().startswith("static const GUID IID_"):
            giu.append("// [DONTRUNG 11/09] bo ban rieng: trung voi dinh nghia do DEFINE_GUID sinh ra ben\n")
            giu.append("// S3Client/NetConnect/NetConnectAgent.cpp (<initguid.h> + Headers/IClient.h). Gia tri y het.\n")
            i += 2                       # bo dong khai bao + dong gia tri
            continue
        giu.append(dong[i]); i += 1
    s = "".join(giu)
    if bc(s) != n0: sys.exit("LOI: IClient.cpp byte cao doi")
    ghi(p, s); tong += 2
    print("   %-50s bo 2 dinh nghia thua" % "Sources/MultiServer/Rainbow/IClient.cpp")
else:
    print("   %-50s  -  da bo tu truoc" % "Sources/MultiServer/Rainbow/IClient.cpp")

print("=" * 74)
print("xong va_dontrung_kyhieu.py - tong %d cho" % tong)
