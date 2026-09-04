-- auction_def.lua - [DAUGIA 04/09] tham so + hang so DAU GIA (dung chung may chu / client). Viet lai tu
-- \script\auction_house\auction_def.lua cua client JX1 cu (2.0). Chu chot 04/09: tien XU va NGAN LUONG.
AUCTION_DEF = {}
AUCTION_DEF.nAuctionTaxRate        = 5              -- thue giao dich (%) tru vao tien nguoi ban nhan
AUCTION_DEF.nPersonalPutOnCost     = 10             -- phi ky gui (%) thu ngay khi dat ban, khong hoan
-- [A14] moi trang 2 mon cho KHOP so hang thay duoc (AUCUI_ROW_COUNT = 2): khung danh sach cao 321,
-- moi hang cao 118 nen hang thu ba tran ra ngoai khung. (Bo dem ObjBuffer 4096 byte, moi dong ~600.)
AUCTION_DEF.nMaxItemPerPage        = 2
AUCTION_DEF.nMaxItemPerSeller      = 5              -- mot nguoi ky gui toi da
AUCTION_DEF.nPersonalDuration      = 24 * 60 * 60   -- ky gui 24 gio
-- [A14] Ky gui CUNG la dau gia kieu Ha Lan (dung ban goc 2.0: nPersonalFloatTimes = 1):
-- mo ban o 150% gia nguoi ban muon, sau nua thoi gian ha ve dung gia do.
-- Nho vay Mua ngay va Gia san khac nhau: ai muon chac thi mua ngay, ai chiu cho thi mua re.
AUCTION_DEF.nPersonalFloatTimes    = 1
AUCTION_DEF.nPersonalFloatInterval = 12 * 60 * 60   -- ha gia dung mot lan, o nua chang
AUCTION_DEF.nEnglishRemainingTime  = 30 * 60        -- kieu Anh: 30 phut
AUCTION_DEF.nRefreshRemingTimeWhenOffer = 60        -- kieu Anh: moi lan tra gia gia han them 60 giay
AUCTION_DEF.nDutchInitRate         = 1.5            -- kieu Ha Lan: gia mo = 150% gia bao dam
AUCTION_DEF.nDutchFloatTimes       = 8              -- giam 8 lan
AUCTION_DEF.nDutchFloatRange       = 0.1            -- moi lan 10%
AUCTION_DEF.nDutchFloatInterval    = 5 * 60         -- cach 5 phut
AUCTION_DEF.nMailDays              = 30             -- thu tra tien / do giu 30 ngay
AUCTION_DEF.XU_TASK                = 251            -- xu = o nhiem vu 251 (nhu hop thu)
AUCTION_DEF.tbAuctionTypeEnum = { eType_TONG = 1, eType_WORLD = 2, eType_PERSONAL = 3, eType_COUNT = 4 }
AUCTION_DEF.tbItemTypeEnum    = { eType_ENGLISH = 1, eType_DUTCH = 2 }
AUCTION_DEF.tbCurrency        = { MONEY = 1, XU = 2 }
AUCTION_DEF.tbCurrencyName    = { [1] = "Ng©n l­îng", [2] = "Xu" }
AUCTION_DEF.szPersonalActivity = "Ký göi"       -- ten phien duy nhat cua tab Ca nhan
AUCTION_DEF.szSender           = "Ch­ëng QuÇy Khu §Êu Gi¸"
