# -*- coding: utf-8 -*-
"""vhtd_data_patch4.py [VHTD 02/09g] - du lieu dot 4 Vu Hon / Tieu Dao (E:\\...\\bin, KHONG trong git):
 1. server\\script\\header\\factionhead.lua   SKILLNORMAL[13] (Tieu Dao) + {2135,1}: don thuong cam (EqtLimit 103) cap luc nhap mon
 2. server\\script\\global\\skills_table.lua  add_xy(10) + AddMagic(2135, 1) (NPC test hoc 150 / hocvocong)
 3. client\\ui\\StatePos.ini                  +21 muc: icon/ten/mo ta cho buff 3 phai + o 'tang' No (1976) / Am Luat (2116)
 4. client\\ui\\Ui3\\UiHeaderControlBar.ini    [Main]/[Main1024] Button6=Shield + [Shield] (ClassType=Player_Shield) + [Shield_Image]:
                                             thanh khien tinh (anh thanh noi luc) de len nua duoi thanh sinh luc = 'ong mau thu 2'
Doc/ghi latin-1, ban luu <tep>.truoc_vhtd_0209g, idempotent (chay lai = [=]). DUNG: python vhtd_data_patch4.py [--kiem]
"""
import io, os, sys, shutil, re
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

KIEM = "--kiem" in sys.argv
B = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
MK = "[VHTD 02/09g]"
BAK = ".truoc_vhtd_0209g"
NL = "\r\n"
T = "\t"

def V(u):
    return unicode_to_tcvn3_bytes(u).decode("latin-1")

def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()

def save(p, s, orig):
    if s == orig:
        print("  (khong doi) %s" % p); return
    if "\xef\xbf\xbd" in s:
        raise SystemExit("EF BF BD " + p)
    if KIEM:
        print("  => KIEM %s" % p); return
    if not os.path.exists(p + BAK):
        shutil.copy2(p, p + BAK)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  => ghi %s" % p)

def rep(s, old, new, tag, p):
    if tag in s:
        print("  [=] %s" % tag); return s
    if s.count(old) != 1:
        raise SystemExit("neo %s: %d lan trong %s" % (tag, s.count(old), p))
    print("  [+] %s" % tag)
    return s.replace(old, new)

# 1. factionhead.lua
p = os.path.join(B, "server", "script", "header", "factionhead.lua")
s = rd(p); o = s
s = rep(s, T + "{2136,0},---Tuy y Khuc" + NL,
        T + "{2136,0},---Tuy y Khuc" + NL + T + "{2135,1},---Cong kich vat ly cam (don thuong Moc Cam, EqtLimit 103) " + MK + NL,
        MK + " SKILLNORMAL[13] 2135", p)
save(p, s, o)

# 2. skills_table.lua add_xy
p = os.path.join(B, "server", "script", "global", "skills_table.lua")
s = rd(p); o = s
old = (T*2 + "if (HaveMagic(2136) == -1) then" + NL + T*3 + "AddMagic(2136)" + T*2 + "-- Tuy y Khuc" + NL + T*2 + "end" + NL)
new = old + (T*2 + "if (HaveMagic(2135) == -1) then" + NL + T*3 + "AddMagic(2135, 1)" + T*2 + "-- " + MK + " don thuong cam (2135, EqtLimit 103): client chon qua ClientWeaponSkill (1,3)" + NL + T*2 + "end" + NL)
s = rep(s, old, new, MK + " add_xy 2135", p)
save(p, s, o)

# 3. StatePos.ini
skills = {}
for l in io.open(os.path.join(B, "client", "settings", "skills.txt"), encoding="latin-1"):
    r = l.rstrip("\r\n").split("\t")
    if len(r) > 6 and r[2].isdigit():
        skills[int(r[2])] = (r[0].strip(), r[5].strip())   # ten TCVN3, icon GBK
