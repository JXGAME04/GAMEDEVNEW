-- DOT E (E5): goc ghi IL("TITLE") nhung IL khong ton tai o dau ca (lo goc thu 5
-- - file nay CHET tu dau o ban goc); doi sang IncludeLib de file nap duoc.
IncludeLib("TITLE");
MAX_TITLE_PERPAGE = 5
TASK_ACTIVE_TITLE = 1122

-- ==== 14/08: NOI he danh hieu JX2 sang he JX1 de CLIENT VE duoc ====
-- He Title_* (bang \settings\playertitle_jx2.txt) chi song trong RAM may chu:
-- KJx2Title.cpp:8-415 nam TRON trong #ifdef _SERVER, khong mot goi tin nao mang
-- no ra client. Vi vay bat danh hieu bang Title_ActiveTitle KHONG lam hien chu
-- tren dau nhan vat.
--
-- Duong VE chu tren dau la he JX1: KNpc.cpp:6078-6103 doc m_btPlayerTitle roi tra
-- bang \settings\PlayerTitle.txt (CoreUseNameDef.h:149), va m_btPlayerTitle di
-- sang client qua PlayerSync.PlayerTitle (KNpc.cpp:5391 va :5533).
--
-- CA HAI bang deu co du 7 dong Thai thu nhung LECH ID dung 15 don vi:
--   JX2 153..159  (playertitle_jx2.txt dong 154..160)
--   JX1 168..174  (PlayerTitle.txt      dong 169..175)  <- ban client giong het
-- vd Thai thu Bien Kinh: JX2 = 156, JX1 = 171.
TITLE_JX2_VICEROY_MIN = 153
TITLE_JX2_VICEROY_MAX = 159
TITLE_JX1_OFFSET = 15
-- KNpc.cpp:10613 quy doi nSaveTime = time(0) + nTime/18 => 18 nam
TITLE_TIME_FOREVER = 567648000

-- doi id danh hieu JX2 -> id JX1; 0 = khong phai danh hieu Thai thu
function title_jx1_id(nJx2Id)
	if (nJx2Id == nil) then
		return 0
	end
	if (nJx2Id < TITLE_JX2_VICEROY_MIN or nJx2Id > TITLE_JX2_VICEROY_MAX) then
		return 0
	end
	return nJx2Id + TITLE_JX1_OFFSET
end

-- Dat (hoac go) chu tren dau nhan vat theo danh hieu JX2 vua bat.
-- SetPlayerTitle BAT BUOC du 3 tham so - ScriptFuns.cpp:9542 return 0 neu thieu.
function title_jx1_apply(titleid)
	local n = title_jx1_id(titleid)
	if (n > 0) then
		SetPlayerTitle(n, TITLE_TIME_FOREVER, 0)
		return n
	end
	-- Khong phai Thai thu: CHI go khi dang deo dung mot danh hieu Thai thu,
	-- de khong cuop mat danh hieu cua he khac (Tong Kim, dai hoi vo lam...)
	local nCur = GetPlayerTitle()
	if (nCur ~= nil and nCur >= TITLE_JX2_VICEROY_MIN + TITLE_JX1_OFFSET
		and nCur <= TITLE_JX2_VICEROY_MAX + TITLE_JX1_OFFSET) then
		RemovePlayerTitle()
	end
	return 0
end

-- File nay duoc Include vao NHIEU state khac nhau (NPC, lenh bai admin,
-- playerlogin). Nut "Giu lai ban dau" tro toi OnCancel - state nao khong co
-- ham do thi m_ActionScriptID bi dat 0 va CHET CA CAY MENU (KPlayer.cpp:6822).
-- Chi dinh nghia khi CHUA co, de khong de len OnCancel that cua state khac.
if (OnCancel == nil) then
	function OnCancel()
	end
end

function change_title()
	Say("B¹n muèn ta gióp viÖc g×?", 3,  "T×m xem hoÆc kÝch ho¹t danh hiÖu ®· cã /#title_ShowTitleList(1)", "Kh«ng kÝch ho¹t danh hiÖu hiÖn thêi/#title_choose(0)", "Kh«ng cÇn/OnCancel")
end

function title_ShowTitleList(nPage)
	local tab = Title_GetTitleTab()
	
	if (tab == nil or getn(tab) == 0) then
		Say("Tr­íc mÆt b¹n kh«ng cã danh hiÖu nµo hÕt!",0)
		return
	end
	
	local titletab = {};
	local iter = 1
	--·ÇµÚÒ»Ò³ÔòÏÔÊ¾ÉÏÒ»Ò³
	if (nPage > 1) then
		titletab[iter] = "Trang tr­íc/#title_ShowTitleList("..(nPage - 1)..")"
		iter = iter + 1
	end
	
	--Èç¹ûÒ³ÊýÓÃÍêµÄ»°
	if (getn(tab) <= MAX_TITLE_PERPAGE * nPage) then
		for i = MAX_TITLE_PERPAGE * (nPage - 1) + 1, getn(tab) do 
			titletab[iter] = Title_GetTitleName(tab[i]).."/#title_choose("..tab[i]..")";
			iter = iter + 1
		end
	else
		for i = 1, MAX_TITLE_PERPAGE do 
			tabiter = i + (nPage - 1)*MAX_TITLE_PERPAGE 
			titletab[iter] = Title_GetTitleName(tab[tabiter]).."/#title_choose("..tab[tabiter]..")";
			iter = iter + 1
		end
		titletab[iter] = "Trang kÕ /#title_ShowTitleList("..(nPage + 1)..")"
		iter = iter + 1
	end
	
	titletab[iter] = "Gi÷ l¹i ban ®Çu/OnCancel"
	iter = iter + 1
	Say("Tr­íc m¾t b¹n ®ang nhËn danh hiÖu lµ "..getn(tab)..", b¹n muèn kÝch ho¹t danh hiÖu nµo?", getn(titletab), titletab);
end

function title_choose(titleid)
	Title_ActiveTitle(titleid)
	SetTask(TASK_ACTIVE_TITLE,titleid)
	title_jx1_apply(titleid)
end

function title_loginactive()
	local nId = GetTask(TASK_ACTIVE_TITLE)
	Title_ActiveTitle(nId)
	title_jx1_apply(nId)
end;