#include "stdafx.h"
#include "stl_file.h"

namespace CDICommon
{

int SaveSTLFile(char *inFile, char* inInfo, bool isBin, int inNum, float* inTriangles, float* inNormals)
{
	FILE *file = fopen(inFile, "wb");
	if(file==NULL)
		return 1;

	fclose(file);
	return 1;
};

int LoadSTLFile(char *inFile, char* outInfo, bool isBin, int* outNum, float* outTriangles, float* outNormals)
{
	FILE *f = fopen(inFile, "rb");
	if(f==NULL)
		return 1;

	char cSkip1[80], cSkip2[2];

	// only binary yet
	if(!isBin)
		return 3;

	// read the STL model
	// read intro
	if(outInfo)
		fread(outInfo, 80, 1, f); 
	else
		fread(cSkip1, 80, 1, f); 
	
	// read number of elements
	DWORD dwNum=0;
	fread(&dwNum, 4, 1, f);
	*outNum = dwNum;

	// if need only size
	if(outTriangles==NULL || outNormals==NULL)
	{
		fclose (f);
		return 0;
	}
	
	// get normals and triangles
	for(DWORD i=0; i<dwNum; i++)
	{
		// read normal
		fread(&outNormals[3*i], 4, 3, f);

		// read triangle
		fread(&outTriangles[9*i],   4, 3, f);
		fread(&outTriangles[9*i+3], 4, 3, f);
		fread(&outTriangles[9*i+6], 4, 3, f);

		// skip 2 bytes
		fread(cSkip2, 2, 1, f);
	}

	// are there errors?
	if(ferror(f))
	{
		fclose (f);
		return 2;
	}
	
	fclose (f);
	return 0;

};
}