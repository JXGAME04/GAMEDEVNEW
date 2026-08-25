---Script -- By-NguyenKhai-------
Include("\\script\\battles\\battlehead.lua")
Include("\\script\\global\\judgeoffline_limit.lua");
Include("\\script\\missions\\basemission\\lib.lua")
IncludeLib("ITEM")
IncludeLib("LEAGUE")
Include("\\script\\lib\\progressbar.lua")
Include("\\script\\activitysys\\g_activity.lua")
IncludeLib("TASKSYS");
Include("\\script\\lib\\awardtemplet.lua")
-- [3HD 25/08] bo Include tep khong ton tai o moi goc (BS-1): Include("\\script\\global\\Â·ÈË_Àñ¹Ù.lua")
Include("\\script\\dailogsys\\dailogsay.lua")
Include("\\script\\lib\\composelistclass.lua")
Include("\\script\\lib\\composeex.lua")
Include("\\script\\lib\\droptemplet.lua")


function thuytacdaulinh()
--bossben1A()
--bossben1B()
--bossben2A()
--bossben2B()
--bossben3A()
--bossben3B()
end

function bossben1A()
nNpcIndex = AddNpcEx(513, 85,random(0,4),SubWorldID2Idx(337), 1636*32, 3221*32, 1, "DiÖu Nh­", 1)
if nNpcIndex > 0 then
SetNpcDeathScript(nNpcIndex,"\\script\\global\\tamhiep\\callbossdeathmini.lua");
SetNpcTimer(nNpcIndex,900*18)
end
end
function bossben1B()
nNpcIndex = AddNpcEx(511, 85,random(0,4),SubWorldID2Idx(337), 1632*32, 3221*32, 1, "Tr­¬ng T«ng ChÝnh", 1)
if nNpcIndex > 0 then
SetNpcDeathScript(nNpcIndex,"\\script\\global\\tamhiep\\callbossdeathmini.lua");
SetNpcTimer(nNpcIndex,900*18)
end
end


function bossben2A()
nNpcIndex = AddNpcEx(513, 85,random(0,4),SubWorldID2Idx(338), 1636*32, 3221*32, 1, "DiÖu Nh­", 1)
if nNpcIndex > 0 then
SetNpcDeathScript(nNpcIndex,"\\script\\global\\tamhiep\\callbossdeathmini.lua");
SetNpcTimer(nNpcIndex,900*18)
end
end
function bossben2B()
nNpcIndex = AddNpcEx(511, 85,random(0,4),SubWorldID2Idx(338), 1632*32, 3221*32, 1, "Tr­¬ng T«ng ChÝnh", 1)
if nNpcIndex > 0 then
SetNpcDeathScript(nNpcIndex,"\\script\\global\\tamhiep\\callbossdeathmini.lua");
SetNpcTimer(nNpcIndex,900*18)
end
end


function bossben3A()
nNpcIndex = AddNpcEx(513, 85,random(0,4),SubWorldID2Idx(339), 1636*32, 3221*32, 1, "DiÖu Nh­", 1)
if nNpcIndex > 0 then
SetNpcDeathScript(nNpcIndex,"\\script\\global\\tamhiep\\callbossdeathmini.lua");
SetNpcTimer(nNpcIndex,900*18)
end
end
function bossben3B()
nNpcIndex = AddNpcEx(511, 85,random(0,4),SubWorldID2Idx(339), 1632*32, 3221*32, 1, "Tr­¬ng T«ng ChÝnh", 1)
if nNpcIndex > 0 then
SetNpcDeathScript(nNpcIndex,"\\script\\global\\tamhiep\\callbossdeathmini.lua");
SetNpcTimer(nNpcIndex,900*18)
end
end


function OnTimer(nNpcIndex,nTimeOut)
DelNpc(nNpcIndex)
end






