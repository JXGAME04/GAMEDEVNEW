# -*- coding: utf-8 -*-
# vhtd_engine_patch12.py [VHTD 02/09w]
# DOT 13 - hai viec Hoa Son chu bao o dot 11:
#   (A) Bong mo Huyen Nhan Van Yen (1358) NGAN + GIUT
#   (B) Ma Van Kiem Khi (1380) khong no / khong mat khi bam Than Quang Toan Nhieu (1384)
#
# ---------------------------------------------------------------------------
# (A) BONG MO - hai loi doc lap, deu phia client
#
# A-1: MOT trong bay anh KHONG BAO GIO duoc ve.
#      KNpcBlur::SetNextNo chay chi so o 0..6 va dang ky bang CGOG_NPC_BLUR_DETAIL(m_nCurNo).
#      CGOG_NPC_BLUR_DETAIL(0) rut gon DUNG BANG CGOG_NPC_BLUR = 8, nhung switch trong
#      CoreDrawGameObj.cpp chi co case cho 1..7 -> anh o o 0 roi vao default va bi vut.
#      (case DETAIL(7) nguoc lai la MA CHET: KNpcBlur::Draw chan nIdx >= MAX_BLUR_FRAME = 7.)
#      Loi nay co tu ban goc, anh huong ca bong mo chieu luot.
#
# A-2: NHIP CHUP bam theo SO LAN VE chu khong phai nhip logic.
#      KNpcBlur::NowGetBlur() tra TRUE khi m_dwTimer == 0. m_dwTimer chi nhich o khung ve
#      DAU TIEN cua moi nhip logic (chot m_dwLastTick) va chi ve 0 moi 3 nhip (m_dwInterval = 3).
#      Vay o dung nhip bo dem vua ve 0, ham tra TRUE cho MOI khung ve cua nhip do.
#      Client chay PaintFps=60 tren nhip logic 18 = 3,33 khung ve/nhip -> cu 3 nhip lai ha
#      MOT CHUM 3-4 anh chong khit roi im 2 nhip. Bay o bi hai chum an het trong ~0,33 giay
#      -> vet chi con ~2 vi tri phan biet duoc, alpha chua kip giam nen cac bong gan nhu DAC.
#      Va: chot MOT anh moi nhip logic cho moi npc.
#
# Sau A-1 + A-2: bay bong rieng biet, cach deu 3 nhip, vet trai 21 nhip ~ 1,17 giay,
# alpha giam deu 128 -> 16. Tuc tra ve DUNG THIET KE GOC.
#
# CAM (keo pha bong mo chieu luot): doi MAX_BLUR_FRAME (lech bo cuc struct qua ranh gioi
# CoreClient.dll <-> Game.exe), doi BLUR_ALPHA_CHANGE / START_BLUR_ALPHA, doi m_dwInterval = 3.
#
# ---------------------------------------------------------------------------
# (B) KICH NO MA VAN KIEM KHI - ba tang cung chan
#
# Tang 1: KNpc::DetonateMissles bi boc #ifdef _SERVER -> ban client KHONG CO ma kich no.
#         Do tren nhi phan dang chay: chuoi "HS-DETONATE" co 1 lan trong CoreServer.dll,
#         0 lan trong CoreClient.dll.
# Tang 2: JX1 KHONG dong bo tung vien dan qua mang. May chu no roi gui GWM_MISSLE_DEL vao
#         hang doi NOI BO (KWorldMsg::m_LocalMsgQueue). Ban sao dan tren may nguoi choi VAN
#         SONG tiep den het LifeTime = 360 khung ~ 20 giay - dung y hien tuong chu mo ta.
# Tang 3: ca duong ap thuoc tinh candetonate deu la ma may chu -> BAT BUOC them goi mang.
#
# Tin tot: client DA CO san nua con lai. KMissle::DoVanish phia client dat m_bHaveEnd = TRUE
# roi chay khoi `if (m_bVanishedEvent) pOrdinSkill->Vanish(this);` - khoi nay KHONG nam trong
# guard nao. Chi can bao cho client, no TU sinh dan 420 tai cho = hieu ung no, va tu don khi
# truong. Khong co nguy co sat thuong kep vi KMissle::ProcessDamage la server-only.
#
# LUU Y: khi truong se bien mat TUC KHAC, KHONG co hoat anh tan (DoVanish phia client khong
# goi CreateSpecialEffect). "Hieu ung no" chu nhin thay chinh la dan 420 moi sinh.
#
# ---------------------------------------------------------------------------
# DINH CHINH bao cao tac tu: diem neo "KNpc.h dong 879 nam san trong #ifdef _SERVER" la SAI
# (chi thi tien xu ly gan nhat truoc do la #endif o dong 792). Theo dung mau san co cua
# HS_ResetBuffTime: KHAI BAO trong .h KHONG guard, DINH NGHIA trong .cpp trong #ifdef _SERVER.
#
# Doc/ghi latin-1 tung tep, TON TRONG kieu xuong dong rieng cua tung tep
# (KProtocolProcess.cpp dung LF, cac tep con lai CRLF) va GIU NGUYEN BOM cua KProtocol.h
# (tep co BOM = moi literal da boc cp1258->UTF-8; chi chen ASCII, TUYET DOI khong bo BOM).
#
# DUNG: python vhtd_engine_patch12.py --kiem   (thu, khong ghi)
#       python vhtd_engine_patch12.py          (ghi that)
import io
import os
import sys

