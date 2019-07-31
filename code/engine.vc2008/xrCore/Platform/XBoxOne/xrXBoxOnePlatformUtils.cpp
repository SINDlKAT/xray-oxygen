//////////////////////////////////////////////////////////////
// Desc		: XBox One miscellaneous functions
// Author	: ForserX
//////////////////////////////////////////////////////////////
// Oxygen Engine 2016-2019
//////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "xrXBoxOnePlatformUtils.h"

XRCORE_API xrXBoxOnePlatformUtils PlatformUtils;

LPCSTR xrXBoxOnePlatformUtils::GetName()
{
	return "XBox-One";
}

int xrXBoxOnePlatformUtils::ShowCursor(bool bShowCursor)
{
	return ::ShowCursor(bShowCursor);
}

const char* xrXBoxOnePlatformUtils::GetRender()
{
	return "xrRender_R4_X.dll";
}
