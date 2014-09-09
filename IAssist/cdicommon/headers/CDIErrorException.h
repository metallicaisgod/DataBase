
#pragma once

namespace CDICommon
{

class CDIErrorException
{
public:
	CDIErrorException(cdi_str inErr) { m_strErr = inErr; };
	~CDIErrorException(){};
	cdi_str GetErrorText(){ return m_strErr; }

private:
	cdi_str m_strErr;
};

class CDIBRepException
{
public:
	CDIBRepException(cdi_str inSTL, cdi_str inEPS) { m_strEPS = inEPS; m_strSTL = inSTL;};
	~CDIBRepException(){};
	cdi_str GetEpsText(){ return m_strEPS; }
	cdi_str GetStlText(){ return m_strSTL; }

private:
	cdi_str m_strEPS;
	cdi_str m_strSTL;
};

} //end of namespace CDICommon