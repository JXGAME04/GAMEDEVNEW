-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local inssort
-- 价值量脚本头文件
-- Fanghao_Wu 2005.1.12

-- 功能：物品价值量概率转移函数
-- 参数：dSrcItemVal 源物品价值量，arydDesItemVal 目标物品价值量数组
-- 返回：选中的目标物品索引，0表示出错或者没有选中任何一个目标物品
function TransItemVal( dSrcItemVal, arydDesItemVal )
	if( dSrcItemVal <= 0 or getn( arydDesItemVal ) <= 0 ) then
		return 0;
	end
	
	local nDesCount = getn( arydDesItemVal );
	local dRandNum = random();
	local dProbSum = 0;
	local dCurProb = 0;
	local arydDesItemValSort = {};
	local arynDesItemValIdx = {};
	local arydDesItemValTemp = {};
	local arydDesProb = {};
	local nSelDesItemIdx;
	local dSelDesItemProb;
	
	for i = 1, nDesCount do
		arydDesItemValSort[i] = arydDesItemVal[i];
		arydDesItemValTemp[i] = arydDesItemVal[i];
		arydDesProb[i] = 0;	-- [LOREN 27/08] KHOI PHUC LOP CHON THUAT TOAN: ban Linux khoi tao 0
	end
	inssort( arydDesItemValSort, 1, nDesCount );
	for i = 1, nDesCount do
		for j = 1, nDesCount do
			if( arydDesItemValSort[i] == arydDesItemValTemp[j] ) then
				arynDesItemValIdx[i] = j;
				arydDesItemValTemp[j] = -1;
				break;
			end
		end
	end
	transItemValImpl( dSrcItemVal, arydDesItemValSort, arydDesProb );
	for i = 1, nDesCount do
		dProbSum = dProbSum + arydDesProb[i];
		if( dRandNum < dProbSum ) then
			nSelDesItemIdx = arynDesItemValIdx[i];
			dSelDesItemProb = arydDesProb[i];
			break;
		end
	end

	local fileLog = openfile( "./Logs/KSG_CompoundLog_Prob.txt", "a+" );
	local dProbSum = 0;
	local dGenVal = 0;
--	print( format( "SrcVal\t%0.0f", dSrcItemVal ) );
--	Msg2Player( format( "SrcVal\t%0.0f", dSrcItemVal ) );
	write( fileLog, format( "SrcVal\t%0.0f\r\n", dSrcItemVal ) );
	for i = 1, nDesCount do
--		print( format( "DesVal\t%0.0f\t%0.4f%%", arydDesItemVal[arynDesItemValIdx[i]], arydDesProb[i] * 100 ) );
--		Msg2Player( format( "DesVal\t%0.0f\t%0.4f%%", arydDesItemVal[arynDesItemValIdx[i]], arydDesProb[i] * 100 ) );
		write( fileLog, format( "DesVal\t%0.0f\t%0.4f%%\r\n", arydDesItemVal[arynDesItemValIdx[i]], arydDesProb[i] * 100 ) );
		dProbSum = dProbSum + arydDesProb[i];
		dGenVal = dGenVal + arydDesItemVal[arynDesItemValIdx[i]] * arydDesProb[i];
	end
--	Msg2Player( "ProbSum:"..dProbSum );
--	Msg2Player( "GenVal:"..dGenVal );
	if( nSelDesItemIdx ~= nil ) then
		return nSelDesItemIdx, arydDesItemVal[nSelDesItemIdx], dSelDesItemProb;
	else
		return -1, 0, 1 - dProbSum;
	end
end

