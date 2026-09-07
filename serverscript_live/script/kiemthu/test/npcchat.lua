-- Constants
MAX_DIS = 5
timer_sec_chat = 3
chatIndex = 1
chat = {
    "Trêi n¾ng qu¸... Ai cho t«i c¸i nãn víi!",
    "§i hoµi mái ch©n ghª... Nh­ng vÉn ®Ñp trai!",
    "Cã ai thÊy c¸i dÐp cña t«i kh«ng?",
    "T«i thÒ lµ t«i võa thÊy con gµ biÕt móa!",
    "§i tuÇn mµ nh­ ®i du lÞch vËy !",
    "ña... tui ®ang ®i ®©u vËy ta?",
    "Lµm c¶nh vÖ mµ còng cã lóc thÊy c« ®¬n...",
    "CÈn thËn nha, t«i luyÖn vâ 3 ngµy råi ®?!",
    "S¾p dÆn ®i tuÇn, ai ngê ®i lu«n kh«ng vÒ...",
    "Mçi b­íc ch©n lµ mét ­íc m¬... ®­îc nghØ!",
    "Ng­êi ta ®i cµy, tui ®i... vßng vßng.",
    "Tr«ng vËy th«i chø t«i biÕt 7 lo¹i vâ!",
    "NÕu ai ®ang nghe t«i... cho t«i ly trµ s÷a!",
    "T«i lµ AI... µ nhÇm, lµ NPC thiÖt!",
    "Cø ®i råi sÏ ®Õn... nh­ng ®ªn ®©u th× kh«ng biÕt.",
}
chat1= {
    "(gt)ALPHATEST tõ ngµy 19/6 ®Õn hÕt 24/6/2025",
    "(qd)(qd)(qd)Chµo mõng ®¹i hiÖp tham gia !",
    "H·y nhËn cÊp vµ trang bÞ t¹i ®©y(dd)(dd)",
    "§Ó tr¶i nghiÖm mét s©n ch¬i(am) míi !",
    "T×m l¹i nh÷ng ký øc x­a ! :0",
}
chat2= {
    "(gt)Mçi ngµy lµm tèi ®a 40 nhiÖm vô",
    "(qd)(qd)(qd)NhËn ngay 10 lÖnh bµi boss !",
    "50 triÖu Exp céng ®ån(dd)(dd)",
    "100 Ch©n Nguyªn §¬n Khãa(am)!",
    "500 Hé M¹ch §¬n Khãa ! :0",
}
chat3= {
    "(sl)(bear)(bear)§óng 19h chÝnh thøc Open",
    "(bear)(sl)Phiªn B¶n CTC (sl)",
    "(bear)AE Like Share ®Ó nhËn GiftCode VIP",
    "(bear)Víi NhiÒu Item Gi¸ TrÞ !(sl)(sl)",
    "(sl)(bear)(bear)C¶m ¥n AE ®· Tham Gia ! :0",
}
function NpcChatDaTau(nNpcIndex)

    if chat2[chatIndex] then
        local msg = chat2[chatIndex]
        NpcChat(nNpcIndex, "<color=red>"..msg.." <color>")
        chatIndex = chatIndex + 1  --
    else
        chatIndex = 1  
    end
end
function NpcChatHoTro(nNpcIndex)

    if chat3[chatIndex] then
        local msg = chat3[chatIndex]
        NpcChat(nNpcIndex, "<color=Green>"..msg.."<color>")
        chatIndex = chatIndex + 1  --
    else
        chatIndex = 1  
    end
end
function NpcChatWalkTest(nNpcIndex)

    if chat1[chatIndex] then
        local msg = chat1[chatIndex]
        NpcChat(nNpcIndex, "<color=red>"..msg.."<color>")
        chatIndex = chatIndex + 1  --
    else
        chatIndex = 1  
    end
end

function NpcChatWalk(nNpcIndex)
		if random(1, 10) == 1 then -- 20% chance to chat
			local msg = chat[random(1, 15)]
			NpcChat(nNpcIndex, msg)
		end
end

function OnTimer(nNpcIndex, nTimeOut)
	NpcChatWalk(nNpcIndex)
	NpcChatWalkTest(nNpcIndex)
	SetNpcTimer(nNpcIndex, 18 * timer_sec_chat)
end

function OnRevive(nNpcIndex, nTimeOut)

end
