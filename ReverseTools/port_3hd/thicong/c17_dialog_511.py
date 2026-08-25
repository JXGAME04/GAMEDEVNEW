# -*- coding: utf-8 -*-
"""C17 - hop thoai NPC 769 (chu chup anh 13:2x):
  - "<co lor>" hien nguyen van: byte cao TCVN3 dung TRUOC '<' nuot mat '<'
    (TEncodeText, Text.cpp:468) => PHAI co dau cach truoc MOI the <color...>
    va <color> dong.
  - "8nhiem vu" dinh lien, "dong doiva kha nang" thieu cach.
  - Menu dut "<#>M": tong goi Describe ~729B > tran 511B cua goi thoai
    => rut gon [1] + cac nhan menu de <=~500B ke ca nhanh chen [29].
Chi sua CHUOI hien thi, khong doi function/logic (/killer90, /cancel...).
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
REL = r"script\task\tollgate\killer\nieshichen.lua"

p = SRV + "\\" + REL
d = io.open(p, encoding="latin-1", newline="").read()
lines = d.split("\n")

# ---- chuoi moi (do byte truoc khi ghi) ----
new1 = '\t"<#>' + V("Gộp đủ 160 sát thủ giản sẽ thành đệ nhất sát thủ. Mỗi ngày chỉ cần xong ") + '<color=yellow>' + V("8 nhiệm vụ") + ' <color>' + V("là được.") + ' <enter>' + V("Hãy nhận 'thách thức thời gian' để thử sức đồng đội.") + '",'
new10 = '\t"<#>' + V("Ta không muốn giết người, hủy nhiệm vụ.") + '/cancel",\t--10'
new11 = '\t"<#>' + V("Máu chảy đầu rơi, ta nên tránh xa") + '/no",'
new29 = '\t"<#>' + V("Quá quan tầm bảo") + '/guoguan_xunbao",'
new30 = '\t"<#>' + V("Nhận thưởng") + '/rank_award",'
new31 = '\t"<#>' + V("Xếp hạng 5 đội hôm nay.") + '/get_top5team"'

for tag, s in (("[1]", new1), ("[10]", new10), ("[11]", new11), ("[29]", new29), ("[30]", new30), ("[31]", new31)):
    print(tag, len(s.encode("latin-1")), "B")

# ContentList muc i nam o dong index 21+i (0-based): [1]=22, [10]=31, [11]=32, [29]=50, [30]=51, [31]=52
def rep(iline, new, must):
    old = lines[iline]
    assert must in old, (iline, old[:60])
    # giu \r neu co
    tail = "\r" if old.endswith("\r") else ""
    lines[iline] = new + tail

rep(22, new1, "160")
rep(31, new10, "/cancel")
rep(32, new11, "/no")
rep(50, new29, "guoguan_xunbao")
rep(51, new30, "rank_award")
rep(52, new31, "get_top5team")

d2 = "\n".join(lines)
io.open(p, "w", encoding="latin-1", newline="").write(d2)
print("da ghi", REL)
import shutil, os
os.makedirs(os.path.dirname(MIRROR + "\\" + REL), exist_ok=True)
shutil.copyfile(p, MIRROR + "\\" + REL)
print("da sync guong")

# ---- do tong goi thoai chinh ----
desc = 92  # DescLink_NieShiChen
tong = desc
for i in (22, 45, 38, 30, 31, 32, 51, 52, 36):  # [1],[24],[17],[9],[10],[11],[30],[31],[15]
    s = lines[i].strip().rstrip(",").strip('"')
    tong += len(s.encode("latin-1"))
print("tong goi chinh (chua [29]):", tong, "B; +[29]:", tong + len(lines[50].strip().rstrip(",").strip('"').encode("latin-1")), "B (tran 511)")
