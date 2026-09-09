# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 CAN/DOLUOT/MAILBOX] Ba viec rieng mobile sau khi gop main (VUNG/NAPCHIEU/NAPNPC...) vao mobile-0809:
#
# 1. CAN DIEU KHIEN AO gui lenh di bo qua nhieu (goc lag chi co o mobile):
#    JxCan_Nhip() goi g_pCoreShell->Goto(huong, 0) MOI VONG LAP KSdlApp::Run (8 ms, hay 1 ms khi PaintFps > 60).
#    Goto -> KPlayer::Walk -> SendClientCmdWalk KHONG co cong gac => 125..1000 goi c2s_npcwalk/giay khi giu can.
#    Moi goi: may chu KNpc::Goto/NewPath (tim duong) + KNpc::DoWalk phat NPC_WALK_SYNC cho CA VUNG + 8 vung ke
#    (MAX_BROADCAST_COUNT nguoi) => trong Tong Kim nhan len theo so nguoi. Ban PC giu chuot chi gui 1 goi / 5 tick
#    (GotoWhere: m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME), nguoi choi PC lai hay bam tung cu.
#    Sua: CoreShell.cpp them JxCore_GotoHuong(huong, mode, nBuoc, bEp) (chi JX_ANDROID): nhu Goto nhung dich xa nBuoc
#    buoc (Goto = 2) va co cong gac 5 tick nhu chuot PC; bEp = 1 gui ngay (doi huong / dung lai). JxCan_Nhip: doi huong
#    -> gui ngay (toi da 1 lan / 110 ms), cung huong -> theo cong gac; nha can / ngon ve giua -> MOT lenh dich gan 2 buoc
#    de dung (dung nhu truoc). Cam giac di chuyen giu nguyen, so goi giam ~30-300 lan. [Cham] CanBuocXa=8 (2 = nhu cu).
#
# 2. DOLUOT (main 09/09) dung SuspendThread/GetThreadContext/dbghelp/_beginthreadex - khong co trong lop JX_POSIX:
#    rao #ifndef JX_POSIX, Android dung 2 ham rong (DoLuotBatDau tra 0 = tat). Windows khong doi.
#
# 3. LDPlayer/dien thoai khong ho tro SDL_GPU_PRESENTMODE_IMMEDIATE ([GPU] Reset: vsync=1 du config vsync=0) ->
#    SDL_WaitAndAcquireGPUSwapchainTexture cho vblank + lop dich cua may ao ([PDET] end 16-28 ms moi khung).
#    Them MAILBOX (khong cho vblank, khong xe hinh) khi IMMEDIATE khong co; chi JX_ANDROID; [Client] Rep3GpuMailbox=0 de tat.
#
# Chay lai vo hai (co dau DAU). Doc/ghi latin-1, giu CRLF. Chi them dong ASCII: so byte cao cua moi tep khong doi.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 CAN]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def chen_sau(s, mau, dong_moi, ten, so=1):
    """Chen cac dong dong_moi ngay SAU dong (duy nhat) chua chuoi mau."""
    nl = nl_cua(s)
    dong = s.split(nl)
    idx = [i for i, d in enumerate(dong) if mau in d]
    if len(idx) != so:
        raise SystemExit("khong tim thay dung %d dong chua (%d): %s" % (so, len(idx), ten))
    for i in reversed(idx):
        dong[i + 1:i + 1] = dong_moi
    return nl.join(dong)


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


