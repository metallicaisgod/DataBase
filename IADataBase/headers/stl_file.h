namespace CDICommon
{
int SaveSTLFile(char *inFile, char* inInfo, bool isBin, int inNum, float* inTriangles, float* inNormals);
int LoadSTLFile(char *inFile, char* outInfo, bool isBin, int* outNum, float* outTriangles, float* outNormals);
}