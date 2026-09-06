--Function:Bo cau tong kim vËt phÈm tèng kim
--Author: Fong KiÒu
--Date: 2021

Include( "\\script\\header\\forbidmap.lua" )
Include( "\\script\\lib\\lib_task.lua" )

function main(nItemIdx)
	local W,X,Y = GetWorldPos()
	if ( checkSJMaps(W) ~= 1 ) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return 
	end
	local curcamp = GetCurCamp()
	X = floor( X /32/ 8 )
	Y = floor( Y /32/ 16 )
	local stringtb = "<color=yellow>"..GetName().."<color><color=pink>: 'Täa ®é hiÖn t¹i cña ta lµ <color=yellow> ("..X..", "..Y..")<color>. H·y ®Õn gióp ta mét tay.'"
	Msg2MSGroup(MS_TONGKIM, stringtb, curcamp-1)
	Msg2Player("B¹n sö dông bå c©u th«ng b¸o cho chiÕn h÷u täa ®é hiÖn t¹i cña b¹n.")
	RemoveItem(nItemIdx,1)
end
