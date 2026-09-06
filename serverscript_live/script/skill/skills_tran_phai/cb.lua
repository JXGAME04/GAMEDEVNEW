

id_skills =130
id_cl =230

function main(sel)
if (GetLevel() <= 89) then
skill_toi_da = GetTask(id_cl) + (GetLevel() - 59)
else
skill_toi_da = GetTask(id_cl) + 30
end
dang_cap_skill = GetMagicLevel(id_skills)
dang_cap_skill_nang = dang_cap_skill + 1
if (dang_cap_skill < skill_toi_da) then
if (GetMagicPoint()>=1) then
AddMagicPoint(-1)
AddMagic(id_skills,dang_cap_skill_nang )
Msg2Player("Ki nang linh ngo len cap "..dang_cap_skill_nang)
end
end

end;

		


 
