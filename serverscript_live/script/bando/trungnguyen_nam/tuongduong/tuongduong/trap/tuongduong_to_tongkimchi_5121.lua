--中原南区 襄阳to宋金战场
--Trap ID：中原南区 49
Include("\\script\\battles\\battlehead.lua")
function main(sel)
	if ( GetLevel() < 40 ) then
		Talk( 1, "", "Chi課 trng T鑞g Kim gian kh?kh鑓 li謙, ngi ch璦 t n c蕄 40 h穣 v?luy謓 th猰 r錳 h穣 t輓h." );
	elseif ( GetLevel() >= 40 and GetLevel() < 80 ) then
		NewWorld( 324, 1541, 3178);
		SetFightState(0);
		Msg2Player( "дn n琲 b竜 danh Chi課 Trng T鑞g Kim S?C蕄" );
	elseif ( GetLevel() >= 80 and GetLevel() < 120 ) then
		NewWorld( 324, 1541, 3178);
		SetFightState(0);
		Msg2Player( "дn n琲 b竜 danh Chi課 Trng T鑞g Kim Trung C蕄" );
	else
		NewWorld( 324, 1541, 3178);
		SetFightState(0);
		Msg2Player( "дn n琲 b竜 danh Chi課 Trng T鑞g Kim Cao C蕄" );
	end
SetProtectTime(18*3)
	AddSkillState(963, 1, 0, 18*3) 
end;