# ---------------------------------------------------------------- 1a. CoreShell.cpp: JxCore_GotoHuong
def va_coreshell(s):
    khoi = [
        "}",
        "",
        "#ifdef JX_ANDROID",
        "// %s Di theo huong cho CAN DIEU KHIEN AO (JxCanDieuKhien.cpp): nhu Goto() nhung dich xa nBuoc buoc (Goto = 2 buoc)" % DAU,
        "// va co CONG GAC gui giong duong chuot cua ban PC (GotoWhere: m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME = 5 tick).",
        "// Truoc day JxCan_Nhip goi Goto() MOI VONG LAP (1-8 ms): moi lan mot goi c2s_npcwalk, may chu tim duong + KNpc::DoWalk phat",
        "// s2c_npcwalk cho ca vung => hang tram goi/giay chi de di bo, trong Tong Kim nhan len theo so nguoi. bEp = 1: gui ngay",
        "// (doi huong / dung lai), dat lai cong gac. Tra 1 khi da gui. Chi Android: ban Windows khong co ham nay.",
        "extern \"C\" int JxCore_GotoHuong(int nDir, int mode, int nBuoc, int bEp)",
        "{",
        "\tif (nDir < 0 || nDir > 63 || mode < 0 || mode > 2 || nBuoc < 1)",
        "\t\treturn 0;",
        "\tif (!bEp && Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames < defMAX_PLAYER_SEND_MOVE_FRAME)",
        "\t\treturn 0;",
        "\tint nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;",
        "\tif (nIndex <= 0 || nIndex >= MAX_NPC || Player[CLIENT_PLAYER_INDEX].CheckTrading())",
        "\t\treturn 0;",
        "\tint nSubWorld = Npc[nIndex].m_SubWorldIndex;",
        "\tif (nSubWorld < 0 || Npc[nIndex].m_RegionIndex < 0)",
        "\t\treturn 0;",
        "\tint bRun = ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) || mode == 2) ? 1 : 0;",
        "\tint nSpeed = (int)(bRun ? Npc[nIndex].m_CurrentRunSpeed : Npc[nIndex].m_CurrentWalkSpeed);",
        "\tint nX = 0, nY = 0;",
        "\tSubWorld[nSubWorld].Map2Mps(Npc[nIndex].m_RegionIndex, Npc[nIndex].m_MapX, Npc[nIndex].m_MapY, Npc[nIndex].m_OffX, Npc[nIndex].m_OffY, &nX, &nY);",
        "\tSubWorld[nSubWorld].GetMps(&nX, &nY, nSpeed * nBuoc, nDir);",
        "\tif (bRun)",
        "\t{",
        "\t\tNpc[nIndex].SendCommand(do_run, nX, nY);",
        "\t\tSendClientCmdRun(nX, nY);",
        "\t}",
        "\telse",
        "\t{",
        "\t\tNpc[nIndex].SendCommand(do_walk, nX, nY);",
        "\t\tSendClientCmdWalk(nX, nY);",
        "\t}",
        "\tPlayer[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;",
        "\treturn 1;",
        "}",
        "#endif\t// JX_ANDROID",
        "",
        "void KCoreShell::Turn(int nDir)",
    ]
    return thay(s, ["}", "", "void KCoreShell::Turn(int nDir)"], khoi, "CoreShell.cpp: truoc KCoreShell::Turn")


