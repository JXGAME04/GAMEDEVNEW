-- bigboss.lua  (S3Relay)  [RELAYHT 06/09]
-- Port tu D:\ServerLinux\gateway\s3relay\script\mission\boss\bigboss.lua
--
-- CU PHAP: ban goc KHONG co cho nao phai sua de chay Lua 5.4 -- khong co upvalue
--   "%bien", khong co "for k,v in bang" thieu pairs, khong dung bang "arg", khong
--   gan lai bien dieu khien for. Da ra soat lai mot luot sau khi viet: 0 cho.
--
-- NHUNG CHO DA SUA so voi ban Linux (deu la va chong loi, KHONG doi hop dong):
--   [1] SetBigBossDead: chi nhan gia tri khi pop ra dung KIEU SO. Truoc day neu
--       goi den pop ra nil thi isBigBossDead = nil, sau do IsBigBossDead day
--       nguoc OBJTYPE_NIL sang GameServer -> nguoi choi mat thuong trong im lang.
--   [2] IsBigBossDead: bao dam LUON day dung MOT so (0 hoac 1) nhu hop dong doi.
--   [3] MakeAllPlayerCanGetReward: giu nguyen cau GlobalExecute nhung them
--       OutputMsg canh bao, vi lenh GM "dwf" HIEN CHUA CO tren JX1 (xem CANH BAO
--       so 2 ben duoi) -> de khoi hong im lang, nhin log la biet.
--   [4] Them OutputMsg cho SetBigBossDead / IsBigBossDead de do duong ong.
--   [5] Log dung tostring() cho gon. DINH CHINH (soat 06/09): ban dau cho nay ghi
--       mot luat SAI la "format('%d', so thuc) la LOI o 5.4". DA DO THAT tren
--       dung ngan xep dang chay (Lua54Dll + lua4_baselibopen): format('%d',1.5)
--       = "1", format('%d',-2.7) = "-2" -- KHONG loi, vi bien toan cuc `format`
--       la ham C l4_b_format cua shim, no tu ep nguyen. Chi string.format cua
--       Lua 5.4 goc moi bao loi. Dung format("%d", ...) o day cung duoc.
--
-- DUONG DAN PHAI DAT DUNG:  .\script\mission\boss\bigboss.lua   tinh tu
--   bin\multiserver.  Ben relay la MISSION (so it), ben GameServer la
--   script\missions\boss (MISSIONS, so nhieu).
--   [SOAT 06/09] Dat sai cho BEN RELAY thi GameServer VAN gui len relay nhu thuong
--   (no ghi cung mot duong dan, khong nhin thu muc ben nay); dau hieu hong nam o
--   NHAT KY RELAY: "khong nap duoc kich ban [...]", khong phai nhat ky GameServer.
--
-- HOP DONG voi bin\server\script\missions\boss\bigboss.lua:
--
--   SetBigBossDead(ParamHandle, ResultHandle)
--     <- BigBoss:BigBossDeath() dong 246-251 (Doc Co Thien Phong chet).
--        Day vao ObjBuffer: DUNG MOT SO = 1.
--        KHONG co callback, GameServer khong doi gia tri tra ve (dwCallId = 0).
--
--   IsBigBossDead(ParamHandle, ResultHandle)
--     <- BigBoss:BigBossGlobalReward() dong 176.
--        Day vao ObjBuffer: KHONG GI CA (tham so 3 la handle 0 -> goi rong,
--        OB_IsEmpty(ParamHandle) = 1 ben nay).
--        Tra ve: DUNG MOT SO 0 hoac 1.
--        GameServer doc o BigBossGlobalReward_CallBack (dong 179-200):
--        = 1 -> AddTaskDaily(2661) + kiem bit + GiveAwardByList
--        khac 1 -> Talk "Hom nay van chua danh bai Doc Co Thien Phong".
--
--   Ca hai ham thuc te duoc RelayRpc.cpp:220 goi voi BA doi
--   (hParam, hRes, dwGameSvrId). Ban Linux chi khai HAI, doi thu ba bi bo -- giu
--   nguyen nhu vay. dwGameSvrId luon = 0 (KJx2SharedStore.cpp:185).
--
-- ============================ CANH BAO (chua sua duoc o day) ============================
--   1) IsBigBossDead se GOI DUOC nhung KET QUA KHONG VE. KJx2SharedStore.cpp:163
--      cat pend.strScript = duong dan kich ban DICH (\script\mission\boss\bigboss.lua),
--      roi KJx2_OnRelayScriptPacket dong 225 tra g_GetScript dung duong dan do --
--      ma duong dan do KHONG duoc nap ben GameServer (chinh vi the moi phai day
--      len relay) -> chi ghi log "[RELAYHT] callback: khong thay kich ban" roi im.
--      Phai sua CoreServer.dll (dung g_GetScriptNameByState(L) trong KSortScript.h)
--      thi BigBossGlobalReward_CallBack moi chay. RelayRpc.cpp:307 co loi y het
--      cho chieu nguoc lai.
--   2) MakeAllPlayerCanGetReward KHONG chay duoc: lenh GM "dwf" khong ton tai tren
--      JX1 (KGMCommand.cpp:9-25 chi co DoSct/ds, dw, RunSctFile/RSF, ReLoadSct/RLS,
--      ReLoadAllSct/RLAS). TextMsgProcessGMCmd tra FALSE lang le. Thay bang "dw"
--      cung khong cuu duoc vi "dw" dung mot KLuaScript MOI chi dang ky 8 ham
--      WorldScriptFuns -- khong co Include, khong co GetFirstPlayerAtServer /
--      GetNextPlayerAtServer / doFunByPlayer ma ham ben GameServer can.
--   3) isBigBossDead chi nam trong BO NHO relay, khong luu ShareData, va KHONG co
--      cho nao dat lai 0 (ban Linux cung vay). Dat 1 roi la 1 mai toi khi relay
--      khoi dong lai. Cai chan that nam o bit nhiem vu ngay phia GameServer
--      (PlayerFunLib:AddTaskDaily task 2661).
--   4) Ca hai diem hop dong hien la MA CHET tren JX1: BigBoss:BigBossDeath va
--      BigBoss:BigBossGlobalReward khong co cho nao goi trong ca cay bin\server
--      (thieu bigboss_npc.lua, call_big_boss.lua, callboss_incity.lua va
--      activitysys\config\14). Dat tep nay len relay la de DU HA TANG, chua lam
--      chay duoc hoat dong Doc Co Thien Phong.
-- =======================================================================================