-- [LOREN 27/08] VONG LAP thay cho DE QUY.
-- Ngan xep Lua cua du an chi 128 o (LuaLib\src\llimits.h:74 -
-- "#define DEFAULT_STACK_SIZE 128//1024", da ha tu 1024). Ban de quy chi can
-- ~8 tang la vuot => luaD_checkstack nem "stack Overflow" => kich ban chet,
-- nguoi choi thay "Lo ren gap loi khong ro".
-- Thuat toan va ket qua GIU NGUYEN tung buoc; chi thay hai lenh goi de quy
-- bang hai lenh day vao ngan xep tu quan (ba bang chay song song).
function calcProbLoop( arydValue, arydProb, nStartIdx, nEndIdx, dDivVal )
	local aryS = {};
	local aryE = {};
	local aryD = {};
	local nTop = 1;
	aryS[1] = nStartIdx;
	aryE[1] = nEndIdx;
	aryD[1] = dDivVal;

	while( nTop > 0 ) do
		local nS = aryS[nTop];
		local nE = aryE[nTop];
		local dD = aryD[nTop];
		nTop = nTop - 1;

		if( nS == nil or nE == nil or dD == nil or nS > nE ) then
			-- muc hong: bo qua, khong lam sap kich ban
		elseif( nS < nE and arydValue[nS] == arydValue[nE] and dD == arydValue[nS] ) then
			for i = nS, nE do
				arydProb[i] = arydProb[i] / ( nE - nS + 1 );
			end
		elseif( dD <= arydValue[nS] ) then
			local nEqualCount = 0;
			for i = nS, nE do
				if( arydValue[nS] == arydValue[i] ) then
					nEqualCount = nEqualCount + 1;
				else
					break;
				end
			end
			for i = nS, nS + nEqualCount - 1 do
				arydProb[i] = arydProb[i] * ( dD / arydValue[i] ) / nEqualCount;
			end
			for i = nS + nEqualCount, nE do
				arydProb[i] = 0;
			end
		elseif( dD >= arydValue[nE] ) then
			local nEqualCount = 0;
			for i = nE, nS, -1 do
				if( arydValue[nE] == arydValue[i] ) then
					nEqualCount = nEqualCount + 1;
				else
					break;
				end
			end
			for i = nS, nE - nEqualCount do
				arydProb[i] = 0;
			end
			for i = nE - nEqualCount + 1, nE do
				arydProb[i] = arydProb[i] / nEqualCount;
			end
		else
			local nDivIdx = nS;
			local dLessSum = 0;
			local dMoreSum = 0;

			for i = nS, nE do
				if( arydValue[i] <= dD ) then
					dLessSum = dLessSum + arydValue[i];
					nDivIdx = nDivIdx + 1;
				else
					dMoreSum = dMoreSum + arydValue[i];
				end
			end

			local dLessMean = ( dLessSum / (nDivIdx - nS) + arydValue[nDivIdx-1] ) / 2;
			local dMoreMean = ( dMoreSum / (nE - nDivIdx + 1) + arydValue[nDivIdx] ) / 2;
			local dLessProb = ( dMoreMean - dD ) / ( dMoreMean - dLessMean );
			local dMoreProb = 1 - dLessProb;

			for i = nS, nDivIdx - 1 do
				arydProb[i] = arydProb[i] * dLessProb;
			end
			for i = nDivIdx, nE do
				arydProb[i] = arydProb[i] * dMoreProb;
			end

			if( nS < nDivIdx - 1 ) then
				nTop = nTop + 1;
				aryS[nTop] = nS;
				aryE[nTop] = nDivIdx - 1;
				aryD[nTop] = dLessMean;
			end
			if( nDivIdx < nE ) then
				nTop = nTop + 1;
				aryS[nTop] = nDivIdx;
				aryE[nTop] = nE;
				aryD[nTop] = dMoreMean;
			end
		end
	end
end
-- [LOREN 27/08] KHOI PHUC LOP CHON THUAT TOAN (port nguyen van tu ban Linux).
-- Ban Linux goi qua BIEN `transItemValImpl` roi cuoi tep gan bien do bang
-- `_transItemValImpl_2` (itemvalue_header.lua:246 - chu thich goc ghi ro
-- "dung ham chuyen xac suat gia tri PHIEN BAN 2"). Ban JX1 truoc day bo lop
-- gian tiep nay va goi thang `calcProbLoop`, tuc ket cung o PHIEN BAN 1.
--
-- Khac nhau quyet dinh:
--   PB1 chia xac suat cho TOAN BO day cua theo ti le => cua RE NHAT luon
--       duoc chia mot phan. Trong kham nam, cua re nhat chinh la cua
--       "giu nguyen trang bi" = THAT BAI => khong bao gio ve 0%.
--   PB2 chi rai xac suat cho 4 CUA LAN CAN quanh gia tri nguyen lieu,
--       noi suy tuyen tinh (dHiProb = 1 - dLowProb) nen tong luon = 1.
--       Bo du nguyen lieu la cua that bai nam ngoai vung => DUNG 0%.
-- Do tren day cua that trong log: nguyen lieu 212.340.000 -> PB1 hong
-- 77,97% / PB2 hong 52,67%; tu 420.000.000 tro len PB2 hong 0,00%.

