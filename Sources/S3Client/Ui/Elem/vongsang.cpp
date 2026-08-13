#include "KWin32.h"
#include "../../Represent/iRepresent/iRepresentShell.h"

#include "vongsang.h"

extern iRepresentShell*	g_pRepresentShell;

#define  minframeitemback 30
#define  maxframeitemback 120
#define  toframeitemback  4
#define  framedelayitemback 6
#define	 framegoitemback 2

void _settypeitemcolor(int type,unsigned char &a,unsigned char &r,unsigned char &g,unsigned char &b,unsigned char ki=0)
{
	unsigned char zcol=255;
	zcol-=ki;
	switch(type)
	{
	case enomal:a=r=g=b=zcol;
		break;
	case ehuyenkim:a=r=b=zcol;g=0;//tim
		break;
	case ehoangkim:a=r=g=zcol;b=0;//vang
		break;
	case ebachkim:a=r=g=zcol;b=255;//sang bac,chua tim ra mau
		break;
	case eblue:a = g = b = zcol; r = 0;//xanh
		break;
	case ethan:a=r=g=zcol;b=0;
		break;
	}
}
void _draw_boder(KRUPoint *zdata,int zsizeline,int &zpos,unsigned char &zdir,int x,int y,int xe,int ye,int typecol)
{
	int i,temp,temp1;
	int tempcol=255/zsizeline;
	for(i=0;i<zsizeline;i++)
	{
		_settypeitemcolor(typecol,zdata[i].Color.Color_b.a,zdata[i].Color.Color_b.r, zdata[i].Color.Color_b.g,zdata[i].Color.Color_b.b, i*tempcol);
	}
	switch(zdir)
	{
	case  0:
		for(i=0;i<zsizeline;i++)
		{
			temp=zpos-i;
			if(temp>=x)//dir 0
			{
				zdata[i].oPosition.nX=temp;
				zdata[i].oPosition.nY=y;	
			}
			else 
			{
				temp1=y+x-temp;//pos end
				if(temp1<=ye)//chua dat den height cua dir 3
				{
					zdata[i].oPosition.nX=x;
					zdata[i].oPosition.nY=temp1;
				}
				else//truong hop cuoi,khong qua phan nua rect
				{
					zdata[i].oPosition.nX=x+temp1-ye;
					zdata[i].oPosition.nY=ye;
				}
			}
		}
		break;
	case  1:
		for(i=0;i<zsizeline;i++)
		{
			temp=zpos-i;
			if(temp>=y)//dir 1
			{
				zdata[i].oPosition.nY=temp;
				zdata[i].oPosition.nX=xe;	
			}
			else 
			{
				temp1=xe-(y-temp);//pos end
				if(temp1>=x)//chua dat den height cua dir 3
				{
					zdata[i].oPosition.nX=temp1;
					zdata[i].oPosition.nY=y;
				}
				else//truong hop cuoi,khong qua phan nua rect
				{
					zdata[i].oPosition.nY=y+x-temp1;
					zdata[i].oPosition.nX=x;
				}
			}
		}
		break;
	case  2:
		for(i=0;i<zsizeline;i++)
		{
			temp=zpos+i;
			if(temp<=xe)//dir 2
			{
				zdata[i].oPosition.nX=temp;
				zdata[i].oPosition.nY=ye;	
			}
			else 
			{
				temp1=ye-(temp-xe);//pos end
				if(temp1>=y)
				{
					zdata[i].oPosition.nX=xe;
					zdata[i].oPosition.nY=temp1;
				}
				else//truong hop cuoi,khong qua phan nua rect
				{
					zdata[i].oPosition.nX=xe-(y-temp1);
					zdata[i].oPosition.nY=y;
				}
			}
		}
		break;
	case  3:
		for(i=0;i<zsizeline;i++)
		{
			temp=zpos+i;
			if(temp<=ye)//dir 2
			{
				zdata[i].oPosition.nX=x;
				zdata[i].oPosition.nY=temp;	
			}
			else 
			{
				temp1=x+(temp-ye);//pos end
				if(temp1<=xe)
				{
					zdata[i].oPosition.nX=temp1;
					zdata[i].oPosition.nY=ye;
				}
				else//truong hop cuoi,khong qua phan nua rect
				{
					zdata[i].oPosition.nY=ye-(temp1-xe);
					zdata[i].oPosition.nX=xe;
				}
			}
		}
		break;
	}
	g_pRepresentShell->DrawPrimitives(zsizeline,zdata,RU_T_POINT,true);
}

