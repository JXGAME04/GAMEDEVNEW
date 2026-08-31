-------------------------------------------------------------------------
-- FileName		:	NewWorldScript_H.lua
-- Author		:	LuoBaohang
-- CreateTime	:	2005-09-02
-- Desc			:  	地图切换触发脚本头文件--函数定义
-------------------------------------------------------------------------
IncludeLib("PARTNER")
aryFuncStore = {} --函数定义，每个函数规定有且只有一个开关参数(进入为1，退出为0)

function SetPartnerSwitch(bIn)
do return end
	if (bIn == 1) then
		PARTNER_CallOutCurPartner(0)
		PARTNER_SetCallOutSwitch(0)
		Msg2Player("T総/ g鋓 ng h祅h")
	else
		PARTNER_SetCallOutSwitch(1)	
		Msg2Player("T総/ g鋓 ng h祅h")
	end
end

function CD_ForbidEnemy(bIn)
	if (bIn == 1) then
		SetPKFlag(0)
		ForbidChangePK(1);
		ForbidEnmity(1);
		SetTaskTemp(200,1);
	else
		ForbidChangePK(0);
		SetTaskTemp(200,0);
		ForbidEnmity(0);
	end
end

function forbidCreateTeam(bIn)
	if (bIn == 1) then
		LeaveTeam()
		SetCreateTeam(0)
		--Msg2Player("组队关闭！")
	else
		SetCreateTeam(1)
		--Msg2Player("组队开启！")
	end
end

aryFuncStore["PARTNER_OFF"] = SetPartnerSwitch
aryFuncStore["CD_Forbid_OFF"] = CD_ForbidEnemy
aryFuncStore["CreateTeam_OFF"] = forbidCreateTeam
