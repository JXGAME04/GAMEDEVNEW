# -*- coding: utf-8 -*-
r"""goi_va_bot_satthu_tklac.py - [SATTHU/TKLAC 14/09] hai viec chu game giao:

(1) "fix lai bot khi luyen cong khong danh boss sat thu"  -> chu chot: BOT TRANH HAN BOSS SAT THU.
    DO THUC TE (bot.log 40 MB cuoi, 4 gio 14/09): 233 luot bot bo muc tieu co HP max 1.300.000
    = template 767/811..819 "Nhiem vu sat thu cap 90" (settings/npcs.txt: LifeParam3 1300000,
    LifeReplenish 0|1000 = TU HOI MAU). 14 boss bi 6-12 bot thay nhau can; luat chong-danh-vo-ich
    (pb_Fight: 10 giay khong sut noi 10% mau -> bo + cam 45 giay) lam MOI bot nha sau dung 10 giay,
    boss hoi mau lai gan day (do duoc: npc 90345 HP 772.318 -> 1.138.415) => khong con nao chet,
    bot phi ca gio quanh boss. Nhan dien boss sat thu = ActionScript chua "kill_level": script
    \script\task\tollgate\killer\kill_level.lua CHI duoc gan boi bang addkillertasknpc
    (killbosshead.lua, 160 dong / 59 template 761..820) - da quet ca cay script, khong NPC nao
    khac dung. KHONG dung cot LevelScript task_killboss.lua vi cot do CON gom Tong/Kim Tien phong
    (npcs.txt 1068/1073) = NPC trong tran Tong Kim, chan nham la hong Tong Kim.
    Vi tri va: pb_FindTarget (khong CHON boss) + pb_Fight (NHA ngay neu dang cam boss).

(2) "bot dang danh tong kim ma bao tri nua chung vao lai thi phai ra map bao danh chu khong duoc
    dung trong map tong kim". Sau restart bot nap lai DUNG CHO DA LUU (PB_OnRoleData: bot cu ->
    nAi = PB_AI_IN_FACTION), may trang thai Tong Kim nam trong bo nho nen b.nTk = 0, ma duong
    duy nhat keo bot ra khoi map la pb_RaBai - chi chay trong nhanh PB_AI_FIGHT (pb_DriveBot),
    tuc phai cho chu game bat lai che do danh quai. Trong khi do bot con MANG camp phe Tong/Kim
    + khoa co PK + diem hoi sinh trong tran tu blob. Va: bot o map 379 ma b.nTk == 0 -> tra trang
    thai (bot_tk_thoat) roi ChangeWorld ve map bao danh 324, dung diem battle_transprot cua
    lib_tktc nhu pha 1; so le 0-30 giay cho khoi ca dan doi map mot khung.

Idempotent (dau [SATTHU 14/09] / [TKLAC 14/09]); --thu = chi kiem neo, khong ghi.
Doc/ghi latin-1, giu kieu xuong dong cua tep, kiem so byte cao khong doi (ma them la ASCII).
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

    def _pat(self, pat):
        # Neo la VAN BAN THUAN, khong phai regex: escape tung dong (giu nguyen ( ) [ ] * . | va
        # ca literal \n cua chuoi C) roi noi lai bang \r?\n de khop ca tep LF lan CRLF.
        return "\r?\n".join(re.escape(x) for x in pat.split("\n"))

    def sub(self, pat, new, count=1):
        p = self._pat(pat)
        found = len(re.findall(p, self.s, re.M))
        if found != count:
            print("  NEO KHONG KHOP (%d thay vi %d): %s" % (found, count, pat[:70].replace("\n", "|")))
            self.err += 1
            return
        nl = (lambda t: t.replace("\n", "\r\n")) if self.crlf else (lambda t: t)
        self.s = re.sub(p, lambda m: nl(new.replace("\r\n", "\n")), self.s, count=0, flags=re.M)
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


# --------------------------------------------------------------------------- (1) BOSS SAT THU
H1_NEO = """// Tim quai GAN NHAT quanh bot.
"""
H1_NEW = r'''// [SATTHU 14/09] (chu game: "fix lai bot khi luyen cong khong danh boss sat thu" - chot: BOT
// TRANH HAN BOSS SAT THU) Boss nhiem vu Sat Thu (bang addkillertasknpc, killbosshead.lua - 160
// con tren 40 ban do, spawn luc boot qua HD3_DriverInit) co MAU RAT LON va TU HOI MAU:
// settings/npcs.txt template 760..819 LifeParam3 22.000 -> 1.300.000, LifeReplenish 0|20 -> 0|1000.
// Luat chong-danh-vo-ich cua pb_Fight (10 giay khong sut noi 10% mau -> bo muc tieu + cam 45 giay)
// vi vay LUON nha boss sau dung 10 giay, con boss thi hoi day mau tro lai - do that 14/09: 14 boss
// cap 90 bi 6-12 bot thay nhau can suot 4 tieng, khong con nao chet (npc 90345: HP 772.318 ->
// 1.138.415). Ket qua dung nhu chu game thay: "bot khong danh boss sat thu", con ca dan thi phi
// thoi gian quanh mot con khong bao gio ha duoc - va boss cua nguoi choi lam nhiem vu bi chiem cho.
//
// NHAN DIEN: ActionScript cua NPC chua "kill_level". Script
// \script\task\tollgate\killer\kill_level.lua CHI duoc gan boi add_killertasknpc (SetNpcScript,
// killbosshead.lua:190) - da quet ca cay script, khong NPC nao khac dung. CO Y KHONG dung cot
// LevelScript (task_killboss.lua) de nhan dien: cot do con gom Tong Tien phong / Kim Tien phong
// (npcs.txt 1068/1073) la NPC TRONG TRAN TONG KIM - chan nham la hong Tong Kim.
// Loc HP truoc (boss nho nhat 22.000 mau) nen quai thuong khong bao gio cham toi strstr.
#define PB_ST_HP_MIN   20000

static int pb_LaBossSatThu(int nNpcIdx)
{
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return 0;
	if (Npc[nNpcIdx].m_CurrentLifeMax < PB_ST_HP_MIN)
		return 0;
	if (!Npc[nNpcIdx].ActionScript[0])
		return 0;
	return pb_CoChuoi(Npc[nNpcIdx].ActionScript, "kill_level") ? 1 : 0;
}

// Tim quai GAN NHAT quanh bot.
'''

H2A_NEO = """	const int bTrongTK = (b.nTk >= 4 && SubWorld[nSub].m_SubWorldID == PB_TK_MAP);
"""
H2A_NEW = """	// [SATTHU 14/09] tach rieng "dang dung tren chien truong" (moi pha) de mien hang kiem boss
	// Sat Thu ben duoi; bTrongTK ("da RA TRAN") giu nguyen nghia cu, khong doi hanh vi nao.
	const int bMapTK   = (SubWorld[nSub].m_SubWorldID == PB_TK_MAP);
	const int bTrongTK = (b.nTk >= 4 && bMapTK);
