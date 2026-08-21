// ============================================================================
// repack_typeframe - transcode .pak entries to per-frame compression (TYPE_FRAME)
//
// Purpose (DIEUTRA_KHUNG_DONG_NGUOI.md 12.3): the running paks store SPR files
// whole-file UCL-compressed, so drawing ONE frame (~1.6 KB) forces the client
// to read + decompress the WHOLE file (~200 KB) synchronously inside the paint
// loop. The engine already has a per-frame path (XPackFile::GetSprFrame,
// auto-selected when the index flag has TYPE_FRAME) - only the pak data lacks
// the flag: spr.pak has just 43/14616 frame-mode entries (the old packer only
// converted SPR >= 800 KB, Sources\Pack\main.cpp:32).
//
// This tool rewrites a pak: every whole-file-UCL entry whose decompressed
// bytes are a valid SPR (magic "SPR\0") with Frames >= 2 and size >= threshold
// is re-encoded exactly like Sources\Pack\main.cpp:115-155 does:
//     SPRHEAD + palette (raw) + XPackSprFrameInfo[Frames] + frames
//     frame >= 256 B -> ucl_nrv2b_99_compress level 10 (lSize positive)
//     frame <  256 B -> stored raw                      (lSize negative)
//     index flag byte 0x11 = TYPE_FRAME | TYPE_UCL, low 3 bytes = blob size
// Entry ids/order/uncompressed sizes stay identical; everything else is copied
// byte-for-byte. Output is a NEW file - the source pak is opened read-only.
//
// Safety: every converted entry is VERIFIED in memory before being written:
// the tool replays XPackFile::GetSprFrame (XPackFile.cpp:546-593) on the new
// blob and compares every frame, the header and the palette byte-for-byte
// against the original decompressed SPR. Any mismatch aborts the whole run.
// Entries that fail any sanity check (zero-length frame, bounds, 16 MiB flag
// limit, >65535 entries which would overflow the WORD node-index stored in
// SPRHEAD.Reserved[2]) are copied through unchanged.
//
// Build:  see build_repack.cmd next to this file.
// Usage:  repack_typeframe <in.pak> <out.pak> [threshold_bytes]
//         threshold default 65536; pass 0 to only copy (round-trip test).
// ============================================================================

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucl/ucl.h>

#pragma pack(push, 1)
struct XPackFileHeader
{
    unsigned char cSignature[4];      // 'PACK'
    unsigned long uCount;
    unsigned long uIndexTableOffset;
    unsigned long uDataOffset;
    unsigned long uCrc32;             // never checked by the engine
    unsigned char cReserved[12];
};
struct XPackIndexInfo
{
    unsigned long uId;
    unsigned long uOffset;
    long          lSize;              // uncompressed size
    long          lCompressSizeFlag;  // high byte = method, low 3 bytes = stored size
};
struct XPackSprFrameInfo
{
    long lCompressSize;               // bytes on disk for this frame
    long lSize;                       // decompressed bytes; NEGATIVE => stored raw
};
struct SPRHEAD
{
    unsigned char Comment[4];         // "SPR\0"
    unsigned short Width, Height, CenterX, CenterY;
    unsigned short Frames, Colors, Directions, Interval;
    unsigned short Reserved[6];
};
struct SPROFFS
{
    unsigned long Offset;             // relative to first frame byte
    unsigned long Length;
};
#pragma pack(pop)

#define SPR_COMMENT_FLAG   0x525053
#define TYPE_NONE          0x00
#define TYPE_UCL           0x01
#define TYPE_BZIP2         0x02
#define TYPE_FRAME         0x10
#define TYPE_UCL_VNG       0x20   // VNG-labelled pak entry; ExtractRead (XPackFile.cpp:258)
                                  // treats it exactly like TYPE_UCL (same NRV2B stream)
#define FLAG_SIZE_MASK     0x00FFFFFF

static unsigned long long g_BytesSaved = 0;

static void die(const char* msg)
{
    fprintf(stderr, "FATAL: %s\n", msg);
    exit(2);
}

static void* xmalloc(size_t n)
{
    void* p = malloc(n ? n : 1);
    if (!p) die("out of memory");
    return p;
}

// decompress one whole-file UCL entry; returns malloc'd buffer of lSize bytes
static unsigned char* decompress_ucl(const unsigned char* comp, unsigned int compSize,
                                     unsigned int rawSize)
{
    unsigned char* out = (unsigned char*)xmalloc(rawSize);
    ucl_uint outLen = rawSize;
    int r = ucl_nrv2b_decompress_8(comp, compSize, out, &outLen, NULL);
    if (r != UCL_E_OK || outLen != rawSize)
    {
        free(out);
        return NULL;
    }
    return out;
}

