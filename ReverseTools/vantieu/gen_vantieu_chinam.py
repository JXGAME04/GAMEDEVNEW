# -*- coding: utf-8 -*-
"""SINH TU DONG hai header cho muc VAN TIEU tren bang Chi nam nhiem vu (F11) - DUNG SUA TAY:
  1) Sources/Core/Src/KVanTieuPos.h              (so: mau NPC, toa do 44 diem, 49 tuyen)
  2) Sources/S3Client/Ui/UiCase/UiTaskGuideVanTieu.h (chuoi TCVN3 tho: ten diem + toan bo bai huong dan)
Nguon so lieu = cay may chu dang chay:
  script/activitysys/config/129/extend.lua  (pActivity.tbBJPathLevel, pActivity.tbBJPoints)
  script/activitysys/config/129/award.lua   (tbAllCountCell, tbAward[*][1] nExp)
  script/startgame/lmbj_addnpc.lua          (LMBJ_POS_ZHANGGUI)
Chay lai khi cac tep tren doi:  python ReverseTools/vantieu/gen_vantieu_chinam.py
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
OUT_CORE = os.path.join(ROOT, "Sources", "Core", "Src", "KVanTieuPos.h")
OUT_UI = os.path.join(ROOT, "Sources", "S3Client", "Ui", "UiCase", "UiTaskGuideVanTieu.h")

def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()

def V(u):
    """unicode -> latin-1 str mang byte TCVN3; tu dong chen dau cach truoc '<' neu ky tu truoc la chu Viet
    (TEncodeText coi byte cao la nua dau cua chu Han 2 byte va nuot '<' - RULE 0 cua skill)."""
    out = []
    for i, ch in enumerate(u):
        if ch == "<" and i > 0 and ord(u[i - 1]) >= 0x80:
            out.append(" ")
        out.append(ch)
    return unicode_to_tcvn3_bytes("".join(out)).decode("latin-1")

def clit(s_latin1):
    """noi dung chuoi C: giu byte cao tho (source-charset = execution-charset windows-1258), thoat \\ va \" """
    return '"' + s_latin1.replace("\\", "\\\\").replace('"', '\\"') + '"'

# ---------------- extend.lua ----------------
# [SAPXEP 06/09] cay script da sap xep lai: activitysys nam duoi script\tinhnang\ (duong cu con bi danh)
CFG129 = os.path.join(SRV, r"script\tinhnang\activitysys\config\129")
if not os.path.isdir(CFG129):
    CFG129 = os.path.join(SRV, r"script\activitysys\config\129")
ext = rd(os.path.join(CFG129, "extend.lua"))
i0 = ext.index("pActivity.tbBJPathLevel = {")
i1 = ext.index("pActivity.tbAllTask", i0)
tuyen = {}
for m in re.finditer(r"\[(\d+)\]\s*=\s*\{\{\s*(\d+),\s*(\d+)\},\s*(\d+),\}", ext[i0:i1]):
    tuyen[int(m.group(1))] = (int(m.group(2)), int(m.group(3)), int(m.group(4)))
assert len(tuyen) == 49, len(tuyen)

j0 = ext.index("pActivity.tbBJPoints = {")
j1 = ext.index("\r\n}", j0)
diem = {}
for m in re.finditer(r'\[(\d+)\]\s*=\s*\{\s*"([^"]*)",\s*\{\s*(\d+),\s*(\d+),\s*(\d+)\},\},', ext[j0:j1 + 3]):
    diem[int(m.group(1))] = (m.group(2).strip(), int(m.group(3)), int(m.group(4)), int(m.group(5)))
assert len(diem) == 44, len(diem)

# ---------------- award.lua ----------------
aw = rd(os.path.join(CFG129, "award.lua"))
k0 = aw.index("pActivity.tbAllCountCell = {")
k1 = aw.index("}", aw.index("[10]", k0))
boc = {}
for m in re.finditer(r"\[(\d+)\]\s*=\s*\{\s*(\d+),\s*(\d+),\}", aw[k0:k1 + 1]):
    boc[int(m.group(1))] = (int(m.group(2)), int(m.group(3)))
