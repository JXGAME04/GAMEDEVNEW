# -*- coding: utf-8 -*-
"""[TKCHAT/TKDIEM 04/09] Va script TONG KIM may chu (cay chay that, TCVN3, doc/ghi latin-1):
 - tongtu.lua / kimtu.lua: mau cau giet dich bo dau cach thua quanh chuc vu + gui bang diem cho ca tran sau moi lan giet
 - lib_tktc.lua: TK_GuiDiemChoToi / TK_GuiDiemPhe (kenh UpdateBattleBox co san, nKind 6 = cap nhat, 9 = an)
 - maps/tongkim/trap/tongratrai.lua + kimratrai.lua: ra khoi hau doanh -> gui bang diem cho nguoi do
 - timertask/task03.lua PlayerEndTongKim: an bang truoc khi roi tran
Sao luu .truoc_tkchat_0409; idempotent theo dau [TKDIEM 04/09]. Chay: python va_server_tkchat.py [--check] [--root <script dir>]
"""
import io, os, sys, shutil

ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
if "--root" in sys.argv:
    ROOT = sys.argv[sys.argv.index("--root") + 1]
CHECK = "--check" in sys.argv
MARK = "[TKDIEM 04/09]"


def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def wr(p, s):
    if CHECK:
        print("  (check) ghi", p, len(s)); return
    bak = p + ".truoc_tkchat_0409"
    if not os.path.exists(bak):
        shutil.copyfile(p, bak)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  ghi:", p, len(s))


def eol(s):
    return "\r\n" if "\r\n" in s else "\n"


def hi(s):
    return sum(1 for c in s if ord(c) >= 128)


def rep_n(s, old, new, n_expect, label):
    n = s.count(old)
    if n != n_expect:
        raise AssertionError("%s: neo khop %d lan (mong %d): %s" % (label, n, n_expect, ascii(old[:80])))
    return s.replace(old, new)


def va_giet(fn, anchor_setmission):
    p = os.path.join(ROOT, "tinhnang", "tong_kim_tcap", fn)
    s = rd(p)
    if MARK in s:
        print("  da co, bo qua:", p); return
    E = eol(s); h0 = hi(s)
    # 1) bo dau cach thua: "%s %s <color> %s " -> "%s%s<color> %s " (2 cho: chuoi ca nhan dong 10 + Msg2MSAll dong 87)
    s = rep_n(s, "%s %s <color> %s ", "%s%s<color> %s ", 2, fn + " mau cau")
    # 2) sau dong cong diem phe: gui bang diem cho ca tran
    i = s.find(anchor_setmission)
    assert i >= 0 and s.count(anchor_setmission) == 1, fn + ": neo SetMission"
    j = s.find(E, i)
    assert j > i
    add = E + "\t\tTK_GuiDiemPhe(TKDIEM_KIND_CAPNHAT)\t-- " + MARK + " bang diem Tong VS Kim tren client (moi lan giet)"
    s = s[:j] + add + s[j:]
    assert hi(s) == h0
    wr(p, s)


