# -*- coding: utf-8 -*-
"""hs_script2.py [HOASON 01/09c] - noi Hoa Son vao cac he phat/nang ky nang theo phai con sot (ket qua quet workflow):
  skill\\skillfaction.lua (NPC nang tran phai), skill\\nangskillkieumoi5x|9x|tp.lua (NPC linh ngo), item\\daithanhbk90|120.lua (sach Dai Thanh),
  lib\\lib_faction.lua (bang tbSkillBase/120/150 + nhanh 2 ky nang 90/150), npcthon\\balanghuyen\\hotrotest.lua (PUBG chon phai ngau nhien).
"""
import io, os, sys
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
MARK = "[HOASON 01/09c]"
S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
def V(s): return unicode_to_tcvn3_bytes(s).decode("latin-1")
def G(s): return s.encode("gbk").decode("latin-1")
HS = G("华山派"); CB = G("丐帮")
class F:
    def __init__(self, rel):
        self.p = os.path.join(S, rel); self.d = io.open(self.p, "r", encoding="latin-1", newline="").read()
        self.nl = "\r\n" if "\r\n" in self.d else "\n"; self.rel = rel; self.n = 0
    def nx(self, s): return s.replace("\r\n", "\n").replace("\n", "\r\n") if self.nl == "\r\n" else s
    def thay(self, old, new, so=1):
        old = self.nx(old); new = self.nx(new); c = self.d.count(old)
        assert c == so, "%s: neo %d lan (can %d): %r" % (self.rel, c, so, old[:80])
        self.d = self.d.replace(old, new); self.n += 1
    def ghi(self):
        if MARK in self.d or self.n:
            io.open(self.p, "w", encoding="latin-1", newline="").write(self.d); print("  ghi", self.rel, self.n)

# 1. skillfaction.lua
f = F(r"skill\skillfaction.lua")
if MARK not in f.d:
    f.thay('elseif (player_Faction == "' + CB + '") then\nreturn 7\nelse\nreturn -1\n',
           'elseif (player_Faction == "' + CB + '") then\nreturn 7\nelseif (player_Faction == "' + HS + '") then -- ' + MARK + '\nreturn 11\nelse\nreturn -1\n')
    f.thay('{275,"Suong Ngao Con Luan"}\n}\n', '{275,"Suong Ngao Con Luan"},\n{1358,"Huyen Nhan Van Yen"} -- ' + MARK + '\n}\n')
    f.ghi()
# 2. nangskillkieumoi5x.lua
f = F(r"skill\nangskillkieumoi5x.lua")
if MARK not in f.d:
    f.thay('{{169,179},{176,182}}\n\n}\n', '{{169,179},{176,182}},\n{{1349,1374},{1355,1379}} -- ' + MARK + ' Hoa Son: ho tro = tong quyet 10 (kiem/khi), 5x = 1355/1379\n\n}\n')
    f.thay('elseif (player_Faction == "' + CB + '") then\nmp = 7\nelse \nmp = 0\n',
           'elseif (player_Faction == "' + CB + '") then\nmp = 7\nelseif (player_Faction == "' + HS + '") then -- ' + MARK + '\nmp = 11\nelse \nmp = 0\n')
    f.ghi()
# 3. nangskillkieumoitp.lua
f = F(r"skill\nangskillkieumoitp.lua")
if MARK not in f.d:
    f.thay('{167,168}},\n', '{167,168},{1347,1372}}, -- ' + MARK + ' Hoa Son nhap mon\n')
    f.thay('{273,36,48,75,252,114,130,150,166,275},\n', '{273,36,48,75,252,114,130,150,166,275,1358}, -- ' + MARK + ' 1358 Huyen Nhan Van Yen\n')
    c = f.d.count('elseif (player_Faction == "' + CB + '") then\nmp = 7\nelse \nmp = 0\n')
    if c == 1:
        f.thay('elseif (player_Faction == "' + CB + '") then\nmp = 7\nelse \nmp = 0\n',
               'elseif (player_Faction == "' + CB + '") then\nmp = 7\nelseif (player_Faction == "' + HS + '") then -- ' + MARK + '\nmp = 11\nelse \nmp = 0\n')
    else:
        print("  tp: nhanh phai khac dang (so lan %d) - can xem tay" % c)
    f.ghi()
