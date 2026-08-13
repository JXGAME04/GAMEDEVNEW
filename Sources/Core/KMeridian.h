#ifndef KMeridianH
#define KMeridianH

#include "KWorldMsg.h"
#include "KNpc.h"
#include "KNode.h"
#include "KIndexNode.h"
#include "GameDataDef.h"

class KMeridian
{
	friend class KPlayer;
public:
	KMeridian();
	void Init(int nPlayerIdx);
	void Reset();
	~KMeridian();
	BYTE* getMeridian();
	int setMeridian(int nMeridianType, int nMeridianLevel);
	bool setMeridian(BYTE* meridian);
	BOOL canEnhance(int type, int level);
private:
	int		m_nPlayerIndex;
	BYTE m_abActiveMeridianLevel[MAX_MERIDIAN];
};


class KMeridianManager
{
	friend class KPlayer;
	friend class KMeridian;
	friend class KNpc;
public:
	KTabFile MagicTab;

	KMeridianManager();
	void Init();
	~KMeridianManager();
	void ApplyMaridianToNPC(IN KNpc* pNPC, IN int nMaridianType, IN int nMaridianLevel, IN int levelDiff = 0);
	void ApplyMaridianToNPC(IN KNpc* pNPC, IN BYTE* baMeridian);
	void RemoveMaridianFromNPC(IN KNpc* pNPC, IN int nMaridianType, IN int nMaridianLevel, IN int levelDiff = 0);
};
extern KMeridianManager MeridianManager;
#endif
