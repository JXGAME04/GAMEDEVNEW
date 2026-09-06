# -*- coding: ascii -*-
# [TKINFO 06/09b] Sinh lai UiTongKimInfo.ini + dat lai [CloseBtn] cho UiBattleBig.ini / UiBattleSmall.ini.
# Da sua theo 3 goc tham dinh:
#  - [BtnFold] BO Trans=1 (switch.spr la khung RONG RUOT: 47% diem anh trong suot -> voi Trans=1 thi
#    KWndImage::PtInWindow do do duc TUNG DIEM ANH, bam vao giua nut khong an). [SwitchBtn] cua bang chien bao
#    dung chinh anh nay va cung khong dat Trans.
#  - [BtnFold] doi sang goc TRAI dai tieu de: than bang Chien Bao (600x350 tai x=100) che TRON goc phai,
#    nut se khong bam duoc khi bang do dang mo. Ben trai (abs x 11..26) luon tu do.
#  - [CloseBtn] dat vao khe TRONG THAT: che do To x 527..563 (giua hai cum chu ve san tren main.spr),
#    che do Nho chi con khe x 33..47 giua [SwitchBtn] va [NumPlayerT] -> dung nhan mot ky tu "X".
# Tep TCVN3: sinh chuoi Viet bang vn_to_octal, ghi latin-1. KHONG dung Edit/Write tool.
import io, os, re, sys
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes as V

BS = chr(92)
LIVE = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIR = r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\tongkim_chat\client"

def vn(s): return V(s).decode("latin-1")
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    if os.path.dirname(p): os.makedirs(os.path.dirname(p), exist_ok=True)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) >= 128)

TIEUDE   = vn("T" + chr(0x1ED1) + "ng Kim")
GIAIDOAN = vn("Giai " + chr(0x111) + "o" + chr(0x1EA1) + "n:")
CON      = vn("C" + chr(0xF2) + "n:")
GIAY     = vn("gi" + chr(0xE2) + "y")
HANG     = vn("H" + chr(0x1EA1) + "ng")
TEN      = vn("T" + chr(0xEA) + "n")
DIEM     = vn(chr(0x110) + "i" + chr(0x1EC3) + "m")
NUTBAO   = vn("Nh" + chr(0x1EA5) + "n xem Chi" + chr(0x1EBF) + "n B" + chr(0xE1) + "o")
P_BAODANH= vn("B" + chr(0xE1) + "o danh")
P_CHIEN  = vn("Chi" + chr(0x1EBF) + "n " + chr(0x111) + chr(0x1EA5) + "u")
P_KET    = vn("K" + chr(0x1EBF) + "t th" + chr(0xFA) + "c")
TONG     = vn("T" + chr(0x1ED1) + "ng")
DONG     = vn(chr(0x110) + chr(0xF3) + "ng")
THUGON   = vn("Thu g" + chr(0x1ECD) + "n / M" + chr(0x1EDF) + " r" + chr(0x1ED9) + "ng")

YEL, WHITE, GREEN, HEAD, GOLD, LINK = "255,255,0", "255,255,255", "0,255,0", "255,255,153", "230,200,130", "100,170,255"
COT = {"Rank": (8, 26), "Name": (36, 94), "Camp": (132, 34), "Point": (168, 45)}
SPR_NEN = BS + "Spr" + BS + "Ui3" + BS + "UiGameMain" + BS + "UiTongKim" + BS
SPR_SW  = BS + "Spr" + BS + "Ui3" + BS + "UiGameMain" + BS + "UiBattleReport" + BS + "switch.spr"

def text(sec, l, t, w, h, font, color, txt, halign=1, valign=1):
    out = ["[%s]" % sec, "Left=%d" % l, "Top=%d" % t, "Width=%d" % w, "Height=%d" % h,
           "Font=%d" % font, "HAlign=%d" % halign, "VAlign=%d" % valign, "Color=%s" % color, "BorderColor=0,0,0"]
    if txt is not None:
        out.append("Text=" + txt)
    return "\n".join(out) + "\n\n"

# ---------------------------------------------------------------- UiTongKimInfo.ini
ini = (";[TKINFO 06/09b] Cua so THONG TIN TRAN Tong Kim kieu 'Lien Dau Bang' 2.0 (lop KUiTongKimInfo).\n"
       ";Nhan tinh o day CHI hien khi ben C++ khai bang KWndText32/80 (KWndText tran khong co bo dem -> im lang).\n"
       ";[BtnFold] = nut thu gon: dat goc TRAI dai tieu de (goc phai bi than bang Chien Bao che) va KHONG dat Trans=1\n"
       ";(switch.spr rong ruot -> Trans=1 lam bam vao giua nut khong an).\n"
       ";Cot: Hang 8/26, Ten 36/94, Phe 132/34, Diem 168/45 - vua trong khung ke nen x=8..213, khong chong nhau.\n")
