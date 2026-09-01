#include "KCore.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KMath.h"
#include "KNpcAttribModify.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#include <KPlayerSet.h>

KNpcAttribModify	g_NpcAttribModify;
KNpcAttribModify::KNpcAttribModify()
{
	ZeroMemory(ProcessFunc, sizeof(ProcessFunc));
	ProcessFunc[magic_physicsresmax_p] = &KNpcAttribModify::PhysicsResMaxP;
	ProcessFunc[magic_coldresmax_p] = &KNpcAttribModify::ColdResMaxP;
	ProcessFunc[magic_fireresmax_p] = &KNpcAttribModify::FireResMaxP;
	ProcessFunc[magic_lightingresmax_p] = &KNpcAttribModify::LightingResMaxP;
	ProcessFunc[magic_poisonresmax_p] = &KNpcAttribModify::PoisonResMaxP;
	ProcessFunc[magic_allresmax_p] = &KNpcAttribModify::AllResMaxP;
	ProcessFunc[magic_lifepotion_v] = &KNpcAttribModify::LifePotionV;
	ProcessFunc[magic_manapotion_v] = &KNpcAttribModify::ManaPotionV;
	ProcessFunc[magic_meleedamagereturn_v] = &KNpcAttribModify::MeleeDamageReturnV;
	ProcessFunc[magic_meleedamagereturn_p] = &KNpcAttribModify::MeleeDamageReturnP;
	ProcessFunc[magic_rangedamagereturn_v] = &KNpcAttribModify::RangeDamageReturnV;
	ProcessFunc[magic_rangedamagereturn_p] = &KNpcAttribModify::RangeDamageReturnP;
	ProcessFunc[magic_damage2addmana_p] = &KNpcAttribModify::Damage2AddManaP;
	ProcessFunc[magic_adddefense_v] = &KNpcAttribModify::ArmorDefenseV;			// Óë×°±¸¼Ó·ÀÓùÍ³Ò»´¦Àí
	ProcessFunc[magic_poisonenhance_p] = &KNpcAttribModify::PoisonEnhanceP;
	ProcessFunc[magic_lightingenhance_p] = &KNpcAttribModify::LightingEnhanceP;
	ProcessFunc[magic_fireenhance_p] = &KNpcAttribModify::FireEnhanceP;
	ProcessFunc[magic_coldenhance_p] = &KNpcAttribModify::ColdEnhanceP; //thêi gian tr× ho·n
	ProcessFunc[magic_armordefense_v] = &KNpcAttribModify::ArmorDefenseV;
	ProcessFunc[magic_lifemax_v] = &KNpcAttribModify::LifeMaxV;
	ProcessFunc[magic_lifemax_yan_v] = &KNpcAttribModify::LifeMaxV;
	ProcessFunc[magic_lifemax_p] = &KNpcAttribModify::LifeMaxP;
	ProcessFunc[magic_lifemax_yan_p] = &KNpcAttribModify::LifeMaxP;
	ProcessFunc[magic_life_v] = &KNpcAttribModify::LifeV;
	ProcessFunc[magic_lifereplenish_v] = &KNpcAttribModify::LifeReplenishV;
	ProcessFunc[magic_manamax_v] = &KNpcAttribModify::ManaMaxV;
	ProcessFunc[magic_manamax_yan_v] = &KNpcAttribModify::ManaMaxV;
	ProcessFunc[magic_manamax_p] = &KNpcAttribModify::ManaMaxP;
	ProcessFunc[magic_manamax_yan_p] = &KNpcAttribModify::ManaMaxP;
	ProcessFunc[magic_mana_v] = &KNpcAttribModify::ManaV;
	ProcessFunc[magic_manareplenish_v] = &KNpcAttribModify::ManaReplenishV;
	ProcessFunc[magic_staminamax_v] = &KNpcAttribModify::StaminaMaxV;
	ProcessFunc[magic_staminamax_p] = &KNpcAttribModify::StaminaMaxP;
	ProcessFunc[magic_stamina_v] = &KNpcAttribModify::StaminaV;
	ProcessFunc[magic_staminareplenish_v] = &KNpcAttribModify::StaminaReplenishV;
	ProcessFunc[magic_strength_v] = &KNpcAttribModify::StrengthV;
	ProcessFunc[magic_dexterity_v] = &KNpcAttribModify::DexterityV;
	ProcessFunc[magic_vitality_v] = &KNpcAttribModify::VitalityV;
	ProcessFunc[magic_energy_v] = &KNpcAttribModify::EnergyV;
	ProcessFunc[magic_poisonres_p] = &KNpcAttribModify::PoisonresP;
	ProcessFunc[magic_poisonres_yan_p] = &KNpcAttribModify::PoisonresP;
	ProcessFunc[magic_fireres_p] = &KNpcAttribModify::FireresP;
	ProcessFunc[magic_fireres_yan_p] = &KNpcAttribModify::FireresP;
	ProcessFunc[magic_lightingres_p] = &KNpcAttribModify::LightingresP;
	ProcessFunc[magic_lightingres_yan_p] = &KNpcAttribModify::LightingresP;
	ProcessFunc[magic_physicsres_p] = &KNpcAttribModify::PhysicsresP;
	ProcessFunc[magic_physicsres_yan_p] = &KNpcAttribModify::PhysicsresP;
	ProcessFunc[magic_coldres_p] = &KNpcAttribModify::ColdresP;
	ProcessFunc[magic_coldres_yan_p] = &KNpcAttribModify::ColdresP;
	ProcessFunc[magic_freezetimereduce_p] = &KNpcAttribModify::FreezeTimeReduceP;
	ProcessFunc[magic_burntimereduce_p] = &KNpcAttribModify::BurnTimeReduceP;
	ProcessFunc[magic_poisontimereduce_p] = &KNpcAttribModify::PoisonTimeReduceP;
	ProcessFunc[magic_poisondamagereduce_v] = &KNpcAttribModify::PoisonDamageReduceV;
	ProcessFunc[magic_stuntimereduce_p] = &KNpcAttribModify::StunTimeReduceP;
	ProcessFunc[magic_fastwalkrun_p] = &KNpcAttribModify::FastWalkRunP;
	ProcessFunc[magic_fastwalkrun_yan_p] = &KNpcAttribModify::FastWalkRunP;
	ProcessFunc[magic_visionradius_p] = &KNpcAttribModify::VisionRadiusP;
	ProcessFunc[magic_fasthitrecover_v] = &KNpcAttribModify::FastHitRecoverV;		//thêi gian phôc håi
	ProcessFunc[magic_fasthitrecover_yan_v] = &KNpcAttribModify::FastHitRecoverV;		//thêi gian phôc håi
	ProcessFunc[magic_allres_p] = &KNpcAttribModify::AllresP;
	ProcessFunc[magic_allres_yan_p] = &KNpcAttribModify::AllresP;
	ProcessFunc[magic_attackrating_v] = &KNpcAttribModify::AttackRatingV;
	ProcessFunc[magic_attackratingenhance_v] = &KNpcAttribModify::AttackRatingV;
	ProcessFunc[magic_attackrating_p] = &KNpcAttribModify::AttackRatingP;
	ProcessFunc[magic_attackratingenhance_p] = &KNpcAttribModify::AttackRatingP;
	ProcessFunc[magic_attackspeed_v] = &KNpcAttribModify::AttackSpeedV;
	ProcessFunc[magic_attackspeed_yan_v] = &KNpcAttribModify::AttackSpeedV;
	ProcessFunc[magic_castspeed_v] = &KNpcAttribModify::CastSpeedV;
	ProcessFunc[magic_castspeed_yan_v] = &KNpcAttribModify::CastSpeedV;
	ProcessFunc[magic_addphysicsdamage_v] = &KNpcAttribModify::AddPhysicsDamageV;
	ProcessFunc[magic_addfiredamage_v] = &KNpcAttribModify::AddFireDamageV;
	ProcessFunc[magic_addcolddamage_v] = &KNpcAttribModify::AddColdDamageV;
	ProcessFunc[magic_addlightingdamage_v] = &KNpcAttribModify::AddLightingDamageV;
	ProcessFunc[magic_addpoisondamage_v] = &KNpcAttribModify::AddPoisonDamageV;
	ProcessFunc[magic_addphysicsdamage_p] = &KNpcAttribModify::AddPhysicsDamageP;	//S¸t th­¬ng vËt lý ngo¹i %
	ProcessFunc[magic_slowmissle_b] = &KNpcAttribModify::SlowMissleB;
	ProcessFunc[magic_changecamp_b] = &KNpcAttribModify::ChangeCampV;
	ProcessFunc[magic_physicsarmor_v] = &KNpcAttribModify::PhysicsArmorV;
	ProcessFunc[magic_coldarmor_v] = &KNpcAttribModify::ColdArmorV;
	ProcessFunc[magic_firearmor_v] = &KNpcAttribModify::FireArmorV;
	ProcessFunc[magic_poisonarmor_v] = &KNpcAttribModify::PoisonArmorV;
	ProcessFunc[magic_lightingarmor_v] = &KNpcAttribModify::LightingArmorV;
	ProcessFunc[magic_lucky_v] = &KNpcAttribModify::LuckyV;
	ProcessFunc[magic_steallife_p] = &KNpcAttribModify::StealLifeP;
	ProcessFunc[magic_steallifeenhance_p] = &KNpcAttribModify::StealLifeP;
	ProcessFunc[magic_stealstamina_p] = &KNpcAttribModify::StealStaminaP;
	ProcessFunc[magic_stealstaminaenhance_p] = &KNpcAttribModify::StealStaminaP;
	ProcessFunc[magic_stealmana_p] = &KNpcAttribModify::StealManaP;
	ProcessFunc[magic_stealmanaenhance_p] = &KNpcAttribModify::StealManaP;
	ProcessFunc[magic_allskill_v] = &KNpcAttribModify::AllSkillV;
	ProcessFunc[magic_metalskill_v] = &KNpcAttribModify::MetalSkillV;
	ProcessFunc[magic_woodskill_v] = &KNpcAttribModify::WoodSkillV;
	ProcessFunc[magic_waterskill_v] = &KNpcAttribModify::WaterSkillV;
	ProcessFunc[magic_fireskill_v] = &KNpcAttribModify::FireSkillV;
	ProcessFunc[magic_earthskill_v] = &KNpcAttribModify::EarthSkillV;
	ProcessFunc[magic_fatallystrikeres_p] = &KNpcAttribModify::FatallyStrikeResP;
	ProcessFunc[magic_deadlystrike_p] = &KNpcAttribModify::DeadlyStrikeP; //tÊn c«ng chÝ m¹ng
	ProcessFunc[magic_deadlystrikeenhance_p] = &KNpcAttribModify::DeadlyStrikeEnhanceP; //t¨ng tÊn c«ng chÝ m¹ng
	ProcessFunc[magic_fatallystrikeenhance_p] = &KNpcAttribModify::FatallyStrikeEnhanceP;
	ProcessFunc[magic_manashield_p] = &KNpcAttribModify::ManaShieldP;
	ProcessFunc[magic_addcoldmagic_v] = &KNpcAttribModify::AddColdMagicV;
	ProcessFunc[magic_addfiremagic_v] = &KNpcAttribModify::AddFireMagicV;
	ProcessFunc[magic_addlightingmagic_v] = &KNpcAttribModify::AddLightingMagicV;
	ProcessFunc[magic_addpoisonmagic_v] = &KNpcAttribModify::AddPoisonMagicV;
	ProcessFunc[magic_addphysicsmagic_v] = &KNpcAttribModify::AddPhysicsMagicV;
	ProcessFunc[magic_dynamicmagicshield_v] = &KNpcAttribModify::DynamicMagicShieldV;
	ProcessFunc[magic_staticmagicshield_p] = &KNpcAttribModify::StaticMagicShieldP;
	ProcessFunc[magic_expenhance_p] = &KNpcAttribModify::ExpEnhanceP;
	ProcessFunc[magic_lifereplenish_p] = &KNpcAttribModify::LifeReplenishP;
	ProcessFunc[magic_ignoreskill_p] = &KNpcAttribModify::IgnoreSkillP;
	ProcessFunc[magic_poisondamagereturn_p] = &KNpcAttribModify::PoisonDamageReturnP;
	ProcessFunc[magic_returnskill_p] = &KNpcAttribModify::ReturnSkillP;
	ProcessFunc[magic_poison2decmana_p] = &KNpcAttribModify::Poison2DecManaP;
	ProcessFunc[magic_autodeathskill] = &KNpcAttribModify::AutoDeathSkill;
	ProcessFunc[magic_autoattackskill] = &KNpcAttribModify::AutoAttackSkill;
	ProcessFunc[magic_hide] = &KNpcAttribModify::Hide;
	ProcessFunc[magic_ignorenegativestate_p] = &KNpcAttribModify::IgnoreNegativeStateP;
	ProcessFunc[magic_autoreplyskill] = &KNpcAttribModify::AutoReplySkill;
	ProcessFunc[magic_autorescueskill] = &KNpcAttribModify::AutoRescueSkill;
	ProcessFunc[magic_returnres_p] = &KNpcAttribModify::ReturnResP;
	ProcessFunc[magic_skill_enhance] = &KNpcAttribModify::SkillEnhanceP;
	// [KM 27/08b] sat thuong theo he (Khi Doanh Dan Dien)
	ProcessFunc[magic_me2metaldamage_p] = &KNpcAttribModify::Me2MetalDamP;
	ProcessFunc[magic_metal2medamage_p] = &KNpcAttribModify::Metal2MeDamP;
	ProcessFunc[magic_me2wooddamage_p] = &KNpcAttribModify::Me2WoodDamP;
	ProcessFunc[magic_wood2medamage_p] = &KNpcAttribModify::Wood2MeDamP;
	ProcessFunc[magic_me2waterdamage_p] = &KNpcAttribModify::Me2WaterDamP;
	ProcessFunc[magic_water2medamage_p] = &KNpcAttribModify::Water2MeDamP;
	ProcessFunc[magic_me2firedamage_p] = &KNpcAttribModify::Me2FireDamP;
	ProcessFunc[magic_fire2medamage_p] = &KNpcAttribModify::Fire2MeDamP;
	ProcessFunc[magic_me2earthdamage_p] = &KNpcAttribModify::Me2EarthDamP;
	ProcessFunc[magic_earth2medamage_p] = &KNpcAttribModify::Earth2MeDamP;
	ProcessFunc[magic_five_elements_enhance_v] = &KNpcAttribModify::FiveElementsEnhanceV;
	ProcessFunc[magic_five_elements_resist_v] = &KNpcAttribModify::FiveElementsResistV;	
	ProcessFunc[magic_frozen_action] = &KNpcAttribModify::FrozenAction;
	ProcessFunc[magic_walkrunshadow] = &KNpcAttribModify::WalkRunShadow;
	ProcessFunc[magic_randmove] = &KNpcAttribModify::RandMove;
	ProcessFunc[magic_manatoskill_enhance] = &KNpcAttribModify::ManaToSkillEnhanceP;
	ProcessFunc[magic_sorbdamage_p] = &KNpcAttribModify::SorbDamageP;
	// [KM 27/08] sau thuoc tinh he kinh mach
	ProcessFunc[magic_block_rate] = &KNpcAttribModify::BlockRate;
	ProcessFunc[magic_anti_block_rate] = &KNpcAttribModify::AntiBlockRate;
	ProcessFunc[magic_enhancehit_rate] = &KNpcAttribModify::EnhanceHitRate;
	ProcessFunc[magic_anti_enhancehit_rate] = &KNpcAttribModify::AntiEnhanceHitRate;
	ProcessFunc[magic_anti_allres_yan_p] = &KNpcAttribModify::AntiAllResP;
	ProcessFunc[magic_anti_sorbdamage_yan_p] = &KNpcAttribModify::AntiSorbDamageP;
	ProcessFunc[magic_anti_allres_p] = &KNpcAttribModify::AntiAllResP;			// [KM 01/09] ban khong-Duong
	ProcessFunc[magic_anti_sorbdamage_p] = &KNpcAttribModify::AntiSorbDamageP;	// [KM 01/09] ban khong-Duong
	ProcessFunc[magic_enhancehiteffect_rate] = &KNpcAttribModify::EnhanceHitEffectRate;
	ProcessFunc[magic_anti_enhancehiteffect_rate] = &KNpcAttribModify::AntiEnhanceHitEffectRate;
	ProcessFunc[magic_add_damage_p] = &KNpcAttribModify::AddDamageP;
	ProcessFunc[magic_anti_hitrecover] = &KNpcAttribModify::AntiHitRecover;
	ProcessFunc[magic_sorbdamage_yan_p] = &KNpcAttribModify::SorbDamageYanP;	// [PF 31/08k]
	ProcessFunc[magic_anti_stuntimereduce_p] = &KNpcAttribModify::AntiStunTimeReduceP;
	ProcessFunc[magic_do_stun_p] = &KNpcAttribModify::DoStunP;			// [CHOANG 01/09] 261
	ProcessFunc[magic_anti_do_stun_p] = &KNpcAttribModify::AntiDoStunP;	// [CHOANG 01/09] 219	// [PF 31/08k]
	ProcessFunc[magic_anti_poisontimereduce_p] = &KNpcAttribModify::AntiPoisonTimeReduceP;	// [PF 31/08k]
	ProcessFunc[magic_do_hurt_p] = &KNpcAttribModify::DoHurtP;	// [PF 31/08k]
	ProcessFunc[magic_anti_do_hurt_p] = &KNpcAttribModify::AntiDoHurtP;	// [PF 31/08k]
	ProcessFunc[magic_manareplenish_p] = &KNpcAttribModify::ManaReplenishPercent;	// [PF 31/08k]
	ProcessFunc[magic_anti_physicsres_yan_p] = &KNpcAttribModify::AntiPhysicsResYanP;	// [PF 31/08k]
	ProcessFunc[magic_anti_fireres_yan_p] = &KNpcAttribModify::AntiFireResYanP;	// [PF 31/08k]
	ProcessFunc[magic_anti_coldres_yan_p] = &KNpcAttribModify::AntiColdResYanP;	// [PF 31/08k]
	ProcessFunc[magic_anti_poisonres_yan_p] = &KNpcAttribModify::AntiPoisonResYanP;	// [PF 31/08k]
	ProcessFunc[magic_anti_lightingres_yan_p] = &KNpcAttribModify::AntiLightingResYanP;	// [PF 31/08k]
	ProcessFunc[magic_expenhance_s] = &KNpcAttribModify::ExpSkillsEnhanceP;// Add magic x2 Skill
	ProcessFunc[magic_expvip] = &KNpcAttribModify::ExpSkillsVIP;// VIP
}

