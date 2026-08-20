-- =====================================================================
--  jx1_role -- Schema MySQL cho kho nhan vat JX1 (Phuong an A: giu BLOB)
--  Sinh ngay 20/08/2026. Doi tuong: MySQL 5.7.44, InnoDB.
--
--  NGUYEN TAC BAT BUOC (deu da duoc do bang du lieu that, khong phong doan):
--   1. Moi cot chua TEN hoac du lieu nhi phan la VARBINARY/BLOB, charset binary.
--      Do that: 24 nhom ten va cham neu dung collation _ci -> MAT 26 nhan vat.
--      Ngoai le: ten TAI KHOAN ben DB `account` giu nguyen _ci nhu hien tai.
--   2. role_blob la MEDIUMBLOB. Ban ghi that do duoc 70.646 byte > 65.535.
--   3. role_blob luu NGUYEN VEN TUNG BYTE, KE CA 4 byte CRC32 cuoi.
--      KHONG tinh lai CRC khi nhap -- de con doi chieu hash 1-1 voi kho cu.
--      (82,4% ban ghi production co CRC sai san tu truoc, do CDBBackup::SaveStatInfo
--       ghi de nWorldStat/nSectStat ma khong tinh lai CRC. Loc theo CRC = mat 1.506 nv.)
--
--  Kieu cot chon theo bien do THAT do duoc tren kho production 1.816 nhan vat:
--      fight_exp  : co gia tri AM (-26.000.000) -> DOUBLE co dau
--      tong_id    : max 4.130.731.568 > INT co dau -> INT UNSIGNED
--      money+save_money : tong co the vuot INT -> BIGINT
-- =====================================================================

CREATE DATABASE IF NOT EXISTS `jx1_role`
  DEFAULT CHARACTER SET binary;

USE `jx1_role`;

-- ---------------------------------------------------------------------
--  role -- thay the Berkeley DB `roledb` + chi muc phu `roledb.0`
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `role` (
  -- ===== dinh danh + du lieu goc =====
  `role_name`   VARBINARY(32)     NOT NULL COMMENT 'BaseInfo.szName, KHONG gom byte NUL',
  `acc_name`    VARBINARY(32)     NOT NULL COMMENT 'BaseInfo.caccname nguyen ven',
  `data_len`    INT UNSIGNED      NOT NULL COMMENT 'TRoleData.dwDataLen, DA gom 4 byte CRC',
  `role_blob`   MEDIUMBLOB        NOT NULL COMMENT 'TRoleData nguyen ven tung byte',
  `crc32`       INT UNSIGNED      NOT NULL COMMENT '4 byte cuoi blob, chi de soi nhanh',
  `crc_ok`      TINYINT(1)        NOT NULL DEFAULT 0 COMMENT '1 = crc32 khop noi dung',
  `has_crc`     TINYINT(1)        NOT NULL DEFAULT 1
                COMMENT '1 = blob CO 4 byte CRC cuoi (da tung duoc GameServer luu). '
                        '0 = nhan vat MOI TAO: PlayerCreator.cpp:354 dat dwDataLen = '
                        '(BYTE*)pItemData - pData nen KHONG co CRC. Do that tren kho '
                        'production: 1816 co / 3 khong. Ep cung "luon co CRC" = MAT 3 nguoi choi.',
  `fmt_ver`     SMALLINT UNSIGNED NOT NULL DEFAULT 1 COMMENT 'dwVersion trong blob la truong CHET',

  -- ===== cot dan xuat: CHI de truy van/xep hang, KHONG phai nguon su that =====
  `sex`          TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `first_sect`   TINYINT UNSIGNED NOT NULL DEFAULT 255,
  `sect`         TINYINT UNSIGNED NOT NULL DEFAULT 255,
  `series`       INT              NOT NULL DEFAULT 0  COMMENT 'ifiveprop',
  `level`        INT              NOT NULL DEFAULT 0  COMMENT 'ifightlevel',
  `fight_exp`    DOUBLE           NOT NULL DEFAULT 0  COMMENT 'GOC LA double, co the AM',
  `lead_level`   INT              NOT NULL DEFAULT 0,
  `money`        BIGINT           NOT NULL DEFAULT 0  COMMENT 'imoney',
  `save_money`   BIGINT           NOT NULL DEFAULT 0  COMMENT 'isavemoney',
  `pk_value`     INT              NOT NULL DEFAULT 0,
  `kill_num`     INT              NOT NULL DEFAULT 0,
  `tong_id`      INT UNSIGNED     NOT NULL DEFAULT 0  COMMENT 'max that 4.130.731.568',
  `use_revive`   TINYINT UNSIGNED NOT NULL DEFAULT 0  COMMENT 'Bishop chon GameServer theo cot nay',
  `revival_id`   INT              NOT NULL DEFAULT 0,
  `enter_game_id` INT             NOT NULL DEFAULT 0,
  `world_stat`   INT              NOT NULL DEFAULT 0,
  `sect_stat`    INT              NOT NULL DEFAULT 0,
  `n_item`       SMALLINT         NOT NULL DEFAULT 0,
  `n_task`       SMALLINT         NOT NULL DEFAULT 0,
  `n_fight_skill` SMALLINT        NOT NULL DEFAULT 0,
  `n_state_skill` SMALLINT        NOT NULL DEFAULT 0,

  -- ===== dieu phoi ghi: chong mat luot luu va chong NHAN DOI VAT PHAM =====
  `ver`         BIGINT UNSIGNED   NOT NULL DEFAULT 0
                COMMENT 'khoa lac quan: moi luot luu +1, ben ghi phai kiem affected_rows==1',
  `locked_by`   INT                       NULL
                COMMENT 'THAY cho m_csRoleLock trong RAM (von KHONG phai khoa loai tru)',
  `locked_at`   DATETIME                  NULL COMMENT 'de het han khoa cua ket noi rot',
  `created_at`  DATETIME          NOT NULL,
  `updated_at`  TIMESTAMP(3)      NOT NULL DEFAULT CURRENT_TIMESTAMP(3)
                                           ON UPDATE CURRENT_TIMESTAMP(3),

  PRIMARY KEY (`role_name`),
  KEY `idx_acc`   (`acc_name`),
  KEY `idx_level` (`level`),
  KEY `idx_tong`  (`tong_id`),
  KEY `idx_lock`  (`locked_at`)
) ENGINE=InnoDB DEFAULT CHARSET=binary ROW_FORMAT=DYNAMIC;

