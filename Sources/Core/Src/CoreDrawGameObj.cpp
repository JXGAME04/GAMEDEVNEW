#include "KCore.h"
#include "KNpc.h"
#include "KMissle.h"
#include "KItem.h"
#include "KBuySell.h"
#include "KPlayer.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "KSubWorldSet.h"
#include "scene/KScenePlaceC.h"
#include "ImgRef.h"
#include "GameDataDef.h"
#include "KObjSet.h"
#include "KOption.h"

#define  PHYSICSSKILLICON "\\spr\\Ui\\¼¼ÄÜÍ¼±ê\\icon_sk_ty_ap.spr"
#define SHOW_SPACE_HEIGHT 5

void	CoreDrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, KNPARAM nParam)
{
#ifdef JX_ANDROID
	extern int g_nJxKeoAnhVatPham;	// [VEVATPHAM 12/09 d] 1 = keo anh cho vua o
#endif

	switch(uObjGenre)
	{
	case CGOG_NPC:
		if (uId > 0)
		{
			// [TRANGTRI 11/09 f] NPC trang tri (ga, buom, chuon chuon...) cao chi vai chuc diem
			// anh, bi chong THANH MAU + THE TEN + bieu tuong che kin. Lop the khong co y
			// nghia gi voi chung (da khong the chon lam muc tieu) nen bo han, chi ve than.
			if (Npc[uId].m_sClientNpcID.m_dwRegionID > 0)
			{
				if ((nParam & IPOT_RL_OBJECT) == IPOT_RL_OBJECT)
				{
					extern unsigned g_uTTVe;
					g_uTTVe++;
					Npc[uId].Paint();
				}
				break;
			}
			int nHeight = Npc[uId].GetNpcPate();
			int nnHeight = nHeight;

			if ((nParam & IPOT_RL_INFRONTOF_ALL) == IPOT_RL_INFRONTOF_ALL)
			{
				Npc[uId].PaintBlood(nHeight / 2);
				nHeight = Npc[uId].PaintChat(nnHeight);	

				if (nHeight == nnHeight)	
				{
					if (NpcSet.CheckShowLife())
					{
						nHeight = Npc[uId].PaintLife(nnHeight, true);//nHeight = Npc[uId].PaintLife(nnHeight, false);
					}
					//
					if (NpcSet.CheckShowName())
					{
						if (nnHeight != nHeight)	
						{
							nHeight += SHOW_SPACE_HEIGHT;
						}
						//Edit by Fong Kieu
						if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc() && Player[CLIENT_PLAYER_INDEX].GetTargetNpc() == uId)
							nHeight = Npc[uId].PaintInfo(nHeight, true, 14, 0XFF000000);//nHeight = Npc[uId].PaintInfo(nHeight, false, 14, 0XFF000000);	//±»Ñ¡ÖÐµÄÈËÃû·Å´óÏÔÊ¾
						else
							nHeight = Npc[uId].PaintInfo(nHeight, true);//nHeight = Npc[uId].PaintInfo(nHeight, false);
						nHeight += 0;
					}
				}
			}
			else if ((nParam & IPOT_RL_OBJECT) == IPOT_RL_OBJECT)
			{
				Npc[uId].Paint();
				if ((int)uId == Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
				{
				   if (Npc[uId].m_HideState.nTime <= 0 && Npc[uId].m_CurrentCamp != camp_audience)
				   {
						if (Npc[uId].m_Kind == kind_normal && Option.GetLow(LowNpc))
						{

						}
						else
						{
							Npc[uId].DrawBorder();
							if(!NpcSet.CheckShowLife())  // ==== Add by Fong Kieu ==== view life and info F7 F8
							{
								nnHeight -= SHOW_SPACE_HEIGHT;
								nnHeight -= SHOW_SPACE_HEIGHT;
								nHeight = Npc[uId].PaintLife(nnHeight, true);
							}
							if (!NpcSet.CheckShowName()) // ==== Add by Fong Kieu ==== view life and info F7 F8
							{
								nHeight += SHOW_SPACE_HEIGHT;
								nHeight += SHOW_SPACE_HEIGHT;
								nHeight = Npc[uId].PaintInfo(nHeight, true);
							}
						}
				   }
				}
			}				
		}
		break;
	case CGOG_MISSLE:
		if (uId > 0)
			Missle[uId].Paint();
		break;
	case CGOG_PLAYERSELLITEM:
	case CGOG_IME_ITEM:
	case CGOG_ITEM:
		if (uId == 0)
			break;

		if (Width == 0 && Height == 0)
		{
			#define	ITEM_CELL_WIDTH		26
			#define	ITEM_CELL_HEIGHT	26
			Width = Item[uId].GetWidth() * ITEM_CELL_WIDTH;
			Height = Item[uId].GetHeight() * ITEM_CELL_HEIGHT;
			x -= Width / 2;
			y -= Height / 2;
		}
		// [A33 04/09] Phep dich can-giua nay tinh theo kich thuoc GOC cua mon. Khi dang THU NHO
		// ve mot o thi no keo mon lech han ra ngoai (mon 2x4 bi keo len 39 px, sang trai 13 px).
		// Chi dich khi KHONG thu nho. Mon mot o khong doi mot pixel vi (26-26)/2 = 0.
#ifdef JX_ANDROID
		// [HANHTRANG 12/09] o to hon anh goc (hanh trang mobile 44 px) -> keo anh theo o, khong can giua
		// [VEVATPHAM 12/09 d] mac dinh KHONG keo anh nua: khung anh (nWidth/nHeight cua .spr) NHO HON hinh ve that nen
		// phep keo lam anh tran ra ngoai o (do duoc: o 36x36 -> binh thuoc ve 41x50, lech len 16 px).
		// Ve nguyen co + can giua o = giong het ban PC. Bat lai bang config.ini [Ui] KeoAnhVatPham=1.
		else if (uObjGenre != CGOG_IME_ITEM && (nParam & 0x40000000) == 0
			&& Width > Item[uId].GetWidth() * ITEM_CELL_WIDTH && Height > Item[uId].GetHeight() * ITEM_CELL_HEIGHT)
		{
			extern int g_nJxVeVatPhamW, g_nJxVeVatPhamH;
			extern int g_nJxVeVatPhamX, g_nJxVeVatPhamY;	// [VEVATPHAM 12/09] goc o
			g_nJxVeVatPhamW = Width;
			g_nJxVeVatPhamH = Height;
			g_nJxVeVatPhamX = x;
			g_nJxVeVatPhamY = y;
		}
#endif
		else if ((nParam & 0x40000000) == 0 || uObjGenre == CGOG_IME_ITEM
			|| Item[uId].GetWidth() * Item[uId].GetHeight() <= 1)
		{
			x += (Width - Item[uId].GetWidth() * ITEM_CELL_WIDTH) / 2;
			y += (Height - Item[uId].GetHeight() * ITEM_CELL_HEIGHT) / 2;
		}
		if (uObjGenre == CGOG_IME_ITEM)
		{
			Item[uId].PaintItem(x, y, false, false, 0);
		} 
		else
		{
			// [A29 04/09] tham so thu ba = bResize: mon rong hon MOT O thi PaintItem ve bang anh
			// thay the mot o. Chi bat cho o dinh kem hop thu / o vat pham bang dau gia (co tu ini),
			// tui do va cua hang khong bat nen khong doi gi.
			Item[uId].PaintItem(x, y, (nParam & 0x40000000) != 0, true, uId);
#ifdef JX_ANDROID
			{
				extern int g_nJxVeVatPhamW, g_nJxVeVatPhamH;	// [HANHTRANG 12/09] xoa co sau khi ve
				extern int g_nJxVeVatPhamX, g_nJxVeVatPhamY, g_nJxNhatKyVatPham;	// [VEVATPHAM 12/09]
				if (g_nJxNhatKyVatPham > 0 && g_nJxVeVatPhamW == 0)
				{
					g_nJxNhatKyVatPham--;
					g_DebugLog("[VATPHAM] KHONG keo: the loai %d mon %d tai %d,%d khung %dx%d (co goc %dx%d) tham so %08x",
						(int)uObjGenre, (int)uId, x, y, Width, Height,
						Item[uId].GetWidth() * ITEM_CELL_WIDTH, Item[uId].GetHeight() * ITEM_CELL_HEIGHT, (unsigned int)nParam);
				}
				g_nJxVeVatPhamW = g_nJxVeVatPhamH = 0;
				g_nJxVeVatPhamX = g_nJxVeVatPhamY = 0;
			}
#endif
		}	
		break;
	case CGOG_MENU_NPC:
		if (nParam)
		{
			((KNpcRes *)nParam)->SetPos(0, x + Width / 2, y + Height / 2 + 28, 0, FALSE, TRUE);
			#define		STAND_TOTAL_FRAME	15
			int nFrame = g_SubWorldSet.m_nLoopRate % STAND_TOTAL_FRAME;
			((KNpcRes *)nParam)->Draw(0, 0, STAND_TOTAL_FRAME, nFrame, true);
		}
		break;
	
	// [VHTD 02/09w] A-1: SetNextNo chay chi so o 0..6 va dang ky CGOG_NPC_BLUR_DETAIL(m_nCurNo).
	// DETAIL(0) rut gon DUNG BANG CGOG_NPC_BLUR = 8 nhung switch nay truoc gio chi co case 1..7
	// -> anh o o 0 roi vao default va bi vut, vet bong mo thieu deu 1/7 (ca chieu luot).
	case CGOG_NPC_BLUR:
		if (Npc[uId].GetNpcRes()->m_pcNpcBlur) Npc[uId].GetNpcRes()->m_pcNpcBlur->Draw(0);	// [RAMTINH 08/09]
		break;
	case CGOG_NPC_BLUR_DETAIL(1):
		if (Npc[uId].GetNpcRes()->m_pcNpcBlur) Npc[uId].GetNpcRes()->m_pcNpcBlur->Draw(1);	// [RAMTINH 08/09]
		break;
	case CGOG_NPC_BLUR_DETAIL(2):
		if (Npc[uId].GetNpcRes()->m_pcNpcBlur) Npc[uId].GetNpcRes()->m_pcNpcBlur->Draw(2);	// [RAMTINH 08/09]
		break;
	case CGOG_NPC_BLUR_DETAIL(3):
		if (Npc[uId].GetNpcRes()->m_pcNpcBlur) Npc[uId].GetNpcRes()->m_pcNpcBlur->Draw(3);	// [RAMTINH 08/09]
		break;
	case CGOG_NPC_BLUR_DETAIL(4):
		if (Npc[uId].GetNpcRes()->m_pcNpcBlur) Npc[uId].GetNpcRes()->m_pcNpcBlur->Draw(4);	// [RAMTINH 08/09]
		break;
	case CGOG_NPC_BLUR_DETAIL(5):
		if (Npc[uId].GetNpcRes()->m_pcNpcBlur) Npc[uId].GetNpcRes()->m_pcNpcBlur->Draw(5);	// [RAMTINH 08/09]
		break;
	case CGOG_NPC_BLUR_DETAIL(6):
		if (Npc[uId].GetNpcRes()->m_pcNpcBlur) Npc[uId].GetNpcRes()->m_pcNpcBlur->Draw(6);	// [RAMTINH 08/09]
		break;
	case CGOG_NPC_BLUR_DETAIL(7):
		if (Npc[uId].GetNpcRes()->m_pcNpcBlur) Npc[uId].GetNpcRes()->m_pcNpcBlur->Draw(7);	// [RAMTINH 08/09]
		break;
	case CGOG_SKILL:
	case CGOG_SKILL_FIGHT:
	case CGOG_SKILL_LIVE:
	case CGOG_SKILL_SHORTCUT:
		{
			int nSkillId = (int)uId;
			if (nSkillId > 0)
			{
				int i = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(nSkillId);
				ISkill *pISkill = g_SkillManager.GetSkill(nSkillId, 1);
				if (pISkill)
				{
					pISkill->DrawSkillIcon(x, y, Width, Height);
				}
				if(i)
				{
					int nDelay = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.m_Skills[i].NextCastTime -
					SubWorld[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex].m_dwCurrentTime;
					int nTotal = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.m_Skills[i].TotalTime;
					if(nDelay > 0 && nTotal > 0)
					{
						KRUShadow	Shadow;
						Shadow.Color.Color_dw = 0x16000000;
						Shadow.oPosition.nX = x;
						Shadow.oPosition.nY = y;
						Shadow.oEndPos.nX = x + Width;
						Shadow.oEndPos.nY = y + Height;
						g_pRepresent->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
						Shadow.Color.Color_dw = 0x16ffff00;
						Shadow.oPosition.nY = y + (nTotal - nDelay)*Height/nTotal;
						g_pRepresent->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
					}
				}
			}
			else
			{
				if(uId == -1) 
				{
					KRUImage RUIconImage;
					RUIconImage.nType = ISI_T_SPR;
					RUIconImage.Color.Color_b.a = 255;
					RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
					RUIconImage.uImage = 0;
					RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
					RUIconImage.bRenderFlag = 0;
					strcpy(RUIconImage.szImage, PHYSICSSKILLICON);
					RUIconImage.oPosition.nX = x;
					RUIconImage.oPosition.nY = y;
					RUIconImage.oPosition.nZ = 0;
					RUIconImage.nFrame = 0;
					g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, 1);
				}
			}
		}
		break;
	case CGOG_OBJECT:
		if (uId)
		{
			if ((nParam & IPOT_RL_INFRONTOF_ALL) == IPOT_RL_INFRONTOF_ALL)
			{
				if (ObjSet.CheckShowName())
					Object[uId].DrawInfo();
			}
			else
			{
				Object[uId].Draw();
				if ((int)uId == Player[CLIENT_PLAYER_INDEX].GetTargetObj())
				{
					Object[uId].DrawBorder();
					Object[uId].DrawInfo(); //VÏ tªn Object d­íi ®Êt khi hover qua
				}
			}
		}
		break;
	case CGOG_NPCSELLITEM:
		if (uId < 0)
			break;
		BuySell.PaintItem(uId, x, y);
		break;
	default:
		break;
	}
}

