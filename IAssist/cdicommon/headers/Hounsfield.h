#pragma once
//#include "slice.h"
#define DEMO_INTERCEPT 1242
//#include "../../ctassist/CTAssist.h"


typedef USHORT CTA_PIXEL; 
typedef CTA_PIXEL* PCTA_PIXEL;

//extern CCTAssistApp g_theApp;

#define HOUNS_PROT_MASK_1 0x0176AC54
#define HOUNS_PROT_MASK_2 0x801C0EEC
#define HOUNS_PROT_MASK_3 0x0D266165
#define HOUNS_PROT_MASK_4 0x15C4E4EF

#define HOUNS_PROT_MASK_5 0x16FE46E8
#define HOUNS_PROT_MASK_6 0x06202631
#define HOUNS_PROT_MASK_7 0x67185933
#define HOUNS_PROT_MASK_8 0x364ED138	

#define HOUNS_PROT_MASK_01 0x44968DD1
#define HOUNS_PROT_MASK_02 0x3CD8C092
#define HOUNS_PROT_MASK_03 0x3100F538
#define HOUNS_PROT_MASK_04 0x3373BE3E

#define HOUNS_PROT_MASK_05 0x170167C2
#define HOUNS_PROT_MASK_06 0x48D0B637
#define HOUNS_PROT_MASK_07 0x22F6B73C
#define HOUNS_PROT_MASK_08 0x170167C2

class Hounsfield
{
public:
	Hounsfield(void);
	virtual ~Hounsfield(void);

	// Housfeld->Brightness conversion
	//static double Br2Hns(const CTA_PIXEL& inPix);
	
	// Brightness->Housfeld conversion
	//static CTA_PIXEL Hns2Br(double inHouns);
	//static double Hns2BrD(double inHouns);
	
	// Conversion data	
	static double s_RescSlope;
	static int s_shift;
	static double s_RescIntercept;


	 template<ULONG msk>
	 static inline CTA_PIXEL Hns2Br(double inHouns)
	 {
		 return CTA_PIXEL(int((inHouns-s_RescIntercept)/s_RescSlope)<<s_shift); 
	 };

	 template<ULONG msk>
	 static inline double Br2Hns(const CTA_PIXEL& inPix)
	 {
		return (s_RescSlope*double(inPix>>(s_shift)) + s_RescIntercept);
	 };

};

struct Tissue
{
	char szName[256];
	double MinHouns;
	double MaxHouns;
};


//#ifdef IA_DEMO
//inline double Hounsfield::Br2Hns_Demo(const CTA_PIXEL& inPix, ULONG msk)
//{
//	//return double(inPix-DEMO_INTERCEPT);
//	return double(inPix - 1024);
//}
//#endif
//
//#ifdef IA_DEMO
//inline CTA_PIXEL Hounsfield::Hns2Br_Demo(double inHouns, ULONG msk)
//{
//	//return CTA_PIXEL(inHouns+DEMO_INTERCEPT);
//	return unsigned short(inHouns+1024);
//}
//#endif 
