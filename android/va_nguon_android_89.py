# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 TTMT e] Bon y cua chu:
#   1. "bo cai di chuot vao nguoi khac la hien thong tin di - chi de kich vao moi hien thong tin"
#   2. "cai ong mau phai fix cho giong ban goc chu khong duoc ghep bay"
#   3. "kich vao chua di theo"
#   4. "chi fix ban mobile thoi khong duoc fix ban pc"
#
# LAM:
# 1+3. Tren Android, thanh thong tin CHI theo muc tieu DA CHAM: bo han hover (idx_hover) va m_nPeopleIdx khoi duong
#      chon. Khong cham ai -> khong hien gi. Khoa da theo MA nguoi choi (va 88) nen bam duoc ca khi nguoi do di lai.
# 2.   GO phan "ghep" cua va 87: bo o con m_Life (muon anh thanh mau CUA NGUOI CHOI trong uiheadercontrolbar.ini) va
#      bo cac muc [Life]* da them vao kuitargetinfo.ini. Tra ve dung cach ve GOC: Core (KNpc::PaintTargetInfo) ve hai
#      hinh chu nhat do + xam nhu ban PC, chi khac la lay VI TRI va TI LE cua cua so (va 84/85) de khong lech.
#      Them phan tram mau vao nhat ky [TTMT] de biet mau do mat vi ve sai cho hay vi so phan tram = 0.
# 4.   Moi thu deu trong #ifdef JX_ANDROID; nhanh #else cua KNpc::PaintTargetInfo giu nguyen cho ban PC.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 TTMT e]"
LIVE = "D:/jx1_android_data"


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


# ---------------------------------------------------------------- 2. go o con m_Life (bo "ghep")
def va_h(s):
    return thay(s, ["#ifdef JX_ANDROID",
                    "\tKWndImageTextButton\tm_Life;\t\t// [ANDROID 11/09 ONGMAU] ong mau la O CON that (nhu UiNpcBar cua USVOLAM) -> tu di theo cua so, khong lech",
                    "#endif"],
                   ["\t// %s ong mau ve theo dung cach GOC (Core: KNpc::PaintTargetInfo), khong muon anh cua cua so khac" % DAU],
                   "UiTargetInfo.h: bo m_Life")


def va_cpp(s):
    s = thay(s, ["#ifdef JX_ANDROID",
                 "\tm_Life.Hide();\t// [ANDROID 11/09 ONGMAU]",
                 "#endif"],
                ["\t// %s (bo o con m_Life - ong mau ve theo cach goc trong Core)" % DAU], "UpdateData: bo Hide m_Life")
    s = thay(s, ["#ifdef JX_ANDROID",
                 "\t\t\t// [ANDROID 11/09 ONGMAU] ong mau: o con tu ve anh theo phan tram mau (nhu UiNpcBar cua USVOLAM)",
                 "\t\t\tm_Life.Set2IntValue(m_Info.nLifePercent, 100);",
                 "\t\t\tm_Life.Show();",
                 "#endif"],
                ["\t\t\t// %s ong mau do Core ve (cach goc); day chi giu so phan tram cho no" % DAU], "UpdateData: bo Show m_Life")
    s = thay(s, ["#ifdef JX_ANDROID",
                 "\treturn;\t// [ANDROID 11/09 ONGMAU] ong mau da la o con m_Life; khong nho Core ve o toa do cung nua (ban PC giu nguyen)",
                 "#endif"],
                ["\t// %s van nho Core ve ong mau (cach goc), chi khac la theo vi tri + ti le cua cua so" % DAU],
                "PaintWindow: bo tra som")
    s = thay(s, ["#ifdef JX_ANDROID",
                 "\tAddChild(&m_Life);\t// [ANDROID 11/09 ONGMAU]",
                 "#endif"],
                ["\t// %s (khong con o con m_Life)" % DAU], "Initialize: bo AddChild")
    s = thay(s, ["\t\tm_pSelf->m_Life.Init(&Ini, \"Life\");\t// [ANDROID 11/09 ONGMAU] muc [Life] moi trong kuitargetinfo.ini",
                 "\t\tm_pSelf->m_Life.Hide();"],
                ["\t\t// %s (khong con o con m_Life)" % DAU], "LoadScheme: bo Init m_Life")
    s = thay(s, ["\t\tTt_ThuNhoO(&m_pSelf->m_Life, 0);\t// [ANDROID 11/09 ONGMAU]"],
             ["\t\t// %s (khong con o con m_Life)" % DAU], "LoadScheme: bo thu nho m_Life")
    return s