// Re-encode one SPR to the frame-mode blob (layout of Sources\Pack\main.cpp:115-155).
// Returns malloc'd blob (+size via pOutSize), or NULL when the entry must be
// left untouched (any sanity check failed). Never aborts - caller copies through.
static unsigned char* encode_frame_mode(const unsigned char* spr, unsigned int rawSize,
                                        unsigned int* pOutSize, const char* why[1])
{
    *why = "";
    if (rawSize < sizeof(SPRHEAD)) { *why = "too small"; return NULL; }
    const SPRHEAD* head = (const SPRHEAD*)spr;
    if ((*(const unsigned int*)head->Comment & 0x00FFFFFF) != SPR_COMMENT_FLAG ||
        head->Comment[3] != 0)
    { *why = "no SPR magic"; return NULL; }
    if (head->Colors == 0 || head->Colors > 256) { *why = "bad Colors"; return NULL; }
    if (head->Frames < 2) { *why = "Frames < 2"; return NULL; }

    unsigned int base = sizeof(SPRHEAD) + head->Colors * 3 + head->Frames * sizeof(SPROFFS);
    if (base > rawSize) { *why = "tables past EOF"; return NULL; }
    const SPROFFS* offs = (const SPROFFS*)(spr + sizeof(SPRHEAD) + head->Colors * 3);
    const unsigned char* frameData = spr + base;
    unsigned int frameArea = rawSize - base;

    for (unsigned int f = 0; f < head->Frames; f++)
    {
        if (offs[f].Length == 0) { *why = "zero-length frame"; return NULL; }
        if (offs[f].Offset > frameArea || offs[f].Length > frameArea - offs[f].Offset)
        { *why = "frame out of bounds"; return NULL; }
        if (offs[f].Length > 0x7FFFFFFF) { *why = "frame too big"; return NULL; }
    }

    // worst case: every frame stored raw + ucl expansion margin
    size_t cap = (size_t)base + rawSize + rawSize / 8 + 4096 * (size_t)head->Frames;
    unsigned char* blob = (unsigned char*)xmalloc(cap);
    unsigned char* ptr = blob;

    memcpy(ptr, spr, sizeof(SPRHEAD) + head->Colors * 3);      // header + palette raw
    ptr += sizeof(SPRHEAD) + head->Colors * 3;
    XPackSprFrameInfo* finfo = (XPackSprFrameInfo*)ptr;
    ptr += head->Frames * sizeof(XPackSprFrameInfo);           // same size as SPROFFS

    for (unsigned int f = 0; f < head->Frames; f++)
    {
        const unsigned char* src = frameData + offs[f].Offset;
        unsigned int srcLen = offs[f].Length;
        if (srcLen >= 256)
        {
            ucl_uint outLen = (ucl_uint)(cap - (ptr - blob));
            int r = ucl_nrv2b_99_compress(src, srcLen, ptr, &outLen, NULL, 10, NULL, NULL);
            if (r != UCL_E_OK) { free(blob); *why = "compress failed"; return NULL; }
            finfo[f].lCompressSize = (long)outLen;
            finfo[f].lSize = (long)srcLen;                     // positive => compressed
            ptr += outLen;
        }
        else
        {
            memcpy(ptr, src, srcLen);
            finfo[f].lCompressSize = (long)srcLen;
            finfo[f].lSize = -(long)srcLen;                    // negative => raw
            ptr += srcLen;
        }
    }

    unsigned int total = (unsigned int)(ptr - blob);
    if (total > FLAG_SIZE_MASK) { free(blob); *why = "blob > 16 MiB flag limit"; return NULL; }
    *pOutSize = total;
    return blob;
}