KNpcAttribModify::~KNpcAttribModify()
{
}

void KNpcAttribModify::ExpSkillsVIP(KNpc* pNpc, void* pData)// VIP
{
	KMagicAttrib* pMagic = (KMagicAttrib*)pData;
	if (pNpc->IsPlayer())
	{
		if (pMagic->nValue[0] > 1)
			pNpc->m_CurrentExpSkillsVip = 2; //truong hop su dung x2;

		else
			pNpc->m_CurrentExpSkillsVip = 1; // truong hop khong su dung x2;
	}
}

void KNpcAttribModify::ExpSkillsEnhanceP(KNpc* pNpc, void* pData)// ExpSkills x2
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->IsPlayer())
	{
		if (pMagic->nValue[0] > 1)
			pNpc->m_CurrentExpSkillsEnchance = 2; //truong hop su dung x2;

		else
			pNpc->m_CurrentExpSkillsEnchance = 1; // truong hop khong su dung x2;
	}
}



void KNpcAttribModify::ModifyAttrib(KNpc* pNpc, void* pData)
{
	if (!pData || !pNpc)
		return;

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nAttribType < 0 || pMagic->nAttribType >= magic_normal_end || NULL == ProcessFunc[pMagic->nAttribType])
		return;
	
	(this->*ProcessFunc[pMagic->nAttribType])(pNpc, pData);
}

