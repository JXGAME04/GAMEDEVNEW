-- bangthanh_f.lua - BO TEST DAY DU: bang hoi + cong thanh + lanh dia + tac
-- phuong + danh hieu + thue. Goi tu LENH BAI ADMIN (muc "Test bang hoi va
-- cong thanh"). ASCII THUAN - khong duoc go tieng Viet co dau vao day.
--
-- 10 LUAT VIET (vi pham la hong AM THAM - da do bang ma nguon):
--  1. "#Ham(thamso)" toi da 31 KY TU (m_szTaskAnswerFun[50][32], KPlayer.h).
--  2. Cau hoi + TAT CA nhan + dau '|' phan cach <= 511 byte
--     (MAX_SCIRPTACTION_BUFFERNUM = 512, byte cuoi la NUL).
--  3. Phan MO TA chi ve 6 DONG, moi dong 53 nua-ky-tu, KHONG CO CUON
--     (UiMsgSel.ini [InfoText] Width=377 Height=96 Font=14; WndText.cpp:60,236).
--     Dong thu 7 tro di MAT SACH - phai dem dong theo do dai XAU NHAT.
--  4. Client chi ve 6 NUT mot luc; nut thu 7-8 phai keo thanh truot => moi
--     man toi da 6 nut, nut "Tro ve" luon nam trong 6 nut do.
--  5. Ham nhan dap an luon khai 5 tham so (engine truyen tu atoi, thieu = 0).
--  6. Nut thoat dung "/no" - KHONG dung OnCancel (khong co trong chuoi Include
--     cua lenh bai -> nil -> m_ActionScriptID = 0 -> CHET CA CAY MENU).
--  7. Nhan cam ky tu "/", MOI chuoi cam ky tu "|" (la dau phan cach cua goi).
--  8. AddItem phai du 7 tham so.
--  9. Field chua NameID doc bang TONG_GetUTaskValue (ban co dau sai khi >= 2^31).
-- 10. sSendFieldCmd (KTongJX2.cpp:382) CO ap lac quan -> doc lai NGAY duoc.
--     sSendTongOp  (KTongJX2.cpp:1349) KHONG ap lac quan -> moi ham TOP_*
--     (Upgrade/Degrade/Maintain/WeeklyMaintain/CreatMap/DeleteMap/SetStunt/
--     Contribute/Right/Offer/Money) phai TACH nut "lam" va nut "xem", cam in
--     gia tri moi ngay sau khi goi (se luon ra gia tri CU = bao hong gia).
--
-- Moi ham/bien toan cuc deu co tien to TX_/TCT_/TBH_/TLD_/TPT_/TDT_/TIT_ vi
-- lenh bai da Include ~20 lib.

Include("\\script\\timerserver_ctc.lua")

g_TX_TESTTONG = "TESTGAME"      -- DOI thanh ten bang test that truoc khi dung
g_TX_MAXPLAYER = 5              -- tran nguoi online cho phep bam nut toan server

-- Khu tac phuong: ten, file script, va tham so thu 3 cua TWS_ApplyUse.
-- 0 = khu chi nhan 2 tham so. Khu 1/2/7 co USE_G_2(nTongID,nWsID,nUse) nen
-- THIEU tham so 3 se TRU tien + san luong ma KHONG phat vat pham (van tra 1).
-- Doi chieu ban goc: ws_bingjia.lua:228 dung 4, ws_huodong.lua:132 dung 1.
g_TPT_NAME = {"Binh giap", "Thien Cong", "Mat na", "Thi luyen", "Thien Y", "Le vat", "Hoat dong"};
g_TPT_FILE = {"ws_bingjia", "ws_tiangong", "ws_mianju", "ws_shilian", "ws_tianyi", "ws_liwu", "ws_huodong"};
g_TPT_CHOSE = {4, 1, 0, 0, 0, 0, 1};

--==================== tien ich chung ====================

-- ID bang: dung GetTong() y het ban goc (scriptjx2\tong_vn\tong.lua:808).
-- GetTong tra dwNameID lay tu BANG BANG JX2 that (KTongJX2.cpp:3959) - dung
-- nguon ma moi ham TONG_*/TWS_* nhan vao (sArgTongID -> FindTong).
-- KHONG dung GetTongName() vi do la ban sao trong KPlayer (m_cTong).
function TX_T()
    local nT = GetTong();
    if (nT == nil) then
        return 0;
    end
    return nT;
end

-- chot an toan: chi cho thao tac ghi khi dang o BANG TEST
function TX_Guard()
    local nT = TX_T();
    if (nT == 0) then
        Say("Chua vao bang nao - khong the thao tac.", 1, "Tro ve/TX_Root");
        return 0;
    end
    local szName = TONG_GetName(nT);
    if (szName ~= g_TX_TESTTONG) then
        Say("CHAN: bang cua ban la ["..szName.."],\nkhong phai bang test ["..g_TX_TESTTONG.."].\nSua g_TX_TESTTONG trong bangthanh_f.lua roi bam Nap lai file test.", 1, "Tro ve/TX_Root");
        return 0;
    end
    return nT;
end

-- rao cho nut cham vao du lieu NGOAI bang test (7 thanh, toan server)
function TX_GuardAll()
    if (TX_Guard() == 0) then
        return 0;
    end
    if (GetPlayerCount ~= nil) then
        local n = GetPlayerCount();
        if (n ~= nil and n > g_TX_MAXPLAYER) then
            Say("CHAN: dang co "..n.." nguoi online (tran "..g_TX_MAXPLAYER..").\nLuu y: chi dem nguoi tren GameServer nay.", 1, "Tro ve/TX_Root");
            return 0;
        end
    end
    return 1;
end

function TX_Back(s, sMenu)
    Say(s, 1, "Tro ve/"..sMenu);
end

function TX_Root(a, b, c, d, e)
    Say("TEST BANG HOI + CONG THANH (ban goc Linux)", 6,
        "1 Xem nhanh/TX_Status",
        "2 Cong thanh/TCT_Menu",
        "3 Bang hoi/TBH_Menu",
        "4 Lanh dia + kien thiet/TLD_Menu",
        "Sang trang 2/TX_Root2",
        "Thoi/no");
end

function TX_Root2(a, b, c, d, e)
    Say("TEST BANG HOI + CONG THANH - trang 2", 5,
        "5 Tac phuong/TPT_Menu",
        "6 Danh hieu + thue/TDT_Menu",
        "7 Tien ich + ha tang/TIT_Menu",
        "Ve trang 1/TX_Root",
        "Thoi/no");
end

function TX_Status(a, b, c, d, e)
    local nCity = GetCityArea();
    local nT = TX_T();
    local s = "map="..SubWorldIdx2ID(SubWorld).." thanh="..nCity.."\n";
    if (nT == 0) then
        s = s.."Bang: CHUA VAO BANG NAO\n";
    else
        s = s..TONG_GetName(nT).." capKT="..TONG_GetBuildLevel(nT).." khu="..TWS_GetWorkshopCount(nT).."\n";
        s = s.."f45="..TONG_GetTongMap(nT).." f46="..TONG_GetTongMapTemplate(nT).." Ngay="..TONG_GetDay(nT).."\n";
        s = s.."Quy="..TONG_GetMoney(nT).."\n";
        s = s.."QuyKT="..TONG_GetBuildFund(nT).."\n";
    end
    local nOwn = 0;
    local nWar = 0;
    for i = 1, 7 do
        if (GetCityOwner(i) ~= "") then
            nOwn = nOwn + 1;
        end
        if (HaveBeginWar(i) == 1) then
            nWar = nWar + 1;
        end
    end
    s = s.."Thanh co chu "..nOwn.."/7 - dang chien "..nWar;
    TX_Back(s, "TX_Root");
end

--==================== 2. CONG THANH ====================

function TCT_Menu(a, b, c, d, e)
    Say("CONG THANH", 6,
        "Xem 7 thanh (1-4)/TCT_Show1",
        "Thanh dang dung/TCT_Here",
        "Bang bao danh thanh nay/TCT_Sign",
        "Bo nhiem - khieu chien/TCT_OwnMenu",
        "Ep pha 18h-19h-20h-0h/TCT_PMenu",
        "Tro ve/TX_Root");
end

-- Dong PHAI ngan: tran 53 nua-ky-tu moi dong, 6 dong. Ten bang toi da 31 byte
-- nen bo ten Thai Thu (xem o "Thanh dang dung") va dung [C]/[B] thay chu day.
function TCT_Line(i)
    local o = GetCityOwner(i);
    local s = i.." "..GetCityAreaName(i).." ";
    if (o == "") then
        s = s.."(vo chu)";
    else
        s = s..o;
    end
    if (HaveBeginWar(i) == 1) then
        s = s.." [C]";
    end
    if (IsSigningUp(i) == 1) then
        s = s.." [B]";
    end
    return s;
end

function TCT_Show1(a, b, c, d, e)
    local s = "C=dang chien  B=dang bao danh\n";
    for i = 1, 4 do
        s = s..TCT_Line(i).."\n";
    end
    Say(s, 2, "Xem thanh 5-7/TCT_Show2", "Tro ve/TCT_Menu");
end

function TCT_Show2(a, b, c, d, e)
    local s = "";
    for i = 5, 7 do
        s = s..TCT_Line(i).." thue "..CTC_JX2_GetTax(i).."\n";
    end
    Say(s, 2, "Xem thanh 1-4/TCT_Show1", "Tro ve/TCT_Menu");
end

function TCT_Here(a, b, c, d, e)
    local n = GetCityArea();
    if (n < 1 or n > 7) then
        TX_Back("Dang o ngoai 7 thanh (GetCityArea="..n..").", "TCT_Menu");
        return
    end
    TX_Back(GetCitySummary(n), "TCT_Menu");
end

function TCT_Sign(a, b, c, d, e)
    local n = GetCityArea();
    if (n < 1 or n > 7) then
        TX_Back("Phai dung trong 1 trong 7 thanh.", "TCT_Menu");
        return
    end
    local nCnt = NumOfSignUpTongs(n);
    local s = GetCityAreaName(n).." - so bang bao danh: "..nCnt.."\n";
    local i = 0;
    while (i < nCnt and i < 4) do
        s = s.."- "..GetSignUpTongName(n, i).."\n";
        i = i + 1;
    end
    TX_Back(s, "TCT_Menu");
end

function TCT_OwnMenu(a, b, c, d, e)
    local n = GetCityArea();
    if (n < 1 or n > 7) then
        TX_Back("Phai dung trong thanh muon thao tac.", "TCT_Menu");
        return
    end
    local o = GetCityOwner(n);
    Say("Thanh "..GetCityAreaName(n).." - chu hien tai: "..o.."\nMoi nut duoi deu doi DU LIEU THAT cua 7 thanh.", 5,
        "Bang TA lam CHU thanh nay/#TCT_Appt("..n..")",
        "Bang TA KHIEU CHIEN thanh nay/#TCT_Chal("..n..")",
        "Ep trang thai thanh/TCT_StMenu",
        "Ket tran/TCT_WMenu",
        "Tro ve/TCT_Menu");
end

function TCT_Appt(n, b, c, d, e)
    local o = GetCityOwner(n);
    Say("Doi CHU THANH "..GetCityAreaName(n).." thanh bang cua ban.\nChu cu ["..o.."] bi THU danh hieu Thai Thu va mat field 48\n(bang cu mat dieu kien len cap kien thiet 5).", 2,
        "Dong y doi chu/#TCT_ApptGo("..n..")",
        "Thoi/TCT_Menu");
end

function TCT_ApptGo(n, b, c, d, e)
    if (TX_GuardAll() == 0) then
        return
    end
    local szTong = GetTongName();
    if (szTong == nil or szTong == "") then
        TX_Back("Chua co bang.", "TCT_Menu");
        return
    end
    AppointViceroy(GetCityAreaName(n), szTong);
    TX_Back("Da bo nhiem bang ["..szTong.."] lam chu "..GetCityAreaName(n)..".\nBam Xem 7 thanh de kiem.", "TCT_Menu");
end

function TCT_Chal(n, b, c, d, e)
    Say("Dat bang cua ban lam KHIEU CHIEN GIA cua "..GetCityAreaName(n)..".\nThanh se chuyen sang trang thai co khieu chien.", 2,
        "Dong y khieu chien/#TCT_ChalGo("..n..")",
        "Thoi/TCT_Menu");
end

function TCT_ChalGo(n, b, c, d, e)
    if (TX_GuardAll() == 0) then
        return
    end
    local szTong = GetTongName();
    if (szTong == nil or szTong == "") then
        TX_Back("Chua co bang.", "TCT_Menu");
        return
    end
    AppointChallenger(GetCityAreaName(n), szTong);
    TX_Back("Da dat bang ["..szTong.."] lam khieu chien gia.\nHaveBeginWar = "..HaveBeginWar(n), "TCT_Menu");
end

function TCT_StMenu(a, b, c, d, e)
    local n = GetCityArea();
    if (n < 1 or n > 7) then
        TX_Back("Phai dung trong thanh.", "TCT_Menu");
        return
    end
    Say("Ep trang thai thanh "..GetCityAreaName(n)..".\nGhi thang vao du lieu thanh THAT.", 4,
        "0 binh thuong/#TCT_St("..n..",0)",
        "1 da co khieu chien/#TCT_St("..n..",1)",
        "2 DANG DANH/#TCT_St("..n..",2)",
        "Tro ve/TCT_Menu");
end

function TCT_St(n, s, c, d, e)
    if (TX_GuardAll() == 0) then
        return
    end
    CTC_JX2_SetCityState(n, s);
    TX_Back("Da dat trang thai "..s..". HaveBeginWar = "..HaveBeginWar(n), "TCT_Menu");
end

function TCT_PMenu(a, b, c, d, e)
    local n = GetCityArea();
    if (n < 1 or n > 7) then
        TX_Back("Phai dung trong thanh muon ep pha.", "TCT_Menu");
        return
    end
    Say("Ep pha cho "..GetCityAreaName(n)..".\nCA 4 PHA deu cham vao CA 7 THANH o muc do khac nhau.\nDong ho 5 pha KHONG tu chay - chi ep tay o day.", 5,
        "18h mo bao danh/#TCT_P18("..n..")",
        "19h chot khieu chien/#TCT_P19("..n..")",
        "20h khai chien/#TCT_P20("..n..")",
        "0h don ngay/#TCT_P0("..n..")",
        "Tro ve/TCT_Menu");
end

-- 18h: Ladder_ClearLadder(10261) xoa bang dau gia CHUNG ca 7 thanh
function TCT_P18(n, b, c, d, e)
    Say("Ep 18h se XOA BANG DAU GIA khieu chien lenh cua CA 7 THANH\n(Ladder_ClearLadder 10261) roi bat co bao danh cho thanh nay.\nNeu dang ep trong khung 18h that thi dong ho that co the ban lai.", 2,
        "Dong y ep 18h/#TCT_P18G("..n..")",
        "Thoi/TCT_Menu");
end

function TCT_P18G(n, b, c, d, e)
    if (TX_GuardAll() == 0) then
        return
    end
    g_CTC6_D18 = nil;
    CTC_JX2_Tick(TB_CTC6[n][1], 18, 0);
    g_CTC6_D18 = nil;
    TX_Back("Da ep 18h. IsSigningUp = "..IsSigningUp(n), "TCT_Menu");
end

-- 19h: timerserver_ctc.lua:183-194 duyet CA 7 THANH; thanh khong trung lich
-- se bi ctc6_endsignup(i,0) -> LG_ApplySetLeagueTask(508,...,1,0) = HA CO
-- BAO DANH. Day la hanh vi dung cua ban goc (relay chay task 19h moi ngay).
function TCT_P19(n, b, c, d, e)
    Say("Ep 19h chot khieu chien cho thanh nay, DONG THOI HA CO BAO DANH\ncua CA 6 THANH CON LAI. Thanh dang VO CHU se duoc gan chu NGAY.\nSo bang bao danh thanh nay: "..NumOfSignUpTongs(n), 2,
        "Dong y ep 19h/#TCT_P19G("..n..")",
        "Thoi/TCT_Menu");
end

function TCT_P19G(n, b, c, d, e)
    if (TX_GuardAll() == 0) then
        return
    end
    g_CTC6_D19 = nil;
    CTC_JX2_Tick(TB_CTC6[n][1], 19, 0);
    g_CTC6_D19 = nil;
    TX_Back("Da ep 19h. Chu thanh: "..GetCityOwner(n).."\nBam Xem 7 thanh de xem khieu chien gia.", "TCT_Menu");
end

-- 20h: CHU Y cot [2] (thu DANH), khong phai [1] (timerserver_ctc.lua:200)
function TCT_P20(n, b, c, d, e)
    Say("Ep 20h KHAI CHIEN thanh "..GetCityAreaName(n)..".\nMo mission chien truong that trong vong 5 phut.", 2,
        "Dong y khai chien/#TCT_P20G("..n..")",
        "Thoi/TCT_Menu");
end

function TCT_P20G(n, b, c, d, e)
    if (TX_GuardAll() == 0) then
        return
    end
    g_CTC6_D20 = nil;
    CTC_JX2_Tick(TB_CTC6[n][2], 20, 0);
    g_CTC6_D20 = nil;
    TX_Back("Da ep 20h. HaveBeginWar = "..HaveBeginWar(n), "TCT_Menu");
end

-- 0h: go sach member league 508/509 cua 7 thanh
function TCT_P0(n, b, c, d, e)
    Say("Ep 0h se GO SACH thanh vien league 508 va 509 cua CA 7 THANH\n(diem bao danh cua moi bang ve 0, ghi thang xuong file).", 2,
        "Dong y ep 0h/#TCT_P0G(1)",
        "Thoi/TCT_Menu");
end

function TCT_P0G(a, b, c, d, e)
    if (TX_GuardAll() == 0) then
        return
    end
    g_CTC6_D0 = nil;
    CTC_JX2_Tick(0, 0, 0);
    g_CTC6_D0 = nil;
    TX_Back("Da ep 0h don ngay.", "TCT_Menu");
end

function TCT_WMenu(a, b, c, d, e)
    local n = GetCityArea();
    if (n < 1 or n > 7) then
        TX_Back("Phai dung trong thanh.", "TCT_Menu");
        return
    end
    local cong, thu = GetCityWarBothSides(n);
    Say("Ket tran "..GetCityAreaName(n).."\nCong: "..cong.."\nThu: "..thu, 3,
        "CONG thang - doi chu + Title/#TCT_Win("..n..",1)",
        "THU thang - giu nguyen chu/#TCT_Win("..n..",0)",
        "Tro ve/TCT_Menu");
end

function TCT_Win(n, w, c, d, e)
    Say("Ket tran that: doi chu thanh, cap danh hieu Thai Thu cho ben thang\nva THU danh hieu cua Thai Thu cu.", 2,
        "Dong y ket tran/#TCT_WinGo("..n..","..w..")",
        "Thoi/TCT_Menu");
end

function TCT_WinGo(n, w, c, d, e)
    if (TX_GuardAll() == 0) then
        return
    end
    NotifyWarResult(n, w);
    local o, m = GetCityOwner(n);
    TX_Back("Da ket tran.\nChu thanh: "..o.."\nThai Thu: "..m.."\nBam 6 Danh hieu de xem Title 15x.", "TCT_Menu");
end

--==================== 3. BANG HOI: KINH TE + NGUOI ====================

function TBH_Menu(a, b, c, d, e)
    Say("BANG HOI - kinh te va nguoi", 6,
        "Ho so + kinh te (1)/TBH_Fund1",
        "Kinh te (2) tuan - bao tri/TBH_Fund2",
        "Nap quy - cong hien - ngay/TBH_AddMenu",
        "Thanh vien + quyen/TBH_MemMenu",
        "Muc tieu tuan + bao tri/TBH_MntMenu",
        "Tro ve/TX_Root");
end

function TBH_Fund1(a, b, c, d, e)
    local nT = TX_T();
    if (nT == 0) then
        TX_Back("Chua vao bang.", "TBH_Menu");
        return
    end
    local s = TONG_GetName(nT).." - chu "..TONG_GetMaster(nT).."\n";
    s = s.."Nguoi "..TONG_GetMemberCount(nT, -1).." (on "..TONG_GetOnlineCount(nT)..") capKT "..TONG_GetBuildLevel(nT).."\n";
    s = s.."Ngan quy: "..TONG_GetMoney(nT).."\n";
    s = s.."Quy kien thiet: "..TONG_GetBuildFund(nT).."\n";
    s = s.."Quy chien bi: "..TONG_GetWarBuildFund(nT).."\n";
    s = s.."Cong hien du tru: "..TONG_GetStoredOffer(nT);
    TX_Back(s, "TBH_Menu");
end

function TBH_Fund2(a, b, c, d, e)
    local nT = TX_T();
    if (nT == 0) then
        TX_Back("Chua vao bang.", "TBH_Menu");
        return
    end
    local s = "KT tuan: "..TONG_GetWeekBuildFund(nT).." tran "..TONG_GetWeekBuildUpper(nT).."\n";
    s = s.."Phi duy tri: "..TONG_GetMaintainFund(nT).."\n";
    s = s.."Gop moi nguoi: "..TONG_GetPerStandFund(nT).." chuan "..TONG_GetStandFund(nT).."\n";
    s = s.."Ngay "..TONG_GetDay(nT).." Tuan "..TONG_GetWeek(nT).."\n";
    s = s.."Khoa 1050: "..TONG_GetUTaskValue(nT, 1050).."\n";
    s = s.."Tam ngung: "..TONG_GetPauseState(nT);
    TX_Back(s, "TBH_Menu");
end

function TBH_AddMenu(a, b, c, d, e)
    Say("Nap quy - cong hien - ngay.\nTien va cong hien KHONG hien ngay, phai bam nut Xem.", 6,
        "+1000 van ngan quy/#TBH_MonGo(10000000)",
        "+100000 quy kien thiet/#TBH_BfGo(100000)",
        "Dat quy kien thiet = 0/#TBH_BfSet(0)",
        "NGAY +8 (mo khoa nang cap)/#TBH_DayGo(8)",
        "Nop 200 cong hien vao quy/#TBH_StoGo(200)",
        "Xem lai kinh te (1)/TBH_Fund1");
end

function TBH_MonGo(v, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    TONG_ApplyAddMoney(nT, v);
    TX_Back("Da gui lenh cong "..v.." luong len relay.\nBam Xem lai kinh te (1) sau vai giay.", "TBH_AddMenu");
end

function TBH_BfGo(v, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    if (v < 0) then
        TX_Back("CAM cong so am - field 12 la khong dau, se tran ~4,29 ty.", "TBH_AddMenu");
        return
    end
    TONG_ApplyAddBuildFund(nT, v);
    TX_Back("Quy kien thiet: "..TONG_GetBuildFund(nT).."\n(field nay CO ap lac quan nen doi ngay)", "TBH_AddMenu");
end

function TBH_BfSet(v, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    TONG_ApplySetBuildFund(nT, v);
    TX_Back("Quy kien thiet = "..TONG_GetBuildFund(nT).."\n(phai la "..v..", KHONG duoc ra ~4.294.967.xxx)", "TBH_AddMenu");
end

function TBH_DayGo(v, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    local nOld = TONG_GetDay(nT);
    TONG_ApplySetDay(nT, nOld + v);
    TX_Back("Ngay: "..nOld.." -> "..TONG_GetDay(nT).."\n(ApplyAddDay la no-op co chu dich nen phai dung Set)", "TBH_AddMenu");
end

-- Goi ContributeOffer TRUOC, chi tru cong hien ca nhan khi lenh da gui duoc
-- (AddContribution ghi dong bo ngay, con ContributeOffer chi day goi len relay)
function TBH_StoGo(v, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    local nCur = GetContribution();
    if (nCur < v) then
        TX_Back("Cong hien ca nhan chi co "..nCur..", can "..v..".", "TBH_AddMenu");
        return
    end
    local nRet = TONG_ContributeOffer(nT, GetTongMemberID(), v);
    if (nRet ~= 1) then
        TX_Back("Lenh nop bi tu choi (ma "..nRet..") - KHONG tru cong hien.", "TBH_AddMenu");
        return
    end
    AddContribution(-1 * v);
    TX_Back("Cong hien ca nhan: "..nCur.." -> "..GetContribution()..".\nBam Xem lai kinh te (1) de xem Cong hien du tru.", "TBH_AddMenu");
end

function TBH_MemMenu(a, b, c, d, e)
    Say("Thanh vien + quyen. Quyen KHONG hien ngay - phai bam Xem lai.", 6,
        "6 thanh vien dau/TBH_Mem",
        "Quyen cua TRUONG LAO dau/TBH_Right",
        "Cap quyen 9001 tac phuong/#TBH_RgtGo(9001,1)",
        "Go quyen 9001/#TBH_RgtGo(9001,0)",
        "+500 cong hien cho TA/#TBH_OffGo(500)",
        "Tro ve/TBH_Menu");
end

-- Chi in 4 thanh vien: ten nhan vat co the dai 31 byte, moi ban ghi co the
-- chiem 2 dong ve => 6 ban ghi se tran 6 dong.
function TBH_Mem(a, b, c, d, e)
    local nT = TX_T();
    if (nT == 0) then
        TX_Back("Chua vao bang.", "TBH_Menu");
        return
    end
    local s = "";
    local id = TONG_GetFirstMember(nT, -1);
    local n = 0;
    while (id ~= nil and id ~= 0 and n < 4) do
        local szOn = "";
        if (TONGM_GetOnline(nT, id) == 1) then
            szOn = " ON";
        end
        s = s..TONGM_GetName(nT, id).." chuc"..TONGM_GetFigure(nT, id).." CH"..TONGM_GetOffer(nT, id)..szOn.."\n";
        id = TONG_GetNextMember(nT, id, -1);
        n = n + 1;
    end
    if (n == 0) then
        s = "Khong doc duoc thanh vien nao.";
    else
        s = s.."(tong "..TONG_GetMemberCount(nT, -1).." nguoi)";
    end
    TX_Back(s, "TBH_MemMenu");
end

function TBH_Right(a, b, c, d, e)
    local nT = TX_T();
    if (nT == 0) then
        TX_Back("Chua vao bang.", "TBH_Menu");
        return
    end
    local id = TONG_GetFirstMember(nT, 1);
    if (id == nil or id == 0) then
        TX_Back("CHUA CO TRUONG LAO - ket qua kiem quyen se vo nghia\n(bang chu luon tra 1). Phong 1 truong lao bang cua so bang roi lam lai.", "TBH_MemMenu");
        return
    end
    local s = "Truong lao: "..TONGM_GetName(nT, id).."\n";
    s = s.."1002 bo nhiem: "..TONGM_CheckRight(nT, id, 1002).."\n";
    s = s.."1901 duyet don: "..TONGM_CheckRight(nT, id, 1901).."\n";
    s = s.."2004 lanh dia: "..TONGM_CheckRight(nT, id, 2004).."\n";
    s = s.."9001 tac phuong: "..TONGM_CheckRight(nT, id, 9001);
    TX_Back(s, "TBH_MemMenu");
end

function TBH_RgtGo(nRight, nOn, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    local id = TONG_GetFirstMember(nT, 1);
    if (id == nil or id == 0) then
        TX_Back("Chua co truong lao de cap quyen.", "TBH_MemMenu");
        return
    end
    if (nOn == 1) then
        TONGM_ApplyAddRight(nT, id, nRight);
    else
        TONGM_ApplyDelRight(nT, id, nRight);
    end
    TX_Back("Da gui lenh quyen "..nRight.." = "..nOn.." cho "..TONGM_GetName(nT, id)..".\nBam Quyen cua TRUONG LAO dau sau vai giay de kiem.", "TBH_MemMenu");
end

function TBH_OffGo(v, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    TONGM_ApplyAddOffer(nT, GetName(), v);
    TX_Back("Da gui lenh cong "..v.." cong hien cho "..GetName()..".\nBam 6 thanh vien dau sau vai giay de kiem.", "TBH_MemMenu");
end

function TBH_MntMenu(a, b, c, d, e)
    Say("Muc tieu tuan + bao tri.\n2 nut bao tri ca nhan CHI day bo dem o relay (Ngay+1 va Tuan+1),\nKHONG tru phi duy tri, KHONG chot muc tieu tuan - phan kinh te do\nchi chay qua nut TOAN SERVER.", 6,
        "Muc tieu tuan nay/TBH_Goal",
        "Bao tri NGAY - chi bo dem/#TBH_MntGo(1)",
        "Bao tri TUAN - chi bo dem/#TBH_MntGo(2)",
        "TOAN SERVER - kinh te ngay/#TBH_MntAsk(3)",
        "Ghi 20 dong nhat ky/#TBH_LogGo(20)",
        "Tro ve/TBH_Menu");
end

function TBH_Goal(a, b, c, d, e)
    local nT = TX_T();
    if (nT == 0) then
        TX_Back("Chua vao bang.", "TBH_Menu");
        return
    end
    local nMem = TONG_GetMemberCount(nT, -1);
    local s = "Cap muc tieu "..TONG_GetCurWeekGoalLevel(nT).." su kien "..TONG_GetWeekGoalEvent(nT).."\n";
    s = s.."Diem "..TONG_GetWeekGoalValue(nT).." tren "..TONG_GetWeekGoalTotal(nT).."\n";
    s = s.."Tuan truoc: cap "..TONG_GetLWeekGoalLevel(nT).." diem "..TONG_GetLWeekGoalValue(nT).."\n";
    s = s.."So thanh vien: "..nMem.."\n";
    if (nMem < 100) then
        s = s.."Duoi 100 nguoi: bao tri tuan KHONG dat muc tieu (dung goc).";
    end
    Say(s, 2, "+5000 diem muc tieu/#TBH_GoalGo(5000)", "Tro ve/TBH_MntMenu");
end

function TBH_GoalGo(v, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    TONG_ApplyAddWeekGoalValue(nT, v);
    TX_Back("Diem muc tieu tuan: "..TONG_GetWeekGoalValue(nT), "TBH_MntMenu");
end

-- TOP_* -> KHONG ap lac quan: cam in gia tri moi ngay sau khi goi
function TBH_MntGo(nMode, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    if (nMode == 1) then
        local nOld = TONG_GetDay(nT);
        TONG_ApplyMaintain(nT);
        TX_Back("Da gui lenh bao tri NGAY. Ngay cu = "..nOld..".\nBam Kinh te (2) sau vai giay de xem Ngay moi.\nRelay CHI lam Ngay+1, khong tru phi duy tri.", "TBH_MntMenu");
    else
        local nOld = TONG_GetWeek(nT);
        TONG_ApplyWeeklyMaintain(nT);
        TX_Back("Da gui lenh bao tri TUAN. Tuan cu = "..nOld..".\nBam Kinh te (2) sau vai giay de xem Tuan moi.\nRelay CHI lam Tuan+1 va don khoa, khong chot muc tieu.", "TBH_MntMenu");
    end
end

function TBH_MntAsk(nMode, b, c, d, e)
    Say("TOAN SERVER: quet MOI BANG tren may chu, chay MAINTAIN_R that\n(tru quy chien bi, tam ngung tac phuong, chot muc tieu tuan).\nChi lam khi server vang nguoi.", 2,
        "Dong y chay/#TBH_MntAll("..nMode..")",
        "Thoi/TBH_MntMenu");
end

function TBH_MntAll(nMode, b, c, d, e)
    if (TX_GuardAll() == 0) then
        return
    end
    local nRun = TONG_DailyMaintainAll(2);
    if (nRun == 0) then
        TX_Back("Ket qua 0 - CO HAI kha nang khac han nhau:\n(1) khong co bang nao tren may chu, hoac\n(2) g_GetScript CHUA NAP scriptjx2\\tong_vn\\tong.lua luc boot.\nXem log boot de phan biet.", "TBH_MntMenu");
        return
    end
    TX_Back("Da chay bao tri toan server. So bang: "..nRun, "TBH_MntMenu");
end

function TBH_LogGo(nCnt, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    local i = 1;
    while (i <= nCnt) do
        TONG_ApplyAddEventRecord(nT, "TX "..i.." dong nhat ky test do dai trung binh cua mot ban ghi su kien bang hoi");
        i = i + 1;
    end
    TX_Back("Da ghi "..nCnt.." dong.\nMO CUA SO BANG trang Nhat ky: phai thay 16 dong cuoi,\nchu khong cut, khong van game.", "TBH_MntMenu");
end

--==================== 4. LANH DIA + KIEN THIET ====================

function TLD_Menu(a, b, c, d, e)
    Say("LANH DIA + KIEN THIET", 6,
        "Xem lanh dia/TLD_Info",
        "Chon khu vuc CHUNG/TLD_Pub",
        "Tao lanh dia RIENG theo mau/TLD_Crt",
        "Vao lanh dia/TLD_Go",
        "Cap kien thiet + tuyet ky/TLD_LvMenu",
        "Tro ve/TX_Root");
end

function TLD_Info(a, b, c, d, e)
    local nT = TX_T();
    if (nT == 0) then
        TX_Back("Chua vao bang.", "TLD_Menu");
        return
    end
    local nMap = TONG_GetTongMap(nT);
    local s = "f45 map lanh dia: "..nMap.."\n";
    s = s.."f46 map mau: "..TONG_GetTongMapTemplate(nT).."\n";
    s = s.."f47 cam dia: "..TONG_GetTongMapBan(nT).."\n";
    s = s.."f48 ngay chiem thanh: "..TONG_GetUTaskValue(nT, 48).."\n";
    s = s.."Cap kien thiet: "..TONG_GetBuildLevel(nT).."\n";
    if (nMap == 0) then
        s = s.."=> CHUA CO LANH DIA";
    elseif (SubWorldID2Idx(nMap) < 0) then
        s = s.."=> MAP CHUA NAP tren server";
    else
        s = s.."=> map da nap, vao duoc";
    end
    TX_Back(s, "TLD_Menu");
end

function TLD_Pub(a, b, c, d, e)
    Say("Khu vuc CHUNG (nhieu bang dung chung).\nf46 se van la 0 - dung thiet ke.", 5,
        "Khu vuc chung 586/#TLD_PubGo(586)",
        "Khu vuc chung 595/#TLD_PubGo(595)",
        "Khu vuc chung 596/#TLD_PubGo(596)",
        "Khu vuc chung 597/#TLD_PubGo(597)",
        "Tro ve/TLD_Menu");
end

function TLD_PubGo(nMap, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    TONG_ApplySetTongMap(nT, nMap);
    TX_Back("f45 = "..TONG_GetTongMap(nT).."\n(field 45 CO ap lac quan nen doi ngay)\nBam Vao lanh dia de kiem.", "TLD_Menu");
end

-- CHI 7 id duoi day la MAU lanh dia rieng cua ban goc.
-- 586/595/596/597 la khu vuc CHUNG, 592 khong ton tai.
function TLD_Crt(a, b, c, d, e)
    Say("Tao lanh dia RIENG theo map mau.\nRelay chi ghi id mau, KHONG sinh map dong.\nMau hop le: 587 588 589 590 591 593 594.", 5,
        "Mau 587/#TLD_CrtGo(587)",
        "Mau 589/#TLD_CrtGo(589)",
        "Mau 591/#TLD_CrtGo(591)",
        "Mau 594/#TLD_CrtGo(594)",
        "Tro ve/TLD_Menu");
end

function TLD_CrtGo(nTpl, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    if (nTpl ~= 587 and nTpl ~= 588 and nTpl ~= 589 and nTpl ~= 590
        and nTpl ~= 591 and nTpl ~= 593 and nTpl ~= 594) then
        TX_Back("CHAN: "..nTpl.." khong phai map MAU lanh dia rieng.\nChi nhan 587 588 589 590 591 593 594.", "TLD_Menu");
        return
    end
    TONG_ApplyCreatMap(nT, nTpl);
    TX_Back("Da gui lenh tao lanh dia mau "..nTpl..".\nDay la lenh TOP_* nen KHONG doi ngay - bam Xem lanh dia\nsau vai giay de kiem f45 va f46.", "TLD_Menu");
end

function TLD_Go(a, b, c, d, e)
    local nT = TX_T();
    if (nT == 0) then
        TX_Back("Chua vao bang.", "TLD_Menu");
        return
    end
    local nMap = TONG_GetTongMap(nT);
    if (nMap == 0) then
        TX_Back("Bang chua co lanh dia (f45 = 0).", "TLD_Menu");
        return
    end
    if (SubWorldID2Idx(nMap) < 0) then
        TX_Back("Map "..nMap.." CHUA NAP tren server.", "TLD_Menu");
        return
    end
    if (nMap == 591) then
        NewWorld(591, 1712, 3330);
    else
        NewWorld(nMap, 1718, 3313);
    end
end

function TLD_LvMenu(a, b, c, d, e)
    local nT = TX_T();
    local nLv = 0;
    local nDay = 0;
    if (nT ~= 0) then
        nLv = TONG_GetBuildLevel(nT);
        nDay = TONG_GetDay(nT);
    end
    Say("Cap kien thiet "..nLv.." - Ngay "..nDay..".\nCANH BAO: ban port BO 3 dieu kien goc (cho 7 ngay, cap 2 len 3 doi\nlanh dia rieng, cap 4 len 5 doi chiem thanh). Relay chi kiem quy va\nso tac phuong, nen cap 2 len 3 SE THANH CONG - lech so voi goc.", 6,
        "Nang cap kien thiet/#TLD_LvGo(1)",
        "Ha cap kien thiet/#TLD_LvGo(0)",
        "Bat cam dia/#TLD_BanGo(1)",
        "Tat cam dia/#TLD_BanGo(0)",
        "Tuyet ky bang/TLD_StMenu",
        "Xoa lanh dia/TLD_Del");
end

-- TOP_UPGRADE / TOP_DEGRADE -> sSendTongOp, KHONG lac quan: cam doc lai ngay
function TLD_LvGo(nUp, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    local nOld = TONG_GetBuildLevel(nT);
    if (nUp == 1) then
        TONG_ApplyUpgrade(nT);
    else
        TONG_ApplyDegrade(nT);
    end
    TX_Back("Da gui lenh len relay. Cap cu = "..nOld..".\nBam Xem lanh dia sau vai giay de doc cap MOI.\n(doc ngay tai day se luon ra cap cu = bao hong gia)", "TLD_Menu");
end

function TLD_BanGo(nOn, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    TONG_ApplySetTongMapBan(nT, nOn);
    TX_Back("f47 cam dia = "..TONG_GetTongMapBan(nT), "TLD_Menu");
end

-- Khoa tuyet ky THAT: 1011 ID, 1012 MEMLIMIT, 1013 MAINPERIOD, 1014 ENABLED,
-- 1021 SWICTH, 1036 PAUSE (scriptjx2\tong_vn\tong_header.lua:66-74).
function TLD_StMenu(a, b, c, d, e)
    local nT = TX_T();
    if (nT == 0) then
        TX_Back("Chua vao bang.", "TLD_Menu");
        return
    end
    local s = "Khoa tuyet ky (tong_header.lua:66-74):\n";
    s = s.."1011 ID="..TONG_GetTaskValue(nT, 1011).." 1014 bat="..TONG_GetTaskValue(nT, 1014).."\n";
    s = s.."1021 doi sang="..TONG_GetTaskValue(nT, 1021).."\n";
    s = s.."1036 tam ngung="..TONG_GetTaskValue(nT, 1036).."\n";
    s = s.."Cap kien thiet "..TONG_GetBuildLevel(nT).." (can tu 4 tro len)";
    Say(s, 3, "Dat tuyet ky ID 1/#TLD_StGo(1)", "Dat tuyet ky ID 3/#TLD_StGo(3)", "Tro ve/TLD_Menu");
end

function TLD_StGo(nId, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    if (TONG_GetBuildLevel(nT) < 4) then
        TX_Back("Cap kien thiet duoi 4 - lenh se bi bo qua im lang (dung goc).", "TLD_Menu");
        return
    end
    TONG_ApplySetStunt(nT, nId);
    TX_Back("Da gui lenh dat tuyet ky "..nId..".\nBam Tuyet ky bang sau vai giay de xem khoa 1011.", "TLD_Menu");
end

function TLD_Del(a, b, c, d, e)
    Say("Xoa lanh dia se mat luon duong vao 7 xuong tac phuong.", 2,
        "Dong y xoa/#TLD_DelGo(1)",
        "Thoi/TLD_Menu");
end

function TLD_DelGo(a, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    TONG_ApplyDeleteMap(nT);
    TX_Back("Da gui lenh xoa lanh dia.\nBam Xem lanh dia sau vai giay de kiem f45 va f46.", "TLD_Menu");
end

--==================== 5. TAC PHUONG ====================

function TPT_Menu(a, b, c, d, e)
    Say("TAC PHUONG (7 khu)", 6,
        "Xem khu 1-4/TPT_List1",
        "Xem khu 5-7 + tong quan/TPT_List2",
        "DUNG CANH lap + nang 7 khu/TPT_Setup",
        "Mo - dong - do bo - san luong/TPT_OpMenu",
        "SAN XUAT thu/TPT_UseMenu",
        "Tro ve/TX_Root2");
end

function TPT_Line(nT, t)
    local s = t.." "..g_TPT_NAME[t].." ";
    if (TWS_IsExist(nT, t) ~= 1) then
        return s.."chua lap";
    end
    local szOpen = "DONG";
    if (TWS_IsOpen(nT, t) == 1) then
        szOpen = "MO";
    end
    return s.."lv"..TWS_GetLevel(nT, t).." dung"..TWS_GetUseLevel(nT, t).." "..szOpen.." SL"..TWS_GetDayOutput(nT, t);
end

function TPT_List1(a, b, c, d, e)
    local nT = TX_T();
    if (nT == 0) then
        TX_Back("Chua vao bang.", "TPT_Menu");
        return
    end
    local s = "";
    for t = 1, 4 do
        s = s..TPT_Line(nT, t).."\n";
    end
    Say(s, 2, "Xem khu 5-7/TPT_List2", "Tro ve/TPT_Menu");
end

function TPT_List2(a, b, c, d, e)
    local nT = TX_T();
    if (nT == 0) then
        TX_Back("Chua vao bang.", "TPT_Menu");
        return
    end
    local s = "";
    for t = 5, 7 do
        s = s..TPT_Line(nT, t).."\n";
    end
    s = s.."So khu "..TWS_GetWorkshopCount(nT).." quyKT "..TONG_GetBuildFund(nT);
    Say(s, 2, "Xem khu 1-4/TPT_List1", "Tro ve/TPT_Menu");
end

function TPT_Setup(a, b, c, d, e)
    Say("DUNG CANH: lap + nang cap 7 khu bang duong Lua.\nMIEN PHI, BO QUA luat cua relay - chi de dung canh test san xuat.\nMuon NGHIEM THU luat that (tru tien, tran cap, tran so khu)\nphai lam bang CUA SO BANG, tab Tac phuong.", 2,
        "Dong y dung canh/#TPT_SetGo(1)",
        "Thoi/TPT_Menu");
end

function TPT_SetGo(a, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0) then
        return
    end
    for t = 1, 7 do
        TWS_ApplyAdd(nT, t);
        local i = 1;
        while (i <= 9) do
            TWS_ApplyUpgrade(nT, t);
            i = i + 1;
        end
        TWS_ApplyOpen(nT, t);
        TWS_ApplySetUseLevel(nT, t, 10);
        TWS_ApplySetUseLevelSet(nT, t, 10);
        TWS_ApplySetDayOutput(nT, t, 100000);
    end
    TX_Back("Da lap + nang + mo + bom san luong cho ca 7 khu.\nBam Xem khu 1-4 de kiem.", "TPT_Menu");
end

function TPT_OpMenu(a, b, c, d, e)
    Say("Mo - dong - do bo - san luong - bao tri:", 6,
        "Mo khu 6 Le vat/#TPT_OpGo(6,1)",
        "Dong khu 6 Le vat/#TPT_OpGo(6,0)",
        "Bom san luong khu 6/#TPT_OutGo(6)",
        "Bao tri khu 6 (tang SL ngay)/#TPT_MntGo(6)",
        "Do bo khu 6 (mat cap)/#TPT_RmGo(6)",
        "Tro ve/TPT_Menu");
end

function TPT_OpGo(t, nOn, c, d, e)
    local nT = TX_Guard();
    if (nT == 0 or t < 1 or t > 7) then
        return
    end
    if (nOn == 1) then
        TWS_ApplyOpen(nT, t);
    else
        TWS_ApplyClose(nT, t);
    end
    TX_Back(TPT_Line(nT, t), "TPT_OpMenu");
end

function TPT_RmGo(t, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0 or t < 1 or t > 7) then
        return
    end
    TWS_ApplyRemove(nT, t);
    TX_Back("Da do bo khu "..t..". KHONG hoan quy.\n"..TPT_Line(nT, t), "TPT_OpMenu");
end

function TPT_OutGo(t, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0 or t < 1 or t > 7) then
        return
    end
    TWS_ApplySetDayOutput(nT, t, 100000);
    TX_Back(TPT_Line(nT, t), "TPT_OpMenu");
end

-- TWS_ApplyMaintain chay MAINTAIN_R cua ws_*.lua trong state RIENG (khong can
-- Include o day) - KTongJX2.cpp:2124. Chi cham vao bang cua minh.
function TPT_MntGo(t, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0 or t < 1 or t > 7) then
        return
    end
    TWS_ApplyMaintain(nT, t);
    TX_Back(TPT_Line(nT, t), "TPT_OpMenu");
end

function TPT_UseMenu(a, b, c, d, e)
    Say("SAN XUAT thu - goi THANG TWS_ApplyUse.\nBO QUA duong thoai NPC: KHONG kiem khung gio 12h30-22h,\nkhong kiem co chong spam, KHONG kiem 2 o tui trong.\nDON TRONG TUI truoc khi bam, neu khong vat pham se roi mat.", 6,
        "Le vat 6 - hong bao/#TPT_Use(6)",
        "Binh giap 1 - lenh bai Boss/#TPT_Use(1)",
        "Hoat dong 7 - lenh bai Boss/#TPT_Use(7)",
        "Thi luyen 4 (do lo)/#TPT_Use(4)",
        "Thien Y 5 (do lo)/#TPT_Use(5)",
        "Tro ve/TPT_Menu");
end

-- Khu 1/2/7 co USE_G_2(nTongID, nWsID, nUse): THIEU tham so 3 thi USE_R van
-- tru quy + 100 san luong nhung USE_G_2 khong khop nhanh nao => KHONG ra do
-- ma van tra 1 (KTongJX2.cpp:3627-3676). Ban goc luon truyen du 3.
function TPT_Use(t, b, c, d, e)
    local nT = TX_Guard();
    if (nT == 0 or t < 1 or t > 7) then
        return
    end
    local nBefore = GetContribution();
    MODEL_GAMESERVER = 1;
    Include("\\script\\tong\\workshop\\"..g_TPT_FILE[t]..".lua");
    local nRet = 0;
    if (g_TPT_CHOSE[t] ~= 0) then
        nRet = TWS_ApplyUse(nT, t, g_TPT_CHOSE[t]);
    else
        nRet = TWS_ApplyUse(nT, t);
    end
    TX_Back("Khu "..t.." "..g_TPT_NAME[t].." - ma tra "..nRet.."\nCong hien "..nBefore.." -> "..GetContribution().."\nSan luong "..TWS_GetDayOutput(nT, t).."\nMa tra 1 KHONG bao dam co do - phai mo tui kiem tan mat.", "TPT_UseMenu");
end

--==================== 6. DANH HIEU + THUE ====================

function TDT_Menu(a, b, c, d, e)
    Say("DANH HIEU + THUE", 6,
        "Danh hieu cua TA/TDT_My",
        "Cap - bat - go danh hieu/TDT_TtMenu",
        "Man goc linh danh hieu/TDT_Orig",
        "Xem thue 7 thanh/TDT_TaxSee",
        "Dat thue thanh dang dung/TDT_TaxMenu",
        "Tro ve/TX_Root2");
end

-- GetPlayerTitle tra NIL khi khong lay duoc nguoi choi (ScriptFuns.cpp:9504).
-- Noi chuoi hoac so sanh voi nil la loi runtime -> luon di qua ham nay.
function TDT_Jx1Cur()
    local n = GetPlayerTitle();
    if (n == nil) then
        return 0;
    end
    return n;
end

function TDT_My(a, b, c, d, e)
    local tab = Title_GetTitleTab();
    if (tab == nil or getn(tab) == 0) then
        TX_Back("Chua so huu danh hieu nao.", "TDT_Menu");
        return
    end
    local s = "JX2 dang bat: "..Title_GetActiveTitle().."\n";
    -- He JX1 moi la he CLIENT VE duoc chu tren dau. 0 = khong ve gi.
    s = s.."JX1 tren dau: "..TDT_Jx1Cur().."\n";
    local i = 1;
    while (i <= getn(tab) and i <= 3) do
        s = s..tab[i].." "..Title_GetTitleName(tab[i]).."\n";
        i = i + 1;
    end
    s = s.."(tong "..getn(tab)..")";
    TX_Back(s, "TDT_Menu");
end

function TDT_TtMenu(a, b, c, d, e)
    local n = GetCityArea();
    local nId = 153;
    if (n >= 1 and n <= 7) then
        nId = 152 + n;
    end
    Say("Danh hieu nam trong RAM - mat khi restart (dung thiet ke).\nBat danh hieu CO gan buff chien dau that len nhan vat.", 5,
        "Cap + bat Thai Thu thanh nay/#TDT_ViGo("..n..")",
        "Cap quan ham 89 Tong binh/#TDT_AddGo(89)",
        "Tat danh hieu dang bat/#TDT_ActGo(0)",
        "Go danh hieu Thai Thu/#TDT_RmGo("..nId..")",
        "Tro ve/TDT_Menu");
end

function TDT_ViGo(n, b, c, d, e)
    if (TX_Guard() == 0) then
        return
    end
    if (n < 1 or n > 7) then
        TX_Back("Phai dung trong 1 trong 7 thanh.", "TDT_Menu");
        return
    end
    Title_AddTitle(152 + n, 0, 9999999);
    Title_ActiveTitle(152 + n);
    SetTask(1122, 152 + n);
    -- Bat ca he JX1 thi CLIENT moi ve duoc chu tren dau (KNpc.cpp:6078-6103).
    -- Bang JX1 \settings\PlayerTitle.txt lech id 15 don vi so voi bang JX2.
    SetPlayerTitle(167 + n, 567648000, 0);
    TX_Back("Da cap va bat danh hieu.\nJX2 = "..(152 + n).." "..Title_GetTitleName(152 + n).."\nJX1 tren dau = "..TDT_Jx1Cur().." (cho "..(167 + n)..")\nNhin len dau nhan vat: phai thay chu vang.", "TDT_Menu");
end

function TDT_AddGo(nId, b, c, d, e)
    if (TX_Guard() == 0) then
        return
    end
    Title_AddTitle(nId, 0, 9999999);
    Title_ActiveTitle(nId);
    TX_Back("Da cap va bat "..nId.." "..Title_GetTitleName(nId).."\n(bang skill = 0 nen khong co buff - dung du lieu hien tai)", "TDT_Menu");
end

function TDT_ActGo(nId, b, c, d, e)
    if (TX_Guard() == 0) then
        return
    end
    Title_ActiveTitle(nId);
    SetTask(1122, nId);
    -- Tat: chi go chu tren dau khi dang deo dung mot danh hieu Thai Thu
    -- (168..174), de khong cuop mat danh hieu cua he khac.
    local nCur = TDT_Jx1Cur();
    if (nId == 0 and nCur >= 168 and nCur <= 174) then
        RemovePlayerTitle();
    end
    TX_Back("JX2 dang bat: "..Title_GetActiveTitle().."\nJX1 tren dau: "..TDT_Jx1Cur(), "TDT_Menu");
end

function TDT_RmGo(nId, b, c, d, e)
    if (TX_Guard() == 0) then
        return
    end
    Title_RemoveTitle(nId);
    local nCur = TDT_Jx1Cur();
    if (nCur >= 168 and nCur <= 174) then
        RemovePlayerTitle();
    end
    TX_Back("Da go "..nId..". Bam Danh hieu cua TA de kiem.", "TDT_Menu");
end

function TDT_Orig(a, b, c, d, e)
    if (TX_Guard() == 0) then
        return
    end
    Include("\\script\\global\\titlefuncs.lua");
    change_title();
end

function TDT_TaxSee(a, b, c, d, e)
    local s = "Thue (dat 22h-23h, 1 lan moi ngay, tran 20)\n";
    for i = 1, 4 do
        s = s..i.." "..GetCityAreaName(i).." = "..CTC_JX2_GetTax(i).."\n";
    end
    Say(s, 2, "Thue thanh 5-7/TDT_TaxSee2", "Tro ve/TDT_Menu");
end

function TDT_TaxSee2(a, b, c, d, e)
    local s = "";
    for i = 5, 7 do
        s = s..i.." "..GetCityAreaName(i).." = "..CTC_JX2_GetTax(i).."\n";
    end
    Say(s, 2, "Thue thanh 1-4/TDT_TaxSee", "Tro ve/TDT_Menu");
end

function TDT_TaxMenu(a, b, c, d, e)
    local n = GetCityArea();
    if (n < 1 or n > 7) then
        TX_Back("Phai dung trong thanh muon dat thue.", "TDT_Menu");
        return
    end
    local o, m = GetCityOwner(n);
    Say("Thanh "..GetCityAreaName(n).." - thue dang la "..CTC_JX2_GetTax(n).."\nBang chu: "..o.."\nThai Thu: "..m.."\nBan la: "..GetName().." - gio may chu "..GetLocalDate("%H"), 5,
        "Dat 0 phan tram/#TDT_TaxGo("..n..",0)",
        "Dat 5 phan tram/#TDT_TaxGo("..n..",5)",
        "Dat 15 phan tram/#TDT_TaxGo("..n..",15)",
        "Dat 20 phan tram/#TDT_TaxGo("..n..",20)",
        "Tro ve/TDT_Menu");
end

function TDT_TaxGo(n, v, c, d, e)
    if (TX_Guard() == 0) then
        return
    end
    local nRet = CTC_JX2_SetTax(n, v);
    local s = "Ma tra "..nRet.." - ";
    if (nRet == 0) then
        s = s.."OK, thue = "..CTC_JX2_GetTax(n);
    elseif (nRet == 1) then
        s = s.."ban KHONG phai Thai Thu thanh nay\n(Thai Thu rong la nguyen nhan pho bien nhat)";
    elseif (nRet == 2) then
        s = s.."ngoai khung gio 22h-23h";
    elseif (nRet == 3) then
        s = s.."hom nay da dat thue roi";
    else
        s = s.."tham so sai (thue vuot tran 20?)";
    end
    TX_Back(s, "TDT_Menu");
end

--==================== 7. TIEN ICH + HA TANG ====================

function TIT_Menu(a, b, c, d, e)
    Say("TIEN ICH + HA TANG", 6,
        "KIEM HA TANG - chi doc/TIT_Infra",
        "Di chuyen nhanh/TIT_GoMenu",
        "Dang o dau/TIT_Where",
        "Test kho C++ (E2)/TIT_Store",
        "Sang trang 2/TIT_Menu2",
        "Tro ve/TX_Root2");
end

function TIT_Menu2(a, b, c, d, e)
    Say("TIEN ICH - trang 2 (don dep sau khi test xong)", 4,
        "Phat Khieu chien lenh/TIT_Lenh",
        "RESET 7 thanh ve vo chu/TIT_Reset",
        "Nap lai file test/#TIT_Reload(1)",
        "Ve trang 1/TIT_Menu");
end

function TIT_Reload(a, b, c, d, e)
    ReLoadScript("\\script\\test\\bangthanh_f.lua");
    TX_Back("Da nap lai bangthanh_f.lua.\nSoi console GameServer xem co dong loi nao khong.", "TIT_Menu2");
end

-- SetViewTongOwnCity KHONG go danh hieu Thai Thu cu va KHONG xoa field 48
-- cua bang chu cu (khac han duong AppointViceroy) - phai noi ro cho nguoi test.
function TIT_Reset(a, b, c, d, e)
    local s = "Xoa chu cua CA 7 THANH. Chu hien tai:\n";
    for i = 1, 4 do
        local o = GetCityOwner(i);
        if (o ~= "") then
            s = s..i.." "..o.."\n";
        end
    end
    s = s.."KHONG thu hoi duoc danh hieu Thai Thu cua nguoi khac,\ncung KHONG xoa field 48 cua cac bang chu cu.";
    Say(s, 2, "Dong y reset 7 thanh/#TIT_RstGo(1)", "Thoi/TIT_Menu2");
end

function TIT_RstGo(a, b, c, d, e)
    if (TX_GuardAll() == 0) then
        return
    end
    for i = 1, 7 do
        SetViewTongOwnCity("", i);
        CTC_JX2_SetCityState(i, 0);
    end
    TX_Back("Da reset 7 thanh ve vo chu.\nBam 2 Cong thanh - Xem 7 thanh de kiem.", "TIT_Menu2");
end

-- CHI DOC. Khong duoc goi bat ky ham Apply nao o day: nut nay duoc bam
-- DAU TIEN va co the dang dung tren bang that cua nguoi choi.
function TIT_Infra(a, b, c, d, e)
    local s = "";
    if (TWS_ApplyMaintain == nil or TWS_ApplyUse == nil or TONG_GetName == nil) then
        s = s.."ham tac phuong: THIEU DANG KY\n";
    else
        s = s.."ham tac phuong: co day du\n";
    end
    local nMap = 0;
    for i = 586, 597 do
        if (i ~= 592 and SubWorldID2Idx(i) >= 0) then
            nMap = nMap + 1;
        end
    end
    s = s.."map lanh dia nap: "..nMap.." tren 11\n";
    s = s.."league 508 thanh 1: "..LG_GetLeagueObj(508, GetCityAreaName(1)).."\n";
    if (TB_CTC6 == nil) then
        s = s.."lich cong thanh TB_CTC6: KHONG CO\n";
    else
        s = s.."lich cong thanh TB_CTC6: CO\n";
    end
    local w, x, y = GetWorldPos();
    s = s.."Dang o map "..w.." toa do "..x..","..y;
    TX_Back(s, "TIT_Menu");
end

-- Khieu chien lenh KHONG phai vat pham test: doi duoc 50.000 exp moi cai
-- (infocenter_head.lua:266) va so luong nop quyet dinh bang nao gianh duoc
-- suat khieu chien thanh (checkFirstSignUpChallenger). Chi phat 1 cai/lan.
function TIT_Lenh(a, b, c, d, e)
    Say("Khieu chien lenh la vat pham CO GIA TRI THAT:\nmoi cai doi duoc 50.000 diem kinh nghiem, va so luong nop\nquyet dinh bang nao duoc di khieu chien thanh.\nChi phat 1 cai moi lan bam.", 2,
        "Dong y phat 1 cai/#TIT_LenhGo(1)",
        "Thoi/TIT_Menu");
end

function TIT_LenhGo(nCnt, b, c, d, e)
    if (TX_Guard() == 0) then
        return
    end
    AddItem(6, 1, 1508, 1, 0, 0, 0);
    TX_Back("Da phat 1 Khieu chien lenh (6,1,1508).\nMang den NPC Su Gia Cong Thanh o Ba Lang Huyen de nop.", "TIT_Menu");
end

function TIT_GoMenu(a, b, c, d, e)
    Say("Di chuyen nhanh:", 6,
        "53 Ba Lang - NPC Su Gia/#TIT_Go(53)",
        "78 Tuong Duong - bia thue/#TIT_Go(78)",
        "221 Chien truong/#TIT_Go(221)",
        "222 Cong vao phe THU/#TIT_Go(222)",
        "Vao lanh dia bang/TLD_Go",
        "Tro ve/TIT_Menu");
end

-- Toa do lay tu script goc: 53 = quanly.lua:106, 78 = phantang.lua:224,
-- 221/222/223 = diem vao chinh thuc cua chien truong.
function TIT_Go(w, b, c, d, e)
    if (SubWorldID2Idx(w) < 0) then
        TX_Back("Map "..w.." chua nap tren server.", "TIT_Menu");
        return
    end
    local nRet = 0;
    if (w == 53) then
        nRet = NewWorld(53, 1581, 3242);
    elseif (w == 78) then
        nRet = NewWorld(78, 1592, 3377);
    else
        nRet = NewWorld(w, 1614, 3172);
    end
    if (nRet ~= nil and nRet <= 0) then
        TX_Back("Dich chuyen HONG (NewWorld tra "..nRet..").", "TIT_Menu");
    end
end

function TIT_Where(a, b, c, d, e)
    local w, x, y = GetWorldPos();
    local s = "map id: "..w.." (SubWorld idx "..SubWorld..")\n";
    s = s.."Toa do: "..x..","..y.."\n";
    s = s.."GetCityArea: "..GetCityArea();
    TX_Back(s, "TIT_Menu");
end

function TIT_Store(a, b, c, d, e)
    if (TX_Guard() == 0) then
        return
    end
    SetGlbValue(1999, 12345);
    local a1 = GetGlbValue(1999);
    if (a1 == nil) then
        a1 = "NIL";
    end
    local h = OB_Create();
    local b1 = "NIL";
    if (h ~= nil and h ~= 0) then
        OB_PushInt(h, 777);
        local v = OB_PopInt(h);
        if (v ~= nil) then
            b1 = v;
        end
        OB_Release(h);
    end
    local n1, v1 = Ladder_GetLadderInfo(10261, 1);
    local s = "GlbValue(1999) = "..a1.." (cho 12345)\n";
    s = s.."OB pop = "..b1.." (cho 777)\n";
    s = s.."Ladder 10261 top1: "..n1.." / "..v1.."\n";
    s = s.."League 508 lid: "..LG_GetLeagueObj(508, GetCityAreaName(1));
    TX_Back(s, "TIT_Menu");
end