def va_lib():
    p = os.path.join(ROOT, "tinhnang", "tong_kim_tcap", "lib_tktc.lua")
    s = rd(p)
    if MARK in s:
        print("  da co, bo qua:", p); return
    E = eol(s); h0 = hi(s)
    if not s.endswith(E):
        s += E
    add = E.join([
        "",
        "-- ================================================================ " + MARK + " BANG DIEM \"Tong VS Kim\" giua-tren man hinh",
        "-- Chu 04/09: \"mo ban 2.0 lay cai hien so diem nhu tren hinh gan vao tong kim cua du an\". Client Game.exe co lop",
        "-- KUiTongKimScore (mo tu KUiHuaihehepan cua 2.0: hai nhan diem + thanh can bang % = tong*100/(tong+kim)).",
        "-- Kenh: UpdateBattleBox(MS_TONGKIM, nTong, nKim, nDiemToi, nKind) -> S2C_BATTLE_BOX \"tong|kim|diem\" (5 tham so,",
        "-- co san trong ScriptFuns.cpp, khong doi giao thuc). nKind 6 = cap nhat (hien bang), 9 = an bang.",
        "-- Diem phe = GetMissionV(M_TICHLUYA) (Tong) / GetMissionV(M_TICHLUYB) (Kim) - dung bien cac script giet dang cong.",
        "-- PHAI goi trong ngu canh co PlayerIndex + SubWorldIndex cua tran (tongtu/kimtu/trap ra trai/task03 deu co).",
        "TKDIEM_KIND_CAPNHAT = 6",
        "TKDIEM_KIND_AN      = 9",
        "TKDIEM_MAX_O        = 600\t-- MAX_PLAYER_MISSION (KMission.h)",
        "",
        "-- gui cho CHINH nguoi choi PlayerIndex hien tai",
        "function TK_GuiDiemChoToi(nKind)",
        "\tlocal nIdx = PIdx2MSDIdx(MS_TONGKIM, PlayerIndex)",
        "\tif nIdx == nil or nIdx <= 0 then",
        "\t\treturn 0",
        "\tend",
        "\tUpdateBattleBox(MS_TONGKIM, GetMissionV(M_TICHLUYA), GetMissionV(M_TICHLUYB), GetPMParam(MS_TONGKIM, nIdx, 6), nKind or TKDIEM_KIND_CAPNHAT)",
        "\treturn 1",
        "end",
        "",
        "-- gui cho MOI nguoi con trong tran. O du lieu mission khong lien tuc va co the con chi so cu cua nguoi da roi,",
        "-- nen chi nhan o ma chinh nguoi choi do tra nguoc ve dung o (PIdx2MSDIdx == i). UpdateBattleBox tu bo qua",
        "-- nguoi da mat ket noi (m_nNetConnectIdx == -1).",
        "function TK_GuiDiemPhe(nKind)",
        "\tlocal nCu = PlayerIndex",
        "\tlocal nTong = GetMissionV(M_TICHLUYA)",
        "\tlocal nKim = GetMissionV(M_TICHLUYB)",
        "\tlocal nSo = 0",
        "\tfor i = 1, TKDIEM_MAX_O do",
        "\t\tlocal nIdx = MSDIdx2PIdx(MS_TONGKIM, i)",
        "\t\tif nIdx ~= nil and nIdx > 0 and PIdx2MSDIdx(MS_TONGKIM, nIdx) == i then",
        "\t\t\tPlayerIndex = nIdx",
        "\t\t\tUpdateBattleBox(MS_TONGKIM, nTong, nKim, GetPMParam(MS_TONGKIM, i, 6), nKind or TKDIEM_KIND_CAPNHAT)",
        "\t\t\tnSo = nSo + 1",
        "\t\tend",
        "\tend",
        "\tPlayerIndex = nCu",
        "\treturn nSo",
        "end",
        "",
    ])
    s = s + add
    assert hi(s) == h0
    wr(p, s)


def va_trap(fn):
    p = os.path.join(ROOT, "maps", "tongkim", "trap", fn)
    s = rd(p)
    if MARK in s:
        print("  da co, bo qua:", p); return
    E = eol(s); h0 = hi(s)
    anchor = "\tSetPMParam(MS_TONGKIM,nPlayerDataIdx,1,1)"
    i = s.find(anchor)
    assert i >= 0 and s.count(anchor) == 1, fn + ": neo SetPMParam"
    j = s.find(E, i)
    add = E + "\tTK_GuiDiemChoToi(TKDIEM_KIND_CAPNHAT)\t-- " + MARK + " vua ra khoi hau doanh: hien bang diem Tong VS Kim"
    s = s[:j] + add + s[j:]
    assert hi(s) == h0
    wr(p, s)


def va_task03():
    p = os.path.join(ROOT, "timertask", "task03.lua")
    s = rd(p)
    if MARK in s:
        print("  da co, bo qua:", p); return
    E = eol(s); h0 = hi(s)
    anchor = "\tPlayerIndex = MSDIdx2PIdx(MS_TONGKIM, dataindex)"
    i = s.find(anchor)
    assert i >= 0 and s.count(anchor) == 1, "task03: neo PlayerIndex"
    j = s.find(E, i)
    add = E + "\tTK_GuiDiemChoToi(TKDIEM_KIND_AN)\t-- " + MARK + " an bang diem truoc khi dua nguoi choi roi tran"
    s = s[:j] + add + s[j:]
    assert hi(s) == h0
    wr(p, s)


def main():
    ok = True
    steps = [("lib_tktc.lua", va_lib),
             ("tongtu.lua", lambda: va_giet("tongtu.lua", "SetMission(M_TICHLUYB,GetMissionV(M_TICHLUYB)+nPoint)")),
             ("kimtu.lua", lambda: va_giet("kimtu.lua", "SetMission(M_TICHLUYA,GetMissionV(M_TICHLUYA)+nPoint)")),
             ("tongratrai.lua", lambda: va_trap("tongratrai.lua")),
             ("kimratrai.lua", lambda: va_trap("kimratrai.lua")),
             ("task03.lua", va_task03)]
    for name, fn in steps:
        print("==", name)
        try:
            fn()
        except AssertionError as ex:
            ok = False
            print("  LOI:", ex)
    print("XONG" if ok else "CO LOI")


if __name__ == "__main__":
    main()
