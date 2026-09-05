------------------------------------------------------
-- ÎÄ¼şÃû¡¡£ºspider_peak_activity.lua
-- ´´½¨Õß¡¡£º
-- ´´½¨Ê±¼ä£º2021-07-21-14-40
-- Ãè  Êö  £ºÏÀ·åÂÛ½£»î¶¯
------------------------------------------------------
--?Include([[\script\ui\manage.lua]]);
--?UiManage:OpenWindow("SPRING_FESTIVAL") ´ò¿ªµÄÊÇ´º½Ú»î¶¯µÄÒ³Ãæ
--?UiManage:OpenWindow("SPIDER_PEAK_ACTIVITY")
--È·±£ÊÇ¿Í»§¶Ë½çÃæ
if 1 ~= MODEL_GAMECLIENT then
	return
end


Include("\\script\\ui\\manage.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\protocol.lua")

local tbClass	= {}
tbClass.nPak = curpack();

tbClass.UIGROUP	= "UI_GREATTEAMFIGHT_SIGNIN"

tbClass.UICOMP	= {
	ACTIVITY_REWARD = "Activity_Reward",
	Single_attend = "Single_attend",
	Team_attend = "Team_attend",
	ACTIVITY_INTRO = "Activity_intro",
	BTN_CLOSE = "btnClose",
	ACTIVITY_REWARD_PIC1 = "Activity_Reward_pic1",
	ACTIVITY_REWARD_PIC2 = "Activity_Reward_pic2",
	ACTIVITY_REWARD_PIC3 = "Activity_Reward_pic3",
	ACTIVITY_INTRO_PICTURE = "Activity_intro_Picture",
	ACTIVITY_INTRO_TEXT = "Activity_Intro_Text",
	ACTIVITY_REWORD_BOX1 = "Activity_Reword_Box1",
	ACTIVITY_REWORD_BOX2 = "Activity_Reword_Box2",
	ACTIVITY_REWORD_BOX3 = "Activity_Reword_Box3",
}

tbClass.tbControls = {}

tbClass.nItemObjType = 0

tbClass.tbItemDisplay = {
	{6, 1, 5482},
	{6, 1, 5484},
	{6, 1, 5486},
}

tbClass.items = {}
tbClass.test = {}

--ÓÃÀ´´æ´¢ËùÓĞ²¼¾Ö×Ó¿Ø¼şĞÅÏ¢µÄ±í
tbClass.AllChildControlInfro = {}

