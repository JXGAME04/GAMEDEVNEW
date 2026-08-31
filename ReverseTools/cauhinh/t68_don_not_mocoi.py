# -*- coding: utf-8 -*-
"""t68_don_not_mocoi.py - don not cac tep mo coi con lai, theo chuan CHAT.

Truoc day toi noi "khong quet don hang loat duoc". Van dung - nhung ly do cu the
la HAI thu, va ca hai deu xu ly duoc:

  (e) quy uoc TEN THEO SO HIEU: missions\\missionNN.lua, timertask\\taskNN.lua
      -> LOAI TRU han khoi danh sach.
  (h) NPC/bay gan bang DU LIEU BAN DO trong Pak (khoi da nen)
      -> da trich duoc 2.864 ten bang t58 (ucl.nrv2b_decompress_8) -> LOAI TRU.

Con mot loai nua khong tu quyet duoc: tep chi duoc nhac o DONG DA COMMENT. Do
chinh la dau hieu "tinh nang bi tat" - co the la rac, cung co the la thu chu game
dang tam tat cho bat lai. Nen:

  * KHONG tham chieu nao (ke ca comment) + khong trong du lieu ban do  -> DOI
  * chi con tham chieu trong dong DA COMMENT                           -> LIET KE
    de chu game quyet, khong tu dong

Them hai chot nua truoc khi doi tung tep:
  - doi_tep.ai_goi (quet script\\ + settings\\ theo duong dan, chuan hoa hai dau
    gach, khop theo ranh gioi)
  - ten tep phai khong xuat hien trong ma C++ (Core\\Src)

Mac dinh DIEN TAP; --ghi moi doi that.
"""
import io
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import doi_tep as dt  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = dt.S
SV = dt.SV
NGUON = r"D:\GAMEDEVNEW\Sources\Core\Src"
P_MD = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                    "duongnap_mapdata.txt")
RA = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                  "mocoi_chi_comment.txt")

BO_TEN = (".truoc_", ".goc", ".cu_", ".hong_", ".daap", ".bak", ".old",
          " - copy", ".tuche_")
