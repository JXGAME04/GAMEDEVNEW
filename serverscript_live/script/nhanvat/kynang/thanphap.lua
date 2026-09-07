--迭代函数，用于计算技能熟练度
--具体方法：
--根据1级熟练度，升级加速度，级数，重复伤害次数，范围，计算出相应等级熟练度
-- SkillExp(i) = Exp1*a^(i-1)*time*range
function SkillExpFunc(Exp0,a,Level,Time,Range)
	return floor(Exp0*(a^(Level-1))*Time*Range/2)
end


SKILLS={
tanthu={
lifereplenish_v={{{1,5},{2,5}},{{1,-1},{2,-1}}},
manareplenish_v={{{1,5},{2,5}},{{1,-1},{2,-1}}},
},
tongkim={
lifemax_p={{{1,0},{2,20},{3,30},{4,40},{5,50}},{{1,-1},{2,-1}}},
lifemax_v={{{1,0},{2,200},{3,300},{4,400},{5,500}},{{1,-1},{2,-1}}},
fastwalkrun_p={{{1,0},{2,10},{3,20},{4,30},{5,40}},{{1,-1},{2,-1}}},
castspeed_v={{{1,0},{2,0},{3,10},{4,20},{5,30}},{{1,-1},{2,-1}}},
},
chuyensinh1={
lifemax_v={{{1,100},{2,100}},{{1,-1},{2,-1}}},
manamax_v={{{1,100},{2,100}},{{1,-1},{2,-1}}},
allres_p={{{1,1},{2,1}},{{1,-1},{2,-1}}},
allresmax_p={{{1,1},{2,1}},{{1,-1},{2,-1}}},
},
chuyensinh2={
lifemax_v={{{1,200},{2,200}},{{1,-1},{2,-1}}},
manamax_v={{{1,200},{2,200}},{{1,-1},{2,-1}}},
allres_p={{{1,2},{2,2}},{{1,-1},{2,-1}}},
deadlystrikeenhance_p={{{1,5},{2,5}},{{1,-1},{2,-1}}},
allresmax_p={{{1,2},{2,2}},{{1,-1},{2,-1}}},
},
chuyensinh3={
lifemax_v={{{1,300},{2,300}},{{1,-1},{2,-1}}},
manamax_v={{{1,300},{2,300}},{{1,-1},{2,-1}}},
allres_p={{{1,3},{2,3}},{{1,-1},{2,-1}}},
deadlystrikeenhance_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
allresmax_p={{{1,3},{2,3}},{{1,-1},{2,-1}}},
},
chuyensinh4={
lifemax_v={{{1,400},{2,400}},{{1,-1},{2,-1}}},
manamax_v={{{1,400},{2,400}},{{1,-1},{2,-1}}},
allres_p={{{1,4},{2,4}},{{1,-1},{2,-1}}},
deadlystrikeenhance_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
fastwalkrun_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
allresmax_p={{{1,4},{2,4}},{{1,-1},{2,-1}}},
},
chuyensinh5={
lifemax_v={{{1,500},{2,500}},{{1,-1},{2,-1}}},
manamax_v={{{1,500},{2,500}},{{1,-1},{2,-1}}},
allres_p={{{1,5},{2,5}},{{1,-1},{2,-1}}},
deadlystrikeenhance_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
fastwalkrun_p={{{1,20},{2,20}},{{1,-1},{2,-1}}},
allresmax_p={{{1,5},{2,5}},{{1,-1},{2,-1}}},
},
chuyensinh6={
lifemax_v={{{1,600},{2,600}},{{1,-1},{2,-1}}},
manamax_v={{{1,600},{2,600}},{{1,-1},{2,-1}}},
allres_p={{{1,6},{2,6}},{{1,-1},{2,-1}}},
deadlystrikeenhance_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
fastwalkrun_p={{{1,20},{2,20}},{{1,-1},{2,-1}}},
castspeed_v={{{1,10},{2,10}},{{1,-1},{2,-1}}},
allresmax_p={{{1,6},{2,6}},{{1,-1},{2,-1}}},
},
chuyensinh7={
lifemax_v={{{1,700},{2,700}},{{1,-1},{2,-1}}},
manamax_v={{{1,700},{2,700}},{{1,-1},{2,-1}}},
allres_p={{{1,7},{2,7}},{{1,-1},{2,-1}}},
deadlystrikeenhance_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
fastwalkrun_p={{{1,20},{2,20}},{{1,-1},{2,-1}}},
castspeed_v={{{1,20},{2,20}},{{1,-1},{2,-1}}},
allresmax_p={{{1,7},{2,7}},{{1,-1},{2,-1}}},
},
chuyensinh8={
lifemax_v={{{1,800},{2,800}},{{1,-1},{2,-1}}},
manamax_v={{{1,800},{2,800}},{{1,-1},{2,-1}}},
allres_p={{{1,8},{2,8}},{{1,-1},{2,-1}}},
deadlystrikeenhance_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
fastwalkrun_p={{{1,20},{2,20}},{{1,-1},{2,-1}}},
castspeed_v={{{1,20},{2,20}},{{1,-1},{2,-1}}},
freezetimereduce_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
stuntimereduce_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
poisontimereduce_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
fasthitrecover_v={{{1,10},{2,10}},{{1,-1},{2,-1}}},
allresmax_p={{{1,8},{2,8}},{{1,-1},{2,-1}}},
},
chuyensinh9={
lifemax_v={{{1,900},{2,900}},{{1,-1},{2,-1}}},
manamax_v={{{1,900},{2,900}},{{1,-1},{2,-1}}},
allres_p={{{1,9},{2,9}},{{1,-1},{2,-1}}},
deadlystrikeenhance_p={{{1,15},{2,15}},{{1,-1},{2,-1}}},
fastwalkrun_p={{{1,30},{2,30}},{{1,-1},{2,-1}}},
castspeed_v={{{1,20},{2,20}},{{1,-1},{2,-1}}},
freezetimereduce_p={{{1,20},{2,20}},{{1,-1},{2,-1}}},
stuntimereduce_p={{{1,20},{2,20}},{{1,-1},{2,-1}}},
poisontimereduce_p={{{1,20},{2,20}},{{1,-1},{2,-1}}},
fasthitrecover_v={{{1,20},{2,20}},{{1,-1},{2,-1}}},
allresmax_p={{{1,9},{2,9}},{{1,-1},{2,-1}}},
},
chuyensinh10={
lifemax_v={{{1,1000},{2,1000}},{{1,-1},{2,-1}}},
manamax_v={{{1,1000},{2,1000}},{{1,-1},{2,-1}}},
allres_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
deadlystrikeenhance_p={{{1,15},{2,15}},{{1,-1},{2,-1}}},
fastwalkrun_p={{{1,30},{2,30}},{{1,-1},{2,-1}}},
castspeed_v={{{1,30},{2,30}},{{1,-1},{2,-1}}},
freezetimereduce_p={{{1,30},{2,30}},{{1,-1},{2,-1}}},
stuntimereduce_p={{{1,30},{2,30}},{{1,-1},{2,-1}}},
poisontimereduce_p={{{1,30},{2,30}},{{1,-1},{2,-1}}},
fasthitrecover_v={{{1,30},{2,30}},{{1,-1},{2,-1}}},
allresmax_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
},

cailao1={
lifemax_p={{{1,5},{2,5}},{{1,-1},{2,-1}}},
manamax_p={{{1,5},{2,5}},{{1,-1},{2,-1}}},
allres_p={{{1,1},{2,1}},{{1,-1},{2,-1}}},
allresmax_p={{{1,1},{2,1}},{{1,-1},{2,-1}}},

},
cailao2={
lifemax_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
manamax_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
allres_p={{{1,2},{2,2}},{{1,-1},{2,-1}}},
allresmax_p={{{1,2},{2,2}},{{1,-1},{2,-1}}},

},
cailao3={
lifemax_p={{{1,15},{2,15}},{{1,-1},{2,-1}}},
manamax_p={{{1,15},{2,15}},{{1,-1},{2,-1}}},
allres_p={{{1,3},{2,3}},{{1,-1},{2,-1}}},
allresmax_p={{{1,3},{2,3}},{{1,-1},{2,-1}}},
meleedamagereturn_p={{{1,5},{2,5}},{{1,-1},{2,-1}}},
rangedamagereturn_p={{{1,5},{2,5}},{{1,-1},{2,-1}}},

},
cailao4={
lifemax_p={{{1,20},{2,20}},{{1,-1},{2,-1}}},
manamax_p={{{1,20},{2,20}},{{1,-1},{2,-1}}},
allres_p={{{1,4},{2,4}},{{1,-1},{2,-1}}},
allresmax_p={{{1,4},{2,4}},{{1,-1},{2,-1}}},
meleedamagereturn_p={{{1,6},{2,6}},{{1,-1},{2,-1}}},
rangedamagereturn_p={{{1,6},{2,6}},{{1,-1},{2,-1}}},

},
cailao5={
lifemax_p={{{1,25},{2,25}},{{1,-1},{2,-1}}},
manamax_p={{{1,25},{2,25}},{{1,-1},{2,-1}}},
allres_p={{{1,5},{2,5}},{{1,-1},{2,-1}}},
allresmax_p={{{1,5},{2,5}},{{1,-1},{2,-1}}},
meleedamagereturn_p={{{1,7},{2,7}},{{1,-1},{2,-1}}},
rangedamagereturn_p={{{1,7},{2,7}},{{1,-1},{2,-1}}},
meleedamagereturn_v={{{1,5},{2,5}},{{1,-1},{2,-1}}},
rangedamagereturn_v={{{1,5},{2,5}},{{1,-1},{2,-1}}},

},
cailao6={
lifemax_p={{{1,30},{2,30}},{{1,-1},{2,-1}}},
manamax_p={{{1,30},{2,30}},{{1,-1},{2,-1}}},
allres_p={{{1,6},{2,6}},{{1,-1},{2,-1}}},
allresmax_p={{{1,6},{2,6}},{{1,-1},{2,-1}}},
meleedamagereturn_p={{{1,8},{2,8}},{{1,-1},{2,-1}}},
rangedamagereturn_p={{{1,8},{2,8}},{{1,-1},{2,-1}}},
meleedamagereturn_v={{{1,10},{2,10}},{{1,-1},{2,-1}}},
rangedamagereturn_v={{{1,10},{2,10}},{{1,-1},{2,-1}}},

},
cailao7={
lifemax_p={{{1,35},{2,35}},{{1,-1},{2,-1}}},
manamax_p={{{1,35},{2,35}},{{1,-1},{2,-1}}},
allres_p={{{1,7},{2,7}},{{1,-1},{2,-1}}},
allresmax_p={{{1,7},{2,7}},{{1,-1},{2,-1}}},
meleedamagereturn_p={{{1,9},{2,9}},{{1,-1},{2,-1}}},
rangedamagereturn_p={{{1,9},{2,9}},{{1,-1},{2,-1}}},
meleedamagereturn_v={{{1,20},{2,20}},{{1,-1},{2,-1}}},
rangedamagereturn_v={{{1,20},{2,20}},{{1,-1},{2,-1}}},

},
cailao8={
lifemax_p={{{1,40},{2,40}},{{1,-1},{2,-1}}},
manamax_p={{{1,40},{2,40}},{{1,-1},{2,-1}}},
allres_p={{{1,8},{2,8}},{{1,-1},{2,-1}}},
allresmax_p={{{1,8},{2,8}},{{1,-1},{2,-1}}},
meleedamagereturn_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
rangedamagereturn_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
meleedamagereturn_v={{{1,30},{2,30}},{{1,-1},{2,-1}}},
rangedamagereturn_v={{{1,30},{2,30}},{{1,-1},{2,-1}}},

},
cailao9={
lifemax_p={{{1,45},{2,45}},{{1,-1},{2,-1}}},
manamax_p={{{1,45},{2,45}},{{1,-1},{2,-1}}},
allres_p={{{1,9},{2,9}},{{1,-1},{2,-1}}},
allresmax_p={{{1,9},{2,9}},{{1,-1},{2,-1}}},
meleedamagereturn_p={{{1,12},{2,12}},{{1,-1},{2,-1}}},
rangedamagereturn_p={{{1,12},{2,12}},{{1,-1},{2,-1}}},
meleedamagereturn_v={{{1,40},{2,40}},{{1,-1},{2,-1}}},
rangedamagereturn_v={{{1,40},{2,40}},{{1,-1},{2,-1}}},
manashield_p={{{1,15},{2,15}},{{1,-1},{2,-1}}},

},
cailao10={
lifemax_p={{{1,50},{2,50}},{{1,-1},{2,-1}}},
manamax_p={{{1,50},{2,50}},{{1,-1},{2,-1}}},
allres_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
allresmax_p={{{1,10},{2,10}},{{1,-1},{2,-1}}},
meleedamagereturn_p={{{1,15},{2,15}},{{1,-1},{2,-1}}},
rangedamagereturn_p={{{1,15},{2,15}},{{1,-1},{2,-1}}},
meleedamagereturn_v={{{1,50},{2,50}},{{1,-1},{2,-1}}},
rangedamagereturn_v={{{1,50},{2,50}},{{1,-1},{2,-1}}},
manashield_p={{{1,30},{2,30}},{{1,-1},{2,-1}}},
},
}
-----------------------------------------------
--Create by yfeng 2004-05-20
-----------------------------------------------

