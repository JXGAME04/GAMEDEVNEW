# -*- coding: ascii -*-
# [TKINFO 05/09] (1) sinh UiTongKimInfo.ini (TCVN3) live + guong; (2) va Lua lib_tktc.lua (TK_GuiThongTin*), tongratrai/kimratrai (live + guong).
import io, os, sys
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes as V

BS = chr(92)
T = "\t"
LIVE = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
MIR = r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\tongkim_chat"

def vn(s):  # unicode -> chuoi latin-1 mang byte TCVN3
    return V(s).decode("latin-1")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    os.makedirs(os.path.dirname(p), exist_ok=True)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) >= 128)
def crlf(x): return x.replace("\r\n", "\n").replace("\n", "\r\n")
def rep(s, old, new, n=1):
    if s.count(old) != n and s.count(crlf(old)) == n:
        old, new = crlf(old), crlf(new)
    assert s.count(old) == n, ("anchor", old[:70], s.count(old))
    return s.replace(old, new)

# ------------------------------------------------------------ INI
def text(sec, l, t, w, h, font, color, txt, halign=0):
    out = ["[%s]" % sec, "Left=%d" % l, "Top=%d" % t, "Width=%d" % w, "Height=%d" % h, "Font=%d" % font, "HAlign=%d" % halign, "VAlign=1", "Color=%s" % color, "BorderColor=0,0,0"]
    if txt is not None:
        out.append("Text=" + txt)
    return "\n".join(out) + "\n\n"
YEL = "255,255,0"; WHITE = "255,255,255"; GREEN = "0,255,0"; HEAD = "255,255,153"; GOLD = "230,200,130"; LINK = "100,170,255"
ini = ";[TKINFO 05/09] Cua so THONG TIN TRAN Tong Kim kieu 'Lien Dau Bang' 2.0 (KUiTongKimInfo): giai doan, giay con lai (client dem lui),\n"
ini += ";top 5 tich luy Hang|Ten|Phe|Diem (m_MissionLadder may chu), nut 'Nhan xem Chien Bao' mo KUiBattleReport. Nen tu ve thongtin20.spr.\n"
ini += ";May chu gui kind 7 (dau) + 8 (hang) qua UpdateBattleInfo/UpdateBattleInfoAll; kind 9 an. Toa do trong cua so 221x268.\n"
for sec, l, t in [("Main", 5, 170), ("Main1024", 5, 215)]:
    ini += "[%s]\nLeft=%d\nTop=%d\nWidth=221\nHeight=268\nMoveable=0\nImage=%s\n\n" % (sec, l, t, BS + "Spr" + BS + "Ui3" + BS + "UiGameMain" + BS + "UiTongKim" + BS + "thongtin20.spr")
ini += text("Title", 0, 4, 221, 20, 14, GOLD, vn("T" + chr(0x1ED1) + "ng Kim"), 1)
ini += text("StageLabel", 10, 33, 75, 18, 12, YEL, vn("Giai " + chr(0x111) + "o" + chr(0x1EA1) + "n:"))
ini += text("Stage", 88, 33, 125, 18, 12, WHITE, None)
ini += text("CountLabel", 10, 54, 75, 18, 12, YEL, vn("C" + chr(0xF2) + "n:"))
ini += text("Count", 88, 54, 60, 18, 12, GREEN, None)
ini += text("CountSuffix", 150, 54, 60, 18, 12, WHITE, vn("gi" + chr(0xE2) + "y"))
ini += text("HdRank", 8, 96, 36, 18, 12, HEAD, vn("H" + chr(0x1EA1) + "ng"), 1)
ini += text("HdName", 46, 96, 80, 18, 12, HEAD, vn("T" + chr(0xEA) + "n"), 1)
ini += text("HdCamp", 128, 96, 40, 18, 12, HEAD, "Phe", 1)
ini += text("HdPoint", 166, 96, 50, 18, 12, HEAD, vn(chr(0x110) + "i" + chr(0x1EC3) + "m"), 1)
for i in range(5):
    y = 118 + i * 21
    ini += text("Rank_%d" % i, 8, y, 36, 18, 12, WHITE, None, 1)
    ini += text("Name_%d" % i, 46, y, 80, 18, 12, WHITE, None, 1)
    ini += text("Camp_%d" % i, 128, y, 40, 18, 12, WHITE, None, 1)
    ini += text("Point_%d" % i, 166, y, 50, 18, 12, WHITE, None, 1)
ini += "[BtnReport]\nLeft=20\nTop=234\nWidth=181\nHeight=20\nFont=12\nColor=%s\nOverColor=255,255,255\nSelectColor=255,255,0\nLabel=%s\nLabelXOffset=30\nLabelYOffset=3\n\n" % (LINK, vn("Nh" + chr(0x1EA5) + "n xem Chi" + chr(0x1EBF) + "n B" + chr(0xE1) + "o"))
ini += "[Phase]\nP0=?\nP1=%s\nP2=%s\nP3=%s\n\n" % (vn("B" + chr(0xE1) + "o danh"), vn("Chi" + chr(0x1EBF) + "n " + chr(0x111) + chr(0x1EA5) + "u"), vn("K" + chr(0x1EBF) + "t th" + chr(0xFA) + "c"))
ini += "[Camp]\nC0=-\nC1=%s\nC2=Kim\nC3=-\n" % vn("T" + chr(0x1ED1) + "ng")
ini = ini.replace("\n", "\r\n")
for p in [os.path.join(LIVE, "client", "Ui", "Ui3", "UiTongKimInfo.ini"), os.path.join(MIR, "client", "Ui", "Ui3", "UiTongKimInfo.ini")]:
    wr(p, ini); print("ini", p, hi(ini), "byte cao")

