Include("\\script\\startgame\\npcpos.lua");

function addpubgnpc()
	DEATHFILE = "\\script\\global\\luanpcmonsters\\ondeath_pubg.lua"
	DROPFILE = "\\script\\global\\luanpcmonsters\\droprate_pubg.lua"
	local nNpcIndex
	for i=1,getn(BIENKINH) do
		nNpcIndex = AddNpcNew(RANDOMC(11,33,34,12),90,BIENKINHPUBG,BIENKINH[i][1],BIENKINH[i][2],DEATHFILE,
			5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,50,10,15,20,nil,600,nil,nil,DROPFILE);
		SetNpcExp(nNpcIndex, 20000000)
		SetNpcLife(nNpcIndex, 100);
	end
	for i=1,getn(BIENKINHPUBGTRONTTHANHNPC) do
		nNpcIndex = AddNpcNew(RANDOMC(11,33,34,12),90,BIENKINHPUBG,BIENKINHPUBGTRONTTHANHNPC[i][1],BIENKINHPUBGTRONTTHANHNPC[i][2],DEATHFILE,
			5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,50,10,15,20,nil,600,nil,nil,DROPFILE);
		SetNpcExp(nNpcIndex, 20000000)
		SetNpcLife(nNpcIndex, 100);
	end
	
	local Series = GetNpcSeries(nNpcIndex)
	if(Series==0) then		--KIM
		SetNpcSkill(nNpcIndex, 419, 40, 1);
		SetNpcSkill(nNpcIndex, 419, 40, 2);
		SetNpcSkill(nNpcIndex, 419, 40, 3);
		SetNpcSkill(nNpcIndex, 424, 40, 4);
		nSTVL = 1;
	elseif(Series==1) then	--MOC
		SetNpcSkill(nNpcIndex, 420, 40, 1);
		SetNpcSkill(nNpcIndex, 420, 40, 2);
		SetNpcSkill(nNpcIndex, 420, 40, 3);
		SetNpcSkill(nNpcIndex, 425, 40, 4);
		nDoc = 1;--doc sat khong giong sat thuong khac, = sat thuong khac chia 5, vi no co rut' doc
	elseif(Series==2) then	--THUY
		SetNpcSkill(nNpcIndex, 421, 40, 1);
		SetNpcSkill(nNpcIndex, 421, 40, 2);
		SetNpcSkill(nNpcIndex, 421, 40, 3);
		SetNpcSkill(nNpcIndex, 426, 40, 4);
		nBang = 1; --neu quai he thuy thi cho bang sat
	elseif(Series==3) then	--HOA
		SetNpcSkill(nNpcIndex, 422, 40, 1);
		SetNpcSkill(nNpcIndex, 422, 40, 2);
		SetNpcSkill(nNpcIndex, 422, 40, 3);
		SetNpcSkill(nNpcIndex, 427, 40, 4);
		nHoa = 1;--quai he hoa thi cho hoa sat,tuy theo quai manh hay yeu ma` cho so nay, vi du 8x thi cho 75 chan han ok
	elseif(Series==4) then	--THO
		SetNpcSkill(nNpcIndex, 423, 40, 1);
		SetNpcSkill(nNpcIndex, 423, 40, 2);
		SetNpcSkill(nNpcIndex, 423, 40, 3);
		SetNpcSkill(nNpcIndex, 428, 40, 4);
		nLoi = 1;
	else					--KHONG CO HE, truong hop nay la add sai hay sao do
		SetNpcSkill(nNpcIndex, 418, 40, 1);
		SetNpcSkill(nNpcIndex, 418, 40, 2);
		SetNpcSkill(nNpcIndex, 418, 40, 3);
		SetNpcSkill(nNpcIndex, 418, 40, 4);
		nSTVL = 1;
	end
end	