# 4. nangskillkieumoi9x.lua
f = F(r"skill\nangskillkieumoi9x.lua")
if MARK not in f.d:
    f.thay('{{169,172},{176,182}},\n},\n', '{{169,172},{176,182}},\n{{1349,1374},{1355,1379}}, -- ' + MARK + '\n},\n')
    f.thay('{385,384}\n}\n\n}\n', '{385,384},\n{1364,1382} -- ' + MARK + '\n}\n\n}\n')
    f.thay('elseif (player_Faction == "' + CB + '") then\nmp = 7\nelse \nmp = 0\n',
           'elseif (player_Faction == "' + CB + '") then\nmp = 7\nelseif (player_Faction == "' + HS + '") then -- ' + MARK + '\nmp = 11\nelse \nmp = 0\n')
    f.ghi()
# 5. daithanhbk90 / 120 (chi so 0-based = GetFactionNo)
f = F(r"item\daithanhbk90.lua")
if MARK not in f.d:
    f.thay('\t\t[9] = {372, 375}, \n', '\t\t[9] = {372, 375}, \n\t\t[10] = {1364, 1382}, -- ' + MARK + '\n'); f.ghi()
f = F(r"item\daithanhbk120.lua")
if MARK not in f.d:
    f.thay('\t\t[9] = {717}, \n', '\t\t[9] = {717}, \n\t\t[10] = {1365}, -- ' + MARK + '\n'); f.ghi()
# 6. lib_faction.lua
f = F(r"lib\lib_faction.lua")
if MARK not in f.d:
    f.thay('\t{169,179,167,168,174,171,392,178,172,393,173,175,181,176,182,90,275,630},\n}\n',
           '\t{169,179,167,168,174,171,392,178,172,393,173,175,181,176,182,90,275,630},\n\t{1347,1372,1349,1374,1350,1375,1351,1376,1354,1378,1355,1379,1358,1360,1380}, -- ' + MARK + ' Hoa Son\n}\n')
    f.thay('\t{"' + V("Lưỡng Nghi Chân Khí") + '",717},\n}\n', '\t{"' + V("Lưỡng Nghi Chân Khí") + '",717},\n\t{"' + V("Tử Hà Kiếm Khí") + '",1365}, -- ' + MARK + '\n}\n')
    f.thay('\t\t{"' + V("Thiên Lôi Chấn Nhạc") + '", 1081},\n\t},\n}\n',
           '\t\t{"' + V("Thiên Lôi Chấn Nhạc") + '", 1081},\n\t},\n\t{ -- ' + MARK + ' Hoa Son\n\t\t{"' + V("Cửu Kiếm Hợp Nhất") + '", 1369},\n\t\t{"' + V("Thần Quang Toàn Nhiễu") + '", 1384},\n\t},\n}\n')
    # 11 = Hoa Son (2 ky nang 90/150) - KHONG ghi comment sau ')' vi 'then' nam cung dong
    f.thay('(nFactionID == 6 or nFactionID == 7 or nFactionID == 9)', '(nFactionID == 6 or nFactionID == 7 or nFactionID == 9 or nFactionID == 11)', 2)
    f.ghi()
# 7. hotrotest.lua PUBG: he Thuy co the trung Hoa Son
f = F(r"npcthon\balanghuyen\hotrotest.lua")
if MARK not in f.d:
    f.thay('\t\t\tgianhapmonphai(random(series*2,series*2+1))\n',
           '\t\t\tlocal tbPh = {series*2, series*2+1}\t-- ' + MARK + ' he Thuy (2) co them Hoa Son (10)\n\t\t\tif (series == 2) then tinsert(tbPh, 10) end\n\t\t\tgianhapmonphai(tbPh[random(1, getn(tbPh))])\n')
    f.ghi()
print("XONG")
