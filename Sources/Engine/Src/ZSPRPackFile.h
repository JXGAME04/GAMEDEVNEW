#ifndef ZSPRPACKFILE
#define ZSPRPACKFILE

#include <windows.h>
#ifndef JX_LP64_TYPES	// [ANDROID 08/09] long tren dia/goi: Windows 4 byte (giu nguyen long), Android LP64 phai la 32 bit
#define JX_LP64_TYPES
#if defined(JX_POSIX)
typedef int JX_LONG; typedef unsigned int JX_ULONG;
#else
typedef long JX_LONG; typedef unsigned long JX_ULONG;
#endif
#endif
typedef struct {
	JX_ULONG index_high;
	JX_ULONG index_low;
	JX_LONG offset; 
	JX_LONG size; 
} item_info;

#include "KSprite.h"

#define MINIMIZE_BLOCK_SIZE	16					//一个块最少16个字节
#define MAX_LAST			16					//记录最近使用的项

class ZCache {
	char *buffer;								//实际的缓冲区
	item_info *free_items;						//空闲块
	JX_LONG cache_size;
	JX_ULONG last_items[MAX_LAST];
	int last;
	CRITICAL_SECTION mutex;
public:
	ZCache(long size);
	virtual ~ZCache();
	char *getNode(unsigned long index_high, unsigned long index_low, long size);
	void completeNode(char *node);
	char *searchNode(unsigned long index_high, unsigned long index_low = 0xFFFF);
	void releaseNode(char *node);
};

class ZFile {
protected:
	HANDLE m_hFile;
	JX_ULONG m_Size;
	ZCache *m_Cache;
public:
	bool opened;
	ZFile(const char *name, ZCache *cache) {
		opened = false;
#ifdef JX_PLATFORM_SDL	// [SDL 08/09 2b-1b] HANDLE chua SDL_IOStream*
		{
#ifdef JX_POSIX
			char szPosix[1024]; name = JxPathPosix(name, szPosix, sizeof(szPosix));	// [ANDROID 08/09]
#endif
			SDL_IOStream* pIO = SDL_IOFromFile(name, "rb"); m_hFile = pIO ? (HANDLE)pIO : INVALID_HANDLE_VALUE; }
		if(m_hFile != INVALID_HANDLE_VALUE) {
			m_Size = (unsigned long)SDL_GetIOSize((SDL_IOStream*)m_hFile);
#else
		m_hFile = CreateFile(name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if(m_hFile != INVALID_HANDLE_VALUE) {
			m_Size = GetFileSize(m_hFile, NULL);
#endif
			m_Cache = cache;
			opened = true;
		}
	}
	virtual ~ZFile() {
#ifdef JX_PLATFORM_SDL
		if(m_hFile != INVALID_HANDLE_VALUE) SDL_CloseIO((SDL_IOStream*)m_hFile);
#else
		if(m_hFile != INVALID_HANDLE_VALUE) CloseHandle(m_hFile);
#endif
	}
	char *read(unsigned long offset, int size);
	int read(char *buffer, unsigned long offset, int size);
	void release(char *node) {
		m_Cache->releaseNode(node);
	}
};

//一个Pack文件具有下面的结构:
//首先是四个字节的文件的头标志:字符串'PACK',然后是项的数目然后是索引开始的偏移量\数据开始的偏移量,然后是校验和,然后是保留的字节:
//---------------------------------------------------------------------------------------------------------------------------------
typedef struct {		//索引信息
	JX_ULONG id;
	JX_ULONG offset;
	JX_LONG size;
	JX_LONG compress_size;
} index_info;

#define TYPE_NONE			0					//没有压缩
#define TYPE_UCL			1					//UCL压缩
#define TYPE_BZIP2			2					//bzip2压缩
#define TYPE_FRAME			0x10				//使用了独立帧压缩

typedef struct {
	unsigned char signature[4];			//"PACK"
	JX_ULONG count;				//数据的条目数
	JX_ULONG index_offset;			//索引的偏移量
	JX_ULONG data_offset;			//数据的偏移量
	JX_ULONG crc32;
	unsigned char reserved[12];
} z_pack_header;

class ZPackFile : public ZFile {
protected:
	index_info	*index_list;
	z_pack_header header;
	bool _readData(int node_index, char *node);								//将指定node_index的数据全部读到指定内存中，如果需要的话完成解压缩
public:
	ZPackFile(const char *name, ZCache *cache);					//打开文件
	virtual ~ZPackFile();
	int getNodeIndex(unsigned long id);
	char *getData(unsigned long index);
	unsigned long getSize(unsigned long index);
	char *getData(const char *name);										//获取指定节点的数据
	void releaseData(char *data);
};

typedef struct {
	JX_LONG compress_size;
	JX_LONG size;
} frame_info;

typedef struct {
	JX_ULONG	id;
	int				frame;
} frame_index_info;

#define MAX_IMAGE		0x10													//最多临时文件的个数

class ZSPRPackFile : public ZPackFile
{
public:
	ZSPRPackFile(const char *file_name, ZCache *the_cache);
	virtual ~ZSPRPackFile();

	SPRHEAD*	SprGetHeader(unsigned long uNameId, SPROFFS*& pOffsetTable);
	SPRFRAME*	SprGetFrame(SPRHEAD* pSprHeader, int nFrame);
};

unsigned long FileNameHash(const char* file_name);

#define	SPR_COMMENT_FLAG	0x525053

#define NODE_WAITING	0xFFFF0000ul;

#endif