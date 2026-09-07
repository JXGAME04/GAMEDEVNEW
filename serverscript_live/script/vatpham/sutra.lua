-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local sutra_addexp
--Author: Fong Ki“u Item Bat nhuoc tam kinh

NUM20E = 2000000000

function main(nItemIdx)
	local ng,np = FindItemEx(nItemIdx)
	
	local exp=GetExp()
	
	if (exp < 0) then
		exp = exp*-1
		if (ng == 6 and np == 12) then
			if (exp >= NUM20E) then
				exp = NUM20E;
			end
		elseif (ng == 6 and np == 2392) then
			exp = floor (exp / 2)
--		elseif (ng == 6 and np == 2393) then
		end
		
		sutra_addexp(exp)
		Msg2Player("Sau khi nghi™n c¯u k¸ B∏t Nh≠Óc T©m Kinh, bπn c∂m th y toµn th©n khoan kho∏i, kinh nghi÷m th˘c chi’n hÂi phÙc kh´ng ›t. ")
		RemoveItem(nItemIdx,1)
		return 0
	end 
	
	Msg2Player("Bπn nghi™n c¯u B∏t Nh≠Óc T©m Kinh Æ≠Óc nˆa ngµy, nh≠ng v…n ch≠a l‹nh ngÈ Æ≠Óc g◊. ")
	return 1
end


function sutra_addexp(n_exp)
	local n_count = 100
	while (n_exp > 0) do
		if (n_exp >= 20e8) then
			AddOwnExp(20e8)
			n_exp = n_exp - 20e8
		else
			AddOwnExp(n_exp)
			n_exp = 0
			break
		end
	end
end
