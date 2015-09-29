texture2D Texture : register(t0);

struct V_OUT
{
	float4 posH : SV_POSITION;
	float4 Color : COLOR;
	float2 UV : UVCOORD;
	float3 norm : NORMAL;
	float3 worldpos : WORLDPOS;
};

SamplerState filter : register(s0);

float4 main(V_OUT modulate) : SV_TARGET
{

		return Texture.Sample(filter, modulate.UV.xy);

}