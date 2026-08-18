//---------------------------------------------------------------------------
//  PerfHud.h - bang do hieu nang hien ngay trong game (FPS / CPU / GPU / RAM)
//
//  Bat bang config.ini:  [Client] PerfHud=1   (0 = tat, mac dinh)
//  Ve o goc trai-tren, chu co vien den nen doc duoc tren moi nen canh.
//
//  Moi so lieu deu lay bang API he thong nap DONG (kernel32/psapi/pdh) nen
//  khong them thu vien lien ket nao; may nao thieu API thi o do hien "n/a".
//---------------------------------------------------------------------------
#ifndef PERFHUD_H
#define PERFHUD_H

// bat/tat (doc tu config luc khoi dong, co the doi luc chay)
void    PerfHud_SetEnable(int nOn);
int     PerfHud_IsEnable();

// goi moi khung ve, ngay truoc RepresentEnd
//   nPaintFps = so khung VE do duoc trong 1 giay
//   nLogicFps = so vong LOGIC (game loop) trong 1 giay
void    PerfHud_Draw(int nPaintFps, int nLogicFps, unsigned int dwPing);

#endif
