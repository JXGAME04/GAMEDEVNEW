r"""gom_bando.py - GOM script ban do ten Han (9 khu + global\dac thu dung dia) vao script\bando\<khu>\<thanh>\... ten Viet khong dau
(dich Han-Viet tung chu), giu ID cu bang bi danh (_duongdan_cu.txt). Ghi bang doi chieu script\bando\_DOICHIEU_TEN.txt (UTF-8).
  python gom_bando.py         -> kiem (in ke hoach, xung dot, chu chua co trong bang)
  python gom_bando.py sua     -> thuc hien
"""
import os, sys, shutil, io, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
SCRIPT = os.path.join(ROOT, "script")
ALIAS = os.path.join(SCRIPT, "_duongdan_cu.txt")
SUA = len(sys.argv) > 1 and sys.argv[1] == "sua"
BS = chr(92)

HV_TXT = """
一 nhat|七 that|丈 truong|三 tam|下 ha|丐 cai|东 dong|丝 ti|两 luong|中 trung|临 lam|丹 dan|主 chu|久 cuu|乐 lac|乞 khat|书 thu|事 su|二 nhi|云 van|五 ngu|井 tinh|京 kinh|人 nhan|仑 luan|仙 tien|伏 phuc|众 chung|伙 hoa|会 hoi|传 truyen|住 tru|佛 phat|佩 boi|使 su|侧 trac|信 tin|偏 thien|储 tru|僧 tang|儿 nhi|先 tien|光 quang|入 nhap|八 bat|公 cong|六 luc|兰 lan|关 quan|兵 binh|内 noi|冈 cuong|农 nong|冠 quan|冰 bang|凤 phuong|出 xuat|切 thiet|前 tien|剑 kiem|功 cong|勇 dung|包 bao|北 bac|匠 tuong|匪 phi|区 khu|医 y|十 thap|千 thien|华 hoa|卖 mai|南 nam|卧 ngoa|卫 ve|厅 sanh|原 nguyen|县 huyen|叔 thuc|口 khau|古 co|台 dai|右 huu|合 hop|名 danh|后 hau|吕 lu|吴 ngo|告 cao|员 vien|周 chu|响 huong|唐 duong|商 thuong|善 thien|喆 triet|器 khi|四 tu|园 vien|图 do|土 tho|圣 thanh|地 dia|场 truong|坊 phuong|城 thanh|堂 duong|塔 thap|墨 mac|外 ngoai|大 dai|天 thien|太 thai|夫 phu|头 dau|夷 di|女 nu|妇 phu|妓 ky|妹 muoi|始 thuy|娘 nuong|婆 ba|婉 uyen|婶 tham|媳 tuc|嫖 phieu|子 tu|孙 ton|孤 co|学 hoc|孽 nghiet|守 thu|安 an|宋 tong|官 quan|宝 bao|宠 sung|客 khach|室 that|宫 cung|宵 tieu|家 gia|密 mat|寝 tam|寨 trai|对 doi|寺 tu|寻 tam|小 tieu|少 thieu|尤 vuu|尹 doan|局 cuc|层 tang|居 cu|山 son|岑 sam|岛 dao|岳 nhac|峨 nga|峰 phong|嵋 mi|州 chau|左 ta|差 sai|巴 ba|巷 hang|师 su|帐 truong|帮 bang|广 quang|庄 trang|应 ung|底 de|店 diem|府 phu|庭 dinh|开 khai|弟 de|张 truong|当 duong|彭 banh|役 dich|徒 do|御 ngu|微 vi|心 tam|忍 nhan|快 khoai|性 tinh|总 tong|恶 ac|慈 tu|成 thanh|战 chien|房 phong|所 so|手 thu|打 da|扬 duong|护 ho|拾 thap|持 tri|捡 kiem|换 hoan|接 tiep|摊 than|摩 ma|擂 loi|教 giao|散 tan|斗 dau|方 phuong|无 vo|昆 con|明 minh|春 xuan|景 canh|暗 am|曼 man|曾 tang|月 nguyet|期 ky|木 moc|朱 chu|机 co|杀 sat|杂 tap|杈 xa|村 thon|杨 duong|板 ban|林 lam|果 qua|架 gia|标 tieu|栈 san|树 thu|桃 dao|桥 kieu|桩 thung|梅 mai|楼 lau|正 chinh|武 vu|殿 dien|母 mau|毒 doc|水 thuy|永 vinh|汉 han|江 giang|汴 bien|沙 sa|沧 thuong|河 ha|泉 tuyen|法 phap|洞 dong|津 tan|活 hoat|派 phai|流 luu|浪 lang|混 hon|清 thanh|渝 du|渡 do|游 du|湖 ho|源 nguyen|溪 khe|漠 mac|火 hoa|灵 linh|点 diem|烟 yen|燕 yen|父 phu|爷 gia|爹 da|牌 bai|牛 nguu|牢 lao|物 vat|独 doc|狼 lang|猛 manh|猪 tru|玉 ngoc|王 vuong|理 ly|生 sinh|留 luu|白 bach|百 bach|的 dich|皇 hoang|皮 bi|相 tuong|眼 nhan|石 thach|码 ma|碑 bia|碧 bich|示 thi|祖 to|神 than|祭 te|禁 cam|禅 thien|离 ly|秘 bi|秦 tan|稻 dao|穴 huyet|空 khong|窟 khuat|站 tram|竹 truc|笑 tieu|算 toan|箱 suong|紫 tu|红 hong|练 luyen|经 kinh|绣 tu|罗 la|羊 duong|翔 tuong|翠 thuy|老 lao|职 chuc|肉 nhuc|肖 tieu|胡 ho|能 nang|脚 cuoc|腊 lap|舜 thuan|船 thuyen|芙 phu|芝 chi|花 hoa|苍 thuong|英 anh|茶 tra|草 thao|荐 tien|荡 dang|药 duoc|莫 mac|菊 cuc|菜 thai|营 doanh|落 lac|蓉 dung|蔡 thai|蔬 so|薛 tiet|藏 tang|虎 ho|蚓 dan|蚩 xi|蚯 khuu|蛇 xa|蛛 chu|蜀 thuc|蜈 ngo|蝎 hat|蝶 diep|螺 loa|蟾 thiem|街 nhai|衙 nha|衡 hanh|表 bieu|袋 dai|裳 thuong|襄 tuong|西 tay|见 kien|角 giac|计 ke|议 nghi|讲 giang|话 thoai|说 thuyet|谷 coc|豆 dau|豹 bao|货 hoa|贩 phien|贺 ha|赌 do|赤 xich|路 lo|车 xa|轩 hien|辕 vien|辛 tan|达 dat|连 lien|迷 me|送 tong|道 dao|邹 trau|郭 quach|都 do|酋 tu|酒 tuu|采 thai|里 ly|金 kim|钱 tien|铁 thiet|银 ngan|铺 pho|锁 toa|镇 tran|镖 tieu|长 truong|门 mon|阁 cac|阳 duong|阵 tran|阿 a|陀 da|院 vien|陵 lang|雁 nhan|雄 hung|雅 nha|雪 tuyet|零 linh|霄 tieu|霸 ba|青 thanh|面 dien|音 am|风 phong|飞 phi|馆 quan|香 huong|马 ma|驴 lu|驿 dich|高 cao|魄 phach|魏 nguy|鸡 ke|鸨 bao|麓 loc|黄 hoang|黑 hac|鼓 co|龙 long|特 dac|殊 thu|用 dung|钟 chung|艳 diem|凌 lang|敏 man|叛 phan|境 canh|报 bao|梦 mong
"""
HV = {}
for it in HV_TXT.replace("\n", "|").split("|"):
    it = it.strip()
    if not it: continue
    ch, rd = it.split()
    HV[ch] = rd

