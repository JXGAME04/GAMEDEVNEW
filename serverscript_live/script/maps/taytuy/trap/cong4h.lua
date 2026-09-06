
function main(sel)

if ( GetFightState() == 0 ) then
	SetPos(1655,3240)		--
	SetFightState(1)		--
	SetProtectTime(18*3) --ba gi©y b¶o vÖ thêi gian 
	AddSkillState(963, 1, 0, 18*3) 	
	else			       	
	SetPos(1644,3231)		--
	SetFightState(0)		--
end;
end;