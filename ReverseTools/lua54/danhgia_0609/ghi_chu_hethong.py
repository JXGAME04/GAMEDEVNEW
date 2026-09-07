"""ghi_chu_hethong.py - chen khoi chu thich [HE THONG] dau moi tep script he thong (muc dich, ai nap, Include, danh muc ham,
   sua nong duoc khong) + sinh script/_MUCLUC.lua. Byte-safe (latin-1), chi chen ASCII, bo qua tep da co [HE THONG].
   python ghi_chu_hethong.py <scratch dir> [thuc_hien]"""
import os, sys, re, json, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
SP = sys.argv[1]
DO = len(sys.argv) > 2 and sys.argv[2] == "thuc_hien"
ROOT = os.environ.get("SAPXEP_ROOT", r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server")
SCRIPT = os.path.join(ROOT, "script")
BS = chr(92)
LOAI = json.load(open(os.path.join(SP, "r33_loai_tru.json")))
closure = set(k.lower() for k in LOAI["timer_closure"])
cpp = set(k.lower() for k in LOAI["cpp"])

# ---------------------------------------------------------------- mo ta muc dich (ASCII khong dau)
MOTA = {
    "timerserver.lua": "DONG HO MAY CHU. C++ (CoreServerShell.cpp) goi RunTime() moi phut o giay 0. Tu nap lai chinh no khi noi dung (hoac tep Include) doi -> sua nong khong can restart. Dieu phoi: Chien Lenh, bot tu dong, Tong Kim, 3 hoat dong (HD3), Viem De, cong thanh, su kien, dua top, song bac, kick, PUBG. Bo do PROF ghi logs/hethong.log khi >= TS_PROF_NGUONG ms.",
    "timerserver_ctc.lua": "Dong ho 5 pha CONG THANH CHIEN tren GameServer (dot E6); goi tu timerserver.lua.",
    "protocol.lua": "Bo dang ky GIAO THUC SCRIPT (KE_SCRIPT_PROTOCOL): client <-> server goi ham theo id; dung boi script_protocol/*.lua va global/script_protocol.lua.",
    "startgame.lua": "CHAY MOT LAN khi server khoi dong (OnGame): dat NPC/trap (startgame/npcpos.lua, traplib.lua), boot cong thanh, bang hoi JX2, van tieu... Sua xong phai restart.",
    "gmscript.lua": "Diem ha canh lenh GM 'dw ...' (GlobalExecute) - port WLLS 20/08.",
    "gb_taskfuncs.lua": "Ham bien nhiem vu toan cum (Relay share, goc JX2). Trung noi dung voi lib/gb_taskfuncs.lua.",
    "lib/awardtemplet.lua": "MAU PHAT THUONG (award templet) cho nhiem vu/hoat dong: dinh nghia loai thuong va cach trao (awardtype/*.lua).",
    "lib/baseclass.lua": "LOP CO SO kieu JX2: ke thua qua tag method (nay la metatable trong lua4compat).",
    "lib/common.lua": "Ham co ban dung chung (JX2): chuoi, bang, so.",
    "lib/composeclass.lua": "CHE TAO / GHEP DO co giao dien (compose class).",
    "lib/composeex.lua": "CHE TAO / GHEP DO khong giao dien (compose ex) - dung luyen, lo ren.",
    "lib/gb_taskfuncs.lua": "Ham bien nhiem vu toan cum (Relay share, goc JX2).",
    "lib/getrectangle_point.lua": "Tinh diem trong hinh chu nhat (port Linux, Hoa Son 01/09).",
    "lib/lib_faction.lua": "MON PHAI: doi phai, ky nang phai, kiem tra phai/he.",
    "lib/lib_ham.lua": "THU VIEN HAM TIEN ICH CHUNG cua du an: thoi gian, chuoi, bang, thong bao, tien te.",
    "lib/lib_lmbiaoche.lua": "VAN TIEU Long Mon: ham xe tieu (engine KBiaoChe.cpp).",
    "lib/lib_map.lua": "BAN DO: ten/id map, toa do, dich chuyen, kiem tra vung an toan.",
    "lib/lib_reborn.lua": "TRUNG SINH: bang cau hinh + ham (561 dong, phan lon la du lieu).",
    "lib/lib_revivepos.lua": "DIEM HOI SINH theo map (du lieu).",
    "lib/lib_server.lua": "CAU HINH MAY CHU (CFGNEN 30/08): nap cauhinh/ch_*.lua; sua cau hinh -> sua cauhinh/, KHONG sua o day.",
    "lib/lib_sukien.lua": "Ham SU KIEN dung chung (event): mo/dong, kiem gio, phat thuong.",
    "lib/lib_task.lua": "BIEN NHIEM VU: GetTask/SetTask, bit, chuoi nhiem vu. Tep duoc Include nhieu nhat (~500 tep).",
    "lib/lib_trap.lua": "BANG TRAP CHUYEN MAP (1766 dong, du lieu) - dung boi startgame/traplib.lua.",
    "lib/lib_vatpham.lua": "VAT PHAM: them/xoa/kiem/dem vat pham trong tui.",
    "lib/log.lua": "Ghi LOG script (WriteLog) theo loai.",
    "lib/objbuffer_head.lua": "OBJBUFFER: dong goi tham so khi goi RemoteExecute sang Relay (RELAYHT 06/09).",
    "lib/pay.lua": "Diem mo rong (extpoint) - goc JX2.",
    "lib/pfunction.lua": "Ham NGUOI CHOI (player function) goc JX2.",
    "lib/progressbar.lua": "Thanh tien trinh (progress bar) - port Linux, Hoa Son 01/09.",
    "lib/remoteexc.lua": "REMOTEEXECUTE: goi ham tu GameServer sang Relay va nguoc lai (RELAYHT 06/09).",
    "lib/worldlibrary.lua": "THONG TIN THE GIOI: SERVER_OPEN (ngay mo may), mua giai, tinh so ngay mo, mui gio.",
    "header/cauhinh_hoatdong.lua": "CAU HINH HOAT DONG (lich, phan thuong, cua so gio) - timerserver va cac hoat dong doc; sua nong duoc.",
    "header/factionhead.lua": "DINH NGHIA MON PHAI: id, ten, ky nang, vu khi, he.",
    "header/forbidmap.lua": "MAP CAM (khong dung phu/PK/giao dich).",
    "header/fuyuan.lua": "He PHUC DUYEN.",
    "header/loginao.lua": "Map/ao dang nhap.",
    "header/map_helper.lua": "TRO GIUP MAP (48 ham): id <-> ten, toa do, kiem tra map thanh/thi.",
    "header/region.lua": "VUNG (region) ban do, kiem tra toa do trong vung.",
    "header/repute_head.lua": "DANH VONG (repute): loai, ten, gioi han.",
    "header/revivepos_head.lua": "DIEM HOI SINH (du lieu).",
    "header/taskid.lua": "DANH SACH ID BIEN NHIEM VU (TASK ID). Them id moi o day; kiem trung bang ReverseTools quet trung task id (24/08).",
    "header/testgame.lua": "Bo test/khung khi khoi dong game (1412 dong).",
    "header/timerhead.lua": "Ham thoi gian dung chung cho timer.",
    "header/tongkim.lua": "TONG KIM: cau hinh dau truong, gio mo, map.",
    "cauhinh/ch_all.lua": "GOM cau hinh: Include tat ca ch_*.lua. Duoc timerserver/lib_server nap; sua nong duoc (1 phut).",
    "cauhinh/ch_chung.lua": "CAU HINH CHUNG (ten server, gioi han, tham so he thong). Web CFGW (bang gcfg) co the ghi de gia tri khi khoi dong.",
    "cauhinh/ch_drop.lua": "CAU HINH ROI DO (ti le, bang roi).",
    "cauhinh/ch_exp.lua": "CAU HINH KINH NGHIEM (ExpRate, he so).",
    "cauhinh/ch_lib.lua": "HAM DOC/GHI CAU HINH (CH_*), noi voi web CFGW.",
    "cauhinh/ch_lich.lua": "LICH HOAT DONG (gio mo/dong theo ngay).",
    "cauhinh/ch_thuong.lua": "BANG PHAN THUONG (id vat pham, so luong).",
    "cauhinh/ch_thuong_lib.lua": "HAM PHAT THUONG theo bang ch_thuong.",
    "cauhinh_web/cfgw_driver.lua": "CFGW: nap bang gcfg (MySQL) va ap len bien cau hinh Lua khi boot / theo lenh (04-05/09).",
    "cauhinh_web/cfgw_meta.lua": "CFGW: TU DIEN KHOA cau hinh (ten/giai thich/canh bao/nguy co) hien tren web admin. Chi la du lieu.",
    "class/ktabfile.lua": "Doc BANG .TXT tab (KTabFile): GetTabFileHeight/GetTabFileValue... dung boi awardtemplet va hoat dong JX2.",
    "class/lerror.lua": "Bao loi kieu JX2 (lerror).",
    "script_protocol/protocol_def_gs.lua": "DINH NGHIA GIAO THUC SCRIPT phia GameServer (MODEL_GAMESERVER=1). Cap voi client script_protocol/protocol_def_c.lua.",
    "script_protocol/echo_gs.lua": "Handler ECHO thu kenh ScriptProtocol (MAIL 03/09).",
    "startgame/citywar_boot.lua": "Boot CONG THANH JX2 luc khoi dong (dot E5).",
    "startgame/lmbj_addnpc.lua": "Dat NPC VAN TIEU luc khoi dong.",
    "startgame/npccreate.lua": "OnCreate cho NPC (C++ KNpcSet.cpp goi ExecuteScript theo ten tep nay).",
    "startgame/npclib.lua": "Ham dat NPC.",
    "startgame/npcpos.lua": "TOA DO NPC toan the gioi (27.304 dong, DU LIEU). Sua toa do NPC o day.",
    "startgame/npcposarray.lua": "Mang toa do NPC (du lieu).",
    "startgame/tongjx2npc.lua": "Dat NPC BANG HOI JX2 (goi tu OnGame).",
    "startgame/traplib.lua": "BANG TRAP (1739 dong, du lieu) dat luc khoi dong.",
    "global/autoexec_head.lua": "AutoFunctions: danh sach ham chay tu dong luc boot (chi trong state nay - JX1 moi tep mot state).",
    "global/autoexec_npc_hd3.lua": "Tu dat NPC 3 hoat dong luc boot.",
    "global/bot_tongkim.lua": "BOT tham gia Tong Kim (21/08).",
    "global/equip_system.lua": "NPC THO REN - cua vao he lo ren (dung luyen, cuong hoa).",
    "global/hocvocong.lua": "HOC VO CONG (1070 dong): hoc/nang ky nang tai NPC.",
    "global/itemset.lua": "BO TRANG BI (910 dong, du lieu) - Include boi ~286 tep.",
    "global/maplist.lua": "DANH SACH MAP (du lieu).",
    "global/seasonnpc.lua": "DA TAU - chuoi nhiem vu theo mua (1439 dong, 53 ham).",
    "global/skills_table.lua": "BANG KY NANG (1539 dong): ten, id, he, cap.",
    "global/smelt_system.lua": "DUNG LUYEN (port Linux 01/09).",
    "global/station.lua": "DICH TRAM (dich chuyen thanh thi).",
    "global/titlefuncs.lua": "DANH HIEU (title).",
    "global/trangbihoangkim.lua": "TRANG BI HOANG KIM (55 ham).",
    "global/trangbisieunhan.lua": "TRANG BI SIEU NHAN (70 ham).",
    "global/trangbixanh.lua": "TRANG BI XANH.",
    "global/vatpham.lua": "Ham VAT PHAM toan cuc (22 ham).",
    "global/yunbiao_system.lua": "VAN TIEU (yunbiao) - cong dung chung.",
    "global/script_protocol.lua": "Giao thuc script toan cuc (C++ SCRIPT_PROTOCOL_FILE nap truc tiep - KHONG doi cho).",
    "global/judgeoffline_limit.lua": "Gioi han offline / phan xu.",
    "global/onkillnpc.lua": "OnKillNpc toan cuc (DONDATAU 30/08).",
    "global/nhadich_dailao.lua": "Nha dich Dai Lao.",
    "global/signet_head.lua": "An (signet).",
    "global/TalkLib.lua": "Thu vien hoi thoai (Say).",
    "global/TimerHead.lua": "Ham timer (trung header/timerhead.lua).",
    "global/global_tiejiang.lua": "Tho ren toan cuc.",
    "global/fuyuan.lua": "Phuc Duyen (ban global).",
    "global/forbidmap.lua": "Map cam (ban global).",
    "global/map_helper.lua": "Tro giup map (ban global).",
    "global/repute_head.lua": "Danh vong (port Linux 01/09).",
    "global/seasonnpc_death.lua": "Da Tau: xu ly NPC chet.",
    "misc/taskmanager.lua": "QUAN LY NHIEM VU JX2 (task manager) - Include boi ~286 tep.",
    "misc/league_cityinfo.lua": "Thong tin thanh LIEN DAU.",
    "maps/newworldscript_default.lua": "Script map mac dinh: OnNewWorld/OnLeaveWorld (MapList.ini tro newworldscript.lua khong ton tai -> ban compat nay).",
    "vng_lib/bittask_lib.lua": "Bit task (VNG).", "vng_lib/extpoint.lua": "Diem mo rong (VNG).", "vng_lib/extpoint_head.lua": "Dinh nghia diem mo rong.",
    "vng_lib/files_lib.lua": "Doc/ghi tep (VNG).", "vng_lib/taskweekly_lib.lua": "Dem hoat dong theo tuan vao task.", "vng_lib/vngtranslog.lua": "Log giao dich VNG.",
    "log_game/save_log.lua": "HAM GHI LOG CHUNG cua game (21 ham) - log_game/*.lua dung.",
    "log_game/log_giaodich.lua": "Log giao dich.", "log_game/log_tong.lua": "Log bang hoi.", "log_game/log_kytrancac.lua": "Log Ky Tran Cac.",
    "log_game/log_gayroi.lua": "Log gay roi.", "log_game/danhsach_bang.lua": "Danh sach bang (du lieu).",
    "timertask/pubg.lua": "PUBG: viec theo lich.", "timertask/pubgend.lua": "PUBG: ket thuc tran.",
    "npclevelscript/npclevelscript.lua": "AI + THUOC TINH NPC MAC DINH theo cap (C++ KCore.cpp:588 nap TRUC TIEP - KHONG doi cho). 14 ham chuan: OnCreate, GetAttrib...",
    "npclevelscript/property.lua": "Ham tinh thuoc tinh NPC theo cap (23 ham).",
    "npclevelscript/lib.lua": "Ham chia tham so cho npclevelscript.",
    "npclevelscript/supernpc.lua": "Tinh thuoc tinh NPC dac biet (super).",
}
GENERIC = {
    "timertask": "VIEC THEO LICH goi tu timerserver.lua (RunTime): taskNN.lua = nhom viec so NN (task04-07 rong tu 30/08).",
    "npclevelscript": "AI + THUOC TINH NPC theo cap (cot LevelScript trong settings/npcs.txt tro toi tep nay theo ID bam). 14 ham chuan: OnCreate/OnDeath/GetAttrib...",
    "global": "Thu vien TOAN CUC (global) - nap boi Include tu nhieu tep.",
    "lib": "Thu vien dung chung (lib).",
    "header": "Dinh nghia/du lieu dau (header).",
}
HE_THONG_DIRS = [".", "lib", "header", "cauhinh", "cauhinh_web", "class", "script_protocol", "startgame", "global", "misc",
                 "vng_lib", "log_game", "timertask", "maps", "npclevelscript"]
BO_QUA = {"codenew.lua", "giftcode_new.lua", "giftcode_fancung.lua", "LUA54_DA_CHUYEN.txt"}

rx_fn = re.compile(r'^\s*(?:local\s+)?function\s+([A-Za-z_][A-Za-z0-9_.:]*)\s*\(', re.M)
rx_inc = re.compile(r'Include\s*\(\s*"([^"]+)"')

# ai Include tep nay (quet toan cay)
includers = collections.defaultdict(set)
for dp, dn, fn in os.walk(ROOT):
    if os.sep + "_luutru" in dp: continue
    if not (dp.startswith(os.path.join(ROOT, "script")) or dp.startswith(os.path.join(ROOT, "scriptjx2"))): continue
    for f in fn:
        if not f.lower().endswith(".lua"): continue
        p = os.path.join(dp, f)
        try: t = open(p, "rb").read().decode("latin-1")
        except Exception: continue
        for m in rx_inc.finditer(t):
            k = m.group(1).replace(BS + BS, BS).replace(BS, "/").lower().lstrip("/")
            includers[k].add(os.path.relpath(p, ROOT).replace(os.sep, "/").lower())

def header_for(rel, t):
    # rel: 'lib/x.lua' hoac 'timerserver.lua' (thu muc script/)
    key = rel
    mota = MOTA.get(key)
    if mota is None:
        d = rel.split("/")[0] if "/" in rel else "."
        mota = GENERIC.get(d, "Tep he thong (chua co mo ta rieng).")
    fns = [(m.group(1), t.count("\n", 0, m.start()) + 1) for m in rx_fn.finditer(t)]
    incs = [m.group(1).replace(BS + BS, BS) for m in rx_inc.finditer(t)]
    who = sorted(includers.get(("script/" + rel).lower(), []))
    full = ("script/" + rel).lower()
    hot = full in closure
    direct = full in cpp
    lines = ["-- ================================================================================================",
             "-- [HE THONG] script/" + rel,
             "-- Muc dich  : " + mota]
    if who:
        lines.append("-- Duoc nap  : Include tu %d tep (vd %s)%s" % (len(who), ", ".join(os.path.basename(w) for w in who[:4]), "; engine nap moi .lua thanh 1 lua_State luc boot" if True else ""))
    else:
        lines.append("-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)" + (" + C++ goi truc tiep theo ten tep" if direct else ""))
    if incs:
        lines.append("-- Include   : " + ", ".join(os.path.basename(i) for i in incs[:12]) + (" ... (%d)" % len(incs) if len(incs) > 12 else ""))
    if fns:
        s = ", ".join("%s (%d)" % (n, ln) for n, ln in fns[:24])
        if len(fns) > 24: s += " ... (%d ham)" % len(fns)
        lines.append("-- Ham (dong): " + s)
    else:
        lines.append("-- Ham       : (khong co - tep du lieu/cau hinh)")
    lines.append("-- Sua nong  : " + ("CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep" if hot else "KHONG - can restart GameServer (hoac lenh GM nap lai script)"))
    lines.append("-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py")
    lines.append("-- ================================================================================================")
    return lines

changed, skipped = [], []
for d in HE_THONG_DIRS:
    dd = SCRIPT if d == "." else os.path.join(SCRIPT, d)
    if not os.path.isdir(dd): continue
    for f in sorted(os.listdir(dd)):
        if not f.lower().endswith(".lua") or f in BO_QUA or f.startswith("_"): continue
        p = os.path.join(dd, f)
        rel = f if d == "." else d + "/" + f
        b = open(p, "rb").read()
        t = b.decode("latin-1")
        if "[HE THONG]" in t[:600]:
            skipped.append(rel); continue
        eol = "\r\n" if t.count("\r\n") * 2 > t.count("\n") else "\n"
        hdr = eol.join(header_for(rel, t)) + eol
        hdr = "".join(ch if ord(ch) < 256 else "?" for ch in hdr)   # ten Han (GBK) giu byte goc; ky tu ngoai latin-1 -> ?
        if DO:
            open(p, "wb").write(hdr.encode("latin-1") + b)
        changed.append(rel)
print("Chen chu thich [HE THONG]: %d tep%s; bo qua (da co): %d" % (len(changed), "" if DO else " (KHO)", len(skipped)))

# ---------------------------------------------------------------- _MUCLUC.lua
MUCLUC = r'''-- =====================================================================================================
-- _MUCLUC.lua - MUC LUC CAY SCRIPT MAY CHU (sap xep 06/09/2026). Tep nay CHI LA CHU THICH, engine bo qua ten bat dau '_'.
-- =====================================================================================================
-- BO CUC (tieng Viet khong dau):
--   (goc)         timerserver.lua (dong ho moi phut), timerserver_ctc.lua, protocol.lua, startgame.lua (boot), gmscript.lua, gb_taskfuncs.lua
--   lib/          thu vien dung chung (lib_task, lib_ham, lib_map, lib_vatpham, awardtemplet, objbuffer_head, remoteexc...)
--   header/       dinh nghia dau (taskid = ID bien nhiem vu, factionhead = mon phai, cauhinh_hoatdong, tongkim, map_helper...)
--   cauhinh/      CAU HINH may chu (ch_chung, ch_exp, ch_drop, ch_lich, ch_thuong) - sua o day, web CFGW co the ghi de
--   cauhinh_web/  CFGW: nap bang gcfg tu MySQL + tu dien khoa
--   class/        ktabfile (doc bang .txt), lerror
--   script_protocol/  giao thuc script GS <-> client
--   startgame/    dat NPC/trap luc boot (npcpos.lua 27k dong = toa do NPC, traplib.lua)
--   global/       thu vien toan cuc (hocvocong, trangbi*, itemset, seasonnpc = Da Tau, smelt_system = dung luyen, station...)
--   npclevelscript/  AI + thuoc tinh NPC theo cap (npclevelscript.lua = mac dinh, C++ nap truc tiep)
--   timertask/    viec theo lich goi tu timerserver; misc/ taskmanager JX2; maps/ script map mac dinh; log_game/ ghi log; vng_lib/
--   nhanvat/      kynang (skill), nguoichoi (player), thucung (petsys), donghanh (partner)
--   vatpham/      script vat pham (item cu) - dung khi dung vat pham; con: kiemthu/ chua *_admin
--   giaodich/     thu (mail), daugia (auction_house)
--   tinhnang/     phuban (missions), sukien (event), chientruong (battles), liendau, songbac, activitysys, chienlenh, congthanh,
--                 vng_event, vng_feature, nationalwar, huoyuedu, baucua, bonusvlmc, trapcu (trap chuyen map cu), 3hoatdong, tong_kim_tcap, pubg...
--   nhiemvu/      task cu (tollgate, newtask, partner, system, metempsychosis...), hoithoai (dailogsys), tanthu (tagnewplayer)
--   npc/          npcthon cu (npcmonphai, balanghuyen...), thanhthi, khac
--   dulieu/       codenew.lua (32k dong), giftcode_new.lua, giftcode_fancung.lua - CHI DU LIEU
--   kiemthu/      test/, *_admin.lua, tmp - KHONG dung cho nguoi choi
--   ../scriptjx2/ lib (ban JX2 khi script/lib thieu - remap C++), tong_vn (bang hoi JX2, engine nap rieng)
--   ../_luutru/0609/  kho: 1.371 tep Han khong dung, ban cu, backup, Lua 4
--
-- DUONG DAN CU VAN CHAY: script/_duongdan_cu.txt (dong "--@ cu=moi"). Include/dofile/ID bam (trap, settings) tu tra bi danh.
--   Them tep MOI: ghi thang vao thu muc moi. Doi cho tep: them dong bi danh. Kiem: python tools/sapxep/kiem_duongdan_cu.py
--
-- QUY UOC VIET SCRIPT (Lua 5.4 + lop tuong thich lua4compat):
--   - ten ham cu (getn, tinsert, format, floor, random, strfind, date, call...) van dung; KHONG dung %x (viet x); for k, v in pairs(t) do;
--     ham '...' khai local arg = {n = select("#", ...), ...}; chuoi co '\' phai la thoat hop le.
--   - Moi tep .lua = MOT lua_State rieng: bien toan cuc cua tep khac KHONG thay duoc; muon dung chung thi Include.
--   - Include KHONG khu trung nhung da co cache (Lua54Dll): Include lai van chay lai than tep -> dung "X = X or {}" khi khai bang.
--   - Sua nong: tep trong chuoi Include cua timerserver.lua tu nap lai <= 1 phut; tep khac can restart.
--   - Kiem truoc khi ghi: python ReverseTools/lua54/kiem_54.py <tep|thu muc>; loi luc chay ghi ScriptError.log trong THU MUC cua tep loi.
--   - Chu Viet trong chuoi: TCVN3 raw (dung vn_edit.py), chu thich: ASCII khong dau.
--
-- DO HIEU NANG: jx_perf_server.log moi phut (TICK, SW_ACTIVATE, LUA_CALL = tong ms Lua/tick, SCRIPT_TIME = RunTime);
--   console luc boot: "[script] LoadAllScript: N tep, M ms; cache Include ..." va "[script] Bi danh duong dan: ...".
-- =====================================================================================================
'''
if DO:
    open(os.path.join(SCRIPT, "_MUCLUC.lua"), "wb").write(MUCLUC.replace("\n", "\r\n").encode("latin-1"))
    print("da ghi script/_MUCLUC.lua")
open(os.path.join(SP, "ghi_chu_danhsach.txt"), "w", encoding="utf-8").write("\n".join(changed) + "\n")
