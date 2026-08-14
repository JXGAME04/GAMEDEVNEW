-- Stub cho he KHOA BAO MAT cua JX2 (workshop_logic.lua Include file nay;
-- thieu la tbSecurityLock = nil -> moi thao tac Xay/Nang/Mo/Dong phuong
-- loi runtime). Ban goc kiem khoa bao mat tai khoan; JX1 chua co he do
-- nen luon cho qua.
tbSecurityLock = {}
function tbSecurityLock:CheckTong()
	return 1
end
function tbSecurityLock:Check()
	return 1
end
