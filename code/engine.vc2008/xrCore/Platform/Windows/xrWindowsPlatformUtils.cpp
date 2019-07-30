//////////////////////////////////////////////////////////////
// Desc		: Windows miscellaneous functions
// Author	: Giperion, ForserX
//////////////////////////////////////////////////////////////
// Oxygen Engine 2016-2019
//////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "xrWindowsPlatformUtils.h"

XRCORE_API xrWindowsPlatformUtils PlatformUtils;

LPCSTR xrWindowsPlatformUtils::GetName()
{
	return "Windows-x64";
}

int xrWindowsPlatformUtils::ShowCursor(bool bShowCursor)
{
	return ::ShowCursor(bShowCursor);
}

const char* xrWindowsPlatformUtils::GetRender()
{
	return "xrRender_R4.dll";
}
