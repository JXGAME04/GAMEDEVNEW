-- ============================================================================
-- BD_LENHBAI_INFO.LUA - SINH boi ReverseTools/port_3hd/thicong/c51_va_thuong_satthu.py
-- Bam chuot phai vao lenh bai Bac Dau -> chi dan nop o dau. KHONG tru item.
--
-- Vi sao can: o CA ban Linux, 14 ma lenh bai Bac Dau deu de cot Script = 0 nen
-- bam chuot phai khong lam gi - nguoi choi tuong item hong. Script nay chi HIEN
-- THONG TIN, khong doi co che (van phai nop cho NPC Bac Dau lao nhan).
-- KHONG dua duong dan nay vao sIsJx2ItemScript de engine khong tru item.
-- ============================================================================
Include("\\script\\tinhnang\\3hoatdong\\beidou\\bd_activity.lua")

function main(nItemIdx)
	local nCo = 0
	if (BD_DemLenhBai ~= nil) then
		nCo = BD_DemLenhBai()
	end
	local nRieng = 0
	if (tbBeidou ~= nil and tbBeidou.LENHBAI_HD ~= nil) then
		for k, v in tbBeidou.LENHBAI_HD do
			nRieng = nRieng + CalcItemCount(3, 6, 1, v[1], -1)
		end
	end
	local n15 = 15
	local n20 = 20
	if (tbBeidou ~= nil) then
		n15 = tbBeidou.TOKEN_NUM_AWARD10
		n20 = tbBeidou.TOKEN_NUM_AWARD13
	end
	Say("LÖnh bµi B¾c §Èu kh«ng dïng trùc tiÕp. H·y mang tíi <color=yellow>B¾c §Èu l·o nh©n<color> ë Thµnh §«, Ph­îng T­êng, BiÖn Kinh, L©m An, §¹i Lý, T­¬ng D­¬ng, D­¬ng Ch©u (8 giê ®Õn 22 giê).", 0)
	Say("§ang gi÷ <color=yellow>"..nCo.."<color> LÖnh bµi B¾c §Èu vµ <color=yellow>"..nRieng.."<color> lÖnh bµi cña ho¹t ®éng. §ñ <color=yellow>"..n15.."<color> hoÆc <color=yellow>"..n20.."<color> th× ®æi ®­îc phÇn th­ëng lín.", 0)
end
