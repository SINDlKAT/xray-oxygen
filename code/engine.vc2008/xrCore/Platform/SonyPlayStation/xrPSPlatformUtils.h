#pragma once
#include "../xrPlatformUtils.h"

class XRCORE_API xrSonyPSPlatformUtils : public xrGenericPlatformUtils
{
public:
	virtual LPCSTR GetName() override;
	virtual int ShowCursor(bool bShowCursor) override;
	virtual const char* GetRender() override;

	xrSonyPSPlatformUtils& operator=(const xrSonyPSPlatformUtils& Other) = delete;
};

extern XRCORE_API xrSonyPSPlatformUtils PlatformUtils;
