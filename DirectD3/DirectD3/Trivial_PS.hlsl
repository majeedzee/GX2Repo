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
	float3 genDir = (1, 1, 1);
	float3 lightDir = -normalize(genDir);
	float3 normal = normalize(modulate.norm);

	float3 pointpos = (2, 0, 0);
	float3 pointdir = normalize(pointpos - modulate.worldpos);
	float pointratio = clamp(dot(pointdir, modulate.norm), 0.0, 1.0);
	float4 p_ambColor = { 1, 1, 10, 1 };

		float4 base = Texture.Sample(filter, modulate.UV.xy);

		float4 ambColor = { 0.02, 0.02, 0.02, 1 };
		float4 lightColor = { 0.7, 0.7, 0.7, 1 };

		//float diffuse = clamp(dot(normal, lightDir), 0.0, 1.0);
		//float specular = 0;

		//if (diffuse > 0.0)
		//{
		//	specular = pow(saturate(dot(normal, lightDir)), 25);
		//}

		//float3 final = ambColor + lightColor * diffuse + lightColor * specular;
		//return saturate((dot(lightDir, normal) * base) + (base + ambColor));
		float4 m_point = pointratio * p_ambColor * base;
		float4 m_dir = saturate((dot(lightDir, normal) * base) + (base + ambColor));
		return saturate(m_point + m_dir);
		//return pointratio * p_ambColor * base;
	//return float4(final, 1.0);
}