KIEM = "--kiem" in sys.argv
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

MK = "[VHTD 02/09w]"
SRC = "D:/GAMEDEVNEW/Sources/Core/Src"
HDR = "D:/GAMEDEVNEW/Headers"
T = chr(9)


class Tep(object):
    """Doc mot tep nguon theo DONG, giu nguyen kieu xuong dong va BOM."""

    def __init__(self, path):
        self.path = path
        b = io.open(path, "rb").read()
        self.bom = b[:3] == b"\xef\xbb\xbf"
        self.cao_truoc = sum(1 for c in b if c >= 0x80)
        s = b.decode("latin-1")
        self.nl = "\r\n" if s.count("\r\n") * 2 > s.count("\n") else "\n"
        self.lines = s.split(self.nl)
        self.doi = 0

    def tim(self, sub, tu=0):
        """Chi so dong DUY NHAT chua sub. Bao loi neu 0 hoac nhieu hon 1."""
        hit = [i for i in range(tu, len(self.lines)) if sub in self.lines[i]]
        if len(hit) != 1:
            raise SystemExit("  [X] %s: neo %r khop %d dong (can dung 1)"
                             % (os.path.basename(self.path), sub[:60], len(hit)))
        return hit[0]

    def chen_truoc(self, sub, khoi):
        i = self.tim(sub)
        self.lines[i:i] = khoi
        self.doi += 1

    def chen_sau(self, sub, khoi):
        i = self.tim(sub)
        self.lines[i + 1:i + 1] = khoi
        self.doi += 1

    def doi_dong(self, sub, moi):
        i = self.tim(sub)
        self.lines[i] = moi
        self.doi += 1

    def xoa_dong(self, i):
        del self.lines[i]
        self.doi += 1

    def ghi(self):
        s = self.nl.join(self.lines)
        b = s.encode("latin-1")
        if self.bom and b[:3] != b"\xef\xbb\xbf":
            raise SystemExit("  [X] %s: MAT BOM" % self.path)
        cao = sum(1 for c in b if c >= 0x80)
        if cao != self.cao_truoc:
            raise SystemExit("  [X] %s: lech byte cao %d -> %d" % (self.path, self.cao_truoc, cao))
        if b"\xef\xbf\xbd" in (b[3:] if self.bom else b):
            raise SystemExit("  [X] %s: co EF BF BD" % self.path)
        if not KIEM:
            io.open(self.path, "wb").write(b)
        print("  => %s %-24s (%d cho)" % ("KIEM" if KIEM else "GHI ", os.path.basename(self.path), self.doi))


# =========================== (A) BONG MO ===========================

def va_bongmo_a1():
    """CoreDrawGameObj.cpp: them case cho o 0 (CGOG_NPC_BLUR == CGOG_NPC_BLUR_DETAIL(0))."""
    f = Tep(SRC + "/CoreDrawGameObj.cpp")
    if any(MK in x for x in f.lines):
        print("  [=] CoreDrawGameObj.cpp da co " + MK)
        return
    f.chen_truoc(
        "case CGOG_NPC_BLUR_DETAIL(1):",
        [T + "// " + MK + " A-1: SetNextNo chay chi so o 0..6 va dang ky CGOG_NPC_BLUR_DETAIL(m_nCurNo).",
         T + "// DETAIL(0) rut gon DUNG BANG CGOG_NPC_BLUR = 8 nhung switch nay truoc gio chi co case 1..7",
         T + "// -> anh o o 0 roi vao default va bi vut, vet bong mo thieu deu 1/7 (ca chieu luot).",
         T + "case CGOG_NPC_BLUR:",
         T * 2 + "Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(0);",
         T * 2 + "break;"])
    f.ghi()