"""

H2_NEO = """			if (Npc[i].m_Kind == kind_player
			 && !(b.nTk >= 4 && SubWorld[nSub].m_SubWorldID == PB_TK_MAP))
				continue;
"""
H2_NEW = """			if (Npc[i].m_Kind == kind_player
			 && !(b.nTk >= 4 && SubWorld[nSub].m_SubWorldID == PB_TK_MAP))
				continue;
			// [SATTHU 14/09] boss nhiem vu Sat Thu: KHONG nhan lam muc tieu (xem chu thich
			// pb_LaBossSatThu). Chien truong Tong Kim khong co boss Sat Thu nhung LAI day NPC
			// mau lon (Tong/Kim binh 30.000, Tien phong 1.500.000) nen mien hang kiem nay o do
			// - khong ton mot lan strstr nao trong tran.
			if (!bMapTK && pb_LaBossSatThu(i))              continue;
"""

H3_NEO = """			if (Npc[t].m_Doing == do_death)
				b.nLootScanTick = 0;
			t = 0;
		}
	}
	else
		t = 0;
"""
H3_NEW = r"""			if (Npc[t].m_Doing == do_death)
				b.nLootScanTick = 0;
			t = 0;
		}
		// [SATTHU 14/09] dang cam boss Sat Thu (muc tieu chot TRUOC khi nap ban nay, hoac do
		// duong khac gan) -> NHA NGAY, khong doi dong ho 10 giay. Cam 45 giay nhu moi lan bo
		// muc tieu de khong quay dau lai ngay nhip sau. Bot dang trong Tong Kim mien kiem
		// (b.nTk) - trong tran khong co boss Sat Thu ma NPC mau lon thi nhieu.
		else if (b.nTk == 0 && pb_LaBossSatThu(t))
		{
			pb_Log("[BotDanh] %s tranh boss Sat Thu %d (HP %d/%d) -> bo muc tieu, ve danh quai thuong\n",
			       Player[nIdx].m_PlayerName, t,
			       (int)Npc[t].m_CurrentLife, (int)Npc[t].m_CurrentLifeMax);
			pb_CamMucTieu(b, t, now);
			t = 0;
		}
	}
	else
		t = 0;
