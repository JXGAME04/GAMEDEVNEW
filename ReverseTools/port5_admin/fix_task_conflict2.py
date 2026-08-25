# -*- coding: utf-8 -*-
r"""[24/08] VA NOT 8 CAP TASK ID TRUNG (chu game duyet: ban local).

Nguyen tac: GIU he co nhieu cho dung / he goc; DOI he con lai sang dai TRONG 4129-4138
(MAX_TASK = MAX_TEMP_TASK = 4200, da do: 353 id dang dung, 4126-4199 trong).
Uu tien doi HANG CO TEN (chi sua 1 dong khai bao) - an toan hon sua so tran rai rac.

| task | GIU (he nhieu cho dung)              | DOI sang |
|  300 | T_SAVE_TK_KILLPLAYER (Tong Kim)      | citywar_arena toa do -> 4129 |
|  301 | T_SAVE_TK_KILLNPC                    | citywar_arena       -> 4130 |
|  302 | T_SAVE_TK_DEATH                      | citywar_arena       -> 4131 |
|   57 | SetTask(57) moc cap nhat ky nang     | TSK_NV_THIENVUONG   -> 4132 |
|   53 | TASK_DUNGCHUNG3 (26 lan, packed)     | TSK_HAI_QUA_HH_NGAY -> 4133 |
|   12 | T_TIMDOCHI (39 lan)                  | TASK_DUNGCHUNG2     -> 4134 |
|   11 | TASK_DUNGCHUNG (18 lan, packed)      | T_TAYTUY_TYPE       -> 4135 |
|   44 | TASK_NVTHONGTHUONG (7 lan, packed)   | T_REPCAUHOI         -> 4136 |
|   17 | SetByte map Vo Dang (15 lan, packed) | T_USEHATHKIM        -> 4137 |
| 21 T | SetTaskTemp(21) diem ky nang tam     | TMP_BAYTK (temp)    -> 4138 |
"""
import io, os, re, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def bak(p):
    if not os.path.isfile(p + ".truoc_taskfix2_2408"):
        shutil.copyfile(p, p + ".truoc_taskfix2_2408")

# ---------- A. citywar_arena / citywar_global: 300/301/302 -> 4129/4130/4131 ----------
AMAP = {"300": "4129", "301": "4130", "302": "4131"}
for rel, pairs in (
    (r"missions\citywar_arena\head.lua",        [("GetTask(300), GetTask(301), GetTask(302)", "GetTask(4129), GetTask(4130), GetTask(4131)")]),
    (r"missions\citywar_arena\manager.lua",     [("SetTask(300, M)", "SetTask(4129, M)"), ("SetTask(301, X)", "SetTask(4130, X)"), ("SetTask(302, Y)", "SetTask(4131, Y)")]),
    (r"missions\citywar_global\infocenter_head.lua", [("SetTask(300, M)", "SetTask(4129, M)"), ("SetTask(301, X)", "SetTask(4130, X)"), ("SetTask(302, Y)", "SetTask(4131, Y)")]),
):
    p = os.path.join(E, rel)
    if not os.path.isfile(p): print("bo qua %s" % rel); continue
    s = rd(p)
    if "4129" in s: print("%-42s da va" % rel); continue
    n = 0
    for old, new in pairs:
        c = s.count(old)
        if c:
            s = s.replace(old, new); n += c
    assert n == len(pairs), "%s: doi %d/%d" % (rel, n, len(pairs))
    NL = "\r\n" if "\r\n" in s else "\n"
    s = ("-- [FIX TRUNG TASK 24/08] 300/301/302 la T_SAVE_TK_KILLPLAYER/KILLNPC/DEATH cua TONG KIM" + NL +
         "-- (lib\\lib_task.lua) - dau truong ghi de len thong ke Tong Kim. Doi sang 4129/4130/4131." + NL) + s
    bak(p); wr(p, s)
    print("%-42s doi %d cho" % (rel, n))

# ---------- B. cac HANG CO TEN: sua dung 1 dong khai bao ----------
CONSTS = [
    (r"lib\lib_task.lua", "TSK_NV_THIENVUONG", 57, 4132, "57 = moc cap nhat ky nang (global\\skills_table.lua:10)"),
    (r"lib\lib_task.lua", "TSK_HAI_QUA_HH_NGAY", 53, 4133, "53 = TASK_DUNGCHUNG3 (packed-digit, 26 cho dung)"),
    (r"lib\lib_task.lua", "T_TAYTUY_TYPE", 11, 4135, "11 = TASK_DUNGCHUNG (packed-digit, 18 cho dung)"),
    (r"lib\lib_task.lua", "T_REPCAUHOI", 44, 4136, "44 = TASK_NVTHONGTHUONG (packed-digit)"),
    (r"lib\lib_task.lua", "T_USEHATHKIM", 17, 4137, "17 = SetByte cua map Vo Dang (packed-byte, 15 cho dung)"),
    (r"lib\lib_task.lua", "TMP_BAYTK", 21, 4138, "TaskTemp 21 = diem ky nang tam (global\\skills_table.lua:20)"),
    (r"header\taskid.lua", "TASK_DUNGCHUNG2", 12, 4134, "12 = T_TIMDOCHI (39 cho dung)"),
]
for rel, name, old_id, new_id, why in CONSTS:
    p = os.path.join(E, rel)
    s = rd(p)
    rx = re.compile(r"^(\s*" + re.escape(name) + r"\s*=\s*)" + str(old_id) + r"\b", re.M)
    m = rx.search(s)
    if not m:
        if re.search(r"^\s*" + re.escape(name) + r"\s*=\s*" + str(new_id) + r"\b", s, re.M):
            print("%-22s da va" % name); continue
        raise AssertionError("khong thay khai bao %s = %d trong %s" % (name, old_id, rel))
    s = s[:m.end(1)] + str(new_id) + "\t-- [FIX TRUNG TASK 24/08] truoc la " + str(old_id) + "; " + why + s[m.end(0):]
    bak(p); wr(p, s)
    print("%-22s %4d -> %4d  (%s)" % (name, old_id, new_id, os.path.basename(rel)))

print("XONG")