void	CoreGetGameObjLightInfo(unsigned int uObjGenre, unsigned int uId, KLightInfo *pLightInfo)
{
	switch (uObjGenre)
	{
	case CGOG_NPC:
		if (uId > 0 && uId < MAX_NPC)
		{
			Npc[uId].GetDrawPos(&pLightInfo->oPosition.nX, &pLightInfo->oPosition.nY);
			pLightInfo->oPosition.nZ = Npc[uId].m_Height;
			pLightInfo->dwColor = 0;
			if (Npc[uId].m_RedLum > 255)
			{
				Npc[uId].m_RedLum = 255;
			}
			if (Npc[uId].m_GreenLum > 255)
			{
				Npc[uId].m_GreenLum = 255;
			}
			if (Npc[uId].m_BlueLum > 255)
			{
				Npc[uId].m_BlueLum = 255;
			}
			pLightInfo->dwColor = 0xff000000 | Npc[uId].m_RedLum << 16 | Npc[uId].m_GreenLum << 8 | Npc[uId].m_BlueLum;
			pLightInfo->nRadius = Npc[uId].m_CurrentVisionRadius;
		}
		break;
	case CGOG_OBJECT:
		break;
	case CGOG_MISSLE:
		if (uId > 0 && uId < MAX_MISSLE)
		{
			if (Missle[uId].m_nMissleId > 0)
			{
				Missle[uId].GetLightInfo(pLightInfo);
			}
		}
		break;
	default:
		break;
	}
}