-----------------------------------------------
--根据2个点，求线形函数f(x)=k*x+b
--y= (y2-y1)*(x-x1)/(x2-x1)+y1
--当x2=x1, 有x=c,该直线是一条垂直于x轴的直线
--这是可以取得y=任意值
--因此，如果已知两点(x1,y1),(x2,y2)可求得过此2点的
--函数为：
function Line(x,x1,y1,x2,y2)
	if(x2==x1) then
		return y2
	end
	return (y2-y1)*(x-x1)/(x2-x1)+y1
end

-----------------------------------------------
--根据2个点，求2次形函数f(x)=a*x2+c
--y= (y2-y1)*x*x/(x2*x2-x1*x1)-(y2-y1)*x1*x1/(x2*x2-x1*x1)+y1
--当x1或者x2 < 0 ,y =0
--当x2=x1, 有x=c,是一条垂直于x轴的直线
--这是可以取得y=任意值
--因此，如果已知两点(x1,y1),(x2,y2)可求得过此2点的
--函数为：extrac
function Conic(x,x1,y1,x2,y2)
	if((x1 < 0) or (x2<0))then 
		return 0
	end
	if(x2==x1) then
		return y2
	end
	return (y2-y1)*x*x/(x2*x2-x1*x1)-(y2-y1)*x1*x1/(x2*x2-x1*x1)+y1
