# -*- coding: ascii -*-
"""C12 - "boss Tin Su danh khong thay chieu nhung van mat mau" (chu bao 25/08 10:3x).
BANG CHUNG (jx_auto.log client pid 29704 + jx_auto_server.log):
  - server: E3_CANCAST_OK + E4_SKILL_COST skill=328, relation=0x8 (enemy) 62/62
  - client: 38/41 luot E3_RELATION relation=0x1 (relation_none) -> CanCastSkill
    tra 0 -> case 0 -> goto Exit -> E4_SKILL_ABORT => KHONG ve chieu;
    2 luot relation=0x8 (luc rut vu khi) thi ve duoc + 3 E4_SKILL_COST.
GOC: KNpcSet::GetRelation phia CLIENT (KNpcSet.cpp ~1554) ep relation_none cho
  cap (kind_normal -> local player) khi nguoi choi CHUA RUT VU KHI (fight_none).
  Skill 53 (don thuong) khong co co TargetEnemy nen khong qua gate quan he =>
  quai thuong ve binh thuong; skill 328 (Tam Nga Tu Tuyet - boss Tin Su/849)
  co enemy=1 => dinh gate => tang hinh.
VA: trong KSkill::CanCastSkill, CLIENT bo qua gate quan he khi LAUNCHER LA NPC
  (chieu den tu s2c_skillcast - may chu da tham dinh; client chi VE LAI).
  Khong dung GetRelation (giu nguyen hanh vi auto/click), khong dung server.
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KSkills.cpp"
d = io.open(P, encoding="latin-1", newline="").read()
if "chi VE LAI" in d:
    print("da va roi"); raise SystemExit
T = "\t"
NL = "\r\n" if "\r\n" in d else "\n"

anchor = NL.join([
    T*3 + "if ( nParam2 <= 0 || nParam2 >= MAX_NPC) return 0;",
    T*3 + "NPC_RELATION  Relation = NpcSet.GetRelation(nLauncher, nParam2);",
])
assert d.count(anchor) == 1, d.count(anchor)
ins = NL.join([
    T*3 + "if ( nParam2 <= 0 || nParam2 >= MAX_NPC) return 0;",
    "#ifndef _SERVER",
    T*3 + "// [3HD 25/08] Chieu cua NPC den tu s2c_skillcast - MAY CHU da tham dinh",
    T*3 + "// quan he + tam danh roi moi broadcast; client chi VE LAI. GetRelation",
    T*3 + "// phia client ep relation_none cho (quai -> nguoi choi CHUA RUT VU KHI)",
    T*3 + "// lam chieu co co TargetEnemy (vd 328 boss Tin Su) bi huy ve => \"danh",
    T*3 + "// tang hinh van mat mau\". Khong ap dung cho nguoi choi (van tham dinh).",
    T*3 + "if (!Npc[nLauncher].IsPlayer())",
    T*4 + "goto relationisvalid;",
    "#endif",
    T*3 + "NPC_RELATION  Relation = NpcSet.GetRelation(nLauncher, nParam2);",
])
d = d.replace(anchor, ins)
io.open(P, "w", encoding="latin-1", newline="").write(d)
print("OK KSkills.cpp - can build lai CoreClient Win32 (server khong doi hanh vi)")
