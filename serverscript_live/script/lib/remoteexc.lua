-- ================================================================================================
-- [HE THONG] script/lib/remoteexc.lua
-- Muc dich  : REMOTEEXECUTE: goi ham tu GameServer sang Relay va nguoc lai (RELAYHT 06/09).
-- Duoc nap  : Include tu 2 tep (vd tongcastle.lua, hundred_arena.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : objbuffer_head.lua, baseclass.lua, common.lua
-- Ham (dong): RemoteExc (12), RemoteExc_CallBack (29), ReceiveExc (62)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\lib\\baseclass.lua")
Include("\\script\\lib\\common.lua")

if not tbRemoteExcClass then
	tbRemoteExcClass = tbBaseClass:new();
end

tbRemoteExcClass.tbCallBack = {};

-- 传给szCallBackFun的参数将是tbCallBackParam内的值+远程函数的返回值
function RemoteExc(szRemoteScript, szRemoteFun, tbRemoteParam, szCallBackScript, szCallBackFun, tbCallBackParam, dwGameSvrID)
	local handle = OB_Create();
	
	
	ObjBuffer:PushObject(handle, szRemoteScript);
	ObjBuffer:PushObject(handle, szRemoteFun);
	ObjBuffer:PushObject(handle, tbRemoteParam);
	
	if szCallBackScript and szCallBackScript ~= "" then
		local nCallBackID = getn(tbRemoteExcClass.tbCallBack) + 1;
		tbRemoteExcClass.tbCallBack[nCallBackID] = {szCallBackScript, szCallBackFun, tbCallBackParam};
		RemoteExecute("\\script\\lib\\remoteexc.lua", "ReceiveExc", handle, "RemoteExc_CallBack", nCallBackID, dwGameSvrID);	
	else
		RemoteExecute("\\script\\lib\\remoteexc.lua", "ReceiveExc", handle, nil, 0, dwGameSvrID);
	end
	OB_Release(handle);
end

function RemoteExc_CallBack(Param, ResultHandle)
	if (tbRemoteExcClass.tbCallBack[Param] ~= nil) then
		local szCallBackScript	= tbRemoteExcClass.tbCallBack[Param][1];
		local szCallBackFun	= tbRemoteExcClass.tbCallBack[Param][2];
		local tbCallBackParam = tbRemoteExcClass.tbCallBack[Param][3];
		local tbRemoteRet = {};
		if (OB_IsEmpty(ResultHandle) ~= 1) then
			tbRemoteRet = ObjBuffer:PopObject(ResultHandle);
		end
		
		if (szCallBackScript ~= nil and szCallBackScript ~= "" and 
				szCallBackFun ~= nil and szCallBackFun ~= "") then
				-- 将2个参数表合并
				local tbParam = {szCallBackScript, szCallBackFun};
				if (type(tbCallBackParam) == "table") then
					for i=1, getn(tbCallBackParam) do
						tinsert(tbParam, tbCallBackParam[i]);
					end
				end
				if (type(tbRemoteRet) == "table") then
					for i=1, getn(tbRemoteRet) do
						tinsert(tbParam, tbRemoteRet[i]);
					end
				end
		
				--DynamicExecute(szCallBackScript, szCallBackFun, unpack(tbParam))
				call(DynamicExecute, tbParam);
		end
		
		tbRemoteExcClass.tbCallBack[Param] = nil;
	end
end

function ReceiveExc(ParamHandle, ResultHandle, dwGSID)
	if (OB_IsEmpty(ParamHandle) == 1) then
		return
	end
	local szScript = ObjBuffer:PopObject(ParamHandle);
	if (OB_IsEmpty(ParamHandle) == 1) then
		return
	end
	local szFun = ObjBuffer:PopObject(ParamHandle);
	local tbParam = {};
	if (OB_IsEmpty(ParamHandle) ~= 1) then
		tbParam = ObjBuffer:PopObject(ParamHandle);
	end

	local tbCallParam = {szScript, szFun}
	if (type(tbParam) == "table") then
		for i=1, getn(tbParam) do
			tinsert(tbCallParam, tbParam[i]);
		end
	end
	tinsert(tbCallParam, dwGSID);
	
	local tbRet = pack(call(DynamicExecute, tbCallParam));
	if getn(tbRet) > 0 then
		ObjBuffer:PushObject(ResultHandle, tbRet);
	end
end