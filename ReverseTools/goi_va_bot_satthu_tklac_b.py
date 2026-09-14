# -*- coding: utf-8 -*-
r"""goi_va_bot_satthu_tklac_b.py - [SATTHU/TKLAC dot b 14/09] sua theo PHAN BIEN (tac tu doc lap)
tren ban goi_va_bot_satthu_tklac.py. Chuoi tai ap: ..._tklac.py -> ..._tklac_b.py.

CAO-1  Khoi [TKLAC] "return" VO DIEU KIEN: ChangeWorld ve 324 hong (map chua mo / pb_ODat khong ra
       o dat chan) thi bot ket VINH VIEN tren 379 - vi khoi nay dat TRUOC moi nhanh AI nen pb_RaBai
       (duong cuu con lai, van keo duoc bot PB_AI_FIGHT ra khoi map la) khong bao gio chay nua.
       -> them DUONG LUI y het pha 5: 3 lan truot thi ve THANH NHA (pb_ThanhNha + pb_DtVeThanh);
          ca duong do cung truot thi THOI return, de cac nhanh duoi tu lo.
CAO-2  pb_TkTraTrangThai (mot luot vao Lua bot_tk_thoat, ~12 lenh engine) + pb_Log goi lai MOI 10
       GIAY khong co tran, nhan len hang tram bot ket.
       -> tra trang thai DUNG MOT LAN (b.nTkGoiThu == 0); log lan dau + lan thanh cong + lan lui.
VUA    Thieu gac b.nNhomNguoiIdx / b.nBanSap: bot dang trong to doi NGUOI CHOI THAT (hoac dang ngoi
       ban sap) ma dung tren 379 se bi tra trang thai + giat ve 324. -> them hai gac nhu pb_TkDuTuCach.
THAP-1 Chu thich ghi sai dai template boss Sat Thu: bang addkillertasknpc dung 761..820 (khong phai
       760..819).
THAP-2 "kill_level" la chuoi con cua "skill_level" - hien KHONG co duong dan nao nhu vay trong cay
       script song, nhung sieu chat lai cho chac: so "killer\kill_level".

KHONG lam (co y, ghi ro trong ban giao): ep b.nAi = PB_AI_FIGHT sau khi ve 324. Chu game chi yeu cau
"ra map bao danh"; bat che do danh cho bot ma chu chua he bat la doi hanh vi ngoai yeu cau (chu thich
san co o pb_TkDuTuCach da chot y do nay).

Idempotent (dau "[TKLAC dot b]"); --thu = chi kiem neo. Doc/ghi latin-1, giu kieu xuong dong.
"""
import io
import os
import re
import sys

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.cpp"
THU = "--thu" in sys.argv


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


class Hunks(object):
    def __init__(self, path):
        self.path = path
        self.s = io.open(path, "r", encoding="latin-1", newline="").read()
        self.crlf = "\r\n" in self.s
        self.n = 0
        self.err = 0

    def sub(self, pat, new, count=1):
        p = "\r?\n".join(re.escape(x) for x in pat.split("\n"))
        found = len(re.findall(p, self.s, re.M))
        if found != count:
            print("  NEO KHONG KHOP (%d thay vi %d): %s" % (found, count, pat[:70].replace("\n", "|")))
            self.err += 1
            return
        nl = (lambda t: t.replace("\n", "\r\n")) if self.crlf else (lambda t: t)
        self.s = re.sub(p, lambda m: nl(new), self.s, count=0, flags=re.M)
        self.n += found

    def done(self, name):
        if self.err:
            print("%s: %d neo hong -> KHONG GHI" % (name, self.err))
            return False
        if THU:
            print("%s: %d hunk OK (thu)" % (name, self.n))
            return True
        s0 = io.open(self.path, "r", encoding="latin-1", newline="").read()
        if hb(s0) != hb(self.s):
            print("%s: SO BYTE CAO DOI %d -> %d -> KHONG GHI" % (name, hb(s0), hb(self.s)))
            return False
        io.open(self.path, "w", encoding="latin-1", newline="").write(self.s)
        print("%s: AP %d hunk, high-byte %d, %d -> %d byte" % (name, self.n, hb(self.s), len(s0), len(self.s)))
        return True


# --------------------------------------------------------------- THAP-1: dai template
B1_NEO = """// settings/npcs.txt template 760..819 LifeParam3 22.000 -> 1.300.000, LifeReplenish 0|20 -> 0|1000.
"""
B1_NEW = """// settings/npcs.txt template 761..820 LifeParam3 22.000 -> 1.300.000, LifeReplenish 0|20 -> 0|1000
// (mau la HANG SO theo template - LifeParam 100 / LifeParam1 = LifeParam2 = 0 nen Quadratic tra
// thang LifeParam3, khong phu thuoc cap; con nho mau nhat = 22.000 > nguong 20.000 ben duoi).
"""