def va_bongmo_a2():
    """KNpcRes.cpp: chot mot anh bong mo moi NHIP LOGIC thay vi moi khung ve."""
    f = Tep(SRC + "/KNpcRes.cpp")
    if any(MK in x for x in f.lines):
        print("  [=] KNpcRes.cpp da co " + MK)
        return
    f.chen_truoc(
        "void\tKNpcRes::Draw(int nNpcIdx, int nDir",
        ["// " + MK + " A-2: NowGetBlur() tra TRUE khi m_dwTimer == 0, ma m_dwTimer chi nhich o khung ve DAU TIEN",
         "// cua moi nhip logic va chi ve 0 moi m_dwInterval = 3 nhip. Vay o dung nhip bo dem vua ve 0, ham do tra",
         "// TRUE cho MOI khung ve cua nhip -> PaintFps=60 tren nhip logic 18 (3,33 khung/nhip) ha MOT CHUM 3-4 anh",
         "// chong khit roi im 2 nhip. Bay o bi hai chum an het trong ~0,33 giay nen vet vua NGAN vua GIUT.",
         "// Chot dung MOT anh moi nhip logic cho moi npc -> 7 bong cach deu 3 nhip, vet trai ~1,17 giay.",
         "// KHONG dung bien thanh vien (KNpcBlur nam trong KNpcRes nam trong KNpc: them truong la lech bo cuc",
         "// struct qua ranh gioi CoreClient.dll <-> Game.exe). MAX_NPC phia client = 256 nen mang nay chi 1 KB.",
         "static DWORD s_dwNhipHaBong[MAX_NPC] = { 0 };",
         "static BOOL HaBongMoNhipNay(int nNpcIdx)",
         "{",
         T + "if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)",
         T * 2 + "return TRUE;",
         T + "if (s_dwNhipHaBong[nNpcIdx] == SubWorld[0].m_dwCurrentTime)",
         T * 2 + "return FALSE;",
         T + "s_dwNhipHaBong[nNpcIdx] = SubWorld[0].m_dwCurrentTime;",
         T + "return TRUE;",
         "}",
         ""])
    f.doi_dong(
        "if (m_nBlurState == TRUE && m_cNpcBlur.NowGetBlur())",
        T + "if (m_nBlurState == TRUE && m_cNpcBlur.NowGetBlur() && HaBongMoNhipNay(nNpcIdx))" + T + "// " + MK + " A-2")
    f.ghi()


# =========================== (B) KICH NO ===========================

