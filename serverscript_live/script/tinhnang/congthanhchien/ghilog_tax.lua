--Author: Fong KiÒu

Include("\\script\\tinhnang\\congthanhchien\\log_tax.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\lib\\lib_ham.lua")

function write_log_tax(nMoneyTax)
	dofile("script/tinhnang/congthanhchien/ghilog_tax.lua")
	if(nMoneyTax > 0) then
		local nSTT = getn(LOG_TAX)+1
		LOG_TAX[nSTT] = {GetLocalDate("%y-%m-%d %H:%M:%S"), GetName(), nMoneyTax}
		local tbLogTax = TaoBang(LOG_TAX,"LOG_TAX","")
		SaveData("script/tinhnang/congthanhchien/log_tax.lua",tbLogTax)
	end
end