end

-----------------------------------------------
--根据2个点，求-2次形函数f(x)=a*sqrt(x2)+c
--y=(y2-y1)*x/(sqrt(x2)-sqrt(x1))+y1-(y2-y1)/((sqrt(x2)-sqrt(x1))
--当x2或者x1<0, y=0,
--当x1=x2,有x=c,是一条垂直于x轴的直线
--这是可以取得y=任意值
--因此，如果已知两点(x1,y1),(x2,y2)可求得过此2点的
--函数为：extrac
function Extrac(x,x1,y1,x2,y2)
	if((x1 < 0) or (x2<0))then 
		return 0
	end
	if(x2==x1) then
		return y2
	end
	return (y2-y1)*(x-x1)/(x2-x1)+y1
end

-----------------------------------------------
--描绘连接线:Link(x,points)
--根据points提供的一系列点，用相邻的两个点描绘曲线
--return y值
--x 输入值
--points 点集合
--形如：points是形如{{x1,y1,func=xxx},{x2,y2,func=xxx},...{xn,yn,func=xxx}}的映射
function Link(x,points)
	num = getn(points)
	if(num<2) then
		return -1
	end
	for i=1,num do
		if(points[i][3]==nil) then
			points[i][3]=Line
		end
	end
	if(x < points[1][1]) then
		return points[1][3](x,points[1][1],points[1][2],points[2][1],points[2][2])
	end
	if(x > points[num][1]) then
		return points[num][3](x,points[num-1][1],points[num-1][2],points[num][1],points[num][2])
	end
	
	c = 2
	for i=2,num do
		if((x >= points[i-1][1]) and (x <= points[i][1])) then
			c = i
			break
		end
	end
	return points[c][3](x,points[c-1][1],points[c-1][2],points[c][1],points[c][2])
end

------------------------------------------------------
--技能设定格式如下：
--SKILLS={
--	技能名称=	{
--		魔法属性=	{
--			[1]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[2]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[3]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，	
--		}，
--		魔法属性=	{
--			[1]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[2]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[3]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，	
--		}，
--		。。。。。
--	}，
--	技能名称=	{
--		魔法属性=	{
--			[1]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[2]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[3]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，	
--		}，
--		魔法属性=	{
--			[1]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[2]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[3]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，	
--		}，
--		。。。。。
--	}，
--	。。。。。
--}
--如：
--SKILLS={
--	Sanhuan-taoyue={
--		physicsenhance_p={
--			[1]={{1,50},{20,335}},--魔法属性physicsenhance_p参数1，1级时为35，20级时为335，曲线不填，默认线形
--			[2]={{1,0},{20,0}},
--		},--没有[3]，表示魔法属性physicsenhance_p参数2，默认为任何时候都是0
--		lightingdamage_v={
--			[1]={{1,65},{20,350}},
--			[3]={{1,65},{20,350}},
--		}
--	}
--}
--以上描述技能“三环套月”的魔法属性和数值
-----------------------------------------------------------
--函数GetSkillLevelData(levelname, data, level)
--levelname：魔法属性名称
--data：技能名称
--level：技能等级
--return：当技能名称为data，技能等级为level
--			时的魔法属性levelname所需求的三个参数的具体值
-----------------------------------------------------------
function GetSkillLevelData(levelname, data, level)
	if(data==nil) then
		return ""
	end
	if(data == "") then
		return ""
	end
	if(SKILLS[data]==nil) then
		return ""
	end
	if(SKILLS[data][levelname]==nil) then
		return ""
	end
	if(type(SKILLS[data][levelname]) == "function") then
		return SKILLS[data][levelname](level)
	end
	if(SKILLS[data][levelname][1]==nil) then
		SKILLS[data][levelname][1]={{0,0},{20,0}}
	end
	if(SKILLS[data][levelname][2]==nil) then
		SKILLS[data][levelname][2]={{0,0},{20,0}}
	end
	if(SKILLS[data][levelname][3]==nil) then
		SKILLS[data][levelname][3]={{0,0},{20,0}}
	end
	p1=floor(Link(level,SKILLS[data][levelname][1]))
	p2=floor(Link(level,SKILLS[data][levelname][2]))
	p3=floor(Link(level,SKILLS[data][levelname][3]))
	return Param2String(p1,p2,p3)
end;


function Param2String(Param1, Param2, Param3)
return Param1..","..Param2..","..Param3
end;