-- ---------------------------------------------------------------------
--  role_delete_log -- hien tai xoa nhan vat la MAT VINH VIEN, khong tombstone
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `role_delete_log` (
  `id`          BIGINT AUTO_INCREMENT PRIMARY KEY,
  `role_name`   VARBINARY(32) NOT NULL,
  `acc_name`    VARBINARY(32) NOT NULL,
  `data_len`    INT UNSIGNED  NOT NULL,
  `role_blob`   MEDIUMBLOB    NOT NULL,
  `deleted_at`  DATETIME      NOT NULL,
  `deleted_by`  INT           NULL COMMENT 'iClientID cua ket noi ra lenh xoa',
  KEY `idx_name` (`role_name`),
  KEY `idx_time` (`deleted_at`)
) ENGINE=InnoDB DEFAULT CHARSET=binary ROW_FORMAT=DYNAMIC;

-- ---------------------------------------------------------------------
--  role_save_fail -- moi luot luu THAT BAI phai de lai dau vet.
--  Hien tai GameServer VUT BO ma ket qua luu (KSOServer.cpp) => mat phien choi
--  trong im lang. Bang nay la luoi an toan o tang DB.
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `role_save_fail` (
  `id`          BIGINT AUTO_INCREMENT PRIMARY KEY,
  `role_name`   VARBINARY(32) NOT NULL,
  `reason`      VARCHAR(64)   NOT NULL,
  `data_len`    INT           NOT NULL COMMENT 'CO DAU: goi hong co the mang dwDataLen AM, phai ghi lai duoc',
  `role_blob`   MEDIUMBLOB        NULL COMMENT 'giu nguyen goi bi tu choi de cuu ho',
  `at`          DATETIME      NOT NULL,
  KEY `idx_name_time` (`role_name`, `at`)
) ENGINE=InnoDB DEFAULT CHARSET=binary ROW_FORMAT=DYNAMIC;

-- ---------------------------------------------------------------------
--  migrate_audit -- nhat ky moi lan chay cong cu di tru, de doi chieu ve sau
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `migrate_audit` (
  `id`           BIGINT AUTO_INCREMENT PRIMARY KEY,
  `run_at`       DATETIME     NOT NULL,
  `source_path`  VARCHAR(512) NOT NULL,
  `source_sha256` CHAR(64)    NOT NULL,
  `source_bytes` BIGINT       NOT NULL,
  `n_read`       INT          NOT NULL COMMENT 'so khoa doc duoc tu nguon',
  `n_rejected`   INT          NOT NULL COMMENT 'so ban ghi bi loai (rac)',
  `n_inserted`   INT          NOT NULL,
  `n_verified`   INT          NOT NULL COMMENT 'so ban ghi doc lai va khop hash',
  `note`         TEXT         NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------
--  migrate_reject -- MOI ban ghi bi loai phai co ten va ly do. Cam loai im lang.
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `migrate_reject` (
  `id`         BIGINT AUTO_INCREMENT PRIMARY KEY,
  `audit_id`   BIGINT        NOT NULL,
  `src_key`    VARBINARY(64) NOT NULL,
  `reason`     VARCHAR(255)  NOT NULL,
  `raw_len`    INT UNSIGNED  NOT NULL,
  `raw_head`   VARBINARY(64) NOT NULL COMMENT '64 byte dau, de con giam dinh tay',
  KEY `idx_audit` (`audit_id`)
) ENGINE=InnoDB DEFAULT CHARSET=binary;
