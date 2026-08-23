IncludeLib("SETTING");

function tongwar_want2signup()
	local tongname, mytongid = GetTongName();
	if (FALSE(tongname)) then
		CreateTaskSay({"<dec><npc>Ch­a gia nhËp bang, kh«ng thÓ ®i vµo Khu vùc chuÈn bÞ!", "KÕt thóc ®èi tho¹i/OnCancel"});
		return
	end
	local n_lid = LG_GetLeagueObjByRole(TONGWAR_LGTYPE, tongname)
	if (FALSE(n_lid)) then
		CreateTaskSay({"<dec><npc>Bang <color=red>"..tongname.."<color> kh«ng cã liªn minh víi c¸c bang héi chiÕm thµnh, kh«ng thÓ ®i vµo §Êu tr­êng chuÈn bÞ.", "KÕt thóc ®èi tho¹i/OnCancel"})
		return
	end
	--VLDNB 15- ®iÒu kiÖn tham gia: ®· vµo bang tr­íc 0h ngµy 29/03 - Modified By Thanhld - 20140319
	-- [TONGWAR 23/08] bo dieu kien 'vao bang truoc 0h 29/03/2014' (DIEUKIEN #18 - moc mua VNG)
	--VLDNB15- Translife >= 4	
	-- [TONGWAR 23/08] chinh sach chu game: bo trung sinh, cong cap >= 90 (DIEUKIEN #19)
	if (GetLevel() < 90) then
		Talk(1, "", "Ph¶i ®¹t cÊp 90 trë lªn míi ®­îc vµo ®Êu tr­êng.")
		return
	end
	--VLDNB 10 kiÓm tra c¸c lo¹i mÆt n¹ t¨ng ®iÓm TK - Modified By DinhHQ - 20111013	
	-- [TONGWAR 23/08] id mat na Linux (p 482/447/450/446/647/806) -> du an khoa (particular, level)
	local tbForbidMaskInTongWar = {
				["48_3"] = 1,
				["44_8"] = 1,
				["44_7"] = 1,
				["45_1"] = 1,
				["64_8"] = 1,		
				["80_7"] = 1,		
			}
	local tbEquip = GetAllEquipment()
	for i=1, getn(tbEquip) do
		local nG, nD, nP, nLv = GetItemProp(tbEquip[i])	-- [TONGWAR 23/08] GetItemProp JX1 tra (g,d,p,lv,...)
		if nG == 0 and nD == 11 and tbForbidMaskInTongWar[nP.."_"..nLv] then
			CreateTaskSay({format("<dec><npc>Kh«ng ®­îc phÐp mang vËt phÈm <color=red>[%s]<color> vµo ®Êu tr­êng!", GetItemName(tbEquip[i])), "KÕt thóc ®èi tho¹i/OnCancel"});
			return
		end
	end
	
	local cityname = LG_GetLeagueInfo(n_lid)
	local matchmap = LG_GetLeagueTask(n_lid, TONGWAR_LGTASK_MAP)
	local matchcamp = LG_GetLeagueTask(n_lid, TONGWAR_LGTASK_CAMP)
	
	if (LG_GetLeagueTask(n_lid, TONGWAR_LGTASK_TONGID) == 0) then
		LG_ApplySetLeagueTask(TONGWAR_LGTYPE, cityname, TONGWAR_LGTASK_TONGID, mytongid);
	end;
	
	if (FALSE(matchmap) or GetGlbValue(850) ~= 1) then
		CreateTaskSay({"<dec><npc>"..cityname.."Ch­a ®Õn thêi ®iÓm vµo ®Êu tr­êng, h·y ®îi trong gi©y l¸t.", "KÕt thóc ®èi tho¹i/OnCancel"});
		return
	end
	if (FALSE(matchcamp)) then
		print("error the city"..cityname.." no camp!!!!!")
		return
	end
	local signmap, posx, posy = tongwar_getsignpos(matchmap, matchcamp)
	if (FALSE(signmap)) then
		print("error the city "..cityname.." matchmap"..matchcamp.." have no signmap")
		return
	end
	Msg2Player("B¹n ®· vµo"..cityname.." khu vùc chuÈn bÞ")
	NewWorld(signmap, posx, posy)
end

tbTONGWAR_SIGNMAP_POS = {
					{ 1582,3174 },
					{ 1588,3160 },
					{ 1604,3147 },
				}
	
tbTONGWARMAP = {
									{605, 608, 609},
									{606, 610, 611},
									{607, 612, 613},
							}
							
function tongwar_getsignpos(matchmap, camp)
	for i = 1, getn(tbTONGWARMAP) do
		if (matchmap == tbTONGWARMAP[i][1]) then
			signmap = tbTONGWARMAP[i][camp + 1]
			break
		end
	end
	local ranm = random(getn(tbTONGWAR_SIGNMAP_POS))
	return signmap, tbTONGWAR_SIGNMAP_POS[ranm][1], tbTONGWAR_SIGNMAP_POS[ranm][2]
end