assert len(boc) == 10, len(boc)
expsao = {}
for m in re.finditer(r"\[(\d)\]\s*=\s*\{\s*\r?\n\s*\[1\]\s*=\s*\{\s*nExp_tl\s*=\s*1,\s*nCount\s*=\s*(\d+),\s*nRate\s*=\s*([\d.]+),", aw):
    expsao[int(m.group(1))] = (int(m.group(2)), float(m.group(3)))
assert len(expsao) == 9, len(expsao)

# ---------------- lmbj_addnpc.lua ----------------
ad = rd(os.path.join(SRV, r"script\startgame\lmbj_addnpc.lua"))
m = re.search(r"LMBJ_POS_ZHANGGUI\s*=\s*\{\s*\{(\d+),\s*(\d+),\s*(\d+)\}", ad)
cq = (int(m.group(1)), int(m.group(2)), int(m.group(3)))

# 7 Tiep Dan = cac diem cua tuyen bang (27..49) o vi tri DAU
tiepdan = sorted({tuyen[n][0] for n in range(27, 50)})
assert len(tiepdan) == 7, tiepdan

# ---------------- settings\Station.txt (ben Xa Phu: cot DESC = nhan menu "thanh thi da di qua") ----------------
# station.lua StationFun: nhan = GetStationName(id) .. "[gia luong]" ; GetStationName doc dong id+1 cot DESC.
# SECT1 = "map, x, y" -> map cua ben. Client va server co cung tep (da cmp).
st = rd(os.path.join(SRV, r"settings\Station.txt")).split("\n")
ben = []   # (map, DESC da cat khoang trang)
for l in st[1:]:
    c = l.rstrip("\r").split("\t")
    if len(c) < 4 or not c[0].strip().isdigit():
        continue
    nmap = int(c[3].split(",")[0].strip())
    ben.append((nmap, c[1].strip()))
assert len(ben) >= 16, len(ben)

# ---------------- 1) KVanTieuPos.h ----------------
L = []
L.append("// ============================================================================")
L.append("// KVanTieuPos.h - SINH TU DONG boi ReverseTools/vantieu/gen_vantieu_chinam.py tu")
L.append("// script/activitysys/config/129/extend.lua (tbBJPoints, tbBJPathLevel) va")
L.append("// script/startgame/lmbj_addnpc.lua cua may chu - DUNG SUA TAY. Chi CLIENT dung")
L.append("// (dan duong F11 muc Van tieu: CoreShell.cpp TG_VanTieu* + UiTaskGuide.cpp).")
L.append("// ============================================================================")
L.append("#ifndef KVANTIEU_POS_H")
L.append("#define KVANTIEU_POS_H")
L.append("")
L.append("#define VT_TPL_TIEUSU     2145   // Long Mon Tieu Su (npcs.txt dong 2146)")
L.append("#define VT_TPL_CHUONGQUY  2157   // Ong chu Tieu cuc Luc Tam Can")
L.append("#define VT_TPL_TIEPDAN    2230   // Tiep Dan ap Tieu Bang (7 thanh)")
L.append("#define VT_TPL_NHANHANG   2231   // Nhan Hang ap Tieu Bang (23 hang)")
L.append("#define VT_MAP_LONGMON    %d" % cq[0])
L.append("#define VT_CQ_X           %d    // o cell cua Ong chu Tieu cuc" % cq[1])
L.append("#define VT_CQ_Y           %d" % cq[2])
L.append("")
L.append("// tbBJPoints: 1..14 = Tieu Su (nhanh ca nhan); 15..44 = diem nhanh bang (Tiep Dan + Nhan Hang)")
L.append("#define VT_DIEM_SO 44")
L.append("static const short s_nVTDiemMap[VT_DIEM_SO + 1] = { 0, " + ", ".join(str(diem[i][1]) for i in range(1, 45)) + " };")
L.append("static const short s_nVTDiemX[VT_DIEM_SO + 1] = { 0, " + ", ".join(str(diem[i][2]) for i in range(1, 45)) + " };")
L.append("static const short s_nVTDiemY[VT_DIEM_SO + 1] = { 0, " + ", ".join(str(diem[i][3]) for i in range(1, 45)) + " };")
L.append("")
L.append("// tbBJPathLevel: 1..26 = tuyen ca nhan (sao 1..9); 27..49 = tuyen bang (10 sao)")
L.append("#define VT_TUYEN_SO 49")
L.append("static const unsigned char s_nVTTuyenDau[VT_TUYEN_SO + 1] = { 0, " + ", ".join(str(tuyen[i][0]) for i in range(1, 50)) + " };")
L.append("static const unsigned char s_nVTTuyenCuoi[VT_TUYEN_SO + 1] = { 0, " + ", ".join(str(tuyen[i][1]) for i in range(1, 50)) + " };")
L.append("static const unsigned char s_nVTTuyenSao[VT_TUYEN_SO + 1] = { 0, " + ", ".join(str(tuyen[i][2]) for i in range(1, 50)) + " };")
L.append("")
L.append("// 7 diem Tiep Dan ap Tieu Bang (chi so trong tbBJPoints)")
L.append("#define VT_TIEPDAN_SO %d" % len(tiepdan))
L.append("static const unsigned char s_nVTTiepDanDiem[VT_TIEPDAN_SO] = { " + ", ".join(str(x) for x in tiepdan) + " };")
L.append("")
L.append("#endif")
io.open(OUT_CORE, "w", encoding="latin-1", newline="").write("\r\n".join(L) + "\r\n")
print("ghi", OUT_CORE)

