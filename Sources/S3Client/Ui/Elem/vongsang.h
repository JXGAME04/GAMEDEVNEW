#ifndef VONGSANG_H_
#define VONGSANG_H_
#pragma once


#include "../../Represent/iRepresent/KRepresentUnit.h"
enum enumtypeitem
{
	enomal=0,
    eblue,
	ehuyenkim,
	ehoangkim,
	ebachkim,
	ethan,
	enummer
};
void DrawBorder(int &zpos,unsigned char &zdir,unsigned char &ztimedelay,int x,int y,int width,int height,int zsizeline=52,int typecol=ehoangkim);
void DrawBorder2(BOOL &bCountFrame, unsigned short &nFrame, unsigned long &ulTimeDelay, int nX, int nY, int nWidth, int nHeight, int nType = ehoangkim);

#endif
