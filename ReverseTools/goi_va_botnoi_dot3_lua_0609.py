# -*- coding: utf-8 -*-
# [BOTNOI dot 3 06/09] Phan Lua cua dot 3 (di kem goi_va_botnoi_dot3_0609.py):
#   * hocvocong.lua      : them bot_hoc120(nCurFac) - "bot len 120 se co skill 120 full skill"
#   * simcity_admin.lua  : menu lenh bai "Nang kinh mach bot" -> PB_MachMenu / PB_MachBot(nCap)
#                          -> ham C PB_NangMach(nCap, 0)   ("viet them ham nang mach cho toan bo bot")
# Tham so 1 = goc cay script (thu muc chua global\, kiemthu\). Ap cho CA cay chay that
# (E:\...\bin\server\script) LAN guong git (serverscript_live\script). Idempotent.
# Chu Viet trong menu ma hoa TCVN3 bang vn() cua skill (vn_edit.py) - KHONG go byte tay.
import io, os, sys
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_botnoi\serverscript_live\script"

def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()

def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)

class Tep:
    def __init__(self, ten):
        self.p = os.path.join(ROOT, ten)
        self.s = doc(self.p)
        self.truoc = sum(1 for c in self.s if ord(c) > 127)
        self.crlf = "\r\n" in self.s
        self.n = 0
        self.them_hi = 0
    def ap(self, ten, cu, moi):
        if self.crlf:
            cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
        if moi in self.s:
            print("  [=] %s da ap tu truoc" % ten); return
        if self.s.count(cu) != 1:
            print("LOI: neo %s khop %d cho (can 1)" % (ten, self.s.count(cu))); sys.exit(1)
        self.s = self.s.replace(cu, moi); self.n += 1
        self.them_hi += sum(1 for c in moi if ord(c) > 127) - sum(1 for c in cu if ord(c) > 127)
        print("  [+] %s" % ten)
    def xong(self):
        if self.n:
            ghi(self.p, self.s)
        sau = sum(1 for c in self.s if ord(c) > 127)
        ok = (sau == self.truoc + self.them_hi)
        print("%s: %d hunk; high-byte %d -> %d (mong doi +%d) %s" % (os.path.basename(self.p), self.n,
              self.truoc, sau, self.them_hi, "OK" if ok else "LECH!"))
        if not ok:
            sys.exit(1)

# ---------------------------- hocvocong.lua ----------------------------
hv = Tep(os.path.join("global", "hocvocong.lua"))
hv.ap("L1 bot_hoc120 sau bot_hoc90",
 '\tfor i=1,getn(SKILL90_ARRAY[nCurFac]) do\n'
 '\t\tAddMagic(SKILL90_ARRAY[nCurFac][i][2],20)\n'
 '\tend\n'
 'end\n',
 '\tfor i=1,getn(SKILL90_ARRAY[nCurFac]) do\n'
 '\t\tAddMagic(SKILL90_ARRAY[nCurFac][i][2],20)\n'
 '\tend\n'
 'end\n'
 '\n'
 '-- [SKILL120 06/09] chu game: "bot len 120 se co skill 120 full skill". Ham RIENG cho bot\n'
 '-- (KPlayerBot.cpp pb_TrangBiTheoCap buoc 1c goi khi bot dat cap 120): hoc ky nang 120\n'
 '-- cua phai (SKILL120AR - mot chieu/phai) o cap 20 = max (skills.txt MaxLevel 20), dung\n'
 '-- dong AddMagic(SKILL120AR[nCurFac], 20) cua show_kynang90. KHONG kem 210 khinh cong /\n'
 '-- SKILL150_ARRAY. nCurFac 1-based (m_nCurFaction + 1).\n'
 'function bot_hoc120(nCurFac)\n'
 '\tif (GetLevel() < 120) then\n'
 '\t\treturn\n'
 '\tend\n'
 '\tif (nCurFac == nil or SKILL120AR[nCurFac] == nil) then\n'
 '\t\treturn\n'
 '\tend\n'
 '\tAddMagic(SKILL120AR[nCurFac], 20)\n'
 'end\n')
hv.xong()

# ---------------------------- simcity_admin.lua ----------------------------
sa = Tep(os.path.join("kiemthu", "item", "simcity_admin.lua"))
sa.ap("M1 muc menu PB_Menu",
 '\t' + vn('"Bang hội bot/PB_BangMenu",') + '\n',
 '\t' + vn('"Bang hội bot/PB_BangMenu",') + '\n'
 '\t' + vn('"Nâng kinh mạch bot/PB_MachMenu",') + '\n')

sa.ap("M2 PB_MachMenu + PB_MachBot",
 '-- ================= BANG HOI BOT (02/09) =================\n',
 '-- ================= NANG KINH MACH BOT (06/09) =================\n'
 '-- [NANGMACH 06/09] chu game: "toi muon viet them ham nang mach cho toan bo bot".\n'
 '-- Ham C PB_NangMach(nCap, nMach) (KPlayerBot.cpp): nCap 0..32, nMach 0 = ca 12 mach.\n'
 '-- Ap cho MOI bot dang song roi PB_SaveAll -> ben qua restart. Xem bot.log [BotMach].\n'
 'function PB_MachMenu()\n'
 '\tSayEx({' + vn('"<color=yellow>Nâng kinh mạch cho toàn bộ bot<color>\\n12 mạch, mỗi huyệt tới cấp chọn (tối đa 32); ghi blob ngay"') + ',\n'
 '\t' + vn('"Toàn bộ mạch cấp 32 (tối đa)/#PB_MachBot(32)",') + '\n'
 '\t' + vn('"Toàn bộ mạch cấp 24/#PB_MachBot(24)",') + '\n'
 '\t' + vn('"Toàn bộ mạch cấp 16/#PB_MachBot(16)",') + '\n'
 '\t' + vn('"Toàn bộ mạch cấp 8/#PB_MachBot(8)",') + '\n'
 '\t' + vn('"Xoá hết mạch (cấp 0)/#PB_MachBot(0)",') + '\n'
 '\t' + vn('"Quay lại/PB_Menu",') + '\n'
 '\tSC_END_SAY})\n'
 'end\n'
 '\n'
 'function PB_MachBot(nCap)\n'
 '\tif (PB_NangMach == nil) then\n'
 '\t\tMsg2Player(' + vn('"Bản CoreServer đang chạy chưa có PB_NangMach (chưa swap bản 06/09)."') + ')\n'
 '\t\treturn\n'
 '\tend\n'
 '\tlocal nDoi = PB_NangMach(nCap, 0)\n'
 '\tMsg2Player(format(' + vn('"Đã đặt kinh mạch cấp %d cho %d bot (xem bot.log [BotMach])."') + ', nCap, nDoi))\n'
 '\tPB_MachMenu()\n'
 'end\n'
 '\n'
 '-- ================= BANG HOI BOT (02/09) =================\n')
sa.xong()
