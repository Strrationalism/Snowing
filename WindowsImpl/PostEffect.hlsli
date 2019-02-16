
struct PostEffectPSIn
{
	float4 Position : SV_POSITION;
	float2 Coord : TEXCOORD;
};

PostEffectPSIn PostEffectVSFunc(float2 p : POSITION)
{
	PostEffectPSIn o;
	o.Position.xy = (p - float2(0.5, 0.5)) * 2.0;
	o.Position.zw = float2(0, 1);
	o.Coord = float2(p.x,1.0-p.y);
	return o;
}
