--Boss杀手任务接引npc脚本
--By LiuKuo 2005.3.25
Include ("\\script\\class\\ktabfile.lua")
Include ("\\script\\task\\newtask\\newtask_head.lua")
Include("\\script\\missions\\challengeoftime\\npc\\dragonboat_main.lua")
Include("\\script\\event\\birthday_jieri\\200905\\chuangguan\\chuangguan.lua");
Include("\\script\\event\\birthday_jieri\\200905\\class.lua");
Include("\\script\\lib\\common.lua");
Include("\\script\\lib\\log.lua")
-- 闯关活动每日排行榜
Include("\\script\\missions\\challengeoftime\\rank_perday.lua");
---- Describ图片描述
--DescLink_NieShiChen = "<#><link=image[147,167]:\\spr\\npcres\\enemy\\enemy154\\enemy154_pst.spr>聂弑尘：<link>";
--TSKID_KILLTASKID = 1082;	-- 接受的哪个任务
--TSKID_KILLERDATE	= 1192;	--接任务时的日期
--TSKID_KILLERMAXCOUNT	= 1193;	--每天杀人次数
--KILLER_MAXCOUNT		= 8;	--每天杀人上限8人

Include("\\script\\vng_feature\\challengeoftime\\npcNhiepThiTran.lua")
ContentList = {
	"<#>Nếu ai có đủ 160  sát thủ giản trong tay thì giấc mộng trở thành đệ nhất sát thủ đã xem như hoàn thành. Mỗi ngày chỉ cần hoàn thành <color=yellow>8nhiệm vụ<color>là được. <enter>Hãy tiếp nhận nhiệm vụ “thách thức thời gian” để thể nghiệm sức mạnh đồng độivà khả năng sát thủ của mình.",
	"<#>Nhiệm vụ sát thủ cấp 20/killer20",--2
	"<#>Nhiệm vụ sát thủ cấp 30/killer30",
	"<#>Nhiệm vụ sát thủ cấp 40/killer40",
	"<#>Nhiệm vụ sát thủ cấp 50/killer50",
	"<#>Nhiệm vụ sát thủ cấp 60/killer60",
	"<#>Nhiệm vụ sát thủ cấp 70/killer70",
	"<#>Nhiệm vụ sát thủ cấp 80/killer80",--8
	"<#>Nhiệm vụ sát thủ cấp 90/killer90",
	"<#>Ta đánh không muốn giết người đâu, hủy bỏ nhiệm vụ./cancel",	--10
	"<#>Máu chảy đầu rơi, tốt nhất là ta nên tránh xa/no",
	"<#>Ngươi đã hủy bỏ nhiệm vụ. Làm sát thủ trước tiên phải có thủ pháp siêu phàm, hai là hạ thủ vô tình, xem ra ngươi không thích hợp, không đi cũng không sao.",
	"<#>Người lần trước ta nói ngươi đi hạ thủ vẫn còn sống, hãy chứng minh thực lực của mình trước đi đã.",
	"<#>Đẳng cấp của ngươi không phù hợp, phải giao đấu cùng với người có đẳng cấp tương ứng mới được.",
	"<#> Đóng/no",	--15
	"<#> Đẳng cấp như thế có 20 sát thủ, tay mỗi người đều nhuốm đầy máu, ngươi muốn đánh với người nào?",
	"<#>Hợp thành sát thủ giản/compose",
	"<#>Sát thủ giản bạn để không đúng, việc quan sát và cặp mắt tinh tường là rất quan trọng.",
	"<#>Sát thủ lệnh bạn để quá nhiều, sát thủ cũng phải có nghệ thuật của nó, không thể nào mà ngay cả sinh mệnh của bản thân mình cũng không biết.",
	"<#>Sát thủ lệnh bạn để quá ít, sát thủ cũng phải có nghệ thuật của nó, không thể nào mà ngay cả sinh mệnh của bản thân mình cũng không biết.",	--20
	"<#>Hợp lại một lần nữa/compose",
	"<#>Bạn đã hợp thành một<color=",
	"<#>Thuộc tính<color> sát thủ giản, sát thủ giản là một sát thủ phi phàm. Bạn có thể dùng một sát thủ cùng cấp để so tài với sát thủ giản, quy tắc thắng thua được áp dụng theo quy tắc tương khắc của ngũ hành. ",
	"<#>Thử luyện sát thủ /annealofkiller",
	"<#>Tham gia khiêu chiến/want_playboat",	--25
	"<#>Sát thủ luyện thức trước tiên phải bắt đầu từ việc tham gia khiêu chiến, ngươi dám tiếp nhận nhiệm vụ chứ?",
	"<#>Liên quan đến khiêu chiến/aboutchallenge",
	"<#>Cứ mỗi giờ hệ thống sẽ thông báo 1 lần. Thời gian báo danh là 5 phút, thực hiện chỉ trong 30 phút. Mỗi người chỉ tối đa 2 lần/ngày. Phải đo đội trưởng đến báo danh. <enter>”Nhiệm vụ thách thức thời gian” gồm 2 khu vực tham gia. Sơ cấp: người chơi từ cấp 50 đến 89, do đội trưởng mang 2 sát thủ giản dưới cấp 90 (ngũ hành bất kỳ) đi báo danh. Cao cấp: người chơi từ cấp 90, do đội trưởng mang 2 sát thủ giản cấp 90 (ngũ hành bất kỳ) đi báo danh. <enter>Nội trong thời gian quy định, nếu vượt qua hết 28 ải sẽ hoàn thành. Mỗi ải phần thưởng kinh nghiệm sẽ khác nhau. Nếu hoàn thành nhiệm vụ trước thời gian hạn định, phần thưởng kinh nghiệm sẽ càng cao <enter>Nếu trước thời hạn đã hoàn thành, có thể sẽ xuất hiện thêm 1 ải, trong đó có nhiều phần thưởng bất ngờ (vật phẩm ngẫu nhiên, trang bị Hoàng Kim…). Chỉ những đội đã vượt 28 ải đúng thời gian quy định mới có tên trong bảng xếp hạng.",
	"<#>Nhiệm vụ \"Quá quan tầm bảo\"/guoguan_xunbao",
	"<#>Ta đến nhận thưởng/rank_award",
	"<#>Ta đến xem xếp hạng 5 đội cao nhất của hôm nay./get_top5team"
}

