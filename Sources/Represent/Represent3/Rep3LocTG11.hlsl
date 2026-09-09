// [LOCTG 09/09] Bo loc thoi gian luc trinh khung (temporal filter) cho Represent3 tren D3D11.
// Dich: fxc /nologo /T vs_4_0 /E VS /Fh Rep3LocTG11_vs.h /Vn g_Rep3LocVS11 Rep3LocTG11.hlsl
//       fxc /nologo /T ps_4_0 /E PS /Fh Rep3LocTG11_ps.h /Vn g_Rep3LocPS11 Rep3LocTG11.hlsl
//
// Vi sao: tren man hinh giu mau (nhat la tam VA), diem anh duoi net chu / sprite dang chay phai nhay
// toi -> sang trong MOT khung (7 ms o 143 Hz) ma tam nen can >10 ms de chuyen xong => luon o trang thai
// do dang, toi va lech mau ("am do"). Chu da A/B: cap nhat moi khung = am, giu 2 khung (14 ms) = het am.
// Bo loc nay lam tin hieu dua ra man hinh BIEN THIEN DAN thay vi nhay bac: out = lerp(cur, hist, a),
// a = exp(-dt / tau). Tinh chat: tu thich nghi theo tan so ve - 60 Hz (dt 16,7 ms) a ~ 0,12 gan nhu tat;
// 143 Hz a ~ 0,42; 240 Hz a ~ 0,59; may yeu fps thap -> a -> 0. Khong phu thuoc GPU manh yeu.

cbuffer LocCB : register(b0)
{
    float4 g_a;     // x = trong so lich su 0..1
};

Texture2D g_cur  : register(t0);    // khung vua ve xong (ban sao)
Texture2D g_hist : register(t1);    // ket qua da tron cua khung truoc

struct VSOut { float4 pos : SV_Position; };

// tam giac phu man hinh tu SV_VertexID, khong can vertex buffer / input layout
VSOut VS(uint id : SV_VertexID)
{
    VSOut o;
    float2 uv = float2((id << 1) & 2, id & 2);
    o.pos = float4(uv * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.5, 1.0);
    return o;
}

float4 PS(VSOut i) : SV_Target
{
    int3 p = int3((int2)i.pos.xy, 0);
    float4 c = g_cur.Load(p);
    float4 h = g_hist.Load(p);
    return lerp(c, h, g_a.x);
}
