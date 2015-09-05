texture2D Texture : register(t0);

struct V_OUT
{
	float4 posH : SV_POSITION;
	float3 UV : UVCOORD;
	float3 norm : NORMAL;
};

SamplerState filter : register(s0);

float4 main(V_OUT modulate) : SV_TARGET
{
	float4 base = Texture.Sample(filter, modulate.UV);

	float4 color;
	color.a = base.b;
	color.r = base.g;
	color.g = base.r;
	color.b = base.a;

	return color;

}
//float4 main(INPUT_PIXEL input) : SV_TARGET
//{
//	return float4(input.colorOut, 1);
//}