def va_giaothuc():
    """Them goi s2c_detonate: enum + cau truc + bang co + handler."""
    # --- enum ---
    f = Tep(HDR + "/KProtocolDef.h")
    if not any(MK in x for x in f.lines):
        f.chen_sau("s2c_syncvhtd,",
                   [T + "s2c_detonate," + T * 3 + "// " + MK + " may chu bao client kich no dan (S2C_DETONATE)"])
        f.ghi()
    else:
        print("  [=] KProtocolDef.h da co " + MK)

    # --- cau truc (tep CO BOM: chi chen ASCII, giu nguyen BOM) ---
    f = Tep(SRC + "/KProtocol.h")
    if not any(MK in x for x in f.lines):
        f.chen_sau("} S2C_SYNC_VHTD;",
                   ["",
                    "// " + MK + " may chu bao client kich no dan quanh nguoi phat. Tep dang o #pragma pack(1) -> 10 byte, khong dem.",
                    "typedef struct",
                    "{",
                    T + "BYTE" + T + "ProtocolType;" + T * 2 + "// s2c_detonate",
                    T + "DWORD" + T + "dwLauncherId;" + T * 2 + "// m_dwID cua NPC kich no",
                    T + "WORD" + T + "wStyle;" + T * 3 + "// ChildSkillId cua dan bi kich (419)",
                    T + "WORD" + T + "wRadius;" + T * 3 + "// ban kinh mps",
                    T + "BYTE" + T + "btFlag;" + T * 3 + "// 0 = dan phe ta, 1 = dan dich",
                    "} S2C_DETONATE;"])
        f.ghi()
    else:
        print("  [=] KProtocol.h da co " + MK)

    # --- bang co (PHAI cung thu tu voi enum) ---
    f = Tep(SRC + "/KProtocol.cpp")
    if not any(MK in x for x in f.lines):
        f.chen_sau("sizeof(S2C_SYNC_VHTD),",
                   [T + "sizeof(S2C_DETONATE)," + T * 3 + "// s2c_detonate " + MK])
        f.ghi()
    else:
        print("  [=] KProtocol.cpp da co " + MK)

    # --- khai bao handler ---
    f = Tep(SRC + "/KProtocolProcess.h")
    if not any(MK in x for x in f.lines):
        f.chen_sau("void\ts2cSyncVhtd(BYTE* pMsg);",
                   [T + "void" + T + "s2cDetonate(BYTE* pMsg);" + T + "// " + MK])
        f.ghi()
    else:
        print("  [=] KProtocolProcess.h da co " + MK)

    # --- dang ky + than handler ---
    f = Tep(SRC + "/KProtocolProcess.cpp")
    if not any(MK in x for x in f.lines):
        f.chen_sau("ProcessFunc[s2c_syncvhtd] = &KProtocolProcess::s2cSyncVhtd;",
                   [T + "ProcessFunc[s2c_detonate] = &KProtocolProcess::s2cDetonate;" + T + "// " + MK])
        # than ham: dat TRONG khoi #ifndef _SERVER san co, ngay truoc #endif dong khoi
        i = f.tim("Npc[nNpc].m_nHSShieldMax = pSync->nV2;")
        j = i
        while j < len(f.lines) and f.lines[j].strip() != "#endif":
            j += 1
        if j >= len(f.lines):
            raise SystemExit("  [X] KProtocolProcess.cpp: khong thay #endif dong khoi client")
        f.lines[j:j] = [
            "",
            "// " + MK + " Client nhan lenh kich no. JX1 khong dong bo tung vien dan nen ban sao tren may nguoi choi",
            "// van song den het LifeTime (360 khung ~ 20 giay) sau khi may chu da no -> khi truong khong tan, khong",
            "// thay hieu ung. Goi DetonateMissles phia client de no tu chay DoVanish -> Vanish() sinh dan 420 tai cho",
            "// (= hieu ung no) va don khi truong. Khong sinh sat thuong: KMissle::ProcessDamage la server-only.",
            "void KProtocolProcess::s2cDetonate(BYTE* pMsg)",
            "{",
            T + "S2C_DETONATE* pD = (S2C_DETONATE*)pMsg;",
            T + "if (pD->ProtocolType != s2c_detonate)",
            T * 2 + "return;",
            T + "int nIdx = NpcSet.SearchID(pD->dwLauncherId);",
            T + "if (nIdx <= 0 || nIdx >= MAX_NPC)",
            T * 2 + "return;",
            T + "if (Npc[nIdx].m_Index <= 0 || Npc[nIdx].m_RegionIndex < 0)",
            T * 2 + "return;",
            T + "Npc[nIdx].DetonateMissles((int)pD->wStyle, (int)pD->wRadius, (int)pD->btFlag);",
            "}"]
        f.doi += 1
        f.ghi()
    else:
        print("  [=] KProtocolProcess.cpp da co " + MK)


