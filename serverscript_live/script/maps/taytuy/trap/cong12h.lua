
function main(sel)

if ( GetFightState() == 0 ) then
	SetPos(1587,3168)		
	SetFightState(1)		
	SetProtectTime(18*3) --ba gi©y b¶o vÖ thêi gian 
	AddSkillState(963, 1, 0, 18*3) 	
	else			       	
	SetPos(1596,3179)		
	SetFightState(0)		
end;
end;