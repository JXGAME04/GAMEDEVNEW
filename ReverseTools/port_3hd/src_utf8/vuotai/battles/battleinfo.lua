Include("\\script\\battles\\battlehead.lua")
BALANCE_MAMCOUNT = 5
strbattleinfo=
{
	"Tống Kim có những hình thức tham chiến sau, ngươi muốn tìm hiểu……",
	"Ngươi muốn hỏi gì?",
	"Các đạo cụ liên quan/info_sjthing",
	"Điểm tích lũy /info_adduppoint2",--版调用info_adduppoint2
	"Làm sao để theo dõi tin tức chiến trường/info_reportface",
	"Những điểm cần chú ý./info_note",
	"Ta đã biết hết rồi!/cancel",
	"Cách xem tin tức chiến sự: trong lúc tham chiến, có thể dùng phím ~ (phím trên phím Tab) để bật tắt báo cáo tóm tắt, thông qua đó có thể biết được thành tích của chính mình, và thông tin chiến tích của 10 người chơi cao nhất.",--即时战报界面
--	"玩家宋金信息界面说明：点击F12键，玩家可以看到自己在宋金大战中的详细统计信息。当前战役信息——玩家在当前战役周期内的具体表现；历史信息——玩家所参加的所有战役的累计信息。<enter>Crtl+鼠标右键点击其他玩家，再选择宋金信息，可以查看其他玩家的宋金大战信息。"--玩家宋金信息界面
}


function help_sjbattle()--两种夺旗规则
	Say(strbattleinfo[1],5,"Qui tắc đoạt kỳ (cố định) /info_fixation1","Qui tắc đoạt kỳ (ngẫu nhiên) /info_random1","Qui tắc bảo vệ Nguyên Soái/info_marshal","Qui tắc Tống Kim Đại Chiến/help_more","Ta hiểu rồi, cảm ơn!/cancel")
end


function help_more()--宋金大战其他相关信息
	Say(strbattleinfo[2],5,strbattleinfo[3],strbattleinfo[4],strbattleinfo[5],strbattleinfo[6],strbattleinfo[7])
end

function info_book()--宋金诏书
	Say("Tống Kim Chiếu Thư: Sử dụng sẽ được đưa thẳng đến địa điểm báo danh. Có thể mua tại tiệm tạp hóa trong các thành, giá <color=yellow>5000<color>lượng",2,"Tìm hiểu thông tin khác/help_more","Ta hiểu rồi!/cancel")
end
function info_flag()
	Say("Cờ hiệu: Sau khi di chuyển Soái kỳ đến đích, nhấp vào Cờ hiệu để cắm cờ. Lúc đang di chuyển có thể nhấp vào Cờ hiệu để xác định lại tọa độ. Mỗi Cờ hiệu chỉ dùng được 1 lần. Có thể mua ở Quân Nhu Quan, giá <color=yellow>5000<color>lượng.",2,"Tìm hiểu các thông tin khác/help_more","Ta hiểu rồi!/cancel")
end

function info_sjthing()
	Say("Đạo cụ Tống Kim đại chiến:",3,"Cờ hiệu /info_flag","Tống Kim Chiếu Thư /info_book","Ta hiểu rồi!/cancel")
end

stradduppointhelp=
{
--	"积分说明：玩家获得的积分不仅取决于自己在战场中的功绩，还取决于战友的表现和双方的人数。每个人获得的积分都会分成给自己的战友一部分。",
	"Người chơi giết được các loại NPC khác nhau (phân theo cấp bậc) , hoặc PK thắng quân địch (là người chơi) , PK thắng liên tiếp, đoạt báu vật, hoàn thành nhiệm vụ đoạt kỳ, phe giành được chiến thắng cuối cùng sẽ được cộng thêm điểm tích lũy. <enter>Khi chưa khai chiến, có thể dùng điểm tích lũy để mua các đạo cụ chuyên dụng. Sau khi khai chiến, không thể tiến hành giao dịch. ",
	"Quân số là một ưu thế trong chiến trường, nhưng thắng bại lại căn cứ vào điểm tích lũy.",
	"Liên trảm: Khi PK thắng quân địch 3 lần liên tiếp (đối phương thấp hơn mình không quá 3 cấp) sẽ được tính thêm điểm tích lũy.",
	"Khi vào tham chiến, dựa theo tổng điểm tích lũy, trên đầu người chơi sẽ xuất hiện tước hiệu. Thấp nhất là Binh sĩ; 1000 điểm = Hiệu úy; 3000 điểm = Thống Lĩnh; 6000 điểm = Phó tướng; 10 000 điểm = Đại Tướng.",
	"Sau khi kết thúc, phe thắng mỗi người tăng 3 điểm danh vọng và 1000 điểm tích lũy + 1 phần quà ngẫu nhiên; phe thua mỗi người 300 điểm. Nếu hòa nhau, mỗi bên được thưởng 500 điểm/người. Điểm thưởng còn căn cứ vào lực lượng của 2 bên.Bên nào ít người điểm thưởng sẽ cao hơn.",
	
}

