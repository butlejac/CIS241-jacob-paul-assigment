#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
typedef unsigned long int UINT4;
typedef struct {
    UINT4 i[2];
    UINT4 buf[4];
    unsigned char in[64];
    unsigned char digest[16];
} MD5_CTX;
void MD5Init();
void MD5Update();
void MD5Final();
static void Transform();
static unsigned char PADDING[64] = {
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define FF(a, b, c, d, x, s, ac) \
  {(a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) \
  {(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) \
  {(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) \
  {(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }
void MD5Init(mdContext)
MD5_CTX *mdContext;
{
    mdContext->i[0] = mdContext->i[1] = (UINT4) 0;
    mdContext->buf[0] = (UINT4) 0x67452301;
    mdContext->buf[1] = (UINT4) 0xefcdab89;
    mdContext->buf[2] = (UINT4) 0x98badcfe;
    mdContext->buf[3] = (UINT4) 0x10325476;
}
void MD5Update(mdContext, inBuf, inLen)
MD5_CTX *mdContext;
unsigned char *inBuf;
unsigned int inLen;
{
    UINT4 in[16];
    int mdi;
    unsigned int i, ii;
    mdi = (int) ((mdContext->i[0] >> 3) & 0x3F);
    if ((mdContext->i[0] + ((UINT4) inLen << 3)) < mdContext->i[0])
        mdContext->i[1]++;
    mdContext->i[0] += ((UINT4) inLen << 3);
    mdContext->i[1] += ((UINT4) inLen >> 29);
    while (inLen--) {
        mdContext->in[mdi++] = *inBuf++;
        if (mdi == 0x40) {
            for (i = 0, ii = 0; i < 16; i++, ii += 4)
                in[i] = (((UINT4) mdContext->in[ii + 3]) << 24) |
                (((UINT4) mdContext->in[ii + 2]) << 16) |
                (((UINT4) mdContext->in[ii + 1]) << 8) |
                ((UINT4) mdContext->in[ii]);
            Transform(mdContext->buf, in);
            mdi = 0;
        }
    }
}
void MD5Final(mdContext)
MD5_CTX *mdContext;
{
    UINT4 in[16];
    int mdi;
    unsigned int i, ii;
    unsigned int padLen;
    in[14] = mdContext->i[0];
    in[15] = mdContext->i[1];
mdi = (int) ((mdContext->i[0] >> 3) & 0x3F);
    padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
    MD5Update(mdContext, PADDING, padLen);
    for (i = 0, ii = 0; i < 14; i++, ii += 4)
        in[i] = (((UINT4) mdContext->in[ii + 3]) << 24) |
        (((UINT4) mdContext->in[ii + 2]) << 16) |
        (((UINT4) mdContext->in[ii + 1]) << 8) |
        ((UINT4) mdContext->in[ii]);
    Transform(mdContext->buf, in);
    for (i = 0, ii = 0; i < 4; i++, ii += 4) {
        mdContext->digest[ii] = (unsigned char) (mdContext->buf[i] & 0xFF);
        mdContext->digest[ii + 1] =
                (unsigned char) ((mdContext->buf[i] >> 8) & 0xFF);
        mdContext->digest[ii + 2] =
                (unsigned char) ((mdContext->buf[i] >> 16) & 0xFF);
        mdContext->digest[ii + 3] =
                (unsigned char) ((mdContext->buf[i] >> 24) & 0xFF);
    }
}
static void Transform(buf, in)
UINT4 *buf;
UINT4 *in;
{
    UINT4 a = buf[0], b = buf[1], c = buf[2], d = buf[3];
#define S11 7
#define S12 12
#define S13 17
#define S14 22
    FF(a, b, c, d, in[ 0], S11, 3614090360);
    FF(d, a, b, c, in[ 1], S12, 3905402710);
    FF(c, d, a, b, in[ 2], S13, 606105819);
    FF(b, c, d, a, in[ 3], S14, 3250441966);
    FF(a, b, c, d, in[ 4], S11, 4118548399);
    FF(d, a, b, c, in[ 5], S12, 1200080426);
    FF(c, d, a, b, in[ 6], S13, 2821735955);
    FF(b, c, d, a, in[ 7], S14, 4249261313);
    FF(a, b, c, d, in[ 8], S11, 1770035416);
    FF(d, a, b, c, in[ 9], S12, 2336552879);
    FF(c, d, a, b, in[10], S13, 4294925233);
    FF(b, c, d, a, in[11], S14, 2304563134);
    FF(a, b, c, d, in[12], S11, 1804603682);
    FF(d, a, b, c, in[13], S12, 4254626195);
    FF(c, d, a, b, in[14], S13, 2792965006);
    FF(b, c, d, a, in[15], S14, 1236535329);
#define S21 5
#define S22 9
#define S23 14
#define S24 20
    GG(a, b, c, d, in[ 1], S21, 4129170786);
    GG(d, a, b, c, in[ 6], S22, 3225465664);
    GG(c, d, a, b, in[11], S23, 643717713);
    GG(b, c, d, a, in[ 0], S24, 3921069994);
    GG(a, b, c, d, in[ 5], S21, 3593408605);
    GG(d, a, b, c, in[10], S22, 38016083);
    GG(c, d, a, b, in[15], S23, 3634488961);
    GG(b, c, d, a, in[ 4], S24, 3889429448);
    GG(a, b, c, d, in[ 9], S21, 568446438);
    GG(d, a, b, c, in[14], S22, 3275163606);
    GG(c, d, a, b, in[ 3], S23, 4107603335);
    GG(b, c, d, a, in[ 8], S24, 1163531501);
    GG(a, b, c, d, in[13], S21, 2850285829);
    GG(d, a, b, c, in[ 2], S22, 4243563512);
    GG(c, d, a, b, in[ 7], S23, 1735328473);
    GG(b, c, d, a, in[12], S24, 2368359562);
#define S31 4
#define S32 11
#define S33 16
#define S34 23
    HH(a, b, c, d, in[ 5], S31, 4294588738);
    HH(d, a, b, c, in[ 8], S32, 2272392833);
    HH(c, d, a, b, in[11], S33, 1839030562);
    HH(b, c, d, a, in[14], S34, 4259657740);
    HH(a, b, c, d, in[ 1], S31, 2763975236);
    HH(d, a, b, c, in[ 4], S32, 1272893353);
    HH(c, d, a, b, in[ 7], S33, 4139469664);
    HH(b, c, d, a, in[10], S34, 3200236656);
    HH(a, b, c, d, in[13], S31, 681279174);
    HH(d, a, b, c, in[ 0], S32, 3936430074);
    HH(c, d, a, b, in[ 3], S33, 3572445317);
    HH(b, c, d, a, in[ 6], S34, 76029189);
    HH(a, b, c, d, in[ 9], S31, 3654602809);
    HH(d, a, b, c, in[12], S32, 3873151461);
    HH(c, d, a, b, in[15], S33, 530742520);
    HH(b, c, d, a, in[ 2], S34, 3299628645);
#define S41 6
#define S42 10
#define S43 15
#define S44 21
    II(a, b, c, d, in[ 0], S41, 4096336452);
    II(d, a, b, c, in[ 7], S42, 1126891415);
    II(c, d, a, b, in[14], S43, 2878612391);
    II(b, c, d, a, in[ 5], S44, 4237533241);
    II(a, b, c, d, in[12], S41, 1700485571);
    II(d, a, b, c, in[ 3], S42, 2399980690);
    II(c, d, a, b, in[10], S43, 4293915773);
    II(b, c, d, a, in[ 1], S44, 2240044497);
    II(a, b, c, d, in[ 8], S41, 1873313359);
    II(d, a, b, c, in[15], S42, 4264355552);
    II(c, d, a, b, in[ 6], S43, 2734768916);
    II(b, c, d, a, in[13], S44, 1309151649);
    II(a, b, c, d, in[ 4], S41, 4149444226);
    II(d, a, b, c, in[11], S42, 3174756917);
    II(c, d, a, b, in[ 2], S43, 718787259);
    II(b, c, d, a, in[ 9], S44, 3951481745);
    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}
char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
static char* MDPrint(mdContext)
MD5_CTX *mdContext;
{
    int i;
    int pos = 0;
    char *result = "";
    char temp[10];
    printf("\n");
    for (i = 0; i < 16; i++) {
        sprintf(&temp, "%02x", mdContext->digest[i]);
        result = concat(result, temp);
    }
    return result;
}
static char* MDString(inString)
char *inString;
{
    MD5_CTX mdContext;
    unsigned int len = strlen(inString);
    MD5Init(&mdContext);
    MD5Update(&mdContext, inString, len);
    MD5Final(&mdContext);
    char* result = MDPrint(&mdContext);
    return (result);
}
static char* MDFile(filename)
char *filename;
{
    FILE *inFile = fopen(filename, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];
    if (inFile == NULL) {
        printf("%s can't be opened.\n", filename);
        return "";
    }
    MD5Init(&mdContext);
    while ((bytes = fread(data, 1, 1024, inFile)) != 0)
        MD5Update(&mdContext, data, bytes);
    MD5Final(&mdContext);
    char* md5 = MDPrint(&mdContext);
    fclose(inFile);
    return (md5);
}
static void MDFilter() {
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[16];
    MD5Init(&mdContext);
    while ((bytes = fread(data, 1, 16, stdin)) != 0)
        MD5Update(&mdContext, data, bytes);
    MD5Final(&mdContext);
    MDPrint(&mdContext);
    printf("\n");
}
void printFileContent(const char *filepath) {
    FILE *f;
    char s;
    f = fopen(filepath, "r");
    while ((s = fgetc(f)) != EOF) {
        printf("%c", s);
    }
    printf("\n");
}
int main(int argc, char** argv) {
    char* hash_0_0 = MDFile("file1.txt");
    char* hash_0_1 = MDFile("file2.txt");
    char* hash_1_0 = MDFile("file3.txt");
    char* hash_1_1 = MDFile("file4.txt");
    char* hash_0 = MDString(concat(hash_0_0, hash_0_1));
    char* hash_1 = MDString(concat(hash_1_0, hash_1_1));
    char* topHashBeforeChange = MDString(concat(hash_0, hash_1));
    char* hash_0_0_after = MDFile("file1.txt");
    printf("file1 content: ");
    printFileContent("file1.txt");
    printf("Hash: %s", hash_0_0_after);
    char* hash_0_1_after = MDFile("file2.txt");
    printf("file2 content: ");
    printFileContent("file2.txt");
    printf("Hash: %s", hash_0_1_after);
    char* hash_1_0_after = MDFile("file3.txt");
    printf("file3 content: ");
    printFileContent("file3.txt");
    printf("Hash: %s", hash_1_0_after);
    char* hash_1_1_after = MDFile("file4.txt");
    printf("file4 content: ");
    printFileContent("file4.txt");
    printf("Hash: %s", hash_1_1_after);
    char* hash_0_after = MDString(concat(hash_0_0_after, hash_0_1_after));
    char* hash_1_after = MDString(concat(hash_1_0_after, hash_1_1_after));
    char* topHashAfterChange = MDString(concat(hash_0_after, hash_1_after));
    printf("\nTop Hash After Change: %s", topHashAfterChange);
    return (EXIT_SUCCESS);
}
