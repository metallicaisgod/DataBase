#pragma once

using namespace std;

namespace CDICommon
{
	typedef basic_string<char> cdi_str;

	cdi_str& LoadCdiStr(cdi_str& inStr, int inID);
	cdi_str LoadCdiStr(int inID);
	void BuildFilterStr(int *pIDs, int id_size, char *pFilterStr);
	cdi_str& operator+=(cdi_str& str, long inN);
	cdi_str operator+(cdi_str& str, long inN);
	cdi_str GetDirectionString(int inDir);

};