for sec, l, t in [("Main", 5, 170), ("Main1024", 5, 215)]:
    ini += ("[%s]\nLeft=%d\nTop=%d\nWidth=221\nHeight=268\nMoveable=0\n"
            "Image=%sthongtin20.spr\nImageFold=%sthongtin20_thu.spr\nHeightFold=27\n\n" % (sec, l, t, SPR_NEN, SPR_NEN))
ini += text("Title", 0, 4, 221, 20, 14, GOLD, TIEUDE)
ini += text("StageLabel", 10, 33, 62, 18, 12, YEL, GIAIDOAN, halign=0)
ini += text("Stage", 74, 33, 139, 18, 12, WHITE, None, halign=0)
ini += text("CountLabel", 10, 54, 34, 18, 12, YEL, CON, halign=0)
ini += text("Count", 44, 54, 44, 18, 12, GREEN, None, halign=2)
ini += text("CountSuffix", 92, 54, 40, 18, 12, WHITE, GIAY, halign=0)
for sec, txt in [("HdRank", HANG), ("HdName", TEN), ("HdCamp", "Phe"), ("HdPoint", DIEM)]:
    l, w = COT[sec[2:]]
    ini += text(sec, l, 96, w, 18, 12, HEAD, txt)
for i in range(5):
    y = 118 + i * 21
    for ten_cot in ("Rank", "Name", "Camp", "Point"):
        l, w = COT[ten_cot]
        ini += text("%s_%d" % (ten_cot, i), l, y, w, 18, 12, WHITE, None)
ini += ("[BtnReport]\nLeft=20\nTop=234\nWidth=181\nHeight=20\nFont=12\n"
        "Color=%s\nOverColor=255,255,255\nSelectColor=255,255,0\nBorderColor=0,0,0\n"
        "Label=%s\nLabelXOffset=0\nLabelYOffset=5\n\n" % (LINK, NUTBAO))
ini += ("[BtnFold]\nLeft=6\nTop=8\nWidth=15\nHeight=11\nUp=0\nDown=1\nOver=1\nOverFrame=2\n"
        "Image=%s\nTip=%s\n\n" % (SPR_SW, THUGON))
ini += "[Phase]\nP0=?\nP1=%s\nP2=%s\nP3=%s\n\n" % (P_BAODANH, P_CHIEN, P_KET)
ini += "[Camp]\nC0=-\nC1=%s\nC2=Kim\nC3=-\n" % TONG
ini = ini.replace("\n", "\r\n")
for p in [os.path.join(LIVE, "Ui", "Ui3", "UiTongKimInfo.ini"), os.path.join(MIR, "Ui", "Ui3", "UiTongKimInfo.ini")]:
    wr(p, ini); print("ghi %s (%d byte cao)" % (p, hi(ini)))

# ---------------------------------------------------------------- [CloseBtn] cho bang chien bao
KHOI = {
    "UiBattleBig.ini":   ("Left=527\nTop=5\nWidth=36\nHeight=18\n", DONG, 2),
    "UiBattleSmall.ini": ("Left=33\nTop=6\nWidth=14\nHeight=16\n", "X", 1),
}
for ten, (toado, nhan, yoff) in KHOI.items():
    khoi = (";[TKINFO 06/09] nut Dong cua rieng bang chien bao - truoc day cua so nay khong co duong dong nao\n"
            ";([SwitchBtn] chi doi che do To/Nho), nguoi choi mo ra roi khong tat duoc.\n"
            "[CloseBtn]\n" + toado +
            "Font=12\nColor=255,220,120\nOverColor=255,255,255\nSelectColor=255,255,0\nBorderColor=0,0,0\n"
            "Label=%s\nLabelXOffset=0\nLabelYOffset=%d\n" % (nhan, yoff))
    khoi = khoi.replace("\n", "\r\n")
    for goc in (LIVE, MIR):
        p = os.path.join(goc, "Ui", "Ui3", ten)
        if not os.path.exists(p):
            print("BO QUA (khong co):", p); continue
        s = rd(p)
        if "[CloseBtn]" in s:
            # go khoi [CloseBtn] cu (ke ca chu thich TKINFO ngay truoc no)
            s = re.sub(r"(?:;\[TKINFO[^\r\n]*\r?\n)?(?:;\([^\r\n]*\r?\n)?\[CloseBtn\](?:\r?\n(?!\[)[^\r\n]*)*\r?\n?", "", s)
            print("  go [CloseBtn] cu:", os.path.basename(p))
        while s.endswith("\r\n\r\n"):
            s = s[:-2]
        if not s.endswith("\r\n"):
            s += "\r\n"
        h0 = hi(s)
        s = s + "\r\n" + khoi
        assert hi(s) == h0 + hi(khoi)
        wr(p, s); print("  dat [CloseBtn] (%s) vao %s" % (nhan if nhan == "X" else "Dong", p))
print("XONG")