# 9 khu + dac thu: ten co dinh
KHU = {"中原北区": "trungnguyen_bac", "中原南区": "trungnguyen_nam", "西南北区": "taynam_bac", "西南南区": "taynam_nam",
       "西北北区": "taybac_bac", "西北南区": "taybac_nam", "两湖区": "luongho", "江南区": "giangnam", "东北区": "dongbac",
       "特殊用地": "dacthu"}

def ansi_bytes(s):
    out = bytearray()
    for ch in s:
        o = ord(ch)
        if o < 128 or o in (0x81, 0x8D, 0x8F, 0x90, 0x9D): out.append(o)
        else:
            try: out += ch.encode("cp1252")
            except UnicodeEncodeError: out += b"?"
    return bytes(out)
def gbk(name):
    try: return ansi_bytes(name).decode("gbk")
    except Exception: return None
thieu = collections.Counter()
def viet(name_gbk):
    """ten (unicode Han) -> ascii khong dau"""
    out = []; prev_han = False
    for ch in name_gbk:
        o = ord(ch)
        if o < 128:
            if ch.isalnum() or ch in "._-":
                if prev_han and ch.isalpha(): out.append("_")
                out.append(ch.lower())
            else:
                out.append("_")
            prev_han = False
        else:
            rd = HV.get(ch)
            if rd is None:
                thieu[ch] += 1; rd = "x%04x" % o
            if out and not prev_han and out[-1].isalpha(): out.append("_")
            out.append(rd); prev_han = True
    s = "".join(out)
    while "__" in s: s = s.replace("__", "_")
    return s.strip("_")

