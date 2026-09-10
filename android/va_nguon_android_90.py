# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 TTMT f] Nhat ky va 89 (doc that, khong doan):
#   [TTMT] cham (316,316) -> khoa muc tieu 8 ma 93374 (NguyenThang847)
#   [TTMT] thong tin doi: nguoi=0 hover=0 khoa=8 -> chon 8 | mau 5864/5864     <- CHAY, va MAU CO THAT (100%)
#   [TTMT] thong tin doi: nguoi=0 hover=0 khoa=8 -> chon 0 | mau -1/-1         <- roi mat
#   [TTMT] cham (648,271) -> khoa muc tieu 2 ma 91187 (Tong Binh Bao Danh)
#   [TTMT] thong tin doi: nguoi=0 hover=0 khoa=2 -> chon 0 | mau -1/-1         <- mat ngay, du NPC dung im
# => (a) so phan tram mau CO SAN (5864/5864) -> mau do mat la do VE SAI CHO, khong phai thieu du lieu;
#    (b) tim lai theo ma bi truot ngay ca voi NPC dung im -> duong tim cua rieng minh khong dang tin.
#
# SUA cho GIONG BAN GOC: ban PC giu muc tieu bang chinh o cua game - Npc[nguoi choi].m_nPeopleIdx (dong dau cua
# NPC_OI_TARGET_INFO da lay o nay). Tren Android cho do luon = 0 vi KPlayer::OnButtonDown khong chay. Nay luc cham
# thi DAT THANG o do (nhu ban PC lam khi bam chuot), roi de nguyen duong chon goc -> khong can duong tim rieng.
# Giu khoa theo ma lam duong du phong, va ghi ro ly do khi duong nhanh truot.
# Windows: khong doi (rao JX_ANDROID).

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 TTMT f]"


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


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


def va_coreshell(s):
    # 1. luc cham: dat thang o chon muc tieu CUA GAME (nhu ban PC bam chuot)
    s = thay(s, ["\t\t\t\tg_uJxMucTieuMatLuc = 0;",
                 "\t\t\t\tg_DebugLog(\"[TTMT] cham (%d,%d) -> khoa muc tieu %d ma %u (%s)\", pCham->nViTriVeX, pCham->nViTriVeY, nTrung, g_uJxMucTieuKhoaId, Npc[nTrung].Name);"],
                ["\t\t\t\tg_uJxMucTieuMatLuc = 0;",
                 "\t\t\t\t// %s dat thang o chon muc tieu CUA GAME - dong dau cua nhanh nParam==0 lay chinh o nay," % DAU,
                 "\t\t\t\t// giong het ban PC khi bam chuot vao ai. Tren Android o nay luon 0 vi OnButtonDown khong chay.",
                 "\t\t\t\tif (nMe > 0 && nMe < MAX_NPC)",
                 "\t\t\t\t\tNpc[nMe].m_nPeopleIdx = nTrung;",
                 "\t\t\t\tg_DebugLog(\"[TTMT] cham (%d,%d) -> chon muc tieu %d ma %u (%s)\", pCham->nViTriVeX, pCham->nViTriVeY, nTrung, g_uJxMucTieuKhoaId, Npc[nTrung].Name);"],
                "CoreShell.cpp: dat m_nPeopleIdx luc cham")
    # nMe phai co trong nhanh nParam == 3
    s = thay(s, ["\t\t\tKUiTargetDetailInfo* pCham = (KUiTargetDetailInfo*)uParam;",
                 "\t\t\textern int g_nJxMucTieuKhoa;",
                 "\t\t\tint nCu = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();",
                 "\t\t\tint nTrung = 0;"],
                ["\t\t\tKUiTargetDetailInfo* pCham = (KUiTargetDetailInfo*)uParam;",
                 "\t\t\textern int g_nJxMucTieuKhoa;",
                 "\t\t\tint nCu = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();",
                 "\t\t\tint nMe = Player[CLIENT_PLAYER_INDEX].m_nIndex;\t// %s" % DAU,
                 "\t\t\tint nTrung = 0;"], "CoreShell.cpp: nMe trong nParam 3")
    # 2. nhanh hoi: KHONG xoa idx nua - giu o chon cua game, chi bo hover
    s = thay(s, ["\t\t// %s Chu: \"bo cai di chuot vao nguoi khac la hien thong tin - chi de kich vao moi hien\"." % "[ANDROID 11/09 TTMT e]",
                 "\t\t// Tren dien thoai KHONG co re chuot that: SinhHover() bom WM_MOUSEMOVE moi khung tai cho ngon tay dat",
                 "\t\t// truoc, the gioi troi nen NPC duoi diem do doi lien tuc -> thanh thong tin nhay lung tung.",
                 "\t\t// Nen bo han hover VA muc tieu tuong tac; chi con muc tieu DA CHAM (khoa theo ma, dat o nParam == 3).",
                 "\t\tidx = 0;"],
                ["\t\t// [ANDROID 11/09 TTMT e] Chu: \"bo cai di chuot vao nguoi khac la hien thong tin - chi de kich vao moi hien\".",
                 "\t\t// Tren dien thoai KHONG co re chuot that: SinhHover() bom WM_MOUSEMOVE moi khung tai cho ngon tay dat",
                 "\t\t// truoc, the gioi troi nen NPC duoi diem do doi lien tuc -> thanh thong tin nhay lung tung.",
                 "\t\t// %s Chi BO HOVER; van giu idx = Npc[nguoi choi].m_nPeopleIdx (o chon cua game, nay duoc dat luc cham" % DAU,
                 "\t\t// o nhanh nParam == 3) - dung duong cua ban goc, khong dung duong tim rieng nua."], "CoreShell.cpp: giu o chon goc")
    # 3. ghi ly do khi duong nhanh truot
    s = thay(s, ["\t\t\t\telse",
                 "\t\t\t\t{\t// tam thoi khong thay (ra khoi tam nhin): giu them 3 s roi moi bo",
                 "\t\t\t\t\tunsigned int uNay = (unsigned int)GetTickCount();"],
                ["\t\t\t\telse",
                 "\t\t\t\t{\t// tam thoi khong thay (ra khoi tam nhin): giu them 3 s roi moi bo",
                 "\t\t\t\t\tunsigned int uNay = (unsigned int)GetTickCount();",
                 "",
                 "\t\t\t\t\tif (g_uJxMucTieuMatLuc == 0 && g_nJxMucTieuKhoa > 0 && g_nJxMucTieuKhoa < MAX_NPC)",
                 "\t\t\t\t\t\tg_DebugLog(\"[TTMT] truot duong nhanh: o %d ma %u (can %u) vung %d\", g_nJxMucTieuKhoa,\t// " + DAU,
                 "\t\t\t\t\t\t\tNpc[g_nJxMucTieuKhoa].m_dwID, g_uJxMucTieuKhoaId, Npc[g_nJxMucTieuKhoa].m_RegionIndex);"],
                "CoreShell.cpp: ly do truot")
    return s


va("Sources/Core/Src/CoreShell.cpp", va_coreshell)
print("xong")
