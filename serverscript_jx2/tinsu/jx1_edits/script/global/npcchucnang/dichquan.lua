Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\task\\tollgate\\messenger\\posthouse.lua")	-- [TIN SU 21/08] luong Tin Su ban Linux (especiallymessenger)

THANH_DICHTRAM={
{1  	,36},--phuong tuong
{11 	,37},--thanh do
{162	,38},--dai ly
{37 	,39},--bien kinh
{78 	,40},--tuong duong
{80 	,41},--duong chau
{176	,42}--lam an
};

function main(NpcIndex)
-- Talk(1,"","§ang trong thêi gian ®ua Top kh«ng thÓ tham gia ho¹t ®éng .!!!") 
	Say("Ng­¬i t×m ta cã viÖc g×?",2,
		-- "NhiÖm vô ®­a th­ /duathu",
		"NhiÖm vô TÝn Sø/especiallymessenger",	-- [TIN SU 21/08] thay "Thien bao kho /thienbao" (tinh nang cu da go)
		"Kh«ng cã g×. Thø lçi!/no")
end

-- [TIN SU 21/08] 3 ham duoi (thienbao/gothienbao/phanthuong) la Thien Bao Kho tu che cu, KHONG con trong menu
function thienbao()
	local nFinish = GetNumber(2, GetTaskTemp(NVTHIENBAOKHO), 1);
	SetTaskTemp(NVTHIENBAOKHO, 0);
	if(nFinish > 12) then
	phanthuong();
	Talk(1,"","Tèt l¾m! Ng­¬i ®· cã c«ng lín víi quèc gia, ®©y lµ phÇn th­ëng cña ng­¬i!")
	return end
	Say(15663,2,
	"§i vµo Thiªn B¶o Khè /gothienbao",
	"§Ó ta suy nghÜ /no")
end;

function gothienbao()
	if(GetLevel() < 90) then
	Talk(1,"","H·y ®¹t ®Õn <color=red>cÊp 90<color> míi cã thÓ ®ñ søc tham gia nhiÖm vô nµy!");
	return end
	LeaveTeam();
	SetFightState(0);
	SetLogoutRV(1);
	local w,x,y = GetWorldPos();
	local nMapNo = 1;
	if(w == 11) then
	nMapNo = 2;
	elseif(w == 162) then
	nMapNo = 3;
	elseif(w == 37) then
	nMapNo = 4;
	elseif(w == 78) then
	nMapNo = 5;
	elseif(w == 80) then
	nMapNo = 6;
	elseif(w == 176) then
	nMapNo = 7;
	end
	SetTask(TASK_NVST, SetNumber(1, GetTask(TASK_NVST), 9, nMapNo));
	NewWorld(395,1415,3197);
end;

function phanthuong()
	-- DropEventTBK()

	AddSumExp(500000000)
	AddItemSL(3846,20,0)
	AddItemSL(4728,20,0)
	AddItemSL(4850,2,0) -- ruong trang bi xanh
	Msg2Player("B¹n ®· hoµn thµnh nhiÖm vô thiªn b¶o khè , nhËn ®­îc 500.000.000 kinh nghiÖm")
	Msg2SubWorld("§¹i hiÖp ["..GetName().."] ®· ®¬n th©n hoµn thµnh nhiÖm vô thiªn b¶o khè, nhËn ®­îc 500.000.000 kinh nghiÖm ")
	AddNote("Time ("..TimeGame..") <color=red>Thiªn B¶o Khè <color>: ngµi ®· hoµn thµnh nhiÖm vô Thiªn B¶o Khè.");
end;

function no()
end;