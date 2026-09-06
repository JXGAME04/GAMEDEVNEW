Include("\\script\\songbac\\datasongbac.lua")
Include("\\script\\songbac\\datanganluong.lua")
Include("\\script\\songbac\\datatienvang.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")

function main()
	 dofile("script/songbac/chusongbac.lua")
SayEx({"<color=green>Chñ Sßng B¹c<color>: Chóc quý kh¸ch <color=yellow>Ph¸t Tµi Ph¸t Léc<color>. B©y giê nãi xem ta cã thÓ gióp g× cho nhµ ng­¬i?",
--SayNew("<color=green>Chñ Sßng B¹c<color>: Chóc quý kh¸ch <color=yellow>Ph¸t Tµi Ph¸t Léc<color>. B©y giê nãi xem ta cã thÓ gióp g× cho nhµ ng­¬i?",5,
"§Æt c­îc BÇu Cua [Ng©n L­îng]/datcuoc",
"§Æt c­îc BÇu Cua [Xu]/datcuoc",
"Xem t×nh h×nh ®Æt c­îc /xem",
"NhËn tiÒn ®Æt c­îc /nhantien1",
"Tho¸t./no"})
end
function nhantien1()
SayEx({"<color=green>Chñ Sßng B¹c<color>: Ng­¬i muèn rót g× ?",
--SayNew("<color=green>Chñ Sßng B¹c<color>: Ng­¬i muèn rót g× ?",3,
"Ta muèn rót Ng©n L­îng th¾ng c­îc /nhantien",
"Ta muèn rót TiÒn Xu th¾ng cuéc /nhantien",
"Tho¸t./no"})
end


function CheckDataNganLuong(playername)
if getn(DataNganLuong) == 0 then
return 0
end
for i=1,getn(DataNganLuong) do
	if DataNganLuong[i][1] == playername then
		return i
	end
end
return 0
end
function CheckDataTienVang(playername)
if getn(DataTienVang) == 0 then
return 0
end
for i=1,getn(DataTienVang) do
	if DataTienVang[i][1] == playername then
		return i
	end
end
return 0
end
function xem()
Msg2Player("<color=pink>L­ît ®Æt c­îc thø: "..GetGlbMissionVC(11)..":")
van_tom = 0
van_cua = 0
van_bau = 0
van_ca = 0
van_ga = 0
van_nai = 0
vang_tom = 0
vang_cua = 0
vang_bau = 0
vang_ca = 0
vang_ga = 0
vang_nai = 0

for i=1,getn(DataSongBac) do
	if GetName() == DataSongBac[i][1] then
		if DataSongBac[i][3] == 1 then
			if DataSongBac[i][2] == 1 then
				van_tom = van_tom + DataSongBac[i][4]
			elseif DataSongBac[i][2] == 2 then
				van_cua = van_cua + DataSongBac[i][4]
			elseif DataSongBac[i][2] == 3 then
				van_bau = van_bau + DataSongBac[i][4]
			elseif DataSongBac[i][2] == 4 then
				van_ca = van_ca + DataSongBac[i][4]
			elseif DataSongBac[i][2] == 5 then
				van_ga = van_ga + DataSongBac[i][4]
			elseif DataSongBac[i][2] == 6 then
				van_nai = van_nai + DataSongBac[i][4]
			end
		elseif DataSongBac[i][3] == 2 then
			if DataSongBac[i][2] == 1 then
				vang_tom = vang_tom + DataSongBac[i][4]
			elseif DataSongBac[i][2] == 2 then
				vang_cua = vang_cua + DataSongBac[i][4]
			elseif DataSongBac[i][2] == 3 then
				vang_bau = vang_bau + DataSongBac[i][4]
			elseif DataSongBac[i][2] == 4 then
				vang_ca = vang_ca + DataSongBac[i][4]
			elseif DataSongBac[i][2] == 5 then
				vang_ga = vang_ga + DataSongBac[i][4]
			elseif DataSongBac[i][2] == 6 then
				vang_nai = vang_nai + DataSongBac[i][4]
			end
		end
	end
end


if van_tom > 0 then
Msg2Player("- §Æt c­îc T«m: <color=green>"..van_tom.." v¹n l­îng")
end
if van_cua > 0 then
Msg2Player("- §Æt c­îc Cua: <color=green>"..van_cua.." v¹n l­îng")
end
if van_bau > 0 then
Msg2Player("- §Æt c­îc BÇu: <color=green>"..van_bau.." v¹n l­îng")
end
if van_ca > 0 then
Msg2Player("- §Æt c­îc C¸: <color=green>"..van_ca.." v¹n l­îng")
end
if van_ga > 0 then
Msg2Player("- §Æt c­îc Gµ: <color=green>"..van_ga.." v¹n l­îng")
end
if van_nai > 0 then
Msg2Player("- §Æt c­îc Nai: <color=green>"..van_nai.." v¹n l­îng")
end

if vang_tom > 0 then
Msg2Player("- §Æt c­îc T«m: <color=yellow>"..vang_tom.." Xu")
end
if vang_cua > 0 then
Msg2Player("- §Æt c­îc Cua: <color=yellow>"..vang_cua.." Xu")
end
if vang_bau > 0 then
Msg2Player("- §Æt c­îc BÇu: <color=yellow>"..vang_bau.." Xu")
end
if vang_ca > 0 then
Msg2Player("- §Æt c­îc C¸: <color=yellow>"..vang_ca.." Xu")
end
if vang_ga > 0 then
Msg2Player("- §Æt c­îc Gµ: <color=yellow>"..vang_ga.." Xu")
end
if vang_nai > 0 then
Msg2Player("- §Æt c­îc Nai: <color=yellow>"..vang_nai.." Xu")
end
end
function no()
end
function nhantien(nsel)
i = nsel + 1
if i == 1 then
vt = CheckDataNganLuong(GetName())
				if vt == 0 then
					Talk(1,"","Ng­¬i kh«ng cã göi tiÒn ë chç ta, kh«ng thÓ rót ")
				else
					if DataNganLuong[vt][2] == 0 then
						Talk(1,"","Ng­¬i kh«ng cã göi tiÒn ë chç ta, kh«ng thÓ rót ")
					else
						soluong = DataNganLuong[vt][2]
						tientruoc = GetCash()
						DataNganLuong[vt][2] = 0
						luubang()
						if DataNganLuong[vt][2] == 0 then
							Earn(soluong * 10000)
							Msg2Player("B¹n ®· rót "..soluong.." v¹n l­îng ")
							--SaveData()
							SaveNow()		
							thoigian = tonumber(date("%H%M%d%m"))
							LoginLog = openfile("dulieu/SongBac/BauCua_RutNganLuong.txt", "a");
							if LoginLog then
								write(LoginLog,"Ngan Luong: "..GetName().." - Rut: ["..soluong.."] - Tr­íc: "..tientruoc.." - Sau: "..GetCash().." - Time: "..thoigian.."\n");
							end
							closefile(LoginLog)
							Talk(0,"")
						end
					end
				end
else
		
vt = CheckDataTienVang(GetName())
				if vt == 0 then
					Talk(1,"","Ng­¬i kh«ng cã göi tiÒn ë chç ta, kh«ng thÓ rót ")
				else
					if DataTienVang[vt][2] == 0 then
						Talk(1,"","Ng­¬i kh«ng cã göi tiÒn ë chç ta, kh«ng thÓ rót ")
					else
						soluong = DataTienVang[vt][2]
						tientruoc = GetTask(T_PLAYER_XU)
						DataTienVang[vt][2] = 0
						luubang()
						if DataTienVang[vt][2] == 0 then
							SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + soluong)
							
							Msg2Player("B¹n ®· rót "..soluong.." Xu ")
							SaveNow()		
							thoigian = tonumber(date("%H%M%d%m"))
							LoginLog = openfile("dulieu/SongBac/BauCua_RutNganLuong.txt", "a");
							if LoginLog then
								write(LoginLog,"-Tien Xu: "..GetName().." - Rut: ["..soluong.."] - Tr­íc: "..tientruoc.." - Sau: "..GetTask(T_PLAYER_XU).." - Time: "..thoigian.."\n");
							end
							closefile(LoginLog)
							Talk(0,"")
						end
					end
				end
end
end

function datcuoc(nsel)
-- if (check_time_sukien() == 0 or check_time_sukien() == 1) then
-- Talk(1,"","H«m nay kh«ng tæ chøc ®¸nh b¹c, vui lßng quay l¹i vµo ngµy Thø T­ vµ Chñ NhËt hµng tuÇn !")
-- return
-- end


SetTaskTemp(1,nsel + 1)
if GetGlbMissionVC(11) == 0 or GetGlbMissionVC(11) >= 13 then
Talk(1,"","HiÖn t¹i <color=yellow>ch­a tíi giê <color>Më Sßng, Chóng t«i phôc vô quý d©n ch¬i vµo lóc <color=red>12h00 - 13h00 vµ 23h00 - 24h00 mçi ngµy<color>. Mçi ngµy cã <color=yellow>12<color> l­ît ®Æt c­îc ")
return
end
if GetTaskTemp(1) == 1 then
tom,cua,bau,ca,ga,nai = CheckSoLuong(1)
else
tom,cua,bau,ca,ga,nai = CheckSoLuong(2)
end
SayEx({"<color=red>L­ît Thø "..GetGlbMissionVC(11).."<color>\nT«m:  <color=yellow>"..tom.."<color> - Cua: <color=yellow>"..cua.."<color> -  C¸: <color=yellow>"..ca.."<color>\nBÇu: <color=green>"..bau.."<color> - Gµ: <color=green>"..ga.."<color> - Nai: <color=green>"..nai.."",
--SayNew("<color=red>L­ît Thø "..GetGlbMissionV(11).."<color>\nT«m:  <color=yellow>"..tom.."<color> - Cua: <color=yellow>"..cua.."<color> -  C¸: <color=yellow>"..ca.."<color>\nBÇu: <color=green>"..bau.."<color> - Gµ: <color=green>"..ga.."<color> - Nai: <color=green>"..nai.."",7,
--SayNew("<color=red>L­ît Thø 1<color>",7,
"§Æt c­îc T«m/datcuoc1",
"§Æt c­îc Cua/datcuoc1",
"§Æt c­îc BÇu/datcuoc1",
"§Æt c­îc C¸ /datcuoc1",
"§Æt c­îc Gµ /datcuoc1",
"§Æt c­îc Nai/datcuoc1",
"Tho¸t./no"})
end

function datcuoc1(nsel)
SetTaskTemp(2 , nsel + 1)
if GetTaskTemp(1) == 1 then
OpenGetNumber ("§¬n VÞ: V¹n" , "datcuoctienvan")
else
OpenGetNumber ("§¬n VÞ: Xu" , "datcuocvang")
end
end

function datcuoctienvan()
local nSoTienV = GetNumberFromUI()

if nSoTienV <= 0 then
Talk(1,"","Ng­¬i ch­a nhËp sè tiÒn v¹n cÇn ®Æt, vui lßng ®Æt c­îc l¹i !")
return
end
if nSoTienV > 100 or nSoTienV < 10 then
Talk(1,"","ChØ ®­îc ®Æt c­îc tõ 10 v¹n - 100 v¹n l­îng, vui lßng ®Æt c­îc l¹i !")
return
end

datcuoc2(1,nSoTienV)
end
function datcuocvang()
local nSoTienX = GetNumberFromUI()

if nSoTienX <= 0 then
Talk(1,"","Ng­¬i ch­a nhËp sè tiÒn v¹n cÇn ®Æt, vui lßng ®Æt c­îc l¹i !")
return
end
if nSoTienX > 40 or nSoTienX < 5 then
Talk(1,"","ChØ ®­îc ®Æt c­îc tõ 5 vµng - 40 vµng, vui lßng ®Æt c­îc l¹i !")
return
end

datcuoc2(2,nSoTienX)
end

function datcuoc2(loai,cost)
vitri = CheckViTri(GetTaskTemp(2),loai)

if loai == 1 then
	if GetCash() < cost*10000 then
		Talk(1,"","Ng©n l­îng kh«ng ®ñ, kh«ng thÓ ®Æt c­îc")
		return
	end
	
	if vitri == 0 then
		DataSongBac[getn(DataSongBac)+1] = {GetName(),GetTaskTemp(2),loai,cost}
	else
		Talk(1,"","Ng­¬i ®· ®Æt c­îc "..CheckTen(GetTaskTemp(2))..", kh«ng thÓ ®Æt thªm !")
		return
	end
	
	Pay(cost*10000)
	Msg2Player("B¹n ®· ®Æt c­îc "..CheckTen(GetTaskTemp(2))..": <color=yellow>"..cost.." v¹n l­îng")
	thoigian = tonumber(date("%H%M%d%m"))
	LoginLog = openfile("dulieu/SongBac/BauCua_DatCuoc.txt", "a");
	if LoginLog then
		write(LoginLog,"Luot "..GetGlbMissionVC(11).." - "..GetName().." - Dat: ["..GetTaskTemp(2).."] - Gia: ["..cost.." van] - Con: "..GetCash().." - Time: "..thoigian.."\n");
	end
	closefile(LoginLog)
	Talk(0,"")
else
	if GetTask(T_PLAYER_XU) < cost then
		Talk(1,"","TiÒn Xu kh«ng ®ñ, kh«ng thÓ ®Æt c­îc")
		return
	end
	if vitri == 0 then
	DataSongBac[getn(DataSongBac)+1] = {GetName(),GetTaskTemp(2),loai,cost}
	else
	Talk(1,"","Ng­¬i ®· ®Æt c­îc "..CheckTen(GetTaskTemp(2))..", kh«ng thÓ ®Æt thªm !")
	return
	end

	SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) - cost)
	
	
	Msg2Player("B¹n ®· ®Æt c­îc "..CheckTen(GetTaskTemp(2))..": <color=yellow>"..cost.." Xu")
	thoigian = tonumber(date("%H%M%d%m"))
	LoginLog = openfile("dulieu/SongBac/BauCua_DatCuoc.txt", "a");
	if LoginLog then
		write(LoginLog,"Luot "..GetGlbMissionVC(11).." - "..GetName().." - Dat: ["..GetTaskTemp(2).."] - Gia: ["..cost.." Xu] - Con: "..GetTask(T_PLAYER_XU).." - Time: "..thoigian.."\n");
	end
	closefile(LoginLog)
	Talk(0,"")
end

luubang()
end

function CheckViTri(num,loai)
if getn(DataSongBac) == 0 then
return 0
end
for i=1,getn(DataSongBac) do
	if num == DataSongBac[i][2] and loai == DataSongBac[i][3] and GetName() == DataSongBac[i][1]	then
		return i
	end
end
return 0
end
function luubang()
	BANG2 = TaoBang(DataSongBac,"DataSongBac")
	LuuBang("script/songbac/datasongbac.lua",BANG2)
	BANG2 = TaoBang(DataNganLuong,"DataNganLuong")
	LuuBang("script/songbac/datanganluong.lua",BANG2)
	BANG2 = TaoBang(DataTienVang,"DataTienVang")
	LuuBang("script/songbac/datatienvang.lua",BANG2)
end

function CheckTen(i)
if i == 1 then
return "T«m"
elseif i == 2 then
return "Cua"
elseif i == 3 then
return "BÇu"
elseif i == 4 then
return "C¸ "
elseif i == 5 then
return "Gµ "
elseif i == 6 then
return "Nai"
else
return "Kh«ng X¸c §Þnh"
end
end

function CheckSoLuong(loai)
tom = 0
cua = 0
bau = 0
ca = 0
ga = 0
nai = 0
for i=1,getn(DataSongBac) do
	if DataSongBac[i][3] == loai then
		if DataSongBac[i][2] == 1 then
			tom = tom + DataSongBac[i][4]
		elseif DataSongBac[i][2] == 2 then
			cua = cua + DataSongBac[i][4]
		elseif DataSongBac[i][2] == 3 then
			bau = bau + DataSongBac[i][4]
		elseif DataSongBac[i][2] == 4 then
			ca = ca + DataSongBac[i][4]
		elseif DataSongBac[i][2] == 5 then
			ga = ga + DataSongBac[i][4]
		elseif DataSongBac[i][2] == 6 then
			nai = nai + DataSongBac[i][4]
		end
	end
end
return tom,cua,bau,ca,ga,nai
end