-- =====================================================================
--  jx1_game -- toan bo du lieu TINH NANG va NHAT KY cua CoreServer (x64)
--  Sinh ngay 20/08/2026. MySQL 5.7.44 / InnoDB.
--
--  Thay cho cac tep phang hien nay:
--    baucua\deposits.json                  -> baucua_balance + baucua_ledger
--    dulieu\giftcode\*_used.txt + 2 .lua   -> giftcode
--    settings\jx2citywar/ladder/league.txt -> game_kv (ns='jx2.*')
--    StatData.dat                          -> game_kv (ns='statdata')
--    ~10 tep .lua may chu tu ghi de        -> game_kv (moi tinh nang mot ns)
--    dulieu\player_log\**, baucua\game_log -> game_log
--    dulieu\username_kick.txt              -> admin_command
--
--  NGUYEN TAC:
--   * Ten nhan vat la byte TCVN3 tho -> VARBINARY, charset binary. Dung collation
--     _ci se lam "CayTien" va "caytien" thanh mot nguoi (da do: mat 27 nhan vat).
--   * Moi cot TIEN la BIGINT CO DAU. Khong UNSIGNED -- do la goc su co
--     "quy bang tran so am -> 4,29 ty".
--   * Bang nhat ky PHAN VUNG THEO THANG de xoa du lieu cu bang DROP PARTITION
--     (tuc thi) thay vi DELETE (khoa bang rat lau).
-- =====================================================================

CREATE DATABASE IF NOT EXISTS `jx1_game` DEFAULT CHARACTER SET binary;
USE `jx1_game`;

-- ---------------------------------------------------------------------
--  game_kv -- kho khoa/gia tri tong quat.
--  Thay cho: jx2citywar.txt, jx2ladder.txt, jx2league.txt, StatData.dat,
--  va ~10 tep .lua ma may chu tu ghi de.
--  Ly do dung mot bang chung: cach ghi cu (`openfile(f,"w+")` trong
--  script\lib\lib_ham.lua) la CAT TRANG roi ghi lai, khong nguyen tu --
--  mat dien giua chung la mat CA BANG. Mot dong UPDATE thi nguyen tu san.
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `game_kv` (
  `ns`         VARCHAR(64)   NOT NULL COMMENT 'vd: jx2.citywar, songbac.nganluong, statdata',
  `k`          VARBINARY(128) NOT NULL,
  `v`          LONGBLOB      NOT NULL COMMENT 'byte tho -- co the la text, JSON, hoac nhi phan',
  `v_len`      INT UNSIGNED  NOT NULL,
  `updated_at` TIMESTAMP     NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`ns`, `k`),
  KEY `idx_ns_time` (`ns`, `updated_at`)
) ENGINE=InnoDB DEFAULT CHARSET=binary ROW_FORMAT=DYNAMIC;

-- Nhat ky moi lan ghi de mot khoa -- de quay lui khi mot tinh nang ghi hong.
CREATE TABLE IF NOT EXISTS `game_kv_history` (
  `id`        BIGINT AUTO_INCREMENT PRIMARY KEY,
  `ns`        VARCHAR(64)    NOT NULL,
  `k`         VARBINARY(128) NOT NULL,
  `v`         LONGBLOB       NOT NULL,
  `saved_at`  DATETIME       NOT NULL,
  KEY `idx_ns_k_id` (`ns`, `k`, `id`)
) ENGINE=InnoDB DEFAULT CHARSET=binary ROW_FORMAT=DYNAMIC;

-- ---------------------------------------------------------------------
--  giftcode -- gop 4 nguon hien tai lam mot.
--  Hien nay viec "danh dau da dung" la APPEND vao tep text, khong nguyen tu:
--  hai nguoi nhan cung mot ma cung luc thi CA HAI deu duoc. O day mot cau
--  UPDATE ... WHERE used_at IS NULL la du -- kiem affected_rows == 1.
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `giftcode` (
  `code`         VARBINARY(64) NOT NULL PRIMARY KEY,
  `batch`        VARCHAR(24)   NOT NULL COMMENT 'new | tuan | codenew | fancung',
  `payload`      VARBINARY(255)    NULL COMMENT 'phan thuong, neu nguon co ghi',
  `used_by_role` VARBINARY(32)     NULL COMMENT 'HIEN KHONG CO -- bo sung de truy vet duoc',
  `used_at`      DATETIME          NULL,
  `created_at`   DATETIME      NOT NULL,
  KEY `idx_batch_used` (`batch`, `used_at`),
  KEY `idx_role` (`used_by_role`)
) ENGINE=InnoDB DEFAULT CHARSET=binary;

