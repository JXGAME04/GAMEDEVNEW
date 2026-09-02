--迭代函数，用于计算技能熟练度
--具体方法：
--根据1级熟练度，升级加速度，级数，重复伤害次数，范围，计算出相应等级熟练度
-- SkillExp(i) = Exp1*a^(i-1)*time*range

function SkillExpFunc(Exp0,a,Level,Time,Range)
	return floor(Exp0*(a^(Level-1))*Time*Range/2)
end


SKILLS={
	--武魂堂-àả
	changyinghuichi={ --长缨挥斥
		seriesdamage_p={{{1,1},{20,10}}},
		physicsenhance_p={{{1,10},{20,55}}},
		firedamage_v={
			[1]={{1,10},{20,100}},
			[3]={{1,10},{20,150}}
		},
		addskilldamage1={
			[1]={{1,1981},{2,1981}},
			[3]={{1,1},{20,40}}
		},
		addskilldamage2={
			[1]={{1,1983},{2,1983}},
			[3]={{1,1},{20,35}}
		},
		addskilldamage3={
			[1]={{1,1985},{2,1985}},
			[3]={{1,1},{20,32}}
		},
		missle_speed_v={{{1,20},{20,20}}},
		skill_attackradius={{{1,90},{20,90}}},
		skill_cost_v={{{1,10},{20,10}}}
	},
	wuhuntangdaofa={ --武魂堂刀法
		addphysicsdamage_p={{{1,10},{20,150}},{{1,-1},{2,-1}},{{1,11},{2,11}}},
		deadlystrikeenhance_p={{{1,2},{20,25}},{{1,-1},{2,-1}}}
	},
	gongzhongbingxing={ --公忠秉性
		--meleedamagereturn_p={{{1,-1},{20,-15}},{{1,-1},{2,-1}}},
		--rangedamagereturn_p={{{1,-1},{20,-15}},{{1,-1},{2,-1}}},
		anti_enhancehit_rate={{{1,0},{34,0},{35,5},{39,5},{40,10},{50,10}},{{1,-1},{2,-1}}},
		block_rate={{{1,0},{34,0},{35,5},{39,5},{40,5},{50,5}},{{1,-1},{2,-1}}},
		skill_desc=
			function(level)
				return "Mỗi <color=orange> "..floor(Link(level,SKILLS.gongzhongbingxing.autocastskill[3]) / 18/256).." giây <color>tự động nhận được <color=orange>2 tầng [Nộ]<color>,"
				.."Tối đa có <color=orange>"..floor(Link(level,SKILLS.gongzhongbingxing.special_point_base[3])).."Tầng [Nộ]<color>"
			end,
        autocastskill = {
			{{1,1989*256 + 1},{20,1989*256 + 20}},--目标技能id * 256 + 等级
			{{1,-1},{20,-1}},-- 为1表示算CD，不算CD填-1
			{{1, (3*18*256 + 100)},{20, (3*18*256 + 100)}},-- 间隔帧数 * 256 + 发动几率
		},
        special_point_base={
			{{1,1976},{20,1976}},-- 目标技能id
			{{1,-1},{20,-1}},-- 配在主动技能属性上填0，作为buff属性填-1
			{{1,5 },{20,5}},-- 增加个数
		}
	},
	nuqi={ --公忠秉性-怒（主动）
        special_point_add={
			{{1,1976},{20,1976}},-- 目标技能id
			{{1,0},{20,0}},-- 配在主动技能属性上填0，作为buff属性填-1
			{{1,2},{20,2}},-- 增加个数
		}
	},
	nuqi2={ --公忠秉性-怒2（被动）
		special_point_add={
			{{1,1990},{20,1990}},-- 目标技能id
			{{1,-1},{20,-1}},-- 配在主动技能属性上填0，作为buff属性填-1
			{{1,4 * 256},{20,4 * 256}},-- 最大个数*256 + 增加个数
		}
	},
	hanshanji={ --撼山击
		physicsenhance_p={{{1,25*1.05},{20,215*1.05},{28,295*1.05},{29,305*1.15}}},
		deadlystrike_p={{{1,4*1.05},{20,80*1.05},{28,112*1.05},{29,116*1.15}}},
		cost_sp={
			{{1,1976},{20,1976}},  -- 目标技能id
			{{1,0},{20,0}}, -- 填零
			{{1,2},{20,2}}, -- 吃几个
		},
		skill_desc=
			function(level)
				return "<color=orange>Điều kiện thi triển: <color><color=yellow>"..floor(Link(level,SKILLS.hanshanji.cost_sp[3])).." tầng [Nộ]<color>\n"
				.."Tỉ lệ hồi phục sinh lực: <color=orange>-"..floor(Link(level,SKILLS.hanshanji_zi2.lifereplenish_dec_p[1])).."%<color>\n"
			end,
		missle_speed_v={{{1,22},{20,30},{21,32},{22,32}}},
		skill_attackradius={{{1,22*16},{20,30*16},{21,30*16}}},
	},
	hanshanji_zi={ --撼山击伤害
		physicsenhance_p={{{1,25*1.05},{20,215*1.05},{28,295*1.05},{29,305*1.15}}},
		deadlystrike_p={{{1,4*1.05},{20,80*1.05},{28,112*1.05},{29,116*1.15}}},
		missle_speed_v={{{1,22},{20,30},{21,32},{22,32}}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,4},{10,4}}},
		skill_attackradius={{{1,22*16},{20,30*16},{21,30*16}}},
	},
	hanshanji_zi2={ --撼山击减回复
		lifereplenish_dec_p={{{1,1},{28,25},{34,30},{35,30}},{{1,18*1},{28,18*6},{34,18*9},{35,18*9}}},
	},
	nupitianya={ --怒劈天涯
		seriesdamage_p={{{1,10},{20,50},{21,52}}},
		physicsenhance_p={{{1,10},{20,179}}},
		firedamage_v={
			[1]={{1,70},{20,360}},
			[3]={{1,70},{20,420}}
		},
		addskilldamage1={
			[1]={{1,1983},{2,1983}},
			[3]={{1,1},{20,60}}
		},
		addskilldamage2={
			[1]={{1,1985},{2,1985}},
			[3]={{1,1},{20,50}}
		},
		skill_attackradius={{{1,120},{20,120}}},
		skill_cost_v={{{1,28},{20,48}}}
	},
	baijiangguzhanhan={ --孤将百战酣
		attackspeed_v={{{1,1},{30,43},{31,43}},{{1,-1},{30,-1}}},
	    attackspeed_yan_v={{{1,1},{30,43},{31,43}},{{1,-1},{30,-1}}},
		anti_block_rate={{{1,1},{30,30},{31,30}},{{1,-1},{30,-1}}},
		me2metaldamage_p={{{1,20},{30,20},{31,20}},{{1,-1},{30,-1}}},
		metal2medamage_p={{{1,20},{30,20},{31,20}},{{1,-1},{30,-1}}},
		--anti_do_hurt_p={{{1,1},{9,1},{10,10},{19,10},{20,15},{29,15}},{{1,-1},{30,-1}}}, 
		fastwalkrun_p={{{1,1},{9,1},{10,10},{19,10},{20,20},{29,20},{30,30},{31,30}},{{1,-1},{30,-1}}},
		allres_yan_p={{{1,0},{29,0},{30,30},{31,30}},{{1,-1},{30,-1}}},
		fasthitrecover_yan_v={{{1,0},{30,0},{31,15},{37,20},{38,20}},{{1,-1},{30,-1}}},
		--returnres_p={{{1,1},{30,20},{31,20}},{{1,-1},{30,-1}}},
		melee_returnres_p={{{1,0},{30,20},{31,20}},{{1,-1},{30,-1}}}, 
	},
	jicanhunurou={ --饥餐胡虏肉
		physicsenhance_p={{{1,25},{20,231}}},
		seriesdamage_p={{{1,10},{20,50},{21,52}}},
		firedamage_v={
			[1]={{1,10},{20,482}},
			[3]={{1,10},{20,482}}
		},
		addskilldamage1={
			[1]={{1,1983},{2,1983}},
			[3]={{1,1},{20,35}}
		},
		addskilldamage2={
			[1]={{1,1985},{2,1985}},
			[3]={{1,1},{20,32}}
		},
		skill_cost_v={{{1,28},{20,48}}},
		missle_speed_v={{{1,30},{20,30}}},
		skill_attackradius={{{1,120},{20,120}}},
	},
	wumuyizhi={ --武穆遗志
		lock_life={
			{{1,100},{35,3000},{36,3400}}, -- 锁定的数值
			{{1,18*1},{20,18*3},{30,18*4},{41,18*4}}, -- 持续时间，以buff的第一个属性为准
			{{1,1},{20,1,}},-- 0：无效果， 1：不能低于锁定值， 2：不能高于锁定值，3：只能等于锁定值
		},
		cast_when_buff_removed = {
			{{1,1991},{2,1991}}, -- 要放的技能
			{{1,-1},{2,-1}}, -- 等级，-1由玩家决定
			{{1,1984},{1,1984}} -- 用这个技能的等级去放1991的技能
		},
		ignorenegativestate_p={
			{{1,1},{15,1},{20,1},{21,1}},
			{{1,18},{20,18},{21,18}},
		},
		skill_mintimepercast_v={{{1,60*18},{20,50*18},{30,45*18},{40,40*18},{41,40*18}}},
		skill_mintimepercastonhorse_v={{{1,60*18},{10,55*18},{20,50*18},{30,45*18},{40,40*18},{41,40*18}}},
		anti_do_hurt_p={{{1,1},{35,35}},{{1,18*1},{35,18*5},{40,18*7},{41,18*7}}},
		anti_do_stun_p={{{1,1},{35,35}},{{1,18*1},{35,18*5},{40,18*7},{41,18*7}}},
		fasthitrecover_v={{{1,50},{35,1000}},{{1,18*1},{35,18*5},{40,18*7},{41,18*7}}},
		stuntimereduce_p={{{1,50},{35,1000}},{{1,18*1},{35,18*5},{40,18*7},{41,18*7}}},
		skill_enhance={{{1,3},{35,100},{36,110}},{{1,18*1},{35,18*5},{40,18*7},{41,18*7}}},
		skill_desc=
			function(level)
				return "<color=orange>Điều kiện thi triển: <color><color=yellow>"..floor(Link(level,SKILLS.wumuyizhi.cost_sp[3])).." tầng [Nộ]<color>\n"
				.."Thi triển kỹ năng, <color=orange>lập tức<color> xóa trạng thái bất lợi của bản thân\n"
				.."Trong thời gian duy trì trạng thái kỹ năng, tăng khả năng chiến đấu và sinh lực không thể giảm đến <color=orange>"..floor(Link(level,SKILLS.wumuyizhi.lock_life[1])).."<color> trở xuống\n"
				.."Thời gian duy trì: <color=orange>"..floor(Link(level,SKILLS.wumuyizhi.lock_life[2]) / 18).." giây<color>, "
				.."Thời gian khôi phục chiêu thức: <color=orange>"..floor(Link(level,SKILLS.wumuyizhi.skill_mintimepercast_v[1]) / 18).." giây<color>\n"
			end,
		cost_sp={
			{{1,1976},{20,1976}},  -- 目标技能id
			{{1,0},{20,0}}, -- 填零
			{{1,1},{20,1}}, -- 吃几个
		},
	},
	keyinxiongnuxue={ --渴饮匈奴血
		physicsenhance_p={{{1,12*1.05},{15,100*1.05},{20,206*1.05},{28,375*1.05},{29,396*1.05}}},
		seriesdamage_p={{{1,20},{15,20},{20,60},{21,62}}},
		firedamage_v={
			[1]={{1,70*1.05},{15,150*1.05},{20,285*1.05},{28,501*1.05},{29,528*1.15}},
			[3]={{1,70*1.05},{15,200*1.05},{20,432*1.05},{28,803*1.05},{29,849*1.15}}
		},
		addskilldamage1={
			[1]={{1,1985},{2,1985}},
			[3]={{1,1},{20,32}}
		},
		skill_cost_v={{{1,30},{20,50}}},
		missle_speed_v={{{1,35},{20,35}}},
		skill_attackradius={{{1,160},{20,160}}},
		addskillexp1={{{1,0},{2,0}},{{1,1},{20,1}},{{1,0},{2,0}}},
		skill_skillexp_v={{	{1,SkillExpFunc(8600,1.15,1,1,1)},
							{2,SkillExpFunc(8600,1.15,2,1,1)},
							{3,SkillExpFunc(8600,1.16,3,1,1)},
							{4,SkillExpFunc(8600,1.17,4,1,1)},
							{5,SkillExpFunc(8600,1.18,5,1,1)},
							{6,SkillExpFunc(8600,1.19,6,2,1)},
							{7,SkillExpFunc(8600,1.20,7,2,1)},
							{8,SkillExpFunc(8600,1.21,8,2,1)},
							{9,SkillExpFunc(8600,1.22,9,2,1)},
							{10,SkillExpFunc(8600,1.23,10,2,1)},
							{11,SkillExpFunc(8600,1.24,11,2,1)},
							{12,SkillExpFunc(8600,1.23,12,2,1)},
							{13,SkillExpFunc(8600,1.22,13,2,1)},
							{14,SkillExpFunc(8600,1.21,14,2,1)},
							{15,SkillExpFunc(8600,1.20,15,3,1)},
							{16,SkillExpFunc(8600,1.19,16,3,1)},
							{17,SkillExpFunc(8600,1.18,17,3,1)},
							{18,SkillExpFunc(8600,1.17,18,3,1)},
							{19,SkillExpFunc(8600,1.16,19,3,1)},
							{20,SkillExpFunc(8600,1.15,20,4,1)},
						}},
	},
	zhongwuliufeng={ --忠武流风
		attackrating_p={{{1,10},{20,200}}},
		skill_skillexp_v={{	{1,17851239},
							{2,19487603},
							{3,22760330},
							{4,27669421},
							{5,34214875},
							{6,42396694},
							{7,52214875},
							{8,63669421},
							{9,76760330},
							{10,91487603},
							{11,107851239},
							{12,135669421},
							{13,174942148},
							{14,225669421},
							{15,274418181},
							{16,344618181},
							{17,425738181},
							{18,517778181},
							{19,620738181},
							{20,620738181},
							}},	
		skill_desc=
			function(level)
				return "Sau khi chấm dứt trạng thái <color=yellow>Vũ Mục Di Thư<color>, bản thân hồi sinh lực: <color=orange>"..floor(Link(level,SKILLS.zhongwuliufengBUFF.resume_life_p[1])).."% x Sinh lực tối đa bản thân<color>\n\n"
			end,
	},
	zhongwuliufengBUFF={ --忠武流风回血BUFF
		resume_life_p = {
			{{1,30},{28,60},{34,70},{35,71}}, -- 回百分之几,负数无效
			{{1,1},{2,1}} -- 持续时间，每帧生效
		},
	},
	chanjianchuning={ --铲奸除佞
		physicsenhance_p={{{1,17},{40,680}}},
		seriesdamage_p={{{1,40},{15,40},{20,80},{21,82}}},
		firedamage_v={
			[1]={{1,15},{10,150},{20,300},{50,750},{51,750}},
			[3]={{1,15},{10,150},{20,300},{50,750},{51,750}}
		},
		reset_bufftime = {
			{{1,1988},{20,1988}}, -- 目标技能id
			{{1,-1},{20,-1}}, -- 重置时间，-1表示由目标技能决定
			{{1,0},{20,0}} -- 能否将目标技能的时间缩短
		},
		skill_desc=
			function(level)
				return "Sau khi đối phương trúng đòn <color=yellow>Trừ Gian Diệt Nịnh<color>, nếu đối phương đã có <color=orange>trạng thái bất lợi Lay Sơn Kích<color> sẽ tạo lại thời gian duy trì trạng thái này\n\n"
			end,
		skill_cost_v={{{1,18},{20,55},{23,66},{26,72}}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,8},{10,8}}},
		missle_speed_v={{{1,40},{20,40}}},
		skill_attackradius={{{1,480},{20,480}}},
		skill_skillexp_v={{	{1,300},
												{2,600},
												{3,1000},
												{4,1500},
												{5,2100},
												{6,2800},
												{7,3600},
												{8,4500},
												{9,5500},
												{10,6600},
												{11,7800},
												{12,9100},
												{13,10500},
												{14,12000},
												{15,13600},
												{16,15300},
												{17,17100},
												{18,19000},
												{19,21400},
												{20,90000},
												{21,120000},
												{22,150000},
												{23,200000},
												{24,250000},
												{25,300000},
												{26,390000},
												}},	
	},
	feixuezhiren={ --沸血之刃
		seriesdamage_p={{{1,40},{15,40},{20,80},{21,82}}},
		physicsenhance_p={{{1,10*0.5},{20,179*0.5}}},
		firedamage_v={
			[1]={{1,70*0.5},{20,360*0.5}},
			[3]={{1,70*0.5},{20,420*0.5}}
		},
		skill_attackradius={{{1,480},{20,480}}},
		skill_cost_v={{{1,28},{20,48}}}
	},
	jingzhongbaoguo= --精忠报国
	{
		skill_desc=
			function(level)
				return "<color=water>[Dũng Cảm Tiến Lên]<color>, ".."Giúp bản thân tăng tốc độ di chuyển <color=orange>"..floor(Link(level,SKILLS.fenyongdangxian.fastwalkrun_p[1]))..
				"%<color> tốc độ di chuyển trong <color=orange>"..floor(Link(level,SKILLS.fenyongdangxian.fastwalkrun_p[2]) / 18).."<color> giây \n"
			end,	
		autoreplyskill=
		{--自动触发奋勇当先
			{
				{1,1987*256 + 1},{20,1987*256 + 20},{21,1987*256 + 21}
			},
			{
				{1,-1},{20,-1}
			},
			{
				{1,15*18*256 + 1},{20,15*18*256 + 20},{21,15*18*256 + 21}
			}
		},
	},
	 fenyongdangxian={ --奋勇当先
		fastwalkrun_p={{{1,10},{20,30}},{{1,5*18},{20,13*18},{21,13*18}}},--加跑速
	 },
	 ---盾系
	 duozhai_yangqi={ --夺寨扬旗
	 	physicsenhance_p={{{1,5},{20,55}}},
		skill_cost_v={{{1,12},{20,20}}},
		firedamage_v={
			[1]={{1,5},{20,50}},
			[3]={{1,5},{20,50}}
		},
		addskilldamage1={
			[1]={{1,1963},{2,1963}},
			[3]={{1,1},{20,45}}
		},
		addskilldamage2={
			[1]={{1,1967},{2,1967}},
			[3]={{1,1},{20,35}}
		},
		addskilldamage3={
			[1]={{1,1969},{2,1969}},
			[3]={{1,1},{20,38}}
		},
	},
	wuhuntang_dunfa={ --武魂堂盾法
		addphysicsdamage_p={{{1,15},{20,215}},{{1,-1},{2,-1}},{{1,12},{2,12}}},
		--attackratingenhance_p={{{1,35},{20,272}},{{1,-1},{2,-1}}},
		attackratingenhance_p={{{1,20},{20,320}},{{1,-1},{2,-1}}},--增加命中率 LLX140722
		addfiredamage_v={{{1,20},{30,315}},{{1,18*120},{30,18*360}}},
		deadlystrikeenhance_p={{{1,6},{20,35}},{{1,-1},{2,-1}}}
	},
	chaotian_que={ --朝天阙
		seriesdamage_p={{{1,10},{20,50},{21,52}}},
		physicsenhance_p={{{1,25},{20,231}}},
		deadlystrike_p={{{1,4},{20,55}}},
		firedamage_v={
			[1]={{1,10},{20,482}},
			[3]={{1,10},{20,482}}
		},
		addskilldamage1={
			[1]={{1,1967},{2,1967}},
			[3]={{1,1},{20,65}}
		},
		addskilldamage2={
			[1]={{1,1969},{2,1969}},
			[3]={{1,1},{20,54}}
		},
		missle_speed_v={{{1,26},{20,26}}},
		missle_lifetime_v={{{1,4},{2,4}}},
		skill_attackradius={{{1,78},{20,78}}},
		skill_cost_v={{{1,12},{20,20}}}
	},
	zhenbian_chui={ --镇边陲
		physicsenhance_p={{{1,45},{20,445}}},
		seriesdamage_p={{{1,20},{15,20},{20,60},{21,62}}},
		deadlystrike_p={{{1,4},{20,65}}},
		firedamage_v={
			[1]={{1,6},{15,100},{20,378}},
			[3]={{1,6},{15,100},{20,378}}
		},
		addskilldamage1={
			[1]={{1,1969},{2,1969}},
			[3]={{1,1},{20,30}}
		},
		missle_speed_v={{{1,30},{20,30}}},
		missle_lifetime_v={{{1,4},{2,4}}},
		skill_attackradius={{{1,90},{20,90}}},
		skill_cost_v={{{1,30},{20,30}}},
--		skill_eventskilllevel={{{1,1},{20,20}}},
--		skill_startevent={
--			[1]={{1,0},{10,0},{10,1},{20,1}},
--			[3]={{1,378},{20,378}}
--		},
--		skill_showevent={{{1,0},{10,0},{10,1},{20,1}}},
		addskillexp1={{{1,0},{2,0}},{{1,1},{20,1}},{{1,0},{2,0}}},
		skill_skillexp_v={{	{1,SkillExpFunc(11600,1.15,1,1,1)},
							{2,SkillExpFunc(11600,1.15,2,1,1)},
							{3,SkillExpFunc(11600,1.16,3,1,1)},
							{4,SkillExpFunc(11600,1.17,4,1,1)},
							{5,SkillExpFunc(11600,1.18,5,1,1)},
							{6,SkillExpFunc(11600,1.19,6,1,1)},
							{7,SkillExpFunc(11600,1.20,7,1,1)},
							{8,SkillExpFunc(11600,1.21,8,1,1)},
							{9,SkillExpFunc(11600,1.22,9,1,1)},
							{10,SkillExpFunc(11600,1.23,10,1,1)},
							{11,SkillExpFunc(11600,1.24,11,1,1)},
							{12,SkillExpFunc(11600,1.23,12,1,1)},
							{13,SkillExpFunc(11600,1.22,13,1,1)},
							{14,SkillExpFunc(11600,1.21,14,1,1)},
							{15,SkillExpFunc(11600,1.20,15,1,1)},
							{16,SkillExpFunc(11600,1.19,16,1,1)},
							{17,SkillExpFunc(11600,1.18,17,1,1)},
							{18,SkillExpFunc(11600,1.17,18,1,1)},
							{19,SkillExpFunc(11600,1.16,19,1,1)},
							{20,SkillExpFunc(11600,1.15,20,1,1)},
							}},
	},
	qingyin_tiruilv1={ --请缨提锐旅
		sorbdamage_yan_p={{{1,10},{15,8},{32,25}},{{1,36},{2,36}}},
		anti_do_hurt_p={{{1,1},{15,3},{20,5},{25,6}},{{1,36},{2,36}}},
		anti_enhancehit_rate={{{1,1},{5,2},{10,3},{15,4},{20,5},{23,6},{26,7}},{{1,36},{20,36}}},
		--lifereplenish_v={{{1,1},{15,4},{25,15},{26,15}},{{1,18},{2,18}}},
		--manareplenish_v={{{1,1},{15,4},{25,15},{26,15}},{{1,18},{2,18}}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
		cost_sp={
			{{1,1976},{20,1976}},  -- 目标技能id
			{{1,0},{20,0}}, -- 填零
			{{1,4},{20,4}}, -- 吃几个
		},
		skill_desc=
			function(level)
				return "<color=orange>Điều kiện thi triển: <color><color=yellow>"..floor(Link(level,SKILLS.qingyin_tiruilv1.cost_sp[3])).." tầng [Nộ]<color>\n"
			end,
		--skill_desc=
			--function(level)

				--local nMag = floor(Link(level,SKILLS.longxuan2.colddamage_v[3]))
				--local nPois = format("%.2f", (floor(Link(level,SKILLS.longxuan2.frozen_action[2])*100/18 )/100))
				--local nAtk = floor(Link(level,SKILLS.podao_pojian.candetonate3[3]))
				--return format("引爆气场造成<color=orange>%d<color>点的冰属性伤害，并使敌人定身<color=orange>%.2f<color>秒，\n", nMag, nPois)
			--end,
		addskilldamage1={
				[1]={{1,1382},{2,1382}},
				[3]={{1,1},{20,60},{31,70}}
			},
	},
	qingyin_tiruilv2={ --请缨提锐旅
		candetonate1={
			[1]={{1,421*256+0},{2,421*256+0}},
			[3]={{1,36},{26,36},{31,36},{32,36}},
		},
	},
	huishi_mielu={ --挥师灭虏
		physicsenhance_p={{{1,55},{20,535},{23,686},{26,762}}},
		seriesdamage_p={{{1,40},{15,40},{20,80},{21,82}}},
		deadlystrike_p={{{1,10},{10,20},{20,40},{30,60},{40,80},{45,85}}},
		firedamage_v={
			[1]={{1,10},{15,150},{20,500},{23,900},{26,1200}},
			[3]={{1,10},{15,150},{20,500},{23,900},{26,1200}}
		},
		--missle_speed_v={{{1,24},{20,24},{21,24}}},
		missle_speed_v={{{1,55},{20,55},{21,55}}},--增加刀翠烟的子弹飞行速度 ByLLX 140722
		skill_attackradius={{{1,480},{20,480},{21,480}}},
		skill_cost_v={{{1,48},{20,72},{23,79}}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_collideevent={
			[1]={{1,0},{10,0},{10,1},{20,1}},
			[3]={{1,1970},{20,1970}}
		},
		skill_showevent={{{1,0},{10,0},{10,4},{20,4}}},
		skill_skillexp_v={{	{1,300},
												{2,600},
												{3,1000},
												{4,1500},
												{5,2100},
												{6,2800},
												{7,3600},
												{8,4500},
												{9,5500},
												{10,6600},
												{11,7800},
												{12,9100},
												{13,10500},
												{14,12000},
												{15,13600},
												{16,15300},
												{17,17100},
												{18,19000},
												{19,21400},
												{20,90000},
												{21,120000},
												{22,150000},
												{23,200000},
												{24,250000},
												{25,300000},
												{26,390000},
												}},	
	},
	huishi_mielu_2={ --挥师灭虏第2式
		physicsenhance_p={{{1,10},{40,300}}},
		seriesdamage_p={{{1,40},{15,40},{20,80},{21,82}}},
		firedamage_v={
			[1]={{1,1},{30,230},{31,230}},
			[3]={{1,1},{30,400},{31,400}}
		},
		missle_speed_v={{{1,45},{20,45},{21,45}}},
		skill_misslenum_v={{{1,1},{5,1},{20,3},{21,3}}},
	},
	zhiyong_new={ --智勇超伦
		lifemax_p={{{1,5},{25,60},{31,70},{32,70}},{{1,12*18*256},{20,12*18*256},{31,12*18*256},{32,12*18*256}}},
		lifemax_yan_p={{{1,5},{25,60},{31,70},{32,70}},{{1,12*18*256},{20,12*18*256},{31,12*18*256},{32,12*18*256}}},
		--allres_yan_p={{{1,1},{30,25},{40,25}},{{1,12*18*256},{20,12*18*256},{31,12*18*256},{32,12*18*256}}},
		--allres_p={{{1,1},{30,25},{40,25}},{{1,12*18*256},{20,12*18*256},{31,12*18*256},{32,12*18*256}}}
		},
--		missle_speed_v={{{1,28},{20,32},{21,32}}},
--		skill_attackradius={{{1,448},{20,512},{21,512}}},
--		skill_cost_v={{{1,30},{20,35}}}
	zhiyong_new1={ --智勇超伦主动
		--autoreplyskill={{{1,1363*256 + 1},{20,1363*256 + 20},{21,1363*256 + 21}},{{1,10*60*18},{20,10*60*18}},{{1,15*18*256 + 1},{20,15*18*256 + 3},{31,15*18*256 + 3},{32,15*18*256 + 3}}},
		--增加触发概率，减少冷却时间 LLX140722

		skill_desc=
			function(level)
				return "<color=water>[Chiết Kích]<color> gây cho người tấn công, kỹ năng tấn công tăng: Giảm <color=orange>"..floor(-Link(level,SKILLS.zhiyong_chaolun1.skill_enhance[1]))..
				"%<color> tốc độ di chuyển trong <color=orange>"..floor(Link(level,SKILLS.zhiyong_chaolun1.skill_enhance[2]) / 18).."<color> giây \n"
			end,

		autoreplyskill={{{1,16777216+1973*256 + 1},{20,16777216+1973*256 + 20},{21,16777216+1973*256 + 21}},{{1,10*60*18},{20,10*60*18}},{{1,12*18*256 + 1},{20,12*18*256 + 20},{30,12*18*256 + 30},{31,12*18*256 + 30}}},
		skill_cost_v={{{1,48},{20,72},{23,79}}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
	},
	wuhuntangjiangongsu=
	{--武魂堂减攻速
		attackspeed_yan_v=
		{
			{
				{1,-1},{10,-5},{11,-6},{20,-15},{21,-15}
			},
			{
				{1,1*18},{20,5*18},{21,5*18}
			}
		},
		castspeed_yan_v=
		{
			{
				{1,-1},{10,-5},{11,-6},{20,-15},{21,-15}
			},
			{
				{1,1*18},{20,5*18},{21,5*18}
			},
		},
		attackspeed_v={{{1,-1},{10,-5},{11,-6},{20,-15},{21,-15}},{{1,1*18},{20,5*18},{21,5*18}}},
		castspeed_v={{{1,-1},{10,-5},{11,-6},{20,-15},{21,-15}},{{1,1*18},{20,5*18},{21,5*18}}},
		skill_enhance={
			[1]={{1,0},{20,0},{21,-2},{22,-4}},
			[2]={{1,1*18},{20,5*18},{21,5*18}},
			[3]={},
			},
	},
	zhiyong_chaolun1=
	{--智勇超伦减攻击
		skill_enhance={
			[1]={{1,-1},{20,-20},{21,-22},{22,-24}},
			[2]={{1,1*18},{20,5*18},{21,5*18}},
			[3]={},
			},
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
--return yֵ
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
--		}Êơ
--		魔法属性=	{
--			[1]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[2]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[3]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，	
--		}Êơ
--		。。。。。
--	}Êơ
--	技能名称=	{
--		魔法属性=	{
--			[1]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[2]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[3]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，	
--		}Êơ
--		魔法属性=	{
--			[1]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[2]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，
--			[3]={{级别,数值，曲线}，{级别，数值，曲线}，。。。。}，	
--		}Êơ
--		。。。。。
--	}Êơ
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

