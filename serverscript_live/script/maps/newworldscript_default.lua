-- newworldscript_default.lua - ban COMPAT cua du an (WLLS port 20/08/2026).
-- Ban Linux goc keo them newworldscript_h.lua + tianziyuxi.lua (chuoi phu
-- thuoc nang, dinh den 495 map khac). Map lien dau chi dung 3 tham so
-- PARTNER_OFF|CreateTeam_OFF|TISHENZHIREN nen ban nay giu dung ngu nghia do:
--   CreateTeam_OFF : vao map TU ROI to doi (LeaveTeam - giong forbidCreateTeam
--                    goc newworldscript_h.lua:41-50) + cam lap to (SetCreateTeam
--                    0), roi map mo lai
--   PARTNER_OFF / TISHENZHIREN : du an chua co he tuong ung -> bo qua
-- Su kien EnterMap/LeaveMap van phat qua EventSys nhu ban goc.
Include("\\script\\misc\\eventsys\\type\\map.lua")

aryFuncStore = {
	["CreateTeam_OFF"] = function(bEnter)
		if (bEnter == 1) then
			LeaveTeam()
			SetCreateTeam(0)
		else
			SetCreateTeam(1)
		end
	end,
	["PARTNER_OFF"] = function(bEnter) end,
	["TISHENZHIREN"] = function(bEnter) end,
}

aryChangeWorldExec = {}
function PraseParam(szParam)
	if (szParam == nil or szParam == "") then
		return nil
	end
	if (aryChangeWorldExec[SubWorld] == nil) then
		aryChangeWorldExec[SubWorld] = {}
		local aryFuns = split(szParam, "|")
		local j = 1
		for i = 1, getn(aryFuns) do
			local ExecFun = aryFuncStore[aryFuns[i]]
			if (ExecFun ~= nil) then
				aryChangeWorldExec[SubWorld][j] = ExecFun
				j = j + 1
			end
		end
	end
	return 1
end

function OnNewWorldDefault(szParam)
	if (PraseParam(szParam) == 1) then
		for i = 1, getn(aryChangeWorldExec[SubWorld]) do
			aryChangeWorldExec[SubWorld][i](1)
		end
	end
	EventSys:GetType("EnterMap"):OnPlayerEvent(SubWorldIdx2ID(SubWorld), PlayerIndex)
	EventSys:GetType("EnterMap"):OnPlayerEvent("ALL", PlayerIndex)
end

function OnLeaveWorldDefault(szParam)
	if (PraseParam(szParam) == 1) then
		for i = 1, getn(aryChangeWorldExec[SubWorld]) do
			aryChangeWorldExec[SubWorld][i](0)
		end
	end
	EventSys:GetType("LeaveMap"):OnPlayerEvent(SubWorldIdx2ID(SubWorld), PlayerIndex)
	EventSys:GetType("LeaveMap"):OnPlayerEvent("ALL", PlayerIndex)
end
