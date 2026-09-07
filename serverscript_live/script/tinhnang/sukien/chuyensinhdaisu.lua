
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\header\\factionhead.lua")

function main()
dofile("script/event/chuyensinhdaisu.lua")
    local tbOpp = {
		"X∏c nhÀn chuy”n sinh/ZHUANSHENG",
		-- "LÓi ›ch cÒa chuy”n sinh?/WHEREZHUAN",
		-- "Giai thich thuoc tinh chuyen sinh/ZHUANZHIYE",
		"Tho∏t kh·i/NO"
    };

    SayNew("<color=yellow>Th´ng tin chuy”n sinh<color>:\nY™u C«u: c p tËi thi”u 150, Chuy”n Sinh 1 (150 c p)(500 xu + 3.000 vπn), Chuy”n Sinh 2 (170 c p)(1.000 xu + 5.000 vπn), Chuy”n Sinh 3 (180 c p)(1.700 xu + 10.000 vπn), sau chuy”n sinh 3 (3.000 xu, 20.000 vπn)\nTh´ng b∏o: MÁi l«n chuy”n sinh sœ v“ lπi c p 120", getn(tbOpp), tbOpp);
end;


function ZHUANSHENG()

local nLevel=GetLevel()
local nTranLevle=0

 if  nLevel<150 then
		Talk(1,"","Bπn Æ∑ chæc Æπt c p 150 trÎ l™n ch¯ ?")
		return
 end
 
local nReBornNum=GetReBornNum()

local nEqcount=GetEquipCount()
  
 -- if (nEqcount>0) then
	  -- Talk(1,"","H∑y th∏o h’t trang bﬁ tr™n ng≠Íi rÂi quay lπi!")
	  -- return
 -- end

 if  (nReBornNum==0 and nLevel>=150 and nLevel<=200) then 
       if (GetCash() < 30000000) then
          Talk(1,"","Bπn chæc lµ muËn chuy”n sinh <color=yellow>"..(nReBornNum+1).."<color>. Bπn c„ ÆÒ 3.000 vπn l≠Óng ch≠a?")
	      return
       end
	   nTranLevle=130
elseif (nReBornNum==1 and nLevel>=170 and nLevel<=200) then 
      if (GetCash() < 50000000) then
          Talk(1,"","Bπn chæc lµ muËn chuy”n sinh <color=yellow>"..(nReBornNum+1).."<color>. Bπn c„ ÆÒ 5.000 vπn l≠Óng ch≠a")
	      return
       end
	    nTranLevle=130
elseif (nReBornNum==2 and  nLevel>=180 and nLevel<=200) then 
      if (GetCash() < 100000000) then
          Talk(1,"","Bπn chæc lµ muËn chuy”n sinh <color=yellow>"..(nReBornNum+1).."<color>. Bπn c„ ÆÒ 10.000 vπn l≠Óng ch≠a?")
	      return
     end
	    nTranLevle=140
else
     if (nReBornNum > 2 and  nLevel ==200) then
       if (GetCash() < 200000000) then
          Talk(1,"","Bπn chæc lµ muËn chuy”n sinh <color=yellow>"..(nReBornNum+1).."<color>. Bπn c„ ÆÒ 20.000 vπn l≠Óng ch≠a?")
	      return
       end
	   nTranLevle=150 + nReBornNum + 1 
	 else
	    Talk(1,"","Bπn chæc lµ muËn chuy”n sinh <color=yellow>"..(nReBornNum+1).."<color>. C p vµ Ti“n Vπn Æ∑ chu»n bﬁ ÆÒ ch≠a ?")
	    return
    end
end


  local nIspay=0

  if  (nReBornNum==0) then 
      nIspay= SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) - 500)
      if  (nIspay~=1) then
        Talk(1,"","Bπn kh´ng ÆÒ 500 xu!")
        return
      end
      Pay(30000000)
  elseif (nReBornNum==1) then 
      nIspay= SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) - 1000)
      if  (nIspay~=1) then
        Talk(1,"","Bπn kh´ng ÆÒ 1.000 xu!")
        return
      end
      Pay(50000000)
  elseif (nReBornNum==2) then 
      nIspay= SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) - 1700)
      if  (nIspay~=1) then
        Talk(1,"","Bπn kh´ng ÆÒ 1.700 xu!")
        return
      end
      Pay(100000000)
  elseif (nReBornNum>2) then  
      nIspay=SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) - 3000)
      if  (nIspay~=1) then
        Talk(1,"","Bπn kh´ng ÆÒ 3000 xu!")
        return
      end
      Pay(200000000)
  else
     Talk(1,"","ß∑ chuy”n sinh "..nReBornNum.."  l«n Æ∑ Æπt giÌi hπn!")
     return
  end  
  

   
   AddReBorn(1,nTranLevle)
   
   SetLevel(120)       
   
   SetReBornQnPoint()
   
   SetReBornSkill()  
   
end

function SetReBornQnPoint()


local as={
    {35, 25, 25, 15}, -- Kim
    {20, 35, 20, 25}, -- Moc
    {25, 25, 25, 25}, -- Thuy
    {30, 20, 30, 20}, -- Hoa
    {20, 15, 25, 40}, -- Tho
}

