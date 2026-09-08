# -*- coding: utf-8 -*-
"""[SDL 08/09 2b-1b] Tep pak/sprite qua SDL_IOStream khi JX_PLATFORM_SDL (Win32 khong doi):
 - XPackFile.cpp: HANDLE m_hFile chua SDL_IOStream*; CreateFile/GetFileSize/ReadFile/SetFilePointer/CloseHandle -> SDL_IOFromFile/SDL_GetIOSize/
   SDL_ReadIO/SDL_SeekIO/SDL_CloseIO; CRITICAL_SECTION m_ReadCritical -> SDL_Mutex* (macro file-local, 12 diem khoa khong doi).
 - ZSPRPackFile.h/.cpp: ZFile (CreateFile/GetFileSize/CloseHandle/SetFilePointer/ReadFile) + ZCache mutex -> SDL.
 - KFilePath.cpp: CreateDirectory -> SDL_CreateDirectory; GetFileAttributes -> SDL_GetPathInfo.
Byte-safe, moi neo duy nhat, chay lai an toan."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
E = ROOT + "\\Sources\\Engine\\Src\\"
TAG = b"[SDL 08/09 2b-1b]"
def rd(p): return io.open(p, "rb").read()
def wr(p, b, nb):
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127), p
    io.open(p, "wb").write(nb)
def nlof(b): return b"\r\n" if b"\r\n" in b else b"\n"
def rep1(b, a, c):
    n = b.count(a); assert n == 1, (a[:60], n); return b.replace(a, c)
CS_MACROS = [
    b"#ifdef JX_PLATFORM_SDL",
    b"// " + TAG + b" CRITICAL_SECTION cua tep nay chua SDL_Mutex* o 8 byte dau; HANDLE m_hFile chua SDL_IOStream*",
    b"#undef InitializeCriticalSection",
    b"#undef DeleteCriticalSection",
    b"#undef EnterCriticalSection",
    b"#undef LeaveCriticalSection",
    b"#define InitializeCriticalSection(p)  (*(SDL_Mutex**)(p) = SDL_CreateMutex())",
    b"#define DeleteCriticalSection(p)      SDL_DestroyMutex(*(SDL_Mutex**)(p))",
    b"#define EnterCriticalSection(p)       SDL_LockMutex(*(SDL_Mutex**)(p))",
    b"#define LeaveCriticalSection(p)       SDL_UnlockMutex(*(SDL_Mutex**)(p))",
    b"static inline HANDLE XP_Open(const char* pszName) { SDL_IOStream* p = SDL_IOFromFile(pszName, \"rb\"); return p ? (HANDLE)p : INVALID_HANDLE_VALUE; }",
    b"static inline unsigned int XP_Size(HANDLE h) { Sint64 s = SDL_GetIOSize((SDL_IOStream*)h); return (s < 0) ? (unsigned int)INVALID_FILE_SIZE : (unsigned int)s; }",
    b"static inline bool XP_Read(HANDLE h, void* pBuf, unsigned int uLen) { return SDL_ReadIO((SDL_IOStream*)h, pBuf, uLen) == (size_t)uLen; }",
    b"static inline bool XP_ReadAt(HANDLE h, unsigned int uOff, void* pBuf, unsigned int uLen) { return SDL_SeekIO((SDL_IOStream*)h, (Sint64)uOff, SDL_IO_SEEK_SET) == (Sint64)uOff && XP_Read(h, pBuf, uLen); }",
    b"static inline void XP_Close(HANDLE h) { SDL_CloseIO((SDL_IOStream*)h); }",
    b"#endif"]

# ---------------- XPackFile.cpp ----------------
p = E + "XPackFile.cpp"; b = rd(p)
if TAG not in b:
    nl = nlof(b)
    b = rep1(b, b"#include <crtdbg.h>" + nl, b"#include <crtdbg.h>" + nl + nl.join(CS_MACROS) + nl)
    b = rep1(b, b"\tm_hFile = ::CreateFile(pszPackFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);" + nl,
             b"#ifdef JX_PLATFORM_SDL" + nl + b"\tm_hFile = XP_Open(pszPackFileName);" + nl + b"#else" + nl + b"\tm_hFile = ::CreateFile(pszPackFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);" + nl + b"#endif" + nl)
    b = rep1(b, b"\t\tm_uFileSize = ::GetFileSize(m_hFile, NULL);" + nl,
             b"#ifdef JX_PLATFORM_SDL" + nl + b"\t\tm_uFileSize = XP_Size(m_hFile);" + nl + b"#else" + nl + b"\t\tm_uFileSize = ::GetFileSize(m_hFile, NULL);" + nl + b"#endif" + nl)
    b = rep1(b, b"\t\tif (::ReadFile(m_hFile, &Header, sizeof(Header), &dwReaded, NULL) == FALSE)" + nl + b"\t\t\tbreak;" + nl,
             b"#ifdef JX_PLATFORM_SDL" + nl + b"\t\tdwReaded = XP_Read(m_hFile, &Header, sizeof(Header)) ? (DWORD)sizeof(Header) : 0;" + nl + b"\t\tif (dwReaded == 0)" + nl + b"#else" + nl +
             b"\t\tif (::ReadFile(m_hFile, &Header, sizeof(Header), &dwReaded, NULL) == FALSE)" + nl + b"#endif" + nl + b"\t\t\tbreak;" + nl)
    b = rep1(b, b"\t\tif (m_pIndexList == NULL ||" + nl + b"\t\t\t::SetFilePointer(m_hFile, Header.uIndexTableOffset, NULL, FILE_BEGIN) != Header.uIndexTableOffset)" + nl + b"\t\t{" + nl + b"\t\t\tbreak;" + nl + b"\t\t}" + nl +
             b"\t\tif (::ReadFile(m_hFile, m_pIndexList, dwListSize, &dwReaded, NULL) == FALSE)" + nl + b"\t\t\tbreak;" + nl,
             b"#ifdef JX_PLATFORM_SDL" + nl + b"\t\tif (m_pIndexList == NULL)" + nl + b"\t\t\tbreak;" + nl + b"\t\tdwReaded = XP_ReadAt(m_hFile, Header.uIndexTableOffset, m_pIndexList, dwListSize) ? dwListSize : 0;" + nl + b"#else" + nl +
             b"\t\tif (m_pIndexList == NULL ||" + nl + b"\t\t\t::SetFilePointer(m_hFile, Header.uIndexTableOffset, NULL, FILE_BEGIN) != Header.uIndexTableOffset)" + nl + b"\t\t{" + nl + b"\t\t\tbreak;" + nl + b"\t\t}" + nl +
             b"\t\tif (::ReadFile(m_hFile, m_pIndexList, dwListSize, &dwReaded, NULL) == FALSE)" + nl + b"\t\t\tbreak;" + nl + b"#endif" + nl)
    b = rep1(b, b"\t\t::CloseHandle(m_hFile);" + nl, b"#ifdef JX_PLATFORM_SDL" + nl + b"\t\tXP_Close(m_hFile);" + nl + b"#else" + nl + b"\t\t::CloseHandle(m_hFile);" + nl + b"#endif" + nl)
    b = rep1(b, b"\tif (uOffset + uSize <= m_uFileSize &&" + nl + b"\t\t::SetFilePointer(m_hFile, uOffset, 0, FILE_BEGIN) == uOffset)" + nl + b"\t{" + nl + b"\t\tif (ReadFile(m_hFile, pBuffer, uSize, &dwReaded, NULL))" + nl + b"\t\t{" + nl + b"\t\t\tif (dwReaded == uSize)" + nl + b"\t\t\t\tbResult = true;" + nl + b"\t\t}" + nl + b"\t}" + nl,
             b"#ifdef JX_PLATFORM_SDL" + nl + b"\tdwReaded = 0;" + nl + b"\tif (uOffset + uSize <= m_uFileSize && XP_ReadAt(m_hFile, uOffset, pBuffer, uSize))" + nl + b"\t\tbResult = true;" + nl + b"#else" + nl +
             b"\tif (uOffset + uSize <= m_uFileSize &&" + nl + b"\t\t::SetFilePointer(m_hFile, uOffset, 0, FILE_BEGIN) == uOffset)" + nl + b"\t{" + nl + b"\t\tif (ReadFile(m_hFile, pBuffer, uSize, &dwReaded, NULL))" + nl + b"\t\t{" + nl + b"\t\t\tif (dwReaded == uSize)" + nl + b"\t\t\t\tbResult = true;" + nl + b"\t\t}" + nl + b"\t}" + nl + b"#endif" + nl)
    wr(p, rd(p), b); print("XPackFile.cpp: SDL_IOStream + SDL_Mutex")
else: print("XPackFile.cpp: da va")

# ---------------- ZSPRPackFile.h/.cpp ----------------
p = E + "ZSPRPackFile.h"; b = rd(p)
if TAG not in b:
    nl = nlof(b)
    b = rep1(b, b"\t\tm_hFile = CreateFile(name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);" + nl + b"\t\tif(m_hFile != INVALID_HANDLE_VALUE) {" + nl + b"\t\t\tm_Size = GetFileSize(m_hFile, NULL);" + nl,
             b"#ifdef JX_PLATFORM_SDL\t// " + TAG + b" HANDLE chua SDL_IOStream*" + nl + b"\t\t{ SDL_IOStream* pIO = SDL_IOFromFile(name, \"rb\"); m_hFile = pIO ? (HANDLE)pIO : INVALID_HANDLE_VALUE; }" + nl + b"\t\tif(m_hFile != INVALID_HANDLE_VALUE) {" + nl + b"\t\t\tm_Size = (unsigned long)SDL_GetIOSize((SDL_IOStream*)m_hFile);" + nl + b"#else" + nl +
             b"\t\tm_hFile = CreateFile(name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);" + nl + b"\t\tif(m_hFile != INVALID_HANDLE_VALUE) {" + nl + b"\t\t\tm_Size = GetFileSize(m_hFile, NULL);" + nl + b"#endif" + nl)
    b = rep1(b, b"\t\tif(m_hFile != INVALID_HANDLE_VALUE) CloseHandle(m_hFile);" + nl,
             b"#ifdef JX_PLATFORM_SDL" + nl + b"\t\tif(m_hFile != INVALID_HANDLE_VALUE) SDL_CloseIO((SDL_IOStream*)m_hFile);" + nl + b"#else" + nl + b"\t\tif(m_hFile != INVALID_HANDLE_VALUE) CloseHandle(m_hFile);" + nl + b"#endif" + nl)
    wr(p, rd(p), b); print("ZSPRPackFile.h: SDL")
else: print("ZSPRPackFile.h: da va")
p = E + "ZSPRPackFile.cpp"; b = rd(p)
if TAG not in b:
    nl = nlof(b)
    m = list(re.finditer(rb"#include[^\r\n]*\r?\n", b)); assert m
    last = m[-1]
    macros = [l for l in CS_MACROS if not l.startswith(b"static inline")]   # chi macro CRITICAL_SECTION
    b = b[:last.end()] + nl.join(macros) + nl + b[last.end():]
    b = rep1(b, b"\tSetFilePointer(m_hFile, offset, 0, SEEK_SET);" + nl + b"\tif(ReadFile(m_hFile, buffer, size, &read_size, NULL)) return read_size;" + nl,
             b"#ifdef JX_PLATFORM_SDL" + nl + b"\tread_size = 0;" + nl + b"\tif (SDL_SeekIO((SDL_IOStream*)m_hFile, (Sint64)offset, SDL_IO_SEEK_SET) == (Sint64)offset) read_size = (DWORD)SDL_ReadIO((SDL_IOStream*)m_hFile, buffer, (size_t)size);" + nl + b"\treturn read_size;" + nl + b"#else" + nl +
             b"\tSetFilePointer(m_hFile, offset, 0, SEEK_SET);" + nl + b"\tif(ReadFile(m_hFile, buffer, size, &read_size, NULL)) return read_size;" + nl + b"#endif" + nl)
    wr(p, rd(p), b); print("ZSPRPackFile.cpp: SDL")
else: print("ZSPRPackFile.cpp: da va")

# ---------------- KFilePath.cpp ----------------
p = E + "KFilePath.cpp"; b = rd(p)
if TAG not in b:
    nl = nlof(b)
    b = rep1(b, b"\t\tif (szFullPath[i] == '\\\\') {" + nl + b"\t\t\tszFullPath[i] = 0;" + nl + b"\t\t\tCreateDirectory(szFullPath, NULL);" + nl,
             b"\t\tif (szFullPath[i] == '\\\\') {" + nl + b"\t\t\tszFullPath[i] = 0;" + nl + b"#ifdef JX_PLATFORM_SDL" + nl + b"\t\t\tSDL_CreateDirectory(szFullPath);\t// " + TAG + nl + b"#else" + nl + b"\t\t\tCreateDirectory(szFullPath, NULL);" + nl + b"#endif" + nl)
    b = rep1(b, b"#ifdef WIN32" + nl + b"\tCreateDirectory(szFullPath, NULL);" + nl + b"#else" + nl,
             b"#if defined(JX_PLATFORM_SDL)" + nl + b"\tSDL_CreateDirectory(szFullPath);" + nl + b"#elif defined(WIN32)" + nl + b"\tCreateDirectory(szFullPath, NULL);" + nl + b"#else" + nl)
    b = rep1(b, b"\t\t\t\tbExist = !(GetFileAttributes(szFullName) & FILE_ATTRIBUTE_DIRECTORY);",
             b"#ifdef JX_PLATFORM_SDL" + nl + b"\t\t\t\t{ SDL_PathInfo sInfo; bExist = SDL_GetPathInfo(szFullName, &sInfo) && sInfo.type != SDL_PATHTYPE_DIRECTORY; }\t// " + TAG + nl + b"#else" + nl +
             b"\t\t\t\tbExist = !(GetFileAttributes(szFullName) & FILE_ATTRIBUTE_DIRECTORY);" + nl + b"#endif")
    wr(p, rd(p), b); print("KFilePath.cpp: SDL")
else: print("KFilePath.cpp: da va")
print("XONG 2b-1b")
