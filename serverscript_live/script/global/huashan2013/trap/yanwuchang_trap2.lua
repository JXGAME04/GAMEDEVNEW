Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
--华山派演武场trap点2
function main(sel)
	if ( GetFightState() == 0 ) then
		SetPos(1475, 2994)		
		SetFightState(1)	
	else			       	
		SetPos(1473, 2998)	
		SetFightState(0)	
	end
end
