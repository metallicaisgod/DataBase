#include "StdAfx.h"

#include "cdi_str.h"
#include "hounsfield.h"

//#include "../../inc/license/cdi_lic_file.h"

//extern CCTAssistApp g_theApp;


double Hounsfield::s_RescSlope = 0.;
int Hounsfield::s_shift = 0;
double Hounsfield::s_RescIntercept = 0.;

Hounsfield::Hounsfield(void)
{
}

Hounsfield::~Hounsfield(void)
{
}

//#ifdef IA_VIEWER
//double Hounsfield::Br2Hns_Viewer(const CTA_PIXEL& inPix, ULONG msk)
//{
//	return ((s_RescSlope-576.)*double(inPix>>(s_shift-18)) + (s_RescIntercept-768.0));
//};
//#endif

//#ifdef IA_DEMO
//double Hounsfield::Br2Hns_Demo(const CTA_PIXEL& inPix, ULONG msk)
//{
//	//return double(inPix-DEMO_INTERCEPT);
//	return double(pix - 1024);
//}
//#endif

//#ifdef CDI_PROTECTION
//void Hounsfield::Br2Hns_Prot(float *houns, ULONG msk)
//{
//	ULONG *pl = (ULONG *)houns;
//	ULONG mask =CDI_FUNC_PARAM_1^(msk- (msk>>(0xF&CDI_FUNC_PARAM_1+1)));
//	*pl^=mask;
//};
//#endif // CDI_PROTECTION
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef IA_VIEWER
//CTA_PIXEL Hounsfield::Hns2Br_Viewer(double inHouns, ULONG msk)
//{
//	return CTA_PIXEL(int((inHouns-(s_RescIntercept-768.))/(s_RescSlope-576.))<<(s_shift-18));
//}
//#endif 

//#ifdef IA_DEMO
//CTA_PIXEL Hounsfield::Hns2Br_Demo(double inHouns, ULONG msk)
//{
//	//return CTA_PIXEL(inHouns+DEMO_INTERCEPT);
//	return unsigned short(houns+1024);
//}
//#endif 
//
//#ifdef CDI_PROTECTION
//void Hounsfield::Hns2Br_Prot(ULONG *pix, ULONG msk)
//{
//	ULONG mask =CDI_FUNC_PARAM_0^(msk-(msk>>(0xF&CDI_FUNC_PARAM_0+1)));
//	*pix^=mask;
//}
//#endif // CDI_PROTECTION
//	

	