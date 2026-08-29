# Ban sao script he BAN DONG HANH (petsys PC + partner) tu cay van hanh

Nguon: `E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server` (28/08/2026 dem).
Ap nguyen cau truc thu muc vao `bin/server` (va `bin_client` cho `pet_main.ini`).

KHONG kem 2 bang lon dung chung (chi sua vai dong - xem `BANGIAO_BANDONGHANH_2708.md`):
- `magicscript.txt` (server+client): 6 item moi id 4874..4879, khuon dong 3455
- `npcs.txt` (server+client): noi toi dong 2186 (2184 nangongjue, 2185 linhaotian)

Anh SPR/UI da Viet hoa khong kem (binary) - bo rut/ve lai nam o `ReverseTools/bandonghanh`.

## Cap nhat 29/08 trua
- + `script/petsys/xiuzhen.lua` (op 8), `bikip.lua` (hoc 18 skill, id*100+cap),
  `petequip.lua` (6 o trang bi V1 + Duc lai op 10)
- + `script/skill/petskill.lua` (KHUNG JX1 + SKILLS nguyen van VLTK - du lieu
  cap that cho 18 skill 1670..1687), `script/skill/petsys/aura.lua` (130 cap)
- settings/petsys: pet_skill_def.txt = ban PRIVATE 130 cap; levelup.txt noi
  21..130 theo quy luat tuyen tinh bang goc
- Bang lon da sua them (khong kem): magicscript.txt (item 4874..4886:
  Thiep/Thuoc/4 trai/Bi kip/6 trang bi - Script tro petsys), skills.txt
  (+18 dong 1670..1687 tu VLTK + 4 icon aura them duoi .spr)
- O task: 5139..5142 skill(id*100+cap); 5143..5148 equip; 5150..5155 pct;
  5157/5158 bonus HP/MP; 5160..5165 pct tam duc
