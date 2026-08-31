# -*- coding: utf-8 -*-
"""t43_go_he_cu_dot6.py - DOT 6: doi 4 THU MUC he cu ra khoi cay script.

Lam theo BAC vi chung phu thuoc nhau:

  bac 1  doi tinhnang\\vuot_ai            (0 cho goi sau DOT 1-5)
         -> keo theo sugiasatthu.lua, tep duy nhat con Include lib_boss_st
  bac 2  go Include startgame.lua:43 "\\script\\startgame\\khac\\satthu.lua"
         roi doi chinh tep do
         (176 dong; addnpcsatthu() KHONG ai goi - ca 2 cho goi deu da comment:
          startgame.lua:103 va :210. Tep chi con khai DROPFILEST/DEATHFILEST
          tro vao boss_satthu)
  bac 3  doi tinhnang\\boss_satthu        (het cho goi sau bac 1+2)
  bac 4  doi tinhnang\\phonglangdo
         Con 2 chuoi trong hd3_driver.lua:49-50, nhung do la DANH SACH TEN
         SCRIPT DE XOA NPC (HD3_DelNpcByScript). Thu muc di roi thi khong NPC
         nao mang script do nua -> loi goi thanh vo hai. GIU NGUYEN hd3_driver
         (ma cua he dang song, khong dung toi).
  bac 5  go Include congthanhquan.lua:9 (tep MO COI - ca 3 cho tao NPC 'Cong
         thanh quan' deu da comment: balanghuyen.lua:79, lib_ctc.lua:243-244)
         roi doi tinhnang\\loidai

KHONG DOI (co ly do cung):
  tinhnang\\datau            KNpc.cpp:1698 goi cung danhquai.lua MOI LAN giet
                            quai; tep do nay con mang moc dem giet quai cua
                            BAN DONG HANH (danhquai.lua:5,20 BDH_OnKillNpc).
  tinhnang\\congthanhchien   KProtocolProcess.cpp:6874 goi cung ghilog_tax.lua;
                            thu muc con chua danhsach_bang.lua / mapbanghoi.lua
                            / log_tax.lua / quany.lua / ruongchua.lua la DU LIEU
                            va NPC dung chung voi ban JX2.
  tinhnang\\loidaihonchien   Khong co ban Linux thay the - bo la MAT hoat dong.

Mac dinh DIEN TAP; --ghi moi lam that.
"""
import io
import os
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import doi_tep as dt  # noqa: E402
import lua_ham as lh  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

