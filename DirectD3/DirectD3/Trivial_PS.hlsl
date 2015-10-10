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
	float4 base = Texture.Sample(filter, modulate.UV.xy);
	if (base.a < 0.1)
	{
		discard;
	}
	//	return base;

	float3 genDir = (0, 1, 1);
	float3 lightDir = normalize(genDir);
	float3 normal = normalize(modulate.norm);
	float3 spotDir = (0, 1, -1);

	float3 spotCone = normalize(spotDir);
	float3 spotPos = { 2, 2, 8};

	float3 spotdir2 = normalize(spotPos - modulate.worldpos);
	float spotRatio = clamp(dot(-spotdir2, spotCone), 0, 1);

	float spotFactor = (spotRatio > 0.95f) ? 1 : 0;
	float spotLightRatio = clamp(dot(spotdir2, normal), 0, 1);
	float4 spotAmbColor = { 0.3f, 0.3f, 0.3f, 1 };
		float4 spotColor = float4(1.0f, 0.0f, 0.0f, 1);

		float3 pointpos = (0, 0, 0);
		float3 pointdir = pointpos - modulate.worldpos;
		float distance = length(pointdir);

		pointdir /= distance;
	float pointratio = dot(pointdir, modulate.norm);
		
	float4 p_ambColor = { 1, 0, 1, 1 };


		float4 ambColor = { 0.0, 0.0, 0.01, 1 };
		float4 lightColor = { 0.7, 0.7, 0.7, 1 };

		float dirRatio = clamp(dot(lightDir, normal), 0, 1);
		//float diffuse = clamp(dot(normal, lightDir), 0.0, 1.0);
		//float specular = 0;
	
		//if (diffuse > 0.0)
		//{
		//	specular = pow(saturate(dot(normal, lightDir)), 25);
		//}

		//float3 final = ambColor + lightColor * diffuse + lightColor * specular;
		//return saturate((dot(lightDir, normal) * base) + (base + ambColor));
		float4 m_point = (pointratio * p_ambColor * base) + (base + p_ambColor);
		float4 m_dir = saturate((dot(lightDir, normal) * base) + (base + ambColor));
		//float4 m_dir = (dirRatio* ambColor * base) + (base + ambColor);
		//return saturate(m_point + m_dir);

		float4 spot = (spotFactor * spotLightRatio * spotColor * base) + (spotAmbColor * base);
		return spot;
		//return m_point;
		//return m_dir;
		//return saturate(m_dir + spot);
		//return saturate((pointratio * p_ambColor * base) + (base + p_ambColor));
	//return float4(final, 1.0);
}