def va_knpc():
    """KNpc.cpp: mo khoa DetonateMissles cho ca hai phia + phat goi khi no that."""
    f = Tep(SRC + "/KNpc.cpp")
    if any(MK in x for x in f.lines):
        print("  [=] KNpc.cpp da co " + MK)
        return

    # (1) bo #ifdef _SERVER ngay truoc chu thich HOASON cua DetonateMissles
    i = f.tim("// [HOASON 01/09b] Linux 0x08079870")
    if f.lines[i - 1].strip() != "#ifdef _SERVER":
        raise SystemExit("  [X] KNpc.cpp: dong truoc chu thich DetonateMissles khong phai #ifdef _SERVER (%r)"
                         % f.lines[i - 1])
    f.lines[i - 1] = ("// " + MK + " BO #ifdef _SERVER: client CUNG can DetonateMissles de tu don khi truong khi nhan"
                      + " goi s2c_detonate.")
    f.lines[i:i] = ["// Moi thu ham nay dung deu ton tai phia client, moi chan bien con nguyen, pNext lay truoc khi"
                    + " Detonate nen an toan voi viec go nut."]
    f.doi += 1

    # (2) bo #endif dong ham (ngay sau `}` theo sau `return nDem;`)
    i = f.tim("return nDem;")
    if f.lines[i + 1].strip() != "}" or f.lines[i + 2].strip() != "#endif":
        raise SystemExit("  [X] KNpc.cpp: khong thay '}' + '#endif' sau 'return nDem;' (%r / %r)"
                         % (f.lines[i + 1], f.lines[i + 2]))
    f.xoa_dong(i + 2)

    # (3) sua chu thich noi NGUOC ve m_nTotalRegion
    f.doi_dong(
        "// MAX_REGION chi co o client; server dung m_nTotalRegion",
        T * 2 + T + "if (r >= 0 && r < pSW->m_nTotalRegion)" + T
        + "// " + MK + " m_nTotalRegion co CA HAI phia (client = 3*3 = 9, bang MAX_REGION);"
        + " TUYET DOI khong doi sang MAX_REGION vi hang do CHI dinh nghia phia client.")

    # (4) phat goi sau khi biet co no that
    i = f.tim("Npc[nAttacker].DetonateMissles(pMA->nValue[0] >> 8, pMA->nValue[2], pMA->nValue[0] & 0xff);")
    f.lines[i:i + 1] = [
        T * 2 + "{",
        T * 3 + "int nStyleDt = pMA->nValue[0] >> 8;",
        T * 3 + "int nFlagDt = pMA->nValue[0] & 0xff;",
        T * 3 + "int nRadiusDt = pMA->nValue[2];",
        T * 3 + "// " + MK + " no o may chu chi gui GWM_MISSLE_DEL vao hang doi NOI BO -> man hinh khong hay biet.",
        T * 3 + "// Bao cho client de no tu don khi truong va sinh dan hieu ung no ngay, thay vi cho het 20 giay.",
        T * 3 + "if (Npc[nAttacker].DetonateMissles(nStyleDt, nRadiusDt, nFlagDt) > 0)",
        T * 4 + "Npc[nAttacker].HS_BroadcastDetonate(nStyleDt, nRadiusDt, nFlagDt);",
        T * 2 + "}"]
    f.doi += 1

    # (5) dinh nghia HS_BroadcastDetonate, dat NGAY SAU CastAutoSkillAt (van trong #ifdef _SERVER cua no)
    i = f.tim("void KNpc::CastAutoSkillAt(int nSkillId, int nSkillLevel, int nTarget)")
    j = i
    while j < len(f.lines) and f.lines[j] != "}":
        j += 1
    if j >= len(f.lines):
        raise SystemExit("  [X] KNpc.cpp: khong thay dau dong ket CastAutoSkillAt")
    f.lines[j + 1:j + 1] = [
        "",
        "// " + MK + " Phat lenh kich no cho moi client trong tam nhin. Chi may chu (KRegion::BroadCast la server-only).",
        "// Dung dung mau quang ba cua CastAutoSkillAt ngay tren.",
        "void KNpc::HS_BroadcastDetonate(int nStyle, int nRadius, int nFlag)",
        "{",
        T + "if (!m_Index || m_SubWorldIndex < 0 || m_RegionIndex < 0)",
        T * 2 + "return;",
        T + "S2C_DETONATE sD;",
        T + "sD.ProtocolType = s2c_detonate;",
        T + "sD.dwLauncherId = m_dwID;",
        T + "sD.wStyle = (WORD)nStyle;",
        T + "sD.wRadius = (WORD)(nRadius > 0 ? nRadius : 0);",
        T + "sD.btFlag = (BYTE)nFlag;",
        T + "static const POINT POff[8] = { {0, 32}, {-16, 32}, {-16, 0}, {-16, -32}, {0, -32}, {16, -32}, {16, 0}, {16, 32} };",
        T + "int nMaxCount = MAX_BROADCAST_COUNT;",
        T + "CURREGION.BroadCast(&sD, sizeof(sD), nMaxCount, m_MapX, m_MapY);",
        T + "for (int i = 0; i < 8; i++)",
        T + "{",
        T * 2 + "if (CONREGIONIDX(i) == -1)",
        T * 3 + "continue;",
        T * 2 + "CONREGION(i).BroadCast(&sD, sizeof(sD), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);",
        T + "}",
        "}"]
    f.doi += 1
    f.ghi()

    # (6) khai bao trong .h - KHONG guard, theo dung mau HS_ResetBuffTime
    h = Tep(SRC + "/KNpc.h")
    if any(MK in x for x in h.lines):
        print("  [=] KNpc.h da co " + MK)
        return
    h.chen_sau("int\t\t\t\t\tDetonateMissles(int nStyle, int nRadius, int nFlag);",
               [T + "void" + T * 4 + "HS_BroadcastDetonate(int nStyle, int nRadius, int nFlag);"
                + T + "// " + MK + " chi dinh nghia phia may chu"])
    h.ghi()


if __name__ == "__main__":
    print("vhtd_engine_patch12 %s%s" % (MK, " (KIEM)" if KIEM else ""))
    print(" (A) bong mo Huyen Nhan Van Yen")
    va_bongmo_a1()
    va_bongmo_a2()
    print(" (B) kich no Ma Van Kiem Khi - goi s2c_detonate")
    va_giaothuc()
    va_knpc()
    print("XONG. Phai build CA HAI: CoreServer (x64) + CoreClient (Win32), swap 2 .moi CUNG LUC.")