local player_series = GetSeries() + 1;
SetBasePoint(
    as[player_series][1],
    as[player_series][2],
    as[player_series][3],
    as[player_series][4]
);  

local nXiSuiPonit = GetTask(T_TTK) * 5       --- Tay tuy



end;

function SetReBornSkill()

          if (HaveMagic(695)>=0) then

			 DelMagic(695)
          end
		  
		 local  a = GetLevel()             ----Lay cap do hien tai cua nguoi choi
		 local  p = GetRestSP()            ----Lay so diem ky nang con lai
		 local  c = RollbackSkill(0)       ----Xoa toan bo ky nang va tra ve tong diem da su dung
		 local  h = GetTask(T_VLMT)        ----So lan da hoc bi kip cap 50
		 local  r = GetReBornJPoint()      ----Lay diem ky nang giu lai sau khi chuyen sinh
		 local  n = GetReBornQPoint()      ----Lay diem tiem nang giu lai sau khi chuyen sinh
		  -- AddMagicPoint(a+h+r-1,0)
		  AddMagicPoint(a-p-1+h+r,1)
          KickOutSelf()                  
end



nItemWuHang=0   
nRoleWuHang=0  

--------------------------------------------------------------
function ZHUANZHIYE()
local player_Faction = GetFaction()
     if (player_Faction == "") then				
		Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Bπn chæc lµ Æ∑ xu t s≠ ch≠a?")
		return
	elseif (player_Faction == "") then				
		Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Bπn chæc lµ Æ∑ xu t s≠ ch≠a?")
		return
	elseif (player_Faction == "") then
		Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Bπn chæc lµ Æ∑ xu t s≠ ch≠a?")
		return
	elseif (player_Faction == "") then				
		Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Bπn chæc lµ Æ∑ xu t s≠ ch≠a?")
		return
	elseif (player_Faction == "") then				
		Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Bπn chæc lµ Æ∑ xu t s≠ ch≠a?")
		return
	elseif (player_Faction == "") then				
		Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Bπn chæc lµ Æ∑ xu t s≠ ch≠a?")
		return
	elseif (player_Faction == "") then				
		Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Bπn chæc lµ Æ∑ xu t s≠ ch≠a?")
		return
	elseif (player_Faction == "") then				
		Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Bπn chæc lµ Æ∑ xu t s≠ ch≠a?")
		return
	elseif (player_Faction == "") then
		Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Bπn chæc lµ Æ∑ xu t s≠ ch≠a?")
		return
	elseif (player_Faction == "") then
		Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Bπn chæc lµ Æ∑ xu t s≠ ch≠a?")
		return
	end
	 
local nCurCamp=GetCurCamp()
local nCamp=GetCamp()
     if (nCurCamp~=4 or nCamp~=4) then
	    Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Bπn chæc lµ?")
		return
	 end
local nEqcount=GetEquipCount()
     if (nEqcount>0) then
	    Talk(1,"main","<color=yellow>NgÚ Hµnh ßπi S≠<color>:Hay thao het trang bi tren nguoi roi quay lai!")
		return
	 end
	 -- OpenGetString("NhÀp d˜ li÷u","ZHUANZHIYEB")   ---1 ???  2 ????
end

function ZHUANZHIYEB()
local nJinBi = 3
 if (nJinBi>= 6) then
     Talk(1,"ZHUANZHIYE","Thong bao:Ngu hanh nhap vao sai, vui long xem huong dan!")
	 return
 end
 
-- local nIspay=PayXu(100)
   -- if  (nIspay~=1) then
        -- Talk(1,"","Thong bao:Khong du 100 vang???????")
        -- return
   -- end
   
local nLevel=GetLevel()
SetSeries(nJinBi-1)   -- Dat ngu hanh
SetLevel(nLevel-2)    -- Dat lai cap do
SetCurCamp(0)         -- Dat trang thai trang (ten trang)
SetCamp(0)            -- Dat trang thai trang (camp)
ResetAP()             -- Dat lai diem tiem nang
ResetJN()             -- Dat lai diem ky nang
DelAllMagic()         -- Xoa tat ca ky nang
ClearFactionIfnfo()   -- Xoa thong tin mon phai
KickOutSelf()         -- Dang xuat nguoi choi (kick)

end
-------------------------------------------------------------






function ResetAP()

local as={
{35,25,25,15},
{20,35,20,25},
{25,25,25,25},
{30,20,30,20},
{20,15,25,40},
}

player_series = GetSeries() + 1;
SetBasePoint(as[player_series][1],as[player_series][2],as[player_series][3],as[player_series][4]);  

local nXiSuiPonit=GetTask(79)*5        
AddProp(nXiSuiPonit)

ResetBaseAttrib(0,as[player_series][1])
ResetBaseAttrib(1,as[player_series][2])
ResetBaseAttrib(2,as[player_series][3])
ResetBaseAttrib(3,as[player_series][4])

end;

function ResetJN()

	      local  a = GetLevel()       
          local  p = GetRestSP()          
		  local  c = RollbackSkill(0)		
          local  h = GetTask(80)			
		  local  r = GetReBornJPoint()  
		  local  n = GetReBornQPoint()   
		  AddMagicPoint(a-p-1+h+r,1)
         			   
          --KickOutSelf()                 
end;


function NO()
end;