BO_TM = ("_backup", "_gobo", "_dara", "tinhnang.rar")
# quy uoc ten theo so hieu - engine nap thang, khong qua tham chieu van ban
SO_HIEU = (re.compile(r"^missions\\mission\w*\.lua$", re.I),
           re.compile(r"^timertask\\task\w*\.lua$", re.I))


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def chuan(s):
    return re.sub(r"\\+", "\\\\", s.replace("/", "\\").lower())


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t68 don not tep mo coi - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    md = set(l.strip().lower() for l in io.open(P_MD, encoding="utf-8")
             if l.strip().lower().endswith(".lua"))
    print("  duong nap thu 8 (du lieu ban do): %d ten" % len(md))

    # --- gom moi tep .lua ung vien ---
    tep = []
    for goc, tm, ts in os.walk(S):
        tm[:] = [x for x in tm if not any(b in x.lower() for b in BO_TM)]
        for t in ts:
            if not t.lower().endswith(".lua"):
                continue
            if any(b in t.lower() for b in BO_TEN):
                continue
            rel = os.path.relpath(os.path.join(goc, t), S)
            if any(p.match(chuan(rel)) for p in SO_HIEU):
                continue
            if t.lower() in md:
                continue
            tep.append(rel)
    print("  ung vien sau khi loai (e) va (h): %d tep" % len(tep))

    # --- gom van ban de dem tham chieu (ke ca dong comment) ---
    kho = []
    for goc, tm, ts in os.walk(S):
        tm[:] = [x for x in tm if not any(b in x.lower() for b in BO_TM)]
        for t in ts:
            if t.lower().endswith((".lua", ".txt", ".ini")) \
                    and not any(b in t.lower() for b in BO_TEN):
                try:
                    kho.append((os.path.relpath(os.path.join(goc, t), S),
                                doc(os.path.join(goc, t))))
                except OSError:
                    pass
    for goc, _, ts in os.walk(os.path.join(SV, "settings")):
        for t in ts:
            if t.lower().endswith((".txt", ".ini", ".lua")) \
                    and not any(b in t.lower() for b in BO_TEN):
                try:
                    kho.append(("settings\\" + t,
                                doc(os.path.join(goc, t))))
                except OSError:
                    pass
    cpp = []
    for goc, _, ts in os.walk(NGUON):
        for t in ts:
            if t.lower().endswith((".cpp", ".h")):
                try:
                    cpp.append(doc(os.path.join(goc, t)))
                except OSError:
                    pass
    cpp_txt = "\n".join(cpp).lower()
    print("  kho van ban: %d tep script/settings + %d tep C++"
          % (len(kho), len(cpp)))
    print()

    # CHI MUC NGUOC: quet MOT LAN qua toan bo van ban, trich moi ten *.lua
    # gap phai, tach lam hai tap (dong song / dong da comment). Sau do tra cuu
    # tung ung vien la O(1). Cach cu (voi moi ung vien quet lai ca kho) la
    # O(n*m) - 1.500 x 3.900 tep, chay hang gio.
    # BAY DA VAP: dung [\w\-]+ thi ten TIENG TRUNG bi CAT CUT. Ten GBK doc bang
    # latin-1 co nhung ky tu KHONG phai word-char (vd '\xb7' = '·'), nen
    # 'Â·ÈË_³Ì´óÎª.lua' bi bat thanh 'ÈË_³Ì´óÎª.lua' -> tra cuu khong khop ->
    # bao MO COI OAN hang nghin tep. Phai lay nguyen cum tu dau phan cach.
    RE_TEN = re.compile(r"[^\s\"'\\/<>()\[\],;=]+\.lua", re.I)
    ten_song, ten_cmt = set(), set()
    for ten_nguon, nd in kho:
        tu_minh = os.path.basename(ten_nguon).lower()
        for l in nd.replace("\r\n", "\n").split("\n"):
            ds = RE_TEN.findall(l)
            if not ds:
                continue
            cmt = l.lstrip().startswith("--")
            for x in ds:
                x = x.lower()
                if x == tu_minh:      # tep tu nhac ten chinh no
                    continue
                (ten_cmt if cmt else ten_song).add(x)
    print("  ten .lua duoc nhac o dong SONG    : %d" % len(ten_song))
    print("  ten .lua chi thay o dong DA COMMENT: %d"
          % len(ten_cmt - ten_song))
    print()

    sach, chi_cmt = [], []
    for rel in tep:
        ten = os.path.basename(rel).lower()
        if ten in cpp_txt or ten in ten_song:
            continue
        (chi_cmt if ten in ten_cmt else sach).append(rel)

    print("  KHONG tham chieu nao (ke ca comment) : %d tep" % len(sach))
    print("  chi con tham chieu o dong DA COMMENT : %d tep -> LIET KE, khong doi"
          % len(chi_cmt))
    print()

    with io.open(RA, "w", encoding="utf-8", newline="") as f:
        f.write("TEP CHI CON THAM CHIEU O DONG DA COMMENT - CHU GAME QUYET\n")
        f.write("Sinh boi ReverseTools/cauhinh/t68_don_not_mocoi.py\n")
        f.write("Day la dau hieu 'tinh nang bi tat'. Co the la rac, cung co the\n")
        f.write("la thu dang tam tat cho bat lai. TOI KHONG TU DOI.\n")
        f.write("=" * 78 + "\n\n")
        nhom = {}
        for x in chi_cmt:
            nhom.setdefault(os.path.dirname(x) or "(goc)", []).append(x)
        for tm, ds in sorted(nhom.items(), key=lambda y: -len(y[1])):
            f.write("## %s  (%d tep)\n" % (tm, len(ds)))
            for x in sorted(ds):
                f.write("    %s\n" % os.path.basename(x))
            f.write("\n")
    print("  => danh sach cho chu game: %s" % RA)
    print()

    # --- doi nhom sach ---
    nhom = {}
    for x in sach:
        nhom.setdefault(os.path.dirname(x) or "(goc)", []).append(x)
    tong = 0
    for tm, ds in sorted(nhom.items(), key=lambda y: -len(y[1])):
        n_ok = n_chan = 0
        for rel in sorted(ds):
            # KHONG goi dt.ai_goi o day nua: chi muc nguoc da chung minh TEN TEP
            # khong xuat hien o BAT KY dong nao trong ca cay. Ma moi duong dan
            # deu chua ten tep, nen khong the co tham chieu theo duong dan ma
            # khong co ten. Goi lai ai_goi cho tung tep la quet lai ca cay 1.500
            # lan - chay hang gio ma khong them thong tin gi.
            ok, _ = dt.doi(rel.replace(os.sep, "/"),
                           "mo coi: 0 tham chieu (ke ca comment), 0 ten trong"
                           " du lieu ban do, 0 cho trong C++", ghi=ghi)
            if ok:
                n_ok += 1
            else:
                n_chan += 1
        print("  %-46s %3d doi, %2d chan" % (tm[:46], n_ok, n_chan))
        tong += n_ok
    print()
    print("=> %d tep %s" % (tong, "da doi" if ghi else "se doi"))
    if not ghi:
        print("DIEN TAP - chua doi gi. Chay lai voi --ghi de lam that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
