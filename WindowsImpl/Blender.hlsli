
BlendState NoBlend
{
	AlphaToCoverageEnable = false;
	BlendEnable[0] = false;
};

BlendState NormalAlphaBlend
{
	//dstRGB = (srcRGB * srcA) + (dstRGB * (1-srcA))
	//dstA = srcA + (dstA * (1-srcA))
	AlphaToCoverageEnable = false;
	BlendEnable[0] = true;
	SrcBlend[0] = SRC_ALPHA;
	DestBlend[0] = INV_SRC_ALPHA;
	BlendOp[0] = ADD;
	SrcBlendAlpha[0] = ONE;
	DestBlendAlpha[0] = INV_SRC_ALPHA;
	BlendOpAlpha[0] = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

float4 NormalAlphaBlendFunc(float4 src, float4 dst)
{
	float4 ret;
	ret.xyz = src.xyz * src.w + (1-src.w) * dst.xyz;
	ret.w = src.w + (dst.w * (1 - src.w));
	return ret;
}

BlendState AddtiveAlphaBlend
{
	//dstRGB = (srcRGB * srcA) + dstRGB
	//dstA = dstA
	AlphaToCoverageEnable = false;
	BlendEnable[0] = true;
	SrcBlend[0] = SRC_ALPHA;
	DestBlend[0] = ONE;
	BlendOp[0] = ADD;
	SrcBlendAlpha[0] = ZERO;
	DestBlendAlpha[0] = ONE;
	BlendOpAlpha[0] = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

BlendState ModulateAlphaBlend
{
	//dstRGB = srcRGB * dstRGB
	//dstA = dstA
	AlphaToCoverageEnable = false;
	BlendEnable[0] = true;
	SrcBlend[0] = DEST_COLOR;
	DestBlend[0] = ZERO;
	BlendOp[0] = ADD;
	SrcBlendAlpha[0] = ZERO;
	DestBlendAlpha[0] = ONE;
	BlendOpAlpha[0] = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};