void KNpcAttribModify::AddColdDamageV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	// [BANGSAT 01/09] LAM CHUAN THEO LINUX (handler addcolddamage_v 0x08098FA0).
	// Ban cu JX1 co HAI loi:
	//  (1) bac thang 16 nac roi GAN DE nValue[1] = bac cua RIENG mon dang xu ly
	//      -> deo mon A (+200, bac 50) roi mon B (+20, bac 8) thi thoi luong con 8,
	//      tuc deo THEM do bang sat lai thay bang NGAN DI.
	//  (2) duong cong lech han Linux o vung thap (v=9: JX1 6 tick vs Linux 10 tick).
	// Linux: time = min(54, (v/10)*4 + 10) tinh tren TUNG dong, roi giu MAX giua cac dong.
	int nTime = 0;
	if (pMagic->nValue[0] > 0)
	{
		nTime = (pMagic->nValue[0] / 10) * 4 + 10;
		if (nTime > 54)
			nTime = 54;		// tran cung cua Linux (0x08098FF3 cmp 0x36)
	}
	pNpc->m_CurrentColdDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentColdDamage.nValue[2] += pMagic->nValue[0];
	if (pNpc->m_CurrentColdDamage.nValue[0] > 0 && pNpc->m_CurrentColdDamage.nValue[2] > 0)
	{
		// GIU MAX (Linux 0x08098FF9 cmp/jge) - khong gan de
		if (nTime > pNpc->m_CurrentColdDamage.nValue[1])
			pNpc->m_CurrentColdDamage.nValue[1] = nTime;
	}
	else
	{
		pNpc->m_CurrentColdDamage.nValue[0] = 0;
		pNpc->m_CurrentColdDamage.nValue[1] = 0;
		pNpc->m_CurrentColdDamage.nValue[2] = 0;
	}
}