# ---------------------------------------------------------------- 1b. JxCanDieuKhien.cpp: cong gac + dung lai
def va_can(s):
    nl = nl_cua(s)
    dong = s.split(nl)
    i = [k for k, d in enumerate(dong) if d.startswith("extern int") and d.rstrip().endswith("SCREEN_HEIGHT;")]
    if len(i) != 1:
        raise SystemExit("JxCanDieuKhien.cpp: extern SCREEN_HEIGHT")
    dong[i[0] + 1:i[0] + 1] = ["extern \"C\" int\t\tJxCore_GotoHuong(int nDir, int mode, int nBuoc, int bEp);\t// %s CoreShell.cpp (chi Android)" % DAU]
    s = nl.join(dong)
    s = chen_sau(s, "s_nNguong = 14;", [
        "static int\ts_nBuocXa = 8;\t\t// %s dich xa bao nhieu buoc moi lan gui lenh di (Goto cu = 2); [Cham] CanBuocXa" % DAU,
    ], "static s_nNguong")
    s = chen_sau(s, "s_nTamX = 0, s_nTamY = 0;", [
        "static int\ts_nHuongGui = -1;\t\t// %s huong da gui may chu; -1 = chua gui / da gui lenh dung" % DAU,
        "static unsigned int\ts_uCanGuiLuc = 0;\t// %s luc gui gan nhat (doi huong lien tuc cung chi gui 1 lan / 110 ms)" % DAU,
    ], "static s_nTamX")
    s = chen_sau(s, 's_nNguong   = GetPrivateProfileInt("Cham", "CanNguong", 14, szCfg);', [
        '\ts_nBuocXa   = GetPrivateProfileInt("Cham", "CanBuocXa", 8, szCfg);\t// %s' % DAU,
    ], "DocCaiDat CanNguong")
    s = chen_sau(s, "if (s_nNguong < 4) s_nNguong = 4;", [
        "\tif (s_nBuocXa < 2) s_nBuocXa = 2;\t// %s 2 = dich gan nhu Goto cu (thi phai gui lien tuc moi khong dung)" % DAU,
        "\tif (s_nBuocXa > 30) s_nBuocXa = 30;",
    ], "DocCaiDat kep s_nNguong")
    cu = [
        "void JxCan_Nha()",
        "{",
        "\ts_bCam = false;",
        "}",
        "",
        "void JxCan_Nhip()",
        "{",
        "\tif (!s_bCam || !g_pCoreShell)",
        "\t\treturn;",
        "\tint dx = s_nNgonX - s_nTamX;",
        "\tint dy = s_nNgonY - s_nTamY;",
        "\tif (dx * dx + dy * dy < s_nNguong * s_nNguong)",
        "\t\treturn;\t\t// ngon tay con o giua can -> dung yen",
        "\tg_pCoreShell->Goto(s_nHuong, 0);\t// mode 0 = tu chon di bo / chay nhu ban PC",
        "}",
    ]
    moi = [
        "// %s Dung lai: gui MOT lenh dich gan (2 buoc, dung nhu Goto() cu) roi thoi -> nhan vat dung trong ~0,1 s." % DAU,
        "static void JxCan_DungLai()",
        "{",
        "\tif (s_nHuongGui < 0)",
        "\t\treturn;",
        "\tif (g_pCoreShell)",
        "\t\tJxCore_GotoHuong(s_nHuongGui, 0, 2, 1);",
        "\ts_nHuongGui = -1;",
        "\ts_uCanGuiLuc = (unsigned int)GetTickCount();",
        "}",
        "",
        "void JxCan_Nha()",
        "{",
        "\ts_bCam = false;",
        "\tJxCan_DungLai();",
        "}",
        "",
        "void JxCan_Nhip()",
        "{",
        "\tif (!s_bCam || !g_pCoreShell)",
        "\t\treturn;",
        "\tint dx = s_nNgonX - s_nTamX;",
        "\tint dy = s_nNgonY - s_nTamY;",
        "\tif (dx * dx + dy * dy < s_nNguong * s_nNguong)",
        "\t{",
        "\t\tJxCan_DungLai();\t// ngon tay ve giua can -> dung lai",
        "\t\treturn;",
        "\t}",
        "\t// %s Truoc: g_pCoreShell->Goto(s_nHuong, 0) MOI VONG LAP (1-8 ms) = moi lan mot goi c2s_npcwalk khong cong gac," % DAU,
        "\t// may chu tim duong + phat s2c_npcwalk cho ca vung => hang tram goi/giay chi de di bo (PC giu chuot: 1 goi / 5 tick).",
        "\t// Nay: doi huong -> gui ngay (toi da 1 lan / 110 ms); cung huong -> gui lai khi cong gac 5 tick mo (JxCore_GotoHuong);",
        "\t// dich xa s_nBuocXa buoc nen nhan vat khong dung giua hai lan gui. Cam giac di chuyen giu nguyen.",
        "\tunsigned int uNay = (unsigned int)GetTickCount();",
        "\tint bEp = (s_nHuong != s_nHuongGui && (s_nHuongGui < 0 || uNay - s_uCanGuiLuc >= 110)) ? 1 : 0;",
        "\tif (JxCore_GotoHuong(s_nHuong, 0, s_nBuocXa, bEp))",
        "\t{",
        "\t\ts_nHuongGui = s_nHuong;",
        "\t\ts_uCanGuiLuc = uNay;",
        "\t}",
        "}",
    ]
    return thay(s, cu, moi, "JxCanDieuKhien.cpp: JxCan_Nha + JxCan_Nhip")


# ---------------------------------------------------------------- 2. KSubWorld.cpp: rao DOLUOT
def va_subworld(s):
    s = thay(s,
        ["// [DOLUOT 09/09] BO LAY MAU con tro lenh luong chinh ([Client] DoLuot=1; mac dinh 0 = khong tao luong, khong ton gi)."],
        ["#ifndef JX_POSIX\t// [ANDROID 11/09 DOLUOT] SuspendThread/GetThreadContext/dbghelp/_beginthreadex chi co tren Windows; Android: hai ham rong o #else",
         "// [DOLUOT 09/09] BO LAY MAU con tro lenh luong chinh ([Client] DoLuot=1; mac dinh 0 = khong tao luong, khong ton gi)."],
        "KSubWorld.cpp: dau khoi DOLUOT")
    s = thay(s,
        ["}", "", "void WorldTickXong(double dQuet)", "{"],
        ["}",
         "#else",
         "LONG DoLuotBatDau(int nPha, LARGE_INTEGER* pLi0) { (void)nPha; (void)pLi0; return 0; }\t// [ANDROID 11/09 DOLUOT] khong lay mau tren Android (0 = tat)",
         "void DoLuotKetThuc(int nPha, LONG lSeq, const LARGE_INTEGER& li0) { (void)nPha; (void)lSeq; (void)li0; }",
         "#endif\t// JX_POSIX",
         "",
         "void WorldTickXong(double dQuet)", "{"],
        "KSubWorld.cpp: cuoi khoi DOLUOT (truoc WorldTickXong)")
    return s


