-- Author: Fong KiÒu
-- Date: 28/11/2021
-- Chuc nang: Luu giao dich ky tran cac
Include("\\script\\log_game\\save_log.lua")

function main(nKindShop,nIndex,nNumber)
	local szAccount = GetAccount()
	local szName = GetName()
	local szIP = GetIP()
	local szItemName = GetNameItem(nIndex)
	local nExtPoint = GetExtPoint()
	local nMoney = GetCash()
	local nKind,nGen,nDetail,nParti,nLevel,nSeries,nLine = GetCBItem(nIndex);	
	local log = "*************************************\n"
	log = log..date("%H:%M:%S").."\tTµi kho¶n: "..szAccount.."\tTªn: "..szName.."\tIP: "..szIP.."" 
	log = log.."\t[nKindShop: "..nKindShop.."]\t["..szItemName.."]\t[Number: "..nNumber.."]"
	log = log.."\t[nGoldId: "..nKind.."]\t[nGen: "..nGen.."]\t[nDetail: "..nDetail.."]\t[nParti: "..nParti.."]\t[nLevel: "..nLevel.."]\t[nSeries: "..nSeries.."]\t[nLine: "..nLine.."]"	
	log = log.."\t[Xu: "..nExtPoint.."]\t[Tæng tiÒn v¹n: "..nMoney.."]\n"
	logWriteKTC(log)
end