function info_adduppoint2()--积分获得
	Say(stradduppointhelp[1],3,"Trang kế /info_adduppoint2_1","Tìm hiểu thông tin khác/help_more","Ta hiểu rồi!/cancel")
end

function info_adduppoint2_1()
	Say(stradduppointhelp[2],3,"Trang kế /info_adduppoint2_2","Tìm hiểu thông tin khác/help_more","Ta hiểu rồi!/cancel")
end

function info_adduppoint2_2()
	Say(stradduppointhelp[3],3,"Trang kế /info_adduppoint2_3","Tìm hiểu thông tin khác/help_more","Ta hiểu rồi!/cancel")
end

function info_adduppoint2_3()
	Say(stradduppointhelp[4],3,"Trang kế /info_adduppoint2_4","Tìm hiểu thông tin khác/help_more","Ta hiểu rồi!/cancel")
end

function info_adduppoint2_4()
	Say(stradduppointhelp[5],2,"Tìm hiểu thông tin khác/help_more","Ta hiểu rồi!/cancel")
end



function info_reportface()--即时战报界面
	Say(strbattleinfo[8],2,"Tìm hiểu thông tin khác/help_more","Ta hiểu rồi!/cancel")
end


strinfo_note=
{
	"Chú ý: sau khi báo danh sẽ được đưa vào hậu doanh. Từ đó có thể đến hậu doanh. Nhưng trước khi khai chiến không được rời khỏi đại doanh. Sau khi khai chiến có thể tùy ý xông ra chiến tuyến, cũng có thể tùy ý quay lại hậu doanh. Nhưng không thể xâm nhập vào hậu donah của đối phương.",--在后营的玩家不会记入积分统计。",
	"Khi người chơi đang ở trong trạng thái giữ Soái Kỳ, sẽ không thể đi ngược lại hậu doanh mua dược hoặc mở rương"--<enter>要注意在宋金大战结束前5分钟，会关闭宋金大战战场入口和战场中的后营出入口。处于后营的玩家可以下线离开战场，但是离开战场的玩家不会记入积分统计。",
}
function info_note()
	Say(strinfo_note[1],1,"Trang kế /info_note1")	
end

function info_note1()
	Say(strinfo_note[2],2,"Tìm hiểu thông tin khác/help_more","Ta hiểu rồi!/cancel")
end

strfixationhelp=
{
	"Phương thức đoạt cờ cố định: Sau khi khai chiến, song phương sẽ ngẫu nhiên xuất hiện 1 Soái Kỳ (hình dạng binh sĩ cầm cờ) , trên viết 1 chữ Tống hoặc Kim. Nhấp vào Soái Kỳ người chơi sẽ biến thành giống như NPC, đồng thời hệ thống sẽ thông báo tọa độ cần đến (Có thể xem tọa độ trên bản đồ nhỏ!) ",
	"Cũng có thể nhấp vào <color=red>Cờ hiệu<color> để xem tọa độ. Sau khi đã đến đích, nhấp vào <color=red>Cờ hiệu<color> để cắm Soái Kỳ.. Nếu trên đường đi bị trọng thương, cờ sẽ được cắm tại nơi đó, người khác có thể tiếp tục chuyển cờ về đích.",
	"Sau khi Soái Kỳ đã chuyển đến đích, trong doanh trại phe mình sẽ xuất hiện 1 Soái Kỳ mới. Phe chuyển nhiều cờ nhất sẽ giành thắng lợi chung cuộc"
}
function info_fixation1()--固定夺旗规则1
	Say(strfixationhelp[1],1,"Trang kế /info_fixation2")
end

