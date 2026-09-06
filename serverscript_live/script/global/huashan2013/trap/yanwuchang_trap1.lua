Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
--华山派演武场trap点1
function main(sel)
	if ( GetFightState() == 0 ) then
		SetPos(1481, 2977)		
		SetFightState(1)	
	else			       	
		SetPos(1479, 2973)	
		SetFightState(0)	
	end
end
