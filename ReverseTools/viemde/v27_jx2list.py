# -*- coding: utf-8 -*-
"""v27_jx2list.py - Danh dau cay yandibaozang la SCRIPT LINUX (JX2).

GOC: g_IsJx2Script (KSortScript.cpp:113) liet ke cac cay script CHEP NGUYEN VAN
tu Linux, vi 4 ham co NGU NGHIA KHAC giua hai ban. Cay
"\\script\\missions\\yandibaozang\\" KHONG co trong danh sach du no dung la
script Linux chep nguyen => dang chay sai 2 cho:

  1) GetTeamMember (ScriptFuns.cpp:7637-7656)
     Linux 0x08115530: n == 1 -> DOI TRUONG; n >= 2 -> thanh vien hop le thu n-1.
     JX1 (khong co co): n -> m_nMember[n-1] (BO SOT doi truong, doc lo mot o).
     Cay Viem De viet dung khuon Linux "for i = 1, GetTeamSize() do
     GetTeamMember(i)": npc\\yandibaozang_main.lua:90 (kiem tung thanh vien),
     :134 (TRU VE tung nguoi), :174 (lap danh sach dua VAO TRAN).
     => hom nay: doi truong KHONG bi tru ve va KHONG duoc dua vao tran, con
     nguoi cuoi danh sach doc phai o trong (nPlayerIndex = 0).

  2) AddSkillState (ScriptFuns.cpp:13016-13019)
     Linux 0x08125D70 LUON ap skill that; JX1 chi ap khi tham so 3 khac 0.
     include.lua:37-40 goi AddSkillState(461,1,1,0,1) / (458,2,0,0,1) /
     (459,2,0,0,1) va readymap\\include.lua:157/164 + yandibaozang_main.lua:112
     goi (461,1,1,30*60*18,1) - buff Viem De Lenh.
     => cac loi goi tham so 3 = 0 (458/459) hom nay KHONG ap skill that.

Ba cho con lai dung co nay deu KHONG anh huong cay Viem De:
  - GetGameTime (:281): cay Viem De khong goi (da grep).
  - AddNpcEx tham so 7 (:7066): da chuyen sang HD3_AddNpcEx (v24).
  - (SetPunish khong phu thuoc co nay - da kiem.)

Tep dich: Sources\\Core\\Src\\KSortScript.cpp (file CHUNG) => build CA HAI
cau hinh, dat canh cho chu swap.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_ydbzjx2 lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KSortScript.cpp"
HAU_TO = ".truoc_ydbzjx2"
NHAN = "[VIEMDE 29/08]"

NEO = '\t\t"\\\\script\\\\task\\\\partner\\\\",'
MOI = [
    NEO,
    "\t\t// " + NHAN + " cay Viem De Bao Tang la script Linux chep nguyen van:",
    "\t\t// GetTeamMember phai theo quy uoc Linux (vi tri 1 = doi truong) - cay nay",
    "\t\t// viet \"for i = 1, GetTeamSize() do GetTeamMember(i)\" o yandibaozang_main",
    "\t\t// :90/:134/:174 (kiem, tru ve, lap danh sach vao tran); va AddSkillState",
    "\t\t// phai LUON ap skill that (include.lua:37-40 buff 461/458/459).",
    '\t\t"\\\\script\\\\missions\\\\yandibaozang\\\\",',
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== v27_jx2list - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0
    if "yandibaozang" in raw:
        print("!!! LOI TO: da co chuoi yandibaozang trong tep (kiem lai bang tay)")
        return 1

    d = raw.split(eol)
    vt = [i for i, l in enumerate(d) if l.rstrip() == NEO]
    if len(vt) != 1:
        print("!!! LOI TO: neo khop %d lan (can 1)" % len(vt))
        for i, l in enumerate(d):
            if "task\\\\partner" in l:
                print("    dong %d: %r" % (i + 1, l[:80]))
        return 1
    i = vt[0]
    d[i:i + 1] = MOI
    print("  ok  them 1 muc vao danh sach szJx2 (sau dong %d)" % (i + 1))

    nd = eol.join(d)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    # chot: mang szJx2 khai bao kich thuoc TU DONG (sizeof/sizeof) - kiem lai
    if "sizeof(szJx2) / sizeof(szJx2[0])" not in nd:
        print("!!! LOI TO: vong duyet khong con dung sizeof - kiem tay")
        return 1
    if nd.count('"\\\\script\\\\missions\\\\yandibaozang\\\\",') != 1:
        print("!!! LOI TO: muc moi khong duy nhat")
        return 1
    print("  chot: byte cao %d, mang tu dem bang sizeof, muc moi duy nhat" % hi0)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % os.path.basename(sao))
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. => build CA HAI cau hinh, dat canh cho chu swap.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
