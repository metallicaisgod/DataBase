#include "stdafx.h"

int g_scrSizeMM_x = 0;
int g_scrSizeMM_y = 0;
int g_scrSizePIX_x = 0;
int g_scrSizePIX_y = 0;
double g_scrRes_x = 0.;
double g_scrRes_y = 0.;

namespace CDICommon
{
void GetScreenSizePIX(int& h, int& v)
{
	if(g_scrSizePIX_x==0 || g_scrSizePIX_y==0)
	{
		HDC hScrDC = ::GetDC(NULL);
		g_scrSizePIX_x = GetDeviceCaps(hScrDC, HORZRES);
		g_scrSizePIX_y = GetDeviceCaps(hScrDC, VERTRES);
		::ReleaseDC(NULL, hScrDC);
	}
	h = g_scrSizePIX_x;
	v = g_scrSizePIX_y;

};
void GetScreenSizeMM(int& h, int& v)
{
	if(g_scrSizeMM_x==0 || g_scrSizeMM_y==0)
	{
		HDC hScrDC = ::GetDC(NULL);
		g_scrSizeMM_x = GetDeviceCaps(hScrDC, HORZSIZE);
		g_scrSizeMM_y = GetDeviceCaps(hScrDC, VERTSIZE);
		::ReleaseDC(NULL, hScrDC);
	}
	h = g_scrSizeMM_x;
	v = g_scrSizeMM_y;
};
void GetScreenResMM_PIX(double& h /*mm/pix*/, double& v)
{
	if(g_scrRes_x==0. || g_scrRes_y==0.)
	{
		HDC hScrDC = ::GetDC(NULL);
		g_scrRes_x = double(GetDeviceCaps(hScrDC, HORZSIZE))/double(GetDeviceCaps(hScrDC, HORZRES));
		g_scrRes_y= double(GetDeviceCaps(hScrDC, VERTSIZE))/double(GetDeviceCaps(hScrDC, VERTRES));
		::ReleaseDC(NULL, hScrDC);
	}
	h = g_scrRes_x;
	v = g_scrRes_y;
};

void GetScreenResPIX_MM(double& h /*pix/mm*/, double& v)
{
	if(g_scrRes_x==0. || g_scrRes_y==0.)
	{
		HDC hScrDC = ::GetDC(NULL);
		g_scrRes_x = double(GetDeviceCaps(hScrDC, HORZSIZE))/double(GetDeviceCaps(hScrDC, HORZRES));
		g_scrRes_y= double(GetDeviceCaps(hScrDC, VERTSIZE))/double(GetDeviceCaps(hScrDC, VERTRES));
		::ReleaseDC(NULL, hScrDC);
	}
	h = 1./g_scrRes_x;
	v = 1./g_scrRes_y;
};

} //end of namespace CDICommon
