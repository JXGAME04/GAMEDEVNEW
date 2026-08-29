# -*- coding: utf-8 -*-
"""BAN DONG HANH - G5b: noi cot Script trong magicscript.txt cho item dong hanh.

JX1 id (da nan -1 tu Linux):
  833..837  -> \script\item\partner_skillbook_resist.lua    (sach khang, skill 549-553)
  838..847  -> \script\item\partner_skillbook_ultimate.lua  (vo thuong tam phap 558-583)
  848..883  -> \script\item\partner_skillbook_learnt.lua    (phu tro 584-621)
  900       -> learnt (skill 660)
  1064      -> learnt (skill 704)
  446..826  -> \script\item\reward_partner.lua              (381 cuon tu luyen)
  828       -> \script\item\reward\reward_education.lua     (cuon Long Vuong Dai)
Sua CA server LAN client (hai ban rieng). Chi dong cot 9 (Script) khi dang "0".
Giu nguyen byte moi cot khac + CRLF. Sao luu .truoc_bdh_g5. Idempotent.
"""
import os
import shutil

BS = chr(92)
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

SCRIPTS = {}
p_resist = BS + "script" + BS + "item" + BS + "partner_skillbook_resist.lua"
p_ult = BS + "script" + BS + "item" + BS + "partner_skillbook_ultimate.lua"
p_learnt = BS + "script" + BS + "item" + BS + "partner_skillbook_learnt.lua"
p_reward = BS + "script" + BS + "item" + BS + "reward_partner.lua"
p_edu = BS + "script" + BS + "item" + BS + "reward" + BS + "reward_education.lua"
for i in range(833, 838):
    SCRIPTS[i] = p_resist
for i in range(838, 848):
    SCRIPTS[i] = p_ult
for i in range(848, 884):
    SCRIPTS[i] = p_learnt
SCRIPTS[900] = p_learnt
SCRIPTS[1064] = p_learnt
for i in range(446, 827):
    SCRIPTS[i] = p_reward
SCRIPTS[828] = p_edu


def noi(base):
    p = os.path.join(base, r"settings\item\magicscript.txt")
    raw = open(p, "rb").read()
    rows = raw.split(b"\n")
    n_doi = 0
    n_dacoscript = 0
    for pid, path in SCRIPTS.items():
        ln = pid + 1
        if ln >= len(rows):
            continue
        c = rows[ln].split(b"\t")
        if len(c) <= 9:
            continue
        cu = c[9].strip()
        muon = path.encode("latin-1")
        if cu == muon:
            continue
        if cu != b"0" and cu != b"":
            n_dacoscript += 1
            print("  ! id %d dang co script khac: %s (GIU NGUYEN)" % (pid, cu[:50]))
            continue
        c[9] = muon
        rows[ln] = b"\t".join(c)
        n_doi += 1
    if n_doi:
        if not os.path.exists(p + ".truoc_bdh_g5"):
            shutil.copyfile(p, p + ".truoc_bdh_g5")
        # ghi tep tam roi thay (an toan nhu bangtxt)
        tmp = p + ".tmp_bdh"
        open(tmp, "wb").write(b"\n".join(rows))
        os.replace(tmp, p)
    print("%s: doi %d dong, %d dong da co script khac" % (os.path.basename(base), n_doi, n_dacoscript))


noi(SRV)
noi(CLI)
print("XONG b08")
