# -*- coding: utf-8 -*-
# vhtd_data_patch13_vongsang.py [VHTD 02/09w]
# VONG SANG XANH DUOI CHAN cho Huyen Nhan Van Yen (1358) - chu yeu cau o dot 11.
#
# CO CHE (da xac dinh): KHONG phai SetSpecialSpr. Day la nhom trang thai kieu "Foot" -
# KNpcRes.cpp:361-389 duyet o 12..17 dat oPosition.nZ = 0, roi dong 461 DrawPrimitives ve
# TRUOC than nguoi. Nguon du lieu: settings\npcres\state_magic_table_name.txt cot 3 = Head/Foot.
# Duong tu ky nang: skills.txt cot StateSpecialId -> CastInitiativeSkill -> SetStateSkillEffect
# -> m_StateGraphics -> UpdateNpcStateInfo -> NpcSync.StateInfo -> client SetNpcState -> SetState.
# Het buff thi KNpc.cpp:1580-1583 tu goi lai va vong sang TU TAT. Tuc CHI SUA DU LIEU.
#
# CACH LAM: skills.txt hang SkillId 1358, cot StateSpecialId (chi so 9, 0-based): 0 -> 147.
#   Status147 = \spr\skill\others\lvguagnhuan.spr, kieu Foot, Loop, ghi chu = vong sang MAU XANH LA.
#   Da co san trong client cua ta va dang duoc ky nang 887 / 943 / 944 / 1532 dung (StatePriority = 0)
#   nen chac chan nap duoc. Ta theo dung mau do: giu StatePriority = 0.
#
# TUYET DOI KHONG them dong Status244 vao bang trang thai: SkillId 1598 dang co StateSpecialId = 244
# va 1599 = 245. Hom nay chung khong ve gi CHI VI bang dung o Status243 -> ten tep rong -> engine bo
# qua. Them Status244 thi 1598 lap tuc moc vong sang. Bang doc theo VI TRI (GetInfo(nNo) -> hang nNo+1)
# nen cung khong the nhay coc.
# Dung lai id 147 la an toan: KNpcRes.cpp:1263-1272 khu trung lap theo m_nID, va m_btStateInfo duoc
# dung lai tu danh sach buff con song moi lan -> vong sang tat dung luc buff cuoi cung het han.
#
# CANH BAO NGUON GOC - PHAI NOI VOI CHU: do that thi ban VLTK CUNG de 1358 StateSpecialId = 0,
# bang trang thai VLTK khong co dong nao cho 1358, va thu muc spr Hoa Son khong co tep vong sang cua
# 1358. Tuc day la MO RONG theo yeu cau chu, KHONG phai khoi phuc dung ban VLTK.
# Neu chu doi mau: 146 = do (hongguanhuan), 148 = xanh duong (languanhuan), 149 = vang (jinguanhuan).
#
# Thuan du lieu: KHONG build, KHONG swap. Restart GameServer + vao lai client
# (client cung doc skills.txt - CoreShell.cpp goi GetStateSpecailId o 4 cho).
#
# DUNG: python vhtd_data_patch13_vongsang.py --kiem | (khong co) | --hoan-tac
import io
import os
import sys
import shutil

KIEM = "--kiem" in sys.argv
HOANTAC = "--hoan-tac" in sys.argv
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

BIN = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin"
BAK = ".truoc_vongsang_0209"
SKILL_ID = "1358"
VONG_SANG = "147"


def main():
    for side in ("server", "client"):
        p = BIN + "/" + side + "/settings/skills.txt"
        if HOANTAC:
            if os.path.exists(p + BAK):
                shutil.copy2(p + BAK, p)
                print("  [<] %s skills.txt: da hoan tac" % side)
            else:
                print("  [!] %s: khong co ban luu %s" % (side, BAK))
            continue
        s = io.open(p, "r", encoding="latin-1", newline="").read()
        cao_truoc = sum(1 for ch in s if ord(ch) >= 0x80)
        nl = "\r\n" if s.count("\r\n") * 2 > s.count("\n") else "\n"
        lines = s.split(nl)
        hdr = lines[0].split("\t")
        c_id = hdr.index("SkillId")
        c_st = hdr.index("StateSpecialId")
        n = 0
        for i in range(1, len(lines)):
            c = lines[i].split("\t")
            if len(c) <= c_st or c[c_id].strip() != SKILL_ID:
                continue
            cu = c[c_st].strip()
            if cu == VONG_SANG:
                print("  [=] %s hang %s da la %s" % (side, SKILL_ID, VONG_SANG))
                break
            c[c_st] = VONG_SANG
            lines[i] = "\t".join(c)
            n += 1
            print("  [+] %s hang %s: StateSpecialId %s -> %s (vong sang xanh la, kieu Foot)"
                  % (side, SKILL_ID, cu, VONG_SANG))
            break
        if not n:
            continue
        out = nl.join(lines)
        if sum(1 for ch in out if ord(ch) >= 0x80) != cao_truoc:
            raise SystemExit("lech byte cao tai " + p)
        if chr(0xFFFD) in out:
            raise SystemExit("FFFD trong " + p)
        if not KIEM:
            if not os.path.exists(p + BAK):
                shutil.copy2(p, p + BAK)
            io.open(p, "w", encoding="latin-1", newline="").write(out)
        print("  => %s %s" % ("KIEM" if KIEM else "GHI", p))


if __name__ == "__main__":
    print("vhtd_data_patch13_vongsang [VHTD 02/09w]%s%s"
          % (" (KIEM)" if KIEM else "", " (HOAN TAC)" if HOANTAC else ""))
    main()
    print("XONG. Thuan du lieu: KHONG build, KHONG swap. Restart GameServer + vao lai client.")
