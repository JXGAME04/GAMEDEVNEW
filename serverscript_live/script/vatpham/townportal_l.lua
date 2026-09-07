--Author: Fong KiÒu
--Date: 2021
--Funciton: Håi thÇn phï


Include("\\script\\header\\revivepos_head.lua")
Include("\\script\\header\\forbidmap.lua")
Include("\\script\\header\\taskid.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\Script\\Global\\station.lua")

function main(nItemIdx)
	
	if (GetFightState() == 0 or GetTaskTemp(99) == 1) or ( SubWorldIdx2ID( SubWorld ) >= 387 and SubWorldIdx2ID( SubWorld ) <= 395)then
		Msg2Player("HiÖn t¹i b¹n kh«ng thÓ sö dông bïa vÒ thµnh!")
		return
	end
	
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if ( checkSJMaps(nMapId) == 1 ) then
		Msg2Player("Lóc nµy b¹n kh«ng thÓ sö dông vËt phÈm nµy")
		return 1
	end
	
	local nSubWorldID = GetWorldPos()
	
	if (GetTaskTemp(99) == 1 ) or ( nSubWorldID >= 387 and nSubWorldID <= 395)then
		Talk(1,"","HiÖn t¹i ng­¬i kh«ng thÓ sö dông phï!")
		return
	end
	
	if (nSubWorldID >= 375 and nSubWorldID <= 386) then
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông phï.")
		return
	end
	
	if (nSubWorldID >= 416 and nSubWorldID <= 511) then
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông phï.")
		return
	end
	
	if (nSubWorldID == 995 or nSubWorldID == 44 or nSubWorldID == 197 or nSubWorldID == 208 or nSubWorldID == 209 or nSubWorldID == 210 or nSubWorldID == 211 or nSubWorldID == 212 or (nSubWorldID >= 213 and nSubWorldID <= 223)	or nSubWorldID == 341 or nSubWorldID == 342	or nSubWorldID == 175	or nSubWorldID == 337	or nSubWorldID == 338	or nSubWorldID == 339 or ( nSubWorldID >= 387 and  nSubWorldID <= 395 ) )then 
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông phï.")
		return
	end

	if (CheckAllMaps(nSubWorldID) == 1) then
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông phï.")
		return
	end;
	
	if (GetLevel() < 10) then
		Talk(1,"","Ng­êi ch¬i ph¶i ®¹t ®¼ng cÊp 10 trë lªn míi cã thÓ sö dông phï.")
		return
	end	
	
	--if(GetCash() <500) then
	--	Msg2Player("KhÊu trõ 500 l­îng thÊt b¹i")
	--return end
	--Pay(500)
	
	local itemname = GetNameItem(nItemIdx)
	local n_param = GetParamItem(nItemIdx)
	n_param = n_param - 1
	if (n_param <= 0) then
		RemoveItem(nItemIdx,1)
		Msg2Player(""..itemname.." ®· sö dông hÕt sè lÇn, kh«ng thÓ dïng ®­îc n÷a!")
	else
		SetParamItem(nItemIdx,n_param)
		Msg2Player(""..itemname.." cßn cã thÓ sö dông "..n_param.." lÇn")
	end
	UseTownPortal()
end