def fid(b):
    Id = 0
    for i, ch in enumerate(b):
        c = ch if ch < 128 else ch - 256
        Id = ((Id + (i + 1) * c) & 0xFFFFFFFF) % 0x8000000B
        Id = (Id * 0xFFFFFFEF) & 0xFFFFFFFF
    return Id ^ 0x12345678
def key_of(rel):   # rel 'script\...' unicode -> bytes key '\script\...' thuong (ASCII)
    b = ansi_bytes(rel.replace("/", BS))
    b = bytes([c + 32 if 65 <= c <= 90 else c for c in b]).lstrip(BS.encode())
    return BS.encode() + b

# ---- nguon: 9 khu o goc script + global\特殊用地 + global\钱庄功能.lua
nguon = []   # (thu muc goc tuyet doi, rel goc (unicode), khu moi)
for d in os.listdir(SCRIPT):
    g = gbk(d)
    if g in KHU and os.path.isdir(os.path.join(SCRIPT, d)):
        nguon.append((os.path.join(SCRIPT, d), "script" + BS + d, KHU[g]))
for d in os.listdir(os.path.join(SCRIPT, "global")):
    g = gbk(d)
    p = os.path.join(SCRIPT, "global", d)
    if g in KHU and os.path.isdir(p):
        nguon.append((p, "script" + BS + "global" + BS + d, KHU[g]))
tep_le = []
for f in os.listdir(os.path.join(SCRIPT, "global")):
    g = gbk(f)
    if g and any(ord(c) > 127 for c in g) and os.path.isfile(os.path.join(SCRIPT, "global", f)):
        tep_le.append(f)

# ---- ID hien co (moi tep .lua/.txt trong cay + khoa bi danh)
ids = {}
for sub in ("script", os.path.join("scriptjx2", "tong_vn")):
    for dp, dn, fn in os.walk(os.path.join(ROOT, sub)):
        for f in fn:
            if f.lower().endswith((".lua", ".txt")):
                rel = os.path.relpath(os.path.join(dp, f), ROOT)
                ids[fid(key_of(rel))] = rel
alias_lines = open(ALIAS, "rb").read().split(b"\n")
for line in alias_lines:
    if line.startswith(b"--@") and b"=" in line:
        cu = line[3:].split(b"=", 1)[0].strip().lower().replace(b"/", BS.encode()).lstrip(BS.encode())
        ids[fid(BS.encode() + cu)] = "(bi danh) " + cu.decode("latin-1")

# ---- lap ke hoach
plan = []          # (src_abs, dst_rel_ascii, old_rel_unicode)
dst_dirs = {}      # thu muc dich (lower) -> ten goc Han (de doi chieu)
used_lower = set(os.path.relpath(os.path.join(dp, f), ROOT).lower() for dp, dn, fn in os.walk(SCRIPT) for f in fn)
def uniq_path(rel):
    base, ext = os.path.splitext(rel)
    cand = rel; n = 2
    while cand.lower() in used_lower or fid(key_of(cand)) in ids:
        cand = "%s_%d%s" % (base, n, ext); n += 1
    used_lower.add(cand.lower()); ids[fid(key_of(cand))] = cand
    return cand
