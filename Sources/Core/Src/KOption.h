#ifndef KOptionH
#define	KOptionH

enum
{
	LowPlayer,
	LowNpc,
	LowMap,
	LowMissle,
	LowEstPlayer,
	LowEstNpc,
	LowEstMap,
	LowEstMissle,
};

enum
{
	ManTypeNameIdx = 94,
	WomanTypeNameIdx = 472,
};

class KOption
{
private:
	int		m_nSndVolume;
	int		m_nMusicVolume;
	int		m_nGamma;
	BOOL	m_bLow[LowEstMissle+1]; //add by phong kiÒu
public:
	int		GetSndVolume() { return m_nSndVolume; }
	void	SetSndVolume(int nSndVolume);
	int		GetMusicVolume() { return m_nMusicVolume; }
	void	SetMusicVolume(int nMusicVolume);
	int		GetGamma() { return m_nGamma; };
	void	SetGamma(int nGamma);
	void	SetLow(int n, BOOL bLow) //add by phong kiÒu
	{
		if(n >= LowPlayer && n <= LowEstMissle)
			m_bLow[n] = bLow;
	};
	BOOL	GetLow(int n) { return m_bLow[n]; }; //add by phong kiÒu
};

extern KOption Option;
#endif