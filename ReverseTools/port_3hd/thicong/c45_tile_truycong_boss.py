# -*- coding: utf-8 -*-
"""C45 - chu game: "truy cong lenh toi thay rot rat nhieu danh boss thuy tac la rot
roi hau nhu 90% rot".

DO THUC TE tren ca HAI ban (giong het nhau, KHONG phai loi port):
  * QUAI THUONG (shuizeideath.lua): random(1,100) < 50  => 49%
  * BOSS DAU LINH (bossdeath.lua)  : KHONG co kiem tra ti le nao ca - chi can
    jf0904_shuizei_IsActtime() == 1 la roi => 100%, moi con boss 1 cai.
  Ban Linux (script/missions/fengling_ferry/bossdeath.lua:31-38) y het.

=> "hau nhu 90%" ma chu game thay chinh la boss 100% + quai thuong 49% tron lai.

VA: KHONG doi can bang (Gate 4). Chi them KHOA CAU HINH cho nhanh boss, MAC DINH
100 = y het hanh vi hien tai; chu game muon ha thi sua mot so trong
cauhinh_hoatdong.lua. Nhanh quai thuong da co san khoa HD3_PLD_TILE_TRUYCONG = 50.
"""
import io, os, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"


def sync(rel):
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(os.path.join(JX1, rel), dst)


# ---------- 1) bossdeath.lua: boc nhanh roi Truy Cong Lenh bang khoa cau hinh ----
rel = r"script\missions\fengling_ferry\bossdeath.lua"
p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
if "HD3_PLD_TILE_TRUYCONG_BOSS" in d:
    print("bossdeath.lua: da va roi")
else:
    nl = "\r\n" if "\r\n" in d else "\n"
    cu = nl.join([
        "\t\tlocal nItemIdx = DropItem(nNpcIndex, 6, 1, 2024, 1, 0, 0);",
        "\t\t",
        '\t\tlocal nCurtime = tonumber(GetLocalDate("%H%M"));',
        "\t\tlocal nRestMin = 24 * 60 - (floor(nCurtime/100)*60+floor(mod(nCurtime, 100)));",
        "\t\tITEM_SetExpiredTime(nItemIdx, nRestMin);",
        "\t\tSyncItem(nItemIdx);",
    ])
    assert d.count(cu) == 1, "khong khop duy nhat: %d" % d.count(cu)
    moi = nl.join([
        "\t\t-- [3HD 25/08 C45] Ban Linux: BOSS dau linh roi Truy Cong Lenh 100% KHONG",
        "\t\t-- co kiem tra ti le (chi quai thuong moi random - shuizeideath.lua). Day la",
        "\t\t-- ly do chu game thay 'hau nhu 90% rot'. Giu MAC DINH 100 = y het ban goc;",
        "\t\t-- muon ha thi sua HD3_PLD_TILE_TRUYCONG_BOSS trong cauhinh_hoatdong.lua.",
        '\t\tif (random(1, 100) <= HD_CFG("HD3_PLD_TILE_TRUYCONG_BOSS", 100)) then',
        "\t\t\tlocal nItemIdx = DropItem(nNpcIndex, 6, 1, 2024, 1, 0, 0);",
        "\t\t\t",
        '\t\t\tlocal nCurtime = tonumber(GetLocalDate("%H%M"));',
        "\t\t\tlocal nRestMin = 24 * 60 - (floor(nCurtime/100)*60+floor(mod(nCurtime, 100)));",
        "\t\t\tITEM_SetExpiredTime(nItemIdx, nRestMin);",
        "\t\t\tSyncItem(nItemIdx);",
        "\t\tend",
    ])
    d = d.replace(cu, moi)
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    sync(rel)
    print("bossdeath.lua: da boc nhanh Truy Cong Lenh bang HD3_PLD_TILE_TRUYCONG_BOSS")

# ---------- 2) cauhinh_hoatdong.lua: them khoa ----------
rel = r"script\header\cauhinh_hoatdong.lua"
p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
if "HD3_PLD_TILE_TRUYCONG_BOSS" in d:
    print("cauhinh_hoatdong.lua: da co khoa")
else:
    nl = "\r\n" if "\r\n" in d else "\n"
    neo = "HD3_PLD_TILE_TRUYCONG = 50,"
    assert d.count(neo) == 1, d.count(neo)
    them = nl.join([
        neo,
        "",
        "-- Ti le phan tram roi Truy Cong Lenh tu BOSS dau linh (moi con, khi trong gio",
        "-- hoat dong). 100 = luon roi, dung y ban Linux. Ha xuong neu thay ra qua nhieu.",
        "HD3_PLD_TILE_TRUYCONG_BOSS = 100,",
    ])
    d = d.replace(neo, them)
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    sync(rel)
    print("cauhinh_hoatdong.lua: da them HD3_PLD_TILE_TRUYCONG_BOSS = 100")
