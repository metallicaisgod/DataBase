#include "stdafx.h"
#include "ijl.h"

namespace CDICommon
{
bool SaveBMP(unsigned char *pPixels, LPBITMAPINFO pbmi,   const char* szFileName)
{
	BITMAPFILEHEADER bfh;
	BOOL ok;
	DWORD bytesToWrite;
	HANDLE file;

	bfh.bfType = 0x4D42; /* 'BM' */
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	if (pbmi->bmiHeader.biBitCount <= 8)
		bfh.bfOffBits += pbmi->bmiHeader.biClrUsed*sizeof(RGBQUAD);
	if (pbmi->bmiHeader.biCompression == BI_BITFIELDS)
		bfh.bfOffBits += 3*sizeof(DWORD);
	bfh.bfSize = bfh.bfOffBits + pbmi->bmiHeader.biSizeImage;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;

    ok = ((file = CreateFile(QString(szFileName).toStdWString().c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,NULL)) != INVALID_HANDLE_VALUE);
	if (ok)
	{
		ok  = WriteFile(file,&bfh,sizeof(BITMAPFILEHEADER),&bytesToWrite,NULL);
		ok &= WriteFile(file,pbmi,sizeof(BITMAPINFOHEADER),&bytesToWrite,NULL);
		ok &= WriteFile(file,pPixels,pbmi->bmiHeader.biSizeImage,&bytesToWrite,NULL);
		CloseHandle(file);
	}

	return ok==TRUE;
}
bool SaveJPG(unsigned char *pPixels, LPBITMAPINFO pbmi, int inJPEGQual, const char* szFileName)
{
	DWORD dib_pad_bytes;
	BITMAPINFOHEADER *pbmih = &pbmi->bmiHeader;

	JPEG_CORE_PROPERTIES jpegProps;
	IJLERR ijlErr;
	ijlErr = ijlInit(&jpegProps);
	if(ijlErr != IJL_OK)
		return false;

	dib_pad_bytes = IJL_DIB_PAD_BYTES(pbmih->biWidth,3);
	jpegProps.DIBChannels = 3;
	jpegProps.DIBColor = IJL_BGR;
	jpegProps.JPGChannels = 3;
	jpegProps.JPGColor = IJL_YCBCR;
	jpegProps.JPGSubsampling = IJL_422; // 4:2:2 subsampling.
	

	// Set up information to write from the pixel buffer.
	jpegProps.DIBWidth = pbmih->biWidth;
	jpegProps.DIBHeight = -1*pbmih->biHeight; // Implies a bottom-up DIB.
	jpegProps.DIBBytes = pPixels;

	jpegProps.DIBPadBytes = dib_pad_bytes;
	jpegProps.JPGFile = szFileName;
	jpegProps.JPGWidth = pbmih->biWidth;
	jpegProps.JPGHeight = pbmih->biHeight;
	jpegProps.jquality = 100-inJPEGQual*10; // Select "good" image quality

	// evil copypaste - convert 32 to 24
	HGLOBAL hBGR24=NULL;
	if(pbmih->biBitCount == 32) 
	{
		hBGR24 = GlobalAlloc(GHND,((pbmih->biWidth*24+31)/32)*4*abs(pbmih->biHeight));
		BYTE* pBGR24 = reinterpret_cast<BYTE*>(GlobalLock(hBGR24));
		jpegProps.DIBBytes = pBGR24;

		BYTE* pBGR032 = pPixels;
		for(int j=0;j<abs(pbmih->biHeight);j++) 
		{
			for(int i=0;i<pbmih->biWidth;i++) 
			{
				pBGR24[i*3] = pBGR032[i*4];
				pBGR24[i*3+1] = pBGR032[i*4+1];
				pBGR24[i*3+2] = pBGR032[i*4+2];
			}
			pBGR24 += ((pbmih->biWidth*24+31)/32)*4;
			pBGR032 += ((pbmih->biWidth*32+31)/32)*4;
		}
	}

	////////////
	ijlErr = ijlWrite(&jpegProps,IJL_JFILE_WRITEWHOLEIMAGE);
	//								size = jpegProps.JPGSizeBytes;
	ijlFree(&jpegProps);
	if(hBGR24 != NULL) 
	{
		GlobalUnlock(hBGR24);
		GlobalFree(hBGR24);
	}

	if(ijlErr != IJL_OK)
		return false;
	return true;
};

}
