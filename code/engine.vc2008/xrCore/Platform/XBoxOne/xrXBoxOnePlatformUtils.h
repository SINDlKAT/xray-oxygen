#pragma once
#include "../xrPlatformUtils.h"

class XRCORE_API xrXBoxOnePlatformUtils : public xrGenericPlatformUtils
{
public:
	virtual LPCSTR GetName() override;
	virtual int ShowCursor(bool bShowCursor) override;
	virtual const char* GetRender() override;

	xrXBoxOnePlatformUtils& operator=(const xrXBoxOnePlatformUtils& Other) = delete;
};

extern XRCORE_API xrXBoxOnePlatformUtils PlatformUtils;
