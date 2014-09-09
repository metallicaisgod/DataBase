
namespace CDICommon
{
	BOOL ReadStringReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, LPSTR outData, DWORD* size);
	BOOL ReadDWORDReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, DWORD* outData);
	BOOL ReadBINReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, VOID* outData, DWORD* size);
	
	BOOL WriteStringReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, LPCSTR inData);
	BOOL WriteDWORDReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, DWORD* inData);
	BOOL WriteBINReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, VOID* inData, DWORD size);


} //end of namespace CDICommon