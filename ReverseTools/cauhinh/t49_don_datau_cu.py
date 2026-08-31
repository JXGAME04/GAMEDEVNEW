# -*- coding: utf-8 -*-
"""t49_don_datau_cu.py - DON TRIET DE he Da Tau CU (chu game yeu cau 30/08).

VUONG MAC: Sources\\Core\\Src\\KNpc.cpp:1698 goi CUNG
  ExecuteScript2("\\script\\tinhnang\\datau\\danhquai.lua", "OnDeathMonsterDaTau", ...)
MOI LAN nguoi choi giet quai; ma tep do nay con mang MOC DEM GIET QUAI CUA BAN
DONG HANH (danhquai.lua:5 Include bdh_killhook.lua, :20 call BDH_OnKillNpc).

CACH LAM - hai chang, chang A khong can DLL:

CHANG A (lam ngay, khong can dung may chu):
  A1. Tao script\\global\\onkillnpc.lua - NHA MOI cho moc "nguoi choi giet NPC".
      Chi con phan Ban Dong Hanh, khong dinh gi toi Da Tau.
  A2. Rut danhquai.lua thanh BO CHUYEN TIEP 3 dong goi sang tep moi. Nho vay
      DLL DANG CHAY (con tro duong dan cu) van hoat dong binh thuong.
  A3. Go nhanh du phong he cu trong global\\station.lua ham godatau() + dong
      Include lib_datau.lua.
      AN TOAN VI: lib_ham.lua:261-267 reset MOI NGAY cac task cua he cu
      (T_SoLanHuyNV, T_SoNVTrongNgay, T_TimVatPham, T_TIENDONV, T_TIMDOCHI,
      T_TIMMATCHI, T_DanhQuai) -> khong nhan vat nao ket vinh vien.
      Da kiem them: task 87 (T_SoNVTrongNgay) chi he Da Tau CU ghi, va he doc
      no (TB_BU_HD trong lib_ham) KHONG co cho phat thuong nao
      (lenhbaitanthu.lua:107 da comment) -> bo di khong mat quyen loi ai.
  A4. Doi 11 tep con lai cua tinhnang\\datau sang _dara.

CHANG B (can dung ky DLL - lam sau, bao chu game):
  Sua KNpc.cpp:1698 tro thang sang \\script\\global\\onkillnpc.lua /
  "OnPlayerKillNpc", dung lai CoreServer.dll. Sau khi swap thi bo chuyen tiep
  danhquai.lua het viec, doi not la thu muc tinhnang\\datau bien mat han.

Mac dinh DIEN TAP; --ghi moi lam that.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402
import doi_tep as dt  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = dt.S
MOC = "[DONDATAU 30/08]"
DUOI = ".truoc_dondatau"

P_MOI = os.path.join(S, "global", "onkillnpc.lua")
P_DQ = os.path.join(S, "tinhnang", "datau", "danhquai.lua")
P_ST = os.path.join(S, "global", "station.lua")

TEP_MOI = "\r\n".join([
    "-- onkillnpc.lua - %s" % MOC,
    "-- MOC \"nguoi choi giet mot NPC\". Duoc goi tu C++ (KNpc.cpp, KNpc::Die)",
    "-- mot lan cho MOI xac NPC do nguoi choi ha.",
    "--",
    "-- Truoc day moc nay o nho trong script\\tinhnang\\datau\\danhquai.lua cua he",
    "-- Da Tau CU. He do da go (ban thay = task\\newtask\\tasklink, port 15-16/08),",
    "-- nhung phan dem giet quai cua BAN DONG HANH thi van phai chay moi lan giet",
    "-- quai - nen tach ra day, khong con dinh gi toi Da Tau.",
    "--",
    "-- CHU Y HIEU NANG: ham nay chay MOI LAN co NPC chet vi tay nguoi choi.",
    "-- Dung them viec nang vao day.",
    "",
    "Include(\"\\\\script\\\\task\\\\partner\\\\bdh_killhook.lua\")",
    "",
    "function OnPlayerKillNpc(nNpcIndex, nDamageIndex)",
    "\tlocal nPlayerIndex = NpcIdx2PIdx(nDamageIndex)",
    "\tif (nPlayerIndex == 0) then\t-- quai giet quai thi thoi",
    "\t\treturn",
    "\tend",
    "\tPlayerIndex = nPlayerIndex",
    "\tcall(BDH_OnKillNpc, {nNpcIndex}, \"x\")",
    "end",
    "",
    "-- Ten cu, giu lai cho DLL chua swap (KNpc.cpp:1698 van goi ten nay).",
    "function OnDeathMonsterDaTau(nNpcIndex, nDamageIndex)",
    "\tOnPlayerKillNpc(nNpcIndex, nDamageIndex)",
    "end",
    "",
    "function OnRevive(nNpcIndex)",
    "end",
    "",
])

CHUYEN_TIEP = "\r\n".join([
    "-- danhquai.lua - %s BO CHUYEN TIEP, khong con logic Da Tau." % MOC,
    "--",
    "-- He Da Tau CU da go het (ban thay = script\\task\\newtask\\tasklink,",
    "-- port 15-16/08). Tep nay chi con ton tai vi Sources\\Core\\Src\\KNpc.cpp:1698",
    "-- GHI CUNG duong dan nay va goi moi lan nguoi choi giet quai.",
    "--",
    "-- Ma that nam o script\\global\\onkillnpc.lua. Khi nao KNpc.cpp duoc sua tro",
    "-- thang sang do va CoreServer.dll duoc thay, tep nay het viec va doi di duoc.",
    "",
    "Include(\"\\\\script\\\\global\\\\onkillnpc.lua\")",
    "",
])

# 11 tep con lai cua tinhnang\datau (danhquai.lua GIU lai lam bo chuyen tiep)
DOI_TEP = ["datau.lua", "diadochi.lua", "give_item.lua", "lib_datau.lua",
           "matchi.lua", "quest_exp.lua", "quest_item.lua", "quest_lucky.lua",
           "quest_money.lua", "quest_point.lua", "quest_random.lua",
           "danhquai.lua.truoc_bdh_g5"]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def ghi_tep(p, nd, ghi, sao_luu=True):
    if not ghi:
        return True
    if sao_luu and os.path.isfile(p):
        sao = p + DUOI
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
    tm = os.path.dirname(p)
    if not os.path.isdir(tm):
        os.makedirs(tm)
    with io.open(p, "wb") as f:
        f.write(nd.encode("latin-1"))
    return doc(p) == nd


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t49 DON he Da Tau CU - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    # ---- A1 nha moi cho moc giet quai ----
    print("## A1. script/global/onkillnpc.lua (nha moi cho moc giet quai)")
    if lh.can_bang(TEP_MOI) != 0:
        print("!!! LOI TO: tep moi khong can bang (%d)" % lh.can_bang(TEP_MOI))
        return 1
    if os.path.isfile(P_MOI) and MOC in doc(P_MOI):
        print("  da co - bo qua")
    else:
        if not ghi_tep(P_MOI, TEP_MOI, ghi, sao_luu=False):
            print("!!! LOI TO: doc lai KHONG khop onkillnpc.lua")
            return 1
        print("  %d dong, can bang 0  (%s)"
              % (TEP_MOI.count("\r\n") + 1, "da ghi" if ghi else "se ghi"))
    print()

    # ---- A2 danhquai.lua -> bo chuyen tiep ----
    print("## A2. tinhnang/datau/danhquai.lua -> bo chuyen tiep")
    raw = doc(P_DQ)
    if MOC in raw:
        print("  da rut - bo qua")
    else:
        if lh.can_bang(CHUYEN_TIEP) != 0:
            print("!!! LOI TO: bo chuyen tiep khong can bang")
            return 1
        if not ghi_tep(P_DQ, CHUYEN_TIEP, ghi):
            print("!!! LOI TO: doc lai KHONG khop danhquai.lua")
            return 1
        print("  %d dong -> %d dong  (%s)"
              % (raw.count("\n") + 1, CHUYEN_TIEP.count("\r\n") + 1,
                 "da ghi" if ghi else "se ghi"))
    print()

    # ---- A3 station.lua: go nhanh du phong he cu ----
    print("## A3. global/station.lua - go nhanh du phong he Da Tau CU")
    raw = doc(P_ST)
    if MOC in raw:
        print("  da go - bo qua")
    else:
        # TACH THEO DONG LOGIC.
        # BAY: station.lua TRON kieu xuong dong (CRLF=615 nhung LF=637 -> 22
        # dong ket bang LF le). Tach bang "\r\n" thi 22 cho do bi GOP nhieu dong
        # logic vao MOT phan tu; thay/xoa phan tu do la cat lem sang dong khong
        # lien quan, va so dong bao ra cung sai (bao 338 trong khi that la 359).
        # Tach bang "\n" thi tung phan tu giu "\r" o cuoi - byte van y nguyen.
        eol = "\n"
        cb0 = lh.can_bang(raw)
        dong = raw.split(eol)
        # Do bien KHOI `if` bang can bang tu khoa - KHONG lay theo bien cua ham.
        # (Lay theo bien ham thi cat lem/thieu mot dong `end`; da vap.)
        # neo phai DUY NHAT: "GetTask(T_TIMDOCHI)" khop 2 dong (dong `if` va
        # dong `local a = ...`), nen phai lay ca phan `>= 1`.
        k = lh.tim_khoi(dong, "GetTask(T_TIMDOCHI) >= 1")
        if k is None:
            print("!!! LOI TO: khong do duoc khoi nhanh he cu")
            return 1
        i_if, b = k
        # dong chu thich ngay TREN khoi (neu co) cung thuoc nhanh cu
        i0 = i_if
        if i0 > 0 and "nhanh he CU" in dong[i0 - 1]:
            i0 = i0 - 1
        cat = eol.join(dong[i0:b + 1])
        if lh.can_bang(cat) != 0:
            print("!!! LOI TO: doan nhanh cu cat khong can bang (%d)"
                  % lh.can_bang(cat))
            return 1
        b = b + 1        # de dong[i0:b] la lat cat dung
        thut = dong[i0][:len(dong[i0]) - len(dong[i0].lstrip())]
        dong[i0:b] = [
            "%s-- %s da go nhanh du phong cua he Da Tau CU (%d dong)."
            % (thut, MOC, b - i0),
            "%s-- He do da go han; lib_ham.lua:261-267 reset MOI NGAY cac task"
            % thut,
            "%s-- T_TIMDOCHI / T_TIMMATCHI / T_DanhQuai / T_TIENDONV nen khong"
            % thut,
            "%s-- nhan vat nao ket lai. He dang chay = tasklink (nhanh o tren)."
            % thut,
            "%sTalk(1,\"\",\"%s\")" % (thut, "Kh\xf4ng nh\xe2n nhi\xd6m v\xf4"
                                       " m\xb5 d\xb8m l?a ta \xa5!"),
        ]
        nd = eol.join(dong)
        if lh.can_bang(nd) != cb0:
            print("!!! LOI TO: station.lua can bang doi (%d -> %d)"
                  % (cb0, lh.can_bang(nd)))
            return 1
        ma = lh.sach(nd)
        for t in ("DIADOCHI", "MATCHI", "DANHQUAI", "T_TIMDOCHI", "T_TIMMATCHI",
                  "T_DanhQuai"):
            if re.search(r"\b%s\b" % re.escape(t), ma):
                print("!!! LOI TO: station.lua con tham chieu %s" % t)
                return 1
        # go dong Include lib_datau
        neo = 'Include("\\\\script\\\\tinhnang\\\\datau\\\\lib_datau.lua")'
        dong = nd.split(eol)
        ci = [j for j, l in enumerate(dong)
              if neo in l and not l.lstrip().startswith("--")]
        if len(ci) != 1:
            print("!!! LOI TO: Include lib_datau khop %d dong" % len(ci))
            return 1
        j = ci[0]
        cu = dong[j]
        thut = cu[:len(cu) - len(cu.lstrip())]
        dong[j] = "%s-- %s he Da Tau CU da go%s%s-- %s" % (thut, MOC, eol,
                                                           thut, cu.strip())
        nd = eol.join(dong)
        if not ghi_tep(P_ST, nd, ghi):
            print("!!! LOI TO: doc lai KHONG khop station.lua")
            return 1
        print("  go nhanh cu dong %d..%d (%d dong) + Include dong %d  (%s)"
              % (i0 + 1, b, b - i0, j + 1, "da ghi" if ghi else "se ghi"))
    print()

    # ---- A4 doi 11 tep con lai ----
    print("## A4. doi cac tep con lai cua tinhnang/datau")
    n_ok = 0
    for t in DOI_TEP:
        duong = "tinhnang/datau/" + t
        p = os.path.join(S, duong.replace("/", os.sep))
        if not os.path.isfile(p):
            print("  %-26s khong con - bo qua" % t)
            continue
        goi = [g for g in dt.ai_goi(duong)
               if "tinhnang\\datau" not in g[0].lower()]
        if goi:
            print("  [CHAN] %-22s con %d cho goi: %s:%d"
                  % (t, len(goi), goi[0][0], goi[0][1]))
            continue
        ok, msg = dt.doi(duong, "he Da Tau CU - ban thay:"
                         " task\\newtask\\tasklink (port 15-16/08)",
                         ghi=ghi, du_biet_con_goi=True)
        print("  %-26s %s" % (t, msg))
        if ok:
            n_ok = n_ok + 1
    print("  => %d tep" % n_ok)
    print()
    if not ghi:
        print("DIEN TAP - chua lam gi. Chay lai voi --ghi de lam that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