doi_chieu = []
for base_abs, base_rel, khu in nguon:
    for dp, dn, fn in os.walk(base_abs):
        rel_dir = os.path.relpath(dp, base_abs)
        parts = [] if rel_dir == "." else rel_dir.split(os.sep)
        parts_v = []
        for p in parts:
            g = gbk(p) or p
            parts_v.append(viet(g) if any(ord(c) > 127 for c in g) else p.lower())
        for f in fn:
            g = gbk(f) or f
            stem, ext = os.path.splitext(g)
            fv = (viet(stem) if any(ord(c) > 127 for c in stem) else stem.lower()) + ext.lower()
            dst_rel = os.path.join("script", "bando", khu, *parts_v, fv)
            dst_rel = uniq_path(dst_rel)
            old_rel = os.path.join(base_rel, rel_dir, f) if parts else os.path.join(base_rel, f)
            plan.append((os.path.join(dp, f), dst_rel, old_rel))
            doi_chieu.append((dst_rel, (gbk(os.path.join(base_rel, rel_dir, f) if parts else os.path.join(base_rel, f)) or "?")))
for f in tep_le:
    g = gbk(f) or f
    stem, ext = os.path.splitext(g)
    dst_rel = uniq_path(os.path.join("script", "global", viet(stem) + ext.lower()))
    plan.append((os.path.join(SCRIPT, "global", f), dst_rel, os.path.join("script", "global", f)))
    doi_chieu.append((dst_rel, g))

print("Nguon: %d nhanh (%s) + %d tep le; ke hoach: %d tep" % (len(nguon), ", ".join(k for _, _, k in nguon), len(tep_le), len(plan)))
if thieu:
    print("CHU CHUA CO TRONG BANG (%d): %s" % (len(thieu), "".join(sorted(thieu))))
for src, dst, old in plan[:8]:
    print("  ", (gbk(old) or old)[:60], "->", dst)
if not SUA:
    print("(kiem) khong doi gi. Chay 'sua' de thuc hien.")
    sys.exit(0)

# ---- thuc hien
moved = 0
new_alias = []
for src, dst, old in plan:
    d_abs = os.path.join(ROOT, dst)
    os.makedirs(os.path.dirname(d_abs), exist_ok=True)
    shutil.move(src, d_abs); moved += 1
    new_alias.append(b"--@ " + ansi_bytes(old) + b"=" + dst.encode("ascii"))
# xoa thu muc rong
for base_abs, _, _ in nguon:
    for dp, dn, fn in os.walk(base_abs, topdown=False):
        try: os.rmdir(dp)
        except OSError: pass
# bi danh
shutil.copy2(ALIAS, ALIAS + ".truoc_bando_0609")
raw = open(ALIAS, "rb").read()
if not raw.endswith(b"\n"): raw += b"\r\n"
raw += b"-- [BANDO 06/09] script ban do ten Han (9 khu + dac thu) gom vao script\\bando\\<khu>\\..., ten Viet khong dau; dich Han-Viet tung chu\r\n"
raw += b"\r\n".join(new_alias) + b"\r\n"
open(ALIAS, "wb").write(raw)
# bang doi chieu
with io.open(os.path.join(SCRIPT, "bando", "_DOICHIEU_TEN.txt"), "w", encoding="utf-8", newline="\r\n") as f:
    f.write("# Doi chieu ten moi (Viet khong dau) = ten goc (Han). Sinh boi gom_bando.py 06/09/2026. Engine bo qua tep bat dau '_'.\n")
    f.write("# Bi danh ID cu -> tep moi: script/_duongdan_cu.txt (dong --@). Trap/NPC trong Pak\\maps.pak tro theo ID duong dan CU.\n")
    for dst, g in sorted(doi_chieu):
        f.write("%s = %s\n" % (dst, g))
print("DA CHUYEN %d tep; them %d dong bi danh; doi chieu: script\\bando\\_DOICHIEU_TEN.txt" % (moved, len(new_alias)))
