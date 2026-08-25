# -*- coding: ascii -*-
"""B3 - noi driver 3 hoat dong vao startgame.lua (boot) + timerserver.lua (lich),
tat ban VN Phong Lang Do. Byte-an toan (latin-1), ghi ca JX1 lan MIRROR.
Xu ly ca CRLF lan LF (startgame/timerserver dung CRLF)."""
import io, os
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"


def patch(rel, edits):
    p = os.path.join(JX1, rel.replace("/", os.sep))
    d = io.open(p, encoding="latin-1", newline="").read()
    if "[3HD 25/08]" in d:
        print("  (da noi roi):", rel)
        return
    for old, new in edits:
        c = d.count(old)
        assert c == 1, "edit khong khop (%d): %r" % (c, old[:60])
        d = d.replace(old, new)
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    m = os.path.join(MIRROR, rel.replace("/", os.sep))
    os.makedirs(os.path.dirname(m), exist_ok=True)
    io.open(m, "w", encoding="latin-1", newline="").write(d)
    print("  da noi:", rel)


BS = "\\\\"  # 2 backslash (giong trong file Lua)

# ---- startgame.lua (CRLF) ----
inc_line = 'Include("%sscript%smissions%stongcastle%stongcastle_driver.lua")' % (BS, BS, BS, BS)
patch("script/startgame.lua", [
    # 1) them Include driver ngay sau dong tongcastle_driver
    (inc_line,
     inc_line + '\r\nInclude("%sscript%stinhnang%s3hoatdong%shd3_driver.lua")\t-- [3HD 25/08] San boss Sat Thu + Phong Lang Do + Vuot Ai (ban Linux)' % (BS, BS, BS, BS)),
    # 2) OnGame: tat VN PLD + goi HD3_DriverInit
    ("\taddnpcphonglangdo()",
     "\t-- [3HD 25/08] TAT ban VN Phong Lang Do (thay bang ban Linux):\r\n"
     "\t-- addnpcphonglangdo()\r\n"
     "\tHD3_DriverInit()\t-- boot 3 hoat dong Linux (satthu NPC+boss / PLD thuyen phu)"),
])

# ---- timerserver.lua (CRLF) ----
inc_ts = 'Include("%sscript%stimerserver_ctc.lua")' % (BS, BS)
patch("script/timerserver.lua", [
    (inc_ts,
     inc_ts + '\r\nInclude("%sscript%stinhnang%s3hoatdong%shd3_driver.lua")\t-- [3HD 25/08] lich Phong Lang Do + Vuot Ai (thay S3Relay)' % (BS, BS, BS, BS)),
    ("\tsukien_tongkim(nHr,nMi)",
     "\tsukien_tongkim(nHr,nMi)\r\n"
     "\t-- [3HD 25/08] lich Phong Lang Do + Vuot Ai (thay tang S3Relay ban Linux)\r\n"
     "\tif (HD3_Tick ~= nil) then HD3_Tick(nHr, nMi) end"),
])
print("B3 wire: xong.")