Include("\\script\\lib\\objbuffer_head.lua")

if (BigBoss == nil) then
	BigBoss = {}
	BigBoss.gold_boss_count = 0;
	BigBoss.isBigBossDead = 0;
end

-- ------------------------------------------------------------------------------
-- DU LIEU CHET: giu nguyen theo ban Linux de sau nay ai port call_big_boss.lua
-- con cho ma tra. KHONG tep nao trong ca hai cay (Linux + JX1) doc toi hai bang
-- nay. 7 ten thanh co dau -> ban chinh phai chuyen sang TCVN3 bang vn_to_octal.
-- ------------------------------------------------------------------------------
BigBoss.BIG_BOSS_POS = {
					{80, {{1833, 3367},{1511, 3239}, {1582, 2902}, {1987, 2975}}, "D­¬ng Ch©u"},
					{11, {{2909, 5236},{2883, 4835}, {3317, 5280}, {3306, 4860}}, "Thµnh §«"},
					{162, {{1336, 3205},{1426, 3401}, {1723, 3489}, {1668, 2981}}, "§¹i Lý"},
					{1, {{1424, 3044},{1783, 3183}, {1724, 3406}, {1497, 3383}}, "Ph­îng T­êng"},
					{78, {{1753, 3121},{1670, 3462}, {1369, 3411}, {1379, 3141}}, "T­¬ng D­¬ng"},
					{37, {{1770, 3403},{1520, 3281}, {1535, 2902}, {2017, 2679}}, "BiÖn Kinh"},
					{176, {{1162, 2781},{1832, 3342}, {1247, 3354}, {1754, 2805}}, "L©m An"},
}

