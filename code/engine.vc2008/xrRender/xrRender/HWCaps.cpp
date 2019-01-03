#include "stdafx.h"
#pragma hdrstop

#include "hwcaps.h"
#include "hw.h"

#include "NVAPI/nvapi.h"
#include "amd/amd_ags.h"

#pragma comment(lib, "amd_ags.lib")

namespace
{
u32 GetNVGpuNum()
{
	NvLogicalGpuHandle  logicalGPUs	[NVAPI_MAX_LOGICAL_GPUS];
	NvU32               logicalGPUCount;
	NvPhysicalGpuHandle physicalGPUs[NVAPI_MAX_PHYSICAL_GPUS];
	NvU32               physicalGPUCount;

	int iGpuNum = 0;

	NvAPI_Status	status;
	status = NvAPI_Initialize();

	if (status != NVAPI_OK)
	{
		Msg("* NVAPI is missing.");
		return iGpuNum;
	}

	// enumerate logical gpus
	status = NvAPI_EnumLogicalGPUs(logicalGPUs, &logicalGPUCount);
	if (status != NVAPI_OK)
	{
		Msg("* NvAPI_EnumLogicalGPUs failed!");
		return iGpuNum;
		// error
	}

	// enumerate physical gpus
	status = NvAPI_EnumPhysicalGPUs(physicalGPUs, &physicalGPUCount);
	if (status != NVAPI_OK)
	{
		Msg("* NvAPI_EnumPhysicalGPUs failed!");
		return iGpuNum;
		// error
	}

	Msg	("* NVidia MGPU: Logical(%d), Physical(%d)", physicalGPUCount, logicalGPUCount);

	//	Assume that we are running on logical GPU with most physical GPUs connected.
	for ( u32 i = 0; i<logicalGPUCount; ++i )
	{
		status = NvAPI_GetPhysicalGPUsFromLogicalGPU( logicalGPUs[i], physicalGPUs, &physicalGPUCount);
		if (status == NVAPI_OK)
			iGpuNum = std::max( iGpuNum, (int)physicalGPUCount);
	}

	if (iGpuNum>1)
	{
		Msg	("* NVidia MGPU: %d-Way SLI detected.", iGpuNum);
	}

	return iGpuNum;
}

u32 GetATIGpuNum()
{
	AGSContext *ags				= nullptr;
	AGSGPUInfo gpuInfo			= {};
	AGSConfiguration *config	= nullptr;
	AGSReturnCode status		= agsInit(&ags, config ,&gpuInfo);
	if (status != AGS_SUCCESS)
	{
		Msg						("! AGS: Initialization failed (%d)", status);
		return 0;
	}
	int crossfireGpuCount		= 1;
	status						= agsGetCrossfireGPUCount(ags, &crossfireGpuCount);
	if (status != AGS_SUCCESS)
	{
		Msg						("! AGS: Unable to get CrossFire GPU count (%d)", status);
		agsDeInit				(ags);
		return 1;
	}
	Msg							("* AGS: CrossFire GPU count: %d", crossfireGpuCount);
	agsDeInit					(ags);
	return crossfireGpuCount;
}

u32 GetGpuNum()
{
	u32 res						= GetNVGpuNum();
	res							= std::max(res, GetATIGpuNum());
	res							= std::max(res, 2u);

	if (!res)
	{
		Msg("* Can't detect graphic card. Assuming that you have at least one (maybe from intel)");
		res = 1;
	}

	Msg("* Starting rendering as %d-GPU.", res);
	
	return res;
}
}


void CHWCaps::Update()
{
	// ***************** GEOMETRY
	geometry_major				= 4;
	geometry_minor				= 0;
	geometry.bSoftware			= FALSE;
	geometry.bPointSprites		= FALSE;
	geometry.bNPatches			= FALSE;
	DWORD cnt					= 256;
	clamp<DWORD>(cnt,0,256);
	geometry.dwRegisters		= cnt;
	geometry.dwInstructions		= 256;
	geometry.dwClipPlanes		= std::min(6,15);
	geometry.bVTF				= HW.IsFormatSupported(DXGI_FORMAT_R32_FLOAT);

	// ***************** PIXEL processing
	raster_major				= 4;
	raster_minor				= 0;
	raster.dwStages				= 16;
	raster.bNonPow2				= TRUE;
	raster.bCubemap				= TRUE;
	raster.dwMRT_count			= 4;
	raster.b_MRT_mixdepth		= TRUE;
	raster.dwInstructions		= 256;

	// ***************** Info
	Msg							("* GPU shading: vs(%x/%d.%d/%d), ps(%x/%d.%d/%d)",
		0,	geometry_major, geometry_minor, CAP_VERSION(geometry_major,	geometry_minor),
		0,	raster_major,	raster_minor,	CAP_VERSION(raster_major,	raster_minor)
		);

	// *******1********** Vertex cache
	//	TODO: DX10: Find a way to detect cache size
	geometry.dwVertexCache = 24;
	Msg					("* GPU vertex cache: %s, %d","unrecognized",u32(geometry.dwVertexCache));

	// *******1********** Compatibility : vertex shader
	if (0==raster_major)		geometry_major=0;		// Disable VS if no PS

	//
	bTableFog			= FALSE;

	// Detect if stencil available
	bStencil			= TRUE;

	// Scissoring
	bScissor			= TRUE;

	// Stencil relative caps
	soInc				= D3DSTENCILOP_INCRSAT;
	soDec				= D3DSTENCILOP_DECRSAT;
	dwMaxStencilValue=(1<<8)-1;

	// DEV INFO

	iGPUNum = GetGpuNum();
}

