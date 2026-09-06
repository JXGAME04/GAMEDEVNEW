--Fong Kieu Fi Fong

PHI_PHONG = {
	[1] = 24,
	[2] = 24,
	[3] = 24,
	[4] = 24,
	[5] = 25,
	[6] = 25,
	[7] = 25,
	[8] = 26,
	[9] = 26,
	[10] = 26,
}

function main(nItemIdx)
	local nEQIndex,_,_,_,nEQLevel = FindItemEx(2,1)
	if(nEQLevel>0 and nEQLevel<=10) then
	local nEQMantle = GetMantleItem(nEQIndex)
		if(nEQMantle<=0) then
			if(RemoveItem(nItemIdx,1)==1) then
				SetMantleItem(nEQIndex,PHI_PHONG[nEQLevel])
			end
		end
	end
end