ENTRIES = [
    (1976, u"Nộ", u"Tầng Nộ (Vũ Hồn): tích khi tự động thi triển, tiêu hao cho Hãm Sơn Kích / Thỉnh Anh / Vũ Mục"),
    (2116, u"Âm Luật", u"Tầng Âm Luật (Tiêu Dao): tích mỗi giây, tiêu hao cho Tạp Đạp Lưu Tinh / Lạc Nhạn / Thập Bộ"),
    (1989, None, u"Nộ: tăng tốc độ di chuyển, +tầng Nộ"),
    (1966, None, u"Thỉnh Anh Đề Nhuệ Lữ: giảm sát thương nhận vào"),
    (1968, None, u"Kinh Đào Phách Ngạn: giảm tốc độ đánh / xuất chiêu"),
    (1971, None, u"Trí Dũng Siêu Luân: bị đánh có tỉ lệ Chiết Kích"),
    (1973, None, u"Chiết Kích: kỹ năng đối phương suy giảm"),
    (1982, None, u"Vũ Mục Di Thư: khóa sinh lực, kháng khống chế"),
    (1987, None, u"Đầu Dũng Đương Tiên: tăng tốc độ di chuyển"),
    (1988, None, u"Hãm Sơn Kích: giảm hiệu suất hồi sinh lực"),
    (1991, None, u"Trung Vũ Lưu Phong: hồi sinh lực"),
    (2128, None, u"Say: tăng lực công kích"),
    (2130, None, u"Thập Bộ Nhất Sát: ẩn thân, bất tử"),
    (2131, None, u"Thập Bộ Nhất Sát: không thể tấn công"),
    (2133, None, u"Phất Y: tăng tốc, tự kích hoạt hộ thuẫn"),
    (2134, None, u"Phất Y: hộ thuẫn nội lực (khiên)"),
    (2139, None, u"Tiêu Dao Vũ: hộ thuẫn Âm Luật (khiên nội lực)"),
    (1366, None, u"Lạc Nhạn Kinh Hồng: hồi sinh lực"),
    (1376, None, u"Long Huyền Kiếm Khí: giảm sát thương nhận vào"),
    (1380, None, u"Ma Vân Kiếm Khí: tăng tốc, tăng kỹ năng"),
    (1381, None, u"Kiếm Khí Vô Cấp: giảm sát thương nhận vào"),
]
p = os.path.join(B, "client", "ui", "StatePos.ini")
s = rd(p); o = s
m = re.search(r"BuffCount=(\d+)\r\n", s)
if not m:
    raise SystemExit("StatePos.ini: khong thay BuffCount")
cnt = int(m.group(1))
have = set(int(x) for x in re.findall(r"Buff_\d+_ID=(\d+)", s))
add = ""
n = cnt
for sid, name, desc in ENTRIES:
    if sid in have:
        print("  [=] StatePos %d da co" % sid); continue
    if sid not in skills:
        raise SystemExit("skills.txt khong co %d" % sid)
    ten, icon = skills[sid]
    if name is not None:
        ten = V(name)
    add += (NL + "Buff_%d_ID=%d" % (n, sid) + NL + "Buff_%d_Name=%s" % (n, ten) + NL +
            "Buff_%d_Image=%s" % (n, icon) + NL + "Buff_%d_Desc=%s ;%s" % (n, V(desc), MK) + NL)
    n += 1
if add:
    s = s.replace(m.group(0), "BuffCount=%d" % n + NL, 1)
    if not s.endswith(NL):
        s += NL
    s += add
    print("  [+] StatePos.ini +%d muc (BuffCount %d -> %d)" % (n - cnt, cnt, n))
save(p, s, o)

# 4. UiHeaderControlBar.ini
p = os.path.join(B, "client", "ui", "Ui3", "UiHeaderControlBar.ini")
s = rd(p); o = s
if "[Shield]" in s:
    print("  [=] UiHeaderControlBar.ini da co [Shield]")
else:
    for sec in ("[Main]", "[Main1024]"):
        i = s.find(sec)
        if i < 0: raise SystemExit("khong thay " + sec)
        j = s.find("Button5=WorldSort" + NL, i)
        if j < 0: raise SystemExit("khong thay Button5 trong " + sec)
        j += len("Button5=WorldSort" + NL)
        s = s[:j] + "Button6=Shield" + NL + s[j:]
    mi = re.search(r"\[Mana_Image\]\r\n(?:.*\r\n)*?Image=([^\r\n]+)\r\n", s)
    if not mi: raise SystemExit("khong thay [Mana_Image] Image")
    img = mi.group(1)
    # chen truoc ';' + [Mana] (sau khoi Life_Text)
    k = s.find("[Mana]" + NL)
    if k < 0: raise SystemExit("khong thay [Mana]")
    # lui ve dau dong chu thich ngay truoc [Mana]
    k2 = s.rfind(NL + ";", 0, k)
    ins_at = k2 + 2 if k2 >= 0 else k
    blk = (";" + MK + " thanh khien tinh (Player_Shield) de len nua duoi thanh sinh luc, chi hien khi co khien" + NL +
           "[Shield]" + NL + "Left=168" + NL + "Top=8" + NL + "Width=104" + NL + "Height=5" + NL +
           "Tip=" + V(u"Hộ thuẫn") + NL + "Part=1" + NL + "ClassType=Player_Shield" + NL + NL +
           "[Shield_Image]" + NL + "Left=0" + NL + "Top=0" + NL + "Width=104" + NL + "Height=4" + NL + "Trans=0" + NL +
           "Image=" + img + NL + "PartType=0" + NL + NL +
           "[Shield_Text]" + NL + "Left=0" + NL + "Top=0" + NL + "Width=104" + NL + "Height=1" + NL + "Font=12" + NL + "HAlign=1" + NL + "Color=255,255,255" + NL + NL)
    s = s[:ins_at] + blk + s[ins_at:]
    print("  [+] UiHeaderControlBar.ini [Shield] + Button6")
save(p, s, o)
print("XONG.")