void KNpcAttribModify::AddColdMagicV(KNpc* pNpc, void* pData)
{	
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	int COLD_DAMAGE_TIME_Count_1 = 0;
	if (pMagic->nValue[0] <= 0)
	{
	COLD_DAMAGE_TIME_Count_1 = 0;
	}
	else if (pMagic->nValue[0] < 9)
	{
	COLD_DAMAGE_TIME_Count_1 = 4;
	}
	else if (pMagic->nValue[0] < 18)
	{
	COLD_DAMAGE_TIME_Count_1 = 8;
	}
	else if (pMagic->nValue[0] < 27)
	{
	COLD_DAMAGE_TIME_Count_1 = 12;
	}
	else if (pMagic->nValue[0] < 36)
	{
	COLD_DAMAGE_TIME_Count_1 = 16;
	}
	else if (pMagic->nValue[0] < 45)
	{
	COLD_DAMAGE_TIME_Count_1 = 20;
	}
	else if (pMagic->nValue[0] < 54)
	{
	COLD_DAMAGE_TIME_Count_1 = 24;
	}
	else if (pMagic->nValue[0] < 63)
	{
	COLD_DAMAGE_TIME_Count_1 = 28;
	}
	else if (pMagic->nValue[0] < 72)
	{
	COLD_DAMAGE_TIME_Count_1 = 32;
	}
	else if (pMagic->nValue[0] < 81)
	{
	COLD_DAMAGE_TIME_Count_1 = 18;
	}
	else if (pMagic->nValue[0] < 90)
	{
	COLD_DAMAGE_TIME_Count_1 = 40;
	}
	else if (pMagic->nValue[0] < 99)
	{
	COLD_DAMAGE_TIME_Count_1 = 44;
	}
	else if (pMagic->nValue[0] < 108)
	{
	COLD_DAMAGE_TIME_Count_1 = 48;
	}
	else if (pMagic->nValue[0] < 117)
	{
	COLD_DAMAGE_TIME_Count_1 = 52;
	}
	else if (pMagic->nValue[0] < 126)
	{
	COLD_DAMAGE_TIME_Count_1 = 56;
	}
	else if (pMagic->nValue[0] < 135)
	{
	COLD_DAMAGE_TIME_Count_1 = 60;
	}
	else
	{
	COLD_DAMAGE_TIME_Count_1 = 64;
	}
	pNpc->m_CurrentColdMagic.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentColdMagic.nValue[2] += pMagic->nValue[0];
	if (pNpc->m_CurrentColdMagic.nValue[0] > 0 && pNpc->m_CurrentColdMagic.nValue[2] > 0)
		pNpc->m_CurrentColdMagic.nValue[1] = COLD_DAMAGE_TIME_Count_1;
	else
	{
		pNpc->m_CurrentColdMagic.nValue[0] = 0;
		pNpc->m_CurrentColdMagic.nValue[1] = 0;
		pNpc->m_CurrentColdMagic.nValue[2] = 0;
	}
}

void KNpcAttribModify::AddFireDamageV(KNpc* pNpc, void* pData)
{	
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentFireDamage.nValue[2] += pMagic->nValue[0];
}

void KNpcAttribModify::AddFireMagicV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireMagic.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentFireMagic.nValue[2] += pMagic->nValue[0];
}


void KNpcAttribModify::AddLightingDamageV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentLightDamage.nValue[2] += pMagic->nValue[0];
}

void KNpcAttribModify::AddLightingMagicV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightMagic.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentLightMagic.nValue[2] += pMagic->nValue[0];
}

void KNpcAttribModify::AddPhysicsDamageP(KNpc* pNpc, void* pData)
{
#define WEAPON_ALL			((MAX_MELEE_WEAPON) + 0)
#define	WEAPON_RANGE_ALL	((MAX_MELEE_WEAPON) + 1)
#define	WEAPON_MELEE_ALL	((MAX_MELEE_WEAPON) + 2)
#define	WEAPON_NONE			((MAX_MELEE_WEAPON) + 3)

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	int nType = abs(pMagic->nValue[2]);
	//g_DebugLog("[KNpcAttribModify::AddPhysicsDamageP] pMagic->nValue[0]: %d, nValue[1]: %d, nValue[2]: %d", pMagic->nValue[0], pMagic->nValue[1], pMagic->nValue[2]);
	//g_DebugLog("[KNpcAttribModify::AddPhysicsDamageP] nType: %d", nType);
	
	if (nType == WEAPON_ALL)
	{
		for (int i = 0; i < (MAX_MELEE_WEAPON + 1); i++)
		{
			pNpc->m_CurrentMeleeEnhance[i] += pMagic->nValue[0];
		}
		pNpc->m_CurrentHandEnhance += pMagic->nValue[0];
		pNpc->m_CurrentRangeEnhance += pMagic->nValue[0];
	}
	else if (nType == WEAPON_RANGE_ALL)
	{
		pNpc->m_CurrentRangeEnhance += pMagic->nValue[0];
	}
	else if (nType == WEAPON_NONE)
	{
		pNpc->m_CurrentHandEnhance += pMagic->nValue[0];
	}
	else if (nType == WEAPON_MELEE_ALL)
	{
		for (int i = 0; i < (MAX_MELEE_WEAPON + 1); i++)
		{
			pNpc->m_CurrentMeleeEnhance[i] += pMagic->nValue[0];
		}
	}
	else if (nType >= 0 && nType < MAX_MELEE_WEAPON)
	{
		pNpc->m_CurrentMeleeEnhance[nType] += pMagic->nValue[0];
	}
}

void KNpcAttribModify::AddPoisonDamageV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonDamage.nValue[0] += pMagic->nValue[0];

	if (pNpc->m_CurrentPoisonDamage.nValue[0] > 0)
	{
		pNpc->m_CurrentPoisonDamage.nValue[1] = POISON_DAMAGE_TIME;//pMagic->nValue[1];
		pNpc->m_CurrentPoisonDamage.nValue[2] = POISON_DAMAGE_INTERVAL;//pMagic->nValue[2];
	}
	else
	{
		pNpc->m_CurrentPoisonDamage.nValue[0] = 0;
		pNpc->m_CurrentPoisonDamage.nValue[1] = 0;
		pNpc->m_CurrentPoisonDamage.nValue[2] = 0;
	}
}