# --------------------------------------------------------------- THAP-2: chuoi chat hon
B2_NEO = """	return pb_CoChuoi(Npc[nNpcIdx].ActionScript, "kill_level") ? 1 : 0;
"""
B2_NEW = """	// so ca thu muc ("killer\\\\kill_level") chu khong chi ten tep: "kill_level" la chuoi con cua
	// "skill_level" - cay script hien KHONG co duong dan nao nhu vay, nhung sieu chat lai cho chac.
	return pb_CoChuoi(Npc[nNpcIdx].ActionScript, "killer\\\\kill_level") ? 1 : 0;
"""

# --------------------------------------------------------------- CAO-1 + CAO-2 + VUA: khoi [TKLAC]
B3_NEO = """	if (b.nTk == 0 && SubWorld[nSub].m_SubWorldID == PB_TK_MAP)
	{
		const int nLechLac = (int)(&b - s_bots);
		// so le 1-31 giay: hang tram bot cung ChangeWorld mot khung la nghen region sync
		if (b.nTkTick == 0)
		{
			b.nTkTick = nowAll + (unsigned int)(GAME_FPS * (1 + (nLechLac % 30)));
			return;
		}
		if (nowAll < b.nTkTick)
			return;
		b.nTkTick = nowAll + (unsigned int)(GAME_FPS * 10);     // that bai thi 10 giay sau thu lai
		pb_TkTraTrangThai(nIdx);
		const int nSubBd = g_SubWorldSet.SearchWorld(PB_TK_MAP_BD);
		int nRet = 0;
		if (nSubBd >= 0)
		{
			// hai diem tha cua lib_tktc (battle_transprot): Tong (1541,3178) / Kim (1570,3085).
			// Bot lac khong con biet phe cua minh (b.nTkPhe da mat theo tien trinh) nen rai deu
			// hai diem theo chi so bot, moi con mot o rieng trong luoi 9x7 nhu pha 1.
			const int nOX = (nLechLac & 1) ? 1570 : 1541;
			const int nOY = (nLechLac & 1) ? 3085 : 3178;
			const int nDx = ((nLechLac % 9) - 4) * 2;
			const int nDy = (((nLechLac / 9) % 7) - 3) * 2;
			int nVx = 0, nVy = 0;
			if (pb_ODat(nSubBd, nOX + nDx, nOY + nDy, nLechLac, 10, &nVx, &nVy))
				nRet = Npc[nNpcIdx].ChangeWorld(PB_TK_MAP_BD, nVx, nVy);
			if (nRet != 1 && pb_ODat(nSubBd, nOX, nOY, nLechLac, 16, &nVx, &nVy))
				nRet = Npc[nNpcIdx].ChangeWorld(PB_TK_MAP_BD, nVx, nVy);
		}
		pb_Log("[BotTK] %s dung trong chien truong %d ma KHONG o trong tran (bao tri giua tran?)"
		       " -> tra trang thai + ve map bao danh %d (ret=%d)\\n",
		       Player[nIdx].m_PlayerName, PB_TK_MAP, PB_TK_MAP_BD, nRet);
		if (nRet == 1)
		{
			b.nTkTick = 0;
			b.nTargetNpc = 0;
			b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
			b.follow.Reset();  b.loot.Reset();
			b.nRoamX = 0;    b.nRoamY = 0;
		}
		return;
	}
"""
B3_NEW = """	// [TKLAC dot b 14/09] gac them nhom NGUOI CHOI THAT / ban sap (khuon pb_TkDuTuCach): bot dang
	// theo nguoi choi hoac dang ngoi sap ma tinh co dung tren 379 thi KHONG giat no di.
	if (b.nTk == 0 && SubWorld[nSub].m_SubWorldID == PB_TK_MAP
	 && !b.nNhomNguoiIdx && !b.nBanSap)
	{
		const int nLechLac = (int)(&b - s_bots);
		// so le 1-31 giay: hang tram bot cung ChangeWorld mot khung la nghen region sync
		if (b.nTkTick == 0)
		{
			b.nTkTick   = nowAll + (unsigned int)(GAME_FPS * (1 + (nLechLac % 30)));
			b.nTkGoiThu = 0;                    // muon lam bo dem so lan dua ve 324 that bai
			return;
		}
		if (nowAll < b.nTkTick)
			return;
		b.nTkTick = nowAll + (unsigned int)(GAME_FPS * 10);     // that bai thi 10 giay sau thu lai
		// [dot b] TRA TRANG THAI DUNG MOT LAN: bot_tk_thoat la mot luot vao Lua voi ~12 lenh engine,
		// goi lai moi 10 giay cho hang tram bot ket la ganh vo ich (phan bien 14/09).
		if (b.nTkGoiThu == 0)
			pb_TkTraTrangThai(nIdx);
		const int nSubBd = g_SubWorldSet.SearchWorld(PB_TK_MAP_BD);
		int nRet = 0;
		if (nSubBd >= 0)
		{
			// hai diem tha cua lib_tktc (battle_transprot): Tong (1541,3178) / Kim (1570,3085).
			// Bot lac khong con biet phe cua minh (b.nTkPhe da mat theo tien trinh) nen rai deu
			// hai diem theo chi so bot, moi con mot o rieng trong luoi 9x7 nhu pha 1.
			const int nOX = (nLechLac & 1) ? 1570 : 1541;
			const int nOY = (nLechLac & 1) ? 3085 : 3178;
			const int nDx = ((nLechLac % 9) - 4) * 2;
			const int nDy = (((nLechLac / 9) % 7) - 3) * 2;
			int nVx = 0, nVy = 0;
			if (pb_ODat(nSubBd, nOX + nDx, nOY + nDy, nLechLac, 10, &nVx, &nVy))
				nRet = Npc[nNpcIdx].ChangeWorld(PB_TK_MAP_BD, nVx, nVy);
			if (nRet != 1 && pb_ODat(nSubBd, nOX, nOY, nLechLac, 16, &nVx, &nVy))
				nRet = Npc[nNpcIdx].ChangeWorld(PB_TK_MAP_BD, nVx, nVy);
		}
		if (nRet == 1)
		{
			pb_Log("[BotTK] %s dung trong chien truong %d ma KHONG o trong tran (bao tri giua tran?)"
			       " -> da tra trang thai + ve map bao danh %d\\n",
			       Player[nIdx].m_PlayerName, PB_TK_MAP, PB_TK_MAP_BD);
			b.nTkTick = 0;  b.nTkGoiThu = 0;
			b.nTargetNpc = 0;
			b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
			b.follow.Reset();  b.loot.Reset();
			b.nRoamX = 0;    b.nRoamY = 0;
			return;
		}
		b.nTkGoiThu++;
		// [dot b] DUONG LUI (phan bien 14/09): khoi nay nam TRUOC moi nhanh AI nen neu cu "return"
		// mai thi pb_RaBai - duong cuu con lai - khong bao gio chay, bot ket VINH VIEN tren 379 khi
		// map 324 chua mo hoac khong tim duoc o dat chan. Sau 3 lan: lui ve THANH NHA y het pha 5;
		// ca duong do cung truot thi THOI return, de cac nhanh duoi tu lo.
		if (b.nTkGoiThu < 3)
		{
			if (b.nTkGoiThu == 1)
				pb_Log("[BotTK] %s dung trong chien truong %d (bao tri giua tran?) nhung CHUA ve duoc"
				       " map bao danh %d - thu lai moi 10 giay\\n",
				       Player[nIdx].m_PlayerName, PB_TK_MAP, PB_TK_MAP_BD);
			return;
		}
		{
			const PB_DtNpc& tn = pb_ThanhNha(b, nLechLac);
			const int bVe = pb_DtVeThanh(nIdx, nNpcIdx, b, tn, nLechLac);
			pb_Log("[BotTK] %s KHONG ve duoc map bao danh %d sau 3 lan -> lui ve thanh nha %d (ret=%d)\\n",
			       Player[nIdx].m_PlayerName, PB_TK_MAP_BD, tn.nMap, bVe);
			b.nTkTick = 0;  b.nTkGoiThu = 0;
			if (bVe)
				return;                         // da sang thanh nha - nhip sau di duong thuong
		}
		// van dung tren 379: KHONG return nua - de cac nhanh duoi (pb_RaBai...) tiep quan
	}
"""


def main():
    if not os.path.isfile(SRC):
        print("KHONG THAY: " + SRC)
        return 2
    h = Hunks(SRC)
    if "[TKLAC dot b" in h.s:
        print("KPlayerBot.cpp: DA AP")
        return 0
    if "[TKLAC 14/09]" not in h.s:
        print("CHUA AP dot a (goi_va_bot_satthu_tklac.py) - chay tep do truoc")
        return 3
    h.sub(B1_NEO, B1_NEW)
    h.sub(B2_NEO, B2_NEW)
    h.sub(B3_NEO, B3_NEW)
    ok = h.done("KPlayerBot.cpp")
    print("KET QUA: %s" % ("OK" if ok else "CO LOI"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
