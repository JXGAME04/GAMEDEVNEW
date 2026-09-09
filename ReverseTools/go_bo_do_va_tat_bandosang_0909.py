# -*- coding: utf-8 -*-
"""go_bo_do_va_tat_bandosang_0909.py - [DON 09/09] Go cac bo do da dung xong + [SANGTAT] bo qua dung ban do sang.

Chu: "tiep tuc 2 viec" (go bo do; cat RenderLightMap chay phi).
GO (giu nguyen co che + cong tac): [INTERP] (CoreShell.cpp), [SANGDO]/[SANGCPU] (KIpoTree.cpp),
[AMMAU] bo dem (KNpcRes.cpp; GIU ban sua khoi tao m_ulAdjustColorId, bo khoa SuaAmMau - luon sua),
[SANGDUNG] (KRepresentShell3.cpp). GIU dong log [LOCTG]/[CHUGIU] (re, cho biet co che dang bat/tat).
[SANGTAT]: he chieu sang dong da CHET (SetOption(DYNAMICLIGHT) gan false ca hai nhanh) nen RenderLightMap()
0,5 ms/khung bi vut di => bo qua mac dinh; [Client] BanDoSang=1 de chay lai nhu cu.
Xoa theo MOC DONG: tim dong chua moc, dong 'if' ngay truoc, dong '}' dong co cung thut le voi 'if'.
"""
import io
import sys