function info_fixation2()--固定夺旗规则2
	Say(strfixationhelp[2],1,"Trang kế /info_fixation3")
end

function info_fixation3()
	Say(strfixationhelp[3],2,"Tìm hiểu thông tin khác/help_sjbattle","Ta hiểu rồi!/cancel")
end


strrandomhelp=
{
	"Phương thức đoạt cờ ngẫu nhiên: Sau khi khai chiến, trong đại doanh và tiền tuyến sẽ ngẫu nhiên xuất hiện Soái Kỳ, (hình dạng binh sĩ cầm cờ) , trên viết 1 chữ hoặc Kim. Nhấp vào Soái Kỳ người chơi sẽ biến thành giống như NPC, đồng thời hệ thống sẽ thông báo tọa độ cần đến (Có thể xem tọa độ trên bản đồ nhỏ!) . Cũng có thể nhấp phải vào <color=red>Cờ hiệu<color> để xem tọa độ.",
	"Sau khi đã đến đích,  nhấp vào <color=red>Cờ hiệu<color> để cắm Soái Kỳ. Nếu trên đường đi bị trọng thương, cờ sẽ được cắm tại nơi đó, người khác có thể tiếp tục chuyển cờ về đích.",
}
function info_random1()--随机夺旗规则1
	Say(strrandomhelp[1],1,"Trang kế /info_random2")
end

function info_random2()
	Say(strrandomhelp[2],2,"Tìm hiểu thông tin khác/help_sjbattle","Ta hiểu rồi!/cancel")
end

strmarshalhelp="Phương thức bảo vệ Nguyên Soái: Nửa giờ sau khi khai chiến,  tại Doanh trại phe ít điểm tích lũy nhất sẽ xuất hiện Nguyên Soái,  10 phút sau Nguyên soái phe kia cũng sẽ xuất hiện. Nếu điểm tích lũy bằng nhau thì 2 Nguyên Soái sẽ cùng xuất hiện. Mục tiêu lúc này là tiêu diệt Nguyên Soái phe địch. Kết thúc chiến trận,  nếu 2 Nguyên soái vẫn còn thì sẽ căn cứ vào điểm tích lũy của 2 bên đến phân thắng bại."

function info_marshal()
	Say(strmarshalhelp, 2, "Tìm hiểu thông tin khác/help_sjbattle","Ta hiểu rồi!/cancel")
end

TIMELIST = "10:50,12:50,14:50,16:50,18:50,20:50,22:50"
strbt_info=
{
	"Vị trí chiến trường/bt_infoaddress",
	"Thời gian khai chiến/bt_infotime",
	"Điều kiện báo danh/bt_infoneed",
	"Quy tác báo danh/bt_infobaomingrule",
	"Các vật phẩm cần thiết/bt_infoflag",
	"Các đạo cụ liên quan/bt_infothing",
	"Các chức năng mới khi báo danh?/bt_infonew",
	"Điểm báo danh phe Tống nằm ở hướng Bắc Tương Dương, của phe Kim là ở hướng Nam Chu Tiên trấn",
	format("Chiến trường Tống Kim chính thức báo danh lúc %s. Thời gian báo danh là %d phút, hệ thống sẽ liên tục thông báo. <enter>Thời gian chiến đấu là %d phút, cứ mỗi %d phút hệ thống sẽ công báo tình hình. Nếu hết thời gian báo danh số người tham gia vẫn chưa đủ, người chơi vẫn có thể tiếp tục báo danh.",TIMELIST, BAOMING_TIME, FIGHTING_TIME, ANNOUNCE_TIME),
--	"宋金战场将于每天11：00、13：00、15：00、17：00、19：00、21：00、23：00准时开放报名（系统广播滚动通知），报名时间为30分钟。<enter>报名结束后正式开战，战斗时间为60分钟，每20秒公布一次战况。报名结束后若参战人数未达到上限，则可继续报名进入战场。",
	"Người chơi phải đạt đến hoặc cao hơn cấp <color=yellow>40<color> mới có thể báo danh, phí báo danh <color=yellow>100000<color> lượng. Tống kim đại chiến phân ra làm ba cấp bậc: sơ cấp (cấp 40~79), trung cấp (cấp 80~119), cao cấp (cấp 120 trở lên).",
	"Số lượng tối đa là: <color=yellow>400<color>người. Hết thời gian báo danh nếu vẫn chưa đủ nhân số vẫn có thể báo danh, nhưng nếu một phe nhiều hơn phe kia <color=yellow>"..BALANCE_MAMCOUNT.."người<color>, thì sẽ không thể tham gia vào phe đó, cần phải ít hơn phe kia<color=yellow>"..BALANCE_MAMCOUNT.."người<color> mới được nhận thêm.",
	"Qui tắc báo danh: Thời gian của 1 chiến dịch là 1 tuần. Mỗi lần mở 1 chiến dịch mới, số điểm tích lũy của tuần trước sẽ trở về 0. Trọn 1 chiến dịch chỉ được tham gia cho 1 phe, muốn thay đổi, phải đợi đến tuần sau.",--报名时玩家需要权衡双方人数因素，系统会自动根据双方人数的多少调节积分。人数占优方的积分将有衰减，人数劣势方的积分将有加成。",
	"Những chức năng mới: trước khi chính thức khai chiến, có thể dùng điểm tích lũy đến Quân Nhu quan để mua các đạo cụ chuyên dụng. Sau khi khai chiến sẽ không thể mua",
}
function bt_onbattleinfo()
	Say(strbattleinfo[2],9,"Qui tắc tham chiến/bt_infohelprule",strbt_info[1],strbt_info[2],strbt_info[3],strbt_info[4],strbt_info[5],strbt_info[6],strbt_info[7],"Ta hiểu rồi! Cảm ơn!/cancel")
