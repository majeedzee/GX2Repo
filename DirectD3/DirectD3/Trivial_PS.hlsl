texture2D Texture : register(t0);

struct V_OUT
{
	float4 posH : SV_POSITION;
	float4 Color : COLOR;
	float2 UV : UVCOORD;
	float3 norm : NORMAL;
};

SamplerState filter : register(s0);

float4 main(V_OUT modulate) : SV_TARGET
{
	float3 genDir = (1, 1, 1);
	float3 lightDir = -normalize(genDir);
	float3 normal = normalize(modulate.norm);

	float4 base = Texture.Sample(filter, modulate.UV.xy);

	float4 ambColor = { 0, 0.05, 0, 1 };
	return saturate((dot(lightDir, normal) * base) + (base + ambColor));
}