# ---------------------------------------------------------------- 3. MAILBOX
def va_gpudev(s):
    dong_imm = "\tif (m_pp.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE && SDL_WindowSupportsGPUPresentMode(m_pGpu, m_pWin, SDL_GPU_PRESENTMODE_IMMEDIATE)) pm = SDL_GPU_PRESENTMODE_IMMEDIATE;"
    s = thay(s, [dong_imm], [
        dong_imm,
        "#ifdef JX_ANDROID",
        "\tif (pm == SDL_GPU_PRESENTMODE_VSYNC && m_pp.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE && g_nRep3GpuMailbox && SDL_WindowSupportsGPUPresentMode(m_pGpu, m_pWin, SDL_GPU_PRESENTMODE_MAILBOX))",
        "\t\tpm = SDL_GPU_PRESENTMODE_MAILBOX;\t// [ANDROID 11/09 MAILBOX] IMMEDIATE khong co (LDPlayer / dien thoai): MAILBOX khong cho vblank, khong xe hinh; Rep3GpuMailbox=0 de tat",
        "#endif",
    ], "D3D9onGPUDev.cpp: chon present mode (Create + Reset)", so=2)
    s = thay(s, ['pm == SDL_GPU_PRESENTMODE_IMMEDIATE ? "ngay" : "vsync"'],
             ['pm == SDL_GPU_PRESENTMODE_IMMEDIATE ? "ngay" : (pm == SDL_GPU_PRESENTMODE_MAILBOX ? "mailbox" : "vsync")'],
             "D3D9onGPUDev.cpp: log trinh chieu")
    return s


def va_baseinclude(s):
    return chen_sau(s, "extern int  g_nRep3GpuBoBanCpu;", [
        "extern int  g_nRep3GpuMailbox;\t// [ANDROID 11/09 MAILBOX] [Client] Rep3GpuMailbox: dung MAILBOX khi IMMEDIATE khong co; mac dinh 1 Android, 0 Windows",
    ], "BaseInclude.h: extern g_nRep3GpuBoBanCpu")


def va_shell3(s):
    s = chen_sau(s, "int g_nRep3GpuBoBanCpu = 1;", ["int g_nRep3GpuMailbox = 1;\t// [ANDROID 11/09 MAILBOX] Android: mac dinh BAT"], "KRepresentShell3.cpp: BoBanCpu=1")
    s = chen_sau(s, "int g_nRep3GpuBoBanCpu = 0;", ["int g_nRep3GpuMailbox = 0;\t// [ANDROID 11/09 MAILBOX] Windows GameSDL.exe: tat"], "KRepresentShell3.cpp: BoBanCpu=0")
    s = chen_sau(s, 'Rep3Ini("Rep3GpuBoBanCpu"', ['\tg_nRep3GpuMailbox  = Rep3Ini("Rep3GpuMailbox", g_nRep3GpuMailbox) ? 1 : 0;\t// [ANDROID 11/09 MAILBOX]'], "KRepresentShell3.cpp: doc ini BoBanCpu")
    return s


# ---------------------------------------------------------------- config lop ghi de
def va_config(s):
    nl = nl_cua(s)
    s = thay(s, ["PaintFps=120"], [
        "; [ANDROID 11/09] -1 = theo tan so man hinh (60 tren LDPlayer / da so dien thoai). 120 chi tang vong lap len 1 ms va ton pin, man 60 Hz khong ve them duoc.",
        "PaintFps=-1",
    ], "config.ini PaintFps")
    s = thay(s, ["Rep3GpuBoBanCpu=1"], [
        "Rep3GpuBoBanCpu=1",
        "; [ANDROID 11/09 MAILBOX] 1 = khi may khong co present mode IMMEDIATE (LDPlayer, dien thoai) thi dung MAILBOX: khong cho vblank, khong xe hinh. 0 = vsync nhu cu.",
        "Rep3GpuMailbox=1",
    ], "config.ini Rep3GpuBoBanCpu")
    if "[Cham]" not in s:
        s = s.rstrip("\r\n") + nl + nl + "[Cham]" + nl
    s = s.rstrip("\r\n") + nl + "; [ANDROID 11/09 CAN] can dieu khien ao: moi lan gui lenh di thi dich xa bao nhieu buoc (8 = mac dinh; 2 = dich gan nhu ban cu)." + nl + "CanBuocXa=8" + nl
    return s


va("Sources/Core/Src/CoreShell.cpp", DAU, va_coreshell)
va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", DAU, va_can)
va("Sources/Core/Src/KSubWorld.cpp", "[ANDROID 11/09 DOLUOT]", va_subworld)
va("Sources/Represent/Represent3/D3D9onGPUDev.cpp", "[ANDROID 11/09 MAILBOX]", va_gpudev)
va("Sources/Represent/Represent3/BaseInclude.h", "[ANDROID 11/09 MAILBOX]", va_baseinclude)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", "[ANDROID 11/09 MAILBOX]", va_shell3)
va("android/du_lieu_ghi_de/config.ini", "[ANDROID 11/09 MAILBOX]", va_config)
print("xong")