# ------------------------------------------------------------ LUA lib_tktc.lua
block = "\n".join([
    "",
    "-- ================================================================ [TKINFO 05/09] CUA SO 'THONG TIN TRAN' kieu Lien Dau Bang 2.0",
    "-- Client KUiTongKimInfo: giai doan, giay con lai (client tu dem lui), top 5 tich luy (hang/ten/phe/diem), nut xem Chien Bao.",
    "-- Kenh S2C_BATTLE_BOX kind 7 (dau) + 8 (hang), kind 9 (an) dung chung voi bang diem. C++: UpdateBattleInfo / UpdateBattleInfoAll",
    "-- (ScriptFuns.cpp) doc m_MissionLadder (top theo tham so ladder = 6 tich luy, xep san trong C++) + timer con lai.",
    "-- Cu phap trung tinh Lua 4 / 5.4. CoreServer cu chua co ham -> bo qua, khong loi.",
    "TKINFO_KHUNG_GIUA_2LAN = 54" + T + "-- 3 giay giua hai lan phat ca tran",
    "TKINFO_KHUNG_GUI_CUOI  = 0",
    "TKINFO_SO_HANG         = 5",
    "",
    "-- 1 = bao danh (timer 1 con chay), 2 = chien dau",
    "function TK_GiaiDoan()",
    T + "local nBD = GetMSRestTime(MS_TONGKIM, 1)",
    T + "if nBD ~= nil and nBD > 0 then",
    T*2 + "return 1",
    T + "end",
    T + "return 2",
    "end",
    "",
    "-- timer dem lui hien tren cua so: bao danh -> timer 1, chien dau -> timer 3 (ca tran)",
    "function TK_TimerHien(nPha)",
    T + "if nPha == 1 then",
    T*2 + "return 1",
    T + "end",
    T + "return 3",
    "end",
    "",
    "-- gui cho CHINH PlayerIndex hien tai (ra khoi hau doanh / vao tran)",
    "function TK_GuiThongTinChoToi()",
    T + "if UpdateBattleInfo == nil then",
    T*2 + "return 0",
    T + "end",
    T + "local nPha = TK_GiaiDoan()",
    T + "return UpdateBattleInfo(MS_TONGKIM, nPha, TK_TimerHien(nPha), GetMissionV(M_TICHLUYA), GetMissionV(M_TICHLUYB), TKINFO_SO_HANG)",
    "end",
    "",
    "-- gui ca tran, tu tiet che TKINFO_KHUNG_GIUA_2LAN (goi tu TK_GuiDiemPhe moi lan giet)",
    "function TK_GuiThongTinPhe()",
    T + "if UpdateBattleInfoAll == nil then",
    T*2 + "return 0",
    T + "end",
    T + "local nKhung = GetGameTime()",
    T + "if nKhung - TKINFO_KHUNG_GUI_CUOI < TKINFO_KHUNG_GIUA_2LAN then",
    T*2 + "return 0",
    T + "end",
    T + "TKINFO_KHUNG_GUI_CUOI = nKhung",
    T + "local nPha = TK_GiaiDoan()",
    T + "return UpdateBattleInfoAll(MS_TONGKIM, nPha, TK_TimerHien(nPha), GetMissionV(M_TICHLUYA), GetMissionV(M_TICHLUYB), TKINFO_SO_HANG)",
    "end",
    ""])
for p in [os.path.join(LIVE, "server", "script", "tinhnang", "tong_kim_tcap", "lib_tktc.lua"), os.path.join(MIR, "server", "script", "tinhnang", "tong_kim_tcap", "lib_tktc.lua")]:
    s = rd(p); h0 = hi(s)
    if "[TKINFO 05/09]" not in s:
        # hook: dau ham TK_GuiDiemPhe (truoc tiet che diem)
        s = rep(s, "function TK_GuiDiemPhe(nKind)\n",
                   "function TK_GuiDiemPhe(nKind)\n" + T + "if nKind ~= TKDIEM_KIND_AN then\n" + T*2 + "TK_GuiThongTinPhe()" + T + "-- [TKINFO 05/09] cua so thong tin tran (tu tiet che 3 s)\n" + T + "end\n")
        s = s.rstrip("\r\n") + ("\r\n" if "\r\n" in s else "\n") + (crlf(block) if "\r\n" in s else block)
        assert hi(s) == h0
        wr(p, s); print("lib_tktc ok", p)
    else:
        print("lib_tktc da co TKINFO", p)

# ------------------------------------------------------------ LUA tongratrai / kimratrai
for name in ["tongratrai.lua", "kimratrai.lua"]:
    for base in [os.path.join(LIVE, "server", "script"), os.path.join(MIR, "server", "script")]:
        p = os.path.join(base, "maps", "tongkim", "trap", name)
        if not os.path.exists(p):
            print("khong co", p); continue
        s = rd(p); h0 = hi(s)
        if "[TKINFO 05/09]" in s:
            print("da co", p); continue
        old = T + "TK_GuiDiemChoToi(TKDIEM_KIND_CAPNHAT)" + T + "-- [TKDIEM 04/09] vua ra khoi hau doanh: hien bang diem Tong VS Kim\n"
        new = old + T + "TK_GuiThongTinChoToi()" + T + "-- [TKINFO 05/09] cua so thong tin tran (giai doan, giay con lai, top 5)\n"
        s = rep(s, old, new)
        assert hi(s) == h0
        wr(p, s); print("trap ok", p)
print("XONG")
