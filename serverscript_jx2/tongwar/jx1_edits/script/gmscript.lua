-- gmscript.lua - diem ha canh cua GlobalExecute "dw ..." (WLLS port 20/08/2026).
-- Linux goc: \script\gmscript.lua:7 Include wlls_gmscript.lua; cac chuoi "dw"
-- (nay qua KJx2DeferredExec, hoan 1 tick) chay trong state cua file nay.
Include("\\script\\missions\\leaguematch\\wlls_gmscript.lua");
Include("\\script\\event\\tongwar\\tongwar_gmscript.lua");	-- [TONGWAR 23/08] dw tongwar_start/tongwar_gw_say ha canh state nay (Linux gmscript.lua:9)
