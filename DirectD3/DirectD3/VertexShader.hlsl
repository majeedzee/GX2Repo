#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
	float4 Color : COLOR;
	float2 UV : UVCOORD;
	float3 norm : NORMAL;
};

struct OUTPUT_VERTEX
{
	float4 posH : SV_POSITION;
	float4 Color : COLOR;
	float2 UV : UVCOORD;
	float3 norm : NORMAL;
	float3 worldpos : WORLDPOS;
	float3 spotpos : SPOT;
};

cbuffer THIS_IS_VRAM : register(b0)
{
	float3 coordinate;
	float4 Color;
	float2 UV;
	float3 norm;
};
cbuffer Object : register(b1)
{
	float4x4 SV_WorldMatrix;
	float4x4 SV_ProjectionMatrix;
	float4x4 SV_ViewMatrix;
};

cbuffer Instance : register(b2)
{
	float4x4 instance[60];
};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer, uint sub : SV_InstanceID)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	float4 Vector = float4(fromVertexBuffer.coordinate, 1);

		Vector = mul(Vector, instance[sub]);
	sendToRasterizer.worldpos = Vector.xyz;
	Vector = mul(Vector, SV_ViewMatrix);
	sendToRasterizer.spotpos = Vector.xyz;
	Vector = mul(Vector, SV_ProjectionMatrix);

	sendToRasterizer.posH = Vector;
	sendToRasterizer.UV = fromVertexBuffer.UV;
	sendToRasterizer.UV.y = 1 - fromVertexBuffer.UV.y;

	sendToRasterizer.norm = fromVertexBuffer.norm;
	//sendToRasterizer.norm.y = 1 - fromVertexBuffer.norm.y;

	return sendToRasterizer;
}