end


function bt_infoaddress()--宋金大战场位置
	Say(strbt_info[8],2,"Tìm hiểu thông tin khác/bt_onbattleinfo","Ta hiểu rồi!/cancel")
end

function bt_infotime()--宋金时间
	Say(strbt_info[9],2,"Tìm hiểu thông tin khác/bt_onbattleinfo","Ta hiểu rồi/cancel")
end

function bt_infoneed()--报名需求
	Say(strbt_info[10],3,"Trang kế /bt_infoneed1","Tìm hiểu thông tin khác/bt_onbattleinfo","Ta hiểu rồi/cancel")
end

function bt_infoneed1()--报名需求
	Say(strbt_info[11],2,"Tìm hiểu thông tin khác/bt_onbattleinfo","Ta hiểu rồi/cancel")
end

function bt_infobaomingrule()
	Say(strbt_info[12],2,"Tìm hiểu thông tin khác/bt_onbattleinfo","Ta hiểu rồi/cancel")
end


function bt_infonew()--报名点新增功能
	Say(strbt_info[13],2,"Tìm hiểu thông tin khác/bt_onbattleinfo","Ta hiểu rồi/cancel")
end

function bt_infothing()--宋金大战道具
	Say("Đạo cụ Tống Kim đại chiến:",3,"Cờ hiệu /bt_infoflag","Tống Kim Chiếu Thư /bt_infobook","Ta hiểu rồi!/cancel")
end

function bt_infobook()--宋金诏书
	Say("Tống Kim Chiếu Thư: Sử dụng sẽ được đưa thẳng đến địa điểm báo danh. Có thể mua tại tiệm tạp hóa trong các thành, giá <color=yellow>5000<color>lượng",2,"Tìm hiểu thông tin khác/bt_onbattleinfo","Ta hiểu rồi!/cancel")
end

function bt_infoflag()--旗标
	Say("Cờ hiệu: Sau khi di chuyển Soái kỳ đến đích, nhấp vào Cờ hiệu để cắm cờ. Lúc đang di chuyển có thể nhấp vào Cờ hiệu để xác định lại tọa độ. Mỗi Cờ hiệu chỉ dùng được 1 lần. Có thể mua ở Quân Nhu Quan, giá <color=yellow>5000<color>lượng.",2,"Tìm hiểu thông tin khác/bt_onbattleinfo","Ta hiểu rồi!/cancel")
end

function bt_infohelprule()--两种夺旗规则
	Say("Ngươi muốn tìm hiểu quy tắc nào?",5,"Qui tắc đoạt kỳ (cố định) /info_fixation1","Qui tắc đoạt kỳ (ngẫu nhiên) /info_random1","Qui tắc bảo vệ Nguyên Soái/info_marshal","Qui tắc Tống Kim Đại Chiến/bt_onbattleinfo","Ta hiểu rồi, cảm ơn!/cancel")
end

function cancel()
end
