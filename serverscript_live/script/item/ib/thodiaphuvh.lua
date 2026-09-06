--Author: Fong KiÒu
--Function: Thæ §Þa phï
--Date: 21/07/2021

Include("\\script\\header\\revivepos_head.lua")
Include("\\script\\header\\forbidmap.lua")
Include("\\script\\header\\taskid.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\Script\\Global\\station.lua")

function main(nIndex)

	-- dofile("script/item/ib/thodiaphuvh.lua")
	
	local nSubWorldID = GetWorldPos()
	
	if (GetTaskTemp(99) == 1 ) or ( nSubWorldID >= 387 and nSubWorldID <= 395)then
		Talk(1,"","HiÖn t¹i ng­¬i kh«ng thÓ sö dông thæ ®Þa phï!")
		return
	end
	
	if (nSubWorldID >= 375 and nSubWorldID <= 386) then
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thæ ®Þa phï.")
		return
	end
	
	if (nSubWorldID >= 416 and nSubWorldID <= 511) then
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thæ ®Þa phï.")
		return
	end
	
	if (nSubWorldID == 995 or nSubWorldID == 44 or nSubWorldID == 197 or nSubWorldID == 208 or nSubWorldID == 209 or nSubWorldID == 210 or nSubWorldID == 211 or nSubWorldID == 212 or nSubWorldID == 984 or (nSubWorldID >= 213 and nSubWorldID <= 223)	or nSubWorldID == 341 or nSubWorldID == 342	or nSubWorldID == 175	or nSubWorldID == 337	or nSubWorldID == 338	or nSubWorldID == 339 or ( nSubWorldID >= 387 and  nSubWorldID <= 395 ) )then 
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thæ ®Þa phï.")
		return
	end

	if (CheckAllMaps(nSubWorldID) == 1) then
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thæ ®Þa phï.")
		return
	end;
	
	if (GetLevel() < 10) then
		Talk(1,"","Ng­êi ch¬i ph¶i ®¹t ®¼ng cÊp 10 trë lªn míi cã thÓ sö dông thæ ®Þa phï.")
		return
	end
	if GetFightState() == 0 then
		return
	end
	UseTownPortal()
	--RemoveItem(nIndex)
	CheckPlayerTitle()
end