void KNpcAttribModify::AddPoisonMagicV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonMagic.nValue[0] += pMagic->nValue[0];

	if (pNpc->m_CurrentPoisonMagic.nValue[0] > 0)
	{
		pNpc->m_CurrentPoisonMagic.nValue[1] = POISON_DAMAGE_TIME;//pMagic->nValue[1];
		pNpc->m_CurrentPoisonMagic.nValue[2] = POISON_DAMAGE_INTERVAL;//pMagic->nValue[2];
	}
	else
	{
		pNpc->m_CurrentPoisonMagic.nValue[0] = 0;
		pNpc->m_CurrentPoisonMagic.nValue[1] = 0;
		pNpc->m_CurrentPoisonMagic.nValue[2] = 0;
	}
}

void KNpcAttribModify::AddPhysicsDamageV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAddPhysicsDamage += pMagic->nValue[0];
}

void KNpcAttribModify::AddPhysicsMagicV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAddPhysicsMagic += pMagic->nValue[0];
}


void KNpcAttribModify::AllresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireResist += pMagic->nValue[0];
	pNpc->m_CurrentColdResist += pMagic->nValue[0];
	pNpc->m_CurrentLightResist += pMagic->nValue[0];
	pNpc->m_CurrentPoisonResist += pMagic->nValue[0];
	pNpc->m_CurrentPhysicsResist += pMagic->nValue[0];
}

void KNpcAttribModify::AllSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	ISkill * pSkill = NULL;
	int nSkillId = abs(pMagic->nValue[2]);
	int nAddLevel = pMagic->nValue[0];
	pNpc->m_SkillList.AllSkillV(nSkillId, nAddLevel);
}

void KNpcAttribModify::AttackRatingP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAttackRating += pNpc->m_AttackRating * pMagic->nValue[0] / 100;
	pNpc->m_CurrentAttackRating -= Player[pNpc->m_nPlayerIdx].m_nMeridianDexterity * PlayerSet.m_cLevelAdd.GetLifePerVitality(pNpc->m_Series);
	pNpc->m_CurrentAttackRating += Player[pNpc->m_nPlayerIdx].m_nMeridianDexterity * PlayerSet.m_cLevelAdd.GetLifePerVitality(pNpc->m_Series) * (1.0f + (pMagic->nValue[0] / 2) / 100.0f);
}

void KNpcAttribModify::AttackRatingV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAttackRating += pMagic->nValue[0];
}

void KNpcAttribModify::AttackSpeedV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAttackSpeed += pMagic->nValue[0];
}

void KNpcAttribModify::BadStatusTimeReduceV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	return;
}

void KNpcAttribModify::BurnTimeReduceP(KNpc* pNpc, void* pData)
{
	return;
}

void KNpcAttribModify::CastSpeedV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentCastSpeed += pMagic->nValue[0];
}

void KNpcAttribModify::ChangeCampV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
	{
		if (pMagic->nValue[0] > 0 && pMagic->nValue[0] < camp_num)
			pNpc->SetCurrentCamp(pMagic->nValue[0]);
		else
			pNpc->SetCurrentCamp(pNpc->m_Camp);
	}
	
}

void KNpcAttribModify::ColdArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_ColdArmor.nValue[0] += pMagic->nValue[0];
}

void KNpcAttribModify::ColdresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdResist += pMagic->nValue[0];
}

void KNpcAttribModify::DeadlyStrikeEnhanceP(KNpc* pNpc, void* pData) //t¨ng tÊn c«ng chÝ m¹ng
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDeadlyStrikeEnhanceP += pMagic->nValue[0];
}

void KNpcAttribModify::DeadlyStrikeP(KNpc* pNpc, void* pData) //tÊn c«ng chÝ m¹ng
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDeadlyStrikeEnhanceP += pMagic->nValue[0];
}

void KNpcAttribModify::DexterityV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	if (pMagic->nValue[0] == pMagic->nValue[1] && pMagic->nValue[2] == 99887786) { //Merdian
		Player[pNpc->m_nPlayerIdx].m_nMeridianDexterity += pMagic->nValue[0];
	}
	Player[pNpc->m_nPlayerIdx].ChangeCurDexterity(pMagic->nValue[0]);
}

void KNpcAttribModify::EarthSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_SkillList.SeriesSkillV(series_earth, pMagic->nValue[0]);
}

void KNpcAttribModify::FatallyStrikeResP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFatallyStrikeResP += pMagic->nValue[0];	
}

void KNpcAttribModify::EnergyV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	if (pMagic->nValue[0] == pMagic->nValue[1] && pMagic->nValue[2] == 99887786) { //Merdian
		Player[pNpc->m_nPlayerIdx].m_nMeridianEngergy += pMagic->nValue[0];
	}
	Player[pNpc->m_nPlayerIdx].ChangeCurEngergy(pMagic->nValue[0]);
}

void KNpcAttribModify::FastHitRecoverV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentHitRecover += pMagic->nValue[0]; //thêi gian phôc håi
}

void KNpcAttribModify::FastWalkRunP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentWalkSpeed += ((float)(pNpc->m_WalkSpeed * pMagic->nValue[0]) / 100);
	pNpc->m_CurrentRunSpeed += ((float)(pNpc->m_RunSpeed * pMagic->nValue[0]) / 100);
}

void KNpcAttribModify::FireArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_FireArmor.nValue[0] += pMagic->nValue[0];
}

void KNpcAttribModify::FireSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_SkillList.SeriesSkillV(series_fire, pMagic->nValue[0]);
}

void KNpcAttribModify::FreezeTimeReduceP(KNpc* pNpc, void* pData)	//Thêi gian lµm chËm gi¶m bít
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFreezeTimeReducePercent += pMagic->nValue[0];
}

void KNpcAttribModify::LifeMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeMax += pNpc->m_LifeMax * pMagic->nValue[0] / 100;
	pNpc->m_CurrentLifeMax -= Player[pNpc->m_nPlayerIdx].m_nMeridianVitality * PlayerSet.m_cLevelAdd.GetLifePerVitality(pNpc->m_Series);
	pNpc->m_CurrentLifeMax += Player[pNpc->m_nPlayerIdx].m_nMeridianVitality * PlayerSet.m_cLevelAdd.GetLifePerVitality(pNpc->m_Series) * (1.0f + pMagic->nValue[0] / 100.0f);
}

void KNpcAttribModify::LifeMaxV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeMax += pMagic->nValue[0];
}

void KNpcAttribModify::LifeReplenishV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeReplenish += pMagic->nValue[0];
}

void KNpcAttribModify::LifeV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLife += pMagic->nValue[0];
}

void KNpcAttribModify::LightingArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_LightArmor.nValue[0] += pMagic->nValue[0];
}

void KNpcAttribModify::LightingresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightResist += pMagic->nValue[0];
}

void KNpcAttribModify::LuckyV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->IsPlayer())
	{
		Player[pNpc->m_nPlayerIdx].m_nCurLucky += pMagic->nValue[0];
	}
}

void KNpcAttribModify::ManaMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaMax += pNpc->m_ManaMax * pMagic->nValue[0] / 100;
	pNpc->m_CurrentManaMax -= Player[pNpc->m_nPlayerIdx].m_nMeridianEngergy * PlayerSet.m_cLevelAdd.GetManaPerEnergy(pNpc->m_Series);
	pNpc->m_CurrentManaMax += Player[pNpc->m_nPlayerIdx].m_nMeridianEngergy * PlayerSet.m_cLevelAdd.GetManaPerEnergy(pNpc->m_Series) * (1.0f+pMagic->nValue[0] / 100.0f);
}

