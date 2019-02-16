
struct SpriteVSIn
{
	float2 Position : POSITION;
	float2 Size : SIZE;
	float4 Color : COLOR;
	float2 Center : CENTER;
	float Rotating : ROTATING;
	uint ImageID : SPRITEIMAGEINDEX;
};

struct SpriteGSIn
{
	float4 Position : SV_POSITION;
	float2 Size : SIZE;
	float4 Color : COLOR;
	float2 Center : CENTER;
	float Rotating : ROTATING;
	uint ImageID : SPRITEIMAGEINDEX;
};

Texture2D SpriteTexture : register(t0);
float2 TextureSize;
cbuffer SpriteSheet : register(c0)
{
	float4 SpriteSheetRects[512];
};


float4 CoordinateSystemCenter;	//Snowing::Graphics::CoordinateCenter



SamplerState SpriteSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

SpriteGSIn SpriteVSFunc(SpriteVSIn i)
{
	SpriteGSIn o;

	o.Position.xy = (i.Position - CoordinateSystemCenter.xy) / CoordinateSystemCenter.zw * 2;
	o.Position.y = -o.Position.y;
	o.Position.zw = float2(0, 1);
	o.Size = i.Size * SpriteSheetRects[i.ImageID].zw * TextureSize;
	o.Color = i.Color;
	o.Center = i.Center;
	o.Rotating = i.Rotating;
	o.ImageID = i.ImageID;

	return o;
}

struct SpritePSIn
{
	float4 Position : SV_POSITION;
	float4 Color : PS_COLOR;
	float2 TexCoord : TEXCOORD;
	float2 FragPosition : FRAG_POS;
};

void SpriteGSExpander(
	SpriteGSIn i,
	inout TriangleStream<SpritePSIn> os)
{
	SpritePSIn o;
	o.Color = i.Color;
	o.Position.zw = float2(0, 1);
	
	float4 sprite = SpriteSheetRects[i.ImageID];
	float2 size = i.Size * 2;
	float2 center = i.Center;

	float2 posOffset[4] = 
	{
		float2(size.x * center.x,-size.y * (1 - center.y)),
		-size * (float2(1,1) - center),
		size * center,
		float2(-size.x * (1-center.x),size.y * center.y)
	};

	float2x2 rotating = {
		cos(i.Rotating), -sin(i.Rotating),
		sin(i.Rotating), cos(i.Rotating) };

	[unroll]
	for (int index = 0; index < 4; ++index)
		posOffset[index] = mul(rotating, posOffset[index]) / CoordinateSystemCenter.zw;

	o.Position.xy = i.Position.xy + posOffset[0];
	o.TexCoord = float2(sprite.x + sprite.z, sprite.y + sprite.w);
	o.FragPosition = float2(1, 1);
	os.Append(o);

	o.Position.xy = i.Position.xy + posOffset[1];
	o.TexCoord = float2(sprite.x, sprite.y + sprite.w);
	o.FragPosition = float2(0, 1);
	os.Append(o);

	o.Position.xy = i.Position.xy + posOffset[2];
	o.TexCoord = float2(sprite.x + sprite.z, sprite.y);
	o.FragPosition = float2(1, 0);
	os.Append(o);

	o.Position.xy = i.Position.xy + posOffset[3];
	o.TexCoord = float2(sprite.x, sprite.y);
	o.FragPosition = float2(0, 0);
	os.Append(o);
}

[maxvertexcount(6)]
void SpriteGSFunc(
	point SpriteGSIn i[1],
	inout TriangleStream<SpritePSIn> os)
{
	SpriteGSExpander(i[0], os);
}


float4 SpritePSFunc(SpritePSIn i) : SV_TARGET
{
	return i.Color * SpriteTexture.Sample(SpriteSampler,i.TexCoord);
}
