-- ===========================================================================
-- schema_tracuu.sql -- Cac bang TRA CUU (chi doc) sinh ra tu du lieu goc.
--
-- NGUYEN TAC AN TOAN (doc ky truoc khi sua):
--   * Day la cac bang DAN XUAT. Game KHONG doc, KHONG ghi chung.
--     Game van doc/ghi `jx1_role`.`role`.`role_blob` y nhu truoc.
--   * Vi vay: xoa het cac bang nay roi sinh lai luc nao cung duoc, khong
--     anh huong gi toi nguoi choi. Nguoc lai, SUA du lieu trong cac bang nay
--     KHONG lam doi gi trong game.
--   * Moi chuoi tieng Viet giu HAI cot:
--        <ten>      VARBINARY  -- byte GOC (TCVN3), khong mat mat
--        <ten>_vn   VARCHAR utf8mb4 -- ban doc duoc trong Navicat
--     Cot _vn chi de nguoi doc; khi doi chieu voi game phai dung cot VARBINARY.
--
-- Tao: 20/08/2026
-- ===========================================================================

-- ---------------------------------------------------------------------------
-- 1. jx1_role.role_item -- moi VAT PHAM mot dong (bung ra tu role_blob)
--    Nguon: vung vat pham trong blob, moi mon la TDBItemData 233 byte
--    (Lib/S3DBInterface.h:177-226). Bo cuc byte da kiem: 229+4 = 233.
-- ---------------------------------------------------------------------------
USE `jx1_role`;