void KNpcAttribModify::ManaMaxV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaMax += pMagic->nValue[0];
}

void KNpcAttribModify::ManaReplenishV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaReplenish += pMagic->nValue[0];
}

void KNpcAttribModify::ManaV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMana += pMagic->nValue[0];
}

void KNpcAttribModify::ManaShieldP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	//pNpc->m_ManaShield.nValue[0] += pMagic->nValue[0];

	if (pMagic->nValue[1] > 0)
	{
		pNpc->m_ManaShield.nValue[0] += pMagic->nValue[0];
	}
	else
	{
		pNpc->m_ManaShield.nValue[0] += pMagic->nValue[0];
	}
}

void KNpcAttribModify::MeleeDamageReturnP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMeleeDmgRetPercent += pMagic->nValue[0];
}

void KNpcAttribModify::MeleeDamageReturnV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMeleeDmgRet += pMagic->nValue[0];
}

void KNpcAttribModify::MetalSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_SkillList.SeriesSkillV(series_metal, pMagic->nValue[0]);
}

void KNpcAttribModify::PhysicsArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_PhysicsArmor.nValue[0] += pMagic->nValue[0];
}

void KNpcAttribModify::PhysicsresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPhysicsResist += pMagic->nValue[0];
}

void KNpcAttribModify::Damage2AddManaP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDamage2Mana += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_PoisonArmor.nValue[0] += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonDamageReduceV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_PoisonState.nValue[0] -= pMagic->nValue[0];
	if (pNpc->m_PoisonState.nValue[0] <= 0)
	{
		pNpc->m_PoisonState.nTime = 0;
	}
}

void KNpcAttribModify::PoisonresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonResist += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonTimeReduceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonTimeReducePercent += pMagic->nValue[0];
}

void KNpcAttribModify::RangeDamageReturnV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentRangeDmgRet += pMagic->nValue[0];
}

void KNpcAttribModify::RangeDamageReturnP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentRangeDmgRetPercent += pMagic->nValue[0];
}

void KNpcAttribModify::SlowMissleB(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_CurrentSlowMissle += pMagic->nValue[0];

	if (pNpc->m_CurrentSlowMissle > 0)
		pNpc->m_CurrentSlowMissle = 1;
	else
		pNpc->m_CurrentSlowMissle = 0;
}

void KNpcAttribModify::StaminaMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaMax += pNpc->m_StaminaMax * pMagic->nValue[0] / 100;
	pNpc->m_CurrentStaminaMax -= Player[pNpc->m_nPlayerIdx].m_nMeridianVitality * PlayerSet.m_cLevelAdd.GetLifePerVitality(pNpc->m_Series);
	pNpc->m_CurrentStaminaMax += Player[pNpc->m_nPlayerIdx].m_nMeridianVitality * PlayerSet.m_cLevelAdd.GetLifePerVitality(pNpc->m_Series) * (1.0f + pMagic->nValue[0] / 100.0f);
}

void KNpcAttribModify::StaminaMaxV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaMax += pMagic->nValue[0];
}

void KNpcAttribModify::StaminaReplenishV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaGain += pMagic->nValue[0];
}

void KNpcAttribModify::StaminaV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStamina += pMagic->nValue[0];
}


void KNpcAttribModify::StealLifeP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeStolen += pMagic->nValue[0];
}

void KNpcAttribModify::StealManaP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaStolen += pMagic->nValue[0];
}

void KNpcAttribModify::StealStaminaP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaStolen += pMagic->nValue[0];
}

void KNpcAttribModify::StrengthV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	if (pMagic->nValue[0] == pMagic->nValue[1] && pMagic->nValue[2] == 99887786) { //Merdian
		Player[pNpc->m_nPlayerIdx].m_nMeridianStrength += pMagic->nValue[0];
	}
	Player[pNpc->m_nPlayerIdx].ChangeCurStrength(pMagic->nValue[0]);
}

void KNpcAttribModify::StunTimeReduceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStunTimeReducePercent += pMagic->nValue[0];
}

void KNpcAttribModify::VisionRadiusP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentVisionRadius += pMagic->nValue[0];
}

void KNpcAttribModify::VitalityV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	if (pMagic->nValue[0] == pMagic->nValue[1] && pMagic->nValue[2] == 99887786) { //Merdian
		Player[pNpc->m_nPlayerIdx].m_nMeridianVitality += pMagic->nValue[0];
	}
	Player[pNpc->m_nPlayerIdx].ChangeCurVitality(pMagic->nValue[0]);
}

void KNpcAttribModify::WaterSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_SkillList.SeriesSkillV(series_water, pMagic->nValue[0]);
}

void KNpcAttribModify::WoodSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_SkillList.SeriesSkillV(series_wood, pMagic->nValue[0]);	
}

void KNpcAttribModify::FireresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireResist += pMagic->nValue[0];
}

void KNpcAttribModify::ArmorDefenseV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDefend += pMagic->nValue[0];
}

void KNpcAttribModify::ColdEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdEnhance += pMagic->nValue[0]; //thêi gian tr× ho·n
}

void KNpcAttribModify::FireEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireEnhance += pMagic->nValue[0];
}

void KNpcAttribModify::LightingEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightEnhance += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonEnhance += pMagic->nValue[0];
}

void KNpcAttribModify::LifePotionV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[1] <= 0)
		return;
	int nX1, nY1, nX2, nY2;
	nX1 = pNpc->m_LifeState.nValue[0];
	nY1 = pNpc->m_LifeState.nTime;
	nX2 = pMagic->nValue[0];
	nY2 = pMagic->nValue[1];
	pNpc->m_LifeState.nTime = max(nY1, nY2);
	pNpc->m_LifeState.nValue[0] = (nX1 * nY1 + nX2 * nY2) / pNpc->m_LifeState.nTime;
}

void KNpcAttribModify::ManaPotionV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[1] <= 0)
		return;
	int nX1, nY1, nX2, nY2;
	nX1 = pNpc->m_ManaState.nValue[0];
	nY1 = pNpc->m_ManaState.nTime;
	nX2 = pMagic->nValue[0];
	nY2 = pMagic->nValue[1];
	pNpc->m_ManaState.nTime = max(nY1, nY2);
	pNpc->m_ManaState.nValue[0] = (nX1 * nY1 + nX2 * nY2) / pNpc->m_ManaState.nTime;
}

void KNpcAttribModify::PhysicsResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPhysicsResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::ColdResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::FireResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::LightingResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::AllResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireResistMax += pMagic->nValue[0];
	pNpc->m_CurrentColdResistMax += pMagic->nValue[0];
	pNpc->m_CurrentLightResistMax += pMagic->nValue[0];
	pNpc->m_CurrentPoisonResistMax += pMagic->nValue[0];
	pNpc->m_CurrentPhysicsResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::FatallyStrikeEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFatallyStrikeEnhanceP += pMagic->nValue[0];	
}