function tbClass:OnCreate()
	--¸»ÎÄ±¾³õÊ¼»¯
	--Txt_SetRichTxt(self.UIGROUP, self.UICOMP.ACTIVITY_INTRO_TEXT, "ÏÀ·åÂÛ½£ÊÇÒ»ÖÖ3ÈË¶Ô¿¹ÈüĞÎÊ½µÄ¾º¼¼»î¶¯¡£µÈ¼¶´óÓÚµÈÓÚ150¼¶µÄ´óÏÀ£¬¿ÉÑ¡Ôñµ¥ÈË»ò×é¶Ó£¨±ØĞë3ÈË£©±¨Ãû²ÎÈü¡£<enter>Ã¿ÖÜ¶ş¡¢ÖÜËÄ¿ªÆô»ı·ÖÈü£¬Ã¿ÖÜÈÕ¿ªÆôÕù°ÔÈü¡£19£º50¿ªÊ¼±¨Ãû£¨Ö»ÄÜ±¨Ãû1´Î£©£¬20£º00¿ªÊ¼¾º¼¼¡£<enter>»ı·ÖÈüÆÚ¼äÃ¿¸ö¿ª·ÅÈÕÓĞ3³¡¾º¼¼£¬Ã¿ÖÜ¹²6³¡¡£Õù°ÔÈüÆÚ¼ä×î¶àÓĞ5³¡¾º¼¼£¬ÈôÌáÇ°¾ö³ö¹Ú¾ü¶Ó£¬µ±ÈÕÊ£Óà¾º¼¼³¡´Î²»¿ª·Å¡£<enter>»ı·ÖÈüÅÅĞĞÇ°96ÃûµÄ´óÏÀ²ÅÓĞ×Ê¸ñ²Î¼Óµ±ÖÜµÄÕù°ÔÈü¡£ÉÏÖÜÕù°ÔÈüÇ°2Ãû¶ÓÎéµÄ¹²¼ÆÖÁ¶à6Ãû³ÉÔ±ÎŞÂÛÊÇ·ñ²Î¼Óµ±ÖÜ»ı·ÖÈü£¬¾ù¿É»ñµÃµ±ÖÜÕù°ÔÈü×Ê¸ñ¡£<enter>Ã¿³¡¾º¼¼·ÖÓĞÈı¸ö½×¶Î£ºÉÏ°ë³¡Õ½¶·Ê±³¤×î¶à3·ÖÖÓ£»ÖĞ³¡ĞİÏ¢1·ÖÖÓ£¬¶ÓÔ±¸´»î£»ÏÂ°ë³¡Õ½¶·Ê±³¤×î¶à3·ÖÖÓ¡£½«¶Ô·½È«²¿»÷É±»áÌáÇ°½áÊøËùÔÚ°ë³¡µÄÕ½¶·¡£<enter>Ã¿³¡¾º¼¼½áÊø¿É»ñµÃÏàÓ¦½±Àø£¬´ï³ÉÏÀ·åÂÛ½£ÖÜÄ¿±ê¿ÉÁìÈ¡ÖÜ½±Àø£¨µã»÷ÆÁÄ»ÓÒ²à¾º¼¼³¡Í¼±ê£©£¬¸ù¾İÕù°ÔÈüÅÅÃû»¹¿É»ñµÃÅÅÃû½±Àø£¨ÓÊ¼ş·¢ËÍ£©£¬»ı·ÖÈü½áÊø¸ù¾İ°ï»á³ÉÔ±×Ü»ı·Ö»¹¿ÉÔÚÂÛ½£¹ÜÊÂ´¦ÁìÈ¡°ï»á»ı·ÖÅÅÃû½±Àø¡£<enter>½±Àø·áºñ£¬Íû´óÏÀ»ı¼«²ÎÓë£¡")	
	--Wnd_Hide(self.UIGROUP, self.UICOMP.ACTIVITY_INTRO_TEXT)
	self.tbControls.ACTIVITY_INTRO_TEXT = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.ACTIVITY_INTRO_TEXT);
	self.tbControls.ACTIVITY_REWARD = WndImage:CreateByInstance(self.UIGROUP, self.UICOMP.ACTIVITY_REWARD);
	self.tbControls.ACTIVITY_INTRO_TEXT:SetRichText("<enter>    HiÖp Phong LuËn KiÕm lµ mét ho¹t ®éng thi ®Êu 3 ng­êi. <enter>  §¹i hiÖp cÊp 100 trë lªn cã thÓ chän tham gia c¸ nh©n hoÆc theo tæ ®éi (<color=red>b¾t buéc 3 ng­êi<color>).<enter><enter>    <color=red>Vßng b¶ng më vµo thø Hai vµ thø Ba mçi tuÇn, vßng Tranh B¸ më vµo thø N¨m mçi tuÇn<color>. §¨ng kı b¾t ®Çu lóc 20:15 (chØ cã thÓ ®¨ng kı 1 lÇn), thi ®Êu b¾t ®Çu lóc 20:25.<enter><enter>    Trong vßng b¶ng, mçi ngµy më cöa cã <color=red>3<color> trËn thi ®Êu, tæng céng 6 trËn mçi tuÇn. Trong vßng Tranh B¸, tèi ®a cã <color=red>5<color> trËn thi ®Êu, nÕu ®éi qu¸n qu©n ®­îc x¸c ®Şnh sím, c¸c trËn thi ®Êu cßn l¹i trong ngµy sÏ kh«ng më.<enter><enter>    Top <color=red>96<color> ®¹i hiÖp ®iÓm cao nhÊt trong b¶ng xÕp h¹ng vßng b¶ng míi cã t­ c¸ch tham gia vßng Tranh B¸ trong tuÇn. Tæng céng tèi ®a 6 thµnh viªn cña 2 ®éi ®øng ®Çu vßng Tranh B¸ tuÇn tr­íc, bÊt kÓ hä cã tham gia vßng b¶ng tuÇn nµy hay kh«ng, ®Òu cã thÓ nhËn ®­îc t­ c¸ch tham gia vßng Tranh B¸ tuÇn nµy.<enter><enter>    Mçi trËn thi ®Êu ®­îc chia thµnh ba giai ®o¹n: HiÖp 1 dµi tèi ®a 3 phót, nghØ gi¶i lao gi÷a hiÖp 1 phót, håi sinh thµnh viªn. HiÖp 2 dµi tèi ®a 3 phót. ViÖc h¹ gôc toµn bé ®èi thñ sÏ kÕt thóc sím hiÖp ®Êu hiÖn t¹i.<enter><enter>    Mçi trËn thi ®Êu kÕt thóc sÏ nhËn ®­îc th­ëng t­¬ng øng, ®¹t ®­îc môc tiªu tuÇn HiÖp Phong LuËn KiÕm cã thÓ nhËn th­ëng tuÇn (nhÊp vµo biÓu t­îng HiÖp Phong LuËn KiÕm bªn ph¶i mµn h×nh), theo thø h¹ng tranh b¸ còng cã thÓ nhËn ®­îc th­ëng thø h¹ng (göi qua th­), vßng b¶ng kÕt thóc theo tæng ®iÓm thµnh viªn bang còng cã thÓ nhËn th­ëng xÕp h¹ng ®iÓm bang t¹i LuËn KiÕm Qu¶n Sù.<enter><enter>    PhÇn th­ëng hÊp dÉn, c¸c ®¹i hiÖp h·y tİch cùc tham gia nhĞ!")
	self.tbControls.ACTIVITY_REWORD_BOX1 = WndObjBox:CreateByInstance(self.UIGROUP, self.UICOMP.ACTIVITY_REWORD_BOX1);
	self.tbControls.ACTIVITY_REWORD_BOX2 = WndObjBox:CreateByInstance(self.UIGROUP, self.UICOMP.ACTIVITY_REWORD_BOX2);
	self.tbControls.ACTIVITY_REWORD_BOX3 = WndObjBox:CreateByInstance(self.UIGROUP, self.UICOMP.ACTIVITY_REWORD_BOX3);
	self.tbControls.ACTIVITY_REWARD_PIC1 = WndImage:CreateByInstance(self.UIGROUP, self.UICOMP.ACTIVITY_REWARD_PIC1);
	self.tbControls.ACTIVITY_REWARD_PIC2 = WndImage:CreateByInstance(self.UIGROUP, self.UICOMP.ACTIVITY_REWARD_PIC2);
	self.tbControls.ACTIVITY_REWARD_PIC3 = WndImage:CreateByInstance(self.UIGROUP, self.UICOMP.ACTIVITY_REWARD_PIC3);

	self.tbControls.ACTIVITY_REWORD_BOX1:DisablePickPut();
	self.tbControls.ACTIVITY_REWORD_BOX2:DisablePickPut();
	self.tbControls.ACTIVITY_REWORD_BOX3:DisablePickPut();

	self.tbControls.ACTIVITY_INTRO_TEXT:Hide();
	self.tbControls.ACTIVITY_INTRO_TEXT:BringToTop();
	self.nItemObjType = GetCGOGNumber("CGOG_ITEM");

	self:PutIn(self.tbItemDisplay);
