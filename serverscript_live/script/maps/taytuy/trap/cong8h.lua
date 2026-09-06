
function main(sel)

if ( GetFightState() == 0 ) then
	SetPos(1583,3230)		
	SetFightState(1)		
	SetProtectTime(18*3) --ba gi©y b¶o vÖ thêi gian 
	AddSkillState(963, 1, 0, 18*3) 	
	else			       	
	SetPos(1590,3224)		
	SetFightState(0)		
end;
end;