DROP TABLE IF EXISTS `role_item`;
CREATE TABLE `role_item` (
  `id`                BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `role_name`         VARBINARY(32)  NOT NULL COMMENT 'chu so huu (khoa cua bang role)',
  `role_name_vn`      VARCHAR(48)         NULL COMMENT 'ban doc duoc',
  `acc_name`          VARBINARY(32)       NULL COMMENT 'tai khoan',
  `stt`               SMALLINT       NOT NULL COMMENT 'thu tu mon trong blob (0..n-1)',

  -- vi tri: ilocal = nPlace, enum ITEM_POSITION (Core/Src/GameDataDef.h:316)
  `vi_tri`            INT            NOT NULL COMMENT '2=mang tren nguoi 3=hanh trang 4=ruong 5..7=ruong mo rong',
  `vi_tri_ten`        VARCHAR(32)         NULL COMMENT 'nhan de doc cua vi_tri',
  `o_x`               INT            NOT NULL,
  `o_y`               INT            NOT NULL,

  -- dinh danh vat pham
  `iid`               INT            NOT NULL,
  `equip_class_code`  INT            NOT NULL,
  `equip_code`        INT            NOT NULL,
  `detail_type`       INT            NOT NULL,
  `particular_type`   INT            NOT NULL,
  `cap`               INT            NOT NULL COMMENT 'ilevel',
  `he`                INT            NOT NULL COMMENT 'iseries (-1 = khong he)',
  `equip_version`     INT            NOT NULL,
  `rand_seed`         INT            NOT NULL,
  `param`             VARBINARY(64)  NOT NULL COMMENT 'iparam[16] tho -- cap kham nam',

  -- thuoc tinh
  `may_man`           INT            NOT NULL COMMENT 'ilucky',
  `giam_dinh`         INT            NOT NULL COMMENT 'iidentify',
  `do_ben`            INT            NOT NULL COMMENT 'idurability',
  `gold_id`           INT            NOT NULL,
  `so_chong`          INT            NOT NULL COMMENT 'istacknum',
  `cuong_hoa`         INT            NOT NULL COMMENT 'ienchance',
  `diem`              INT            NOT NULL COMMENT 'ipoint',
  `nam`               INT            NOT NULL,
  `thang`             INT            NOT NULL,
  `ngay`              INT            NOT NULL,
  `gio`               INT            NOT NULL,

  -- khoa / bay ban
  `khoa_bh`           INT            NOT NULL COMMENT 'ilockbh -- khoa bao hiem',
  `gio_mo_khoa`       INT            NOT NULL COMMENT 'igiomokhoa',
  `gia_bay_ban`       INT            NOT NULL COMMENT 'iBaiTanPrice -- gia dang bay tren sap, 0 = khong ban',
  `khoa_ban`          INT            NOT NULL COMMENT 'ilocksell',
  `khoa_trao`         INT            NOT NULL COMMENT 'ilocktrade',
  `khoa_vut`          INT            NOT NULL COMMENT 'ilockdrop',

  `hinh_thoi_trang`   TINYINT UNSIGNED NOT NULL COMMENT 'imantle',
  `van_may`           INT            NOT NULL COMMENT 'ifortune',
  `chu_so_huu`        INT UNSIGNED   NOT NULL COMMENT 'iowner',
  `hang`              INT            NOT NULL COMMENT 'irow',
  `equip_nature_code` INT            NOT NULL,

  `sinh_luc`          DATETIME       NOT NULL COMMENT 'thoi diem bang nay duoc sinh lai',
  PRIMARY KEY (`id`),
  UNIQUE  KEY `uq_nv_stt`   (`role_name`, `stt`),
  KEY     `idx_nv`          (`role_name`),
  KEY     `idx_vitri`       (`vi_tri`),
  KEY     `idx_loai`        (`detail_type`, `particular_type`),
  KEY     `idx_bayban`      (`gia_bay_ban`),
  KEY     `idx_goldid`      (`gold_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
  COMMENT='DAN XUAT tu role.role_blob -- game KHONG doc bang nay';


-- ---------------------------------------------------------------------------
-- 2. jx1_game.giao_dich -- GIAO DICH giua hai nguoi choi
--    Nguon: bin/server/dulieu/player_log/log_trade/*.txt
--    Moi ban ghi bat dau bang dong "*****Start Log Trade*****", roi:
--      HH:MM:SS \t Account chu: .. \t PlayerName chu: .. \t IP chu: ..
--               \t Account khach: .. \t PlayerName Khach: .. \t IP khach: ..
--               \t Vi tri giao dich (x/y) TenBanDo
--               \t Tien giao dich [chu nhan: N khach nhan: M]
--               \t [Item khach nhan:] \t [Ten] \t [nGoldId: ..] ...
-- ---------------------------------------------------------------------------
USE `jx1_game`;

DROP TABLE IF EXISTS `giao_dich_item`;
DROP TABLE IF EXISTS `giao_dich`;

CREATE TABLE `giao_dich` (
  `id`              BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `at`              DATETIME            NULL COMMENT 'ngay lay tu ten tep, gio lay trong dong',
  `ngay`            DATE                NULL,
  `acc_chu`         VARBINARY(64)       NULL,
  `ten_chu`         VARBINARY(64)       NULL,
  `ten_chu_vn`      VARCHAR(96)         NULL,
  `ip_chu`          VARBINARY(64)       NULL,
  `acc_khach`       VARBINARY(64)       NULL,
  `ten_khach`       VARBINARY(64)       NULL,
  `ten_khach_vn`    VARCHAR(96)         NULL,
  `ip_khach`        VARBINARY(64)       NULL,
  `vi_tri_x`        INT                 NULL,
  `vi_tri_y`        INT                 NULL,
  `ban_do`          VARBINARY(64)       NULL,
  `ban_do_vn`       VARCHAR(96)         NULL,
  `tien_chu_nhan`   BIGINT              NULL,
  `tien_khach_nhan` BIGINT              NULL,
  `nguon_tep`       VARCHAR(64)         NULL COMMENT 'ten tep nhat ky goc',
  `dong_goc`        INT                 NULL COMMENT 'so dong trong tep goc',
  PRIMARY KEY (`id`),
  KEY `idx_at`      (`at`),
  KEY `idx_chu`     (`ten_chu`),
  KEY `idx_khach`   (`ten_khach`),
  KEY `idx_acc_chu` (`acc_chu`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
  COMMENT='DAN XUAT tu log_trade/*.txt';

CREATE TABLE `giao_dich_item` (
  `id`            BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `giao_dich_id`  BIGINT UNSIGNED NOT NULL,
  `ben_nhan`      VARCHAR(8)      NOT NULL COMMENT 'khach | chu',
  `ten_item`      VARBINARY(96)       NULL,
  `ten_item_vn`   VARCHAR(144)        NULL,
  `gold_id`       INT                 NULL,
  `gen`           INT                 NULL,
  `detail_type`   INT                 NULL,
  `particular`    INT                 NULL,
  `cap`           INT                 NULL,
  `he`            INT                 NULL,
  `options`       VARCHAR(128)        NULL COMMENT 'chuoi [a|b] noi lai',
  PRIMARY KEY (`id`),
  KEY `idx_gd`   (`giao_dich_id`),
  KEY `idx_ten`  (`ten_item`),
  CONSTRAINT `fk_gd` FOREIGN KEY (`giao_dich_id`)
      REFERENCES `giao_dich` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
  COMMENT='cac mon trong tung giao dich';


-- ---------------------------------------------------------------------------
-- 3. jx1_game.mua_shop -- MUA DO O SHOP (Ky Tran Cac)
--    Nguon: bin/server/dulieu/player_log/log_ktc/*.txt
--      HH:MM:SS \t Tai khoan: .. \t Ten: .. \t IP: ..
--               \t [nKindShop: N] \t [TenMon] \t [Number: N] \t [nGoldId: ..]
--               \t [nGen: ..] [nDetail: ..] [nParti: ..] [nLevel: ..]
--               \t [nSeries: ..] [nLine: ..] [Xu: ..] [Tong tien van: ..]
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `mua_shop`;
CREATE TABLE `mua_shop` (
  `id`            BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `at`            DATETIME            NULL,
  `ngay`          DATE                NULL,
  `acc_name`      VARBINARY(64)       NULL,
  `role_name`     VARBINARY(64)       NULL,
  `role_name_vn`  VARCHAR(96)         NULL,
  `ip`            VARBINARY(64)       NULL,
  `kind_shop`     INT                 NULL COMMENT 'nKindShop -- loai quay hang',
  `ten_item`      VARBINARY(96)       NULL,
  `ten_item_vn`   VARCHAR(144)        NULL,
  `so_luong`      INT                 NULL COMMENT 'Number',
  `gold_id`       INT                 NULL,
  `gen`           INT                 NULL,
  `detail_type`   INT                 NULL,
  `particular`    INT                 NULL,
  `cap`           INT                 NULL,
  `he`            INT                 NULL,
  `line`          INT                 NULL,
  `xu`            BIGINT              NULL COMMENT 'so Xu tra',
  `tong_tien_van` BIGINT              NULL COMMENT 'Tong tien van con lai',
  `nguon_tep`     VARCHAR(64)         NULL,
  `dong_goc`      INT                 NULL,
  PRIMARY KEY (`id`),
  KEY `idx_at`    (`at`),
  KEY `idx_nv`    (`role_name`),
  KEY `idx_acc`   (`acc_name`),
  KEY `idx_item`  (`ten_item`),
  KEY `idx_shop`  (`kind_shop`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
  COMMENT='DAN XUAT tu log_ktc/*.txt';