-- ---------------------------------------------------------------------
--  Tien Xu song bac
--  Khoa la TEN NHAN VAT tho (khong phai base64 -- base64 chi la cach ma hoa
--  cua tep JSON cu).
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `baucua_balance` (
  `role_name`  VARBINARY(32) NOT NULL PRIMARY KEY,
  `balance`    BIGINT        NOT NULL DEFAULT 0 COMMENT 'CO DAU',
  `locked`     BIGINT        NOT NULL DEFAULT 0 COMMENT 'dang ket trong cuoc',
  `updated_at` TIMESTAMP     NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=binary;

-- So cai: MOI lan so du doi deu co mot dong. Mat bang tren van dung lai duoc.
CREATE TABLE IF NOT EXISTS `baucua_ledger` (
  `id`         BIGINT AUTO_INCREMENT PRIMARY KEY,
  `role_name`  VARBINARY(32) NOT NULL,
  `viec`       VARCHAR(24)   NOT NULL COMMENT 'gui | rut | cuoc | thang | thua | huy',
  `so_tien`    BIGINT        NOT NULL COMMENT 'CO DAU: am la tru',
  `so_du_sau`  BIGINT        NOT NULL,
  `at`         DATETIME      NOT NULL,
  KEY `idx_role_time` (`role_name`, `at`),
  KEY `idx_time` (`at`)
) ENGINE=InnoDB DEFAULT CHARSET=binary;

-- ---------------------------------------------------------------------
--  game_log -- gop toan bo nhat ky nguoi choi.
--  PHAN VUNG THEO THANG: xoa du lieu cu bang ALTER TABLE ... DROP PARTITION
--  (tuc thi, khong khoa bang) thay vi DELETE hang trieu dong.
--  LUU Y: MySQL doi hoi moi khoa duy nhat phai chua cot phan vung, nen
--  khoa chinh la (id, ngay) chu khong phai (id).
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `game_log` (
  `id`        BIGINT       NOT NULL AUTO_INCREMENT,
  `ngay`      DATE         NOT NULL,
  `loai`      VARCHAR(32)  NOT NULL COMMENT 'vd: baucua, vutdo, boss, giftcode, kick',
  `role_name` VARBINARY(32)    NULL,
  `noi_dung`  VARBINARY(1024) NOT NULL,
  `at`        DATETIME     NOT NULL,
  PRIMARY KEY (`id`, `ngay`),
  KEY `idx_loai_ngay` (`loai`, `ngay`),
  KEY `idx_role_ngay` (`role_name`, `ngay`)
) ENGINE=InnoDB DEFAULT CHARSET=binary ROW_FORMAT=DYNAMIC
PARTITION BY RANGE (TO_DAYS(`ngay`)) (
  PARTITION p2026_08 VALUES LESS THAN (TO_DAYS('2026-09-01')),
  PARTITION p2026_09 VALUES LESS THAN (TO_DAYS('2026-10-01')),
  PARTITION p2026_10 VALUES LESS THAN (TO_DAYS('2026-11-01')),
  PARTITION p2026_11 VALUES LESS THAN (TO_DAYS('2026-12-01')),
  PARTITION p2026_12 VALUES LESS THAN (TO_DAYS('2027-01-01')),
  PARTITION pmax     VALUES LESS THAN MAXVALUE
);

-- ---------------------------------------------------------------------
--  admin_command -- thay dulieu\username_kick.txt
--  Web quan tri ghi vao day; timerserver.lua doc roi danh dau da xu ly.
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `admin_command` (
  `id`         BIGINT AUTO_INCREMENT PRIMARY KEY,
  `cmd`        VARCHAR(32)   NOT NULL COMMENT 'kick | ban | thongbao',
  `arg`        VARBINARY(255) NOT NULL,
  `created_at` DATETIME      NOT NULL,
  `done_at`    DATETIME          NULL,
  KEY `idx_cho` (`done_at`, `id`)
) ENGINE=InnoDB DEFAULT CHARSET=binary;

-- ---------------------------------------------------------------------
--  migrate_audit_game -- nhat ky moi lan chay cong cu di tru
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `migrate_audit_game` (
  `id`          BIGINT AUTO_INCREMENT PRIMARY KEY,
  `run_at`      DATETIME     NOT NULL,
  `phan`        VARCHAR(48)  NOT NULL,
  `nguon`       VARCHAR(512) NOT NULL,
  `nguon_sha256` CHAR(64)        NULL,
  `n_doc`       INT          NOT NULL,
  `n_loai`      INT          NOT NULL,
  `n_nhap`      INT          NOT NULL,
  `n_kiem`      INT          NOT NULL,
  `ghi_chu`     TEXT             NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
