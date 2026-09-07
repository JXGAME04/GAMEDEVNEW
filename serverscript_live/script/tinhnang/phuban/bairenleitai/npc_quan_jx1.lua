-- [BAIREN 23/08] main() NPC "Quan nhac nho Hoang Thanh Tu" = zhaojingling\enternpc.lua:17-30 (thoai)
-- + config\9 ClickNpc detail 3 -> bairen_dialog. Muc "Hoang Thanh Tu huan luyen cung" (Truy tim
-- Moc Nhan - zhaojingling) KHONG dua vao vi chua port.
Include("\\script\\dailogsys\\g_dialog.lua")
Include("\\script\\missions\\bairenleitai\\npc_enter.lua")

function main()
	local nNpcIndex = GetLastDiagNpc()
	local tbDailog = DailogClass:new(GetNpcName(nNpcIndex))
	tbDailog.szTitleMsg = "<npc>§Ó huÊn luyÖn nh÷ng ®ai néi cao thñ míi , ®Ó n©ng cao chÝ sü giang hå, Hoµng Thµnh T­ ta ®©y chiªu mé Hßa KiÖt kh¾p n¬i. NÕu nh­ ng­¬i cã ý ®Þnh b¸o hiÕu cho triÒu ®×nh, h·y tham gia b¸o danh ho¹t ®éng. Häc thµnh v¨n vâ nghÖ, gióp cho §Õ V­¬ng Gia. C¸c h¹ vâ nghÖ ®Çy m×nh, b¶o kiÕm m«ng trÇn."
	tbDailog:AddOptEntry("L«i §µi Hoµng Thµnh T­", bairen_dialog)
	tbDailog:Show()
end
