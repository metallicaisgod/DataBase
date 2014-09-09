#define PICTFILE_BMP 0
#define PICTFILE_JPG 1
namespace CDICommon
{
bool SaveBMP(unsigned char *pPixels, LPBITMAPINFO pbmi,  const char* strFileName);
bool SaveJPG(unsigned char *pPixels, LPBITMAPINFO pbmi,  int inJPEGQual, const char* strFileName);
}