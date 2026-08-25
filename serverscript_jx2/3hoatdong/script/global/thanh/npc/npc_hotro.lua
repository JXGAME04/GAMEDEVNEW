IncludeLib("SETTING")
IncludeLib("ITEM");
IncludeLib("FILESYS")
Include("\\script\\lib\\player.lua")
Include("\\script\\dailogsys\\dailogsay.lua")
Include("\\script\\activitysys\\functionlib.lua")
Include("\\script\\dailogsys\\g_dialog.lua")
Include("\\script\\vng_event\\tanthu\\giveskill60.lua")
------------------------------------------------------------------------------------------------------------

function main()
	dofile("script/global/thanh/npc/npc_hotro.lua")
	--dialog_hotro_main()
	support() 
end


function support() -- Include("\\script\\vng_event\\tanthu\\giveskill60.lua")
	local tbOpt = {}
	--tinsert(tbOpt, {"NhËn VËt ThÇn Hµnh Phï, Thæ §Þa Phï 7 ngµy", tbGiveSkill60.GetItem, {tbGiveSkill60}})
	--tinsert(tbOpt, {"NhËn KÜ N¨ng sau Khi Gia NhËp M«n Ph¸i", tbGiveSkill60.GetSkill, {tbGiveSkill60}})
	tinsert(tbOpt, {"NhËn Skill Hç Trî T©n Thñ", tbGiveSkill60.GetSkillSupport, {tbGiveSkill60}})
	--tinsert(tbOpt, {"NhËn trang bÞ Kim Phong", tbGiveSkill60.GetKimPhong, {tbGiveSkill60}})
	--tinsert(tbOpt, {"NhËn BÝ kÝp 90 tù chän theo ph¸i", tbGiveSkill60.GetBK90, {tbGiveSkill60}})
	--tinsert(tbOpt, {"NhËn KÜ N¨ng CÊp 90", tbGiveSkill60.GetSkill90, {tbGiveSkill60}})
	--tinsert(tbOpt, {"TÈy Tñy",clear_attibute_point})
	--tinsert(tbOpt, {"Céng ®iÓm nhanh",add_prop})
	tinsert(tbOpt, {"KÕt thóc ®èi tho¹i."})
	CreateNewSayEx("<npc>Chµo mõng quý nh©n sÜ ®Õn víi <color=yellow>Vâ L©m TruyÒn Kú<color>", tbOpt)
end

function dialog_hotro_main()
	local szTitle = "Xin chµo ! §¹i hiÖp muèn gióp ®ì g×?"
	local tbOpt=
	{
	{"KÕt thóc ®èi tho¹i."},
	}
	CreateNewSayEx("<npc>"..szTitle.."", tbOpt)
	--Say(szTitle, getn(tbOpt), tbOpt)
end



