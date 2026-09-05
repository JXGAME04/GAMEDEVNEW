-- =====================================================================
--  jx1_role.relay_kv -- thay 5 kho Berkeley DB cua S3Relay.exe
--  Sinh ngay 20/08/2026.
--
--  VI SAO MOT BANG CHUNG cho ca 5 kho:
--   * Ca 5 kho deu la anh xa khoa -> mot struct C co dinh, khong truy van noi dung.
--   * Giu blob NGUYEN VEN tung byte => di tru chung minh duoc bang hash, va KHONG
--     the sai anh xa truong (kieu loi lam hong bang hoi).
--   * Kich thuoc struct DA DOI CHIEU VOI DU LIEU THAT tren may test:
--       TTongStruct   = 6860  (ban ghi that: 6860)
--       TMemberStruct =  404  (ban ghi that:  404)
--       TMoneyStruct  =   36
--       TZhaoMuStruct =   76  (co 1 byte dem tai offset 71 -- ban ghi khong tat dinh)
--     Khong co #pragma pack nao tac dong; can le hieu dung = 4.
--
--  BAY DA BIET, XU LY TRONG TANG LUU TRU:
--   * Khoa bang `friend` KHONG gom byte NUL; cac bang Tong thi CO.
--     Tang luu tru cat moi byte NUL cuoi -> ca hai quy uoc deu khop.
--   * TMoneyStruct.m_dwMoney la DWORD KHONG DAU -- cot `money` duoi day la
--     BIGINT CO DAU, tuyet doi khong UNSIGNED, neu khong tai lap dung su co
--     "quy bang tran so am -> 4,29 ty".
--   * remove(key, size, index=0) ben Berkeley DB BO QUA index (DBTable.cpp:251)
--     nen TONGDB.CPP:181 `while(!remove(tong,size,0)){}` LAP VO TAN khi xoa bang.
--     Ban MySQL hien thuc DUNG ngu nghia khoa phu -> go luon loi treo do.
-- =====================================================================

USE `jx1_role`;

CREATE TABLE IF NOT EXISTS `relay_kv` (
  `store`      VARCHAR(24)   NOT NULL
               COMMENT 'friend | TongTable | MemberTable | MoneyTable | ZhaoMuTable',
  `k`          VARBINARY(64) NOT NULL COMMENT 'khoa chinh, DA cat byte NUL cuoi',
  `v`          MEDIUMBLOB    NOT NULL COMMENT 'struct C nguyen ven tung byte',
  `v_len`      INT UNSIGNED  NOT NULL,
  `idx0`       VARBINARY(64)     NULL
               COMMENT 'khoa phu = szTong. MemberTable: offset 32. ZhaoMuTable: offset 0.',
  `money`      BIGINT            NULL
               COMMENT 'CHI cho MoneyTable: TMoneyStruct.m_dwMoney @32. CO DAU.',
  `updated_at` TIMESTAMP     NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`store`, `k`),
  KEY `idx_sec` (`store`, `idx0`)
) ENGINE=InnoDB DEFAULT CHARSET=binary ROW_FORMAT=DYNAMIC;

-- Nhat ky xoa: ben Berkeley DB xoa la MAT VINH VIEN (bang hoi, thanh vien, quy bang).
CREATE TABLE IF NOT EXISTS `relay_delete_log` (
  `id`         BIGINT AUTO_INCREMENT PRIMARY KEY,
  `store`      VARCHAR(24)   NOT NULL,
  `k`          VARBINARY(64) NOT NULL,
  `v`          MEDIUMBLOB    NOT NULL,
  `v_len`      INT UNSIGNED  NOT NULL,
  `deleted_at` DATETIME      NOT NULL,
  KEY `idx_store_k` (`store`, `k`),
  KEY `idx_time` (`deleted_at`)
) ENGINE=InnoDB DEFAULT CHARSET=binary ROW_FORMAT=DYNAMIC;

-- =====================================================================
--  jx1_role.relay_offline_msg -- LOI NHAN MAT cho nguoi choi NGOAI TUYEN (04/09/2026)
--  Port tu S3Relay Linux (KOfflineMsgStore): bang OfflineMsg(Receiver, Sender, Msg, LastModify).
--  S3Relay tu CREATE TABLE IF NOT EXISTS luc khoi dong (OfflineMsgDB.cpp) -- tep nay chi de
--  tham chieu / tao truoc. Gioi han do tu binary s3relay_y: toi da 10 loi nhan / nguoi nhan
--  (0x80e65dd cmp 9;jg), giu 30 ngay (0x80e5c29 sub 0x278d00). Chinh o relay_friendcfg.ini [offlinemsg].
-- =====================================================================
USE `jx1_role`;

CREATE TABLE IF NOT EXISTS `relay_offline_msg` (
  `id`       BIGINT        NOT NULL AUTO_INCREMENT PRIMARY KEY,
  `receiver` VARBINARY(32) NOT NULL COMMENT 'ten nhan vat nhan (TCVN3, khop chinh xac nhu FriendMgr)',
  `sender`   VARBINARY(32) NOT NULL,
  `msg`      VARBINARY(512) NOT NULL COMMENT 'noi dung tho tu client, toi da 255 byte',
  `created`  DATETIME      NOT NULL,
  KEY `idx_receiver` (`receiver`),
  KEY `idx_created` (`created`)
) ENGINE=InnoDB DEFAULT CHARSET=binary;