BigBoss.BIG_BOSS_SER = {
	[0] = 1505,
	[1] = 1506,
	[2] = 1507,
	[3] = 1508,
	[4] = 1509,
}

-- ------------------------------------------------------------------------------
-- Hai ham dem boss vang. Ban Linux goi tu missions\boss\callboss_incity.lua:40,65
-- va missions\boss\bossdeath.lua:174 -- JX1 KHONG co hai tep do, nen hien khong
-- ai goi. Chi cong/tru, khong ai doc gold_boss_count.
-- ------------------------------------------------------------------------------
function AddGoldBossCount(ParamHandle, ResultHandle)
	BigBoss.gold_boss_count = BigBoss.gold_boss_count + 1;
	OutputMsg( format("GOLD BOSS COUNT :  %d",BigBoss.gold_boss_count) );
end

function ReduceGoldBossCount(ParamHandle, ResultHandle)
	BigBoss.gold_boss_count = BigBoss.gold_boss_count - 1;
	OutputMsg( format("GOLD BOSS COUNT :  %d",BigBoss.gold_boss_count) );
end

-- ------------------------------------------------------------------------------
-- GameServer bao: Doc Co Thien Phong da chet.
-- Vao: DUNG MOT SO = 1.   Ra: khong tra ve gi.
-- ------------------------------------------------------------------------------
function SetBigBossDead(ParamHandle, ResultHandle)
	if (OB_IsEmpty(ParamHandle) == 0) then
		local nDead = ObjBuffer:PopObject(ParamHandle);
		-- [1] chi nhan khi dung kieu so; pop trung nil se lam hong IsBigBossDead
		if (type(nDead) == "number") then
			BigBoss.isBigBossDead = nDead;
			OutputMsg( "[BIGBOSS] SetBigBossDead = " .. tostring(nDead) );
		else
			OutputMsg("[BIGBOSS] SetBigBossDead: goi khong phai kieu so, bo qua");
		end
	end
end

-- ------------------------------------------------------------------------------
-- GameServer hoi: hom nay boss chet chua?
-- Vao: khong gi ca.   Ra: DUNG MOT SO 0 hoac 1.
-- ------------------------------------------------------------------------------
function IsBigBossDead(ParamHandle, ResultHandle)
	-- [2] bao dam luon day dung mot so, khong bao gio day OBJTYPE_NIL
	local nDead = BigBoss.isBigBossDead;
	if (type(nDead) ~= "number") then
		nDead = 0;
	end
	ObjBuffer:PushObject(ResultHandle, nDead);
	-- [SOAT 06/09] KHONG ghi log o day: GameServer goi ham nay THEO TUNG NGUOI CHOI
	-- (bigboss.lua:176 nam trong vong lap nguoi choi), ghi moi luot la do rTRACE
	-- tren luong chinh cua relay. Muon do duong ong thi xem log cua SetBigBossDead.
end

-- ------------------------------------------------------------------------------
-- Mo nut nhan thuong cho MOI nguoi choi toan cum.
-- Ban Linux goi tu missions\boss\call_big_boss.lua:15 -- JX1 KHONG co tep do.
-- CANH BAO 2: lenh GM "dwf" chua co tren JX1 -> cau duoi di het duong roi tra
-- FALSE lang le. Giu nguyen cau lenh cho dung ban goc, them log de biet.
-- ------------------------------------------------------------------------------
function MakeAllPlayerCanGetReward(ParamHandle, ResultHandle)
	OutputMsg("[BIGBOSS] MakeAllPlayerCanGetReward: gui lenh GM 'dwf' -- JX1 CHUA CO lenh nay, nhieu kha nang khong co tac dung");
	GlobalExecute("dwf \\script\\missions\\boss\\bigboss.lua BigBoss:MakeAllPlayerCanGetReward()");
end