MOC = "[GOHECU 30/08]"
DUOI = ".truoc_gohecu"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def comment_include(duong, neo, ly_do, ghi):
    """Comment mot dong Include. Tra ve (ok, thong_diep)."""
    p = os.path.join(dt.S, duong.replace("/", os.sep))
    raw = doc(p)
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                          - raw.count("\r\n")) else "\n"
    dong = raw.split(eol)
    ci = [i for i, l in enumerate(dong)
          if neo in l and not l.lstrip().startswith("--")]
    if not ci:
        return True, "da comment tu truoc"
    if len(ci) != 1:
        return False, "khop %d dong (can 1)" % len(ci)
    i = ci[0]
    cu = dong[i]
    thut = cu[:len(cu) - len(cu.lstrip())]
    dong[i] = "%s-- %s %s%s%s-- %s" % (thut, MOC, ly_do, eol, thut, cu.strip())
    nd = eol.join(dong)
    if lh.can_bang(nd) != lh.can_bang(raw):
        return False, "can bang tu khoa Lua doi"
    if sum(1 for c in nd if ord(c) > 127) != sum(1 for c in raw if ord(c) > 127):
        return False, "byte tieng Viet doi"
    if ghi:
        sao = p + DUOI
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(p) != nd:
            return False, "doc lai KHONG khop"
        return True, "da comment dong %d" % (i + 1)
    return True, "se comment dong %d" % (i + 1)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t43 DOT 6: doi 4 thu muc he cu - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    tong_tep = tong_dong = 0

    # Tep ma chinh dot nay vua comment Include. Trong DIEN TAP chua ghi ra dia
    # nen ai_goi van thay dong cu -> phai tru ra, khong thi dien tap bao chan
    # nham va khong kiem duoc gi.
    # Chua cac tep/thu muc chinh dot nay DA XU LY (comment Include, hoac da doi
    # o bac truoc). Trong DIEN TAP chua ghi ra dia nen ai_goi van thay chung ->
    # phai tru ra, khong thi cac bac sau deu bao chan nham.
    da_comment = []

    def _bo_qua(duong_goi):
        c = dt._chuan(duong_goi)
        for x in da_comment:
            if c == x or c.startswith(x + "\\"):
                return True
        return False

    def doi(duong, ly_do, du_biet=False):
        nonlocal tong_tep, tong_dong
        n_tep, n_dong = dt.dem(duong)
        goi = [g for g in dt.ai_goi(duong) if not _bo_qua(g[0])]
        if goi and not du_biet:
            print("  [CHAN] %-26s CON %d CHO GOI:" % (duong, len(goi)))
            for g in goi[:6]:
                print("         %s:%d | %s" % g)
            return False
        # Toi vua tu kiem o tren voi bo loc CHINH XAC HON (tru cac tep chinh
        # dot nay comment). Den day la da qua chot, nen bao dt.doi dung chan
        # nua - no van ghi day du cho goi vao _dara\NHATKY_DOI.txt.
        ok, msg = dt.doi(duong, ly_do, ghi=ghi, du_biet_con_goi=True)
        print("  %-28s %2d tep %5d dong  %s" % (duong, n_tep, n_dong, msg))
        if ok:
            tong_tep = tong_tep + n_tep
            tong_dong = tong_dong + n_dong
            da_comment.append(dt._chuan("script/" + duong))
        return ok

    print("## bac 1: tinhnang/vuot_ai")
    if not doi("tinhnang/vuot_ai",
               "Vuot Ai ban Viet - ban thay: missions\\challengeoftime"
               " (Linux, BAT_HD3 = 1)"):
        return 1
    print()

    print("## bac 2: startgame/khac/satthu.lua")
    da_comment.append(dt._chuan("script/startgame.lua"))
    ok, msg = comment_include(
        "startgame.lua",
        'Include("\\\\script\\\\startgame\\\\khac\\\\satthu.lua")',
        "he Boss Sat Thu ban Viet da go; addnpcsatthu() khong ai goi"
        " (startgame.lua:103 va :210 deu da comment)", ghi)
    print("  startgame.lua: %s" % msg)
    if not ok:
        return 1
    if not doi("startgame/khac/satthu.lua",
               "bang NPC Boss Sat Thu ban Viet - ban thay:"
               " task\\tollgate\\killer + killbosshead.lua (Linux)"):
        return 1
    print()

    print("## bac 3: tinhnang/boss_satthu")
    if not doi("tinhnang/boss_satthu",
               "Boss Sat Thu ban Viet - ban thay: task\\tollgate\\killer"
               " (Linux, BAT_HD3 = 1)"):
        return 1
    print()

    print("## bac 4: tinhnang/phonglangdo")
    if not doi("tinhnang/phonglangdo",
               "Phong Lang Do ban Viet - ban thay: missions\\fengling_ferry"
               " (Linux, BAT_HD3 = 1). Hai chuoi con lai o hd3_driver.lua:49-50"
               " chi la ten script de XOA NPC cu - thu muc di roi thi thanh"
               " vo hai", du_biet=True):
        return 1
    print()

    print("## bac 5: tinhnang/loidai")
    da_comment.append(dt._chuan(
        "script/tinhnang/congthanhchien/congthanhquan.lua"))
    ok, msg = comment_include(
        "tinhnang/congthanhchien/congthanhquan.lua",
        'Include("\\\\script\\\\tinhnang\\\\loidai\\\\lib_loidai.lua")',
        "tep nay MO COI: ca 3 cho tao NPC 'Cong thanh quan' deu da comment"
        " (balanghuyen.lua:79, lib_ctc.lua:243-244)", ghi)
    print("  congthanhquan.lua: %s" % msg)
    if not ok:
        return 1
    if not doi("tinhnang/loidai",
               "Loi Dai bang hoi ban Viet - ban thay: missions\\citywar_arena"
               " (JX2, BAT_CTC_JX2 = 1)"):
        return 1
    print()

    print("=> %d tep, %d dong" % (tong_tep, tong_dong))
    if not ghi:
        print()
        print("DIEN TAP - chua doi gi. Chay lai voi --ghi de lam that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