void KNpcAttribModify::MixPoisonDamage(KMagicAttrib* pDes, KMagicAttrib* pSrc)
{
	int d1, d2, t1, t2, c1, c2;
	d1 = pDes->nValue[0];
	d2 = pSrc->nValue[0];
	t1 = pDes->nValue[1];
	t2 = pSrc->nValue[1];
	c1 = pDes->nValue[2];
	c2 = pSrc->nValue[2];
	if (c1 == 0 || d1 == 0)
	{
		memcpy(pDes, pSrc, sizeof(KMagicAttrib));
		return;
	}
	if (c2 == 0 || d2 == 0)
	{
		return;
	}
	pDes->nValue[0] = ((c1 + c2) * d1 / c1 + (c1 + c2) * d2 / c2) / 2;
	pDes->nValue[1] = (t1 * d1 * c2 + t2 *d2 * c1) /(d1 * c2 + d2 * c1);
	pDes->nValue[2] = (c1 + c2) / 2;
}

void KNpcAttribModify::DynamicMagicShieldV( KNpc* pNpc, void* pData )//#giam thieu sat thuong ganh chiu HTVC con lon
{
	KMagicAttrib* pMagic = (KMagicAttrib*)pData;
	pNpc->m_CurrentManaShield += pMagic->nValue[0];
}


void KNpcAttribModify::StaticMagicShieldP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaticMagicShieldP += pMagic->nValue[0];
}

void KNpcAttribModify::ExpEnhanceP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->IsPlayer())
	{
		pNpc->m_CurrentExpEnhance += pMagic->nValue[0];
	}
}

void KNpcAttribModify::LifeReplenishP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeReplenishPercent += pMagic->nValue[0];
}

void KNpcAttribModify::IgnoreSkillP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentIgnoreSkillPercent += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonDamageReturnP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonDamageReturnPercent += pMagic->nValue[0];
}

void KNpcAttribModify::ReturnSkillP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentReturnSkillPercent += pMagic->nValue[0];
}

void KNpcAttribModify::Poison2DecManaP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	
	if (pMagic->nValue[1] <= 0)
		return;
	pNpc->m_LoseMana.nTime = pMagic->nValue[1];
	pNpc->m_LoseMana.nValue[0] = pNpc->m_CurrentMana * pMagic->nValue[0] / MAX_PERCENT / (pNpc->m_LoseMana.nTime / GAME_FPS);
}

void KNpcAttribModify::AutoDeathSkill( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0])
	{
		for (int i = 0; i < MAX_AUTOSKILL; i ++)
		{
			if (pNpc->m_DeathSkill[i].nSkillId <= 0)
			{
				pNpc->m_DeathSkill[i].nSkillId = pMagic->nValue[0] / 256;
				pNpc->m_DeathSkill[i].nSkillLevel = pMagic->nValue[0] % 256;
				pNpc->m_DeathSkill[i].dwNextCastTime = SubWorld[pNpc->m_SubWorldIndex].m_dwCurrentTime;
				pNpc->m_DeathSkill[i].nWaitCastTime = pMagic->nValue[2] / 256;
				pNpc->m_DeathSkill[i].nRate = pMagic->nValue[2] % 256;
				break;
			}
		}
	}
}

void KNpcAttribModify::AutoAttackSkill( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0])
	{
		for (int i = 0; i < MAX_AUTOSKILL; i ++)
		{
			if (pNpc->m_AttackSkill[i].nSkillId <= 0)
			{
				pNpc->m_AttackSkill[i].nSkillId = pMagic->nValue[0] / 256;
				pNpc->m_AttackSkill[i].nSkillLevel = pMagic->nValue[0] % 256;
				pNpc->m_AttackSkill[i].dwNextCastTime = SubWorld[pNpc->m_SubWorldIndex].m_dwCurrentTime;
				pNpc->m_AttackSkill[i].nWaitCastTime = pMagic->nValue[2] / 256;
				pNpc->m_AttackSkill[i].nRate = pMagic->nValue[2] % 256;
				break;
			}
		}
	}
}

void KNpcAttribModify::Hide( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if(pMagic->nValue[1] > 0)
		pNpc->m_HideState.nTime = pMagic->nValue[1];
}

void KNpcAttribModify::IgnoreNegativeStateP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentIgnoreNegativeStateP += pMagic->nValue[0];
}

void KNpcAttribModify::AutoReplySkill( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0])
	{
		for (int i = 0; i < MAX_AUTOSKILL; i ++)
		{
			if (pNpc->m_ReplySkill[i].nSkillId <= 0)
			{
				pNpc->m_ReplySkill[i].nSkillId = pMagic->nValue[0] / 256;
				pNpc->m_ReplySkill[i].nSkillLevel = pMagic->nValue[0] % 256;
				pNpc->m_ReplySkill[i].dwNextCastTime = SubWorld[pNpc->m_SubWorldIndex].m_dwCurrentTime;
				pNpc->m_ReplySkill[i].nWaitCastTime = pMagic->nValue[2] / 256;
				pNpc->m_ReplySkill[i].nRate = pMagic->nValue[2] % 256;
				break;
			}
		}
	}
}

void KNpcAttribModify::AutoRescueSkill( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0])
	{
		for (int i = 0; i < MAX_AUTOSKILL; i ++)
		{
			if (pNpc->m_RescueSkill[i].nSkillId <= 0)
			{
				pNpc->m_RescueSkill[i].nSkillId = pMagic->nValue[0] / 256;
				pNpc->m_RescueSkill[i].nSkillLevel = pMagic->nValue[0] % 256;
				pNpc->m_RescueSkill[i].dwNextCastTime = SubWorld[pNpc->m_SubWorldIndex].m_dwCurrentTime;
				pNpc->m_RescueSkill[i].nWaitCastTime = pMagic->nValue[2] / 256;
				pNpc->m_RescueSkill[i].nRate = pMagic->nValue[2] % 256;
				break;
			}
		}
	}
}

void KNpcAttribModify::ReturnResP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentReturnResPercent += pMagic->nValue[0];
}

void KNpcAttribModify::SkillEnhanceP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentSkillEnhancePercent += pMagic->nValue[0];
}

// [KM 27/08b] cong don vao mang theo he; tru = truyen gia tri am (RemoveMaridian...)
void KNpcAttribModify::Me2MetalDamP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_nMe2SeriesDamP[0] += pMagic->nValue[0];
}

void KNpcAttribModify::Metal2MeDamP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_nSeries2MeDamP[0] += pMagic->nValue[0];
}

void KNpcAttribModify::Me2WoodDamP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_nMe2SeriesDamP[1] += pMagic->nValue[0];
}

void KNpcAttribModify::Wood2MeDamP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_nSeries2MeDamP[1] += pMagic->nValue[0];
}

void KNpcAttribModify::Me2WaterDamP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_nMe2SeriesDamP[2] += pMagic->nValue[0];
}

void KNpcAttribModify::Water2MeDamP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_nSeries2MeDamP[2] += pMagic->nValue[0];
}

void KNpcAttribModify::Me2FireDamP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_nMe2SeriesDamP[3] += pMagic->nValue[0];
}

