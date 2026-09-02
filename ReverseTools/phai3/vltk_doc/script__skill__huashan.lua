--迭代函数，用于计算技能熟练度
--具体方法：
--根据1级熟练度，升级加速度，级数，重复伤害次数，范围，计算出相应等级熟练度
-- SkillExp(i) = Exp1*a^(i-1)*time*range
function SkillExpFunc(Exp0,a,Level,Time,Range)
	return floor(Exp0*(a^(Level-1))*Time*Range/2)
end


SKILLS={
	--华山
	baihong_guanri={ --白虹贯日
		physicsenhance_p={
			[1]={{1,30},{20,75}},
		},
		seriesdamage_p={{{1,1},{20,10}}},
		colddamage_v={
			[1]={{1,10},{20,80}},
			[3]={{1,20},{20,80}}
		},
		reduceskillcd1={
			[1]={{1,1368},{2,1368}},
			[3]={{1,6},{20,18*1}}
		},
		reduceskillcd2={
			[1]={{1,1363},{2,1363}},
			[3]={{1,6},{20,18*1}}
		},
		addskilldamage1={
			[1]={{1,1351},{2,1351}},
			[3]={{1,1},{20,60},{31,65}}
		},
		addskilldamage2={
			[1]={{1,1355},{2,1355}},
			[3]={{1,1},{20,50},{31,50}}
		},
		addskilldamage3={
			[1]={{1,1360},{2,1360}},
			[3]={{1,1},{20,15},{31,20}}
		},
		--missle_speed_v={{{1,20},{20,24}}},
		--skill_attackradius={{{1,320},{20,384}}},
		--skill_cost_v={{{1,10},{20,10}}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
	},
	jianzong_zongjue={ --剑宗总决
		addphysicsdamage_p={{{1,15},{20,75},{26,150},{32,220},{33,220}},{{1,-1},{2,-1}},{{1,0},{2,0}}},
		deadlystrikeenhance_p={{{1,6},{20,26}},{{1,-1},{2,-1}}},
--		deadlystrikeenhance_p={{{1,6},{20,36}},{{1,-1},{2,-1}}},
		attackspeed_yan_v={{{1,6},{26,32},{31,32},{32,32}},{{1,-1},{2,-1}}},
		attackspeed_v={{{1,6},{26,32},{31,32},{32,32}},{{1,-1},{2,-1}}},
		manareplenish_v={{{1,-10000},{20,-10000}},{{1,-1},{2,-1}}},
		manamax_yan_v={{{1,-1000},{20,-20000}},{{1,-1},{2,-1}}},
		manamax_v={{{1,-1000},{20,-20000}},{{1,-1},{2,-1}}},
		manareplenish_p={{{1,-200},{20,-200}},{{1,-1},{2,-1}}},
		lifemax_yan_p={{{1,10},{25,30},{36,30}},{{1,-1},{2,-1}}},
		addcolddamage_v={{{1,5},{20,100},{50,250},{51,250}},{{1,-1},{2,-1}}},
		anti_hitrecover={{{1,0},{10,5},{30,15},{31,15}},{{1,-1},{2,-1}}},
	},
	yangwu_jianfa={ --养吾剑法
		clearallcd={{{1,1},{25,2},{26,3},{27,3},{31,4},{32,4}},{{1,-1},{2,-1}}},
		deadlystrikeenhance_p={{{1,6},{30,36},{31,38},{32,38}},{{1,-1},{2,-1}}},
	},
	jinyan_hengkong={ --金雁横空
		seriesdamage_p={{{1,10},{20,50},{21,52}}},
		colddamage_v={
			[1]={{1,35},{20,450}},
			[3]={{1,45},{20,550}}
		},
		deadlystrike_p={{{1,6},{20,40}},{{1,-1},{2,-1}}},
		physicsenhance_p={
			--[1]={{1,30},{20,600},{30,900},{50,1500},{51,1500}}, 
			[1]={{1,30},{45,2000}}, 
		},
		reduceskillcd1={
			[1]={{1,1363},{2,1363}},
			[3]={{1,6},{20,18*1}}
		},
		reduceskillcd2={
			[1]={{1,1368},{2,1368}},
			[3]={{1,6},{20,18*1}}
		},
		addskilldamage1={
			[1]={{1,1363},{2,1363}},
			[3]={{1,15},{20,112},{21,114}}
		},
		addskilldamage2={
			[1]={{1,1368},{2,1368}},
			[3]={{1,1},{20,70}}
		},
		--skill_attackradius={{{1,384},{20,416}}}, 
		skill_attackradius={{{1,512},{20,512}}},
		--skill_cost_v={{{1,25},{20,35}}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
		skill_desc=
			function(level)

				local nMag = floor(Link(level,SKILLS.podao_pojian.candetonate1[3]))
				return format("Gỡ đồ trên người <color=orange>%d<color> trong phạm vi màn hình<color=blue> Kiếm Hoa Hoán Tinh<color>, <color=blue> Băng Tước Việt Chi<color>, <color=blue> U Hồn Phệ ảnh<color>,\n", nMag)
			end,
	},
	podao_pojian={ --破刀破剑式
		candetonate1={
			[1]={{1,323*256+1},{2,323*256+1}},
			[3]={{1,10},{15,25},{26,128},{27,128}}
		},
		candetonate2={
			[1]={{1,326*256+1},{2,326*256+1}},
			[3]={{1,10},{15,25},{26,128},{27,128}}
		},
		candetonate3={
			[1]={{1,329*256+1},{2,329*256+1}},
			[3]={{1,10},{15,25},{26,128},{27,128}}
		},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
	},
	xiyi_jianfa={ --希夷剑法
		addphysicsdamage_p={{{1,35},{20,145},{26,185},{32,245},{33,245}},{{1,-1},{2,-1}},{{1,0},{2,0}}},
		--deadlystrikeenhance_p={{{1,6},{20,36}},{{1,-1},{2,-1}}},
	},
	tianshen_daoxuan={ --天绅倒悬
		seriesdamage_p={{{1,5},{20,30}}},
		colddamage_v={
			[1]={{1,35},{20,315}},
			[3]={{1,45},{20,450}}
		},
		deadlystrike_p={{{1,10},{20,52},{21,52}},{{1,-1},{2,-1}}},
		physicsenhance_p={
			[1]={{1,30},{44,264},{45,264}},
		},
		reduceskillcd1={
			[1]={{1,1363},{2,1363}},
			[3]={{1,6},{20,18*1}}
		},
		reduceskillcd2={
			[1]={{1,1368},{2,1368}},
			[3]={{1,6},{20,18*1}}
		},
		addskilldamage1={
			[1]={{1,1363},{2,1363}},
			[3]={{1,15},{20,112},{21,114}}
		},
		addskilldamage2={
			[1]={{1,1368},{2,1368}},
			[3]={{1,1},{20,70}}
		},
		--skill_attackradius={{{1,384},{20,416}}},
		--skill_cost_v={{{1,25},{20,35}}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
		skill_desc=
			function(level)

				local nMag = floor(Link(level,SKILLS.pogun_poqiang.candetonate1[3]))
				return format("Gỡ đồ trên người <color=orange>%d<color> trong phạm vi màn hình<color=blue> Bỗng Huýnh Lược Địa<color>, <color=blue>Giang Hải Nộ Lan<color>,\n", nMag)
			end,
	},
	pogun_poqiang={ --破长兵式
		candetonate1={
			[1]={{1,336*256+1},{2,336*256+1}},
			[3]={{1,10},{15,25},{26,128},{27,128}}
		},
		candetonate2={
			[1]={{1,337*256+1},{2,337*256+1}},
			[3]={{1,10},{15,25},{26,128},{27,128}}
		},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
		
	},
	huanyan_yunyan={ --幻眼云湮
		addcoldmagic_v={{{1,20},{30,315}},{{1,300*18},{2,300*18}}},
		addcolddamage_v={{{1,100},{30,315}},{{1,300*18},{2,300*18}}},
		--coldenhance_p={{{1,5},{9,5},{10,20},{19,20},{20,40},{29,40},{30,60},{39,60},{40,80},{49,80},{50,100},{51,100}},{{1,60*18},{2,180*18}}}, 
		coldenhance_p={{{1,5},{40,200},{40,205}},{{1,300*18},{2,300*18}}},
		lifemax_yan_p={{{1,10},{25,35},{36,55},{37,55}},{{1,300*18},{2,300*18}}},
		me2firedamage_p={{{1,1},{30,20},{31,20}},{{1,300*18},{2,300*18}}},--对火系伤害增加：15%
		fire2medamage_p={{{1,1},{30,20},{31,20}},{{1,300*18},{2,300*18}}},--减少来自火系的伤害：15%
		anti_block_rate={{{1,1},{10,10},{30,30},{31,30}},{{1,300*18},{2,300*18}}},
		--fasthitrecover_yan_v={{{1,1},{10,5},{40,20},{41,20}},{{1,-1},{2,-1}}},
		fasthitrecover_yan_v={{{1,1},{40,0},{41,15},{47,20},{48,20}},{{1,300*18},{2,300*18}}},
		--anti_do_hurt_p={{{1,0},{29,0},{30,5},{40,10},{41,10}},{{1,300*18},{2,300*18}}}, 
		walkrunshadow={
			[1]={{1,1},{20,1}},
			[2]={{1,300*18},{20,300*18}}
		},
		fastwalkrun_p={{{1,9},{30,40}},{{1,300*18},{2,300*18}}},
		fatallystrikeres_p={{{1,1},{40,40},{41,41}},{{1,300*18},{30,300*18}}}, 
	},
	cangsong_yingke={ --苍松迎客
		seriesdamage_p={{{1,0},{20,5},{26,10},{32,15},{33,15}}},
		colddamage_v={
			[1]={{1,10},{20,30},{26,40},{32,50},{33,50}},
			[3]={{1,10},{20,30},{26,40},{32,50},{33,50}}
		},
		deadlystrike_p={{{1,10},{20,38},{21,38}},{{1,-1},{2,-1}}},
		physicsenhance_p={
			[1]={{1,1},{10,10},{20,20},{30,30},{40,40},{41,40}},
		},
		reduceskillcd1={
			[1]={{1,1363},{2,1363}},
			[3]={{1,6},{20,18*1}}
		},
		reduceskillcd2={
			[1]={{1,1368},{2,1368}},
			[3]={{1,6},{20,18*1}}
		},
		addskilldamage1={
			[1]={{1,1363},{2,1363}},
			[3]={{1,15},{20,112},{21,114}}
		},
		addskilldamage2={
			[1]={{1,1368},{2,1368}},
			[3]={{1,1},{20,50}}
		},
		--skill_attackradius={{{1,384},{20,416}}},
		--skill_cost_v={{{1,25},{20,35}}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
		skill_desc=
			function(level)

				local nMag = floor(Link(level,SKILLS.poanqi_pozhang.candetonate1[3]))
				return format("Gỡ đồ trên người <color=orange>%d<color> trong phạm vi màn hình<color=blue> Vô ảnh Xuyên<color>, <color=blue> Thiết Liên Tứ Sát<color>, <color=blue> Càn Khôn Nhất Trịch<color>,\n", nMag)
			end,
	},
	poanqi_pozhang={ --破唐门
		candetonate1={
			[1]={{1,331*256+1},{2,331*256+1}},
			[3]={{1,10},{15,25},{26,128},{27,128}}
		},
		candetonate2={
			[1]={{1,332*256+1},{2,332*256+1}},
			[3]={{1,10},{15,25},{26,128},{27,128}}
		},
		candetonate3={
			[1]={{1,333*256+1},{2,333*256+1}},
			[3]={{1,10},{15,25},{26,128},{27,128}}
		},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
	},
	
--	liushui={--流水1
--		fastwalkrun_p={{{1,9},{20,66}},{{1,18},{2,18}}}
--	},
	duoming_lianhuan={ --夺命连环三仙剑
		physicsenhance_p={{{1,10},{15,156},{20,566},{26,788},{32,812},{33,812}}},
		seriesdamage_p={{{1,10},{20,60},{21,62}}},
		colddamage_v={
			[1]={{1,10},{20,70},{26,100},{31,130},{32,130}},
			[3]={{1,10},{20,70},{26,100},{31,130},{32,130}}
		},
		deadlystrike_p={{{1,5},{20,20}}},
		addskilldamage1={
			[1]={{1,1368},{2,1368}},
			[3]={{1,1},{20,70}}
		},
--		missle_speed_v={{{1,28},{20,32},{21,32}}},
--		skill_attackradius={{{1,448},{20,512},{21,512}}},
--		skill_cost_v={{{1,30},{20,35}}}
		
	},
	duoming_start={ --主动辅助夺命连环三仙剑
		autoreplyskill={{{1,(1*65536 + 1363)*256 + 1},{20,(1*65536 + 1363)*256 + 20},{21,(1*65536 + 1363)*256 + 21}},{{1,10*60*18},{20,10*60*18}},{{1,15*18*256 + 1},{20,15*18*256 + 3},{31,15*18*256 + 3},{32,15*18*256 + 3}}},
		manareplenish_v={{{1,-10000},{20,-10000}},{{1,10*60*18},{2,10*60*18}}},
		manareplenish_p={{{1,-200},{20,-200}},{{1,10*60*18},{2,10*60*18}}},
		skill_skillexp_v={{	{1,SkillExpFunc(5000,1.25,1,3,1)},
							{2,SkillExpFunc(5000,1.15,2,3,1)},
							{3,SkillExpFunc(5000,1.16,3,3,1)},
							{4,SkillExpFunc(5000,1.17,4,3,1)},
							{5,SkillExpFunc(5000,1.18,5,3,1)},
							{6,SkillExpFunc(5000,1.19,6,3,1)},
							{7,SkillExpFunc(5000,1.20,7,3,1)},
							{8,SkillExpFunc(5000,1.21,8,3,1)},
							{9,SkillExpFunc(5000,1.22,9,3,1)},
							{10,SkillExpFunc(5000,1.23,10,3,1)},
							{11,SkillExpFunc(5000,1.24,11,3,1)},
							{12,SkillExpFunc(5000,1.23,12,3,1)},
							{13,SkillExpFunc(5000,1.22,13,3,1)},
							{14,SkillExpFunc(5000,1.21,14,3,1)},
							{15,SkillExpFunc(5000,1.20,15,3,1)},
							{16,SkillExpFunc(5000,1.19,16,3,1)},
							{17,SkillExpFunc(5000,1.18,17,3,1)},
							{18,SkillExpFunc(5000,1.17,18,3,1)},
							{19,SkillExpFunc(5000,1.16,19,3,1)},
							{20,SkillExpFunc(5000,1.15,20,3,1)},
							}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
	},
	zixia_jianqi={ --紫霞剑气
		autorescueskill={{{1,1366*256 + 1},{20,1366*256 + 20},{21,1366*256 + 21}},{{1,-1},{20,-1}},{{1,5*18*256 + 1},{20,5*18*256 + 50},{21,5*18*256 + 50}}},
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
				local nMag =  floor(Link(level,SKILLS.zixia_jianqi.autorescueskill[3]) - 5*18*256)
				local nPois = floor(Link(level,SKILLS.zixia_xiaoguo.lifereplenish_v[1]))
				local nTime = floor(Link(level,SKILLS.zixia_xiaoguo.lifereplenish_v[2])/18)
				return format(" Sinh mệnh thấp hơn 25%% trong chớp mắt có <color=orange>%d%%<color> tỷ lệ tiếp xúc mỗi nửa giây hồi phục %d điểm, kéo dài trong %d giây.\n", nMag, nPois, nTime)
			end,
	},
	zixia_xiaoguo={ --紫霞剑气的效果
		--lifereplenish_p={{{1,-1},{20,-10},{26,-15},{27,-15}},{{1,18*1},{26,18*5},{26,18*5}}},
		lifereplenish_v={{{1,500},{20,10000},{21,10000}},{{1,18*1},{26,18*5},{26,18*5}}},
	},
	jiujian_heyi={ --九剑合一
		physicsenhance_p={{{1,30},{50,1500},{51,1500}}},
		seriesdamage_p={{{1,10},{20,80},{21,82}}},
		fatallystrike_p={{{1,2},{15,25},{20,50},{25,75},{26,75}}},
		colddamage_v={
			[1]={{1,15},{20,110},{26,200},{31,244},{32,244}},
			[3]={{1,15},{20,110},{26,200},{31,244},{32,244}}
		},
		deadlystrike_p={{{1,15},{20,54},{26,66},{31,76},{32,76}}},
		skill_misslenum_v={{{1,4},{20,9},{21,9}}},
		anti_block_rate={{{1,1},{30,30},{31,30}}},
		anti_do_hurt_p={{{1,0},{29,0},{30,5},{40,10},{50,10}}},
--		missle_speed_v={{{1,28},{20,32},{21,32}}},
--		skill_attackradius={{{1,448},{20,512},{21,512}}},
--		skill_cost_v={{{1,30},{20,35}}}

	},
	jiujian_start={ --主动辅助九剑合一
		autoattackskill={{{1,1368*256 + 1},{20,1368*256 + 20},{21,1368*256 + 21}},{{1,10*60*18},{20,10*60*18}},{{1,5*18*256 + 1},{9,5*18*256 + 1},{10,5*18*256 + 10},{19,5*18*256 + 10},{20,5*18*256 + 20},{29,5*18*256 + 20},{30,5*18*256 + 30},{34,5*18*256 + 40},{40,5*18*256 + 40},{41,5*18*256 + 40}}},
		manareplenish_v={{{1,-10000},{20,-10000}},{{1,10*60*18},{2,10*60*18}}},
		manareplenish_p={{{1,-200},{20,-200}},{{1,10*60*18},{2,10*60*18}}},
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
		skill_desc=
			function(level)
				local nMag = floor(Link(level,SKILLS.jiujian_start.autoattackskill[3])-5*18*256)
				local nPois = floor(Link(level,SKILLS.jiujian_start.autoattackskill[3])/18/256)
				return format("Khi né tránh đối phương, <color=orange>%d%%<color> tỷ lệ phóng thích <color=blue>Kiếm pháp - Độc Cô Cửu Kiếm<color>, Thời gian đóng băng<color=orange>%d<color> giây.", nMag,nPois)
			end,
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
	},
	haoran_zhiqi={ --浩然之气
		addblockrate={
			[1]={{1,10},{20,3},{21,3}},
			[2]={{1,-1},{20,-1}},
			[3]={{1,1},{19,1},{20,2},{40,3},{41,3}},
		},
		--anti_coldres_yan_p={{{1,1},{40,20},{41,20}},{{1,-1},{20,-1}}},
	},
	wuji_jianqi1={ --无极剑气1
		lifemax_p={{{1,5},{20,80},{31,100},{32,100}},{{1,10},{31,18},{32,18}}},
		lifemax_yan_p={{{1,5},{25,60},{31,70},{32,70}},{{1,10},{31,18},{32,18}}},
		lifereplenish_v={{{1,1},{20,15}},{{1,18},{2,18}}},
		manareplenish_v={{{1,1},{20,15}},{{1,18},{2,18}}},
		colddamage_v={
			[1]={{1,24},{15,420},{20,930},{23,1542},{26,1848}},
			[3]={{1,36},{15,480},{20,1200},{23,2064},{26,2496}}
		},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
		skill_cost_v={{{1,30},{20,400},{26,500},{31,800},{32,800}}},
		skill_desc=
			function(level)

				local nMag = floor(Link(level,SKILLS.longxuan2.colddamage_v[3]))
				local nPois = format("%.2f", (floor(Link(level,SKILLS.longxuan2.frozen_action[2])*100/18 )/100))
				return format("Khí trường bùng nổ tạo thành <color=orange>%d<color> điểm sát thương thuộc tính băng, đồng thời làm kẻ địch đứng yên trong <color=orange>%.2f<color> giây,\n", nMag, nPois)
			end,
		
	},
	
		qingfeng_songshuang={ --清风送爽
			seriesdamage_p={{{1,1},{20,20}}},
			skill_cost_v={{{1,5},{20,43},{26,50}}},
			colddamage_v={
				[1]={{1,10},{20,320}},
				[3]={{1,20},{20,320}}
			},
			addskilldamage1={
				[1]={{1,1382},{2,1382}},
				[3]={{1,1},{20,60},{31,70}}
			},
			skill_eventskilllevel={{{1,1},{20,20}}},
			skill_showevent={{{1,1},{10,1},{20,1}}},
	},
	
	wuji_jianqi2={ --无极剑气2
		candetonate1={
			[1]={{1,419*256+0},{2,419*256+0}},
			[3]={{1,100},{26,180},{31,300},{32,300}},
		},
	},
	longraoshen={ --龙扰身
		manareplenish_v={{{1,1},{20,22}},{{1,-1},{2,-1}}},
		manamax_yan_p={{{1,35},{35,200},{36,200}},{{1,-1},{2,-1}}},
	},
	haina_baichuan={ --海纳百川
		damage2addmana_p={{{1,1},{20,10}},{{1,-1},{20,-1}}},
	},
	longxuan_jianqi2={ --龙旋剑气2
		candetonate1={
			[1]={{1,421*256+0},{2,421*256+0}},
			[3]={{1,360},{26,360},{31,360},{32,360}},
		},
	},
	longxuan_jianqi1={ --龙旋剑气1
		sorbdamage_yan_p={{{1,10},{15,8},{32,25}},{{1,18},{2,18}}},
		anti_do_hurt_p={{{1,1},{15,3},{20,5},{31,5},{32,5}},{{1,18},{2,18}}},
		lifemax_p={{{1,5},{15,25},{20,30},{31,30},{32,30}},{{1,10},{31,18},{32,18}}},
		lifemax_yan_p={{{1,5},{15,10},{25,15},{31,20},{32,20}},{{1,10},{31,18},{32,18}}},
		anti_sorbdamage_yan_p={{{1,1},{20,20},{50,50},{51,50}},{{1,10},{31,18},{32,18}}},
		enhancehiteffect_rate={{{1,1},{20,20},{50,50},{51,50}},{{1,10},{31,18},{32,18}}},
		enhancehit_rate={{{1,1},{20,20},{40,40},{41,40}},{{1,10},{31,18},{32,18}}},
		--lifereplenish_v={{{1,1},{15,4},{25,15},{26,15}},{{1,18},{2,18}}},
		--manareplenish_v={{{1,1},{15,4},{25,15},{26,15}},{{1,18},{2,18}}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
		skill_cost_v={{{1,30},{20,400},{26,500},{31,800},{32,800}}},
		skill_desc=
			function(level)

				local nMag = floor(Link(level,SKILLS.longxuan2.colddamage_v[3]))
				local nPois = format("%.2f", (floor(Link(level,SKILLS.longxuan2.frozen_action[2])*100/18 )/100))
				return format("Khí trường bùng nổ tạo thành <color=orange>%d<color> điểm sát thương thuộc tính băng, đồng thời làm kẻ địch đứng yên trong <color=orange>%.2f<color> giây,\n", nMag, nPois)
			end,
		addskilldamage1={
				[1]={{1,1382},{2,1382}},
				[3]={{1,1},{20,60},{31,70}}
			},
	},
	qizhen_shanhe={ --气震山河
		--meleedamagereturnmana_p={{{1,10},{20,20},{32,25}},{{1,-1},{20,-1},{32,-1}}},
		rangedamagereturnmana_p={{{1,10},{20,20},{32,25}},{{1,-1},{20,-1},{32,-1}}},
	},
	qiguan_changhong={ --气贯长虹
		manatoskill_enhance={{{1,30},{20,100}},{{1,-1},{20,-1}}},
	},
	moyun_jianqi2={ --摩云剑气2
		candetonate1={
			[1]={{1,423*256+0},{2,423*256+0}},
			[3]={{1,360},{26,360},{31,360},{32,360}},
		},
	},
	moyun_jianqi1={ --摩云剑气1
		fastwalkrun_p={{{1,-1},{15,-6},{20,-10},{32,-10},{33,-10}},{{1,18},{2,18}}},
		skill_enhance={
			[1]={{1,-1},{15,-5},{20,-20},{33,-20},{34,-20}},
			[2]={{1,18},{20,18}},
			[3]={},
			},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
		skill_cost_v={{{1,30},{20,400},{26,500},{31,800},{32,800}}},
		skill_desc=
			function(level)

				local nMag = floor(Link(level,SKILLS.longxuan2.colddamage_v[3]))
				local nPois = format("%.2f", (floor(Link(level,SKILLS.longxuan2.frozen_action[2])*100/18 )/100))
				return format("Khí trường bùng nổ tạo thành <color=orange>%d<color> điểm sát thương thuộc tính băng, đồng thời làm kẻ địch đứng yên trong <color=orange>%.2f<color> giây,\n", nMag, nPois)
			end,
		addskilldamage1={
				[1]={{1,1382},{2,1382}},
				[3]={{1,1},{20,60},{31,70}}
			},
	},
	shenguang_xuanrao={ --摩云剑气
		fastwalkrun_p={{{1,-5},{15,-20},{20,-50},{32,-75},{33,-75}},{{1,18},{2,18}}},
		skill_enhance={
			[1]={{1,-5},{15,-25},{20,-85},{33,-135},{34,-135}},
			[2]={{1,18},{20,18}},
			[3]={},
			},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
		skill_cost_v={{{1,30},{20,400},{26,500},{31,800},{32,800}}},
		skill_desc=
			function(level)

				local nMag = floor(Link(level,SKILLS.longxuan2.colddamage_v[3]))
				local nPois = format("%.2f", (floor(Link(level,SKILLS.longxuan2.frozen_action[2])*100/18 )/100))
				return format("Khí trường bùng nổ tạo thành <color=orange>%d<color>điểm sát thương thuộc tính băng, đồng thời làm kẻ địch đứng yên trong <color=orange>%.2f<color> giây.\n", nMag, nPois)
			end,
			addskilldamage1={
				[1]={{1,1382},{2,1382}},
				[3]={{1,1},{20,60},{31,70}}
			},
	},
	qisha_qichang2={ --七杀气场2
		candetonate1={
			[1]={{1,419*256+0},{2,419*256+0}},
			[3]={{1,360},{26,360},{31,360},{32,360}},
		},
		candetonate2={
			[1]={{1,428*256+0},{2,428*256+0}},
			[3]={{1,360},{26,360},{31,360},{32,360}},
		},
	},
	shenguang_xuanrao1={ --神光旋绕
		candetonate1={
			[1]={{1,419*256+0},{2,419*256+0}},
			[3]={{1,100},{26,450},{31,550},{32,560}}
		},
		candetonate2={
			[1]={{1,428*256+0},{2,428*256+0}},
			[3]={{1,100},{26,450},{31,550},{32,560}}
		},
		skill_cost_v={{{1,10},{15,300},{20,1100},{26,1300},{31,1500},{32,1600}}},
		skill_desc=
			function(level)

				local nMag = floor(Link(level,SKILLS.shenguang_xuanrao1.candetonate1[3]))
				return format("Bùng nổ xung quanh <color=orange>%d<color> trong phạn vi phe ta <color=blue> Ma Vân Kiếm Khí <color>, gây ra sát thương cao và phá hiệu quả đóng băng.\n", nMag)
			end,
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
	new_wuji={ --新无极剑气，上面的无极剑气作废
		sorbdamage_yan_p={{{1,10},{15,25},{32,80}},{{1,18},{2,18}}},
		anti_do_hurt_p={{{1,1},{15,5},{20,10},{31,20},{32,21}},{{1,18},{2,18}}},
		lifemax_p={{{1,5},{15,25},{20,80},{31,100},{32,100}},{{1,10},{31,18},{32,18}}},
		lifemax_yan_p={{{1,5},{15,20},{25,60},{31,70},{32,70}},{{1,10},{31,18},{32,18}}},
		lifereplenish_v={{{1,1},{15,4},{25,15},{26,15}},{{1,18},{2,18}}},
		manareplenish_v={{{1,1},{15,4},{25,15},{26,15}},{{1,18},{2,18}}},
		skill_eventskilllevel={{{1,1},{20,20}}},
		skill_showevent={{{1,1},{10,1},{20,1}}},
		skill_cost_v={{{1,30},{20,400},{26,500},{31,800},{32,800}}},
		skill_desc=
			function(level)

				local nMag = floor(Link(level,SKILLS.longxuan2.colddamage_v[3]))
				local nPois = format("%.2f", (floor(Link(level,SKILLS.longxuan2.frozen_action[2])*100/18 )/100))
				return format("Khí trường bùng nổ tạo thành <color=orange>%d<color> điểm sát thương thuộc tính băng, đồng thời làm kẻ địch đứng yên trong <color=orange>%.2f<color> giây,\n", nMag, nPois)
			end,
	},
	longxuan2={ --龙旋2
		seriesdamage_p={{{1,20},{15,20},{20,60},{21,62}}},
		colddamage_v={
			[1]={{1,40},{25,2416},{28,2916}},
			[3]={{1,40},{25,2416},{28,2916}}
		},
		frozen_action=
			{
				[1]={{1,1},{20,1}},
				[2]={{1,0},{15,3},{20,6},{31,9},{32,9}},
				[3]={{1,1},{20,1}}
			},
		
	},
	pishi_poyu={ --劈石破玉 neihuashan150
		seriesdamage_p={{{1,40},{15,40},{20,80},{21,82}}},
		colddamage_v={
			--[1]={{1,200},{40,8000},{41,8000}},
			--[3]={{1,200},{40,8000},{41,8000}},
			[1]={{1,160},{25,12000*1.2},{40,24000*1.5}},  
   			[3]={{1,160},{25,12000*1.2},{40,24000*1.5}},  
		},
		fatallystrike_p={{{1,0},{27,0},{28,1},{36,5},{37,5}}},
		skill_misslenum_v={{{1,1},{2,1},{11,2},{12,2},{20,3},{21,3},{35,4},{36,4}}},
		skill_cost_v={{{1,30},{20,65}}},
		addskillexp1={{{1,0},{2,0}},{{1,1},{20,1}},{{1,0},{2,0}}},
		skill_skillexp_v={{	{1,SkillExpFunc(5000,1.25,1,3,1)},
							{2,SkillExpFunc(5000,1.15,2,3,1)},
							{3,SkillExpFunc(5000,1.16,3,3,1)},
							{4,SkillExpFunc(5000,1.17,4,3,1)},
							{5,SkillExpFunc(5000,1.18,5,3,1)},
							{6,SkillExpFunc(5000,1.19,6,3,1)},
							{7,SkillExpFunc(5000,1.20,7,3,1)},
							{8,SkillExpFunc(5000,1.21,8,3,1)},
							{9,SkillExpFunc(5000,1.22,9,3,1)},
							{10,SkillExpFunc(5000,1.23,10,3,1)},
							{11,SkillExpFunc(5000,1.24,11,3,1)},
							{12,SkillExpFunc(5000,1.23,12,3,1)},
							{13,SkillExpFunc(5000,1.22,13,3,1)},
							{14,SkillExpFunc(5000,1.21,14,3,1)},
							{15,SkillExpFunc(5000,1.20,15,3,1)},
							{16,SkillExpFunc(5000,1.19,16,3,1)},
							{17,SkillExpFunc(5000,1.18,17,3,1)},
							{18,SkillExpFunc(5000,1.17,18,3,1)},
							{19,SkillExpFunc(5000,1.16,19,3,1)},
							{20,SkillExpFunc(5000,1.15,20,3,1)},
							}},
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

