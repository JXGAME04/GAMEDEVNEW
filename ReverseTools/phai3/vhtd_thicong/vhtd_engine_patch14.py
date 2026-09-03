# -*- coding: utf-8 -*-
# vhtd_engine_patch14.py [VHTD 02/09y]
# SUA HAI CHO SAI TRONG TOOLTIP KY NANG TU PHONG (khoi [VHTD 02/09i], KSkills.cpp VhtdAutoSkillDesc).
# Ca hai deu la CHU HIEN THI, khong dung toi co che. Chi anh huong CLIENT.
#
# ---------------------------------------------------------------------------
# SAI 1 - "Don danh co 10% ty le xuat ..." la NGUOC NGHIA voi autoreplyskill.
#
# Khoi hien tai gom NAM loai thuoc tinh tu phong nhung chi tach rieng cau chu cho autocastskill,
# autorescueskill va autodeathskill; autoreplyskill VA autoattackskill dung CHUNG nhanh "else" =
# "Don danh co R% ty le xuat TEN". Nhung hai loai nay NGUOC NHAU (KNpc.cpp:4835-4837, trong ReceiveDamage):
#     4835:  ReplySkill(nLauncher);            <- NAN NHAN quay so  => autoreplyskill = KHI BI DANH
#     4837:  Npc[nLauncher].AttackSkill(...);  <- KE DANH quay so   => autoattackskill = KHI DANH RA
# ReplySkill chi co DUNG MOT noi goi, va no nam trong duong nhan sat thuong.
#
# Hau qua: 1364 Doat Menh Lien Hoan Tam Tien Kiem (autoreplyskill) hien "Don danh co 10%...", chu dem
# so don MINH VUNG RA roi cho 10% nen thay nhu hong. Thuc te phai BI DANH moi co luot quay.
# Ban VLTK ghi dung: MagicDesc.ini muc autoreplyskill = "Khi bi cong kich, lay ...% ty le tu dong phong thich".
#
# VA: them nhanh rieng cho magic_autoreplyskill. KHONG bo khoi gõ cung nay de tra ve chuoi MagicDesc,
# vi chinh khoi nay duoc them o dot 02/09i sau khi chu che cau MagicDesc cu "chung chung".
#
# ---------------------------------------------------------------------------
# SAI 2 - "So luong kiem xuat ra: 3/9 kiem" - mau so 9 la GÕ CUNG cho CA HAI ky nang.
#
#     if (nSkillId == 1363 || nSkillId == 1368)
#         sprintf(..., "So luong kiem xuat ra: %d/9 kiem\n", nNum);
#
# Do lai du lieu dang chay:
#   1368 Doc Co Cuu Kiem  : huashan.lua:340 skill_misslenum_v={{{1,4},{20,9},{21,9}}} -> THAT SU len 4..9 kiem.
#                           "4/9".."9/9" la DUNG. (Ban VLTK dong 339 y het.)
#   1363 Thai Nhac Tam Thanh Phong : KHONG co skill_misslenum_v (ca ban ta lan ban VLTK) -> CO DINH 3 kiem
#                           o moi cap. Mau so 9 la BIA -> chu tuong dang thieu 6 kiem.
# VA: giu "/9" rieng cho 1368; 1363 in so kiem tran.
#
# ---------------------------------------------------------------------------
# CHU VIET: TUYET DOI khong tu go. Moi chuoi moi deu GHEP TU BYTE CO SAN:
#   - "Khi bi cong kich" lay tu MagicDesc.ini (muc autoreplyskill) cua chinh cay dang chay.
#   - Phan con lai lay nguyen tu chinh dong "Don danh co ..." dang co trong tep.
#   - Dong 1363 lay nguyen dong "/9" roi bo "/9".
# Doc/ghi latin-1, ton trong kieu xuong dong, kiem so byte cao truoc/sau.
#
# DUNG: python vhtd_engine_patch14.py --kiem | (khong co)
import io
import sys

KIEM = "--kiem" in sys.argv
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

MK = "[VHTD 02/09y]"
P = "D:/GAMEDEVNEW/Sources/Core/Src/KSkills.cpp"
INI = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/settings/MagicDesc.ini"
T = chr(9)


MONG_DOI = "Khi b\u1ecb c\u00f4ng k\u00edch"   # "Khi bi cong kich" co dau, de TU KIEM sau khi rut byte


def bang_tcvn3():
    import importlib.util
    sp = importlib.util.spec_from_file_location(
        "vn", r"C:/Users/nguye/.claude/skills/swordonline-dev/scripts/vn_edit.py")
    m = importlib.util.module_from_spec(sp)
    sp.loader.exec_module(m)
    return getattr(m, "_TCVN3_TO_UNICODE")


def giai_ma(s, tbl):
    ra = []
    for c in s:
        k = ord(c)
        ra.append(tbl.get(k, tbl.get(chr(k), c)) if k >= 0x80 else c)
    return "".join(ra)


def lay_khi_bi_cong_kich():
    """Rut cum 'Khi bi cong kich' (TCVN3) tu MagicDesc.ini - KHONG tu go chu Viet.

    LUU Y: chinh du lieu goc VIET SAI CHINH TA - byte thu 3 la 0xDE = 'i nang' nen doc ra
    "Khi bi cong kich" (sai). Sua dung MOT byte do ve chu 'i' ASCII roi TU KIEM bang bang TCVN3.
    """
    for ln in io.open(INI, "r", encoding="latin-1", newline="").read().split("\n"):
        if ln.startswith("autoreplyskill="):
            v = ln.split("=", 1)[1]
            i = v.find(",")
            if i <= 0:
                raise SystemExit("  [X] MagicDesc.ini autoreplyskill khong co dau phay")
            cum = v[:i].strip()
            tbl = bang_tcvn3()
            if giai_ma(cum, tbl) != MONG_DOI:
                # sua loi chinh ta cua du lieu goc: ky tu thu 3 phai la 'i' tran
                cum = cum[:2] + "i" + cum[3:]
                if giai_ma(cum, tbl) != MONG_DOI:
                    raise SystemExit("  [X] giai ma ra %r, mong doi %r" % (giai_ma(cum, tbl), MONG_DOI))
                print("  [!] du lieu goc sai chinh ta (\"Kh\u1ecb\") - da sua 1 byte thanh \"Khi\"")
            return cum
    raise SystemExit("  [X] khong thay muc autoreplyskill trong " + INI)


