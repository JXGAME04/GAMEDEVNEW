# -*- coding: utf-8 -*-
"""C39 - GOC THAT cua "len thuyen khong dung duoc ky nang / khong danh duoc quai".

DO THUC TE, khong doan:
  fld_head.lua:  AddMSPlayer(MISSIONID, 1)      -> nguoi choi vao NHOM 1
  mission.lua:26 GetMSPlayerCount(MISSIONID, 0) -> dem NHOM 0

Engine JX1 (KMission.h GetGroupPlayerCount) co nhanh "nhom 0 = TAT CA" nhung
nhanh do BI COMMENT:
      //if (ucGroup == 0)
      //return GetPlayerCount();
=> GetMSPlayerCount(MISSIONID, 0) tra ve 0 (khong ai o nhom 0; ca cay script deu
   AddMSPlayer voi group >= 1) => vong "for i = 1, nCount" KHONG CHAY LAN NAO
   => KHONG AI duoc SetFightState(1) khi thuyen roi ben
   => nguoi choi ket o trang thai THU VU KHI (SetFightState(0) luc len thuyen)
   => "khong dung duoc ky nang", "khong danh duoc quai". KHOP 100% trieu chung.

Ban Linux (JX2) coi group 0 = tat ca nen script goc chay dung.
Luu y: GetNextPlayer(MISSIONID, idx, 0) thi JX1 LAI coi 0 = tat ca
(KMission.h GetNextPlayerC) - HAI HAM HIEU SO 0 KHAC NHAU.

VA (toi thieu, dung pham vi): bo tham so nhom o 4 cho cua Phong Lang Do
=> nParamCount == 1 -> nGroupId = -1 -> GetPlayerCount() = TAT CA (dung y Linux).
KHONG sua engine de khong doi hanh vi cua he khac.
"""
import io, os, re, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
TEP = [
    r"script\missions\fengling_ferry\mission.lua",
    r"script\missions\fengling_ferry\fld_smalltimer.lua",
]
RE = re.compile(r"GetMSPlayerCount\(\s*MISSIONID\s*,\s*0\s*\)")

tong = 0
for rel in TEP:
    p = os.path.join(JX1, rel)
    d = io.open(p, encoding="latin-1", newline="").read()
    n = len(RE.findall(d))
    if not n:
        print("  (khong co)", rel.rsplit("\\", 1)[-1])
        continue
    d2 = RE.sub("GetMSPlayerCount(MISSIONID)", d)
    io.open(p, "w", encoding="latin-1", newline="").write(d2)
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(p, dst)
    tong += n
    print("  OK %-24s %d cho" % (rel.rsplit("\\", 1)[-1], n))
print("da va", tong, "cho (bo tham so nhom => dem TAT CA nguoi trong mission)")

# canh bao cac he khac dinh cung loi (chi BAO CAO, khong sua)
print("\nCAC HE KHAC dinh cung loi (bao cao, KHONG tu sua):")
for dp, dn, fs in os.walk(os.path.join(JX1, "script")):
    for f in fs:
        if not f.endswith(".lua"):
            continue
        q = os.path.join(dp, f)
        try:
            t = io.open(q, encoding="latin-1", newline="").read()
        except Exception:
            continue
        for m in re.finditer(r"GetMSPlayerCount\(\s*([A-Za-z_0-9]+)\s*,\s*0\s*\)", t):
            print("   ", os.path.relpath(q, JX1), "->", m.group(0))
