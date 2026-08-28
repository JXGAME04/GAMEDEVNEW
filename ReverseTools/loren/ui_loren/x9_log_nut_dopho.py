# -*- coding: utf-8 -*-
"""x9_log_nut_dopho.py - LOG luong BAM NUT cua the Do pho + goi CapNhatNguyenLieu
khi tung o doi.

TRANG THAI HIEN TAI (doc tu loren_dopho.log cua chu game):
    [DOPHO]   UpdateItem: Region.v=0 -> khop o 0 (AtlasBox)
    [DOPHO]   UpdateItem: Region.v=1 -> khop o 1 (CryoliteBox)
    ... day du o 0..8
=> Phan HIEN THI da chay dung: mon vao o nao ve dung o do. Khop voi anh chu game.

CON HAI VIEC:
 (1) Bam nut "Ket hop" van khong hien gi. Tep log KHONG co dong nao cua luong
     bam nut, vi chua dat log o do. Them log vao ba khau:
        KUiAtlas::WndProc     - nut co nhan duoc cu bam khong
        KUiAtlas::Breathe     - may trang thai co chay khong, dung o buoc nao
        KUiAtlas::PlayEffect  - hieu ung chay toi khung thu may
        KUiAtlas::ProcessAtlas- co goi len may chu khong
     Ba khau nay dan toi ba nguyen nhan khac nhau:
        khong co dong WndProc  -> nut khong nhan duoc cu bam (vung bam/anh nut)
        co WndProc, khong Breathe -> nhip trang van chua chay
        Breathe ket o ATLASING -> PlayEffect khong bao gio tra 0 (anh hieu ung)
        co ProcessAtlas        -> lenh DA gui, loi nam ben may chu (Lua/luat ghep)

 (2) Nhan ten nguyen lieu chua hien, vi `CapNhatNguyenLieu()` chi duoc goi trong
     `UpdateAllItem` - ma duong thuc te chay la `UpdateItem` (tin bao tung o).
     Them mot loi goi o cuoi `UpdateItem`.

Chi THEM log va mot loi goi; khong doi logic nao khac.
Go log: x5_log_client_dopho.py --go --ghi (khoi phuc ban truoc log)

Tep dich: Sources\\S3Client\\Ui\\UiCase\\UiCompoundItem.cpp => build S3Client.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_lognut lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] log luong bam nut Do pho"
DICH = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiCompoundItem.cpp"
HAU_TO = ".truoc_lognut"


def K(*d):
    return list(d)


MIENG = [
    ("log WndProc nhan cu bam",
     K(T + "case WND_N_BUTTON_CLICK:",
       T*2 + "if (uParam == (unsigned int)&m_Atlas)",
       T*2 + "{"),
     K(T + "case WND_N_BUTTON_CLICK:",
       T*2 + "sDoPhoLog(\"[DOPHO] WndProc: BUTTON_CLICK, la nut Ket hop = %d, m_nStatus=%d\",",
       T*3 + "(uParam == (unsigned int)&m_Atlas) ? 1 : 0, m_nStatus);",
       T*2 + "if (uParam == (unsigned int)&m_Atlas)",
       T*2 + "{")),

    ("log Breathe",
     K("void KUiAtlas::Breathe()",
       "{",
       T + "if (m_nStatus == STATUS_BEGIN_ATLAS)"),
     K("void KUiAtlas::Breathe()",
       "{",
       T + "static int s_nTruoc = -1;",
       T + "if (m_nStatus != s_nTruoc)",
       T + "{",
       T*2 + "sDoPhoLog(\"[DOPHO] Breathe: m_nStatus %d -> %d\", s_nTruoc, m_nStatus);",
       T*2 + "s_nTruoc = m_nStatus;",
       T + "}",
       T + "if (m_nStatus == STATUS_BEGIN_ATLAS)")),

    ("log PlayEffect",
     K("int KUiAtlas::PlayEffect()",
       "{",
       T + "if (m_AtlasEffect.GetCurrentFrame() >= MAX_SPR_FRAME)"),
     K("int KUiAtlas::PlayEffect()",
       "{",
       T + "static int s_nDem = 0;",
       T + "if ((++s_nDem % 10) == 1)",
       T*2 + "sDoPhoLog(\"[DOPHO] PlayEffect: khung=%d / MAX=%d\",",
       T*3 + "m_AtlasEffect.GetCurrentFrame(), MAX_SPR_FRAME);",
       T + "if (m_AtlasEffect.GetCurrentFrame() >= MAX_SPR_FRAME)")),

    ("log ProcessAtlas",
     K(T + "if (g_pCoreShell)",
       T*2 + "g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 7, (unsigned int)\"LR_UI_Atlas\");"),
     K(T + "sDoPhoLog(\"[DOPHO] ProcessAtlas: GUI LENH LR_UI_Atlas len may chu\");",
       T + "if (g_pCoreShell)",
       T*2 + "g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 7, (unsigned int)\"LR_UI_Atlas\");")),

    # Neo HEP: cau truc cuoi UpdateItem giong nhau o ca 7 pad, nen phai neo bang
    # dong dau cua ham dung NGAY SAU no (chu thich cua CapNhatNguyenLieu) -
    # chi KUiAtlas moi co.
    ("goi CapNhatNguyenLieu khi tung o doi",
     K(T + "}",
       "}",
       "",
       "// [LOREN 28/08] Do pho: ve ten nguyen lieu tren trang."),
     K(T + "}",
       T + "// " + NHAN + ": duong thuc te chay la UpdateItem (tin bao tung o),",
       T + "// khong phai UpdateAllItem - nen phai goi o day thi nhan ten nguyen",
       T + "// lieu moi duoc lam moi.",
       T + "CapNhatNguyenLieu();",
       "}",
       "",
       "// [LOREN 28/08] Do pho: ve ten nguyen lieu tren trang.")),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== x9_log_nut_dopho - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)
    for ten, cu, moi in MIENG:
        vt = [i for i in range(len(dong) - len(cu) + 1) if dong[i:i + len(cu)] == cu]
        if len(vt) != 1:
            print("  !!! %-34s khop %d lan (can 1)" % (ten, len(vt)))
            return 1
        print("  ok  %-34s dong %d" % (ten, vt[0] + 1))
        dong = dong[:vt[0]] + moi + dong[vt[0] + len(cu):]

    nd = eol.join(dong)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if nd.count("{") - raw.count("{") != nd.count("}") - raw.count("}"):
        print("!!! LOI TO: ngoac lech")
        return 1
    print("  chot: CapNhatNguyenLieu duoc goi %d cho" % nd.count("CapNhatNguyenLieu();"))
    print("  byte cao %d (khong doi) | ngoac can bang" % hi0)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. => build S3Client, dat Game.exe, bam nut roi doc log")
    return 0


if __name__ == "__main__":
    sys.exit(main())