"""

# --------------------------------------------------------------------------- (2) BOT LAC MAP TK
H4_NEO = """	// ---------------------------------------------------------------- TONG KIM
	// (21/08) Dat NGAY DAY: sau khoi tu hoi sinh (bot chet trong tran van phai
"""
H4_NEW = r'''	// ------------------------------------------------- [TKLAC 14/09] LAC TRONG CHIEN TRUONG
	// (chu game: "bot dang danh tong kim ma bao tri nua chung vao lai thi phai ra map bao danh
	// chu khong duoc dung trong map tong kim")
	//
	// May trang thai Tong Kim (b.nTk) nam trong BO NHO tien trinh, con vi tri bot thi nam trong
	// blob roledb. Bao tri giua tran -> bot nap lai DUNG GIUA map 379 voi b.nTk = 0, va con mang
	// theo camp phe Tong/Kim + khoa co PK + diem hoi sinh trong tran (pb_TkTraTrangThai chua tung
	// chay cho no). Duong duy nhat keo bot ra khoi map la pb_RaBai, ma pb_RaBai nam trong nhanh
	// PB_AI_FIGHT (bot cu nap lai la PB_AI_IN_FACTION - PB_OnRoleData) nen bot DUNG YEN trong
	// chien truong cho toi khi chu game bat lai che do danh quai.
	//
	// Xu ly y het nguoi choi that luc het tran (task03 NewWorld ve map bao danh): tra trang thai
	// roi ChangeWorld ve 324, dap dung diem battle_transprot cua lib_tktc nhu pha 1. Bot DANG
	// trong tran (b.nTk 1..5) khong bi dong toi - moi nhanh do van do pb_TkLai lo.
	if (b.nTk == 0 && SubWorld[nSub].m_SubWorldID == PB_TK_MAP)
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
		       " -> tra trang thai + ve map bao danh %d (ret=%d)\n",
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

	// ---------------------------------------------------------------- TONG KIM
	// (21/08) Dat NGAY DAY: sau khoi tu hoi sinh (bot chet trong tran van phai
'''


def main():
    if not os.path.isfile(SRC):
        print("KHONG THAY: " + SRC)
        return 2
    h = Hunks(SRC)
    if "[SATTHU 14/09]" in h.s and "[TKLAC 14/09]" in h.s:
        print("KPlayerBot.cpp: DA AP")
        return 0
    h.sub(H1_NEO, H1_NEW)
    h.sub(H2A_NEO, H2A_NEW)
    h.sub(H2_NEO, H2_NEW)
    h.sub(H3_NEO, H3_NEW)
    h.sub(H4_NEO, H4_NEW)
    ok = h.done("KPlayerBot.cpp")
    print("KET QUA: %s" % ("OK" if ok else "CO LOI"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
