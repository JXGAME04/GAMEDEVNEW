#include "KCore.h"
#include "KItem.h"
#include "MyAssert.H"
#include "KInventory.h"

KInventory::KInventory()
{
	m_pArray = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nXu = 0;
	m_nMoney = 0;
}

KInventory::~KInventory()
{
    Release();
}

BOOL KInventory::Init(int nWidth, int nHeight)
{
	if (m_pArray)
	{
		delete [] m_pArray;
		m_pArray = NULL;
	}
	m_pArray = new int[nWidth * nHeight];

	if (!m_pArray)
		return FALSE;

	ZeroMemory(m_pArray, sizeof(int) * nWidth * nHeight);
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	return TRUE;
}

void KInventory::Release()
{
	if (m_pArray)
	{
		delete []m_pArray;
	}
	m_pArray = NULL;
	this->m_nWidth = 0;
	this->m_nHeight = 0;
	this->m_nMoney = 0;
	this->m_nXu = 0;
}

void KInventory::Clear()
{
	if (m_pArray)
		memset(m_pArray, 0, sizeof(int) * m_nWidth * m_nHeight);
	m_nMoney = 0;
	m_nXu = 0;
}

#pragma optimize( "y", off)
BOOL KInventory::PlaceItem(int nX, int nY, int nIdx, int nWidth, int nHeight)
{
	if (!m_pArray)
		return FALSE;

	if (nX < 0 || nY < 0 || nWidth < 1 || nHeight < 1 || nX + nWidth > m_nWidth || nY + nHeight > m_nHeight)
		return FALSE;

	if (nIdx <= 0)
	{
		_ASSERT(0);
		return FALSE;
	}

	int i, j;
	int nOldIdx = 0;

	for (i = nX; i < nX + nWidth; i++)
	{
		for (j = nY; j < nY + nHeight; j++)
		{
			if (!nOldIdx)
				nOldIdx = m_pArray[j * m_nWidth + i];
			// 位置上有东西
			if (nOldIdx)
			{
				// 位置上有多个东西，无法放置
				if (nOldIdx != m_pArray[j * m_nWidth + i])
					return FALSE;
				return FALSE;
			}
		}
	}
	// Set Item Idx to Inventory
	for (i = nX; i < nX + nWidth; i++)
	{
		for (j = nY; j < nY + nHeight; j++)
		{
			m_pArray[j * m_nWidth + i] = nIdx;
		}
	}
	return TRUE;
}

#pragma optimize( "", on)

BOOL KInventory::HoldItem(int nIdx, int nWidth, int nHeight)
{
	int i, j;
	for (i = 0; i < m_nWidth - nWidth + 1; i++)
	{
		for (j = 0; j < m_nHeight - nHeight + 1; j++)
		{
			if (PlaceItem(i, j, nIdx, nWidth, nHeight))
				return TRUE;
		}
	}
	return FALSE;
}

BOOL	KInventory::PickUpItem(int nIdx, int nX, int nY, int nWidth, int nHeight)
{
	if (nX < 0 || nY < 0 || nWidth < 1  || nHeight < 1 || nX + nWidth > this->m_nWidth || nY + nHeight > this->m_nHeight)
		return FALSE;

	int		i;
	for (i = nX; i < nX + nWidth; i++)
	{
		for (int j = nY; j < nY + nHeight; j++)
		{
			if (m_pArray[j * m_nWidth + i] != nIdx)
			{
				//_ASSERT(0); // tat cai nay 
				return FALSE;
			}
		}
	}

	for (i = nX; i < nX + nWidth; i++)
	{
		for (int j = nY; j < nY + nHeight; j++)
		{
			m_pArray[j * m_nWidth + i] = 0;
		}
	}

	return TRUE;
}

int		KInventory::FindItem(int nX, int nY)
{
	if (!m_pArray)
		return -1;
	if (nX < 0 || nX >= this->m_nWidth || nY < 0 || nY >= this->m_nHeight)
		return -1;

	int		nPos = nY * m_nWidth + nX;
	int		*pArray = &m_pArray[nPos];
	if (*pArray <= 0)
		return 0;
	int		nIdx = *pArray;

	if ((nPos > 0 && *(pArray - 1) == nIdx) || (nPos >= m_nWidth && *(pArray - m_nWidth) == nIdx))
		return -1;

	return nIdx;

/*
	for (int i = 0; i < nY * m_nWidth + nX; i++)
	{
		if (m_pArray[i] == nIdx)
			return -1;
	}

	return nIdx;
*/
}