static int dllmaxsizeline[16]={
	26,2*26,3*26,4*26,5*26,6*26,7*26,8*26,
	9*26,10*26,11*26,12*26,13*26,14*26,15*26,16*26
};

void _swapposvs(int &zpos,unsigned char &zdir,int x,int y,int width,int height)
{
	if(zdir==0)
	{
		zdir=2;
		zpos=2*x+width-zpos;
	}
	else if(zdir==1)
	{
		zdir=3;
		zpos=2*y+height-zpos;
	}
	else if(zdir==2)
	{
		zdir=0;
		zpos=2*x+width-zpos;
	}
	else if(zdir==3)
	{
		zdir=1;
		zpos=2*y+height-zpos;
	}
}

void DrawBorder(int &zpos,unsigned char &zdir,unsigned char &ztimedelay,int x,int y,int width,int height,int zsizeline,int typecol)
{
	ztimedelay+=framegoitemback;
	if(ztimedelay>=framedelayitemback)
	{
		ztimedelay-=framedelayitemback;
		if(zdir<=1)
			zpos++;
		else
			zpos--;
	}
	if(zdir==0)
	{
		if(zpos<x)
			zpos=x;
		else if(zpos>=x+width)
		{
			zpos=y+1;zdir=1;
		}
	}
	else if(zdir==1)
	{
		if(zpos<y)
			zpos=y;
		else if(zpos>=y+height)
		{
			zpos=x+width-1;zdir=2;
		}
	}
	else if(zdir==2)
	{
		if(zpos>x+width)
			zpos=x+width;
		else if(zpos<=x)
		{
			zpos=y+height-1;zdir=3;
		}
	}
	else if(zdir==3)
	{
		if(zpos>y+height)
			zpos=y+height;
		else if(zpos<=y)
		{
			zpos=x+1;zdir=0;
		}
	}

	int tempsize=width+height;
	if(zsizeline>tempsize||zsizeline<0)
		return;

	else if(zsizeline<=dllmaxsizeline[0])
	{
		KRUPoint temp[26];
		_draw_boder(temp,dllmaxsizeline[0],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[0],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[1])
	{
		KRUPoint temp[2*26];
		_draw_boder(temp,dllmaxsizeline[1],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[1],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[2])
	{
		KRUPoint temp[3*26];
		_draw_boder(temp,dllmaxsizeline[2],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[2],zpos,zdir,x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[3])
	{
		KRUPoint temp[4*26];
		_draw_boder(temp,dllmaxsizeline[3],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		
		_draw_boder(temp,dllmaxsizeline[3],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[4])
	{
		KRUPoint temp[5*26];
		_draw_boder(temp,dllmaxsizeline[4],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[4],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[5])
	{
		KRUPoint temp[6*26];
		_draw_boder(temp,dllmaxsizeline[5],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[5],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[6])
	{
		KRUPoint temp[7*26];
		_draw_boder(temp,dllmaxsizeline[6],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[6],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[7])
	{
		KRUPoint temp[8*26];
		_draw_boder(temp,dllmaxsizeline[7],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[7],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[8])
	{
		KRUPoint temp[9*26];
		_draw_boder(temp,dllmaxsizeline[8],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[8],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[9])
	{
		KRUPoint temp[10*26];
		_draw_boder(temp,dllmaxsizeline[9],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[9],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[10])
	{
		KRUPoint temp[11*26];
		_draw_boder(temp,dllmaxsizeline[10],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[10],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[11])
	{
		KRUPoint temp[12*26];
		_draw_boder(temp,dllmaxsizeline[11],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[11],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[12])
	{
		KRUPoint temp[13*26];
		_draw_boder(temp,dllmaxsizeline[12],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[12],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[13])
	{
		KRUPoint temp[14*26];
		_draw_boder(temp,dllmaxsizeline[13],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[13],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[14])
	{
		KRUPoint temp[15*26];
		_draw_boder(temp,dllmaxsizeline[14],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[14],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
	else if(zsizeline<=dllmaxsizeline[15])
	{
		KRUPoint temp[16*26];
		_draw_boder(temp,dllmaxsizeline[15],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
		_draw_boder(temp,dllmaxsizeline[15],zpos,zdir, x,y,x+width,y+height,typecol);
		_swapposvs(zpos,zdir,x,y,width,height);
	}
}
unsigned int l_BlueBorder[] =
{
	0x0a002d2d,
	0x0a003232,
	0x0a003737,
	0x0a003c3c,
	0x0a004141,
	0x0a004646,
	0x0a004b4b,
	0x0a005050,
	0x0a005555,
	0x0a005a5a,
	0x0a005f5f,
	0x0a006464,
	0x0a006969,
};


unsigned int l_PurpleBorder[] =
{    
    0x0a2d002d,
    0x0a320032,
    0x0a370037,
    0x0a3c003c,
    0x0a410041,
    0x0a460046,
    0x0a4b004b,
    0x0a500050,
    0x0a550055,
    0x0a5a005a,
    0x0a5f005f,
    0x0a640064,
    0x0a690069,
};

unsigned int l_GoldBorder[] =
{    
    0x0a2d2d00,
    0x0a323200,
    0x0a373700,
    0x0a3c3c00,
    0x0a414100,
    0x0a464600,
    0x0a4b4b00,
    0x0a505000,
    0x0a555500,
    0x0a5a5a00,
    0x0a5f5f00,
    0x0a646400,
    0x0a696900,    
};

unsigned int l_PlatinaBorder[] =
{
	0x0a1a1a1a,
	0x0a2d2d2d,
	0x0a404040,
	0x0a535353,
	0x0a666666,
	0x0a797979,
	0x0a8c8c8c,
	0x0a9f9f9f,
	0x0ab2b2b2,
	0x0ac5c5c5,
	0x0ad8d8d8,
	0x0aebebeb,
	0x0affffff,
};



void DrawBorder2(BOOL &bCountFrame, unsigned short &nFrame, unsigned long &ulTimeDelay, int nX, int nY, int nWidth, int nHeight, int nType)
{    
    KRURect        Rect;
    
    switch(nType)
    {
        case eblue:
            Rect.Color.Color_dw = l_BlueBorder[nFrame];
            break;
        case ehuyenkim:
            Rect.Color.Color_dw = l_PurpleBorder[nFrame];
            break;
        case ehoangkim:
            Rect.Color.Color_dw = l_GoldBorder[nFrame];
            break;
		case ebachkim:
			Rect.Color.Color_dw = l_PlatinaBorder[nFrame];
			break;
        default:
            return;
            break;
    }

    if(GetTickCount() -  ulTimeDelay >= maxframeitemback)
    {    
        ulTimeDelay = GetTickCount();
        if(bCountFrame == 0)
        {
            nFrame++;
            if(nFrame >= 12)
                bCountFrame = 1;
        }
        else
        {
            nFrame--;
            if(nFrame <= 0)
                bCountFrame = 0;
        }
        
        switch(nType)
        {
            case eblue:
                Rect.Color.Color_dw = l_BlueBorder[nFrame];
                break;
            case ehuyenkim:
                Rect.Color.Color_dw = l_PurpleBorder[nFrame];
                break;
            case ehoangkim:
                Rect.Color.Color_dw = l_GoldBorder[nFrame];
                break;
			case ebachkim:
				Rect.Color.Color_dw = l_PlatinaBorder[nFrame];
				break;
            default:
                return;
				break;
        }
    }
    for(int i = 0; i < 2; i++)
    {    
		Rect.oPosition.nX = nX + i - 1;
		Rect.oPosition.nY = nY + i - 1;
		Rect.oEndPos.nX = nX + nWidth - i;
		Rect.oEndPos.nY = nY + nHeight - i;
		Rect.oEndPos.nZ = Rect.oPosition.nZ = 0;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
        
    }    
}