BS = chr(92)
CFG = '".' + BS + BS + 'config.ini"'
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources"
F_CS = ROOT + r"\Core\Src\CoreShell.cpp"
F_IPO = ROOT + r"\Core\Src\Scene\KIpoTree.cpp"
F_RES = ROOT + r"\Core\Src\KNpcRes.cpp"
F_R3 = ROOT + r"\Represent\Represent3\KRepresentShell3.cpp"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, s.count("\r\n"), s.count("\n") - s.count("\r\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, dong, eol, crlf0, lf0, h0, ten):
    s = eol.join(dong)
    lf = s.count("\n") - s.count("\r\n")
    ok = (s.count("\r\n") == 0) if crlf0 == 0 else (lf == lf0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "�" in s:
        print("FAIL ma hoa %s (byte cao %d/%d, LF don %d/%d)" % (ten, sum(1 for c in s if ord(c) >= 0x80), h0, lf, lf0)); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def thut(l):
    return l[:len(l) - len(l.lstrip("\t "))]


def tim_dong(dong, chua, ten, tu=0):
    idx = [i for i in range(tu, len(dong)) if chua in dong[i]]
    if len(idx) != 1:
        print("FAIL moc %s: thay %d, can 1" % (ten, len(idx))); sys.exit(1)
    return idx[0]


def xoa_khoi_if(dong, moc, ten):
    """dong moc la '{ // [..]' ; dong 'if' ngay truoc; dong dong la '}' cung thut le voi 'if'. Xoa [if .. }].
    Lay lan xuat hien DAU TIEN (cung mot moc co the co nhieu khoi - goi nhieu lan)."""
    idx = [k for k, l in enumerate(dong) if moc in l]
    if not idx:
        print("FAIL moc %s: khong thay" % ten); sys.exit(1)
    i = idx[0]
    a = i - 1
    if not dong[a].lstrip().startswith("if ("):
        print("FAIL %s: dong truoc moc khong phai if: %r" % (ten, dong[a])); sys.exit(1)
    lv = thut(dong[a])
    b = None
    for j in range(i + 1, len(dong)):
        if dong[j] == lv + "}":
            b = j; break
    if b is None:
        print("FAIL %s: khong thay dong dong" % ten); sys.exit(1)
    del dong[a:b + 1]
    return a


# ==================================================================== CoreShell.cpp
s, c0, l0, h0 = doc(F_CS); eol = "\r\n" if c0 else "\n"; dong = s.split(eol)
if any("[INTERP 09/09]" in l for l in dong):
    # A1: khoi khai bao + IpInDong; dong '}' cuoi mang theo duoi chu thich cua dong g_nCorePaintLog goc
    i = tim_dong(dong, "// [INTERP 09/09] Do TOAN BO", "CS A1")
    j = tim_dong(dong, "static void IpInDong()", "CS IpInDong")
    b = None
    for k in range(j + 1, len(dong)):
        if dong[k].startswith("}"):
            b = k; break
    duoi = dong[b][1:]                       # phan sau '}' = chu thich goc cua dong g_nCorePaintLog
    g = tim_dong(dong, "int\tg_nCorePaintLog = 0;", "CS g_nCorePaintLog")
    if g != i - 1:
        print("FAIL CS: g_nCorePaintLog khong ngay truoc khoi"); sys.exit(1)
    dong[g] = dong[g] + duoi
    del dong[i:b + 1]
    # g_nIpBuocK = nK; (khong con bien)
    k = tim_dong(dong, "g_nIpBuocK = nK;", "CS BuocK"); del dong[k]
    # A2, A3, A4
    xoa_khoi_if(dong, "// [INTERP 09/09] moi khung ve mot lan", "CS A2")
    xoa_khoi_if(dong, "// [INTERP 09/09] nut cay canh VAN nam o vi tri TICK", "CS A3")
    xoa_khoi_if(dong, "// [INTERP 09/09] chot ky va in moi 10 giay", "CS A4")
    if any("s_uIp" in l or "IpInDong" in l or "g_nIpBuocK" in l for l in dong):
        print("FAIL CS: con sot bien INTERP"); sys.exit(1)
    ghi(F_CS, dong, eol, c0, l0, h0, "CoreShell.cpp (go [INTERP])")
else:
    print("CoreShell.cpp: khong con [INTERP]")

# ==================================================================== KIpoTree.cpp
s, c0, l0, h0 = doc(F_IPO); eol = "\r\n" if c0 else "\n"; dong = s.split(eol)
if any("[SANGDO 09/09]" in l for l in dong):
    # bien toan cuc SANGCPU -> thay bang khoa BanDoSang
    g = tim_dong(dong, "double   g_dSangMs = 0.0, g_dSangMax = 0.0;", "IPO gvar")
    if "unsigned g_uSangLan = 0;" not in dong[g + 1]:
        print("FAIL IPO: g_uSangLan khong ngay sau"); sys.exit(1)
    dong[g:g + 2] = [
        "// [SANGTAT 09/09] He chieu sang dong da CHET tu lau: KRepresentShell3::SetOption(DYNAMICLIGHT) gan",
        "// m_bDoLighting = false o CA hai nhanh (dong 551-554) va CoreShell.cpp:23014 co goi, nen ket qua cua",
        "// RenderLightMap() (luoi 48x96 o, ~40 nguon sang, ~400 o/nguon kem sqrt) bi VUT DI - do duoc 0,5 ms/khung",
        "// o 143 Hz ([SANGDO]); bo do [SANGDUNG] xac nhan 0 lenh ve qua nhanh co chieu sang. Nay BO QUA mac dinh.",
        "// [Client] BanDoSang = 1 de dung lai nhu cu (chi co y nghia neu sau nay sua SetOption cho den song lai).",
        "static int s_nBanDoSang = -1;",
        "static int BanDoSang()",
        "{",
        "\tif (s_nBanDoSang < 0)",
        "\t\ts_nBanDoSang = (int)GetPrivateProfileIntA(\"Client\", \"BanDoSang\", 0, " + CFG + ") ? 1 : 0;",
        "\treturn s_nBanDoSang;",
        "}",
    ]
    # khoi Paint: tu dong if(eLayer == IPOT_RL_COVER_GROUND ...) den dong '\t}' dong
    a = tim_dong(dong, "if(eLayer == IPOT_RL_COVER_GROUND && m_bDynamicLighting && g_pRepresent && g_pRepresent->IsRep3D())", "IPO Paint if")
    b = None
    for k in range(a + 1, len(dong)):
        if dong[k] == "\t}":
            b = k; break
    if b is None or not any("[SANGDO 09/09]" in l for l in dong[a:b]):
        print("FAIL IPO: khoi Paint khong dung"); sys.exit(1)
    # giu nguyen cac dong chu thich goc co BYTE CAO (tieng Trung) trong khoi cu: 1 truoc RenderLightMap,
    # 1 truoc SetLightInfo - khong duoc lam mat noi dung goc
    cao = [l.strip() for l in dong[a:b + 1] if any(ord(c) >= 0x80 for c in l)]
    if len(cao) != 2:
        print("FAIL IPO: khoi cu co %d dong byte cao, du kien 2" % len(cao)); sys.exit(1)
    dong[a:b + 1] = [
        "\tif(eLayer == IPOT_RL_COVER_GROUND && m_bDynamicLighting && g_pRepresent && g_pRepresent->IsRep3D())",
        "\t{",
        "\t\tif (BanDoSang())",
        "\t\t{",
        "\t\t\t" + cao[0],
        "\t\t\tRenderLightMap();",
        "\t\t\t" + cao[1],
        "\t\t\tg_pRepresent->SetLightInfo(m_nLeftTopX, m_nLeftTopY, (unsigned int*)pLightingArray);",
        "\t\t}",
        "\t\telse",
        "\t\t{\t// [SANGTAT 09/09] khong dung ban do sang; bao shell mot lan de chac m_bDoLighting = false",
        "\t\t\tstatic bool s_bDaBao = false;",
        "\t\t\tif (!s_bDaBao) { g_pRepresent->SetLightInfo(0, 0, NULL); s_bDaBao = true; }",
        "\t\t}",
        "\t}",
    ]
    # StrewRtoLeafs: vat can chi phuc vu ban do sang
    k = tim_dong(dong, "\tif(m_bDynamicLighting && g_pRepresent && g_pRepresent->IsRep3D())", "IPO Strew")
    dong[k] = "\tif(m_bDynamicLighting && BanDoSang() && g_pRepresent && g_pRepresent->IsRep3D())\t// [SANGTAT 09/09]"
    # chi bat DINH DANH ma va the day du; chu thich [SANGTAT] moi co nhac "[SANGDO]" la co y
    if any("g_dSangMs" in l or "g_uSangLan" in l or "liSang" in l or "[SANGDO 09/09]" in l for l in dong):
        print("FAIL IPO: con sot"); sys.exit(1)
    ghi(F_IPO, dong, eol, c0, l0, h0, "KIpoTree.cpp (go [SANGDO]/[SANGCPU], bo qua ban do sang)")
else:
    print("KIpoTree.cpp: khong con [SANGDO]")

# ==================================================================== KNpcRes.cpp
s, c0, l0, h0 = doc(F_RES); eol = "\r\n" if c0 else "\n"; dong = s.split(eol)
if any("AmMauInDong" in l for l in dong):
    i = tim_dong(dong, "// [AMMAU 09/09] Dem so o ve bi AM MAU OAN", "RES khoi")
    j = tim_dong(dong, "static void AmMauInDong()", "RES ham")
    b = None
    for k in range(j + 1, len(dong)):
        if dong[k] == "}":
            b = k; break
    if dong[b + 1].strip() == "":
        b += 1
    del dong[i:b + 1]
    # Init: bo khoa SuaAmMau, luon dat lai
    i = tim_dong(dong, "// [AMMAU 09/09] Khe NPC doi chu thi phai xoa mau", "RES Init")
    b = None
    for k in range(i + 1, i + 8):
        if dong[k] == "\t\tm_ulAdjustColorId = 0;":
            b = k; break
    if b is None:
        print("FAIL RES Init"); sys.exit(1)
    dong[i:b + 1] = ["\tm_ulAdjustColorId = 0;\t// [AMMAU 09/09] khe NPC doi chu: xoa mau bang/doc/chay cua NGUOI CU (truoc day khong khoi tao)"]
    k = tim_dong(dong, "\tAmMauInDong();", "RES goi"); del dong[k]
    xoa_khoi_if(dong, "// [AMMAU 09/09] co that su dang bang/doc/chay khong?", "RES dem 1")
    xoa_khoi_if(dong, "// [AMMAU 09/09] co that su dang bang/doc/chay khong?", "RES dem 2")
    if any("s_uAm" in l or "s_nSuaAmMau" in l or "g_nCorePaintLog" in l for l in dong):
        print("FAIL RES: con sot"); sys.exit(1)
    ghi(F_RES, dong, eol, c0, l0, h0, "KNpcRes.cpp (go bo dem [AMMAU], giu ban sua)")
else:
    print("KNpcRes.cpp: khong con [AMMAU]")

# ==================================================================== KRepresentShell3.cpp
s, c0, l0, h0 = doc(F_R3); eol = "\r\n" if c0 else "\n"; dong = s.split(eol)
if any("g_uRep3VeCoSang" in l for l in dong):
    k = tim_dong(dong, "unsigned g_uRep3VeCoSang = 0;", "R3 var"); del dong[k]
    n = 0
    for i, l in enumerate(dong):
        if "? (++g_uRep3VeCoSang, true) : false)" in l:
            dong[i] = l.replace(" ? (++g_uRep3VeCoSang, true) : false)", ")").split("\t// [SANGDUNG")[0]
            n += 1
    if n != 3:
        print("FAIL R3: dem duoc %d cho if, can 3" % n); sys.exit(1)
    k = tim_dong(dong, "Rep3Log(\"[SANGDUNG]", "R3 log")
    if "g_uRep3VeCoSang = 0;" not in dong[k + 1]:
        print("FAIL R3: dong reset"); sys.exit(1)
    del dong[k:k + 2]
    if any("g_uRep3VeCoSang" in l or "SANGDUNG" in l for l in dong):
        print("FAIL R3: con sot"); sys.exit(1)
    ghi(F_R3, dong, eol, c0, l0, h0, "KRepresentShell3.cpp (go [SANGDUNG])")
else:
    print("KRepresentShell3.cpp: khong con [SANGDUNG]")
print("XONG DON + SANGTAT")
