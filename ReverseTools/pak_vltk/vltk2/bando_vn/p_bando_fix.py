# -*- coding: utf-8 -*-
"""[BANDO20 08/09] Sua 3 loi chu bao:
  (A) bam dia diem -> chay toi GOC CHET  = thieu goc anh ban do khi doi k_Point -> MPS
  (B) dang tu chay ma bam chuot ra man hinh -> phai HUY (hien tai 2,5 s sau no chay tiep)
  (C) tro/bam bi LECH = 64% dia diem dung chung toa do + 6/7 thanh lon bi nhan bang hoi nuot chuot
Nguon TCVN3 -> doc/ghi latin-1 (RULE 0). Moi neo phai khop DUY NHAT. Chay lai duoc.
Dung: python p_bando_fix.py <goc worktree>
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_bando2"
TAG = "[BANDO20 08/09]"
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

def rd(rel):
    p = os.path.join(ROOT, rel)
    d = io.open(p, "r", encoding="latin-1", newline="").read()
    return p, d, ("\r\n" if "\r\n" in d[:4000] else "\n")

def wr(p, d):
    io.open(p, "w", encoding="latin-1", newline="").write(d)

def hb(d):
    return sum(1 for c in d if ord(c) >= 0x80)

def once(d, pat, repl, name, flags=re.M):
    ms = list(re.finditer(pat, d, flags))
    assert len(ms) == 1, "neo '%s': mong 1, thay %d" % (name, len(ms))
    m = ms[0]
    return d[:m.start()] + (repl(m) if callable(repl) else repl) + d[m.end():]

# =====================================================================
# 1) CoreShell.cpp : (A) goc anh + nNear + chan ngoai luoi ; (B) huy khi bam
# =====================================================================
p, d, E = rd(r"Sources\Core\Src\CoreShell.cpp")
h0 = hb(d)
if "[BANDO20 08/09]" in d:
    print("[=] CoreShell.cpp da sua")
else:
    # ---- (A1) BD_LoadLinks: giu Point THO ----
    d = once(d, r"^\t\t\tl\.nX = nX \* 16;\r?\n\t\t\tl\.nY = nY \* 32;\r?\n",
             lambda m: ("\t\t\t// %s k_Point la diem tren ANH tieu ban do cua RIENG map nMap (TUONG DOI goc anh)," % TAG + E +
                        "\t\t\t// goc anh khac nhau tung map va chi biet duoc khi map do dang mo => giu THO," + E +
                        "\t\t\t// doi sang MPS o TG_BanDoTick. (Truoc day nhan *16/*32 ngay tai day = THIEU goc anh.)" + E +
                        "\t\t\tl.nX = nX;" + E +
                        "\t\t\tl.nY = nY;" + E),
             "A1 BD_LoadLinks giu Point tho")
    # chu thich cu o dau struct
    d = d.replace("struct sBDLink { int nFrom; int nTo; int nX; int nY; };\t// nX/nY = MPS cua cua",
                  "struct sBDLink { int nFrom; int nTo; int nX; int nY; };\t// nX/nY = diem tren ANH tieu ban do (tuong doi goc anh map nFrom)")

    # ---- (A2)+(A3)+(A4) TG_BanDoTick ----
    NEW = (
        "\t// %s LOI \"di toi GOC CHET\": k_Point la diem tren ANH tieu ban do cua rieng map nay," % TAG + E +
        "\t// KHONG phai MPS tuyet doi. MPS = Point*(16,32) + goc anh; goc anh = m_EntireMapLTPosition" + E +
        "\t// (ScenePlaceMapC.cpp:247-276 doc MapLTRegionIndex, khong co thi lay rect, roi nhan" + E +
        "\t// RWPP_AREGION_WIDTH=512 / RWPP_AREGION_HEIGHT=1024). Thieu so hang nay thi" + E +
        "\t// KSubWorld::FindPath (KSubWorld.cpp:965-974) tinh cx/cy AM roi KEP ve 0 => muc tieu luon la" + E +
        "\t// o luoi (0,0) = goc tren-trai ban do. Guard o tren da bao dam l.nFrom == map dang dung." + E +
        "\tint nGx0 = SubWorld[0].m_nRegionBeginX * (REGION_GRID_WIDTH  * 32);\t// = *512" + E +
        "\tint nGy0 = SubWorld[0].m_nRegionBeginY * (REGION_GRID_HEIGHT * 32);\t// = *1024" + E +
        "#ifndef _SERVER" + E +
        "\t// Uu tien goc THAT cua anh: co map dat MapLTRegionIndex KHAC rect, luc do suy tu" + E +
        "\t// m_nRegionBeginX se lech. GetMapRect tra m_bHavePicMap (0 = chua co anh -> giu fallback tren)." + E +
        "\t// GetKScenePlaceMapC nam trong #ifndef _SERVER (KScenePlaceC.h:258-261) nen phai guard." + E +
        "\tRECT rcBD;" + E +
        "\tif (g_ScenePlace.GetKScenePlaceMapC()->GetMapRect(&rcBD))" + E +
        "\t{" + E +
        "\t\tnGx0 = rcBD.left;" + E +
        "\t\tnGy0 = rcBD.top;" + E +
        "\t}" + E +
        "#endif" + E +
        "\tint nX = l.nX * KScenePlaceMapC::MAP_SCALE_H + nGx0;\t// MAP_SCALE_H = 16" + E +
        "\tint nY = l.nY * KScenePlaceMapC::MAP_SCALE_V + nGy0;\t// MAP_SCALE_V = 32" + E +
        "\t// Neu du lieu MapTraffic sai thi BAO ra thay vi de FindPath kep ve goc ban do." + E +
        "\t{" + E +
        "\t\tint bx0 = 0, by0 = 0, bx1 = 0, by1 = 0;" + E +
        "\t\tif (SubWorld[0].LuoiPhamViMps(bx0, by0, bx1, by1) &&" + E +
        "\t\t\t(nX < bx0 || nX >= bx1 || nY < by0 || nY >= by1))" + E +
        "\t\t{" + E +
        "\t\t\tTG_BanDoStop(\"%s\");" % V("<color=Yellow>[Bản đồ] Cửa map ghi sai toạ độ - dừng tự chạy.") + E +
        "\t\t\treturn;" + E +
        "\t\t}" + E +
        "\t}" + E)
    d = once(d, r"^\tint nX = l\.nX, nY = l\.nY;\r?\n", NEW, "A2 doi Point sang MPS")
    # nNear 20 -> 48 (mot diem anh = 16 mps ngang / 32 doc, nhan vat dung giua o 32)
    d = once(d, r"^\tif \(DT_WalkTo\(nPlayerIdx, nX, nY, 20, uCur\)\)\r?\n",
             lambda m: ("\t// %s 20 mps < 1 o luoi (32) va < sai so lam tron cua Point (16 ngang / 32 doc)" % TAG + E +
                        "\t// => gan nhu khong bao gio \"toi noi\", nhanh do quanh cua khong chay. 48 = 1,5 o." + E +
                        "\tif (DT_WalkTo(nPlayerIdx, nX, nY, 48, uCur))" + E),
             "A3 nNear 20 -> 48")

    # ---- (B) huy khi nguoi choi bam chuot ra khung canh game ----
    d = once(d, r"^\tcase GSMOI_SCENE_MAP_REMOVE_FLAG:\r?\n",
             lambda m: (m.group(0) +
                        "\t\t// %s Op nay CHI den tu UiGame.cpp (bam chuot trai xuong khung canh game)." % TAG + E +
                        "\t\t// DT_WalkTo va TG_BanDoStop goi THANG g_ScenePlace.RemoveFlag() nen khong qua day" + E +
                        "\t\t// => dat moc huy o day khong the tu huy chinh no." + E +
                        "\t\tif (g_nBDOn)" + E +
                        "\t\t\tTG_BanDoStop(\"%s\");" % V("<color=Cyan>[Bản đồ] Ngươi tự di chuyển - đã huỷ tự chạy.") + E),
             "B moc huy GSMOI_SCENE_MAP_REMOVE_FLAG")
    wr(p, d)
    print("[+] CoreShell.cpp: (A) goc anh + nNear 48 + chan ngoai luoi ; (B) huy khi bam man hinh")
print("    CoreShell.cpp high-byte %d -> %d" % (h0, hb(d)))

# =====================================================================
# 2) UiWorldMap.h : them truong cho gop trung toa do
# =====================================================================
p, d, E = rd(r"Sources\S3Client\Ui\UiCase\UiWorldMap.h")
h0 = hb(d)
if "nGop" not in d:
    d = once(d, r"^\tchar\tszType\[16\];[^\r\n]*\r?\n",
             lambda m: m.group(0) + "\tint\t\tnGop;\t\t\t// %s so map khac dung chung dung toa do nay (gop lam 1 diem)%s" % (TAG, E),
             "them nGop")
    wr(p, d)
    print("[+] UiWorldMap.h: truong nGop")
else:
    print("[=] UiWorldMap.h da sua")
assert hb(d) == h0

# =====================================================================
# 3) UiWorldMap.cpp : (C) gop trung toa do + ban kinh + nhan bang hoi khong nuot chuot
# =====================================================================
p, d, E = rd(r"Sources\S3Client\Ui\UiCase\UiWorldMap.cpp")
h0 = hb(d)
if "WML_UuTien" in d:
    print("[=] UiWorldMap.cpp da sua")
else:
    # --- (C1) bang uu tien + gop trung toa do trong Load ---
    UUTIEN = (
        "// %s Do dai dia diem: MapList.ini chi co 97 toa do KHAC NHAU cho 222 map -" % TAG + E +
        "// 142 map (64%) dung chung toa do voi map khac, rieng (452,314) co 82 map (chien truong /" + E +
        "// diem bao danh Tong Kim). Neu khong gop thi tro chuot va bam luon ra map DAU TIEN theo id," + E +
        "// khac han cai nguoi choi nham => chu game thay \"bam bi lech\". Gop lai 1 diem, giu map co" + E +
        "// y nghia nhat de di toi (thanh/kinh do truoc, chien truong / khac sau cung)." + E +
        "static int WML_UuTien(const char* szType)" + E +
        "{" + E +
        "\tstatic const char* aTu[] = { \"Capital\", \"City\", \"Tong\", \"Field\", \"Cave\", \"Country\", \"Battlefield\", \"Others\" };" + E +
        "\tfor (int i = 0; i < (int)(sizeof(aTu) / sizeof(aTu[0])); i++)" + E +
        "\t\tif (stricmp(szType, aTu[i]) == 0)" + E +
        "\t\t\treturn i;" + E +
        "\treturn 99;" + E +
        "}" + E + E)
    d = once(d, r"^static const char\* WML_LoaiTen\(const char\* szType\)\r?\n", lambda m: UUTIEN + m.group(0), "chen WML_UuTien")

    # trong Load: thay khoi ket thuc mot muc bang khoi co gop
    OLD = (r"\t\tsprintf\(szKey, \"%d_MapType\", i\);\r?\n"
           r"\t\tl\.szType\[0\] = 0;\r?\n"
           r"\t\tpIni->GetString\(\"List\", szKey, \"\", l\.szType, sizeof\(l\.szType\)\);\r?\n"
           r"\t\tm_nCount\+\+;\r?\n")
    NEW = ("\t\tsprintf(szKey, \"%d_MapType\", i);" + E +
           "\t\tl.szType[0] = 0;" + E +
           "\t\tpIni->GetString(\"List\", szKey, \"\", l.szType, sizeof(l.szType));" + E +
           "\t\tl.nGop = 0;" + E +
           "\t\t// %s gop map trung toa do: giu cai uu tien cao hon, dem so cai bi gop" % TAG + E +
           "\t\tint nTrung = -1;" + E +
           "\t\tfor (int k = 0; k < m_nCount; k++)" + E +
           "\t\t{" + E +
           "\t\t\tif (m_aLoc[k].nX == l.nX && m_aLoc[k].nY == l.nY)" + E +
           "\t\t\t{" + E +
           "\t\t\t\tnTrung = k;" + E +
           "\t\t\t\tbreak;" + E +
           "\t\t\t}" + E +
           "\t\t}" + E +
           "\t\tif (nTrung >= 0)" + E +
           "\t\t{" + E +
           "\t\t\tm_aLoc[nTrung].nGop++;" + E +
           "\t\t\tif (WML_UuTien(l.szType) < WML_UuTien(m_aLoc[nTrung].szType))" + E +
           "\t\t\t{" + E +
           "\t\t\t\tint nGiu = m_aLoc[nTrung].nGop;" + E +
           "\t\t\t\tm_aLoc[nTrung] = l;" + E +
           "\t\t\t\tm_aLoc[nTrung].nGop = nGiu;" + E +
           "\t\t\t}" + E +
           "\t\t\tcontinue;" + E +
           "\t\t}" + E +
           "\t\tm_nCount++;" + E)
    d = once(d, OLD, NEW, "Load gop trung toa do")

    # --- (C3) ban kinh 14 -> 24 ---
    d = once(d, r"^\tint nBest = -1, nBestD = 14 \* 14 \+ 1;\r?\n",
             lambda m: ("\t// %s 14 px nho hon chu ten thanh ve san tren anh (24-74 px) -> rat kho tro trung." % TAG + E +
                        "\tint nBest = -1, nBestD = 24 * 24 + 1;" + E),
             "C3 ban kinh 24")

    # --- (C2) nhan bang hoi khong nuot chuot: chuyen tiep WND_N_BUTTON_CLICK ---
    d = once(d, r"^\tcase WM_LBUTTONDOWN:\t// \[BANDO20 06/09\][^\r\n]*\r?\n",
             lambda m: ("\tcase WND_N_BUTTON_CLICK:\t// %s 7 nhan \"Bang hoi chiem linh\" (KWndPureTextBtn) NAM DE" % TAG + E +
                        "\t\t// len 6/7 thanh lon va NUOT chuot trai (TopChildFromPoint chon con truoc cha)," + E +
                        "\t\t// nen bam Phuong Tuong / Thanh Do / Bien Kinh / Duong Chau / Lam An / Dai Ly" + E +
                        "\t\t// truoc day KHONG bao gio toi duoc ban do. Nhan bao click thi xu ly y het." + E +
                        m.group(0)),
             "C2 chuyen tiep WND_N_BUTTON_CLICK")
    wr(p, d)
    print("[+] UiWorldMap.cpp: (C) gop trung toa do + ban kinh 24 + nhan bang hoi khong nuot chuot")
print("    UiWorldMap.cpp high-byte %d -> %d" % (h0, hb(d)))

# =====================================================================
# 4) UiMapCave.cpp : cung duong bam (khong co nhan bang hoi nen chi can ban kinh/gop dung chung lop)
# =====================================================================
p, d, E = rd(r"Sources\S3Client\Ui\UiCase\UiMapCave.cpp")
h0 = hb(d)
print("    UiMapCave.cpp: dung chung lop KWorldMapLocs -> tu huong (C1)(C3), khong can sua rieng (high-byte %d)" % h0)
print("XONG.")
