#include "stdafx.h"

#include "dxRenderDeviceRender.h"

void CRenderTarget::PhaseGammaGenerateLUT()
{
	u32 Offset = 0;
	float _w = float(Device.dwWidth);
	float _h = float(Device.dwHeight);

	Fvector2 p0, p1;

	p0.set(0.0f, 0.0f);
	p1.set(1.0f, 1.0f);

	/////// Create gamma LUT //////////////////
	u_setrt				(rt_GammaLUT, nullptr, nullptr, nullptr);
	RCache.set_Stencil	(FALSE);

	// Constants
	float brightness		= dxRenderDeviceRender::Instance().GetBrightness();
	float gamma				= dxRenderDeviceRender::Instance().GetGamma();
	float contrast			= dxRenderDeviceRender::Instance().GetContrast();
	Fvector color_grading	= dxRenderDeviceRender::Instance().GetBalance();

	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0.0f,	1.0f, 0.0f, 1.0f, 0, p0.x, p1.y); pv++;
	pv->set(0.0f,	0.0f, 0.0f, 1.0f, 0, p0.x, p0.y); pv++;
	pv->set(256.0f,	1.0f, 0.0f, 1.0f, 0, p1.x, p1.y); pv++;
	pv->set(256.0f,	0.0f, 0.0f, 1.0f, 0, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	// Draw COLOR
	RCache.set_Element	(s_gamma->E[0]);
	RCache.set_c		("color_params", brightness, gamma, contrast, 0.0f);
	RCache.set_c		("color_grading", color_grading.x, color_grading.y, color_grading.z, 0.0f);
	RCache.set_Geometry	(g_combine);
	RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::PhaseGammaApply()
{
	// Copy back buffer content to the rt_Generic_0
	ID3DResource* pTmpTexture = rt_Generic_0->pTexture->surface_get();
	HW.pBaseRT->GetResource(&pTmpTexture);
	HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), pTmpTexture);
	_RELEASE(pTmpTexture);

	RCache.set_Z(FALSE);
	RenderScreenQuad(Device.dwWidth, Device.dwHeight, HW.pBaseRT, s_gamma->E[1]);
}

#ifdef DEBUG
void CRenderTarget::SaveGammaLUT()
{
	ID3DBlob* saved = nullptr;

	ID3DResource* pSrcTexture;
	rt_GammaLUT->pRT->GetResource(&pSrcTexture);
	VERIFY(pSrcTexture);

	CHK_DX(D3DX11SaveTextureToMemory(HW.pContext, pSrcTexture, D3DX11_IFF_PNG, &saved, 0));
	_RELEASE(pSrcTexture);

	IWriter* fs = FS.w_open("$screenshots$", "GammaLUT.png");
	R_ASSERT(fs);
	fs->w(saved->GetBufferPointer(), saved->GetBufferSize());
	FS.w_close(fs);
	_RELEASE(saved);
}
#endif