BOOL KInventory::FindRoom(int nWidth, int nHeight, POINT* pPos)
{
	if (!pPos)
		return FALSE;
	if (nWidth < 1 || nWidth > m_nWidth || nHeight < 1 || nHeight > m_nHeight)
		return FALSE;

	int i, j;
	for (i = 0; i < m_nWidth - nWidth + 1; i++)
	{
		for (j = 0; j < m_nHeight - nHeight + 1; j++)
		{
			if (CheckRoom(i, j, nWidth, nHeight))
			{
				pPos->x = i;
				pPos->y = j;
				return TRUE;
			}
		}
	}
	pPos->x = 0;
	pPos->y = 0;
	return FALSE;
}

BOOL KInventory::CheckRoom(int nX, int nY, int nWidth, int nHeight)
{
	if (!m_pArray)
	{
		_ASSERT(0);
		return FALSE;
	}

	if (nX < 0 || nY < 0 || nWidth < 1 || nHeight < 1 || nX + nWidth > m_nWidth || nY + nHeight > m_nHeight)
		return FALSE;

	int i, j;
	int nOldIdx = 0;

	for (i = nX; i < nX + nWidth; i++)
	{
		for (j = nY; j < nY + nHeight; j++)
		{
			if (m_pArray[j * m_nWidth + i])
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL KInventory::AddMoney(int nMoney)
{
	if (m_nMoney + nMoney < 0)
		return FALSE;
	m_nMoney += nMoney;
	return TRUE;
}

BOOL KInventory::AddXu(int nXu)
{
	if (m_nXu + nXu < 0)
		return FALSE;
	m_nXu += nXu;
	return TRUE;
}

int		KInventory::GetNextItem(int nStartIdx, int nXpos, int nYpos, int *pX, int *pY)
{
	if (!m_pArray)
		return 0;
	if (nXpos < 0 || nYpos < 0 || nXpos >= m_nWidth || nYpos >= m_nHeight || !pX || !pY)
		return 0;
	int		nSize = m_nWidth * m_nHeight;
	int		i = nYpos * m_nWidth + nXpos;
	int		*pArray = &m_pArray[i];
	for ( ; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (*pArray == nStartIdx)
			continue;
		if (i < m_nWidth || pArray[-m_nWidth] != *pArray)
		{
			*pX = i % m_nWidth;
			*pY = i / m_nWidth;
			return *pArray;
		}
	}
	return 0;
}

int	KInventory::FindFreeCell(int nWidth, int nHeight)
{
	if (nWidth < 1 || nWidth > m_nWidth || nHeight < 1 || nHeight > m_nHeight)
		return 0;

	int i, j;
	int nCount = 0;
	for (i = 0; i < m_nWidth; i++)
	{
		int nReduce = 0;
		for (j = 0; j < m_nHeight; j++)
		{
			if (CheckRoom(i, j, nWidth, nHeight))
			{
				nReduce=(nWidth-1);
				j+=(nHeight-1);
				nCount++;
			}
		}
		i+=nReduce;
	}
	return nCount;
}

int	KInventory::CalcSameItemType(int nGenre, int nDetail, int nParticular, int nLevel)
{
	if (!m_pArray)
		return 0;

	int	nNum = 0;
	int	nCurIdx = 0;
	int	nSize = m_nWidth * m_nHeight;
	int	*pArray = m_pArray;

	for (int i = 0; i < nSize; i++)
	{
		if (*pArray <= 0)
		{
			pArray++;
			continue;
		}

		if (nCurIdx == *pArray)
		{
			pArray++;
			continue;
		}

		if (i < m_nWidth || pArray[-m_nWidth] != *pArray)
		{
			nCurIdx = *pArray;
			if (Item[nCurIdx].GetGenre() == nGenre && Item[nCurIdx].GetDetailType() == nDetail 
				&& Item[nCurIdx].GetParticular() == nParticular && Item[nCurIdx].GetLevel() == nLevel)
				nNum += Item[nCurIdx].GetStackNum();
		}

		pArray++;
	}
	return nNum;
}

BOOL	KInventory::FindEmptyPlace(int nWidth, int nHeight, POINT *pPos)
{
	if (!m_pArray)
		return FALSE;
	if (!pPos || nWidth <= 0 || nHeight <= 0 || nWidth > m_nWidth || nHeight > m_nHeight)
		return FALSE;

	int		i = 0, j = 0, nIdx = 0, a = 0, b = 0, nFind = 0;
	for (i = 0; i < m_nHeight - nHeight + 1; i++)
	{
		for (j = 0; j < m_nWidth - nWidth + 1; )
		{
			nIdx = m_pArray[i * m_nWidth + j];
			if (nIdx)
			{
				_ASSERT(Item[nIdx].GetWidth() > 0);
				j += Item[nIdx].GetWidth();
			}
			else
			{
				nFind = 1;
				for (a = i; a < i + nHeight; a++)
				{
					for (b = j; b < j + nWidth; b++)
					{
						if (m_pArray[a * m_nWidth + b])
						{
							nFind = 0;
							break;
						}
					}
					if (nFind == 0)
						break;
				}
				if (nFind)
				{
					pPos->x = a;
					pPos->y = b;
					return TRUE;
				}
				j++;
			}
		}
	}

	return FALSE;
}

BOOL	KInventory::FindSameToRemove(int nItemNature, int nItemGenre, int nDetailType, int nItemParticular, int nLevel, int nSeries, int *pnIdx)
{
	if (!m_pArray)
		return FALSE;
	if (!pnIdx)
		return FALSE;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;

	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (Item[*pArray].GetNature() == nItemNature && 
			Item[*pArray].GetGenre() == nItemGenre && 
			(nDetailType < 0 || Item[*pArray].GetDetailType() == nDetailType) && 
			Item[*pArray].GetParticular() == nItemParticular && 
			(nLevel < 0 || Item[*pArray].GetLevel() == nLevel) && 
			(nSeries < 0 || Item[*pArray].GetSeries() == nSeries))
		{
			*pnIdx = *pArray;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL	KInventory::FindSameDetailType(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY)
{
	if (!m_pArray)
		return FALSE;
	if (!pnIdx || !pnX || !pnY)
		return FALSE;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;

	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (Item[*pArray].GetGenre() == nGenre && Item[*pArray].GetDetailType() == nDetail && Item[*pArray].GetParticular() == nParticular)
		{
			*pnIdx = *pArray;
			*pnX = i % m_nWidth;
			*pnY = i / m_nWidth;
			return TRUE;
		}
	}

	return FALSE;

/*
	int j;

	for (i = 0; i < this->m_nHeight; i++)
	{
		for (j = 0; j < this->m_nWidth; j++)
		{
			if (!m_pArray[i * m_nWidth + j])
				continue;
			if (Item[m_pArray[i * m_nWidth + j]].GetGenre() != nGenre)
				continue;
			if (Item[m_pArray[i * m_nWidth + j]].GetDetailType() != nDetail)
				continue;
			*pnIdx = m_pArray[i * m_nWidth + j];
			*pnX = j;
			*pnY = i;
			return TRUE;
		}
	}

	return FALSE;
*/
}

int	KInventory::FindNumberArrayItem(int nIdx)//add by phong ki襲 antihack
{
	if (!m_pArray)
		return 0;

	if (nIdx <= 0 || nIdx >= MAX_ITEM)
		return 0;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;
	int     nNumber = 0;

	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (nIdx == *pArray)
		{
			nNumber ++;
		}
	}
	return nNumber;
}

BOOL KInventory::CheckSameItemType(int nGenre, int nDetail, int nParticular, int nLevel)
{
	if (!m_pArray)
		return FALSE;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;

	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (Item[*pArray].GetGenre() == nGenre && Item[*pArray].GetDetailType() == nDetail 
			&& Item[*pArray].GetParticular() == nParticular && Item[*pArray].GetLevel() == nLevel)
			return TRUE;
	}

	return FALSE;
}

BOOL  KInventory::FindSameItemToSort(int nIdx, int *pnIdx, int *pnX, int *pnY)//#xu ly xep chong x?l?x誴 ch錸g
{
	if (!m_pArray)
		return FALSE;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;
	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (Item[*pArray].GetKind() == Item[nIdx].GetKind()
			&& Item[*pArray].GetGenre() == Item[nIdx].GetGenre()
			&& Item[*pArray].GetDetailType() == Item[nIdx].GetDetailType()
			&& Item[*pArray].GetParticular() == Item[nIdx].GetParticular()
			&& Item[*pArray].GetSeries() == Item[nIdx].GetSeries()
			&& Item[*pArray].GetLevel() == Item[nIdx].GetLevel()
			&& Item[*pArray].GetParam() == Item[nIdx].GetParam()
			&& Item[*pArray].GetTime()->bYear == Item[nIdx].GetTime()->bYear
			&& Item[*pArray].GetPlayerItemLock() == Item[nIdx].GetPlayerItemLock()
			&& Item[*pArray].GetPlayerItemHLock() == Item[nIdx].GetPlayerItemHLock()
			&& Item[*pArray].GetStackNum() < Item[*pArray].GetMaxStackNum()
			)
		{
			*pnIdx = *pArray;
			*pnX = i % m_nWidth;
			*pnY = i / m_nWidth;
			return TRUE;
		}
	
	}
	return FALSE;
}

void KInventory::FindSameItemName(int nIdx, int pnIdx[])
{
	if (!m_pArray)
		return;

	int* pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;
	int count = 0;
	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (strcmp(Item[nIdx].GetName(), Item[*pArray].GetName()) == 0)
		{
			pnIdx[count++] = *pArray;
		}
	}
	if (count >= 60) return;
	pnIdx[count] = nIdx;
}