// Replay XPackFile::GetSprFrame on the new blob and compare with the original.
static int verify_frame_mode(const unsigned char* blob, unsigned int blobSize,
                             const unsigned char* spr, unsigned int rawSize)
{
    const SPRHEAD* head = (const SPRHEAD*)spr;
    unsigned int lead = sizeof(SPRHEAD) + head->Colors * 3;
    if (blobSize < lead + head->Frames * sizeof(XPackSprFrameInfo)) return 0;
    if (memcmp(blob, spr, lead) != 0) return 0;                // header + palette

    const XPackSprFrameInfo* finfo = (const XPackSprFrameInfo*)(blob + lead);
    const SPROFFS* offs = (const SPROFFS*)(spr + lead);
    const unsigned char* origFrames = spr + lead + head->Frames * sizeof(SPROFFS);
    unsigned int cur = lead + head->Frames * sizeof(XPackSprFrameInfo);

    for (unsigned int f = 0; f < head->Frames; f++)
    {
        long ls = finfo[f].lSize;
        unsigned int want = (unsigned int)(ls < 0 ? -ls : ls);
        if (want != offs[f].Length) return 0;
        if (cur + (unsigned int)finfo[f].lCompressSize > blobSize) return 0;

        const unsigned char* orig = origFrames + offs[f].Offset;
        if (ls < 0)
        {
            if ((unsigned int)finfo[f].lCompressSize != want) return 0;
            if (memcmp(blob + cur, orig, want) != 0) return 0;
        }
        else
        {
            unsigned char* tmp = (unsigned char*)xmalloc(want);
            ucl_uint outLen = want;
            int r = ucl_nrv2b_decompress_8(blob + cur, (ucl_uint)finfo[f].lCompressSize,
                                           tmp, &outLen, NULL);
            int ok = (r == UCL_E_OK && outLen == want && memcmp(tmp, orig, want) == 0);
            free(tmp);
            if (!ok) return 0;
        }
        cur += (unsigned int)finfo[f].lCompressSize;
    }
    return cur == blobSize;
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf("usage: repack_typeframe <in.pak> <out.pak> [threshold_bytes=65536]\n");
        return 1;
    }
    unsigned int threshold = (argc > 3) ? (unsigned int)strtoul(argv[3], NULL, 10) : 65536;

    // ucl-1.01's ucl_init() is a compile-time sanity suite written for year-2000
    // compilers; on MSVC 2022 some of its asserts trip even though the codec is
    // fine. Replace it with a FUNCTIONAL self-test: compress + decompress a
    // pattern buffer and require byte identity. (Every converted entry is later
    // round-trip verified too, so a broken codec cannot slip through anyway.)
    if (ucl_init() != UCL_E_OK)
        fprintf(stderr, "note: ucl_init self-checks failed (old library vs new compiler); running functional self-test instead\n");
    {
        unsigned int n = 300000, i;
        unsigned char* sample = (unsigned char*)xmalloc(n);
        for (i = 0; i < n; i++)
            sample[i] = (unsigned char)((i * 31) ^ (i >> 7) ^ ((i / 1000) * 17));
        unsigned char* compBuf = (unsigned char*)xmalloc(n + n / 8 + 256);
        ucl_uint compLen = n + n / 8 + 256;
        if (ucl_nrv2b_99_compress(sample, n, compBuf, &compLen, NULL, 10, NULL, NULL) != UCL_E_OK)
            die("self-test: compress failed");
        unsigned char* backBuf = (unsigned char*)xmalloc(n);
        ucl_uint backLen = n;
        if (ucl_nrv2b_decompress_8(compBuf, compLen, backBuf, &backLen, NULL) != UCL_E_OK ||
            backLen != n || memcmp(sample, backBuf, n) != 0)
            die("self-test: round-trip mismatch - codec unusable on this build");
        printf("codec self-test OK (%u -> %u -> %u bytes)\n", n, (unsigned)compLen, (unsigned)backLen);
        free(sample); free(compBuf); free(backBuf);
    }

    FILE* fin = fopen(argv[1], "rb");
    if (!fin) die("cannot open input pak");
    XPackFileHeader hdr;
    if (fread(&hdr, 1, sizeof(hdr), fin) != sizeof(hdr)) die("short header");
    if (memcmp(hdr.cSignature, "PACK", 4) != 0) die("not a PACK file");
    if (hdr.uCount == 0 || hdr.uCount > 2000000) die("bad entry count");

    XPackIndexInfo* idx = (XPackIndexInfo*)xmalloc(hdr.uCount * sizeof(XPackIndexInfo));
    if (fseek(fin, hdr.uIndexTableOffset, SEEK_SET) != 0) die("seek index");
    if (fread(idx, sizeof(XPackIndexInfo), hdr.uCount, fin) != hdr.uCount) die("short index");

    XPackIndexInfo* newIdx = (XPackIndexInfo*)xmalloc(hdr.uCount * sizeof(XPackIndexInfo));
    memcpy(newIdx, idx, hdr.uCount * sizeof(XPackIndexInfo));

    FILE* fout = fopen(argv[2], "wb");
    if (!fout) die("cannot open output pak");
    XPackFileHeader outHdr;
    memset(&outHdr, 0, sizeof(outHdr));
    fwrite(&outHdr, 1, sizeof(outHdr), fout);   // placeholder
    unsigned long outOffset = sizeof(outHdr);

    unsigned int nConverted = 0, nCopied = 0, nSkipped = 0;
    int bTooManyEntries = (hdr.uCount > 65535);  // WORD node index limit (R4)
    if (bTooManyEntries)
        printf("WARN: %lu entries > 65535 - frame conversion disabled, copy only\n", hdr.uCount);

    for (unsigned long i = 0; i < hdr.uCount; i++)
    {
        unsigned int compSize = (unsigned int)(idx[i].lCompressSizeFlag & FLAG_SIZE_MASK);
        unsigned int method = ((unsigned long)idx[i].lCompressSizeFlag) >> 24;
        unsigned char* comp = (unsigned char*)xmalloc(compSize);
        if (fseek(fin, idx[i].uOffset, SEEK_SET) != 0) die("seek entry");
        if (compSize && fread(comp, 1, compSize, fin) != compSize) die("short entry");

        unsigned char* outBlob = comp;          // default: pass-through
        unsigned int outSize = compSize;
        long outFlag = idx[i].lCompressSizeFlag;
        unsigned char* raw = NULL;
        unsigned char* frameBlob = NULL;

        // Source may be 0x01 (TYPE_UCL) or 0x20 (VNG label, same UCL stream -
        // updatejx14/15 use it for nearly every entry). Output flag is always
        // 0x11: GetSprFrame masks with TYPE_METHOD_FILTER (0x0f) so a
        // 0x30-style flag would decode as TYPE_NONE and break (risk R2).
        if (!bTooManyEntries && (method == TYPE_UCL || method == TYPE_UCL_VNG) &&
            idx[i].lSize > 0 &&
            (unsigned int)idx[i].lSize >= threshold && threshold > 0)
        {
            raw = decompress_ucl(comp, compSize, (unsigned int)idx[i].lSize);
            if (raw)
            {
                const char* why = "";
                unsigned int fbSize = 0;
                frameBlob = encode_frame_mode(raw, (unsigned int)idx[i].lSize, &fbSize, &why);
                if (frameBlob)
                {
                    if (!verify_frame_mode(frameBlob, fbSize, raw, (unsigned int)idx[i].lSize))
                    {
                        fprintf(stderr, "FATAL: verify failed on entry %lu (id %08lx)\n",
                                i, idx[i].uId);
                        return 2;               // never ship an unverified conversion
                    }
                    outBlob = frameBlob;
                    outSize = fbSize;
                    outFlag = (long)(fbSize | ((unsigned long)(TYPE_FRAME | TYPE_UCL) << 24));
                    nConverted++;
                    if (outSize > compSize) g_BytesSaved -= (outSize - compSize);
                    else                    g_BytesSaved += (compSize - outSize);
                }
                else
                {
                    if (why[0] && strcmp(why, "no SPR magic") != 0 && strcmp(why, "Frames < 2") != 0)
                        printf("skip entry %lu (id %08lx): %s\n", i, idx[i].uId, why);
                    nSkipped++;
                }
            }
            else
            {
                printf("skip entry %lu (id %08lx): whole-file decompress failed\n", i, idx[i].uId);
                nSkipped++;
            }
        }
        else
            nCopied++;

        if (outBlob == comp && raw)             // counted as skipped, still a copy
            ;

        fwrite(outBlob, 1, outSize, fout);
        newIdx[i].uOffset = outOffset;
        newIdx[i].lCompressSizeFlag = outFlag;  // lSize + uId unchanged
        outOffset += outSize;

        free(comp);
        if (raw) free(raw);
        if (frameBlob) free(frameBlob);

        if ((i % 2000) == 1999)
            printf("... %lu / %lu\n", i + 1, hdr.uCount);
    }

    unsigned long indexOffset = outOffset;
    fwrite(newIdx, sizeof(XPackIndexInfo), hdr.uCount, fout);

    memcpy(outHdr.cSignature, "PACK", 4);
    outHdr.uCount = hdr.uCount;
    outHdr.uIndexTableOffset = indexOffset;
    outHdr.uDataOffset = sizeof(outHdr);
    outHdr.uCrc32 = 0;                          // engine never checks it
    fseek(fout, 0, SEEK_SET);
    fwrite(&outHdr, 1, sizeof(outHdr), fout);
    fclose(fout);
    fclose(fin);

    printf("done: %lu entries, %u converted to TYPE_FRAME, %u copied, %u skipped\n",
           hdr.uCount, nConverted, nCopied, nSkipped);
    printf("compressed payload delta: %s%llu bytes\n",
           (long long)g_BytesSaved >= 0 ? "-" : "+",
           (unsigned long long)((long long)g_BytesSaved >= 0 ? g_BytesSaved : -(long long)g_BytesSaved));
    return 0;
}