killertabfile = new(KTabFile,"/settings/task/tollgate/killer/killer.txt","KILLER")

function main()
	UWorld1082 = nt_getTask(1082);
	local tbDialog = {ContentList[24],ContentList[17],ContentList[9],ContentList[10],ContentList[11],ContentList[30], ContentList[31], ContentList[15]};
	if (tbBirthday0905:IsActDate() == 1) then
		tinsert(tbDialog, 12, ContentList[29]);
	end
	Describe(DescLink_NieShiChen..ContentList[1], getn(tbDialog), unpack(tbDialog));
end

function annealofkiller()
	Describe(DescLink_NieShiChen..ContentList[26], 4, ContentList[25], "<#> Kiểm tra điều kiện tổ đội/#tbCOT_Party:CheckCondition()",ContentList[27],ContentList[11]);
end

function aboutchallenge()
	Describe(DescLink_NieShiChen..ContentList[28],1, ContentList[15]);
end

--function killer20()
--	if ( killerCoundTakedTask(20, 29) == 0) then
--		return 0;
--	end;
--	showboss( 0 );
--end
--
--function killer30()
--	if ( killerCoundTakedTask(30, 39) == 0) then
--		return 0;
--	end;
--	showboss( 20 );
--end
--
--function killer40()
--	if ( killerCoundTakedTask(40, 49) == 0) then
--		return 0;
--	end;
--	showboss( 40 );
--end
--
--function killer50()
--	if ( killerCoundTakedTask(50, 59) == 0) then
--		return 0;
--	end;
--	showboss( 60 );
--end
--
--function killer60()
--	if ( killerCoundTakedTask(60, 69) == 0) then
--		return 0;
--	end;
--	showboss( 80 );
--end
--
--function killer70()
--	if ( killerCoundTakedTask(70, 79) == 0) then
--		return 0;
--	end;
--	showboss( 100 );
--end
--
--function killer80()
--	if ( killerCoundTakedTask(80, 89) == 0) then
--		return 0;
--	end;
--	showboss( 120 );
--end

function killer90()
	if ( killerCoundTakedTask(90, 350) == 0) then
		return 0;
	end;
	showboss( 140 );
	tbLog:PlayerActionLog("TinhNangKey","NhanNhiemVuBossSatThu")
end

function cancel()
	nt_setTask(1082, 0);
	Describe(DescLink_NieShiChen..ContentList[12], 1,ContentList[15]);
end

function havetask()
	if ( UWorld1082 ~= 0 ) then
		Describe(DescLink_NieShiChen..ContentList[13], 1,ContentList[15]);
		return 0
	end
	return 1
end

function showboss(row)
	Describe(DescLink_NieShiChen..ContentList[16], 12, killertabfile:getCell("BossName",row+1).."/#givetask("..(row+1)..")", killertabfile:getCell("BossName",row+2).."/#givetask("..(row+2)..")", killertabfile:getCell("BossName",row+3).."/#givetask("..(row+3)..")", killertabfile:getCell("BossName",row+4).."/#givetask("..(row+4)..")", killertabfile:getCell("BossName",row+5).."/#givetask("..(row+5)..")", killertabfile:getCell("BossName",row+6).."/#givetask("..(row+6)..")", killertabfile:getCell("BossName",row+7).."/#givetask("..(row+7)..")", killertabfile:getCell("BossName",row+8).."/#givetask("..(row+8)..")", killertabfile:getCell("BossName",row+9).."/#givetask("..(row+9)..")", killertabfile:getCell("BossName",row+10).."/#givetask("..(row+10)..")", " Trang kế /#showbossnext("..row..")", ContentList[15]);
