# -*- coding: utf-8 -*-
"""[PS 04/09 b] GOC that su lam va [PS 04/09] chi bo duoc 3%: goi ngoai hinh MANG THEO RAC NGAN XEP.
 KNpc::NormalSync khai bao 'PLAYER_NORMAL_SYNC PlayerSync;' KHONG khoi tao, roi:
   - strcpy(PlayerSync.MateName, ...) va strcpy(PlayerSync.GameTitle, ...) chi ghi toi dau NUL,
     32-64 byte con lai giu nguyen RAC NGAN XEP;
   - vai truong (RankID, RankBattleID, PlayerTitle, m_bBaiTan, ExItemID, ExBoxID, MantleLevel...)
     khong duoc gan o moi nhanh.
 Rac do DOI moi lan goi => bam noi dung luon khac => 97%% goi van phat (nhat ky [PS-BO] 3%% bo).
 Va: memset ca goi ve 0 truoc khi dien. Hai cai loi cung luc:
   1. Bam on dinh => dung han viec gui lai goi khong doi (dung y do cua muc 10.16).
   2. Thoi RO RI: moi goi 234 byte truoc day mang vai chuc byte ngan xep may chu toi MOI client.
 Lam ca o KNpc::SendSyncData (cung kieu strcpy, cung ro ri, va do la goi nguoi vua nhin thay minh nhan).
 Doc/ghi latin-1 newline=''. Neo kiem dem == 1."""
import io, os

P = r"D:\GAMEDEVNEW_wt_bc\Sources\Core\Src\KNpc.cpp"
MARK = "[PS 04/09 b]"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)

s = rd(P); N = "\r\n" if "\r\n" in s else "\n"
if MARK in s:
    print("da va"); raise SystemExit

for kieu, ten in (("PLAYER_NORMAL_SYNC", "NormalSync"), ("PLAYER_SYNC", "SendSyncData")):
    old = "\t\t%s\tPlayerSync;" % kieu
    if old not in s:
        old = "\t\t%s PlayerSync;" % kieu
    n = s.count(old)
    assert n == 1, "%s: tim thay %d cho (can 1)" % (ten, n)
    new = (old + N +
           "\t\t// %s goi nay dung strcpy cho MateName/GameTitle nen phan duoi buffer la RAC NGAN XEP," % MARK + N +
           "\t\t// vai truong lai khong duoc gan o moi nhanh. Rac doi moi lan goi -> bam luon khac ->" + N +
           "\t\t// khong the bo goi trung, va con day vai chuc byte ngan xep may chu toi client." + N +
           "\t\tmemset(&PlayerSync, 0, sizeof(PlayerSync));" + N)
    s = s.replace(old, new, 1)
    print("%-14s: da them memset" % ten)
wr(P, s)