# ---------------------------------------------------------------- 1+3. bo hover, chi theo cai da cham
def va_coreshell(s):
    s = thay(s, ["\t\tint idx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx;",
                 "\t\tint idx_hover = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();",
                 "\t\tif (idx_hover)",
                 "\t\t\tidx = idx_hover;"],
                ["\t\tint idx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx;",
                 "\t\tint idx_hover = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();",
                 "#ifdef JX_ANDROID",
                 "\t\t// %s Chu: \"bo cai di chuot vao nguoi khac la hien thong tin - chi de kich vao moi hien\"." % DAU,
                 "\t\t// Tren dien thoai KHONG co re chuot that: SinhHover() bom WM_MOUSEMOVE moi khung tai cho ngon tay dat",
                 "\t\t// truoc, the gioi troi nen NPC duoi diem do doi lien tuc -> thanh thong tin nhay lung tung.",
                 "\t\t// Nen bo han hover VA muc tieu tuong tac; chi con muc tieu DA CHAM (khoa theo ma, dat o nParam == 3).",
                 "\t\tidx = 0;",
                 "#else",
                 "\t\tif (idx_hover)",
                 "\t\t\tidx = idx_hover;",
                 "#endif"], "CoreShell.cpp: bo hover tren Android")
    # them phan tram mau vao nhat ky
    s = thay(s, ["\t\t\t\t\tg_DebugLog(\"[TTMT] thong tin doi: nguoi=%d hover=%d khoa=%d -> chon %d\",",
                 "\t\t\t\t\t\tNpc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx, idx_hover, g_nJxMucTieuKhoa, idx);"],
                ["\t\t\t\t\tg_DebugLog(\"[TTMT] thong tin doi: nguoi=%d hover=%d khoa=%d -> chon %d | mau %d/%d\",",
                 "\t\t\t\t\t\tNpc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx, idx_hover, g_nJxMucTieuKhoa, idx,",
                 "\t\t\t\t\t\t(idx > 0 && idx < MAX_NPC) ? Npc[idx].m_CurrentLife : -1,",
                 "\t\t\t\t\t\t(idx > 0 && idx < MAX_NPC) ? Npc[idx].m_CurrentLifeMax : -1);\t// %s xem mau do mat vi ve sai cho hay vi phan tram = 0" % DAU],
                "CoreShell.cpp: nhat ky them mau")
    return s


# ---------------------------------------------------------------- ini: bo cac muc [Life]* da them
def bo_muc_life(p):
    if not os.path.isfile(p):
        return
    s = doc(p)
    if "[Life]" not in s:
        print("ini khong co [Life], bo qua:", p)
        return
    nl = nl_cua(s)
    dong = s.split(nl)
    ra = []
    bo = False
    for d in dong:
        t = d.strip()
        if t.startswith("[") and t.endswith("]"):
            bo = t in ("[Life]", "[Life_Image]", "[Life_Text]")
        if t.startswith("; [ANDROID 11/09 ONGMAU]") or t.startswith("; nen tu di theo cua so") \
                or t.startswith("; Toa do goc (chua thu nho)") or t.startswith("; Co thuc te ="):
            continue
        if not bo:
            ra.append(d)
    ghi(p, nl.join(ra).rstrip("\r\n") + nl)
    print("da bo [Life]* khoi:", p)


va("Sources/S3Client/Ui/UiCase/UiTargetInfo.h", va_h)
va("Sources/S3Client/Ui/UiCase/UiTargetInfo.cpp", va_cpp)
va("Sources/Core/Src/CoreShell.cpp", va_coreshell)
bo_muc_life(LIVE + "/ui/Ui3/kuitargetinfo.ini")
bo_muc_life("android/du_lieu_ghi_de/ui/ui3/kuitargetinfo.ini")
print("xong")
