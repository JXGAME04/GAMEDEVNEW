function main(nItemIdx)
	if(GetLevel() < 80) then
		Talk(1,"","Ch­a ®ñ level 80 kh«ng thÓ sö dông")
		return
	end
	if (GetNpcSkillsExpRate() < 2) then
		AddSkillState(1555, 1, 1, 60*60*18, 1);
		RemoveItem(nItemIdx,1);
	else
	Talk(1,"","C¸c h¹ ®ang cßn thêi gian nh©n ®«i kinh nghiÖm luyÖn kü n¨ng");
	end
end