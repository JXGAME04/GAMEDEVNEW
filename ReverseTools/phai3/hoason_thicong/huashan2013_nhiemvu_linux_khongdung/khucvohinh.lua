Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
--   Edit by Youtube PGaming--

function OnDeath () 
if (GetTask(169) == 69) then
Msg2Player("§¹i hiÖp tiªu diÖt Khóc V« H×nh.") 
AddNote("§¹i hiÖp tiªu diÖt Khóc V« H×nh.")
SetTask(169,70)
DelCommonItem(6,1,4958)
end
end 