def main():
    b = io.open(P, "rb").read()
    cao_truoc = sum(1 for c in b if c >= 0x80)
    s = b.decode("latin-1")
    nl = "\r\n" if s.count("\r\n") * 2 > s.count("\n") else "\n"
    lines = s.split(nl)

    if any(MK in x for x in lines):
        print("  [=] KSkills.cpp da co " + MK)
        return

    # ---------- SAI 1 ----------
    hit = [i for i, x in enumerate(lines) if "ty le xuat" in x.lower() or ("%d%%" in x and "%s" in x and "sprintf(szLine" in x)]
    # neo chinh xac: nhanh else cuoi cung cua chuoi if/else if
    idx = [i for i, x in enumerate(lines) if x.strip() == "else" and i + 1 < len(lines)
           and "sprintf(szLine," in lines[i + 1] and "%d%%" in lines[i + 1] and "%s" in lines[i + 1]]
    if len(idx) != 1:
        raise SystemExit("  [X] neo nhanh 'else' cuoi khop %d cho (can 1)" % len(idx))
    i_else = idx[0]
    dong_don_danh = lines[i_else + 1]

    # tach phan chuoi trong dau nhay kep cua dong "Don danh co ..."
    a = dong_don_danh.find('"')
    z = dong_don_danh.find('"', a + 1)
    if a < 0 or z < 0:
        raise SystemExit("  [X] khong tach duoc chuoi cua dong 'Don danh'")
    chuoi_cu = dong_don_danh[a + 1:z]
    k = chuoi_cu.find(" c")          # vi tri " co " (chu 'co' co dau -> chi khop khoang trang + 'c')
    if k <= 0:
        raise SystemExit("  [X] khong tim duoc cum ' co ' trong %r" % chuoi_cu[:40])
    duoi = chuoi_cu[k:]              # " co <color=orange>%d%%<color> ty le xuat <color=blue>%s<color>\n"
    chuoi_moi = lay_khi_bi_cong_kich() + duoi

    them = [
        T + "// " + MK + " autoreplyskill NGUOC nghia voi autoattackskill: KNpc.cpp:4835 ReplySkill do NAN NHAN quay so",
        T + "// (trong ReceiveDamage), con 4837 AttackSkill do KE DANH quay so. Truoc gio hai loai dung chung cau",
        T + '// "Don danh co R%% ty le xuat ..." -> 1364 Doat Menh hien nguoc nghia, chu dem don MINH VUNG RA nen tuong hong.',
        T + "// Chuoi duoi ghep tu byte co san cua MagicDesc.ini (muc autoreplyskill, ban VLTK) - khong tu go chu Viet.",
        T + "else if (pA->nAttribType == magic_autoreplyskill)",
        T * 2 + 'sprintf(szLine, "' + chuoi_moi + '", nRate, pEv->GetSkillName());',
    ]
    lines[i_else:i_else] = them
    print("  [+] SAI 1: them nhanh autoreplyskill -> %r" % (chuoi_moi[:34] + "..."))

    # ---------- SAI 2 ----------
    j = [i for i, x in enumerate(lines) if "nSkillId == 1363 || nSkillId == 1368" in x]
    if len(j) != 1:
        raise SystemExit("  [X] neo '1363 || 1368' khop %d cho (can 1)" % len(j))
    j = j[0]
    dong_9 = lines[j + 1]
    if "%d/9" not in dong_9:
        raise SystemExit("  [X] dong sau neo khong chua %%d/9: %r" % dong_9)
    lines[j] = T * 2 + "if (nSkillId == 1368)" + T + "// " + MK + " chi 1368 moi THAT SU len 4..9 kiem (huashan.lua:340 skill_misslenum_v)"
    lines[j + 2:j + 2] = [
        T * 2 + "else if (nSkillId == 1363)" + T + "// 1363 KHONG co skill_misslenum_v (ca ban ta lan VLTK) -> co dinh 3 kiem, mau so 9 la bia",
        dong_9.replace("%d/9", "%d"),
    ]
    print("  [+] SAI 2: '/9' chi con cho 1368; 1363 in so kiem tran")

    out = nl.join(lines)
    nb = out.encode("latin-1")
    cao = sum(1 for c in nb if c >= 0x80)
    if cao <= cao_truoc:
        raise SystemExit("  [X] byte cao khong tang (%d -> %d) - chuoi Viet moi khong duoc chen?" % (cao_truoc, cao))
    if b"\xef\xbf\xbd" in nb:
        raise SystemExit("  [X] co EF BF BD")
    print("  [.] byte cao %d -> %d (tang do them chuoi Viet ghep tu byte co san)" % (cao_truoc, cao))
    if not KIEM:
        io.open(P, "wb").write(nb)
    print("  => %s %s" % ("KIEM" if KIEM else "GHI", P))


if __name__ == "__main__":
    print("vhtd_engine_patch14 %s%s" % (MK, " (KIEM)" if KIEM else ""))
    main()
    print("XONG. Chi CLIENT -> build CoreClient (Win32), gop chung vao CoreClient.dll.moi dang cho swap.")