void KNpcAttribModify::Fire2MeDamP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_nSeries2MeDamP[3] += pMagic->nValue[0];
}

void KNpcAttribModify::Me2EarthDamP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_nMe2SeriesDamP[4] += pMagic->nValue[0];
}

void KNpcAttribModify::Earth2MeDamP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_nSeries2MeDamP[4] += pMagic->nValue[0];
}

void KNpcAttribModify::FiveElementsEnhanceV( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFiveElementsEnhance += pMagic->nValue[0];
}

void KNpcAttribModify::FiveElementsResistV( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFiveElementsResist += pMagic->nValue[0];
}

void KNpcAttribModify::FrozenAction( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if(pMagic->nValue[1] > 0)
		pNpc->m_FrozenAction.nTime = pMagic->nValue[1];
	else
		pNpc->m_FrozenAction.nTime = 0;
}

void KNpcAttribModify::WalkRunShadow( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if(pMagic->nValue[1] > 0)
		pNpc->m_WalkRun.nTime += pMagic->nValue[1];
	else
		pNpc->m_WalkRun.nTime = 0;
}

void KNpcAttribModify::RandMove( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if(pMagic->nValue[1] > 0)
		pNpc->m_RandMove.nTime += pMagic->nValue[1];
	else
		pNpc->m_RandMove.nTime = 0;
}

void KNpcAttribModify::ManaToSkillEnhanceP( KNpc* pNpc, void* pData )//#khi noi cong day tang ky nang cong kich
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaToSkillEnhanceP += pMagic->nValue[0];
}

void KNpcAttribModify::SorbDamageP( KNpc* pNpc, void* pData ) //#triet tieu sat thuong
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentSorbDamageP += pMagic->nValue[0];
}

// [KM 27/08] sau thuoc tinh he kinh mach - dung chuan ban Linux + client VLTK
void KNpcAttribModify::BlockRate( KNpc* pNpc, void* pData )			//#hoa giai sat thuong
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentBlockRate += pMagic->nValue[0];
}

void KNpcAttribModify::AntiBlockRate( KNpc* pNpc, void* pData )		//#triet tieu hoa giai
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiBlockRate += pMagic->nValue[0];
}

void KNpcAttribModify::EnhanceHitRate( KNpc* pNpc, void* pData )		//#trong kich
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentEnhanceHitRate += pMagic->nValue[0];
}

void KNpcAttribModify::AntiEnhanceHitRate( KNpc* pNpc, void* pData )	//#khang trong kich
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiEnhanceHitRate += pMagic->nValue[0];
}

void KNpcAttribModify::AntiAllResP( KNpc* pNpc, void* pData )		//#bo qua toan khang
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiAllResP += pMagic->nValue[0];
}

void KNpcAttribModify::AntiSorbDamageP( KNpc* pNpc, void* pData )	//#xuyen giam thuong
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiSorbDamageP += pMagic->nValue[0];
}

void KNpcAttribModify::EnhanceHitEffectRate( KNpc* pNpc, void* pData )	//#hieu qua trong kich
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentEnhanceHitEffect += pMagic->nValue[0];
}

void KNpcAttribModify::AntiEnhanceHitEffectRate( KNpc* pNpc, void* pData )	//#khang hieu qua trong kich
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiEnhanceHitEffect += pMagic->nValue[0];
}

// Linux 0x08096130: pNpc[0x1414] += nValue[0], goc dat 0x64 (=100) o 3 cho.
void KNpcAttribModify::AddDamageP( KNpc* pNpc, void* pData )	//#cong % sat thuong
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAddDamageP += pMagic->nValue[0];
}

void KNpcAttribModify::SorbDamageYanP( KNpc* pNpc, void* pData )	// [PF 31/08k] triet tieu sat thuong ban Duong, PHAN NGHIN, tran 500 (237)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentSorbDamageYanP += pMagic->nValue[0];
	// Linux 0x08095EB0: kep tran 500 (=50%), san 0
	if (pNpc->m_CurrentSorbDamageYanP > 500)
		pNpc->m_CurrentSorbDamageYanP = 500;
	if (pNpc->m_CurrentSorbDamageYanP < 0)
		pNpc->m_CurrentSorbDamageYanP = 0;
}

void KNpcAttribModify::AntiStunTimeReduceP( KNpc* pNpc, void* pData )	// [PF 31/08k] keo dai thoi gian choang TA GAY RA (220)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiStunTimeReduceP += pMagic->nValue[0];
}

// [CHOANG 01/09] do_stun_p (261) - Linux handler 0x080968F0 'DoStunP:%d'
void KNpcAttribModify::DoStunP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDoStunP += pMagic->nValue[0];
}

// [CHOANG 01/09] anti_do_stun_p (219) - Linux handler 0x080968B0 'AntiDoStunP:%d'
void KNpcAttribModify::AntiDoStunP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiDoStunP += pMagic->nValue[0];
}

void KNpcAttribModify::AntiPoisonTimeReduceP( KNpc* pNpc, void* pData )	// [PF 31/08k] keo dai thoi gian doc TA GAY RA (204)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiPoisonTimeReduceP += pMagic->nValue[0];
}

void KNpcAttribModify::DoHurtP( KNpc* pNpc, void* pData )	// [PF 31/08k] cong xac suat gay dong tac bi thuong (205)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDoHurtP += pMagic->nValue[0];
}

void KNpcAttribModify::AntiDoHurtP( KNpc* pNpc, void* pData )	// [PF 31/08k] khang dong tac bi thuong (223)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiDoHurtP += pMagic->nValue[0];
}

void KNpcAttribModify::ManaReplenishPercent( KNpc* pNpc, void* pData )	// [PF 31/08k] hoi noi luc % (254, muon o reserve6)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaReplenishPercent += pMagic->nValue[0];
}

void KNpcAttribModify::AntiPhysicsResYanP( KNpc* pNpc, void* pData )	// [PF 31/08k] bo qua pho phong doi phuong (244)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiPhysicsResYanP += pMagic->nValue[0];
}

void KNpcAttribModify::AntiFireResYanP( KNpc* pNpc, void* pData )	// [PF 31/08k] bo qua hoa phong (217)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiFireResYanP += pMagic->nValue[0];
}

void KNpcAttribModify::AntiColdResYanP( KNpc* pNpc, void* pData )	// [PF 31/08k] bo qua bang phong (222)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiColdResYanP += pMagic->nValue[0];
}

void KNpcAttribModify::AntiPoisonResYanP( KNpc* pNpc, void* pData )	// [PF 31/08k] bo qua doc phong (221)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiPoisonResYanP += pMagic->nValue[0];
}

void KNpcAttribModify::AntiLightingResYanP( KNpc* pNpc, void* pData )	// [PF 31/08k] bo qua loi phong (248)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiLightingResYanP += pMagic->nValue[0];
}


// Linux 0x08096CD0: pNpc[0x12b0] += nValue[0] roi soi sang [0x12b4] (ban dang dung).
void KNpcAttribModify::AntiHitRecover( KNpc* pNpc, void* pData )	//#keo dai dong tac bi thuong gay ra
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAntiHitRecover += pMagic->nValue[0];
}
