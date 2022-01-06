
/*
D3D11_FILL_MODE FillMode;
D3D11_CULL_MODE CullMode;
BOOL FrontCounterClockwise;
INT DepthBias;
FLOAT DepthBiasClamp;
FLOAT SlopeScaledDepthBias;
BOOL DepthClipEnable;
BOOL ScissorEnable;
BOOL MultisampleEnable;
BOOL AntialiasedLineEnable;
*/




RasterizerState Rasterizer_Wireframe
{
	FillMode = Wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState Rasterizer_Solid
{
	FillMode = Solid;
CullMode = Back;
FrontCounterClockwise = false;
};

RasterizerState Rasterizer_Sky
{
	FillMode = Solid;
CullMode = Front;
FrontCounterClockwise = false;
};



/*
BOOL DepthEnable;
D3D11_DEPTH_WRITE_MASK DepthWriteMask;
D3D11_COMPARISON_FUNC DepthFunc;
BOOL StencilEnable;
UINT8 StencilReadMask;
UINT8 StencilWriteMask;
D3D11_DEPTH_STENCILOP_DESC FrontFace;
D3D11_DEPTH_STENCILOP_DESC BackFace;
*/
DepthStencilState DepthStecil_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less;
};

DepthStencilState  DepthStecil_NotZTest
{ 
	DepthEnable = false;
	DepthWriteMask = all;
	DepthFunc = always;
};

DepthStencilState  DepthStecil_NotZWrite
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
};

DepthStencilState  DepthStecil_NotZTestWrite
{
	DepthEnable = false;
	DepthWriteMask = zero;
	DepthFunc = always;
};


/*
BOOL BlendEnable;
D3D11_BLEND SrcBlend;
D3D11_BLEND DestBlend;
D3D11_BLEND_OP BlendOp;
D3D11_BLEND SrcBlendAlpha;
D3D11_BLEND DestBlendAlpha;
D3D11_BLEND_OP BlendOpAlpha;
UINT8 RenderTargetWriteMask;
desc.RenderTarget[0].BlendEnable = true;
desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
*/

BlendState Blend_None
{
	BlendEnable[0] = false;
};

BlendState Blend_Alpha
{
	BlendEnable[0] = true;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
	// RenderTargetWriteMask[0] = 0x0F;
};

BlendState Blend_One
{
	BlendEnable[0] = true;
	SrcBlend = One;
	DestBlend = One;
	BlendOp = Add;
};