end


--ÔÚÕâÀïÊµÏÖÒ»¸ö×î¼òµ¥µÄ¹¦ÄÜ£¬½«Ò»¸ö¿Ø¼ş¼Óµ½ÁíÍâÒ»¸ö¿Ø¼şÉÏÃæ

function tbClass:PutIn(tbItemsProp)
	for i = 1, getn(tbItemsProp) do
		self.items[i] = self:NewItem(tbItemsProp[i]);
	end
	self.tbControls.ACTIVITY_REWORD_BOX1:HoldObject(self.nItemObjType, self.items[1], 1, 1);
	self.tbControls.ACTIVITY_REWORD_BOX2:HoldObject(self.nItemObjType, self.items[2], 1, 1);
	self.tbControls.ACTIVITY_REWORD_BOX3:HoldObject(self.nItemObjType, self.items[3], 1, 1);
end



--´´½¨Ò»¸öĞÂµÄÎïÆ·
function tbClass:NewItem(tbProp)
	return NewItemEx(4, 0, 0, tbProp[1], tbProp[2], tbProp[3], 1, 0, 0);
end


function tbClass:OnLBClick(szWnd,nParam)
	if szWnd == self.UICOMP.BTN_CLOSE then
		UiManage:CloseWindow(self.UIGROUP)
	elseif szWnd == self.UICOMP.Team_attend then
		self:TeamAttend()
	elseif szWnd == self.UICOMP.Single_attend then
		self:SingleAttend()
	elseif szWnd == self.UICOMP.ACTIVITY_INTRO then
		self.tbControls.ACTIVITY_INTRO_TEXT:Show();
		self.tbControls.ACTIVITY_INTRO_TEXT:BringToTop();
	elseif szWnd == self.UICOMP.ACTIVITY_INTRO_TEXT then
		self.tbControls.ACTIVITY_INTRO_TEXT:Hide();
	end
end

function tbClass:SingleAttend()
	self:SignIn("Single");
	UiManage:CloseWindow(self.UIGROUP)
end

--×é¶Ó²Î¼Ó»î¶¯¶ÔÓ¦µÄº¯Êı
function tbClass:TeamAttend()
	self:SignIn("Team");
	UiManage:CloseWindow(self.UIGROUP)
end

--É¾³ı´´½¨µÄitemË÷ÒıÖµ
function tbClass:deleteItem(items)	
	--É¾³ıÌØ¶¨µÄÄ¿±ê¶ÔÓ¦µÄ´úÂë£ºDelItemNonBelong
	--DelItemNonBelong(items[3]);
	for i = 1, getn(items) do
		DelItemNonBelong(items[i]);
	end
end


--ÔÚÒ³ÃæÏú»ÙµÄÊ±ºòµ÷ÓÃ£¬×¢ÒâÕâÀïÍ¨¹ıdeleteItemº¯Êı£¬ÊµÏÖÁËÉ¾³ıÒÑ¾­´´½¨ÁËÎïÆ·µÄ¹¦ÄÜ¡£
function tbClass:OnDestroy()
	self.tbControls = {};
	self:deleteItem(self.items);
	self.items = {};
end

function tbClass:SignIn(szSigninType)
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_TABLE, {szType = "Signin", szSigninType = szSigninType});
	ScriptProtocol:SendData("emScript_PROTOCOL_C2S_GREATTEAMFIGHT_REQUEST", handle);
	OB_Release(handle);
end

UiManage:RegisterClass(tbClass)