# ---------------- 2) UiTaskGuideVanTieu.h ----------------
def van(n):
    return "%d vạn" % (n // 10000) if n % 10000 == 0 else "%.1f vạn" % (n / 10000.0)

boc_txt = ", ".join("%d sao %d lần" % (lv, boc[lv][0]) for lv in range(1, 10))
exp_txt = ", ".join("%s (%d sao)" % (van(expsao[lv][0]), lv) for lv in range(1, 10))
thuong = ("Số lần bốc thưởng khi giao xe: %s. Mỗi lần bốc trúng kinh nghiệm được %s; bốc trúng vật phẩm thì nhận vật phẩm." % (boc_txt, exp_txt))

S = {}
# ---- nhanh ca nhan (TaskId 11) ----
S["VT_CN_TIEU"] = "<color=yellow>Long Môn Tiêu Cục - vận tiêu cá nhân (1 đến 9 sao)<color>"
S["VT_CN_DK"] = ("Điều kiện: cấp 90 trở lên (hoặc đã chuyển sinh) và đã gia nhập môn phái. Giờ mở: 10:00 đến 23:00 hằng ngày. "
                 "Mỗi ngày nhận tối đa 3 nhiệm vụ; làm mới nhiệm vụ ngẫu nhiên miễn phí 5 lần, hết lượt thì dùng Hoán Tiêu Chỉ.")
S["VT_CN_B1"] = ("Bước 1 - Nhận nhiệm vụ: đến <color=yellow>Long Môn trấn<color> gặp <color=yellow>Ông chủ Tiêu cục Lục Tam Cân<color> chọn Nhận nhiệm vụ áp Tiêu. "
                 "Có 3 loại: 1 sao (dễ nhất); ngẫu nhiên 2 đến 9 sao (xem tuyến rồi mới nhận, được làm mới); cao cấp (nộp áp tiêu ủy nhiệm trạng cao cấp "
                 "để nhận 7, 8 hoặc 9 sao theo tỉ lệ 60, 30, 10 phần trăm).")
S["VT_CN_B2"] = ("Bước 2 - Xuất phát: nhận xong ngươi được đưa ngay tới <color=yellow>Long Môn Tiêu Sư<color> ở điểm đầu tuyến. "
                 "Nói chuyện với Tiêu Sư chọn Bắt đầu áp Tiêu, Tiêu Xa xuất hiện cạnh ngươi và bám theo. Đồng hồ 30 phút chạy từ lúc này.")
S["VT_CN_B3"] = ("Bước 3 - Hộ tống: đi bộ (cưỡi ngựa được) dẫn Tiêu Xa tới Tiêu Sư ở điểm cuối trước khi hết 30 phút. "
                 "Xe bám theo khi ngươi ở gần (dưới 16 ô); trong cùng bản đồ mà ngươi đi xa hơn (kể cả dùng phù) thì xe đứng lại đợi, "
                 "quá 5 phút không ai quay lại thì xe biến mất. Ngươi sang bản đồ khác bằng bất cứ cách nào (cửa bản đồ, phù về thành, Xa Phu) "
                 "thì xe được kéo sang theo ngay. Thoát game thì xe đứng đợi 5 phút: vào lại kịp thì xe theo tiếp, quá 5 phút là mất xe.")
S["VT_CN_B3B"] = ("Xe có máu và bị người khác đánh phá. Xe vỡ thì rơi <color=yellow>Tiêu Kỳ<color> (chỉ chủ xe nhặt được, còn 10 phút) và "
                  "<color=yellow>Tán Lạc Tiêu Vật<color> (3 phút đầu chỉ chủ xe và tổ đội nhặt, sau đó ai đủ cấp 90 cũng nhặt được, mỗi người 5 lần một ngày). "
                  "Mang Tiêu Kỳ về Tiêu Sư vẫn nhận một phần thưởng; mất cả xe lẫn kỳ thì chọn Kết thúc nhiệm vụ để nhận 100 vạn kinh nghiệm.")
S["VT_CN_B4"] = ("Bước 4 - Trả hàng: đưa xe tới sát <color=yellow>Tiêu Sư điểm cuối<color> rồi chọn Nhận phần thưởng nhiệm vụ áp Tiêu. "
                 "Thưởng bốc theo cấp sao: kinh nghiệm hoặc vật phẩm quý (Bách Niên Huy Hoàng quả, Quả Huy Hoàng, Huyền Kim Khoáng Thạch, Hộ Mạch Đơn, "
                 "Chân Nguyên Đan, Bạo Nộ Lệnh Phù, Ngũ Hành Nguyên Thạch, Mảnh công huân chương Tống Kim, Nhất Kỷ Càn Khôn Phù hiếm), kèm 2 <color=yellow>Hộ Tiêu Lệnh<color>.")
S["VT_CN_THUONG"] = thuong
S["VT_CN_SHOP"] = ("Cửa hàng Tiêu cục (ở Ông chủ và Tiêu Sư) đổi Hộ Tiêu Lệnh lấy đạo cụ dùng khi ở gần xe: Khoái Mã Gia Tiên (tăng tốc xe 15 giây), "
                   "Kiên Bất Khả Tỏa (hồi 10 phần trăm máu xe), Tiêu Xa Di Vị (kéo xe về chỗ mình đứng); mỗi món chờ 3 phút mới dùng lại. "
                   "Lạc xe thì gặp Ông chủ hoặc Tiêu Sư chọn Truyền tống đến vị trí của Tiêu Xa (1 vạn lượng, cách nhau 30 giây).")
S["VT_CN_LUUY"] = ("Từ bỏ nhiệm vụ chỉ được khi xe chưa xuất phát và nhiệm vụ dưới 7 sao. "
                   "Đang nhận vận tiêu cá nhân thì không nhận được áp Tiêu Bang và ngược lại.")
S["VT_CN_TT_NONE"] = "Trạng thái: <color=yellow>chưa nhận nhiệm vụ<color>."
S["VT_CN_TT_FMT"] = "Trạng thái: đang nhận nhiệm vụ <color=yellow>%d sao<color>, tuyến <color=yellow>%s<color> tới <color=yellow>%s<color>, xe %s."
S["VT_CN_XE_CHUA"] = "chưa xuất phát"
S["VT_CN_XE_CON_FMT"] = "đã xuất phát, còn khoảng %d phút"
S["VT_CN_XE_HET"] = "đã xuất phát và đã hết giờ"
S["VT_CN_NGAY_FMT"] = "Hôm nay: đã nhận <color=yellow>%d/3<color> nhiệm vụ, làm mới miễn phí <color=yellow>%d/5<color>."
S["VT_CN_HINT"] = ("<color=Cyan>Nhấp vào dòng nhiệm vụ để tự đi: chưa nhận thì tới Ông chủ Tiêu cục ở Long Môn trấn, đã nhận thì tới Tiêu Sư điểm đầu "
                   "(đang ở thành hay thôn thì chạy ra Xa Phu đi xe, đang ở map luyện công thì dùng phù về thành trước); xe đã xuất phát thì tới "
                   "Tiêu Sư điểm cuối nếu cùng bản đồ. Không dẫn đường khi đang ở map sự kiện.<color>")
S["VT_CN_TRACE_TITLE"] = "<color=yellow>Vận tiêu cá nhân<color>"
S["VT_CN_BRIEF_NONE"] = "Chưa nhận nhiệm vụ áp Tiêu."
S["VT_CN_BRIEF_FMT"] = "%d sao: %s tới %s (%s)"
# ---- nhanh bang (TaskId 12) ----
S["VT_BH_TIEU"] = "<color=yellow>Long Môn Tiêu Cục - áp Tiêu Bang (10 sao)<color>"
S["VT_BH_DK"] = ("Thời gian: <color=yellow>thứ bảy và chủ nhật, 12:00 đến 23:00<color>. Điều kiện bang: đang chiếm giữ thành sau Công thành chiến; "
                 "số lượt trong tuần bằng (số thành chiếm trong tuần + 1) chia 2. Người nhận xe: bang chủ hoặc trưởng lão, cấp 90 trở lên "
                 "(hoặc chuyển sinh), vào bang đủ 7 ngày, không đang làm vận tiêu cá nhân.")
S["VT_BH_B1"] = ("Bước 1 - Bang chủ mở: gặp <color=yellow>Tiếp Dẫn áp Tiêu Bang<color> tại thành bang đang chiếm (có ở Dương Châu, Lâm An, Biện Kinh, "
                 "Thành Đô, Tương Dương, Phượng Tường, Đại Lý) chọn Mở áp Tiêu Bang. Mỗi tuần mở một lần.")
S["VT_BH_B2"] = ("Bước 2 - Nhận xe: bang chủ hoặc trưởng lão gặp Tiếp Dẫn chọn Nhận áp Tiêu Bang. <color=yellow>Tiêu Xa Bang 10 sao<color> xuất hiện cạnh người nhận, "
                 "loa toàn máy chủ báo bang, người nhận và điểm đầu. Cả bang chỉ giữ một xe một lúc, lần nhận tiếp theo cách 30 phút.")
S["VT_BH_B3"] = ("Bước 3 - Cả bang hộ tống: người nhận dẫn xe đi bộ tới <color=yellow>Nhận Hàng áp Tiêu Bang<color> ở hang động điểm cuối "
                 "(tuyến chọn ngẫu nhiên trong 3 đến 4 hang quanh thành) trong 30 phút. Thành viên bang đứng gần xe (trong 25 ô) ở trạng thái chiến đấu "
                 "mỗi phút nhận <color=yellow>400 vạn kinh nghiệm<color> và một điểm bám xe.")
S["VT_BH_B3B"] = ("Xe bị bang khác phá thì rơi <color=yellow>Rương Tiêu Vật Bang<color>: 3 phút đầu chỉ bang phá xe và bang chủ xe nhặt được, sau đó bang nào cũng nhặt được "
                  "(cần vào bang 7 ngày). Rương nộp cho bang chủ tại Tiếp Dẫn để bang chủ đổi thưởng nhóm cho bang.")
S["VT_BH_B4"] = ("Bước 4 - Giao xe: đưa xe tới sát Nhận Hàng rồi người nhận chọn Giao Tiêu Xa Bang: nhận ngay <color=yellow>8 Rương Tiêu Vật Bang<color> (hạn dùng đến hết tuần). "
                 "Sau khi chuyến kết thúc, thành viên đã bám xe gặp Nhận Hàng chọn Nhận phần thưởng hộ tiêu cá nhân: dưới 5 điểm nhận Cẩm nang thay đổi trời đất, "
                 "5 đến 11 điểm Thưởng Tận Trung, 12 điểm trở lên Thưởng Trung Thành (mở ra kinh nghiệm, Hộ Mạch Đơn, Chân Nguyên Đơn, Huyết Long Đằng, Túi Dược Phẩm...).")
S["VT_BH_LUUY"] = ("Tại Tiếp Dẫn còn có Xem thông tin Tiêu Xa, Truyền tống đến Tiêu Xa (chờ 20 giây, tăng dần tới 60) "
                   "và Từ bỏ (chỉ trong giờ 12:00 đến 23:00).")
S["VT_BH_TT_NONE"] = "Trạng thái: <color=yellow>ngươi chưa giữ xe bang<color>."
S["VT_BH_TT_FMT"] = "Trạng thái: đang giữ Tiêu Xa Bang tuyến <color=yellow>%s<color> tới <color=yellow>%s<color>, %s."
S["VT_BH_DIEM_FMT"] = "Điểm bám xe tuần này (4 lượt): %d, %d, %d, %d."
S["VT_BH_HINT"] = ("<color=Cyan>Nhấp vào dòng nhiệm vụ để tự đi tới Tiếp Dẫn áp Tiêu Bang ở thành bang đang chiếm (đang ở thành hay thôn thì "
                   "chạy ra Xa Phu đi xe, đang ở map luyện công thì dùng phù về thành trước); đang giữ xe thì đi tới Nhận Hàng điểm cuối "
                   "nếu cùng bản đồ. Không dẫn đường khi đang ở map sự kiện.<color>")
S["VT_BH_TRACE_TITLE"] = "<color=yellow>áp Tiêu Bang<color>"
S["VT_BH_BRIEF_NONE"] = "Chưa giữ xe bang."
S["VT_BH_BRIEF_FMT"] = "%s tới %s (%s)"
# ---- thong bao dan duong (DT_Msg) - v2 06/09: KHONG nhay map, di bang phu ve thanh + Xa Phu ----
S["VT_MSG_HUY"] = "<color=Cyan>[Chỉ nam] Đã hủy dẫn đường vận tiêu."
S["VT_MSG_DT"] = "<color=Yellow>[Chỉ nam] Auto Dã Tẩu đang chạy - để auto tự lo việc di chuyển."
S["VT_MSG_LAU"] = "<color=Yellow>[Chỉ nam] Đi quá lâu - dừng dẫn đường."
S["VT_MSG_SUKIEN_FMT"] = "<color=Yellow>[Chỉ nam] Đang ở map sự kiện (%s) - Chỉ nam không tự di chuyển ở đây."
S["VT_MSG_CN_XUATPHAT"] = "<color=Yellow>[Chỉ nam] Xe đã xuất phát - hãy hộ tống xe đi bộ tới Tiêu Sư điểm cuối (không dẫn đường qua bản đồ)."
S["VT_MSG_TOI_NPC_FMT"] = "<color=Cyan>[Chỉ nam] Đang chạy tới %s - bấm lại vào dòng nhiệm vụ để hủy."
S["VT_MSG_DEN_NPC_FMT"] = "<color=Cyan>[Chỉ nam] Đã tới %s - hãy chọn mục trong khung thoại."
S["VT_MSG_PHU"] = "<color=Cyan>[Chỉ nam] Đang ở map luyện công - dùng phù về thành, rồi ra Xa Phu đi tiếp..."
S["VT_MSG_KHONGPHU"] = "<color=Yellow>[Chỉ nam] Không có phù về thành trong túi - hãy tự về thành rồi bấm lại."
S["VT_MSG_XAPHU_DI_FMT"] = "<color=Cyan>[Chỉ nam] Đang chạy tới Xa Phu để đi %s (tốn tiền xe) - bấm lại để hủy."
S["VT_MSG_XAPHU_KHONGCO_FMT"] = "<color=Yellow>[Chỉ nam] Thoại Xa Phu không có mục %s - hãy tự chọn trong khung thoại."
S["VT_MSG_XAPHU_CHUADI"] = "<color=Yellow>[Chỉ nam] Chưa lên được bản đồ đích (thiếu tiền xe?) - dừng dẫn đường."
S["VT_MSG_XAPHU_KHONGBEN"] = "<color=Yellow>[Chỉ nam] Nơi cần đến không có bến Xa Phu - hãy tự đi bộ."
S["VT_MSG_BANG_KHONGBANG"] = "<color=Yellow>[Chỉ nam] Ngươi chưa vào bang - Tiếp Dẫn áp Tiêu Bang chỉ tiếp bang chiếm thành."
S["VT_MSG_BANG_KHONGTHANH"] = "<color=Yellow>[Chỉ nam] Bang của ngươi chưa chiếm thành nào - chưa có Tiếp Dẫn nào nhận xe."
S["VT_MSG_BH_XE"] = "<color=Yellow>[Chỉ nam] Đang giữ xe bang - xe phải đi bộ; chỉ dẫn đường tới Nhận Hàng khi ngươi đang ở đúng bản đồ điểm cuối."
S["VT_MSG_KHONG_NPC"] = "<color=Yellow>[Chỉ nam] Không thấy NPC ở vị trí quy định - có thể máy chủ chưa sinh NPC vận tiêu."
S["VT_MENU_THANHTHI"] = "thành thị đã đi qua"
S["VT_NPC_CHUONGQUY"] = "Ông chủ Tiêu cục Lục Tam Cân"
S["VT_NPC_TIEUSU"] = "Long Môn Tiêu Sư"
S["VT_NPC_TIEPDAN"] = "Tiếp Dẫn áp Tiêu Bang"
S["VT_NPC_NHANHANG"] = "Nhận Hàng áp Tiêu Bang"

U = []
U.append("// ============================================================================")
U.append("// UiTaskGuideVanTieu.h - SINH TU DONG boi ReverseTools/vantieu/gen_vantieu_chinam.py")
U.append("// (ten 44 diem tu extend.lua tbBJPoints, so thuong tu award.lua, bai huong dan soan trong")
U.append("// chinh script sinh) - DUNG SUA TAY. Chuoi TCVN3 tho (source-charset = execution-charset).")
U.append("// Dung boi UiTaskGuide.cpp (bang F11 muc Van tieu).")
U.append("// ============================================================================")
U.append("#ifndef UITASKGUIDE_VANTIEU_H")
U.append("#define UITASKGUIDE_VANTIEU_H")
U.append("")
U.append("static const char* const s_szVTDiemTen[44 + 1] = {")
U.append('\t"",')
for i in range(1, 45):
    U.append("\t" + clit(diem[i][0]) + ",")
U.append("};")
U.append("")
for k, v in S.items():
    if k.startswith("VT_MSG_") or k.startswith("VT_NPC_") or k.startswith("VT_MENU_"):
        continue
    U.append("#define %-20s %s" % (k, clit(V(v))))
U.append("")
U.append("#endif")
io.open(OUT_UI, "w", encoding="latin-1", newline="").write("\r\n".join(U) + "\r\n")
print("ghi", OUT_UI)

# ---------------- 3) KVanTieuMsg.h (Core: thong bao dan duong cua CoreShell.cpp) ----------------
OUT_MSG = os.path.join(ROOT, "Sources", "Core", "Src", "KVanTieuMsg.h")
M = []
M.append("// ============================================================================")
M.append("// KVanTieuMsg.h - SINH TU DONG boi ReverseTools/vantieu/gen_vantieu_chinam.py - DUNG SUA TAY.")
M.append("// Thong bao [Chi nam] cua bo dan duong van tieu (CoreShell.cpp TG_VanTieu*) + ten NPC.")
M.append("// Chuoi TCVN3 tho (Core.vcxproj: source-charset = execution-charset windows-1258).")
M.append("// ============================================================================")
M.append("#ifndef KVANTIEU_MSG_H")
M.append("#define KVANTIEU_MSG_H")
M.append("")
M.append("// ben Xa Phu (settings\\Station.txt cot DESC): map -> nhan trong thoai 'thanh thi da di qua'")
M.append("#define VT_XAPHU_SO %d" % len(ben))
M.append("static const short s_nVTXaPhuMap[VT_XAPHU_SO] = { " + ", ".join(str(b[0]) for b in ben) + " };")
M.append("static const char* const s_szVTXaPhuTen[VT_XAPHU_SO] = {")
for b in ben:
    M.append("\t" + clit(b[1]) + ",")
M.append("};")
M.append("")
for k, v in S.items():
    if k.startswith("VT_MSG_") or k.startswith("VT_NPC_") or k.startswith("VT_MENU_"):
        M.append("#define %-24s %s" % (k, clit(V(v))))
M.append("")
M.append("#endif")
io.open(OUT_MSG, "w", encoding="latin-1", newline="").write("\r\n".join(M) + "\r\n")
print("ghi", OUT_MSG)
print("tiepdan =", tiepdan, "| chuong quy =", cq)