-- Phien ban 1 (2005.1.12): giu lai lam duong lui, khong xoa.
function _transItemValImpl_1( dSrcItemVal, arydDesItemValSort, arydDesItemProbSort )
	for i = 1, getn( arydDesItemValSort ) do
		arydDesItemProbSort[i] = 1;
	end
	calcProbLoop( arydDesItemValSort, arydDesItemProbSort, 1, getn( arydDesItemValSort ), dSrcItemVal );
end

LOW_NEAR_PERCENT	= 0.95;		-- phan xac suat cua cua THAP hon lien ke
HI_NEAR_PERCENT		= 0.95;		-- phan xac suat cua cua CAO hon lien ke

-- Phien ban 2 (ban Linux dong 185-241) - port 1:1, khong doi mot phep tinh.
function _transItemValImpl_2( dSrcItemVal, arydDesItemValSort, arydDesItemProbSort )
	-- tim cua dau tien co gia tri > gia tri nguyen lieu
	local nSrcItemValIdx = 0;
	for i = 1, getn( arydDesItemValSort ) do
		if( arydDesItemValSort[i] > dSrcItemVal ) then
			nSrcItemValIdx = i;
			break;
		end
	end

	-- nguyen lieu >= MOI cua: 100% trung cua CAO NHAT
	if( nSrcItemValIdx == 0 ) then
		arydDesItemProbSort[getn( arydDesItemValSort )] = 1;
		return
	end
	-- nguyen lieu < MOI cua: khong trung cua nao (tat ca giu 0%)
	if( nSrcItemValIdx == 1 ) then
		return
	end

	-- gia tri tron cua hai cua THAP
	local dLowValSum = 0;
	if( nSrcItemValIdx > 2 ) then
		dLowValSum = arydDesItemValSort[nSrcItemValIdx - 1] * LOW_NEAR_PERCENT +
				 arydDesItemValSort[nSrcItemValIdx - 2] * ( 1 - LOW_NEAR_PERCENT );
	else
		dLowValSum = arydDesItemValSort[1];
	end
	-- gia tri tron cua hai cua CAO
	local dHiValSum = 0;
	if( nSrcItemValIdx < getn( arydDesItemValSort ) ) then
		dHiValSum = arydDesItemValSort[nSrcItemValIdx] * HI_NEAR_PERCENT +
				 arydDesItemValSort[nSrcItemValIdx + 1] * ( 1 - HI_NEAR_PERCENT );
	else
		dHiValSum = arydDesItemValSort[getn( arydDesItemValSort )];
	end

	local dLowProb = ( dHiValSum - dSrcItemVal ) / ( dHiValSum - dLowValSum );
	local dHiProb = 1 - dLowProb;

	if( nSrcItemValIdx > 2 ) then
		arydDesItemProbSort[nSrcItemValIdx - 1] = dLowProb * LOW_NEAR_PERCENT;
		arydDesItemProbSort[nSrcItemValIdx - 2] = dLowProb * ( 1 - LOW_NEAR_PERCENT );
	else
		arydDesItemProbSort[1] = dLowProb;
	end
	if( nSrcItemValIdx < getn( arydDesItemValSort ) ) then
		arydDesItemProbSort[nSrcItemValIdx] = dHiProb * HI_NEAR_PERCENT;
		arydDesItemProbSort[nSrcItemValIdx + 1] = dHiProb * ( 1 - HI_NEAR_PERCENT );
	else
		arydDesItemProbSort[getn( arydDesItemValSort )] = dHiProb;
	end
end

-- Dung phien ban 2 (giong ban Linux dong 246).
transItemValImpl = _transItemValImpl_2;


-- 插入排序
function inssort( aryNumber, nStartIdx, nEndIdx )
	if( nStartIdx >= nEndIdx ) then
		return
	end
	for i = nStartIdx + 1, nEndIdx do
		for j = i, nStartIdx + 1, -1 do
			if( aryNumber[j] < aryNumber[j-1] ) then
				aryNumber[j-1], aryNumber[j] = aryNumber[j], aryNumber[j-1];
			end
		end
	end
end