end

function showbossnext(row)
	Describe(DescLink_NieShiChen..ContentList[16], 12, killertabfile:getCell("BossName",row+11).."/#givetask("..(row+11)..")", killertabfile:getCell("BossName",row+12).."/#givetask("..(row+12)..")", killertabfile:getCell("BossName",row+13).."/#givetask("..(row+13)..")", killertabfile:getCell("BossName",row+14).."/#givetask("..(row+14)..")", killertabfile:getCell("BossName",row+15).."/#givetask("..(row+15)..")", killertabfile:getCell("BossName",row+16).."/#givetask("..(row+16)..")", killertabfile:getCell("BossName",row+17).."/#givetask("..(row+17)..")", killertabfile:getCell("BossName",row+18).."/#givetask("..(row+18)..")", killertabfile:getCell("BossName",row+19).."/#givetask("..(row+19)..")", killertabfile:getCell("BossName",row+20).."/#givetask("..(row+20)..")", "Trang trước/#showboss("..row..")", ContentList[15]);
end

function givetask(taskid)
	nt_setTask(1082,taskid);
	Describe(DescLink_NieShiChen..killertabfile:getCell("BossName",taskid)..killertabfile:getCell("BossInfo",taskid), 1, ContentList[15]);
end

function compose()
	GiveItemUI("Giao diện hợp thành sát thủ giản","5 sát thủ lệnh cùng đẳng cấp sẽ hợp thành 1 sát thủ giản cấp tương ứng, thuộc tính của sát thủ giản được tạo thành có liên quan đến thu6ọc tính của 5 sát thủ lệnh. Bạn có thể dùng sát thủ giản của mình để so tài với sát thủ giản đồng cấp của người khác, quy tắc thắng thua được tính theo quy tắc tương khắc của ngũ hành.","exchange_token", "no")
end

function exchange_token(ncount)
	local scrollidx = {}
	local scrollattr = {}
	local y = 0
	local compare_level = 0
	for i=1, ncount do
		local nItemIdx = GetGiveItemUnit(i);
		itemgenre, detailtype, parttype, level, attribute = GetItemProp(nItemIdx)
		if (itemgenre == 6 and detailtype == 1 and parttype == 399  ) then	
			if( y > 0 ) then
				if( level ~= compare_level ) then
					Describe(DescLink_NieShiChen..ContentList[18], 2, ContentList[21], ContentList[15]);
					return
				end
			end
			y = y + 1;
			scrollidx[y] = nItemIdx;
			scrollattr[y] = attribute;
			compare_level = level;
		end
	end
	if( y ~= ncount) then
		Describe(DescLink_NieShiChen..ContentList[18], 2, ContentList[21], ContentList[15]);
		return
	end
	if( y > 5 ) then
		Describe(DescLink_NieShiChen..ContentList[19], 2, ContentList[21], ContentList[15]);
		return
	end
	if( y < 5 ) then
		Describe(DescLink_NieShiChen..ContentList[20], 2, ContentList[21], ContentList[15]);
		return
	end
	if( y == 5 ) then
		for i = 1, y do
			RemoveItemByIndex(scrollidx[i]);
		end
		givesword(scrollattr,compare_level);
	end
end

function givesword(attr,level)
	series = {"metal>Kim", "wood>Mộc", "water>Thủy", "fire>Hỏa", "earth>Thổ "};
	i = random( 1, 5 );
	AddItem( 6, 1, 400, level, attr[i], 0);
	j = attr[i] + 1;
	Describe(DescLink_NieShiChen..ContentList[22]..series[j]..ContentList[23], 1, ContentList[15]);
end

function no()
end

--每天接任务的限制
function killerCoundTakedTask(nLowLevel, nHighLevel)
	if ( havetask() == 0) then
		return 0;
	end;
	local myLevel = GetLevel();
	if( myLevel < nLowLevel or myLevel >  nHighLevel) then
		Describe(DescLink_NieShiChen..ContentList[14], 1,ContentList[15]);
		return 0;
	end;
	local nDate = tonumber(GetLocalDate("%y%m%d"));
	local myDate = nt_getTask(TSKID_KILLERDATE);
	if (myDate == nDate and nt_getTask(TSKID_KILLERMAXCOUNT) >= KILLER_MAXCOUNT) then
		Describe(DescLink_NieShiChen.."Sát thủ có một tố chất rất quan trọng gọi là khinh kẻ bại trận.  Hôm nay ngươi đã bị hạ"..KILLER_MAXCOUNT.."  người rồi, ngày mai hãy quay lại.", 1, ContentList[15]);
		return 0;
	elseif (myDate ~= nDate) then
		nt_setTask(TSKID_KILLERMAXCOUNT, 0);
		nt_setTask(TSKID_KILLERDATE, nDate);
	end;
	return 1;
end;
